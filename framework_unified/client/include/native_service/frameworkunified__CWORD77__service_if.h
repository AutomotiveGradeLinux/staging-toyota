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

////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_HMI_CWORD77_Controller
/// \brief
///
////////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified__CWORD77__service_if.h
 * @brief \~english This file contains a interface class for all services classes.
 *
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */
/** @addtogroup _CWORD77_
 *  @ingroup framework_unified
 *  @{
 */
#ifndef _FRAMEWORKUNIFIED__CWORD77_SERVICE_H_  // NOLINT  (build/header_guard)
#define _FRAMEWORKUNIFIED__CWORD77_SERVICE_H_

#include <native_service/ns__CWORD77__common.h>
#include <native_service/frameworkunified__CWORD77__session_if.h>
#include <string>
#include <map>
#include <vector>

///////////////////////////////////////////////////////////////////////////
/// class: CFrameworkunified_CWORD77_Service
/// Description: This is interface class for all services classes.
///
////////////////////////////////////////////////////////////////////////////

class CFrameworkunified_CWORD77_Service {
 protected:
  std::map<UI_32, CFrameworkunified_CWORD77_Session *> m_SessTypeToSessObj;  ///< Map of session type to session object
  ResponseServiceTo_CWORD77_ m_cbResponse;  ///< Call back function pointer
  SessionAckTo_CWORD77_ m_cbSessionACK;  // Call back function pointer for session ACK
  std::vector<FrameworkunifiedNotificationCallbackHandler> m_vServiceNotifications;  ///< vector of Frameworkunifiednotification callbacks
  CFrameworkunified_CWORD77_Session *m_pSession;       ///< Pointer to an instance of I_CWORD77_Session
  std::string m_cServiceName;
  BOOL m_bServiceAvailable;
  UI_8 m_ui8SessionRetryCount;
  HANDLE m_hApp;  ///> application framework handle
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        Function to send message to a session
  /// \~english @param[in] f_uiSessionType
  ///        UI_32 - session type
  /// \~english @param[in] f_uiSrvProtocol
  ///        UI_32 - Protocol ID
  /// \~english @retval none
  /// \~english @par Preconditons
  ///        CFrameworkunified_CWORD77_Service::SetSession()
  /// \~english @par Change of internal status
  ///        -  The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///          Send specified sessionType message by FrameworkunifiedSendMsg().
  /// \~english @par
  ///          If the session object corresponding to the session Public specified by the arguments is not registered,
  ///          the system terminates normally without sending the object.\n
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see SetSession,FrameworkunifiedSendMsg
  ////////////////////////////////////////////////////////////////////////////////////////////
  void SendMessageToSession(UI_32 f_uiSessionType, UI_32 f_uiSrvProtocol);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        Function to open sessions with specified Service Name
  /// \~english @param [in] f_hApp
  ///           f_hApp - Handle to the Framework
  /// \~english @par
  ///           f_hApp   HANDLE*
  /// \~english @param [in] f_sServiceName
  ///           f_sServiceName  - Service Name
  /// \~english @par
  ///           f_sServiceName - const std::string&
  /// \~english @retval none
  /// \~english @par Preconditons
  ///        - none
  /// \~english @par Change of internal status
  ///        -  Save service name into member variable.
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///          Function to open sessions with specified Service Name
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////
  void OpenSession(HANDLE f_hApp, const std::string &f_sServiceName);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        Function to close sessions
  /// \~english @param [in] f_hApp
  ///           f_hApp - Handle to the Framework
  /// \~english @par
  ///           f_hApp   HANDLE*
  /// \~english @retval none
  /// \~english @par Preconditons
  ///        - none
  /// \~english @par Change of internal status
  ///        -   The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///          Function to close sessions
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////

  void CloseSession(HANDLE f_hApp);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        Function to add notification
  /// \~english @param[in] f_pNotification
  ///        PCSTR - Notification name
  /// \~english @retval none
  /// \~english @par Preconditons
  ///        - Session obeject is declared in CFrameworkunified_CWORD77_Service::SetSession()
  /// \~english @par Change of internal status
  ///        - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///        - none
  /// \~english @par Detail
  ///        -Bind Notification name declared in parameter with callbak function in OnServiceNtf()
  /// \~english @par Detail
  ///        -subscribed notifications will transfor into subscribe instance in SubscribeNotifications()
  /// \~english @par Detail
  ///        -Process will not commit when notification parameter name is null
  /// \~english @par Classification
  ///        -public
  /// \~english @par Type
  ///        -None
  /// \~english @see SetSession,OnServiceNtf,SubscribeNotifications,UnSubscribeNotifications
  ////////////////////////////////////////////////////////////////////////////////////////////
  void AddNotification(PCSTR f_pNotification);

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   AttachOpenSessionAckCallback
  /// virtual function to attche open session ack callback
  ///
  /// \param  [in] hApp
  ///     HANDLE - Handle to the Framework
  /// \return status
  ///     EFrameworkunifiedStatus - Success or Error
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus AttachOpenSessionAckCallback(HANDLE hApp) = 0;

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// OpenSessionRetry
  /// This function sends open session request to the service m_cServiceName
  ///
  /// \param  [in] f_ui32SessionType
  ///     UI_32 - Session type to open
  /// \return none
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////

  void OpenSessionRetry(UI_32 f_ui32SessionType);

 public:
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        Constructor for CFrameworkunified_CWORD77_Service
  /// \~english @retval none
  /// \~english @par Preconditions
  ///        -No preconditions
  /// \~english @par Change of internal status
  ///        -The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///        -None
  /// \~english @par Detail
  ///        -Create CFrameworkunified_CWORD77_Service class instance
  ///        -Initialize class member variable(m_cbResponse, m_cbSessionACK, m_pSession, m_cServiceName,\n
  ///         m_bServiceAvailable, m_ui8SessionRetryCount, m_hApp, m_SessTypeToSessObj).\n
  /// \~english @par Classification
  ///        -public
  /// \~english @par type
  ///        -None
  /// \~english @see ~CFrameworkunified_CWORD77_Service
  ////////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunified_CWORD77_Service();

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        Destructor for CFrameworkunified_CWORD77_Service
  /// \~english @retval none
  /// \~english @par Preconditons
  ///         -CFrameworkunified_CWORD77_Service instance is decleared in Constructor
  /// \~english @par Change of internal status
  ///         -The internal status is not changed
  /// \~english @par Conditions of processing failure
  ///         -None
  /// \~english @par Detail
  ///         -Release the CFrameworkunified_CWORD77_Service class instance
  /// \~english @par Classification
  ///         -Pubulic
  /// \~english @par Type
  ///         -None
  /// \~english @see CFrameworkunified_CWORD77_Service
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunified_CWORD77_Service();


  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        HandleMessage
  ///        virtual function to process message coming from _CWORD77_
  /// \~english @param [in] cmdID
  ///           cmdID - Request ID
  /// \~english @par
  ///           cmdID   UI_32
  /// \~english @retval EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK or Error
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        -   The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///        -  none
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus HandleServiceMessage(UI_32 cmdID) = 0;


  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        function to open session with specified Service name when service available.
  /// \~english @param [in] f_hApp
  ///           f_hApp - Handle to the Framework
  /// \~english @par
  ///           f_hApp   HANDLE*
  /// \~english @param [in] f_sServiceName
  ///           f_sServiceName - service name
  /// \~english @par
  ///           f_sServiceName -  const std::string &
  /// \~english @retval EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK or Error
  /// \~english @par Preconditions
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        -   save Framework handler into m_hApp.
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///           function to open session with specified Service name
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see AttachOpenSessionAckCallback, OpenSession
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OpenServiceOnAvailability(HANDLE f_hApp, const std::string &f_sServiceName);


  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        function to close session when service unavailable.
  /// \~english @param [in] f_hApp
  ///           f_hApp - Handle to the Framework
  /// \~english @par
  ///           f_hApp   HANDLE*
  /// \~english @retval EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK or Error
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        -   set m_bServiceAvailable FALSE.
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///          function to close session when service unavailable.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see CloseSession
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CloseServiceOnUnavailability(HANDLE hApp);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        Set Call back function into Sessions.
  /// \~english @param [in] fptr
  ///           fptr - Pointer to call back function
  /// \~english @par
  ///           fptr   ResponseServiceTo_CWORD77_*
  /// \~english @retval
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        -   set m_cbResponse with fptr.
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///         Set Call back function into Sessions.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////
  void SetResponseCallback(ResponseServiceTo_CWORD77_ fptr);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        API to set session objects in service.
  /// \~english @param [in] f_pSessionObj
  ///           f_pSessionObj - Pointer to an instance of session object.
  /// \~english @par
  ///           f_pSessionObj   CFrameworkunified_CWORD77_Session*
  /// \~english @retval
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        - set sessionobj into m_SessTypeToSessObj.
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///         Set session objects in service.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////
  void SetSession(CFrameworkunified_CWORD77_Session *f_pSessionObj);


  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        Subscribe notifications.
  /// \~english @param [in] hApp
  ///           hApp - Handle to the Framework.
  /// \~english @par
  ///           hApp   HANDLE*
  /// \~english @retval EFrameworkunifiedStatus
  ///                 eFrameworkunifiedStatusOK or eFrameworkunifiedStatusInvldParam
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        -    save pointer of notifications into framework handler.
  /// \~english @par Conditions of processing failure
  ///        -  FrameworkunifiedSubscribeNotificationWithCallback() failed [eFrameworkunifiedStatusInvldParam]
  /// \~english @par Detail
  ///         Send subscribe message to NPPService for Subscribe notifications.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see FrameworkunifiedSubscribeNotificationWithCallback
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SubscribeNotifications(HANDLE hApp);


  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        unsubscribe notifications
  /// \~english @param [in] hApp
  ///           hApp - Handle to the Framework.
  /// \~english @par
  ///           hApp   HANDLE*
  /// \~english @retval EFrameworkunifiedStatus
  ///                 eFrameworkunifiedStatusOK or eFrameworkunifiedStatusInvldParam
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        -    delete notifications from framework handler.
  /// \~english @par Conditions of processing failure
  ///        -  FrameworkunifiedNPUnsubscribeFromNotifications() failed [eFrameworkunifiedStatusInvldParam]
  /// \~english @par Detail
  ///         Send unsubscribe message to NPPService for delete subscribe notifications.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see FrameworkunifiedSubscribeNotificationWithCallback
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus UnSubscribeNotifications(HANDLE hApp);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        virtual function to enable session in service
  /// \~english @param [in] f_uiNum
  ///           f_uiNum - Number of arguments
  /// \~english @par
  ///           f_uiNum   UI_32
  /// \~english @param [in] f_uiFirst
  ///           f_uiFirst - first argument
  /// \~english @par
  ///           f_uiFirst   UI_32
  /// \~english @retval None
  /// \~english @par Preconditons
  ///        -
  /// \~english @par Change of internal status
  ///        -   The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///          None
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          None
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual void EnableSession(UI_32 f_uiNum, UI_32 f_uiFirst, ...) = 0;

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        unsubscribe notifications
  /// \~english @param [in] hApp
  ///           hApp - Handle to the Framework.
  /// \~english @par
  ///           hApp   HANDLE*
  /// \~english @retval EFrameworkunifiedStatus
  ///                 eFrameworkunifiedStatusOK or eFrameworkunifiedStatusNullPointer
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        - none.
  /// \~english @par Conditions of processing failure
  ///        -  FrameworkunifiedGetMsgDataOfSize() failed [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///         Call register subscribe notification.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see FrameworkunifiedGetMsgDataOfSize, SetRespNotfnDataIn_CWORD77_DataPool
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OnServiceNtf(HANDLE hApp);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        Acknowledgement process of open session
  /// \~english @param[in] f_hApp
  ///        HANDLE - Handle to the Application
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           - Set Session object in CFrameworkunified_CWORD77_Service::SetSession()
  ///           - Set Callback function in CFrameworkunified_CWORD77_Service::Set_CWORD77_OpenSessionACK()
  /// \~english @par Change of internal status
  ///           none
  /// \~english @par Conditions of processing failure
  ///           none
  /// \~english @par Detail
  ///           Set the application handle as Parameter when receiving acknowledgement for open session process.
  ///           Call OpenSessionAcknowledge() in session object which has same session type.
  ///           Call the callback function for open session acknowledgement.
  /// \~english @par
  ///           Message sending process will be stoped in reguler when the session type of open session process is not
  ///           declared.
  /// \~english @par
  ///           Callback function will be stoped in reguler when callback function is not declared.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see SetSession,Set_CWORD77_OpenSessionACK
  ////////////////////////////////////////////////////////////////////////////////////////////
  void OpenSessionAcks(HANDLE f_hApp);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Service
  /// \~english @par Brief
  ///        Attach open session ack callback function to service.
  /// \~english @param [in] fptr
  ///           fptr - Callback Function pointer of notify open session ack.
  /// \~english @par
  ///           fptr   SessionAckTo_CWORD77_*
  /// \~english @retval
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        -  set callback function into m_cbSessionACK..
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///         Attach open session ack callback function to service.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////
  void Set_CWORD77_OpenSessionACK(SessionAckTo_CWORD77_ fptr);
};

#endif  // _FRAMEWORKUNIFIED__CWORD77_SERVICE_H_  NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
