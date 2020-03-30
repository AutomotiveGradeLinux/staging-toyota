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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup system_manager
/// \file ss_system_manager_protocol.h
/// \brief    This file supports the System Manager client interface.
///////////////////////////////////////////////////////////////////////////////

#ifndef __SS_SYSTEM_MANAGER_PROTOCOL_H__  // NOLINT (build/header_guard)
#define __SS_SYSTEM_MANAGER_PROTOCOL_H__

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_service_protocol.h>

/// \addtogroup   system_manager
/// @{

#define   SS_SM_MAX_MODULE_NAME_LENGTH        (256)
#define     SS_SM_MAX_TEST_CASE_NAME_SIZE           (128)
#define     SS_SM_MAX_RESP_MSG_SIZE                 (2048)

// system manager protocol --> define all protocol messages in and out of system manager
// that are pertinent to the functionality offered by system manager.
/**
 * \~english System control protocol type.
 */
typedef enum _SS_SystemManagerProtocol {
  // System Manager - command Protocol
    SS_SYSTEM_MANAGER_PROTOCOL_BEGINNING_INDEX = 0x800,   //!< \~english
  SS_SM_NOTIFY_SYSTEM_LAUNCH_COMPLETE = SS_SYSTEM_MANAGER_PROTOCOL_BEGINNING_INDEX  //!< System Manager to Power manager
  , SS_SM_PROTOCOL_INTERFACE_END          //!< \~english

  // SystemManager-command Protocol
  , SS_SM_START                           //!< \~english Normal-boot start request.
  , SS_SM_START_COMPL_RSPN                //!< \~english Normal-boot start completion notification.
  , SS_SM_STOP                            //!< \~english Shutdown request.
  , SS_SM_STOP_COMPL_RSPN                 //!< \~english Shutdown completion notification.
  // SystemManager-command Protocol
  , SS_SM_WAKEUP_MODULES_CMPL_RSPN        //!< \~english Response to request of power state transition to power_service.(message format to use: \ref _wakeinfo "wakeInfo")
  , SS_SM_SHUTDOWN_MODULES_CMPL_RSPN      //!< SM to power

  , SS_SM_WAKEUP_MODULES                  //!< client to service
  , SS_SM_POWER_OFF_MODULES               //!< client to service
  , SS_SM_SHUTDOWN_MODULES                //!< client to service

  , SS_SM_CRNT_STATE_QUERY                //!< client to service
  , SS_SM_CRNT_STATE_QUERY_RSPN           //!< service to client
  , SS_SM_SYSTEM_MODE_INFO_REQ            //!< client to service
  , SS_SM_SYSTEM_MODE_INFO_RSPN           //!< service to client
  , SS_SM_INITCOMP_REP                    //!< client to service

  , SS_SM_PROTOCOL_OPEN_SESSION_REQ       //!< \~english
  , SS_SM_PROTOCOL_OPEN_SESSION_ACK       //!< \~english

    //*************************************************************************
    // Get Extended Parameter Protocol                                        *
    //                                                                        *
    // Protocol between applications and SM                                   *
  , SS_SM_GET_START_EXT_INFO              //!< FrameworkunifiedOnStart Extended Parameter
  , SS_SM_GET_STOP_EXT_INFO               //!< FrameworkunifiedOnStop Extended Parameter
    // End of Get Extended Parameter Protocol                                 *
    //*************************************************************************

  //*************************************************************************
  // Boot Mode Protocol:                                                    *
  //                                                                        *
  // Request by Privileged clients to set the system boot mode:             *
  //     Application, or                                                    *
  //      Programming                                                        *
  //                                                                        *
  // The Set Request message carries the requested Boot Mode.               *
  // The Set Response message carries the pass/fail of the Set request,     *
  // and if the Set request succeeded, the Boot Mode at the time the        *
  // Response message was generated.                                        *
  , SS_SM_BOOT_MODE_SET_REQ               //!< Client to Service (Sys Mgr)
  , SS_SM_BOOT_MODE_SET_RESP              //!< Service (Sys Mgr) to Client
  //                                                                        *
  // End of Boot Mode Protocol                                              *
  //*************************************************************************

  //*************************************************************************
  // Reset Mode Protocol:                                                   *
  //                                                                        *
  // Request by Privileged clients to set the data reset mode:              *
  //     None, UserData Reset or                                            *
  //     Factory Reset                                                      *
  //                                                                        *
  // The Set Request message carries the requested reset Mode.              *
  // Set Result message is not defined, because the Set Request is send by  *
  // FrameworkunifiedInvokeSync()                                                        *
  , SS_SM_DATA_RESET_MODE_SET_REQ         //!< Client to Service (Sys Mgr)
  //                                                                        *
  // End of Reset Mode Protocol                                             *
  //*************************************************************************

  //*************************************************************************
  // Program Update Status Protocol:                                        *
  //                                                                        *
  // Request by Privileged clients to set the program update status:        *
  //     None or Program Updated                                            *
  //                                                                        *
  // The Set Request message carries the requested program update status.   *
  // Set Result message is not defined, because the Set Request is send by  *
  // FrameworkunifiedInvokeSync()                                                        *
  , SS_SM_PROG_UPDATE_STATE_SET_REQ       //!< Client to Service (Sys Mgr)
  //                                                                        *
  // End of Reset Mode Protocol                                             *
  //*************************************************************************

  //*************************************************************************
  // 'CPU Reset' Protocol:                                                  *
  //                                                                        *
  // CPU Reset request from applications to SM and SM to PS                 *
  //                                     /// < Client to Service when       *
  //                     ///    message originating from    *
  //                     ///    Application to SS\SM.       *
  //                                     /// < Service to Client when       *
  //                     ///    message originating from    *
  //                     ///    SS\SM to SS\PS.             *
  , SS_SM_CPU_RESET_REQ                   //!< \~english
  //                                                                        *
  // End of CPU Reset Protocol                                              *
  //*************************************************************************

  //*************************************************************************
  // 'Local Data Reset' Protocol:                                           *
  //                                                                        *
  // Local Data Reset request from applications to SM.                      *
  //                                     /// < Client to Service when       *
  //                     ///    message originating from    *
  //                     ///    Application to SS\SM.       *
  , SS_SM_LOCAL_DATA_RESET_REQ            //!< \~english
  //                                                                        *
  // End of Local Data Reset Protocol                                        *
  //*************************************************************************

  //*************************************************************************
  // 'Remote Data Reset' Protocol:                                          *
  //                                                                        *
  // Remote Data Reset request from SM to PS                                 *
  //                                     /// < Service to Client when       *
  //                     ///    message originating from    *
  //                     ///    SS\SM to SS\PS.             *
  , SS_SM_REMOTE_DATA_RESET_REQ           //!< \~english
  //                                                                        *
  // End of Remote Data Reset Protocol                                      *
  //*************************************************************************


  //*************************************************************************
  // Startup Confirmation Protocol:                                         *
  //                                                                        *
  // Request by the Power Shadow to the Power Service to forward to the     *
  // System Manager the Startup Confirmation message.                       *
  //                                                                        *
  // The Request message carries the Startup Confirmation message.          *
  // The Response message carries the EFrameworkunifiedStatus of the request.            *
  , SS_SM_FWD_STARTUP_CONFIRMATION_MSG_REQ  //!< Pwr Svc to Sys Mgr
  , SS_SM_FWD_START_CONFIRMATION_MSG_RESP   //!< Sys Mgr to Pwr Svc
  //                                                                        *
  // End of Startup Confirmation Protocol                                   *
  //*************************************************************************

  //*************************************************************************
  // Power Request Protocol:                                                *
  //                                                                        *
  // Power Request message from the Power Shadow to the System Manager with *
  // _CWORD56_'s "Power_Request" IPC_Command data.                               *
  //                                                                        *
  // The Request message carries the _CWORD56_'s data.                           *
  // The Response message carries the EFrameworkunifiedStatus of the request.            *
  , SS_SM_POWER_REQUEST_MSG               //!< \~english Startup process start request from power_service.
  , SS_SM_POWER_REQUEST_MSG_RESP          //!< Sys Mgr to Pwr Svc
  //                                                                        *
  // End of Power Request Protocol                                          *
  //*************************************************************************

  //*************************************************************************
  // User Mode Protocol:                                                    *
  //                                                                        *
  // Request from the Power Service to the System Manager, originally from  *
  // the Power Shadow, to turn on or off the video & audio output of HMI    *
  // and Audio services.                                                    *
  // User Mode 'On'  => turn video & audio output on                        *
  // User Mode 'Off' => turn video & audio output off                       *
  //                                                                        *
  // The Request message carries the requested User Mode state.             *
  // The Response message carries the actual User Mode state.               *
  , SS_SM_USER_MODE_SET_RESP              //!< Service (Sys Mgr) to Client (Pwr Svc)
  //                                                                        *
  // Note: See the corresponding Power Shadow / Power Service               *
  // 'SS_PowerServiceProtocol' enums in                                     *
  // /SystemServices/inc/ss_power_service_protocol.h                        *
  // End of User Mode Protocol                                              *
  //*************************************************************************

  , SS_SM_DEBUG_VAR_CODE_DATA_SET_REQ     //!< \~english
  , SS_SM_SET_BOOTLOADER_INFO             //!< \~english

  , SS_SM_GET_BOOTLOADER_INFO             //!< \~english

  , SS_SM_WAKEUP_ORDER_SET_REQ            //!< \~english
  , SS_SM_NEXT_WAKEUP_TYPE_SET_REQ        //!< \~english

    //*************************************************************************
    // Error Event Protocol                                                   *
    //
    , SS_SM_EVENT_ERROR                         //!< SM  to HMI(_CWORD77_)
    , SS_SM_EVENT_ERROR_TO_SSL                  //!< SM  to SSL
    , SS_SM_ERROR_EVENT_LOGGING_START_REQ       //!< SSL to SM
    , SS_SM_ERROR_EVENT_LOGGING_START_RSPN      //!< SM  to SSL
    , SS_SM_ERROR_EVENT_ARTIFACT_REQ            //!< SSL to SM
    , SS_SM_ERROR_EVENT_ARTIFACT_RSPN           //!< SM  to SSL
    , SS_SM_ERROR_EVENT_LOGGING_COMPLETE        //!< SSL to SM
    , SS_SM_ERROR_EVENT_LOGGING_COMPLETE_RSPN   //!< SM  to SSL

    , SS_SM_ERROR_EVENT_TIMER_ID_LOGGER_START_REQ     //!< \~english
    , SS_SM_ERROR_EVENT_TIMER_ID_DEBUG_DUMP_RSPN      //!< \~english
    , SS_SM_ERROR_EVENT_TIMER_ID_BOOT_MICRO_LOG_RSPN  //!< \~english
    , SS_SM_ERROR_EVENT_TIMER_ID_CORE_FILE_POLL       //!< \~english

    , SS_SM_USER_INVOKED_LOG_REQ            //!< Key Shadow to SM

    , SS_SM_ERROR_EVENT_EEL_EXPORT_REQ      //!< SSL to SM
    , SS_SM_ERROR_EVENT_INTERFACEUNIFIED_EMMC_LOGS_REQ   //!< SSL to SM
    , SS_SM_ERROR_EVENT_INTERFACEUNIFIED_EMMC_LOGS_RSPN  //!< SM  to SVC
    , SS_SM_ERROR_EVENT_CLR_LOGS_REQ        //!< SVC to SM
    , SS_SM_ERROR_EVENT_PROCESS_EXIT        //!< SM\CD to SM
    , SS_SM_ERROR_EVENT_DIAG_LOG_REQ        //!< SSL to SM
    , SS_SM_ERROR_EVENT_CAN_LOG_REQ         //!< SSL to SM
    , SS_SM_ERROR_EVENT_DTC_LOG_REQ         //!< SSL to SM
    //
    // End of Error Event Protocol                                            *
    //*************************************************************************

  , SS_SM_GROUP_LAUNCH_TRIGGER            //!< \~english

    //*************************************************************************
    // Debug Dump Protocol                                                    *
    //                                                                        *
    // Protocol between applications and SM                                   *
    , SS_SM_DEBUG_DUMP                    //!< SM to client
    , SS_SM_DEBUG_DUMP_RSPN               //!< client to SM
    //                                                                        *
    // End of Debug Dump Protocol                                             *
    //*************************************************************************

    //*************************************************************************
    // IAT Propagate Error (Module Relaunch Request)                          *
    //                                                                        *
    // Protocol between applications and SM                                   *
    , SS_SM_MODULE_RELAUNCH_REQ           //!< client to SM
    //                                                                        *
    // End of IAT Propagate Error (Module Relaunch Request)                   *
    //*************************************************************************

    //*************************************************************************
    // CPU High Load                                                          *
    //                                                                        *
    // Protocol between SS_CPU_Monitor and SM                                 *
    , SS_SM_CPU_HIGH_LOAD_DETECTED        //!< client to SM
    //                                                                        *
    // End of CPU High Load                                                   *
    //*************************************************************************

    //*************************************************************************
    // IAT Propagate Error (System Error)                                     *
    //                                                                        *
    // Protocol between application and SM                                    *
    , SS_SM_PROPAGATE_SYSTEM_ERROR        //!< client to SM
    //                                                                        *
    // End of IAT Propagate Error (System Error)                              *
    //*************************************************************************

    //*************************************************************************
    // _CWORD56_ HeartBeat Request Protocol                                        *
    //                                                                        *
    // Protocol between Power Services and SM                                 *
    , SS_SM__CWORD56__HEARTBEAT_REQ            //!< PS to SM
    , SS_SM__CWORD56__HEARTBEAT_RSPN           //!< SM to PS
    //                                                                        *
    // End of _CWORD56_ HeartBeat Request Protocol                                 *
    //*************************************************************************

    //*************************************************************************
    // User Data Reset Event Protocol                                         *
    //                                                                        *
    // Protocol between SM and Service                                        *
    , SS_SM_EVENT_USER_DATA_RESET         //!< SM to MS
    //                                                                        *
    // End of User Data Reset Event Protocol                                  *
    //*************************************************************************

    //*************************************************************************
    // Boot Micro Logging Protocol                                            *
    //                                                                        *
    // Protocol between SS_Logger and SM.                                     *
    , SS_SM_BOOT_MICRO_RESET_NTF          //!< Logging Shadow to SM
    , SS_SM_BOOT_MICRO_LOG_REQ            //!< SM to Logging Shadow
    , SS_SM_BOOT_MICRO_LOG_RSP            //!< Logging Shadow to SM  aka ENDING_INDEX
    //                                                                        *
    // End Boot Micro Logging Protocol                                        *
    //*************************************************************************

  // SystemManager-command Protocol
  , SS_SM_PRE_START                    //!< \~english Pre-boot(+BA) start request.
  , SS_SM_PRE_START_COMPL_RSPN         //!< \~english Pre-boot(+BA) start completion notification.
  , SS_SM_PRE_STOP                     //!< \~english Pre-boot(+BA) stop request.
  , SS_SM_PRE_STOP_COMPL_RSPN          //!< \~english Pre-boot(+BA) stop completion notification.
  , SS_SM_BACKGROUND_START             //!< \~english Background-boot(+BA) start request.
  , SS_SM_BACKGROUND_START_COMPL_RSPN  //!< \~english Background-boot(+BA) start completion notification.
  , SS_SM_BACKGROUND_STOP              //!< \~english Background-boot(+BA) stop request.
  , SS_SM_BACKGROUND_STOP_COMPL_RSPN   //!< \~english Background-boot(+BA) stop completion notification.

    // UPDATE IF ANOTHER 'LAST' ENUMERATION IS ADDED !
    , SS_SYSTEM_MANAGER_PROTOCOL_ENDING_INDEX = SS_SM_BACKGROUND_STOP_COMPL_RSPN  //!< \~english
} SS_SystemManagerProtocol;

typedef struct _SS_SMCurrentState {
  CHAR  testCaseIdString[SS_SM_MAX_TEST_CASE_NAME_SIZE];
  CHAR  respMsgString[SS_SM_MAX_RESP_MSG_SIZE];
}SS_SMCurrentState;

typedef struct _SMCompleteAck {
  UI_16 unSessionId;
  CHAR  szServiceName[MAX_QUEUE_NAME_SIZE];
} SMStopCompleteAck;

typedef struct _SMGroupLaunchTrigger {
    UI_32 NextGroupId;
}SMGroupLaunchTrigger;
//*************************************************************************


/// @}

#endif /* __SS_SYSTEM_MANAGER_PROTOCOL_H__ */  // NOLINT (build/header_guard)
// EOF
