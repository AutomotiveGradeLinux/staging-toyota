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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SystemManagerIf
/// \brief    This file provides support for the System Manager client interface.
///
///////////////////////////////////////////////////////////////////////////////
#include "system_service/ss_sm_client_if.h"
#include <sys/mount.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/frameworkunified_dispatcher.h>
#include <native_service/frameworkunified_sm_framework_dispatch.h>
#include <native_service/frameworkunified_sm_hsmframework.h>

#include <string>

#include "system_service/ss_system_manager_if.h"
#include "system_service/ss_system_manager_protocol.h"
#include "system_service/ss_system_manager_notifications.h"
#include "system_service/ss_system_if.h"
#include "system_service/ss_services.h"
#include "system_service/ss_power_service_protocol.h"
#include "system_service/ss_templates.h"
#include "system_service/ss_string_maps.h"
#include "system_service/ss_error_message.h"
#include "system_service/ss_sm_client_if_local.h"

#include "system_service/ss_boot_map.h"
#include "ss_system_if_interfaceunifiedlog.h"

typedef struct {
  BOOL fAvailable;
  HANDLE hService;
  std::string szServiceName;
  std::string strIAm;
} TSystemManagerSession;  // LCOV_EXCL_BR_LINE 11:Unexpected branch

typedef enum {
  MODULE_STATE_INVALID = 0,   /// Initial state of all modules
  MODULE_STATE_CONNECTING,    /// Attempting to established a session connection
  MODULE_STATE_CONNECTED,     /// A session connection has been established
  MODULE_STATE_STARTING,      /// Received a START from System Manager,
  ///     called FrameworkunifiedOnStart or enqueued evStart
  MODULE_STATE_START_FAILED,  /// Application returned error from FrameworkunifiedOnStart
  MODULE_STATE_STARTED,       /// Sent Start Response to System Manager
  MODULE_STATE_STOPPING,      /// Received a Stop Request from System Manager,
  ///     called FrameworkunifiedOnStop or enqueued evStop
  MODULE_STATE_STOP_FAILED,   /// Application returned error from FrameworkunifiedOnStop
  MODULE_STATE_STOPPED,       /// Sent Start Response to System Manager
  MODULE_STATE_DEBUG_DUMPING,
  MODULE_STATE_STARTED_PRE,               //!< Sent Pre-Start Response to System Manager
  MODULE_STATE_START_PRE_FAILED,          //!< Application returned error from FrameworkunifiedOnPreStart
  MODULE_STATE_STOPPED_PRE,               //!< Sent Pre-Stop Response to System Manager
  MODULE_STATE_STOP_PRE_FAILED,           //!< Application returned error from FrameworkunifiedOnPreStop
  MODULE_STATE_STARTED_BACKGROUND,        //!< Sent Background-Start Response to System Manager
  MODULE_STATE_START_BACKGROUND_FAILED,   //!< Application returned error from FrameworkunifiedOnBackgroundStart
  MODULE_STATE_STOPPED_BACKGROUND,        //!< Sent Background-Stop Response to System Manager
  MODULE_STATE_STOP_BACKGROUND_FAILED     //!< Application returned error from FrameworkunifiedOnBackgroundStop
} ModuleStateType;

static SMStopCompleteAck g_responseCompleteAck;

static EFrameworkunifiedStatus OnSystemManagerStart(HANDLE hApp);
static EFrameworkunifiedStatus OnSystemManagerStop(HANDLE hApp);
static EFrameworkunifiedStatus OnSystemManagerPreStart(HANDLE hApp);
static EFrameworkunifiedStatus OnSystemManagerPreStop(HANDLE hApp);
static EFrameworkunifiedStatus OnSystemManagerBackgroundStart(HANDLE hApp);
static EFrameworkunifiedStatus OnSystemManagerBackgroundStop(HANDLE hApp);
static EFrameworkunifiedStatus OnSystemManagerOpenSessionAck(HANDLE hApp);
static EFrameworkunifiedStatus DebugDumpAppCbStatistics(HANDLE hApp);
static EFrameworkunifiedStatus DebugDumpMemoryMap(HANDLE hApp);
static EFrameworkunifiedStatus SystemManagerOpenSender(HANDLE hApp);

static TSystemManagerSession g_tSystemManagerSession = { };  // LCOV_EXCL_BR_LINE 11:Unexpected branch
static pthread_mutex_t gMutexObj = PTHREAD_MUTEX_INITIALIZER;

static EFrameworkunifiedStatus SendMsgToSystemManager(UI_32 uiCmd, UI_32 uiLength,
    PCVOID pData);
static EFrameworkunifiedStatus SendMsgToSystemManager(UI_32 uiCmd, PCSTR f_pCmdName,
    UI_32 uiLength, PCVOID pData);

static EFrameworkunifiedStatus InvokeSyncRequestToSystemManager(UI_32 uiCmd,
    PCSTR f_pCmdName,
    UI_32 uiLength, PCVOID pData,
    UI_32 uiResLength,
    PVOID pResData);

static EFrameworkunifiedStatus CommonSystemManagerStartStopCallbackProcessing(
  HANDLE hApp, UI_32 f_InterfaceunifiedEvID, SS_String f_pInterfaceunifiedEvName, CbFuncPtr f_InterfaceunifiedFncCb,
  PCSTR f_pInterfaceunifiedFncName, ModuleStateType f_moduleSuccessState,
  ModuleStateType f_moduleFailureState, BOOL f_bUseNotificationVsFncFlag,
  PCSTR f_pUseNotificationText, SS_SystemManagerProtocol f_ProtocolId,
  PCSTR f_pProtocolName);

static EFrameworkunifiedStatus SendInterfaceunifiedOnResponseToSystemManager(
  SS_SystemManagerProtocol f_ProtocolID, PCSTR f_pProtocolName,
  PCSTR f_InterfaceunifiedOnFncName, ModuleStateType f_moduleSuccessState,
  ModuleStateType f_moduleFailureState, EFrameworkunifiedStatus f_eStatus);

// Pointer of function which is called when SM OpenSession Ack is received.
// client can register this function pointer using RegisterSMSessionAckCallback() API
static CbFuncPtr CallbackFnPtr = NULL;

static BOOL UseStopCompleteNotificationVsInterfaceunifiedOnStopFnc = FALSE;

static VOID LoadCbHandlerCmdIDsIntoDetachCbIDsArray(
  const FrameworkunifiedProtocolCallbackHandler* pMsgHandler, UI_32 * puiCmdIdArray,
  UI_32 uiHandlerCount);

static VOID SetModuleState(ModuleStateType f_moduleState, BOOL f_bLog = TRUE);

static ModuleStateType g_moduleState = MODULE_STATE_INVALID;
extern HANDLE g_SystemIf_hApp;

//******************************************************************************
void Init_SS_IF_ModuleState_StrMap(
  std::map<ModuleStateType, SS_String> & m_strMap) {  // NOLINT (runtime/references)
// LCOV_EXCL_BR_START 11:Unexpected branch
  MAP_ENTRY(m_strMap, MODULE_STATE_INVALID);
  MAP_ENTRY(m_strMap, MODULE_STATE_CONNECTING);
  MAP_ENTRY(m_strMap, MODULE_STATE_CONNECTED);
  MAP_ENTRY(m_strMap, MODULE_STATE_STARTING);
  MAP_ENTRY(m_strMap, MODULE_STATE_START_FAILED);
  MAP_ENTRY(m_strMap, MODULE_STATE_STARTED);
  MAP_ENTRY(m_strMap, MODULE_STATE_STOPPING);
  MAP_ENTRY(m_strMap, MODULE_STATE_STOP_FAILED);
  MAP_ENTRY(m_strMap, MODULE_STATE_STOPPED);
  MAP_ENTRY(m_strMap, MODULE_STATE_STARTED_PRE);
  MAP_ENTRY(m_strMap, MODULE_STATE_START_PRE_FAILED);
  MAP_ENTRY(m_strMap, MODULE_STATE_STOPPED_PRE);
  MAP_ENTRY(m_strMap, MODULE_STATE_STOP_PRE_FAILED);
  MAP_ENTRY(m_strMap, MODULE_STATE_STARTED_BACKGROUND);
  MAP_ENTRY(m_strMap, MODULE_STATE_START_BACKGROUND_FAILED);
  MAP_ENTRY(m_strMap, MODULE_STATE_STOPPED_BACKGROUND);
  MAP_ENTRY(m_strMap, MODULE_STATE_STOP_BACKGROUND_FAILED);
  MAP_ENTRY(m_strMap, MODULE_STATE_DEBUG_DUMPING);
// LCOV_EXCL_BR_STOP
}  // End of void Init_SS_IF_ModuleState_StrMap(std::map<ModuleStateType, SS_String> & m_strMap)

class EnumStringMap<ModuleStateType, Init_SS_IF_ModuleState_StrMap> g_oSS_IF_ModuleStateStrMap;

// LCOV_EXCL_START 6:Because the condition cannot be set
SS_String GetStr(ModuleStateType f_enum) {
  return g_oSS_IF_ModuleStateStrMap.GetStr(f_enum);
}

HANDLE GetSystemManagerSessionHandle(void) {
  return g_tSystemManagerSession.hService;
}  // End of HANDLE GetSystemManagerSessionHandle(void)
// LCOV_EXCL_STOP

EFrameworkunifiedStatus InterfaceunifiedSystemConnectToSystemManagerService(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CallbackFnPtr = NULL;

  /// Avoid opening a session to the SystemManager
  /// service Since it uses the framework as well
  if (0 != strcmp(SERVICE_SYSMANAGER, FrameworkunifiedGetAppName(hApp))) {
    // LCOV_EXCL_BR_LINE 6:Duplicate check(AppName has been checked by the caller)
    memset(&g_responseCompleteAck, 0, sizeof(g_responseCompleteAck));
    g_responseCompleteAck.unSessionId = 0;
    strcpy(g_responseCompleteAck.szServiceName, SERVICE_SYSMANAGER);  // NOLINT (runtime/printf)

    pthread_mutex_lock(&gMutexObj);

    /// Need to save the Group and Subgroup values
    g_tSystemManagerSession.szServiceName = SERVICE_SYSMANAGER;
    g_tSystemManagerSession.strIAm = FrameworkunifiedGetAppName(hApp);

    /// Cleanup any other data members
    g_tSystemManagerSession.fAvailable = FALSE;
    g_tSystemManagerSession.hService = NULL;

    pthread_mutex_unlock(&gMutexObj);

    l_eStatus = SystemManagerOpenSender(hApp);

    LOG_STATUS_IF_ERRORED(l_eStatus, "SystemManagerOpenSender()");
  } else {
    // LCOV_EXCL_START 6:Duplicate check(AppName has been checked by the caller)
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " I am SystemManager ! Not registering for '%s' notification",
           NTFY_SSSystemMgrAvailability);
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////
/// Function: SystemManagerOpenSender
///
///////////////////////////////////////////////////////////

EFrameworkunifiedStatus SystemManagerOpenSender(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  pthread_mutex_lock(&gMutexObj);

  // LCOV_EXCL_START 6:Duplicate check(hService is NULL only)
  if (g_tSystemManagerSession.hService != NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, " Warn: hService != NULL");
    pthread_mutex_unlock(&gMutexObj);
    return eFrameworkunifiedStatusFail;
  }
  // LCOV_EXCL_STOP

  // opening the service
  if (NULL
      == (g_tSystemManagerSession.hService = FrameworkunifiedMcOpenSender(hApp,
          SERVICE_SYSMANAGER))) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("FrameworkunifiedMcOpenSender(SERVICE_SYSMANAGER)");
  } else {
    FrameworkunifiedProtocolCallbackHandler l_SystemManager_Protocol_Cbs[] = {
      // Command ID,              Call back functions
      { SS_SM_PROTOCOL_OPEN_SESSION_ACK, OnSystemManagerOpenSessionAck },
      { SS_SM_START, OnSystemManagerStart },
      { SS_SM_STOP, OnSystemManagerStop },
      { SS_SM_PRE_START, OnSystemManagerPreStart },
      { SS_SM_PRE_STOP, OnSystemManagerPreStop },
      { SS_SM_BACKGROUND_START, OnSystemManagerBackgroundStart },
      { SS_SM_BACKGROUND_STOP, OnSystemManagerBackgroundStop },
      { SS_SM_DEBUG_DUMP, OnSystemManagerDebugDump },
    };  // LCOV_EXCL_BR_LINE 11:Unexpected branch

    /// Attach the valid callback for this service
    if (eFrameworkunifiedStatusOK !=  // LCOV_EXCL_BR_LINE 11:Unexpected branch
        (l_eStatus = FrameworkunifiedAttachCallbacksToDispatcher(
                       hApp, SERVICE_SYSMANAGER, l_SystemManager_Protocol_Cbs,
                       _countof(l_SystemManager_Protocol_Cbs)))) {
      LOG_ERROR("FrameworkunifiedAttachCallbacksToDispatcher()");
    } else if (eFrameworkunifiedStatusOK !=  // LCOV_EXCL_BR_LINE 11:Unexpected branch
               (l_eStatus = FrameworkunifiedSendMsg(g_tSystemManagerSession.hService,
                                       SS_SM_PROTOCOL_OPEN_SESSION_REQ, 0, NULL))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: FrameworkunifiedSendMsg(%s) errored: 0x%X/%s",
             g_tSystemManagerSession.szServiceName.c_str(), l_eStatus,
             GetStr(l_eStatus).c_str());
    }  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  }
  pthread_mutex_unlock(&gMutexObj);

  if (eFrameworkunifiedStatusOK == l_eStatus) {
    g_moduleState = MODULE_STATE_CONNECTING;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;  // LCOV_EXCL_BR_LINE 11:Unexpected branch
}

// LCOV_EXCL_START 6:Because the condition cannot be set
///////////////////////////////////////////////////////////
/// Function: OnSystemManagerOpenSessionAck
/// Service Manager OpenSession callback
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus OnSystemManagerOpenSessionAck(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL == hApp) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("NULL == hApp");
  } else {
    pthread_mutex_lock(&gMutexObj);
    if (NULL == g_tSystemManagerSession.hService) {
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      LOG_ERROR("FrameworkunifiedGetOpenSessionHandle()");
      g_tSystemManagerSession.fAvailable = FALSE;
    } else {
      LOG_SUCCESS("FrameworkunifiedGetOpenSessionHandle(hApp)");
      g_tSystemManagerSession.fAvailable = TRUE;
      g_moduleState = MODULE_STATE_CONNECTED;
    }
    pthread_mutex_unlock(&gMutexObj);

    if ((eFrameworkunifiedStatusOK == l_eStatus) && (CallbackFnPtr != NULL)) {
      // Call the RegisterSMSessionAckCallback()-registered function
      if (eFrameworkunifiedStatusOK != (l_eStatus = (CallbackFnPtr)(hApp))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error: The 'CallbackFnPtr' function as set by "
               "the RegisterSMSessionAckCallback() function "
               "errored: 0x%x/%s",
               l_eStatus, GetStr(l_eStatus).c_str());
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // EFrameworkunifiedStatus OnSystemManagerOpenSessionAck( HANDLE hApp )
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////
/// Macro:
///
///////////////////////////////////////////////////////////
#define SETUP_AND_CALL_COMMON_SYSTEM_MANAGER_START_STOP_PROCESSING(      \
        hApp, evInterfaceunifiedId, InterfaceunifiedFnc, ModStateSuccess, ModStateFailure, \
                NotifFlag, NotifText, ProtocolId) \
  l_eStatus = CommonSystemManagerStartStopCallbackProcessing(         \
                          hApp,                \
                          evInterfaceunifiedId,             \
                          #evInterfaceunifiedId,            \
                          InterfaceunifiedFnc,              \
                          #InterfaceunifiedFnc "()",        \
                          ModStateSuccess,     \
                          ModStateFailure,     \
                          NotifFlag,           \
                          NotifText,           \
                          ProtocolId,          \
                                                    #ProtocolId);       \
  LOG_STATUS(l_eStatus,                                               \
                "CommonSystemManagerStartStopCallbackProcessing(" #InterfaceunifiedFnc "())");

// LCOV_EXCL_START 6:Because the condition cannot be set
///////////////////////////////////////////////////////////
/// Function: OnSystemManagerStart
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus OnSystemManagerStart(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;
  FrameworkunifiedDefaultCallbackHandler cbFuncs;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_eStatus = FrameworkunifiedGetDefaultCbHandler(&cbFuncs);
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedGetDefaultCbHandler()");

  // Call CommonSystemManagerStartStopCallbackProcessing(), & log status
  SETUP_AND_CALL_COMMON_SYSTEM_MANAGER_START_STOP_PROCESSING(
    hApp, FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedStart), cbFuncs.onStart, MODULE_STATE_STARTED,
    MODULE_STATE_START_FAILED, FALSE, "", SS_SM_START_COMPL_RSPN);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus OnSystemManagerStart( HANDLE hApp )

///////////////////////////////////////////////////////////
/// Function: OnSystemManagerStop
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus OnSystemManagerStop(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;
  FrameworkunifiedDefaultCallbackHandler cbFuncs;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_eStatus = FrameworkunifiedGetDefaultCbHandler(&cbFuncs);
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedGetDefaultCbHandler()");

  // Call CommonSystemManagerStartStopCallbackProcessing(), & log status
  SETUP_AND_CALL_COMMON_SYSTEM_MANAGER_START_STOP_PROCESSING(
    hApp, FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedStop), cbFuncs.onStop, MODULE_STATE_STOPPED,
    MODULE_STATE_STOP_FAILED, UseStopCompleteNotificationVsInterfaceunifiedOnStopFnc,
    " 'UseStopCompleteNotificationVsInterfaceunifiedOnStopFnc' is 'True': "
    "setting 'l_eStatus' to 'eFrameworkunifiedStatusFail'",
    SS_SM_STOP_COMPL_RSPN);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus OnSystemManagerStop( HANDLE hApp )

///////////////////////////////////////////////////////////
/// Function: OnSystemManagerPreStart
///
///////////////////////////////////////////////////////////
static EFrameworkunifiedStatus OnSystemManagerPreStart(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;
  FrameworkunifiedDefaultCallbackHandler cbFuncs;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov restrictions(others)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_eStatus = FrameworkunifiedGetDefaultCbHandler(&cbFuncs);  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov restrictions(others)
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedGetDefaultCbHandler()");

  // Call CommonSystemManagerStartStopCallbackProcessing(), & log status
  SETUP_AND_CALL_COMMON_SYSTEM_MANAGER_START_STOP_PROCESSING(
      hApp,
      FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedPreStart),
      cbFuncs.onPreStart,
      MODULE_STATE_STARTED_PRE,
      MODULE_STATE_START_PRE_FAILED,
      FALSE,
      "",
      SS_SM_PRE_START_COMPL_RSPN);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:Excluded for function that are expanded inline
  return l_eStatus;
}  // End of EFrameworkunifiedStatus OnSystemManagerPreStart( HANDLE hApp )

///////////////////////////////////////////////////////////
/// Function: OnSystemManagerPreStop
///
///////////////////////////////////////////////////////////
static EFrameworkunifiedStatus OnSystemManagerPreStop(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;
  FrameworkunifiedDefaultCallbackHandler cbFuncs;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov restrictions(others)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_eStatus = FrameworkunifiedGetDefaultCbHandler(&cbFuncs);  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov restrictions(others)
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedGetDefaultCbHandler()");

  // Call CommonSystemManagerStartStopCallbackProcessing(), & log status
  SETUP_AND_CALL_COMMON_SYSTEM_MANAGER_START_STOP_PROCESSING(
      hApp,
      FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedPreStop),
      cbFuncs.onPreStop,
      MODULE_STATE_STOPPED_PRE,
      MODULE_STATE_STOP_PRE_FAILED,
      FALSE,
      "",
      SS_SM_PRE_STOP_COMPL_RSPN);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:Excluded for function that are expanded inline
  return l_eStatus;
}  // End of EFrameworkunifiedStatus OnSystemManagerPreStop( HANDLE hApp )

///////////////////////////////////////////////////////////
/// Function: OnSystemManagerBackgroundStart
///
///////////////////////////////////////////////////////////
static EFrameworkunifiedStatus OnSystemManagerBackgroundStart(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;
  FrameworkunifiedDefaultCallbackHandler cbFuncs;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov restrictions(others)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_eStatus = FrameworkunifiedGetDefaultCbHandler(&cbFuncs);  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov restrictions(others)
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedGetDefaultCbHandler()");

  // Call CommonSystemManagerStartStopCallbackProcessing(), & log status
  SETUP_AND_CALL_COMMON_SYSTEM_MANAGER_START_STOP_PROCESSING(
      hApp,
      FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedBackgroundStart),
      cbFuncs.onBackgroundStart,
      MODULE_STATE_STARTED_BACKGROUND,
      MODULE_STATE_START_BACKGROUND_FAILED,
      FALSE,
      "",
      SS_SM_BACKGROUND_START_COMPL_RSPN);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:Excluded for function that are expanded inline
  return l_eStatus;
}  // End of EFrameworkunifiedStatus OnSystemManagerBackgroundStart( HANDLE hApp )

///////////////////////////////////////////////////////////
/// Function: OnSystemManagerBackgroundStop
///
///////////////////////////////////////////////////////////
static EFrameworkunifiedStatus OnSystemManagerBackgroundStop(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus;
  FrameworkunifiedDefaultCallbackHandler cbFuncs;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov restrictions(others)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_eStatus = FrameworkunifiedGetDefaultCbHandler(&cbFuncs);  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov restrictions(others)
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedGetDefaultCbHandler()");

  // Call CommonSystemManagerStartStopCallbackProcessing(), & log status
  SETUP_AND_CALL_COMMON_SYSTEM_MANAGER_START_STOP_PROCESSING(
      hApp,
      FRAMEWORKUNIFIED_EVENT(evFrameworkunifiedBackgroundStop),
      cbFuncs.onBackgroundStop,
      MODULE_STATE_STOPPED_BACKGROUND,
      MODULE_STATE_STOP_BACKGROUND_FAILED,
      FALSE,
      "",
      SS_SM_BACKGROUND_STOP_COMPL_RSPN);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:Excluded for function that are expanded inline
  return l_eStatus;
}  // End of EFrameworkunifiedStatus OnSystemManagerBackgroundStop( HANDLE hApp )

///////////////////////////////////////////////////////////
/// Function: CommonSystemManagerStartStopCallbackProcessing
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus CommonSystemManagerStartStopCallbackProcessing(
  HANDLE hApp, UI_32 f_InterfaceunifiedEvID, SS_String f_pInterfaceunifiedEvName, CbFuncPtr f_InterfaceunifiedFncCb,
  PCSTR f_pInterfaceunifiedFncName, ModuleStateType f_moduleSuccessState,
  ModuleStateType f_moduleFailureState, BOOL f_bUseNotificationVsFncFlag,
  PCSTR f_pUseNotificationText, SS_SystemManagerProtocol f_ProtocolId,
  PCSTR f_pProtocolName) {
  EFrameworkunifiedStatus l_eStatus;
  EFrameworkunifiedStatus l_StatusFncReturned;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  SS_String l_pInterfaceunifiedFncName;

  if (f_moduleSuccessState == g_moduleState) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
           " Warning: Module state already is '%s', just "
           "returning '%s' to System Manager",
           GetStr(g_moduleState).c_str(), f_pProtocolName);
    l_eStatus = eFrameworkunifiedStatusOK;
    l_StatusFncReturned = l_eStatus;
  } else {
    // If client application is a state machine, post an Event instead of calling
    // InterfaceunifiedOnXYZ()
    if (FrameworkunifiedIsStateMachineApp(hApp)) {
      l_pInterfaceunifiedFncName = "FrameworkunifiedPostEvent(FRAMEWORKUNIFIED_EVENT(" + f_pInterfaceunifiedEvName + " ))";
      CFrameworkunifiedHSMFramework* l_pStateMachine = FrameworkunifiedGetStateMachine(hApp);
      if (NULL == l_pStateMachine) {
        l_eStatus = eFrameworkunifiedStatusNullPointer;
        LOG_ERROR("FrameworkunifiedGetStateMachine()");
        l_StatusFncReturned = l_eStatus;
      } else {
        l_eStatus = l_pStateMachine->FrameworkunifiedPostEvent(f_InterfaceunifiedEvID);
        if (eFrameworkunifiedStatusOK != l_eStatus) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 " Error: l_pStateMachine->FrameworkunifiedPostEvent(%s) "
                 "errored: %d/'%s'",
                 f_pInterfaceunifiedEvName.c_str(), l_eStatus, GetStr(l_eStatus).c_str());
          l_StatusFncReturned = l_eStatus;
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                 " f_bUseNotificationVsFncFlag is '%s'",
                 GetStr(f_bUseNotificationVsFncFlag).c_str());
          if (f_bUseNotificationVsFncFlag) {
            //
            // DON'T send an 'Ok' back to the SendInterfaceunifiedOnResponse function: if
            // it detects a 'Ok' it will send a SS_SM_<protocol>_RSPN back
            // to System Manager, telling System Manager that the request
            // has been completely serviced.
            l_StatusFncReturned = eFrameworkunifiedStatusFail;
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s", f_pUseNotificationText);
          } else {
            //
            // Always send an 'Ok' back for the evInterfaceunified<protocol> version
            // of InterfaceunifiedOn<protocol>; the l_eStatus applies to the
            // FrameworkunifiedPostEvent() function, not the actual <protocol> event
            // handler.
            l_StatusFncReturned = eFrameworkunifiedStatusOK;
          }
        }  // End else successful FrameworkunifiedPostEvent()
      }  // End else valid FrameworkunifiedGetStateMachine()
    } else {  // End if FrameworkunifiedIsStateMachineApp() is TRUE
      l_pInterfaceunifiedFncName = f_pInterfaceunifiedFncName;
      if (f_InterfaceunifiedFncCb) {
        l_eStatus = (f_InterfaceunifiedFncCb)(hApp);
        LOG_STATUS(l_eStatus, f_pInterfaceunifiedFncName);
        l_StatusFncReturned = l_eStatus;
      } else {
        SS_ASERT(0);
        l_StatusFncReturned = eFrameworkunifiedStatusOK;
      }
    }
  }

  l_eStatus = SendInterfaceunifiedOnResponseToSystemManager(f_ProtocolId, f_pProtocolName,
              l_pInterfaceunifiedFncName.c_str(),
              f_moduleSuccessState,
              f_moduleFailureState,
              l_StatusFncReturned);
  const char l_cFormat[] = "SendInterfaceunifiedOnResponseToSystemManager(%s)";
  char l_cBuf[sizeof(l_cFormat) + strlen(f_pProtocolName) + 1];  // NOLINT (runtime/arrays)
  snprintf(l_cBuf, sizeof(l_cBuf), l_cFormat, f_pProtocolName);
  LOG_STATUS_IF_ERRORED(l_eStatus, l_cBuf);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of CommonSystemManagerStartStopCallbackProcessing(


///////////////////////////////////////////////////////////
/// Function: SendInterfaceunifiedOnStopResponseToSystemManager()
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendInterfaceunifiedOnStopResponseToSystemManager(EFrameworkunifiedStatus f_eStatus) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = SendInterfaceunifiedOnResponseToSystemManager(
                           SS_SM_STOP_COMPL_RSPN, "SS_SM_STOP_COMPL_RSPN", "FrameworkunifiedOnStop()",
                           MODULE_STATE_STOPPED, MODULE_STATE_STOP_FAILED, f_eStatus);
  LOG_STATUS_IF_ERRORED(l_eStatus, "SendInterfaceunifiedOnResponseToSystemManager("
                        "SS_SM_STOP_COMPL_RSPN");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus SendInterfaceunifiedOnStopResponseToSystemManager(EFrameworkunifiedStatus f_eStatus)

///////////////////////////////////////////////////////////
/// Function: GetInterfaceunifiedOnStartExtInfo()
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus GetInterfaceunifiedOnStartExtInfo(T_SS_SM_START_ExtDataStructType &f_info) {  // NOLINT (runtime/references)
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_eStatus = InvokeSyncRequestToSystemManager(
                SS_SM_GET_START_EXT_INFO, "SS_SM_GET_START_EXT_INFO", 0, NULL,
                sizeof(T_SS_SM_START_ExtDataStructType), (PVOID) & f_info);
  LOG_STATUS_IF_ERRORED(l_eStatus, "InvokeSyncRequestToSystemManager()");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////
/// Function: GetInterfaceunifiedOnStopExtInfo()
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus GetInterfaceunifiedOnStopExtInfo(T_SS_SM_STOP_ExtDataStructType &f_info) {  // NOLINT (runtime/references)
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_eStatus = InvokeSyncRequestToSystemManager(
                SS_SM_GET_STOP_EXT_INFO, "SS_SM_GET_STOP_EXT_INFO", 0, NULL,
                sizeof(T_SS_SM_STOP_ExtDataStructType), (PVOID) & f_info);
  LOG_STATUS_IF_ERRORED(l_eStatus, "InvokeSyncRequestToSystemManager()");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

// LCOV_EXCL_START 6:Because the condition cannot be set
///////////////////////////////////////////////////////////
/// Function: OnSystemManagerDebugDump
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus OnSystemManagerDebugDump(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  FrameworkunifiedDefaultCallbackHandler cbFuncs;

  g_moduleState = MODULE_STATE_DEBUG_DUMPING;
  l_eStatus = FrameworkunifiedGetDefaultCbHandler(&cbFuncs);
  LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedGetDefaultCbHandler()");

  if (cbFuncs.onDebugDump) {
    l_eStatus = cbFuncs.onDebugDump(hApp);
    LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedOnDebugDump()");
  } else {
    SS_ASERT(0);
  }

  l_eStatus = DebugDumpAppCbStatistics(hApp);
  LOG_STATUS_IF_ERRORED(l_eStatus, "DebugDumpAppCbStatistics()");

  l_eStatus = DebugDumpMemoryMap(hApp);
  LOG_STATUS_IF_ERRORED(l_eStatus, "DebugDumpMemoryMap()");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus OnSystemManagerDebugDump( HANDLE hApp )

///////////////////////////////////////////////////////////
/// Function: DebugDumpAppCbStatistics
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus DebugDumpAppCbStatistics(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
l_eStatus = eFrameworkunifiedStatusOK;

FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
return l_eStatus;
}

///////////////////////////////////////////////////////////
/// Function: DebugDumpMemoryMap
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus DebugDumpMemoryMap(HANDLE hApp) {
#define MEM_MAP_READ_SIZE 4096
#define DUMP_MAX_SIZE   131072  // 128KB

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  char buf[MEM_MAP_READ_SIZE];
  std::string bufString;
  int fd, ret;

  fd = open("/proc/self/maps", O_RDONLY);
  if (fd != -1) {
    while ((ret = read(fd, buf, MEM_MAP_READ_SIZE)) > 0) {
      if ((bufString.size() + ret) > DUMP_MAX_SIZE) {
        // If read it again, it will break because it overflows
        break;
      }
      bufString.append(buf, ret);
    }
    close(fd);
    if (ret >= 0) {
      l_eStatus = eFrameworkunifiedStatusOK;
      SendDebugDumpResponseToSystemManager(bufString);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
// SendDebugDumpResponseToSystemManager
//////////////////////////////////////////
VOID SendDebugDumpResponseToSystemManager(BOOL f_bFormatStrRequired,
    PCSTR f_cFormat, ...) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  va_list l_argList;
  const UI_32 l_maxDebugDumpMsgSize = (1024 * 4);
  CHAR l_cMsg[l_maxDebugDumpMsgSize] = { 0 };
  PCSTR l_pAppName;

  pthread_mutex_lock(&gMutexObj);

  l_pAppName = FrameworkunifiedGetAppName(g_SystemIf_hApp);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  if (f_bFormatStrRequired) {
    // build a format string
    // Format: AppName/debugdump_data
    snprintf(l_cMsg, l_maxDebugDumpMsgSize, "%s/", l_pAppName);
  }

  va_start(l_argList, f_cFormat);
  vsnprintf(&l_cMsg[strlen(l_cMsg)], l_maxDebugDumpMsgSize - strlen(l_cMsg),
            f_cFormat, l_argList);
  va_end(l_argList);

  if (0 == strcmp(SERVICE_SYSMANAGER, l_pAppName)) {
    l_eStatus = FrameworkunifiedSendSelf(g_SystemIf_hApp, SS_SM_DEBUG_DUMP_RSPN,
                            strlen(l_cMsg) + 1, l_cMsg);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSendSelf(SS_SM_DEBUG_DUMP_RSPN)");
  } else if (NULL != g_tSystemManagerSession.hService) {
    l_eStatus = FrameworkunifiedSendMsg(g_tSystemManagerSession.hService,
                           SS_SM_DEBUG_DUMP_RSPN, strlen(l_cMsg) + 1, l_cMsg);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSendMsg(SS_SM_DEBUG_DUMP_RSPN)");
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error: Invalid handle[g_tSystemManagerSession.hService == NULL]");
  }

  pthread_mutex_unlock(&gMutexObj);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}  // End of EFrameworkunifiedStatus SendDebugDumpResponseToSystemManager( HANDLE hApp )

// LCOV_EXCL_START 6:Because the condition cannot be set
//////////////////////////////////////////
// SendDebugDumpResponseToSystemManager
//////////////////////////////////////////
VOID SendDebugDumpResponseToSystemManager(std::string & f_messageStr) {  // NOLINT (runtime/references)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  pthread_mutex_lock(&gMutexObj);

  if (0 == strcmp(SERVICE_SYSMANAGER, FrameworkunifiedGetAppName(g_SystemIf_hApp))) {
    l_eStatus = FrameworkunifiedSendSelf(g_SystemIf_hApp, SS_SM_DEBUG_DUMP_RSPN,
                            f_messageStr.size() + 1, f_messageStr.c_str());
    LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSendSelf(SS_SM_DEBUG_DUMP_RSPN)");
  } else if (NULL != g_tSystemManagerSession.hService) {
    l_eStatus = FrameworkunifiedSendMsg(g_tSystemManagerSession.hService,
                           SS_SM_DEBUG_DUMP_RSPN, f_messageStr.size() + 1,
                           f_messageStr.c_str());
    LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSendMsg(SS_SM_DEBUG_DUMP_RSPN)");
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: hService = NULL");
  }

  pthread_mutex_unlock(&gMutexObj);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}  // End of EFrameworkunifiedStatus SendDebugDumpResponseToSystemManager(std::string & f_messageStr)
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////
/// Function: SendInterfaceunifiedOnResponseToSystemManager()
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendInterfaceunifiedOnResponseToSystemManager(
  SS_SystemManagerProtocol f_ProtocolID, PCSTR f_pProtocolName,
  PCSTR f_InterfaceunifiedOnFncName, ModuleStateType f_moduleSuccessState,
  ModuleStateType f_moduleFailureState, EFrameworkunifiedStatus f_eStatus) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  if (eFrameworkunifiedStatusOK != f_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
           " Warning: '%s' returned %d/%s, NOT sending '%s' to System Manager",
           f_InterfaceunifiedOnFncName, f_eStatus, GetStr(f_eStatus).c_str(),
           f_pProtocolName);
    l_eStatus = f_eStatus;
    SetModuleState(f_moduleFailureState);
  } else {
    if (f_moduleSuccessState == g_moduleState) {
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
             " Warning: Module state already is '%s', just "
             "returning '%s' to System Manager",
             GetStr(g_moduleState).c_str(), f_pProtocolName);
    }

    l_eStatus = SendMsgToSystemManager(static_cast<UI_32>(f_ProtocolID),
                                       f_pProtocolName,
                                       sizeof(g_responseCompleteAck),
                                       &g_responseCompleteAck);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    const char l_cFormat[] = "SendMsgToSystemManager(%s)";
    char l_cBuf[sizeof(l_cFormat) + strlen(f_pProtocolName) + 1];  // NOLINT (runtime/arrays)
    snprintf(l_cBuf, sizeof(l_cBuf), l_cFormat, f_pProtocolName);
    LOG_STATUS(l_eStatus, l_cBuf);
    SetModuleState(f_moduleSuccessState);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of SendInterfaceunifiedOnResponseToSystemManager(

///////////////////////////////////////////////////////////
/// Function: SendMsgToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendMsgToSystemManager(UI_32 uiCmd, PCSTR f_pCmdName, UI_32 uiLength,
                                  PCVOID pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Sending %s", f_pCmdName);

  return SendMsgToSystemManager(uiCmd, uiLength, pData);
}  // End of EFrameworkunifiedStatus SendMsgToSystemManager(UI_32 uiCmd, PCSTR f_pCmdName, UI_32 uiLength, PCVOID pData)

///////////////////////////////////////////////////////////
/// Function: SendMsgToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendMsgToSystemManager(UI_32 uiCmd, UI_32 uiLength, PCVOID pData) {
  EFrameworkunifiedStatus l_eStatus;
  if (NULL == g_tSystemManagerSession.hService) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("NULL == g_tSystemManagerSession.hService");
  } else {
    pthread_mutex_lock(&gMutexObj);
    l_eStatus = FrameworkunifiedSendMsg(g_tSystemManagerSession.hService, uiCmd, uiLength,
                           pData);
    pthread_mutex_unlock(&gMutexObj);
    LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedSendMsg()");
  }
  return l_eStatus;
}  // End of EFrameworkunifiedStatus SendMsgToSystemManager(UI_32 uiCmd, UI_32 uiLength, PCVOID pData)

///////////////////////////////////////////////////////////
/// Function: InvokeSyncRequestToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus InvokeSyncRequestToSystemManager(UI_32 uiCmd, PCSTR f_pCmdName,
    UI_32 uiLength, PCVOID pData,
    UI_32 uiResLength, PVOID pResData) {
  EFrameworkunifiedStatus l_eStatus;
  UI_32 uiRcvLength = 0;
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Sending %s", f_pCmdName);

  if (NULL == g_tSystemManagerSession.hService) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("NULL == g_tSystemManagerSession.hService");
  } else {
    pthread_mutex_lock(&gMutexObj);
    l_eStatus = FrameworkunifiedInvokeSync(g_tSystemManagerSession.hService, uiCmd, uiLength,
                              pData, uiResLength, pResData, &uiRcvLength);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    if (eFrameworkunifiedStatusOK == l_eStatus) {
      if (uiResLength != uiRcvLength) {
        LOG_ERROR("uiResLength != uiRcvLength");
        l_eStatus = eFrameworkunifiedStatusFail;
      }
    }
    pthread_mutex_unlock(&gMutexObj);
    LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedInvokeSync()");
  }
  return l_eStatus;
}  // End of Function

// LCOV_EXCL_START 6:Because the condition cannot be set
///////////////////////////////////////////////////////////
/// Function: SendWakeUpToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendWakeUpToSystemManager(wakeInfo *pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = SendMsgToSystemManager(SS_SM_WAKEUP_MODULES,
                         "SS_SM_WAKEUP_MODULES",
                         sizeof(wakeInfo),
                         (PCVOID) pData);
  LOG_STATUS_IF_ERRORED(l_eStatus, "SendMsgToSystemManager()");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus SendWakeUpToSystemManager(wakeInfo *pData)
// LCOV_EXCL_STOP

// LCOV_EXCL_START 6:Because the condition cannot be set
///////////////////////////////////////////////////////////
/// Function: SendShutdownToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendShutdownToSystemManager(Pwr_ServiceSetInterface *pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = SendMsgToSystemManager(SS_SM_SHUTDOWN_MODULES,
                         "SS_SM_SHUTDOWN_MODULES",
                         sizeof(Pwr_ServiceSetInterface),
                         (PCVOID) pData);
  LOG_STATUS_IF_ERRORED(l_eStatus, "SendMsgToSystemManager()");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus SendShutdownToSystemManager(Pwr_ServiceSetInterface *pData)
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////
/// Function: RegisterSMSessionAckCallback
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus RegisterSMSessionAckCallback(EFrameworkunifiedStatus (*CallbackPtr)(HANDLE)) {
  CallbackFnPtr = CallbackPtr;
  return eFrameworkunifiedStatusOK;
}  // End of EFrameworkunifiedStatus RegisterSMSessionAckCallback(EFrameworkunifiedStatus (*CallbackPtr)(HANDLE))

// LCOV_EXCL_START 6:Because the condition cannot be set
///////////////////////////////////////////////////////////
/// Function: SendSystemModeRequestToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendSystemModeRequestToSystemManager(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = SendMsgToSystemManager(SS_SM_SYSTEM_MODE_INFO_REQ,
                         "SS_SM_SYSTEM_MODE_INFO_REQ", 0,
                         NULL);
  LOG_STATUS_IF_ERRORED(l_eStatus, "SendMsgToSystemManager()");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus SendSystemModeRequestToSystemManager(void)

///////////////////////////////////////////////////////////
/// Function: SendInitCompReportToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendInitCompReportToSystemManager(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = SendMsgToSystemManager(SS_SM_INITCOMP_REP,
                         "SS_SM_INITCOMP_REP", 0, NULL);
  LOG_STATUS_IF_ERRORED(l_eStatus, "SendMsgToSystemManager()");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus  SendInitCompReportToSystemManager(void)


///////////////////////////////////////////////////////////
/// Function: AttachCallbackToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus AttachCallbackToSystemManager(HANDLE hApp, UI_32 iCmd,
    CbFuncPtr fpOnCmd) {
  return FrameworkunifiedAttachCallbackToDispatcher(
           hApp, g_tSystemManagerSession.szServiceName.c_str(), iCmd, fpOnCmd);
}  // End of EFrameworkunifiedStatus AttachCallbackToSystemManager( HANDLE hApp, UI_32 iCmd, CbFuncPtr fpOnCmd )
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////
/// Function: SetDataResetModeToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SetDataResetModeToSystemManager(ESMDataResetModeInfo dataResetMode) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  EFrameworkunifiedStatus l_resData;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  switch (dataResetMode) {
  case e_SS_SM_DATA_RESET_MODE_NONE:
  case e_SS_SM_DATA_RESET_MODE_USER:
  case e_SS_SM_DATA_RESET_MODE_FACTORY:
    break;
  default:
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "dataResetMode %d", dataResetMode);
    return eFrameworkunifiedStatusInvldParam;
  }

  l_eStatus = InvokeSyncRequestToSystemManager(SS_SM_DATA_RESET_MODE_SET_REQ,
              "SS_SM_DATA_RESET_MODE_SET_REQ",
              sizeof(dataResetMode),
              (PVOID) & dataResetMode,
              sizeof(l_resData),
              (PVOID) & l_resData);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  LOG_STATUS_IF_ERRORED(l_eStatus, "InvokeSyncRequestToSystemManager()");

  if (eFrameworkunifiedStatusOK == l_eStatus) {
    if (eFrameworkunifiedStatusOK != l_resData) {
      l_eStatus = l_resData;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " l_eStatus %d", l_eStatus);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus SetDataResetModeToSystemManager(ESMDataResetModeInfo dataResetMode)

///////////////////////////////////////////////////////////
/// Function: SetProgUpdateStateToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SetProgUpdateStateToSystemManager(
  SMProgUpdateState progUpdateState) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  EFrameworkunifiedStatus l_resData;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (progUpdateState & SS_SM_PROG_UPDATE_STATE_UPDATED) {
  } else if (progUpdateState & SS_SM_PROG_UPDATE_STATE_MAP_UPDATED) {
  } else if (progUpdateState & SS_SM_PROG_UPDATE_STATE_MAPDIFF_UPDATED) {
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "unknown  %#x", progUpdateState);
    return eFrameworkunifiedStatusInvldParam;
  }

  l_eStatus = InvokeSyncRequestToSystemManager(
                SS_SM_PROG_UPDATE_STATE_SET_REQ, "SS_SM_PROG_UPDATE_STATE_SET_REQ",
                sizeof(progUpdateState), (PVOID) & progUpdateState, sizeof(l_resData),
                (PVOID) & l_resData);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  LOG_STATUS_IF_ERRORED(l_eStatus, "InvokeSyncRequestToSystemManager()");

  if ((eFrameworkunifiedStatusOK == l_eStatus) && (eFrameworkunifiedStatusOK != l_resData)) {
    l_eStatus = l_resData;
    LOG_ERROR("SM RESP");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus SetProgUpdateStateToSystemManager(SMProgUpdateState progUpdateState)

///////////////////////////////////////////////////////////
/// Function: SetNextWakeupTypeToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SetNextWakeupTypeToSystemManager(ESMNextWakeupType f_wakeupType) {
  EFrameworkunifiedStatus l_eStatus;
  EFrameworkunifiedStatus l_resData;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  switch (f_wakeupType) {
  case e_SS_SM_NEXT_WAKEUP_TYPE_NONE:
  case e_SS_SM_NEXT_WAKEUP_TYPE_COLD:
  case e_SS_SM_NEXT_WAKEUP_TYPE_HOT:
    break;
  default:
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "unknown type(%d)", f_wakeupType);
    return eFrameworkunifiedStatusInvldParam;
  }

  l_eStatus = InvokeSyncRequestToSystemManager(SS_SM_NEXT_WAKEUP_TYPE_SET_REQ,
              "SS_SM_NEXT_WAKEUP_TYPE_SET_REQ",
              sizeof(f_wakeupType),
              (PVOID) & f_wakeupType,
              sizeof(l_resData),
              (PVOID) & l_resData);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  LOG_STATUS_IF_ERRORED(l_eStatus, "InvokeSyncRequestToSystemManager()");

  if (eFrameworkunifiedStatusOK == l_eStatus) {
    if (eFrameworkunifiedStatusOK != l_resData) {
      l_eStatus = l_resData;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " l_eStatus %d", l_eStatus);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////
/// Function: SendCpuResetRequestToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendCpuResetRequestToSystemManager(
  ESMCpuResetReason l_eCpuResetReason, std::string f_messageStr/* = "" */,
  std::string f_suffixStr/* = "" */) {
  TSystemManagerCpuResetInfo l_resetInfo;  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  EFrameworkunifiedStatus l_eStatus;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_resetInfo.resetReason = l_eCpuResetReason;
  snprintf(l_resetInfo.messageStr, SS_SM_RESET_MSG_STR_SIZE, "%s",
           f_messageStr.c_str());  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  snprintf(l_resetInfo.suffixStr, SS_SM_SUFFIX_STR_SIZE, "%s",
           f_suffixStr.c_str());  // LCOV_EXCL_BR_LINE 11:Unexpected branch

  l_eStatus = SendMsgToSystemManager(SS_SM_CPU_RESET_REQ, "SS_SM_CPU_RESET_REQ",
                                     sizeof(l_resetInfo),
                                     (PVOID) & l_resetInfo);  // LCOV_EXCL_BR_LINE 11:Unexpected branch

  LOG_STATUS_IF_ERRORED(l_eStatus, "SendMsgToSystemManager()");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of SendCpuResetRequestToSystemManager

///////////////////////////////////////////////////////////
/// Function: SendStartupConfirmationToSystemManager
///
///////////////////////////////////////////////////////////
// LCOV_EXCL_START 6:Because the condition cannot be set
EFrameworkunifiedStatus SendStartupConfirmationToSystemManager(
  StartupConfirmationMsgStrut &f_startupConfirmationMsg) {  //NOLINT (runtime/references)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  Pwr_ServiceSetInterface tServiceSetIf;
  tServiceSetIf.data.startupConfirmationMsg = f_startupConfirmationMsg;

  l_eStatus = SendMsgToSystemManager(SS_SM_FWD_STARTUP_CONFIRMATION_MSG_REQ,
                                     "SS_SM_FWD_STARTUP_CONFIRMATION_MSG_REQ",
                                     sizeof(Pwr_ServiceSetInterface),
                                     (PCVOID) & tServiceSetIf);
  LOG_STATUS_IF_ERRORED(l_eStatus, "SendMsgToSystemManager()");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus SendStartupConfirmationToSystemManager(
// LCOV_EXCL_STOP

// LCOV_EXCL_START 6:Because the condition cannot be set
EFrameworkunifiedStatus SendSystemErrorToSystemManager(EFrameworkunifiedSystemError f_systemError) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = SendMsgToSystemManager(SS_SM_PROPAGATE_SYSTEM_ERROR,
                         sizeof(f_systemError),
                         reinterpret_cast<void*>(&f_systemError));
  LOG_STATUS_IF_ERRORED(l_eStatus, "SendMsgToSystemManager()");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus Send_CWORD56_HeartBeatRequestToSystemManager(
  EPWR_HB_REQ_MSG_STRUCT f_HbReq) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = SendMsgToSystemManager(SS_SM__CWORD56__HEARTBEAT_REQ,
                         sizeof(f_HbReq),
                         reinterpret_cast<void*>(&f_HbReq));
  LOG_STATUS_IF_ERRORED(l_eStatus, "SendMsgToSystemManager()");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

VOID Set_UseStopCompleteNotificationVsInterfaceunifiedOnStopFnc_StateVar(BOOL f_SetTrue) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  UseStopCompleteNotificationVsInterfaceunifiedOnStopFnc = f_SetTrue;
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
         " UseStopCompleteNotificationVsInterfaceunifiedOnStopFnc set '%s'",
         UseStopCompleteNotificationVsInterfaceunifiedOnStopFnc ? "True" : "False");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

EFrameworkunifiedStatus SendBootMicroResetNotificationToSystemManager(
  eSMBootMicroResetReason f_ResetReason) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  l_eStatus = SendMsgToSystemManager(SS_SM_BOOT_MICRO_RESET_NTF,
                                     "SS_SM_BOOT_MICRO_RESET_NTF",
                                     sizeof(f_ResetReason), &f_ResetReason);

  LOG_STATUS_IF_ERRORED(l_eStatus,
                        "SendMsgToSystemManager(SS_SM_BOOT_MICRO_RESET_NTF)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus SendUserInvokedLoggingRequestToSystemManager(
  eSMUserLogType f_userInvokedLogType, std::string f_messageStr/* = "" */,
  std::string f_suffixStr/* = "" */) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  try {
    switch (f_userInvokedLogType) {
    case e_SS_SM_CAPTURE_ALL_LOGS:
    case e_SS_SM_SCREEN_CAPTURE:
    case e_SS_SM_CAPTURE_INTERFACEUNIFIED_LOGS:
    case e_SS_SM_CAPTURE_DEV_LOGS:
    case e_SS_SM_CAPTURE_MODULE_LOGS:
    case e_SS_SM_CAPTURE_DTC_LOGS:
    case e_SS_SM_CAPTURE_NAVI_LOGS:
    case e_SS_SM_CAPTURE_GROUP_RELAUNCH:
      break;
    default:
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: Unknown key ID '%d'",
             f_userInvokedLogType);
      throw eFrameworkunifiedStatusInvldParam;
      break;
    }

    TSystemManagerLoggingRequestInfo l_logInfo = { };

    snprintf(l_logInfo.messageStr, SS_SM_LOG_MSG_STR_SIZE, "%s",
             f_messageStr.c_str());  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    snprintf(l_logInfo.suffixStr, SS_SM_SUFFIX_STR_SIZE, "%s",
             f_suffixStr.c_str());  // LCOV_EXCL_BR_LINE 11:Unexpected branch

    l_logInfo.logType = f_userInvokedLogType;
    l_eStatus = SendMsgToSystemManager(SS_SM_USER_INVOKED_LOG_REQ,
                                       "SS_SM_USER_INVOKED_LOG_REQ",
                                       sizeof(l_logInfo), &l_logInfo);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    LOG_STATUS_IF_ERRORED(l_eStatus,
                          "SendMsgToSystemManager(SS_SM_USER_INVOKED_LOG_REQ)");
  } catch (EFrameworkunifiedStatus e) {
    l_eStatus = e;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
}

// LCOV_EXCL_START 6:Because the condition cannot be set
EFrameworkunifiedStatus SendDiagLoggingRequestToSystemManager(
  std::string f_copyDestPathStr) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  l_eStatus = SendMsgToSystemManager(SS_SM_ERROR_EVENT_DIAG_LOG_REQ,
                                     "SS_SM_ERROR_EVENT_DIAG_LOG_REQ",
                                     f_copyDestPathStr.length() + 1,
                                     f_copyDestPathStr.c_str());
  LOG_STATUS_IF_ERRORED(
    l_eStatus, "SendMsgToSystemManager(SS_SM_ERROR_EVENT_DIAG_LOG_REQ)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus SendCANLoggingRequestToSystemManager(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  l_eStatus = SendMsgToSystemManager(SS_SM_ERROR_EVENT_CAN_LOG_REQ,
                                     "SS_SM_ERROR_EVENT_CAN_LOG_REQ", 0, NULL);

  LOG_STATUS_IF_ERRORED(
    l_eStatus, "SendMsgToSystemManager(SS_SM_ERROR_EVENT_CAN_LOG_REQ)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus SendDTCLoggingRequestToSystemManager(UI_32 f_dtc) {
  SS_ASERT(0);
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus RegisterBootMicroLogRequestCb(HANDLE hApp, CbFuncPtr fpOnCmd) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  if (NULL == hApp) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("(NULL == hApp)");
  } else if (NULL == fpOnCmd) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("(NULL == fpOnCmd)");
  } else {
    l_eStatus = FrameworkunifiedSubscribeToSessionEventWithCallback(
                  hApp, SS_SM_BOOT_MICRO_LOG_REQ, fpOnCmd,
                  g_tSystemManagerSession.hService);  // LCOV_EXCL_BR_LINE 11:Unexpected branch

    LOG_STATUS_IF_ERRORED(
      l_eStatus,
      "FrameworkunifiedSubscribeToSessionEventWithCallback(SS_SM_BOOT_MICRO_LOG_REQ)");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus SendBootMicroLogResponseToSystemManager(std::string f_logString) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  l_eStatus = SendMsgToSystemManager(SS_SM_BOOT_MICRO_LOG_RSP,
                                     "SS_SM_BOOT_MICRO_LOG_RSP",
                                     f_logString.size() + 1,
                                     f_logString.c_str());

  LOG_STATUS_IF_ERRORED(l_eStatus,
                        "SendMsgToSystemManager(SS_SM_BOOT_MICRO_LOG_RSP)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

// LCOV_EXCL_START 6:Because the condition cannot be set
EFrameworkunifiedStatus DetachCallbacksFromInterfaceunifiedDispatcher(
  HANDLE hApp, PCSTR pServiceName,
  const FrameworkunifiedProtocolCallbackHandler* pMsgHandler, UI_32 uiHandlerCount,
  HANDLE hSession) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  PUI_32 l_puiCmdIdArray;

  if (NULL == pServiceName) {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    LOG_ERROR("NULL == pServiceName");
  } else if (NULL == pMsgHandler) {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    LOG_ERROR("NULL == pMsgHandler");
  } else if (0 == uiHandlerCount) {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    LOG_ERROR("0 == uiHandlerCount");
  } else if (NULL == hSession) {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    LOG_ERROR("NULL == hSession");
  } else if (NULL
             == (l_puiCmdIdArray = new (std::nothrow) UI_32[uiHandlerCount])) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("new (std::nothrow)UI_32[ uiHandlerCount ]");
  } else {
    LoadCbHandlerCmdIDsIntoDetachCbIDsArray(pMsgHandler, l_puiCmdIdArray,
                                            uiHandlerCount);

    l_eStatus = FrameworkunifiedDetachCallbacksFromDispatcher(hApp, pServiceName,
                l_puiCmdIdArray,
                uiHandlerCount, hSession);
    LOG_STATUS(l_eStatus, "FrameworkunifiedDetachCallbacksFromDispatcher()");
    delete[] l_puiCmdIdArray;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus DetachCallbacksFromInterfaceunifiedDispatcher(HANDLE hApp,

EFrameworkunifiedStatus DetachParentCallbacksFromInterfaceunifiedDispatcher(
  HANDLE hApp, const FrameworkunifiedProtocolCallbackHandler* pMsgHandler,
  UI_32 uiHandlerCount) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  PUI_32 l_puiCmdIdArray;

  if (NULL == pMsgHandler) {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    LOG_ERROR("NULL == pMsgHandler");
  } else if (0 == uiHandlerCount) {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    LOG_ERROR("0 == uiHandlerCount");
  } else if (NULL
             == (l_puiCmdIdArray = new (std::nothrow) UI_32[uiHandlerCount])) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("new (std::nothrow)UI_32[ uiHandlerCount ]");
  } else {
    LoadCbHandlerCmdIDsIntoDetachCbIDsArray(pMsgHandler, l_puiCmdIdArray,
                                            uiHandlerCount);

    l_eStatus = FrameworkunifiedDetachParentCallbacksFromDispatcher(hApp, l_puiCmdIdArray,
                uiHandlerCount);
    LOG_STATUS(l_eStatus, "FrameworkunifiedDetachParentCallbacksFromDispatcher()");
    delete[] l_puiCmdIdArray;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus DetachParentCallbacksFromInterfaceunifiedDispatcher(HANDLE hApp,

VOID LoadCbHandlerCmdIDsIntoDetachCbIDsArray(
  const FrameworkunifiedProtocolCallbackHandler* pMsgHandler, UI_32 * puiCmdIdArray,
  UI_32 uiHandlerCount) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  for (UI_32 i = 0; i < uiHandlerCount; i++) {
    puiCmdIdArray[i] = pMsgHandler[i].iCmd;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}  // End of VOID LoadCbHandlerCmdIDsIntoDetachCbIDsArray(const FrameworkunifiedProtocolCallbackHandler* pMsgHandler,

///////////////////////////////////////////////////////////
/// Function: SendLogStartRequestToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendLogStartRequestToSystemManager(EErrorEventType f_errorEvent) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  l_eStatus = SendMsgToSystemManager(SS_SM_ERROR_EVENT_LOGGING_START_REQ,
                                     "SS_SM_ERROR_EVENT_LOGGING_START_REQ",
                                     sizeof(f_errorEvent), &f_errorEvent);

  LOG_STATUS_IF_ERRORED(
    l_eStatus, "SendMsgToSystemManager(SS_SM_ERROR_EVENT_LOGGING_START_REQ)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////
/// Function: SendLogArtifactRequestToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendLogArtifactRequestToSystemManager(EArtifactId f_artifactId) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  l_eStatus = SendMsgToSystemManager(SS_SM_ERROR_EVENT_ARTIFACT_REQ,
                                     "SS_SM_ERROR_EVENT_ARTIFACT_REQ",
                                     sizeof(f_artifactId), &f_artifactId);

  LOG_STATUS_IF_ERRORED(
    l_eStatus, "SendMsgToSystemManager(SS_SM_ERROR_EVENT_ARTIFACT_REQ)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////
/// Function: SendLogCompleteRequestToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendLogCompleteRequestToSystemManager(EFrameworkunifiedStatus f_eStatus) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  if (eFrameworkunifiedStatusOK != f_eStatus) {
    l_eStatus = eFrameworkunifiedStatusFail;
  } else {
    l_eStatus = f_eStatus;
  }

  l_eStatus = SendMsgToSystemManager(SS_SM_ERROR_EVENT_LOGGING_COMPLETE,
                                     "SS_SM_ERROR_EVENT_LOGGING_COMPLETE",
                                     sizeof(l_eStatus), &l_eStatus);

  LOG_STATUS_IF_ERRORED(
    l_eStatus, "SendMsgToSystemManager(SS_SM_ERROR_EVENT_LOGGING_COMPLETE)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////
/// Function: SendEelExportRequestToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendEelExportRequestToSystemManager(std::string f_path) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  l_eStatus = SendMsgToSystemManager(SS_SM_ERROR_EVENT_EEL_EXPORT_REQ,
                                     "SS_SM_ERROR_EVENT_EEL_EXPORT_REQ",
                                     f_path.length() + 1, f_path.c_str());

  LOG_STATUS_IF_ERRORED(
    l_eStatus, "SendMsgToSystemManager(SS_SM_ERROR_EVENT_EEL_EXPORT_REQ)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////
/// Function: SendInterfaceunifiedEmmcLogsRequestToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendInterfaceunifiedEmmcLogsRequestToSystemManager(std::string f_path) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  l_eStatus = SendMsgToSystemManager(SS_SM_ERROR_EVENT_INTERFACEUNIFIED_EMMC_LOGS_REQ,
                                     "SS_SM_ERROR_EVENT_INTERFACEUNIFIED_EMMC_LOGS_REQ",
                                     f_path.length() + 1, f_path.c_str());

  LOG_STATUS_IF_ERRORED(
    l_eStatus, "SendMsgToSystemManager(SS_SM_ERROR_EVENT_INTERFACEUNIFIED_EMMC_LOGS_REQ)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////
/// Function: SendClearLogsRequestToSystemManager
///
///////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendClearLogsRequestToSystemManager(
  TSystemManagerClearLogsInfo *f_info) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  if (!f_info) {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  } else {
    l_eStatus = SendMsgToSystemManager(SS_SM_ERROR_EVENT_CLR_LOGS_REQ,
                                       "SS_SM_ERROR_EVENT_CLR_LOGS_REQ", 0,
                                       NULL);

    LOG_STATUS_IF_ERRORED(
      l_eStatus, "SendMsgToSystemManager(SS_SM_ERROR_EVENT_CLR_LOGS_REQ)");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

VOID SetModuleState(ModuleStateType f_moduleState, BOOL f_bLog) {
  if (f_bLog) {  // LCOV_EXCL_BR_LINE 8:f_bLog is true only
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Changing SM_IF state from '%s' to '%s'",
           GetStr(g_moduleState).c_str(), GetStr(f_moduleState).c_str());
  }
  g_moduleState = f_moduleState;
}  // LCOV_EXCL_BR_LINE 10:Because the last line
// End of VOID ModuleLaunchParams::SetModuleState(SMModuleState f_moduleState )

// EOF /SS_SystemIf/src/ss_sm_client.cpp
