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
/// \brief    This file contains implementation of CNSConfigParser class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_config_parser_if.h>
#include <string>
#include "ns_config_parser_frameworkunifiedlog.h"

CNSConfigParser::CNSConfigParser() : CNSConfigReader(), CNSConfigWriter() {  // LCOV_EXCL_BR_LINE 11:except branch
}

CNSConfigParser::CNSConfigParser(const std::string &f_c_filepath): CNSConfigReader(f_c_filepath),
  CNSConfigWriter(f_c_filepath, TRUE) {  // LCOV_EXCL_BR_LINE 11:except branch
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "File Path %s", f_c_filepath.c_str());

  // get the data pointer from the config reader
  PVOID l_pdata = GetDataPtr();  // LCOV_EXCL_BR_LINE 11:except branch

  // set the same data pointer in config writer
  SetDataPtr(l_pdata);  // LCOV_EXCL_BR_LINE 11:except branch
}

CNSConfigParser::~CNSConfigParser() {  // LCOV_EXCL_BR_LINE 11:except branch
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Destructor");

  // set the data pointer to NULL in writer
  SetDataPtr(NULL);  // LCOV_EXCL_BR_LINE 11:except branch
}

EFrameworkunifiedStatus CNSConfigParser::Parse(const std::string &f_c_filepath) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "File Path %s", f_c_filepath.c_str());

  // parse and create config document structure with reader
  if (eFrameworkunifiedStatusOK == (l_e_status = CNSConfigReader::Parse(f_c_filepath))) {
    // set the data pointer to NULL in writer
    SetDataPtr(NULL);

    // sets the same config document structure in writer
    if (eFrameworkunifiedStatusOK == (l_e_status = CNSConfigWriter::Create(f_c_filepath))) {
      // get the data pointer from the config reader
      PVOID l_pdata = GetDataPtr();

      // set the same data pointer in config writer
      SetDataPtr(l_pdata);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_e_status;
}
