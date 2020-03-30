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
#include "readerWriter/writer/udp_file_writer.h"
#include "readerWriter/writer/file_writer.h"
#include "readerWriter/writer/cached_file_writer.h"

namespace ReaderWriter {

// LCOV_EXCL_START 8:dead code
CUdpFileWriter::CUdpFileWriter()
    : m_pUdpWriter(NULL),
      m_pFileWriter(NULL),
      m_pLoggerCfg(NULL),
      m_ipAddress(""),
      m_port(0),
      m_FileName(""),
      m_FileSize(0) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

CUdpFileWriter::~CUdpFileWriter() {  // LCOV_EXCL_START 8:dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (this->m_pFileWriter != NULL) {
    delete (this->m_pFileWriter);
    this->m_pFileWriter = NULL;
  }
  if (this->m_pUdpWriter != NULL) {
    delete (this->m_pUdpWriter);
    this->m_pUdpWriter = NULL;
  }
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 8:dead code
EFrameworkunifiedStatus CUdpFileWriter::Initialize(CLoggerCfg* f_pLoggerCfg,
                                      std::string f_Name1, UI_32 f_size1,
                                      std::string f_Name2, UI_32 f_size2) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  if ((NULL == this->m_pFileWriter) && (NULL == this->m_pUdpWriter)
      && (NULL != f_pLoggerCfg)) {
    this->m_pFileWriter = new (std::nothrow) CCachedFileWriter();
    this->m_pUdpWriter = new (std::nothrow) CUdpWriter();
    m_pLoggerCfg = f_pLoggerCfg;
    m_ipAddress = f_Name2;
    m_port = f_size2;
    m_FileName = f_Name1;
    m_FileSize = f_size1;
    EFrameworkunifiedStatus l_eStatus1 = this->m_pUdpWriter->Initialize(m_pLoggerCfg,
                                                           m_ipAddress, m_port,
                                                           "", 0);
    EFrameworkunifiedStatus l_eStatus2 = this->m_pFileWriter->Initialize(m_pLoggerCfg,
                                                            m_FileName,
                                                            m_FileSize, "", 0);
    l_eStatus =
        ((l_eStatus1 != eFrameworkunifiedStatusOK) || (eFrameworkunifiedStatusOK != l_eStatus2)) ?
            eFrameworkunifiedStatusFail : eFrameworkunifiedStatusOK;
    if (l_eStatus == eFrameworkunifiedStatusOK) {
      l_eStatus = this->Open();
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %d", l_eStatus);
  return (l_eStatus);
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CUdpFileWriter::Open(void) {  // LCOV_EXCL_START 8:dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  if ((NULL != this->m_pFileWriter) && (NULL != this->m_pUdpWriter)) {
    EFrameworkunifiedStatus l_eStatus1 = this->m_pUdpWriter->Open();
    EFrameworkunifiedStatus l_eStatus2 = this->m_pFileWriter->Open();
    l_eStatus =
        ((l_eStatus1 != eFrameworkunifiedStatusOK) || (eFrameworkunifiedStatusOK != l_eStatus2)) ?
            eFrameworkunifiedStatusFail : eFrameworkunifiedStatusOK;
  }

  return (l_eStatus);
}
// LCOV_EXCL_STOP

BOOL CUdpFileWriter::IsOpen(void) {  // LCOV_EXCL_START 8:dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  BOOL l_ret = FALSE;
  if ((this->m_pUdpWriter != NULL) && (this->m_pFileWriter != NULL)) {
    l_ret = (m_pUdpWriter->IsOpen() && m_pFileWriter->IsOpen());
  }
  return l_ret;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 8:dead code
EFrameworkunifiedStatus CUdpFileWriter::Write(UI_8* f_data, UI_32 f_length,
                                 SI_32& f_bytesWritten) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if ((NULL != this->m_pUdpWriter)) {
    l_eStatus = this->m_pUdpWriter->Write(f_data, f_length, f_bytesWritten);
    LOG_STATUS_IF_ERRORED(l_eStatus, "this->m_pUdpWriter->Write() failed.");
  }

  if ((NULL != this->m_pFileWriter)) {
    l_eStatus = this->m_pFileWriter->Write(f_data, f_length, f_bytesWritten);
    LOG_STATUS_IF_ERRORED(l_eStatus, "this->m_pFileWriter->Write() failed.");
  } else {
    l_eStatus = eFrameworkunifiedStatusFail;
  }
  return (l_eStatus);
}
// LCOV_EXCL_STOP

void CUdpFileWriter::Close() {    // LCOV_EXCL_START 8:dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (this->m_pFileWriter != NULL) {
    this->m_pFileWriter->Close();
    delete (this->m_pFileWriter);
    this->m_pFileWriter = NULL;
  }
  if (this->m_pUdpWriter != NULL) {
    this->m_pUdpWriter->Close();
    delete (this->m_pUdpWriter);
    this->m_pUdpWriter = NULL;
  }
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CUdpFileWriter::FlushCache(void) {  // LCOV_EXCL_START 8:dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return (this->m_pFileWriter->FlushCache());
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CUdpFileWriter::UpdateLoggingParameters(void) {  // LCOV_EXCL_START 8:dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return (this->m_pUdpWriter->UpdateLoggingParameters());
}
}  // namespace ReaderWriter

// LCOV_EXCL_STOP
