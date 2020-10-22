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
/// \ingroup  tag_StateMachine
/// \brief
///
/// This file has the CFrameworkunifiedLocalTransition class definitions. CFrameworkunifiedLocalTransition is derived
/// from CFrameworkunifiedTransition class.This class provides the interface for local transition over event
///////////////////////////////////////////// //////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_localtransition.h
 * @brief \~english This file has the CFrameworkunifiedLocalTransition class definitions. CFrameworkunifiedLocalTransition is derived
 *        from CFrameworkunifiedTransition class.
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
#ifndef _FRAMEWORKUNIFIEDLOCALTRANSITIONS_H  // NOLINT  (build/header_guard)
#define _FRAMEWORKUNIFIEDLOCALTRANSITIONS_H

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_transition.h>

class CFrameworkunifiedEventData;
///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class provides the interface for local transition over event
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedLocalTransition : public CFrameworkunifiedTransition {
 public :

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedLocalTransition
  /// \~english @par Brief
  ///        CFrameworkunifiedLocalTransition constructor
  /// \~english @param [in] f_pTargetState
  ///        CFrameworkunifiedState *f_pTargetState  -  target state
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        None
  /// \~english @par Detail
  ///        Constructor of CFrameworkunifiedLocalTransition, to create CFrameworkunifiedLocalTransition object
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedLocalTransition(CFrameworkunifiedState *f_pTargetState);  // NOLINT  (readability/nolint)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedLocalTransition
  /// \~english @par Brief
  ///        CFrameworkunifiedLocalTransition destructor
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        None
  /// \~english @par Detail
  ///        CFrameworkunifiedLocalTransition destructor, for destory CFrameworkunifiedLocalTransition object
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunifiedLocalTransition();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedLocalTransition
  /// \~english @par Brief
  ///        reaction for location transition
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        None
  /// \~english @par Detail
  ///        For local transition from inner
  ///        state to outer state exit of the inner state is invoked, but entry of outer state is not
  ///        invoked and from outer state to inner state entry of the inner state is invoked but exit of
  ///        outer state is not invoked
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedReaction
  /// The reaction for an event is implemented in this function. For local transition from inner
  /// state to outer state exit of the inner state is invoked, but entry of outer state is not
  /// invoked and from outer state to inner state entry of the inner state is invoked but exit of
  /// outer state is not invoked
  ///
  /// \param [in] f_pSourceState
  ///     CFrameworkunifiedState* - Source state in which reaction is being executed
  ///
  /// \param [in] f_pData
  ///     CEventDataPtr - event data
  ///
  /// \return ActiveState
  ///         CFrameworkunifiedState* - Returns Active state
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual CFrameworkunifiedState *FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState, CEventDataPtr f_pData);
};

#endif  // _FRAMEWORKUNIFIEDLOCALTRANSITIONS_H  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
