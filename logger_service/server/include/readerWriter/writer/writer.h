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
/// \ingroup  tag_SS_LoggerService
/// \brief    TODO
///
///////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_WRITER_WRITER_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_WRITER_WRITER_H_
#include <native_service/frameworkunified_types.h>
#include <string>
#include "readerWriter/reader_writer_cfg.h"
#include "ss_logger_cfg.h"

namespace ReaderWriter {

class CWriter {
 public:
  CWriter();
  virtual ~CWriter();

  static CWriter* OpenWriter(CLoggerCfg* f_pLoggerCfg, EReaderType f_type,
                             std::string f_Name1, UI_32 f_size1,
                             std::string f_Name2, UI_32 f_size2);
  virtual EFrameworkunifiedStatus Initialize(CLoggerCfg* f_pLoggerCfg, std::string f_Name1,
                                UI_32 f_size1, std::string f_Name2,
                                UI_32 f_size2) = 0;
  virtual EFrameworkunifiedStatus Write(UI_8 *f_data, UI_32 f_length,
                           SI_32 &f_bytesWritten) = 0;  // NOLINT (runtime/references)
  virtual void Close() = 0;
  virtual EFrameworkunifiedStatus FlushCache(void);
  virtual EFrameworkunifiedStatus UpdateLoggingParameters(void);
  virtual BOOL IsOpen(void) = 0;
  virtual EFrameworkunifiedStatus Open(void) = 0;

 protected:
  CLoggerCfg * m_pLoggerCfg;
};

}  // namespace ReaderWriter

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_WRITER_WRITER_H_
