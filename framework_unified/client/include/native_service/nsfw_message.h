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
/**
 * @file nsfw_message.h
 * @brief \~english  The file definition of NSFW message for NSFW independent module reference
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
/** @addtogroup utility
 *  @ingroup framework_unified
 *  @{
 */
/*
 * The definition of NSFW message for NSFW independent module reference.
 *
 */
//@{
#ifndef _nsfw_message_h_  // NOLINT  (build/header_guard)
#define _nsfw_message_h_

/*
 * The following definitions are defined only for NSFW to check consistency.
 * NOT TO BE USED IN ANY WAY.
 */
#define _NSFW_MSG_LEN_ (40)
#define _NSFW_SYSINFO_FLAG_ (4)
#define _NSFW_SYSINFO_SIZE_ (64)

/*
 * The following definitions are published definitions.
 */
#define NSFW_GET_MESSAGE(msg) \
  ((*((unsigned int *)msg) & _NSFW_SYSINFO_FLAG_) ? \
    ((unsigned char *)msg) + _NSFW_MSG_LEN_ + _NSFW_SYSINFO_SIZE_ : \
    ((unsigned char *)msg) + _NSFW_MSG_LEN_)

#endif  // _nsfw_message_h_  // NOLINT  (build/header_guard)
//@}
/**@}*/  // end of utility
/**@}*/  // end of framework_unified
/**@}*/  // end of native_service
/**@}*/  // end of BaseSystem
