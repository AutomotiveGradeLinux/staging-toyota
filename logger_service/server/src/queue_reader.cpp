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
#include "readerWriter/reader/queue_reader.h"
#include <string>

namespace ReaderWriter {
CQueueReader::CQueueReader()
    : m_handle(-1),
      m_queName("") {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
}

CQueueReader::~CQueueReader() {
  if (this->IsOpen()) {
    this->Close();
  }
}

EFrameworkunifiedStatus CQueueReader::Initialize(CLoggerCfg* f_pLoggerCfg,
                                    std::string f_name, UI_32 f_maxSize) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldParam;
  if (f_pLoggerCfg != NULL) {  // LCOV_EXCL_BR_LINE 6:Because the applicable variable cannot be changed from the external API
    this->m_pLoggerCfg = f_pLoggerCfg;
    // Check for invalid name
    if (f_name.length() != 0) {  // LCOV_EXCL_BR_LINE 6:it can not be 0
      m_queName = f_name;
      l_eStatus = this->Open();
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %d", l_eStatus);
  return (l_eStatus);
}

EFrameworkunifiedStatus CQueueReader::Open(void) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldParam;

  if (-1 != m_handle) {  // LCOV_EXCL_BR_LINE 6:Because m_handle is always -1
    // Queue already opened, no action required.
    l_eStatus = eFrameworkunifiedStatusOK;
  } else if (m_queName.length() != 0) {  // LCOV_EXCL_BR_LINE 6:Because m_queName is not always an empty string
    const UI_32 MAX_MESSAGES_STORED_IN_QUEUE = 512;
    struct mq_attr mqattr;
    mqattr.mq_flags = 0;
    mqattr.mq_maxmsg = MAX_MESSAGES_STORED_IN_QUEUE;
    mqattr.mq_msgsize = MAX_QUEUE_MSG_SIZE;

    this->m_handle = mq_open(m_queName.c_str(), O_RDONLY | O_CREAT, 0666,
                             &mqattr);
    if (-1 != m_handle) {  // LCOV_EXCL_BR_LINE 5:mq_open cannot be passed because it cannot be turned mock
      l_eStatus = eFrameworkunifiedStatusOK;
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %d", l_eStatus);
  return (l_eStatus);
}

BOOL CQueueReader::IsOpen(void) {
  return (-1 != m_handle) ? TRUE : FALSE;  // LCOV_EXCL_BR_LINE 6:Because the condition cannot be set
}

EFrameworkunifiedStatus CQueueReader::Read(UI_8* f_data, UI_32 f_length,
                              SI_32& f_bytesRead) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  if (this->m_handle != -1) {  // LCOV_EXCL_BR_LINE 6:Because the condition cannot be set
    if (f_length <= MAX_QUEUE_MSG_SIZE) {  // LCOV_EXCL_BR_LINE 6:Because the applicable variable cannot be changed from the external API
      f_bytesRead = static_cast<SI_32>(mq_receive(this->m_handle, reinterpret_cast<char *>(f_data),
                               (size_t) MAX_QUEUE_MSG_SIZE, NULL));

      if ((0 <= f_bytesRead) && ((UI_32) f_bytesRead <= f_length)) {  // LCOV_EXCL_BR_LINE 6:Because the condition cannot be set
        l_eStatus = eFrameworkunifiedStatusOK;
      }
    }
  }
  return (l_eStatus);
}

void CQueueReader::Close(void) {
  if (this->m_handle != -1) {  // LCOV_EXCL_BR_LINE 200: m_handle is aways not -1
    (void) mq_close(this->m_handle);
  }
}
}  // namespace ReaderWriter
