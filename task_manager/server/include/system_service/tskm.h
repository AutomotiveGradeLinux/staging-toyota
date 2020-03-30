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
 * @file tskm.h
 * @brief \~english This file contains primary API of task manager
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

#ifndef TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_TSKM_H_
#define TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_TSKM_H_


#include <other_service/rpc.h>
#include "system_service/tskm_type.h"


#ifdef __cplusplus
extern "C" {
#endif

#define TSKM_ID             RPC_ID
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup TSKM_Init
/// \~english @par Summary:
///       Creates the resource to communicate with Task_Manager.
/// \~english @param [out] *pID
///        UINT32   - Unique ID of the automatically allocated program
/// \~english @retval None
/// \~english @par Preconditions:
///       - None
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       None
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Creates the resource to communicate with Task_Manager.  \n
///       It is necessary to call this before using the APIs of TSKM_xxxx defined after this. \n
///       It is necessary to call this by the thread. \n
/// \~english @see  TSKM_End
////////////////////////////////////////////////////////////////////////////////////
#define TSKM_Init(p_tskmId) RPC_START_CLIENT(p_tskmId)
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup TSKM_End
/// \~english @par Summary:
///        Releases the resource created at TSKM_Init.
/// \~english @param [in] ID
///        UINT32   - ID gotten at TSKM_Init()
/// \~english @retval None
/// \~english @par Preconditions:
///       - TSKM_Init must be used and initialized
/// \~english @par Changes of the internal stat:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       None
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Releases the resource created at TSKM_Init.
/// \~english @see  TSKM_Init
////////////////////////////////////////////////////////////////////////////////////
#define TSKM_End(tskmId)    RPC_end(tskmId)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup TSKM_SvcCtl
// \~english @par Summary:
///        Run the control for service.
/// \~english @param [in] svcid
///        TSKM_SVCID_t   - Service ID
/// \~english @param [in] ctl
///        TSKM_SVC_CTL_t*   - Command
/// \~english @code
///             typedef struct{
///               TSKM_SVC_CMD_t cmd;
///             } TSKM_SVC_CTL_t;
///            @endcode
/// \~english @retval TSKM_E_OK Succeeded
/// \~english @retval TSKM_E_PAR %Parameter error
/// \~english @retval TSKM_E_STATE State error
/// \~english @retval TSKM_E_NG Error
/// \~english @par Preconditions:
///       - TSKM_Init must be used and initialized
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - Parameters ctl is NULL [TSKM_E_PAR]
///       - Service context is not found [TSKM_E_PAR]
///       - Before FrameworkunifiedOnStart of Task_Manager is called [TSKM_E_STATE]
///       - It is prohibited to run designated service [TSKM_E_STATE]
///       - Message sending failed [TSKM_E_RETRY]
///       - When the specified service executes the start prohibition except during
///         the stop or start prohibition status. [TSKM_E_STATE]
///       - Maximum numbers to register running reservation service are exceeded [TSKM_E_STATE]
///       - Anything except for the commands specified as parameters is designated [TSKM_E_PAR]
///       - Writing in files of running reservation information failed [TSKM_E_NG]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       - TSKM_SVC_CMD_EXEC\n
///         Designated service runs. \n
///         When designated service already runs, this returns TSKM_E_OK without processing service running. \n
///         Running equal to local_step=all of request element is done. \n
///         When designated service designates the service that running is prohibited by
///         TSKM_SVC_CMD_DISABLE, this returns TSKM_E_STATE. \n
///         This guarantees that service running finished in API succeeding. \n
///       - TSKM_SVC_CMD_DISABLE\n
///         This prohibits to run the designated service. \n
///         At the prohibited service to run already, this returns TSKM_E_OK without doing anything. \n
///         When this API uses the service to be run already, this returns TSKM_E_STATE. \n
///         By this, at resident service, it is necessary to call this before running corresponding service. \n
///         About the prohibited service, this doesn't decide the wait condition of event completion notice
///         which service running, phase startup, phase end request, corresponding service control. \n
///         This command is prepared in consideration of changing the service to run in response to
///         with or without extended BOX and device configuration. \n
///         Initial values are running permission. \n
///       - TSKM_SVC_CMD_ENABLE\n
///         This permits to run the service to be set as running prohibition. \n
///         At running permission service, this returns TSKM_E_OK without doing anything. \n
///       - TSKM_SVC_RESERVE\n
///         This does the running reservation of non-resident service. \n
///         In next running, designated service runs automatically after finishing in running resident service. \n
///         Once this runs automatically, this forgets reservation information. \n
///         Maximum numbers of service able to do running reservation is the values of TSKM_SVC_RESERVE_MAX. \n
///         When this reserves over maximum numbers, this returns TSKM_E_STATE. \n
///         When backup RAM is invalid in running, this does not run automatically and
///         discards reservation information. \n
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t TSKM_SvcCtl(TSKM_SVCID_t svcId, const TSKM_SVC_CTL_t* ctl);
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup TSKM_SvcGetInfo
// \~english @par Summary:
///        Gets the information of designated service.
/// \~english @param [in] svcid
///        TSKM_SVCID_t   - Service ID
/// \~english @param [out] svcInfo
///        TSKM_SVC_INFO_t*   - Service information
/// \~english @code
///            typedef struct {
///              TSKM_SVCID_t   svcId;
///               TSKM_BOOL_t   isExecDisable;
///            } TSKM_SVC_INFO_t;
///              svcId  : Service ID
///              isExecDisable  : Service running prohibition state
///                TSKM_TRUE  : Running prohibition
///                TSKM_FALSE : Running permission
///            @endcode
/// \~english @retval TSKM_E_OK Succeeded
/// \~english @retval TSKM_E_PAR %Parameter error
/// \~english @par Preconditions:
///       - TSKM_Init must be used and initialized
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - Service context is not found [TSKM_E_PAR]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Gets the information of designated service.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t TSKM_SvcGetInfo(TSKM_SVCID_t svcId, TSKM_SVC_INFO_t* svcInfo);
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup TSKM_ErrorReboot
/// \~english @par Summary:
///        Let system do abnormal reboot.
/// \~english @param [in] p_info
///        TSKM_ERROR_REBOOT_t*   - REBOOT information
/// \~english @code
///            typedef struct {
///              TSKM_ERROR_REBOOT_TYPE_t type;
///              TSKM_LOGGING_ INFO_t   log;
///            } TSKM_ERROR_REBOOT_t;
///              type
///                TSKM_ERROR_REBOOT_NORMAL : Abnormal detection
///              log  : Log saving information
///                typedef struct {
///                  char   messageStr[TSKM_LOGGING_MSG_STR_SIZE];
///                } TSKM_LOGGING_INFO_ t;
///                  messageStr : Abnormal message character string
///            @endcode
/// \~english @retval TSKM_E_OK Succeeded
/// \~english @retval TSKM_E_PAR %Parameter error
/// \~english @par Preconditions:
///       - TSKM_Init must be used and initialized
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - Parameters p_info is NULL [TSKM_E_PAR]
///       - Reboot factors are except for TSKM_ERROR_REBOOT_NORMAL [TSKM_E_PAR]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Does REBOOT. \n
///       It is assumed to issue when system is damaged if processing continues any more
///       such as device trouble and non-volatile data trouble. \n
///       Access limitation is done to this API by gid and uid. \n
///       When an application with no authorization calls this, this force-quits the application to have called API. \n
///       This stores the string designated to messageStr as factors in system information LOG. \n
///       To link with PSMShadow about trouble factors, this designates abnormal message character string defined
///       inside ss_error_message.h to messageStr.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t TSKM_ErrorReboot(const TSKM_ERROR_REBOOT_t* p_info);
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup TSKM_Reboot
/// \~english @par Summary:
///         Let system do reboot.
/// \~english @param [in] rsv
///        TSKM_RSV_t*   - Reservation ( This must designate NULL)
/// \~english @retval TSKM_E_OK Succeeded
/// \~english @retval TSKM_E_PAR %Parameter error
/// \~english @par Preconditions:
///       - TSKM_Init must be used and initialized
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - Parameters p_rsv is except for NULL [TSKM_E_PAR]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync x Method
/// \~english @par Detail:
///       Let system do normal reboot.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t TSKM_Reboot(const TSKM_RSV_t * p_rsv);
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup TSKM_Logging
/// \~english @par Summary:
///        Stores LOG in internal non-volatile area.
/// \~english @param [in] p_info
///        TSKM_LOGGING_INFO_t *   - Log storing information
/// \~english @code
///            typedef struct{
///              TSKM_LOGGING_TYPE_NORMAL_t type;
///              char  messageStr[TSKM_LOGGING_MSG_STR_SIZE];
///            }TSKM_LOGGING_INFO_t;
///              type   : Log storing type
///                TSKM_LOGGING_TYPE_MODULE_LOGS: General LOG storing request from service
///                TSKM_LOGGING_TYPE_GRP_RELAUNCH: Log collection by Group Relaunch
///              messageStr : Trouble factors
///            @endcode
/// \~english @retval TSKM_E_OK Succeeded
/// \~english @retval TSKM_E_PAR %Parameter error
/// \~english @par Preconditions:
///       - TSKM_Init must be used and initialized
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - Parameters p_info is NULL [TSKM_E_PAR]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Collects LOG information and stores it in internal non-volatile area \n
///       This stores the string designated to messageStr as factors in system information LOG.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t TSKM_Logging(const TSKM_LOGGING_INFO_t* p_info);
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup TSKM_DataInit
/// \~english @par Summary:
///        Does the initialization process of service data.
/// \~english @param [in] hApp
///        HANDLE   - Application handle
/// \~english @param [in] p_info
///        TSKM_DATAINIT_t *   - Data initialization information
/// \~english @code
///               typedef struct {
///                 TSKM_DATAINIT_TYPE_t  type;
///                 EFrameworkunifiedStatus (*onCompInit)(HANDLE hApp);
///               } TSKM_DATAINIT_t;
///                 type  : Data initialization type
///                   TSKM_DATAINIT_TYPE_USER : User data initialization
///                 onCompInit  : Initialization complete Callback
///            @endcode
/// \~english @retval TSKM_E_OK Succeeded
/// \~english @retval TSKM_E_PAR %Parameter error
/// \~english @retval TSKM_E_NG Error
/// \~english @par Preconditions:
///       - TSKM_Init must be used and initialized
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - Parameters hAPP is NULL [TSKM_E_PAR]
///       - Parameters p_info is NULL [TSKM_E_PAR]
///       - Member type of parameters p_info is except for TSKM_DATAINIT_TYPE_USER [TSKM_E_PAR]
///       - Member onCompInit of parameters p_info is NULL [TSKM_E_PAR]
///       - The process of FrameworkunifiedAttachCallbackToDispatcher failed [TSKM_E_NG]
///       - The process of FrameworkunifiedOpenService failed [TSKM_E_STATE]
///       - The process of FrameworkunifiedSendMsg failed [TSKM_E_NG]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Method only
/// \~english @par Detail:
///       - How to report \n
///       Reports by Callback of onCompInit. \n
///       This can get message data by calling GetMsgData inside onCompInit. \n
///       - Structure of message data \n
///       TSKM_ERR_t:Processing result \n
///       TSKM_E_OK :Normal end \n
///       TSKM_E_STATE  :State error \n
///       TSKM_E_NG :Error
///       - Summary of action \n
///       Calls initialization functions which each service implemented. \n
///       When this API was called before Availability of Task_Manager becomes TRUE, this
///       does not call initialization functions and returns TSKM_E_STATE by onCompInit. \n
///       It is permitted for same application to call this API several times, at this case,
///       it is necessary to designate the pointer to same functions to onCompInit. \n
///       When this API was called by designating TSKM_DATAINIT_TYPE_USER to type, this calls
///       the initialization functions of each service by designating the following parameters. \n
///       <table>
///       <tr><th>Member</th><th>Values</th></tr>
///       <tr><td>startupReason</td><td>epswfIGN_ACC</td></tr>
///       <tr><td>isUserModeOn</td><td>TRUE</td></tr>
///       <tr><td>dataResetMode</td><td>e_SS_SM_DATA_RESET_MODE_USER</td></tr>
///       <tr><td>wakeupType</td><td>epsstWARMSTART</td></tr>
///       <tr><td>dramBackupStatus</td><td>e_SS_SM_DRAM_BACKUP_OK</td></tr>
///       <tr><td>resetStatus</td><td>e_SS_SM_RESET_STATUS_NONE</td></tr>
///       <tr><td>resetCount</td><td>0</td></tr>
///       </table>
///       Because Task_Manager calls the initialization functions of each service and implements
///       the recovery processing according to running factors at the timing of fixing running factor,
///       this designates the above parameters not to do duplicate recover processing.
/// \~english @see  TSKM_DATA_INIT_FUNC
////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t TSKM_DataInit(HANDLE hApp, const TSKM_DATAINIT_t *p_info);
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup TSKM_SetWakeupOrder
/// \~english @par Summary:
///        Holds running order.
/// \~english @param [in] p_order
///        TSKM_WAKEUP_ORDER_t *   - Running order information
/// \~english @code
///               typedef struct {
///                 char orderName[TSKM_ORDER_NAME_MAX];
///               } TSKM_WAKEUP_ORDER_t;
///                 orderName: Order name (MAX:31 characters)
///            @endcode
/// \~english @retval TSKM_E_OK Succeeded
/// \~english @retval TSKM_E_PAR %Parameter error
/// \~english @retval TSKM_E_NG Error
/// \~english @par Preconditions:
///       - TSKM_Init must be used and initialized
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - Parameters p_order is NULL [TSKM_E_PAR]
///       - The string length of the member orderName of Parameters p_order exceeds 32 [TSKM_E_PAR]
///       - The process of SetWakeupOrderToSystemManager failed [TSKM_E_NG]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync x Sync
/// \~english @par Detail:
///       This function is Wrapper function to provide SetWakeupOrderToSystemManager
///       by sessionless which SystemManger provides.
///       This designates NULL to p_order and returns TSKM_E_PAR.
///       For the detail of action, refer to SystemManager (SetWakeupOrderToSystemManager).
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t TSKM_SetWakeupOrder(const TSKM_WAKEUP_ORDER_t* p_order);
#ifdef __cplusplus
}
#endif

#endif  // TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_TSKM_H_

/** @}*/  // end of Task_Manager
/** @}*/  // end of SystemService
/** @}*/  // end of BaseSystem


