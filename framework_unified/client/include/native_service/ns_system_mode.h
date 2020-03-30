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

/*
 * NS_SYSTEM_MODE.h
 */

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file ns_system_mode.h
 * @brief \~english The file defines enum SystemModeProtocol
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
/** @addtogroup native
 *  @ingroup framework_unified
 *  @{
 */
#ifndef __NATIVESERVICES_SYSTEM_MODE_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_SYSTEM_MODE_H__

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_service_protocol.h>

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_System
/// \brief    System Mode Protocol
////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum _SystemModeProtocol {
  SYSTEM_ON_INITIALIZATION = PROTOCOL_THREAD_INITIALIZATION,  ///< Initialize thread after creation
  SYSTEM_ON_WAKEUP         = PROTOCOL_THREAD_WAKEUP,     ///< Wakeup Thread after sleep
  SYSTEM_ON_SHUTDOWN       = PROTOCOL_THREAD_SHUTDOWN,   ///< Stop the thread
  SYSTEM_ON_DESTROY        = PROTOCOL_THREAD_DESTROY  ///< Destroy the thread
} SystemModeProtocol;

#endif /* __NATIVESERVICES_SYSTEM_MODE_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
