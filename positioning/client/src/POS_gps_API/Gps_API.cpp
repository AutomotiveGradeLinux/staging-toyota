/*
 * @copyright Copyright (c) 2016-2019 TOYOTA MOTOR CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file
 *  Gps_API.cpp
 * @brief
 *   Module : POSITIONING
 *   GPS I/F service functionality
 */
#include <vehicle_service/positioning_base_library.h>
#include <vehicle_service/POS_define.h>
#include <vehicle_service/POS_gps_API.h>
#include <vehicle_service/POS_common_API.h>
#include <vehicle_service/POS_sensor_API.h>
#include "POS_common_private.h"
#include "POS_private.h"
#include "Vehicle_API_private.h"
#include "Gps_API_private.h"


/**
 * @brief
 *  GPS reset request
 *
 *  Request a GPS reset
 *
 * @param[in] hApp          HANDLE  - Application handle
 * @param[in] ResName       PCSTR   - Destination thread name
 * @param[in] mode          uint8_t - Reset mode
 *
 * @return  POS_RET_NORMAL           Normal completion(Include illegal)<br>
 *          POS_RET_ERROR_PARAM      Parameter error<br>
 *          POS_RET_ERROR_INNER      Internal processing error<br>
 *          POS_RET_ERROR_BUSY       Busy occurrence<br>
 *          POS_RET_ERROR_NOSUPPORT  Unsupported environment
 *
 */
POS_RET_API POS_ReqGPSReset(HANDLE hApp, PCSTR ResName, uint8_t mode) {  // NOLINT(readability/nolint)
    UNIT_TYPE        type = UNIT_TYPE_NONE;    /* Supported HW Configuration Type */
    POS_RET_API      ret = POS_RET_NORMAL;     /* Return value of this function */
    RET_API          ret_api = RET_NORMAL;     /* API return value */
    POS_RESETINFO    snd_msg;                  /* Reset information */
    EventID          event_id;                 /* Event ID */
    int32_t          event_val;                /* Event value */
    PNO              my_pno;                   /* Caller PNO */
    PNO              rs_pno;                   /* Destination PNO */
    uint32_t         pid;                      /* Process ID */
    uint32_t         tid;                      /* Thread ID */
    char             name[128];                /* Name buffer */

    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");

    /* Arguments & Support Configuration Check */
    if ((hApp == NULL) || (ResName == NULL)) {
        ret = POS_RET_ERROR_PARAM;
//    } else if (GPS_RST_COLDSTART != mode) {
//        /* Parameter error except */
//        ret = POS_RET_ERROR_PARAM;
    } else {
        /* Positioning Base API initialization */
        _pb_Setup_CWORD64_API(hApp);

        /* Supported HW Configuration Check */
        type = GetEnvSupportInfo();
        if (UNIT_TYPE_GRADE1 == type) {
            /* GRADE1 */
            ret = POS_RET_NORMAL;
        } else if (UNIT_TYPE_GRADE2 == type) {
          /*
           *  Note.
           *  This feature branches processing depending on the unit type.
           */
            ret = POS_RET_ERROR_NOSUPPORT;
        } else {
            /* Environment error */
            ret = POS_RET_ERROR_NOSUPPORT;
        }

        if (ret == POS_RET_NORMAL) {
            /* Resource acquisition */
            if (VehicleGetResource() == TRUE) {
                /* Event Generation */
                rs_pno = _pb_CnvName2Pno(ResName);
                pid = (u_int32)getpid();
                tid = GetTid();

                snprintf(name, sizeof(name), "PR_p%u_t%u", pid, tid);
                my_pno = _pb_CnvName2Pno(name);
                event_id = VehicleCreateEvent(my_pno);
                if (0 != event_id) {
                    /* Successful event generation */
                    memset(&snd_msg, 0x00, sizeof(POS_RESETINFO));

                    /* Message creation */
                    snd_msg.mode     = mode;      /* Reset mode */
                    snd_msg.snd_pno  = my_pno;    /* Caller PNo. */
                    snd_msg.res_pno  = rs_pno;    /* Destination PNo. */

                    /* Message transmission request */
                    ret_api = _pb_SndMsg_Ext(POS_THREAD_NAME,
                                             CID_GPS_REQRESET,
                                             sizeof(POS_RESETINFO),
                                             reinterpret_cast<void*>(&snd_msg), 0);

                    if (RET_NORMAL == ret_api) {
                        /* If the data setup process is successful,Wait for a completion event */
                        ret_api = _pb_WaitEvent(event_id,
                                                SAPI_EVWAIT_VAL,
                                                VEHICLE_RET_ERROR_MIN, VEHICLE_RET_NORMAL, &event_val, INFINITE);
                        if (RET_NORMAL != ret_api) {
                            /* When not put in event wait state */
                            /* Event generation failure */
                            ret = POS_RET_ERROR_INNER;
                        } else {
                            /* Return from Event Wait */
                            /* Set event value (processing result) as return value */
                            ret = (POS_RET_API)event_val;
                        }
                    } else {
                        /* Message transmission processing failed */
                        ret = POS_RET_ERROR_INNER;
                    }
                    /* Event deletion */
                    ret_api = VehicleDeleteEvent(event_id);
                } else {
                    /* Event generation failure */
                    ret = POS_RET_ERROR_INNER;
                }
            } else {
                /* Insufficient resource */
                ret = POS_RET_ERROR_RESOURCE;
            }
            /* Resource release */
            VehicleReleaseResource();
        }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "- [ret = %d]", ret);
    return ret;
}

/**
 * @brief
 *  Get GPS version
 *
 *  Get GPS version information
 *
 * @param[in] hApp          HANDLE   - Application handle
 * @param[in] buf_size      uint8_t  - Storage size of version information(Byte)
 * @param[in] buf           int8_t*  - Pointer to store the version information
 * @param[in] size          uint8_t* - Size of the character string written to buf(Byte)
 *
 * @return  POS_RET_NORMAL           Normal completion(Include illegal)<br>
 *          POS_RET_ERROR_PARAM      Parameter error<br>
 *          POS_RET_ERROR_NOSUPPORT  Unsupported environment
 *
 */
POS_RET_API POS_GetGPSVersion(HANDLE hApp,  // NOLINT(readability/nolint)
                            uint8_t buf_size,
                            int8_t* buf,  // NOLINT(readability/nolint)
                            uint8_t* size) {
    POS_RET_API  ret = POS_RET_NORMAL;     /* Return value */
    UNIT_TYPE    type = UNIT_TYPE_NONE;    /* Supported HW Configuration Type */

    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");

    /* Positioning Base API initialization */
    _pb_Setup_CWORD64_API(hApp);

    /* Supported HW Configuration Check */
    type = GetEnvSupportInfo();
    if (UNIT_TYPE_GRADE1 == type) {
        /* GRADE1 */
        ret = POS_RET_ERROR_NOSUPPORT;
    } else if (UNIT_TYPE_GRADE2 == type) {
      /*
       *  Note.
       *  This feature branches processing depending on the unit type.
       */
        ret = POS_RET_ERROR_NOSUPPORT;
    } else {
        /* Environment error */
        ret = POS_RET_ERROR_NOSUPPORT;
    }
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "- [ret = %d]", ret);
    return ret;
}

/**
 * @brief
 *  GPS time setting request delivery registration
 *
 *  Register delivery of GPS time setting request
 *
 * @param[in] hApp          HANDLE  - Application handle
 * @param[in] notifyName    PCSTR   - Destination thread name
 * @param[in] ucCtrlFlg     uint8_t - Delivery control(Delivery registration/Delivery stop/Resume delivery)
 *
 * @return POS_RET_NORMAL               Normal completion(Include illegal)<br>
 *         POS_RET_ERROR_BUFFULL        Buffer-full<br>
 *         POS_RET_ERROR_INNER          Internal error<br>
 *         POS_RET_ERROR_PARAM          Parameter error<br>
 *         POS_RET_ERROR_NOSUPPORT      Unsupported environment
 *
 */
POS_RET_API POS_RegisterListenerGPSTimeSetReq(HANDLE hApp,  // NOLINT(readability/nolint)
                                            PCSTR notifyName,  // NOLINT(readability/nolint)
                                            uint8_t ucCtrlFlg) {  // NOLINT(readability/nolint)
    POS_RET_API       ret = POS_RET_NORMAL;      /* Return value of this function */
    SENSOR_RET_API    ret_sens = SENSOR_RET_NORMAL;  /* API return value */
    UNIT_TYPE         type = UNIT_TYPE_NONE;      /* Supported HW Configuration Type */

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    /* Arguments & Support Configuration Check */
    if ((hApp == NULL) || (notifyName == NULL)) {
        ret = POS_RET_ERROR_PARAM;
    } else if (SENSOR_DELIVERY_REGIST != ucCtrlFlg) {
        /* Parameter error other than delivery registration */
        ret = POS_RET_ERROR_PARAM;
    } else {
        /* Positioning Base API initialization */
        _pb_Setup_CWORD64_API(hApp);

        /* Supported HW Configuration Check */
        type = GetEnvSupportInfo();
        if (UNIT_TYPE_GRADE1 == type) {
            /* GRADE1 */
            ret = POS_RET_NORMAL;
        } else if (UNIT_TYPE_GRADE2 == type) {
          /*
           *  Note.
           *  This feature branches processing depending on the unit type.
           */
            ret = POS_RET_ERROR_NOSUPPORT;
        } else {
            /* Environment error */
            ret = POS_RET_ERROR_NOSUPPORT;
        }
    }

    /* Delivery registration */
    if (POS_RET_NORMAL == ret) {
        /* Delivery registry SensorAPI calls */
        ret_sens = PosRegisterListenerProc(notifyName,                         /* Destination thread name */
                                           VEHICLE_DID_SETTINGTIME,            /* Data ID */
                                           ucCtrlFlg,                          /* Delivery control */
                                           VEHICLE_DELIVERY_TIMING_UPDATE);    /* Delivery timing */

        /* Decision of delivery registration result */
        if (ret_sens == SENSOR_RET_NORMAL) {
            ret = POS_RET_NORMAL;
        } else if (ret_sens == SENSOR_RET_ERROR_PARAM) {
            ret = POS_RET_ERROR_PARAM;
        } else if (ret_sens == SENSOR_RET_ERROR_BUFFULL) {
            ret = POS_RET_ERROR_BUFFULL;
        } else if (ret_sens == SENSOR_RET_ERROR_RESOURCE) {
            ret = POS_RET_ERROR_RESOURCE;
        } else {
            ret = POS_RET_ERROR_INNER;
        }
    }
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "- [ret = %d]", ret);
    return ret;
}

/**
 * @brief
 *  GPS time setting
 *
 *  Set the GPS time
 *
 * @param[in] hApp          HANDLE        - Application handle
 * @param[in] pstDateTime   POS_DATETIME  - Pointer to GPS time information
 *
 * @return POS_RET_NORMAL               Normal completion(Include illegal)<br>
 *         POS_RET_ERROR_INNER          Internal error<br>
 *         POS_RET_ERROR_TIMEOUT        Timeout error<br>
 *         POS_RET_ERROR_PARAM          Parameter error<br>
 *         POS_RET_ERROR_NOSUPPORT      Unsupported environment
 *
 */
//POS_RET_API POS_SetGPStime(HANDLE hApp, POS_DATETIME* pstDateTime) {  // NOLINT(readability/nolint)
//    POS_RET_API    ret = POS_RET_NORMAL;     /* Return value of this function */
//    UNIT_TYPE      type = UNIT_TYPE_NONE;    /* Supported HW Configuration Type */
//
//    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");
//
//    /* Arguments & Support Configuration Check */
//    if ((pstDateTime == NULL) || (hApp == NULL)) {
//        ret = POS_RET_ERROR_PARAM;
//    } else {
//        /* Positioning Base API initialization */
//        _pb_Setup_CWORD64_API(hApp);
//
//        /* Supported HW Configuration Check */
//        type = GetEnvSupportInfo();
//        if (UNIT_TYPE_GRADE1 == type) {
//            /* GRADE1 */
//            ret = POS_RET_NORMAL;
//        } else if (UNIT_TYPE_GRADE2 == type) {
//          /*
//           *  Note.
//           *  This feature branches processing depending on the unit type.
//           */
//            ret = POS_RET_ERROR_NOSUPPORT;
//        } else {
//            /* Environment error */
//            ret = POS_RET_ERROR_NOSUPPORT;
//        }
//    }
//
//    if (ret == POS_RET_NORMAL) {
//        /* Parameter range check */
//        /* Month */
//        if (POS_RET_ERROR == POS_CHKPARAMU8(pstDateTime->month, 1, 12)) {
//            return POS_RET_ERROR_PARAM;
//        }
//        /* Day */
//        if (POS_RET_ERROR == POS_CHKPARAMU8(pstDateTime->date, 1, 31)) {
//            return POS_RET_ERROR_PARAM;
//        }
//        /* Hour */
//        if (POS_RET_ERROR == POS_CHKPARAMU8(pstDateTime->hour, 0, 23)) {
//            return POS_RET_ERROR_PARAM;
//        }
//        /* Minutes */
//        if (POS_RET_ERROR == POS_CHKPARAMU8(pstDateTime->minute, 0, 59)) {
//            return POS_RET_ERROR_PARAM;
//        }
//        /* Second */
//        if (POS_RET_ERROR == POS_CHKPARAMU8(pstDateTime->second, 0, 59)) {
//            return POS_RET_ERROR_PARAM;
//        }
//
//        /* Data setting(Immediate recovery)*/
//        ret = PosSetProc(VEHICLE_DID_SETTINGTIME,
//                         reinterpret_cast<void*>(pstDateTime), sizeof(POS_DATETIME), FALSE);
//    }
//
//    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "- [ret = %d]", ret);
//    return ret;
//}

/**
 * @brief
 *  GPS Time Delivery Registration
 *
 *  Registering GPS time delivery
 *
 * @param[in]  hApp          HANDLE  - Application handle
 * @param[in]  nofifyName      PCSTR   - Destination thread name
 * @param[in]  ucCtrlFlg      uint8_t - Delivery control(Delivery registration/Delivery stop/Resume delivery)
 * @param[in]  ucDeliveryTiming   uint8_t - Delivery timing(Changing/Updating)
 *
 * @return  SENSOR_RET_NORMAL               Successful registration<br>
 *      SENSOR_RET_ERROR_CREATE_EVENT   Event generation failure<br>
 *      SENSOR_RET_ERROR_PARAM          Parameter error<br>
 *      SENSOR_RET_ERROR_BUFFULL        FULL of delivery registers<br>
 *          SENSOR_RET_ERROR_NOSUPPORT      Unsupported environment
 *
 */
SENSOR_RET_API POS_RegisterListenerGPStime(HANDLE hApp,  // NOLINT(readability/nolint)
                                           PCSTR notifyName,  // NOLINT(readability/nolint)
                                           uint8_t ucCtrlFlg,  // NOLINT(readability/nolint)
                                           uint8_t ucDeliveryTiming) {  // NOLINT(readability/nolint)
    SENSOR_RET_API  ret = SENSOR_RET_NORMAL;
    UNIT_TYPE       type = UNIT_TYPE_NONE;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    if (hApp == NULL) {
        /* Parameter error */
        ret = SENSOR_RET_ERROR_PARAM;
    } else {
        /* Positioning Base API initialization */
        _pb_Setup_CWORD64_API(hApp);

        /* Supported HW Configuration Check */
        type = GetEnvSupportInfo();
        if (UNIT_TYPE_GRADE1 == type) {
            /* GRADE1 */
            ret = SENSOR_RET_NORMAL;
        } else if (UNIT_TYPE_GRADE2 == type) {
          /*
           *  Note.
           *  This feature branches processing depending on the unit type.
           */
            ret = SENSOR_RET_ERROR_NOSUPPORT;
        } else {
            /* fail */
            ret = SENSOR_RET_ERROR_NOSUPPORT;
        }
    }

    if (SENSOR_RET_NORMAL == ret) {
        if (notifyName == NULL) {
            /* Parameter error */
            ret = SENSOR_RET_ERROR_PARAM;
        }
        if (ucCtrlFlg != SENSOR_DELIVERY_REGIST) {
            /* Parameter error */
            ret = SENSOR_RET_ERROR_PARAM;
        }
        if ((ucDeliveryTiming != SENSOR_DELIVERY_TIMING_CHANGE) &&
            (ucDeliveryTiming != SENSOR_DELIVERY_TIMING_UPDATE)) {
            /* Parameter error */
            ret = SENSOR_RET_ERROR_PARAM;
        }
    }

    if (SENSOR_RET_NORMAL == ret) {
        /* Delivery registration process */
        ret = PosRegisterListenerProc(notifyName, VEHICLE_DID_GPS_TIME, ucCtrlFlg, ucDeliveryTiming);
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "- [ret = %d]", ret);
    return ret;
}

/**
 * @brief
 *  Get GPS time
 *
 *  Get the GPS time
 *
 * @param[in] hApp   HANDLE         - Application handle
 * @param[in] *dat   SENSOR_GPSTIME - Pointer to GPS time information
 *
 * @return POS_RET_NORMAL               Normal completion(Include illegal)<br>
 *         POS_RET_ERROR_INNER          Internal error<br>
 *         POS_RET_ERROR_PARAM          Parameter error<br>
 *         POS_RET_ERROR_NOSUPPORT      Unsupported environment
 *
 */
//POS_RET_API POS_GetGPStime(HANDLE hApp, SENSOR_GPSTIME* dat) {  // NOLINT(readability/nolint)
//    POS_RET_API ret = POS_RET_NORMAL;          /* Return value */
//    UNIT_TYPE   type = UNIT_TYPE_NONE;         /* Supported HW Configuration Type */
//    DID         did = VEHICLE_DID_GPS_TIME;    /* DID */
//    int32       ret_get_proc;                  /* POS_GetProc Return Values */
//
//    /* Internal debug log output */
//    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");
//
//    /* Arguments & Support Configuration Check */
//    if (hApp == NULL) {
//        /* If the handler is NULL, the process terminates with an error. */
//        ret = POS_RET_ERROR_PARAM;
//    } else if (dat == NULL) {
//        /* If the longitude/latitude data is NULL, it ends with an abnormal parameter. */
//        ret = POS_RET_ERROR_PARAM;
//    } else {
//        /* Positioning Base API initialization */
//        _pb_Setup_CWORD64_API(hApp);
//
//        /* Supported HW Configuration Check */
//        type = GetEnvSupportInfo();
//        if (UNIT_TYPE_GRADE1 == type) {
//            ret = POS_RET_NORMAL;
//        } else {
//          /*
//           *  Note.
//           *  This feature branches processing depending on the unit type.
//           */
//            ret = POS_RET_ERROR_NOSUPPORT;
//        }
//    }
//
//    /* Sensor information acquisition */
//    if (ret == POS_RET_NORMAL) {
//        /* Data acquisition process */
//        ret_get_proc = PosGetProc(did, reinterpret_cast<void*>(dat), sizeof(SENSOR_GPSTIME));
//        if (static_cast<int32>(sizeof(SENSOR_GPSTIME)) > ret_get_proc) {
//            /** Failed to acquire */
//            if (ret_get_proc == POS_RET_ERROR_RESOURCE) {
//                /** Insufficient resource */
//                ret = POS_RET_ERROR_RESOURCE;
//            } else {
//                ret = POS_RET_ERROR_INNER;
//            }
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PosGetProc ERROR [ret = %d]", ret);
//        }
//    }
//
//    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "- [ret = %d]", ret);
//
//    return ret;
//}
