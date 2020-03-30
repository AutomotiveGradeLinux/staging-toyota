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
/// \ingroup  tag_StateMachine
/// \brief
///
/// This file has the CFrameworkunifiedTransition class definitions.This class provides the
/// interface for reacting local transition and external transition
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_transition.h
 * @brief \~english This file has the CFrameworkunifiedTransition class definitions.
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
 *  @ingroup framework_unified
 *  @{
 */
/** @addtogroup statemachine
 *  @ingroup framework
 *  @{
 */
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_TRANSITION_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_TRANSITION_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_reaction.h>

class CFrameworkunifiedState;
class CFrameworkunifiedEventData;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class provides the interface for reacting local transition and external transition
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedTransition : public CFrameworkunifiedReaction {
 public :

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedTransition
  /// \~english @par Summary
  ///       CFrameworkunifiedTransition constructor
  /// \~english @param [in] f_pTargetState
  ///        CFrameworkunifiedState *f_pTargetState  - set m_pTargetState value
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///      - m_pTargetState = f_pTargetState
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       CFrameworkunifiedTransition constructor, for create CFrameworkunifiedTransition object
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedTransition(CFrameworkunifiedState *f_pTargetState);  // NOLINT  (readability/nolint)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedTransition
  /// \~english @par Summary
  ///       CFrameworkunifiedTransition destructor
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///      None
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       CFrameworkunifiedTransition destructor, for destroy CFrameworkunifiedTransition object
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunifiedTransition();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedTransition
  /// \~english @par Summary
  ///       virtual fuction
  /// \~english @param [in] f_pSourceState
  ///        CFrameworkunifiedState *f_pSourceState  -  source state
  /// \~english @param [in] f_pData
  ///        CEventDataPtr f_pData  -  event data
  /// \~english @retval CFrameworkunifiedState* depend on implement class
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///      None
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       virtual function , implement by child class
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
  virtual CFrameworkunifiedState *FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState, CEventDataPtr f_pData) = 0;

 protected :

  /// Pointer to the target state of transition
  CFrameworkunifiedState *m_pTargetState;
};

#endif  // __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_TRANSITION_H__  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
