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
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_WRITER_CACHED_FILE_WRITER_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_WRITER_CACHED_FILE_WRITER_H_

#include <native_service/frameworkunified_types.h>
#include "file_writer.h"

#define CACHED_BLOCK_SIZE        (UI_32)(4*1024)

namespace ReaderWriter {

class CCachedFileWriter : public CFileWriter {
 public:
  CCachedFileWriter();
  virtual ~CCachedFileWriter();
  EFrameworkunifiedStatus Write(UI_8 *f_data, UI_32 f_length, SI_32 &f_bytesWritten);  // NOLINT (runtime/references)
  EFrameworkunifiedStatus FlushCache(void);
 private:
  UI_8 m_buffer[CACHED_BLOCK_SIZE ];
  UI_32 m_index;
};

}  // namespace ReaderWriter

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_WRITER_CACHED_FILE_WRITER_H_
