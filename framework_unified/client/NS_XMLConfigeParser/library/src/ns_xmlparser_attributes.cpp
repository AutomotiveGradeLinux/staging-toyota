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
/// \ingroup  tag_NS_XmlParser
/// \brief    This file contains implementation of CXmlAttr class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_xmlparser_if.h>
#include <string>
#include "ns_xmlconfig_parser_frameworkunifiedlog.h"

////////////////////////////////////////////////////////////////////////////////////////////////
/// CXmlAttr
/// Constructor of CXmlAttr class
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlAttr::CXmlAttr(): m_cKey(""), m_cValue("") {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// CXmlAttr
/// Parameterized constructor of CXmlAttr class
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlAttr::CXmlAttr(std::string f_cKey, std::string f_cValue): m_cKey(f_cKey), m_cValue(f_cValue) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CXmlAttr
/// Destructor of CXmlAttr class
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlAttr::~CXmlAttr() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
