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
/// \brief    This file contains the declaration of class CCFGParser.
///       This class contains the parsing logic for reading and writing from and to cfg files
///       respectively.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_CONFIGPARSER_INCLUDE_NS_CFG_PARSER_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_CONFIGPARSER_INCLUDE_NS_CFG_PARSER_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>
#include <string>
#include <map>

// forward declaration of classes
class CNode;
class CBlock;

typedef std::map<std::string, CBlock *> CFGData_Type;

class CCFGParser {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CCFGParser
  /// Constructor of CCFGReader class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CCFGParser();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CCFGParser
  /// Constructor of CCFGReader class
  ///
  /// \param  [IN] f_c_filepath
  ///     std::string - Full path of the configuration file
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  explicit CCFGParser(const std::string &f_c_filepath);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CCFGParser
  /// Destructor of CCFGReader class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CCFGParser();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CFGParseFile
  /// This function is used to parse the configuration file and store data in internal structure
  ///
  /// \param  [IN] f_c_filepath
  ///     std::string - full path of the configuration file
  ///
  /// \return EFrameworkunifiedStatus - success or failure
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CFGParseFile(const std::string &f_c_filepath);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CFGGetData
  /// This function is used to get the value associated with the key from internal CFG structure
  ///
  /// \param  [IN] f_c_key
  ///     std::string - key to search
  ///
  /// \return std::string - value for key
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  std::string CFGGetData(const std::string &f_c_key);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CFGGetData
  /// This function is used to get the value associated with the key from internal CFG structure
  ///
  /// \param  [IN] f_c_key
  ///     std::string - key to search
  /// \param  [IN] f_c_value
  ///     std::string - Value of key
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - error if key not founf or else eFrameworkunifiedStatusOK
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CFGGetData(const std::string &f_c_key, std::string &f_c_value);  // NOLINT (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CFGSetData
  /// This function is used to set the value for the key in internal CFG structure
  ///
  /// \param  [IN] f_c_key
  ///     std::string - key to search
  /// \param  [IN] f_c_value
  ///     std::string - value to set
  ///
  /// \return EFrameworkunifiedStatus - success or failure
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CFGSetData(const std::string &f_c_key, std::string f_c_value);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CFGSaveData
  /// This function is used to save the changed value to the cfg config file
  ///
  /// \param  [IN] f_c_filepath
  ///     std::string - full path of the configuration file
  ///
  /// \return EFrameworkunifiedStatus - success or failure
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CFGSaveData(const std::string &f_c_filepath);

 private:
  // internal data structure created by parsing the cfg file
  CFGData_Type *m_pmCFGData;
  std::string m_cLastComment;
};

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_CONFIGPARSER_INCLUDE_NS_CFG_PARSER_H_
