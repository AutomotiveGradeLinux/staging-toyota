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
/// \ingroup  tag_NSFramework
/// \brief
///
/// This file has the declaration of class CFrameworkunifiedShallowHistoryState . CFrameworkunifiedShallowHistoryState is
/// derived from CFrameworkunifiedHistoryState class.This class implements the additional functionality supported
/// by HSM Shallow History state.
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_shallowhistorystate.h
 * @brief \~english This file has the declaration of class CFrameworkunifiedShallowHistoryState . CFrameworkunifiedShallowHistoryState is
 *        derived from CFrameworkunifiedHistoryState class.This class implements the additional functionality supported
 *        by HSM Shallow History state.
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
 *  @ingroup framework_unified
 *  @{
 */
/** @addtogroup statemachine
 *  @ingroup framework
 *  @{
 */
#ifndef __FRAMEWORKUNIFIED_SM_SHALLOWHISTORYSTATE_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_SM_SHALLOWHISTORYSTATE_H__

#include <native_service/frameworkunified_sm_historystate.h>
#include <string>
///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class implements the additional functionality supported by HSM Shallow History state.
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedShallowHistoryState : public CFrameworkunifiedHistoryState {
 public :
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedShallowHistoryState
  /// \~english @par Brief
  ///        CFrameworkunifiedShallowHistoryState constructor
  /// \~english @param [in] f_pName
  ///        std::string   - Name of the state
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        - None
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///        - None
  /// \~english @par Detail
  ///       This class inherits from CFrameworkunifiedHistoryState class and creates CFrameworkunifiedShallowHistoryState object. \n
  ///       In this function, we set member variable m_uiEventId to ShallowHistoryID(71), \n
  ///       and set member variable m_cEventName to evShallowHistory"evShallowHistory".
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see ~CFrameworkunifiedShallowHistoryState
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// CFrameworkunifiedShallowHistoryState
  /// Parameterized constructor
  /// \param [in] f_pName
  ///     string - Name of the state
  ///
  /// \return none
  CFrameworkunifiedShallowHistoryState(std::string f_pName);  // NOLINT  (readability/nolint)

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedShallowHistoryState
  /// \~english @par Brief
  ///        CFrameworkunifiedShallowHistoryState destructor
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        - Calls constructor to successfully create object.
  /// \~english @par Change of internal state
  ///        - None
  /// \~english @par Conditions of processing failure
  ///        - None
  /// \~english @par Detail
  ///       Destory CFrameworkunifiedShallowHistoryState object.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see CFrameworkunifiedShallowHistoryState::CFrameworkunifiedShallowHistoryState
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFrameworkunifiedShallowHistoryState
  /// Class destructor
  ///
  /// \return none
  ~CFrameworkunifiedShallowHistoryState();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedShallowHistoryState
  /// \~english @par Brief
  ///        update history infomation
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK   update history infomation success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  exception occur when update history infomation
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        m_pParentState is NULL
  /// \~english @par Detail
  ///        update last active state, failed if m_pParentState is NULL
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// UpdateHistory
  /// This function stores the last active state
  /// \param
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - Returns status of operation
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus UpdateHistory();

 private:
  static const UI_32 m_suievShallowHistory;
};

#endif /* __FRAMEWORKUNIFIED_SM_SHALLOWHISTORYSTATE_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
