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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief    This file contains declaration of common APIs for NS_UtilityCenter.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file ns_utility_if.h
 */

#ifndef NS_NATIVESERVICES_INC_NATIVE_NS_UTILITY_IF_H_  // NOLINT  (build/header_guard)
#define NS_NATIVESERVICES_INC_NATIVE_NS_UTILITY_IF_H_

#include <native_service/frameworkunified_types.h>

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

/**
 * \~english Macro for invalid/undefined return
 */
#define NS_INVALID_RETURN    -1

/**
 * \~english Macro for setting the i bit of x to one
 */
#define NS_SETBIT(x, i)   ((x) |= (1 << (i)))

/**
 * \~english Macro for setting the i bit of x to zero
 */
#define NS_CLEARBIT(x, i)   ((x) &= ~(1 << (i)))

/**
 * \~english Macro for making the exclusive-OR operator on the i bit of x with one
 */
#define NS_TOGGLEBIT(x, i)   ((x) ^= (1 << (i)))

/**
 * \~english Macro for checking whether the i bit of x is one or not
 */
#define NS_ISBITSET(x, i)   (((x) & (1 << (i))) != 0)

#endif  // NS_NATIVESERVICES_INC_NATIVE_NS_UTILITY_IF_H_  // NOLINT  (build/header_guard)

/** @}*/  // end of native
/** @}*/  // end of framework_unified
/** @}*/  // end of native_service
/** @}*/  // end of BaseSystem
