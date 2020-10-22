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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_ConfigParser
/// \brief    This file contains implementation of CCFGWriter class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

#include <ns_cfg_parser.h>
#include <ns_cfg_writer.h>
#include <ns_config_parser_frameworkunifiedlog.h>

#include <fstream>
#include <string>


CCFGWriter::CCFGWriter(): m_cFilePath(""), m_pCFGParser(NULL) {  // LCOV_EXCL_BR_LINE 11:except branch
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
}

CCFGWriter::CCFGWriter(const std::string &f_c_filepath): m_cFilePath(f_c_filepath), m_pCFGParser(NULL) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "File Path %s", m_cFilePath.c_str());

  m_pCFGParser = new CCFGParser(m_cFilePath);

  if (NULL == m_pCFGParser) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Memory allocation error for m_pCFGParser");
  }
}

CCFGWriter::~CCFGWriter() {
  if (NULL != m_pCFGParser) {
    delete m_pCFGParser;  // LCOV_EXCL_BR_LINE 11:except branch
    m_pCFGParser = NULL;
  }
}

EFrameworkunifiedStatus CCFGWriter::ParseFile(const std::string &f_c_filepath) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "File Path %s", f_c_filepath.c_str());

  m_cFilePath.assign(f_c_filepath);

  m_pCFGParser = new(std::nothrow) CCFGParser();  // LCOV_EXCL_BR_LINE 11:except branch

  if (NULL == m_pCFGParser) {  // LCOV_EXCL_BR_LINE 5:m_pCFGParser cannot be NULL, because internal logic guarantee
    // LCOV_EXCL_START 5:m_pCFGParser cannot be NULL, because internal logic guarantee
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Memory allocation error for m_pCFGParser");
    l_e_status = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  } else {
    l_e_status = m_pCFGParser->CFGParseFile(f_c_filepath);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_e_status;
}

EFrameworkunifiedStatus CCFGWriter::SetPath(const std::string &f_c_path) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  if (!f_c_path.empty()) {  // LCOV_EXCL_BR_LINE 11:except branch
    // set the file path
    m_cFilePath.assign(f_c_path);
  } else {
    l_e_status = eFrameworkunifiedStatusInvldParam;
  }

  return l_e_status;
}

VOID CCFGWriter::SetDataPtr(PVOID f_p_data) {
  if (NULL != f_p_data) {
    // set the doc pointer
    m_pCFGParser = static_cast<CCFGParser *>(f_p_data);
  } else {
    m_pCFGParser = NULL;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Data Pointer not set in cfg writer");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }
}

EFrameworkunifiedStatus CCFGWriter::SetValue(const std::string &f_c_key, std::string f_c_value) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  if (NULL != m_pCFGParser && !f_c_key.empty()) {
    // set the data in internal CFG structure
    l_e_status = m_pCFGParser->CFGSetData(f_c_key, f_c_value);  // LCOV_EXCL_BR_LINE 11:except branch
  } else {
    l_e_status = eFrameworkunifiedStatusInvldParam;
  }

  return l_e_status;
}

EFrameworkunifiedStatus CCFGWriter::SaveData() {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  if (NULL != m_pCFGParser) {
    if (0 == access(m_cFilePath.c_str(), W_OK)) {
      // save the data permanently in config file
      l_e_status = m_pCFGParser->CFGSaveData(m_cFilePath);
    } else {
      l_e_status = eFrameworkunifiedStatusFail;
    }
  } else {
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}
