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
#include "readerWriter/reader/reader.h"
#include <string>
#include <new>
#include <iostream>
#include "readerWriter/reader/queue_reader.h"
#include "readerWriter/reader/mem_reader.h"
#include "ss_logger_cfg.h"
namespace ReaderWriter {
CReader::CReader()
    : m_pLoggerCfg(NULL) {
}

CReader::~CReader() {
}

CReader* CReader::OpenReader(CLoggerCfg* f_pLoggerCfg, EReaderType f_type,
                             std::string f_name, UI_32 f_maxSize) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CReader* l_ret = NULL;
  if (f_pLoggerCfg != NULL) {  // LCOV_EXCL_BR_LINE 200:As it is not always NULL
    if (f_name.length() != 0) {  // LCOV_EXCL_BR_LINE 200:Because it is not always an empty string
      switch (f_type) {  // LCOV_EXCL_BR_LINE 200:As it must be eReaderWriterTypeQueue or eReaderWriterTypeMem
        case eReaderWriterTypeFile:
          break;

        case eReaderWriterTypeQueue:
          l_ret = new (std::nothrow) CQueueReader();  // LCOV_EXCL_BR_LINE 5:Cannot pass because it cannot be new
          break;

        case eReaderWriterTypeMem:
          l_ret = new (std::nothrow) CMemReader();  // LCOV_EXCL_BR_LINE 5:Cannot pass because it cannot be new
          break;

        default:
          break;
      }
    }
    if (NULL != l_ret) {  // LCOV_EXCL_BR_LINE 200:As it is not always NULL
      if (eFrameworkunifiedStatusOK != l_ret->Initialize(f_pLoggerCfg, f_name, f_maxSize)) {
        delete (l_ret);
        l_ret = NULL;
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_ret;
}


EFrameworkunifiedStatus CReader::ReadToFile(std::string f_fileName, UI_32& f_Written) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusNotImplemented;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CReader::ResetPosition(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusNotImplemented;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
}  // namespace ReaderWriter
