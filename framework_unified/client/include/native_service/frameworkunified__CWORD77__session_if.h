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
 * @file frameworkunified__CWORD77__session_if.h
 * @brief \~english This file provides API for operating session
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
#ifndef _FRAMEWORKUNIFIED__CWORD77_SESSION_H_  // NOLINT  (build/header_guard)
#define _FRAMEWORKUNIFIED__CWORD77_SESSION_H_

#include <native_service/ns__CWORD77__common.h>
#include <string>
#include <vector>

class CFrameworkunified_CWORD77_Session {
 protected:
  HANDLE m_hSession;    ///< Handle to session
  HANDLE m_hApp;      ///< Handle to Framework
  HANDLE m_hService;    ///< Handle to Service
  UI_32  m_uiSessionType;  ///< Session Type
  ResponseServiceTo_CWORD77_ m_ResponseTo_CWORD77_;   ///< Call back function pointer
  std::vector<FrameworkunifiedProtocolCallbackHandler> m_vSessionProtocolHanlder;
  std::vector<UI_32> m_vEventVector;
  std::vector<FrameworkunifiedProtocolCallbackHandler> m_vSessionEventHandler;
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   AddProtocol
  /// Function to add notification
  ///
  /// \param[in]  f_uiCmdId
  ///     UI_32 - Command id
  /// \param[in]  f_cbFuncPtr
  ///     CbFuncPtr - Callback function pointer
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  void AddProtocolHandler(UI_32 f_uiCmdId, CbFuncPtr f_cbFuncPtr);
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   AttachProtocolwithDispatcher
  /// API to Register Session 1 of Service A protocols with Dispatcher
  ///
  /// \param  [in] f_hApp
  ///     HANDLE - Handle to the Framework
  /// \return status
  ///     EFrameworkunifiedStatus - Success or Error
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus AttachProtocolwithDispatcher(HANDLE f_hApp);
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   DetachProtocolwithDispatcher
  /// API to Un Register Session 1 of Service A protocols with Dispatcher
  ///
  /// \param  [in] f_hApp
  ///     HANDLE - Handle to the Framework
  /// \return status
  ///     EFrameworkunifiedStatus - Success or Error
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus DetachProtocolwithDispatcher(HANDLE f_hApp);
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   AttachEventswithDispatcher
  /// Function to register events with dispatcher
  ///
  /// \param  [in] f_hApp
  ///     HANDLE - Handle to the Framework
  /// \return status
  ///     EFrameworkunifiedStatus - Success or Error
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus AttachEventswithDispatcher(HANDLE f_hApp);
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   RegisterEvents
  /// Function to register events
  ///
  /// \param  [in] f_hApp
  ///     HANDLE - Handle to the Framework
  /// \return status
  ///     EFrameworkunifiedStatus - Success or Error
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus RegisterEvents(HANDLE f_hApp);  // Function: added by Shyam to overcome
  /// the naming conflict in registering the event

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   AttachEventHandlerswithDispatcher
  /// API to Register Session 1 of Service A protocols with Dispatcher
  ///
  /// \param  [in] f_hApp
  ///     HANDLE - Handle to the Framework
  /// \return status
  ///     EFrameworkunifiedStatus - Success or Error
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus AttachEventHandlerswithDispatcher(HANDLE f_hApp);
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   DetachEventHandlerswithDispatcher
  /// API to Un Register Session 1 of Service A protocols with Dispatcher
  ///
  /// \param  [in] f_hApp
  ///     HANDLE - Handle to the Framework
  /// \return status
  ///     EFrameworkunifiedStatus - Success or Error
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus DetachEventHandlerswithDispatcher(HANDLE f_hApp);
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   AddEventHandler
  /// Function to add event handler
  ///
  /// \param[in]  f_uiCmdId
  ///     UI_32 - Command id
  /// \param[in]  f_cbFuncPtr
  ///     CbFuncPtr - Callback function pointer
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  void AddEventHandler(UI_32 f_uiCmdId, CbFuncPtr f_cbFuncPtr);

 public:
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Session
  /// \~english @par Brief
  ///        Constructor for CFrameworkunified_CWORD77_Session
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           no preconditions
  /// \~english @par Change of internal status
  ///           none
  /// \~english @par Conditions of processing failure
  ///           none
  /// \~english @par Detail
  ///           Create CFrameworkunified_CWORD77_Session instance and initialize the member parameter(m_hSession, m_hApp,m_hService,\n
  ///           m_uiSessionType)
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see ~CFrameworkunified_CWORD77_Session
  ////////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunified_CWORD77_Session();

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Session
  /// \~english @par Brief
  ///        Destructor for CFrameworkunified_CWORD77_Session
  /// \~english @retval none
  /// \~english @par Preconditons
  ///                CFrameworkunified_CWORD77_Session is decleared in constructor
  /// \~english @par Change of internal status
  ///                none
  /// \~english @par Conditions of processing failure
  ///                none
  /// \~english @par Detail
  ///                Release CFrameworkunified_CWORD77_Session class instance
  /// \~english @par Classification
  ///                public
  /// \~english @par Type
  ///                none
  /// \~english @see CFrameworkunified_CWORD77_Session
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunified_CWORD77_Session();

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Session
  /// \~english @par Brief
  ///         send specific command by session
  /// \~english @param [in] cmdID
  ///           cmdID - commond id
  /// \~english @par
  ///           cmdID   UI_32
  /// \~english @retval EFrameworkunifiedStatus
  ///           eFrameworkunifiedStatusOK or failed
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///       -   GetReqDataFrom_CWORD77_DataPool() failed [eFrameworkunifiedStatusDbRecNotFound]
  ///       -   FrameworkunifiedSendMsg() failed
  ///       -   if m_hSession is NULL. [eFrameworkunifiedStatusSessionErr]
  /// \~english @par Detail
  ///         send specified commond by session.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see GetReqDataFrom_CWORD77_DataPool
  ////////////////////////////////////////////////////////////////////////////////////////////
  // virtual EFrameworkunifiedStatus HandleSessionMessage(UI_32 cmdID);
  EFrameworkunifiedStatus HandleSessionMessage(UI_32 cmdID);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Session
  /// \~english @par Brief
  ///           Set Call back function in Session
  /// \~english @param [in] fptr
  ///           fptr - Pointer to call back function
  /// \~english @par
  ///           fptr   ResponseServiceTo_CWORD77_*
  /// \~english @retval none
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        -  set callback into m_ResponseTo_CWORD77_.
  /// \~english @par Conditions of processing failure
  ///        - none
  /// \~english @par Detail
  ///          Set Call back function in Session
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////
  void SetResponseCallback(ResponseServiceTo_CWORD77_ fptr);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Session
  /// \~english @par Brief
  ///          function to Open Session.(pure virtual function)
  /// \~english @param [in] f_hApp
  ///           f_hApp - Handle of framework
  /// \~english @par
  ///           f_hApp   HANDLE*
  /// \~english @param [in] f_sServiceName
  ///           f_sServiceName - Service Name
  /// \~english @par
  ///           f_sServiceName   std::string
  /// \~english @retval EFrameworkunifiedStatus
  ///            Success or Error
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///        - none
  /// \~english @par Detail
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus OpenSession(HANDLE f_hApp, std::string f_sServiceName) = 0;

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Session
  /// \~english @par Brief
  ///          function to close Session.(pure virtual function)
  /// \~english @param [in] f_hApp
  ///           f_hApp - Handle of framework
  /// \~english @par
  ///           f_hApp   HANDLE*
  /// \~english @retval EFrameworkunifiedStatus
  ///            Success or Error
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///        -  none
  /// \~english @par Detail
  ///        - none
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus CloseSession(HANDLE f_hApp) = 0;

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Session
  /// \~english @par Brief
  ///          Open Session acknowledge.(pure virtual function)
  /// \~english @param [in] f_hApp
  ///           f_hApp - Handle of framework
  /// \~english @par
  ///           f_hApp   HANDLE*
  /// \~english @retval EFrameworkunifiedStatus
  ///            Success or Error
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///        - none
  /// \~english @par Detail
  ///        -none
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus OpenSessionAcknowledge(HANDLE f_hApp) = 0;
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetSessionType
  /// API to get session type
  ///
  /// \return UI_32 - Session type
  ///     EFrameworkunifiedStatus - Success or Error
  ////////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Session
  /// \~english @par Brief
  ///          API to get session type
  /// \~english @param none
  /// \~english @retval UI_32 - Session type
  /// \~english @par Preconditons
  ///        - no preconditions
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///        - none
  /// \~english @par Detail
  ///           API to get session type.\n
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////
  UI_32 GetSessionType();

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Session
  /// \~english @par Brief
  ///        API to set session type into class CFrameworkunified_CWORD77_Session object.
  /// \~english @param[in] sessioType
  ///        const UI_32& - Session type
  /// \~english @retval none
  /// \~english @par Preconditons
  ///            none
  /// \~english @par Change of internal status
  ///            none
  /// \~english @par Conditions of processing failure
  ///            none
  /// \~english @par Detail
  ///            API to set session type into class CFrameworkunified_CWORD77_Session object.
  /// \~english @par Classification
  ///            public
  /// \~english @par Type
  ///            sync only
  /// \~english @see
  ///            none
  ////////////////////////////////////////////////////////////////////////////////////////////
  void SetSessionType(const UI_32 &sessioType);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Session
  /// \~english @par Brief
  ///        declear callback funcion into Dispatcher
  /// \~english @param[in] hApp
  ///        HANDLE - Handle to Application
  /// \~english @retval none
  /// \~english @par Preconditons
  ///            no precondition
  /// \~english @par Change of internal status
  ///            none
  /// \~english @par Conditions of processing failure
  ///            none
  /// \~english @par Detail
  ///           declear OnResponse() of callback function by calling FrameworkunifiedAttachCallbackToDispatcher()\n
  ///           get decleared protocol ID through GetResponseProtocolArray()\n
  ///           get decleared event ID through GetEventArray() and regist event-sending instance in FrameworkunifiedRegisterEvent
  /// \~english @par
  ///           cautions in using this API:\n
  ///           -Registrition will fail without msg When Application handle is uncorrect set by parameter\n
  ///           -Registrition process in FrameworkunifiedAttachCallbackToDispatcher() will not commit when
  ///           GetResponseProtocolArray()\n
  ///            or GetEventArray() returns NULL or NULL list
  /// \~english @par Classification
  ///            public
  /// \~english @par Type
  ///            none
  /// \~english @see FrameworkunifiedAttachCallbackToDispatcher, FrameworkunifiedRegisterEvent,
  ///                GetResponseProtocolArray, GetEventArray, OnResponse
  ////////////////////////////////////////////////////////////////////////////////////////////
  void AttachResponseCallbacks(HANDLE hApp);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Session
  /// \~english @par Brief
  ///           unscrible dispatcher callback function
  /// \~english @param[in] hApp
  ///        HANDLE - Handle to Application
  /// \~english @retval none
  /// \~english @par Preconditons
  ///                no preconditions
  /// \~english @par Change of internal status
  ///                none
  /// \~english @par Conditions of processing failure
  ///                none
  /// \~english @par Detail
  ///           declear OnResponse() of callback function by calling FrameworkunifiedDetachCallbackFromDispatcher()\n
  ///           get unregistrition protocol ID through GetResponseProtocolArray()\n
  ///           get unregistrition event ID through GetEventArray() and unregist event-sending instance in \n
  ///           FrameworkunifiedUnRegisterEvent()
  /// \~english @par Detail
  ///            cautions in using this API:\n
  ///           -Unregistrition will fail without msg When Application handle is uncorrect set by parameter\n
  ///           -Unregistrition process in FrameworkunifiedDetachCallbackFromDispatcher() will not commit when
  ///           GetResponseProtocolArray()\n
  ///            or GetEventArray() returns NULL or NULL list
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see FrameworkunifiedDetachCallbackFromDispatcher, FrameworkunifiedUnRegisterEvent,
  ///                GetResponseProtocolArray, GetEventArray
  ////////////////////////////////////////////////////////////////////////////////////////////
  void DetachResponseCallbacks(HANDLE hApp);

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   OnResponse
  /// Callback function on receiving response protocol or event specified in the list, returned
  /// by either \ref GetResponseProtocolArray or \ref GetEventArray
  ///
  /// \param  [in] hApp
  ///     HANDLE - Handle to framework
  /// \return None
  ////////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunified_CWORD77_Session
  /// \~english @par Brief
  ///        Callback function on receiving response protocol or event specified in the list, returned
  ///            by either \ref GetResponseProtocolArray or \ref GetEventArray
  /// \~english @param [in] f_hApp
  ///           HANDLE - Handle of framework

  /// \~english @retval EFrameworkunifiedStatus
  ///            SuceFrameworkunifiedStatusOK or eFrameworkunifiedStatusNullPointer
  /// \~english @par Preconditions
  ///        -  no preconditions
  /// \~english @par Change of internal status
  ///        - framework handler is invalid handler.[eFrameworkunifiedStatusNullPointer]
  /// \~english @par Conditions of processing failure
  ///        - none
  /// \~english @par Detail
  ///        Callback function on receiving response protocol or event specified in the list, returned
  ///            by either \ref GetResponseProtocolArray or \ref GetEventArray
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see FrameworkunifiedGetMsgDataOfSize
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OnResponse(HANDLE hApp);

  // FIXME: Following set of functions should be pure virtual functions.
  // But, in order to avoid build errors for HMI__CWORD77_, it is defined as virtual functions only.
  // Because nobody who is currently deriving off of frameworkunified__CWORD77__session_if.h has these functions implemented.
  virtual UI_32 const *GetResponseProtocolArray() = 0;
  virtual UI_32 const GetResponseProtocolArrayLength() = 0;
  virtual UI_32 const *GetEventArray() = 0;
  virtual UI_32 const GetEventArrayLength() = 0;
};

#endif   // _FRAMEWORKUNIFIED__CWORD77_SESSION_H_  NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
