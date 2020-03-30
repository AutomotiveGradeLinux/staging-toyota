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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    structures used by ns_logger.cpp for event and counter logging
///
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @brief \~english structures used by ns_logger.cpp for event and counter logging
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
 *  @ingroup Native_service
 *  @{
 */
/** @addtogroup native
 *  @ingroup framework_unified
 *  @{
 */
#ifndef  __NATIVESERVICES_EVENTLOGGER_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_EVENTLOGGER_H__


#ifdef __cplusplus
extern "C" {
#endif

// event log messageQ name
#define EVENTLOG_MSGQ_NAME          "/EvntLogQue"

typedef enum _Datatype_ {
  COMMON,
  EVENT_SPECIFIC
} Datatype;

typedef enum _SystemPhase_ {
  STARTUP = 0x21,
  NORMAL = 0x22,
  SHUTDOWN = 0x23,
} SystemPhase;

typedef enum _NStoSS_LOGGERSERVICEPROTOCOL {
  SS_MSG_EVTLOG = 6463,               // NS to Event Logger Thread
  SS_MSG_LOGGERCNT,                   // NS to Event logger Thread
  SS_MSG_LOGGER_CNT_EVTLOG    // NS to event logger thread
} NStoSS_loggerserviceprotocol;

#pragma pack(1)
typedef struct _stLogEvent_ {
  UI_32 ts;
  UI_8 grp_ID;
  UI_8 event_id;
  UI_8 data[4];
  SystemPhase phase;
  UI_16 cnt_ID;
  Datatype typeofdata;
} st_LogEvent;
#pragma pack(0)

#pragma pack(1)
typedef struct _stLogCount_ {
  SystemPhase phase;
  UI_16 cnt_id;
} st_LogCount;
#pragma pack(0)


#ifdef __cplusplus
}
#endif

#endif /* __NATIVESERVICES_EVENTLOGGER_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
