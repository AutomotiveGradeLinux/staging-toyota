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
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_WRITER_UDP_FILE_WRITER_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_WRITER_UDP_FILE_WRITER_H_
#include <native_service/frameworkunified_types.h>
#include <string>
#include "writer.h"
#include "udp_writer.h"
#include "file_writer.h"

namespace ReaderWriter {

class CUdpFileWriter : public CWriter {
 public:
  CUdpFileWriter();
  virtual ~CUdpFileWriter();
  EFrameworkunifiedStatus Initialize(CLoggerCfg* f_pLoggerCfg, std::string f_Name1,
                        UI_32 f_size1, std::string f_Name2, UI_32 f_size2);
  EFrameworkunifiedStatus Write(UI_8 *f_data, UI_32 f_length, SI_32 &f_bytesWritten);  // NOLINT (runtime/references)
  void Close();
  BOOL IsOpen(void);
  EFrameworkunifiedStatus Open(void);
  EFrameworkunifiedStatus FlushCache(void);
  EFrameworkunifiedStatus UpdateLoggingParameters(void);
 private:
  CUdpWriter *m_pUdpWriter;
  CFileWriter *m_pFileWriter;
  CLoggerCfg* m_pLoggerCfg;
  std::string m_ipAddress;
  UI_32 m_port;
  std::string m_FileName;
  UI_32 m_FileSize;
};
}  // namespace ReaderWriter
#endif  // LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_WRITER_UDP_FILE_WRITER_H_
