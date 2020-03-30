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
/// \brief    This file contains implementation of CNSConfigWriter class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_config_parser_if.h>
#include <native_service/ns_writer.h>
#include <dlfcn.h>
#include <sstream>
#include <cstring>
#include <string>
#include "ns_cfg_writer.h"
#include "ns_config_parser_frameworkunifiedlog.h"


CNSConfigWriter::CNSConfigWriter(): m_pWriter(NULL) {
}

CNSConfigWriter::CNSConfigWriter(const std::string &f_c_filepath): m_pWriter(NULL) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "File Path %s", f_c_filepath.c_str());

  Parse(f_c_filepath);
}

CNSConfigWriter::CNSConfigWriter(const std::string &f_c_filepath, BOOL f_b_noload): m_pWriter(NULL) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "File Path %s", f_c_filepath.c_str());

  Create(f_c_filepath);
}

CNSConfigWriter::~CNSConfigWriter() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Destructor");

  if (NULL != m_pWriter) {
    delete m_pWriter;  // LCOV_EXCL_BR_LINE 11:except branch
    m_pWriter = NULL;
  }
  // TODO(my_username): dlclose.
}

EFrameworkunifiedStatus CNSConfigWriter::Parse(const std::string &f_c_filepath) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  if (NULL != m_pWriter) {
    delete m_pWriter;  // LCOV_EXCL_BR_LINE 11:except branch
    m_pWriter = NULL;
  }

  // create parser object depending on file extension
  if (std::strstr(f_c_filepath.c_str(), ".cfg") != NULL) {
    m_pWriter = new(std::nothrow) CCFGWriter();  // LCOV_EXCL_BR_LINE 11:except branch
  } else {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Not CFG");
    // LCOV_EXCL_BR_STOP
  }

  if (NULL != m_pWriter) {
    l_e_status = m_pWriter->ParseFile(f_c_filepath);
  } else {
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}

EFrameworkunifiedStatus CNSConfigWriter::Create(const std::string &f_c_filepath) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  if (NULL != m_pWriter) {
    delete m_pWriter;  // LCOV_EXCL_BR_LINE 11:except branch
    m_pWriter = NULL;
  }

  // create parser object depending on file extension
  if (std::strstr(f_c_filepath.c_str(), ".cfg") != NULL) {
    m_pWriter = new(std::nothrow) CCFGWriter();  // LCOV_EXCL_BR_LINE 11:except branch
  } else {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Not CFG");
    // LCOV_EXCL_BR_STOP
    l_e_status = eFrameworkunifiedStatusFail;
  }

  if (NULL != m_pWriter) {
    l_e_status = m_pWriter->SetPath(f_c_filepath);
  } else {
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}

VOID CNSConfigWriter::SetDataPtr(PVOID f_p_data) {
  if (NULL != m_pWriter) {
    m_pWriter->SetDataPtr(f_p_data);
  } else {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "m_pWriter is NULL");
    // LCOV_EXCL_BR_STOP
  }
}

EFrameworkunifiedStatus CNSConfigWriter::Save() {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  if (NULL != m_pWriter) {
    // save the updated config file
    l_e_status = m_pWriter->SaveData();
  } else {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Writer (m_pWriter) is NULL");
    // LCOV_EXCL_BR_STOP
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}

EFrameworkunifiedStatus CNSConfigWriter::SetBool(const std::string &f_c_key, BOOL f_b_value) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  if (NULL != m_pWriter) {
    std::string l_c_temp = "";

    if (TRUE == f_b_value) {
      l_c_temp = "TRUE";
      l_e_status = m_pWriter->SetValue(f_c_key, l_c_temp);  // LCOV_EXCL_BR_LINE 11:except branch
    } else if (FALSE == f_b_value) {  // LCOV_EXCL_BR_LINE 11:except branch
      l_c_temp = "FALSE";
      l_e_status = m_pWriter->SetValue(f_c_key, l_c_temp);  // LCOV_EXCL_BR_LINE 11:except branch
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "received bool value is not proper:: %d", f_b_value);
      l_e_status = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Writer (m_pWriter) is NULL");
    // LCOV_EXCL_BR_STOP
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}

EFrameworkunifiedStatus CNSConfigWriter::Set(const std::string &f_cKey, F_64 f_f_value) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  if (NULL != m_pWriter) {
    std::stringstream l_cStream;
    l_cStream << f_f_value;

    l_e_status = m_pWriter->SetValue(f_cKey, l_cStream.str());  // LCOV_EXCL_BR_LINE 11:except branch
  } else {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Writer (m_pWriter) is NULL");
    // LCOV_EXCL_BR_STOP
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}

EFrameworkunifiedStatus CNSConfigWriter::Set(const std::string &f_cKey, F_32 f_fValue) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  if (NULL != m_pWriter) {  // LCOV_EXCL_BR_LINE 11:except branch
    std::stringstream l_cStream;
    l_cStream << f_fValue;

    l_e_status = m_pWriter->SetValue(f_cKey, l_cStream.str());  // LCOV_EXCL_BR_LINE 11:except branch
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Writer (m_pWriter) is NULL");
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}

EFrameworkunifiedStatus CNSConfigWriter::Set(const std::string &f_cKey, SI_32 f_si_value) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  if (NULL != m_pWriter) {
    std::stringstream l_cStream;
    l_cStream << f_si_value;  // LCOV_EXCL_BR_LINE 11:except branch

    l_e_status = m_pWriter->SetValue(f_cKey, l_cStream.str());  // LCOV_EXCL_BR_LINE 11:except branch
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Writer (m_pWriter) is NULL");  // LCOV_EXCL_BR_LINE 11:except branch
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}

EFrameworkunifiedStatus CNSConfigWriter::Set(const std::string &f_cKey, const std::string &f_s_value) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  if (NULL != m_pWriter) {
    l_e_status = m_pWriter->SetValue(f_cKey, f_s_value);  // LCOV_EXCL_BR_LINE 11:except branch
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Writer (m_pWriter) is NULL");  // LCOV_EXCL_BR_LINE 11:except branch
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}
