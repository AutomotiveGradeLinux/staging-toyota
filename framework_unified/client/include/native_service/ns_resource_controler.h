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
/**
 * @file ns_resource_controler.h
 * @brief \~english This file provides api for operating Resource
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
#ifndef _ns_resource_controler_h_  // NOLINT  (build/header_guard)
#define _ns_resource_controler_h_

#define FRAMEWORKUNIFIED_RES_ABNMLMON "FRAMEWORKUNIFIED_RES_ABNMLMON"
#define FRAMEWORKUNIFIED_RES_TIMER "FRAMEWORKUNIFIED_RES_TIMER"

#ifdef __cplusplus
extern "C" {
#endif
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup BaseSystem
  /// \~english @par Summary
  ///       This function is used to get the resourceid associated with the mod and the key.
  /// \~english @param [IN] mod
  ///        const char * - the mod of resource
  /// \~english @param [IN] key
  ///        const char * - the key of resource
  /// \~english @param [OUT] resource
  ///        long * - resource ID
  /// \~english @retval -1 fail
  /// \~english @retval 0  success
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       This function is used to get the resourceid associated with the mod and the key.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
int frameworkunifiedGetResource(const char *mod, const char *key, long *resource);  // NOLINT  (readability/nolint)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup BaseSystem
  /// \~english @par Summary
  ///       This function is used to acquire the resourceid associated with the mod and the key.
  /// \~english @param [IN] mod
  ///        const char * - the mod of resource
  /// \~english @param [IN] key
  ///        const char * - the key of resource
  /// \~english @param [OUT] resource
  ///        long * - resource ID
  /// \~english @retval ref_counter the counter of user
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       This function is used to acquire the resourceid associated with the mod and the key.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
int frameworkunifiedAcquireResouce(const char *mod, const char *key, long *resource);  // NOLINT  (readability/nolint)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup BaseSystem
  /// \~english @par Summary
  ///       This function is used to release the resourceid associated with the mod and the key.
  /// \~english @param [IN] mod
  ///        const char * - the mod of resource
  /// \~english @param [IN] key
  ///        const char * - the key of resource
  /// \~english @retval ref_counter the counter of user
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       This function is used to release the resourceid associated with the mod and the key.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
int frameworkunifiedReleaseResouce(const char *mod, const char *key);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup BaseSystem
  /// \~english @par Summary
  ///       This function is used to get the key associated with the mod and the resourceid.
  /// \~english @param [IN] mod
  ///        const char * - the mod of resource
  /// \~english @param [IN] resource
  ///        long - resource ID
  /// \~english @param [OUT] key
  ///        const char * - the key of resource
  /// \~english @retval -1 fail
  /// \~english @retval 0  success
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       This function is used to get the key associated with the mod and the resourceid.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
int frameworkunifiedSearchResourseKey(const char *mod, long resource, const char **key);  // NOLINT  (readability/nolint)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup BaseSystem
  /// \~english @par Summary
  ///       This function is used to register the resource associated with the mod and the key.
  /// \~english @param [IN] mod
  ///        const char * - the mod of resource
  /// \~english @param [IN] key
  ///        const char * - the key of resource
  /// \~english @param [IN] resource
  ///        const char * - resource ID
  /// \~english @param [IN] init_counter
  ///        long - resource user counter
  /// \~english @retval -1 fail
  /// \~english @retval 0  success
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       This function is used to register the resource associated with the mod and the key.init counter.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////

int frameworkunifiedRegistResouce(const char *mod, const char *key, long resource, int init_counter);  // NOLINT  (readability/nolint)
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup BaseSystem
  /// \~english @par Summary
  ///       This function is used to unregister the resource associated with the mod and the key.
  /// \~english @param [IN] mod
  ///        const char * - the mod of resource
  /// \~english @param [IN] key
  ///        const char * - the key of resource
  /// \~english @retval 0  success
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       This function is used to unregister the resource associated with the mod and the key.init counter.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
int frameworkunifiedUnregistResouce(const char *mod, const char *key);

#ifdef __cplusplus
}
#endif

#endif  // _ns_resource_controler_h_ NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
