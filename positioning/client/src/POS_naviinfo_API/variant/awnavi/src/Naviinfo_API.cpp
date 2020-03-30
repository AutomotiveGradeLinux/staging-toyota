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

/******************************************************************************
@file       Naviinfo_API.cpp
@detail     Naviinfo_API Functions
@lib        libNaviinfo_API.so
******************************************************************************/

/*****************************************************************************
 *            Include                                                        *
 *****************************************************************************/
#include "Naviinfo_API.h"
#include <vehicle_service/POS_define.h>
#include <vehicle_service/POS_sensor_API.h>
#include <vehicle_service/POS_gps_API.h>
#include "Vehicle_API_Dummy.h"
#include "POS_private.h"
#include <native_service/frameworkunified_types.h>  // NOLINT(build/include_order)

//void PosCnvGpsInfo(NAVIINFO_ALL *navi_loc_info);


/**
 * @brief
 *  GPS information setting
 *
 *  Set GPS information
 *
 * @param[in]  hApp         HANDLE        - Application handle
 * @param[in]  navilocinfo  NAVIINFO_ALL* - Pointer to GPS information storage area
 *
 * @return NAVIINFO_RET_NORMAL              Normal completion<br>
 *         NAVIINFO_RET_ERROR_PARAM         Parameter error<br>
 *         NAVIINFO_RET_ERROR_INNER         Internal error<br>
 *         NAVIINFO_RET_ERROR_NOSUPPORT     Unsupported environment
 *
 */
//NAVIINFO_RET_API POS_SetGPSInfo(HANDLE hApp, NAVIINFO_ALL *navilocinfo)
//{
//    NAVIINFO_RET_API        ret = NAVIINFO_RET_NORMAL;    /* Return value of this function    */
//    UNIT_TYPE               type = UNIT_TYPE_NONE;        /* Supported HW Configuration Type    */
//    NAVIINFO_ALL            navi_loc_info_tmp;            /* Conversion quantity area */
//    RET_API                 ret_api;
//
//    /** NULL checking */
//    if (navilocinfo == NULL) {
//        /** Parameter error */
//        ret = NAVIINFO_RET_ERROR_PARAM;
//    } else if (hApp == NULL) {
//        /** Parameter error */
//        ret = NAVIINFO_RET_ERROR_PARAM;
//    } else {
//        /* Positioning Base API initialization */
//        _pb_Setup_CWORD64_API(hApp);
//
//        /* Supported HW Configuration Check */
//        type = GetEnvSupportInfo();
//        if (UNIT_TYPE_GRADE1 == type) {
//            /* GRADE1 */
//            ret = NAVIINFO_RET_NORMAL;
//        } else if (UNIT_TYPE_GRADE2 == type) {
//          /*
//           *  Note.
//           *  This feature branches processing depending on the unit type.
//           */
//            ret = NAVIINFO_RET_ERROR_NOSUPPORT;
//        } else {
//            /* Environment error */
//            ret = NAVIINFO_RET_ERROR_NOSUPPORT;
//        }
//    }
//
//    if (ret == NAVIINFO_RET_NORMAL) {
//        /* Parameter range check */
//        if (navilocinfo->stNaviGps.tdsts != 0) { /* Other than ""Time not calibrated after receiver reset"" */
//            /* Positioning status information */
//            if (POS_RET_ERROR == POS_CHKPARAMU8(navilocinfo->stDiagGps.stFix.ucFixSts, 0, 2)) {
//                return NAVIINFO_RET_ERROR_PARAM;
//            }
//            /* Latitude */
//            if (POS_RET_ERROR == POS_CHKPARAM32(navilocinfo->stDiagGps.stFix.stWgs84.lLat, -82944000, 82944000)) {
//                return NAVIINFO_RET_ERROR_PARAM;
//            }
//            /* Longitude */
//            if (POS_RET_ERROR == POS_CHKPARAM32(navilocinfo->stDiagGps.stFix.stWgs84.lLon, -165888000, 165888000)) {
//                return NAVIINFO_RET_ERROR_PARAM;
//            }
//            /* Measurement Azimuth */
//            if (POS_RET_ERROR == POS_CHKPARAMU16(navilocinfo->stNaviGps.heading, 0, 3599)) {
//                return NAVIINFO_RET_ERROR_PARAM;
//            }
//            /* UTC(Month) */
//            if (POS_RET_ERROR == POS_CHKPARAMU8(navilocinfo->stNaviGps.utc.month, 1, 12)) {
//                return NAVIINFO_RET_ERROR_PARAM;
//            }
//            /* UTC(Day) */
//            if (POS_RET_ERROR == POS_CHKPARAMU8(navilocinfo->stNaviGps.utc.date, 1, 31)) {
//                return NAVIINFO_RET_ERROR_PARAM;
//            }
//            /* UTC(Hour) */
//            if (POS_RET_ERROR == POS_CHKPARAMU8(navilocinfo->stNaviGps.utc.hour, 0, 23)) {
//                return NAVIINFO_RET_ERROR_PARAM;
//            }
//            /* UTC(Minutes) */
//            if (POS_RET_ERROR == POS_CHKPARAMU8(navilocinfo->stNaviGps.utc.minute, 0, 59)) {
//                return NAVIINFO_RET_ERROR_PARAM;
//            }
//            /* UTC(Second) */
//            if (POS_RET_ERROR == POS_CHKPARAMU8(navilocinfo->stNaviGps.utc.second, 0, 59)) {
//                return NAVIINFO_RET_ERROR_PARAM;
//            }
//        }
//        /* Date and Time Status */
//        if (POS_RET_ERROR == POS_CHKPARAMU8(navilocinfo->stNaviGps.tdsts, 0, 2)) {
//            return NAVIINFO_RET_ERROR_PARAM;
//        }
//
//        /* Copy to conversion area  */
//        memcpy(&navi_loc_info_tmp, navilocinfo, sizeof(NAVIINFO_ALL));
//        /** Data unit conversion */
//        PosCnvGpsInfo(&navi_loc_info_tmp);
//
//        /* Resource acquisition */
//        if (VehicleGetResource() == TRUE) {
//
//        /** Send navigation information to vehicle sensor */
//            ret_api = _pb_SndMsg_Ext(POS_THREAD_NAME,
//                                     CID_NAVIINFO_DELIVER,
//                                     sizeof(NAVIINFO_ALL),
//                                     reinterpret_cast<void*>(&navi_loc_info_tmp), 0);
//            if (ret_api != RET_NORMAL) {
//                /** Message transmission failure */
//                ret = NAVIINFO_RET_ERROR_INNER;
//            }
//        } else {
//            /* When resource shortage occurs, the system terminates with an insufficient resource error. */
//            ret = NAVIINFO_RET_ERROR_RESOURCE;
//        }
//        /* Resource release */
//        VehicleReleaseResource();
//    }
//
//    return ret;
//}

/**
 * @brief
 *   GPS information acquisition
 *
 *   Access GPS information
 *
 * @param[in]  hApp         HANDLE             - Application handle
 * @param[in]  navidiaginfo NAVIINFO_DIAG_GPS* - Pointer to GPS information storage area
 *
 * @return  NAVIINFO_RET_NORMAL             Normal completion<br>
 *          NAVIINFO_RET_ERROR_PARAM        Parameter error<br>
 *          NAVIINFO_RET_ERROR_INNER        Internal error<br>
 *          NAVIINFO_RET_ERROR_NOSUPPORT    Unsupported environment
 *
 */
//NAVIINFO_RET_API POS_GetGPSInfo(HANDLE hApp, NAVIINFO_DIAG_GPS *navidiaginfo)
//{
//    NAVIINFO_RET_API     ret = NAVIINFO_RET_NORMAL;    /* Return value of this function    */
//    UNIT_TYPE            type = UNIT_TYPE_NONE;        /* Supported HW Configuration Type    */
//    int32                ret_veh;                      /* VehicleAPI Return Values */
//    NAVIINFO_DIAG_GPS    dest_data;                    /* Data acquisition area */
//
//    /** NULL checking */
//    if (navidiaginfo == NULL) {
//        /** Parameter error */
//        ret = NAVIINFO_RET_ERROR_PARAM;
//    } else if (hApp == NULL) {
//        /** Parameter error */
//        ret = NAVIINFO_RET_ERROR_PARAM;
//    } else {
//        /* Positioning Base API initialization */
//        _pb_Setup_CWORD64_API(hApp);
//
//        /* Supported HW Configuration Check */
//        type = GetEnvSupportInfo();
//        if (UNIT_TYPE_GRADE1 == type) {
//            /* GRADE1 */
//            ret = NAVIINFO_RET_NORMAL;
//        } else if (UNIT_TYPE_GRADE2 == type) {
//          /*
//           *  Note.
//           *  This feature branches processing depending on the unit type.
//           */
//            ret = NAVIINFO_RET_ERROR_NOSUPPORT;
//        } else {
//            /* Environment error */
//            ret = NAVIINFO_RET_ERROR_NOSUPPORT;
//        }
//    }
//
//    if (ret == NAVIINFO_RET_NORMAL) {
//        /** Acquisition of navigation data for Diag provide */
//        ret_veh = PosGetProc(
//                             (DID)VEHICLE_DID_NAVIINFO_DIAG_GPS,
//                             reinterpret_cast<void*>(&dest_data),
//                             (u_int16)sizeof(dest_data));
//
//        if (static_cast<int32>(sizeof(NAVIINFO_DIAG_GPS)) > ret_veh) {
//            /** Failed to acquire */
//            if (ret_veh == POS_RET_ERROR_RESOURCE) {
//                ret = NAVIINFO_RET_ERROR_RESOURCE;
//            } else {
//                ret = NAVIINFO_RET_ERROR_INNER;
//            }
//        } else {
//            /** Successful acquisition */
//            memcpy( navidiaginfo, &dest_data, sizeof(NAVIINFO_DIAG_GPS));
//        }
//    }
//
//    return ret;
//}

/* ++ GPS _CWORD82_ support */
/**
 * @brief
 *   GPS setting transmission request
 *
 *   Requesting GPS Settings with Complete Return
 *
 * @param[in]  hApp     HANDLE               - Application handle
 * @param[in]  p_data   SENSOR_MSG_SEND_DAT* - GPS setting information to be sent
 *
 * @return  SENSOR_RET_NORMAL              Normal completion<br>
 *          SENSOR_RET_ERROR_CREATE_EVENT  Event generation failure<br>
 *          SENSOR_RET_ERROR_PARAM         Parameter error<br>
 *          SENSOR_RET_ERROR_DID           Unregistered DID<br>
 *          SENSOR_RET_ERROR_NOSUPPORT     Unsupported environment
 *
 */
int32 POS_ReqGPSSetting(HANDLE hApp, SENSOR_MSG_SEND_DAT *p_data) {    /* Ignore->MISRA-C++:2008 Rule 7-1-2 */
    SENSOR_RET_API    ret = SENSOR_RET_NORMAL;        /* Return value                                */
    RET_API           ret_api;                        /* System API return value                     */
    u_int16           expected_size;                  /* Message size for the specified DID */

    UNIT_TYPE         type = UNIT_TYPE_NONE;          /* Supported HW Configuration Type    */

    if (hApp == NULL) {
        /* Parameter error */
        ret = SENSOR_RET_ERROR_PARAM;
    }

    if (ret == SENSOR_RET_NORMAL) {
        /* Positioning Base API initialization */
        _pb_Setup_CWORD64_API(hApp);

        /* Argument check (DID) + Size calculation */
        if (p_data != reinterpret_cast<SENSOR_MSG_SEND_DAT*>(NULL)) {
            switch (p_data->did) {
                case VEHICLE_DID_GPS__CWORD82__SETINITIAL:
                {
                    expected_size = 71;
                    break;
                }
                case VEHICLE_DID_GPS__CWORD82__SETRMODE:
                {
                    expected_size = 50;
                    break;
                }
                case VEHICLE_DID_GPS__CWORD82__SETRMODEEX:
                {
                    expected_size = 63;
                    break;
                }
                case VEHICLE_DID_GPS__CWORD82__SELSENT:
                {
                    expected_size = 21;
                    break;
                }
                case VEHICLE_DID_GPS__CWORD82__SETSBAS:
                {
                    expected_size = 34;
                    break;
                }
                case VEHICLE_DID_GPS__CWORD82__SETCONF1:
                {
                    expected_size = 37;
                    break;
                }
                case VEHICLE_DID_GPS__CWORD82__SETCONF2:
                {
                    expected_size = 45;
                    break;
                }
                default:    /* Ignore->MISRA-C++:2008 Rule 6-3-1, 6-4-1 */
                    ret = SENSOR_RET_ERROR_DID;
                    break;
            }
        } else {
            ret = SENSOR_RET_ERROR_PARAM;
        }
    }

    /* Supported HW Configuration Check */
    if (ret == SENSOR_RET_NORMAL) {
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
            /* Environment error */
            ret = SENSOR_RET_ERROR_NOSUPPORT;
        }
    }

    if (ret == SENSOR_RET_NORMAL) {
        /* Argument check (Size)*/
        if (expected_size != p_data->usSize) {
            ret = SENSOR_RET_ERROR_PARAM;
        } else {
            /* Message buffer initialization */

            /* Create message data */

            /* Resource acquisition */
            if (VehicleGetResource() == TRUE) {
            /* External Process Transmission and Reception Messages */
            FRAMEWORKUNIFIEDLOG(ZONE_27, __FUNCTION__, 
                          "POSITIONING: POS_ReqGPSSetting() --> cid = %d",
                          CID_SENSORIF__CWORD82__REQUEST);
            ret_api = _pb_SndMsg_Ext(POS_THREAD_NAME,
                                     CID_SENSORIF__CWORD82__REQUEST,
                                     sizeof(SENSOR_MSG_SEND_DAT),
                                     reinterpret_cast<void *>(p_data), 0);
            FRAMEWORKUNIFIEDLOG(ZONE_27, __FUNCTION__, "POSITIONING: POS_ReqGPSSetting() <--");

            if (ret_api != RET_NORMAL) {
                ret = SENSOR_RET_ERROR_CREATE_EVENT;
            }
            } else {
                /* When resource shortage occurs, the system terminates with an insufficient resource error. */
                ret = SENSOR_RET_ERROR_RESOURCE;
            }
            /* Resource release */
            VehicleReleaseResource();
        }
    }

    return ret;
}
/* -- GPS _CWORD82_ support */

/**
 * @brief
 *   GPS information conversion process
 *
 *   Convert GPS information to a format to be provided to the vehicle sensor
 *
 * @param[in]  none
 * @param[in]  navi_loc_info NAVIINFO_ALL* - GPS information pointer
 *
 * @return  none
 */
//void PosCnvGpsInfo(NAVIINFO_ALL *navi_loc_info) {
//    int32      altitude;
//    int64_t    tmp;
//    u_int16    heading;
//
//    /* Unit conversion of fix altitude[1m]->[0.01m] */
//    tmp = (int64_t)((int64_t)(navi_loc_info->stNaviGps.altitude) * 100);
//    if (tmp > static_cast<int32>(0x7FFFFFFF)) {
//        /* +Overflow of digits */
//        altitude = static_cast<int32>(0x7FFFFFFF);
//    } else if (tmp < static_cast<int32>(0x80000000)) {    /* Ignore->MISRA-C:2004 Rule 3.1 */
//        /* -Overflow of digits */
//        altitude = static_cast<int32>(0x80000000);        /* Ignore->MISRA-C:2004 Rule 3.1 */
//    } else {
//        altitude = static_cast<int32>(tmp);
//    }
//    navi_loc_info->stNaviGps.altitude = altitude;
//
//    /* Measurement Azimuth Conversion[0.Once]->[0.01 degree] */
//    heading = navi_loc_info->stNaviGps.heading;
//    heading = static_cast<u_int16>(heading - ((heading / 3600) * 3600));
//    heading = static_cast<u_int16>(heading * 10);
//    navi_loc_info->stNaviGps.heading = heading;
//
//    return;
//}
