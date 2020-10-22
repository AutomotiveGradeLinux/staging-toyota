/*
 * @copyright Copyright (c) 2016-2020 TOYOTA MOTOR CORPORATION.
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
 *    DevDetectSrvIf.cpp
 * @brief
 *    DevDetectSrv service-to-service interface
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "DevDetectSrvIf.h"

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Structre                                                                        *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Local Function Prototype                                                        *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Grobal Value                                                                    *
 *---------------------------------------------------------------------------------*/
static BOOL g_dev_detect_srv_availability = FALSE;

/** Device insertion detection */
DeviceDetectionServiceIf    g_if_ss_dev;


/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *    DevDetectSrv Services IF-Availability Settings
 *
 * @param[in]  b_is_available    Available state
 * @return     none
 */
void DevDetectSrvIfSetAvailability(BOOL b_is_available) {
    g_dev_detect_srv_availability = b_is_available;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, \
        "g_dev_detect_srv_availability=%d", g_dev_detect_srv_availability);

    return;
}

/**
 * @brief
 *    DevDetectSrv Services IF-Initialization
 *
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus DevDetectSrvIfInitialize(void) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
    BOOL bret;
    HANDLE happ;

    happ = _pb_GetAppHandle();
    if (happ != NULL) {
        bret = g_if_ss_dev.Initialize(happ);
        if (bret == TRUE) {
            estatus = eFrameworkunifiedStatusOK;
        } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                "g_if_ss_dev::Initialize ERROR!! [estatus=%d]", estatus);
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
    }

    return estatus;
}

/**
 * @brief
 *    DevDetectSrv Services IF Availability Change Notification Registration
 *
 * @param[in]  fp_call_back_fn    Callback function
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus DevDetectSrvIfNotifyOnDeviceDetectionAvailability(CbFuncPtr fp_call_back_fn) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;

    estatus = g_if_ss_dev.NotifyOnDeviceDetectionAvailability(fp_call_back_fn);
    if (eFrameworkunifiedStatusOK != estatus) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
            "DeviceDetectionServiceIf Availability ERROR!! [estatus = %d]", estatus);
    }

    return estatus;
}

/**
 * @brief
 *    DevDetectSrv Services IF OpenSessionRequest Complete Answer Receive Callback Registration
 *
 * @param[in]  fp_call_back_fn    Callback function
 * @param[out] pb_is_available    Available state
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus DevDetectSrvIfNotifyOnOpenSessionAck(CbFuncPtr fp_call_back_fn, BOOL* pb_is_available) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;

    if (g_dev_detect_srv_availability == TRUE) {
        estatus = g_if_ss_dev.NotifyOnOpenSessionAck(fp_call_back_fn);
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "g_dev_detect_srv_availability=FALSE");
    }

    *pb_is_available = g_dev_detect_srv_availability;

    return estatus;
}

/**
 * @brief
 *    DevDetectSrv Services IF CloseSessionRequest Complete Answer Receive Callback Registration
 *
 * @param[in]  fp_call_back_fn    Callback function
 * @param[out] pb_is_available    Available state
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus DevDetectSrvIfNotifyOnCloseSessionAck(CbFuncPtr fp_call_back_fn, BOOL* pb_is_available) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;

    if (g_dev_detect_srv_availability == TRUE) {
        estatus = g_if_ss_dev.NotifyOnCloseSessionAck(fp_call_back_fn);
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "g_dev_detect_srv_availability=FALSE");
    }

    *pb_is_available = g_dev_detect_srv_availability;

    return estatus;
}

/**
 * @brief
 *    DevDetectSrv Service IF Service and Session Generation
 *
 * @param[out] pb_is_available    Available state
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus DevDetectSrvIfOpenSessionRequest(BOOL* pb_is_available) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;

    if (g_dev_detect_srv_availability == TRUE) {
        estatus = g_if_ss_dev.OpenSessionRequest();
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "g_dev_detect_srv_availability=FALSE");
    }

    *pb_is_available = g_dev_detect_srv_availability;

    return estatus;
}

/**
 * @brief
 *    DevDetectSrv Services IF-Session Id Retention
 *
 * @param[out] pb_is_available    Available state
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus DevDetectSrvIfDecodeOpenSessionResponse(BOOL* pb_is_available) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;

    if (g_dev_detect_srv_availability == TRUE) {
        estatus = g_if_ss_dev.DecodeOpenSessionResponse();
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "g_dev_detect_srv_availability=FALSE");
    }

    *pb_is_available = g_dev_detect_srv_availability;

    return estatus;
}

/**
 * @brief
 *    DevDetectSrv Services IF-Device Event Receive Callback Registration
 *
 * @param[in]  fe_dev_detect_event
 * @param[in]  fp_call_back_fn
 * @param[in]  p_file_path
 * @param[out] pb_is_available    Available state
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus DevDetectSrvIfRegisterForDeviceDetectionEvent(SS_DeviceDetectionServerEvents fe_dev_detect_event, \
        CbFuncPtr fp_call_back_fn, PCSTR p_file_path, BOOL* pb_is_available) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;

    if (g_dev_detect_srv_availability == TRUE) {
        estatus = g_if_ss_dev.RegisterForDeviceDetectionEvent(fe_dev_detect_event, fp_call_back_fn, p_file_path);
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "g_dev_detect_srv_availability=FALSE");
    }

    *pb_is_available = g_dev_detect_srv_availability;

    return estatus;
}

/**
 * @brief
 *    DevDetectSrv Services IF DeviceDetectionServiceIf Callback Release
 *
 * @param[in]  fe_dev_detect_event
 * @param[out] pb_is_available    Available state
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus DevDetectSrvIfUnRegisterForDeviceDetectionEvent(SS_DeviceDetectionServerEvents fe_dev_detect_event, \
        BOOL* pb_is_available) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;

    if (g_dev_detect_srv_availability == TRUE) {
        estatus = g_if_ss_dev.UnRegisterForDeviceDetectionEvent(fe_dev_detect_event);
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "g_dev_detect_srv_availability=FALSE");
    }

    *pb_is_available = g_dev_detect_srv_availability;

    return estatus;
}

/**
 * @brief
 *    DevDetectSrv Service IF Service and Destroy Sessions
 *
 * @param[out] pb_is_available    Available state
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus DevDetectSrvIfCloseSessionRequest(BOOL* pb_is_available) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;

    if (g_dev_detect_srv_availability == TRUE) {
        estatus = g_if_ss_dev.CloseSessionRequest();
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "g_dev_detect_srv_availability=FALSE");
    }

    *pb_is_available = g_dev_detect_srv_availability;

    return estatus;
}

