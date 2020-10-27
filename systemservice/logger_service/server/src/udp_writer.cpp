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
#include "readerWriter/writer/udp_writer.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#ifdef AGL_STUB
#include <other_service/PosixBasedOS001ClockCycleApi.h>
#else
#include <sys/syspage.h>
#include <sys/neutrino.h>
#endif
#include <native_service/ns_logger_if.h>
#include <string>
#include <algorithm>

#define TIME_UNTIL_UDP_START   (UI_32)25

namespace ReaderWriter {

CUdpWriter::CUdpWriter()
    : m_SocketHandle(-1),
      m_IPAddress(""),
      m_index(0),
      m_port(REMOTE_IP_PORT),
      m_InitialTimeElapsed(FALSE) {
  std::memset(&m_oSAInfo, 0, sizeof(sockaddr_in));
  std::memset(&m_buffer[0], 0, UDP_CACHED_BLOCK_SIZE);
}

CUdpWriter::~CUdpWriter() {
  if (m_SocketHandle != -1) {
    this->Close();
  }
}

EFrameworkunifiedStatus CUdpWriter::Initialize(CLoggerCfg* f_pLoggerCfg, std::string f_Name1,
                                  UI_32 f_size1, std::string f_Name2,
                                  UI_32 f_size2) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  this->m_pLoggerCfg = f_pLoggerCfg;
  this->m_IPAddress = f_Name1;
  if (NULL == f_pLoggerCfg) { // LCOV_EXCL_BR_LINE 200: f_pLoggerCfg can not be null
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;  // LCOV_EXCL_LINE 200: f_pLoggerCfg can not be null
  } else if ((f_pLoggerCfg->IsUDPLoggingEnabled())) {
    if (m_IPAddress.length() > 0) {
      this->m_port = f_size1;
      l_eStatus = this->Open();
      LOG_STATUS_IF_ERRORED(
          l_eStatus,
          "this->Open() failed. UDP logging disabled. Enable via Engineering Menu.");
      // Continue without UDP Logging;
      l_eStatus = eFrameworkunifiedStatusOK;
    } else {
      l_eStatus = eFrameworkunifiedStatusInvldParam;
    }
  } else {
    this->m_SocketHandle = -1;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %d", l_eStatus);
  return (l_eStatus);
}

BOOL CUdpWriter::IsOpen(void) {
  BOOL l_ret = (-1 != m_SocketHandle) ? TRUE : FALSE;
  if (TRUE == l_ret) {
#ifdef AGL_STUB
    UI_32 l_elapsed = ((UI_32)((F_64)ClockCycle()/1000000));
#else
    UI_32 l_elapsed = ((UI_32) ((F_64) ClockCycle()
        / (F_64) (SYSPAGE_ENTRY(qtime)->cycles_per_sec)));
#endif
    // check if 20 Seconds have elapsed since startup
    if (TIME_UNTIL_UDP_START <= l_elapsed) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             "%d seconds elapsed. Starting UDP logging", l_elapsed);
      m_InitialTimeElapsed = TRUE;
      l_ret = TRUE;

    } else {
      FRAMEWORKUNIFIEDLOG(
          ZONE_INFO,
          __FUNCTION__,
          "Time before starting UDP not yet elapsed. Waiting for %d more seconds",
          (TIME_UNTIL_UDP_START - l_elapsed));
      usleep((TIME_UNTIL_UDP_START - l_elapsed)*1000);
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Starting UDP now!");
    }
  }
  return l_ret;
}

EFrameworkunifiedStatus CUdpWriter::Open() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  if ((m_IPAddress.length() > 0)) {
    int l_SocketOption = 1;
    // Set the domain using the classes domain value.
    m_oSAInfo.sin_family = AF_INET;
    m_oSAInfo.sin_addr.s_addr = inet_addr(OWN_ADDRESS);  // m_IPAddress.c_str()); // "192.168.1.2");//htonl(INADDR_ANY);
    m_oSAInfo.sin_port = htons(this->m_port);
    if (FALSE == this->m_pLoggerCfg->IsUDPLoggingEnabled()) {
      this->Close();
      l_eStatus = eFrameworkunifiedStatusFail;
    } else if (this->m_SocketHandle != -1) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             " Info. socket already opened. No action required");
      l_eStatus = eFrameworkunifiedStatusOK;
    } else if (-1 == (this->m_SocketHandle = socket(AF_INET, SOCK_DGRAM, 0))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. socket(AF_INET, SOCK_DGRAM, 0) returned -1. errno: %s",
             strerror(errno));
      errno = 0;
      l_eStatus = eFrameworkunifiedStatusDbExecuteFail;
    } else if (setsockopt(m_SocketHandle, SOL_SOCKET, SO_REUSEADDR,
                          &l_SocketOption, sizeof(l_SocketOption)) == -1) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "setsockopt(SO_REUSEADDR) failed.errno: %s", strerror(errno));
      errno = 0;
      l_eStatus = eFrameworkunifiedStatusDbResultError;
    }
#ifdef AGL_STUB
    else if (setsockopt(m_SocketHandle, SOL_SOCKET, SO_REUSEADDR ,  // LCOV_EXCL_BR_LINE 6: the same calls will get the same return  // NOLINT (readability/braces)
            &l_SocketOption, sizeof(l_SocketOption)) == -1)
#else
    else if (setsockopt(m_SocketHandle, SOL_SOCKET, SO_REUSEPORT,  // NOLINT (readability/braces)
                        &l_SocketOption, sizeof(l_SocketOption)) == -1)
#endif
        {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "setsockopt(SO_REUSEPORT) failed. errno: %s", strerror(errno));
      errno = 0;
      l_eStatus = eFrameworkunifiedStatusDbResultError;
    } else if (setsockopt(m_SocketHandle, SOL_SOCKET, SO_DONTROUTE,
                          &l_SocketOption, sizeof(l_SocketOption)) == -1) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "setsockopt(SO_DONTROUTE) failed. errno: %s", strerror(errno));
      errno = 0;
      l_eStatus = eFrameworkunifiedStatusDbResultError;
    } else if (INADDR_NONE == m_oSAInfo.sin_addr.s_addr) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Invalid local address.");
      l_eStatus = eFrameworkunifiedStatusDbRecNotFound;
    } else if (0
        != bind(this->m_SocketHandle, (const struct sockaddr *) &m_oSAInfo,
                sizeof(m_oSAInfo))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. bind() returned not zero. errno: %s", strerror(errno));
      errno = 0;
      l_eStatus = eFrameworkunifiedStatusSemUnlockFail;
    } else {
      m_oSAInfo.sin_addr.s_addr = inet_addr(m_IPAddress.c_str());
      if (INADDR_NONE == m_oSAInfo.sin_addr.s_addr) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Invalid target address.");
        l_eStatus = eFrameworkunifiedStatusFault;
      } else {
        l_eStatus = eFrameworkunifiedStatusOK;
      }
    }

    if (eFrameworkunifiedStatusOK != l_eStatus) {
      this->Close();
      m_SocketHandle = -1;
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "- : %d", l_eStatus);
  return (l_eStatus);
}

EFrameworkunifiedStatus CUdpWriter::WriteData(UI_8 *f_data, UI_32 f_length,
                                 SI_32 &f_bytesWritten) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_8 l_retry = SEND_RETRY_VALUE;
  if (this->m_SocketHandle == -1) {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  } else {
    while ((0 != l_retry)
        && ((f_bytesWritten >= 0) && ((UI_32) f_bytesWritten != f_length))) {
      SI_32 l_bytesWritten = (SI_32)(sendto(m_SocketHandle, &f_data[f_bytesWritten],
                                    f_length - f_bytesWritten, 0,
                                    (const struct sockaddr *) &m_oSAInfo,
                                    sizeof(m_oSAInfo)));
      f_bytesWritten += l_bytesWritten;
      l_eStatus = (l_bytesWritten >= 0) ? eFrameworkunifiedStatusOK : eFrameworkunifiedStatusFail;
      if (eFrameworkunifiedStatusOK != l_eStatus) {
        FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "l_retry: %d, errno: %s", l_retry,
               strerror(errno));
        errno = 0;
        l_retry--;
        usleep(100);
      } else {
        l_retry = SEND_RETRY_VALUE;
      }
    }
  }
  return (l_eStatus);
}

EFrameworkunifiedStatus CUdpWriter::Write(UI_8* f_data, UI_32 f_length,
                             SI_32& f_bytesWritten) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  f_bytesWritten = 0;
  if ((f_data == NULL)) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  } else if ((-1 == m_SocketHandle)) {
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  if (eFrameworkunifiedStatusOK == l_eStatus) {
    if (f_length > UDP_MAX_LENGTH) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Possible memory corruption.");
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Length(%d) bigger than cache. ",
             f_length);
    } else {
      UI_32 l_remainingIndex = 0;

      while (l_remainingIndex != f_length) {
        UI_32 l_offset = (UDP_CACHED_BLOCK_SIZE - this->m_index);
        l_offset = std::min(l_offset, (f_length - l_remainingIndex));
        (void) std::memcpy(&this->m_buffer[this->m_index],
                           &f_data[l_remainingIndex], l_offset);
        l_remainingIndex += l_offset;
        this->m_index += l_offset;
        if (this->m_index == UDP_CACHED_BLOCK_SIZE) {
          (void) this->FlushCache();
          this->m_index = 0;
        }
      }
      f_bytesWritten = f_length;
    }
  }
  return (l_eStatus);
}
void CUdpWriter::Close() {
  if (m_SocketHandle != -1) {
    (void) close(m_SocketHandle);
    m_SocketHandle = -1;
  }
}

EFrameworkunifiedStatus CUdpWriter::UpdateLoggingParameters(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (this->m_pLoggerCfg != NULL) {
    BOOL l_enabled = this->m_pLoggerCfg->IsUDPLoggingEnabled();
    if ((TRUE == l_enabled) && (m_SocketHandle == -1)) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Info. Opening UDP.");
      l_eStatus = this->Open();
    } else if (((FALSE == l_enabled) && (m_SocketHandle != -1))) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Info. Closing UDP.");
      l_eStatus = eFrameworkunifiedStatusOK;
      this->Close();
    } else {
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "- :%d", l_eStatus);
  return (l_eStatus);
}

EFrameworkunifiedStatus CUdpWriter::FlushCache(void) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  SI_32 l_written = 0;
  SI_32 l_writtenTotal = 0;
  while (((l_writtenTotal >= 0) && ((UI_32) l_writtenTotal < this->m_index))
      && (eFrameworkunifiedStatusOK == l_eStatus)) {
    l_eStatus = this->WriteData(&this->m_buffer[l_writtenTotal],
                                this->m_index - l_writtenTotal, l_written);
    l_writtenTotal += l_written;
    l_written = 0;
  }
  this->m_index = 0;
  return l_eStatus;
}
}  // namespace ReaderWriter
