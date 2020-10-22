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
/// \brief    This file supports reading from a shared mem area.
///
///////////////////////////////////////////////////////////////////////////////
#include "readerWriter/reader/mem_reader.h"
#include <string>

namespace ReaderWriter {

CMemReader::CMemReader()
    : m_pSharedBuf(NULL),
      m_sharedBufSize(0) {
}

CMemReader::~CMemReader() {
  this->Close();
}

EFrameworkunifiedStatus CMemReader::Initialize(CLoggerCfg* f_pLoggerCfg, std::string f_name,
                                  UI_32 f_maxSize) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldParam;
  m_sharedBufSize = f_maxSize;
  m_pSharedBuf = new (std::nothrow) CNSSharedMemReader(f_name, TRUE);  // LCOV_EXCL_BR_LINE 5:Cannot pass because it cannot be new
  if (NULL != m_pSharedBuf) {  // LCOV_EXCL_BR_LINE 5:Cannot pass because it cannot be new
    l_eStatus = this->Open();
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %d", l_eStatus);
  return (l_eStatus);
}

EFrameworkunifiedStatus CMemReader::Open(void) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldParam;
  if (NULL != m_pSharedBuf) {  // LCOV_EXCL_BR_LINE 5: new will aways sucess, so m_pSharedBuf will noe be null
    if (!m_pSharedBuf->IsOpen()) {
      // maps the shared memory buffer
      l_eStatus = m_pSharedBuf->Open();
    } else {
      // Already opened no action required
      l_eStatus = eFrameworkunifiedStatusOK;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return l_eStatus;
}

BOOL CMemReader::IsOpen(void) {
  return this->m_pSharedBuf->IsOpen();
}

EFrameworkunifiedStatus CMemReader::Read(UI_8* f_data, UI_32 f_length, SI_32& f_bytesRead) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (this->m_pSharedBuf != NULL) {
    if (TRUE != this->m_pSharedBuf->IsOpen()) {
      l_eStatus = m_pSharedBuf->Open();
    }
    if (l_eStatus == eFrameworkunifiedStatusOK) {
      f_bytesRead = m_pSharedBuf->Read(reinterpret_cast<char *>(f_data), f_length);
      if (NS_SHM_ERROR != f_bytesRead) {
        l_eStatus = eFrameworkunifiedStatusOK;
      } else {
        l_eStatus = eFrameworkunifiedStatusFail;
      }
    }

  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return (l_eStatus);
}

EFrameworkunifiedStatus CMemReader::ReadToFile(std::string f_fileName, UI_32& f_Written) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  if ((NULL != m_pSharedBuf) &&  // LCOV_EXCL_BR_LINE 5: new will aways sucess, so m_pSharedBuf will noe be null
      (f_fileName.length() != 0)) {
    l_eStatus = m_pSharedBuf->DumpToFile(f_fileName.c_str(), &f_Written);
  }

  return l_eStatus;
}

void CMemReader::Close(void) {
  if (NULL != m_pSharedBuf) {  // LCOV_EXCL_BR_LINE 6: m_pSharedBuf can not be null
    // un-map the shared memory object
    (void) m_pSharedBuf->Close();

    delete m_pSharedBuf;
    m_pSharedBuf = NULL;
  }
}

EFrameworkunifiedStatus CMemReader::ResetPosition(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusNullPointer;
  if (NULL != m_pSharedBuf) {  // LCOV_EXCL_BR_LINE 6:Because m_pSharedBuf is always NULL
    // un-map the shared memory object
    l_eStatus = m_pSharedBuf->SetReadPtrToWritePtr();
    LOG_STATUS_IF_ERRORED(l_eStatus, "m_pSharedBuf->SetReadPtrToWritePtr();");  // LCOV_EXCL_BR_LINE 15:Macro
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %d", l_eStatus);
  return (l_eStatus);
}
}  // namespace ReaderWriter
