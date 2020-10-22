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
/// \ingroup  tag_NSFramework
/// \brief    This file has class declaration of the CFrameworkunifiedAction. This is an
///       interface class that defines interfaces for implementing action
///
///
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_action.h
 * @brief \~english This file has class declaration of the CFrameworkunifiedAction. This is an interface class that defines
 *        interfaces for implementing action
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
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NSFRAMEWORKCORE_STATEMACHINE_INC_FRAMEWORKUNIFIEDSMACTION_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NSFRAMEWORKCORE_STATEMACHINE_INC_FRAMEWORKUNIFIEDSMACTION_H__

#include <native_service/frameworkunified_types.h>
#include <boost/smart_ptr.hpp>
#include <string>

class CFrameworkunifiedState;

class CEventData;
typedef boost::shared_ptr<CEventData> CEventDataPtr;

class CFrameworkunifiedAction {
 public:
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedAction
  /// \~english @par Brief
  ///          Constructor for class CFrameworkunifiedAction.
  /// \~english @param [in] f_strName
  ///     std::string -Name of Action object
  /// \~english @retval
  /// \~english @par Prerequisite
  ///            none
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///            none
  /// \~english @par Detail
  ///           Constructor of CFrameworkunifiedAction class. Create an object of CFrameworkunifiedAction. \n
  ///           Initialize member value with name designated by f_strName. \n
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see ~CFrameworkunifiedAction
  ///////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedAction(std::string f_strName);  // NOLINT  (readability/nolint)

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedAction
  /// \~english @par Brief
  ///        Destructor for CFrameworkunifiedAction
  /// \~english @retval none
  /// \~english @par Preconditons
  ///                none
  /// \~english @par Change of internal status
  ///                none
  /// \~english @par Conditions of processing failure
  ///                none
  /// \~english @par Detail
  ///           Pure virtual function , implemented by child class
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see CFrameworkunifiedAction
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunifiedAction() {
  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedAction
  /// \~english @par Brief
  ///          Callback function of CFrameworkunifiedAction.(pure virtual function)
  /// \~english @param [in] f_pSourceState
  ///           CFrameworkunifiedState - pointer of source state
  /// \~english @param [in] f_pTargetState
  ///           CFrameworkunifiedState - pointer of target state
  /// \~english @param [in] f_pData
  ///           CEventDataPtr - pointer of event data
  /// \~english @retval none
  /// \~english @par Preconditons
  ///        -  none
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///        - none
  /// \~english @par Detail
  ///          Callback function of CFrameworkunifiedAction.(pure virtual function)
  ///          Implement action by subclass
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual VOID FrameworkunifiedAction(CFrameworkunifiedState *f_pSourceState, CFrameworkunifiedState *f_pTargetState,
                         CEventDataPtr f_pData) = 0;

  // action name
  std::string m_strName;
};

#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_NSFRAMEWORKCORE_STATEMACHINE_INC_FRAMEWORKUNIFIEDSMACTION_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
