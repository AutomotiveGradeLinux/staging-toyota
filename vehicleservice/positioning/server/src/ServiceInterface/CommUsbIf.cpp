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
 *    CommUsbIf.cpp
 * @brief
 *    CommUSB service-to-service interface
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "CommUsbIf.h"

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
static BOOL g_comm_usb_availability = FALSE;

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *    Registering CommUSB Services IF Callback Functions
 *
 * @param[in]  *p_msg_handler    Callback function table
 * @param[in]  Ui_handler_count Number of callback functions
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus CommUsbIfAttachCallbacksToDispatcher(    // NOLINT(readability/nolint)
                                                _FrameworkunifiedProtocolCallbackHandler const* p_msg_handler,
                                                unsigned int ui_handler_count) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
    HANDLE happ;

    happ = _pb_GetAppHandle();
    if (happ != NULL) {
        estatus = FrameworkunifiedAttachCallbacksToDispatcher(happ, "NS_ANY_SRC", p_msg_handler, ui_handler_count);
        if (eFrameworkunifiedStatusOK != estatus) {  // LCOV_EXCL_BR_LINE 4: nsfw error
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                          "FrameworkunifiedAttachCallbacksToDispatcher ERROR!! [estatus=%d]", estatus);
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
    }

    return estatus;
}

/**
 * @brief
 *    Remove CommUSB Services IF Callback Functions
 *
 * @param[in]  *p_msg_handler    CID table
 * @param[in]  ui_handler_count  Number of CIDs
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus CommUsbIfDetachCallbacksFromDispatcher(const PUI_32 pui_cmd_array, UI_32 ui_command_count) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
    HANDLE happ;

    happ = _pb_GetAppHandle();
    if (happ != NULL) {
        estatus = FrameworkunifiedDetachCallbacksFromDispatcher(happ, "NS_ANY_SRC", pui_cmd_array, ui_command_count, NULL);
        /* In the event of failure */
        if (estatus != eFrameworkunifiedStatusOK) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                          "PositioningDetachCallbacksToDispatcher Failed in FrameworkunifiedOnStop [estatus=%d]", estatus);
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
    }

    return estatus;
}

/**
 * @brief
 *    CommUSB Services IF Availability Change Notification Registration
 *
 * @param[in]  fp_on_cmd    Callback function
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus CommUsbIfNotifyOnCommUSBAvailability(CbFuncPtr fp_on_cmd) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
    HANDLE happ;

    happ = _pb_GetAppHandle();
    if (happ != NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        /* PS_CommUSB/Availability Changing notification registration */
        estatus = FrameworkunifiedSubscribeNotificationWithCallback(happ, NTFY_CommUSB_Availability, fp_on_cmd);  // LCOV_EXCL_BR_LINE 6:unexpected branch  //NOLINT (whitespace/line_length)
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
 *    CommUSB Services IF-Availability Settings
 *
 * @param[in]  pb_is_available
 * @return     none
 */
void CommUsbIfSetAvailability(BOOL b_is_available) {
    g_comm_usb_availability = b_is_available;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "g_comm_usb_availability=%d", g_comm_usb_availability);

    return;
}
