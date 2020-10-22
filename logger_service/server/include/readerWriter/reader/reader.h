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
/// \brief    TODO
///
///////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_READER_READER_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_READER_READER_H_
#include <native_service/frameworkunified_types.h>
#include <string>
#include "readerWriter/reader_writer_cfg.h"
#include "ss_logger_cfg.h"

namespace ReaderWriter {

class CReader {
 public:
  CReader();
  virtual ~CReader();

  static CReader* OpenReader(CLoggerCfg* f_pLoggerCfg, EReaderType f_type,
                             std::string f_name, UI_32 f_maxSize);
  virtual EFrameworkunifiedStatus Initialize(CLoggerCfg* f_pLoggerCfg, std::string f_name,
                                UI_32 f_maxSize) = 0;
  virtual BOOL IsOpen(void) = 0;
  virtual EFrameworkunifiedStatus Open(void) = 0;
  virtual EFrameworkunifiedStatus Read(UI_8* f_data, UI_32 f_length, SI_32& f_bytesRead) = 0;  // NOLINT (runtime/references)
  virtual EFrameworkunifiedStatus ReadToFile(std::string f_fileName, UI_32& f_Written);  // NOLINT (runtime/references)

  virtual void Close() = 0;
  virtual EFrameworkunifiedStatus ResetPosition(void);

 protected:
  CLoggerCfg * m_pLoggerCfg;
};
}  // namespace ReaderWriter
#endif  // LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_READER_READER_H_
