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

/*****************************************************************************
 * @file    DIAG_API_DCCC.h
 * @brief   Diagcode feature API defined
*****************************************************************************/


/*****************************************************************************
 * *********************************
 * *  Diagcode feature API defined *
 * *********************************
 *
 * ** Creation and release considerations **
 *   -Because DiagCode API depends on product specifications, it is created at the product level.
 *
 *   -Submit and request the created headers and dummy Lib(empty functions defined only for I/F)
 *    from DiagService to BaseSystem and build them from BaseSystem.
 *
 *   -The century Lib created(not dummy Lib) is built from DiagService.
 *
 *     ->Since this API is also used in BaseSystem,
 *       the API must be inserted from the BaseSystem built to avoid errors in JointBuild.
 *
 * ** Notes on change **
 *   -When modifying a header without cahnging the I/F,
 *    only the header should be submitted to the BaseSystem and requested.
 *
 *   -Submit header and source code for dummy Lib when the header is modified due to an I/F change.
 *
 *   -When the legitimate Lib is changed without changing headers and I/F,
 *    no need to submit or request to BaseSystem.
 *
*****************************************************************************/


#ifndef DIAG_CODE_LIBRARY_DUMMY_DCCC_INCLUDE_VEHICLE_SERVICE_DIAG_API_DCCC_H_
#define DIAG_CODE_LIBRARY_DUMMY_DCCC_INCLUDE_VEHICLE_SERVICE_DIAG_API_DCCC_H_

#include <native_service/frameworkunified_framework_if.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define        NTFY_DiagService_Availability_DCCC            ("DiagService/Availability")

/***********************************************************
    Enumerated type
***********************************************************/
/*! @brief  Return value for DiagCode re-checking and clearing feature APIs */
typedef enum {
    RET_DIAG_API_DCCC_NORMAL = 0,           //!<Success
    RET_DIAG_API_DCCC_ERROR                 //!<Error
} RET_DIAG_API_DCCC;

/***********************************************************
    Constant
***********************************************************/
// Common
/*! @brief  message ID */
#define MSGID_DIAG_API_DCCC_DELIVERY_ENTRY                  (0xFF000001)        // Delivery registration notice
#define MSGID_DIAG_API_DCCC_SND_RECHECK                     (0xFF000002)        // Re-assay instruction
#define MSGID_DIAG_API_DCCC_RET_RECHECK                     (0xFF000003)        // Re-assay completion notice
#define MSGID_DIAG_API_DCCC_SND_DIAGCODECLR                 (0xFF000004)        // Diag deletion instruction
#define MSGID_DIAG_API_DCCC_RET_DIAGCODECLR                 (0xFF000005)        // Daig deletion completion notice

/*! @brief  Delivery features */
#define DIAG_API_DCCC_DELIVERY_RECHECK                      (0x00000001)        // Recheck
#define DIAG_API_DCCC_DELIVERY_DIAGCODECLR                  (0x00000002)        // Diag delete
#define DIAG_API_DCCC_DELIVERY_ALL                          (0xFFFFFFFF)        // All function

/*! @brief  Result of re-assay execution */
#define DIAG_API_DCCC_RET_RECHECK_SUCCESS                   (0x00000000)        // Recheck successed
#define DIAG_API_DCCC_RET_RECHECK_ERROR                     (0x00000001)        // Recheck failed


/*! @brief  Diag deletion result */
#define DIAG_API_DCCC_RET_DIAGCODECLR_SUCCESS               (0x00000000)        // Diag deleion successed
#define DIAG_API_DCCC_RET_DIAGCODECLR_ERROR                 (0x00000001)        // Diag deleion failed

/***********************************************************
    API
***********************************************************/
extern RET_DIAG_API_DCCC DIAG_API_DCCC_DeliveryEntry(HANDLE hApp, PCSTR notifyName, u_int32_t delivery);
extern RET_DIAG_API_DCCC DIAG_API_DCCC_SND_ReCheck(HANDLE hApp, PCSTR notifyName);
extern RET_DIAG_API_DCCC DIAG_API_DCCC_RET_ReCheck(HANDLE hApp, PCSTR notifyName, u_int32_t status);
extern RET_DIAG_API_DCCC DIAG_API_DCCC_SND_DiagCodeCLR(HANDLE hApp, PCSTR notifyName);
extern RET_DIAG_API_DCCC DIAG_API_DCCC_RET_DiagCodeCLR(HANDLE hApp, PCSTR notifyName, u_int32_t status);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // DIAG_CODE_LIBRARY_DUMMY_DCCC_INCLUDE_VEHICLE_SERVICE_DIAG_API_DCCC_H_
