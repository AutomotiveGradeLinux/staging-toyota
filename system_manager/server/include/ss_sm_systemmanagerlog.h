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
/// \ingroup  tag_SystemManager
/// \brief    This file supports agl application logging.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef __SS_SM_SYSTEMMANAGERLOG_H__  // NOLINT
#define __SS_SM_SYSTEMMANAGERLOG_H__

#include <native_service/ns_logger_if.h>

#define ZONE_INIT           ZONEMASK(10)
#define ZONE_FUNC           ZONEMASK(11)
#define ZONE_MEM            ZONEMASK(12)
#define ZONE_FUNC_IPC       ZONEMASK(13)
#define ZONE_APP_LAUNCH     ZONEMASK(14)
#define ZONE_ACTIVE_INDEX   ZONEMASK(15)
#define ZONE_CONFIG_TABLE   ZONEMASK(16)
#define ZONE_FUNC_ARGS      ZONEMASK(17)
#define ZONE_LOG_FILE       ZONEMASK(18)
#define ZONE_MSG            ZONEMASK(19)
#define ZONE_TIMER          ZONEMASK(20)
#define ZONE_LAUNCH_TIME    ZONEMASK(21)
#define ZONE_22             ZONEMASK(22)
#define ZONE_UNUSED         ZONEMASK(23)
#define ZONE_PERIODIC_FUNC  ZONEMASK(24)
#define ZONE_PERIODIC_INFO  ZONEMASK(25)
#define ZONE_26             ZONEMASK(26)
#define ZONE_DYNAMIC_LNCH   ZONEMASK(27)
#define ZONE_STATE          ZONEMASK(28)
#define ZONE_INFO           ZONEMASK(29)

#define ZONE_SYSLOG           ZONE_SYSTEMDATA
#define ZONE_PERFORM          ZONE_PERFORMANCE
#define ZONE_PROC_LAUNCH_INFO ZONE_FUNC


#define ZONE_TEXT_10  "Init"
#define ZONE_TEXT_11  "Function"
#define ZONE_TEXT_12  "Memory"
#define ZONE_TEXT_13  "Ipc Function"
#define ZONE_TEXT_14  "Active Index"
#define ZONE_TEXT_15  ""
#define ZONE_TEXT_16  "Config Table"
#define ZONE_TEXT_17  ""
#define ZONE_TEXT_18  ""
#define ZONE_TEXT_19  ""
#define ZONE_TEXT_20  ""
#define ZONE_TEXT_21  ""
#define ZONE_TEXT_22  ""
#define ZONE_TEXT_23  ""
#define ZONE_TEXT_24  ""
#define ZONE_TEXT_25  ""
#define ZONE_TEXT_26  ""
#define ZONE_TEXT_27  ""
#define ZONE_TEXT_28  "Ipc Info"
#define ZONE_TEXT_29  "Info"
// These ARE RESERVED AND SHOULDN'T BE TOUCHED
// USE THEM WHEN REALLY NEEDED!!!!!
#define ZONE_TEXT_30  "Warning"
#define ZONE_TEXT_31  "Error"
#define ZONE_WARN     ZONEMASK(30)
#define ZONE_ERR      ZONEMASK(31)
#define ALL_ZONES_BUT_IPC_INFO 0xEFFFFFFF

#ifndef  FRAMEWORKUNIFIEDLOGOPTIONS
#define FRAMEWORKUNIFIEDLOGOPTIONS (LMSGQ)  // LPRINT , LMSGQ, LSLOGGER
#endif


#ifndef FRAMEWORKUNIFIEDLOGAPPZONES
#define FRAMEWORKUNIFIEDLOGAPPZONES ZONE_WARN, ZONE_ERR, ZONE_STATE, ZONE_SYSLOG, ZONE_PERFORM, ZONE_NS_WAR, ZONE_NS_ERR
#endif  // FRAMEWORKUNIFIEDLOGAPPZONES

#endif  // __SS_SM_SYSTEMMANAGERLOG_H__  // NOLINT
