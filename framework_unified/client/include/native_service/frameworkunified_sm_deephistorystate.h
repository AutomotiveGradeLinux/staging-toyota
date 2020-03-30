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
/// This file has the declaration of class CFrameworkunifiedDeepHistoryState . CFrameworkunifiedDeepHistoryState is
/// derived from CFrameworkunifiedHistoryState class.This class implements the additional functionality supported
/// by HSM Deep History state.
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_deephistorystate.h
 * @brief \~english This file has the declaration of class CFrameworkunifiedDeepHistoryState.CFrameworkunifiedDeepHistoryState is
 *        derived from CFrameworkunifiedHistoryState class.This class implements the additional functionality supported
 *        by HSM Deep History state.
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
#ifndef __FRAMEWORKUNIFIED_SM_DEEPHISTORYSTATE_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_SM_DEEPHISTORYSTATE_H__

#include <native_service/frameworkunified_sm_historystate.h>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class implements the additional functionality supported by HSM Shallow History state.
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedDeepHistoryState : public CFrameworkunifiedHistoryState {
 public :

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedDeepHistoryState
  /// \~english @par Brief
  ///          Constructor for class CFrameworkunifiedDeepHistoryState.
  /// \~english @param [in] f_pName
  ///        std::string   - Name of the state
  /// \~english @retval
  /// \~english @par Prerequisite
  ///            none
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///            none
  /// \~english @par Detail
  ///           Inherite from base class CFrameworkunifiedHistoryState.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see CFrameworkunifiedHistoryState, ~CFrameworkunifiedDeepHistoryState
  ///////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedDeepHistoryState(std::string f_pName);  // NOLINT  (readability/nolint)

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedDeepHistoryState
  /// \~english @par Brief
  ///        Destructor for CFrameworkunifiedDeepHistoryState
  /// \~english @retval none
  /// \~english @par Preconditons
  ///          - Self instance of CFrameworkunifiedDeepHistoryState created.
  /// \~english @par Change of internal status
  ///                none
  /// \~english @par Conditions of processing failure
  ///                none
  /// \~english @par Detail
  ///          Class CFrameworkunifiedHistoryState instance and CFrameworkunifiedDeepHistoryState instance release.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///            none
  /// \~english @see CFrameworkunifiedDeepHistoryState::CFrameworkunifiedDeepHistoryState
  ///////////////////////////////////////////////////////////////////////////////////////////
  ~CFrameworkunifiedDeepHistoryState();

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedDeepHistoryState
  /// \~english @par Brief
  ///           This function stores the last active state
  /// \~english @param
  /// \~english @retval EFrameworkunifiedStatus     eFrameworkunifiedStatusOK
  ///                                   eFrameworkunifiedStatusNullPointer
  /// \~english @par Preconditons
  ///        -  Self instance of CFrameworkunifiedConditionConnector created.
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///            - If m_pParentState is NULL. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///            This function stores the last active state.\n
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus UpdateHistory();

 private:
  static const UI_32 m_suievDeepHistory;
};

#endif /* __FRAMEWORKUNIFIED_SM_DEEPHISTORYSTATE_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
