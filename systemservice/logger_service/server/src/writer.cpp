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
#include <string>
#include <new>
#include <iostream>
#include "readerWriter/writer/writer.h"
#include "readerWriter/writer/file_writer.h"
#include "readerWriter/writer/udp_writer.h"
#include "readerWriter/writer/udp_file_writer.h"
#include "readerWriter/writer/cached_file_writer.h"

namespace ReaderWriter {

CWriter::CWriter()
    : m_pLoggerCfg(NULL) {
}

CWriter::~CWriter() {
}

CWriter* CWriter::OpenWriter(CLoggerCfg* f_pLoggerCfg, EReaderType f_type,
                             std::string f_Name1, UI_32 f_size1,
                             std::string f_Name2, UI_32 f_size2) {
  CWriter* l_ret = NULL;
  if ((f_Name1.length() != 0) && (f_pLoggerCfg != NULL)) {  // LCOV_EXCL_BR_LINE 200:As it is always TRUE
    switch (f_type) {// LCOV_EXCL_BR_LINE 200:As it must be eReaderWriterTypeQueue or eReaderWriterTypeMem
      case eReaderWriterTypeFile:
        l_ret = new (std::nothrow) CCachedFileWriter();  // LCOV_EXCL_BR_LINE 5:Cannot pass because it cannot be new
        break;

      case eReaderWriterTypeUdp:
        l_ret = new (std::nothrow) CUdpWriter();  // LCOV_EXCL_BR_LINE 5:Cannot pass because it cannot be new
        break;
      case eReaderWriterTypeUdpAndFile:  // LCOV_EXCL_START 8:dead code
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        if (f_Name2.length() != 0) {
          l_ret = new (std::nothrow) CUdpFileWriter();
        }
      // LCOV_EXCL_STOP
      default:
        break;
    }
  }
  if (NULL != l_ret) {  // LCOV_EXCL_BR_LINE 200:As it is not always NULL
    if (eFrameworkunifiedStatusOK  != l_ret->Initialize(f_pLoggerCfg, f_Name1, f_size1, f_Name2, f_size2)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Open Writer Failed");
      delete (l_ret);
      l_ret = NULL;
    }
  }

  return l_ret;
}

EFrameworkunifiedStatus CWriter::FlushCache(void) {  // LCOV_EXCL_START 8:Base class virtual functions
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Default function called!");
  return (eFrameworkunifiedStatusOK);
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CWriter::UpdateLoggingParameters(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Default function called!");
  return (eFrameworkunifiedStatusOK);
}
}  // namespace ReaderWriter

