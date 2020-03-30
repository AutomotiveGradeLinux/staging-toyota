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
#include "readerWriter/writer/cached_file_writer.h"
#include <native_service/frameworkunified_types.h>

namespace ReaderWriter {

CCachedFileWriter::CCachedFileWriter() {
  std::memset(this->m_buffer, 0, CACHED_BLOCK_SIZE);
  m_index = 0u;
}

CCachedFileWriter::~CCachedFileWriter() {
}

EFrameworkunifiedStatus CCachedFileWriter::Write(UI_8* f_data, UI_32 f_length,
                                    SI_32& f_bytesWritten) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  // length check
  if (f_data != NULL) {  // LCOV_EXCL_BR_LINE 200: f_data can not be null
    if ((this->m_index + f_length) > CACHED_BLOCK_SIZE) {  // LCOV_EXCL_BR_LINE 200: data size can not over 4*1024
      l_eStatus = this->FlushCache();
    }
    if (f_length > CACHED_BLOCK_SIZE) {  // LCOV_EXCL_BR_LINE 200: data size can not over 4*1024
      // LCOV_EXCL_START 200: data size can not over 4*1024
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Possible memory corruption.");
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          " Error. Length(%d) bigger than cache. Will dump memory to core and ignore data. ",
          f_length);
      // LCOV_EXCL_STOP
    } else {
      (void) std::memcpy(&this->m_buffer[this->m_index], f_data, f_length);
      this->m_index += f_length;
      f_bytesWritten = f_length;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return (l_eStatus);
}

EFrameworkunifiedStatus CCachedFileWriter::FlushCache(void) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  SI_32 l_written = 0;
  SI_32 l_writtenTotal = 0;
  while (((l_writtenTotal >= 0) && ((UI_32) l_writtenTotal < this->m_index))
      && (eFrameworkunifiedStatusOK == l_eStatus)) {
    l_eStatus = this->WriteData(&this->m_buffer[l_writtenTotal],
                                this->m_index - l_writtenTotal, l_written);
    l_writtenTotal += l_written;
    l_written = 0;
  }
  this->m_index = 0;
  return l_eStatus;
}
}  // namespace ReaderWriter
