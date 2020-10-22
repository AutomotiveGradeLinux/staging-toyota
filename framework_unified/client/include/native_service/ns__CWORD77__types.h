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

////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_HMI_CWORD77_Controller
/// \brief
///
////////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file
 * @brief \~english This file contains enum of PE_CWORD77_ResponseType
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
/** @addtogroup _CWORD77_
 *  @ingroup framework_unified
 *  @{
 */
#ifndef _NS__CWORD77__TYPES_H_  // NOLINT  (build/header_guard)
#define _NS__CWORD77__TYPES_H_

#include <native_service/frameworkunified_types.h>
#include <sstream>
#include <utility>
#include <string>
// using namespace std;

typedef enum _E_CWORD77_ResponseType {
  RESPONSE = 0,
  NOTIFICATION,
  EVENT
} E_CWORD77_ResponseType, *PE_CWORD77_ResponseType;

// Pair to uniquely identify protocol commands and notifications for different services
// pair of protocol id and service name in case of protocol interfaces
// pair of 0 and notification name in case of notifications, as notification name will always be unique
typedef std::pair<UI_32, std::string> _CWORD77_DataPoolKey;

#endif /* _NS__CWORD77__TYPES_H_ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
