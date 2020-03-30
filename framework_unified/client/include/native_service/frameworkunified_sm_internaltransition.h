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
/// This file has the CFrameworkunifiedInternalTransition class definitions. CFrameworkunifiedInternalTransition is derived
/// from CFrameworkunifiedReaction class.This class provides the interface for defining a reaction for an event
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_internaltransition.h
 * @brief \~english This file has the CFrameworkunifiedInternalTransition class definitions. CFrameworkunifiedInternalTransition is derived
 *        from CFrameworkunifiedReaction class.This class provides the interface for defining a reaction for an event
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
#ifndef _FRAMEWORKUNIFIEDINTERNALTRANSITION_H  // NOLINT  (build/header_guard)
#define _FRAMEWORKUNIFIEDINTERNALTRANSITION_H

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_reaction.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class provides the interface for defining a reaction for an event
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedInternalTransition : public CFrameworkunifiedReaction {
 public :

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedInternalTransition
  /// \~english @par Summary
  ///       pure virtual fuction
  /// \~english @param [in] f_pSourceState
  ///        CFrameworkunifiedState* f_pSourceState  -  source state
  /// \~english @param [in] f_pData
  ///        CEventDataPtr f_pData  -  event data
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - None
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       Pure virtual function , implemented by child class
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedReaction
  /// The reaction for the event has to be implemented in this function
  ///
  /// \param [in] f_pSourceState
  ///     CFrameworkunifiedState* - Event id of the event to be added in the state
  ///
  /// \param [in] f_pData
  ///     CEventDataPtr - Reaction to be associated with the with event id in the state
  ///
  /// \return ActiveState
  ///         CFrameworkunifiedState* - Returns Active state
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual CFrameworkunifiedState *FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState, CEventDataPtr f_pData) = 0;
};

#endif  // _FRAMEWORKUNIFIEDINTERNALTRANSITION_H  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
