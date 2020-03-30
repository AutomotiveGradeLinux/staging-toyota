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
/// \brief    This file contains implementation of CNSConfigReader class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>

#include <native_service/ns_config_parser_if.h>
#include <native_service/ns_reader.h>

#include <cstring>
#include <string>

#include "ns_config_parser_frameworkunifiedlog.h"
#include "ns_cfg_reader.h"
CNSConfigReader::CNSConfigReader() {
  m_pReader = NULL;
}

CNSConfigReader::CNSConfigReader(const std::string &f_c_filepath): m_pReader(NULL) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "File Path %s", f_c_filepath.c_str());

  if (eFrameworkunifiedStatusOK != Parse(f_c_filepath)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error Parsing file %s", f_c_filepath.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }
}

CNSConfigReader::~CNSConfigReader() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Destructor");

  if (NULL != m_pReader) {
    delete m_pReader;  // LCOV_EXCL_BR_LINE 11:except branch
    m_pReader = NULL;
  }
}

EFrameworkunifiedStatus CNSConfigReader::Parse(const std::string &f_c_filepath) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "File Path %s", f_c_filepath.c_str());

  if (NULL != m_pReader) {
    delete m_pReader;  // LCOV_EXCL_BR_LINE 11:except branch
    m_pReader = NULL;
  }

  // create parser object depending on file extension
  if (NULL != std::strstr(f_c_filepath.c_str(), ".cfg")) {
    m_pReader = new(std::nothrow) CCFGReader();  // LCOV_EXCL_BR_LINE 11:except branch
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Not CFG");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    l_e_status = eFrameworkunifiedStatusFail;
  }

  if (NULL != m_pReader) {
    l_e_status = m_pReader->ParseFile(f_c_filepath);
  } else {
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_e_status;
}

PVOID CNSConfigReader::GetDataPtr() {
  if (NULL != m_pReader) {
    // get the data pointer
    return m_pReader->GetDataPtr();
  } else {
    return NULL;
  }
}

BOOL CNSConfigReader::GetBool(const std::string &f_c_key) {
  BOOL l_bResult = FALSE;

  if (NULL != m_pReader) {
    std::string l_c_temp = m_pReader->GetValue(f_c_key);

    l_c_temp = FormatValue(l_c_temp);  // LCOV_EXCL_BR_LINE 11:except branch

    if (!strcasecmp(l_c_temp.c_str(), "true")) {
      l_bResult = TRUE;
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Reader (m_pReader) is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  return l_bResult;
}

F_64 CNSConfigReader::GetDouble(const std::string &f_c_key) {
  F_64 l_fValue = 0;

  if (NULL != m_pReader) {
    std::string l_c_temp = m_pReader->GetValue(f_c_key);

    errno = EOK;

    // convert string to double
    l_fValue = strtod(l_c_temp.c_str(), NULL);

    if (EOK != errno) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error converting string %s to double, errno=%d", l_c_temp.c_str(), errno);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Reader (m_pReader) is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  return l_fValue;
}

F_32 CNSConfigReader::GetFloat(const std::string &f_c_key) {
  F_32 l_fValue = 0;

  if (NULL != m_pReader) {
    std::string l_c_temp = m_pReader->GetValue(f_c_key);
    errno = EOK;
    l_fValue = strtof(l_c_temp.c_str(), NULL);

    if (EOK != errno) {
      l_fValue = 0;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error converting string %s to float, errno=%d", l_c_temp.c_str(), errno);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Reader (m_pReader) is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  return l_fValue;
}

SI_32 CNSConfigReader::GetInt(const std::string &f_c_key) {
  UI_64 l_ui_value = 0;

  if (NULL != m_pReader) {
    std::string l_c_temp = m_pReader->GetValue(f_c_key);

    errno = EOK;

    if ((l_c_temp.size() >= 3) && ('0' == l_c_temp[0]) && ('X' == toupper(l_c_temp[1]))) {
      l_ui_value = strtoul(l_c_temp.c_str(), NULL, 16);
    } else {
      l_ui_value = strtoul(l_c_temp.c_str(), NULL, 10);
    }

    if (EOK != errno) {
      l_ui_value = 0;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error converting string %s to int, errno=%d", l_c_temp.c_str(), errno);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Reader (m_pReader) is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  return static_cast<SI_32>(l_ui_value);
}

std::string CNSConfigReader::GetString(const std::string &f_c_key) {
  std::string l_c_temp = "";

  if (NULL != m_pReader) {
    l_c_temp = m_pReader->GetValue(f_c_key);  // LCOV_EXCL_BR_LINE 11:except branch

    l_c_temp = FormatValue(l_c_temp);  // LCOV_EXCL_BR_LINE 11:except branch
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Reader (m_pReader) is NULL");
  }

  return l_c_temp;
}

EFrameworkunifiedStatus CNSConfigReader::GetBool(const std::string &f_c_key, BOOL &f_b_value) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  std::string l_c_temp = "";

  if (NULL != m_pReader) {
    l_e_status = m_pReader->GetValue(f_c_key, l_c_temp);  // LCOV_EXCL_BR_LINE 11:except branch

    if (eFrameworkunifiedStatusOK == l_e_status) {
      l_c_temp = FormatValue(l_c_temp);  // LCOV_EXCL_BR_LINE 11:except branch

      if (!strcasecmp(l_c_temp.c_str(), "true")) {
        f_b_value = TRUE;
      } else if (!strcasecmp(l_c_temp.c_str(), "false")) {
        f_b_value = FALSE;
      } else {
        l_e_status = eFrameworkunifiedStatusErrOther;
      }
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Reader (m_pReader) is NULL");
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}

EFrameworkunifiedStatus CNSConfigReader::GetDouble(const std::string &f_c_key, F_64 &f_d_value) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  std::string l_c_temp = "";

  if (NULL != m_pReader) {
    l_e_status = m_pReader->GetValue(f_c_key, l_c_temp);  // LCOV_EXCL_BR_LINE 11:except branch

    if (eFrameworkunifiedStatusOK == l_e_status) {
      errno = EOK;
      f_d_value = strtod(l_c_temp.c_str(), NULL);

      if (EOK != errno) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error converting string %s to double, errno=%d", l_c_temp.c_str(), errno);
        l_e_status = eFrameworkunifiedStatusInvldBuf;
      }
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Reader (m_pReader) is NULL");
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}

EFrameworkunifiedStatus CNSConfigReader::GetFloat(const std::string &f_c_key, F_32 &f_f_value) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  std::string l_c_temp = "";

  if (NULL != m_pReader) {
    l_e_status = m_pReader->GetValue(f_c_key, l_c_temp);  // LCOV_EXCL_BR_LINE 11:except branch

    if (eFrameworkunifiedStatusOK == l_e_status) {
      errno = EOK;
      f_f_value = strtof(l_c_temp.c_str(), NULL);

      if (EOK != errno) {
        f_f_value = 0;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error converting string %s to float, errno=%d", l_c_temp.c_str(), errno);
        l_e_status = eFrameworkunifiedStatusInvldBuf;
      }
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Reader (m_pReader) is NULL");
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}

EFrameworkunifiedStatus CNSConfigReader::GetInt(const std::string &f_c_key, SI_32 &f_i_value) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  std::string l_c_temp = "";

  UI_64 l_ui_value = 0;

  if (NULL != m_pReader) {
    l_e_status = m_pReader->GetValue(f_c_key, l_c_temp);  // LCOV_EXCL_BR_LINE 11:except branch

    if (eFrameworkunifiedStatusOK == l_e_status) {
      errno = EOK;

      if ((l_c_temp.size() >= 3) && ('0' == l_c_temp[0]) && ('X' == toupper(l_c_temp[1]))) {
        l_ui_value = strtoul(l_c_temp.c_str(), NULL, 16);
      } else {
        l_ui_value = strtoul(l_c_temp.c_str(), NULL, 10);
      }

      f_i_value = static_cast<SI_32>(l_ui_value);

      if (EOK != errno) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error converting string %s to int, errno=%d", l_c_temp.c_str(), errno);
        l_e_status = eFrameworkunifiedStatusInvldBuf;
      }
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Reader (m_pReader) is NULL");
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}

EFrameworkunifiedStatus CNSConfigReader::GetString(const std::string &f_c_key, std::string &f_c_value) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  std::string l_c_temp = "";

  if (NULL != m_pReader) {
    l_e_status = m_pReader->GetValue(f_c_key, l_c_temp);  // LCOV_EXCL_BR_LINE 11:except branch

    if (eFrameworkunifiedStatusOK == l_e_status) {
      l_c_temp = FormatValue(l_c_temp);  // LCOV_EXCL_BR_LINE 11:except branch

      f_c_value.assign(l_c_temp);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Config Reader (m_pReader) is NULL");
    l_e_status = eFrameworkunifiedStatusNullPointer;
  }

  return l_e_status;
}

std::string CNSConfigReader::FormatValue(const std::string &f_c_value) {
  std::string l_c_newvalue = f_c_value;

  if (0 < f_c_value.length()) {
    UI_32 l_uiStrPos = static_cast<UI_32>(l_c_newvalue.length() - 1);

    while ('\n' == l_c_newvalue[l_uiStrPos] || '\r' == l_c_newvalue[l_uiStrPos]) {
      l_c_newvalue = l_c_newvalue.substr(0, l_uiStrPos);

      if (0 == l_uiStrPos) {
        break;
      }

      l_uiStrPos--;
    }
  }

  return l_c_newvalue;
}
