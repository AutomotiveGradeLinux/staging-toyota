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
/// \ingroup
/// \brief
///
///
///
///
///
///////////////////////////////////////////////////////////////////////////////

/*
 * This file has been generated automatically.
 * User hand written code entry is not allowed. Do not modify the file content.
 * 
 */
#ifndef COMMUNICATION_SERVER_INCLUDE_MAIN_COMMUNICATION_COMMUNICATIONLOG_H_
#define COMMUNICATION_SERVER_INCLUDE_MAIN_COMMUNICATION_COMMUNICATIONLOG_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_logger_if.h>

#define ZONE_INIT ZONEMASK(10)
#define ZONE_FUNC ZONEMASK(11)
#define ZONE_MEM ZONEMASK(12)
#define ZONE_APP ZONEMASK(13)
#define ZONE__CWORD83__DEBUG ZONEMASK(14)
#define ZONE_CAN_DEBUG ZONEMASK(15)
#define ZONE_16 ZONEMASK(16)
#define ZONE_COMMSYS ZONEMASK(17)
#define ZONE_ICR ZONEMASK(18)
#define ZONE_ICR_TABLE ZONEMASK(19)
#define ZONE_TRANSLOG ZONEMASK(20)
#define ZONE_LOOPBACK ZONEMASK(22)
#define ZONE_ENTRY ZONEMASK(23)
#define ZONE_24 ZONEMASK(24)
#define ZONE_25 ZONEMASK(25)
#define ZONE_26 ZONEMASK(26)
#define ZONE_27 ZONEMASK(27)
#define ZONE_DEBUG ZONEMASK(28)
#define ZONE_INFO ZONEMASK(29)
#define ZONE_WARN ZONEMASK(30)
#define ZONE_ERR ZONEMASK(31)
#define ZONE_COMM_SYS_STS ZONEMASK(107)

#define ZONE_TEXT_10 "Init"
#define ZONE_TEXT_11 "Function"
#define ZONE_TEXT_12 "Memory"
#define ZONE_TEXT_13 "StateMachine Example"
#define ZONE_TEXT_14 "_CWORD83_"
#define ZONE_TEXT_15 "CAN"
#define ZONE_TEXT_16 ""
#define ZONE_TEXT_17 "COMSYS"
#define ZONE_TEXT_18 "ICR"
#define ZONE_TEXT_19 "ICR_TABLE"
#define ZONE_TEXT_20 "TRANSLOG"
#define ZONE_TEXT_21 ""
#define ZONE_TEXT_22 ""
#define ZONE_TEXT_23 ""
#define ZONE_TEXT_24 ""
#define ZONE_TEXT_25 ""
#define ZONE_TEXT_26 ""
#define ZONE_TEXT_27 ""
#define ZONE_TEXT_28 "Debug"
#define ZONE_TEXT_29 "Info"
#define ZONE_TEXT_30 "Warning"
#define ZONE_TEXT_31 "Error"

#ifndef FRAMEWORKUNIFIEDLOGOPTIONS
#define FRAMEWORKUNIFIEDLOGOPTIONS (LPRINT)  // LPRINT , LMSGQ, LSLOGGER
#endif

#ifndef FRAMEWORKUNIFIEDLOGAPPZONES
#define FRAMEWORKUNIFIEDLOGAPPZONES ZONE_APP, ZONE_ERR, ZONE_WARN, ZONE_INFO, ZONE_COMM_SYS, ZONE_CAN_FILTER, ZONE_COMM_SYS_STS
#endif

#endif  // COMMUNICATION_SERVER_INCLUDE_MAIN_COMMUNICATION_COMMUNICATIONLOG_H_
