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

/* ====================================================================== */
/**
 * @file VP_GetEnv.h
 * @brief \~english Get vehicle parameter environment variable API header file
 */
/* ====================================================================== */

#ifndef VEHICLE_PARAMETER_LIBRARY_LIBRARY_INCLUDE_OTHER_SERVICE_VP_GETENV_H_
#define VEHICLE_PARAMETER_LIBRARY_LIBRARY_INCLUDE_OTHER_SERVICE_VP_GETENV_H_

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
#include <other_service/env_ext.h>

/**
* @~english the max length of vehicle parameter environment viarable
*/

#define     VP_MAX_LENGTH   128

/**
* @~english definition of parameter environment viarable such as destination
*           destination diff
*/

#define     VEHICLEPARAMETERLIBRARY_AREA       "VEHICLEPARAMETERLIBRARY_AREA"

/**
* @~english brand diff
*/
#define     VEHICLEPARAMETERLIBRARY_BRAND      "VEHICLEPARAMETERLIBRARY_BRAND"

/**
* @~english  definition of _CWORD31_ vehicle parameter environment viarable
*/
#include <other_service/env_vehicle.h>

/**
* @~english definition of vehicle parameter environment viarable
*/
#include <other_service/env_vehicleparameterlibrary.h>

/**
* @~english definition of Serial vehicle parameter environment viarable
*/
#include <other_service/env_vehicleparameterlibrary_serial.h>

/* Function prototype declaration */
#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
/// \ingroup VP_GetEnv
/// \~english @par Brief
///         VP_GetEnv
/// \~english @param [in] pEnvStr
///         The pointer of environment variable name string
/// \~english @param [out] pEnvBuff
///         The head address of saving environment variable value
/// \~english @retval None
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
///         - To get the environment variable value by the environment variable name,
///           and save it in pEnvBuff.
///         - When environment variable name is NULL or an undefined value,
///           a blank string shall be saved in pEnvBuff.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
void VP_GetEnv(const char *pEnvStr, char *pEnvBuff);

#ifdef __cplusplus
}
#endif

/** @}*/  // end of vehicle_parameter_library
/** @}*/  // end of other_service
/** @}*/  // end of BaseSystem

#endif  // VEHICLE_PARAMETER_LIBRARY_LIBRARY_INCLUDE_OTHER_SERVICE_VP_GETENV_H_
