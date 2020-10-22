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
/// \ingroup  tag_NS_InterfaceunifiedLogCapture
/// \brief    Class that threads for usb, and readers use to AGL and get
///           information from the parent.
///
///////////////////////////////////////////////////////////////////////////////
// System Headers
#include <loggerservicedebug_thread_if.h>
#include <new>

const CHAR g_strLogQueReaderWorkerName[] = "pdg.LogQueR";
const CHAR g_strTransmitQueReaderWorkerName[] = "pdg.TransQueR";
const CHAR g_strPerformanceQueReaderWorkerName[] = "pdg.PerformQueR";
const CHAR g_strUsbEjectWorkerName[] = "pdg.UsbEject";
const CHAR g_strUsbLogCaptureWorkerName[] = "pdg.UsbLogCapt";
const CHAR g_strCaptureLogScriptExeName[] = "pdg.CaptLogScri";

LoggerserviceDebugChildThread::LoggerserviceDebugChildThread() {
  names[LoggerserviceDebugChildThread::kLoggerserviceDebugLogQueReader] =
      g_strLogQueReaderWorkerName;
  names[LoggerserviceDebugChildThread::kLoggerserviceDebugTransmitQueReader] =
      g_strTransmitQueReaderWorkerName;
  names[LoggerserviceDebugChildThread::kLoggerserviceDebugCaptureLogScript] =
      g_strCaptureLogScriptExeName;
  names[LoggerserviceDebugChildThread::kLoggerservicePerformanceLogQueReader] =
      g_strPerformanceQueReaderWorkerName;

  threads[LoggerserviceDebugChildThread::kLoggerserviceDebugLogQueReader] = NULL;
  threads[LoggerserviceDebugChildThread::kLoggerserviceDebugTransmitQueReader] = NULL;
  threads[LoggerserviceDebugChildThread::kLoggerserviceDebugCaptureLogScript] = NULL;
  threads[LoggerserviceDebugChildThread::kLoggerservicePerformanceLogQueReader] = NULL;
}  // LCOV_EXCL_BR_LINE 10:the last line

LoggerserviceDebugChildThread::~LoggerserviceDebugChildThread() {  // LCOV_EXCL_START 14:global instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

BOOL LoggerserviceDebugChildThread::Start(ELoggerserviceDebugThreads id, const char* parentsName,
                                const char * sendDataToQue,
                                void* (*routine)(void*),
                                const char * sendToUdpQue) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  BOOL rtn = FALSE;
  if (id < LoggerserviceDebugChildThread::kLoggerserviceDebugMax && id > -1) {
    if (threads[id] == NULL) {
      if ( NULL
          != (threads[id] = new (std::nothrow) CChild(parentsName,
                                                      sendDataToQue,
                                                      names[id].c_str(),
                                                      routine, sendToUdpQue))) {
        rtn = TRUE;
      }
    } else {
      rtn = TRUE;
    }
  }

  return rtn;
}
// LCOV_EXCL_STOP

void LoggerserviceDebugChildThread::Stop(ELoggerserviceDebugThreads id) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (id < LoggerserviceDebugChildThread::kLoggerserviceDebugMax && id > -1) {
    if (threads[id] != NULL) {
      delete threads[id];
      threads[id] = NULL;
    }
  }
}
// LCOV_EXCL_STOP

BOOL LoggerserviceDebugChildThread::Running(ELoggerserviceDebugThreads id) const {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  BOOL rtn = FALSE;
  if (id < LoggerserviceDebugChildThread::kLoggerserviceDebugMax && id > -1) {
    if (threads[id] != NULL) {
      rtn = TRUE;
    }
  }

  return rtn;
}
// LCOV_EXCL_STOP

void LoggerserviceDebugChildThread::StopAll() {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  for (int id = kLoggerserviceDebugLogQueReader; id < kLoggerserviceDebugMax; id++) {
    if (threads[id] != NULL) {
      threads[id]->Join();
      delete threads[id];
      threads[id] = NULL;
    }
  }
}
// LCOV_EXCL_STOP

const char * LoggerserviceDebugChildThread::Name(ELoggerserviceDebugThreads id) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (id < LoggerserviceDebugChildThread::kLoggerserviceDebugMax && id > -1) {
    return names[id].c_str();
  }

  return NULL;
}
// LCOV_EXCL_STOP

void LoggerserviceDebugChildThread::LiftWaitBarrier(ELoggerserviceDebugThreads id) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (id < LoggerserviceDebugChildThread::kLoggerserviceDebugMax && id > -1) {
    if (threads[id] != NULL) {
      threads[id]->IsValidWaitBarrier();
    }
  }
}
// LCOV_EXCL_STOP

