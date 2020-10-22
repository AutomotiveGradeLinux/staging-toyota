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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SS_LoggerService
/// \brief    This file supports notifying CAN abou HK and CAN error events
///
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <time.h>
#include <stub/Clock_API.h>
#include "ss_logger_error_event_can_evt_reply.h"
CLoggerErrorEventCANEvtReply::CLoggerErrorEventCANEvtReply() {
}

CLoggerErrorEventCANEvtReply::~CLoggerErrorEventCANEvtReply() {  // LCOV_EXCL_START 14:globle instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CLoggerErrorEventCANEvtReply::Initialize(CLoggerCfg* f_pLoggerCfg) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

BOOL CLoggerErrorEventCANEvtReply::IsEventNeeded(TLoggerErrorEvent f_event) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_ret =
      ((f_event.EventBitMask & EVENT_BIT_CAN_NOTIFICATION) != 0) ? TRUE : FALSE;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_ret);
}

EFrameworkunifiedStatus CLoggerErrorEventCANEvtReply::PublishStartEvent(
    HANDLE f_hApp, TLoggerErrorEvent f_event) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (TRUE == IsEventNeeded(f_event)) {
    l_eStatus = FrameworkunifiedPublishPublicEvent(f_hApp, eSSLoggerCANEventStart, NULL, 0);
    LOG_STATUS_IF_ERRORED(
        l_eStatus,
        "FrameworkunifiedPublishPublicEvent(f_hApp,eSSLoggerCANEventStart,NULL,0)");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerErrorEventCANEvtReply::PublishEndEvent(
    HANDLE f_hApp, TLoggerErrorEvent f_event, UI_32 f_triggerNumber,
    uint32_t f_time) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (TRUE == IsEventNeeded(f_event)) {
    STEventCANLoggerEventInfo l_info;
    struct tm l_tm;
    CLOCK_RETURN clock_ret = Clock_getLocalTimeY2K38(&f_time, &l_tm);
    if (CLOCK_OK == clock_ret) {
      l_info.valid = TRUE;
      l_info.success = TRUE;
      l_info.triggerNumber = f_triggerNumber;
      l_info.dateAndTime.DateTime_Stat = 0;
      l_info.dateAndTime.DateTimeDay = static_cast<UI_8>(l_tm.tm_mday);
      l_info.dateAndTime.DateTimeHour = static_cast<UI_8>(l_tm.tm_hour);
      l_info.dateAndTime.DateTimeMinute = static_cast<UI_8>(l_tm.tm_min);
      l_info.dateAndTime.DateTimeMonth = static_cast<UI_8>(l_tm.tm_mon + 1);
      l_info.dateAndTime.DateTimeSecond = static_cast<UI_8>(l_tm.tm_sec);
      l_info.dateAndTime.DateTimeYear = static_cast<UI_8>(l_tm.tm_year % 100);
      l_info.dateAndTime.TimeFormat = 0;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             " Info. Time sent to CAN is: 20%2d/%2d/%2d %2d:%2d:%2d",
             l_info.dateAndTime.DateTimeYear, l_info.dateAndTime.DateTimeMonth,
             l_info.dateAndTime.DateTimeDay, l_info.dateAndTime.DateTimeHour,
             l_info.dateAndTime.DateTimeMinute,
             l_info.dateAndTime.DateTimeSecond);

    } else {
      l_info.success = TRUE;
      l_info.valid = FALSE;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Passed time is invalid.");
    }
    l_eStatus = FrameworkunifiedPublishPublicEvent(f_hApp, eSSLoggerCANEventFinished,
                                      &l_info, sizeof(l_info));

    LOG_STATUS_IF_ERRORED(
        l_eStatus,
        "FrameworkunifiedPublishPublicEvent(f_hApp,eSSLoggerCANEventFinished,NULL,0)");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerErrorEventCANEvtReply::PublishErrorEvent(
    HANDLE f_hApp, TLoggerErrorEvent f_event, BOOL f_isValid,
    UI_32 f_triggerNumber, uint32_t f_time) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (TRUE == IsEventNeeded(f_event)) {
    STEventCANLoggerEventInfo l_info;
    struct tm l_tm;
    CLOCK_RETURN clock_ret = Clock_getLocalTimeY2K38(&f_time, &l_tm);
    l_info.valid = f_isValid;
    l_info.success = FALSE;
    if (CLOCK_OK == clock_ret) {
      l_info.triggerNumber = f_triggerNumber;
      l_info.dateAndTime.DateTime_Stat = 0;
      l_info.dateAndTime.DateTimeDay = static_cast<UI_8>(l_tm.tm_mday);
      l_info.dateAndTime.DateTimeHour = static_cast<UI_8>(l_tm.tm_hour);
      l_info.dateAndTime.DateTimeMinute = static_cast<UI_8>(l_tm.tm_min);
      l_info.dateAndTime.DateTimeMonth = static_cast<UI_8>(l_tm.tm_mon + 1);
      l_info.dateAndTime.DateTimeSecond = static_cast<UI_8>(l_tm.tm_sec);
      l_info.dateAndTime.DateTimeYear = static_cast<UI_8>(l_tm.tm_year % 100);
      l_info.dateAndTime.TimeFormat = 0;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             " Info. Time sent to CAN is: 20%2d/%2d/%2d %2d:%2d:%2d",
             l_info.dateAndTime.DateTimeYear, l_info.dateAndTime.DateTimeMonth,
             l_info.dateAndTime.DateTimeDay, l_info.dateAndTime.DateTimeHour,
             l_info.dateAndTime.DateTimeMinute,
             l_info.dateAndTime.DateTimeSecond);

    } else {
      l_info.valid = FALSE;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Passed time is invalid.");
    }
    l_eStatus = FrameworkunifiedPublishPublicEvent(f_hApp, eSSLoggerCANEventFinished,
                                      &l_info, sizeof(l_info));
    LOG_STATUS_IF_ERRORED(  // LCOV_EXCL_BR_LINE 15: macro
        l_eStatus,
        "FrameworkunifiedPublishPublicEvent(f_hApp,eSSLoggerCANEventFinished,NULL,0)");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
