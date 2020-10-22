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
/// \brief
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSIST_FOLDER_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSIST_FOLDER_H_

#include <native_service/frameworkunified_framework_if.h>
#include <string>

#include "ns_npp_persistence.h"


// class CPersistence;

class CFolderPersistence : public CPersistence {
 private:
  TSourceRegistryList m_mPersistFolderRegistry;  // map to hold the folder registry entries

 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CFolderPersistence
  /// Constructor of CFolderPersistence class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CFolderPersistence();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFolderPersistence
  /// Destructor of CFolderPersistence class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CFolderPersistence();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Release
  /// Entry for the folder is stored in map for persistence.
  /// If f_bPersist is TRUE folder will be persisted immediately else will be persisted on shutdown
  ///
  /// \param [IN] f_cappname
  ///      string - Name of the application requesting for persistence
  ///
  /// \param [IN] f_ctag
  ///      string - Folder will be persist against this tag.
  ///
  /// \param [IN] f_cmempath
  ///      string - Folder which needs to persist.
  ///
  /// \param [IN] enotificationpersistentservicereleasetype
  ///      EFrameworkunifiedReleaseType - eFrameworkunifiedNotOnRelease = 0  :not on release
  ///                        eFrameworkunifiedPersistOnShutdown :persist on shutdown
  ///                        eFrameworkunifiedPersistInstantly  :persist instantly
  ///
  /// \param [IN] f_cusername
  ///      std::string - If tag is registered as user, then f_cusername holds the name of user,
  ///            else an empty string
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Release(std::string f_crequesterappname,
                     std::string f_ctag,
                     std::string f_cmempath,
                     EFrameworkunifiedReleaseType enotificationpersistentservicereleasetype,
                     std::string f_cusername);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Load
  ///  Load folder from persistent memory to the specified location.
  ///
  /// \param [IN] f_cappname
  ///      string - Name of the application requesting for persistence
  ///
  /// \param [IN] f_ctag
  ///      string - Folder corresponding to this tag will be retrieved.
  ///
  /// \param [IN] f_cretrievepath
  ///      string - Filepath for retrieved file.
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Load(std::string f_cappname,
                  std::string f_ctag,
                  std::string f_cretrievepath,
                  std::string f_cusername);
};
#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSIST_FOLDER_H_
