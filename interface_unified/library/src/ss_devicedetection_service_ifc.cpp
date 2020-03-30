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
/// \ingroup  tag_SystemServices_DeviceDetection
/// \brief    This file contains service interface functions.
///
///////////////////////////////////////////////////////////////////////////////
#include "system_service/ss_devicedetection_service_ifc.h"
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>
#include <other_service/VP_GetEnv.h>
#include <string.h>

#ifdef AGL_STUB
#else
#include "dcmd_sim_mmcsd.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#ifdef AGL_STUB
#else
#include <sys/dcmd_cam.h>
#endif
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include <asm-generic/param.h>

#include "system_service/ss_services.h"
#include "system_service/ss_devicedetection_service_protocol.h"
#include "system_service/ss_devicedetection_service_protocol_local.h"
#include "system_service/ss_devicedetection_service_notifications.h"
#include "system_service/ss_devicedetection_service_local.h"
#include "system_service/ss_devicedetection_service_types_local.h"
#include "ss_devicedetection_service_if_interfaceunifiedlog.h"

DeviceDetectionServiceIf::DeviceDetectionServiceIf()
    : m_hApp(NULL),
      m_hService(NULL),
      m_hSession(NULL) {
}

DeviceDetectionServiceIf::~DeviceDetectionServiceIf() {
  CloseSessionRequest();  // if the session is still connected
  if ((NULL != m_hApp) && (NULL != m_hService)) {
    FrameworkunifiedCloseService(m_hApp, m_hService);
  }
  m_hService = NULL;
  m_hSession = NULL;
  m_hApp = NULL;
}  // LCOV_EXCL_BR_LINE 10:Because destructor

EFrameworkunifiedStatus DeviceDetectionServiceIf::NotifyOnDeviceDetectionAvailability(
    CbFuncPtr f_pCallBackFn) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;
  if (NULL != m_hApp && NULL != f_pCallBackFn) {
    // Subscriptions
    FrameworkunifiedNotificationCallbackHandler
    g_aryDeviceDetect_Notif_Cbs[] = {
      // Notifications name,                Call back function
      { NTFY_SS_Device_Detection_Service_Availability, f_pCallBackFn},
    };

    if (0 != strcmp(SS_DEV_DETECT_SRV, FrameworkunifiedGetAppName(m_hApp))) {
      // Subscribe and attach call backs to notifications
      if (eFrameworkunifiedStatusOK
          != (eStatus = FrameworkunifiedSubscribeNotificationsWithCallback(
              m_hApp, g_aryDeviceDetect_Notif_Cbs,
              _countof(g_aryDeviceDetect_Notif_Cbs)))) {
        FRAMEWORKUNIFIEDLOG(
            ZONE_ERR,
            __FUNCTION__,
            "Error: FrameworkunifiedAttachNotificationCallbacksToDispatcher Failed Status:0x%x ",
            eStatus);
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::DetachDeviceDetectionAvailabilityCb() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;
  if (NULL != m_hApp) {
    if (0 != strcmp(SS_DEV_DETECT_SRV, FrameworkunifiedGetAppName(m_hApp))) {
      // Unsubscribe and detach call backs to notifications
      if (eFrameworkunifiedStatusOK != (eStatus =
          FrameworkunifiedUnsubscribeNotificationWithCallback(m_hApp, NTFY_SS_Device_Detection_Service_Availability))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: InterfaceunifiedDetachNotificationCallback Failed Status:0x%x ",
               eStatus);
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::NotifyOnOpenSessionAck(
    CbFuncPtr f_pCallBackFn) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;
  if (NULL != m_hApp && NULL != f_pCallBackFn) {
    if (eFrameworkunifiedStatusOK
        != (eStatus = FrameworkunifiedAttachCallbackToDispatcher(m_hApp,
                                                    SS_DEV_DETECT_SRV,
                                                    PROTOCOL_OPEN_SESSION_ACK,
                                                    f_pCallBackFn))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: FrameworkunifiedAttachCallbackToDispatcher Failed Status:0x%x ",
             eStatus);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::DetachOpenSessionAckCb() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;
  if (NULL != m_hApp) {
    if (eFrameworkunifiedStatusOK
        != (eStatus = FrameworkunifiedDetachCallbackFromDispatcher(m_hApp,
        SS_DEV_DETECT_SRV,
                                                      PROTOCOL_OPEN_SESSION_ACK))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: FrameworkunifiedDetachCallbackFromDispatcher Failed Status:0x%x ",
             eStatus);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::NotifyOnCloseSessionAck(
    CbFuncPtr f_pCallBackFn) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hApp && NULL != f_pCallBackFn) {
    if (eFrameworkunifiedStatusOK
        != (eStatus = FrameworkunifiedAttachCallbackToDispatcher(m_hApp,
        SS_DEV_DETECT_SRV,
                                                    PROTOCOL_CLOSE_SESSION_ACK,
                                                    f_pCallBackFn))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: FrameworkunifiedAttachCallbackToDispatcher Failed Status:0x%x ",
             eStatus);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::DetachCloseSessionAckCb() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hApp) {
    if (eFrameworkunifiedStatusOK
        != (eStatus = FrameworkunifiedDetachCallbackFromDispatcher(
            m_hApp,
            SS_DEV_DETECT_SRV,
            PROTOCOL_CLOSE_SESSION_ACK))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: FrameworkunifiedDetachCallbackFromDispatcher Failed Status:0x%x ",
             eStatus);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::RegisterForDeviceDetectionEvent(
    SS_DeviceDetectionServerEvents f_eDevDetectEvent, CbFuncPtr f_pCallBackFn,
    PCSTR pFilepath) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;
  char l_filepathname[FILEPATH_SIZE] = { '\0' };
  int len = 0;

  if (NULL != pFilepath) {
    len = strlen(pFilepath);
    strcpy(l_filepathname, pFilepath);  // NOLINT (runtime/printf)
  }

  if (NULL != m_hSession && NULL != f_pCallBackFn) {
    if (CheckDetectEvent(f_eDevDetectEvent)) {
      if (eFrameworkunifiedStatusOK
          != (eStatus = FrameworkunifiedAttachCallbackToDispatcher(m_hApp,
          SS_DEV_DETECT_SRV,
                                                      f_eDevDetectEvent,
                                                      f_pCallBackFn, m_hSession))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: FrameworkunifiedAttachCallbackToDispatcher Failed Status:0x%x ",
               eStatus);
      }

      eStatus = FrameworkunifiedSendMsg(m_hSession, SS_REGISTER_DETECT_EVENT,
                                         sizeof(SS_DeviceDetectionServerEvents),
                                         &f_eDevDetectEvent);
    } else {
      if (eFrameworkunifiedStatusOK
          != (eStatus = FrameworkunifiedAttachCallbackToDispatcher(m_hApp,
          SS_DEV_DETECT_SRV,
                                                      f_eDevDetectEvent,
                                                      f_pCallBackFn, m_hSession))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: FrameworkunifiedAttachCallbackToDispatcher Failed Status:0x%x ",
               eStatus);
      }

      // client registers for the event
      if (eFrameworkunifiedStatusOK
          != (eStatus = FrameworkunifiedRegisterEvent(m_hSession, f_eDevDetectEvent))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: FrameworkunifiedRegisterEvents Failed Status:0x%x", eStatus);
      }

      if (NULL != pFilepath) {
        eStatus = FrameworkunifiedSendMsg(m_hSession, SS_REGISTER_FILEPATHNAME, len,
                                             l_filepathname);
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::UnRegisterForDeviceDetectionEvent(
    SS_DeviceDetectionServerEvents f_eDevDetectEvent) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (eStatus = FrameworkunifiedDetachCallbackFromDispatcher(m_hApp,
                                                      SS_DEV_DETECT_SRV,
                                                      f_eDevDetectEvent,
                                                      m_hSession))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: InterfaceunifiedDetachCallbackToDispatcher Failed Status:0x%x ", eStatus);
    }

    // client registers for the event
    if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedUnRegisterEvent(m_hSession, f_eDevDetectEvent))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: FrameworkunifiedUnRegisterEvents Failed Status:0x%x", eStatus);
    }

    if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedSendMsg(m_hSession, SS_UNREGISTER_DETECT_EVENT,
                                              sizeof(SS_DeviceDetectionServerEvents),
                                              &f_eDevDetectEvent))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "UnRegister Detect Event[0x%x] Send Error[0x%x]", f_eDevDetectEvent, eStatus);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

BOOL DeviceDetectionServiceIf::Initialize(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_eStatus = TRUE;

  if (NULL != hApp) {
    m_hApp = hApp;
  } else {
    l_eStatus = FALSE;  // eFrameworkunifiedStatusInvldHandle;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::OpenSessionRequest() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;
  if (NULL != m_hApp) {
    // if the session is there then we need to close it first!
    if (NULL != m_hService) {
      CloseSessionRequest();
      FrameworkunifiedCloseService(m_hApp, m_hService);
      m_hService = NULL;
    }
    BOOL l_bServiceAvailable = FrameworkunifiedIsServiceAvailable(m_hApp);
    if (FALSE == l_bServiceAvailable) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
             "DeviceError: Service is unavailable");
      l_eStatus = eFrameworkunifiedStatusFail;
    } else {
      // Now open the service.
      if (l_bServiceAvailable
          && (NULL != (m_hService = OpenService()))) {  // LCOV_EXCL_BR_LINE 8:Because l_bServiceAvailable is always TRUE
        // [DM: TODO] Check if we need to open a session with some data sent to Server.
        UI_32 l_pTestData = 1;
        if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedOpenSessionWithData(m_hService,
                                                   (PVOID) &l_pTestData,
                                                   sizeof(UI_32)))) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "Error: FrameworkunifiedOpenSessionWithData Failed");
        }
      } else {
        l_eStatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "Error: FrameworkunifiedOpenService Failed");
      }
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: OpenSessionRequest Failed %X",
           l_eStatus);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

HANDLE DeviceDetectionServiceIf::OpenService() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  HANDLE l_hService = NULL;

  if (NULL == (l_hService = FrameworkunifiedOpenService(m_hApp, SS_DEV_DETECT_SRV))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "Error: FrameworkunifiedOpenService : Failed to open Device Detection Service");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_hService;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::DecodeOpenSessionResponse() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hService && NULL != m_hApp) {  // LCOV_EXCL_BR_LINE 8:Because when m_hApp is NULL, m_hService is NULL
    if (NULL == (m_hSession = FrameworkunifiedGetOpenSessionHandle(m_hApp))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "Error: Error in extracting OpenSessionAck response.");
      l_eStatus = eFrameworkunifiedStatusFail;
    } else {
      l_eStatus = eFrameworkunifiedStatusOK;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Logger Session Name: %s, Session Id: %d",
             FrameworkunifiedGetSessionName(m_hSession), FrameworkunifiedGetSessionId(m_hSession));
    }
  } else {
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        "Error. m_hService: %p, m_hApp: %p is NULL. Open session handle not acquired,"
        " l_eStatus: eFrameworkunifiedStatusInvldHandle.", m_hService, m_hApp);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::CloseSessionRequest() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hService && NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedCloseSession(m_hService, m_hSession))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FrameworkunifiedCloseSession Failed");
    }
    m_hSession = NULL;  // clear our session handle
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

BOOL DeviceDetectionServiceIf::CheckDetectEvent(
    SS_DeviceDetectionServerEvents f_eDevDetectEvent) {
  BOOL chkevt = FALSE;

  switch (f_eDevDetectEvent) {
    case SS_DEV_INFO_ANY_USB_EV:
    case SS_DEV_DETECT_ANY_USB_EV:
    case SS_DEV_DETECT_ANY_SD_EV:
    case SS_DEV_DETECT_ANY_DISC_EV:
    case SS_DEV_DETECT_ANY_USB_NCM_EV:

    case SS_DEV_DETECT_ANY_USB_DCM_NCM_EV:
    case SS_DEV_DETECT_ANY_USB__CWORD57__EV:
    case SS_DEV_DETECT_ANY_USB_ACM_EV:
    case SS_DEV_DETECT_ANY_USB_DEV_MNG_EV:
    case SS_DEV_DETECT_ANY_USB_VEHICLE_CTRL_EV:
    case SS_DEV_DETECT_ANY_USB_DSRC_APP_EV:
    case SS_DEV_DETECT_ANY_USB_DSRC_CTRL_EV:
    case SS_DEV_DETECT_ANY_USB_IR_VICS_DATA_EV:
    case SS_DEV_ERR_USB_OVER_CURRENT_EV:
    case SS_DEV_DETECT_ANY_USB_MTP_EV:
    case SS_DEV_INFO_ANY_USB_NOTIFY_EV:
    case SS_DEV_DETECT_ANY_USB__CWORD57__ROLE_EV:
    case SS_DEV_INFO_SD_FORMAT_COMP_EV:
      chkevt = TRUE;
      break;
    default:
      break;
  }
  return chkevt;
}

static void ReadString(const char *file, char *buf, int len) {
  int fd;
  ssize_t r;
  char *tmp;

  if (file == NULL) {  // LCOV_EXCL_BR_LINE 8:Because file is not NULL
    // LCOV_EXCL_START  8:Because file is not NULL
    buf[0] = '\0';
    return;
    // LCOV_EXCL_STOP
  }

  fd = open(file, O_RDONLY);
  if (fd < 0) {  // LCOV_EXCL_BR_LINE 6:Because the sequence at the time of open failure cannot be passed
    // LCOV_EXCL_START 6:Because the sequence at the time of open failure cannot be passed
    buf[0] = '\0';
    return;
    // LCOV_EXCL_STOP
  }

  r = read(fd, buf, len);
  close(fd);
  if (r > 0 && r < len) {
    buf[r] = '\0';
    r--;
    /* If there is a line feed code'\n' at the end of the acquired data, replace it with '\0' */
    while (buf[r] == '\n') {
      buf[r] = '\0';
      if (r == 0)
        break;
      r--;
    }
    /* If there is a line feed code'\n' in the middle of the acquired data, replace with '\0' */
    tmp = buf;
    while ((tmp = strchr(tmp, '\n')) != NULL) {
      *tmp = ' ';
      tmp++;
    }
    return;
  } else {
    buf[0] = '\0';
  }
}

static bool isMounttedDir(char* pMountDir) {
  char buf[4096];

  if (!pMountDir) {  // LCOV_EXCL_BR_LINE 8:Because pMountDir is not NULL
    // LCOV_EXCL_START 8:Because pMountDir is not NULL
    return false;
    // LCOV_EXCL_STOP
  }

  ReadString("/proc/mounts", buf, sizeof(buf));
  if (strstr(buf, pMountDir)) {  // The mount folder is already in use as a mount point
    return true;
  }

  return false;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::MountSD(uint8_t part_num,
                                             BOOL *already_mounted) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "part_num %d", part_num);

  if (NULL == m_hSession) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "m_hSession is NULL");
    return eFrameworkunifiedStatusInvldHandle;
  }

  if (part_num > 3) {
    return eFrameworkunifiedStatusInvldParam;
  }

  char mount_dir[256];
  if (part_num != 0) {
    sprintf(mount_dir, "/mnt/mmcblk1p%d", part_num);  // NOLINT (runtime/printf)
  } else {
    sprintf(mount_dir, "/mnt/mmcblk1");  // NOLINT (runtime/printf)
  }
  if (isMounttedDir(mount_dir)) {
    *already_mounted = TRUE;
    return eFrameworkunifiedStatusOK;
  } else {
    *already_mounted = FALSE;
  }

  char filename[256];
  struct stat st;
  if (part_num != 0) {
    sprintf(filename, "/dev/mmcblk1p%d", part_num);  // NOLINT (runtime/printf)
  } else {
    sprintf(filename, "/dev/mmcblk1");  // NOLINT (runtime/printf)
  }
  if (stat(filename, &st) != 0) {
    return eFrameworkunifiedStatusAccessError;
  }

  if (eFrameworkunifiedStatusOK
      == (eStatus = FrameworkunifiedSendMsg(m_hSession, SS_DEV_MOUNT_SD, sizeof(uint8_t),
                               &part_num))) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Send MountSD msg Sucessfully");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

  return eStatus;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::UmountSD(uint8_t part_num,
                                              BOOL *already_umounted) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "part_num %d", part_num);

  if (NULL == m_hSession) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "m_hSession is NULL");
    return eFrameworkunifiedStatusInvldHandle;
  }

  if (part_num > 3) {
    return eFrameworkunifiedStatusInvldParam;
  }

  char mount_dir[256];
  if (part_num != 0) {
    sprintf(mount_dir, "/mnt/mmcblk1p%d", part_num);  // NOLINT (runtime/printf)
  } else {
    sprintf(mount_dir, "/mnt/mmcblk1");  // NOLINT (runtime/printf)
  }

  if (isMounttedDir(mount_dir)) {
    *already_umounted = FALSE;
  } else {
    *already_umounted = TRUE;
    return eFrameworkunifiedStatusOK;
  }

  char filename[256];
  struct stat st;

  if (part_num != 0) {
    sprintf(filename, "/dev/mmcblk1p%d", part_num);  // NOLINT (runtime/printf)
  } else {
    sprintf(filename, "/dev/mmcblk1");  // NOLINT (runtime/printf)
  }
  if (stat(filename, &st) != 0) {
    return eFrameworkunifiedStatusAccessError;
  }

  if (eFrameworkunifiedStatusOK
      == (eStatus = FrameworkunifiedSendMsg(m_hSession, SS_DEV_UMOUNT_SD, sizeof(uint8_t),
                               &part_num))) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Send UmountSD msg Sucessfully");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

  return eStatus;
}

static EFrameworkunifiedStatus UsbVBusResetParamChk(int8_t port) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  char* env;
  env = getenv("TARGET_BOARD");
  if (env == NULL) {
    return eFrameworkunifiedStatusFail;
  }

  switch (port) {
    case SS_DEV_USB_PORT0:
    case SS_DEV_USB_PORT1:
    case SS_DEV_USB_PORT2:
      break;
    default:
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid Port Value[%d].", port);
      return eFrameworkunifiedStatusInvldParam;
  }

  /*
   *  Note.
   *  Check the support status of the target board and port.
   */

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

static const uint8_t VBUS_RESET_INTERVAL_MIN = 11;

static EFrameworkunifiedStatus UsbVBusResetIntervalChk(int8_t port, uint8_t interval)
{
  if (port == SS_DEV_USB_PORT1 || port == SS_DEV_USB_PORT2) {
    goto invalidParam;
  }
  if (interval < VBUS_RESET_INTERVAL_MIN) {
    goto invalidParam;
  }
  return eFrameworkunifiedStatusOK;

invalidParam:
  FRAMEWORKUNIFIEDLOG(ZONE_WARN, __func__, "Invalid Param port:%d interval:%d.", port, interval);
  return eFrameworkunifiedStatusInvldParam;
}

static EFrameworkunifiedStatus SendUsbVBusResetMsg(HANDLE handle, int8_t port, uint8_t interval, bool check_interval)
{
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __func__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (NULL == handle) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __func__, "Invalid Handle.");
    return eFrameworkunifiedStatusInvldHandle;
  }

  eStatus = UsbVBusResetParamChk(port);
  if (eStatus != eFrameworkunifiedStatusOK) {
    return eStatus;
  }

  if (check_interval) {
    eStatus = UsbVBusResetIntervalChk(port, interval);
    if (eStatus != eFrameworkunifiedStatusOK) {
      return eStatus;
    }
  }

  int16_t data[] = {port, interval};
  eStatus = FrameworkunifiedSendMsg(handle, SS_USB_VBUS_RESET, sizeof(data), data);

  if (eFrameworkunifiedStatusOK != eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedSendMsg Send Error[0x%x]", eStatus);
    return eStatus;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __func__, "-");
  return eStatus;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::UsbVBusReset(int8_t port, uint8_t interval)
{
  return SendUsbVBusResetMsg(m_hSession, port, interval, true);
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::UsbVBusReset(int8_t port)
{
  return SendUsbVBusResetMsg(m_hSession, port, VBUS_RESET_INTERVAL_MIN, false);
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::RoleSwStateNotify(
    SS_DEV_ROLE_SW_STATE state) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
  EFrameworkunifiedStatus result = eFrameworkunifiedStatusFail;
  UI_32 rcvlength = 0;
  UI_32 bufflength;

  if (NULL == m_hSession) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "No Session");
    return eFrameworkunifiedStatusInvldHandle;
  }

  bufflength = sizeof(EFrameworkunifiedStatus);
  eStatus = FrameworkunifiedInvokeSync(m_hSession, SS_DEV_ROLE_SW_STATE_NOTIFY,
                          sizeof(SS_DEV_ROLE_SW_STATE), &state, bufflength,
                          &result, &rcvlength);

  if (eFrameworkunifiedStatusOK != eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Send Error[0x%x]", eStatus);
    return eStatus;
  }

  if (bufflength != rcvlength) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "FrameworkunifiedInvokeSync Response Error. bufflen[%d], rcvlen[%d]", bufflength,
           rcvlength);
    return eStatus;
  }

  eStatus = result;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus DeviceDetectionServiceIf::FormatSD(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (NULL == m_hSession) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid Handle.");
    return eFrameworkunifiedStatusInvldHandle;
  }

  if (access("/dev/mmcblk1", F_OK) != 0) {
    return eFrameworkunifiedStatusAccessError;
  }

  eStatus = FrameworkunifiedSendMsg(m_hSession, SS_DEV_FORMAT_SD, 0, NULL);

  if (eFrameworkunifiedStatusOK != eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Send Error[0x%x]", eStatus);
    return eStatus;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}  // LCOV_EXCL_BR_LINE 10:Because the last line

