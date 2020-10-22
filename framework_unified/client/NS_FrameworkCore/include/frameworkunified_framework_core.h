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
/// \ingroup  tag_NSFramework
/// \brief    Application framework's core structures and data types.
///
///
///
///////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_FRAMEWORK_CORE_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_FRAMEWORK_CORE_H_

#include <pthread.h>
#include <errno.h>

#include <other_service/strlcpy.h>
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/ns_logger_if.h>

#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <cstdio>
#include <vector>
#include <utility>

static const UI_32 THREAD_SESSION_ID  = 0;

typedef HANDLE SessionHandle;
typedef int SessionId;  // < service protocol command
typedef int ServiceProtocolCommand;  // < service protocol command
typedef int Fd;  // < FD associated with callback
typedef std::string Notification;   // < notification name
typedef std::string ServiceName;    // < service name
typedef std::string ThreadName;
typedef std::string DataKey;
typedef std::string InvokerName;
typedef int Socket;
typedef std::string ConnectKey;
typedef PVOID Data;

typedef std::map<Notification, CallbackFunctionPtr> NotificationTable;


typedef std::map<ServiceProtocolCommand, CallbackFunctionPtr> ServiceProtocolTable;
typedef std::map<SessionId, ServiceProtocolTable> SessionTable;
typedef std::map<ServiceName, SessionTable> Services;
typedef std::map<HANDLE, ServiceName> ServiceSendMsgHandle;
typedef std::map<Fd, CallbackFunctionPtr> FdTable;

typedef int EventId;
typedef std::map<ServiceProtocolCommand, EventId> ServiceEventProtocolTable;
typedef std::map<SessionId, ServiceEventProtocolTable> EventSessionTable;
typedef std::map<ServiceName, EventSessionTable> EventServices;
typedef std::map<Notification, EventId> NotificationEventTable;
typedef std::map<SessionId, SessionHandle> SessionHandleTable;
typedef std::map<ServiceName, SessionHandleTable> ServiceSessionTable;
typedef std::map<InvokerName, HANDLE> InvokeResponseQTable;
typedef std::map<Socket, ConnectKey> AcceptMonitorTable;
typedef std::map<Socket, ConnectKey> ConnectMonitorTable;

typedef std::pair<NotificationTable::iterator, bool> NotificationTableRetStatus;
typedef std::map<DataKey, Data> AppData;

// private event data structure
typedef struct _EventData {
  PVOID pData;                 ///  <message data
  UI_32 uiLength;                  ///  <message length
} EventData;

// framework message queue info
typedef struct _MsgQInfo {
  CHAR cSrcName[MAX_NAME_SIZE_APP];  ///  <source message queue name
  CHAR cMsgQName[MAX_NAME_SIZE_APP];  ///  <name of the handle to the msgq
  UI_32 checkCode;
  pthread_t self;
  HANDLE hMsgQ;                  ///  <handle to message queue
  UI_32  sessionId;                  ///  <Used only when it is a session handle
} MsgQInfo;

#define MSGQ_CHECK_CODE (0xACCE55ED)

static inline BOOL frameworkunifiedCheckValidMsgQ(HANDLE hService) {
  static __thread int detect_once = 0;
  if (NULL != hService) {
    MsgQInfo *pMsgQ = reinterpret_cast<MsgQInfo *>(hService);
    /**
     * @todo
     *  If an application handle is specified in place of a session handle,
     *  the element is accessed illegally without type checking,
     *  and a DeathTest occurs because the thread_self check is violated.
     */
    if (pMsgQ->self != pthread_self() && detect_once == 0) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __func__, "Invalid thread context detect(hService/hSession):%#lx:%#lx", pMsgQ->self,
             pthread_self());
      detect_once = 1;
    }
    if (pMsgQ->checkCode == MSGQ_CHECK_CODE) {
      return TRUE;
    }
  }
  return FALSE;
}

/* data structure for public state events and data associated with it */
// map of state events and data associated with it
typedef std::map<UI_32, EventData *> PublicStateEventTable;

/* data structure for private state events and data associated with it */
// list of session id's of service and event data(private) associated with it
typedef std::map<UI_32, EventData *> SessionEventData;

// map of service name and list of its session id's and associated event data
typedef std::map<ServiceName, SessionEventData> ServiceSessionEventData;

// map of private events and the data published on the all sessions of all services subscribed to it
typedef std::map<UI_32, ServiceSessionEventData> PrivateStateEventTable;


// list of session id's for each service registering with event
typedef std::vector<UI_32> SessionIdList;
typedef std::vector<UI_32> EventIdList;

// map of service name and and list of its session id
typedef std::map<ServiceName, SessionIdList> ServiceSessionIdListTable;
// map of events and its subscribers
typedef std::map<UI_32, ServiceSessionIdListTable> EventTable;

// structure to hold the session handle and list of event ids for that session
typedef struct _SessionEventInfo {
  HANDLE    m_hSession;  // Msg queue handle for the specific session
  EventIdList m_vEvents;  // List of event ids corresponding to the session
} SessionEventInfo, *PSessionEventInfo;

// map to hold the session id and structure of session event info
typedef std::map<SessionId, SessionEventInfo *> SessionToEventInfo;

// map of service name with session id and session handle
typedef std::map<ServiceName, SessionToEventInfo > ServiceSessionHandleTable;

// Defer'ed queue
// When a pop is issued this data will be copied to the internal CFrameworkunifiedFrameworkApp struct
// which will call the FrameworkunifiedDispatchProcess that will process the message as if
// it was received off the message queue!
class DeferedMsgInfo {
  void copy_me(const DeferedMsgInfo &defer) {
    if (this != &defer) {
      uiProtocolCmd = defer.uiProtocolCmd;
      strlcpy(cMsgSrcName, defer.cMsgSrcName, sizeof(cMsgSrcName));
      std::memcpy(uiMsgRcvBuffer, defer.uiMsgRcvBuffer, MAX_MSGQ_BUFFER);
    }
  }

 public:
  UI_32  uiProtocolCmd;                   ///  <protocol req/ack received
  CHAR   cMsgSrcName[MAX_NAME_SIZE_APP];  ///  <name of client/server(application) message queue
  UI_8   uiMsgRcvBuffer[MAX_MSGQ_BUFFER];  ///  <receive buffer for message queue

  DeferedMsgInfo() : uiProtocolCmd(0) {
    std::memset(uiMsgRcvBuffer, 0, MAX_MSGQ_BUFFER);
    std::memset(cMsgSrcName, '\0', MAX_NAME_SIZE_APP);
  }

  DeferedMsgInfo(const DeferedMsgInfo &defer) {
    copy_me(defer);  //  LCOV_EXCL_BR_LINE 11:except branch
  }

  DeferedMsgInfo(UI_32 cmd, PCSTR name, UI_8 buf[MAX_MSGQ_BUFFER]) {
    DeferedMsgInfo defer;

    defer.uiProtocolCmd = cmd;
    strlcpy(defer.cMsgSrcName, name, sizeof(defer.cMsgSrcName));
    std::memcpy(defer.uiMsgRcvBuffer, buf, MAX_MSGQ_BUFFER);

    copy_me(defer);
  }

  DeferedMsgInfo &operator = (const DeferedMsgInfo &defer) {
    copy_me(defer);

    return *this;
  }
};

typedef struct _ServiceNotificationInfo {
  CHAR   sNotificationName[MAX_SYS_INFO_SIZE];
  UI_32  uiEventId;
} ServiceNotificationInfo;

typedef std::vector<ServiceNotificationInfo> NotificationList;
typedef std::queue<DeferedMsgInfo>  DeferedMessageQueue;

typedef struct _LostSessionInfo {
  std::string sServiceName;
  SessionId   iSessionId;
} LostSessionInfo;

typedef std::queue<LostSessionInfo> LostSessionQueue;

#ifdef DISPATCHER_PROFILER
class FrameworkunifiedMsgProfiler;
#endif

class ResponseData {
  std::vector<UI_8> rawBuffer;

 public:
  void set(PVOID src, UI_32 size) {
    rawBuffer.resize(size);
    std::memcpy(&rawBuffer[0], src, size);
  }

  UI_8 *data(void) {
    return rawBuffer.empty() ? NULL : &rawBuffer[0];
  }

  void clear(void) {
    std::vector<UI_8>().swap(rawBuffer);
  }

  UI_32 size(void) {
    return static_cast<UI_32>(rawBuffer.size());
  }
};


// framework container
typedef struct _CFrameworkunifiedFrameworkApp {
  CHAR cAppName[MAX_NAME_SIZE_APP];   ///  <application(client/service) name
  UI_32 checkCode;            ///  <check code
  pthread_t self;

  // message receive information
  HANDLE hAppRcvMsgQ;                     ///  <handle to apps receive message queue
  HANDLE hAppSndMsgQ;           ///  <handle to apps send message queue
  UI_32  uiMsgLength;                     ///  <length of data in message queue
  UI_32  uiProtocolCmd;                   ///  <protocol req/ack received
  UI_32  uiSessionId;
  CHAR   cSystemInfo[MAX_SYS_INFO_SIZE];
  CHAR   cMsgSrcName[MAX_NAME_SIZE_APP];  ///  <name of client/server(application) message queue
  UI_8   uiMsgRcvBuffer[MAX_MSGQ_BUFFER];  ///  <receive buffer for message queue
  int    efd;               ///  <FD for multiple wait generated by the epoll_create()
  int    defer_evfd;            ///  <FD for receiving Defer events to Dispatcher

  Services services;                   ///  <services <map>
  ServiceSendMsgHandle servicesHandle;  ///  <serviceHandle <map>
  NotificationTable notifications;     ///  <notification <map>
  FdTable fds;                         ///  <fdTable <map>

  InvokeResponseQTable invokeResQTable;

  AcceptMonitorTable acceptMonitorTable;
  ConnectMonitorTable connectMonitorTable;
  LostSessionQueue lostSessionQueue;
  CallbackFunctionPtr lostSessionHandler;

  EventServices eventservices;
  NotificationEventTable notificationevents;

  AppData appdata;

  // Defer'ed Message Queue
  DeferedMessageQueue deferedMsgQueue;  ///  <defer'ed message <queue>
  BOOL fPopDeferedMsg;

  HANDLE hNPSndMsgQ;  ///  <send msgq handle to NPService

  // application & framework errors
  EFrameworkunifiedStatus eLastFrameworkError;  ///  <Framework internal errors
  EFrameworkunifiedStatus eLastAppError;       ///  <application(client/server/child thread) reported errors

  /////////////////////////////////////////
  void *FrameworkData;
  HANDLE hParentSndMsgQ;
  CHAR cParentAppName[MAX_NAME_SIZE_APP];   ///  <application(client/service) name
  /////////////////////////////////////////

  HANDLE hConfigFileHandle;

  class CFrameworkunifiedHSMFramework *m_pFrameworkunifiedStateMachine;

  ServiceSessionTable sessiontable;
  NotificationList servicenotificationlist;
  CHAR   sServiceAvailabilityNotification[MAX_SYS_INFO_SIZE];
  BOOL bIsAvailable;

  ServiceSessionHandleTable servicesessionhandletable;

  void *pRsrcMgr;

  // framework state information
  // TODO(framework_unified): consider this for future

  // Current user
  HANDLE hUser;

  // table holding list of all the events of service and its subscribers
  EventTable eventtable;

  // table holding list of all the public events of service and the event data corresponding to it
  PublicStateEventTable publicstateeventtable;

  // table holding list of all the private events of service and
  // the event data corresponding to each of the subscribed session of its subscribers
  PrivateStateEventTable privatestateeventtable;

  ResponseData responsedata;

  UI_64 total_occupy_time;
  UI_64 total_run_count;
  UI_32 max_occupy_time;

  SI_32 siMonitorSock;

  BOOL fDeferredSyncResponse;

#ifdef DISPATCHER_PROFILER
  // Profiler object. This is required per dispatcher
  FrameworkunifiedMsgProfiler *m_pFrameworkunifiedMsgProfiler;
#endif
} CFrameworkunifiedFrameworkApp;

// Structure containing the synchronization data
struct SFrameworkunifiedSyncDataPacket {
  PVOID   m_pNotificationData;  // Data published with the notification
  UI_32   m_uiDataSize;    // Size of the data published with the notification
  time_t  m_tTimeStamp;    // Time when the notification was recently published
};

struct PCData {
  PCData(pthread_barrier_t *bar, EFrameworkunifiedStatus *status, PCSTR parent, PCSTR child, CbFuncPtr init, CbFuncPtr shdn,
         EFrameworkunifiedSchedPolicy policy, SI_32 priority, CbFuncPtr CreateHSM = NULL)
    : barrier(bar),
      childStatus(status),
      parentName(parent),
      childName(child),
      initFn(init),
      shdnFn(shdn),
      schedPolicy(policy),
      schedPriority(priority),
      CbCreateStateMachine(CreateHSM) {  // LCOV_EXCL_BR_LINE 11:except branch
  }

  pthread_barrier_t *barrier;
  EFrameworkunifiedStatus *childStatus;
  const ServiceName parentName;
  const ServiceName childName;
  CbFuncPtr initFn;
  CbFuncPtr shdnFn;
  EFrameworkunifiedSchedPolicy schedPolicy;
  SI_32 schedPriority;
  CbFuncPtr CbCreateStateMachine;
};

#define APP_CHECK_CODE (0xAC1D5EED)

static inline BOOL frameworkunifiedCheckValidAppHandle(HANDLE hApp) {
  static __thread int detect_once = 0;
  if (NULL != hApp) {
    CFrameworkunifiedFrameworkApp *pApp = reinterpret_cast< CFrameworkunifiedFrameworkApp * >(hApp);
    /**
     * @todo
     *  eFrameworkunifiedStatusOK is returned even if a appHandle created in another thread is passed,
     *  and the specifications and operations differ.
     * @todo
     *  If an application handle is specified in place of a session handle,
     *  the element is accessed illegally without type checking,
     *  and a DeathTest occurs because the thread_self check is violated.
     * @todo
     *  If you specify an app handle for a child thread,
     *  the NULL's return value is not terminated abnormally at the intended point (frameworkunifiedCheckValidAppHandle).
     */
    if (pApp->self != pthread_self() && detect_once == 0) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __func__, "Invalid thread context detect(hApp):%#lx:%#lx", pApp->self, pthread_self());
      detect_once = 1;
    }
    if (pApp->checkCode == APP_CHECK_CODE) {
      return TRUE;
    }
  }
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
///   NPSubscribeToNotification
/// API to send message to Notification Service to add to subscription list for
/// that notification
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pNotification
///         PCSTR - Name of Notification
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPSubscribeToNotification(HANDLE hApp, PCSTR pNotification);


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPSubscribeToNotifications
/// API to send message to Notification Service to add multiple subscriptions list for
/// that notification
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pList
///         FrameworkunifiedNotificationCallbackHandler - Name of Notification
/// \param [in] uiListLength
///         UI_32 - number of notification that your subscribing too.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPSubscribeToNotifications(HANDLE hApp, const  FrameworkunifiedNotificationCallbackHandler *pList, UI_32 uiListLength);

////////////////////////////////////////////////////////////////////////////////////////////
///   NPUnsubscribeFromNotification
/// API to send message to Notification Service to remove from subscription list for
/// that notification
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pNotification
///         PCSTR - Name of Notification
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPUnsubscribeFromNotification(HANDLE hApp, PCSTR pNotification);

////////////////////////////////////////////////////////////////////////////////////////////
///   FrameworkunifiedNPUnsubscribeFromNotifications
/// API to send message to Notification Service to remove multiple notification from subscription list for
/// that notification
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pList
///         FrameworkunifiedNotificationCallbackHandler - notifications list
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPUnsubscribeFromNotifications(HANDLE hApp, const  FrameworkunifiedNotificationCallbackHandler *pList,
                                             UI_32 uiListLength);


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPHSMSubscribeToNotificationsEvents
/// API to send message to Notification Service to add multiple subscriptions list for
/// that notification
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pList
///         FrameworkunifiedNotificationEvent - list of notification events
/// \param [in] uiListLength
///         UI_32 - number of notification that your subscribing too.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMSubscribeToNotificationsEvents(HANDLE hApp, const  FrameworkunifiedNotificationEvent *pList,
                                                  UI_32 uiListLength);

////////////////////////////////////////////////////////////////////////////////////////////
///   FrameworkunifiedNPHSMSubscribeToNotificationEvent
/// API to send message to Notification Service to add to subscription list for
/// that notification
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pNotification
///         PCSTR - Name of Notification
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMSubscribeToNotificationEvent(HANDLE hApp, PCSTR pNotification);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPHSMUnsubscribeFromNotificationEvent
/// API to send message to Notification Service to remove from subscription list for
/// that notification
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pNotification
///         PCSTR - Name of Notification
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMUnsubscribeFromNotificationEvent(HANDLE hApp, PCSTR pNotification);

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPHSMUnsubscribeFromNotificationEvents
/// API to send message to Notification Service to remove multiple subscriptions list for
/// that notification
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] pList
///         FrameworkunifiedNotificationEvent - list of notification events
/// \param [in] uiListLength
///         UI_32 - number of notification that your subscribing too.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedNPHSMUnsubscribeFromNotificationEvents(HANDLE hApp, const  FrameworkunifiedNotificationEvent *pList,
                                                     UI_32 uiListLength);

////////////////////////////////////////////////////////////////////////////////////////////
/// CreateDispatcher
///
/// \param [in] cAppName
///         PCSTR - Application name
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedProcessServiceProtocol(HANDLE hApp);

EFrameworkunifiedStatus frameworkunifiedAddConnectMonitor(HANDLE hApp, PCSTR serverName, PCSTR clientName);
EFrameworkunifiedStatus frameworkunifiedDelConnectMonitorPre(int efd, PCSTR serverName, PCSTR clientName);
EFrameworkunifiedStatus frameworkunifiedDelConnectMonitor(HANDLE hApp, PCSTR serverName, PCSTR clientName);

////////////////////////////////////////////////////////////////////////////////////////////
/// CreateDispatcher
///
/// \param [in] cAppName
///         PCSTR - Application name
/// \param [out] hApp
///         HANDLE& - Reference of application handle
/// \param [in] bIsChildThread
///         BOOL - child thread
///
/// \return status
///         EFrameworkunifiedStatus - eFrameworkunifiedStatusOK if success
///            eFrameworkunifiedStatusFail if invalid app name
///            eFrameworkunifiedStatusNullPointer if unable to open message Q handle
///            or possible errors from call FrameworkunifiedAttachSystemCallbacksDispatcher/
///                           FrameworkunifiedAttachLoggerCallbacksDispatcher/
///                           FrameworkunifiedAttachVersionCallbacksDispatcher
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CreateDispatcher(PCSTR cAppName, HANDLE &hApp, BOOL bIsChildThread = FALSE);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////
/// RunDispatcher
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus RunDispatcher(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// RunChildDispatcher
/// Dispatcher for child thread. Only difference with \ref RunDispatcher is, child dispatcher
/// exits when non eFrameworkunifiedStatusOK value by callback function is returned.
/// Whereas, \ref RunDispatcher is not supposed to exit.
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus RunChildDispatcher(HANDLE hChildApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// IsValidWaitBarrier
///
/// \param [in] wbret
///         int - barrier
///
/// \return bool
///         valid or invalid
////////////////////////////////////////////////////////////////////////////////////////////
bool IsValidWaitBarrier(int wbret);

////////////////////////////////////////////////////////////////////////////////////////////
/// setChildThreadSched
///
/// \param [in] policy
///         EFrameworkunifiedSchedPolicy - scheduling policy
/// \param [in] priority
///         SI_32 - scheduling priority
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus setChildThreadSched(EFrameworkunifiedSchedPolicy policy, SI_32 priority);

////////////////////////////////////////////////////////////////////////////////////////////
/// CreateChildThread
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
/// \param [in] childName
///     PCSTR - Name of child thread to be created
/// \param [in] CbInitialize
///     CbFuncPtr - Initialize function.
/// \param [in] CbShutdown
///     CbFuncPtr - Shutdown function.
/// \param [in] attr
///     const FrameworkunifiedChildThreadAttr * - child thread attribute
/// \param [in] CbCreateStateMachine
///     CbFuncPtr - Function pointer to create state machine, in case of SM child thread, else NULL
///
/// \return HANDLE
///         Non-null value if thread is created else NULL
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE CreateChildThread(HANDLE hApp, PCSTR childName, CbFuncPtr CbInitialize, CbFuncPtr CbShutdown,
                         const FrameworkunifiedChildThreadAttr *attr, CbFuncPtr CbCreateStateMachine);

////////////////////////////////////////////////////////////////////////////////////////////
/// Child thread attribute initialize
///
/// \param [out] attr
///         FrameworkunifiedChildThreadAttr - Child thread attribute
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CreateChildThreadAttrInit(FrameworkunifiedChildThreadAttr *attr);

////////////////////////////////////////////////////////////////////////////////////////////
/// Child thread attribute set schedule policy & priority
///
/// \param [out] attr
///         FrameworkunifiedChildThreadAttr - Child thread attribute
/// \param [int] policy
///         EFrameworkunifiedSchedPolicy - Child thread scheduling policy
/// \param [in] priority
///         SI_32 - Child thread scheduling priority
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CreateChildThreadAttrSetSched(FrameworkunifiedChildThreadAttr *attr, EFrameworkunifiedSchedPolicy policy, SI_32 priority);

////////////////////////////////////////////////////////////////////////////////////////////
/// DestroyThread
//  Sends eFrameworkunifiedStatusExit status to exits the dispatcher loop to terminate the thread.
///
/// \param [in] hApp
///         HANDLE - Handle to the Framework
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus DestroyThread(HANDLE hApp);

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_FRAMEWORK_CORE_H_
