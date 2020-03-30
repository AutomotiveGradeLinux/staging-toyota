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
 * @file ss_logger_service_ifc.h
 * @brief \~english This file supports the the Logger Service.
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup logger_service
 *  @ingroup system_service
 *  @{
 */

#ifndef SS_LOGGER_SERVICE_IFC_H_  // NOLINT (build/header_guard)
#define SS_LOGGER_SERVICE_IFC_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>
#include "system_service/ss_logger_service.h"
#include "system_service/ss_logger_service_protocol.h"

/**
 * @class LoggerServiceIf
 * \~english @brief logger_service
 * \~english @par   Brief Introduction
 *        Class to provide LoggerServiceIf
 *
 */
class LoggerServiceIf {
 public:
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LoggerServiceIf
  /// \~english @par Summary
  ///       Constructor of LoggerServiceIf class.
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       None
  /// \~english @par Detail
  ///       To generate a LoggerServiceIf class, and initialize member variables(m_hApp, m_hService, m_hSession) to be
  ///       NULL. \n
  ///       After the constructor, be sure to call the Initialize.
  /// \~english @see  ~LoggerServiceIf, Initialize
  ////////////////////////////////////////////////////////////////////////////////////
  LoggerServiceIf();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~LoggerServiceIf
  /// \~english @par Summary
  ///       Destructor of LoggerServiceIf class.
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       None
  /// \~english @par Detail
  ///       To generate a LoggerServiceIf class. \n
  ///       If the service and the session is started (open) state, causing the end (closed).
  /// \~english @see  LoggerServiceIf
  ////////////////////////////////////////////////////////////////////////////////////
  virtual ~LoggerServiceIf();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Initialize
  /// \~english @par Summary
  ///       API to initialize the LoggerServiceIf class.
  /// \~english @param [in] hApp
  ///        HANDLE   - HANDLE for Application
  /// \~english @retval TRUE Success
  /// \~english @retval FALSE Failure
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLE(hApp) specified in the argument is NULL. [FALSE]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       Application to use the services of SS_LoggerService is, after generating a LoggerServiceIf class, which is a
  ///       method to be executed first. \n
  ///       Member variable(m_hApp) of LoggerServiceIf class initialized with hApp that has been specified by the
  ///       argument.
  /// \~english @see  LoggerServiceIf
  ////////////////////////////////////////////////////////////////////////////////////
  BOOL Initialize(HANDLE hApp);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NotifyOnLoggerServiceAvailability
  /// \~english @par Summary
  ///       API to set the Callback function for Availability notification of SS_LoggerService.
  /// \~english @param [in] f_pCallBackFn
  ///        CbFuncPtr   - Pointer to function callback for SS_LoggerService Availability Notification
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc.) has been
  ///       done.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Callback function pointer specified by the argument is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Can not get the App/Thread name from HANDLE in the class member variables (m_hApp).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE (m_hApp) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldParam]
  ///       - The acquisition of the buffer that stores the Callback information specified in the argument failed.
  ///       [eFrameworkunifiedStatusInvldParam]
  ///       - Can not be acquired memory in the acquisition of the buffer of the message for transmission (malloc).
  ///       [eFrameworkunifiedStatusFail]
  ///       - Message queue HANDLE is NULL in the HANDLE of the class member variables (m_hApp). [eFrameworkunifiedStatusInvldParam]
  ///       - Message queue HANDLE in HANDLE (m_hApp) of the class member variables are not appropriate(which is
  ///       invalid). [eFrameworkunifiedStatusInvldParam]
  ///       - It is not appropriate message queue name of HANDLE (m_hApp) in the class member variables(The name is
  ///       NULL, more than 20byte). [eFrameworkunifiedStatusInvldHandle]
  ///       - Is not a transmission type message queue type of HANDLE (m_hApp) in the class member variables.
  ///       [eFrameworkunifiedStatusInvldHndlType]
  ///       - Session message queue is full to SS_LoggerService. [eFrameworkunifiedStatusMsgQFull]
  ///       - It is invalid transmission file descriptor of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message for
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - Incorrect size of the transmit buffer of the session message to SS_LoggerService. [eFrameworkunifiedStatusInvldBufSize]
  ///       - Any errors occur during the transmission of a session message to SS_LoggerService. [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message for SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Pub-Sub x Pub-Sub
  /// \~english @par Detail
  ///       Which is a method to register a Callback function for the initialization of the SS_LoggerService receives
  ///       the timing at which the start (open) is possible of completed service.\n
  ///       (Using API:FrameworkunifiedSubscribeNotificationsWithCallback of Native Service, to register a Callback function for
  ///       Availability notification of SS_LoggerService.)
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotifyOnLoggerServiceAvailability(CbFuncPtr f_pCallBackFn);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NotifyOnOpenSessionAck
  /// \~english @par Summary
  ///       API to set the Callback function for OpenSessionRequest response from SS_LoggerService.
  /// \~english @param [in] f_pCallBackFn
  ///        CbFuncPtr   - Callback function pointer for receiving the response of OpenSessionRequest
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @par Preconditions
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc.) has been
  ///       done.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Callback function pointer specified by the argument is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE (m_hApp) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldParam]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       To register the Callback function for receiving a response of completion of OpenSessionRequest to
  ///       Dispatcher. \n
  ///       (Using API:FrameworkunifiedAttachCallbackToDispatcher of Native Service, to register a Callback function for receiving a
  ///       response of completion of OpenSessionRequest to Dispatcher.)
  /// \~english @see  NotifyOnCloseSessionAck
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotifyOnOpenSessionAck(CbFuncPtr f_pCallBackFn);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NotifyOnCloseSessionAck
  /// \~english @par Summary
  ///       API to set the Callback function for CloseSessionRequest response from SS_LoggerService.
  /// \~english @param [in] f_pCallBackFn
  ///        CbFuncPtr   - Callback function pointer for receiving the response of OpenSessionRequest
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @par Preconditions
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Callback function pointer specified by the argument is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE (m_hApp) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldParam]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       To register the Callback function for receiving a response of completion of CloseSessionRequest to
  ///       Dispatcher. \n
  ///       (Using API:FrameworkunifiedAttachCallbackToDispatcher of Native Service, to register a Callback function for receiving a
  ///       response of completion of CloseSessionRequest to Dispatcher.) \n
  /// \~english @see  NotifyOnOpenSessionAck
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotifyOnCloseSessionAck(CbFuncPtr f_pCallBackFn);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup OpenSessionRequest
  /// \~english @par Summary
  ///       API to generate service/session of SS_LoggerService.
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Availability of SS_LoggerService is TRUE.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  ///       - Use the NotifyOnOpenSessionAck(), that you register a callback function for receiving a completion
  ///       response of OpenSessionRequest to Dispatcher
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - SS_LoggerService is not Availability state. [eFrameworkunifiedStatusFail]
  ///       - SS_LoggerService can not be opened (can not start). [eFrameworkunifiedStatusFail]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Method x Fire and Forget
  /// \~english @par Detail
  ///       Which is a method for generating service/session of SS_LoggerService.\n
  ///       (To send the command of PROTOCOL_OPEN_SESSION_REQ to SS_LoggerService.)\n
  ///       If you already service has been created, once to close the service, and to re-open.
  /// \~english @see  CloseSessionRequest
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OpenSessionRequest();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup DecodeOpenSessionResponse
  /// \~english @par Summary
  ///       API to hold the session ID in SS_LoggerServiceIf class
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc.) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Incorrect PROTOCOL_OPEN_SESSION_ACK message length of SS_LoggerService. [eFrameworkunifiedStatusFail]
  ///       - PROTOCOL_OPEN_SESSION_ACK message of SS_LoggerService can not be acquired. [eFrameworkunifiedStatusFail]
  ///       - Can not session generation of message transmission to SS_LoggerService. [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       Get the session initiation response message(PROTOCOL_OPEN_SESSION_ACK) of SS_LoggerService, and get the
  ///       session ID from the message. \n
  ///       Acquired session ID, (after opening the message transmission queue) created after a session of message
  ///       transmission, hold as the session information in the session HANDLE(m_hSession) of the class member
  ///       variable.
  /// \~english @see  OpenSessionRequest, CloseSessionRequest
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus DecodeOpenSessionResponse();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CloseSessionRequest
  /// \~english @par Summary
  ///       API to discard service/session of SS_LoggerService.
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc.) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  ///       - Use the NotifyOnCloseSessionAck(), that you register a callback function for receiving a completion
  ///       response of CloseSessionRequest to Dispatcher
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Pub-Sub x Pub-Sub
  /// \~english @par Detail
  ///       which is a method for discarding service/session of SS_LoggerService.\n
  ///       (To send the command of PROTOCOL_CLOSE_SESSION_REQ to SS_LoggerService.)
  /// \~english @see  OpenSessionRequest
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CloseSessionRequest();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup RegisterForLoggerEvent
  /// \~english @par Summary
  ///       API to register a Callback function for SS_LoggerService.
  /// \~english @param  [in] f_eLoggerEvent
  ///        SS_LoggerServerEvents   - Event type
  /// \~english @param  [in] f_pCallBackFn
  ///        CbFuncPtr   -  Pointer to a callback function that corresponds to the event
  /// \~english @par
  ///            SS_LoggerServerEvents  enum value
  /// \~english @code
  ///            typedef enum _LoggerServerEvents
  ///            {
  ///                  SS_LOGGER_SCREENCAPTURE_EVT = 0xA0, /* Event notification of the Screen Capture    */
  ///                  SS_LOGGER_ERRORINFO_EVT,            /* Event notification of error log information */
  ///                  SS_LOGGER_LOGINFO_EVT,              /* Event notification of log information       */
  ///                  SS_LOGGER_LOGSTARTED_EVT            /* Event notification at the time of log start */
  ///            }SS_LoggerServerEvents;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc.) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Callback function pointer specified by the argument is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldParam]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       Which is a method to register a callback function for receiving the timing of the occurrence of an event in
  ///       the SS_LoggerService to the Dispatcher. \n
  ///       (To send the command of PROTOCOL_REGISTER_EVENTS to SS_LoggerService.)
  /// \~english @see  UnRegisterForLoggerEvent
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus RegisterForLoggerEvent(SS_LoggerServerEvents f_eLoggerEvent, CbFuncPtr f_pCallBackFn);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup UnRegisterForLoggerEvent
  /// \~english @par Summary
  ///       API to cancel a Callback function for SS_LoggerService.
  /// \~english @param  [in] f_eLoggerEvent
  ///        SS_LoggerServerEvents   - Event type
  /// \~english @par
  ///            SS_LoggerServerEvents  enum value
  /// \~english @code
  ///            typedef enum _LoggerServerEvents
  ///            {
  ///                  SS_LOGGER_SCREENCAPTURE_EVT = 0xA0, /* Event notification of the Screen Capture    */
  ///                  SS_LOGGER_ERRORINFO_EVT,            /* Event notification of error log information */
  ///                  SS_LOGGER_LOGINFO_EVT,              /* Event notification of log information       */
  ///                  SS_LOGGER_LOGSTARTED_EVT            /* Event notification at the time of log start */
  ///            }SS_LoggerServerEvents;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc.) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       Which is a method to cancel a callback function for receiving the timing of the occurrence of an event in
  ///       the SS_LoggerService. \n
  /// \~english @see  RegisterForLoggerEvent
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus UnRegisterForLoggerEvent(SS_LoggerServerEvents f_eLoggerEvent);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SendDiagStat
  /// \~english @par Summary
  ///       API to send CAN Diagnostic status data(it has mileage information) to SS_LoggerService.
  /// \~english @param  [in] pCanDiagStat_t
  ///        STLOGGER_CANDIAGSTAT   - Pointer to structure of CAN Diagnostic state data
  /// \~english @par
  ///            STLOGGER_CANDIAGSTAT Structure
  /// \~english @code
  ///            typedef struct _TInterfaceunifiedCANMileageInfo
  ///            {
  ///                  UI_8  DidA_ExtTest_Pres;    /* For extended test(unused)     */
  ///                  UI_8  EngRun_Stat;          /* Engine run state(unused)      */
  ///                  UI_8  Odo_MSB_H;            /* Mileage information(MSB:High) */
  ///                  UI_8  Odo_MSB_L;            /* Mileage information(MSB:Low)  */
  ///                  UI_8  Odo_LSB_H;            /* Mileage information(LSB:High) */
  ///                  UI_8  Odo_LSB_L;            /* Mileage information(LSB:Low)  */
  ///                  UI_8  PN14_SupBat_Volt;     /* Battery voltage(unused)       */
  ///            }STLOGGER_CANDIAGSTAT;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Sync
  /// \~english @par Detail
  ///       Which is a method to send the mileage information in the CAN diagnostic state data to SS_LoggerService. \n
  ///       Send a message in the command ID of SS_LOGGER_MILEAGE_DATA to SS_LoggerService. \n
  ///       SS_LoggerService uses the log write thread, to output a log of the mileage information.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SendDiagStat(STLOGGER_CANDIAGSTAT *pCanDiagStat_t);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LoggerIf_CANGetCurrentDateAndTime
  /// \~english @par Summary
  ///       API to send the current date and time of the CAN to SS_LoggerService.
  /// \~english @param  [in] stDateAndTime
  ///        STCanCurrentDateTime   - Structure of CAN current date and time
  /// \~english @par
  ///            STCanCurrentDateTime Structure
  /// \~english @code
  ///            typedef struct __CWORD62_DateAndTime
  ///            {
  ///                  UI_8  DateTime_Stat;     /* Date and Time State */
  ///                  UI_8  DateTimeDay;       /* Day                 */
  ///                  UI_8  DateTimeHour;      /* Hour                */
  ///                  UI_8  DateTimeMinute;    /* Minute              */
  ///                  UI_8  DateTimeMonth;     /* Month               */
  ///                  UI_8  DateTimeSecond;    /* Second              */
  ///                  UI_8  DateTimeYear;      /* Year                */
  ///                  UI_8  TimeFormat;        /* Time display format */
  ///            }STCanCurrentDateTime;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Sync
  /// \~english @par Detail
  ///       Which is a method to send current date and time of the CAN to SS_LoggerService. \n
  ///       Send a message in the command ID of SS_LOGGER_SET_DATETIME to SS_LoggerService.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus LoggerIf_CANGetCurrentDateAndTime(STCanCurrentDateTime stDateAndTime);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LoggerIf_CANSetVIN
  /// \~english @par Summary
  ///       API to send the Vehicle Identification Number(VIN) to SS_LoggerService.
  /// \~english @param  [in] stVinNumber
  ///        STVIN_NUMBER&   - Structure of Vehicle Identification Number(VIN)
  /// \~english @par
  ///            STVIN_NUMBER Structure
  /// \~english @code
  ///            typedef struct _TVINnumber
  ///            {
  ///                  static const UI_8 VIN_LEN = 18;
  ///                  CHAR   VINstr[VIN_LEN];        /* Vehicle Identification Number(VIN)  */
  ///            }STVIN_NUMBER;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Sync
  /// \~english @par Detail
  ///       Which is a method to send the Vehicle Identification Number(VIN) to SS_LoggerService. \n
  ///       Send a message in the command ID of SS_LOGGER_SET_VIN to Logger Service. \n
  ///       SS_LoggerService uses the log write thread, to output a log of the Vehicle Identification Number(VIN).
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus LoggerIf_CANSetVIN(STVIN_NUMBER&  stVinNumber);  // NOLINT (runtime/references)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LoggerIf_SetLoggerParams
  /// \~english @par Summary
  ///       API to send the logging propriety information of the selected device to SS_LoggerService.
  /// \~english @param  [in] eLoggerStatus
  ///        ELOGGER_STAT   - Logging propriety information
  /// \~english @param  [in] eDevType
  ///        EDEV_TYPE   - Device type
  /// \~english @par
  ///            ELOGGER_STAT  enum value
  /// \~english @code
  ///            typedef enum _ELoggerState
  ///            {
  ///                  eDeactivate,          /* Logging impossible(Deativate) */
  ///                  eActivate,            /* Logging possible(Activate)    */
  ///                  eInvalid_LoggerState  /* Invalid logging state         */
  ///            }ELOGGER_STAT;
  ///            @endcode
  /// \~english @par
  ///            EDEV_TYPE  enum value
  /// \~english @code
  ///            typedef enum _ELoggerDeviceTypes
  ///            {
  ///                  eDevUSB1,             /* USB1                 */
  ///                  eDevUSB2,             /* USB2                 */
  ///                  eDevSD,               /* SD                   */
  ///                  eTotalDevicesTypes,   /* Total device types   */
  ///                  eInvalid_DevType      /* Invalid device type  */
  ///            }EDEV_TYPE;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Sync
  /// \~english @par Detail
  ///       Which is a method to send the logging propriety information of the selected device to SS_LoggerService. \n
  ///       Send a message in the command ID of SS_LOGGER_SET_PARAMS to SS_LoggerService. \n
  ///       SS_LoggerService is based on the logging propriety information of the transmission selection device, and
  ///       update as a member variable of the class.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus LoggerIf_SetLoggerParams(ELOGGER_STAT eLoggerStatus, EDEV_TYPE eDevType);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LoggerIf_UDPLogging
  /// \~english @par Summary
  ///       API to send the logging propriety information of UDP to SS_LoggerService.
  /// \~english @param  [in] eLoggerStatus
  ///        ELOGGER_STAT   - Logging propriety information
  /// \~english @par
  ///            ELOGGER_STAT  enum value
  /// \~english @code
  ///            typedef enum _ELoggerState
  ///            {
  ///                  eDeactivate,          /* Logging impossible(Deativate) */
  ///                  eActivate,            /* Logging possible(Activate)    */
  ///                  eInvalid_LoggerState  /* Invalid logging state         */
  ///            }ELOGGER_STAT;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Sync
  /// \~english @par Detail
  ///       Which is a method to send the logging propriety information of UDP to SS_LoggerService. \n
  ///       Send a message in the command ID of SS_LOGGER_UDP_LOGGING to SS_LoggerService. \n
  ///       SS_LoggerService is based on the logging propriety information of the transmission UDP, and update as a
  ///       member variable of the class.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus LoggerIf_UDPLogging(ELOGGER_STAT eLoggerStatus);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LoggerIf_ScreenCaptureEventACK
  /// \~english @par Summary
  ///       API to send the event response of screen capture to SS_LoggerService.
  /// \~english @param  [in] stScreenCaptureInfo
  ///        STScreenCaptureEvt   - Structure of event data of screen capture
  /// \~english @par
  ///            STScreenCaptureEvt Structure
  /// \~english @code
  ///            typedef struct _TInterfaceunifiedScreenCaptureEvt
  ///            {
  ///                  static const UI_16  STR_BUFF_LEN = 64;
  ///                  BOOL                fSucessful;                       /* Success propriety flag        */
  ///                  UI_32               uiSceenShotId;                    /* Screen capture ID             */
  ///                  CHAR                strNameAndLocation[STR_BUFF_LEN]; /* Screen capture save file name */
  ///                  UI_32               uiFaultReasonCode;                /* Failure reason code           */
  ///            }STScreenCaptureEvt;
  ///            @endcode
  /// \~english @par
  ///            uiFaultReasonCode : Failure reason code (plm::screen_shot::status List)
  ///               - ok : Success
  ///               - invalid_window_handle         : Invalid window handle
  ///               - invalid_context_handle        : Invalid context handle
  ///               - display_count_error           : Display count error
  ///               - rgb_display_not_found         : RGB display not found
  ///               - create_pixmap_error           : Create error of PIXMAP
  ///               - usage_pixmap_property_error   : Usage error of PIXMAP property
  ///               - format_pixmap_property_error  : Format error of PIXMAP property
  ///               - buffer_pixmap_property_error  : Buffer error of PIXMAP property
  ///               - create_pixmap_buffer_error    : Create error of PIXMAP buffer
  ///               - render_pixmap_property_error  : Render error of PIXMAP property
  ///               - pointer_pixmap_property_error : Pointer error of PIXMAP property
  ///               - stride_pixmap_property_error  : Stride error of PIXMAP property
  ///               - window_read_error             : Window read error
  ///               - write_bitmap_file_error       : Writing error of bitmap files
  ///               - window_post_error             : Window post error
  ///               - window_property_error         : Window property error
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Sync
  /// \~english @par Detail
  ///       As the data in the event the response of the screen capture to SS_LoggerService, which is a method for
  ///       transmitting the information of the screen capture ID and screen capture save file name.\n
  ///       Send a message in the command ID of SS_LOGGER_SCREENCAPTURE_EVT_ACK to SS_LoggerService. \n
  ///       This API is an API for screen capture in PLM(Physical Layer Management) function support. In PLM
  ///       non-support, to prohibit the use.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus LoggerIf_ScreenCaptureEventACK(STScreenCaptureEvt stScreenCaptureInfo);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LoggerIf_EventLoggingCommonInfo
  /// \~english @par Summary
  ///       API to send the event information of the common log in SS_LoggerService.
  /// \~english @param  [in] stEventCommonInfo
  ///        STEventLoggerCommonInfo   - Event information structure of the common log
  /// \~english @par
  ///            STEventLoggerCommonInfo Structure
  /// \~english @code
  ///            typedef struct _SEventLoggerCommonInfo
  ///            {
  ///                  UI_8  BodyCAN_Stat:4;        /* CAN state of the body           */
  ///                  UI_8  HeadUnitCAN_Stat:4;    /* CAN state of the Head Unit      */
  ///                  UI_8  HMIInteraction:4;      /* HMI Interaction                 */
  ///                  UI_8  IGN_Status:4;          /* Ignition state                  */
  ///                  UI_8  FOT_Temp:2;            /* Field Operation Tests Temporary */
  ///                  UI_8  SystemVoltage:6;       /* System Voltage                  */
  ///            }STEventLoggerCommonInfo;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Fire and Forget
  /// \~english @par Detail
  ///       It is a method to send the event information of the common log to SS_LoggerService. \n
  ///       Send a message in the command ID of SS_LOGGER_EVENT_COMMONINFO to SS_LoggerService. \n
  ///       Using the log write thread, SS_LoggerService outputs the following log information.
  ///       - CAN state of the body
  ///       - CAN state of the Head Unit
  ///       - HMI Interaction
  ///       - Ignition state
  ///       - System Voltage
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus LoggerIf_EventLoggingCommonInfo(STEventLoggerCommonInfo stEventCommonInfo);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LoggerIf_EventLoggingEventInfo
  /// \~english @par Summary
  ///       API to send the event information of the _CWORD56_ log to SS_LoggerService.
  /// \~english @param  [in] stEventInfo
  ///        STEventLoggerEventInfo   - Event information structure of _CWORD56_ log
  /// \~english @par
  ///            STEventLoggerEventInfo Structure
  /// \~english @code
  ///            typedef struct _SEventLoggerCommonInfo
  ///            {
  ///                  UI_8 NumberOfEvents;       /* Event count       */
  ///                  UI_8 EventGroup;           /* Event group       */
  ///                  UI_8 EventIdentifier;      /* Event identifier  */
  ///                  UI_8 EventData[4];         /* Event data        */
  ///            }STEventLoggerEventInfo;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Fire and Forget
  /// \~english @par Detail
  ///       It is a method to send the event information of _CWORD56_ log to SS_LoggerService. \n
  ///       Send a message in the command ID of SS_LOGGER__CWORD56__EVENT_INFO to SS_LoggerService.\n
  ///       Using the log write thread, SS_LoggerService outputs the following log information.
  ///       - Event count
  ///       - Event group
  ///       - Event identifier
  ///       - Event data
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus LoggerIf_EventLoggingEventInfo(STEventLoggerEventInfo stEventInfo);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LoggerIf_EventLoggingResetInfo
  /// \~english @par Summary
  ///       API to send the event information of _CWORD56_ reset count to SS_LoggerService.
  /// \~english @param  [in] stResetInfo
  ///        STEventLoggerResetInfo   - Event information structure of _CWORD56_ reset counter
  /// \~english @par
  ///            STEventLoggerResetInfo Structure
  /// \~english @code
  ///            typedef struct _SEventLoggerResetInfo
  ///            {
  ///                  UI_8 _CWORD56__ResetInfo;      /* Reset counter information of _CWORD56_   */
  ///                  UI_8 _CWORD102__ResetInfo;        /* Reset counter information of _CWORD102_     */
  ///            }STEventLoggerResetInfo;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Fire and Forget
  /// \~english @par Detail
  ///       It is a method to send the event information of _CWORD56_ reset counter to SS_LoggerService. \n
  ///       Send a message in the command ID of SS_LOGGER__CWORD56__RESET_INFO to SS_LoggerService. \n
  ///       Using the log write thread, SS_LoggerService outputs the following log information.
  ///       - Reset counter information of _CWORD56_
  ///       - Reset counter information of _CWORD102_
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus LoggerIf_EventLoggingResetInfo(STEventLoggerResetInfo stResetInfo);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LoggerIf_PersistEventLogOnActiveDTC
  /// \~english @par Summary
  ///       API to persist the event log on the DTC to SS_LoggerService.
  /// \~english @param  None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Sync
  /// \~english @par Detail
  ///       Which is a method to send a message of the order to immediately persist the event log on the DTC(Diagnostic
  ///       Trouble Code) to SS_LoggerService. \n
  ///       Send a message in the command ID of SS_LOGGER_DIAGDTC_ACTIVE to SS_LoggerService.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus LoggerIf_PersistEventLogOnActiveDTC(void);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LoggerIf_Shutdown_Complete
  /// \~english @par Summary
  ///       API to send a shutdown completion message to SS_LoggerService.
  /// \~english @param  None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Fire and Forget
  /// \~english @par Detail
  ///       Which is a method to send a shutdown completion message to SS_LoggerService.\n
  ///       Send a message in the command ID of SS_LOGGER_SHUTDOWN_COMPLETE to SS_LoggerService.\n
  ///       SS_LoggerService stops the thread for the log writing, and the Availability of self-service is set to FALSE,
  ///       to disable the service.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus LoggerIf_Shutdown_Complete(void);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup StoreEventLogsToUSB
  /// \~english @par Summary
  ///       API to request an event log output to USB/SD to SS_LoggerService.
  /// \~english @param  [in] eUSBDevNumber
  ///        EEvtLogUSBDevNumber   - Event Log output destination USB/SD device number
  /// \~english @par
  ///            EEvtLogUSBDevNumber  enum value
  /// \~english @code
  ///            typedef enum  _EEventLoggerUSBDeviceNumber
  ///            {
  ///                  USB0=0,         /* USB0 */
  ///                  USB1=1,         /* USB1 */
  ///                  SD=2            /* SD   */
  ///            }EEvtLogUSBDevNumber;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Method x Fire and Forget
  /// \~english @par Detail
  ///      Which is a method to request the event log output to USB/SD to SS_LoggerService.\n
  ///      To send a message in the command ID of SS_LOGGERCOPYEVENTUSB against SS_LoggerService. \n
  ///      Using the event log writing thread, SS_LoggerService output the event log to the specified device (USB/SD).
  ///      \n
  ///      As a result of the event output, the message is notified to the caller of the API in the command ID below.
  ///       - Success: SS_LOGGERCOPYEVENTUSB_SUCCESS_RESP
  ///       - Failure: SS_LOGGERCOPYEVENTUSB_ERROR_RESP
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus StoreEventLogsToUSB(EEvtLogUSBDevNumber eUSBDevNumber);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup StoreEmergencyErrorLogsToUSB
  /// \~english @par Summary
  ///       API to request the emergency error log output to USB/SD to SS_LoggerService.
  /// \~english @param  [in] eDevNumber
  ///        EDevNumber    - Emergency error log output destination USB/SD device number
  /// \~english @par
  ///            EDevNumber   value of enum
  /// \~english @code
  ///            typedef enum  _EDevNumber
  ///            {
  ///                  eEEL_USB1=1,         /* USB0 */
  ///                  eEEL_USB2=2,         /* USB1 */
  ///                  eEEL_SD=3            /* SD   */
  ///            }EDevNumber ;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Fire and Forget
  /// \~english @par Detail
  ///      Which is a method to request the emergency error log output to USB/SD to SS_LoggerService.\n
  ///      To send a message in the command ID of SS_LOGGERCOPYEMERGENCYLOGS against SS_LoggerService. \n
  ///      Using the diagnostic log output function of System Manager, SS_LoggerService output the emergency error log
  ///      to the specified device(USB/SD). \n
  ///      As a result of the clearing process of the event log, the message is notified to the caller of the API in the
  ///      command ID below.
  ///       - Success: SS_LOGGERCOPYEMERGENCYLOGS_SUCCESS_RESP
  ///       - Failure: SS_LOGGERCOPYEMERGENCYLOGS_ERROR_RESP
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus StoreEmergencyErrorLogsToUSB(EDevNumber eDevNumber);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup StartCANLogging
  /// \~english @par Summary
  ///       API to request the start of CAN logging to SS_LoggerService.
  /// \~english @param  None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Fire and Forget
  /// \~english @par Detail
  ///      Which is a method to request the start of CAN logging against SS_LoggerService. \n
  ///      This function is called by CAN to trigger Logging to USB. \n
  ///      Progress is published by notifications eSSLoggerCANEventStart, eSSLoggerCANEventError and
  ///      eSSLoggerCANEventFinished. \n
  ///      To send a message in the command ID of eSSLoggerCANProtocolIDCANTrigger against SS_LoggerService. \n
  ///      SS_LoggerService requests the logging start of CAN to the System Manager.
  /// \~english @see  RegisterLoggingStartNotification, RegisterLoggingFinishNotification
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus StartCANLogging(void);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup RegisterLoggingStartNotification
  /// \~english @par Summary
  ///       API to set a callback function for CAN logging start notification.
  /// \~english @param [in] f_pCallBackFn
  ///        CbFuncPtr   - Callback function pointer for CAN logging start notification
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc.) has been
  ///       done.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Callback function pointer specified by the argument is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - HANDLE (m_hApp) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldParam]
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE (m_hSession) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Handle name of the session message queue is NULL to SS_LoggerService. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message queue HANDLE of the session message to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message queue HANDLE of the session message to SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - It is not appropriate message queue name of the session message to SS_LoggerService(The name is NULL, more
  ///       than 20byte). [eFrameworkunifiedStatusInvldHandle]
  ///       - Is not a transmission type message queue type of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldHndlType]
  ///       - Session message queue is full to SS_LoggerService. [eFrameworkunifiedStatusMsgQFull]
  ///       - It is invalid transmission file descriptor of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message for
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - Incorrect size of the transmit buffer of the session message to SS_LoggerService. [eFrameworkunifiedStatusInvldBufSize]
  ///       - Any errors occur during the transmission of a session message to SS_LoggerService. [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message for SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Pub-Sub x Pub-Sub
  /// \~english @par Detail
  ///       Use the StartCANLogging, after the request of the start of the CAN logging operation, which is a method to
  ///       register a callback function to receive a notification of the start of the logging operation.\n
  ///       (Using API:FrameworkunifiedSubscribeToSessionEventWithCallback of Native Service, to register a Callback function for CAN
  ///       logging start notification.) \n
  ///       SS_LoggerService publish the logging start of CAN in the notification of eSSLoggerCANEventStart to
  ///       Subscribers.
  /// \~english @see  StartCANLogging, RegisterLoggingFinishNotification
  ////////////////////////////////////////////////////////////////////////////////////
    EFrameworkunifiedStatus RegisterLoggingStartNotification(CbFuncPtr f_pCallBackFn);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup RegisterLoggingFinishNotification
  /// \~english @par Summary
  ///       API to set a callback function for CAN logging finish notification.
  /// \~english @param [in] f_pCallBackFn
  ///        CbFuncPtr   - Callback function pointer for CAN logging finish notification
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc.) has been
  ///       done.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - Callback function pointer specified by the argument is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - HANDLE (m_hApp) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldParam]
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE (m_hSession) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Handle name of the session message queue is NULL to SS_LoggerService. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message queue HANDLE of the session message to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message queue HANDLE of the session message to SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - It is not appropriate message queue name of the session message to SS_LoggerService(The name is NULL, more
  ///       than 20byte). [eFrameworkunifiedStatusInvldHandle]
  ///       - Is not a transmission type message queue type of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldHndlType]
  ///       - Session message queue is full to SS_LoggerService. [eFrameworkunifiedStatusMsgQFull]
  ///       - It is invalid transmission file descriptor of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message for
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - Incorrect size of the transmit buffer of the session message to SS_LoggerService. [eFrameworkunifiedStatusInvldBufSize]
  ///       - Any errors occur during the transmission of a session message to SS_LoggerService. [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message for SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Pub-Sub x Pub-Sub
  /// \~english @par Detail
  ///       Which is a method to register a callback function to receive a notification of the finish of the logging
  ///       operation.\n
  ///       (Using API:FrameworkunifiedSubscribeToSessionEventWithCallback of Native Service, to register a Callback function for CAN
  ///       logging finish notification.) \n
  ///       SS_LoggerService publish the logging finish of CAN in the notification of eSSLoggerCANEventFinished to
  ///       Subscribers.
  /// \~english @see  StartCANLogging, RegisterLoggingStartNotification
  ////////////////////////////////////////////////////////////////////////////////////
    EFrameworkunifiedStatus RegisterLoggingFinishNotification(CbFuncPtr f_pCallBackFn);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup StartDTCLoggingToEmmc
  /// \~english @par Summary
  ///       API to request the start of CAN logging to SS_LoggerService.
  /// \~english @param  None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Fire and Forget
  /// \~english @par Detail
  ///      Which is a method to request the start of CAN logging to SS_LoggerService. \n
  ///      This function is called by CAN at the time of the trigger of the error that occurred in the DTC. \n
  ///      Progress is published by notifications eSSLoggerCANEventStart, eSSLoggerCANEventError and
  ///      eSSLoggerCANEventFinished. \n
  ///      To send a message in the command ID of eSSLoggerCANProtocolIDDTCTrigger against SS_LoggerService. \n
  ///      SS_LoggerService sends a message in the command ID of eThrdCmdImmPersistEvtLog against the event log writing
  ///      thread. \n
  ///      Event log writing thread will immediately start with respect to the event log for Persistent.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus StartDTCLoggingToEmmc(UI_32 f_dtc);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ClearEventLogs
  /// \~english @par Summary
  ///       API to request to clear the event log to SS_LoggerService.
  /// \~english @param  None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Pub-Sub
  /// \~english @par Detail
  ///      Which is a method to request to clear the event log to SS_LoggerService. \n
  ///      To send a message in the command ID of SS_LOGGERCLEAREVENT against SS_LoggerService. \n
  ///      SS_LoggerService sends a message in the command ID of eThrdCmdClearEvntLogs against the event log writing
  ///      thread. \n
  ///      Event log write thread, delete all the event logs that are queued in the log write waiting. \n
  ///      As a result of the clearing process of the event log, the message is notified to the caller of the API in the
  ///      command ID below.
  ///       - Success: SS_LOGGERCLEAREVENT_SUCCESS_RESP
  ///       - Failure: SS_LOGGERCLEAREVENT_ERROR_RESP
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ClearEventLogs(void);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ReadStatisticalCounter
  /// \~english @par Summary
  ///       API to request a statistics counter reading of the event log to SS_LoggerService.
  /// \~english @param [in] eCounterGroup
  ///        EStatCounterGroupID   - Group ID of the statistics counter
  /// \~english @par
  ///            EStatCounterGroupID   enum value
  /// \~english @code
  ///            typedef enum  _ECounterGroupID
  ///            {
  ///                  STARTUP_SHUTDOWN_COUNTER_GROUP =0x01, /* Statistics counter group for StartUp/Shutdown    */
  ///                  APP_USAGE_COUNTER_GROUP,              /* Statistics counter group for application usage   */
  ///                  F_BLK_STABILITY_COUNTER_GROUP,        /* Do not use : Statistics counter for flash access */
  ///                  MAX_COUNTER_GROUP                     /* Max value of statistics counter group            */
  ///            }EStatCounterGroupID ;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Method x Fire and Forget
  /// \~english @par Detail
  ///      Against SS_LoggerService, which is a method that requires a reading of the statistics counter corresponding
  ///      to the statistics counter groups in the event log. \n
  ///      To send a message in the command ID of SS_LOGGER_READ_STATL_COUNTER against SS_LoggerService. \n
  ///      SS_LoggerService sends a message in the command ID of eThrdCmdStatisticalCounter against the event log
  ///      writing thread. \n
  ///      Event log write thread read the statistics counter corresponding to the statistics counter groups in the
  ///      event log.  \n
  ///      As a result of the clearing process of the event log, the message is notified to the caller of the API in the
  ///      command ID below.
  ///       - Success: SS_LOGGER_READ_STATL_COUNTER_SUCCESS_RESP
  ///       - Failure: SS_LOGGER_READ_STATL_COUNTER_ERROR_RESP
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ReadStatisticalCounter(EStatCounterGroupID eCounterGroup);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ResetStatisticalCounter
  /// \~english @par Summary
  ///       API to request the reset of statistics counters of the event log to SS_LoggerService.
  /// \~english @param [in] eCounterGroup
  ///        EStatCounterGroupID   - Group ID of the statistics counter
  /// \~english @par
  ///            EStatCounterGroupID   enum value
  /// \~english @code
  ///            typedef enum  _ECounterGroupID
  ///            {
  ///                  STARTUP_SHUTDOWN_COUNTER_GROUP =0x01, /* Statistics counter group for StartUp/Shutdown    */
  ///                  APP_USAGE_COUNTER_GROUP,              /* Statistics counter group for application usage   */
  ///                  F_BLK_STABILITY_COUNTER_GROUP,        /* Do not use : Statistics counter for flash access */
  ///                  MAX_COUNTER_GROUP                     /* Max value of statistics counter group            */
  ///            }EStatCounterGroupID ;
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Fire and Forget
  /// \~english @par Detail
  ///      Against SS_LoggerService, which is a method to reset the statistics counters corresponding to the statistics
  ///      counter groups in the event log. \n
  ///      To send a message in the command ID of SS_LOGGER_READ_STATL_COUNTER against SS_LoggerService. \n
  ///      SS_LoggerService sends a message in the command ID of eThrdCmdStatisticalCounter against the event log
  ///      writing thread. \n
  ///      Event log write thread reset the statistics counter corresponding to the statistics counter groups in the
  ///      event log.  \n
  ///      As a result of the clearing process of the event log, the message is notified to the caller of the API in the
  ///      command ID below.
  ///       - Success: SS_LOGGER_RESET_STATL_COUNTER_SUCCESS_RESP
  ///       - Failure: SS_LOGGER_RESET_STATL_COUNTER_ERROR_RESP
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ResetStatisticalCounter(EStatCounterGroupID eCounterGroup);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LoggerIf_SetDiagID
  /// \~english @par Summary
  ///       API to send the diagnosis ID to SS_LoggerService.
  /// \~english @param  [in] f_u16DiagID
  ///        UI_16   - Diagnosis ID
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget x Sync
  /// \~english @par Detail
  ///       It is a method to send the diagnosis ID to SS_LoggerService. \n
  ///       Send a message in the command ID of SS_LOGGER_SET_DIAGID to SS_LoggerService. \n
  ///       SS_LoggerService sets the diagnosis ID as a part of the suffix name of the event log storage file name.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus LoggerIf_SetDiagID(UI_16 f_u16DiagID);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LoggerIf_UploadEventLog
  /// \~english @par Summary
  ///       API to request the upload of the event log to SS_LoggerService.
  /// \~english @param  None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_LoggerService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop) has been
  ///       done, and Availability of SS_LoggerService is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusNullPointer]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_LoggerService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_LoggerService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_LoggerService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_LoggerService. [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_LoggerService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_LoggerService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Method x Fire and Forget
  /// \~english @par Detail
  ///       Which is a method to request the upload of the event log to SS_LoggerService. \n
  ///       Send a message in the command ID of SS_LOGGER_UPLOAD_EVENTLOG to SS_LoggerService. \n
  ///       SS_LoggerService sends a message in the command ID of eThrdCmdUploadEventLog against the event log writing
  ///       thread. \n
  ///       Event log write thread upload the event log in the interior of the buffer. \n
  ///       Upload a result, the message is notified in the command ID of SS_LOGGER_UPLOAD_EVENTLOG_RES to the caller of
  ///       the API.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus LoggerIf_UploadEventLog();

 private:
  HANDLE OpenService();
  EFrameworkunifiedStatus SendMessage(UI_32 f_cmdID, UI_32 f_size, void* f_pData);

  HANDLE   m_hApp;
  HANDLE  m_hService;
  HANDLE   m_hSession;
};

#endif /* SS_LOGGER_SERVICE_IFC_H_ */  // NOLINT (build/header_guard)

/** @}*/  // end of logger_service
/** @}*/  // end of SystemService
/** @}*/  // end of BaseSystem
