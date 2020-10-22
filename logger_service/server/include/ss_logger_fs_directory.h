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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SS_LoggerService
/// \brief    This file contains declaration of class CFSDirectory.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_FS_DIRECTORY_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_FS_DIRECTORY_H_
#include <native_service/frameworkunified_types.h>
#include<string>

// Class to provide file system directory operations like create,delete etc.
class CFSDirectory {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CFSDirectory
  /// Constructor of CFSDirectory class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CFSDirectory();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFSDirectory
  /// Destructor of CFSDirectory class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CFSDirectory();

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// This method is used to check if the directory exists.
  ///
  /// \param [in] f_cDirPath
  ///         std::string - Path of the directory to delete.
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////
  static BOOL DoesDirectoryExist(std::string &f_cDirPath);  // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// This method is used to create a directory.
  ///
  /// \param [in] f_cDirPath
  ///         std::string - Path of the directory to delete.
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////
  static EFrameworkunifiedStatus CreateDirectory(std::string &f_cDirPath);  // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// This method is used to verify directory.
  ///
  /// \param [in] f_cPath
  ///         std::string - Path of the directory to delete.
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////
  static BOOL IsDirectory(std::string &f_cPath);  // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// This method is used to delete directory.
  ///
  /// \param [in] f_cPath
  ///         std::string - Path of the directory to delete.
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////
  static BOOL RemoveDirectory(std::string &f_cPath);  // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// This method is used to delete file in direcotry.
  ///
  /// \param [in] f_cPath
  ///         std::string - Path of the directory to delete.
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////
  static BOOL RemoveSubDirectory(std::string &f_cPath);  // NOLINT (runtime/references)
};

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_FS_DIRECTORY_H_
