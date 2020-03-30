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
/// This file has the CFrameworkunifiedExternalTransition class definitions. CFrameworkunifiedExternalTransition is derived
/// from CFrameworkunifiedTransition class.This class implements the FrameworkunifiedReaction interface to support transition
/// from one state to another state.
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_externaltransition.h
 * @brief \~english This file has the CFrameworkunifiedExternalTransition class definitions. CFrameworkunifiedExternalTransition is derived
 *        from CFrameworkunifiedTransition class.This class implements the FrameworkunifiedReaction interface to support transition
 *        from one state to another state.
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
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_EXTERNALTRANSITION_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_EXTERNALTRANSITION_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_transition.h>
#include <vector>
class CFrameworkunifiedGuard;
class CFrameworkunifiedAction;
class CEventData;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class implements the FrameworkunifiedReaction interface to support transition
/// from one state to another state.
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedExternalTransition : public CFrameworkunifiedTransition {
 public :

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedExternalTransition
  /// \~english @par Brief
  ///          Constructor for class CFrameworkunifiedExternalTransition.
  /// \~english @param [in] f_pTargetState
  ///        CFrameworkunifiedState*   - pointer of target CFrameworkunifiedState object
  /// \~english @retval none
  /// \~english @par Prerequisite
  ///            none
  /// \~english @par Change of internal state
  ///            none
  /// \~english @par Conditions of processing failure
  ///            none
  /// \~english @par Detail
  ///           inherit the CFrameworkunifiedTransition class and create instance of CFrameworkunifiedExternalTransition class \n
  ///           create the list for CFrameworkunifiedAction class,set pointer as member parameter\n
  ///           this class is intanced with FrameworkunifiedReaction interfase in order to support changing one state to other state.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           none
  /// \~english @see CFrameworkunifiedExternalTransition, FrameworkunifiedReaction, FrameworkunifiedAddAction
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// CFrameworkunifiedExternalTransition
  /// Parameterized constructor
  /// \param [in] f_pTargetState
  ///     CFrameworkunifiedState* - Target state object
  ///
  /// \return none
  CFrameworkunifiedExternalTransition(CFrameworkunifiedState *f_pTargetState);  // NOLINT  (readability/nolint)

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedExternalTransition
  /// \~english @par Brief
  ///        Destructor for CFrameworkunifiedExternalTransition
  /// \~english @retval none
  /// \~english @par Preconditons
  ///          - Self instance of CFrameworkunifiedExternalTransition created.
  /// \~english @par Change of internal status
  ///                none
  /// \~english @par Conditions of processing failure
  ///                none
  /// \~english @par Detail
  ///          Class CFrameworkunifiedHistoryState instance release. \n
  ///          Member variable active action list of CFrameworkunifiedAction release.\n
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///            none
  /// \~english @see CFrameworkunifiedExternalTransition::CFrameworkunifiedExternalTransition
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFrameworkunifiedExternalTransition
  /// Class destructor
  ///
  /// \return none
  virtual ~CFrameworkunifiedExternalTransition();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedExternalTransition
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedReaction
  /// The reaction for an event is implemented in this function. For external transition, Exit
  /// of the source state is called and entry of the target state is called recursively.
  ///
  /// \~english @par Brief
  ///         CFrameworkunifiedExternalTransition instance event process.
  /// \~english @param f_pSourceState
  ///       CFrameworkunifiedState* - source state object pointer
  /// \~english @param f_pData
  ///       CEventDataPtr* - CEventData event pointer
  /// \~english @retval CFrameworkunifiedState* active state pointer
  /// \~english @retval NULL
  /// \~english @par Preconditons
  ///          - Self instance of CFrameworkunifiedExternalTransition created.
  /// \~english @par Change of internal status
  ///                - If input parameter f_pSourceState is NULL. [NULL]
  ///                - If member variable m_pTargetState is NULL. [NULL]
  /// \~english @par Conditions of processing failure
  ///                none
  /// \~english @par Detail
  ///      if the self-instance has guard object,event operation will not conduct.
  ///      return soucestate(CFrameworkunifiedState class)object()f_pSourceState(set from parameter) as
  ///      activestate(CFrameworkunifiedState class)object\n
  ///      as activestate(CFrameworkunifiedState class)object
  ///      if there is no guard object:
  ///       - Exit process(FrameworkunifiedOnHSMStop call)for sorcestate(CFrameworkunifiedStateclass)object will conduct once more.
  ///       - Entryt process(FrameworkunifiedOnHSMStart call)for target state(CFrameworkunifiedState class)objectwill conduct once more.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///            none
  /// \~english @see FrameworkunifiedSetGuard, FrameworkunifiedAddAction
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual CFrameworkunifiedState *FrameworkunifiedReaction(CFrameworkunifiedState *f_pSourceState, CEventDataPtr f_pData);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedExternalTransition
  /// \~english @par Brief
  ///         Sets the Guard object in External Transition
  /// \~english @param f_pGuard
  ///       CFrameworkunifiedGuard* - CFrameworkunifiedGuard object pointer
  /// \~english @retval eFrameworkunifiedStatusOK           success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  set null pointer
  /// \~english @par Preconditons
  ///          - Self instance of CFrameworkunifiedExternalTransition created.
  /// \~english @par Change of internal status
  ///                - If input parameter CFrameworkunifiedGuard is NULL. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Conditions of processing failure
  ///                none
  /// \~english @par Detail
  ///           Sets the Guard object in CFrameworkunifiedExternalTransition instance\n
  ///           in order to guard the transform process. and during setting process,\n
  ///           transform is not conduct
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///            none
  /// \~english @see FrameworkunifiedReaction, FrameworkunifiedAddAction
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedSetGuard
  /// Sets the Guard object in External Transition
  ///
  /// \param [in] f_pGuard
  ///     CFrameworkunifiedGuard* - guard object
  ///
  ///
  /// \return status
  ///         EFrameworkunifiedStatus -
  ///
  virtual EFrameworkunifiedStatus FrameworkunifiedSetGuard(CFrameworkunifiedGuard *f_pGuard);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedExternalTransition
  /// \~english @par Brief
  ///         Adds CFrameworkunifiedAction object to the action list
  /// \~english @param f_pAction
  ///       CFrameworkunifiedAction* - CFrameworkunifiedAction object pointer
  /// \~english @retval eFrameworkunifiedStatusOK           success
  /// \~english @retval eFrameworkunifiedStatusNullPointer  set null pointer
  /// \~english @par Preconditons
  ///          - Self instance of CFrameworkunifiedExternalTransition created.
  /// \~english @par Change of internal status
  ///               none
  /// \~english @par Conditions of processing failure
  ///                - If input parameter CFrameworkunifiedAction is NULL. [eFrameworkunifiedStatusNullPointer]
  ///                - If member variable m_pActionList is NULL. [eFrameworkunifiedStatusNullPointer]
  /// \~english @par Detail
  ///              add CFrameworkunifiedAction class object to the end of the list for CFrameworkunifiedAction class in parameter
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///            none
  /// \~english @see FrameworkunifiedReaction, FrameworkunifiedSetGuard
  ///////////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus FrameworkunifiedAddAction(CFrameworkunifiedAction *f_pAction);


 protected :
  /// pointer to the guard condition object
  CFrameworkunifiedGuard *m_pGuard;

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ExecuteTransition
  /// Execute the transition actions, calls OnExit of Source State and OnEntry of target state
  ///
  /// \param [in] f_pSourceState
  ///     CEventDataPtr - source state
  ///
  /// \param [in] f_pData
  ///     CFrameworkunifiedState* - Event data
  ///
  ///
  /// \return status
  ///         EFrameworkunifiedStatus -
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedState *ExecuteTransition(CFrameworkunifiedState *f_pSourceState, CEventDataPtr f_pData);

  // Action list
  typedef std::vector<CFrameworkunifiedAction *> TActionList;

  // Action list iterator
  typedef TActionList::iterator TActionListIterator;

  // List of conditions
  TActionList *m_pActionList;
};
#endif  // __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_EXTERNALTRANSITION_H__  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
