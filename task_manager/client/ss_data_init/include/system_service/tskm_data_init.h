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
 * @file tskm_data_init.h
 * @brief \~english This file contains API of initial task manager data
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup task_manager
 *  @ingroup system_service
 *  @{
 */

#ifndef TASK_MANAGER_CLIENT_SS_DATA_INIT_INCLUDE_SYSTEM_SERVICE_TSKM_DATA_INIT_H_
#define TASK_MANAGER_CLIENT_SS_DATA_INIT_INCLUDE_SYSTEM_SERVICE_TSKM_DATA_INIT_H_

#include <native_service/frameworkunified_types.h>
#include <system_service/ss_system_manager_if.h>
#include "system_service/tskm_type.h"

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup TSKM_GetExtBootInfo
/// \~english @par Summary
///        Gets boot extended information.
/// \~english @param [in] p_info
///        T_SS_SM_START_ExtDataStructType *   - Boot extended information
/// \~english @code
///            typedef struct {
///                BOOL             isProgUpdated;
///                EMRelaunchStatus relaunchStatus;
///                BOOL             isMapUpdated;
///                BOOL             isMapDiffUpdated;
///                uint8_t          reserved[];
///            }T_SS_SM_START_ExtDataStructType;
///            @endcode
/// \~english @retval TSKM_E_OK Normal end
/// \~english @retval TSKM_E_PAR %Parameter error
/// \~english @par Preconditions
///       - None
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       - p_info is NULL. [TSKM_E_PAR]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail
///       Is available only at the functions defined by using TSKM_DATA_INIT_FUNC.
/// \~english @see  TSKM_DATA_INIT_FUNC
////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t TSKM_GetExtBootInfo(T_SS_SM_START_ExtDataStructType *p_info);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup TSKM_DATA_INIT_FUNC
/// \~english @par Summary
///       Defines callback functions to initialize data.
/// \~english @param [in] serviceName
///        const char*   - Service name
/// \~english @param [in] argName
///        const T_SS_SM_START_DataStructType*   - Argument name
/// \~english @code
///            typedef struct T_SS_SM_START_DataStruct{
///                EPWR_WAKEUP_FACTOR_TYPE  startupReason;
///                BOOL                     isUserModeOn;
///                ESMDataResetModeInfo       dataResetMode;
///                EPWR_SC_SECURITY_STATUS  securityStatus;
///                EPWR_SC_WAKEUP_TYPE    wakeupType;
///                ESMDramBackupStatus    dramBackupStatus;
///                ESMResetStatus   resetStatus;
///                UI_32                        resetCount;
///            } T_SS_SM_START_DataStructType;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK Normal
/// \~english @retval eFrameworkunifiedStatusFail Check error
/// \~english @par Preconditions
///       - None
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       - Return values depend on the mount of callback functions. [eFrameworkunifiedStatusFail]
/// \~english @par Classification
///       Public
/// \~english @par Detail
///      Defines the callback functions to initialize data. \n
///       The callback functions defined by this macro are executed at a timing
///        when boot factor is fixed after booting the system. \n
///       (Task_Manager executes Callback prior to the boot of ActivityManager.)\n
///       If each service mounts and installs initialization functions, Task_Manager boots it
///        at a timing when data initialization is necessary. \n
///       Each service can install the object file of initialization functions by setting them
///        to MAKE variables INST_DATAINITOBJ of Makefile.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
#define TSKM_DATA_INIT_FUNC(ServiceName, ArgName) \
  EFrameworkunifiedStatus tskm_ ## ServiceName ## _data_init(T_SS_SM_START_DataStructType* ArgName)

#endif  // TASK_MANAGER_CLIENT_SS_DATA_INIT_INCLUDE_SYSTEM_SERVICE_TSKM_DATA_INIT_H_
/** @}*/  // end of Task_Manager
/** @}*/  // end of SystemService
/** @}*/  // end of BaseSystem
