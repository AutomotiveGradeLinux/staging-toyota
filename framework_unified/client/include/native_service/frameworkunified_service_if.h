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

////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_HMI_CWORD77_Controller
/// \brief
///
///
///
////////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_service_if.h
 * @brief \~english This is base class for service interface classes
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
/** @addtogroup framework
 *  @ingroup native_service
 *  @{
 */
#ifndef __CSERVICESESSION_H__  // NOLINT  (build/header_guard)
#define __CSERVICESESSION_H__
#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/frameworkunified_framework_if.h>
#include <string>

///////////////////////////////////////////////////////////////////////////
/// class: CServiceSession
/// Description: This is base class for service interface classes
///
////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedServiceInterface {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedServiceInterface
  /// \~english @par Brief
  ///        Constructor for CFrameworkunifiedServiceInterface
  /// \~english @retval none
  /// \~english @par Preconditions
  ///           no preconditions
  /// \~english @par Change of internal status
  ///           none
  /// \~english @par Conditions of processing failure
  ///           none
  /// \~english @par Detail
  ///           create CFrameworkunifiedServiceInterface class instance
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see ~CFrameworkunifiedServiceInterface
  ////////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedServiceInterface();

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedServiceInterface
  /// \~english @par Brief
  ///        Destructor for CFrameworkunifiedServiceInterface
  /// \~english @retval none
  /// \~english @par Preconditons
  ///                CFrameworkunifiedServiceInterface is decleared in constructor
  /// \~english @par Change of internal status
  ///                none
  /// \~english @par Conditions of processing failure
  ///                none
  /// \~english @par Detail
  ///                Release CFrameworkunifiedServiceInterface instance
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///                none
  /// \~english @see CFrameworkunifiedServiceInterface
  ////////////////////////////////////////////////////////////////////////////////////////////
  ~CFrameworkunifiedServiceInterface();

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedServiceInterface
  /// \~english @par Brief
  ///          API to open Service.
  /// \~english @param [in] f_hApp
  ///           HANDLE* - Handle of framework
  /// \~english @param [in] f_sServiceName
  ///           std::string - Service Name
  /// \~english @retval HANDLE
  /// \~english @par Preconditions
  ///        -  no preconditions
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///        - none
  /// \~english @par Detail
  ///           Open service by specified name and retrun handle of service. \n
  ///           If FrameworkunifiedOpenService failed, handle is NULL.\n
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see FrameworkunifiedOpenService
  ////////////////////////////////////////////////////////////////////////////////////////////
  HANDLE OpenService(const HANDLE f_hApp, std::string f_sServiceName);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedServiceInterface
  /// \~english @par Brief
  ///          API to close service.
  /// \~english @param [in] f_hApp
  ///           HANDLE* - Handle of framework
  /// \~english @param [in] f_hService
  ///           HANDLE - Handle of the service
  /// \~english @retval EFrameworkunifiedStatus
  ///           Success or Error
  /// \~english @par Preconditons
  ///        -  none
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///           - If parameter f_hApp is NULL. [eFrameworkunifiedStatusInvldParam]
  ///           - If parameter f_hService is NULL. [eFrameworkunifiedStatusInvldParam]
  ///           - If FrameworkunifiedCloseService failed.       [eFrameworkunifiedStatusInvldHandle]
  /// \~english @par Detail
  ///           close service by handle \n
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see FrameworkunifiedCloseService
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CloseService(const HANDLE f_hApp, const HANDLE f_hService);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedServiceInterface
  /// \~english @par Brief
  ///          API to open session
  /// \~english @param [in] f_hApp
  ///           HANDLE* -framework handle
  /// \~english @param [in] f_uisessionType
  ///           UI_32 - sessiontype
  /// \~english @retval EFrameworkunifiedStatus
  ///           Success or Error
  /// \~english @par Preconditons
  ///        -  none
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///           - If parameter f_hApp is NULL. [eFrameworkunifiedStatusInvldParam]
  ///           - If FrameworkunifiedOpenSessionWithData failed.   [eFrameworkunifiedStatusInvldHandle]
  /// \~english @par Detail
  ///           open session by specified session type \n
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see FrameworkunifiedOpenSessionWithData
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OpenSession(const HANDLE f_hApp, UI_32 f_uisessionType);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedServiceInterface
  /// \~english @par Brief
  ///          API to open session Acknowledge
  /// \~english @param [in] f_hApp
  ///           HANDLE* - Handle of framework
  /// \~english @param [in] hSession
  ///          HANDLE - Session Handle
  /// \~english @retval EFrameworkunifiedStatus
  ///           Success or Error
  /// \~english @par Preconditons
  ///        -  none
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///           none
  /// \~english @par Detail
  ///           Empty implement.\n
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see one
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus OpenSessionAcknowledge(const HANDLE f_hApp, HANDLE &hSession);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedServiceInterface
  /// \~english @par Brief
  ///          API to close session.
  /// \~english @param [in] f_hService
  ///           HANDLE* - Handle of service
  /// \~english @param [in] hSession
  ///          HANDLE - Session Handle
  /// \~english @retval EFrameworkunifiedStatus
  ///           Success or Error
  /// \~english @par Preconditons
  ///        -  none
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///           - If parameter f_hService is NULL. [eFrameworkunifiedStatusInvldParam]
  ///           - If parameter f_hSession is NULL. [eFrameworkunifiedStatusInvldParam]
  ///           - If FrameworkunifiedCloseSession failed.   [eFrameworkunifiedStatusInvldHandle]
  /// \~english @par Detail
  ///           close session of specified service. \n
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see FrameworkunifiedCloseSession
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CloseSession(const HANDLE f_hService, const HANDLE f_hSession);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedServiceInterface
  /// \~english @par Brief
  ///          API to close session.
  /// \~english @param [in] f_hService
  ///           HANDLE* - Handle of service
  /// \~english @param [in] hSession
  ///          HANDLE - Session Handle
  /// \~english @retval EFrameworkunifiedStatus
  ///           Success or Error
  /// \~english @par Preconditons
  ///        -  none
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///           - If parameter hClient is NULL. [eFrameworkunifiedStatusNullPointer]
  ///           - If FrameworkunifiedGetMsgDataOfSize failed.   [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///           Check close session acknowledge.\n
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          sync only
  /// \~english @see FrameworkunifiedGetMsgDataOfSize
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CloseSessionAcknowledge(const HANDLE hClient);
};
#endif  // __CSERVICESESSION_H__  NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
