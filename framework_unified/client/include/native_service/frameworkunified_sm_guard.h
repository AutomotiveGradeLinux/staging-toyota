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
/// This file has the CFrameworkunifiedGuard class definitions. This is an interface that defines the Evaluate
/// interface for guard condition validation.
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_guard.h
 * @brief \~english This file has the CFrameworkunifiedGuard class definitions.
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
#ifndef _FRAMEWORKUNIFIEDGUARD_H  // NOLINT  (build/header_guard)
#define _FRAMEWORKUNIFIEDGUARD_H

#include <native_service/frameworkunified_types.h>
#include <string>


/**
 * @class CNSSharedMem
 * \~english @brief this file has the CFrameworkunifiedGuard class definitions
 * \~english @par   Brief Introduction
 *        This file has the CFrameworkunifiedGuard class definitions. This is an interface \n
 *        that defines the Evaluate interface for guard condition validation.
 *
 */
class CFrameworkunifiedGuard {
 public :
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedGuard
  /// \~english @par Brief
  ///           CFrameworkunifiedGuard's constructor
  /// \~english @param [in] f_strName
  ///           string - Name of the guard
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           - None
  /// \~english @par Conditions of processing failure
  ///           - None
  /// \~english @par Detail
  ///           - CFrameworkunifiedGuard's constructor
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - Sync only
  /// \~english @see ~CFrameworkunifiedGuard
  ////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedGuard(std::string f_strName);  // NOLINT  (readability/nolint)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~CFrameworkunifiedGuard
  /// \~english @par Brief
  ///           - Destructor of CFrameworkunifiedGuard class
  /// \~english @param None
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           - Calls constructor to successfully create object.
  /// \~english @par Change of internal status
  ///           - None
  /// \~english @par Conditions of processing failure
  ///           - None
  /// \~english @par Detail
  ///           - Destruct the object of this class.
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - Sync only
  /// \~english @see CFrameworkunifiedGuard
  ////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunifiedGuard();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedGuard
  /// \~english @par Brief
  ///              This is a pure virtual function that defines the logic for evaluating the guard condition
  /// \~english @param  None
  /// \~english @retval TRUE - if test passes
  /// \~english @retval FALSE - otherwise returns false
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           - None
  /// \~english @par Conditions of processing failure
  ///           - None
  /// \~english @par Detail
  ///           - This is a pure virtual function that defines the logic for evaluating the guard condition
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - Sync only
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////////
  virtual BOOL FrameworkunifiedEvaluate() = 0;

 private:
  std::string m_strName;
};

#endif  // _FRAMEWORKUNIFIEDGUARD_H  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
