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
 *    PSMShadowIf.cpp
 * @brief
 *    PSMShadow service-to-service interface
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "PSMShadowIf.h"

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

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *    PSMShadow Services IF Availability Change Notification Registration
 *
 * @param[in]  fp_on_cmd    Callback function
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus PSMShadowIfNotifyOnPSMShadowAvailability(CbFuncPtr fp_on_cmd) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
    HANDLE happ;

    happ = _pb_GetAppHandle();
    if (happ != NULL) {
        /* PS_PSMShadow/Availability Changing notification registration */
        estatus = FrameworkunifiedSubscribeNotificationWithCallback(happ, NTFY_PSMShadowService_Availability, fp_on_cmd);  // LCOV_EXCL_BR_LINE 6:unexpected branch  //NOLINT (whitespace/line_length)
        if (eFrameworkunifiedStatusOK != estatus) {  // LCOV_EXCL_BR_LINE 4: nsfw error
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                          "PositioningSubscriveNotificationswithCallback ERROR!! [estatus=%d]", estatus);
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
    }

    return estatus;
}

/**
 * @brief
 *    Register PSMShadow services IF start completion notification
 *
 * @param[in]  fp_on_cmd    Callback function
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus PSMShadowIfNotifyOnPSMShadowInitComp(CbFuncPtr fp_on_cmd) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
    HANDLE happ;

    happ = _pb_GetAppHandle();
    if (happ != NULL) {
        /* PS_PSMShadow startup completion notice */
        estatus = FrameworkunifiedSubscribeNotificationWithCallback(happ, NTFY_PSM_INITCOMP, fp_on_cmd);  // LCOV_EXCL_BR_LINE 6:unexpected branch  //NOLINT (whitespace/line_length)
        if (eFrameworkunifiedStatusOK != estatus) {  // LCOV_EXCL_BR_LINE 4: nsfw error
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                          "PositioningSubscriveNotificationswithCallback ERROR!! [estatus=%d]", estatus);
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
    }

    return estatus;
}

