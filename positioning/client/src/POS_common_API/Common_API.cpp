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
 *    Common_API.cpp
 * @brief
 *   Module : POSITIONING
 *   Common I/F service functionality
 */
#include <vehicle_service/positioning_base_library.h>
#include <vehicle_service/POS_define.h>
#include <vehicle_service/POS_common_API.h>
#include "POS_common_private.h"
#include "Vehicle_API_private.h"
#include "POS_private.h"

/**
 * @brief
 *  Latitude and longitude informationDelivery registration
 *
 *  Registering Latitude and Longitude Information Delivery
 *
 * @param[in] hApp              HANDLE  - Application handle
 * @param[in] notifyName        PCSTR   - Destination thread name
 * @param[in] ucCtrlFlg         uint8_t - Delivery control(Delivery registration/Delivery stop/Resume delivery)
 * @param[in] ucDeliveryTiming  uint8_t - Delivery timing(Changing/Updating)
 * @param[in] ucGetMethod       uint8_t - Acquisition method(GPS/Navigation/DR/Not specified)
 *
 * @return POS_RET_NORMAL               Normal completion(Include illegal)<br>
 *         POS_RET_ERROR_BUFFULL        Buffer-full<br>
 *         POS_RET_ERROR_INNER          Internal error<br>
 *         POS_RET_ERROR_PARAM          Parameter error<br>
 *         POS_RET_ERROR_NOSUPPORT      Unsupported environment
 *
 */
POS_RET_API POS_RegisterListenerLonLat(HANDLE hApp,  // NOLINT(readability/nolint)
                                       PCSTR notifyName,  // NOLINT(readability/nolint)
                                       uint8_t ucCtrlFlg,  // NOLINT(readability/nolint)
                                       uint8_t ucDeliveryTiming,  // NOLINT(readability/nolint)
                                       uint8_t ucGetMethod) {  // NOLINT(readability/nolint)
    POS_RET_API       ret = POS_RET_NORMAL;                 /* Return value of this function */
    SENSOR_RET_API    ret_sens = SENSOR_RET_NORMAL;         /* API return value */
    UNIT_TYPE         type = UNIT_TYPE_NONE;                /* Supported HW Configuration Type */
    DID               did = VEHICLE_DID_LOCATION_LONLAT;    /* Data ID */

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    /* Arguments & Support Configuration Check */
    if ((ucDeliveryTiming != SENSOR_DELIVERY_TIMING_CHANGE) &&
        (ucDeliveryTiming != SENSOR_DELIVERY_TIMING_UPDATE)) {
        /* Change delivery timing,Terminate as a parameter error except update */
        ret = POS_RET_ERROR_PARAM;
    } else if (ucCtrlFlg != SENSOR_DELIVERY_REGIST) {
        /* Parameters other than delivery registration are terminated abnormally when delivery control is terminated. */
        ret = POS_RET_ERROR_PARAM;
    } else if (hApp == NULL) {
        /* If the handler is NULL, the process terminates with an error. */
        ret = POS_RET_ERROR_PARAM;
    } else if (notifyName == NULL) {
        /* If the thread name is NULL, it terminates with an abnormal parameter. */
        ret = POS_RET_ERROR_PARAM;
    } else {
        /* Positioning Base API initialization */
        _pb_Setup_CWORD64_API(hApp);

        /* Supported HW Configuration Check */
        type = GetEnvSupportInfo();
        if (UNIT_TYPE_GRADE1 == type) {
            /* GRADE1 */
            ret = POS_RET_NORMAL;
            if ((ucGetMethod == SENSOR_GET_METHOD_GPS) ||
                (ucGetMethod == SENSOR_GET_METHOD_AUTO)) {
                did = VEHICLE_DID_LOCATION_LONLAT;
            } else {
                /* End as a parameter error abnormality except for GPS/unspecified acquisition method */
                ret = POS_RET_ERROR_PARAM;
            }
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
    if (ret == POS_RET_NORMAL) {
        /* Delivery registry SensorAPI calls */
        ret_sens = PosRegisterListenerProc(notifyName,           /* Destination thread name */
                                           did,                  /* Data ID */
                                           ucCtrlFlg,            /* Delivery control */
                                           ucDeliveryTiming);    /* Delivery timing */

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
 *  Altitude information delivery registration
 *
 *  Register for the delivery of altitude information
 *
 * @param[in] hApp              HANDLE  - Application handle
 * @param[in] notifyName        PCSTR   - Destination thread name
 * @param[in] ucCtrlFlg         uint8_t - Delivery control(Delivery registration/Delivery stop/Resume delivery)
 * @param[in] ucDeliveryTiming  uint8_t - Delivery timing(Changing/Updating)
 * @param[in] ucGetMethod       uint8_t - Acquisition method(GPS/Navigation/DR/Not specified)
 *
 * @return POS_RET_NORMAL               Normal completion(Include illegal)<br>
 *         POS_RET_ERROR_PARAM          Parameter error<br>
 *         POS_RET_ERROR_NOSUPPORT      Unsupported environment
 *
 */
POS_RET_API POS_RegisterListenerAltitude(HANDLE hApp,  // NOLINT(readability/nolint)
                                         PCSTR notifyName,  // NOLINT(readability/nolint)
                                         uint8_t ucCtrlFlg,  // NOLINT(readability/nolint)
                                         uint8_t ucDeliveryTiming,  // NOLINT(readability/nolint)
                                         uint8_t ucGetMethod) {  // NOLINT(readability/nolint)
    POS_RET_API       ret = POS_RET_NORMAL;                   /* Return value */
    SENSOR_RET_API    ret_sens = SENSOR_RET_NORMAL;           /* API return value */
    UNIT_TYPE         type = UNIT_TYPE_NONE;                  /* Supported HW Configuration Type */
    DID               did = VEHICLE_DID_LOCATION_ALTITUDE;    /* Data ID */

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    /* Arguments & Support Configuration Check */
    if ((ucDeliveryTiming != SENSOR_DELIVERY_TIMING_CHANGE) &&
        (ucDeliveryTiming != SENSOR_DELIVERY_TIMING_UPDATE)) {
        /* Change delivery timing,Terminate as a parameter error except update */
        ret = POS_RET_ERROR_PARAM;
    } else if (ucCtrlFlg != SENSOR_DELIVERY_REGIST) {
        /* Parameters other than delivery registration are terminated abnormally when delivery control is terminated. */
        ret = POS_RET_ERROR_PARAM;
    } else if (hApp == NULL) {
        /* If the handler is NULL, the process terminates with an error. */
        ret = POS_RET_ERROR_PARAM;
    } else if (notifyName == NULL) {
        /* If the thread name is NULL, it terminates with an abnormal parameter. */
        ret = POS_RET_ERROR_PARAM;
    } else {
        /* Positioning Base API initialization */
        _pb_Setup_CWORD64_API(hApp);

        /* Supported HW Configuration Check */
        type = GetEnvSupportInfo();
        if (UNIT_TYPE_GRADE1 == type) {
            /* GRADE1 */
            ret = POS_RET_NORMAL;
            if ((ucGetMethod == SENSOR_GET_METHOD_GPS) ||
                (ucGetMethod == SENSOR_GET_METHOD_AUTO)) {
                did = VEHICLE_DID_LOCATION_ALTITUDE;
            } else {
                /* End as a parameter error abnormality except for GPS/unspecified acquisition method */
                ret = POS_RET_ERROR_PARAM;
            }
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

    /* Delivery registration */
    if (ret == POS_RET_NORMAL) {
        /* Delivery registry SensorAPI calls */
        ret_sens = PosRegisterListenerProc(notifyName,           /* Destination thread name */
                                           did,                  /* Data ID */
                                           ucCtrlFlg,            /* Delivery control */
                                           ucDeliveryTiming);    /* Delivery timing */

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
 *  Vehicle Speed Information Transmission Registration
 *
 *  Register delivery of vehicle speed information
 *
 * @param[in] hApp              HANDLE  - Application handle
 * @param[in] notifyName        PCSTR   - Destination thread name
 * @param[in] ucCtrlFlg         uint8_t - Delivery control(Delivery registration/Delivery stop/Resume delivery)
 * @param[in] ucDeliveryTiming  uint8_t - Delivery timing(Changing/Updating)
 * @param[in] ucGetMethod       uint8_t - Acquisition method(POS/Navigation/DR/Not specified)
 *
 * @return POS_RET_NORMAL               Normal completion(Include illegal)<br>
 *         POS_RET_ERROR_BUFFULL        Buffer-full<br>
 *         POS_RET_ERROR_INNER          Internal error<br>
 *         POS_RET_ERROR_PARAM          Parameter error<br>
 *         POS_RET_ERROR_NOSUPPORT      Unsupported environment
 *
 */
POS_RET_API POS_RegisterListenerSpeed(HANDLE hApp,  // NOLINT(readability/nolint)
                                      PCSTR notifyName,  // NOLINT(readability/nolint)
                                      uint8_t ucCtrlFlg,   // NOLINT(readability/nolint)
                                      uint8_t ucDeliveryTiming,  // NOLINT(readability/nolint)
                                      uint8_t ucGetMethod) {  // NOLINT(readability/nolint)
    POS_RET_API       ret = POS_RET_NORMAL;            /* Return value of this function            */
    SENSOR_RET_API    ret_sens = SENSOR_RET_NORMAL;    /* API return value             */
    UNIT_TYPE         type = UNIT_TYPE_NONE;           /* Supported HW Configuration Type    */
    DID               did;                             /* Data ID             */

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    /* Arguments & Support Configuration Check */
    if ((ucDeliveryTiming != SENSOR_DELIVERY_TIMING_CHANGE) &&
        (ucDeliveryTiming != SENSOR_DELIVERY_TIMING_UPDATE)) {
        /* Change delivery timing,Terminate as a parameter error except update */
        ret = POS_RET_ERROR_PARAM;
    } else if (ucCtrlFlg != SENSOR_DELIVERY_REGIST) {
        /* Parameters other than delivery registration are terminated abnormally when delivery control is terminated. */
        ret = POS_RET_ERROR_PARAM;
    } else if (hApp == NULL) {
        /* If the handler is NULL, the process terminates with an error. */
        ret = POS_RET_ERROR_PARAM;
    } else if (notifyName == NULL) {
        /* If the thread name is NULL, it terminates with an abnormal parameter. */
        ret = POS_RET_ERROR_PARAM;
    } else {
        /* Positioning Base API initialization */
        _pb_Setup_CWORD64_API(hApp);

        /* Supported HW Configuration Check */
        type = GetEnvSupportInfo();
        if (UNIT_TYPE_GRADE1 == type) {
            /* GRADE1 */
            ret = POS_RET_NORMAL;
            if ((ucGetMethod == SENSOR_GET_METHOD_POS) ||
                (ucGetMethod == SENSOR_GET_METHOD_AUTO)) {
                did = VEHICLE_DID_MOTION_SPEED_INTERNAL;
            } else {
                /* End as a parameter error abnormality except for POS/unspecified acquisition method */
                ret = POS_RET_ERROR_PARAM;
            }
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
    if (ret == POS_RET_NORMAL) {
        /* Delivery registry SensorAPI calls */
        ret_sens = PosRegisterListenerProc(notifyName,           /* Destination thread name */
                                           did,                  /* Data ID */
                                           ucCtrlFlg,            /* Delivery control */
                                           ucDeliveryTiming);    /* Delivery timing */

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
 *  Compass Information Transmission Register
 *
 *  Register the delivery of the vehicle orientation information
 *
 * @param[in] hApp              HANDLE  - Application handle
 * @param[in] notifyName        PCSTR   - Destination thread name
 * @param[in] ucCtrlFlg         uint8_t - Delivery control(Delivery registration/Delivery stop/Resume delivery)
 * @param[in] ucDeliveryTiming  uint8_t - Delivery timing(Changing/Updating)
 * @param[in] ucGetMethod       uint8_t - Acquisition method(GPS/Navigation/DR/Not specified)
 *
 * @return POS_RET_NORMAL               Normal completion(Include illegal)<br>
 *         POS_RET_ERROR_PARAM          Parameter error<br>
 *         POS_RET_ERROR_NOSUPPORT      Unsupported environment
 *
 */
POS_RET_API POS_RegisterListenerHeading(HANDLE hApp,  // NOLINT(readability/nolint)
                                        PCSTR notifyName,  // NOLINT(readability/nolint)
                                        uint8_t ucCtrlFlg,  // NOLINT(readability/nolint)
                                        uint8_t ucDeliveryTiming,  // NOLINT(readability/nolint)
                                        uint8_t ucGetMethod) {  // NOLINT(readability/nolint)
    POS_RET_API       ret = POS_RET_NORMAL;                /* Return value */
    SENSOR_RET_API    ret_sens = SENSOR_RET_NORMAL;        /* API return value */
    UNIT_TYPE         type = UNIT_TYPE_NONE;               /* Supported HW Configuration Type */
    DID               did = VEHICLE_DID_MOTION_HEADING;    /* TODO VEHICLE_DID_LOCATION_HEADING Missing */
    /* Data ID */

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    /* Arguments & Support Configuration Check */
    if ((ucDeliveryTiming != SENSOR_DELIVERY_TIMING_CHANGE) &&
        (ucDeliveryTiming != SENSOR_DELIVERY_TIMING_UPDATE)) {
        /* Change delivery timing,Terminate as a parameter error except update */
        ret = POS_RET_ERROR_PARAM;
    } else if (ucCtrlFlg != SENSOR_DELIVERY_REGIST) {
        /* Parameters other than delivery registration are terminated abnormally when delivery control is terminated. */
        ret = POS_RET_ERROR_PARAM;
    } else if (hApp == NULL) {
        /* If the handler is NULL, the process terminates with an error. */
        ret = POS_RET_ERROR_PARAM;
    } else if (notifyName == NULL) {
        /* If the thread name is NULL, it terminates with an abnormal parameter. */
        ret = POS_RET_ERROR_PARAM;
    } else {
        /* Positioning Base API initialization */
        _pb_Setup_CWORD64_API(hApp);

        /* Supported HW Configuration Check */
        type = GetEnvSupportInfo();
        if (UNIT_TYPE_GRADE1 == type) {
            /* GRADE1 */
            ret = POS_RET_NORMAL;
            if ((ucGetMethod == SENSOR_GET_METHOD_GPS) ||
                (ucGetMethod == SENSOR_GET_METHOD_AUTO)) {
                did = VEHICLE_DID_MOTION_HEADING;
            } else {
                /* End as a parameter error abnormality except for GPS/unspecified acquisition method */
                ret = POS_RET_ERROR_PARAM;
            }
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

    /* Delivery registration */
    if (ret == POS_RET_NORMAL) {
        /* Delivery registry SensorAPI calls */
        ret_sens = PosRegisterListenerProc(notifyName,           /* Destination thread name */
                                           did,                  /* Data ID */
                                           ucCtrlFlg,            /* Delivery control */
                                           ucDeliveryTiming);    /* Delivery timing */

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
 *  Get Lltitude and longitude information
 *
 *  Get Lltitude and longitude information
 *
 * @param[in] hApp          HANDLE                          - Application handle
 * @param[in] dat           SENSORLOCATION_LONLATINFO_DAT*  - Pointer to the acquired latitude/longitude information storage destination
 * @param[in] ucGetMethod   uint8_t                         - Acquisition method(GPS/Navigation/DR/Not specified)
 *
 * @return POS_RET_NORMAL               Normal completion(Include illegal)<br>
 *         POS_RET_ERROR_PARAM          Parameter error<br>
 *         POS_RET_ERROR_NOSUPPORT      Unsupported environment<br>
 *         POS_RET_ERROR_INNER          Internal error
 *
 */
//POS_RET_API POS_GetLonLat(HANDLE hApp,   // NOLINT(readability/nolint)
//                         SENSORLOCATION_LONLATINFO_DAT *dat,  // NOLINT(readability/nolint)
//                         uint8_t ucGetMethod) {  // NOLINT(readability/nolint)
//    POS_RET_API ret = POS_RET_NORMAL;                 /* Return value */
//    UNIT_TYPE   type = UNIT_TYPE_NONE;                /* Supported HW Configuration Type */
//    DID         did = VEHICLE_DID_LOCATION_LONLAT;    /* DID */
//    int32       ret_get_proc;                                /* POS_GetProc Return Values */
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
//            /* GRADE1 */
//            if ((ucGetMethod == SENSOR_GET_METHOD_GPS) ||
//                (ucGetMethod == SENSOR_GET_METHOD_AUTO)) {
//                did = VEHICLE_DID_LOCATION_LONLAT;
//            } else {
//                /* End as a parameter error abnormality except for GPS/unspecified acquisition method */
//                ret = POS_RET_ERROR_PARAM;
//            }
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
//    /* Sensor information acquisition */
//    if (ret == POS_RET_NORMAL) {
//        /* Data acquisition process */
//        ret_get_proc = PosGetProc(did, reinterpret_cast<void *>(dat), sizeof(SENSORLOCATION_LONLATINFO_DAT));
//        if (static_cast<int32>(sizeof(SENSORLOCATION_LONLATINFO_DAT)) > ret_get_proc) {
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
//    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "- [ret = %d]", ret);
//    return ret;
//}

/**
 * @brief
 *  Altitude information acquisition
 *
 *  Obtain altitude information
 *
 * @param[in] hApp          HANDLE                           - Application handle
 * @param[in] dat           SENSORLOCATION_ALTITUDEINFO_DAT* - Pointer to the acquired altitude information storage destination
 * @param[in] ucGetMethod   uint8_t                          - Acquisition method(GPS/Navigation/DR/Not specified)
 *
 * @return POS_RET_NORMAL               Normal completion(Include illegal)<br>
 *         POS_RET_ERROR_PARAM          Parameter error<br>
 *         POS_RET_ERROR_NOSUPPORT      Unsupported environment<br>
 *         POS_RET_ERROR_INNER          Internal error
 *
 */
//POS_RET_API POS_GetAltitude(HANDLE hApp,  // NOLINT(readability/nolint)
//                            SENSORLOCATION_ALTITUDEINFO_DAT *dat,  // NOLINT(readability/nolint)
//                            uint8_t ucGetMethod) {  // NOLINT(readability/nolint)
//    POS_RET_API ret = POS_RET_NORMAL;                   /* Return value */
//    UNIT_TYPE   type = UNIT_TYPE_NONE;                  /* Supported HW Configuration Type */
//    DID         did = VEHICLE_DID_LOCATION_ALTITUDE;    /* DID */
//    int32       ret_get_proc;                           /* POS_GetProc Return Values */
//
//    /* Internal debug log output */
//    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");
//
//    /* Arguments & Support Configuration Check */
//    if (hApp == NULL) {
//        /* If the handler is NULL, the process terminates with an error. */
//        ret = POS_RET_ERROR_PARAM;
//    } else if (dat == NULL) {
//        /* If the altitude data is NULL, it terminates with an abnormal parameter. */
//        ret = POS_RET_ERROR_PARAM;
//    } else {
//        /* Positioning Base API initialization */
//        _pb_Setup_CWORD64_API(hApp);
//
//        /* Supported HW Configuration Check */
//        type = GetEnvSupportInfo();
//        if (UNIT_TYPE_GRADE1 == type) {
//            /* GRADE1 */
//            if ((ucGetMethod == SENSOR_GET_METHOD_GPS) ||
//                (ucGetMethod == SENSOR_GET_METHOD_AUTO)) {
//                did = VEHICLE_DID_LOCATION_ALTITUDE;
//            } else {
//                /* End as a parameter error abnormality except for GPS/unspecified acquisition method */
//                ret = POS_RET_ERROR_PARAM;
//            }
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
//    /* Sensor information acquisition */
//    if (ret == POS_RET_NORMAL) {
//        /* Data acquisition process */
//        ret_get_proc = PosGetProc(did, reinterpret_cast<void *>(dat), sizeof(SENSORLOCATION_ALTITUDEINFO_DAT));
//        if (static_cast<int32>(sizeof(SENSORLOCATION_ALTITUDEINFO_DAT)) > ret_get_proc) {
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
//    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "- [ret = %d]", ret);
//    return ret;
//}

/**
 * @brief
 *  Vehicle Speed Information Acquisition
 *
 *  Obtain vehicle speed information
 *
 * @param[in] hApp          HANDLE                      - Application handle
 * @param[in] dat           SENSORMOTION_SPEEDINFO_DAT* - Pointer to the acquired car speed information storage destination
 * @param[in] ucGetMethod   uint8_t                     - Acquisition method(GPS/Navigation/DR/Not specified)
 *
 * @return POS_RET_NORMAL               Normal completion(Include illegal)<br>
 *         POS_RET_ERROR_PARAM          Parameter error<br>
 *         POS_RET_ERROR_NOSUPPORT      Unsupported environment<br>
 *         POS_RET_ERROR_INNER          Internal error
 *
 */
//POS_RET_API POS_GetSpeed(HANDLE hApp,  // NOLINT(readability/nolint)
//                         SENSORMOTION_SPEEDINFO_DAT *dat,  // NOLINT(readability/nolint)
//                         uint8_t ucGetMethod) {  // NOLINT(readability/nolint)
//    POS_RET_API    ret = POS_RET_NORMAL;     /* Return value                 */
//    UNIT_TYPE      type = UNIT_TYPE_NONE;    /* Supported HW Configuration Type     */
//    DID            did;                      /* Data ID             */
//    int32          ret_get_proc;             /* POS_GetProc Return Values */
//
//    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");
//    if (hApp == NULL) {
//        /* If the handler is NULL, the process terminates with an error. */
//        ret = POS_RET_ERROR_PARAM;
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [hApp = %p]", hApp);
//    } else if (dat == NULL) {
//        /* When the pointer to the vehicle speed data storage destination is NULL, the pointer terminates with an error in the parameter. */
//        ret = POS_RET_ERROR_PARAM;
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [dat = %p]", dat);
//    } else {
//        /* Positioning Base API initialization */
//        _pb_Setup_CWORD64_API(hApp);
//
//        /* Supported HW Configuration Check */
//        type = GetEnvSupportInfo();
//        if (UNIT_TYPE_GRADE1 == type) {
//            /* GRADE1 */
//            ret = POS_RET_NORMAL;
//            if ((ucGetMethod == SENSOR_GET_METHOD_POS) ||
//                (ucGetMethod == SENSOR_GET_METHOD_AUTO)) {
//                did = VEHICLE_DID_MOTION_SPEED_INTERNAL;
//            } else {
//                /* End as a parameter error abnormality except for POS/unspecified acquisition method */
//                ret = POS_RET_ERROR_PARAM;
//                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
//                              "Argument ERROR [type = %d, ucGetMethod = %d]",
//                              type, ucGetMethod);
//            }
//        } else if (UNIT_TYPE_GRADE2 == type) {
//          /*
//           *  Note.
//           *  This feature branches processing depending on the unit type.
//           */
//            ret = POS_RET_ERROR_NOSUPPORT;
//        } else {
//            /* Environment error */
//            ret = POS_RET_ERROR_NOSUPPORT;
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "GetEnvSupportInfo ERROR [type = %d]", type);
//        }
//    }
//
//    if (ret == POS_RET_NORMAL) {
//        /* Data acquisition process */
//        ret_get_proc = PosGetProc(did, dat, sizeof(SENSORMOTION_SPEEDINFO_DAT));
//        if (static_cast<int32>(sizeof(SENSORMOTION_SPEEDINFO_DAT)) > ret_get_proc) {
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
//    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "- [ret = %d]", ret);
//
//    return ret;
//}

/**
 * @brief
 *  Compass information acquisition
 *
 *  Get Bill Direction Information
 *
 * @param[in] hApp          HANDLE                        - Application handle
 * @param[in] dat           SENSORMOTION_HEADINGINFO_DAT* - Pointer to the acquired altitude information storage destination
 * @param[in] ucGetMethod   uint8_t                       - Acquisition method(GPS/Navigation/DR/Not specified)
 *
 * @return POS_RET_NORMAL               Normal completion(Include illegal)<br>
 *         POS_RET_ERROR_PARAM          Parameter error<br>
 *         POS_RET_ERROR_NOSUPPORT      Unsupported environment<br>
 *         POS_RET_ERROR_INNER          Internal error
 *
 */
//POS_RET_API POS_GetHeading(HANDLE hApp,  // NOLINT(readability/nolint)
//                           SENSORMOTION_HEADINGINFO_DAT *dat,  // NOLINT(readability/nolint)
//                           uint8_t ucGetMethod) {  // NOLINT(readability/nolint)
//    POS_RET_API ret = POS_RET_NORMAL;                /* Return value */
//    UNIT_TYPE   type = UNIT_TYPE_NONE;               /* Supported HW Configuration Type */
//    DID         did = VEHICLE_DID_MOTION_HEADING;    /* Data ID */
//    int32       ret_get_proc;                        /* POS_GetProc Return Values */
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
//            /* GRADE1 */
//            if ((ucGetMethod == SENSOR_GET_METHOD_GPS) ||
//                (ucGetMethod == SENSOR_GET_METHOD_AUTO)) {
//                did = VEHICLE_DID_MOTION_HEADING;
//            } else {
//                /* End as a parameter error abnormality except for GPS/unspecified acquisition method */
//                ret = POS_RET_ERROR_PARAM;
//            }
//        } else if (UNIT_TYPE_GRADE2 == type) {
//          /*
//           *  Note.
//           *  This feature branches processing depending on the unit type.
//           */
//            ret = SENSOR_RET_ERROR_NOSUPPORT;
//        } else {
//            /* Environment error */
//            ret = SENSOR_RET_ERROR_NOSUPPORT;
//        }
//    }
//    /* Sensor information acquisition */
//    if (ret == POS_RET_NORMAL) {
//        /* Data acquisition process */
//        ret_get_proc = PosGetProc(did, reinterpret_cast<void *>(dat), sizeof(SENSORMOTION_HEADINGINFO_DAT));
//        if (static_cast<int32>(sizeof(SENSORMOTION_HEADINGINFO_DAT)) > ret_get_proc) {
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
//    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "- [ret = %d]", ret);
//    return ret;
//}

/**
 * @brief
 *  Vehicle speed information setting
 *
 *  Setting Vehicle Speed Information
 *
 * @param[in] hApp      HANDLE   - Application handle
 * @param[in] navispped uint16_t - Vehicle speed information[Unit:1.0km/h]
 *
 * @return POS_RET_NORMAL               Normal completion(Include illegal)<br>
 *         POS_RET_ERROR_PARAM          Parameter error<br>
 *         POS_RET_ERROR_NOSUPPORT      Unsupported environment
 *
 */
POS_RET_API POS_SetSpeedInfo(HANDLE hApp, uint16_t navispeed) {  // NOLINT(readability/nolint)
    POS_RET_API  ret = POS_RET_NORMAL;     /* Return value */
    UNIT_TYPE    type = UNIT_TYPE_NONE;    /* Supported HW Configuration Type */
    uint16_t     speed;                    /* Vehicle speed */

    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");

    /* Arguments & Support Configuration Check */
    if (hApp == NULL) {
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
            ret = SENSOR_RET_ERROR_NOSUPPORT;
        } else {
            /* Environment error */
            ret = SENSOR_RET_ERROR_NOSUPPORT;
        }
    }

    if (ret == POS_RET_NORMAL) {
        /* Adjustment by unit [1.0km/h]->[0.01m/sec] */
        speed = static_cast<u_int16>(navispeed * 10000 / 360);
        /* Data setting(After setting,Immediate termination) */
        ret = PosSetProc(VEHICLE_DID_MOTION_SPEED_NAVI,
                         reinterpret_cast<void *>(&speed), sizeof(uint16_t), FALSE);
    }

    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "- [ret = %d]", ret);
    return ret;
}

/**
 * @brief
 *  Location information setting
 *
 *  Set location information
 *
 * @param[in] hApp          HANDLE       - Application handle
 * @param[in] posData       POS_POSDATA  - Pointer to location information
 *
 * @return  POS_RET_NORMAL           Normal completion(Include illegal)<br>
 *          POS_RET_ERROR_PARAM      Parameter error<br>
 *          POS_RET_ERROR_INNER      Internal error<br>
 *          POS_RET_ERROR_NOSUPPORT  Unsupported environment
 *
 */
POS_RET_API POS_SetLocationInfo(HANDLE hApp, POS_POSDATA* pstPosData) {  // NOLINT(readability/nolint)
    POS_RET_API    ret = POS_RET_NORMAL;     /* Return value of this function */
    UNIT_TYPE      type = UNIT_TYPE_NONE;    /* Supported HW Configuration Type */

    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");

    /* Arguments & Support Configuration Check */
    if ((pstPosData == NULL) || (hApp == NULL)) {
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
            ret = SENSOR_RET_ERROR_NOSUPPORT;
        } else {
            /* Environment error */
            ret = SENSOR_RET_ERROR_NOSUPPORT;
        }
    }

    if (ret == POS_RET_NORMAL) {
        /* Data status check */
        if ((0x01 > pstPosData->status) || (0x0F < pstPosData->status)) {
            ret = POS_RET_ERROR_PARAM;
        } else {
            /* Parameter range check */
            /* Latitude */
            if ((pstPosData->status & POS_LOC_INFO_LAT) == POS_LOC_INFO_LAT) {
                (void)POS_CHKPARAM32(pstPosData->latitude, -41472000, 41472000);
            }
            /* Longitude */
            if ((pstPosData->status & POS_LOC_INFO_LON) == POS_LOC_INFO_LON) {
                (void)POS_CHKPARAM32(pstPosData->longitude, -82944000, 82944000);
            }
            /* Orientation */
            if ((pstPosData->status & POS_LOC_INFO_HEAD) == POS_LOC_INFO_HEAD) {
                (void)POS_CHKPARAM16(pstPosData->heading, -179, 180);
            }

            /* Data setting(After setting,Immediate termination) */
            ret = PosSetProc(VEHICLE_DID_GPS_CUSTOMDATA_NAVI,
                             reinterpret_cast<void *>(pstPosData), sizeof(POS_POSDATA), FALSE);
        }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "- [ret = %d]", ret);
    return ret;
}


/**
 * @brief
 *  Location Information (NMEA) Setting
 *
 *  Set Location Information (NEMA)
 *
 * @param[in] hApp          HANDLE       - Application handle
 * @param[in] locationInfo  POS_LOCATIONINFO_NEMA - Pointer to location information
 *
 * @return  POS_RET_NORMAL           Normal completion(Include illegal)<br>
 *          POS_RET_ERROR_PARAM      Parameter error<br>
 *          POS_RET_ERROR_INNER      Internal error<br>
 *          POS_RET_ERROR_NOSUPPORT  Unsupported environment
 *
 */
POS_RET_API POS_SetLocationInfoNmea( HANDLE hApp, POS_LOCATIONINFO_NMEA* locationInfo ) {
    POS_RET_API	lRet = POS_RET_NORMAL;		/* Return value of this function */
    UNIT_TYPE		eType = UNIT_TYPE_NONE;		/* Supported HW Configuration Type */

    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");

    /* Arguments & Support Configuration Check */
    if ((locationInfo == NULL) || (hApp == NULL)) {
        lRet = POS_RET_ERROR_PARAM;
    } else {
        /* Positioning Base API initialization */
        _pb_Setup_CWORD64_API(hApp);

        /* Supported HW Configuration Check */
        eType = GetEnvSupportInfo();
        if ( UNIT_TYPE_GRADE1 == eType ) {
            /* GRADE1 */
            lRet = POS_RET_NORMAL;
        } else if ( UNIT_TYPE_GRADE2 == eType ) {
          /*
           *  Note.
           *  This feature branches processing depending on the unit type.
           */
            lRet = SENSOR_RET_ERROR_NOSUPPORT;
        } else {
            /* Environment error */
            lRet = SENSOR_RET_ERROR_NOSUPPORT;
        }
    }

    if ( lRet == POS_RET_NORMAL ) {
        /* check data length */
        if ((locationInfo->length == 0) || (locationInfo->length > LOCATIONINFO_NMEA_MAX)) {
            lRet = POS_RET_ERROR_PARAM;
        } else {
            /* Data setting(After setting,Immediate termination) */
            lRet = PosSetProc( VEHICLE_DID_LOCATIONINFO_NMEA_NAVI, (void*)locationInfo,
                               locationInfo->length + sizeof(locationInfo->length), FALSE );
        }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "- [lRet = %d]", lRet);
    return lRet;
}


