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
/// \ingroup  SS_SM_Logging
/// \brief    This file supports SM logging.
///
///////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>
#include <native_service/ns_plogger_if.h>
//#include <stub/ss_diag.h>
#include <system_service/ss_ver.h>
#include <system_service/ss_services.h>
#include <system_service/ss_templates.h>
#include <system_service/ss_client_names.h>
#include <system_service/ss_sm_client_if.h>

#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <list>

#include "ss_system_manager.h"

///////////////////////////////////////////////////////////////////////////
//  Function : OnObtainLoggerserviceLogRequest
//  brief    : Collect frameworkunifiedlog artifact and return a response to
//             SS_Logger.
///////////////////////////////////////////////////////////////////////////
static int getExecedString(char* const argv[], std::stringstream &ss) {
  int ret;
  int pipeFd[2];  // 0:read, 1:write
  pid_t pid;
  ret = pipe(pipeFd);
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 5:pipe's error case
    // LCOV_EXCL_START 5:pipe's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
    SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    goto error_exit;
    // LCOV_EXCL_STOP
  }

  pid = fork();
  if (pid == -1) {  // LCOV_EXCL_BR_LINE 5:fork's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
    SS_ASERT_ERRNO(0);  // LCOV_EXCL_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else if (pid == 0) {
    close(pipeFd[0]);    // unnecessary pile (closing read)
    close(1);        // closing stdout
    dup2(pipeFd[1], 1);  // take stdout to pipe
    execve(argv[0], argv, environ);
    SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    exit(EXIT_FAILURE);
  } else {
    char readBuf[256];
    ssize_t rs;
    SS_ASERT_ERRNO(0 == close(pipeFd[1]));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

    fd_set rfds;
    FD_ZERO(&rfds);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
    FD_SET(pipeFd[0], &rfds);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    int selRet = select(pipeFd[0] + 1, &rfds, NULL, NULL, &timeout);
    switch (selRet) {
    case 1:
      if (FD_ISSET(pipeFd[0], &rfds)) {
        do {
          rs = read(pipeFd[0], readBuf, sizeof(readBuf) - 1);
          if (rs > 0) {
            readBuf[rs] = '\0';
            ss << readBuf;
          } else if (rs == -1) {  // LCOV_EXCL_BR_LINE 5:read's error case
            // LCOV_EXCL_START 5:read's error case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
            if (errno != EINTR) {
              SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
              ret = -1;
              break;
            }
            // LCOV_EXCL_STOP
          }
        } while (rs != 0);
      } else {
        SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      }
      break;
    case -1:
      SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      break;
    default:
      // LCOV_EXCL_START 5:never be this case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
      if (0 == selRet) {
        ss << "timeout" << std::endl;
      } else {
        ss << "ERR:rslt=" << selRet << std::endl;
      }
      break;
      // LCOV_EXCL_STOP
    }

    // SM leaves processing to OnProcessExitDetected when using child process
    // SS_ASERT_ERRNO(-1 != waitpid(pid,&status,0));
  }
  SS_ASERT_ERRNO(0 == close(pipeFd[0]));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
error_exit:
  return ret;
}

static int saveProcsMemInfo(std::ofstream &fo) {
  DIR *dir = NULL;
  int ret = 0;

  try {
    struct dirent dent, *next;
    // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
    {
      fo << "******** mem info **********" << std::endl;
      std::ifstream fin("/proc/meminfo");
      std::string line;
      while (fin && std::getline(fin, line)) {
        fo << line << endl;
      }
    }

    {
      fo << "******** slab info **********" << std::endl;
      std::ifstream fin("/proc/slabinfo");
      std::string line;
      while (fin && std::getline(fin, line)) {
        fo << line << endl;
      }
    }

    {
      fo << "******** zone info **********" << std::endl;
      std::ifstream fin("/proc/zoneinfo");
      std::string line;
      while (fin && std::getline(fin, line)) {
        fo << line << endl;
      }
    }
    // LCOV_EXCL_BR_STOP

    // Linux dependency codes
    dir = opendir("/proc");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    if (dir == NULL) {  // LCOV_EXCL_BR_LINE 5:opendir's error case
      // LCOV_EXCL_START 11:opendir's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
      SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      throw eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    }

    fo << "******** proc status **********" << std::endl;
    while (0 == readdir_r(dir, &dent, &next) && next) {
      if (DT_DIR == dent.d_type) {
        struct stat statbuf;
        std::string statPath("/proc/");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        statPath += dent.d_name;
        statPath += "/status";

        if (stat(statPath.c_str(), &statbuf) == 0) {
          std::ifstream fin(statPath.c_str());  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
          std::string line;
          while (fin && std::getline(fin, line)) {
            if (strstr(line.c_str(), "Name") != NULL
                || strstr(line.c_str(), "Pid") != NULL
                || strstr(line.c_str(), "Vm") != NULL) {
              fo << line << endl;
            }
          }
          fo << "************************************" << std::endl;
        }
      }
    }
    SS_ASERT_ERRNO(0 == closedir(dir));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } catch (...) {
    if (dir) {  // LCOV_EXCL_BR_LINE 5:opendir's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
      SS_ASERT_ERRNO(0 == closedir(dir));  // LCOV_EXCL_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    }
    ret = -1;
  }
  return ret;
}

///////////////////////////////////////////////////////////////////////////
//  Function : OnObtainLoggerserviceLogRequest
//  brief    : Collect frameworkunifiedlog artifact and return a response to
//             SS_Logger.
///////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::SendLogArtifactResponseToLogger(HANDLE f_hApp,
        EArtifactId f_artifactId, std::string f_artifactFilePathAndName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  HANDLE l_hLoggerServiceSession;
  ModuleLaunchListIter l_ModuleListIter;
  ARTIFACT_RESPONSE l_artifactResponse;

  l_artifactResponse.ArtifactId = f_artifactId;

  strncpy(l_artifactResponse.FilePathAndName,
      f_artifactFilePathAndName.c_str(),
      sizeof(l_artifactResponse.FilePathAndName) - 1);

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = GetModuleIterator(SERVICE_LOGGER, l_ModuleListIter))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: GetModuleIterator(%s) errored: %d/'%s'",
        SERVICE_LOGGER, l_eStatus, GetStr(l_eStatus).c_str());
  } else {
    l_hLoggerServiceSession = l_ModuleListIter->hsession;

    l_eStatus = FrameworkunifiedSendMsg(l_hLoggerServiceSession,
        SS_SM_ERROR_EVENT_ARTIFACT_RSPN, sizeof(l_artifactResponse),
        &l_artifactResponse);

    LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
        "FrameworkunifiedSendMsg(SS_SM_ERROR_EVENT_ARTIFACT_RSPN)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnErrorEventArtifactRequest
/// Dispatch logging requests to the various handlers.
///
/// \param f_hApp Framework application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnErrorEventArtifactRequest(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = ReadMsg < EArtifactId
          > (hApp, m_requestedArtifactId))) {  // LCOV_EXCL_BR_LINE 200:NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  } else {
    switch (m_requestedArtifactId) {
    case eArtifactIdBootMicroLog:
      l_eStatus = OnObtainBootMicroLog(hApp);
      LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "OnObtainBootMicroLog()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      break;

    case eArtifactIdSystemDataCsv:
      l_eStatus = OnObtainSystemmanagerSystemDataCsv(hApp);
      LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "OnObtainSystemmanagerSystemDataCsv()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      break;

    case eArtifactIdShowMemTxt:
      l_eStatus = OnObtainShowMemTxt(hApp);
      LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "OnObtainShowMemTxt()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      break;

    case eArtifactIdProcessCore:
      SS_ASERT(0);  // Never called in Linux PF // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      break;

    case eArtifactIdDebugDumpLog:
      l_eStatus = OnObtainDebugDumpLog(hApp);
      LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "OnObtainDebugDumpLog()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      break;

    default:
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: Unsupported logging artifact requested: %d.",
          m_requestedArtifactId);
      break;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnObtainBootMicroLog
/// Obtain the boot micro log content, write file to disk, and send filename to SL.
///
/// \param [in] f_hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnObtainBootMicroLog(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(f_hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(f_hApp));

  l_eStatus = RequestBootMicroLog(f_hApp);
  LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus, "RequestBootMicroLog");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnBootMicroLogResponse
/// Boot micro log response sent from the logging shadow.
///
/// \param [in] f_hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnBootMicroLogResponse(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_len;
  std::string l_artifactFilePathAndName;

  m_errorEventTimers[eSM_ERROR_EVENT_TIMER_BOOT_MICRO_LOG_RSPN].Stop();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  INTERFACEUNIFIEDLOG_RECEIVED_FROM(f_hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", FrameworkunifiedGetMsgSrc(f_hApp));

  if (0 == strcmp(TIMER_SERVICE_NAME, FrameworkunifiedGetMsgSrc(f_hApp))) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
        " Info. Timer expired while waiting for the boot micro log.");
  } else if (0 == (l_len = FrameworkunifiedGetMsgLength(f_hApp))) {  // LCOV_EXCL_BR_LINE 200:restricted by ss_sm_client
    // LCOV_EXCL_START 200:restricted by ss_sm_client
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: Invalid log response received.  Length cannot be 0.");
    // LCOV_EXCL_STOP
  } else {
    char l_buf[l_len];  // NOLINT
    l_eStatus = FrameworkunifiedGetMsgDataOfSize(f_hApp, l_buf, static_cast<UI_32>(sizeof(l_buf)), eSMRRelease);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200:NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
      LOG_ERROR("FrameworkunifiedGetMsgDataOfSize()");  // LCOV_EXCL_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    } else {
      l_artifactFilePathAndName = "/tmp/bootmicro.log";
      l_len = static_cast<UI_32>(strlen((const char *) l_buf));
      std::ofstream l_stream(l_artifactFilePathAndName.c_str());  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      l_stream.write(l_buf, l_len);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      l_stream.close();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    }
  }

  l_eStatus = SendLogArtifactResponseToLogger(f_hApp, m_requestedArtifactId,
      l_artifactFilePathAndName);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if (l_eStatus != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: SendLogArtifactResponseToLogger(Artifact '%d', '%s') "
            "errored: %d/'%s'", m_requestedArtifactId,
        l_artifactFilePathAndName.c_str(), l_eStatus,
        GetStr(l_eStatus).c_str());
  }

  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnErrorEventBootMicroLogResponseTimeout
/// Called when the boot micro log request timer expires (e.g. no response).
///
/// \param [in] f_hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnErrorEventBootMicroLogResponseTimeout(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  std::string l_artifactFilePathAndName = "";

  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
      " Error: Boot micro log response timed out. Sending empty artifact response to SSL.");

  l_eStatus = SendLogArtifactResponseToLogger(f_hApp, m_requestedArtifactId,
      l_artifactFilePathAndName);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if (l_eStatus != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: SendLogArtifactResponseToLogger(Artifact '%d', '%s') "
            "errored: %d/'%s'", m_requestedArtifactId,
        l_artifactFilePathAndName.c_str(), l_eStatus,
        GetStr(l_eStatus).c_str());
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnObtainSystemmanagerSystemDataCsv
/// Obtain system content, write file to disk, and send filename to SL.
///
/// \param [in] f_hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnObtainSystemmanagerSystemDataCsv(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  // std::string   l_artifactFilePathAndName = "/tmp/frameworkunified_systemdata.csv";
  // std::ofstream   l_stream(l_artifactFilePathAndName.c_str());
  std::stringstream l_stream;
  char *l_pSignalName;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(f_hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  // Output version info
  {
    // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
    CSSVer ver;
    l_stream << "********** PACKAGE VERSIONS **********\n";
    l_stream << left;
    l_stream << setw(16) << "PACKAGE" << setw(24) << "VERSION" << setw(10)
        << "DATE" << endl;
    for (SSVerPkgListIter ite = ver.begin(); ite != ver.end(); ite++) {
      l_stream << setw(16) << ite->first;
      l_stream << setw(24) << ite->second.version;
      l_stream << setw(10) << ite->second.date << std::endl;
    }
    // LCOV_EXCL_BR_STOP
    FRAMEWORKUNIFIEDLOG(ZONE_SYSLOG, __FUNCTION__, "\n%s", l_stream.str().c_str());
    l_stream.str("");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    l_stream.clear();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  // LCOV_EXCL_BR_START 11:unexpected branch  // NOLINT(whitespace/line_length)
  l_stream << "********** Error Description Start ***********" << endl;
  l_stream << "Error Event Type,"
      << GetStr(m_errorEventCurrentIter->m_eventType) << endl;
  l_stream << m_errorEventCurrentIter->m_eventEnqueueTimeStamp.c_str() << endl;
  l_stream << "MessageStr,"
      << m_errorEventCurrentIter->m_loggingInfo.messageStr.c_str() << endl;
  // LCOV_EXCL_BR_STOP

  {
    ROM_AccessIf rom_access;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    UI_32 l_ErrLogCount;

    rom_access.Initialize();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    if (eFrameworkunifiedStatusOK != GetErrLogCount(rom_access, l_ErrLogCount)) {  // LCOV_EXCL_BR_LINE 200:will not be this case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: GetErrLogCount()");  // LCOV_EXCL_LINE 200:will not be this case
    } else {
      l_stream << "ErrLogCount," << l_ErrLogCount << "/"
          << SS_SM_ERR_LOGGING_LIMIT << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    }
  }

  switch (m_errorEventCurrentIter->m_eventType) {
  case eErrorEventTypeProcessCrash:
    l_stream << "Crash Failure Binary Name,"
        << m_errorEventCurrentIter->m_loggingInfo.binaryFileName << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    l_stream << "Module PID," << m_errorEventCurrentIter->m_loggingInfo.pid << endl;
    l_stream << "Exit Value,"
        << m_errorEventCurrentIter->m_loggingInfo.exitValue << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    l_pSignalName = strsignal(
        m_errorEventCurrentIter->m_loggingInfo.signalNumber);
    if (NULL != l_pSignalName) {
      l_stream << "Exit Signal,"
          << m_errorEventCurrentIter->m_loggingInfo.signalNumber  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
          << "," << l_pSignalName << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    }

    break;

  case eErrorEventTypeProcessExit:
    l_stream << "Exit Binary Name,"
        << m_errorEventCurrentIter->m_loggingInfo.binaryFileName << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    l_stream << "Module PID," << m_errorEventCurrentIter->m_loggingInfo.pid << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    l_stream << "Exit Value,"
        << m_errorEventCurrentIter->m_loggingInfo.exitValue << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    break;

  case eErrorEventTypeHeartBeatFailure:
    l_stream << "HB Failure Module Queue Name,"
        << m_errorEventCurrentIter->m_moduleQueueName.c_str() << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    l_stream << "***************** HB Information Start *******************" << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    l_stream << "Entire State," << m_HBReport.eEntireState << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    l_stream << "Module Name,HB State,Retry Count" << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    for (UI_32 i = 0; i < m_HBReport.nNumOfModules; i++) {
      if (0 != m_HBReport.tModuleList[i].HeartBeatRetryCount) {
        l_stream << m_HBReport.tModuleList[i].ProcQueueName << ","  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
            << m_HBReport.tModuleList[i].ProcHBState << ","  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
            << m_HBReport.tModuleList[i].HeartBeatRetryCount  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
            << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      }
    }
    l_stream << "***************** HB Information End *******************" << endl;
    break;

  case eErrorEventTypeSystemLowMemory:
    l_stream << "Free Memory Available (Byte)," << m_FreeMemAvailable << endl;
    break;

  case eErrorEventTypeBootMicroReset:
    l_stream << "Boot Micro Reset Reason," << m_BootMicroResetReason << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    break;

  case eErrorEventTypeModConnFailed:
    l_stream << "Failed Module Queue Name,"
        << m_errorEventCurrentIter->m_moduleQueueName.c_str() << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    break;

  case eErrorEventTypeStartRespFailed:
    l_stream << "Failed Module Queue Name,"
        << m_errorEventCurrentIter->m_moduleQueueName.c_str() << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    break;

       // No additional event specific log information to add.
  case eErrorEventTypeUserInvokedCollectAllLogs:
  case eErrorEventTypeUserInvokedCollectScreenShot:
  case eErrorEventTypeUserInvokedCollectInterfaceunifiedLogs:
  case eErrorEventTypeUserInvokedUserForceReset:
  case eErrorEventTypeUserInvokedCollectDevLogs:
  default:
    break;
  }

  {
    std::ifstream finNum("/proc/sys/vm/nr_oom_kill_process");
    std::string strNum;
    if (finNum && std::getline(finNum, strNum)) {  // LCOV_EXCL_BR_LINE 200:will not be this case
      // LCOV_EXCL_START 200:will not be this case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
      char *endptr = NULL;
      long lNum = strtol(strNum.c_str(), &endptr, 10);  // NOLINT
      if (('\0' != *endptr) || (lNum < 0)) {
        SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      } else {
        l_stream << "nr_oom_kill_process," << strNum.c_str() << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        if (0 != lNum) {
          std::ifstream finLast("/proc/sys/vm/last_oom_kill_victim");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
          std::string strLast;
          if (finLast && std::getline(finLast, strLast)) {
            l_stream << "last_oom_kill_victim," << strLast.c_str() << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
          }
        }
      }
    }
    // LCOV_EXCL_STOP
  }

  l_stream << "********** Error Description End ***********" << endl << endl;
  FRAMEWORKUNIFIEDLOG(ZONE_SYSLOG, __FUNCTION__, "\n%s", l_stream.str().c_str());

  fprintf(stderr, "SS_SysManager/\n%s", l_stream.str().c_str());  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  l_stream.str("");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  l_stream.clear();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  l_stream << "********** Variant Code Start ***********" << endl;
  if (NULL != m_pVarCodeStr) {
    l_stream << m_pVarCodeStr << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  } else {
    l_stream << "Variant coding not available." << endl;
  }
  l_stream << "********** Variant Code End ***********" << endl << endl;
  FRAMEWORKUNIFIEDLOG(ZONE_SYSLOG, __FUNCTION__, "\n%s", l_stream.str().c_str());
  l_stream.str("");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  l_stream.clear();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  l_stream << "********** File System Information Start ***********" << endl;
  l_stream << "== mounts info start==" << endl;
  {
    std::ifstream fin("/proc/mounts");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    std::string line;
    while (fin && std::getline(fin, line)) {
      l_stream << line << endl;
    }
  }
  l_stream << "== mounts info end ==" << endl;
  FRAMEWORKUNIFIEDLOG(ZONE_SYSLOG, __FUNCTION__, "\n%s", l_stream.str().c_str());
  l_stream.str("");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  l_stream.clear();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  l_stream << "== DF info start==" << endl;
  {
    char* const argv[] = { const_cast<char*>("/bin/df"), const_cast<char*>("-a"), static_cast<char*>(NULL), };
    SS_ASERT(0 == getExecedString(argv, l_stream));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }
  l_stream << "== DF info end==" << endl;

  FRAMEWORKUNIFIEDLOG(ZONE_SYSLOG, __FUNCTION__, "\n%s", l_stream.str().c_str());
  l_stream.str("");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  l_stream.clear();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

#if 0   // Need to install lsblk if requested so that it is not installed
  l_stream << "== lsblk info start==" << endl;
  {
    char* const argv[] = {
      const_cast<char*>("/usr/debug/bin/lsblk"),
      const_cast<char*>("-o"),
      const_cast<char*>("NAME,KNAME,MAJ:MIN,FSTYPE,PARTLABEL,MODEL,SERIAL,REV,VENDOR"),
      const_cast<char*>(NULL),
    };
    SS_ASERT(0 == getExecedString(argv, l_stream));
  }
  l_stream << "== lsblk info end==" << endl;
  FRAMEWORKUNIFIEDLOG(ZONE_SYSLOG, __FUNCTION__, "\n%s", l_stream.str().c_str());
  l_stream.str("");
  l_stream.clear();
#endif

  l_stream << "== proc info start==" << endl;
  {
    char* const argv[] = { const_cast<char*>("/bin/ps"), const_cast<char*>("auxc"),
        const_cast<char*>("-L"), static_cast<char*>(NULL), };
    SS_ASERT(0 == getExecedString(argv, l_stream));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }
  l_stream << "== proc info end==" << endl;
  {
    // Output is splitted so that the FRAMEWORKUNIFIEDLOG can not write data over 4K Bytes at a time
    std::list<std::string> strList;
    boost::split(strList, static_cast<const std::string>(l_stream.str()),
        boost::is_any_of("\n"));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    int ii = 0;
    std::stringstream ss;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    for (std::list<std::string>::iterator ite = strList.begin();
        ite != strList.end(); ite++) {
      ss << *ite << endl;
      ii++;
      if (ii > 20) {
        FRAMEWORKUNIFIEDLOG(ZONE_SYSLOG, __FUNCTION__, "\n%s", ss.str().c_str());
        ss.str("");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        ss.clear();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        ii = 0;
      }
    }
    FRAMEWORKUNIFIEDLOG(ZONE_SYSLOG, __FUNCTION__, "\n%s", ss.str().c_str());
  }
  l_stream.str("");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  l_stream.clear();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  l_eStatus = SendLogArtifactResponseToLogger(f_hApp, m_requestedArtifactId, "");  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if (l_eStatus != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: SendLogArtifactResponseToLogger(Artifact '%d') "
            "errored: %d/'%s'", m_requestedArtifactId, l_eStatus,
        GetStr(l_eStatus).c_str());
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnObtainShowMemTxt
/// Obtain showmem content, write file to disk, and send filename to SL.
///
/// \param [in] f_hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnObtainShowMemTxt(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(f_hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

  std::string l_artifactFilePathAndName = "/tmp/showmem.txt";
  {
    std::ofstream fo(l_artifactFilePathAndName.c_str());  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    saveProcsMemInfo(fo);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  }

  l_eStatus = SendLogArtifactResponseToLogger(f_hApp, m_requestedArtifactId,
                                              l_artifactFilePathAndName);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if (l_eStatus != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: SendLogArtifactResponseToLogger(Artifact '%d', '%s') "
            "errored: %d/'%s'", m_requestedArtifactId,
        l_artifactFilePathAndName.c_str(), l_eStatus,
        GetStr(l_eStatus).c_str());
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnErrorEventCoreFilePollTimeout
/// Called periodically to verify whether the process core file has completed
/// being written to disk. When complete, this function sends an artifact
/// response to SSL.
///
/// \param [in] f_hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnErrorEventCoreFilePollTimeout(HANDLE f_hApp) {  // LCOV_EXCL_START 8:dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  BOOL l_sendResponse = FALSE;
  std::string l_artifactFilePathAndName = "";
  std::ostringstream l_coreFilePathAndName;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

//  struct stat l_fileInfo;
  struct stat64 l_fileInfo;
  int l_result;
  off_t l_coreFileSize;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(f_hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  l_coreFilePathAndName << "/debug/"
      << m_errorEventCurrentIter->m_loggingInfo.binaryFileName << ".core.gz";  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)


//  l_result = stat(l_coreFilePathAndName.str().c_str(), &l_fileInfo);
  l_result = stat64(l_coreFilePathAndName.str().c_str(), &l_fileInfo);
  if (l_result == 0) {
    l_coreFileSize = l_fileInfo.st_size;
    if ((l_coreFileSize > m_coreFileSizeBytes) ||  // Core file grew from last read.
        (l_coreFileSize == 0)) {  // Core file not yet updated externally.
      m_coreFileSizeBytes = l_coreFileSize;  // Wait until file stops growing.
      bool result =
          m_errorEventTimers[eSM_ERROR_EVENT_TIMER_CORE_FILE_POLL].Start(  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
              0, SS_ERROR_EVENT_CORE_FILE_POLL_TO_MS, 0, 0);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      if (FALSE == result) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
            " Error: Failed to start timer SS_ERROR_EVENT_CORE_FILE_POLL_TO_MS.");
        l_sendResponse = TRUE;
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
            " Info. Core file: %s, size: %ld still growing.",
            l_coreFilePathAndName.str().c_str(),
            m_coreFileSizeBytes);

        l_sendResponse = FALSE;
      }
    } else {  // File has stopped growing.
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
          " Info. Core file: %s, size: %ld write complete. Sending artifact response.",
          l_coreFilePathAndName.str().c_str(), m_coreFileSizeBytes);

      l_artifactFilePathAndName = l_coreFilePathAndName.str();
      l_sendResponse = TRUE;
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Info. Core file: %s, unable to determine size.  Sending empty artifact response to SSL.",
        l_coreFilePathAndName.str().c_str());

    l_sendResponse = TRUE;
  }

  if (TRUE == l_sendResponse) {
    l_eStatus = SendLogArtifactResponseToLogger(f_hApp,  // Error getting file size. Send empty path to SSL.
    m_requestedArtifactId, l_artifactFilePathAndName);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

    if (l_eStatus != eFrameworkunifiedStatusOK) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: SendLogArtifactResponseToLogger(Artifact '%d', '%s') "
              "errored: %d/'%s'", m_requestedArtifactId,
          l_artifactFilePathAndName.c_str(), l_eStatus,
          GetStr(l_eStatus).c_str());
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnObtainDebugDumpLog
/// Obtain debug dump content, write file to disk, and send filename to SL.
/// See OnModuleDebugDumpResponse().
///
/// \param [in] f_hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnObtainDebugDumpLog(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(f_hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  ModuleLaunchListIter l_moduleIter;

  m_NbrDebugDumpRspnRecv = 0;

  for (GroupLaunchMapIter l_GroupIter = m_MapProclaunchGrps.begin();
      l_GroupIter != m_MapProclaunchGrps.end(); l_GroupIter++) {
    for (l_moduleIter = l_GroupIter->second.modules.begin();
        l_moduleIter != l_GroupIter->second.modules.end();
        l_moduleIter++) {
      const BOOL isModuleConnected = l_moduleIter->IsModuleConnected();
      if ((SERVICE_NS_NPP != l_moduleIter->name) &&  // NPP is a special case and does NOT session connect with SM.
          (TRUE == isModuleConnected)) {
        l_eStatus = FrameworkunifiedSendMsg(l_moduleIter->hsession, SS_SM_DEBUG_DUMP,
            0, NULL);

        if (eFrameworkunifiedStatusOK == l_eStatus) {  // LCOV_EXCL_BR_LINE 200:NSFW error case
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
              " Info. FrameworkunifiedSendMsg(SS_SM_DEBUG_DUMP) to %s succeeded.", // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
              l_moduleIter->name.c_str());  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
        } else {
          // LCOV_EXCL_START 200:NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
              " Error: FrameworkunifiedSendMsg(%p, SS_SM_DEBUG_DUMP) to %s errored: %d/'%s'",
              (void *) l_moduleIter->hsession,
              l_moduleIter->name.c_str(), l_eStatus,
              GetStr(l_eStatus).c_str());
          // LCOV_EXCL_STOP
        }
      }
    }
  }

  // Call debug dump handler for SM.
  l_eStatus = OnSystemManagerDebugDump(f_hApp);
  LOG_STATUS(l_eStatus, "OnSystemManagerDebugDump()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  // Refresh debug dump timeout timer after each debug dump response received.
  // Non AGL framework modules will have the remaining time after the last
  // received response to complete their debug dump.
  bool result =
      m_errorEventTimers[eSM_ERROR_EVENT_TIMER_DEBUG_DUMP_RSPN].Start(
          SS_ERROR_EVENT_DEBUG_DUMP_RSPN_TO_SEC, 0, 0, 0);
  if (FALSE == result) {  // LCOV_EXCL_BR_LINE 200:will not be this case
    // LCOV_EXCL_START 200:will not be this case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: Failed to start timer eSM_TIMER_DEBUG_DUMP_RSPN_MONITOR.");
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnDebugDumpResponseReceived
/// Debug Dump complete Response handler.
/// See OnDebugDumpCompleteTimeout().
///
/// \param [in] f_hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnDebugDumpResponseReceived(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ModuleLaunchListIter l_ModuleListIter;
  UI_32 l_len;
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(f_hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)

  std::string l_moduleName = FrameworkunifiedGetMsgSrc(f_hApp);
  FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "from %s", l_moduleName.c_str());

  SetCmdHist("SS_SM_DEBUG_DUMP_RSPN", m_SMCmdHist, m_SMHistIter, FrameworkunifiedGetMsgSrc(f_hApp));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  // SM is not launched by SM and is therefore not in the process list.
  // SM does however use the same debug dump mechanism as other processes.
  if (l_moduleName != SERVICE_SYSMANAGER) {
    l_eStatus = GetModuleIterator(l_moduleName.c_str(), l_ModuleListIter);
    if (eFrameworkunifiedStatusOK != l_eStatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: Module %s not found in Group Launch Map", l_moduleName.c_str());
    } else {
      l_ModuleListIter->SetModuleDebugDumpState(MODULE_DEBUG_DUMP_STATE_RESPONSE_RECEIVED);
    }
  }

  if (0 == (l_len = FrameworkunifiedGetMsgLength(f_hApp))) {  // LCOV_EXCL_BR_LINE 200:restricted by ss_sm_client
    // LCOV_EXCL_START 200:restricted by ss_sm_client
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
    l_eStatus = eFrameworkunifiedStatusInvldBufSize;
    LOG_ERROR("0 == FrameworkunifiedGetMsgLength(f_hApp)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    // LCOV_EXCL_STOP
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
        " Received debug dump response from %s, size: %d bytes. "
            "Total number of responses received: %d",
        l_moduleName.c_str(), l_len, m_NbrDebugDumpRspnRecv);

    UI_8 l_buf[l_len];  // NOLINT
    l_eStatus = FrameworkunifiedGetMsgDataOfSize(f_hApp, l_buf, static_cast<UI_32>(sizeof(l_buf)),
        eSMRRelease);
    if (eFrameworkunifiedStatusOK != l_eStatus) {
      LOG_ERROR("FrameworkunifiedGetMsgDataOfSize()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    } else {
      ios_base::openmode l_mode;
      if (0 == m_NbrDebugDumpRspnRecv) {
        // Create new file.
        l_mode = std::ios_base::trunc;
      } else {
        // Append existing file.
        l_mode = std::ios_base::app;
      }

      std::ofstream l_stream("/tmp/systemmanager_debugdump.log", l_mode);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      l_stream << l_buf << endl;  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      l_stream.close();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    }

    m_NbrDebugDumpRspnRecv++;

    // Refresh debug dump timeout timer after each debug dump response received.
    // Non AGL framework modules will have the remaining time after the last
    // received response to complete their debug dump.
    bool result =
        m_errorEventTimers[eSM_ERROR_EVENT_TIMER_DEBUG_DUMP_RSPN].Start(  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
            SS_ERROR_EVENT_DEBUG_DUMP_RSPN_TO_SEC, 0, 0, 0);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
    if (FALSE == result) {  // LCOV_EXCL_BR_LINE 200:will not be this case
      // LCOV_EXCL_START 200:will not be this case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200:test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
          " Error: Failed to start timer eSM_TIMER_DEBUG_DUMP_RSPN_MONITOR.");
      l_eStatus = OnDebugDumpCompleteTimeout(f_hApp);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
      LOG_ERROR_REC_HIST_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
          "OnDebugDumpCompleteTimeout()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_system_manager.h  // NOLINT(whitespace/line_length)
      // LCOV_EXCL_STOP
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup OnDebugDumpCompleteTimeout
/// Called after the last debug dump message is received, or when the debug
/// dump monitor timer expires.
///
/// \param [in] f_hApp - Application handle.
///
/// \return EFrameworkunifiedStatus
/// Success ==> eFrameworkunifiedStatusOK
/// Failure ==> Other values
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CSystemManager::OnDebugDumpCompleteTimeout(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  std::string l_artifactFilePathAndName = "/tmp/systemmanager_debugdump.log";
  INTERFACEUNIFIEDLOG_RECEIVED_FROM(f_hApp);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  m_errorEventTimers[eSM_ERROR_EVENT_TIMER_DEBUG_DUMP_RSPN].Stop();  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Received '%d' Debug Dump responses.",
      m_NbrDebugDumpRspnRecv);

  l_eStatus = SendLogArtifactResponseToLogger(f_hApp, m_requestedArtifactId,
      l_artifactFilePathAndName);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

  if (l_eStatus != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
        " Error: SendLogArtifactResponseToLogger(Artifact '%d', '%s') "
            "errored: %d/'%s'", m_requestedArtifactId,
        l_artifactFilePathAndName.c_str(), l_eStatus,
        GetStr(l_eStatus).c_str());
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

