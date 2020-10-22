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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NS_NPPService
/// .
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_NPPService
/// \brief
///
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOTIFICATIONPERSISTENTSERVICELOG_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOTIFICATIONPERSISTENTSERVICELOG_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_logger_if.h>

#define ZONE_INIT     ZONEMASK(10)
#define ZONE_FUNC     ZONEMASK(11)
#define ZONE_MEM      ZONEMASK(12)
#define ZONE_13       ZONEMASK(13)
#define ZONE_14       ZONEMASK(14)
#define ZONE_15       ZONEMASK(15)
#define ZONE_16       ZONEMASK(16)
#define ZONE_17       ZONEMASK(17)
#define ZONE_18       ZONEMASK(18)
#define ZONE_19       ZONEMASK(19)
#define ZONE_20       ZONEMASK(20)
#define ZONE_21       ZONEMASK(21)
#define ZONE_22       ZONEMASK(22)
#define ZONE_23       ZONEMASK(23)
#define ZONE_24       ZONEMASK(24)
#define ZONE_NPP_INFO ZONEMASK(25)
#define ZONE_PRD_INFO3  ZONEMASK(26)
#define ZONE_PRD_INFO2  ZONEMASK(27)
#define ZONE_PRD_INFO1  ZONEMASK(28)
#define ZONE_INFO     ZONEMASK(29)
#define ZONE_WARN     ZONEMASK(30)
#define ZONE_ERR      ZONEMASK(31)

#define ZONE_TEXT_10      "Init"
#define ZONE_TEXT_11      "Function"
#define ZONE_TEXT_12      "Memory"
#define ZONE_TEXT_13      ""
#define ZONE_TEXT_14      ""
#define ZONE_TEXT_15      ""
#define ZONE_TEXT_16      ""
#define ZONE_TEXT_17      ""
#define ZONE_TEXT_18      ""
#define ZONE_TEXT_19      ""
#define ZONE_TEXT_20      ""
#define ZONE_TEXT_21      ""
#define ZONE_TEXT_22      ""
#define ZONE_TEXT_23      ""
#define ZONE_TEXT_24      ""
#define ZONE_TEXT_25      "NPP Info"
#define ZONE_TEXT_26      "Product Info3"
#define ZONE_TEXT_27      "Product Info2"
#define ZONE_TEXT_28      "Product Info1"
#define ZONE_TEXT_29      "Info"
#define ZONE_TEXT_30      "Warning"
#define ZONE_TEXT_31      "Error"

#ifndef  FRAMEWORKUNIFIEDLOGOPTIONS
#define FRAMEWORKUNIFIEDLOGOPTIONS (LPRINT)  // LPRINT , LMSGQ, LSLOGGER
#endif

#ifndef FRAMEWORKUNIFIEDLOGAPPZONES
#define FRAMEWORKUNIFIEDLOGAPPZONES ZONE_WARN, ZONE_ERR, ZONE_NS_WAR, ZONE_NS_ERR, ZONE_PRD_INFO2
#endif

extern const CHAR AppName[];  // NOLINT (readability/naming)
#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOTIFICATIONPERSISTENTSERVICELOG_H_
