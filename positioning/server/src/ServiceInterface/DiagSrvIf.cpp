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
 *    DiagSrvIf.cpp
 * @brief
 *    DiagSrv service-to-service interface
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
//#include <stub/DiagCodeAPI.h>
#include "DiagSrvIf.h"

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
static BOOL g_registration_permission = FALSE; /* Whether or not the dialog code can be registered */

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *    DiagSrv Services IF Registration Enable Status Setting
 *
 * @param[in]  b_is_true
 * @return     none
 */
void DiagSrvIfSetRegistrationPermission(BOOL b_is_true) {
    g_registration_permission = b_is_true;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, \
        "g_registration_permission=%d", g_registration_permission);

    return;
}

/**
 * @brief
 *    Registering DiagSrv Services IF-Diag Codes
 *
 * @param[in]  err_id       Diag code definition name
 * @param[in]  positioning_code    Positioning Code-Defined Names
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus DiagSrvIfDiagPutDiagCode(uint64_t err_id, uint16_t positioning_code) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
//    HANDLE happ;
//    DGCODE_RET_API dc_ret_api;
//
//    if (g_registration_permission == TRUE) {
//        happ = _pb_GetAppHandle();
//        if (happ != NULL) {
//            /* Dialog code registration */
//            dc_ret_api = Diag_PutDiagCode(happ, err_id, positioning_code);
//            if (dc_ret_api != DGCODE_RET_NORMAL) {
//                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
//                              "Diag_PutDiagCode ERROR!! [ret=%d, positioning_code=%u]", dc_ret_api, positioning_code);
//            } else {
//                estatus = eFrameworkunifiedStatusOK;
//                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, 
//                              "Diag_PutDiagCode CALL [ret=%d, positioning_code=%u]", dc_ret_api, positioning_code);
//            }
//        } else {
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
//        }
//    } else {
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "g_registration_permission=FALSE");
//    }

    return estatus;
}

/**
 * @brief
 *    Deleting DiagSrv Services IF-Diag Codes
 *
 * @param[in]  err_id       Diag code definition name
 * @param[in]  positioning_code    Positioning Code-Defined Names
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus DiagSrvIfDiagDeleteDiagCode(uint64_t err_id, uint16_t positioning_code) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
//    HANDLE happ;
//    DGCODE_RET_API dc_ret_api;
//
//    if (g_registration_permission == TRUE) {
//        happ = _pb_GetAppHandle();
//        if (happ != NULL) {
//            /* Diag code deletion */
//            dc_ret_api = Diag_DeleteDiagCode(happ, err_id, positioning_code);
//            if (dc_ret_api != DGCODE_RET_NORMAL) {
//                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
//                              "Diag_DeleteDiagCode ERROR!! [ret=%d, positioning_code=%u]", dc_ret_api, positioning_code);
//            } else {
//                estatus = eFrameworkunifiedStatusOK;
//                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, 
//                              "DiagSrvIfDiagDeleteDiagCode CALL [ret=%d, positioning_code=%u]", dc_ret_api, positioning_code);
//            }
//        } else {
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
//        }
//    } else  {
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "g_registration_permission=FALSE");
//    }

    return estatus;
}
