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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SS_LoggerService
/// \brief    This file supports error event logging.
///
///////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <boost/bind.hpp>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/klog.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_multithreading.h>
#include <native_service/ns_sharedmem.h>
#include <native_service/ns_sharedmem_reader.h>
#include <native_service/ns_ringbuffer.h>
#include <native_service/ns_logger_if.h>
#include <system_service/ss_sm_client_if.h>
#include <system_service/ss_logger_service_protocol.h>
#include <system_service/ss_services.h>
#include <system_service/ss_templates.h>
#include <native_service/cl_process.h>
#include <native_service/cl_lockid.h>
#include <sstream>
#include <string>
#include "loggerservicedebug_loggerservicelog.h"
#include "loggerservicedebug_thread_if.h"
#include "ss_logger_error_event.h"
#include "ss_logger_service_callbacks.h"
#include "ss_logger_scrshot.h"
#include "ss_logger_fs_directory.h"
#include "ss_logger_common.h"
#include "ss_logger_types.h"

EFrameworkunifiedStatus SSLogger_SendtoSM(HANDLE f_hApp, TLoggingArtifact f_artifact) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  l_eStatus = SendLogArtifactRequestToSystemManager(f_artifact.ArtifactId);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. SendLogArtifactRequestToSystemManager returned: %d for artifact ID: %d",
        l_eStatus, f_artifact.ArtifactId);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus SSLogger_SendtoSelf(HANDLE f_hApp, TLoggingArtifact f_artifact) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusNullPointer;
  if (NULL != f_hApp) {  // LCOV_EXCL_BR_LINE 6: double check
    l_eStatus = FrameworkunifiedSendSelf(f_hApp, SS_LOGGER_ERROR_EVENT_ARTIFACT_REQ,
                            sizeof(f_artifact.ArtifactId),
                            &f_artifact.ArtifactId);

    if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          " Error. FrameworkunifiedSendSelf(SS_LOGGER_ERROR_EVENT_ARTIFACT_REQ) returned error: %d for artifact ID: %d.",
          l_eStatus, f_artifact.ArtifactId);
      // LCOV_EXCL_STOP
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
///////////////////////////////////////////////////////////////////////////
//  Function : SendLogArtifactResponseToSelf
//  brief    : Collect frameworkunifiedlog artifact and return a response to
//             SS_Logger.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::SendLogArtifactResponseToSelf(
    HANDLE f_hApp, EArtifactId f_artifactId,
    std::string f_artifactFilePathAndName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ARTIFACT_RESPONSE l_artifactResponse;

  l_artifactResponse.ArtifactId = f_artifactId;

  strncpy(l_artifactResponse.FilePathAndName, f_artifactFilePathAndName.c_str(),
          sizeof(l_artifactResponse.FilePathAndName) - 1);

  l_eStatus = FrameworkunifiedSendSelf(f_hApp, SS_SM_ERROR_EVENT_ARTIFACT_RSPN,
                          sizeof(l_artifactResponse), &l_artifactResponse);

  LOG_STATUS_IF_ERRORED(l_eStatus,
                        "FrameworkunifiedSendSelf(SS_SM_ERROR_EVENT_ARTIFACT_RSPN)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnObtainLoggerserviceLogRequest
//  brief    : Collect frameworkunifiedlog artifact and return a response to
//             SS_Logger.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnObtainLoggerserviceLogRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  std::string dir_path = DEBUG_LOG_PATH_DIR;

  l_eStatus = CFSDirectory::CreateDirectory(dir_path);
  if (l_eStatus != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: Create FRAMEWORKUNIFIEDLOG tmp directory %d",
           l_eStatus);
  } else {
    UI_32 l_DumpSize = 0;
    std::string src_file_path;
    std::string dst_file_path;
    PCSTR file_name;
    UI_32 num;
    bool log_clear = true;

    switch (m_errorEventNtfData.EventType) {
      // _DIAG
      case eErrorEventTypeUserInvokedUserForceReset:
      case eErrorEventTypeUserInvokedCollectAllLogs:
      case eErrorEventTypeUserInvokedCollectScreenShot:
      case eErrorEventTypeUserInvokedCollectInterfaceunifiedLogs:
      case eErrorEventTypeDiagEvent:
      case eErrorEventTypeCanEvent:
      case eErrorEventTypeDtcEvent:
      case eErrorEventTypeModuleInvokedCollectDebugLogs:
        // _DEBUG
      case eErrorEventTypeUserInvokedCollectDevLogs:
        log_clear = false;
        break;
      default:
        break;
    }

    num = NSLogGetFrameworkunifiedlogFileTotalNum();
    for (UI_32 cont = 0; cont < num; cont++) {
      file_name = NSLogGetFrameworkunifiedlogFileName(cont);
      if (file_name == NULL) {
        break;
      }
      src_file_path = FRAMEWORKUNIFIEDLOG_RAMDISC_PATH;
      src_file_path.append("/");
      src_file_path.append(file_name);
      dst_file_path = DEBUG_LOG_PATH_DIR;
      dst_file_path.append("/");
      dst_file_path.append(file_name);
      CNSRingBuffer* l_pLoggerservicelog;
      int index;
      index = NSLogGetFrameworkunifiedlogIndex(src_file_path.c_str());
      l_pLoggerservicelog = new (std::nothrow) CNSRingBuffer(
          src_file_path, 0, index + LOCK_NSLOG_ACCES_IF_1);
      if (l_pLoggerservicelog == NULL) {  // LCOV_EXCL_BR_LINE 5: new's error case
        // LCOV_EXCL_START 5: new's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_eStatus = eFrameworkunifiedStatusInvldHandle;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: FRAMEWORKUNIFIEDLOG RingBuffer handle is NULL %s",
               src_file_path.c_str());
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pLoggerservicelog->Open())) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: FRAMEWORKUNIFIEDLOG RingBuffer Open failed %s %d",
               src_file_path.c_str(), l_eStatus);
      } else if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pLoggerservicelog->DumpToFile(dst_file_path.c_str(),
                                                &l_DumpSize))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: FRAMEWORKUNIFIEDLOG RingBuffer DumpToFile failed %s %d",
               src_file_path.c_str(), l_eStatus);
      }
      if (NULL != l_pLoggerservicelog) {  // LCOV_EXCL_BR_LINE 5: new's error case
        if (log_clear) {
          if (eFrameworkunifiedStatusOK != (l_eStatus = l_pLoggerservicelog->ClearBuf())) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "Error: FRAMEWORKUNIFIEDLOG RingBuffer ClearBuf failed %s %d",
                   src_file_path.c_str(), l_eStatus);
          }
        }
        (void) l_pLoggerservicelog->Close();
        delete l_pLoggerservicelog;
      }
    }
    l_eStatus = SendLogArtifactResponseToSelf(f_hApp, eArtifactIdInterfaceunifiedDebugLog,
                                              DEBUG_LOG_PATH_DIR);
    LOG_STATUS_IF_ERRORED(
        l_eStatus, "SendLogArtifactResponseToSelf(eArtifactIdInterfaceunifiedDebugLog)");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnObtainLoggerserviceLogRequest
//  brief    : Collect frameworkunifiedlog artifact and return a response to
//             SS_Logger.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnObtainTransmitLogRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  std::string l_RootPath;

  l_eStatus = m_pReaderWriterControl->FlushCache(eReaderWriterLogTransmit);
  LOG_STATUS_IF_ERRORED(l_eStatus, "m_pReaderWriterControl->FlushCache(eReaderWriterLogTransmit)");
  l_RootPath = TRANSMIT_LOG_PATH_FN;
  if (access(l_RootPath.c_str(), R_OK) != 0) {
    l_RootPath = "";
  }
  l_eStatus = SendLogArtifactResponseToSelf(f_hApp, eArtifactIdTransmitLog,
                                            l_RootPath.c_str());

  LOG_STATUS_IF_ERRORED(l_eStatus,
                        "SendLogArtifactResponseToSelf(eArtifactIdTransmitLog)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnObtainLoggerserviceLogRequest
//  brief    : Collect frameworkunifiedlog artifact and return a response to
//             SS_Logger.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnObtainPerformanceLogRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  l_eStatus = m_pReaderWriterControl->FlushCache(eReaderWriterLogPerformance);
  LOG_STATUS_IF_ERRORED(
      l_eStatus,
      "m_pReaderWriterControl->FlushCache(eReaderWriterLogPerformance)");
  l_eStatus = SendLogArtifactResponseToSelf(f_hApp, eArtifactIdPerformanceLog,
                                            PERFORMANCE_LOG_PATH_FN);

  LOG_STATUS_IF_ERRORED(
      l_eStatus, "SendLogArtifactResponseToSelf(eArtifactIdPerformanceLog)");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

#define SCREEN_SHOT_INTVAL 500000  // 500msec
#define SCRREN_SHOT_CAPTURE_RETRY_CNT 2

static EFrameworkunifiedStatus SaveScreenShot(char *chldexe, char **argv, int m_sfd,
                                 CL_ProcessAttr_t *attr) {
  int selection = -1;
  fd_set fds;
  struct timeval tv;
  pid_t childpid;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  int retry = 0;

  while (1) {
    childpid = CL_ProcessCreate(chldexe, argv, NULL, attr);
    if (childpid == -1) {  // LCOV_EXCL_BR_LINE 5: c code case
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "create process failed returned: %d",
             errno);
    } else {
      do {
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(m_sfd, &fds);
        selection = select(m_sfd + 1, &fds, NULL, NULL, &tv);
      } while ((selection < 0) && (errno == EINTR));  // LCOV_EXCL_BR_LINE 5: c code case
      if (selection < 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "select failed %d", errno);
      } else {
        if (FD_ISSET(m_sfd, &fds)) {  // LCOV_EXCL_BR_LINE 5: FD_ISSET's error case
          CL_ProcessCleanupInfo_t cinfo;
          int ret;

          ret = CL_ProcessCleanup(m_sfd, &cinfo);
          if ((ret != -1) && (cinfo.code == CLD_EXITED)) {
            if (cinfo.status != 0) {
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Save Error! cinfo.status:%d",
                     cinfo.status);
            } else {
              l_eStatus = eFrameworkunifiedStatusOK;
            }
          }
        } else {
          // LCOV_EXCL_START 5: FD_ISSET's error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          // (FD_ISSET() failed Error
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "fd Not Exist fd Error\n");
          // LCOV_EXCL_STOP
        }
      }
    }
    if ((l_eStatus == eFrameworkunifiedStatusOK)
        || (retry >= (SCRREN_SHOT_CAPTURE_RETRY_CNT - 1))) {
      break;
    }
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "retry!");
    usleep(SCREEN_SHOT_INTVAL);  // interval
    retry++;
  }
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnObtainScreenShotRequest
//  brief    : Collect screen shot artifact from PLM and return a response
//             to SS_Logger.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnObtainScreenShotRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  std::string dir_path = SCREEN_SHOT_PATH;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  EFrameworkunifiedStatus l_eStatusCh0 = eFrameworkunifiedStatusFail;
  EFrameworkunifiedStatus l_eStatusCh1 = eFrameworkunifiedStatusFail;

  char *c_argv0[4] = { NULL, const_cast<char*>(JPEG_PATH_CH0), const_cast<char*>("0"), NULL };
  char *c_argv1[4] = { NULL, const_cast<char*>(JPEG_PATH_CH1), const_cast<char*>("1"), NULL };
  char *chldexe;

  /*check m_sfd vlue*/
  if (m_sfd == -1) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SS_Logger_SaveScreenShot m_sfd error -1");
    return l_eStatus;
  }

  /*check chldexe value*/
  chldexe = realpath(SCRSHOT_EXE_PATH, NULL);
  if (chldexe == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "SS_Logger_SaveScreenShot realpath failed %d", errno);
    return l_eStatus;
  }

  c_argv0[0] = chldexe;
  c_argv1[0] = chldexe;
  l_eStatus = CFSDirectory::CreateDirectory(dir_path);
  if (l_eStatus != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "SS_Logger_SaveScreenShot tmp directory failed");
    free(chldexe);
    return l_eStatus;
  } else {
    CL_ProcessAttr_t attr;
    CL_ProcessCreateAttrInit(&attr);

    /* Save Picture Ch0(E2) */
    l_eStatusCh0 = SaveScreenShot(chldexe, c_argv0, m_sfd, &attr);
    if (l_eStatusCh0 == eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 200:As there are no ScreenShot processes
      // LCOV_EXCL_START 200:As there are no ScreenShot processes
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      /* Start Save Picture Ch1(M2) */
      l_eStatusCh1 = SaveScreenShot(chldexe, c_argv1, m_sfd, &attr);
      if (l_eStatusCh1 != eFrameworkunifiedStatusOK) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "SS_Logger_SaveScreenShot Ch1(M2) failed");
      }
      // LCOV_EXCL_STOP
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SS_Logger_SaveScreenShot Ch0(E2) failed");
    }
  }

  // Send Artifact Responce.
  if ((l_eStatusCh0 == eFrameworkunifiedStatusOK) || (l_eStatusCh1 == eFrameworkunifiedStatusOK)) {  // LCOV_EXCL_BR_LINE 200:As there are no ScreenShot processes
    // LCOV_EXCL_START 200:As there are no ScreenShot processes
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    // Set "SCREEN_SHOT_PATH" to save two pictures(ch0(E2) and ch1(M2))
    l_eStatus = SendLogArtifactResponseToSelf(f_hApp, eArtifactIdScreenShot,
    SCREEN_SHOT_PATH);
    LOG_STATUS_IF_ERRORED(
        l_eStatus, "SendLogArtifactResponseToSelf(eArtifactIdScreenShot)");
    // LCOV_EXCL_STOP
  } else {
    // If both Ch0 and Ch1 is failed,
    // update l_eStatus to eFrameworkunifiedStatusFail and delete directory.
    l_eStatus = eFrameworkunifiedStatusFail;
    if (CFSDirectory::DoesDirectoryExist(dir_path)) {
      if (CFSDirectory::RemoveDirectory(dir_path)) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
               "SS_Logger_SaveScreenShot directory delete successful.");
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "SS_Logger_SaveScreenShot directory delete unsuccessful");
      }
    }
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "SS_Logger_SaveScreenShot Save Failed returned:%d", l_eStatus);
  }
  free(chldexe);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnObtainScreenShotResponse
//  brief    : Callback from PLM when the screen shot file has been written
//             to storage.  The data received contains the file path and
//             name to the written screen shot file. See
//             SS_LOGGER_SCREENCAPTURE_EVT_ACK.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnObtainScreenShotResponse(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  STScreenCaptureEvt l_ScreenCaptureEvtAck;

  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<STScreenCaptureEvt>(f_hApp, l_ScreenCaptureEvtAck))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 200:To ensure success
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");
    // LCOV_EXCL_STOP
  } else {
    if (FALSE == l_ScreenCaptureEvtAck.fSucessful) {
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          " Error. Screen shot acquisition failed. l_ScreenCaptureEvtAck.fSucessful == FALSE.");
      l_ScreenCaptureEvtAck.strNameAndLocation[0] = '\0';
    }

    l_eStatus = SendLogArtifactResponseToSelf(
        f_hApp, eArtifactIdScreenShot,
        l_ScreenCaptureEvtAck.strNameAndLocation);  // LCOV_EXCL_BR_LINE 11:Unexpected branch

    LOG_STATUS_IF_ERRORED(  // LCOV_EXCL_BR_LINE 5:macro
        l_eStatus, "SendLogArtifactResponseToSelf(eArtifactIdScreenShot)");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnObtainKernelLogInfoRequest
//  brief    : Collect kernel log artifact and return a response to
//             SS_Logger.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnObtainKernelLogInfoRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  EFrameworkunifiedStatus l_eStatus = CreateKernelLog(f_hApp, SS_LOGGER_KLOG_GET);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnObtainDRInitialLogRequest
//  brief    : Collect DRInitialLog artifact and return a response to
//             SS_Logger.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnObtainDRInitialLogRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_DumpSize = 0;

  CNSSharedMemReader* l_pDRInitial = new (std::nothrow) CNSSharedMemReader(
      SHMEM_DRINITIALLOG, FALSE);

  if (NULL == l_pDRInitial) {  // LCOV_EXCL_BR_LINE 5: new's error case
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: DRInitial SharedMem handle is NULL");
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pDRInitial->Open())) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: DRInitial Open failed %d", l_eStatus);
  } else if (eFrameworkunifiedStatusOK
      != (l_eStatus = l_pDRInitial->DumpToFile(DRINITIAL_LOG_PATH_FN,
                                               &l_DumpSize))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: DRInitial DumpToFile failed %d",
           l_eStatus);
  } else {
    l_eStatus = SendLogArtifactResponseToSelf(f_hApp, eArtifactIdDRInitialLog,
                                              DRINITIAL_LOG_PATH_FN);
    LOG_STATUS_IF_ERRORED(
        l_eStatus, "SendLogArtifactResponseToSelf(eArtifactIdDRInitialLog)");
  }

  if (NULL != l_pDRInitial) {  // LCOV_EXCL_BR_LINE 5: new's error case
    (void) l_pDRInitial->Close();
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnObtainDRLocationLogRequest
//  brief    : Collect DRLocationLog artifact and return a response to
//             SS_Logger.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnObtainDRLocationLogRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_DumpSize = 0;

  CNSSharedMemReader* l_pDRLocation = new (std::nothrow) CNSSharedMemReader(
      SHMEM_DRLOCATIONLOG, FALSE);

  if (NULL == l_pDRLocation) {  // LCOV_EXCL_BR_LINE 5: new's error case
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: DRInitial SharedMem handle is NULL");
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pDRLocation->Open())) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: DRLocation Open failed %d",
           l_eStatus);
  } else if (eFrameworkunifiedStatusOK
      != (l_eStatus = l_pDRLocation->DumpToFile(DRLOCATION_LOG_PATH_FN,
                                                &l_DumpSize))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: DRLocation DumpToFile failed %d",
           l_eStatus);
  } else {
    l_eStatus = SendLogArtifactResponseToSelf(f_hApp, eArtifactIdDRLocationLog,
                                              DRLOCATION_LOG_PATH_FN);

    LOG_STATUS_IF_ERRORED(
        l_eStatus, "SendLogArtifactResponseToSelf(eArtifactIdDRLocationLog)");
  }

  if (NULL != l_pDRLocation) {  // LCOV_EXCL_BR_LINE 5: new's error case
    (void) l_pDRLocation->Close();
    delete l_pDRLocation;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnClearAllLogRequest
//  brief    : Callback from LoggerService when the event log file has been delete
//             from storage.  The data received contains the file path.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnClearAllLogRequest(HANDLE f_hApp) {
  std::string clear_file_path = CLEAR_LOG_PATH_FN;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  if (CFSDirectory::RemoveSubDirectory(clear_file_path)) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           "SS_Logger Clear %s directory delete successful.",
           clear_file_path.c_str());
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "SS_Logger Clear %s directory delete failed.",
           clear_file_path.c_str());
  }

  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  clear_file_path = "/nv/log2/awlog/";
  if (CFSDirectory::RemoveSubDirectory(clear_file_path)) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           "SS_Logger Clear %s directory delete successful.",
           clear_file_path.c_str());
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "SS_Logger Clear %s directory delete failed.",
           clear_file_path.c_str());
  }

  clear_file_path = "/nv/log2/core/";
  if (CFSDirectory::RemoveSubDirectory(clear_file_path)) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           "SS_Logger Clear %s directory delete successful.",
           clear_file_path.c_str());
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "SS_Logger Clear %s directory delete failed.",
           clear_file_path.c_str());
  }
  // LCOV_EXCL_STOP

  l_eStatus = SendLogArtifactResponseToSelf(f_hApp, eArtifactIdClearAllLog,
                                            clear_file_path.c_str());
  LOG_STATUS_IF_ERRORED(l_eStatus,
                        "SendLogArtifactResponseToSelf(eArtifactIdClearAllLog)");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnNaviLogRequest
//  brief    : Callback from LoggerService when the event log file has been xxxxxx BUGBUG
//             from storage.  The data received contains the file path.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CErrorEvent::OnNaviLogRequest(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  // LCOV_EXCL_START 13:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus naviLog_status;
  switch (m_errorEventNtfData.EventType) {
    case eErrorEventTypeProcessCrash:
    case eErrorEventTypeProcessExit:
    case eErrorEventTypeHeartBeatFailure:
    case eErrorEventTypeSystemLowMemory:
    case eErrorEventTypeBootMicroReset:
    case eErrorEventTypeModConnFailed:
    case eErrorEventTypeStartRespFailed:
    case eErrorEventTypeModuleInvokedResetRequest:
    case eErrorEventTypeGroupRelaunch:
      naviLog_status = SaveNaviLog(epssdmsdtGENERIC_ERROR_RESET);
      break;
    default:
      naviLog_status = SaveNaviLog(epssdmsdtNORMAL_RESET);
      break;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: Fail to save Navi Log %d",
         naviLog_status);
  // LCOV_EXCL_STOP

  l_eStatus = SendLogArtifactResponseToSelf(f_hApp, eArtifactIdNaviLog, "");
  LOG_STATUS_IF_ERRORED(l_eStatus,
                        "SendLogArtifactResponseToSelf(eArtifactIdNaviLog)");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CErrorEvent::CheckPathForArtifact(HANDLE f_hApp,
                                             TLoggingArtifact f_artifact) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldParam;
  std::string l_Path;

  if (f_artifact.Information.Cb.Type == eCbTypePath) {
    l_Path = f_artifact.Information.Cb.Path;
    if (access(l_Path.c_str(), R_OK) != 0) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             "File/Directory %s not found. Returning empty string.",
             l_Path.c_str());
      l_Path = "";
    }
    l_eStatus = SendLogArtifactResponseToSelf(f_hApp, f_artifact.ArtifactId,
                                              l_Path.c_str());

    LOG_STATUS_IF_ERRORED(l_eStatus, "SendLogArtifactResponseToSelf()");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

/* Read all messages remaining in the ring buffer. (allowed for non-root) */
#define SYSLOG_ACTION_READ_ALL       3
/* Return size of the log buffer */
#define SYSLOG_ACTION_SIZE_BUFFER   10

EFrameworkunifiedStatus CErrorEvent::CreateKernelLog(HANDLE f_hApp,
                                        SS_LOGGER_KLOG_OPE_TYPE type) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  int len;
  int fd;
  char* buf;
  ssize_t wr_len;

  len = klogctl(SYSLOG_ACTION_SIZE_BUFFER, NULL, 0);
  if (len < 0) {  // LCOV_EXCL_BR_LINE 5: klogctl's error case
    // LCOV_EXCL_START 5: klogctl's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusFail;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error. Failed to get syslog buffer size: %d, %s", errno,
           strerror(errno));
    // LCOV_EXCL_STOP
  } else {
    buf = reinterpret_cast<char*>(malloc(len));
    if (buf != NULL) {  // LCOV_EXCL_BR_LINE 5: new's error case
      len = klogctl(SYSLOG_ACTION_READ_ALL, buf, len);
      if ((len < 0) || (len == 0)) {  // LCOV_EXCL_BR_LINE 5: klogctl's error case
        // LCOV_EXCL_START 5: klogctl's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_eStatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error. Failed to read syslog buffer: %d, %s", errno,
               strerror(errno));
        // LCOV_EXCL_STOP
      } else {
        std::string file_put_path = "";
        EArtifactId artifact_id = eArtifactIdMaxValue;
        switch (type) {
          case SS_LOGGER_KLOG_GET:
            file_put_path = KERNEL_LOG_PATH_FN;
            artifact_id = eArtifactIdKernelLog;
            fd = open(file_put_path.c_str(),
                      O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
            break;
          case SS_LOGGER_KBOOTLOG_CREATE:
            file_put_path = KERNEL_BOOT_LOG_PATH_FN;
            artifact_id = eArtifactIdKernelBootLog;
            fd = open(file_put_path.c_str(),
                      O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
            break;
          default:
            fd = -1;
            break;
        }

        if (fd == -1) {
          l_eStatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Error. Failed to open file: %s: %d, %s",
                 file_put_path.c_str(), errno, strerror(errno));
        } else {
          wr_len = write(fd, buf, len);
          if (wr_len == -1) {  // LCOV_EXCL_BR_LINE 5: write's error case
            // LCOV_EXCL_START 5: write's error case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   " Error. File write failed. errno: %d, %s", errno,
                   strerror(errno));
            // LCOV_EXCL_STOP
          }
          fsync(fd);
          close(fd);

          if (SS_LOGGER_KLOG_GET == type) {
            l_eStatus = SendLogArtifactResponseToSelf(f_hApp, artifact_id,
                                                      file_put_path.c_str());
            if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
              // LCOV_EXCL_START 4: NSFW error case.
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(
                  ZONE_ERR,
                  __FUNCTION__,
                  "SendLogArtifactResponseToSelf failed. EFrameworkunifiedStatus:%d, ArtifactID:%d, Path:%s",
                  l_eStatus, artifact_id, file_put_path.c_str());
              // LCOV_EXCL_STOP
            }
          }
        }
      }
      free(buf);
    } else {
      // LCOV_EXCL_START 5: new's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Memory allocation Failed %s",
             strerror(errno));
      // LCOV_EXCL_STOP
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

