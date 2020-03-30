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
/// \brief    This file contains the declaration of internal structure for parsing cfg configuration
///       file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_CONFIGPARSER_INCLUDE_NS_CFG_INTERNAL_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_CONFIGPARSER_INCLUDE_NS_CFG_INTERNAL_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>

class CNode {
 public:
  CNode(): m_cComment(""), m_cTag(""), m_cValue("") {}  // LCOV_EXCL_BR_LINE 11:except branch

  ~CNode() {}

  std::string m_cComment;
  std::string m_cTag;
  std::string m_cValue;
};

class CBlock {
 public:
  CBlock(): m_cComment("") {}  // LCOV_EXCL_BR_LINE 11:except branch

  ~CBlock() {}

  std::string m_cComment;
  std::vector<CNode> m_vElementPair;
};

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_CONFIGPARSER_INCLUDE_NS_CFG_INTERNAL_H_
