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
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NS_NPPService
/// .
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_NPPService
/// \brief    This file contains declaration of class CFSDirectory.
///
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_FS_DIRECTORY_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_FS_DIRECTORY_H_
#include <native_service/frameworkunified_types.h>
#include <string>



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
  /// \param [in] f_cdirpath
  ///         std::string - Path of the directory to delete.
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////
  static BOOL DoesDirecotryExist(const std::string &f_cdirpath);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// This method is used to create a directory.
  ///
  /// \param [in] f_cdirpath
  ///         std::string - Path of the directory to delete.
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////
  static EFrameworkunifiedStatus CreateDirectory(const std::string &f_cdirpath);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// This method is used to verify directory.
  ///
  /// \param [in] f_cpath
  ///         std::string - Path of the directory to delete.
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////
  static BOOL IsDirectory(const std::string &f_cpath);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// This method is used to delete directory with delay of f_uidelay(msec)
  /// between consecutive deletes.
  ///
  /// \param [in] f_cpath
  ///         std::string - Path of the directory to delete.
  /// \param [in] f_uidelay
  ///         UI_32 - delay in millisec before recursive call.
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ////////////////////////////////////////////////////////////////////////////////////////////
  static BOOL RemoveDirectory(std::string f_cpath, const UI_32 f_uidelay = 0);
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_FS_DIRECTORY_H_
