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
/// \brief    System callbacks that applications should address.
///
///
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_application.h
 * @brief \~english This file contains the base api of an application that must be provided.
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
 *  @ingroup native_service
 *  @{
 */
#ifndef __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_APPLICATION_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_APPLICATION_H__

#include <native_service/frameworkunified_types.h>
////////////////////////////////////////////////////////////////////////////////
/// \ingroup IsNull
/// \~english @par Brief
///       Base class of application
/// \~english @param None
/// \~english @retval EFrameworkunifiedStatus depends on the implement of derived class
/// \~english @par Prerequisite
///       - depends on the implement of derived class
/// \~english @par Inside state change
///       - depends on the implement of derived class
/// \~english @par None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The callback of applicaiton initialize
////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE hApp);

/// \~english @par Brief
///       Base class of application
/// \~english @param None
/// \~english @retval EFrameworkunifiedStatus depends on the implement of derived class
/// \~english @par Prerequisite
///       - depends on the implement of derived class
/// \~english @par Inside state change
///       - depends on the implement of derived class
/// \~english @par None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The callback of applicaiton WaleUp
////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnWakeup(HANDLE hApp);   /// << deprecated

/// \~english @par Brief
///       Base class of application
/// \~english @param None
/// \~english @retval EFrameworkunifiedStatus depends on the implement of derived class
/// \~english @par Prerequisite
///       - depends on the implement of derived class
/// \~english @par Inside state change
///       - depends on the implement of derived class
/// \~english @par None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The callback of applicaiton shutdown
////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnShutdown(HANDLE hApp);  /// << deprecated

/// \~english @par Brief
///       Base class of application
/// \~english @param None
/// \~english @retval EFrameworkunifiedStatus depends on the implement of derived class
/// \~english @par Prerequisite
///       - depends on the implement of derived class
/// \~english @par Inside state change
///       - depends on the implement of derived class
/// \~english @par None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The callback of applicaiton EShutdown
////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnEShutdown(HANDLE hApp);  /// << deprecated

/// \~english @par Brief
///       Base class of application
/// \~english @param None
/// \~english @retval EFrameworkunifiedStatus depends on the implement of derived class
/// \~english @par Prerequisite
///       - depends on the implement of derived class
/// \~english @par Inside state change
///       - depends on the implement of derived class
/// \~english @par None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The callback of applicaiton start
////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp);

/// \~english @par Brief
///       Base class of application
/// \~english @param None
/// \~english @retval EFrameworkunifiedStatus depends on the implement of derived class
/// \~english @par Prerequisite
///       - depends on the implement of derived class
/// \~english @par Inside state change
///       - depends on the implement of derived class
/// \~english @par None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The callback of applicaiton stop
////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE hApp);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedOnPreStart
/// \~english @par Brief
///     callback of Application Life cycle event
/// \~english @param [in] hApp
///     HANDLE - Application Handle
/// \~english @retval EFrameworkunifiedStatus depends on the implement of derived class
/// \~english @par Prerequisite
///     Depend on the implementation in the inheriting class
/// \~english @par Change of internal state
///     Depend on the implementation in the inheriting class
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     callback of Application Pre start
/// \~english @par
///      - include frameworkunified_application.h
///      - Library libNS_FrameworkUnified.so
///      - Set compile option for "-DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND" when this API in the each application implements.
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedOnPreStop
/// \~english @par Brief
///     callback of Application Life cycle event
/// \~english @param [in] hApp
///     HANDLE - Application Handle
/// \~english @retval EFrameworkunifiedStatus depends on the implement of derived class
/// \~english @par Prerequisite
///     Depend on the implementation in the inheriting class
/// \~english @par Change of internal state
///     Depend on the implementation in the inheriting class
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     callback of Application Pre stop
/// \~english @par
///      - include frameworkunified_application.h
///      - Library libNS_FrameworkUnified.so
///      - Set compile option for "-DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND" when this API in the each application implements.
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedOnBackgroundStart
/// \~english @par Brief
///     callback of Application Life cycle event
/// \~english @param [in] hApp
///     HANDLE - Application Handle
/// \~english @retval EFrameworkunifiedStatus depends on the implement of derived class
/// \~english @par Prerequisite
///     Depend on the implementation in the inheriting class
/// \~english @par Change of internal state
///     Depend on the implementation in the inheriting class
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     callback of Application Background start
/// \~english @par
///      - include frameworkunified_application.h
///      - Library libNS_FrameworkUnified.so
///      - Set compile option for "-DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND" when this API in the each application implements.
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedOnBackgroundStop
/// \~english @par Brief
///     callback of Application Life cycle event
/// \~english @param [in] hApp
///     HANDLE - Application Handle
/// \~english @retval EFrameworkunifiedStatus depends on the implement of derived class
/// \~english @par Prerequisite
///     Depend on the implementation in the inheriting class
/// \~english @par Change of internal state
///     Depend on the implementation in the inheriting class
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     callback of Application Background stop
/// \~english @par
///      - include frameworkunified_application.h
///      - Library libNS_FrameworkUnified.so
///      - Set compile option for "-DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND" when this API in the each application implements.
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp);

/// \~english @par Brief
///       Base class of application
/// \~english @param None
/// \~english @retval EFrameworkunifiedStatus depends on the implement of derived class
/// \~english @par Prerequisite
///       - depends on the implement of derived class
/// \~english @par Inside state change
///       - depends on the implement of derived class
/// \~english @par None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The callback of applicaiton reinit
////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnReinit(HANDLE hApp);

/// \~english @par Brief
///       Base class of application
/// \~english @param None
/// \~english @retval EFrameworkunifiedStatus depends on the implement of derived class
/// \~english @par Prerequisite
///       - depends on the implement of derived class
/// \~english @par Inside state change
///       - depends on the implement of derived class
/// \~english @par None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The callback of applicaiton destroy
////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE hApp);

/// \~english @par Brief
///       Base class of application
/// \~english @param None
/// \~english @retval EFrameworkunifiedStatus depends on the implement of derived class
/// \~english @par Prerequisite
///       - depends on the implement of derived class
/// \~english @par Inside state change
///       - depends on the implement of derived class
/// \~english @par None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The callback of applicaiton debugdump
////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE hApp);  // << Used for CCR Debugging


/// \~english @par Brief
///       Base class of application
/// \~english @param None
/// \~english @retval PCSTR depends on the implement of derived class
/// \~english @par Prerequisite
///       - depends on the implement of derived class
/// \~english @par Inside state change
///       - depends on the implement of derived class
/// \~english @par None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The callback of get applicaiton servcice avaible
////////////////////////////////////////////////////////////////////////////////
PCSTR FrameworkunifiedGetServiceAvailabilityNotification(HANDLE hApp);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedOnDummy
/// \~english @par Brief
///     dummy callback of Application Life cycle event
/// \~english @param [in] hApp
///     HANDLE - Application Handle
/// \~english @retval EFrameworkunifiedStatus eFrameworkunifiedStatusOK
/// \~english @par Prerequisite
///     Prerequisites are nothing.
/// \~english @par Change of internal state
///     Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     Registered dummy callback when not processing
/// \~english @par
///      - include frameworkunified_application.h
///      - Library libNS_FrameworkUnified.so
/// \~english @see
///
/////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedOnDummy(HANDLE hApp);

#endif /* __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_APPLICATION_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
