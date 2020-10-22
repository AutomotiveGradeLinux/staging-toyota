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
/// \ingroup  tag_NS_InterfaceunifiedLogCapture
/// \brief    This file contains the standard set functions called by the NS
//            dispatcher on application initialization, cleanup and wakeup.
///
///////////////////////////////////////////////////////////////////////////////
// System Headers
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/time.h>
#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <loggerservicedebug_loggerservicelog.h>
#include <native_service/frameworkunified_multithreading.h>
#include <system_service/ss_system_manager_if.h>
#include <system_service/ss_system_manager_notifications.h>
#include <native_service/frameworkunified_timer.h>
#include <system_service/ss_devicedetection_service_notifications.h>
#include <system_service/ss_devicedetection_service.h>
#include <system_service/ss_devicedetection_service_protocol.h>
#include <system_service/ss_devicedetection_service_ifc.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/frameworkunified_types.h>
#include <queue>
#include <string>

// System definition
#ifndef TN_LOGGERRTIME
#define TN_LOGGERRTIME     "LoggerRtime"
#define PR_LOGGERRTIME_S   PR_TSS_S
#define PR_LOGGERRTIME     PR_TSS
#endif  // TN_LOGGERRTIME
// Queue resource
#define USB_REALTIME_MSGMAX 256
#define USB_REALTIME_SIZMAX 4096
#define USB_REALTIME_QNAME  "/PlRtLogger"
// Folder path
#define USB_RT_FOLDERPATH_ "/REALTIME_"

// older path
#define USB_RT_TMP_PATH "/tmp"
// File name
#define USB_RT_LOGNAME_TMP "usbrt_tmp.log"
#define ENC_BUF_MAX_SIZE   (1024*8)

#ifdef RELEASE_BUILD
// encrypt values
#define DEV_ENC_EXTENSION  ".enc1"
#define REL_ENC_EXTENSION  ".enc2"
#define ALIGNMENT_SIZE 4
#endif  // RELEASE_BUILD

// File path
#define USB_RT_LOGNAME_ "loggerservice_usb__realtime"
// Max size of each log file
#define USB_RT_FILE_SIZE (1024*1024)
// Num of logs to write in std::queue at one time
#define QUELOG_WRITE_BLOCK_NUM 300
// Max of logs to save in std::queue
#define QUELOG_SAVE_MAX   12000
#define QUELOG_DELNUM   (QUELOG_WRITE_BLOCK_NUM*2)  // num of deletion at one time
// TIMER ID
#define USB_RT_TIMER_EV  0x89
#define USB_RT_TIMER_SYNC 1   // kind
#define USB_RT_TIMER_CYCLE 10000  // cycle of timeout(msec)

static HANDLE g_rt_tid = NULL;
static int g_wfd = -1;
static mqd_t g_qid = (mqd_t) - 1;
static int g_num_of_file = 1;
static char g_mnt_path[MAXPATHLEN];
static char g_fld_path[MAXPATHLEN];
static char g_log_path[MAXPATHLEN];
static char *g_log_name;
static bool g_need_sync = false;
static size_t g_filesize = 0;

//static queue<string> g_saveq;
static std::queue<std::string> g_saveq;

static DeviceDetectionServiceIf g_devDetect;
static bool g_usb_available = false;

static char g_log_tmp_path[MAXPATHLEN];

EFrameworkunifiedStatus rtUsbLogThread(HANDLE hApp);
EFrameworkunifiedStatus rtUsbLogTShutdown(HANDLE hApp);
EFrameworkunifiedStatus OnPowerOnOffNotification(HANDLE hApp);

EFrameworkunifiedStatus StartTimer(HANDLE hApp, const UI_32 timeout);
EFrameworkunifiedStatus CancelTimer(void);

// DeviceDetection Related funcs
EFrameworkunifiedStatus OnDeviceDetectionAvailability(HANDLE hApp);
EFrameworkunifiedStatus OnDeviceDetectionOpenSessionACK(HANDLE hApp);
EFrameworkunifiedStatus onDeviceDetectionCloseSessionACK(HANDLE hApp);
EFrameworkunifiedStatus onUsbDetectCallback(HANDLE hApp);

EFrameworkunifiedStatus EncryptAndCopyFile(void);

EFrameworkunifiedStatus StartRtUsbLogThread(HANDLE hApp) {
  FrameworkunifiedChildThreadAttr attr;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (g_rt_tid != NULL) {
    return eFrameworkunifiedStatusOK;
  }

  // Create Realtime USB Logging Thread and start
  memset(&attr, 0, sizeof(FrameworkunifiedChildThreadAttr));
  // One time while to break on Error
  if (eFrameworkunifiedStatusOK
      != (l_eStatus = FrameworkunifiedCreateChildThreadAttrSetSched(&attr,
                                                       eFrameworkunifiedSchedPolicyTSS,
                                                       PR_LOGGERRTIME))) {
    return l_eStatus;
  }
  g_rt_tid = FrameworkunifiedCreateChildThreadWithAttribute(hApp, TN_LOGGERRTIME,
                                               rtUsbLogThread,
                                               rtUsbLogTShutdown, &attr);
  if (g_rt_tid == NULL) {
    l_eStatus = eFrameworkunifiedStatusFail;
  } else {
    l_eStatus = FrameworkunifiedStartChildThread(hApp, g_rt_tid, 0, (PVOID) NULL);
  }

  return l_eStatus;
}

EFrameworkunifiedStatus StopRtUsbLogThread(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  // Close fd to sync
  if (g_wfd != -1) {
    if (0 != close(g_wfd)) {
      l_eStatus = eFrameworkunifiedStatusFail;
    }
    EncryptAndCopyFile();
  }
  // Close message queue
  if (g_qid != (mqd_t) - 1) {
    if (0 != mq_close(g_qid)) {
      l_eStatus = eFrameworkunifiedStatusFail;
    }
  }
  // Terminate Thread
  if (eFrameworkunifiedStatusOK != FrameworkunifiedStopChildThread(hApp, g_rt_tid, 0, NULL)) {
    l_eStatus = eFrameworkunifiedStatusFail;
  }
  if (eFrameworkunifiedStatusOK != FrameworkunifiedDestroyChildThread(hApp, g_rt_tid)) {
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

EFrameworkunifiedStatus OnPowerOnOffNotification(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  T_SS_SM_UserModeOnOffNotification_StructType onoff_mode;

  l_eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, (PVOID) &onoff_mode,
                                  sizeof(onoff_mode));
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    fprintf(stderr, "%s: Failed to fetch OnOff Mode\n", __FUNCTION__);
    return l_eStatus;
  }
  if (!onoff_mode.isUserModeOn) {  // ACC-OFF
    // Close fd to sync
    if (g_wfd >= 0) {
      syncfs(g_wfd);
      if (0 != close(g_wfd)) {
        l_eStatus = eFrameworkunifiedStatusFail;
      }
      EncryptAndCopyFile();
      g_wfd = -2;  // ACC-OFF detected
    }
  }
  return l_eStatus;
}

static int getStartFileNum(void) {
  int start = 0;
  int num;
  DIR *dirp;
  struct dirent entry;
  struct dirent *next;

  if ((dirp = opendir(g_fld_path)) == NULL) {
    fprintf(stderr, "%s: Failed in opendir, errno=%d\n", __FUNCTION__, errno);
    return -1;
  }
  for (;;) {
    if (readdir_r(dirp, &entry, &next) != 0) {
      fprintf(stderr, "%s: Failed in readdir_r, errno=%d\n", __FUNCTION__,
              errno);
      closedir(dirp);
      return -1;
    }
    if (next == NULL) {  // NO more
      break;
    }
    if (strncmp(entry.d_name, g_log_name, strlen(g_log_name)) != 0) {
      continue;
    }
    num = atoi(entry.d_name + strlen(g_log_name));
    if (num > start) {
      start = num;
    }
  }
  closedir(dirp);
  start++;

  return start;
}

EFrameworkunifiedStatus WriteAndSwitchLogFile(const char* data, ssize_t size) {
  ssize_t wrotelen;
  char logpath[MAXPATHLEN];

  wrotelen = write(g_wfd, data, size);
  if (wrotelen != size) {
    close(g_wfd);
    g_wfd = -1;
    g_filesize = 0;
    EncryptAndCopyFile();
    fprintf(stderr, "%s: Failed to write, size=%d, wrotelen=%d, errno=%d\n",

//            __FUNCTION__, static_cast<int32_t>size, static_cast<int32_t>wrotelen, errno);
            __FUNCTION__, static_cast<int32_t>(size), static_cast<int32_t>(wrotelen), errno);

    return eFrameworkunifiedStatusFail;
  } else {
    g_filesize += wrotelen;
    g_need_sync = true;
    if (static_cast<int>(g_filesize) >= USB_RT_FILE_SIZE) {
      g_filesize = 0;
      close(g_wfd);
      EncryptAndCopyFile();
      g_num_of_file++;


//      snprintf(logpath, "%s/%s", g_log_tmp_path, USB_RT_LOGNAME_TMP);
      snprintf(logpath, MAXPATHLEN, "%s/%s", g_log_tmp_path, USB_RT_LOGNAME_TMP);

      if ((g_wfd = open(logpath, O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0644))
          == -1) {
        fprintf(stderr, "%s: Failed to open file %s, errno=%d\n", __FUNCTION__,
                logpath, errno);
        return eFrameworkunifiedStatusFail;
      }
    }
  }
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus OnMessageQueueReceived(HANDLE hApp) {
  ssize_t len, len2;
  char msg[USB_REALTIME_SIZMAX], msg2[USB_REALTIME_SIZMAX];
  char logpath[MAXPATHLEN];
  unsigned int prior;
  struct stat stbuf;

//  string work_str;
  std::string work_str;

  bool pushed = false;
  const struct timespec tspec = { 0, 0 };

  // write loop
  while (1) {
    pushed = false;
    if (g_qid == (mqd_t) - 1) {  // Already ACC-OFFed
      return eFrameworkunifiedStatusOK;
    }
    len = mq_timedreceive(g_qid, msg, sizeof(msg), &prior, &tspec);
    if (len == (ssize_t) - 1) {  // NO more data
      if (errno != ETIMEDOUT) {
        fprintf(stderr, "%s: Unexpected error %d\n", __FUNCTION__, errno);
      }
      break;
    }
    // Remove overflowed log
    if (g_saveq.size() > QUELOG_SAVE_MAX) {
      int delnum = g_saveq.size() - QUELOG_SAVE_MAX + QUELOG_DELNUM;
      for (int i = 0; i < delnum; i++) {
        g_saveq.pop();
      }
    }
    // check if file is opened
    if (g_wfd == -1) {
      /*
       * Make folder and open log file
       */
      // save dequeued log

//      g_saveq.push(string((const char*) msg, len));
      g_saveq.push(std::string((const char*) msg, len));

      pushed = true;  // request pushed or NON-log message -> NO NEED to write this data to log file
      // Check USB mounted
      if (!g_usb_available) {
        // No mount event is received
        continue;
      }
      // Check USB mounted
      if (stat(g_mnt_path, &stbuf) != 0) {
        // No mount path found
        continue;
      }
      // Check and make subfolder
      int mkdres = mkdir(g_fld_path, 0666);
      if (mkdres == 0) {
        g_num_of_file = 1;
      } else if (mkdres == -1 && errno == EEXIST) {
        // Decide file number to start(Check current log files)
        g_num_of_file = getStartFileNum();
        if (g_num_of_file <= 0) {
          fprintf(stderr, "%s: Failed in getStartFileNum\n", __FUNCTION__);
          continue;
        }
      } else {
        fprintf(stderr, "%s: Failed to mkdir %s, errno=%d\n", __FUNCTION__,
                g_fld_path, errno);
        continue;
      }

      // Open File
      g_filesize = 0;


//      snprintf(logpath, "%s/%s", g_log_tmp_path, USB_RT_LOGNAME_TMP);
      snprintf(logpath, MAXPATHLEN, "%s/%s", g_log_tmp_path, USB_RT_LOGNAME_TMP);

      if ((g_wfd = open(logpath, O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0644))
          == -1) {
        fprintf(stderr, "%s: Failed to open file %s, errno=%d\n", __FUNCTION__,
                logpath, errno);
        continue;
      }
    } else if (g_wfd == -2) {
      // Already ACC-OFF, Just throw the logs away
      continue;
    }
    // Write out saved log if any
    int written_once = QUELOG_WRITE_BLOCK_NUM;  // num of written at once
    while (1) {
      if (g_saveq.empty()) {
        break;
      }
      if (written_once >= QUELOG_WRITE_BLOCK_NUM) {
        written_once = 0;
        while (1) {
          // read request queue
          len2 = mq_timedreceive(g_qid, msg2, sizeof(msg2), &prior, &tspec);
          if (len2 == (ssize_t) - 1) {  // NO MORE data
            if (errno != ETIMEDOUT) {
              fprintf(stderr, "%s: Unexpected error %d\n", __FUNCTION__, errno);
            }
            break;
          }

//          g_saveq.push(string((const char*) msg2, len2));
          g_saveq.push(std::string((const char*) msg2, len2));

        }
      }
      work_str = g_saveq.front();
      if (eFrameworkunifiedStatusOK
          != WriteAndSwitchLogFile(work_str.data(), work_str.length())) {
        fprintf(stderr, "%s: Failed in write Saved data, errno=%d\n",
                __FUNCTION__, errno);
        break;
      } else {
        written_once++;
        g_saveq.pop();
      }
    }
    if (pushed || g_wfd == -1) {
      continue;
    }
    // write data
    if (eFrameworkunifiedStatusOK != WriteAndSwitchLogFile(msg, len)) {
      fprintf(stderr, "%s: Failed in write Saved data-2, errno=%d\n",
              __FUNCTION__, errno);
    }
  }
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus OnSyncTimeout(HANDLE hApp) {
  if (g_need_sync) {
    if (g_wfd != -1 && g_wfd != -2) {
      char logpath[MAXPATHLEN];
      fsync(g_wfd);
      close(g_wfd);
      g_wfd = -1;
      EncryptAndCopyFile();

//      snprintf(logpath, "%s/%s", g_log_tmp_path, USB_RT_LOGNAME_TMP);
      snprintf(logpath, MAXPATHLEN, "%s/%s", g_log_tmp_path, USB_RT_LOGNAME_TMP);

      if ((g_wfd = open(logpath, O_RDWR | O_CREAT | O_APPEND | O_CLOEXEC, 0644))
          == -1) {
        fprintf(stderr, "%s: Failed to open file %s, errno=%d (%s)\n",
                __FUNCTION__, logpath, errno, strerror(errno));
        // because in order to continue to obtain logs.
        return eFrameworkunifiedStatusOK;
      }
      g_need_sync = false;
    }
  }
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus rtUsbLogThread(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  struct mq_attr qattr;
  const struct timespec idle_time = { 1, 0 };  // 1sec
  HANDLE timer_handle = NULL;

  // Subscribe Boot Mode Notifcation to SysMgr
  l_eStatus = FrameworkunifiedSubscribeNotificationWithCallback(hApp,
  NTFY_SSSystemMgrPowerOnOff,
                                                   OnPowerOnOffNotification);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    fprintf(stderr, "%s: Subscribe PowerOnOff failed, status=%d\n",
            __FUNCTION__, l_eStatus);
  }
  // Subscribe Device Detection Availability
  while (1) {
    if (g_devDetect.Initialize(hApp)) {
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = g_devDetect.NotifyOnDeviceDetectionAvailability(
              OnDeviceDetectionAvailability))) {
        fprintf(stderr,
                "%s: DevDetec AvailabilityCallback registration failed\n",
                __FUNCTION__);
      }
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = g_devDetect.NotifyOnOpenSessionAck(
              OnDeviceDetectionOpenSessionACK))) {
        fprintf(stderr, "%s: DevDetec OpenSession ACK registration failed\n",
                __FUNCTION__);
      }
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = g_devDetect.NotifyOnCloseSessionAck(
              onDeviceDetectionCloseSessionACK))) {
        fprintf(stderr, "%s: DevDetec CloseSession ACK registration failed\n",
                __FUNCTION__);
      }
      break;
    } else {
      fprintf(stderr, "%s: Device Detection Object Initialization failed\n",
              __FUNCTION__);
      nanosleep(&idle_time, NULL);
    }
  }

  // open message queue
  while (1) {
    qattr.mq_flags = 0;
    qattr.mq_maxmsg = USB_REALTIME_MSGMAX;
    qattr.mq_msgsize = USB_REALTIME_SIZMAX;
    qattr.mq_curmsgs = 0;
    if ((g_qid = mq_open(USB_REALTIME_QNAME, O_RDWR | O_CREAT, 0666, &qattr))
        >= (mqd_t) 0) {
      break;
    }
    fprintf(stderr, "%s: Failed in mq_open, errno=%d\n", __FUNCTION__, errno);
    nanosleep(&idle_time, NULL);
  }

  // Attach callback func to message queue fd
  l_eStatus = FrameworkunifiedAttachCallbackToDispatcherWithFd(hApp, static_cast<int>(g_qid),
                                                  OnMessageQueueReceived);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    fprintf(
        stderr,
        "%s: Error in FrameworkunifiedAttachCallbackToDispatcherWithFd with mq_fd, status=%d\n",
        __FUNCTION__, l_eStatus);
    return l_eStatus;
  }
  // Start Cyclic Timer
  timer_handle = FrameworkunifiedAttachTimerCallback(hApp, USB_RT_TIMER_CYCLE,
  USB_RT_TIMER_CYCLE,
                                        OnSyncTimeout);
  if (timer_handle == NULL) {
    fprintf(stderr, "%s: Error in FrameworkunifiedAttachTimerCallback\n", __FUNCTION__);
    return eFrameworkunifiedStatusFail;
  }

  // Enter dispatcher ==> Just return
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus rtUsbLogTShutdown(HANDLE hApp) {
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus OnDeviceDetectionAvailability(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (FrameworkunifiedIsServiceAvailable(hApp)) {
    if (eFrameworkunifiedStatusOK != (eStatus = g_devDetect.OpenSessionRequest())) {
      fprintf(stderr, "%s: Open session request failed\n", __FUNCTION__);
    }
  } else {
    if (eFrameworkunifiedStatusOK != (eStatus = g_devDetect.CloseSessionRequest())) {
      fprintf(stderr, "%s: Close session request failed\n", __FUNCTION__);
    }
  }
  return eStatus;
}

EFrameworkunifiedStatus OnDeviceDetectionOpenSessionACK(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (eFrameworkunifiedStatusOK == (eStatus = g_devDetect.DecodeOpenSessionResponse())) {
    if (eFrameworkunifiedStatusOK
        != (eStatus = g_devDetect.RegisterForDeviceDetectionEvent(
            SS_DEV_DETECT_ANY_USB_EV, onUsbDetectCallback))) {
      fprintf(stderr, "%s: Registration for SS_DEV_DETECT_ANY_USB_EV failed\n",
              __FUNCTION__);
    }
  } else {
    fprintf(stderr, "%s: Decode open session response failed\n", __FUNCTION__);
  }
  return eStatus;
}

EFrameworkunifiedStatus onDeviceDetectionCloseSessionACK(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CloseSessionAck tCloseSessionAck;

  if (hApp) {
    if (eFrameworkunifiedStatusOK
        == (eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &tCloseSessionAck,
                                          sizeof(tCloseSessionAck)))) {
      if (eFrameworkunifiedStatusOK != tCloseSessionAck.eStatus) {
        eStatus = eFrameworkunifiedStatusFail;
      }
    }
  }
  return eStatus;
}

EFrameworkunifiedStatus onUsbDetectCallback(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  SS_MediaDetectInfo l_tMediaDetectInfo;

  if (eFrameworkunifiedStatusOK
      != (l_eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &l_tMediaDetectInfo,
                                          sizeof(l_tMediaDetectInfo)))) {
    fprintf(stderr, "%s: FrameworkunifiedGetMsgDataOfSize Failed Status:0x%x\n",
            __FUNCTION__, l_eStatus);
  } else {
    if (l_tMediaDetectInfo.dev_type != eUSB) {  // NOT mass storage device
      return l_eStatus;
    }
    if (l_tMediaDetectInfo.bIsDeviceAvailable) {
      if (g_usb_available) {  // Already mounted
        return l_eStatus;
      }
      // set USB device available
      g_usb_available = true;
      // Organize paths to access
      strncpy(g_mnt_path, l_tMediaDetectInfo.deviceMountpath,
              sizeof(g_mnt_path));

      snprintf(g_fld_path, MAXPATHLEN, "%s%s", g_mnt_path, USB_RT_FOLDERPATH_);
      snprintf(g_log_path, MAXPATHLEN, "%s/%s", g_fld_path, USB_RT_LOGNAME_);
      g_log_name = const_cast<char*>(USB_RT_LOGNAME_);



//      snprintf(g_log_tmp_path, "%s", USB_RT_TMP_PATH);
      snprintf(g_log_tmp_path, MAXPATHLEN, "%s", USB_RT_TMP_PATH);


    } else {
      // unset USB device available
      if (strcmp(l_tMediaDetectInfo.deviceMountpath, g_mnt_path) == 0) {
        g_usb_available = false;
        g_mnt_path[0] = 0;
        if (g_wfd != -1 && g_wfd != -2) {
          close(g_wfd);
          g_wfd = -1;
          EncryptAndCopyFile();
        }
      }
    }
  }

  return (l_eStatus);
}

EFrameworkunifiedStatus EncryptAndCopyFile(void) {
  char usblogpath[MAXPATHLEN];
  char logpath[MAXPATHLEN];
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  int rfd, wfd;
  int ret;
  struct stat istat_buf;


//  snprintf(usblogpath, "%s%05d.log", g_log_path, g_num_of_file);

//  snprintf(logpath, "%s/%s", g_log_tmp_path, USB_RT_LOGNAME_TMP);
  snprintf(usblogpath, MAXPATHLEN, "%s%05d.log", g_log_path, g_num_of_file);

  snprintf(logpath, MAXPATHLEN, "%s/%s", g_log_tmp_path, USB_RT_LOGNAME_TMP);


  rfd = open(logpath, O_RDWR | O_CLOEXEC);
  if (rfd == -1) {
    fprintf(stderr, "%s: Error. Failed to open file: %s for reading.\n",
            __FUNCTION__, logpath);
    return eFrameworkunifiedStatusFail;
  }

  ret = fstat(rfd, &istat_buf);
  if (ret < 0) {
    fprintf(stderr, "%s: Error. stat(%s). errno: %d, %s \n", __FUNCTION__,
            logpath, errno, strerror(errno));
    close(rfd);
    return eFrameworkunifiedStatusFail;
  } else {
    BYTE *pData;
    int total_len;
    int read_len;
    off_t total_size = istat_buf.st_size;

    wfd = open(usblogpath, O_RDWR | O_APPEND | O_TRUNC | O_CREAT | O_CLOEXEC,
               0644);
    if (wfd == -1) {
      fprintf(stderr, "%s: Error. Failed to open file: %s for reading.\n",
              __FUNCTION__, usblogpath);
      close(rfd);
      return eFrameworkunifiedStatusFail;
    }

    pData = reinterpret_cast<BYTE*>(malloc(ENC_BUF_MAX_SIZE));
    if (pData == NULL) {
      fprintf(stderr, "%s: Error. Failed to malloc %d byte for dst \n",
              __FUNCTION__, ENC_BUF_MAX_SIZE);
      close(rfd);
      close(wfd);
      return eFrameworkunifiedStatusFail;
    }

    total_len = static_cast<int>(total_size);
    do {
      read_len = read(rfd, pData, ENC_BUF_MAX_SIZE);
      if (read_len == -1) {
        fprintf(stderr, "%s: Error. File read failed. errno: %d, %s \n",
                __FUNCTION__, errno, strerror(errno));
        l_eStatus = eFrameworkunifiedStatusFail;
        break;
      }
      ret = write(wfd, pData, read_len);
      if (ret == -1) {
        fprintf(stderr, "%s: Error. length(%d) no space to write: %d, %s \n",
                __FUNCTION__, ret, errno, strerror(errno));
        l_eStatus = eFrameworkunifiedStatusFail;
        break;
      }
      total_len -= read_len;
    } while (total_len > 0);

    fsync(wfd);
    free(pData);
    close(wfd);
    close(rfd);

    if (l_eStatus == eFrameworkunifiedStatusFail) {
      return l_eStatus;
    }
    {
      // Synchronization security
      struct stat ostat_buf;
      while (1) {
        ret = stat(usblogpath, &ostat_buf);
        if (ret < 0) {
          fprintf(stderr, "%s: Error. stat(%s). errno: %d, %s", __FUNCTION__,
                  usblogpath, errno, strerror(errno));
          break;
        }
        if (ostat_buf.st_size >= total_size) {
          break;
        }
        usleep(1000);  // interval
      }
    }
  }
  return l_eStatus;
}
