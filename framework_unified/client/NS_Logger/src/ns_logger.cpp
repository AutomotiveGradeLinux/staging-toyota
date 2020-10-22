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
/// \ingroup  tag_NSLogger
/// \brief
///
/// Frameworkunified Framework Handlers Logging to log services.
///////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <syslog.h>
#include <boost/algorithm/string.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <aglpath.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <native_service/ns_message_center_if.h>
#include <native_service/ns_plogger_if.h>
#include <native_service/ns_ringbuffer.h>
#include <native_service/ns_config_parser_if.h>
#include <native_service/ns_logger_if.h>
#include <native_service/ns_eventlogger.h>
#include <native_service/cl_lockid.h>

#ifdef AGL_STUB
#include <other_service/strlcat.h>
#include <other_service/strlcpy.h>
#include <other_service/PosixBasedOS001ClockCycleApi.h>
#include "frameworkunified_stub.h"
#else
#include <sys/neutrino.h>
#include <sys/trace.h>  // trace_nlogf
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <sys/syspage.h>
#endif

#include <map>
#include <vector>
#include <sstream>
#include <utility>
#include <string>

#include "ns_logger_internal.h"
#include "ns_msg_queue.h"
#include "mqueue.h"

static HANDLE g_hSSEventLogQ = INVALID_HANDLE;
#ifndef AGL_STUB
static HANDLE g_hNSplogMsgQ =  INVALID_HANDLE;
#endif
static HANDLE g_hNSSysEventLogQ =  INVALID_HANDLE;
static __thread CHAR tls_strProcessName[MAX_NAME_SIZE_APP];
static __thread pid_t tid = 0;
static UI_32 g_msgcounter = 0;
static FRAMEWORKUNIFIEDLOGPARAM g_LocalFrameworkunifiedLogParams;
static UI_32 syslogopened;
static int shm_fd = -1;
static const unsigned int default_log_cfgtbl[] = {
  // realTimeLog
  FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF,
  // flagIdNum
  1,
  // logLevel
  FRAMEWORKUNIFIEDLOG_FLAG_MODE_RELEASE,
};
static void *shm_pmap = reinterpret_cast<void *>(const_cast<unsigned int*>(default_log_cfgtbl));
#define EXE_STR_LEN 64
static char exe_link_path[EXE_STR_LEN] = {0};

/// For PLog
#define PLOG_GAP_FILE        "/dev/shmem/Plog.gap"
#define MAX_GAP_BUFFER_LEN   80
#define MAX_PLOG_STRING_SIZE 1024
#define MAX_TIME_FORMATSTR_LEN 3
#define RETRY_COUNT 5
FRAMEWORKUNIFIEDLOGPPARAM g_FrameworkunifiedLogPParams = {FALSE, EPLOG_TIME_FORMAT_USEC, 0, EPLOG_MSGQ};  // default values for PLog
FRAMEWORKUNIFIEDLOGSYSEVENTPARAM g_FrameworkunifiedLogSysEventParams = {FALSE};  // default values for System event log

#define ZONEMASK_MAX BITS_IN_TZONE  // ((UI_32)(BITS_IN_TZONE * TZONE_COUNT))

#define FRAMEWORKUNIFIEDLOG_DEFAULT_FLAG_ID (0)
#define FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__FN "/usr/agl/conf/BS/ns/framework_unified/rodata/ns_logger_frameworkunifiedlog__CWORD84_.cfg"
#define FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__TMP "ns_logger_frameworkunifiedlog__CWORD84_.tmp"
#define FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__NV "ns_logger_frameworkunifiedlog__CWORD84_.cfg"

#define FRAMEWORKUNIFIEDLOG_CONFIG_NV "/nv/BS/ns/framework_unified/rwdata/"

#define FRAMEWORKUNIFIEDLOG_CONFIG_TMP_NV "ns_logger_frameworkunifiedlog_tmp.cfg"
#ifndef TN_LOGGERRTIME
#define TN_LOGGERRTIME     "LoggerRtime"
#define PR_LOGGERRTIME_S   PR_TSS_S
#define PR_LOGGERRTIME     PR_TSS
#endif  // TN_LOGGERRTIME
#define USB_REALTIME_MSGMAX 256
#define USB_REALTIME_SIZMAX 4096
#define USB_REALTIME_QNAME  "/PlRtLogger"
#define USB_REALTIME_RTRYMAX  10
static mqd_t  g_qid = (mqd_t)(-1);

unsigned int g_FlagIdNum;
std::vector<std::string> g_flag_name;

typedef struct {
  CNSRingBuffer *ring_buffer;
  std::string file_name;
  UI_32 size;
  BOOL force_output;

  BOOL real_time_log;
} FrameworkunifiedLogMapTbl;

typedef struct {
  TFrameworkunifiedZone zone;
  FrameworkunifiedLogMapTbl info;
} FrameworkunifiedLogTbl;

typedef std::map<TFrameworkunifiedZone, FrameworkunifiedLogMapTbl> TFrameworkunifiedLogMap;
static TFrameworkunifiedLogMap g_FrameworkunifiedLogMap;
static TFrameworkunifiedLogMap g_FrameworkunifiedLogFlagMap;
static UI_32 g_FrameworkunifiedLogFlagId = FRAMEWORKUNIFIEDLOG_DEFAULT_FLAG_ID;

static bool g_FrameworkunifiedLogFlagRealTimeLog = TRUE;
pthread_mutex_t FrameworkunifiedLogMtx = PTHREAD_MUTEX_INITIALIZER;

#ifdef ENABLE_DEBUG_SYSLOG  // for FRAMEWORKUNIFIED_SET_ZONES() error
#define DEB_SYSLOG(fmt, ...) {openlog(0, LOG_PID, LOG_USER); \
  syslog(LOG_ERR, "[FRAMEWORKUNIFIEDLOG][ERR]EXE_LINK_PATH(%s)[%s:L%d] " fmt, exe_link_path, \
    __FUNCTION__, __LINE__, ## __VA_ARGS__); \
  closelog();}
#else
#define DEB_SYSLOG(fmt, ...)
#endif

static UI_32
GetFrameworkunifiedLogRealtimeUartOutput(CNSConfigReader *cfg_reader) {
  if (cfg_reader == NULL) {
    return -1;
  }

  std::string output_str = cfg_reader->GetString("REALTIME_LOG.Output");  // LCOV_EXCL_BR_LINE 11:expect branch

  if (output_str.compare("UART") == 0) {
    return FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_UART;
  } else if (output_str.compare("USB") == 0) {
    return FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB;
  } else {
    return FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF;
  }
}

static EFrameworkunifiedStatus
GetFrameworkunifiedLogZoneFileInfo(CNSConfigReader *cfg_reader, std::string &name,  // NOLINT  (readability/nolint)
                      FrameworkunifiedLogMapTbl &tbl, UI_32 &zone) {  // NOLINT  (readability/nolint)
  std::string zone_key = name;
  zone_key.append(".Zone");
  std::string zone_filename_key = name;  // LCOV_EXCL_BR_LINE 11:expect branch
  zone_filename_key.append(".FileName");
  std::string zone_filesize_key = name;  // LCOV_EXCL_BR_LINE 11:expect branch
  zone_filesize_key.append(".FileSizeMax");
  std::string zone_force_output_key = name;  // LCOV_EXCL_BR_LINE 11:expect branch
  zone_force_output_key.append(".ForceOutput");

  if (cfg_reader == NULL) {
    return eFrameworkunifiedStatusNullPointer;
  }

  tbl.ring_buffer = NULL;
  tbl.file_name = cfg_reader->GetString(zone_filename_key);
  tbl.size = static_cast<UI_32>(cfg_reader->GetInt(zone_filesize_key));
  tbl.force_output
    = (cfg_reader->GetString(zone_force_output_key) == "True") ? TRUE : FALSE;

  tbl.real_time_log = TRUE;
  zone = ZONEMASK(static_cast<UI_32>(cfg_reader->GetInt(zone_key)));
  return eFrameworkunifiedStatusOK;
}

static EFrameworkunifiedStatus
GetFrameworkunifiedLogFlagFileInfo(CNSConfigReader *cfg_reader, std::string &name,  // NOLINT  (readability/nolint)
                      FrameworkunifiedLogMapTbl &tbl, UI_32 &loglevel) {  // NOLINT  (readability/nolint)
  std::string flag_loglevel_key = name;
  flag_loglevel_key.append(".LogLevel");
  std::string flag_filename_key = name;  // LCOV_EXCL_BR_LINE 11:expect branch
  flag_filename_key.append(".FileName");
  std::string flag_filesize_key = name;   // LCOV_EXCL_BR_LINE 11:expect branch
  flag_filesize_key.append(".FileSizeMax");

  std::string flag_real_time_log_key = name;
  flag_real_time_log_key.append(".Realtimelog");

  if (cfg_reader == NULL) {
    return eFrameworkunifiedStatusNullPointer;
  }

  tbl.ring_buffer = NULL;
  tbl.file_name = cfg_reader->GetString(flag_filename_key);  // LCOV_EXCL_BR_LINE 11:expect branch
  tbl.size = static_cast<UI_32>(cfg_reader->GetInt(flag_filesize_key));  // LCOV_EXCL_BR_LINE 11:expect branch
  tbl.force_output = FALSE;

  tbl.real_time_log
    = (cfg_reader->GetString(flag_real_time_log_key) == "False") ? FALSE : TRUE;


  std::string loglevel_str = cfg_reader->GetString(flag_loglevel_key);  // LCOV_EXCL_BR_LINE 11:expect branch

  if (loglevel_str.compare("Debug") == 0) {
    loglevel = FRAMEWORKUNIFIEDLOG_FLAG_MODE_DEBUG;
  } else {
    loglevel = FRAMEWORKUNIFIEDLOG_FLAG_MODE_RELEASE;
  }

  if ((tbl.file_name.empty()) || (tbl.size == 0)) {
    return eFrameworkunifiedStatusFail;
  }
  return eFrameworkunifiedStatusOK;
}

static VOID
GetFrameworkunifiedLogList(CNSConfigReader *cfg_reader, std::vector<std::string> &list,  // NOLINT  (readability/nolint)
              std::string req_list) {
  if (cfg_reader == NULL) {
    return;
  }

  std::string list_str = cfg_reader->GetString(req_list);
  if (!list_str.empty()) {
    try {
      boost::algorithm::split(list, list_str, boost::is_any_of(", "));
    }
    /*
     * @todo
     * Even if throw is executed by the process called by 'try', it cannot be captured by catch().
     */
    catch (...) {
      fprintf(stderr, "occured exception handling by boost::algorithm::split in ns_logger\n");
    }
  }
}

static BOOL
CheckMatchFrameworkunifiedLogFlagMap(TFrameworkunifiedLogMap &map, FrameworkunifiedLogMapTbl &check_tbl,  // NOLINT  (readability/nolint)
                        FrameworkunifiedLogMapTbl &out_tbl) {  // NOLINT  (readability/nolint)
  TFrameworkunifiedLogMap::iterator itr;
  for (itr = map.begin(); itr != map.end(); itr++) {
    if (itr->second.file_name.compare(check_tbl.file_name) == 0) {
      out_tbl = itr->second;
      return TRUE;
    }
  }
  return FALSE;
}

static BOOL
IsDefinedFrameworkunifiedLogFlag(CNSConfigReader *cfg_reader, std::string &flag_name,  // NOLINT  (readability/nolint)
                    std::string &exe_name) {  // NOLINT  (readability/nolint)
  UI_32 cnt = 1;
  size_t pos1;

  if (cfg_reader == NULL) {
    return FALSE;
  }

  while (1) {
    std::string service_key = flag_name;
    std::ostringstream service_num;  // LCOV_EXCL_BR_LINE 11:expect branch
    service_num << ".Service" << cnt;
    service_key.append(service_num.str());
    std::string service_name = cfg_reader->GetString(service_key);
    if (service_name.empty()) {
      break;
    }
    pos1 = exe_name.find(service_name);
    if ((pos1 != std::string::npos)
        && (exe_name.length() - pos1) == service_name.length()) {
      return TRUE;
    }
    cnt++;
  }
  return FALSE;
}

static VOID
CreateFrameworkunifiedLogFileTbl(CNSConfigReader *cfg_reader, std::string &exe_str,  // NOLINT  (readability/nolint)
                    int *realTimeLog, int **logLevelArray) {
  int *logLevel;

  std::vector<std::string> zone_vector;
  GetFrameworkunifiedLogList(cfg_reader, zone_vector, "ZONE_LIST.List");
  for (UI_32 i = 0; i < zone_vector.size(); i++) {
    FrameworkunifiedLogMapTbl zone_tbl;
    UI_32 zone;
    if (GetFrameworkunifiedLogZoneFileInfo(cfg_reader, zone_vector.at(i), zone_tbl, zone)
        == eFrameworkunifiedStatusOK) {
      g_FrameworkunifiedLogMap.insert(std::make_pair(zone, zone_tbl));
    }
  }

  *realTimeLog = static_cast<int>(GetFrameworkunifiedLogRealtimeUartOutput(cfg_reader));

  FrameworkunifiedLogMapTbl default_tbl;
  UI_32   default_id = FRAMEWORKUNIFIEDLOG_DEFAULT_FLAG_ID;
  std::string  default_flag = "FLAG_DEFAULT";  // LCOV_EXCL_BR_LINE 11:expect branch
  UI_32   default_loglevel;
  if (GetFrameworkunifiedLogFlagFileInfo(cfg_reader, default_flag, default_tbl, default_loglevel)
      != eFrameworkunifiedStatusOK) {
    fprintf(stderr, "<WARNING>FRAMEWORKUNIFIEDLOG[%s:L%d]FLAG_DEFAULT is not definition into ns_logger_frameworkunifiedlog.cfg.\n",
      __FUNCTION__, __LINE__);

    DEB_SYSLOG("<WARNING>FLAG_DEFAULT is not definition into ns_logger_frameworkunifiedlog.cfg.)");
    default_tbl.ring_buffer = NULL;
    default_tbl.file_name = "frameworkunified_debug.log";
    default_tbl.size = 100;  // 100bytes

    default_tbl.real_time_log = TRUE;
  }
  std::vector<std::string> flag_vector;
  UI_32 flag_id = FRAMEWORKUNIFIEDLOG_DEFAULT_FLAG_ID + 1;
  GetFrameworkunifiedLogList(cfg_reader, flag_vector, "FLAG_LIST.List");  // LCOV_EXCL_BR_LINE 11:expect branch
  if (g_FlagIdNum != (flag_vector.size() + 1)) {
    if (!g_FrameworkunifiedLogFlagMap.empty()) {
      TFrameworkunifiedLogMap::iterator delitr;
      unsigned int delflag_id;
      for (delflag_id = 0; delflag_id < g_FlagIdNum; delflag_id++) {
        delitr = g_FrameworkunifiedLogFlagMap.find(delflag_id);
        if (delitr->second.ring_buffer != NULL) {
          delete delitr->second.ring_buffer;
          delitr->second.ring_buffer = NULL;
        }
      }
      g_FrameworkunifiedLogFlagMap.clear();
      if (!g_flag_name.empty()) {
        g_flag_name.clear();
      }
    }
    g_FlagIdNum = static_cast<unsigned int>(flag_vector.size() + 1);  // +1: [Add Default Flag]
    g_flag_name.push_back(default_flag);  // LCOV_EXCL_BR_LINE 11:expect branch
    g_FrameworkunifiedLogFlagMap.insert(std::make_pair(default_id, default_tbl));  // LCOV_EXCL_BR_LINE 11:expect branch
  }
  logLevel = reinterpret_cast<int *>(malloc(sizeof(int) * g_FlagIdNum));
  logLevel[0] = default_loglevel;

  for (UI_32 i = 0; i < flag_vector.size(); i++) {
    FrameworkunifiedLogMapTbl flag_tbl;
    UI_32 loglevel;
    if (GetFrameworkunifiedLogFlagFileInfo(cfg_reader, flag_vector.at(i), flag_tbl, loglevel)
        == eFrameworkunifiedStatusOK) {
      FrameworkunifiedLogMapTbl check_result_tbl;
      if (CheckMatchFrameworkunifiedLogFlagMap(g_FrameworkunifiedLogFlagMap, flag_tbl, check_result_tbl)
          == TRUE) {
        flag_tbl.file_name = check_result_tbl.file_name;
        flag_tbl.size = check_result_tbl.size;
      }
      if (g_FrameworkunifiedLogFlagMap.find(flag_id) == g_FrameworkunifiedLogFlagMap.end()) {
        if (IsDefinedFrameworkunifiedLogFlag(cfg_reader, flag_vector.at(i), exe_str)
            == TRUE) {
          g_FrameworkunifiedLogFlagId = flag_id;

          g_FrameworkunifiedLogFlagRealTimeLog = flag_tbl.real_time_log;
        }
        g_flag_name.push_back(flag_vector.at(i));
        g_FrameworkunifiedLogFlagMap.insert(std::make_pair(flag_id, flag_tbl));
      }
    }
    logLevel[flag_id] = loglevel;
    flag_id++;
  }
  *logLevelArray = logLevel;
}

static VOID
GetFrameworkunifiedLogRingBuffer(const UI_16 zone, CNSRingBuffer **ring_buf) {
  TFrameworkunifiedLogMap::iterator itr = g_FrameworkunifiedLogMap.find(zone);

  if (ring_buf == NULL) {
    return;
  }

  if (itr == g_FrameworkunifiedLogMap.end()) {
    itr = g_FrameworkunifiedLogFlagMap.find(g_FrameworkunifiedLogFlagId);
    if (itr == g_FrameworkunifiedLogFlagMap.end()) {
      // ASSERT
      *ring_buf = NULL;
      return;
    }
  }

  pthread_mutex_lock(&FrameworkunifiedLogMtx);

  if (itr->second.ring_buffer == NULL) {
    std::string file_path;
    int index;

    file_path = FRAMEWORKUNIFIEDLOG_RAMDISC_PATH;
    file_path.append("/");  // LCOV_EXCL_BR_LINE 11:expect branch
    file_path.append(itr->second.file_name);

    index = NSLogGetFrameworkunifiedlogIndex(file_path.c_str());
    itr->second.ring_buffer = new(std::nothrow) CNSRingBuffer(file_path, itr->second.size,
                                                              index + LOCK_NSLOG_ACCES_IF_1);  // LCOV_EXCL_BR_LINE 11:expect branch
    if (itr->second.ring_buffer != NULL) {
      itr->second.ring_buffer->Open();
    }
  }
  pthread_mutex_unlock(&FrameworkunifiedLogMtx);

  *ring_buf = itr->second.ring_buffer;
}

static int
MapShmInfo(int flagId) {
  int size;
  void *pmap;

  if (shm_fd != -1) {
    return -1;
  }
  shm_fd = shm_open(FRAMEWORKUNIFIEDLOG_SHARED_MEM_NAME, O_RDONLY, S_IRWXU | S_IRWXO);
  if (shm_fd == -1) {

    DEB_SYSLOG("shm_open error errno:%d", errno);
    return -1;
  }
  size = static_cast<int>(sizeof(int) * 2);   // realTimeLog + flagIdNum
  size += static_cast<int>(sizeof(int) * (flagId + 1));  // g_FrameworkunifiedLogFlagId + DefaultFlagId

  pmap = reinterpret_cast<int *>(mmap(NULL, size, PROT_READ, MAP_SHARED, shm_fd, 0));
  if (pmap == MAP_FAILED) {

    DEB_SYSLOG("mmap error errno:%d", errno);
    close(shm_fd);
    shm_fd = -1;
    return -1;
  }
  shm_pmap = pmap;

  return 0;
}

static inline void
GetShmInfo(int flagId, int *realTimeLog, int *logLevel) {
  volatile ssize_t *p;

  if ((realTimeLog == NULL) || (logLevel == NULL)) {
    return;
  }

  p = static_cast<ssize_t *>(shm_pmap);
  *realTimeLog = static_cast<int>(p[0]);
  p += 2;
  *logLevel = static_cast<int>(p[flagId]);
}

static BOOL
IsExistFrameworkunifiedLogCfgShmem(void) {
  if (shm_pmap == reinterpret_cast<void *>(const_cast<unsigned int*>(default_log_cfgtbl))) {
    return FALSE;
  }
  return TRUE;
}

static BOOL
NsLogIsZonePrintEnable(const UI_16 zone, UI_8 *mode) {
  int l_realTimeLog = FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF;
  int l_loglevel = FRAMEWORKUNIFIEDLOG_FLAG_MODE_RELEASE;

  if (mode == NULL) {
    return FALSE;
  }

  GetShmInfo(g_FrameworkunifiedLogFlagId, &l_realTimeLog, &l_loglevel);
  *mode = static_cast<UI_8>(l_realTimeLog);
  if (l_realTimeLog == FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF) {
    return FALSE;
  }

  if ((ZONEMASK(96) <= (TFrameworkunifiedZone)zone)
      && ((TFrameworkunifiedZone)zone <= ZONEMASK(127))) {
    return FALSE;
  }


  if (g_FrameworkunifiedLogFlagRealTimeLog == FALSE) {
    return FALSE;
  }

  return TRUE;
}


static int
copyFrameworkunifiedLogCfgFile(const char *l_cSrcPath, const char *l_cDestPath) {
  int fd_in = -1;
  int fd_ot = -1;
  char l_buf[8*1024];
  ssize_t l_nrd, l_nwr;

  fd_in = open(l_cSrcPath, O_RDONLY | O_CLOEXEC);
  if (fd_in == -1) {
    return -1;
  }

  fd_ot = open(l_cDestPath, O_WRONLY | O_CREAT | O_CLOEXEC, 0640);
  if (fd_ot == -1) {
    close(fd_in);
    return -1;
  }

  while (1) {
    l_nrd = read(fd_in, l_buf, sizeof(l_buf));
    if (l_nrd == 0) {
      break;
    }
    if (l_nrd < 0) {
      close(fd_in);
      close(fd_ot);
      return -1;
    }

    l_nwr = write(fd_ot, l_buf, l_nrd);
    if (l_nwr < 0) {
      close(fd_in);
      close(fd_ot);
      return -1;
    }
  }

  if (fsync(fd_ot) != 0) {
    close(fd_in);
    close(fd_ot);
    return -1;
  }

  close(fd_in);
  close(fd_ot);

  return 0;
}


static VOID
copyFrameworkunifiedLogCfgFileToNv(void) {
  int fd_in = -1;
  int fd_ot = -1;
  int fd_dr = -1;
  char c[1];
  ssize_t len;

  // LCOV_EXCL_BR_START 11:Excluded due to gcov constraints (others)
  std::string config_dirpath = FRAMEWORKUNIFIEDLOG_CONFIG_NV;
  std::string config_tempname = FRAMEWORKUNIFIEDLOG_CONFIG_NV;
  std::string config_filename = FRAMEWORKUNIFIEDLOG_CONFIG_NV;
  fd_in = open(FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__FN, O_RDONLY);
  config_tempname.append(FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__TMP);
  config_filename.append(FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__NV);
  // LCOV_EXCL_BR_STOP 11:Excluded due to gcov constraints (others)

  if (fd_in == -1) {
    return;
  }

  unlink(config_tempname.c_str());

  fd_ot = open(config_tempname.c_str(), O_WRONLY | O_CREAT, 0640);  // LCOV_EXCL_BR_LINE 11:expect branch
  if (fd_ot == -1) {
    DEB_SYSLOG("open error errno:%d", errno);
    close(fd_in);  // LCOV_EXCL_BR_LINE 11:expect branch
    return;
  }

  while (1) {
    len = read(fd_in, c, sizeof(c));
    if (len == 0) {
      break;
    }
    if (len < 0) {
      close(fd_in);
      close(fd_ot);
      return;
    }
    len = write(fd_ot, c, sizeof(c));
    if (len < 0) {
      close(fd_in);
      close(fd_ot);
      return;
    }
  }

  if (fsync(fd_ot) != 0) {
    close(fd_in);
    close(fd_ot);
    return;
  }

  close(fd_in);
  close(fd_ot);

  if (rename(config_tempname.c_str(), config_filename.c_str()) != 0) {

    DEB_SYSLOG("rename error errno:%d", errno);
    return;
  }

  fd_dr = open(config_dirpath.c_str(), O_RDONLY);
  if (fd_dr == -1) {

    DEB_SYSLOG("mmap error errno:%d", errno);
    return;
  }

  fsync(fd_dr);

  close(fd_dr);
}

#define WTYPE_NORMAL 0
#define WTYPE_RTIMELOG 1
static int
WriteSharedMem(int flag, int realTimeLog, unsigned int flagIdNum, int *logLevelArray) {
  int fd;
  ssize_t *pp;
  volatile ssize_t *p;
  int size;

  if ((flag == WTYPE_NORMAL)
      && (logLevelArray == NULL)) {
    return -1;
  }

  size = static_cast<int>(sizeof(int) * 2);  // realTimeLog + flagIdNum
  size += static_cast<int>(sizeof(int) * flagIdNum);

  fd = shm_open(FRAMEWORKUNIFIEDLOG_SHARED_MEM_NAME, O_RDWR | O_CREAT, S_IRWXU | S_IRWXO);
  if (fd == -1) {

    DEB_SYSLOG("shm_open error errno:%d", errno);
    return -1;
  }

  if (ftruncate(fd, size) == -1) {

    DEB_SYSLOG("ftruncate error errno:%d", errno);
    close(fd);
    return -1;
  }

//  pp = (int *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  pp = static_cast<ssize_t *>(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
  if (pp == MAP_FAILED) {

    DEB_SYSLOG("mmap error errno:%d", errno);
    close(fd);
    return -1;
  }

  p = pp;
  p[0] = realTimeLog;

  if (flag == WTYPE_NORMAL) {
    // flagIdNum
    p[1] = flagIdNum;

    // log level
    p += 2;
    for (unsigned int i = 0; i < flagIdNum; i++) {
      *p = logLevelArray[i];
      p++;
    }
  }
  close(fd);
  munmap(pp, size);

  return 0;
}

static int
WriteSharedMemForFlagId(int realTimeLog, unsigned int flagIdNum, int flagId,
                        int logLevel) {
  int fd;
  volatile ssize_t *p;
  ssize_t *pp;
  int size;

  size = static_cast<int>(sizeof(int) * 2);  // realTimeLog + flagIdNum
  size += static_cast<int>(sizeof(int) * flagIdNum);

  fd = shm_open(FRAMEWORKUNIFIEDLOG_SHARED_MEM_NAME, O_RDWR | O_CREAT, S_IRWXU | S_IRWXO);
  if (fd == -1) {

    DEB_SYSLOG("shm_open error errno:%d", errno);
    return -1;
  }

  if (ftruncate(fd, size) == -1) {

    DEB_SYSLOG("ftruncate error errno:%d", errno);
    close(fd);
    return -1;
  }

  pp = static_cast<ssize_t *>(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
  if (pp == MAP_FAILED) {

    DEB_SYSLOG("mmap error errno:%d", errno);
    close(fd);
    return -1;
  }

  p = pp;
  p[0] = realTimeLog;

  // flagIdNum
  p[1] = flagIdNum;

  // log level
  p += 2;
  p[flagId] = logLevel;

  close(fd);
  munmap(pp, size);

  return 0;
}

static BOOL
IsExistFrameworkunifiedLogCfgNv(void) {
  FILE *fp;
  // LCOV_EXCL_BR_START 11:Excluded due to gcov constraints (others)
  std::string config_filename = FRAMEWORKUNIFIEDLOG_CONFIG_NV;
  config_filename.append(FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__NV);
  // LCOV_EXCL_BR_STOP 11:Excluded due to gcov constraints (others)

  fp = fopen(config_filename.c_str(), "re");
  if (fp == NULL) {
    return FALSE;
  }

  fclose(fp);
  return TRUE;
}

static int
get_readlink_path(char *path) {
  CHAR exe_path[EXE_STR_LEN];
  CHAR buf[EXE_STR_LEN];
  ssize_t len;

  if (path == NULL) {
    return -1;
  }

  path[0] = 0;
  if (exe_link_path[0] == 0) {
    pid_t pid = static_cast<pid_t>(syscall(__NR_getpid));
    sprintf(exe_path, "/proc/%d/exe", pid);  // NOLINT  (readability/nolint)
    memset(buf, 0, sizeof(buf));
    len = readlink(exe_path, buf, (sizeof(buf) - 1));
    if (len != -1) {  // LCOV_EXCL_BR_LINE 200: exe_path can not non-existent
      strcpy(exe_link_path, buf);  // NOLINT  (readability/nolint)
    } else {
      // LCOV_EXCL_START 200
      AGL_ASSERT_NOT_TESTED();
      fprintf(stderr, "<WARNING>FRAMEWORKUNIFIEDLOG[%s:L%d]Can't get readlink path(%s). errno(%d)\n", __FUNCTION__, __LINE__,
              exe_path, errno);
      DEB_SYSLOG("<WARNING>Can't get readlink path(%s)(errno:%d)", exe_path, errno);
      return 0;
      // LCOV_EXCL_STOP 200
    }
  }
  strcpy(path, exe_link_path);  // NOLINT  (readability/nolint)
  return 1;     // OK
}

static void
readConfig(int *realTimeLog, int **logLevelArray) {
  CHAR buf[EXE_STR_LEN];

  if (get_readlink_path(buf)) {  // LCOV_EXCL_BR_LINE 200:get_readlink_path can not return false
    std::string config_filename;
    std::string exe_str = buf;  // LCOV_EXCL_BR_LINE 11:expect branch

    // LCOV_EXCL_BR_START 11:Excluded due to gcov constraints (others)
    if (IsExistFrameworkunifiedLogCfgNv()) {
      config_filename = FRAMEWORKUNIFIEDLOG_CONFIG_NV;
      config_filename.append(FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__NV);
    } else {
      config_filename = FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__FN;
    }
    // LCOV_EXCL_BR_STOP 11:Excluded due to gcov constraints (others)
    CNSConfigReader *config_reader = new CNSConfigReader();  // LCOV_EXCL_BR_LINE 11:expect branch
    if (config_reader != NULL) {  // LCOV_EXCL_BR_LINE 5: new's error case
      if (config_reader->Parse(config_filename)
          == eFrameworkunifiedStatusOK) {
        CreateFrameworkunifiedLogFileTbl(config_reader, exe_str, realTimeLog, logLevelArray);  // LCOV_EXCL_BR_LINE 11:expect branch
        MapShmInfo(g_FrameworkunifiedLogFlagId);  // LCOV_EXCL_BR_LINE 11:expect branch
      } else {
        DEB_SYSLOG("Parseg error %s", config_filename.c_str());
      }
    } else {
      // LCOV_EXCL_START 5: new's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      DEB_SYSLOG("new CNSConfigReader error");
      // LCOV_EXCL_STOP 5
    }
    delete config_reader;  // LCOV_EXCL_BR_LINE 11:expect branch1
  } else {
    // LCOV_EXCL_START 5: get_readlink_path can not return false
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    DEB_SYSLOG("get_readlink_path error");
    // LCOV_EXCL_STOP 5
  }
}

//////////////////////////////////////////
// NSLogGetFrameworkunifiedlogFileTotalNum
//////////////////////////////////////////
UI_32
NSLogGetFrameworkunifiedlogFileTotalNum(void) {
  return static_cast<UI_32>((g_FrameworkunifiedLogMap.size() + g_FrameworkunifiedLogFlagMap.size()));
}

//////////////////////////////////////////
// NSLogGetFrameworkunifiedlogFileName
//////////////////////////////////////////
PCSTR
NSLogGetFrameworkunifiedlogFileName(UI_32 index) {
  PCSTR name = NULL;

  if ((g_FrameworkunifiedLogMap.size() + g_FrameworkunifiedLogFlagMap.size()) <= index) {
    return NULL;
  }

  TFrameworkunifiedLogMap::iterator itr;
  TFrameworkunifiedLogMap::iterator itr_end;
  UI_32 cnt;

  if (index < g_FrameworkunifiedLogMap.size()) {
    cnt = 0;
    itr = g_FrameworkunifiedLogMap.begin();
    itr_end = g_FrameworkunifiedLogMap.end();
  } else {
    cnt = static_cast<UI_32>(g_FrameworkunifiedLogMap.size());
    itr = g_FrameworkunifiedLogFlagMap.begin();
    itr_end = g_FrameworkunifiedLogFlagMap.end();
  }

  for (; itr != itr_end; itr++, cnt++) {
    if (cnt == index) {
      name = itr->second.file_name.c_str();
      break;
    }
  }
  return name;
}

//////////////////////////////////////////
// NSLogGetFrameworkunifiedlogIndex
//////////////////////////////////////////
int
NSLogGetFrameworkunifiedlogIndex(PCSTR filename) {
  TFrameworkunifiedLogMap::iterator itr;
  TFrameworkunifiedLogMap::iterator itr_end;
  UI_32 i = 0;
  /**
   * @todo
   * Defining a NULL for arguments results in exceptions.
   */
  std::string fname = filename;

  itr = g_FrameworkunifiedLogFlagMap.begin();
  itr_end = g_FrameworkunifiedLogFlagMap.end();
  for (; itr != itr_end; itr++, i++) {
    if (fname.find(itr->second.file_name) != std::string::npos) {
      return i;
    }
  }

  itr = g_FrameworkunifiedLogMap.begin();
  itr_end = g_FrameworkunifiedLogMap.end();
  for (; itr != itr_end; itr++, i++) {
    if (fname.find(itr->second.file_name) != std::string::npos) {
      return i;
    }
  }
  return 0;
}

UI_32
ConvertEndian(UI_32 *pvar) {
  UI_32 retVal = (UI_32)0;

  if (pvar == NULL) {
    return retVal;
  }

  UI_32 var = *pvar;
  retVal = (((var & 0xFF000000) >> 24) |
            ((var & 0x00FF0000) >> 8) |
            ((var & 0x0000FF00) << 8) |
            ((var & 0x000000FF) << 24));
  return (retVal);
}

static void
check_syslog_opened(void) {
  if (!syslogopened) {
    openlog(0, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_USER);
    syslogopened = 1;
  }
}

static time_t
tick_msec_get(void) {
  struct timespec tp;

  if (clock_gettime(CLOCK_MONOTONIC, &tp) != 0) {  // LCOV_EXCL_BR_LINE 5: clock_gettime's error case
    // LCOV_EXCL_START 5: clock_gettime's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return 0; /* NG */
    // LCOV_EXCL_STOP 5
  }
  return tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
}

static inline int
num2string(PSTR buf, int digits, UI_32 num) {
  PSTR p = NULL;
  char c;

  switch (digits) {
    case 5:
      p = buf;
      if (num >= 100000) {
        *p++ = '0';
        *p++ = '0';
        *p++ = '0';
        *p++ = '0';
        *p = '0';
        break;
      }
      c = static_cast<char>(num / 10000);
      *p++ = static_cast<char>(c + '0');
      num = num - (c * 10000);
      c = static_cast<char>(num / 1000);
      *p++ = static_cast<char>(c + '0');
      num = num - (c * 1000);
    case 3:
      if (p == NULL) {
        p = buf;
      }
      if (num >= 1000) {
        *p++ = '0';
        *p++ = '0';
        *p = '0';
        break;
      }
      c = static_cast<char>(num / 100);
      *p++ = static_cast<char>(c + '0');
      num = num - (c * 100);
    case 2:
      if (p == NULL) {
        p = buf;
      }
      if (num >= 100) {
        *p++ = '0';
        *p = '0';
        break;
      }
      c = static_cast<char>(num / 10);
      *p++ = static_cast<char>(c + '0');
      *p = static_cast<char>(num - (c * 10) + '0');
      break;
    default: {
        int dig = 1;
        int n = num;
        p = buf;
        while (n >= 10) {
          n /= 10;
          dig *= 10;
        }
        while (dig > 1) {
          c = static_cast<char>(num / dig);
          *p++ = static_cast<char>(c + '0');
          num = num - (c * dig);
          dig /= 10;
        }
        *p = static_cast<char>(num + '0');
      }
      break;
  }
  return static_cast<int>(p - buf + 1);
}

static void
_GetTimeString2(PSTR format) {
  time_t tick;
  UI_32 hh, mm, ss, ms;

  // "%02d:%02d:%02d.%03d/"
  tick = tick_msec_get();

  ms = static_cast<UI_32>(tick % 1000);
  format[8] = '.';
  num2string(&format[9], 3, ms);
  format[12] = '/';

  tick /= 1000;
  ss = static_cast<UI_32>(tick % 60);
  format[5] = ':';
  num2string(&format[6], 2, ss);

  tick /= 60;
  mm = static_cast<UI_32>(tick % 60);
  format[2] = ':';
  num2string(&format[3], 2, mm);

  tick /= 60;
  hh = static_cast<UI_32>(tick);
  num2string(&format[0], 2, hh);
}

// LCOV_EXCL_START 8: dead code
void
_GetTimeString(PSTR format, UI_16 length) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 8: dead code
  time_t tick;
  UI_32 hh, mm, ss, ms;

  tick = tick_msec_get();
  ms = static_cast<UI_32>(tick % 1000);
  tick /= 1000;
  ss = static_cast<UI_32>(tick % 60);
  tick /= 60;
  mm = static_cast<UI_32>(tick % 60);
  tick /= 60;
  hh = static_cast<UI_32>(tick);

  snprintf(format, (length - 1), "%02d:%02d:%02d.%03d/", hh, mm, ss, ms);
}
// LCOV_EXCL_STOP 8

static UI_64
ClockCyclesToMs(void) {
  return ClockCycle() / 1000;
}

static CHAR *
_in_NsLog_part1(const UI_16 p_lLine_i, const UI_16 f_uiZoneIndex,
                PCSTR p_pstrClassName_i, CHAR *format) {
  CHAR t_name[16];    // thread name (not more than 15 characters)
  int len;
  CHAR *p = format;

  if (format == NULL) {
    return NULL;
  }

  // fixed format : "%s%ld/%d/%s/%05d/%s/%d/=%s\r\n"
  // time_string
  _GetTimeString2(p);
  p += 13;

  // g_msgcounter
  g_msgcounter++;
  len = num2string(p, 0, g_msgcounter);
  p += len;
  *p++ = '/';

  // f_uiZoneIndex
  len = num2string(p, 0, f_uiZoneIndex);
  p += len;
  *p++ = '/';

  // Check whether thread name has previously been set
  t_name[0] = 0;
  if (tls_strProcessName[0] != 0) {
    tls_strProcessName[sizeof(t_name) - 1] = '\0';
    strcpy(t_name, tls_strProcessName);  // NOLINT  (readability/nolint)
  } else {
    if (prctl(PR_GET_NAME, t_name) != 0) {
      strcpy(t_name, "UNKNOWN");  // NOLINT  (readability/nolint)
    }
    NsLogSetProcessName(t_name);
  }

  // t_name
  strcpy(p, t_name);  // NOLINT  (readability/nolint)
  p += strlen(t_name);
  *p++ = '/';

  // t_id
  if (tid == 0) {
    tid = static_cast<pid_t>(syscall(__NR_gettid));
  }
  len = num2string(p, 5, tid);
  p += len;
  *p++ = '/';

  // p_pstrClassName_i
  if (p_pstrClassName_i) {
    strcpy(p, p_pstrClassName_i);  // NOLINT  (readability/nolint)
  } else {
    strcpy(p, "(null)");  // NOLINT  (readability/nolint)
  }
  p += strlen(p);
  *p++ = '/';

  // p_lLine_i
  len = num2string(p, 0, p_lLine_i);
  p += len;
  *p++ = '/';
  *p++ = '=';

  return p;
}

static void
_in_NsLog_part2(const UI_16 f_uiZoneIndex, CHAR *p, CHAR *format) {
  UI_32 l_uiLength;

  if (p == NULL) {
    return;
  }

  *p++ = '\r';
  *p++ = '\n';
  *p = '\0';

  if (format == NULL) {
    return;
  }

  l_uiLength = static_cast<UI_32>(p - format);

  // Write log message
  if (l_uiLength > 0) {
    UI_8 l_realTimeLog = FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF;
    BOOL IsZonePrintEnable = NsLogIsZonePrintEnable(f_uiZoneIndex, &l_realTimeLog);
    if (l_realTimeLog == FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_FREEZE) {
      if (g_qid != (mqd_t)(-1)) {
        mq_close(g_qid);
        g_qid = (mqd_t)(-1);
      }
      return;
    }
    if (g_LocalFrameworkunifiedLogParams.uiLogOptions & LPRINT) {
      ssize_t l_writtenbyte;
      std::string usb_realtime_filename;
      std::string ownAdrr;
      std::string writeAdrr;
      if (IsZonePrintEnable == TRUE) {
        switch (l_realTimeLog) {
          case FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF:
            // Nothing
            break;
          case FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_UART:
            // Write console
            fwrite(format, 1, l_uiLength, stdout);
            break;
          case FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB:
            if (g_qid == (mqd_t)(-1)) {
              struct mq_attr qattr;

              qattr.mq_flags   = 0;
              qattr.mq_maxmsg  = USB_REALTIME_MSGMAX;
              qattr.mq_msgsize = USB_REALTIME_SIZMAX;
              qattr.mq_curmsgs = 0;
              if ((g_qid = mq_open(USB_REALTIME_QNAME, O_WRONLY | O_CREAT, 0666, &qattr)) == (mqd_t)(-1)) {
                fprintf(stderr, "Failed to create Message Queue %s, errno=%d\n", USB_REALTIME_QNAME, errno);
                break;
              }
            }
            l_writtenbyte = mq_send(g_qid, format, l_uiLength, 1);
            if (l_writtenbyte == -1) {
              fprintf(stderr, "mq_send ERROR, errno=%d\n", errno);
              mq_close(g_qid);
              g_qid = (mqd_t)(-1);
            }
            break;
          case FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB_DISABLE:
            if (g_qid != (mqd_t)(-1)) {
              mq_close(g_qid);
              g_qid = (mqd_t)(-1);
            }
            break;
          default:
            break;
        }
      }
    }
    if (g_LocalFrameworkunifiedLogParams.uiLogOptions & LSLOGGER) {
      check_syslog_opened();
      syslog(LOG_ERR, "%s", format);
    }

    {
      CNSRingBuffer *ring_buffer = NULL;
      GetFrameworkunifiedLogRingBuffer(f_uiZoneIndex, &ring_buffer);

      if (ring_buffer != NULL) {
        if (ring_buffer->IsOpen()) {
          ring_buffer->Write(format, l_uiLength);
        }
      }
    }
  }
}

void
NsLogTime(const UI_16 f_uiZoneIndex, PCSTR lpszFormat) {
  CHAR format[MAX_QUEUE_MSG_SIZE];
  CHAR *p = format;
  CHAR *q = reinterpret_cast<CHAR *>(const_cast<char *>(lpszFormat));
  int i;

  // fixed format : "%s/=%s\r\n"
  // time_string
  _GetTimeString2(p);
  p += 13;
  *p++ = '=';

  /**
   * @todo
   * Defining a NULL for the lpszFormat causes a segmentation fault.
   */
  for (i = 0; *q && i < MAX_QUEUE_MSG_SIZE - (p - format) - 3; i++) {  // '3': '\r''\n''\0'
    *p++ = *q++;
  }
  _in_NsLog_part2(f_uiZoneIndex, p, format);
}

void
NsLogData(const UI_16 f_uiZoneIndex, PCSTR data, UI_32 size) {
  int l_realTimeLog;
  int l_logLevel;

  GetShmInfo(g_FrameworkunifiedLogFlagId, &l_realTimeLog, &l_logLevel);
  if (l_realTimeLog == FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_FREEZE) {
    return;
  }

  if (size > 0) {
    CNSRingBuffer *ring_buffer = NULL;
    GetFrameworkunifiedLogRingBuffer(f_uiZoneIndex, &ring_buffer);  // LCOV_EXCL_BR_LINE 11:expect branch
    if (ring_buffer != NULL) {
      if (ring_buffer->IsOpen()) {
        ring_buffer->Write(data, size);
      }
    }
  }
}

void
NsLog0(const UI_16 p_lLine_i,  // Line number where message was generated
       const UI_16 f_uiZoneIndex,
       PCSTR p_pstrClassName_i,
       PCSTR lpszFormat) {
  CHAR format[MAX_QUEUE_MSG_SIZE];
  CHAR *p;
  CHAR *q = reinterpret_cast<CHAR *>(const_cast<char *>(lpszFormat));
  int i;

  p = _in_NsLog_part1(p_lLine_i, f_uiZoneIndex, p_pstrClassName_i, format);
  /**
   * @todo
   * Setting p_pstrClassName_i to NULL results in a segmentation fault.
   */
  for (i = 0; *q && i < MAX_QUEUE_MSG_SIZE - (p - format) - 3; i++) {  // '3': '\r''\n''\0'
    *p++ = *q++;
  }
  _in_NsLog_part2(f_uiZoneIndex, p, format);
}

void
NsLog(const UI_16 p_lLine_i,  // Line number where message was generated
      const UI_16 f_uiZoneIndex,
      PCSTR p_pstrClassName_i,
      PCSTR lpszFormat,
      ...) {
  va_list argList;
  CHAR format[MAX_QUEUE_MSG_SIZE];
  CHAR *p;
  int num;
  int size;

  va_start(argList, lpszFormat);

  p = _in_NsLog_part1(p_lLine_i, f_uiZoneIndex, p_pstrClassName_i, format);
  size = static_cast<int>(MAX_QUEUE_MSG_SIZE - (p - format) - 3);
  num = vsnprintf(p, size, lpszFormat, argList);
  if (num >= size) {
    num = size - 1;
  } else if (num < 0) {
    num = 0;
  }
  p += num;
  _in_NsLog_part2(f_uiZoneIndex, p, format);

  va_end(argList);
}

void
NsLogSetFrameworkunifiedLogParams(FRAMEWORKUNIFIEDLOGPARAM *p_FrameworkunifiedLogParams) {
  int l_realTimeLog = 0;
  int *l_logLevelArray = NULL;
  CHAR buf[EXE_STR_LEN];

  if (p_FrameworkunifiedLogParams == NULL) {
    return;
  }

  get_readlink_path(buf);
  if (g_flag_name.empty()) {  // LCOV_EXCL_BR_LINE 200:get_readlink_path can not return false
    std::string config_filename;
    CHAR buf[EXE_STR_LEN];

    if (IsExistFrameworkunifiedLogCfgNv()) {  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
      config_filename = FRAMEWORKUNIFIEDLOG_CONFIG_NV;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
      config_filename.append(FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__NV);  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
    } else {
      config_filename = FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__FN;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
    }
    if (get_readlink_path(buf)) {
      std::string exe_str = buf;  // LCOV_EXCL_BR_LINE 11:expect branch
      CNSConfigReader *config_reader = new CNSConfigReader();  // LCOV_EXCL_BR_LINE 11:expect branch
      if (config_reader != NULL) {  // LCOV_EXCL_BR_LINE 5: new's error case
        if (config_reader->Parse(config_filename)
            == eFrameworkunifiedStatusOK) {
          CreateFrameworkunifiedLogFileTbl(config_reader, exe_str, &l_realTimeLog, &l_logLevelArray);  // LCOV_EXCL_BR_LINE 11:expect branch
          MapShmInfo(g_FrameworkunifiedLogFlagId);  // LCOV_EXCL_BR_LINE 11:expect branch
        } else {
          DEB_SYSLOG("Parse error %s", config_filename.c_str());
        }
      } else {
        // LCOV_EXCL_START 5: new's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        DEB_SYSLOG("new CNSConfigReader error");
        // LCOV_EXCL_STOP 5
      }
      delete config_reader;  // LCOV_EXCL_BR_LINE 11:expect branch
    } else {
      // LCOV_EXCL_START 5: get_readlink_path can not return false
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      DEB_SYSLOG("get_readlink_path error");
      // LCOV_EXCL_STOP 5
    }
  } else {
    // already read
    // do nothing
  }

  if (!IsExistFrameworkunifiedLogCfgShmem()) {
    if (l_logLevelArray == NULL) {
      readConfig(&l_realTimeLog, &l_logLevelArray);
    }
    WriteSharedMem(WTYPE_NORMAL, l_realTimeLog, g_FlagIdNum, l_logLevelArray);
    MapShmInfo(g_FrameworkunifiedLogFlagId);
  }
  if (l_logLevelArray != NULL) {
    free(l_logLevelArray);
  }

  /**
   * @tood
   * Segmentation faults occur when NULL is defined as arguments.
   */
  memcpy(&g_LocalFrameworkunifiedLogParams, p_FrameworkunifiedLogParams, sizeof(FRAMEWORKUNIFIEDLOGPARAM));

  // Force Output ZONE_NS_WAR(8),ZONE_NS_ERR(9)
  TFrameworkunifiedZone l_uiZoneMask = g_LocalFrameworkunifiedLogParams.uiZoneMask[0];
  l_uiZoneMask = l_uiZoneMask | GET_ZONE_BIT_MASK(ZONE_NS_WAR) | GET_ZONE_BIT_MASK(ZONE_NS_ERR);
  g_LocalFrameworkunifiedLogParams.uiZoneMask[0] = l_uiZoneMask;

  // Force Output Zones
  {
    TFrameworkunifiedLogMap::iterator itr = g_FrameworkunifiedLogMap.begin();
    TFrameworkunifiedLogMap::iterator itr_end = g_FrameworkunifiedLogMap.end();
    for (; itr != itr_end; itr++) {
      if (itr->second.force_output) {
        UI_16 zone = static_cast<UI_16>(itr->first);
        TFrameworkunifiedZone *pl_uiZoneMask = &g_LocalFrameworkunifiedLogParams.uiZoneMask[GET_ZONE_INDEX(zone)];
        *pl_uiZoneMask |= GET_ZONE_BIT_MASK(zone);
      }
    }
  }
}

void
NsLogSetProcessName(PCSTR p_strProcessName_i) {
  if ((p_strProcessName_i != NULL) && (strlen(p_strProcessName_i) < _countof(tls_strProcessName))) {
    strcpy(tls_strProcessName, p_strProcessName_i);  // NOLINT  (readability/nolint)
  }
}

void
NsLogSetControlMask(TFrameworkunifiedZoneMask p_NSLogControl_i) {
  for (UI_32 zoneIndex = 0; zoneIndex < ZONE_MASK_ARRAY_ELTS; zoneIndex++) {
    /**
     * @todo
     * Setting NULL to arguments terminates abnormally
     */
    g_LocalFrameworkunifiedLogParams.uiZoneMask[zoneIndex] = p_NSLogControl_i[zoneIndex];
  }
}

void
NsLogGetControlMask(TFrameworkunifiedZoneMask p_Zonemask_i) {
  for (UI_32 zoneIndex = 0; zoneIndex < ZONE_MASK_ARRAY_ELTS; zoneIndex++) {
    /**
     * @todo
     * Setting NULL to arguments terminates abnormally
     */
    p_Zonemask_i[zoneIndex] = g_LocalFrameworkunifiedLogParams.uiZoneMask[zoneIndex];
  }
}

BOOL
NsLogIsZoneSet(UI_32 set_zone) {
  int realTimeLog;
  int logLevel = 0;

  GetShmInfo(g_FrameworkunifiedLogFlagId, &realTimeLog, &logLevel);

  if (logLevel == FRAMEWORKUNIFIEDLOG_FLAG_MODE_DEBUG)
  {
    return TRUE;
  }

  if (set_zone >= BITS_IN_ZONE_MASK) {
    return FALSE;
  }

  if (0 != (g_LocalFrameworkunifiedLogParams.uiZoneMask[ GET_ZONE_INDEX(set_zone) ] & GET_ZONE_BIT_MASK(set_zone))) {
    return TRUE;
  }

  return FALSE;
}

void
NsLogSetLogMethod(UI_8 p_eMethod_i) {
  g_LocalFrameworkunifiedLogParams.uiLogOptions = p_eMethod_i;
}

UI_8
NsLogGetLogMethod(void) {
  return g_LocalFrameworkunifiedLogParams.uiLogOptions;
}

void
NsLogSetSeverity(TFrameworkunifiedLoggerSeverity p_eLogSeverity_i) {
  g_LocalFrameworkunifiedLogParams.eSeverity = p_eLogSeverity_i;
}

TFrameworkunifiedLoggerSeverity
NsLogGetSeverity(void) {
  return g_LocalFrameworkunifiedLogParams.eSeverity;
}

UI_8
NsLogDetermineLogMethod(PCSTR output_type) {
  PSTR p = NULL;
  const CHAR delims[] = "|";
  UI_8 outmask = 0;
  PSTR parse = NULL;
  PSTR saveptr = NULL;

  if (output_type != NULL) {
    // since strtok modifies the var. we
    // must create a copy of the string.
    parse = strdup(output_type);
    p = strtok_r (parse, delims, &saveptr);  // NOLINT  (readability/nolint)
    while (p != NULL) {
      // slogger|msgq|console
#ifdef AGL_STUB
      if (!strcasecmp(p, "slogger")) {
#else
      if (!strcmpi(p, "slogger")) {
#endif
        outmask |= LSLOGGER;
#ifdef AGL_STUB
      } else if (!strcasecmp(p, "msgq")) {
#else
      } else if (!strcmpi(p, "msgq")) {
#endif
        outmask |= LMSGQ;
#ifdef AGL_STUB
      } else if (!strcasecmp(p, "console")) {
#else
      } else if (!strcmpi(p, "console")) {
#endif
        outmask |= LPRINT;
#ifdef AGL_STUB
      } else if (!strcasecmp(p, "shmem")) {
#else
      } else if (!strcmpi(p, "shmem")) {
#endif
        outmask |= LSHAREDMEM;
      } else {
        // do nothing
      }

      p = strtok_r (NULL, delims, &saveptr);  // NOLINT  (readability/nolint)
    }

    // since we made dup of the char * passed we need to free it!
    if (parse != NULL) {
      free(parse);
      parse = NULL;
    }
  }
  return outmask;
}

void
NsLogSetZones(UI_32 f_uiZoneCount, ...) {
  va_list l_tArguments;

  // Initialize the variable argument list
  va_start(l_tArguments, f_uiZoneCount);

  // Iterate the variable argument list
  for (UI_32 l_uiArgumentCount = 0; l_uiArgumentCount < f_uiZoneCount; l_uiArgumentCount++) {
    // get zone value
    UI_32 l_uiZone = va_arg(l_tArguments, UI_32);  // LCOV_EXCL_BR_LINE 11:expect branch

    if (l_uiZone < BITS_IN_ZONE_MASK) {
      // add zone value to existing mask
      UI_32 l_uiZoneMask = g_LocalFrameworkunifiedLogParams.uiZoneMask[GET_ZONE_INDEX(l_uiZone)];
      l_uiZoneMask = l_uiZoneMask | GET_ZONE_BIT_MASK(l_uiZone);
      g_LocalFrameworkunifiedLogParams.uiZoneMask[GET_ZONE_INDEX(l_uiZone)] = l_uiZoneMask;
    }
  }

  va_end(l_tArguments);
}

void
NsLogParseZones(FRAMEWORKUNIFIEDLOGPARAM *p_FrameworkunifiedLogParams, UI_32 f_uiZoneCount, ...) {
  va_list l_tArguments;

  if (p_FrameworkunifiedLogParams == NULL) {
    return;
  }

  // Initialize the variable argument list
  va_start(l_tArguments, f_uiZoneCount);

  // Iterate the variable argument list
  for (UI_32 l_uiArgumentCount = 0; l_uiArgumentCount < f_uiZoneCount; l_uiArgumentCount++) {
    // get zone value
    UI_32 l_uiZone = va_arg(l_tArguments, UI_32);  // LCOV_EXCL_BR_LINE 11:expect branch

    if (l_uiZone < BITS_IN_ZONE_MASK) {
      // add zone
      UI_32 l_uiZoneMask = p_FrameworkunifiedLogParams->uiZoneMask[GET_ZONE_INDEX(l_uiZone)];
      l_uiZoneMask = l_uiZoneMask | GET_ZONE_BIT_MASK(l_uiZone);
      p_FrameworkunifiedLogParams->uiZoneMask[GET_ZONE_INDEX(l_uiZone)] = l_uiZoneMask;
    }
  }

  va_end(l_tArguments);
}

void
NsLog_EvtCnt(UI_16 Cnt_Id, UI_16 Evt_Id, UI_8 nu, ...) {
  st_LogEvent log;
  va_list EC_list;
  va_start(EC_list, nu);
  UI_8 i = 0;
  UI_32 ts = (UI_32)ClockCyclesToMs();

  memset(&log, 0, sizeof(log));

  log.ts = ConvertEndian(&ts);
  log.event_id = (UI_8)Evt_Id;
  log.grp_ID = (UI_8)(Evt_Id >> 8);
  log.cnt_ID = Cnt_Id;

  switch (nu) {
    case 1:
      log.data[0] = (UI_8)va_arg(EC_list, int);
      log.phase = NORMAL;
      log.typeofdata = COMMON;
      break;
    case 2:
      log.data[0] = (UI_8)va_arg(EC_list, int);;
      log.phase = (SystemPhase)va_arg(EC_list, int);
      log.typeofdata = COMMON;
      break;
    case 4:
      for (i = 0; i < nu; i++) {
        log.data[i] = (UI_8)va_arg(EC_list, int);
      }
      log.phase = NORMAL;
      log.typeofdata = EVENT_SPECIFIC;
      break;
    case 5:
      for (i = 0; i < (nu - 1); i++) {
        log.data[i] = (UI_8)va_arg(EC_list, int);
      }
      log.phase = (SystemPhase)va_arg(EC_list, int);
      log.typeofdata = EVENT_SPECIFIC;
      break;
    default:
      memset(&log, 0, sizeof(log));
      break;
  }

  if (log.cnt_ID != 0 && log.grp_ID != 0) {
    if (g_hSSEventLogQ == INVALID_HANDLE) {
      g_hSSEventLogQ = McOpenSender(EVENTLOG_MSGQ_NAME);
    }
    McSend(g_hSSEventLogQ, tls_strProcessName, SS_MSG_LOGGER_CNT_EVTLOG, sizeof(log), &log);
  }
  va_end(EC_list);
}


void
NsLog_Evt(UI_16 Evt_Id, UI_8 nu, ...) {
  st_LogEvent log_Evt;
  va_list E_list;
  va_start(E_list, nu);
  UI_32 ts = (UI_32)ClockCyclesToMs();

  memset(&log_Evt, 0, sizeof(st_LogEvent));

  log_Evt.ts = ConvertEndian(&ts);
  log_Evt.grp_ID = (UI_8)(Evt_Id >> 8);
  log_Evt.event_id = (UI_8)Evt_Id;

  UI_8 i;
  switch (nu) {
    case 1:
      log_Evt.data[0] = (UI_8)va_arg(E_list, int);  // LCOV_EXCL_BR_LINE 11:expect branch
      log_Evt.typeofdata = COMMON;
      break;
    case 4:
      for (i = 0; i < nu; i++) {
        log_Evt.data[i] = (UI_8)va_arg(E_list, int);  // LCOV_EXCL_BR_LINE 11:expect branch
      }
      log_Evt.typeofdata = EVENT_SPECIFIC;
      break;
    default:
      memset(&log_Evt, 0, sizeof(st_LogEvent));
      break;
  }
  if (log_Evt.grp_ID != 0) {
    if (g_hSSEventLogQ == INVALID_HANDLE) {
      g_hSSEventLogQ = McOpenSender(EVENTLOG_MSGQ_NAME);
    }

    McSend(g_hSSEventLogQ, tls_strProcessName, SS_MSG_EVTLOG, sizeof(log_Evt), &log_Evt);
  }
  va_end(E_list);
}


void
NsLog_Cnt(UI_16 Cnt_Id, UI_8 nu, ...) {
  st_LogCount log_Cnt;
  va_list C_list;
  va_start(C_list, nu);

  memset(&log_Cnt, 0, sizeof(st_LogCount));
  log_Cnt.cnt_id = Cnt_Id;

  switch (nu) {
    case 0:
      log_Cnt.phase = NORMAL;
      break;
    case 1:
      log_Cnt.phase = (SystemPhase)va_arg(C_list, int);  // LCOV_EXCL_BR_LINE 11:expect branch
      break;
    default:
      memset(&log_Cnt, 0, sizeof(st_LogCount));
      break;
  }
  if (log_Cnt.cnt_id != 0) {
    if (g_hSSEventLogQ == INVALID_HANDLE) {
      g_hSSEventLogQ = McOpenSender(EVENTLOG_MSGQ_NAME);
    }
    McSend(g_hSSEventLogQ, tls_strProcessName, SS_MSG_LOGGERCNT, sizeof(log_Cnt), &log_Cnt);
  }
  va_end(C_list);
}

//////////////////////////////////////////
// NSPrintPerformanceLog
//////////////////////////////////////////
VOID
NSLogPrintPerformanceLog(const UI_16 f_ui16Line, PCSTR f_cFuncName, PCSTR __format, ...) {
#ifndef AGL_STUB
  va_list arg;
  CHAR l_cFormat[MAX_PLOG_STRING_SIZE] = {};
  CHAR l_cTimeFormat[MAX_TIME_FORMATSTR_LEN] = {};
  UI_64 l_ui64CurrentTime  = 0;

  if (g_hNSplogMsgQ == INVALID_HANDLE) {
    g_hNSplogMsgQ = OpenSender(NS_PLOGQ);
  }

  if (g_FrameworkunifiedLogPParams.ui64GapInuS == 0) {
    FILE *l_fpPLogGap = NULL;
    l_fpPLogGap = fopen(PLOG_GAP_FILE, "re");
    if (l_fpPLogGap != NULL) {
      CHAR l_cBuffer[MAX_GAP_BUFFER_LEN] = {};
      if (fgets(l_cBuffer, MAX_GAP_BUFFER_LEN, l_fpPLogGap) != NULL) {
        g_FrameworkunifiedLogPParams.ui64GapInuS = (unsigned)atoll(l_cBuffer);
      }
      fclose(l_fpPLogGap);
    }
  }

  if (g_FrameworkunifiedLogPParams.eTimeFormat == EPLOG_TIME_FORMAT_USEC) {
#ifdef AGL_PosixBasedOS001LEGACY_USED
    l_ui64CurrentTime = ClockCycle() + g_FrameworkunifiedLogPParams.ui64GapInuS;
#endif
    snprintf(l_cTimeFormat, MAX_TIME_FORMATSTR_LEN, "%s", "us");
  } else if (g_FrameworkunifiedLogPParams.eTimeFormat == EPLOG_TIME_FORMAT_MSEC) {
    l_ui64CurrentTime = ClockCyclesToMs() + (g_FrameworkunifiedLogPParams.ui64GapInuS / 1000);
    snprintf(l_cTimeFormat, MAX_TIME_FORMATSTR_LEN, "%s", "ms");
  } else if (g_FrameworkunifiedLogPParams.eTimeFormat == EPLOG_TIME_FORMAT_SEC) {
#ifdef AGL_PosixBasedOS001LEGACY_USED
    l_ui64CurrentTime = (ClockCycle() / 1000000) + (g_FrameworkunifiedLogPParams.ui64GapInuS / 1000000);
#endif
    snprintf(l_cTimeFormat, MAX_TIME_FORMATSTR_LEN, "%s", "s");
  } else {
    // do nothing
  }

  va_start(arg, __format);
  vsnprintf(l_cFormat, MAX_PLOG_STRING_SIZE - 1, __format, arg);
  l_cFormat[MAX_PLOG_STRING_SIZE - 1] = '\0';
  va_end(arg);

  CHAR l_cMsg[MAX_QUEUE_MSG_SIZE] = {};
  snprintf(l_cMsg, MAX_QUEUE_MSG_SIZE - 1, "PLog: %12llu[%s]:%s %s: %s: %s",
           l_ui64CurrentTime, l_cTimeFormat, (g_FrameworkunifiedLogPParams.ui64GapInuS > 0) ? ("o") : ("x"),
           tls_strProcessName, f_cFuncName, l_cFormat);

  if (g_FrameworkunifiedLogPParams.uiLogOutputOptions & EPLOG_SLOG) {
    // send to system logger
    check_syslog_opened();
    syslog(LOG_INFO, "%s", l_cMsg);
  }

  if (g_FrameworkunifiedLogPParams.uiLogOutputOptions & EPLOG_TRACEEVENT) {
#ifdef AGL_PosixBasedOS001LEGACY_USED
    (VOID)trace_nlogf(_NTO_TRACE_USERFIRST, MAX_QUEUE_MSG_SIZE - 1, l_cMsg);
#endif
  }

  if (g_FrameworkunifiedLogPParams.uiLogOutputOptions & EPLOG_MSGQ) {
    // send to message queue /NSplog
    if (g_hNSplogMsgQ != INVALID_HANDLE) {
#ifdef AGL_PosixBasedOS001LEGACY_USED
      strlcat(l_cMsg, "\n", sizeof(l_cMsg));  // insert new line... user may not have.
#endif
      if (SendMessage(g_hNSplogMsgQ, strlen(l_cMsg),
        reinterpret_cast<void *>(const_cast<char *>(l_cMsg))) != eFrameworkunifiedStatusOK) {
        // Note: Nothing will be printed on a failure.
      }
    }
  }
#endif
}

//////////////////////////////////////////
// NSLogEnablePLog
//////////////////////////////////////////
VOID
NSLogEnablePLog(BOOL f_bEnable) {
  g_FrameworkunifiedLogPParams.bIsPLogEnabled = f_bEnable;
}

//////////////////////////////////////////
// NsLogIsPLogEnabled
//////////////////////////////////////////
BOOL
NsLogIsPLogEnabled(void) {
  return g_FrameworkunifiedLogPParams.bIsPLogEnabled;
}

//////////////////////////////////////////
// NSLogSetPlogTimeFormat
//////////////////////////////////////////
VOID
NSLogSetPlogTimeFormat(EPLOG_TIME_FORMAT f_ePlogTimeFormat) {
  g_FrameworkunifiedLogPParams.eTimeFormat = f_ePlogTimeFormat;
}

//////////////////////////////////////////
// NSLogSetPlogOutputOptions
//////////////////////////////////////////
VOID
NSLogSetPlogOutputOptions(UI_8 f_uiPlogOutputOption) {
  g_FrameworkunifiedLogPParams.uiLogOutputOptions = f_uiPlogOutputOption;
}

//////////////////////////////////////////
// NsLogInitialize for Diag Method
//////////////////////////////////////////
VOID
NsLogInitialize(void) {
  std::string config_filename = FRAMEWORKUNIFIEDLOG_CONFIG_NV;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
  config_filename.append(FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__NV);  // LCOV_EXCL_BR_LINE 11:expect branch

  int l_realTimelog;
  int *l_logLevelArray = NULL;

  if (unlink(config_filename.c_str()) != 0) {
    // error TODO wirte log, bit, not use frameworkunifiedlog
  }

  readConfig(&l_realTimelog, &l_logLevelArray);    // LCOV_EXCL_BR_LINE 11:expect branch
  if (l_logLevelArray == NULL) {
    return;
  }
  WriteSharedMem(WTYPE_NORMAL, l_realTimelog, g_FlagIdNum, l_logLevelArray);  // LCOV_EXCL_BR_LINE 11:expect branch
  if (l_logLevelArray != NULL) {
    free(l_logLevelArray);
  }
}

static void writeUpdateConfigForString(std::string &name,  // NOLINT  (readability/nolint)
                                       std::string set_key,
                                       std::string value) {
  std::string flag_loglevel_key = name;
  flag_loglevel_key.append(set_key);

  std::string config_filename = FRAMEWORKUNIFIEDLOG_CONFIG_NV;  // LCOV_EXCL_BR_LINE 11:expect branch
  std::string config_tmpfilename = FRAMEWORKUNIFIEDLOG_CONFIG_NV;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
  config_filename.append(FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__NV);  // LCOV_EXCL_BR_LINE 11:expect branch

  config_tmpfilename.append(FRAMEWORKUNIFIEDLOG_CONFIG_TMP_NV);

  unlink(config_tmpfilename.c_str());
  if (copyFrameworkunifiedLogCfgFile(config_filename.c_str(), config_tmpfilename.c_str()) != 0) {
    DEB_SYSLOG("fail to copy file. no update config.");
    return;
  }

//  CNSConfigWriter *l_pConfigWriter = new(std::nothrow) CNSConfigWriter(config_filename.c_str());
  CNSConfigWriter *l_pConfigWriter = new(std::nothrow) CNSConfigWriter(config_tmpfilename.c_str());  // LCOV_EXCL_BR_LINE 11:expect branch

  if (l_pConfigWriter != NULL) {
    try {
      // l_iReturnState = ConfigWriterUpdateValue(l_hHandle, l_sKeyValue.c_str(), actualOutput.c_str());
      l_pConfigWriter->Set(flag_loglevel_key, value);
    } catch (...) {
      // error TODO
    }
    // ConfigWriterClose(l_hHandle);
    l_pConfigWriter->Save();
    delete l_pConfigWriter;
    l_pConfigWriter = NULL;

    if (rename(config_tmpfilename.c_str(), config_filename.c_str()) != 0) {
      DEB_SYSLOG("rename error errno:%d", errno);
    }

  }
}

static void
writeUpdateConfig(std::string set_key, std::string value) {
  std::string config_filename = FRAMEWORKUNIFIEDLOG_CONFIG_NV;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
  std::string config_tmpfilename = FRAMEWORKUNIFIEDLOG_CONFIG_NV;  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
  config_filename.append(FRAMEWORKUNIFIEDLOG_CONFIG__CWORD84__NV);  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
  config_tmpfilename.append(FRAMEWORKUNIFIEDLOG_CONFIG_TMP_NV);

  unlink(config_tmpfilename.c_str());
  if (copyFrameworkunifiedLogCfgFile(config_filename.c_str(), config_tmpfilename.c_str())) {
    DEB_SYSLOG("fail to copy file. no update config.");
    return;
  }

//  CNSConfigWriter *l_pConfigWriter = new(std::nothrow) CNSConfigWriter(config_filename.c_str());
  CNSConfigWriter *l_pConfigWriter = new(std::nothrow) CNSConfigWriter(config_tmpfilename.c_str());  // LCOV_EXCL_BR_LINE 11:expect branch


  if (l_pConfigWriter != NULL) {
    l_pConfigWriter->Set(set_key.c_str(), value);  // LCOV_EXCL_BR_LINE 11:expect branch
    l_pConfigWriter->Save();  // LCOV_EXCL_BR_LINE 11:expect branch
    delete l_pConfigWriter;  // LCOV_EXCL_BR_LINE 11:expect branch

    if (rename(config_tmpfilename.c_str(), config_filename.c_str()) != 0) {
      DEB_SYSLOG("rename error errno:%d", errno);
    }
  }
}

//////////////////////////////////////////
// NsLogSetFrameworkunifiedLogFlag for Diag Method
//////////////////////////////////////////
VOID
NsLogSetFrameworkunifiedLogFlag(UI_8 flag_id, UI_8 mode) {
  std::string l_key = ".LogLevel";
  int l_realTimelog;
  int *l_logLevelArray;
  std::string l_value;

  if (mode != FRAMEWORKUNIFIEDLOG_FLAG_MODE_DEBUG
      && mode != FRAMEWORKUNIFIEDLOG_FLAG_MODE_RELEASE) {
    return;
  }

  if (!IsExistFrameworkunifiedLogCfgNv()) {  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
    copyFrameworkunifiedLogCfgFileToNv();  // LCOV_EXCL_BR_LINE 11:expect branch
  }

  l_realTimelog = FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF;
  l_logLevelArray = NULL;

  readConfig(&l_realTimelog, &l_logLevelArray);  // for make ramd data
  if (l_logLevelArray == NULL) {
    return;
  }

  if (!IsExistFrameworkunifiedLogCfgShmem()) {
    WriteSharedMem(WTYPE_NORMAL, l_realTimelog, g_FlagIdNum, l_logLevelArray);
    MapShmInfo(g_FrameworkunifiedLogFlagId);
  }

  if (flag_id >= g_FlagIdNum) {
    if (l_logLevelArray != NULL) {
      free(l_logLevelArray);
    }
    return;
  }

  if (mode == FRAMEWORKUNIFIEDLOG_FLAG_MODE_DEBUG) {
    l_value = "Debug";
    l_logLevelArray[flag_id] = FRAMEWORKUNIFIEDLOG_FLAG_MODE_DEBUG;
  } else {
    l_value = "Release";
    l_logLevelArray[flag_id] = FRAMEWORKUNIFIEDLOG_FLAG_MODE_RELEASE;
  }
  writeUpdateConfigForString(g_flag_name[flag_id], l_key, l_value);  // LCOV_EXCL_BR_LINE 11:expect branch
  WriteSharedMemForFlagId(l_realTimelog, g_FlagIdNum, flag_id, l_logLevelArray[flag_id]);  // LCOV_EXCL_BR_LINE 11:expect branch

  free(l_logLevelArray);
}

//////////////////////////////////////////
// NsLogGetFrameworkunifiedLogFlag for Diag Method
//////////////////////////////////////////
EFrameworkunifiedStatus
NsLogGetFrameworkunifiedLogFlag(UI_8 flag_id, UI_8 *mode) {
  int l_realTimeLog;
  int l_logLevel = 0;
  int *l_logLevelArray;

  if (mode == NULL) {
    return eFrameworkunifiedStatusFail;
  }

  l_realTimeLog = FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF;
  l_logLevelArray = NULL;

  readConfig(&l_realTimeLog, &l_logLevelArray);  // for make ramd data
  if (l_logLevelArray == NULL) {
    *mode = FRAMEWORKUNIFIEDLOG_FLAG_MODE_RELEASE;
    return eFrameworkunifiedStatusFail;
  }

  if (!IsExistFrameworkunifiedLogCfgShmem()) {
    WriteSharedMem(WTYPE_NORMAL, l_realTimeLog, g_FlagIdNum, l_logLevelArray);
    MapShmInfo(g_FrameworkunifiedLogFlagId);
  }

  if (l_logLevelArray != NULL) {
    free(l_logLevelArray);
  }

  if ((flag_id >= g_FlagIdNum)) {
    return eFrameworkunifiedStatusFail;
  }

  GetShmInfo(flag_id, &l_realTimeLog, &l_logLevel);
  *mode = static_cast<UI_8>(l_logLevel);

  return eFrameworkunifiedStatusOK;
}

//////////////////////////////////////////
// NsLogSetRealtimeLog for Diag Method
//////////////////////////////////////////
VOID
NsLogSetRealtimeLog(UI_8 mode) {
  int l_realTimelog = FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF;
  int *l_logLevelArray;
  std::string l_key = "REALTIME_LOG.Output";
  std::string l_value = "OFF";  // LCOV_EXCL_BR_LINE 11:expect branch

  if (mode == FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_FREEZE) {
    WriteSharedMem(WTYPE_RTIMELOG, FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_FREEZE, g_FlagIdNum, NULL);  // LCOV_EXCL_BR_LINE 11:expect branch
    return;
  }

  if ((mode >= FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_MAX)
      && (mode != FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB_DISABLE)) {
    return;
  }

  if (!IsExistFrameworkunifiedLogCfgNv()) {  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
    copyFrameworkunifiedLogCfgFileToNv();  // LCOV_EXCL_BR_LINE 11:expect branch
  }

  l_logLevelArray = NULL;
  readConfig(&l_realTimelog, &l_logLevelArray);    // LCOV_EXCL_BR_LINE 11:expect branch
  if (l_logLevelArray == NULL) {
    return;
  }

  if (!IsExistFrameworkunifiedLogCfgShmem()) {
    WriteSharedMem(WTYPE_NORMAL, l_realTimelog, g_FlagIdNum, l_logLevelArray);
    MapShmInfo(g_FrameworkunifiedLogFlagId);
  }

  if (mode == FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_UART) {
    l_value = "UART";
    l_realTimelog = FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_UART;
  } else if ((mode == FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB)
             || (mode == FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB_DISABLE)) {
    l_value = "USB";
    l_realTimelog = mode;
  } else {
    l_value = "OFF";
    l_realTimelog = FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF;
  }

  writeUpdateConfig(l_key, l_value);  // LCOV_EXCL_BR_LINE 11:expect branch
  if (l_logLevelArray != NULL) {
    WriteSharedMemForFlagId(l_realTimelog, g_FlagIdNum, FRAMEWORKUNIFIEDLOG_DEFAULT_FLAG_ID,
                          l_logLevelArray[FRAMEWORKUNIFIEDLOG_DEFAULT_FLAG_ID]);  // LCOV_EXCL_BR_LINE 11:expect branch
    free(l_logLevelArray);
  }
}

//////////////////////////////////////////
// NsLogGetRealtimeLog for Diag
//////////////////////////////////////////
VOID
NsLogGetRealtimeLog(UI_8 *mode) {
  int l_realTimelog;
  int l_logLevel;
  int *l_logLevelArray;

  if (mode == NULL) {
    return;
  }
  l_realTimelog = FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF;
  l_logLevelArray = NULL;

  readConfig(&l_realTimelog, &l_logLevelArray);  // for make ramd data
  if (l_logLevelArray == NULL) {
    *mode = FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF;
    return;
  }
  if (!IsExistFrameworkunifiedLogCfgShmem()) {
    WriteSharedMem(WTYPE_NORMAL, l_realTimelog, g_FlagIdNum, l_logLevelArray);
    MapShmInfo(g_FrameworkunifiedLogFlagId);
  }
  if (l_logLevelArray != NULL) {
    free(l_logLevelArray);
  }

  GetShmInfo(g_FrameworkunifiedLogFlagId, &l_realTimelog, &l_logLevel);
  *mode = static_cast<UI_8>(l_realTimelog);
}

//////////////////////////////////////////
// NSLogPrintSysEventLog
//////////////////////////////////////////
VOID NSLogSysEvent(const UI_16 f_ui16Line, PCSTR f_cFuncName, PCSTR __format, ...) {
  va_list arg;
  CHAR l_cFormat[MAX_QUEUE_MSG_SIZE] = {};
  CHAR l_cTimeFormat[MAX_TIME_FORMATSTR_LEN] = {};
  UI_64 l_ui64CurrentTime = 0;
  static UI_64 cps = 0;
  static UI_64 l_ui64GapInUS = 0;

  if (cps == 0) {
    cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
  }
  if (g_hNSSysEventLogQ == INVALID_HANDLE) {
    g_hNSSysEventLogQ = OpenSender(NS_SYSEVENTLOGQ);
  }
  if (l_ui64GapInUS == 0) {
    FILE *l_fpPLogGap = NULL;
    l_fpPLogGap = fopen(PLOG_GAP_FILE, "re");
    if (l_fpPLogGap != NULL) {
      CHAR l_cBuffer[MAX_GAP_BUFFER_LEN] = {};
      if (fgets(l_cBuffer, MAX_GAP_BUFFER_LEN, l_fpPLogGap) != NULL) {
        l_ui64GapInUS = static_cast<UI_64>(atoll(l_cBuffer));
      }
      fclose(l_fpPLogGap);
    }
  }

  if (g_hNSSysEventLogQ != INVALID_HANDLE) {
    CHAR l_cMsg[MAX_QUEUE_MSG_SIZE] = {};
    // build a format string
#ifdef AGL_PosixBasedOS001LEGACY_USED
    l_ui64CurrentTime = (ClockCycle() / (cps / 1000000)) + l_ui64GapInUS;
#endif
    snprintf(l_cTimeFormat, MAX_TIME_FORMATSTR_LEN, "%s", "us");

    va_start(arg, __format);
    vsnprintf(l_cFormat, MAX_PLOG_STRING_SIZE - 1, __format, arg);
    l_cFormat[MAX_PLOG_STRING_SIZE - 1] = '\0';
    va_end(arg);
    UI_32 l_uiCharsWritten =
    snprintf(l_cMsg, MAX_QUEUE_MSG_SIZE - 1,
            "%12llu[%s]:%s %s: %s: %s\n",
            static_cast<long long unsigned int>(l_ui64CurrentTime),  // NOLINT  (readability/nolint)
            l_cTimeFormat,
            (l_ui64GapInUS > 0) ? ("o") : ("x"),
            tls_strProcessName != 0 ? tls_strProcessName : NULL,
            f_cFuncName != 0 ? f_cFuncName : NULL, l_cFormat);

    if (SendMessage(g_hNSSysEventLogQ, l_uiCharsWritten + 1,
        reinterpret_cast<void *>(const_cast<char *>(l_cMsg)))
        != eFrameworkunifiedStatusOK) {
      // Note: Nothing will be printed on a failure.
    }
  }
}

/////////////////////////////////////////
// NSLogEnableSysEventLog
//////////////////////////////////////////
VOID
NSLogEnableSysEventLog(BOOL f_bEnable) {
  g_FrameworkunifiedLogSysEventParams.bIsSysEventLogEnabled = f_bEnable;
}

//////////////////////////////////////////
// NSLogIsSysEventLogEnabled
//////////////////////////////////////////
BOOL
NSLogIsSysEventLogEnabled(void) {
  return g_FrameworkunifiedLogSysEventParams.bIsSysEventLogEnabled;
}

//////////////////////////////////////////
// NsLogGetZoneTextList
//////////////////////////////////////////
VOID
NsLogGetZoneTextList(CHAR f_cZoneList[][ZONE_TEXT_SIZE]) {
  if (f_cZoneList != NULL) {
    UI_32 l_ui32CharsToWrite = ZONE_TEXT_SIZE - 1;
    // first fill ns zone text list
    strncpy(f_cZoneList[0], ZONE_TEXT_0, l_ui32CharsToWrite);
    strncpy(f_cZoneList[1], ZONE_TEXT_1, l_ui32CharsToWrite);
    strncpy(f_cZoneList[2], ZONE_TEXT_2, l_ui32CharsToWrite);
    strncpy(f_cZoneList[3], ZONE_TEXT_3, l_ui32CharsToWrite);
    strncpy(f_cZoneList[4], ZONE_TEXT_4, l_ui32CharsToWrite);
    strncpy(f_cZoneList[5], ZONE_TEXT_5, l_ui32CharsToWrite);
    strncpy(f_cZoneList[6], ZONE_TEXT_6, l_ui32CharsToWrite);
    strncpy(f_cZoneList[7], ZONE_TEXT_7, l_ui32CharsToWrite);
    strncpy(f_cZoneList[8], ZONE_TEXT_8, l_ui32CharsToWrite);
    strncpy(f_cZoneList[9], ZONE_TEXT_9, l_ui32CharsToWrite);

    // fill user zone text list
    UI_32 l_ui32Count = 10;  // start of user zones
    UI_32 l_ui32ZoneListCount = 0;
    for (; l_ui32Count < BITS_IN_ZONE_MASK; l_ui32Count++, l_ui32ZoneListCount++) {
      strncpy(f_cZoneList[l_ui32Count], g_LocalFrameworkunifiedLogParams.cZones[l_ui32ZoneListCount], l_ui32CharsToWrite);
    }
  }
}

VOID
NsForceClose(void) {
  if (!syslogopened) {
    closelog();
    syslogopened = 0;
  }

  if (shm_pmap != reinterpret_cast<void *>(const_cast<unsigned int*>(default_log_cfgtbl))) {
    int size;
    size = static_cast<int>(sizeof(int) * 2);  // realTimeLog + flagIdNum
    size += static_cast<int>(sizeof(int) * g_FlagIdNum);
    munmap(shm_pmap, size);
    shm_pmap = reinterpret_cast<void *>(const_cast<unsigned int*>(default_log_cfgtbl));
  }

  if (shm_fd != -1) {
    close(shm_fd);
    shm_fd = -1;
  }

  if (g_qid != (mqd_t)(-1)) {
    mq_close(g_qid);
    g_qid = (mqd_t)(-1);
  }

  TFrameworkunifiedLogMap::iterator itr;
  TFrameworkunifiedLogMap::iterator itr_end;
  UI_32 i = 0;
  unsigned int flag_id;
  if (!g_FrameworkunifiedLogMap.empty()) {
    itr = g_FrameworkunifiedLogMap.begin();
    itr_end = g_FrameworkunifiedLogMap.end();
    for (; itr != itr_end; itr++, i++) {
      if (itr->second.ring_buffer != NULL) {
        delete itr->second.ring_buffer;  // LCOV_EXCL_BR_LINE 11:expect branch
        itr->second.ring_buffer = NULL;
      }
    }
    g_FrameworkunifiedLogMap.clear();
  }

  if (!g_FrameworkunifiedLogFlagMap.empty()) {
    for (flag_id = 0; flag_id < g_FlagIdNum; flag_id++) {
      itr = g_FrameworkunifiedLogFlagMap.find(flag_id);
      if (itr->second.ring_buffer != NULL) {
        delete itr->second.ring_buffer;  // LCOV_EXCL_BR_LINE 11:expect branch
        itr->second.ring_buffer = NULL;
      }
    }
    g_FrameworkunifiedLogFlagMap.clear();
  }

  if (!g_flag_name.empty()) {
    g_flag_name.clear();
  }
  g_FrameworkunifiedLogFlagId  = FRAMEWORKUNIFIEDLOG_DEFAULT_FLAG_ID;
  g_FlagIdNum = 0;
}  // LCOV_EXCL_BR_LINE 10:The final line
