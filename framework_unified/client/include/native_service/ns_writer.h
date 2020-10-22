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
/// \brief    This file contains declaration of class IConfigWriter.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @file ns_writer.h
 * @brief \~english This file contains declaration of class IConfigWriter.
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

#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NSCONFIGPARSER_INC_CONFIG_WRITER_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NSCONFIGPARSER_INC_CONFIG_WRITER_H__  // NOLINT(build/header_guard)

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>  // NOLINT(build/header_guard)
#include <string>


/**
 * @class IConfigWriter
 * \~english @brief Config writer abstract class.
 * \~english @par   Brief Introduction
 *        IConfigWriter is an abstract class which is inherited by\n
 *        different type configuration file writer class.
 *
 */
class IConfigWriter {
 public:
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IConfigWriter
  /// \~english @par Brief
  ///        Constructor of IConfigWriter class
  /// \~english @param None
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           - None
  /// \~english @par Conditions of processing failure
  ///           - None
  /// \~english @par Detail
  ///           - To get a instance of this class.
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - Sync only
  /// \~english @see ~IConfigWriter
  ////////////////////////////////////////////////////////////////////////////////////
  IConfigWriter();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~IConfigWriter
  /// \~english @par Brief
  ///        Destructor of IConfigWriter class
  /// \~english @param None
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           - Calls constructor to successfully create object.
  /// \~english @par Change of internal status
  ///           - None
  /// \~english @par Conditions of processing failure
  ///           - None
  /// \~english @par Detail
  ///           - Destruct the object of this class.
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - Sync only
  /// \~english @see IConfigWriter
  ////////////////////////////////////////////////////////////////////////////////////
  virtual ~IConfigWriter();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IConfigWriter
  /// \~english @par Brief
  ///        This function is used to parse the file.
  /// \~english @param [in] f_c_filepath
  ///           std::string - path of file to parse
  /// \~english @retval EFrameworkunifiedStatus - success or failure
  /// \~english @par Preconditons
  ///           - Calls constructor to successfully create object.
  /// \~english @par Change of internal status
  ///           - None
  /// \~english @par Conditions of processing failure
  ///           - None
  /// \~english @par Detail
  ///           This function is used to parse the file.
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - Sync only
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus ParseFile(const std::string &f_c_filepath) = 0;

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IConfigWriter
  /// \~english @par Brief
  ///        This function is used to set the value for the key
  /// \~english @param [in] f_c_key
  ///           std::string - key to search
  /// \~english @param [in] f_c_value
  ///           std::string - value to set
  /// \~english @retval EFrameworkunifiedStatus - error if key not found else eFrameworkunifiedStatusOK
  /// \~english @par Preconditons
  ///           - Calls constructor to successfully create object.
  /// \~english @par Change of internal status
  ///           - None
  /// \~english @par Conditions of processing failure
  ///           - None
  /// \~english @par Detail
  ///           This function is used to set the value for the key
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - Sync only
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus SetValue(const std::string &f_c_key, std::string f_c_value) = 0;

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IConfigWriter
  /// \~english @par Brief
  ///        This function is used to save the changed value permanently to the config source
  /// \~english @param None
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           - None
  /// \~english @par Conditions of processing failure
  ///           - None
  /// \~english @par Detail
  ///           This function is used to save the changed value permanently to the config source
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - Sync only
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus SaveData() = 0;

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IConfigWriter
  /// \~english @par Brief
  ///        This function is used to set the config file path
  /// \~english @param [in] f_cpath
  ///           std::string - Path of file
  /// \~english @retval EFrameworkunifiedStatus - success or failure
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           - None
  /// \~english @par Conditions of processing failure
  ///           - None
  /// \~english @par Detail
  ///           This function is used to set the config file path
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - Sync only
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus SetPath(const std::string &f_cpath) = 0;

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IConfigWriter
  /// \~english @par Brief
  ///              This function is used to set the data pointer in config writer class with data pointer\n
  ///              created in config reader class.\n
  ///              This is needed to avoid recreation of same data structure object in both reader and writer\n
  ///              when we create object of NSConfigParser.\n
  /// \~english @param [in] f_pdata
  ///           PVOID - Pointer to data structure
  /// \~english @retval None
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           - None
  /// \~english @par Conditions of processing failure
  ///           - None
  /// \~english @par Detail
  ///              This function is used to set the data pointer in config writer class with data pointer\n
  ///              created in config reader class.\n
  ///              This is needed to avoid recreation of same data structure object in both reader and writer\n
  ///              when we create object of NSConfigParser.\n
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - Sync only
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////////
  virtual VOID SetDataPtr(PVOID f_pdata) = 0;
};

#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_NSCONFIGPARSER_INC_CONFIG_WRITER_H__ */  // NOLINT  (build/header_guard)
/** @}*/  // end of framework_unified
/** @}*/  // end of native_service
/** @}*/  // end of BaseSystem
