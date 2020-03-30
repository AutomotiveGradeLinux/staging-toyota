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
/// \ingroup  tag_NS_LogTraceSender
/// \brief    frameworkunifieddebug writer thread class, handles writing a log file
///           all really done by my_writer class.
///
///////////////////////////////////////////////////////////////////////////////
// System Headers
#include "loggerservicedebug_writer_Evntworker.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/timeb.h>
#include <boost/bind.hpp>
#include <errno.h>
#include <native_service/ns_np_service_protocol.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/frameworkunified_timer.h>
#include <loggerservicedebug_thread_if.h>
#include <loggerservicedebug_loggerservicelog.h>
#include <system_service/ss_logger_service_notifications.h>
#include <system_service/ss_devicedetection_service_ifc.h>
#include <system_service/ss_logger_service_protocol.h>
#include <system_service/ss_logger_service.h>
#include <system_service/ss_templates.h>
//#include <vehicle_service/Clock_API.h>
#include <new>
#include <utility>
#include <string>

/// Macros definition for the constants
#define SIZE_0 (size_t)0
#define SIZE_1 (size_t)1
#define COUNTER_SIZE 6            ///< Counter ID 2 bytes and Counter Value is 4 bytes
#define SINGLE_EVENT_INFO_SIZE 10    ///< Event log size
#define INITIAL_CNT_VALUE 0x00000001  ///< Init count value for counter logging
#define MAX_CNT_VALUE 0xFFFFFFFF    ///< Max count calue for counter logging
#define MAX_USB_DEVICE 2    ///< Max USB devices

// Temp path for event logs that used for copying to USB
const CHAR TMP_DEBUG_EVNTLOG_PATH_FN[] = "/tmp/loggerservice_Evntdebug.dat";

// Structure to hold the USB device information
TThrdEvtLogStore gSUSBDevInfo[MAX_USB_DEVICE] = { };

// Structure to hold the SD card device information
TThrdEvtLogStore gSSDCardevInfo = { };

// Object for device detection class
DeviceDetectionServiceIf m_device_detector_obj;

// Map container  for startup phase counter
CounterInformation CEvntWriterWorker::counter;

// Deque container for event logging
DEQUE_Event_Type CEvntWriterWorker::deque_event_info;

static CEvntWriterWorker * pObj = new (std::nothrow) CEvntWriterWorker();  // LCOV_EXCL_BR_LINE 11:Unexpected branch
static TEvntWriterInfo wi = { };

// Event Common Info global structure
extern UEvtLoggerCommonInfo g_uEvtLoggerCommonInfo;
extern UI_16 g_u16DiagId;
static EFrameworkunifiedStatus ReadEventLogQueue(HANDLE hThread);
// created global variables as they are used in non member function
HANDLE g_hReceive = NULL;
HANDLE g_hEvtLogTimer = NULL;

CEvntWriterWorker::CEvntWriterWorker()
    : m_u32MileageData(0),
      m_u32NumberEventsLogged(0) {
  memset(m_stVINnumber.VINstr, 0, m_stVINnumber.VIN_LEN);
}

CEvntWriterWorker::~CEvntWriterWorker() {  // LCOV_EXCL_START 14:global instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CEvntWriterWorker::Initialize(HANDLE hThread) {
  return eFrameworkunifiedStatusOK;
}

///////////////////////////////////////////////////////////////////////
/// Function :EventLogRegisterCbHandlers
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::EventLogRegisterCbHandlers(HANDLE hThread) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  FrameworkunifiedProtocolCallbackHandler aParentHandlers[] = { {
      eThrdCmdWriteEvntFilesToUsb, boost::bind(
          &CEvntWriterWorker::OnCmdWriteEventFilesToUsb, this, _1) }, {
      eThrdCmdClearEvntLogs, boost::bind(
          &CEvntWriterWorker::OnCmdClearEventLogs, this, _1) }, {
      eThrdCmdStatisticalCounter, boost::bind(
          &CEvntWriterWorker::OnCmdReadStatisticalCounter, this, _1) }, {
      eThrdCmdResetStatisticalCntrs, boost::bind(
          &CEvntWriterWorker::OnCmdResetStatisticalCounter, this, _1) }, {
      eThrdCmdSetVINnumber, boost::bind(&CEvntWriterWorker::OnCmdSetVIN, this,
                                        _1) }, { eThrdCmdImmPersistEvtLog,
      boost::bind(&CEvntWriterWorker::immediate_persist_event_log, this, _1) },
      { eThrdCmdMileageData, boost::bind(&CEvntWriterWorker::EvtThd_SetMileage,
                                         this, _1) }, {
          eThrdCmdGetNumberOfEventsLogged, boost::bind(
              &CEvntWriterWorker::OnCmdReadNumberOfEventsLogged, this, _1) }, {
          eThrdCmdUploadEventLog, boost::bind(
              &CEvntWriterWorker::OnCmdUploadEventLog, this, _1) } };

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = FrameworkunifiedAttachParentCallbacksToDispatcher(
          hThread, aParentHandlers, _countof(aParentHandlers)))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error: parent call backs attach failed , err code: %d", l_eStatus);
  }

  // TO DO replace the queue name with macro EVENTLOG_MSGQ_NAME, after NS delivers it.
  if (NULL != (g_hReceive = McOpenReceiverNotBlocked("/EvntLogQue"))) {
    if (NULL
        == (g_hEvtLogTimer = FrameworkunifiedAttachTimerCallback(hThread, 100, 1000,
                                                    ReadEventLogQueue))) {
      LOG_ERROR("FrameworkunifiedAttachTimerCallback()");
    }
  } else {
    LOG_ERROR("McOpenReceiverNotBlocked");
  }

  return l_eStatus;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus ReadEventLogQueue(HANDLE hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  CHAR source[MAX_QUEUE_NAME_SIZE];
  UI_32 l_cmd = 0;
  UI_8 pbuf[MAX_QUEUE_MSG_SIZE];

  if (NULL != g_hReceive) {  // LCOV_EXCL_BR_LINE 6: g_hReceive is always null
    // LCOV_EXCL_START 8: dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    while (1) {
      if (eFrameworkunifiedStatusOK != (l_eStatus = McReceive(g_hReceive, source, &l_cmd,
      MAX_QUEUE_MSG_SIZE,
                                                 pbuf))) {
        break;
      } else {
        switch (l_cmd) {
          case SS_MSG_EVTLOG:
            if (pObj) {
              if (eFrameworkunifiedStatusOK != (l_eStatus = pObj->OnCmdEventLog(pbuf))) {
                LOG_ERROR("OnCmdEventLog()");
              }
            } else {
              LOG_ERROR("pObj NULL");
            }
            break;

          case SS_MSG_LOGGERCNT:
            if (pObj) {
              if (eFrameworkunifiedStatusOK != (l_eStatus = pObj->OnCmdIncrcount(pbuf))) {
                LOG_ERROR("OnCmdIncrcount()");
              }
            } else {
              LOG_ERROR("pObj NULL");
            }
            break;

          case SS_MSG_LOGGER_CNT_EVTLOG:
            if (pObj) {
              if (eFrameworkunifiedStatusOK
                  != (l_eStatus = pObj->OnCmdIncrwriteevent(pbuf))) {
                LOG_ERROR("OnCmdIncrwriteevent()");
              }
            } else {
              LOG_ERROR("pObj NULL");
            }
            break;

          default:
            break;
        }
      }
    }
    // LCOV_EXCL_STOP
  } else {
    LOG_ERROR("Receive Handle NULL");  // LCOV_EXCL_BR_LINE 15:macro
  }

  if (eFrameworkunifiedStatusErrNoEAGAIN == l_eStatus) {  // LCOV_EXCL_BR_LINE 6: l_eStatus is always eFrameworkunifiedStatusOK
    l_eStatus = eFrameworkunifiedStatusOK;  // LCOV_EXCL_LINE 6: l_eStatus is always eFrameworkunifiedStatusOK
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
///////////////////////////////////////////////////////////////////////
/// Function :EvntWriterWorkerOnStart
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus EvntWriterWorkerOnStart(HANDLE hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedGetMsgDataOfSize(hThread, (PVOID) &wi, sizeof(wi), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error: Unable to read handle from FrameWork Thread.%d", eStatus);
    // LCOV_EXCL_STOP
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           "Successfully read handle from FrameWork Thread.\n");  // LCOV_EXCL_BR_LINE 15:macro
  }

  // Device Detection Obj initialization
  if (m_device_detector_obj.Initialize(hThread)) {  // LCOV_EXCL_BR_LINE 200:To ensure success
    memset(&gSUSBDevInfo, 0, sizeof(gSUSBDevInfo));
    memset(&gSSDCardevInfo, 0, sizeof(gSSDCardevInfo));

    if (eFrameworkunifiedStatusOK != (eStatus = m_device_detector_obj.NotifyOnDeviceDetectionAvailability( DD_USBServiceAvailabilityCallBack))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: ServiceAvailabilityCallback registration failed");
    }

    if (eFrameworkunifiedStatusOK != (eStatus = m_device_detector_obj.NotifyOnOpenSessionAck( DD_USBOpenSessionAckCallBack))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: OpenSessionAckCallback registration failed");
    }

    if (eFrameworkunifiedStatusOK != (eStatus = m_device_detector_obj.NotifyOnCloseSessionAck( DD_USBCloseSessionAckCallBack))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: CloseSessionCallback registration failed");
    }
  } else {
    // LCOV_EXCL_START 200:To ensure success
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error: Device Detection Object Initialization failed!");
    // LCOV_EXCL_STOP
  }

  if (pObj) {  // LCOV_EXCL_BR_LINE 200:As it is always TRUE
    FrameworkunifiedSetThreadSpecificData(hThread, pObj);
    if (eFrameworkunifiedStatusOK != (eStatus = pObj->Initialize(hThread))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: CEvntWriterWorker Initialize Failed");
    }
  } else {
    // LCOV_EXCL_START 200:As it is always TRUE
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "pObj is NULL");
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :EvntWriterWorkerOnStop
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus EvntWriterWorkerOnStop(HANDLE hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  if (pObj) {  // LCOV_EXCL_BR_LINE 200:As it is always TRUE
    if (NULL != g_hEvtLogTimer) {  // LCOV_EXCL_BR_LINE 200:As it is always NULL
      // LCOV_EXCL_START 200:As it is always NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      CALL_AND_LOG_STATUS(FrameworkunifiedDetachTimerCallback(hThread, g_hEvtLogTimer));
      g_hEvtLogTimer = NULL;
      // LCOV_EXCL_STOP
    }

    /// To Read all msgs of EventLog queue on shutdown
    CALL_AND_LOG_STATUS(ReadEventLogQueue(hThread));  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    /// Close ReadEventLog Queue receive handle
    if (NULL != g_hReceive) {  // LCOV_EXCL_BR_LINE 200:As it is always NULL
      // LCOV_EXCL_START 200:As it is always NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      CALL_AND_LOG_STATUS(McClose(g_hReceive));
      g_hReceive = NULL;
      // LCOV_EXCL_STOP
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdIncrcount_phase
///////////////////////////////////////////////////////////////////////
VOID CEvntWriterWorker::OnCmdIncrcount_phase(st_LogCount *cnt) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");

  if (NULL != cnt) {
    EStatCounterGroupID group;
    UI_16 counterID = ((*cnt).cnt_id);
    group = static_cast<EStatCounterGroupID>((counterID >> 8) & 0x00FF);
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,
           "Increment counter(%04X) for group(%02X) by one", counterID, group);
    counterIncrementByValue(group, counterID, 0x01u);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "Function call without parameter");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdIncrcount
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::OnCmdIncrcount(UI_8 *pbuf) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  st_LogCount cnt;

  if (eFrameworkunifiedStatusOK
      == (eStatus = McGetDataOfSize(pbuf, (PVOID) &cnt, sizeof(cnt)))) {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__,
           "call increment function cnt.phase=%x,cnt.ID=%x", cnt.phase,
           cnt.cnt_id);
    OnCmdIncrcount_phase(&cnt);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "Not recieved data from q");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdIncrwriteevent
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::OnCmdIncrwriteevent(UI_8* pbuf) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  st_LogEvent EC;
  st_LogEvent_ss ev = { };
  st_LogCount C;

  if (eFrameworkunifiedStatusOK
      == (eStatus = McGetDataOfSize(pbuf, (PVOID) &EC, sizeof(EC)))) {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "Successful increment");
    ev.event_id = EC.event_id;
    ev.data[0] = EC.data[0];
    ev.data[1] = EC.data[1];
    ev.data[2] = EC.data[2];
    ev.data[3] = EC.data[3];
    OnCmdWriteEventLogs(&ev);
    C.phase = (SystemPhase) (EC.phase);
    C.cnt_id = EC.cnt_ID;
    OnCmdIncrcount_phase(&C);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "Not recieved data from q");
  }
  return eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdWriteEventLogs
///////////////////////////////////////////////////////////////////////
VOID CEvntWriterWorker::OnCmdWriteEventLogs(st_LogEvent_ss *ev) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  UI_32 u32deque_size = 0;
  u32deque_size = static_cast<UI_32>(deque_event_info.size());
  u32deque_size = u32deque_size * SINGLE_EVENT_INFO_SIZE;  // Existing Event Info size
  u32deque_size += SINGLE_EVENT_INFO_SIZE;
  //  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,"Deque size before write:%X ",u32deque_size);

  if (u32deque_size > MAX_EVENTLOG_SIZE) {
//    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,"Deque size will be exceeded after new event:%X ",u32deque_size);
//    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,"Removed the oldest event and add latest event in the the end");
    deque_event_info.pop_front();
    u32deque_size = static_cast<UI_32>(deque_event_info.size());
//    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,"Deque size after pop:%X ",(u32deque_size*SINGLE_EVENT_INFO_SIZE));
  }
  deque_event_info.push_back(*ev);
//  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,"Deque size after write:%X ",
//  (deque_event_info.size()*SINGLE_EVENT_INFO_SIZE));
//  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
}
// LCOV_EXCL_STOP

UI_32 LS_ConvertEndian(UI_32 * pvar) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  UI_32 retVal = (UI_32) 0;
  UI_32 var = *pvar;
  retVal = (((var & 0xFF000000) >> 24) | ((var & 0x00FF0000) >> 8)
      | ((var & 0x0000FF00) << 8) | ((var & 0x000000FF) << 24));
  return (retVal);
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdEventLog
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::OnCmdEventLog(UI_8* pbuf) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
//  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");
  st_LogEvent_ss ev = { };
  st_LogEvent_full ev_full = { };
  UI_32 l_mileage;
  if (eFrameworkunifiedStatusOK
      == (eStatus = McGetDataOfSize(pbuf, &ev_full, sizeof(ev_full)))) {
    if (0 != ev_full.grp_ID) {
      if (0x01 == ev_full.grp_ID) {
        // copied to local var as m_u32MileageData value is persisted
        l_mileage = m_u32MileageData;
        ev.ts = LS_ConvertEndian(&l_mileage);
      } else {
        ev.ts = ev_full.ts;
      }
      ev.grp_ID = ev_full.grp_ID;
      ev.event_id = ev_full.event_id;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "TS %u, GRPID %d, EVTID %d", ev.ts,
             ev.grp_ID, ev.event_id);
      // FRAMEWORKUNIFIEDLOG_EVT(Zone,EventId,4,data0,data1,data2,data3)
      // Data in tool should be represented in Little Endian format means data3(MSB)..data0(LSB)
      // In the COMMON_DATA, 1st element of structure u_stEvtLoggerCommonInfo is stored in the CommonData[0],
      // ie BodayCAN_Stat,
      // in the data array, byte0=>MSB, byte3=>LSB in the tool
      if (ev_full.typeofdata == COMMON_DATA) {
        for (UI_8 j = 0; j < 3; j++) {
          ev.data[j] = g_uEvtLoggerCommonInfo.CommonData[j];
        }
        // byte0 param
        ev.data[3] = ev_full.data[0];
      } else {
        // in array data[0] => MSB in tool, data[3] => LSB in the tool
        for (UI_8 i = 4, j = 0; i > 0 && j < 4; i--, j++) {
          ev.data[i - 1] = ev_full.data[j];
        }
      }
      OnCmdWriteEventLogs(&ev);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: Invalid event");
    }
  }
//  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdStop
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::OnCmdStop(HANDLE hThread) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CEvntWriterWorker::OnCmdStart(HANDLE hThread) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :read_count_from_file
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::readCountFromFile(PCSTR filename) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  UI_32 counter_ID = 0;
  UI_32 counter_val = 0;
  FILE* fp;
  if (filename != NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Open counter file:%s", filename);
    if (NULL != (fp = ::fopen(filename, "r"))) {
      while (!feof(fp)) {
        EStatCounterGroupID group;
        UI_8 groupID;
        UI_8 count = static_cast<UI_8>(::fscanf(fp, "%04X%08X", &counter_ID, &counter_val));
        groupID = (counter_ID >> 8) & 0xFF;
        FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Read: 0x%04X 0x%08X", counter_ID,
               counter_val);
        if ((count == 2u)
            && (groupID >= static_cast<UI_8>(STARTUP_SHUTDOWN_COUNTER_GROUP))
            && (groupID <= static_cast<UI_8>(APP_USAGE_COUNTER_GROUP))) {
          // Check if group is valid before casting it o EStatCounterGroup
          group = static_cast<EStatCounterGroupID>(groupID);
          counterIncrementByValue(group, static_cast<UI_16>(counter_ID), counter_val);
        }
      }
      ::fclose(fp);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Normal File failed to open");
      eStatus = eFrameworkunifiedStatusFileLoadError;
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Filename is NULL-pointer");
    eStatus = eFrameworkunifiedStatusAccessError;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 8: dead code
EFrameworkunifiedStatus CEvntWriterWorker::writeGroupToFile(FILE *fp,
                                               EStatCounterGroupID group) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  if (fp != NULL) {
    CounterInformation::iterator countInfIt = counter.find(group);
    if (countInfIt != counter.end()) {
      Counter count = countInfIt->second;
      if (!count.empty()) {
        Counter::iterator countIt;
        for (countIt = count.begin(); countIt != count.end(); ++countIt) {
          ::fprintf(fp, "%04X%08X\n", countIt->first, countIt->second);
        }
        FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Written %d entries for Group 0x%02X",
               (UI_8)count.size(), (UI_8)group);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Group 0x%02X is empty", (UI_8)group);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Group not found in map");
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Parameter fp is NULL-pointer");
    eStatus = eFrameworkunifiedStatusAccessError;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :writeCountToFile
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::writeCountToFile(PCSTR filename) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FILE *fp;
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CounterInformation::iterator countInfIt;
  if (0 == strcmp(filename, Counter_LOG_PATH_FN)) {
    if (NULL != (fp = ::fopen(filename, "w"))) {
      for (countInfIt = counter.begin(); countInfIt != counter.end();
          ++countInfIt) {
        (void) writeGroupToFile(fp, countInfIt->first);
      }
      ::fclose(fp);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: Not valid file");
    eStatus = eFrameworkunifiedStatusErrNoEBADF;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :read_events_from_buffer
///////////////////////////////////////////////////////////////////////
// LCOV_EXCL_START 8: dead code
VOID CEvntWriterWorker::read_events_from_buffer(
    STEventLogPersistBuffer* f_pstEvtLogBuf) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  deque_event_info.clear();
  for (int i = 0; i < (f_pstEvtLogBuf->Current_Log_Size); i++) {
    if (i < f_pstEvtLogBuf->EVT_BUFMAXSIZE) {
      deque_event_info.push_back(f_pstEvtLogBuf->EvtLog_Buffer[i]);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: EventLog buffer size greater than EVT_BUFMAXSIZE");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Size of DEQUE event is :%d",
         static_cast<int>(deque_event_info.size()));
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :write_events_to_buffer
///////////////////////////////////////////////////////////////////////
// LCOV_EXCL_START 8: dead code
VOID CEvntWriterWorker::write_events_to_buffer(
    STEventLogPersistBuffer* f_pstEvtLogBuf) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  st_LogEvent_ss l_event_info;
  DEQUE_Event_Type::iterator deque_it;
  deque_it = deque_event_info.begin();
  int i = 0;

  memset(f_pstEvtLogBuf, 0x00, sizeof(STEventLogPersistBuffer));
  while (deque_it != deque_event_info.end()) {
    if (0x00 != deque_it->grp_ID) {
      l_event_info.ts = deque_it->ts;
      l_event_info.grp_ID = deque_it->grp_ID;
      l_event_info.event_id = deque_it->event_id;
      memcpy(l_event_info.data, deque_it->data, sizeof(l_event_info.data));
//      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__,"Event record is:%X ",++EventRecordCnt);
      if (i < f_pstEvtLogBuf->EVT_BUFMAXSIZE) {
        memcpy(&f_pstEvtLogBuf->EvtLog_Buffer[i], &l_event_info,
               sizeof(l_event_info));
      }
    }
    deque_it++;
    i++;
  }
  f_pstEvtLogBuf->Current_Log_Size = static_cast<UI_16>(deque_event_info.size());
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Size of DEQUE event is :%d",
         static_cast<int>(deque_event_info.size()));
//  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :write_event_to_file
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::write_event_to_file(const CHAR* filename) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  UI_32 u32File_Size;
  st_LogEvent_ss Sevent_Info;
  DEQUE_Event_Type::iterator deque_it;
  FILE *fp;

  if (NULL != (fp = ::fopen(filename, "wb"))) {
    deque_it = deque_event_info.begin();
    while (deque_it != deque_event_info.end()) {
      Sevent_Info.ts = deque_it->ts;
      Sevent_Info.grp_ID = deque_it->grp_ID;
      Sevent_Info.event_id = deque_it->event_id;
      memcpy(Sevent_Info.data, deque_it->data, sizeof(Sevent_Info.data));

      if (0x00 != deque_it->grp_ID) {
        if (SIZE_1
            != (u32File_Size = static_cast<UI_32>(::fwrite(&Sevent_Info, sizeof(Sevent_Info), 1,
                                        fp)))) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: Writing in to file is failed");
          eStatus = eFrameworkunifiedStatusFail;
          break;
        }
      }
      deque_it++;
    }
    ::fclose(fp);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: File failed to open");
    eStatus = eFrameworkunifiedStatusFileLoadError;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdClearEventLogs
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::OnCmdClearEventLogs(HANDLE hThread) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TSessionData l_stSessionData;
  TClearEvntLogCmdResponse l_stCmdResponse;

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg<TSessionData>(hThread,
                                            l_stCmdResponse.stSessiondata))) {
    LOG_ERROR("ReadMsg()");
  } else {
    deque_event_info.clear();
    if (eFrameworkunifiedStatusOK != (l_eStatus = immediate_persist_event_log(hThread))) {
      l_stCmdResponse.u8Response = (UI_8) CLEAR_EVENT_LOG_FAILED;
    } else {
      l_stCmdResponse.u8Response = (UI_8) CLEAR_EVENT_LOG_SUCCESS;
    }
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendParent(hThread, eThrdCmdClearEventLogResponse,
                                      sizeof(TClearEvntLogCmdResponse),
                                      &l_stCmdResponse))) {
      LOG_ERROR("FrameworkunifiedSendParent()");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP 8

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdSetVIN
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::OnCmdSetVIN(HANDLE hThread) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  STVIN_NUMBER l_stVIN_Number;

  if (eFrameworkunifiedStatusOK
      == (l_eStatus = FrameworkunifiedGetMsgDataOfSize(hThread, &l_stVIN_Number,
                                          sizeof(l_stVIN_Number), eSMRRelease))) {
    memcpy(&m_stVINnumber, &l_stVIN_Number, sizeof(STVIN_NUMBER));
  }

  return l_eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdGetVIN
///////////////////////////////////////////////////////////////////////
VOID CEvntWriterWorker::OnCmdGetVIN(STVIN_NUMBER& f_stVIN_Number) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  memcpy(&f_stVIN_Number, &m_stVINnumber, sizeof(STVIN_NUMBER));
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdWriteEventFilesToUsb
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::OnCmdWriteEventFilesToUsb(HANDLE hThread) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TWriteFilesToUsbCmdData l_stCmdData;
  TWriteFilesToUsbCmdResponse l_stCmdResponse;

  TThrdEvtLogStore evt = { };
  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg<TWriteFilesToUsbCmdData>(hThread, l_stCmdData))) {
    LOG_ERROR("ReadMsg()");
  } else {
    if ((USB0 == l_stCmdData.eDevId) || (USB1 == l_stCmdData.eDevId)) {
      memcpy(&evt, &gSUSBDevInfo[l_stCmdData.eDevId], sizeof(TThrdEvtLogStore));
    } else if (SD == l_stCmdData.eDevId) {
      memcpy(&evt, &gSSDCardevInfo, sizeof(TThrdEvtLogStore));
    } else {
      l_eStatus = eFrameworkunifiedStatusInvldID;
      LOG_ERROR("Invalid DeviceId()");
    }
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           "USB dev info:Available %d, Location %s, Parse %s ",
           evt.IsDeviceAvailable, evt.parse_fn, evt.location);
    l_stCmdResponse.u8Response = copy_event_files_to_usb(&evt);
    l_stCmdResponse.stSessiondata.strSrcName = l_stCmdData.stSessiondata.strSrcName;
    l_stCmdResponse.stSessiondata.session_id = l_stCmdData.stSessiondata.session_id;
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendParent(hThread, eThrdCmdCopyEventLogUSBResponse,
                                      sizeof(TWriteFilesToUsbCmdResponse),
                                      &l_stCmdResponse))) {
      LOG_ERROR("FrameworkunifiedSendParent()");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdReadStatisticalCounter
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::OnCmdReadStatisticalCounter(HANDLE hThread) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  SStatisticalCounter buffer = { 0 };
  TStatisticalCntCmdSuccessResp l_stCmdSuccessResp;

  if (hThread != NULL) {
    TStatisticalCountersCmd l_stCmdData;
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = ReadMsg<TStatisticalCountersCmd>(hThread, l_stCmdData))) {
      LOG_ERROR("ReadMsg()");
    } else {
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = ReadStatisticalCounter(l_stCmdData.eGroupId, buffer))) {
        LOG_ERROR("ReadStatisticalCounter()");
        if (eFrameworkunifiedStatusOK
            != (l_eStatus = FrameworkunifiedSendParent(hThread,
                                          eThrdCmdStatisticalCounterErrorResp,
                                          sizeof(TSessionData),
                                          &l_stCmdData.stSessiondata))) {
          LOG_ERROR("FrameworkunifiedSendParent()");
        }
      } else {
        (VOID) memcpy(&l_stCmdSuccessResp.stBuffer, &buffer,
                      sizeof(SStatisticalCounter));
        (VOID) memcpy(&l_stCmdSuccessResp.stSessiondata,
                      &l_stCmdData.stSessiondata, sizeof(TSessionData));
        if (eFrameworkunifiedStatusOK
            != (l_eStatus = FrameworkunifiedSendParent(hThread,
                                          eThrdCmdStatisticalCounterSuccessResp,
                                          sizeof(TStatisticalCntCmdSuccessResp),
                                          &l_stCmdSuccessResp))) {
          LOG_ERROR("FrameworkunifiedSendParent()");
        }
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :ReadStatisticalCounter
///////////////////////////////////////////////////////////////////////
// LCOV_EXCL_START 8: dead code
EFrameworkunifiedStatus CEvntWriterWorker::ReadStatisticalCounter(
    EStatCounterGroupID eCounterGroup, SStatisticalCounter& buffer) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  UI_32 size = 0u;

  CounterInformation::iterator countInfIt = counter.find(eCounterGroup);
  if (counter.end() != countInfIt) {
    Counter count = countInfIt->second;
    Counter::iterator countIt;
    if (((UI_16) count.size()) <= 40) {
      for (countIt = count.begin(); countIt != count.end(); ++countIt) {
        UI_16 counterID = countIt->first;
        UI_32 counterVal = countIt->second;
        u16copy(&buffer.StatisticalCountBuffer[size], counterID);
        size += static_cast<UI_16>( sizeof(counterID));

        counterVal = countIt->second;
        u32copy(&buffer.StatisticalCountBuffer[size], counterVal);
        size += static_cast<UI_16>( sizeof(counterVal));
      }
    } else {
      eStatus = eFrameworkunifiedStatusInvldBuf;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: Too many counter elements %02d > 42 ", static_cast<int>(count.size()));
    }
  } else {
    /*OK, here: empty buffer will be returned*/
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdResetStatisticalCounter
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::OnCmdResetStatisticalCounter(HANDLE hThread) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TStatisticalCountersCmd l_stCmdData;

  if (hThread != NULL) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = ReadMsg<TStatisticalCountersCmd>(hThread, l_stCmdData))) {
      LOG_ERROR("ReadMsg()");
    } else {
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = ResetStatisticalCounter(l_stCmdData.eGroupId))) {
        LOG_ERROR("ResetStatisticalCounter()");
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP
///////////////////////////////////////////////////////////////////////
/// Function :ResetStatisticalCounter
///////////////////////////////////////////////////////////////////////
// LCOV_EXCL_START 8: dead code
EFrameworkunifiedStatus CEvntWriterWorker::ResetStatisticalCounter(
    EStatCounterGroupID eCounterGroup) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  CounterInformation::iterator countInfIt;
  countInfIt = counter.find(eCounterGroup);
  if (counter.end() != countInfIt) {
    countInfIt->second.clear();
    counter.erase(countInfIt->first);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Erased group: 0x%02X from map",
           (UI_8)eCounterGroup);
  } else {
    l_eStatus = eFrameworkunifiedStatusInvldID;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :_GetCurrentTimeString
///////////////////////////////////////////////////////////////////////
PSTR _GetCurrentTimeString(PCHAR TempFileName, PSTR f_pstrVIN) { // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  uint32_t rawtime;
  uint8_t status;
  struct tm local_time;
  CHAR tbuf[30];
  UI_8 l_len = 0;
  std::string l_strVIN = f_pstrVIN;

  // Time string is based on current date and time. Time string format is <year-mon-dayThhmm>.
//  Clock_getSystemTimeY2K38(&rawtime, &status);
//  Clock_getLocalTimeY2K38(&rawtime, &local_time);
  if (0
      != (l_len = static_cast<UI_8>(strftime(tbuf, sizeof(tbuf), "%Y-%m-%dT%H%M",
                           &local_time)))) {
    sprintf(TempFileName, "%s_%s_%04X.dat", l_strVIN.c_str(), tbuf,  // NOLINT (runtime/printf)
            g_u16DiagId);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "EventLog File Name :%s, tbuf_len = %d",
         TempFileName, l_len);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return TempFileName;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :copy_event_files_to_usb
///////////////////////////////////////////////////////////////////////
UI_8 CEvntWriterWorker::copy_event_files_to_usb(TThrdEvtLogStore *pStUSBIndo) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  CHAR buffer[10];
  CHAR TempFileName[MAX_SYS_INFO_SIZE];
  STVIN_NUMBER l_stVIN_Number;
  std::string _filename("");
  UI_8 u8Result = COPY_EVT_USB_SUCCESS;

  // Initializing all Char arrays.
  memset(buffer, 0, 10);
  memset(TempFileName, 0, MAX_SYS_INFO_SIZE);

  if (pStUSBIndo->IsDeviceAvailable == TRUE) {
    _filename = pStUSBIndo->location;
    OnCmdGetVIN(l_stVIN_Number);
    _filename += _GetCurrentTimeString(TempFileName, l_stVIN_Number.VINstr);

    if (eFrameworkunifiedStatusOK != (l_eStatus = write_event_to_file(_filename.c_str()))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: Write to file failed:%d",
             l_eStatus);
      u8Result = (UI_8) USB_DEVICE_WRITE_ERROR;
    }

  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error: Device not available, so event logs can not be copied");
    u8Result = (UI_8) USB_DEVICE_NOT_AVAILABLE;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");
  return u8Result;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :DD_USBSrvDetectionCallBack
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus DD_USBSrvDetectionCallBack(HANDLE hApp) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "+");

  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  SS_MediaDetectInfo tMediaDetectInfo = { };
  std::string FilePathStr;
  UI_8 u8Pos = 0;

  if (eFrameworkunifiedStatusOK
      != (eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &tMediaDetectInfo,
                                        sizeof(tMediaDetectInfo)))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
           "MediaDetectCallBack FrameworkunifiedGetMsgDataOfSize Failed Status:0x%x ",
           eStatus);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Device = %d", tMediaDetectInfo.dev_type);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Availability = %d",
         tMediaDetectInfo.bIsDeviceAvailable);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " device path = %s",
         tMediaDetectInfo.deviceMountpath);
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " File path = %s",
         tMediaDetectInfo.filepathName);

  FilePathStr.append(tMediaDetectInfo.deviceMountpath);

  if (tMediaDetectInfo.dev_type == eUSB) {
    if (tMediaDetectInfo.bIsDeviceAvailable == TRUE) {
      u8Pos = static_cast<UI_8>(FilePathStr.find_last_of("/\\"));

      if (FilePathStr.compare(u8Pos + 1, 12, "usb-00-d0-p0") == 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "USB1 Detected ");
        gSUSBDevInfo[0].IsDeviceAvailable = TRUE;
        snprintf(gSUSBDevInfo[0].parse_fn, gSUSBDevInfo[0].FN_LEN, "%s/",
                 FilePathStr.c_str());
        snprintf(gSUSBDevInfo[0].location, gSUSBDevInfo[0].FN_LEN, "%s/",
                 FilePathStr.c_str());
      } else if (FilePathStr.compare(u8Pos + 1, 12, "usb-01-d0-p0") == 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "USB2 Detected ");
        gSUSBDevInfo[1].IsDeviceAvailable = TRUE;
        snprintf(gSUSBDevInfo[1].parse_fn, gSUSBDevInfo[1].FN_LEN, "%s/",
                 FilePathStr.c_str());
        snprintf(gSUSBDevInfo[1].location, gSUSBDevInfo[1].FN_LEN, "%s/",
                 FilePathStr.c_str());
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "USB Detected :%s", FilePathStr.c_str());
      }
    } else {
      u8Pos = static_cast<UI_8>(FilePathStr.find_last_of("/\\"));

      if (FilePathStr.compare(u8Pos + 1, 12, "usb-00-d0-p0") == 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "USB1 Ejected ");
        gSUSBDevInfo[0].IsDeviceAvailable = FALSE;
      } else if (FilePathStr.compare(u8Pos + 1, 12, "usb-01-d0-p0") == 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "USB2 Ejected ");
        gSUSBDevInfo[1].IsDeviceAvailable = FALSE;
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "USB Ejected :%s", FilePathStr.c_str());
      }
    }

  } else if (tMediaDetectInfo.dev_type == eSD) {
    if (tMediaDetectInfo.bIsDeviceAvailable == TRUE) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "SD card Detected ");
      gSSDCardevInfo.IsDeviceAvailable = TRUE;
      snprintf(gSSDCardevInfo.parse_fn, gSSDCardevInfo.FN_LEN, "%s/",
               FilePathStr.c_str());
      snprintf(gSSDCardevInfo.location, gSSDCardevInfo.FN_LEN, "%s/",
               FilePathStr.c_str());
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "SD card  Ejected ");
      gSSDCardevInfo.IsDeviceAvailable = FALSE;
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __PRETTY_FUNCTION__, "-");

  return eStatus;
}
// LCOV_EXCL_STOP

//////////////////////////////////////////
//  Function : DD_USBServiceAvailabilityCallBack
//////////////////////////////////////////
EFrameworkunifiedStatus DD_USBServiceAvailabilityCallBack(HANDLE hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (FrameworkunifiedIsServiceAvailable(hThread)) {
    if (eFrameworkunifiedStatusOK  // LCOV_EXCL_BR_LINE:200 Because the IF of DetectionService is guaranteed to be successful
        != (eStatus = m_device_detector_obj.OpenSessionRequest())) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Open session request failed");
    }
  } else {
    if (eFrameworkunifiedStatusOK  // LCOV_EXCL_BR_LINE:200 Because the IF of DetectionService is guaranteed to be successful
        != (eStatus = m_device_detector_obj.CloseSessionRequest())) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Close session request failed");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

//////////////////////////////////////////
//  Function : DD_USBOpenSessionAckCallBack
//////////////////////////////////////////
EFrameworkunifiedStatus DD_USBOpenSessionAckCallBack(HANDLE hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (eFrameworkunifiedStatusOK == (eStatus =  // LCOV_EXCL_BR_LINE:200 Because the IF of DetectionService is guaranteed to be successful
      m_device_detector_obj.DecodeOpenSessionResponse())) {
    if (eFrameworkunifiedStatusOK
        != (eStatus = m_device_detector_obj.RegisterForDeviceDetectionEvent(  // LCOV_EXCL_BR_LINE:200 Because the IF of DetectionService is guaranteed to be successful  // NOLINT[whitespace/line_length]
            SS_DEV_DETECT_ANY_USB_EV, DD_USBSrvDetectionCallBack))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Registration for SS_DEV_DETECT_ANY_USB_EV failed");
    }

    if (eFrameworkunifiedStatusOK
        != (eStatus = m_device_detector_obj.RegisterForDeviceDetectionEvent(  // LCOV_EXCL_BR_LINE:200 Because the IF of DetectionService is guaranteed to be successful  // NOLINT[whitespace/line_length]
            SS_DEV_DETECT_ANY_SD_EV, DD_USBSrvDetectionCallBack))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Registration for SS_DEV_DETECT_ANY_SD_EV failed");
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Decode open session response failed");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
//////////////////////////////////////////
//  Function : DD_USBCloseSessionAckCallBack
//////////////////////////////////////////
EFrameworkunifiedStatus DD_USBCloseSessionAckCallBack(HANDLE hApp) {  // LCOV_EXCL_START 200: CloseSessionRequest can not be called  // NOLINT[whitespace/line_length]
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CloseSessionAck tCloseSessionAck = { };

  if (hApp) {
    if (eFrameworkunifiedStatusOK
        == (eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &tCloseSessionAck,
                                          sizeof(tCloseSessionAck)))) {
      if (eFrameworkunifiedStatusOK == tCloseSessionAck.eStatus) {
        UI_32 l_uiSessionId = tCloseSessionAck.sessionId;
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "session  %d closed successfully",
               l_uiSessionId);
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 8: dead code
EFrameworkunifiedStatus CEvntWriterWorker::counterIncrementByValue(EStatCounterGroupID group,
                                                      UI_16 id, UI_32 value) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus result = eFrameworkunifiedStatusFail;
  CounterInformation::iterator count_inf_it;
  count_inf_it = counter.find(group);
  if (counter.end() == count_inf_it) {
    Counter newCounter;
    newCounter.insert(std::pair<UI_16, UI_32>(id, value));
    counter.insert(std::pair<EStatCounterGroupID, Counter>(group, newCounter));
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__,
           "Added new Counter for group %d, id: 0x%04X, value: 0x%08X",
           (UI_8)group, id, value);
  } else {
    Counter::iterator count_it;
    count_it = count_inf_it->second.find(id);
    if (count_inf_it->second.end() == count_it) {
      count_inf_it->second.insert(std::pair<UI_16, UI_32>(id, value));
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__,
             "Added new Entry for group %d, id: 0x%04X, value: 0x%08X",
             (UI_8)group, id, value);
    } else {
      if (count_it->second != MAX_CNT_VALUE) {
        count_it->second += value;
      }
      FRAMEWORKUNIFIEDLOG(
          ZONE_FUNC,
          __FUNCTION__,
          "Modified Entry for group %d, id: 0x%04X, by value: 0x%08X, new value: 0x%08X",
          (UI_8)group, id, value, count_it->second);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return result;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CEvntWriterWorker::immediate_persist_event_log(HANDLE hThread) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  STEventLogPersistBuffer* l_evtlogbuf;
  l_evtlogbuf = new (std::nothrow) STEventLogPersistBuffer;

  if (NULL != l_evtlogbuf) {
    if (pObj) {
      pObj->write_events_to_buffer(l_evtlogbuf);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: CEvntWriterWorker Object handle is NULL");
    }
    delete l_evtlogbuf;
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error: New failed to allocate memory, err: %s", strerror(errno));
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CEvntWriterWorker::EvtThd_SetMileage(HANDLE hThread) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_u32mileage;
  if (eFrameworkunifiedStatusOK
      != (l_eStatus = FrameworkunifiedGetMsgDataOfSize(hThread, &l_u32mileage,
                                          sizeof(l_u32mileage)))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedGetMsgDataOfSize Failed Status:%d ",
           l_eStatus);
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return l_eStatus;
  }
  m_u32MileageData = l_u32mileage;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :write_mileage_to_file
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::write_mileage_to_file(const CHAR* filename) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  UI_32 u32File_Size;
  SI_32 fd = -1;

  if (-1 != (fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC,
  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP))) {
    if (SIZE_0
        >= (u32File_Size = static_cast<UI_32>(::write(fd, &m_u32MileageData, sizeof(UI_32))))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: Writing in to file is failed");
      eStatus = eFrameworkunifiedStatusFail;
    }
    ::close(fd);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: File failed to open");
    eStatus = eFrameworkunifiedStatusFileLoadError;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :read_mileage_from_file
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::read_mileage_from_file(const CHAR* filename) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
//  UI_32 u32File_Size;
  SI_32 fd = -1;

  if (NULL != filename) {
    if (-1 != (fd = ::open(filename, O_RDONLY, S_IRUSR))) {
      if (-1 == ::read(fd, &m_u32MileageData, sizeof(UI_32))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: Reading file failed, %s",
               strerror(errno));
        l_eStatus = eFrameworkunifiedStatusFail;
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "m_u32MileageData = %d",
               m_u32MileageData);
      }
      ::close(fd);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: File failed to open");
      l_eStatus = eFrameworkunifiedStatusFileLoadError;
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: Filename is NULL");
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdReadNumberOfEventsLogged
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::OnCmdReadNumberOfEventsLogged(HANDLE hThread) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  TEvntsLogged l_stEventsLogged;

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg<TSessionData>(hThread,
                                            l_stEventsLogged.stSessiondata))) {
    LOG_ERROR("ReadMsg()");
  } else {
    l_stEventsLogged.u16numberofeventslogged = (UI_16) deque_event_info.size();
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Number of Events Read: %d",
           l_stEventsLogged.u16numberofeventslogged);

    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendParent(hThread,
                                      eThrdCmdNumberOfEventsLoggedResponse,
                                      sizeof(TEvntsLogged), &l_stEventsLogged))) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Error: FrameworkunifiedSendMsg failed %d", l_eStatus);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdUploadEventLog
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::OnCmdUploadEventLog(HANDLE hThread) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg<TSessionData>(
          hThread, m_stUploadEventLogResp.stSessiondata))) {
    LOG_ERROR("ReadMsg()");
  } else {
    if (pObj) {
      pObj->write_events_to_buffer(&m_stUploadEventLogResp.stEventLogBuffer);
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = FrameworkunifiedSendParent(hThread, eThrdCmdUploadEventLogResponse,
                                        sizeof(TUploadEventLogResp),
                                        &m_stUploadEventLogResp))) {
        LOG_ERROR("FrameworkunifiedSendParent()");
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: CEvntWriterWorker Object handle is NULL");
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function :OnCmdCopyEvntLogToTmp
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CEvntWriterWorker::OnCmdCopyEvntLogToTmp(HANDLE hThread) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP
