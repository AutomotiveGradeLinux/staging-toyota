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

/**
 * @file ns_config_parser_if.h
 * @brief \~english This file contains declaration of class CNSConfigReader, CNSConfigWriter and CNSConfigParser. -- API
 *        define head file
 *
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup Native_service
 *  @{
 */
/** @addtogroup native
 *  @ingroup framework_unified
 *  @{
 */
#ifndef NS_NATIVESERVICES_INC_NATIVE_NS_CONFIG_PARSER_IF_H_  // NOLINT  (build/header_guard)
#define NS_NATIVESERVICES_INC_NATIVE_NS_CONFIG_PARSER_IF_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_types.h>
#include <string>

// forward declaration of classes
class IConfigReader;
class IConfigWriter;

/**
 * @class CNSConfigReader
 * @brief \~english Configuration File Read Service Class
 * @par   \~english Brief Introduction
 * \~english This class provides API's to application to read from configuration files.
 *
 */
class CNSConfigReader {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSConfigReader
  /// \~english @par Brief
  ///       Constructor of CNSConfigReader class
  ///       This constructor can only be called by derived class. Application cannot use this constructor
  ///       to create object of CNSConfigReader class.
  /// \~english @retval none
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       It is called when generating the instance of CNSConfigParser class.
  /// \~english @see CNSConfigReader::~CNSConfigReader
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNSConfigReader();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSConfigReader
  /// \~english @par Brief
  ///      Constructor of CNSConfigReader class
  ///      This is depricated. Use parameterless constructor instead to create object and
  ///      Parse(filepath) to parse the file using reader object.
  /// \~english @param [in] f_c_filepath
  ///      const std::string&   - Name of the configuration file
  /// \~english @retval none
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       It doesn't need to call Parse() function to execute parse process of configuration file,
  ///       when using the constructor with file path. However, because we don't execute error process,
  ///       if it needs to reply error about parse process, we must to use the constructor without parameter
  ///       and call Parse() function.
  /// \~english @see CNSConfigReader::~CNSConfigReader
  ////////////////////////////////////////////////////////////////////////////////////////////////
  explicit CNSConfigReader(const std::string &f_c_filepath);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Parse
  /// \~english @par Brief
  ///     This function is used to parse configuration file
  /// \~english @param [in] f_c_filepath
  ///     const std::string& - Name of the configuration file
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - if memory allocation failed or document is not proper
  /// \~english @retval eFrameworkunifiedStatusFail - invalid file type
  /// \~english @retval eFrameworkunifiedStatusInvldParam invalid parameter
  /// \~english @retval eFrameworkunifiedStatusErrOther - file data error
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - File(f_c_filepath) designated by parameter is not proper [eFrameworkunifiedStatusNullPointer]
  ///      - Data of file(f_c_filepath) designated by parameter is NULL [eFrameworkunifiedStatusNullPointer]
  ///      - Expansion name of file(f_c_filepath) designated by parameter is not cfg [eFrameworkunifiedStatusFail]
  ///      - Failed to open file(f_c_filepath) designated by parameter [eFrameworkunifiedStatusFail]
  ///      - Path of file designated by parameter is NULL [eFrameworkunifiedStatusInvldParam]
  ///      - Data of file designated by parameter has error [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Parse designated configuration file. If the data format is correct, get memory to store the parsed result,
  ///       and store the parsed contents in it.
  /// \~english @see CNSConfigWriter::Parse, CNSConfigParser::Parse
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Parse(const std::string &f_c_filepath);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~CNSConfigReader
  /// \~english @par Brief
  ///       Destructor of CNSConfigReader class.
  ///       It is used when release the instance.
  /// \~english @retval none
  /// \~english @par Prerequisite
  ///       - Call the construct of CNSConfigReader
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Release the used memory and the parsed result of configuration file.
  /// \~english @see CNSConfigReader::CNSConfigReader
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CNSConfigReader();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetBool
  /// \~english @par Brief
  ///     This function is used to get the bool value associated with key.
  ///     If there are multiple values associated with the same key, then the first value is returned.
  ///     This API has been depricated, use GetBool(key, ref_value)
  /// \~english @param [in] f_c_key
  ///     const std::string& - key to search
  /// \~english @retval TRUE - value associated with key
  /// \~english @retval FALSE - Failed to get value associated with key
  /// \~english @par Prerequisite
  ///       - Call the construct of CNSConfigReader
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [FALSE]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [FALSE]
  ///      - Can not find search key(f_c_key) designated by parameter [FALSE]
  ///      - Value getted from configuration file is not true or false [FALSE]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Return the value(TRUE/FALSE) associated with key.
  ///       If there are multiple values associated with the same key, return the first value.
  ///       This API has been depricated, please use GetBool(const std::string& f_c_key, BOOL& f_bValue).
  /// \~english @see CNSConfigWriter::SetBool
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL GetBool(const std::string &f_c_key);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetDouble
  /// \~english @par Brief
  ///     This function is used to get the double value associated with key.
  ///     If there are multiple values associated with the same key, then the first value is returned.
  ///     This API has been depricated, use GetDouble(key, ref_value)
  /// \~english @param [in] f_c_key
  ///     const std::string& - key to search
  /// \~english @param [out] - none
  /// \~english @retval F_64 - value associated with key
  /// \~english @retval 0 - Failed to get value associated with key(64bit)
  /// \~english @par Prerequisite
  ///       - Call the construct of CNSConfigReader
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [0]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [0]
  ///      - Can not find search key(f_c_key) designated by parameter [0]
  ///      - Failed to change data getted from configuration file from string type to double type [0]
  /// \~english @par Classification
  ///       Public
  /// \~english @par
  ///      Please pay attention to the following points when you use this API.\n
  ///      - We can take the value in specification only if it is in the range of
  ///        [1.17549435082228750797e-38F, 3.40282346638528859812e+38F](min value of double, max value of double).
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Return the value(64bit) associated with key.
  ///       If there are multiple values associated with the same key, return the first value.
  ///       This API has been depricated, please use GetDouble(const std::string& f_c_key, F_64& f_f_value).
  /// \~english @see CNSConfigWriter::Set
  ////////////////////////////////////////////////////////////////////////////////////////////////
  F_64 GetDouble(const std::string &f_c_key);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetFloat
  /// \~english @par Brief
  ///     This function is used to get the float value associated with key
  ///     If there are multiple values associated with the same key, then the first value is returned.
  ///     This API has been depricated, use GetFloat(key, ref_value)
  /// \~english @param [in] f_c_key
  ///     const std::string& - key to search
  /// \~english @retval F_32 value associated with key
  /// \~english @retval 0 - Failed to get value associated with key(32bit)
  /// \~english @par Prerequisite
  ///       - Call the construct of CNSConfigReader
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [0]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [0]
  ///      - Can not find search key(f_c_key) designated by parameter [0]
  ///      - Can not change the data string, because that data string getted
  ///        from configuration file is over range [9.8765436e+12]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Return the value(32bit) associated with key.
  ///       If there are multiple values associated with the same key, return the first value.
  ///       This API has been depricated, please use GetFloat(const std::string& f_c_key, F_32& f_f_value).
  /// \~english @see CNSConfigWriter::Set
  ////////////////////////////////////////////////////////////////////////////////////////////////
  F_32 GetFloat(const std::string &f_c_key);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetInt
  /// \~english @par Brief
  ///     This function is used to get the integer value associated with key
  ///     If there are multiple values associated with the same key, then the first value is returned.
  ///     This API has been depricated, use GetInt(key, ref_value)
  /// \~english @param [in] f_c_key
  ///     const std::string& - key to search
  /// \~english @retval SI_32 - value associated with key
  /// \~english @retval 0.0f - Failed to get value associated with key(32bit)
  /// \~english @par Prerequisite
  ///       - Call the construct of CNSConfigReader
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [0]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [0]
  ///      - Can not find search key(f_c_key) designated by parameter [0]
  ///      - Failed to change data getted from Configuration file from string type to Int type [0]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Return the value(32bit) associated with key.
  ///       If there are multiple values associated with the same key, return the first value.
  ///       This API has been depricated, please use GetInt(const std::string& f_c_key, SI_32& f_si_value).
  /// \~english @par
  ///    Please pay attention to the following points when you use this API.\n
  ///    - If data string getted from file is larger than INT_MAX(2147483648), return -1.
  /// \~english @see CNSConfigWriter::Set
  ////////////////////////////////////////////////////////////////////////////////////////////////
  SI_32 GetInt(const std::string &f_c_key);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetString
  /// \~english @par Brief
  ///     This function is used to get the string value associated with key
  ///     If there are multiple values associated with the same key, all the values separated by
  ///     delimiter "\n" are returned.
  ///     This API has been depricated, use GetString(key, ref_value)
  /// \~english @param [in] f_c_key
  ///     const std::string& - key to search
  /// \~english @retval std::string value(string) associated with key
  /// \~english @retval nullbuffer - Failed to get value associated with key
  /// \~english @par Prerequisite
  ///       - Call the construct of CNSConfigReader
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [null buffer]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [null buffer]
  ///      - Can not find search key(f_c_key) designated by parameter [null buffer]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Return the value(string) associated with key.
  ///       If there are multiple values associated with the same key,
  ///       return all the values separated by delimiter "\n".
  ///       This API has been depricated, please use GetString(
  ///       const std::string& f_c_key, std::string& f_cValue).
  /// \~english @see CNSConfigWriter::Set
  ////////////////////////////////////////////////////////////////////////////////////////////////
  std::string GetString(const std::string &f_c_key);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetBool
  /// \~english @par Brief
  ///     This function is used to get the bool value associated with key
  ///     If there are multiple values associated with the same key, then the first value is returned.
  /// \~english @param [in] f_c_key
  ///     const std::string& - key string
  /// \~english @param [in] f_b_value
  ///     BOOL& - value associated with key
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - memory allocation failed
  /// \~english @retval eFrameworkunifiedStatusInvldParam - invalid parameter
  /// \~english @retval eFrameworkunifiedStatusFail - can not find key
  /// \~english @retval eFrameworkunifiedStatusErrOther - value is not bool type
  /// \~english @par Prerequisite
  ///       - Call the construct of CNSConfigReader
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [eFrameworkunifiedStatusInvldParam]
  ///      - Can not find search key(f_c_key) designated by parameter [eFrameworkunifiedStatusFail]
  ///      - Value getted from configuration file is not true or false  [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Return the value(TRUE/FALSE) associated with key.
  ///       If there are multiple values associated with the same key, return the first value.
  /// \~english @see CNSConfigWriter::SetBool
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetBool(const std::string &f_c_key, BOOL &f_b_value);  // NOLINT (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetDouble
  /// \~english @par Brief
  ///     This function is used to get the double value associated with key.
  ///     If there are multiple values associated with the same key, then the first value is returned.
  /// \~english @param [in] f_c_key
  ///     const std::string& - key to search
  /// \~english @param [in] f_f_value
  ///     F_64& - value associated with key
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - memory allocation failed
  /// \~english @retval eFrameworkunifiedStatusInvldParam - invalid parameter
  /// \~english @retval eFrameworkunifiedStatusFail - can not find key
  /// \~english @retval eFrameworkunifiedStatusInvldBuf - change error
  /// \~english @par Prerequisite
  ///       - Call the construct of CNSConfigReader
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [eFrameworkunifiedStatusInvldParam]
  ///      - Can not find search key(f_c_key) designated by parameter [eFrameworkunifiedStatusFail]
  ///      - Changed result of data string getted from configuration file is overflow or underflow [eFrameworkunifiedStatusInvldBuf]
  /// \~english @par Classification
  ///       Public
  /// \~english @par
  ///      Please pay attention to the following points when you use this API.\n
  ///      - Return eFrameworkunifiedStatusOK if we didn't excute the changing operation
  ///      - We can take the value in specification only if it is in the range of
  ///        [1.17549435082228750797e-38F, 3.40282346638528859812e+38F](min value of double, max value of double).
  /// \~english @par Type
  ///      Sync Only
  /// \~english @par Detail
  ///       Return the value(64bit) associated with key.
  ///       If there are multiple values associated with the same key, return the first value.
  /// \~english @see CNSConfigWriter::Set
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetDouble(const std::string &f_c_key, F_64 &f_f_value);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetFloat
  /// \~english @par Brief
  ///     This function is used to get the float value associated with key
  ///     If there are multiple values associated with the same key, then the first value is returned.
  /// \~english @param [in] f_c_key
  ///     const std::string& - key to search
  /// \~english @param [in] f_f_value
  ///     F_32& - value associated with key
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - memory allocation failed
  /// \~english @retval eFrameworkunifiedStatusInvldParam - invalid parameter
  /// \~english @retval eFrameworkunifiedStatusFail - can not find key
  /// \~english @retval eFrameworkunifiedStatusInvldBuf - change error
  /// \~english @par Prerequisite
  ///       - Call the construct of CNSConfigReader
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [eFrameworkunifiedStatusInvldParam]
  ///      - Can not find search key(f_c_key) designated by parameter [eFrameworkunifiedStatusFail]
  ///      - Can not change the data string, because that data string getted
  ///        from configuration file is over range [9.8765436e+12]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Return the value(32bit) associated with key.
  ///       If there are multiple values associated with the same key, return the first value.
  /// \~english @see CNSConfigWriter::Set
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetFloat(const std::string &f_c_key, F_32 &f_f_value);  // NOLINT (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetInt
  /// \~english @par Brief
  ///     This function is used to get the integer value associated with key
  ///     If there are multiple values associated with the same key, then the first value is returned.
  /// \~english @param [in] f_c_key
  ///     const std::string& - key to search
  /// \~english @param [out] f_si_value
  ///     SI_32& - value associated with key
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - memory allocation failed
  /// \~english @retval eFrameworkunifiedStatusInvldParam - invalid parameter
  /// \~english @retval eFrameworkunifiedStatusFail - can not find key
  /// \~english @retval eFrameworkunifiedStatusInvldBuf - change error
  /// \~english @par Prerequisite
  ///       - Call the construct of CNSConfigReader
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [eFrameworkunifiedStatusInvldParam]
  ///      - Can not find search key(f_c_key) designated by parameter [eFrameworkunifiedStatusFail]
  ///      - Failed to change data getted from Configuration file from string type to Int type [eFrameworkunifiedStatusInvldBuf]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///      Sync Only
  /// \~english @par Detail
  ///      Return the value(32bit) associated with key.
  ///      If there are multiple values associated with the same key, return all the values separated by delimiter "\n".
  /// \~english @see CNSConfigWriter::Set
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetInt(const std::string &f_c_key, SI_32 &f_si_value);  // NOLINT (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetString
  /// \~english @par Brief
  ///     This function is used to get the string value associated with key
  ///     If there are multiple values associated with the same key, all the values separated by
  ///     delimiter "\n" are returned.
  /// \~english @param [in] f_c_key
  ///     const std::string& - key to search
  /// \~english @param [in] f_c_value
  ///     std::string& - value(string) associated with key
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - memory allocation failed
  /// \~english @retval eFrameworkunifiedStatusInvldParam - invalid parameter
  /// \~english @retval eFrameworkunifiedStatusFail - can not find key
  /// \~english @par Prerequisite
  ///       - Call the construct of CNSConfigReader
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [eFrameworkunifiedStatusInvldParam]
  ///      - Can not find search key(f_c_key) designated by parameter [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Return the value(string) associated with key.
  ///       If there are multiple values associated with the same key,
  ///       return all the values separated by delimiter "\n".
  /// \~english @see CNSConfigWriter::Set
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetString(const std::string &f_c_key, std::string &f_c_value);  // NOLINT (readability/nolint)

 protected:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetDataPtr
  /// This function is used to get the data pointer. This pointer is then set in config writer.
  /// This is needed to avoid recreation of same data structure object in both reader and writer
  /// when we create object of NSConfigParser.
  ///
  /// \return PVOID - pointer of data structure
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  PVOID GetDataPtr();

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// FormatValue
  /// This function is used to remove the line feed or carriage return from string
  ///
  /// \param  [in] f_cValue
  ///     std::string - value
  ///
  /// \return std::string - formated data
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  std::string FormatValue(const std::string &f_c_value);

  // config reader pointer
  IConfigReader *m_pReader;
  static HANDLE m_hNSXMLLibHandle;  // NOLINT (readability/nolint)
};


/**
 * @class CNSConfigWriter
 * @brief \~english File Write Service Class
 * @par   \~english Brief Introduction
 * \~english This class provides API's to application to write to configuration files.
 *
 */
class CNSConfigWriter {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSConfigWriter
  /// \~english @par Brief
  ///     Constructor of CNSConfigWriter class
  /// \~english @retval none
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       It is called when generating the instance of CNSConfigWriter class.
  /// \~english @see CNSConfigWriter::~CNSConfigWriter
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNSConfigWriter();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSConfigWriter
  /// \~english @par Brief
  ///     Constructor of CNSConfigWriter class
  ///     This is depricated. Use parameterless constructor instead to create object and
  ///     Parse(filepath) to parse the file using writer object.
  /// \~english @param [in] f_c_filepath
  ///     const std::string& - Full path of the configuration file
  /// \~english @retval none
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       It doesn't need to call Parse() function to execute parse process of configuration File,
  ///       when using the constructor with file path. However, because we don't execute error process,
  ///       if it needs to reply error about parse process, we must to use the constructor without parameter
  ///       and call Parse() function.
  /// \~english @see CNSConfigWriter::~CNSConfigWriter
  ////////////////////////////////////////////////////////////////////////////////////////////////
  explicit CNSConfigWriter(const std::string &f_c_filepath);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~CNSConfigWriter
  /// \~english @par Brief
  ///     Destructor of CNSConfigWriter class
  /// \~english @retval none
  /// \~english @par Prerequisite
  ///     - Call the construct of CNSConfigWriter
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Release the used memory and the parsed result of Configuration File.
  /// \~english @see CNSConfigWriter::CNSConfigWriter
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CNSConfigWriter();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Parse
  /// \~english @par Brief
  ///     This function is used to parse the configuration file
  /// \~english @param [in] f_c_filepath
  ///     const std::string& - Name of the configuration file
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - if memory allocation failed or document not proper
  /// \~english @retval eFrameworkunifiedStatusFail - invalid file type
  /// \~english @retval eFrameworkunifiedStatusInvldParam - invalid parameter
  /// \~english @retval eFrameworkunifiedStatusErrOther - file data error
  /// \~english @par Prerequisite
  ///     - Call the construct of CNSConfigWriter
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - File(f_c_filepath) designated by parameter is not proper [eFrameworkunifiedStatusNullPointer]
  ///      - Data of file(f_c_filepath) designated by parameter is NULL [eFrameworkunifiedStatusNullPointer]
  ///      - Expansion name of file(f_c_filepath) designated by parameter is not cfg [eFrameworkunifiedStatusFail]
  ///      - Failed to open file(f_c_filepath) designated by parameter [eFrameworkunifiedStatusFail]
  ///      - Path of file designated by parameter is NULL [eFrameworkunifiedStatusInvldParam]
  ///      - Data of file designated by parameter has error [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Parse designated configuration file. If the data format is correct, get memory to store the parsed result,
  ///       and store the parsed contents in it.
  /// \~english @see CNSConfigParser::Parse, CNSConfigWriter::Parse
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Parse(const std::string &f_c_filepath);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Save
  /// \~english @par Brief
  ///     This function is used to save the configuration settings to config file
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - memory allocation failed
  /// \~english @retval eFrameworkunifiedStatusFail - file access fail
  /// \~english @par Prerequisite
  ///     - Call the construct of CNSConfigWriter
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - File data is NULL [eFrameworkunifiedStatusNullPointer]
  ///      - File doesn't exit [eFrameworkunifiedStatusFail]
  ///      - File access error [eFrameworkunifiedStatusFail]
  ///      - Can't open file [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Save the data stored in memory to configuration file.
  /// \~english @see CNSConfigWriter::SetBool, CNSConfigWriter::Set
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Save();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetBool
  /// \~english @par Brief
  ///     This function is used to set the bool value corresponding to key
  ///     This function is not overloaded with other Set() API's of this class because
  ///     BOOL is typedef of UI_32 so when we pass integer or BOOL to Set API it would have
  ///     called the same API.
  ///     If multiple identical keys are found it sets the value for all the keys.
  /// \~english @param [in] f_c_key
  ///     const std::string& - key
  /// \~english @param [in] f_b_value
  ///     BOOL - bool value to set for key
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - memory allocation failed
  /// \~english @retval eFrameworkunifiedStatusInvldParam - invalid parameter
  /// \~english @retval eFrameworkunifiedStatusFail - can not find key
  /// \~english @retval eFrameworkunifiedStatusErrOther - if passsed value is invalid
  /// \~english @par Prerequisite
  ///     - Call the construct of CNSConfigWriter
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - File data is NULL [eFrameworkunifiedStatusNullPointer]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [eFrameworkunifiedStatusInvldParam]
  ///      - Can not find search key(f_c_key) designated by parameter [eFrameworkunifiedStatusFail]
  ///      - Data(f_bValue) of search key designated by parameter is not bool [eFrameworkunifiedStatusInvldParam]
  /// \~english @par Classification
  ///       Public
  /// \~english @par
  ///      Please pay attention to the following points when you use this API.\n
  ///      - Valid condition of search key string: there is no separator and line feed in config tag
  /// \~english @par Type
  ///      Sync Only
  /// \~english @par Detail
  ///      Set the value(bool type) associated with key to data stored in memory.
  ///      If there are multiple keys, set the values associated with all keys.
  /// \~english @see CNSConfigReader::GetBool
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetBool(const std::string &f_c_key, BOOL f_b_value);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Set
  /// \~english @par Brief
  ///     This function is used to set the double value corresponding to key.
  ///     If multiple identical keys are found it sets the value for all the keys.
  /// \~english @param [in] f_c_key
  ///     const std::string& - key
  /// \~english @param [in]  f_f_value
  ///     F_64 - double value to set for key
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - memory allocation failed
  /// \~english @retval eFrameworkunifiedStatusInvldParam - invalid parameter
  /// \~english @retval eFrameworkunifiedStatusFail - can not find key
  /// \~english @par Prerequisite
  ///     - Call the construct of CNSConfigWriter
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - File data is NULL [eFrameworkunifiedStatusNullPointer]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [eFrameworkunifiedStatusInvldParam]
  ///      - Can not find search key(f_c_key) designated by parameter [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par
  ///       Please pay attention to the following points when you use this API.\n
  ///      - We can take the value in specification only if it is in the range of
  ///        [2.22507385850720138309e-308L, 1.7976931348623157e+308L](min value of double, max value of double).
  ///       - Valid condition of search key string: there is no separator and line feed in config tag
  /// \~english @par Type
  ///      Sync Only
  /// \~english @par Detail
  ///      Set the bool type value(64bit) associated with key to data stored in memory.
  ///      If there are multiple keys, set the values associated with all keys.
  /// \~english @see CNSConfigReader::GetDouble
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Set(const std::string &f_c_key, F_64 f_f_value);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Set
  /// \~english @par Brief
  ///     This function is used to set the float value corresponding to key.
  ///     If multiple identical keys are found it sets the value for all the keys.
  /// \~english @param [in]  f_c_key
  ///     const std::string& - key
  /// \~english @param [in]  f_f_value
  ///     F_32 - float value to set for key
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - memory allocation failed
  /// \~english @retval eFrameworkunifiedStatusInvldParam - invalid parameter
  /// \~english @retval eFrameworkunifiedStatusFail - can not find key
  /// \~english @par Prerequisite
  ///     - Call the construct of CNSConfigWriter
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - File data is NULL [eFrameworkunifiedStatusNullPointer]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [eFrameworkunifiedStatusInvldParam]
  ///      - Can not find search key(f_c_key) designated by parameter [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par
  ///       Please pay attention to the following points when you use this API.\n
  ///      - We can take the value in specification only if it is in the range of
  ///        [1.17549435082228750797e-38F, 3.40282346638528859812e+38F](min value of double, max value of double).
  ///       - Valid condition of search key string: there is no separator and line feed in config tag
  /// \~english @par Type
  ///      Sync Only
  /// \~english @par Detail
  ///      Set the float type value(32bit) associated with key to data stored in memory.
  ///      If there are multiple keys, set the values associated with all keys.
  /// \~english @see CNSConfigReader::GetFloat
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Set(const std::string &f_c_key, F_32 f_f_value);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Set
  /// \~english @par Brief
  ///     This function is used to set the integer value corresponding to key.
  ///     If multiple identical keys are found it sets the value for all the keys.
  /// \~english @param [in] f_c_key
  ///     const std::string& - key
  /// \~english @param [in] f_si_value
  ///     SI_32 - integer value to set for key
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - memory allocation failed
  /// \~english @retval eFrameworkunifiedStatusInvldParam - invalid parameter
  /// \~english @retval eFrameworkunifiedStatusFail - can not find key
  /// \~english @par Prerequisite
  ///     - Call the construct of CNSConfigWriter
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - File data is NULL [eFrameworkunifiedStatusNullPointer]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [eFrameworkunifiedStatusInvldParam]
  ///      - Can not find search key(f_c_key) designated by parameter [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par
  ///      Please pay attention to the following points when you use this API.\n
  ///      - Valid condition of search key string: there is no separator and line feed in config tag
  /// \~english @par Type
  ///      Sync Only
  /// \~english @par Detail
  ///      Set the int type value(32bit) associated with key to data stored in memory.
  ///      If there are multiple keys, set the values associated with all keys.
  /// \~english @see CNSConfigReader::GetInt
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Set(const std::string &f_c_key, SI_32 f_si_value);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Set
  /// \~english @par Brief
  ///     This function is used to set the string value corresponding to key.
  ///     If multiple identical keys are found it sets the value for all the keys.
  /// \~english @param [in] f_c_key
  ///     const std::string& - key
  /// \~english @param [in] f_s_value
  ///     const std::string&  - string value to set for key
  /// \~english @retval eFrameworkunifiedStatusOK - success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - memory allocation failed
  /// \~english @retval eFrameworkunifiedStatusInvldParam - invalid parameter
  /// \~english @retval eFrameworkunifiedStatusFail - can not find key
  /// \~english @par Prerequisite
  ///      - Call the construct of CNSConfigWriter
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - File data is NULL [eFrameworkunifiedStatusNullPointer]
  ///      - Data of search key(f_c_key) designated by parameter is NULL [eFrameworkunifiedStatusInvldParam]
  ///      - Can not find search key(f_c_key) designated by parameter [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par
  ///      Please pay attention to the following points when you use this API.\n
  ///      - Valid condition of search key string: there is no separator and line feed in config tag
  /// \~english @par Type
  ///      Sync Only
  /// \~english @par Detail
  ///      Set the string type value associated with key to data stored in memory.
  ///      If there are multiple keys, set the values associated with all keys.
  /// \~english @see CNSConfigReader::GetString
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Set(const std::string &f_c_key, const std::string &f_s_value);

 protected:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CNSConfigWriter
  /// Constructor of CNSConfigWriter class
  ///
  /// \param  [in] f_c_filepath
  ///     const std::string& - Full path of the configuration file
  /// \param  [in] f_b_noload
  ///     BOOL - load writer with no data
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNSConfigWriter(const std::string &f_c_filepath, BOOL f_b_noload);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetDataPtr
  /// This function is used to set the data pointer in writer class with data pointer
  /// created in config reader class.
  /// This is needed to avoid recreation of same data structure object in both reader and writer
  /// when we create object of NSConfigParser
  ///
  /// \param  [in] f_p_data
  ///     PVOID - Pointer to data structure
  ///
  /// \return VOID
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetDataPtr(PVOID f_p_data);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Create
  /// This function is used to create config object
  ///
  /// \param  [in] f_c_filepath
  ///     const std::string& - Name of the configuration file
  ///
  /// \return EFrameworkunifiedStatus - success or failure
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Create(const std::string &f_c_filepath);

 private:
  // config writer pointer
  IConfigWriter *m_pWriter;
  static HANDLE m_hNSXMLLibHandle;  // NOLINT (readability/nolint)
};


/**
 * @class CNSConfigParser
 * @brief \~english File both Reade and Write Service Class
 * @par   \~english Brief Introduction
 * \~english This class provides API's to application for both reading and writing into configuration files.
 *
 */
class CNSConfigParser: public CNSConfigReader, public CNSConfigWriter {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSConfigParser
  /// \~english @par Brief
  ///       Constructor of CNSConfigParser class
  /// \~english @retval none
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       It is called when generating the instance of CNSConfigParser class.
  /// \~english @see CNSConfigParser::~CNSConfigParser
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNSConfigParser();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSConfigParser
  /// \~english @par Brief
  ///       Constructor of CNSConfigParser class
  ///       This is depricated. Use parameterless constructor instead to create object and
  ///       Parse(filepath) to parse the file using parser object.
  /// \~english @param [in] f_c_filepath
  ///     const std::string& - Full path of the configuration file
  /// \~english @retval none
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       It doesn't need to call Parse() function to execute parse process of configuration File,
  ///       when using the constructor with file path. However, because we don't execute error process,
  ///       if it needs to reply error about parse process, we must to use the constructor without parameter
  ///       and call Parse() function.
  /// \~english @see CNSConfigParser::~CNSConfigParser
  ////////////////////////////////////////////////////////////////////////////////////////////////
  explicit CNSConfigParser(const std::string &f_c_filepath);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~CNSConfigParser
  /// \~english @par Brief
  ///     Destructor of CNSConfigParser class
  /// \~english @retval none
  /// \~english @par Prerequisite
  ///     - Call the construct of CNSConfigParser
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Release the used memory and the parsed result of Configuration File.
  /// \~english @see CNSConfigParser::CNSConfigParser
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CNSConfigParser();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Parse
  /// \~english @par Brief
  ///     This function is used to create config reader object and config writer object
  ///     which then parses the configuration file.
  /// \~english @param [in] f_c_filepath
  ///     const std::string& - Name of the configuration file
  /// \~english @retval eFrameworkunifiedStatusOK on success
  /// \~english @retval eFrameworkunifiedStatusNullPointer - if memory allocation failed or document not proper
  /// \~english @retval eFrameworkunifiedStatusFail - invalid file type
  /// \~english @retval eFrameworkunifiedStatusInvldParam invalid parameter
  /// \~english @retval eFrameworkunifiedStatusErrOther file data error
  /// \~english @par Prerequisite
  ///     - Call the construct of CNSConfigParser
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///      - Memory allocation failed [eFrameworkunifiedStatusNullPointer]
  ///      - File(f_c_filepath) designated by parameter is not proper [eFrameworkunifiedStatusNullPointer]
  ///      - Data of file(f_c_filepath) designated by parameter is NULL [eFrameworkunifiedStatusNullPointer]
  ///      - Expansion name of file(f_c_filepath) designated by parameter is not cfg [eFrameworkunifiedStatusFail]
  ///      - Failed to open file(f_c_filepath) designated by parameter [eFrameworkunifiedStatusFail]
  ///      - Path of file designated by parameter is NULL [eFrameworkunifiedStatusInvldParam]
  ///      - Data of file designated by parameter has error [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Parse designated configuration file. If the data format is correct, get memory to store the parsed result,
  ///       and store the parsed contents in it.
  /// \~english @see CNSConfigReader::Parse, CNSConfigWriter::Parse
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Parse(const std::string &f_c_filepath);
};

#endif  // NS_NATIVESERVICES_INC_NATIVE_NS_CONFIG_PARSER_IF_H_  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
