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
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_WRITER_UDP_WRITER_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_WRITER_UDP_WRITER_H_

#include <netinet/in.h>
#include <string>
#include "writer.h"

#define REMOTE_IP_PORT    3500
namespace ReaderWriter {
#define UDP_MAX_LENGTH (UI_32)1024*4
#define UDP_CACHED_BLOCK_SIZE (UI_32)1024*4
#define SEND_RETRY_VALUE (UI_8) 5

#define OWN_ADDRESS "169.254.80.8"
class CUdpWriter : public CWriter {
 public:
  CUdpWriter();
  virtual ~CUdpWriter();
  EFrameworkunifiedStatus Initialize(CLoggerCfg* f_pLoggerCfg, std::string f_Name1,
                        UI_32 f_size1, std::string f_Name2, UI_32 f_size2);

  EFrameworkunifiedStatus Write(UI_8 *f_data, UI_32 f_length, SI_32 &f_bytesWritten);  // NOLINT (runtime/references)
  void Close();
  EFrameworkunifiedStatus UpdateLoggingParameters(void);
  BOOL IsOpen(void);
  EFrameworkunifiedStatus Open(void);
  EFrameworkunifiedStatus FlushCache(void);
 private:
  EFrameworkunifiedStatus WriteData(UI_8 *f_data, UI_32 f_length, SI_32 &f_bytesWritten);  // NOLINT (runtime/references)

  int m_SocketHandle;
  sockaddr_in m_oSAInfo;
  std::string m_IPAddress;
  UI_32 m_index;
  UI_32 m_port;
  BOOL m_InitialTimeElapsed;
  UI_8 m_buffer[UDP_CACHED_BLOCK_SIZE];
};

}  // namespace ReaderWriter

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_WRITER_UDP_WRITER_H_
