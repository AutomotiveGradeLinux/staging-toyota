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
/// \brief    TODO
///
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// INCLUDES
///////////////////////////////////////////////////////////////////////////////
#include "ss_logger_error_event_storage.h"
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include "ss_logger_util.h"
#include "ss_logger_error_event_archive.h"
#include "ss_logger_fs_directory.h"
#include "ss_logger_common.h"

CLoggerErrorEventStorage::CLoggerErrorEventStorage()
    : m_parentMcQueue(NULL),
      m_thread(-1),
      m_sendQueue(-1),
      m_receiveQueue(-1),
      m_destination(""),
      m_mutexInit(-1),
      m_pLoggerCfg(NULL),
      m_logTime(0) {
  m_responseVec.clear();
  m_mqattr.mq_flags = 0;
  m_mqattr.mq_maxmsg = 1;
  m_mqattr.mq_msgsize = 32;
  m_mqattr.mq_curmsgs = 0;
}

CLoggerErrorEventStorage::~CLoggerErrorEventStorage() {  // LCOV_EXCL_START 14:globle instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  this->Cleanup();
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CLoggerErrorEventStorage::Initialize(HANDLE f_hApp,
                                                std::string f_ParentName,
                                                CLoggerCfg *f_pLoggerCfg) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  pthread_attr_t l_attr;
  struct sched_param l_params;

  /* Open the queue */
  if (NULL == (m_pLoggerCfg = f_pLoggerCfg)) {  // LCOV_EXCL_BR_LINE 200:As it is not always NULL
    // LCOV_EXCL_START 200:As it is not always NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Parameter f_pLoggerCfg is NULL.");
    // LCOV_EXCL_STOP
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = m_pLoggerCfg->GetLoggerStorageInfo(m_loggerStorageInfo))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 200:To ensure success
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. GetLoggerStorageInfo(m_loggerStorageInfo)");
    // LCOV_EXCL_STOP
  } else if (NULL == (m_parentMcQueue = McOpenSender(f_ParentName.c_str()))) {  // LCOV_EXCL_BR_LINE 4:NSFW
    // LCOV_EXCL_START 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusErrOther;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. McOpenSender(f_ParentName)");
    // LCOV_EXCL_STOP
  } else if (-1 == (this->m_sendQueue = mq_open(m_loggerStorageInfo.Name.c_str(), O_CREAT | O_WRONLY, 0666, &m_mqattr))) {  // LCOV_EXCL_BR_LINE 5:C code eror  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 5:C code eror
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusErrOther;
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. mq_open(ERROR_EVENT_STORAGE_QUEUE_NAME,O_CREAT | O_WRONLY, 0666, NULL)");
    // LCOV_EXCL_STOP
  } else if (-1 == (this->m_receiveQueue = mq_open(m_loggerStorageInfo.Name.c_str(), O_RDONLY))) {  // LCOV_EXCL_BR_LINE 5:C code eror  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 5:C code eror
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusSessionLimitMaxedOut;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. mq_open(ERROR_EVENT_STORAGE_QUEUE_NAME,O_RDONLY)");
    // LCOV_EXCL_STOP
  } else if (EOK != (m_mutexInit = pthread_mutex_init(&this->m_mutex, NULL))) {  // LCOV_EXCL_BR_LINE 5:C code eror
    // LCOV_EXCL_START 5:C code eror
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusFail;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. pthread_mutex_init(&this->m_mutex, NULL)");
    // LCOV_EXCL_STOP
  } else if (EOK != pthread_attr_init(&l_attr)) {  // LCOV_EXCL_BR_LINE 5:C code eror
    // LCOV_EXCL_START 5:C code eror
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusSemUnlockFail;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. pthread_attr_init()");
    // LCOV_EXCL_STOP
  } else if (EOK != pthread_attr_getschedparam(&l_attr, &l_params)) {  // LCOV_EXCL_BR_LINE 5:C code eror
    // LCOV_EXCL_START 5:C code eror
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusSemCloseFail;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. pthread_attr_getschedparam()");
    // LCOV_EXCL_STOP
  } else {
    l_params.sched_priority = m_loggerStorageInfo.Priority;
    if (EOK != pthread_attr_setinheritsched(&l_attr, PTHREAD_EXPLICIT_SCHED)) {  // LCOV_EXCL_BR_LINE 5:C code eror
      // LCOV_EXCL_START 5:C code eror
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. pthread_attr_setinheritsched()");
      // LCOV_EXCL_STOP
    } else if (EOK != pthread_attr_setschedparam(&l_attr, &l_params)) {  // LCOV_EXCL_BR_LINE 5:C code eror
      l_eStatus = eFrameworkunifiedStatusDbResultError;
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          " Error but continue. pthread_attr_setschedparam(), priority = %d: %s",
          m_loggerStorageInfo.Priority, strerror(errno));  // LCOV_EXCL_BR_LINE 11:Unexpected branch
      l_eStatus = eFrameworkunifiedStatusOK;
    }
    if (EOK != pthread_create(&this->m_thread, &l_attr, &CLoggerErrorEventStorage::ThreadFunctionWrapper, this)) {  // LCOV_EXCL_BR_LINE 5:C code eror  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_START 5:C code eror
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusDbRecNotFound;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. pthread_create()");
      // LCOV_EXCL_STOP
    } else {
      l_eStatus = eFrameworkunifiedStatusOK;
    }
  }
  if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200:To ensure success
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    this->Cleanup();  // LCOV_EXCL_LINE 200:To ensure success
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %d", l_eStatus);
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerErrorEventStorage::Start(
    std::string f_target, TArtifactResponseVec f_responseVector,
    uint32_t f_time) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  /* Start the thread*/
  if (EOK == pthread_mutex_lock(&this->m_mutex)) {  // LCOV_EXCL_BR_LINE 200:To ensure success
    this->m_destination = f_target;
    this->m_responseVec = f_responseVector;
    this->m_logTime = f_time;
    (void) pthread_mutex_unlock(&this->m_mutex);
  }
  UI_8 l_cmd[MAX_QUEUE_MSG_SIZE];
  l_cmd[0] = 1;
  if (-1 == mq_send(this->m_sendQueue, reinterpret_cast<CHAR*>(l_cmd), m_mqattr.mq_msgsize, 0)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. mq_send(this->m_sendQueue %d) return %s", m_sendQueue,
           strerror(errno));
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %d", l_eStatus);
  return (l_eStatus);
}

void CLoggerErrorEventStorage::ThreadFunction(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  char l_data[MAX_QUEUE_MSG_SIZE] = { };
  SI_32 l_bytesRead = -1;
  SI_32 l_oldState;
  ELoggerStorageThreadCommands l_cmdId = eLoggerStorageThreadCmdWriteFailed;
  EFrameworkunifiedStatus l_eStatus;

  (void) pthread_setname_np(pthread_self(), ERROR_EVENT_STORAGE_QUEUE_NAME);

  pthread_cleanup_push(&CLoggerErrorEventStorage::CleanupWrapper, this);
    while (-1  // LCOV_EXCL_BR_LINE 200: mq_send can not send size of -1
        != (l_bytesRead =
            static_cast<SI_32>(mq_receive(this->m_receiveQueue, reinterpret_cast<char *>(l_data),
                       (size_t) m_mqattr.mq_msgsize, NULL)))) {
      std::string l_target;
      TArtifactResponseVec l_responseVec;
      uint32_t l_logTime;
      uint8_t status;
      Clock_getSystemTimeY2K38(&l_logTime, &status);
      (void) pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &l_oldState);
      if (EOK == pthread_mutex_lock(&this->m_mutex)) {  // LCOV_EXCL_BR_LINE 5: pthread_mutex_lock's error case.
        l_target = this->m_destination;
        l_responseVec = this->m_responseVec;
        l_logTime = this->m_logTime;
        this->m_responseVec.clear();
        this->m_destination = "";

        (void) pthread_mutex_unlock(&this->m_mutex);
      }
      (void) pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &l_oldState);
      if (access(l_target.c_str(), F_OK) == 0) {
        if (CFSDirectory::IsDirectory(l_target)) {  // LCOV_EXCL_BR_LINE 6: can not be a dir
          // LCOV_EXCL_START 6: new file create, it can not be find
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          BOOL isDeleteFiles = FALSE;
          SI_32 l_filesCopied = this->CopyFiles(l_target, l_responseVec,
                                                isDeleteFiles);

          switch (l_filesCopied) {
            case -1:
              /*Error while moving files*/
              l_cmdId = eLoggerStorageThreadCmdWriteFailed;
              break;
            case 0:
              /*No files where moved*/
              l_cmdId = eLoggerStorageThreadCmdNoWritten;
              break;
            default:
              if (l_filesCopied > 0) {
                l_cmdId = eLoggerStorageThreadCmdOK;
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                       " Info. %d files successfully copied.", l_filesCopied);
              }
              break;
          }
          // LCOV_EXCL_STOP
        }
      } else {
        if (eFrameworkunifiedStatusOK
            != (l_eStatus = this->PackageAndPlaceArtifacts(l_target,
                                                           l_responseVec,
                                                           l_logTime))) {
          /*Write Error.*/
          l_cmdId = eLoggerStorageThreadCmdWriteFailed;
        } else {
          l_cmdId = eLoggerStorageThreadCmdOK;
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                 " Info. Archive successfully written.");
        }
        if (eFrameworkunifiedStatusOK != RemoveDeprecatedArtifacts(l_responseVec)) {
          FRAMEWORKUNIFIEDLOG(
              ZONE_WARN, __FUNCTION__,
              " Warning. Wasn't able to delete all deprecated log artifacts.");
        }
      }

      std::string l_usb_path;
      size_t l_usb_path_length = 0;
      if (m_pLoggerCfg != NULL) {  // LCOV_EXCL_BR_LINE 6: m_pLoggerCfg can not be null
        l_usb_path = m_pLoggerCfg->getUsb0MountPath();
        l_usb_path_length = l_usb_path.length();
        if ((l_usb_path_length > 1)
            && (l_usb_path[l_usb_path.length() - 1] == '/')) {
          l_usb_path_length--;
        }
      }
      if ((l_usb_path_length > 0)
          && (l_target.compare(0, l_usb_path_length, l_usb_path, 0,
                               l_usb_path_length) != 0)) {
        l_usb_path_length = 0;
      }
      if ((l_usb_path_length == 0)
          && (l_target.compare(0, sizeof(DEBUG_USB_PATH) - 1, DEBUG_USB_PATH, 0,
                               sizeof(DEBUG_USB_PATH) - 1) == 0)) {
        l_usb_path = DEBUG_USB_PATH;
        l_usb_path_length = sizeof(DEBUG_USB_PATH) - 1;
      }
      if (l_usb_path_length > 1) {  // if USB path is '/' only, length is 1.
                                    // So conditions to be syncfs is greater than 1
        // sync usb device
        DIR* l_dirp = opendir(l_usb_path.c_str());
        if (l_dirp != NULL) {  // LCOV_EXCL_BR_LINE 5: c code error case
          int fd = dirfd(l_dirp);
          if (fd != -1) {  // LCOV_EXCL_BR_LINE 5: c code error case
            if (syncfs(fd) == -1) {
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                     " Error. syncfs failed path=%s [%s]", l_usb_path.c_str(),
                     strerror(errno));
            }
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   " Error. syncfs get fd failed path=%s, [%s]",
                   l_usb_path.c_str(), strerror(errno));
          }
          closedir(l_dirp);
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 " Error. syncfs opendir failed path=%s, [%s]",
                 l_usb_path.c_str(), strerror(errno));
        }
        {
#define USB_MEMORY_SYNC_WAITTIME 8  // 8 sec
          const struct timespec intval = { USB_MEMORY_SYNC_WAITTIME, 0 };
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                 "%d sec:Waiting for USB Memory Synchronization.(%s)",
                 USB_MEMORY_SYNC_WAITTIME, l_usb_path.c_str());
          nanosleep(&intval, NULL);
        }
      }
      if (eFrameworkunifiedStatusOK
          != McSend(this->m_parentMcQueue, m_loggerStorageInfo.Name.c_str(),
                    l_cmdId, 0, NULL)) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. McSend(this->m_parentMcQueue)");
      }
    }
    pthread_cleanup_pop(1);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

void CLoggerErrorEventStorage::Cleanup(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  if (-1 != this->m_receiveQueue) {  // LCOV_EXCL_BR_LINE 200: m_receiveQueue cam not be -1
    if (-1 == mq_close(this->m_receiveQueue)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. mq_close()");
    }
    this->m_receiveQueue = -1;
  }
  if (-1 != this->m_sendQueue) {  // LCOV_EXCL_BR_LINE 200: m_sendQueue cam not be -1
    if (-1 == mq_close(this->m_sendQueue)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. mq_close()");
    }
    this->m_sendQueue = -1;
    if (-1 == mq_unlink(m_loggerStorageInfo.Name.c_str())) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. mq_unlink()");
    }
  }

  if (-1 != m_mutexInit) {  // LCOV_EXCL_BR_LINE 200: m_mutexInit cam not be -1
    if (EOK != pthread_mutex_destroy(&this->m_mutex)) {  // LCOV_EXCL_BR_LINE 5: c code error case
      // LCOV_EXCL_START 5: c code error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. mq_unlink()");
      // LCOV_EXCL_STOP
    }
    m_mutexInit = -1;
  }

  if (NULL != m_parentMcQueue) {  // LCOV_EXCL_BR_LINE 200: m_parentMcQueue cam not be null
    if (eFrameworkunifiedStatusOK != McClose(m_parentMcQueue)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. McClose()");
    }
    this->m_parentMcQueue = NULL;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

void* CLoggerErrorEventStorage::ThreadFunctionWrapper(void* param) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CLoggerErrorEventStorage* l_pObj =
      reinterpret_cast<CLoggerErrorEventStorage *>(param);

  l_pObj->ThreadFunction();
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return NULL;
}

void CLoggerErrorEventStorage::CleanupWrapper(void* param) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CLoggerErrorEventStorage* l_pObj =
      reinterpret_cast<CLoggerErrorEventStorage *>(param);

  l_pObj->Cleanup();
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

///////////////////////////////////////////////////////////////////////////
//  Function : PackageAndPlaceArtifacts
//  brief    : This function creates an archive for all collected artifacts
//             and places the archive in the required location depending
//             on the event type and required naming criteria.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CLoggerErrorEventStorage::PackageAndPlaceArtifacts(
    std::string f_archiveDestination, TArtifactResponseVec &f_responseVector,
    uint32_t f_logTime) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  std::string l_coreFileName = "";
  SI_32 l_ret = 0;

  if (f_archiveDestination.size() == 0) {
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. Unknown package file path and name. Logging artifacts not stored!");
    l_eStatus = eFrameworkunifiedStatusFail;
  } else {
    CErrorEventArchive l_archiver;
    l_eStatus = l_archiver.openArchive(f_archiveDestination);
    if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 6: it can not be fail
      // LCOV_EXCL_START 6: it can not be fail
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          " Error. Failed to create logging archive: %s. Log archive will not be stored!",
          f_archiveDestination.c_str());
        // LCOV_EXCL_STOP
    } else {
      for (UI_32 i = 0; (i < f_responseVector.size()); i++) {
        if (f_responseVector[i].Filepath != std::string("")) {
          std::string l_destName = f_responseVector[i].Filepath;

          if ((f_responseVector[i].Filepath.find(".bmp")  // LCOV_EXCL_BR_LINE 200: no bmp file be set
              == f_responseVector[i].Filepath.find_last_of('.'))
              && (f_responseVector[i].Filepath.find(".bmp")
                  != std::string::npos)) {
            // LCOV_EXCL_START 200: no bmp file be set
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            char buffer[20];
            struct tm local_time;
            Clock_getLocalTimeY2K38(&f_logTime, &local_time);
            if (0
                != strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S.bmp",
                            &local_time)) {
              l_destName = buffer;
            }
            // LCOV_EXCL_STOP
          } else {
            l_destName = f_responseVector[i].Filepath.substr(
                f_responseVector[i].Filepath.find_last_of('/') + 1);
          }
          if (f_responseVector[i].ArtifactId == eArtifactIdPstoreLog) {  // LCOV_EXCL_BR_LINE 200: can not be eArtifactIdPstoreLog  // NOLINT[whitespace/line_length]
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            l_destName = KERNEL_LOG_PATH_DIR;  // LCOV_EXCL_LINE 200: can not be eArtifactIdPstoreLog
          }
          l_eStatus = l_archiver.addToArchive(f_responseVector[i].Filepath,
                                              l_destName);
          if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200: can not be fail
            // LCOV_EXCL_START 200: can not be fail
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(
                ZONE_ERR,
                __FUNCTION__,
                " Error. Failed to add artifact: %s to archive %s. Continuing with next artifact.",
                f_responseVector[i].Filepath.c_str(),
                f_archiveDestination.c_str());
            // LCOV_EXCL_STOP
          }
          l_ret++;
          if ((l_ret % m_loggerStorageInfo.TimeoutAfter) == 0) {
            usleep(m_loggerStorageInfo.Timeout);
          }
        }
      }

      if (eFrameworkunifiedStatusOK != l_archiver.closeArchive()) {  // LCOV_EXCL_BR_LINE 200: closeArchive can not be fail
        // LCOV_EXCL_START 200: closeArchive can not be fail
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_eStatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. l_archiver.closeArchive()");
        // LCOV_EXCL_STOP
      } else {
        // Cleanup artifacts that are no longer required.
        int rc = access(f_archiveDestination.c_str(), F_OK);
        if (0 != rc) {  // LCOV_EXCL_BR_LINE 5: f_archiveDestination is aways exist
          // LCOV_EXCL_START 5: f_archiveDestination is aways exist
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 " Error. Archive verification failed. Errno: %d, %s.", errno,
                 strerror(errno));
          l_eStatus = eFrameworkunifiedStatusFail;
          // LCOV_EXCL_STOP
        } else {
          TEXT(__FUNCTION__,
               " Error Event: A archive successfully written to: %s.",
               f_archiveDestination.c_str());
          l_eStatus = eFrameworkunifiedStatusOK;
        }
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///  File Copy
///  arg:
///  isDeleteFiles : TRUE  - delete original files after copy.
///                  FALSE - not delete after copy.
///  returns:
///  -1: error while writing
///   else, number of files written
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
// LCOV_EXCL_START 8: can not be called
SI_32 CLoggerErrorEventStorage::CopyFiles(
    std::string f_destination, TArtifactResponseVec &f_responseVector,
    BOOL isDeleteFiles) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  SI_32 l_ret = 0;
  BOOL l_isFaile = FALSE;
  for (UI_32 i = 0; i < f_responseVector.size(); i++) {
    if (f_responseVector[i].Filepath != "") {
      if (CFSDirectory::IsDirectory(f_responseVector[i].Filepath)) {
        DIR * l_pDir = opendir(f_responseVector[i].Filepath.c_str());
        if (l_pDir == NULL) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Could not open directory: %s",
                 f_responseVector[i].Filepath.c_str());
        } else {
          struct dirent l_pDirent;
          struct dirent* next;
          while (0 == readdir_r(l_pDir, &l_pDirent, &next) && next != NULL) {
            if ((0 != strcmp(l_pDirent.d_name, ".")) && /* Ignore special  . directory.                     */
            (0 != strcmp(l_pDirent.d_name, "..")) && /* Ignore special .. directory.                     */
            (0 != strcmp(l_pDirent.d_name, "lost+found")) && /* Ignore lost+found.                     */
            ('.' != l_pDirent.d_name[0])) {  /* Ignore hidden files                              */
              std::string l_fileSource = f_responseVector[i].Filepath;
              std::string l_fileDestination = f_destination;
              l_fileSource.append("/");
              l_fileSource.append(l_pDirent.d_name);
              l_fileDestination.append("/");
              l_fileDestination.append(l_pDirent.d_name);
              EFrameworkunifiedStatus loggerserviceRet = eFrameworkunifiedStatusOK;
              if (isDeleteFiles == TRUE) {
                loggerserviceRet = CLoggerUtil::MoveUntyped(l_fileSource,
                                                  l_fileDestination);
              } else {
                loggerserviceRet = CLoggerUtil::CopyUntyped(l_fileSource,
                                                  l_fileDestination);
              }

              if (eFrameworkunifiedStatusOK == loggerserviceRet) {
                l_ret++;
                if ((l_ret % m_loggerStorageInfo.TimeoutAfter) == 0) {
                  usleep(m_loggerStorageInfo.Timeout);
                }
              } else {
                l_isFaile = TRUE;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Could not move: %s",
                       l_fileSource.c_str());
              }
            }
          }
          (void) closedir(l_pDir);
        }
      } else {
        std::string l_filename = f_responseVector[i].Filepath.substr(
            f_responseVector[i].Filepath.find_last_of('/'));
        std::string l_destination = std::string(f_destination).append(
            l_filename);
        if (eFrameworkunifiedStatusOK
            == CLoggerUtil::MoveUntyped(f_responseVector[i].Filepath,
                                        l_destination)) {
          l_ret++;
          if ((l_ret % m_loggerStorageInfo.TimeoutAfter) == 0) {
            usleep(m_loggerStorageInfo.Timeout);
          }
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Could not move: %s",
                 f_responseVector[i].Filepath.c_str());
          l_isFaile = TRUE;
        }
      }
    }
  }
  if (l_isFaile)
    l_ret = -1;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_ret);
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CLoggerErrorEventStorage::RemoveDeprecatedArtifacts(
    TArtifactResponseVec &f_responseVector) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  BOOL l_success = TRUE;
  for (UI_32 i = 0; i < f_responseVector.size(); i++) {
    if ((f_responseVector[i].Filepath != "")
        && (TRUE == f_responseVector[i].Remove)) {
      if (CFSDirectory::IsDirectory(f_responseVector[i].Filepath)) {
        if (TRUE
            != CFSDirectory::RemoveDirectory(f_responseVector[i].Filepath)) {
          FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
                 " Warning. Directory %s failed to be deleted.",
                 f_responseVector[i].Filepath.c_str());
          l_success &= FALSE;
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                 " Info. Directory %s has been successfully deleted.",
                 f_responseVector[i].Filepath.c_str());
        }

      } else {
        errno = 0;
        if (0 != remove(f_responseVector[i].Filepath.c_str())) {  // LCOV_EXCL_BR_LINE 5: c code error case
          // LCOV_EXCL_START 5: c code error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          if (errno == ENOENT) {
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                   " Info. %s not available. Skipping file deletion.",
                   f_responseVector[i].Filepath.c_str());
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
                   " Warning. File %s failed to be deleted.",
                   f_responseVector[i].Filepath.c_str());
            l_success &= FALSE;
          }
          errno = 0;
          // LCOV_EXCL_STOP
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                 " Info. File %s has been successfully deleted.",
                 f_responseVector[i].Filepath.c_str());
        }
      }
    }
  }
  l_eStatus = (l_success == TRUE) ? eFrameworkunifiedStatusOK : eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
