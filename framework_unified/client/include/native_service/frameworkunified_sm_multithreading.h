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
/// \~english \brief    Application framework's core structures and data types.
///
///
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_multithreading.h
 * @brief \~english Application framework's core structures and data types.
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

#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_MULTITHREADING_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_MULTITHREADING_H__

#include <native_service/frameworkunified_framework_types.h>

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedCreateHSMChildThread
/// \~english @par Brief
/// Start a subordinate dispatcher in a separate thread
///
/// \~english \param [in] hApp
///         HANDLE - parent framework HANDLE
/// \~english \param [in] childName
///     PCSTR - Name to give to the child object
/// \~english \param [in] CbInitialize
///          CbFuncPtr -  Initialization callback - use this to initialize and configure
///             the child's dispatcher.
/// \~english \param [in] CbShutdown
///      CbFuncPtr -  Shutdown callback - use this to shutdown and close the child
///             The correct way for child to exit dispatch loop is to return eFrameworkunifiedStatusExit
///                       directly from this callback, or from some other interaction triggered by this
///                       callback
///
/// \~english \param [in] CbCreateStateMachine
///      CbFuncPtr -  StateMachine callback - use this to add new states, events and reactions
///             in the Thread statemachine.
/// \~english @retval HANDLE    create thread with Priority success
/// \~english @retval NULL    create thread with Priority failed
/// \~english @par Preconditions
///        - Generation/Initialization of Dispatcher for the Application
///          (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been done.
/// \~english @par Change of the internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hApp) is NULL. [NULL]
///        - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [NULL]
///        - Message queue name specified in the argument (childName) is not appropriate (NULL, over 16byte). [NULL]
///        - Callback function pointer specified in the argument (CbInitialize,CbShutdown) is NULL. [NULL]
///        - Thread attribute specified in the argument (attr) is NULL. [NULL]
///        - Failed to set the inherit-scheduler attribute (pthread_attr_setinheritsched) of the child thread. [NULL]
///        - Failed to initialize barrier object (pthread_barrier_init) for thread synchronization. [NULL]
///        - Failed to create child thread (pthread_create). [NULL]
///        - Failed to wait thread synchronization (pthread_barrier_wait). [NULL]
///        - Can not be acquired memory for message queue name(malloc). [NULL]
///        - Failed to open message queue(mq_open). [NULL]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       sync only
/// \~english @par Detail
///       Start a subordinate dispatcher in a separate thread
///       This API creates a child thread and returns a handle for communication with the child thread.\n
///       After creating/initializing the Dispatcher for the child thread, the child thread receives requests
///       and notifications to the Dispatcher and starts a main loop that executes the registered callback.
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedCreateHSMChildThread(HANDLE hApp, PCSTR childName, CbFuncPtr CbInitialize, CbFuncPtr
                               CbShutdown, CbFuncPtr CbCreateStateMachine);


  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup FrameworkunifiedCreateHSMChildThread
  /// \~english @par Summary
  ///       Start a subordinate dispatcher in a separate thread with given Priority
  /// \~english @param [in] hApp
  ///         HANDLE - parent framework HANDLE
  /// \~english @param [in] childName
  ///     PCSTR - Name to give to the child object
  /// \~english @param [in] CbInitialize
  ///         CbFuncPtr -  Initialization callback - use this to initialize and configure
  ///             the child's dispatcher.
  /// \~english @param [in] CbShutdown
  ///         CbFuncPtr -  Shutdown callback - use this to shutdown and close the child
  ///             The correct way for child to exit dispatch loop is to return eFrameworkunifiedStatusExit
  ///                       directly from this callback, or from some other interaction triggered by this
  ///                       callback
  ///
  /// \~english @param [in] CbCreateStateMachine
  ///         CbFuncPtr -  StateMachine callback - use this to add new states, events and reactions
  ///             in the Thread statemachine.
  /// \~english @param [in] schedPrio
  ///       SI_32 - given Priority for create thread
  /// \~english @retval HANDLE    create thread with Priority success
  /// \~english @retval NULL    create thread with Priority failed
  /// \~english @par Preconditions
///        - Generation/Initialization of Dispatcher for the Application
///          (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been done.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hApp) is NULL. [NULL]
///        - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [NULL]
///        - Message queue name specified in the argument (childName) is not appropriate (NULL, over 16byte). [NULL]
///        - Callback function pointer specified in the argument (CbInitialize,CbShutdown) is NULL. [NULL]
///        - Thread attribute specified in the argument (attr) is NULL. [NULL]
///        - Failed to set the inherit-scheduler attribute (pthread_attr_setinheritsched) of the child thread. [NULL]
///        - Failed to initialize barrier object (pthread_barrier_init) for thread synchronization. [NULL]
///        - Failed to create child thread (pthread_create). [NULL]
///        - Failed to wait thread synchronization (pthread_barrier_wait). [NULL]
///        - Can not be acquired memory for message queue name(malloc). [NULL]
///        - Failed to open message queue(mq_open). [NULL]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       Start a subordinate dispatcher in a separate thread with given Priority
  ///       This API creates a child thread and returns a handle for communication with the child thread.\n
  ///       After creating/initializing the Dispatcher for the child thread, the child thread receives requests
  ///       and notifications to the Dispatcher and starts a main loop that executes the registered callback.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedCreateHSMChildThreadWithPriority(HANDLE hApp, PCSTR childName, CbFuncPtr CbInitialize,
                                           CbFuncPtr CbShutdown, CbFuncPtr CbCreateStateMachine, SI_32 schedPrio);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup FrameworkunifiedCreateHSMChildThread
  /// \~english @par Summary
  ///       Start a subordinate dispatcher in a separate thread with given attribute
  /// \~english @param [in] hApp
  ///         HANDLE - parent framework HANDLE
  /// \~english @param [in] childName
  ///     PCSTR - Name to give to the child object
  /// \~english @param [in] CbInitialize
  ///         CbFuncPtr -  Initialization callback - use this to initialize and configure
  ///             the child's dispatcher.
  /// \~english @param [in] CbShutdown
  ///         CbFuncPtr -  Shutdown callback - use this to shutdown and close the child
  ///             The correct way for child to exit dispatch loop is to return eFrameworkunifiedStatusExit
  ///                       directly from this callback, or from some other interaction triggered by this
  ///                       callback
  ///
  /// \~english @param [in] CbCreateStateMachine
  ///         CbFuncPtr -  StateMachine callback - use this to add new states, events and reactions
  ///             in the Thread statemachine.
  /// \~english @param [in] attr
  ///       const FrameworkunifiedChildThreadAttr * -  given attribute to create thread
  /// \~english @retval HANDLE    create thread with Priority success
  /// \~english @retval NULL    create thread with Priority failed
  /// \~english @par Preconditions
///        - Generation/Initialization of Dispatcher for the Application
///          (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been done.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hApp) is NULL. [NULL]
///        - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [NULL]
///        - Message queue name specified in the argument (childName) is not appropriate (NULL, over 16byte). [NULL]
///        - Callback function pointer specified in the argument (CbInitialize,CbShutdown) is NULL. [NULL]
///        - Thread attribute specified in the argument (attr) is NULL. [NULL]
///        - Failed to set the inherit-scheduler attribute (pthread_attr_setinheritsched) of the child thread. [NULL]
///        - Failed to initialize barrier object (pthread_barrier_init) for thread synchronization. [NULL]
///        - Failed to create child thread (pthread_create). [NULL]
///        - Failed to wait thread synchronization (pthread_barrier_wait). [NULL]
///        - Can not be acquired memory for message queue name(malloc). [NULL]
///        - Failed to open message queue(mq_open). [NULL]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       Start a subordinate dispatcher in a separate thread with given attribute
  ///       This API creates a child thread and returns a handle for communication with the child thread.\n
  ///       After creating/initializing the Dispatcher for the child thread, the child thread receives requests
  ///       and notifications to the Dispatcher and starts a main loop that executes the registered callback.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedCreateHSMChildThreadWithAttribute(HANDLE hApp, PCSTR childName, CbFuncPtr CbInitialize,
                                            CbFuncPtr CbShutdown, CbFuncPtr CbCreateStateMachine,
                                            const FrameworkunifiedChildThreadAttr *attr);

#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_NATIVESERVICES_INC_FRAMEWORK_STATEMACHINE_FRAMEWORKUNIFIED_SM_MULTITHREADING_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
