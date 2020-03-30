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
/// \brief    This file contains declaration of class IConfigReader.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @file ns_reader.h
 * @brief \~english This file contains declaration of class IConfigReader.
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */

#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NSCONFIGPARSER_INC_CONFIG_READER_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NSCONFIGPARSER_INC_CONFIG_READER_H__  // NOLINT(build/header_guard)

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>  // NOLINT(build/header_guard)
#include <string>

/*
 * IConfigReader is an abstract class which is inherited by different type configuration file
 * reader class.
 */
/**
 * @class IConfigReader
 * \~english @brief IConfigReader
 * \~english @par   Brief Introduction
 *        Class to provide class of parse config file.
 */
class IConfigReader {
 public:
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IConfigReader
  /// \~english @par Summary
  ///       Constructor of IConfigReader class.
  /// \~english @param none
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       Constructor of IConfigReader class.
  /// \~english @see  ~IConfigReader
  ////////////////////////////////////////////////////////////////////////////////////
  IConfigReader();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IConfigReader
  /// \~english @par Summary
  ///       Destructor of IConfigReader class.
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       To delete a IConfigReader class.
  /// \~english @see  IConfigReader
  ////////////////////////////////////////////////////////////////////////////////////
  virtual ~IConfigReader();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IConfigReader
  /// \~english @par Summary
  ///       This function is used to parse the file.
  /// \~english @param [IN] f_c_filepath
  ///        const std::string - path of file to parse.
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       This function is used to parse the file.\n
  ///       This is abstract function.
  /// \~english @see  IConfigReader
  ////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus ParseFile(const std::string &f_c_filepath) = 0;

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IConfigReader
  /// \~english @par Summary
  ///       This function is used to get the value associated with the key.
  /// \~english @param [IN] f_c_key
  ///        const std::string - key to search.
  /// \~english @retval l_cValue value for key
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       This function is used to get the value associated with the key.\n
  ///       This is abstract function.
  /// \~english @see  IConfigReader
  ////////////////////////////////////////////////////////////////////////////////////
  virtual std::string GetValue(const std::string &f_c_key) = 0;

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IConfigReader
  /// \~english @par Summary
  ///       This function is used to get the value associated with the key.
  /// \~english @param [IN] f_c_key
  ///        const std::string - key to search.
  /// \~english @param [OUT] f_c_value
  ///        const std::string - value for key.
  /// \~english @retval EFrameworkunifiedStatus error  error if key not found else eFrameworkunifiedStatusOK
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       This function is used to get the value associated with the key.\n
  ///       This is abstract function.
  /// \~english @see  IConfigReader
  ////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus GetValue(const std::string &f_c_key, std::string &f_c_value) = 0;  // NOLINT (readability/nolint)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IConfigReader
  /// \~english @par Summary
  ///       This function is used to get the data pointer.
  /// \~english @param none.
  /// \~english @retval PVOID - pointer of data structure
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       This function is used to get the data pointer. This pointer is then set in config writer.\n
  ///       This is needed to avoid recreation of same data structure object in both reader and writer.\n
  ///       when we create object of NSConfigParser.
  ///       This is abstract function.
  /// \~english @see  IConfigReader
  ////////////////////////////////////////////////////////////////////////////////////
  virtual PVOID GetDataPtr() = 0;
};

#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_NSCONFIGPARSER_INC_CONFIG_READER_H__ */  // NOLINT  (build/header_guard)
/** @}*/  // end of framework_unified
/** @}*/  // end of native_service
/** @}*/  // end of BaseSystem
