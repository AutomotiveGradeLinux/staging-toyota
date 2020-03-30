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
 * mem_reader.h
 *
 *  Created on: Oct 22, 2013
 */

#ifndef LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_READER_MEM_READER_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_READER_MEM_READER_H_

#include <native_service/ns_sharedmem.h>
#include <native_service/ns_sharedmem_reader.h>
#include <string>
#include "reader.h"

namespace ReaderWriter {
class CMemReader : public ReaderWriter::CReader {
 public:
  CMemReader();
  virtual ~CMemReader();
  EFrameworkunifiedStatus Initialize(CLoggerCfg* f_pLoggerCfg, std::string f_name,
                        UI_32 f_maxSize);
  EFrameworkunifiedStatus Read(UI_8* f_data, UI_32 f_length, SI_32& f_bytesRead);  // NOLINT (runtime/references)
  EFrameworkunifiedStatus ReadToFile(std::string f_fileName, UI_32& f_Written);  // NOLINT (runtime/references)
  EFrameworkunifiedStatus Open(void);
  BOOL IsOpen(void);
  void Close();
  EFrameworkunifiedStatus ResetPosition(void);

 private:
  CNSSharedMemReader* m_pSharedBuf;
  UI_32 m_sharedBufSize;
};
}  // namespace ReaderWriter

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_READER_MEM_READER_H_
