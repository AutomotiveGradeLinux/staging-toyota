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
#include "readerWriter/writer/file_writer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

namespace ReaderWriter {
CFileWriter::CFileWriter()
    : m_FileHandle(-1),
      m_fileposn(-1),
      m_maxFileSize(0) {
}

CFileWriter::~CFileWriter() {
  if (this->m_FileHandle != -1) {  // LCOV_EXCL_BR_LINE 200: it aways open, so m_FileHandle can not be -1
    this->Close();
    this->m_FileHandle = -1;
  }
}

EFrameworkunifiedStatus CFileWriter::Initialize(CLoggerCfg* f_pLoggerCfg,
                                   std::string f_Name1, UI_32 f_size1,
                                   std::string f_Name2, UI_32 f_size2) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  this->m_pLoggerCfg = f_pLoggerCfg;
  this->m_maxFileSize = f_size1;
  if ((f_Name1.length() != 0) && (this->m_FileHandle == -1)) {  // LCOV_EXCL_BR_LINE 6:Due to the initial status
    m_filename = f_Name1;
    l_eStatus = this->Open();
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %d", l_eStatus);
  return (l_eStatus);
}

EFrameworkunifiedStatus CFileWriter::Open(void) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  if (this->m_FileHandle != -1) {  // LCOV_EXCL_BR_LINE 6:Due to the initial status
    // File already opened, no action required.
    l_eStatus = eFrameworkunifiedStatusOK;
  } else if (m_filename.length() != 0) {  // LCOV_EXCL_BR_LINE 6:Because the applicable variable cannot be changed from the external API
    this->m_FileHandle = open(m_filename.c_str(),
    O_WRONLY | O_CREAT | O_TRUNC,
                              0644);
    if (-1 != this->m_FileHandle) {  // LCOV_EXCL_BR_LINE 5:The open cannot pass because it cannot be mock
      l_eStatus = eFrameworkunifiedStatusOK;
      m_fileposn = lseek(m_FileHandle, 0L, SEEK_CUR);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %d", l_eStatus);
  return (l_eStatus);
}

BOOL CFileWriter::IsOpen(void) {
  return (-1 == m_FileHandle) ? FALSE : TRUE;
}

EFrameworkunifiedStatus CFileWriter::Write(UI_8* f_data, UI_32 f_length,
                              SI_32& f_bytesWritten) {
  return (this->WriteData(f_data, f_length, f_bytesWritten));
}

EFrameworkunifiedStatus CFileWriter::WriteData(UI_8* f_data, UI_32 f_length,
                                  SI_32& f_bytesWritten) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  f_bytesWritten = 0;
  if (-1 == m_FileHandle) {  // LCOV_EXCL_BR_LINE 200: m_FileHandle can not be null
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  while ((eFrameworkunifiedStatusOK == l_eStatus)
      && ((f_bytesWritten >= 0) && ((UI_32) f_bytesWritten < f_length))) {
    SI_32 l_bytesWritten = -1;
    m_fileposn = lseek(m_FileHandle, 0L, SEEK_CUR);
    if ((m_fileposn + (UI_32) f_length) >= this->m_maxFileSize) {
      SI_32 l_deltaLength = static_cast<SI_32>(this->m_maxFileSize - m_fileposn);
      l_bytesWritten = static_cast<SI_32>(write(m_FileHandle, &f_data[f_bytesWritten],
                             l_deltaLength));
      f_bytesWritten += l_bytesWritten;
      if ((l_bytesWritten >= 0) && (l_deltaLength == l_bytesWritten)) {
        m_fileposn = lseek(m_FileHandle, 0L, SEEK_SET);
      }
    } else {
      l_bytesWritten = static_cast<SI_32>(write(m_FileHandle, &f_data[f_bytesWritten],
                             f_length - f_bytesWritten));
      f_bytesWritten += l_bytesWritten;
    }
    l_eStatus = (l_bytesWritten > -1) ? eFrameworkunifiedStatusOK : eFrameworkunifiedStatusFail;
  }

  return (l_eStatus);
}

void CFileWriter::Close() {  // LCOV_EXCL_START 6:Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (this->m_FileHandle != -1) {
    (void) close(this->m_FileHandle);
    this->m_FileHandle = -1;
  }
}
// LCOV_EXCL_STOP
}  // namespace ReaderWriter
