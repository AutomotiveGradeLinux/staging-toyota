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
/// \brief    This file contains declaration of class CCFGReader.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_CONFIGPARSER_INCLUDE_NS_CFG_READER_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_CONFIGPARSER_INCLUDE_NS_CFG_READER_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_reader.h>
#include <string>

class CCFGParser;

/*
 * This class provides functionalities to read from CFG config file
 */
class CCFGReader: public IConfigReader {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CCFGReader
  /// Constructor of CCFGReader class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CCFGReader();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CCFGReader
  /// Constructor of CCFGReader class
  ///
  /// \param  [IN] f_c_filepath
  ///     std::string - Full path of the configuration file
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  explicit CCFGReader(const std::string &f_c_filepath);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CCFGReader
  /// Destructor of CCFGReader class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CCFGReader();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ParseFile
  /// This function is used to parse the file
  ///
  /// \param  [IN] f_c_filepath
  ///     std::string - path of file to parse
  ///
  /// \return EFrameworkunifiedStatus - success or failure
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ParseFile(const std::string &f_c_filepath);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetString
  /// This function is used to get the value associated with the key
  ///
  /// \param  [IN] f_c_key
  ///     std::string - key to search
  ///
  /// \return std::string - value for key
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  std::string GetValue(const std::string &f_c_key);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetString
  /// This function is used to get the value associated with the key
  ///
  /// \param  [IN] f_c_key
  ///     std::string - key to search
  /// \param  [REF] f_c_value
  ///     std::string - Value of key
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - error if key not found else eFrameworkunifiedStatusOK
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetValue(const std::string &f_c_key, std::string &f_c_value);  // NOLINT (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetDataPtr
  /// This function is used to get the data pointer. This pointer is then set in config writer.
  /// This is needed to avoid recreation of same data structure object in both reader and writer
  /// when we create object of NSConfigParser.
  ///
  /// \param
  ///
  /// \return PVOID - pointer of data structure
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  PVOID GetDataPtr();

 private:
  // path of config file
  std::string m_cFilePath;

  // CFG file parser object
  CCFGParser *m_pCFGParser;
};

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_CONFIGPARSER_INCLUDE_NS_CFG_READER_H_
