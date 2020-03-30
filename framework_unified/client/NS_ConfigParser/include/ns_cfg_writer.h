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
/// \brief    This file contains declaration of class CCFGWriter.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_CONFIGPARSER_INCLUDE_NS_CFG_WRITER_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_CONFIGPARSER_INCLUDE_NS_CFG_WRITER_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_writer.h>
#include <string>

class CCFGParser;

/*
 * This class provides functionalities to write to CFG config file
 */
class CCFGWriter: public IConfigWriter {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CCFGWriter
  /// Constructor of CCFGWriter class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CCFGWriter();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CCFGWriter
  /// Constructor of CCFGWriter class
  ///
  /// \param  [IN] f_c_filepath
  ///     std::string - Full path of the configuration file
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  explicit CCFGWriter(const std::string &f_c_filepath);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CCFGWriter
  /// Destructor of CCFGWriter class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CCFGWriter();

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
  /// SetValue
  /// This function is used to set the value for the key
  ///
  /// \param  [IN] f_c_key
  ///     std::string - key to search
  /// \param  [IN] f_c_value
  ///     std::string - value to set
  ///
  /// \return EFrameworkunifiedStatus
  ///     EFrameworkunifiedStatus - error if key not found else eFrameworkunifiedStatusOK
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetValue(const std::string &f_c_key, std::string f_c_value);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SaveData
  /// This function is used to save the changed value to the cfg config file
  ///
  /// \param
  ///
  /// \return EFrameworkunifiedStatus - success or failure
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SaveData();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetDataPtr
  /// This function is used to set the config file path
  ///
  /// \param  [IN] f_c_path
  ///     std::string - Path of file
  ///
  /// \return EFrameworkunifiedStatus - success or failure
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetPath(const std::string &f_c_path);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetDataPtr
  /// This function is used to set the data pointer in writer class with data pointer
  /// created in config reader class.
  /// This is needed to avoid recreation of same data structure object in both reader and writer
  /// when we create object of NSConfigParser
  ///
  /// \param  [IN] f_p_data
  ///     PVOID - Pointer to data structure
  ///
  /// \return VOID
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetDataPtr(PVOID f_p_data);

 private:
  // path of config file
  std::string m_cFilePath;

  // CFG file parser object
  CCFGParser *m_pCFGParser;
};

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_CONFIGPARSER_INCLUDE_NS_CFG_WRITER_H_
