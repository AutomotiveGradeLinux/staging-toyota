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
/// \brief    The file contains declaration of CBinaryAccesser class.
///       This class stores data in a file in binary format.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_BINARY_ACCESSER_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_BINARY_ACCESSER_H_

#include <native_service/frameworkunified_types.h>
#include <string>
#include <vector>
#include "ns_npp_persistent_accesser.h"

class CPersistentAccesser;
class CPersistentData;
class CNotificationsToPersist;
class CPersistDataHeader;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// vector of all available persistent notification and its data.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::vector<CNotificationsToPersist *> Persistent_Notification_List_Type;

typedef Persistent_Notification_List_Type::iterator Persistent_Notification_List_Iterator;

/**
 *  This class stores data in a file in binary format.
 *
 */
class CBinaryAccesser: public CPersistentAccesser {
 public:
  typedef enum FileOpentype {
    EREAD = 0,
    EWRITE
  } FILE_OPEN_TYPE;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CBinaryAccesser
  /// Constructor of CBinaryAccesser class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CBinaryAccesser();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CBinaryAccesser
  /// Destructor of CBinaryAccesser class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CBinaryAccesser();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// PersistData
  /// Persist data in persistent memory in a file in binary format.
  ///
  /// \param [IN] f_cmemfilepath
  ///      std::string - Memory file path for saving notification.
  ///
  /// \param [IN] f_vdata
  ///      Persistent_Notification_List_Type* - Data which has to be persisted.
  ///
  /// \param [IN] f_epersistenttype
  ///      EFrameworkunifiedNotificationType - Type of Notification.
  ///
  /// \param [IN] f_epersistcategory
  ///      EFrameworkunifiedPersistCategory - persist category
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus PersistData(std::string f_cmemfilepath,
                         Persistent_Notification_List_Type *f_vdata,
                         EFrameworkunifiedNotificationType f_epersistenttype,
                         EFrameworkunifiedPersistCategory f_epersistcategory = eFrameworkunifiedUserData);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// RetrieveData
  /// Retrieve data from from a file from persistent memory and store in a vector.
  ///
  /// \param [IN] f_cmemfilepath
  ///      std::string - Memory file path from where to retrieve notification data.
  ///
  /// \param [OUT] f_vdata
  ///      Persistent_Notification_List_Type*& - Retrieved notification data is filled in this vector.
  ///
  /// \param [IN] f_epersistcategory
  ///      EFrameworkunifiedPersistCategory - persist category
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus RetrieveData(std::string f_cmemfilepath,
                          Persistent_Notification_List_Type *&f_vdata, // NOLINT (runtime/references)
                          EFrameworkunifiedPersistCategory f_epersistcategory = eFrameworkunifiedUserData);

 private:
  UI_32 m_uiCurStrOffset;
  UI_32 m_uiCurStrSize;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// OpenFileForReading
  /// Open file in read mode.
  ///
  /// \param [OUT] f_Fd
  ///      int * - File descriptor to read from a file
  ///
  /// \param [IN] f_cfilepath
  ///      std::string - File Path from the file is to be read
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OpenFileForReading(int *f_infd,
                                std::string f_cfilepath);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// OpenFileForWriting
  /// Open file in write mode.
  ///
  /// \param [OUT] f_Fd
  ///      int * - File descriptor to write to a file
  ///
  /// \param [IN] f_cfilepath
  ///      std::string - File Path for  the file that is to be written
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OpenFileForWriting(int *f_outfd,
                                std::string f_cfilepath);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// WriteHeaderAndData
  /// Write header and data in a file.
  ///
  /// \param [IN] f_pnotificationstopersist
  ///      CNotificationsToPersist - Object of class CNotificationsToPersist
  ///
  /// \param [IN] f_pdata
  ///      PVOID - notification data
  ///
  /// \param [IN] f_Fd
  ///      int - File descriptor to write the file.
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus WriteHeaderAndData(CNotificationsToPersist *f_pnotificationstopersist,
                                PVOID f_pdata,
                                int f_outfd);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ReadHeaderAndData
  /// Read header from the file.
  ///
  /// \param [IN] f_objcpersistdataheader
  ///      CPersistDataHeader& - Object of class CPersistDataHeader
  ///
  /// \param [OUT] f_pdata
  ///      CHAR*& -
  ///
  /// \param [IN] f_Fd
  ///      int - File descriptor to read the file.
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ReadHeaderAndData(CPersistDataHeader &f_objcpersistdataheader, // NOLINT (runtime/references)
                               CHAR *&f_pdata, int f_infd); // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// FillNotificationList
  /// This function retrieves the notification data from file and fill it in map.
  ///
  /// \param [IN] f_Fd
  ///      int - File descriptor to retrieve notification data.
  ///
  /// \param [OUT] f_vdata
  ///      Persistent_Notification_List_Type*& - Retrieved notification data is filled in this vector.
  ///
  /// \param [IN] f_epersistcategory
  ///      EFrameworkunifiedPersistCategory - persist category
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FillNotificationList(int f_infd,
                                  Persistent_Notification_List_Type *&f_vdata, // NOLINT (runtime/references)
                                  EFrameworkunifiedPersistCategory f_epersistcategory);

  EFrameworkunifiedStatus WriteFileHeaderAndDuplicate(int f_outfd, std::string f_cfilepath);
  EFrameworkunifiedStatus CheckFileHeader(int f_infd);
  EFrameworkunifiedStatus OpenBakFileForReading(int *f_infd, std::string f_cfilepath);
  UI_32 CalcCRC(PVOID f_pdata, UI_32 f_size);
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_BINARY_ACCESSER_H_
