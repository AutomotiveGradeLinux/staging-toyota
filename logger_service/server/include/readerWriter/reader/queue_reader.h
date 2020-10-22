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
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_READER_QUEUE_READER_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_READER_QUEUE_READER_H_

#include <mqueue.h>
#include <native_service/frameworkunified_types.h>
#include <string>
#include "reader.h"

namespace ReaderWriter {

class CQueueReader : public CReader {
 public:
  CQueueReader();
  virtual ~CQueueReader();
  EFrameworkunifiedStatus Initialize(CLoggerCfg* f_pLoggerCfg, std::string f_name,
                        UI_32 f_maxSize);
  EFrameworkunifiedStatus Read(UI_8 *f_data, UI_32 f_length, SI_32 &f_bytesRead);  // NOLINT (runtime/references)
  BOOL IsOpen(void);
  EFrameworkunifiedStatus Open(void);
  void Close();
 private:
  mqd_t m_handle;
  std::string m_queName;
};

}  // namespace ReaderWriter

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_READER_QUEUE_READER_H_
