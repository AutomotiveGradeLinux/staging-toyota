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
/// This file has the CFrameworkunifiedReaction class definitions.This class provides the interface
/// for defining a reaction for an event
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_reaction.h
 * @brief \~english This file has the CFrameworkunifiedReaction class definitions.This class provides the interface
 *        for defining a reaction for an event
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
#ifndef _FRAMEWORKUNIFIEDREACTION_H  // NOLINT  (build/header_guard)
#define _FRAMEWORKUNIFIEDREACTION_H

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <boost/smart_ptr.hpp>
#include <native_service/frameworkunified_types.h>
#include <native_service/ns_logger_if.h>
#include <exception>

class CFrameworkunifiedState;
class CEventData;

typedef boost::shared_ptr<CEventData> CEventDataPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class provides the interface for defining a reaction for an event
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedReaction {
 public :
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedReaction
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
  ///       pure virtual function , implement by child class
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedReaction
  /// The reaction for the event has to be implemented in this function
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
  virtual CFrameworkunifiedState *FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState, CEventDataPtr f_pData) = 0;

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedReaction
  /// \~english @par Summary
  ///       CFrameworkunifiedReaction constructor
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - m_ucRefCount = 0
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       CFrameworkunifiedReaction Constructor, for create CFrameworkunifiedReaction object
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// CFrameworkunifiedReaction
  /// Constructor
  ///
  /// \param
  ///
  /// \return
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedReaction() {
    m_ucRefCount = 0;
  }

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedReaction
  /// \~english @par Summary
  ///       CFrameworkunifiedReaction destructor
  /// \~english @param None
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
  ///       CFrameworkunifiedReaction destructor, for destory CFrameworkunifiedReaction object
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFrameworkunifiedReaction
  /// Destructor
  ///
  /// \param
  ///
  /// \return
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunifiedReaction() {}

  // count to check the number of reference of this reaction in all the states of statemachine
  UI_8 m_ucRefCount;
};
#endif  // _FRAMEWORKUNIFIEDREACTION_H  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
