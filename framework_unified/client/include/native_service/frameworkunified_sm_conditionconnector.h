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
/// \brief
///       This file has class declaration of the CFrameworkunifiedConditionConnector. This class is responsible for
///     implementing interfaces required to use condition connector in statemachine.
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_conditionconnector.h
 * @brief \~english This file has class declaration of the CFrameworkunifiedConditionConnector. This class is responsible for
 *        implementing interfaces required to use condition connector in statemachine.
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
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NSFRAMEWORKCORE_STATEMACHINE_INC_FRAMEWORKUNIFIEDSMCONDITIONCONNECTOR_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NSFRAMEWORKCORE_STATEMACHINE_INC_FRAMEWORKUNIFIEDSMCONDITIONCONNECTOR_H__

#include <native_service/frameworkunified_sm_externaltransition.h>
#include <string>
#include <vector>

// This class is responsible for  implementing interfaces required to use condition connector in statemachine.
class CFrameworkunifiedConditionConnector : public CFrameworkunifiedExternalTransition {
 public:
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedConditionConnector
  /// \~english @par Brief
  ///          Constructor for class CFrameworkunifiedConditionConnector.
  /// \~english @param [in] f_strName
  ///     std::string - Name of the condition connector
  /// \~english @retval
  /// \~english @par Prerequisite
  ///            none
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///            none
  /// \~english @par Detail
  ///           Inherite from base class CFrameworkunifiedExternalTransition.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see CFrameworkunifiedExternalTransition, ~CFrameworkunifiedConditionConnector
  ///////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedConditionConnector(std::string f_strName);  // NOLINT  (readability/nolint)

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedConditionConnector
  /// \~english @par Brief
  ///        Destructor for CFrameworkunifiedCompositeState
  /// \~english @retval none
  /// \~english @par Preconditons
  ///          - Self instance of CFrameworkunifiedConditionConnector created.
  /// \~english @par Change of internal status
  ///                none
  /// \~english @par Conditions of processing failure
  ///                none
  /// \~english @par Detail
  ///          Class CFrameworkunifiedExternalTransition instance and CFrameworkunifiedConditionConnector instance release.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///            none
  /// \~english @see CFrameworkunifiedConditionConnector::CFrameworkunifiedConditionConnector
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunifiedConditionConnector();

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedConditionConnector
  /// \~english @par Brief
  ///     This API evaluates the guards added in the condition list. If the guard is evaluated as
  ///     true then statemachine transitions to target state associated with guard.
  /// \~english @param [in] f_pSourceState
  ///           CFrameworkunifiedState* - Source state in which reaction is being executed
  /// \~english @param [in] f_pData
  ///           CEventDataPtr - event data
  /// \~english @retval CFrameworkunifiedState*
  ///                   CFrameworkunifiedState* - Returns Active state
  /// \~english @par Preconditons
  ///        -  Self instance of CFrameworkunifiedConditionConnector created.
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///            - If parameter f_pSourceState is NULL. [NULL]
  ///            - If m_pConditionList is NULL. [NULL]
  /// \~english @par Detail
  ///     This API evaluates the guards added in the condition list. If the guard is evaluated as
  ///     true then statemachine transitions to target state associated with guard.
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual CFrameworkunifiedState *FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState, CEventDataPtr f_pData);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedConditionConnector
  /// \~english @par Brief
  ///           Adds connect condition into condition list of current connection
  /// \~english @param [in] f_pGuard
  ///           CFrameworkunifiedState* - Pointer to the guard object to be added in the currrent connection
  /// \~english @param [in] f_pTargetState
  ///           CFrameworkunifiedState* - Target state for given guard condition
  /// \~english @retval EFrameworkunifiedStatus   eFrameworkunifiedStatusOK
  ///                                 eFrameworkunifiedStatusNullPointer
  /// \~english @par Preconditons
  ///        -  Self instance of CFrameworkunifiedConditionConnector created.
  /// \~english @par Change of internal status
  ///        -  none
  /// \~english @par Conditions of processing failure
  ///            - If m_pConditionList is NULL. [eFrameworkunifiedStatusNullPointer]
  ///            - If parameter f_pGuard is NULL. [eFrameworkunifiedStatusNullPointer]
  ///            - If parameter f_pTargetState is NULL. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///            Adds connect condition into condition list of current connection.\n
  /// \~english @par Classification
  ///          public
  /// \~english @par Type
  ///          none
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedAddCondition(CFrameworkunifiedGuard *f_pGuard, CFrameworkunifiedState *f_pTargetState);

 private:
  // This class defines the conditions required in condition connector
  class CCondition {
   public:
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// CCondition
    /// Parameterized constructor
    /// \param [in] f_pGuard
    ///     CFrameworkunifiedGuard* - guard object
    /// \param [in] f_pTargetState
    ///     CFrameworkunifiedState* - target state for a condition
    ///
    /// \return none
    ///////////////////////////////////////////////////////////////////////////////////////////
    CCondition(CFrameworkunifiedGuard *f_pGuard, CFrameworkunifiedState *f_pTargetState);

    // pointer to guard that will be evaluated for a condition
    CFrameworkunifiedGuard *m_pGuard;

    // pointer to target state for a condition
    CFrameworkunifiedState *m_pTargetState;
  };

  // Condition list
  typedef std::vector<CCondition *> TConditionList;

  // Condition list iterator
  typedef TConditionList::iterator TConditionIterator;

  // condition connector name
  std::string m_strName;

  // List of conditions
  TConditionList *m_pConditionList;
};

#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_NSFRAMEWORKCORE_STATEMACHINE_INC_FRAMEWORKUNIFIEDSMCONDITIONCONNECTOR_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
