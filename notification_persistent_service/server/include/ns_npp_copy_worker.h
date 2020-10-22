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
/// \brief    This file contain declaration of class CCopyWorker and CArchive.
///       Class CCopyWorker holds the implementation for worker thread.
///       Class CArchive holds the implementation for archive.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_COPY_WORKER_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_COPY_WORKER_H_

#define LIBTARFAIL   -1
#define LIBTARSUCCESS 0
#define TARMODE       0644
#define FILEERROR   -1

#include <pthread.h>
#include <libtar.h>
#include <stdio.h>
#include <zlib.h>

#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_utility_sys.hpp>
#include <native_service/frameworkunified_multithreading.h>
#include <native_service/ns_np_service.h>

#include <map>
#include <string>

#include "ns_npp_types.h"

typedef std::map<std::string, UI_32> TMTagCRC;  // map of tag and corresponding CRC
typedef std::map<std::string, TMTagCRC> TMServiceTagCRC;  // map of service and tag list
typedef std::map<int, gzFile> GZFiles;

////////////////////////////////////////////////////////////////////////////////////////////////
/// NSPCopyWorkerOnStart
/// Callback method on start of worker thread.
///
/// \param [IN] f_hthread
///      HANDLE - Thread Handle
///
/// \return EFrameworkunifiedStatus
//      EFrameworkunifiedStatus - success or failure status
///
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSPCopyWorkerOnStart(HANDLE f_hthread);

////////////////////////////////////////////////////////////////////////////////////////////////
/// NSPCopyWorkerOnStop
/// Callback method on stopping of worker thread.
///
/// \param [IN] f_hthread
///      HANDLE - Thread Handle
///
/// \return EFrameworkunifiedStatus
//      EFrameworkunifiedStatus - success or failure status
///
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSPCopyWorkerOnStop(HANDLE f_hthread);

////////////////////////////////////////////////////////////////////////////////////////////////
/// gzopen_frontend
/// Method to set archiving options.
///
/// \param [IN] f_pcpathname
///      PCHAR - Thread Handle
///
/// \param [IN] f_sioflags
///      SI_32 - Open file flags
///
/// \param [IN] f_simode
///      SI_32 - File Mode
///
/// \return EFrameworkunifiedStatus
//      EFrameworkunifiedStatus - success or failure status
///
////////////////////////////////////////////////////////////////////////////////////////////////
SI_32 OpenArchive(PCHAR f_pcpathname, SI_32 f_sioflags, SI_32 f_simode);

int CloseArchive(int fd);
ssize_t ReadArchive(int fd, void *buf, size_t count);
ssize_t WriteArchive(int fd, const void *buf, size_t count);

////////////////////////////////////////////////////////////////////////////////////////////
/// NPServiceOnDeleteOldDataCmd
/// This callback is used to delete the data which was requested to be deleted during
/// previous shutdown.
///
/// \param [in] f_happ
///         HANDLE - Handle to notificationpersistentservice_application Framework.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPServiceOnDeleteOldDataCmd(HANDLE f_happ);

////////////////////////////////////////////////////////////////////////////////////////////
/// NSPDataResetThreadCallback
/// Callback of data reset thread.
/// This callback delete all the data which was requested for reset during previous shutdown.
///
/// \param [in] f_parg
///         PVOID - Thread argument
///
/// \return PVOID
///         PVOID - NULL
////////////////////////////////////////////////////////////////////////////////////////////
PVOID NSPDataResetThreadCallback(PVOID f_parg);

class CCopyWorker {
 public:
  static TMServiceTagCRC g_mservicetagcrc;
  static CMutex g_objmtxservicetag;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CCopyWorker
  /// Constructor of class CCopyWorker
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CCopyWorker();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CCopyWorker
  /// Destructor of class CCopyWorker
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CCopyWorker();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// OnWrkCmdCopy
  /// Callback method for start copy command.
  ///
  /// \param [IN] f_hthread
  ///      HANDLE - Thread Handle
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OnWrkCmdCopy(HANDLE hthread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// OnWrkCmdResume
  /// Callback method for resuming worker thread.
  ///
  /// \param [IN] f_hthread
  ///      HANDLE - Thread Handle
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OnWrkCmdResume(HANDLE hthread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// OnWrkCmdStart
  /// Callback method for starting worker thread.
  ///
  /// \param [IN] f_hthread
  ///      HANDLE - Thread Handle
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OnWrkCmdStart(HANDLE hthread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// OnWrkCmdArchive
  /// Callback method to start archiving/unarchiving for file or folder
  ///
  /// \param [IN] f_hthread
  ///      HANDLE - Thread Handle
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OnWrkCmdArchive(HANDLE f_hthread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// OnWrkCmdStop
  /// Callback method for stopping worker thread.
  ///
  /// \param [IN] f_hthread
  ///      HANDLE - Thread Handle
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OnWrkCmdStop(HANDLE hthread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// OnCmdShutdownRequest
  /// Callback method on shutdown request.
  ///
  /// \param [IN] f_hthread
  ///      HANDLE - Thread Handle
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OnCmdShutdownRequest(HANDLE hthread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// Abort
  /// Method to abort worker thread tasks.
  ///
  ///
  /// \return VOID
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  static VOID Abort();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetCopyStatusToFailure
  /// Method to set the copy status to failure.
  ///
  /// \param [IN] f_tcopystatusresponse
  ///      NSP_CopyStatusResponse - Response status struct
  ///
  /// \param [IN] f_eworkerfailcmd
  ///      ENPS_CopyWorkerFailures - Worker failure Commands
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetCopyStatusToFailure(NSP_CopyStatusResponse &f_tcopystatusresponse, // NOLINT (runtime/references)
                                    const ENPS_CopyWorkerFailures f_eworkerfailcmd);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// InAbortState
  /// Method to check if the thread is in abort state
  ///
  /// \param [IN] f_hthread
  ///      HANDLE - Thread Handle
  ///
  /// \return BOOL
  //      BOOL - true if in abort state or false.
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL InAbortState() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SendAck
  /// Method to send ack to the parent thread
  ///
  /// \param [IN] f_hthread
  ///      HANDLE - Thread Handle
  ///
  /// \param [IN] f_eworkercmd
  ///      ENSP_CopyWorkerProtocol - Worker protocol command
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SendAck(HANDLE hthread, const ENSP_CopyWorkerProtocol &f_eworkercmd);

 private:
  static pthread_mutex_t m_sAbortMutex;  // NOLINT (readability/naming) , Mutex for abort
  static BOOL m_sbAbortCopy;  // NOLINT (readability/naming) , Check for abort state

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CopyFile
  /// Copy the content of source file f_csrcpath to destination file f_cdestpath.
  /// Creates the destination file path if does not exists.
  ///
  /// \param [IN] f_csrcpath
  ///        PCSTR - source file path
  ///
  /// \param [IN] f_cdestpath
  ///        PCSTR - dest file path
  ///
  /// \param [OUT] f_uiwritesize
  ///        UI_32 - number of bytes written to the destination file
  ///
  /// \param [IN] f_btmpfile
  ///        BOOL - TRUE:RELEASE-temp file use /FALSE:LOAD-temp file not use
  ///
  /// \return EFrameworkunifiedStatus
  ///          EFrameworkunifiedStatus - eFrameworkunifiedStatusOK on success else eFrameworkunifiedStatusFail
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CopyFile(PCSTR f_csrcpath, PCSTR f_cdestpath, UI_32 &f_uiwritesize, BOOL btmpfile = FALSE); // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetCopyBufSize
  /// Get the buffer size appropriate for file copy
  ///
  /// \param [IN] f_si32openfd
  ///        SI_32 - File descriptor of a file opened with open() api
  ///
  /// \param [OUT] f_ui32filesize
  ///        UI_32 - Get the size of file
  ///
  /// \return SI_32
  ///          SI_32 - Returns the buffer size appropriate for file copy
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  SI_32 GetCopyBufSize(SI_32 f_si32openfd, UI_32 &f_ui32filesize); // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// VerifyWithStoredCRC
  /// Verify CRC stored internally for the tag matches with the CRC of a file (file passed as a argument)
  ///
  /// \param [IN] f_crequesterappname
  ///        PCSTR - Name of the requester. This is to find the corresponding stored CRC
  ///
  /// \param [IN] f_cpersistenttag
  ///        PCSTR - Tag. This is to find the corresponding stored CRC
  ///
  /// \param [IN] f_cFile
  ///        PCSTR - Complete file path on temporary storage.
  ///        Comparison of CRC needs to be done with this temporary file.
  ///
  /// \return BOOL
  ///          BOOL - Returns TRUE if f_cFile's CRC matches with CRC stored internally for the corresponding tag
  ///         else FALSE
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL VerifyWithStoredCRC(PCSTR f_crequesterappname, PCSTR f_cpersistenttag, PCSTR f_cFile, UI_32 &f_ui32crc); // NOLINT (runtime/references)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// UpdateTagCRC
  /// Update the passed CRC in the internal map for the corresponding requester and tag.
  ///
  /// \param [IN] f_crequesterappname
  ///        PCSTR - Name of the requester.
  ///
  /// \param [IN] f_cpersistenttag
  ///        PCSTR - File Tag.
  ///
  /// \param [IN] f_ui32crc
  ///        UI_32 - New CRC.
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID UpdateTagCRC(PCSTR f_crequesterappname, PCSTR f_cpersistenttag, UI_32 f_ui32crc);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// TryFileCopyWithCRCCheck
  /// Try to copy file from source to destination until src and dest CRC matches or max f_ui32checkcount
  /// number of times
  ///
  /// \param [IN] f_csource
  ///        PCSTR - Source file
  ///
  /// \param [IN] f_cdest
  ///        PCSTR - Destination file
  ///
  /// \param [IN] f_ui32checkcount
  ///        UI_32 - Maximum number of times to perform file copy
  ///
  /// \return EFrameworkunifiedStatus
  ///          EFrameworkunifiedStatus - eFrameworkunifiedStatusOK - If file copy done successfully and CRC matches.
  ///             eFrameworkunifiedStatusFail - otherwise
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus TryFileCopyWithCRCCheck(PCSTR f_csource, PCSTR f_cdest, UI_32 f_ui32checkcount);

  class CArchive {
   public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// CArchive
    /// Constructor of CArchive class
    ///
    /// \param
    ///
    /// \return
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    CArchive();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// CArchive
    /// Destructor of CArchive class
    ///
    /// \param
    ///
    /// \return
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~CArchive();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Archive
    ///  Method for performing folder archive or extraction operation.
    ///
    /// \param [IN] f_tarchiveinfocmd
    ///      NSP_CopyInfoCmd - Archive Command Info Structure
    ///
    /// \param [IN] f_tarchivestatusresponse
    ///      NSP_CopyStatusResponse - Archive Command Response Structure
    ///
    /// \return EFrameworkunifiedStatus
    //      EFrameworkunifiedStatus - success or failure status
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    EFrameworkunifiedStatus Archive(NSP_CopyInfoCmd &f_tarchiveinfocmd, NSP_CopyStatusResponse &f_tarchivestatusresponse); // NOLINT (runtime/references)

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///  FileOperationUsingLibz
    ///  Method to determine whether to compress or decompres file using libz.
    ///
    /// \param [IN] f_toperainfocmd
    ///      NSP_CopyInfoCmd - File  Operation Command Info Structure
    ///
    /// \param [IN] f_toperarespstatus
    ///      NSP_CopyStatusResponse - File Operation Command Response Structure
    ///
    /// \return EFrameworkunifiedStatus
    //      EFrameworkunifiedStatus - success or failure status
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    EFrameworkunifiedStatus FileOperationUsingLibz(NSP_CopyInfoCmd &f_toperainfocmd, NSP_CopyStatusResponse &f_toperarespstatus); // NOLINT (runtime/references)

   private:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// CreateTar
    /// Method to Create tar archive.
    ///
    /// \param [IN] f_csourepath
    ///      std::string - Source path of the folder to be archived
    ///
    /// \param [IN] f_cdestpath
    ///      std::string - Destination path for the archived folder.
    ///
    /// \param [IN] f_tarchivestatusresponse
    ///      NSP_CopyStatusResponse - Archive Command Response Structure
    ///
    /// \return EFrameworkunifiedStatus
    //      EFrameworkunifiedStatus - success or failure status
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    EFrameworkunifiedStatus CreateTar(const std::string &f_csourepath, const std::string &f_cdestpath,
                         NSP_CopyStatusResponse &f_tarchivestatusresponse); // NOLINT (runtime/references)

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// ExtractTar
    /// Method to extract tar archive.
    ///
    /// \param [IN] f_csourepath
    ///      std::string - Source path of the folder to be extracted
    ///
    /// \param [IN] f_csourcedest
    ///      std::string - Destination path for the extracted folder.
    ///
    /// \param [IN] f_tarchivestatusresponse
    ///      NSP_CopyStatusResponse - Archive Command Response Structure
    ///
    /// \return EFrameworkunifiedStatus
    //      EFrameworkunifiedStatus - success or failure status
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    EFrameworkunifiedStatus ExtractTar(const std::string &f_csourepath, const std::string &f_csourcedest,
                          NSP_CopyStatusResponse &f_tarchivestatusresponse); // NOLINT (runtime/references)

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    /// Method to Create archive
    ///
    /// \param [IN] f_pctarfiledestpath
    ///      CHAR  - Destination path for the archived folder.
    ///
    /// \param [IN] f_pcrootdirpath
    ///      CHAR  - Source path of the folder to be archived
    ///
    /// \param [IN] f_plibtarlist
    ///      BOOL - TRUE if user file else FALSE
    ///
    /// \return SI_32
    //
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    EFrameworkunifiedStatus Create(std::string &f_pctarfiledestpath, std::string &f_pcrootdirpath, libtar_list_t *f_plibtarlist); // NOLINT (runtime/references)

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Extract
    ///  Nethod to extract archive.
    ///
    /// \param [IN] f_pcrootdirpath
    ///      CHAR - Source path of the folder to be extracted
    ///
    /// \param [IN] f_pctarfiledestpath
    ///      CHAR - Destination path for extraction.
    ///
    /// \return SI_32
    //
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    EFrameworkunifiedStatus Extract(std::string &f_pcrootdirpath, std::string &f_pctarfiledestpath); // NOLINT (runtime/references)

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// CompressUsingZlib
    /// Method to compress file using libz
    ///
    /// \param [IN] f_csourepath
    ///      std::string - Source path of the file to be compressed
    ///
    /// \param [IN] f_cdestpath
    ///      std::string - Destination path for the compressed file.
    ///
    /// \param [IN] f_iziplevel
    ///      Level of compression i.e.
    ///      More compression results in more time taken for compression operation and vice versa.
    ///
    /// \return EFrameworkunifiedStatus
    //      EFrameworkunifiedStatus - success or failure status
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    EFrameworkunifiedStatus CompressUsingZlib(const std::string &f_csourepath, const  std::string &f_cdestpath, SI_32 f_iziplevel);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// DeCompressUsingZlib
    /// Method to decompress file using libz.
    ///
    /// \param [IN] f_csourepath
    ///      std::string - Source path of the file to be decompressed.
    ///
    /// \param [IN] f_csourcedest
    ///      std::string - Destination path for the decompressed file.
    ///
    /// \return EFrameworkunifiedStatus
    //      EFrameworkunifiedStatus - success or failure status
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    EFrameworkunifiedStatus DeCompressUsingZlib(const std::string &f_csourepath, const std::string &f_csourcedest);
  };
};
#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_COPY_WORKER_H_
