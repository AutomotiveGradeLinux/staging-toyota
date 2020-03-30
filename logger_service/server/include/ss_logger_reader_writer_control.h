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
 * ss_logger_reader_writer_control.h
 */

#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_READER_WRITER_CONTROL_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_READER_WRITER_CONTROL_H_

#include <native_service/frameworkunified_types.h>
#include <string>
#include "readerWriter/reader_writer.h"
#include "ss_logger_cfg.h"

enum EReaderWriterType {
  eReaderWriterLogDebug,
  eReaderWriterLogTransmit,
  eReaderWriterLogPerformance,
  eReaderWriterLogInvalid
};
class CReaderWriterControl {
 public:
  CReaderWriterControl();
  virtual ~CReaderWriterControl();
  EFrameworkunifiedStatus Initialize(CLoggerCfg* f_pLoggerCfg);
  EFrameworkunifiedStatus UpdateLoggingParams(void* param);
  EFrameworkunifiedStatus FlushCache(EReaderWriterType f_type);
 private:
  CLoggerCfg * m_pLoggerCfg;
  ReaderWriter::CReaderWriter m_debugReaderWriter;
  ReaderWriter::CReaderWriter m_transmitReaderWriter;
  ReaderWriter::CReaderWriter m_performanceReaderWriter;
};

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_READER_WRITER_CONTROL_H_
