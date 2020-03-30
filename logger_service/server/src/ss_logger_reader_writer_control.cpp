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

/*
 * ss_logger_reader_writer_control.cpp
 */
#include "ss_logger_reader_writer_control.h"
#include <boost/bind.hpp>
#include <native_service/ns_sharedmem.h>
#include <native_service/ns_transmit_log.h>
#include <native_service/ns_logger_if.h>
#include <string>
#include "ss_logger_common.h"

#define LOGGERSERVICELOG_REMOTE_IP_PORT    3500
#define TRANSMIT_REMOTE_IP_PORT    3500

CReaderWriterControl::CReaderWriterControl()
    : m_pLoggerCfg(NULL) {  // LCOV_EXCL_BR_LINE 11:Unexpected branch
}

CReaderWriterControl::~CReaderWriterControl() {  // LCOV_EXCL_START 14:globle instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CReaderWriterControl::Initialize(CLoggerCfg * f_pLoggerCfg) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (NULL != f_pLoggerCfg) {  // LCOV_EXCL_BR_LINE 200:As it is not always NULL
    this->m_pLoggerCfg = f_pLoggerCfg;

    l_eStatus = m_debugReaderWriter.Initialize(  // LCOV_EXCL_BR_LINE 11:Unexpected branch
        f_pLoggerCfg, ReaderWriter::eReaderWriterTypeMem, std::string(FRAMEWORKUNIFIEDLOG_SHAREDMEM_NAME),
        FRAMEWORKUNIFIEDLOG_SHAREDMEM_SIZE, ReaderWriter::eReaderWriterTypeUdp,
        std::string("169.254.80.9"), LOGGERSERVICELOG_REMOTE_IP_PORT, "", 0);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    if (eFrameworkunifiedStatusOK == l_eStatus) {  // LCOV_EXCL_BR_LINE 200:To ensure success
      l_eStatus = m_debugReaderWriter.Start(std::string("pdg.LogQueR"));
    }

    LOG_STATUS_IF_ERRORED(  // LCOV_EXCL_BR_LINE 15:macro
        l_eStatus, "m_debugReaderWriter.Start(std::string(pdg.TransQueR)");


    l_eStatus = m_transmitReaderWriter.Initialize(  // LCOV_EXCL_BR_LINE 11:Unexpected branch
        f_pLoggerCfg, ReaderWriter::eReaderWriterTypeMem,
        std::string(TRANSMIT_LOG_SHAREDMEM_NAME), TRANSMIT_LOG_SHAREDMEM_SIZE,
        ReaderWriter::eReaderWriterTypeUdp, std::string("169.254.80.9"),
        TRANSMIT_REMOTE_IP_PORT, "", 0);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    if (eFrameworkunifiedStatusOK == l_eStatus) {    // LCOV_EXCL_BR_LINE 6:it can not be fail
      l_eStatus = m_transmitReaderWriter.Start(std::string("pdg.TransQueR"));  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    }

    LOG_STATUS_IF_ERRORED(  // LCOV_EXCL_BR_LINE 15:macro
        l_eStatus, "m_transmitReaderWriter.Start(std::string(pdg.TransQueR)");


    l_eStatus = m_performanceReaderWriter.Initialize(  // LCOV_EXCL_BR_LINE 11:Unexpected branch
        f_pLoggerCfg, ReaderWriter::eReaderWriterTypeQueue, std::string("/NSplog"), 0,
        ReaderWriter::eReaderWriterTypeFile, std::string(PERFORMANCE_LOG_PATH_FN),
        f_pLoggerCfg->m_performanceLogMaxFileSize, std::string("169.254.80.9"),
        0);  // LCOV_EXCL_BR_LINE 11:except,C++ STL

    if (eFrameworkunifiedStatusOK == l_eStatus) {    // LCOV_EXCL_BR_LINE 6:it can not be fail
      l_eStatus = m_performanceReaderWriter.Start(
          std::string("pdg.PerformQueR"));  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    }

    LOG_STATUS_IF_ERRORED(  // LCOV_EXCL_BR_LINE 15:macro
        l_eStatus,
        "m_performanceReaderWriter.Start(std::string(pdg.PerformQueR)");


    this->m_pLoggerCfg->AddLoggingParamCB(
        boost::bind(&CReaderWriterControl::UpdateLoggingParams, this, _1));
  }
  return (l_eStatus);
}

EFrameworkunifiedStatus CReaderWriterControl::UpdateLoggingParams(void* param) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  l_eStatus = this->m_debugReaderWriter.UpdateLoggingParameters();
  LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15:macro
                        "this->m_debugReaderWriter.UpdateLoggingParameters()");

  l_eStatus = this->m_transmitReaderWriter.UpdateLoggingParameters();
  LOG_STATUS_IF_ERRORED(  // LCOV_EXCL_BR_LINE 15:macro
      l_eStatus, "this->m_transmitReaderWriter.UpdateLoggingParameters()");

  l_eStatus = this->m_performanceReaderWriter.UpdateLoggingParameters();
  LOG_STATUS_IF_ERRORED(  // LCOV_EXCL_BR_LINE 15:macro
      l_eStatus,
      "this->m_performanceReaderWriter."
      "UpdateLoggingParameters()");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "- : %d", l_eStatus);
  return (l_eStatus);
}

EFrameworkunifiedStatus CReaderWriterControl::FlushCache(EReaderWriterType f_type) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldParam;
  switch (f_type) {  // LCOV_EXCL_BR_LINE 8: eReaderWriterLogDebug and eReaderWriterLogInvalid can not be called
    // LCOV_EXCL_START 8: dead code
    case eReaderWriterLogDebug:
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = this->m_debugReaderWriter.FlushCache(
          std::string(DEBUG_LOG_PATH_FN));
      LOG_STATUS_IF_ERRORED(
          l_eStatus, "this->m_debugReaderWriter.FlushCache(DEBUG_LOG_PATH_FN)");
      break;
    // LCOV_EXCL_STOP
    case eReaderWriterLogTransmit:
      l_eStatus = this->m_transmitReaderWriter.FlushCache(
          std::string(TRANSMIT_LOG_PATH_FN));
      LOG_STATUS_IF_ERRORED(
          l_eStatus,
          "this->m_debugReaderWriter.FlushCache(TRANSMIT_LOG_PATH_FN)");
      break;

    case eReaderWriterLogPerformance:
      l_eStatus = this->m_performanceReaderWriter.FlushCache(
          std::string(PERFORMANCE_LOG_PATH_FN));
      LOG_STATUS_IF_ERRORED(
          l_eStatus,
          "this->m_debugReaderWriter.FlushCache(PERFORMANCE_LOG_PATH_FN)");
      break;
    // LCOV_EXCL_START 8: dead code
    case eReaderWriterLogInvalid:
    default:
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      break;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "- : %d", l_eStatus);
  return (l_eStatus);
}

