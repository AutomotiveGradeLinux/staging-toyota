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
/// This file defines the events used by the statemachine framework
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_eventfactory.h
 * @brief \~english This file defines the events used by the statemachine framework.
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
#ifndef CFRAMEWORKUNIFIEDEVENTFACTORY_H_  // NOLINT  (build/header_guard)
#define CFRAMEWORKUNIFIEDEVENTFACTORY_H_

#include <native_service/frameworkunified_types.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class defines the events used by the statemachine framework
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedEventFactory {
 public:
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedEventFactory
  /// \~english @par Brief
  ///          Constructor for class CFrameworkunifiedEventFactory.
  /// \~english @param none
  /// \~english @retval
  /// \~english @par Prerequisite
  ///            none
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///            none
  /// \~english @par Detail
  ///           none
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see ~CFrameworkunifiedEventFactory
  ///////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedEventFactory();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedEventFactory
  /// \~english @par Brief
  ///        Destructor for CFrameworkunifiedEventFactory
  /// \~english @retval none
  /// \~english @par Preconditons
  ///          - Self instance of CFrameworkunifiedEventFactory created.
  /// \~english @par Change of internal status
  ///                none
  /// \~english @par Conditions of processing failure
  ///                none
  /// \~english @par Detail
  ///          Class CFrameworkunifiedHistoryState instance release.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///            none
  /// \~english @see CFrameworkunifiedEventFactory::CFrameworkunifiedEventFactory
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunifiedEventFactory();

  /// Defines the events for starting an application
  static const UI_32 evFrameworkunifiedStart = 0;

  /// Defines the Higher limit for event id reserved only for framework
  static const UI_32 evFrameworkunifiedEventLimit = 10;
};

#endif /* CFRAMEWORKUNIFIEDEVENTFACTORY_H_ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
