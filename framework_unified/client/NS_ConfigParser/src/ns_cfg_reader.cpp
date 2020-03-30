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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_ConfigParser
/// \brief    This file contains implementation of CCFGReader class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <ns_cfg_reader.h>
#include <ns_cfg_parser.h>
#include <ns_config_parser_frameworkunifiedlog.h>
#include <string>



CCFGReader::CCFGReader(): m_cFilePath(""), m_pCFGParser(NULL) {  // LCOV_EXCL_BR_LINE 11:except branch
}

// LCOV_EXCL_START 200:CCFGReader is not external API class, this constructed function isn't used by other function
CCFGReader::CCFGReader(const std::string &f_c_filepath): m_cFilePath(f_c_filepath), m_pCFGParser(NULL) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "File Path %s", m_cFilePath.c_str());

  m_pCFGParser = new CCFGParser(m_cFilePath);

  if (NULL == m_pCFGParser) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Memory allocation error for m_pCFGParser");
  }
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CCFGReader::ParseFile(const std::string &f_c_filepath) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "File Path %s", f_c_filepath.c_str());

  m_cFilePath.assign(f_c_filepath);

  m_pCFGParser = new(std::nothrow) CCFGParser();  // LCOV_EXCL_BR_LINE 11:except branch

  if (NULL == m_pCFGParser) {  // LCOV_EXCL_BR_LINE 5:new's error case
    // LCOV_EXCL_START 5:new's error case
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

CCFGReader::~CCFGReader() {
  if (NULL != m_pCFGParser) {
    delete m_pCFGParser;
    m_pCFGParser = NULL;
  }
}

PVOID CCFGReader::GetDataPtr() {
  return m_pCFGParser;
}

std::string CCFGReader::GetValue(const std::string &f_c_key) {
  std::string l_cValue = "";

  if (NULL != m_pCFGParser && !f_c_key.empty()) {  // LCOV_EXCL_BR_LINE 11:except branch
    // read the data from internal CFG structure
    l_cValue = m_pCFGParser->CFGGetData(f_c_key);  // LCOV_EXCL_BR_LINE 11:except branch
  }

  return l_cValue;
}

EFrameworkunifiedStatus CCFGReader::GetValue(const std::string &f_c_key, std::string &f_c_value) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  if (NULL == m_pCFGParser) {  // LCOV_EXCL_BR_LINE 5:m_pCFGParser cannot be NULL, because internal logic guarantee
    // LCOV_EXCL_START 5:m_pCFGParser cannot be NULL, because internal logic guarantee
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_e_status = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Parser object(m_pCFGParser) is NULL");
    // LCOV_EXCL_STOP
  } else if (f_c_key.empty()) {
    l_e_status = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Invalid Key");
    // LCOV_EXCL_BR_STOP
  } else {
    // read the data from internal CFG structure
    l_e_status = m_pCFGParser->CFGGetData(f_c_key, f_c_value);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_e_status;
}
