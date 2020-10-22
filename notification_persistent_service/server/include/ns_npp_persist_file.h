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
/// \ingroup  tag_NPPService
/// \brief    The file contains declaration of CFilePersistence class.
///           This class is responsible for persisting and retrieving of files.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSIST_FILE_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSIST_FILE_H_

#include <native_service/ns_np_service.h>
#include <string>
#include <map>
#include "ns_npp_persistence.h"

// Forward declaration of classes.
class CPersistence;
class CRegistryEntry;

/**
 *  This class is responsible for persisting and retrieving of files..
 */
class CFilePersistence : public CPersistence {
 private:
  TSourceRegistryList m_mPersistFileRegistry;

 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CFilePersistence
  /// Constructor of CFilePersistence class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CFilePersistence();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CFilePersistence
  /// Destructor of CFilePersistence class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CFilePersistence();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Release
  /// Request for release file to persistent storage. File persist will be done at shutdown.
  ///
  /// \param [IN] f_cappname
  ///      string - Name of the application requesting for persistence
  ///
  /// \param [IN] f_ctag
  ///      string - File will be persist against this tag.
  ///
  /// \param [IN] f_cmempath
  ///      string - File which needs to persist.
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
  EFrameworkunifiedStatus Release(std::string f_cappname, std::string f_ctag, std::string f_cmempath,
                     EFrameworkunifiedReleaseType enotificationpersistentservicereleasetype, std::string f_cusername);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Load
  ///  Load file from persistent memory to the specified location.
  ///
  /// \param [IN] f_cappname
  ///      string - Name of the application requesting for persistence
  ///
  /// \param [IN] f_ctag
  ///      string - File corresponding to this tag will be retrieved.
  ///
  /// \param [IN] f_cretrievepath
  ///      string - Filepath for retrieved file.
  ///
  /// \param [IN] f_cusername
  ///      std::string - If tag is registered as user, then f_cusername holds the name of user,
  ///            else an empty string
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Load(std::string f_cappname, std::string f_ctag, std::string f_cretrievepath,
                  std::string f_cusername);
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_PERSIST_FILE_H_
