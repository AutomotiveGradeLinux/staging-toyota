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
 * @file ss_last_to_order.h
 * @brief \~english This file contains declaration of enum SS_CONT_CATEGORY_t and api convert lastinfo to order
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup interface_unified
 *  @ingroup system_service
 *  @{
 */
#ifndef _SS_CONTID_TO_ORDER_H_  // NOLINT (build/header_guard)
#define _SS_CONTID_TO_ORDER_H_

#include <stdint.h>
#include <native_service/frameworkunified_types.h>
#include <map>
#include <string>

typedef enum {
  SS_CCAT_F_VIDEO,
  SS_CCAT_F_SUB_VIDEO,
  SS_CCAT_F_AUDIO,
  SS_CCAT_R_VIDEO,
  SS_CCAT_R_AUDIO,
  SS_CCAT_MAX,
}SS_CONT_CATEGORY_t;

typedef std::string SS_CONTENT_NAME_t;

typedef std::map<SS_CONT_CATEGORY_t, SS_CONTENT_NAME_t> SS_LAST_INFO_t;

EFrameworkunifiedStatus SS_ConvLastInfoToOrder(SS_LAST_INFO_t &curLastMode, std::string &order, const char* p_cfgPath = NULL);  //NOLINT (runtime/references)


#endif  // NOLINT (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
