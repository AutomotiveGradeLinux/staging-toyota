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

/* ====================================================================== */
/**
 * @file VP_FuncCheck_CanRcv.h
 * @brief \~english Get function existence API header file
 */
/* ====================================================================== */

#ifndef VEHICLE_PARAMETER_LIBRARY_LIBRARY_INCLUDE_OTHER_SERVICE_VP_FUNCCHECK_CANRCV_H_
#define VEHICLE_PARAMETER_LIBRARY_LIBRARY_INCLUDE_OTHER_SERVICE_VP_FUNCCHECK_CANRCV_H_

#include <sys/types.h>

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup other_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle_parameter_library
 *  @ingroup other_service
 *  @{
 */

// typedef unsigned char u_int8;


/**
* @~english  XM Audio
*/
#define XM_AUDIO                        ((u_int8_t)1)
/**
* @~english  _CWORD76_
*/
#define _CWORD76_                             ((u_int8_t)2)

/**
* @~english HD Data (Include traffic)
*/
#define VP_CANRCV__CWORD27_               ((u_int8_t)3)                           /**< _CWORD27_                   */
/**
* @~english  WiFi HotSpot
*/
#define WIFI_HOTSPOT                    ((u_int8_t)4)
/**
* @~english standard driving rregulation of AREA1
*/
#define AREA1_STD_TRAF_LIMIT               ((u_int8_t)5)
/**
* @~english HD Data (Include traffic)
*/
#define HD_DATA                         ((u_int8_t)6)

/**
* @~english definiton of return value of API(VP_FuncCheck_CanRcv),which is used to get status of function implemention
*/
#define FUNCTION_CHECK_RESULT_OK        ((u_int8_t)0)
/**
* @~english   this function is invalid
*/
#define FUNCTION_CHECK_RESULT_NG        ((u_int8_t)1)
/**
* @~english this function does not exist
*/
#define FUNCTION_CHECK_RESULT_UNKNOWN   ((u_int8_t)2)
/**
* @~english reference out of AREA1
*/
#define FUNCTION_CHECK_RESULT_OUT_OF_AREA1 ((u_int8_t)3)

/* Function prototype declaration */
#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
/// \ingroup VP_FuncCheck_CanRcv
/// \~english @par Brief
///         VP_FuncCheck_CanRcv
/// \~english @param [in] FunctionName
///         function name
/// \~english @retval FUNCTION_CHECK_RESULT_OK: This function is available
/// \~english @retval FUNCTION_CHECK_RESULT_NG: This function is unavailable
/// \~english @retval FUNCTION_CHECK_RESULT_UNKNOWN: This function does not exist
/// \~english @par Prerequisite
///         None
/// \~english @par Change of internal state
///         None
/// \~english @par Conditions of processing failure
///         None
/// \~english @par Classification
///         Public
/// \~english @par Type
///         Sync Only
/// \~english @par Detail
///         - To get the function check result from
///            - vehicle parameter:CAN info judge
///            - vehicle parameter:country ID
///            - country ID and function judge info in API
///         - When vehicle parameter:CAN info judge
///           is [CAN_judge] or [country_ID],
///           and if country_ID is unacquainted or undefined, the API returns [available].
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
u_int8_t VP_FuncCheck_CanRcv(u_int8_t FunctionName);

#ifdef __cplusplus
}
#endif

/** @}*/  // end of vehicle_parameter_library
/** @}*/  // end of other_service
/** @}*/  // end of BaseSystem

#endif  // VEHICLE_PARAMETER_LIBRARY_LIBRARY_INCLUDE_OTHER_SERVICE_VP_FUNCCHECK_CANRCV_H_
