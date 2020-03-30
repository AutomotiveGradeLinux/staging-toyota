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
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NS_NPPService
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_NPPService
/// \brief    This file implements the CCopyWorker class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_util_crc.h>
#include <native_service/ns_logger_if.h>

// standard headers
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>

#include <string>
#include <utility>

// ns headers
#include "ns_npp_copy_worker.h"
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_fs_directory.h"
#include "ns_npp_persistence.h"
#include "ns_npp_threads.h"
#include "ns_npp.h"

#ifdef AGL_STUB
#include <other_service/strlcpy.h>
/*#include "PosixBasedOS001legacy_types.h"*/
#include <unistd.h>
#include <sys/syscall.h>
#endif

#define NS_NPP_WRITESIZE1   4096
#define NS_NPP_WRITESIZE2   32768
#define NS_NPP_REMOVE_DIR_DELAY 100  // in ms
#define NS_NPP_FILEWRITE_DELAY  50  // in ms

pthread_mutex_t g_mutworkerthread = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t CCopyWorker::m_sAbortMutex = PTHREAD_MUTEX_INITIALIZER;
BOOL CCopyWorker::m_sbAbortCopy = FALSE;

template <typename C, eFrameworkunifiedStatus(C::*M)(HANDLE)> EFrameworkunifiedStatus WorkerCallback(HANDLE hthread) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  C *l_pClass = static_cast<C *>(FrameworkunifiedGetThreadSpecificData(hthread));

  if (l_pClass) {  // LCOV_EXCL_BR_LINE 6: l_pClass can't be NULL
    l_estatus = (l_pClass->*M)(hthread);
  }

  return l_estatus;
}

// Initialize static members
TMServiceTagCRC CCopyWorker::CCopyWorker::g_mservicetagcrc;
CMutex CCopyWorker::CCopyWorker::g_objmtxservicetag;

static FrameworkunifiedProtocolCallbackHandler aServiceHandlers[] = {  // NOLINT (readability/naming)  // LCOV_EXCL_BR_LINE 11: unexpected branch
  { CP_WRK_CMD_COPY,            WorkerCallback<CCopyWorker, &CCopyWorker::OnWrkCmdCopy>   },
  { CP_WRK_CMD_RESUME,          WorkerCallback<CCopyWorker, &CCopyWorker::OnWrkCmdResume> },
  { AR_CMD_ARCHIVE,               WorkerCallback<CCopyWorker, &CCopyWorker::OnWrkCmdArchive>    },
  { CMD_WRK_SHUTDOWN_REQ,           WorkerCallback<CCopyWorker, &CCopyWorker::OnCmdShutdownRequest>},
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CCopyWorker
/// Constructor of CCopyWorker class
////////////////////////////////////////////////////////////////////////////////////////////////////
CCopyWorker::CCopyWorker() {
  pthread_mutex_init(&m_sAbortMutex, NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CCopyWorker
/// Destructor of CCopyWorker class
////////////////////////////////////////////////////////////////////////////////////////////////////
CCopyWorker::~CCopyWorker() {
  pthread_mutex_destroy(&m_sAbortMutex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Abort
/// Method to abort worker thread.
////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CCopyWorker::Abort() {   // LCOV_EXCL_START 8: not used
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  pthread_mutex_lock(&m_sAbortMutex);
  m_sbAbortCopy = TRUE;
  pthread_mutex_unlock(&m_sAbortMutex);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// InAbortState
/// Method to check for abort state.
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CCopyWorker::InAbortState() const {
  BOOL l_bAbortStatus = FALSE;
  pthread_mutex_lock(&m_sAbortMutex);
  l_bAbortStatus = m_sbAbortCopy;
  pthread_mutex_unlock(&m_sAbortMutex);

  return l_bAbortStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetCopyStatusToFailure
/// Method to set the copy status failure.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::SetCopyStatusToFailure(NSP_CopyStatusResponse &f_tcopystatusresponse,  // LCOV_EXCL_START 8: not used  // NOLINT[whitespace/line_length]
                                               const ENPS_CopyWorkerFailures f_eworkerfailcmd) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  f_tcopystatusresponse.m_bpersistencechk = FALSE;
  f_tcopystatusresponse.m_ecopyfailures = f_eworkerfailcmd;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusFail;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// OnWrkCmdCopy
/// Callback method on copy command.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::OnWrkCmdCopy(HANDLE hthread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  NSP_CopyStatusResponse l_tCopyStatus = {};
  NSP_CopyInfoCmd l_tCopyInfo = {};
  UI_32 l_uiFileSize = 0;
  UI_32 l_ui32FileCRC = 0;
  PSTR  l_cTmpStorageFile = NULL;

  if (NULL != hthread) {  // LCOV_EXCL_BR_LINE 6: hthread can't be NULL
    if (FrameworkunifiedGetMsgLength(hthread) == sizeof(l_tCopyInfo)) {  // LCOV_EXCL_BR_LINE 200: the size of msg must be sizeof(NSP_CopyInfoCmd)  // NOLINT[whitespace/line_length]
      if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(hthread, &l_tCopyInfo, sizeof(NSP_CopyInfoCmd), eSMRRelease))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        l_tCopyStatus.m_eloadtype = l_tCopyInfo.m_eloadtype;
#ifdef AGL_PosixBasedOS001LEGACY_USED
        strlcpy(l_tCopyStatus.m_cpersistenttag, l_tCopyInfo.m_cpersistenttag, sizeof(l_tCopyStatus.m_cpersistenttag));
        strlcpy(l_tCopyStatus.m_crequesterappname,
                l_tCopyInfo.m_crequesterappname,
                sizeof(l_tCopyStatus.m_crequesterappname));
#endif

        l_tCopyStatus.m_epersisttype = l_tCopyInfo.m_epersisttype;

        if (ENOTIFICATIONPERSISTENTSERVICECOMPRESSUSINGLIBZ == l_tCopyInfo.m_ecompressiontype) {  // LCOV_EXCL_BR_LINE 6: l_tCopyInfo.m_ecompressiontype can't be ENOTIFICATIONPERSISTENTSERVICECOMPRESSUSINGLIBZ  // NOLINT[whitespace/line_length]
          // LCOV_EXCL_START 6: l_tCopyInfo.m_ecompressiontype can't be ENOTIFICATIONPERSISTENTSERVICECOMPRESSUSINGLIBZ
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          CCopyWorker::CArchive *l_pCArchive =  new(std::nothrow) CCopyWorker::CArchive();

          if (NULL != l_pCArchive) {
            // Start the folder archive or extract operation
            if (eFrameworkunifiedStatusOK == (l_estatus = l_pCArchive->FileOperationUsingLibz(l_tCopyInfo, l_tCopyStatus))) {
              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Compress/Decompress Successful");
            } else {
              l_estatus = eFrameworkunifiedStatusFail;  // Set the archive status to failure
            }
            delete l_pCArchive;
            l_pCArchive = NULL;
          }
          // LCOV_EXCL_STOP
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Tag: %s SourceFile: %s DestFile: %s",
                 l_tCopyInfo.m_cpersistenttag != 0 ? l_tCopyInfo.m_cpersistenttag : NULL,
                 l_tCopyInfo.m_csourcepath != 0 ? l_tCopyInfo.m_csourcepath : NULL,
                 l_tCopyInfo.m_cdestinationpath != 0 ? l_tCopyInfo.m_cdestinationpath : NULL);

          if (LOADTYPE_RELEASE == l_tCopyStatus.m_eloadtype) {
            // if release, source file needs to check whether it's changed or not
            l_cTmpStorageFile = l_tCopyInfo.m_csourcepath;
          } else if (LOADTYPE_LOAD == l_tCopyStatus.m_eloadtype) {  // LCOV_EXCL_BR_LINE 6: m_eloadtype must be LOADTYPE_RELEASE or LOADTYPE_LOAD  // NOLINT[whitespace/line_length]
            // if load, dest file needs to check whether it's changed or not
            l_cTmpStorageFile = l_tCopyInfo.m_cdestinationpath;

            // check if file on temporary location exists, then no need to load from persistent memory
            if (0 == access(l_tCopyInfo.m_cdestinationpath, F_OK)) {  // LCOV_EXCL_BR_LINE 5: access's error case
              FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__, "File to load::%s for source::%s "
                     "is already present on path:: %s.",
                     l_tCopyInfo.m_cpersistenttag != 0 ? l_tCopyInfo.m_cpersistenttag : NULL,
                     l_tCopyStatus.m_crequesterappname != 0 ? l_tCopyStatus.m_crequesterappname : NULL,
                     l_tCopyInfo.m_cdestinationpath != 0 ? l_tCopyInfo.m_cdestinationpath : NULL);
              l_estatus = eFrameworkunifiedStatusFileLoadSuccess;
            }
          } else {
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            // let l_cTmpStorageFile be NULL.
          }

          if (eFrameworkunifiedStatusFileLoadSuccess == l_estatus) {
            l_estatus = eFrameworkunifiedStatusOK;
          } else {
            if (NULL != l_cTmpStorageFile) {  // LCOV_EXCL_BR_LINE 6: l_cTmpStorageFile can't be NULL
              if (!VerifyWithStoredCRC(l_tCopyStatus.m_crequesterappname,
                                       l_tCopyStatus.m_cpersistenttag,
                                       l_cTmpStorageFile,
                                       l_ui32FileCRC)) {  // l_ui32FileCRC: out param
                // Check if file release and CRC verification is enabled
                if ((LOADTYPE_RELEASE == l_tCopyStatus.m_eloadtype) && (CNSNPP::m_siCRCCheckCount > 0)) {  // LCOV_EXCL_BR_LINE 200: cannot test code  // NOLINT[whitespace/line_length]
                  // LCOV_EXCL_START 200: cannot test code
                  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                  l_estatus = TryFileCopyWithCRCCheck(l_tCopyInfo.m_csourcepath,
                                                      l_tCopyInfo.m_cdestinationpath,
                                                      CNSNPP::m_siCRCCheckCount);
                  // LCOV_EXCL_STOP
                } else {
                  if (LOADTYPE_RELEASE == l_tCopyStatus.m_eloadtype) {
                    l_estatus = CopyFile(l_tCopyInfo.m_csourcepath,
                                         l_tCopyInfo.m_cdestinationpath,
                                         l_uiFileSize,
                                         TRUE);
                  } else {
                    l_estatus = CopyFile(l_tCopyInfo.m_csourcepath,
                                         l_tCopyInfo.m_cdestinationpath,
                                         l_uiFileSize,
                                         FALSE);
                  }
                }
                if (eFrameworkunifiedStatusOK == l_estatus) {
                  FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO3, __FUNCTION__, "Bytes written:%d file=%s",
                         l_uiFileSize, l_tCopyInfo.m_cdestinationpath != 0 ? l_tCopyInfo.m_cdestinationpath : NULL);
                  if (0 == l_ui32FileCRC) {
                    if (eFrameworkunifiedStatusOK != CalculateCRC32(l_cTmpStorageFile, l_ui32FileCRC)) {  // LCOV_EXCL_BR_LINE 200: CalculateCRC32() return ok  // NOLINT[whitespace/line_length]
                      // LCOV_EXCL_START 200: CalculateCRC32() return ok
                      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Warn: getting crc. File:%s, crc:%d",
                             l_cTmpStorageFile, l_ui32FileCRC);
                      // LCOV_EXCL_STOP
                    }
                  }
                  UpdateTagCRC(l_tCopyStatus.m_crequesterappname, l_tCopyInfo.m_cpersistenttag, l_ui32FileCRC);
                } else {
                  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FileCopy Error, status : 0x%x", l_estatus);
                }
              } else {
                FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO2, __FUNCTION__,
                       "File %s not changed. No copy done. Load type:%d. Tag:%s for Requester:%s.",
                       l_cTmpStorageFile,
                       l_tCopyStatus.m_eloadtype,
                       l_tCopyStatus.m_cpersistenttag != 0 ? l_tCopyStatus.m_cpersistenttag : NULL,
                       l_tCopyStatus.m_crequesterappname != 0 ? l_tCopyStatus.m_crequesterappname : NULL);
              }
            } else {
              // LCOV_EXCL_START 6: l_cTmpStorageFile can't be NULL
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unknown load type:%d, for tag:%s",
                     l_tCopyStatus.m_eloadtype,
                     l_tCopyStatus.m_cpersistenttag != 0 ? l_tCopyStatus.m_cpersistenttag : NULL);
              // LCOV_EXCL_STOP
            }
          }
        }

        if (eFrameworkunifiedStatusOK != l_estatus) {
          l_tCopyStatus.m_bpersistencechk = FALSE;

          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "COPY_FAILED_ID sending parent message. Tag:%s for Requester:%s.",
                 l_tCopyStatus.m_cpersistenttag != 0 ? l_tCopyStatus.m_cpersistenttag : NULL,
                 l_tCopyStatus.m_crequesterappname != 0 ? l_tCopyStatus.m_crequesterappname : NULL);

          FrameworkunifiedSendParent(hthread, CP_WRK_NTFY, sizeof(NSP_CopyStatusResponse), &l_tCopyStatus);
        } else {
          l_tCopyStatus.m_bpersistencechk = TRUE;

          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "COPY_COMPLETE_ID sending parent message. Tag:%s for Requester:%s.",
                 l_tCopyStatus.m_cpersistenttag != 0 ? l_tCopyStatus.m_cpersistenttag : NULL,
                 l_tCopyStatus.m_crequesterappname != 0 ? l_tCopyStatus.m_crequesterappname : NULL);

          FrameworkunifiedSendParent(hthread, CP_WRK_NTFY, sizeof(NSP_CopyStatusResponse), &l_tCopyStatus);
        }
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error:FrameworkunifiedGetMsgDataOfSize returned:%d", l_estatus);  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      }
    } else {
      // LCOV_EXCL_START 200: the size of msg must be sizeof(NSP_CopyInfoCmd)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error:MsgLength: Expected:%ld, Received:%d.",
             static_cast<long int>(sizeof(l_tCopyInfo)), FrameworkunifiedGetMsgLength(hthread)); // NOLINT (runtime/int)
      // LCOV_EXCL_STOP
    }
    l_estatus = SendAck(hthread, CP_WRK_CMD_COPY);
  } else {
    // LCOV_EXCL_START 6: hthread can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Thread App Handle Null");
    l_estatus = eFrameworkunifiedStatusInvldHandle;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// GetCopyBufSize
/// Get the proper buffer size.
////////////////////////////////////////////////////////////////////////////////////////////////
SI_32 CCopyWorker::GetCopyBufSize(SI_32 f_si32openfd, UI_32 &f_ui32filesize) {
  UI_32 l_ui32BufSize = NS_NPP_WRITESIZE1;
  // to get the size of file
  struct stat l_tSrcFstat = {};

  if (-1 == fstat(f_si32openfd , &l_tSrcFstat)) {  // LCOV_EXCL_BR_LINE 5: fstat's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "fstat failure, errno:%d", errno);  // LCOV_EXCL_LINE 5: fstat's error case
  } else {
    f_ui32filesize = static_cast<UI_32>(l_tSrcFstat.st_size);  // set out param
    if (l_tSrcFstat.st_size < NS_NPP_WRITESIZE1) {
      l_ui32BufSize = NS_NPP_WRITESIZE1;
    } else {
      l_ui32BufSize = NS_NPP_WRITESIZE2;
    }
  }
  return l_ui32BufSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// CopyFile
/// Copy the content of source file f_csrcpath to destination file f_cdestpath
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::CopyFile(PCSTR f_csrcpath, PCSTR f_cdestpath, UI_32 &f_uiNS_NPP_WRITESIZE, BOOL btmpfile) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // declare variables
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  SI_32 l_iSourceFileFd  = -1;        // file descriptor of source file
  SI_32 l_iDestFileFd = -1;         // file descriptor of destination file
  PSTR l_cWriteBuffer = NULL;
  SI_32 l_si32BufSize = NS_NPP_WRITESIZE1;
  size_t l_iReadBuffLength = 0;        // size of data read from source file
  UI_32 l_ui32SrcFileSize = 0;
  std::string l_cTempDestPath;  // temporary file path
  if (btmpfile == FALSE) {
    l_cTempDestPath = f_cdestpath;
  } else {
    l_cTempDestPath = CPersistence::GetStoragePath();  // LCOV_EXCL_BR_LINE 11: unexpected branch
    if (l_cTempDestPath.length() > 0) {  // LCOV_EXCL_BR_LINE 6: l_cTempDestPath.length() is bigger than 0
      l_cTempDestPath.append(RELEASETEMP_DIR);  // LCOV_EXCL_BR_LINE 11: unexpected branch
      // create the destination folder path if does not exists
      if (!CFSDirectory::DoesDirecotryExist(l_cTempDestPath)) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s DOESN'T exist, Creating...", l_cTempDestPath.c_str());
        if (eFrameworkunifiedStatusOK != (l_estatus = CFSDirectory::CreateDirectory(l_cTempDestPath))) {  // LCOV_EXCL_BR_LINE 200: CreateDirectory() is eFrameworkunifiedStatusOK  // NOLINT[whitespace/line_length]
          // LCOV_EXCL_START 200: CreateDirectory() is eFrameworkunifiedStatusOK
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to create %s, errno:%d", l_cTempDestPath.c_str(), errno);
          return l_estatus;
          // LCOV_EXCL_STOP
        }
      }
      l_cTempDestPath.append(RELEASETEMP_FILENAME);

    } else {
      // LCOV_EXCL_START 6: l_cTempDestPath.length() is bigger than 0
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_cTempDestPath.length():%ld", static_cast<long int>(l_cTempDestPath.length())); // NOLINT (runtime/int)
      return eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    }
  }
  // get the destination folder path from full file path
  std::string l_cOutDirPath(f_cdestpath);
  size_t l_ifound = l_cOutDirPath.rfind("/");
  l_cOutDirPath = l_cOutDirPath.substr(0, l_ifound);

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Full Path for persistence %s.", f_cdestpath);

  // create the destination folder path if does not exists
  if (!CFSDirectory::DoesDirecotryExist(l_cOutDirPath)) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s DOESN'T exist, Creating...", l_cOutDirPath.c_str());

    if (eFrameworkunifiedStatusOK != (l_estatus = CFSDirectory::CreateDirectory(l_cOutDirPath))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed to create %s, errno:%d", l_cOutDirPath.c_str(), errno);
      return eFrameworkunifiedStatusFail;
    }
  }

  // open source file
  if (-1 == (l_iSourceFileFd = open(f_csrcpath, O_RDONLY, S_IRUSR))) {  // LCOV_EXCL_BR_LINE 5: open's error case
    // LCOV_EXCL_START 5: open's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Warning :: CP_WRK_FAILURE_SRC_NOT_FND, errno:%d", errno);
    l_estatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  } else {
    // open the destination file for writing or create if does not exists
    if (-1 == (l_iDestFileFd = open(l_cTempDestPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC,  // LCOV_EXCL_BR_LINE 5: open's error case  // NOLINT[whitespace/line_length]
                                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))) {
      // LCOV_EXCL_START 5: open's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CP_WRK_FAILURE_DST_CREATE, errno:%d", errno);
      l_estatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    } else {
      l_si32BufSize = GetCopyBufSize(l_iSourceFileFd, l_ui32SrcFileSize);

      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Src file:%s, size:%d", f_csrcpath, l_ui32SrcFileSize);
      l_cWriteBuffer = (PSTR) std::malloc(l_si32BufSize);
      f_uiNS_NPP_WRITESIZE = 0;

      if (NULL != l_cWriteBuffer) {  // LCOV_EXCL_BR_LINE 5: malloc's error case
#ifdef AGL_STUB
        while (1) {
          std::memset(l_cWriteBuffer, 0, l_si32BufSize);
          l_iReadBuffLength = read(l_iSourceFileFd, l_cWriteBuffer, l_si32BufSize);
          if (0 == l_iReadBuffLength) {
            break;
          } else if (0 < l_iReadBuffLength) {  // LCOV_EXCL_BR_LINE 5: read's error case
            f_uiNS_NPP_WRITESIZE += static_cast<UI_32>(l_iReadBuffLength);

            if (-1 == write(l_iDestFileFd, l_cWriteBuffer, l_iReadBuffLength)) {
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CP_WRK_FAILURE_WRITE_ERROR_CP, errno:%d", errno);
              l_estatus = eFrameworkunifiedStatusFail;
            }

            if (InAbortState()) {  // LCOV_EXCL_START 200: the state can't be aborted
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CP_WRK_FAILURE_ABORT_DURING_CP");
              l_estatus = eFrameworkunifiedStatusFail;
            }
            // LCOV_EXCL_STOP
          } else {
            // LCOV_EXCL_START 5: read's error case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error reading from source file, errno:%d", errno);
            l_estatus = eFrameworkunifiedStatusFail;
            // LCOV_EXCL_STOP
          }

          if (eFrameworkunifiedStatusOK != l_estatus) {
            break;
          }
        }
#else
        while (!eof(l_iSourceFileFd)) {
          std::memset(l_cWriteBuffer, 0, l_si32BufSize);
          if (-1 != (l_iReadBuffLength = read(l_iSourceFileFd, l_cWriteBuffer, l_si32BufSize))) {
            f_uiNS_NPP_WRITESIZE += static_cast<UI_32>(l_iReadBuffLength);

            if (-1 == write(l_iDestFileFd, l_cWriteBuffer, l_iReadBuffLength)) {
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CP_WRK_FAILURE_WRITE_ERROR_CP, errno:%d", errno);
              l_estatus = eFrameworkunifiedStatusFail;
            }

            if (InAbortState()) {
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CP_WRK_FAILURE_ABORT_DURING_CP");
              l_estatus = eFrameworkunifiedStatusFail;
            }
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error reading from source file, errno:%d", errno);
            l_estatus = eFrameworkunifiedStatusFail;
          }

          if (eFrameworkunifiedStatusOK != l_estatus) {
            break;
          }
        }
#endif

        free(l_cWriteBuffer);
        l_cWriteBuffer = NULL;
      } else {
        // LCOV_EXCL_START 5: malloc's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Mem alloc error for l_cWriteBuffer");
        l_estatus = eFrameworkunifiedStatusNullPointer;
        // LCOV_EXCL_STOP
      }

      close(l_iDestFileFd);

      if (eFrameworkunifiedStatusOK == l_estatus) {
        if (btmpfile == TRUE) {
          if (0 != rename(l_cTempDestPath.c_str(), f_cdestpath)) {  // LCOV_EXCL_START 5: rename's error case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            l_estatus = eFrameworkunifiedStatusFail;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
                   "Error renaming file %s to %s, errno: %d", l_cTempDestPath.c_str(), f_cdestpath,
                   errno);
          }
          // LCOV_EXCL_STOP
        }
      } else {
        // remove the source file after successful copy
        if (0 != remove(l_cTempDestPath.c_str())) {
          FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Temporary file not deleted::%s, errno:%d", l_cTempDestPath.c_str(), errno);
        }
      }
    }

    close(l_iSourceFileFd);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// OnWrkCmdResume
/// Callback on resume worker thread command
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::OnWrkCmdResume(HANDLE hthread) {  // LCOV_EXCL_START 8: not used
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (hthread) {
    pthread_mutex_lock(&m_sAbortMutex);
    m_sbAbortCopy = FALSE;
    pthread_mutex_unlock(&m_sAbortMutex);
    l_estatus =  SendAck(hthread, CP_WRK_CMD_RESUME);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Thread App Handle Null.");
    l_estatus = eFrameworkunifiedStatusInvldHandle;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// OnWrkCmdStart
/// Callback method on start worker thread command.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::OnWrkCmdStart(HANDLE hthread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (hthread) {  // LCOV_EXCL_BR_LINE 6:hthread can't be NULL
    l_estatus = SendAck(hthread, CP_WRK_CMD_START);
  } else {
    // LCOV_EXCL_START 6:hthread can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Thread App Handle Null.");
    l_estatus = eFrameworkunifiedStatusInvldHandle;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// OnWrkCmdStop
/// Callback method on stop worker thread command.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::OnWrkCmdStop(HANDLE hthread) {  // LCOV_EXCL_START 200: can't test
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (hthread) {
    l_estatus = SendAck(hthread, CP_WRK_CMD_STOP);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Thread App Handle Null.");
    l_estatus = eFrameworkunifiedStatusInvldHandle;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// OnCmdShutdownRequest
/// Callback method for  shutdown request command.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::OnCmdShutdownRequest(HANDLE hthread) {  // LCOV_EXCL_START 8: not used
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  if (hthread) {
    NSP_CopyShutdownAck l_tCopyShutdownAck = {};
    if (FrameworkunifiedGetMsgLength(hthread) == sizeof(l_tCopyShutdownAck)) {
      if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(hthread, &l_tCopyShutdownAck, sizeof(NSP_CopyShutdownAck),
                                                           eSMRRelease))) {
        FrameworkunifiedSendParent(hthread,  CMD_WRK_SHUTDOWN_ACK, sizeof(NSP_CopyShutdown), &l_tCopyShutdownAck);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error:FrameworkunifiedGetMsgDataOfSize returned:%d", l_estatus);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error:MsgLength: Expected:%ld, Received:%d.",
             static_cast<long int>(sizeof(l_tCopyShutdownAck)), FrameworkunifiedGetMsgLength(hthread)); // NOLINT (runtime/int)
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Thread App Handle Null.");
    l_estatus = eFrameworkunifiedStatusInvldHandle;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SendAck
/// Method to send ack to the parent thread
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::SendAck(HANDLE hthread, const ENSP_CopyWorkerProtocol &f_eworkercmd) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (hthread) {  // LCOV_EXCL_BR_LINE 6: hthread can't be NULL
    NSP_CopyAckMsg l_tCopyAckMs = {};
    l_tCopyAckMs.m_eworkerprotocol = f_eworkercmd;
    l_estatus =  FrameworkunifiedSendParent(hthread,  CP_WRK_ACK_CMD_COMPLETE, sizeof(l_tCopyAckMs), &l_tCopyAckMs);
  } else {
    // LCOV_EXCL_START 6: hthread can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Thread App Handle Null.");
    l_estatus = eFrameworkunifiedStatusInvldHandle;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// OnWrkCmdArchive
/// Callback method on archive command
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::OnWrkCmdArchive(HANDLE f_hThread) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // Archive response struct
  NSP_CopyStatusResponse l_tArchiveStatusResponse;

  if (f_hThread) {  // LCOV_EXCL_BR_LINE 6: f_hThread can't be NULL
    // Archive info struct
    NSP_CopyInfoCmd l_tArchiveInfo = {};

    if (FrameworkunifiedGetMsgLength(f_hThread) == sizeof(l_tArchiveInfo)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(f_hThread, &l_tArchiveInfo, sizeof(NSP_CopyInfoCmd),  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
                                                           eSMRRelease))) {
        // Copy archive status response
        l_tArchiveStatusResponse.m_eloadtype = l_tArchiveInfo.m_eloadtype;
        l_tArchiveStatusResponse.m_epersisttype = l_tArchiveInfo.m_epersisttype;
        std::strncpy(l_tArchiveStatusResponse.m_cpersistenttag, l_tArchiveInfo.m_cpersistenttag, (MAX_PATH_LENGTH - 1));
        std::strncpy(l_tArchiveStatusResponse.m_crequesterappname,
                     l_tArchiveInfo.m_crequesterappname, (MAX_PATH_LENGTH - 1));

        CCopyWorker::CArchive *l_pCArchive =  new CCopyWorker::CArchive();

        // Start the folder archive or extract operation
        if (eFrameworkunifiedStatusOK == (l_estatus = l_pCArchive->Archive(l_tArchiveInfo, l_tArchiveStatusResponse))) {
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Archive/Extract Successful!");
        } else {
          // Set the archive status to failure
          l_estatus = eFrameworkunifiedStatusFail;
        }
        if (NULL != l_pCArchive) {  // LCOV_EXCL_BR_LINE 5: l_pCArchive can't be null
          delete l_pCArchive;
          l_pCArchive = NULL;
        }
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error:FrameworkunifiedGetMsgDataOfSize returned:%d", l_estatus);  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      }
    } else {
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error:MsgLength: Expected:%ld, Received:%d.",
             static_cast<long int>(sizeof(l_tArchiveInfo)), FrameworkunifiedGetMsgLength(f_hThread)); // NOLINT (runtime/int)
      // LCOV_EXCL_STOP
    }

    if (eFrameworkunifiedStatusOK != l_estatus) {
      l_tArchiveStatusResponse.m_bpersistencechk = FALSE;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Archive/Extract fail.Sending parent message!");
    } else {
      l_tArchiveStatusResponse.m_bpersistencechk = TRUE;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Archive/Extract success.Sending parent message!");
    }

    FrameworkunifiedSendParent(f_hThread,  CP_WRK_NTFY, sizeof(NSP_CopyStatusResponse), &l_tArchiveStatusResponse);
  } else {
    // LCOV_EXCL_START 6: f_hThread can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusFail;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Thread Application Handle NULL");
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NSPCopyWorkerOnStart
/// Callback method on start of worker thread
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSPCopyWorkerOnStart(HANDLE hthread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (hthread) {  // LCOV_EXCL_BR_LINE 6: hthread can't be NULL
    pthread_mutex_lock(&g_mutworkerthread);

    // Note: this variable is made static just to ignore the resource leak showing in coverity analysis
    static CCopyWorker *l_pccopyworker = NULL;

    l_pccopyworker = new(std::nothrow) CCopyWorker;  // LCOV_EXCL_BR_LINE 11: unexpected branch

    if (NULL != l_pccopyworker) {  // LCOV_EXCL_BR_LINE 5: l_pccopyworker can't be NULL
      if (eFrameworkunifiedStatusOK == (l_estatus = l_pccopyworker->OnWrkCmdStart(hthread))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        l_estatus = FrameworkunifiedSetThreadSpecificData(hthread, l_pccopyworker);

        if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
          l_estatus = FrameworkunifiedAttachCallbacksToDispatcher(
                        hthread, AppName, aServiceHandlers, static_cast<UI_32>(_countof(aServiceHandlers)));

          if (0 == std::strcmp(NS_NPP_WRITE_THREAD_NAME, FrameworkunifiedGetAppName(hthread))) {
            l_estatus = FrameworkunifiedAttachCallbackToDispatcher(hthread, AppName,
                                                      CMD_DELETE_OLD_DATA, NPServiceOnDeleteOldDataCmd);
          }
        } else {
          // LCOV_EXCL_START 4: NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          delete l_pccopyworker;
          l_pccopyworker = NULL;
          // LCOV_EXCL_STOP
        }
      } else {
        // LCOV_EXCL_START 4: NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        delete l_pccopyworker;
        l_pccopyworker = NULL;

        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Thread Start failed.");
        l_estatus = eFrameworkunifiedStatusFail;
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_START 5: l_pccopyworker can't be NULL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Worker Object Null.");
      l_estatus = eFrameworkunifiedStatusNullPointer;
      // LCOV_EXCL_STOP
    }
    pthread_mutex_unlock(&g_mutworkerthread);
  } else {
    // LCOV_EXCL_START 6: hthread can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Thread App Handle Null.");
    l_estatus = eFrameworkunifiedStatusInvldHandle;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NSPCopyWorkerOnStop
/// Callback method on stop of worker thread
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSPCopyWorkerOnStop(HANDLE hthread) {  // LCOV_EXCL_START 200: can't test
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (NULL != hthread) {
    pthread_mutex_lock(&g_mutworkerthread);
    CCopyWorker *l_pccopyworker = static_cast<CCopyWorker *>(FrameworkunifiedGetThreadSpecificData(hthread));

    if (NULL != l_pccopyworker) {
      l_pccopyworker->OnWrkCmdStop(hthread);

      delete l_pccopyworker;
      l_pccopyworker = NULL;
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Worker Object Null");
      l_estatus = eFrameworkunifiedStatusNullPointer;
    }

    if (eFrameworkunifiedStatusOK != (l_estatus = FrameworkunifiedSetThreadSpecificData(hthread, NULL))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSetThreadSpecificData Error, status=%d", l_estatus);
    }

    PCSTR l_cservice = FrameworkunifiedGetAppName(hthread);
    if (NULL != l_cservice) {
      if (eFrameworkunifiedStatusOK != (l_estatus = FrameworkunifiedDetachServiceFromDispatcher(hthread, l_cservice))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error Detaching service %s from dispatcher, status=%d", l_cservice, l_estatus);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedGetAppName returned NULL");
      l_estatus = eFrameworkunifiedStatusNullPointer;
    }

    pthread_mutex_unlock(&g_mutworkerthread);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Thread App Handle Null.");
    l_estatus = eFrameworkunifiedStatusInvldHandle;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

  return l_estatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////
/// NPServiceOnDeleteOldDataCmd
/// This callback is used to delete the data which was requested to be deleted during
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPServiceOnDeleteOldDataCmd(HANDLE f_happ) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  SI_32 l_ithreadcreate = 0;
  pthread_attr_t l_tattr;
  struct sched_param l_tschedparam = {};

  // thread id
  pthread_t childthread = 0;

  if (EOK == pthread_attr_init(&l_tattr)) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    // detach the thread from parent thread
    if (EOK != pthread_attr_setdetachstate(&l_tattr, PTHREAD_CREATE_DETACHED)) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
      // LCOV_EXCL_START 11:except,C++ STL
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error setting detach state attribute for thread %s", NS_NPP_DATA_RESET_THREAD_NAME);
      l_estatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    }

    if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 200: l_estatus must be eFrameworkunifiedStatusOK
      // set the schedule property of thread
      if (EOK != pthread_attr_setinheritsched(&l_tattr, PTHREAD_EXPLICIT_SCHED)) {  // LCOV_EXCL_BR_LINE 5:except,C++ STL  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 5:except,C++ STL
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error setting inherit schedule attribute for thread %s", NS_NPP_DATA_RESET_THREAD_NAME);
        l_estatus = eFrameworkunifiedStatusFail;
        // LCOV_EXCL_STOP
      }
    }

    if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 200: l_estatus must be eFrameworkunifiedStatusOK
      if (EOK != pthread_attr_getschedparam(&l_tattr, &l_tschedparam)) {  // LCOV_EXCL_BR_LINE 5:except,C++ STL
        // LCOV_EXCL_START 5:except,C++ STL
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error getting schedule param attribute for thread %s", NS_NPP_DATA_RESET_THREAD_NAME);
        l_estatus = eFrameworkunifiedStatusFail;
        // LCOV_EXCL_STOP
      }
    }

    if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 200: l_estatus must be eFrameworkunifiedStatusOK
      // set the schedule priority of thread
      l_tschedparam.sched_priority = NS_NPP_DATA_RESET_THREAD_PRIO;
      if (EOK != pthread_attr_setschedparam(&l_tattr, &l_tschedparam)) {  // LCOV_EXCL_BR_LINE 5:except,C++ STL
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error setting schedule param attribute for thread %s", NS_NPP_DATA_RESET_THREAD_NAME);
        l_estatus = eFrameworkunifiedStatusFail;
      }
    }

    if (eFrameworkunifiedStatusOK == l_estatus) {
      // start the child thread
      if (EOK == (l_ithreadcreate = pthread_create(&childthread, &l_tattr, NSPDataResetThreadCallback, NULL))) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
               "Thread id %ld created, name=%s, prio=%d", childthread, NS_NPP_DATA_RESET_THREAD_NAME,
               NS_NPP_DATA_RESET_THREAD_PRIO);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "pthread_create failed for %s, error = %d", NS_NPP_DATA_RESET_THREAD_NAME,
               l_ithreadcreate);
      }
    }

    // release the thread attribute object
    pthread_attr_destroy(&l_tattr);
  } else {
    // LCOV_EXCL_START 11:except,C++ STL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to initialize attribute for thread %s", NS_NPP_DATA_RESET_THREAD_NAME);
    l_estatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NSPDataResetThreadCallback
/// Callback of data reset thread.
///////////////////////////////////////////////////////////////////////////////////////////
PVOID   NSPDataResetThreadCallback(PVOID f_parg) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  std::string f_csourcepath = "";
  std::string f_ctargetpath = "";
  std::string l_ctargetdir = "";

  DIR *l_pdir = NULL;
  struct dirent *l_pdirent = NULL;

  // set the name of thread
  if (EOK == pthread_setname_np(0, NS_NPP_DATA_RESET_THREAD_NAME)) {  // LCOV_EXCL_BR_LINE 5:pthread_setname_np's error case  // NOLINT[whitespace/line_length]
#ifdef AGL_STUB
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Name of thread id %ld set to %s",
           syscall(__NR_gettid), NS_NPP_DATA_RESET_THREAD_NAME);
#else
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Name of thread id %d set to %s", gettid(), NS_NPP_DATA_RESET_THREAD_NAME);
#endif
  } else {
    // LCOV_EXCL_START 5:pthread_setname_np's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
#ifdef AGL_STUB
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Error setting name %s for thread-id %ld", NS_NPP_DATA_RESET_THREAD_NAME,
           syscall(__NR_gettid));
#else
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Error setting name %s for thread-id %d", NS_NPP_DATA_RESET_THREAD_NAME, gettid());
#endif
    // LCOV_EXCL_STOP
  }

  f_csourcepath = CPersistence::GetStoragePath();
  l_pdir = opendir(f_csourcepath.c_str());

  if (NULL != l_pdir) {  // LCOV_EXCL_BR_LINE 5:opendir's error case
    if ('/' != f_csourcepath[f_csourcepath.length() - 1]) {  // LCOV_EXCL_BR_LINE 200: there must be '/' in f_csourcepath  // NOLINT[whitespace/line_length]
      f_csourcepath.append("/");
    }

    /*
     * Old Persistent data folders which needs to be deleted are renamed with the orignal folder name
     * followed by underscore and date and time.
     */
    std::string l_colduserdata = USERDATA;
    l_colduserdata.append("_");

    std::string l_coldfactorydata = FACTORYDATA;
    l_coldfactorydata.append("_");

    std::string l_coldfactorycustomerdata = FACTORYCUSTOMERDATA;
    l_coldfactorycustomerdata.append("_");

    std::string l_colddealerdata = DEALERDATA;
    l_colddealerdata.append("_");

    while (NULL != (l_pdirent = readdir(l_pdir))) {
      l_ctargetdir = l_pdirent->d_name;

      if (0 == l_ctargetdir.find(l_colduserdata) ||
          0 == l_ctargetdir.find(l_coldfactorydata) ||
          0 == l_ctargetdir.find(l_coldfactorycustomerdata) ||
          0 == l_ctargetdir.find(l_colddealerdata)) {
        f_ctargetpath.assign(f_csourcepath);
        f_ctargetpath.append(l_ctargetdir);

        if (!CFSDirectory::RemoveDirectory(f_ctargetpath, NS_NPP_REMOVE_DIR_DELAY)) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to delete directory :: %s", f_ctargetpath.c_str());
        }
      }
    }
    closedir(l_pdir);  // close the directory
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Directory does not exists :: %s", f_csourcepath.c_str());  // LCOV_EXCL_LINE 5:opendir's error case  // NOLINT[whitespace/line_length]
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// VerifyWithStoredCRC
/// Check if crc of the file passed matches with existing crc of a file
///////////////////////////////////////////////////////////////////////////////////////////
BOOL CCopyWorker::VerifyWithStoredCRC(PCSTR f_crequesterappname, PCSTR f_cpersistenttag, PCSTR f_cFile,
                                      UI_32 &f_ui32crc) {
  UI_32 l_ui32StoredCrc = 0;
  BOOL l_bRet = FALSE;
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;
  TMServiceTagCRC::iterator l_itServiceTag;
  TMTagCRC::iterator l_itTagCRC;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  f_ui32crc = 0;  // initialize with default

  if ((NULL != f_crequesterappname) && (NULL != f_cpersistenttag) && (NULL != f_cFile)) {   // LCOV_EXCL_BR_LINE 6: f_crequesterappname, f_cpersistenttag, f_cFile can't be null  // NOLINT[whitespace/line_length]
    // Get the existing crc of a file from the map
    CCopyWorker::g_objmtxservicetag.ReadLock();
    l_itServiceTag = CCopyWorker::g_mservicetagcrc.find(f_crequesterappname);
    if (l_itServiceTag != CCopyWorker::g_mservicetagcrc.end()) {
      l_itTagCRC = (l_itServiceTag->second).find(f_cpersistenttag);
      if (l_itTagCRC != (l_itServiceTag->second).end()) {
        l_ui32StoredCrc = (l_itTagCRC->second);
        l_estatus = eFrameworkunifiedStatusOK;
      } else {
        // Do nothing.
      }
    } else {
      // Do nothing.
    }
    CCopyWorker::g_objmtxservicetag.Unlock();

    if (eFrameworkunifiedStatusOK == l_estatus) {
      // get the crc of a file
      l_estatus = CalculateCRC32(f_cFile, f_ui32crc);
      if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 200: CalculateCRC32() return ok
        if (f_ui32crc == l_ui32StoredCrc) {
          l_bRet = TRUE;
          FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "CRC matched. crc:%u", l_ui32StoredCrc);
        } else {
          // Do nothing. Return value already set.
          FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "CRC different. stored:%u, new:%u",
                 l_ui32StoredCrc, f_ui32crc);
        }
      } else {
        // LCOV_EXCL_START: CalculateCRC32() return ok
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Warn: getting crc:file:%s, status:%d, errno:%d",
               f_cFile, l_estatus, errno);
        // LCOV_EXCL_STOP
      }
    } else {
      // Do nothing. Return value already set.
    }
  } else {
    // LCOV_EXCL_START 6: f_crequesterappname, f_cpersistenttag, f_cFile can't be null
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid param::f_crequesterappname:%p, f_cpersistenttag:%p, f_cFile:%p",
           f_crequesterappname, f_cpersistenttag, f_cFile);
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bRet;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// UpdateTagCRC
/// Update the internal map with new crc
///////////////////////////////////////////////////////////////////////////////////////////
VOID CCopyWorker::UpdateTagCRC(PCSTR f_crequesterappname, PCSTR f_cpersistenttag, UI_32 f_ui32crc) {
  TMServiceTagCRC::iterator l_itServiceTag;
  TMTagCRC::iterator l_itTagCRC;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  CCopyWorker::g_objmtxservicetag.ReadLock();
  l_itServiceTag = CCopyWorker::g_mservicetagcrc.find(f_crequesterappname);
  if (l_itServiceTag != CCopyWorker::g_mservicetagcrc.end()) {
    l_itTagCRC = (l_itServiceTag->second).find(f_cpersistenttag);
    if (l_itTagCRC != (l_itServiceTag->second).end()) {
      l_itTagCRC->second = f_ui32crc;
    } else {
      (l_itServiceTag->second).insert(std::make_pair(f_cpersistenttag, f_ui32crc));
    }
  } else {
    TMTagCRC l_mTagCRC;
    l_mTagCRC.insert(std::make_pair(f_cpersistenttag, f_ui32crc));
    CCopyWorker::g_mservicetagcrc.insert(std::make_pair(f_crequesterappname, l_mTagCRC));
  }
  CCopyWorker::g_objmtxservicetag.Unlock();
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////
/// TryFileCopyWithCRCCheck
/// Try file copy with CRC validation
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::TryFileCopyWithCRCCheck(PCSTR f_csource, PCSTR f_cdest, UI_32 f_ui32checkcount) {  // LCOV_EXCL_START 200: cannot test code  // NOLINT[whitespace/line_length]
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  UI_32 l_uiSrcFileCrc = 0;
  UI_32 l_uiDestFileCrc = 0;
  UI_32 l_uiFileSize = 0;
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  for (UI_16 l_uiCrcCnt = 1; l_uiCrcCnt <= CNSNPP::m_siCRCCheckCount; l_uiCrcCnt++) {
    l_uiSrcFileCrc = 0;
    l_uiDestFileCrc = 0;
    l_uiFileSize = 0;

    l_estatus = CopyFile(f_csource, f_cdest, l_uiFileSize, TRUE);
    if (eFrameworkunifiedStatusOK == l_estatus) {
      // calculate crc of file in temp memory(RAM)
      l_estatus = CalculateCRC32(f_csource, l_uiSrcFileCrc);
      if (eFrameworkunifiedStatusOK == l_estatus) {
        // calculate crc of file stored in persistent memory
        l_estatus = CalculateCRC32(f_cdest, l_uiDestFileCrc);
        if (eFrameworkunifiedStatusOK == l_estatus) {
          if (l_uiSrcFileCrc == l_uiDestFileCrc) {   // if checksum matches for both the files
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "CRC Checksum matches for file:%s :: 0x%x", f_csource, l_uiSrcFileCrc);
            FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO3, __FUNCTION__, "Bytes written:%d file=%s",
                   l_uiFileSize, f_cdest);
            break;
          } else {  // else delete the file from target
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "CRC doesn't match for src:%s::0x%x and dest:%s::0x%x, CRC cnt=%d",
                   f_csource, l_uiSrcFileCrc, f_cdest, l_uiDestFileCrc, l_uiCrcCnt);
            // if the checksum does not matches even for the last time, do not delete the file from target
            if (l_uiCrcCnt < (CNSNPP::m_siCRCCheckCount)) {
              if (0 != remove(f_cdest)) {
                FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Error deleting file:%s, errno:%d", f_cdest, errno);
              }
            }
          }
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error Calculating Checksum for DestFile:%s",
                 f_cdest != 0 ? f_cdest : NULL);
          l_estatus = eFrameworkunifiedStatusFail;
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error Calculating Checksum for SrcFile:%s",
               f_csource != 0 ? f_csource : NULL);
        l_estatus = eFrameworkunifiedStatusFail;
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FileCopy Error:%d", l_estatus);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
// LCOV_EXCL_STOP
