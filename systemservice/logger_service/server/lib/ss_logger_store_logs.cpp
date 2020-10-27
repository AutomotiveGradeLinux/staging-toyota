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
/// \ingroup  tag_SystemServices_Logger
/// \brief    This file supports the logger service interface for SystemManager.
///
///////////////////////////////////////////////////////////////////////////////
#include "system_service/ss_logger_store_logs.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libtar_listhash.h>
#include <libtar.h>
#include <zlib.h>
#include <sys/klog.h>

#include <stdlib.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <system_service/ss_system_types.h>
#include <native_service/ns_ringbuffer.h>
#include <native_service/ns_logger_if.h>
#include <native_service/ns_config_parser_if.h>
#include <native_service/cl_lockid.h>
#include <stub/Clock_API.h>
#include <vector>
#include <sstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include "ss_logger_store_logs_loggerservicelog.h"
#include "ss_logger_cfg.h"

#define LOGGERSERVICELOG_CONFIG_PATH       "/usr/agl/conf/BS/ss/logger_service/rodata/ss_logger.cfg"
#define LOGGER_TMP_FN            "__LOGGER.tmp"
#define LOGGERSERVICELOG_TMP_OUTPUT_PATH   "/tmp"
#define PSTORE_DIR_PATH          "/dev/pstore"
#define KERNEL_BOOT_LOG_FN       "kernelboot.log"
#define KERNEL_LOG_FN            "kernel.log"
#define COMMUNICATION_DIR_NAME   "communication"
#define ACCOFFON_DIR_NAME        "accoffon"

#define PATHNAME_SEPARATOR       '/'
#define PATHNAME_SEPARATOR_STR   "/"
#define SS_STORE_FILENAME_SYSILG_TERM "_SYS_ILG_RESET.tar.gz"
#define SS_STORE_FILENAME__CWORD52__TERM "__CWORD52_.log"
#define LOGARCHIVE_FILENAME_STORE_ARCHIVE "/tmp/ss_logger_ilgfilenum"
#define TMPFILE_MODE                     0640
#define SSLOGGER_PATHNAME                (255 + 1)
#define FILENUMBER_UPPER_LIMIT_OVER      100000u
#define FILENUMBER_LOWER_LIMIT           1u
#define FILENUMBER_DIGIT_NUM             5
#define INVALID_FILENUMBER               (FILENUMBER_UPPER_LIMIT_OVER)


/* Read all messages remaining in the ring buffer. (allowed for non-root) */
#define SYSLOG_ACTION_READ_ALL       3
/* Return size of the log buffer */
#define SYSLOG_ACTION_SIZE_BUFFER   10

#define ENABLE_DEBUG_LOG 0
#if ENABLE_DEBUG_LOG
#define DEB_LOGGERSERVICELOG(zone,funcname,print_fmt, ...) if(zone!=ZONE_FUNC) \
fprintf(stderr,"%d/%s/%d:="print_fmt"\n", zone, funcname, __LINE__,##__VA_ARGS__)
#else
#define DEB_LOGGERSERVICELOG(zone,funcname,print_fmt, ...)
#endif

typedef struct {
  std::string output_path;
  SI_32 output_max;
  SI_32 output_errmax;
  SI_32 external_log_num;
  SI_32 external_log_max;
} CfgInfo;

typedef struct {
  std::string file_path;
  std::string dest_name;
  BOOL tmp_file_delete;
} LogInfo;

typedef struct {
  std::string path;
  int index;
} LoggerservicelogInfo;


static const char* sys_illegal_log[] = {
                                               "frameworkunified_sys.log",
                                       };


typedef enum {
  SS_STORELOGS_KERNEL_LOG = 0,
  SS_STORELOGS_KERNEL_BOOT_LOG
} SS_STORELOGS_KERNEL_LOG_TYPE;

typedef void (*GetLogFileFunc)(SS_STORELOGS_OPE_TYPE type,
                               std::vector<LogInfo>& list);  // NOLINT (runtime/references)

static const UI_8 MAX_DIRECTORY_DEPTH = 4;
static const UI_32 TARMODE = 0644;

static EFrameworkunifiedStatus addToArchive(TAR* f_pTarInfo, std::string f_filePathAndName,
                               std::string f_destinationName, UI_8 f_depth =
                                 MAX_DIRECTORY_DEPTH);
static EFrameworkunifiedStatus addDirectoryToArchive(TAR* f_pTarInfo, std::string f_path,
                                        std::string f_destinationName,
                                        UI_8 f_depth = MAX_DIRECTORY_DEPTH);
static SI_32 gzOpenArchive(PCHAR f_pcPathname, SI_32 f_siOflags,
                           SI_32 f_siMode);
static SI_32 gzCloseArchive();
static ssize_t gzWriteArchive(int fd, const void* buf, size_t count);
static ssize_t gzReadArchive(int fd, void* buf, size_t count);
static EFrameworkunifiedStatus addFileToArchive(TAR* f_pTarInfo, std::string f_filename,
                                   std::string f_destinationName);
static EFrameworkunifiedStatus addAllFileToArchive (TAR* f_pTarInfo, std::string collectDir);
static EFrameworkunifiedStatus copyFile(std::string f_source, std::string f_destination);
static EFrameworkunifiedStatus copyDirectory (std::string f_source, std::string f_destination);
static EFrameworkunifiedStatus copyUntyped (std::string f_source, std::string f_destination);
static EFrameworkunifiedStatus copyFileToAccOffOnDir(std::vector<LogInfo>& list, SS_STORELOGS_OPE_TYPE type);
static void getLoggerservicelogFile(SS_STORELOGS_OPE_TYPE type,
                          std::vector<LogInfo>& list,  // NOLINT (runtime/references)
                          std::vector<LoggerservicelogInfo> &loggerservicelogList);  // NOLINT (runtime/references)
static void getKernellogFile(SS_STORELOGS_OPE_TYPE type,
                             std::vector<LogInfo>& list);  // NOLINT (runtime/references)
static void getLastKernelLog(std::vector<LogInfo>& list);  // NOLINT (runtime/references)
static void getKernelLog(std::vector<LogInfo>& list);  // NOLINT (runtime/references)
static void getKernelBootLog(SS_STORELOGS_OPE_TYPE type,
                             std::vector<LogInfo>& list);  // NOLINT (runtime/references)
static void getCommunicationlogFile(SS_STORELOGS_OPE_TYPE type,
                                    std::vector<LogInfo>& list);  // NOLINT (runtime/references)
static void getExternallogFile(SS_STORELOGS_OPE_TYPE type,
                             std::vector<LogInfo>& list);  // NOLINT (runtime/references)

//static std::string getFileName(std::string &path, uint32_t f_time,  // NOLINT (runtime/references)
//                               SS_STORELOGS_OPE_TYPE type);
static std::string getFileName(std::string &path, uint32_t f_time,  // NOLINT (runtime/references)
                               SS_STORELOGS_OPE_TYPE type, UI_32 l_counter);

static void getEmmcCounter(std::string &path, UI_32& f_counter);  // NOLINT (runtime/references)
static void getConfigInfo(CfgInfo& f_cfginfo);  // NOLINT (runtime/references)
static EFrameworkunifiedStatus deleteOldEmmcLog(CfgInfo& f_cfginfo,  // NOLINT (runtime/references)
                                   std::string& f_archive_destination,  // NOLINT (runtime/references)
                                   SS_STORELOGS_OPE_TYPE type);
static EFrameworkunifiedStatus storeLogs(SS_STORELOGS_OPE_TYPE type);
static EFrameworkunifiedStatus deleteOldLog(std::string log_path,
                               std::vector<std::string>& l_vector,  // NOLINT (runtime/references)
                               std::string& f_archive_destination,  // NOLINT (runtime/references)
                               UI_32 max_num);
static size_t deleteRequestLogs(std::string f_emmcpath, ELOGGERSERVICELOGTYPE f_reqtype,
                                size_t f_reqsize, ELOGGERSERVICELOGTYPE f_deltype);
static EFrameworkunifiedStatus forceDeleteOldLog(std::string f_archive_destination,
                                    size_t f_req_size);
static ELOGGERSERVICELOGTYPE QueryLoggerservicelogType(std::string f_logname);
static void SyncDir(std::string f_dir_path);
static BOOL RemoveSubDirectory(std::string &f_cPath);  // NOLINT (runtime/references)
static void CreateSaveFinFlag(SS_STORELOGS_OPE_TYPE type);
static void ClearLoggerservicelogBuffer(std::vector<LoggerservicelogInfo> &loggerservicelogList);  // NOLINT (runtime/references)
static void copyLoggerservicelogFile(std::string distPath);
static void deleteTmpAccOffOn(std::string accOffOnDir);
static void deleteAccOffOnDir(std::string deletePath);
static void deleteAccOffOnUntyped(std::string deletePath);

static EFrameworkunifiedStatus getFileNumber(const std::string &filename,UI_32 &number);
static bool compareFileNumber(UI_32 filename1_number,const std::string &filename2);
static EFrameworkunifiedStatus removeFiles(const std::string &log_path, std::vector<std::string> &l_vector,
                              size_t begin_pos, size_t end_pos);
static EFrameworkunifiedStatus removeSameNumFiles(std::vector<std::string>::iterator &itr, const std::string &log_path,
                                     std::vector<std::string> &l_vector);
static EFrameworkunifiedStatus deleteOldLogAbnrm(const std::string& log_path, std::vector<std::string>& l_vector,
                                    const std::string& f_archive_destination, SI_32 max_num, UI_32 abnrm_total);
static EFrameworkunifiedStatus storeTempArchive(const std::string &real_name);
static EFrameworkunifiedStatus getFileNumberFromTempArchive(UI_32 &filenumber);


static std::string m_pathAndName;  // NOLINT (runtime/string)
static std::string m_tmpName;  // NOLINT (runtime/string)

const GetLogFileFunc GetLogFuncTbl[] = { getKernellogFile,
                                         getCommunicationlogFile,
                                         getExternallogFile
                                       };
// GZF save variables
static gzFile g_gztype;

// ->Bus-Error avoidance
static tartype_t l_gztype;

static EFrameworkunifiedStatus openArchive(TAR** f_pTarInfo, std::string f_pathAndName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

// ->Bus-Error avoidance (because of local variables, content becomes indeterminate when functions exit)
//  tartype_t l_gztype;

  *f_pTarInfo = NULL;
  g_gztype = NULL;
  l_gztype.openfunc = (openfunc_t) &gzOpenArchive;
  l_gztype.closefunc = (closefunc_t) &gzCloseArchive;
  l_gztype.readfunc = (readfunc_t) &gzReadArchive;
  l_gztype.writefunc = (writefunc_t) &gzWriteArchive;

  m_pathAndName = f_pathAndName;

  /* check if directory exists */
  size_t l_found = f_pathAndName.find_last_of('/');
  std::string l_path = f_pathAndName.substr(0, l_found);

  if (0 != access(l_path.c_str(), F_OK)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Directory does not exist. Creating...");
    return eFrameworkunifiedStatusFail;
  }

  /* create archive in tmp */
  m_tmpName = "/tmp/log.tar.gz";

  int l_tarRetVal = tar_open(f_pTarInfo, const_cast<char*>(m_tmpName.c_str()), &l_gztype,
                              O_WRONLY | O_CREAT, TARMODE, TAR_GNU);

  if ((l_tarRetVal != 0) || (*f_pTarInfo == NULL) || g_gztype == NULL) {  // LCOV_EXCL_BR_LINE 6: tar_open must return ok  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 6: tar_open must return ok
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Could not open TAR-Archive!");
    l_eFrameworkunifiedStatus = eFrameworkunifiedStatusAccessError;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eFrameworkunifiedStatus);
}

static EFrameworkunifiedStatus addToArchive(TAR* f_pTarInfo, std::string f_filePathAndName,
                               std::string f_destinationName, UI_8 f_depth) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusFileLoadError;
  struct stat l_stat;
  int l_statRetVal;

  if (0 != access(f_filePathAndName.c_str(), F_OK)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. The specified file path and name does not exist: %s",
           f_filePathAndName.c_str());
  } else if (0 != (l_statRetVal = stat(f_filePathAndName.c_str(), &l_stat))) {  // LCOV_EXCL_BR_LINE 5: stat's error case  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 5: stat's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. stat() for file or directory returned error: %s -> %d",
           f_filePathAndName.c_str(), l_statRetVal);
    // LCOV_EXCL_STOP
  } else if (0 != S_ISDIR(l_stat.st_mode)) {
    if (0 < f_depth) {  // LCOV_EXCL_BR_LINE 6: f_depth must be greater than 0
      l_eFrameworkunifiedStatus = addDirectoryToArchive(f_pTarInfo, f_filePathAndName,
                                           f_destinationName, f_depth);
    } else {
      // LCOV_EXCL_START 6: f_depth must be greater than 0
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
             "Maximum recursive depth reached! File %s not added to archive.",
             f_filePathAndName.c_str());
      // LCOV_EXCL_STOP
    }
  } else if (0 != S_ISREG(l_stat.st_mode)) {
    char path[PATH_MAX];
    if (realpath(f_filePathAndName.c_str(), path) != NULL) {
      f_filePathAndName = path;
      l_eFrameworkunifiedStatus = addFileToArchive(f_pTarInfo, f_filePathAndName,
                                      f_destinationName);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
             "Archive not opened or could not access file: \"%s\"",
             f_filePathAndName.c_str());
    }
  } else {
    FRAMEWORKUNIFIEDLOG(
      ZONE_ERR,
      __FUNCTION__,
      " Error. specified file may be a symbolic link or other non 'regular' type. File: %s, mode: 0x%04X.",
      f_filePathAndName.c_str(), l_stat.st_mode);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eFrameworkunifiedStatus);
}


static EFrameworkunifiedStatus getFileNumberFromTempArchive(UI_32 &filenumber)
{
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  char      buf[FILENUMBER_DIGIT_NUM + 1];
  int       fd;
  int       result;
  EFrameworkunifiedStatus    l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  fd = open(LOGARCHIVE_FILENAME_STORE_ARCHIVE, O_RDONLY);
  if (fd == -1) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR,__FUNCTION__," Error. open(). path = %s ,errno = %d", LOGARCHIVE_FILENAME_STORE_ARCHIVE, errno);
    return eFrameworkunifiedStatusAccessError;
  }

  result = read(fd, buf, FILENUMBER_DIGIT_NUM);
  if (result != FILENUMBER_DIGIT_NUM) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR,__FUNCTION__," Error. read(). path = %s ,result = %d, errno = %d",
           LOGARCHIVE_FILENAME_STORE_ARCHIVE, result, errno);
    l_eFrameworkunifiedStatus  = eFrameworkunifiedStatusAccessError;
  } else {
    char    *endptr;
    int      error_number;
    int      temp_value;
    buf[FILENUMBER_DIGIT_NUM]  = '\x0';
    errno         = EOK;
    temp_value    = strtol(buf, &endptr, 10);
    error_number  = errno;
    if ((error_number != EOK) || (*endptr != '\x0')) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR,__FUNCTION__," Error. Failed to comvert string. source = %s ,errno = %d", buf, errno);
      l_eFrameworkunifiedStatus  = eFrameworkunifiedStatusInvldParam;
    } else {
      filenumber = temp_value;
    }
  }
  result = close(fd);
  if (result != 0) {  // LCOV_EXCL_BR_LINE 6: close() must return ok
    // LCOV_EXCL_START 6: close() must return ok
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR,__FUNCTION__," Error. close(). path = %s ,errno = %d", LOGARCHIVE_FILENAME_STORE_ARCHIVE, errno);
    // LCOV_EXCL_STOP
  }
  result = remove(LOGARCHIVE_FILENAME_STORE_ARCHIVE);
  if (result != 0) {  // LCOV_EXCL_BR_LINE 6: remove() must return ok
    // LCOV_EXCL_START 6: remove() must return ok
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR,__FUNCTION__," Error. remove(). path = %s ,errno = %d", LOGARCHIVE_FILENAME_STORE_ARCHIVE, errno);
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eFrameworkunifiedStatus;
}

static EFrameworkunifiedStatus storeTempArchive(const std::string &real_name)
{
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::string filename;
  size_t      pos;
  int         fd;
  int         result;
  const char  *p_real_filename;
  EFrameworkunifiedStatus  l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  fd = open(LOGARCHIVE_FILENAME_STORE_ARCHIVE, O_CREAT | O_WRONLY, TMPFILE_MODE);
  if (fd == -1) {  // LCOV_EXCL_BR_LINE 5: open's error case
    // LCOV_EXCL_START 5: open's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR,__FUNCTION__," Error. open(). path = %s ,errno = %d", LOGARCHIVE_FILENAME_STORE_ARCHIVE, errno);
    return eFrameworkunifiedStatusAccessError;
    // LCOV_EXCL_STOP
  }

  pos = real_name.find_last_of(PATHNAME_SEPARATOR);
  if (pos == std::string::npos) {  // LCOV_EXCL_BR_LINE 6: there must be '/' in string real_name
    // LCOV_EXCL_START 6: there must be '/' in string real_name
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    p_real_filename  = real_name.c_str();
    // LCOV_EXCL_STOP
  } else {
    p_real_filename  = &real_name[pos + 1];
  }

  result = write(fd, p_real_filename, FILENUMBER_DIGIT_NUM);
  if (result != FILENUMBER_DIGIT_NUM) {  // LCOV_EXCL_BR_LINE 5: write's error case
    // LCOV_EXCL_START 5: write's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR,__FUNCTION__," Error. write(). path = %s ,result = %d, errno = %d",
           LOGARCHIVE_FILENAME_STORE_ARCHIVE, result, errno);
    l_eFrameworkunifiedStatus = eFrameworkunifiedStatusAccessError;
    // LCOV_EXCL_STOP
  }

  result = close(fd);
  if (result != 0) {  // LCOV_EXCL_BR_LINE 5: close's error case
    // LCOV_EXCL_START 5: close's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR,__FUNCTION__," Error. close(). path = %s ,errno = %d", LOGARCHIVE_FILENAME_STORE_ARCHIVE, errno);
    l_eFrameworkunifiedStatus = eFrameworkunifiedStatusAccessError;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eFrameworkunifiedStatus;
}



//static EFrameworkunifiedStatus closeArchive(TAR* f_pTarInfo) {
static EFrameworkunifiedStatus closeArchive(TAR* f_pTarInfo, SS_STORELOGS_OPE_TYPE type) {

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (0 != tar_append_eof(f_pTarInfo)) {  // LCOV_EXCL_BR_LINE 5: tar_append_eof's error case
    // LCOV_EXCL_START 5: tar_append_eof's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "tar_append_eof()");
    l_eFrameworkunifiedStatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }
  if (0 != tar_close(f_pTarInfo)) {  // LCOV_EXCL_BR_LINE 5: tar_close's error case
    // LCOV_EXCL_START 5: tar_close's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Could not close archive");
    l_eFrameworkunifiedStatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }

  if (eFrameworkunifiedStatusOK == l_eFrameworkunifiedStatus) {  // LCOV_EXCL_BR_LINE 6: l_eFrameworkunifiedStatus must be eFrameworkunifiedStatusOK
    l_eFrameworkunifiedStatus = copyFile(m_tmpName, m_pathAndName);
    if (eFrameworkunifiedStatusOK == l_eFrameworkunifiedStatus) {

      if (type == SS_STORELOGS_ILLEGAL) {
        l_eFrameworkunifiedStatus = storeTempArchive(m_pathAndName);
        if (eFrameworkunifiedStatusOK != l_eFrameworkunifiedStatus) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error. Failed to store temporary file.");
        }
      }

      l_eFrameworkunifiedStatus =
        (0 == remove(m_tmpName.c_str())) ? eFrameworkunifiedStatusOK : eFrameworkunifiedStatusFault;
      if (eFrameworkunifiedStatusOK != l_eFrameworkunifiedStatus) {  // LCOV_EXCL_BR_LINE 5: remove's error case
        // LCOV_EXCL_START 5: remove's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(
          ZONE_WARN,
          __FUNCTION__,
          " Warning. Failed to remove temporary archive. Will overwrite on next event");
        // LCOV_EXCL_STOP
      }
    } else if (eFrameworkunifiedStatusErrNoEAGAIN == l_eFrameworkunifiedStatus) {  // LCOV_EXCL_BR_LINE 4: no space error case
      // LCOV_EXCL_START 4: no space error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      // disk space full
      struct stat st_buf;
      size_t req_size;
      if (stat(m_tmpName.c_str(), &st_buf) != -1) {
        req_size = st_buf.st_size;
        if (eFrameworkunifiedStatusOK == forceDeleteOldLog(m_pathAndName, req_size)) {
          l_eFrameworkunifiedStatus = copyFile(m_tmpName, m_pathAndName);
          if (eFrameworkunifiedStatusOK != l_eFrameworkunifiedStatus) {
            FRAMEWORKUNIFIEDLOG(
              ZONE_ERR,
              __FUNCTION__,
              "Try Again but Error. Failed to copy to destination.%s, size=%d",
              m_pathAndName.c_str(), req_size);
          }
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Error. force delete failed.%s, size=%d",
                 m_pathAndName.c_str(), req_size);
        }
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " emmc full: dest=%s, size=%d",
               m_pathAndName.c_str(), req_size);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Warning. Failed to get tmp log file stat. dest=%s",
               m_pathAndName.c_str());
      }
      // LCOV_EXCL_STOP
    } else {
      /* should we copy lost archive to emmc?? */
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. Failed to copy to destination. %s", m_pathAndName.c_str());
    }
    remove(m_tmpName.c_str());
    m_tmpName = "";
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eFrameworkunifiedStatus);
}

static EFrameworkunifiedStatus addFileToArchive(TAR* f_pTarInfo, std::string f_filename,
                                   std::string f_destinationName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK; /* check if Tar is open and if file can be read */

  if ((NULL != f_pTarInfo) && (0 == access(f_filename.c_str(), R_OK))
      && (0 != f_destinationName.length())) {
    int l_tarRetVal = tar_append_file(f_pTarInfo, const_cast<char*>(f_filename.c_str()),
                                      const_cast<char*>(f_destinationName.c_str()));
    if (0 != l_tarRetVal) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Could not add file \'%s\' to tar archive.", f_filename.c_str());
      l_eFrameworkunifiedStatus = eFrameworkunifiedStatusAccessError;
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Archive not opened or could not access file: \"%s\"",
           f_filename.c_str());
    l_eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eFrameworkunifiedStatus);
}

static EFrameworkunifiedStatus addAllFileToArchive (TAR* f_pTarInfo, std::string collectDir)
{
  struct dirent l_pDirent;
  struct dirent* next;
  EFrameworkunifiedStatus l_eStatus  = eFrameworkunifiedStatusFail;
  DEB_LOGGERSERVICELOG(ZONE_FUNC, __FUNCTION__, "+");
  DIR * l_pDir = opendir(collectDir.c_str());
  if (l_pDir == NULL) {
    DEB_LOGGERSERVICELOG(ZONE_ERR, __FUNCTION__, "Could not open directory: %s", collectDir.c_str());
  } else {
    while (0 == readdir_r(l_pDir,&l_pDirent,&next) && next != NULL) {
      if ( ( 0   != strcmp( l_pDirent.d_name, "."  ) ) &&
      ( 0   != strcmp( l_pDirent.d_name, ".." ))  &&
      ( 0   != strcmp( l_pDirent.d_name, "lost+found" ))  &&
      ( '.' != l_pDirent.d_name[0] ) ) {
        std::string l_fileSource = collectDir;
        l_fileSource.append("/");
        l_fileSource.append(l_pDirent.d_name);
        std::string l_fileDestination = l_pDirent.d_name;
        l_eStatus = addToArchive(f_pTarInfo,l_fileSource,l_fileDestination);
        if (l_eStatus != eFrameworkunifiedStatusOK) {
          break;
        }
      }
    }
    closedir (l_pDir);
  }
  DEB_LOGGERSERVICELOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

static EFrameworkunifiedStatus addDirectoryToArchive(TAR* f_pTarInfo, std::string f_path,
                                        std::string f_destinationName,
                                        UI_8 f_depth) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  DIR *l_pDir = opendir(f_path.c_str());
  if (l_pDir != NULL) {
    struct dirent* l_pDirent;
    while ((l_pDirent = readdir(l_pDir)) != NULL) {
      if ((0 != strcmp(l_pDirent->d_name, ".")) && /* Ignore special  . directory.                     */
          (0 != strcmp(l_pDirent->d_name, "..")) && /* Ignore special .. directory.                     */
          ('.' != l_pDirent->d_name[0])) {  /* Ignore hidden files                              */
        std::string l_extension = "/";
        std::string l_fileName = f_path;
        std::string l_destName = f_destinationName;
        l_extension.append(l_pDirent->d_name);
        l_fileName.append(l_extension);
        l_destName.append(l_extension);
        l_eFrameworkunifiedStatus = addToArchive(f_pTarInfo, l_fileName, l_destName,
                                    static_cast<UI_8>(f_depth - 1));
      }
    }
    closedir(l_pDir);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eFrameworkunifiedStatus);
}

static SI_32 gzOpenArchive(PCHAR f_pcPathname, SI_32 f_siOflags,
                           SI_32 f_siMode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  SI_32 l_siFileDescriptor = -1;

  if (f_pcPathname) {
    PCHAR l_pcGzoflags;

    switch (f_siOflags & O_ACCMODE) {  // LCOV_EXCL_BR_LINE 200: only O_WRONLY be set
    case O_WRONLY:
      l_pcGzoflags = (PCHAR) "wb";
      break;
    // LCOV_EXCL_START 200: only O_WRONLY be set
    case O_RDONLY:
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_pcGzoflags = (PCHAR) "rb";
      break;
    // LCOV_EXCL_STOP
    case O_RDWR:
    default:
      return -1;
    }
    if (-1 == (l_siFileDescriptor = open(f_pcPathname, f_siOflags, f_siMode))) {
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
      return -1;
    }

    if ((f_siOflags & O_CREAT) && fchmod(l_siFileDescriptor, f_siMode)) {
      close(l_siFileDescriptor);
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
      return -1;
    }

    g_gztype = gzdopen(l_siFileDescriptor, l_pcGzoflags);
    if (!g_gztype) {
      close(l_siFileDescriptor);
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
      return -1;
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Source Path empty.");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (SI_32) l_siFileDescriptor;
}

static SI_32 gzCloseArchive() {
  return gzclose(g_gztype);
}

static ssize_t gzWriteArchive(int fd, const void* buf, size_t count) {
  return gzwrite(g_gztype, const_cast<void*>(buf), (unsigned int) count);
}

static ssize_t gzReadArchive(int fd, void* buf, size_t count) {
  return gzread(g_gztype, buf, (unsigned int) count);
}

static EFrameworkunifiedStatus copyFile(std::string f_source, std::string f_destination) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ssize_t l_nrd;
  ssize_t l_nwr;
  char l_buf[1024];

  int l_if = open(f_source.c_str(), O_RDONLY | O_CLOEXEC);
  if (-1 == l_if) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error. Failed to open file: %s for reading.", f_source.c_str());
    l_eStatus = eFrameworkunifiedStatusFail;
  } else {
    std::string l_tmpDest;
    size_t l_pos = f_destination.find_last_of('/');

    if (std::string::npos != l_pos) {  // LCOV_EXCL_BR_LINE 6: there must be '/' in string f_destination
      l_tmpDest = f_destination.substr(0, l_pos);
      l_tmpDest.append("/");
      l_tmpDest.append(LOGGER_TMP_FN);
      if (unlink(l_tmpDest.c_str()) != 0) {
        // always delete tmpLog, and create new tmpLog.
      }
    } else {
      // LCOV_EXCL_START 6: there must be '/' in string f_destination
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_tmpDest = f_destination;
      // LCOV_EXCL_STOP
    }

    int l_of = open(l_tmpDest.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC,
                    0640);
    if (-1 == l_of) {  // LCOV_EXCL_BR_LINE 5: open's error case
      // LCOV_EXCL_START 5: open's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "dest open fail:%s,errno:%d,msg:%s",
             l_tmpDest.c_str(), errno, strerror(errno));
      l_eStatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    } else {
      do {
        l_nrd = read(l_if, l_buf, sizeof(l_buf));
        if (l_nrd == -1) {  // LCOV_EXCL_BR_LINE 5: read's error case
          // LCOV_EXCL_START 5: read's error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "read fail:%s,errno:%d,msg:%s",
                 f_source.c_str(), errno, strerror(errno));
          l_eStatus = eFrameworkunifiedStatusAccessError;
          break;
          // LCOV_EXCL_STOP
        }
        l_nwr = write(l_of, l_buf, l_nrd);
        if (l_nwr == -1) {  // LCOV_EXCL_BR_LINE 5: write's error case
          // LCOV_EXCL_START 5: write's error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          if (ENOSPC == errno) {  // LCOV_EXCL_BR_LINE 4: no space error case
            // LCOV_EXCL_START 4: no space error case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "write fail(ENOSPC):%s,errno:%d,msg:%s", l_tmpDest.c_str(),
                   errno, strerror(errno));
            l_eStatus = eFrameworkunifiedStatusErrNoEAGAIN;
            // LCOV_EXCL_STOP
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "write fail:%s,errno:%d,msg%s",
                   l_tmpDest.c_str(), errno, strerror(errno));
            l_eStatus = eFrameworkunifiedStatusAccessError;
          }
          break;
          // LCOV_EXCL_STOP
        }
      } while (l_nrd > 0);
      fsync(l_of);
      close(l_of);
    }
    close(l_if);

    if (rename(l_tmpDest.c_str(), f_destination.c_str()) != 0) {  // LCOV_EXCL_BR_LINE 5: rename's error case
      // LCOV_EXCL_START 5: rename's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "rename %s to %s fail,errno:%d,msg:%s",
             l_tmpDest.c_str(), f_destination.c_str(), errno, strerror(errno));
      // LCOV_EXCL_STOP
    }

    if (std::string::npos != l_pos) {  // LCOV_EXCL_BR_LINE 6: there must be '/' in string f_destination
      std::string l_destPath = f_destination.substr(0, l_pos);
      SyncDir(l_destPath);
    } else {
      // LCOV_EXCL_START 6: there must be '/' in string f_destination
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "dest invalid:%s", f_destination.c_str());
      // LCOV_EXCL_STOP
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

static EFrameworkunifiedStatus copyDirectory (std::string f_source, std::string f_destination)
{
  DEB_LOGGERSERVICELOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  DIR * l_pDir = opendir(f_source.c_str());
  if (l_pDir == NULL) {
    DEB_LOGGERSERVICELOG(ZONE_ERR, __FUNCTION__, "Could not open directory: %s", f_source.c_str());
  } else {
    if (mkdir(f_destination.c_str(),0775) == -1) {
      if (errno == EEXIST) {
        DEB_LOGGERSERVICELOG(ZONE_FUNC, __FUNCTION__, "Directory: %s already exists.", f_destination.c_str());
      } else {
        DEB_LOGGERSERVICELOG(ZONE_ERR, __FUNCTION__, "Could not create directory: %s. Errno: %s", f_destination.c_str(),strerror(errno));
        l_eStatus = eFrameworkunifiedStatusFail;
      }
    } else {
      SyncDir(f_destination);
      size_t l_pos = f_destination.find_last_of('/');
      if (l_pos != std::string::npos) {
        std::string l_destPath = f_destination.substr(0, l_pos);
        SyncDir(l_destPath);
      }
    }

    struct dirent l_pDirent;
    struct dirent* next;
    while (0 == readdir_r(l_pDir,&l_pDirent,&next) && next != NULL) {
      if ( ( 0   != strcmp( l_pDirent.d_name, "."  ) ) &&
      ( 0   != strcmp( l_pDirent.d_name, ".." ))  &&
      ( 0   != strcmp( l_pDirent.d_name, "lost+found" ))  &&
      ( '.' != l_pDirent.d_name[0] ) ) {
        std::string l_fileSource = f_source;
        std::string l_fileDestination = f_destination;
        l_fileSource.append("/");
        l_fileSource.append(l_pDirent.d_name);
        l_fileDestination.append("/");
        l_fileDestination.append(l_pDirent.d_name);
        l_eStatus = copyUntyped(l_fileSource,l_fileDestination);
      }
    }
    closedir (l_pDir);
  }
  DEB_LOGGERSERVICELOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

static EFrameworkunifiedStatus copyUntyped (std::string f_source, std::string f_destination)
{
  DEB_LOGGERSERVICELOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  struct stat  l_stat;
  SI_8         l_statRetVal;
  if (0 != access(f_source.c_str(), F_OK)) {
    DEB_LOGGERSERVICELOG(ZONE_ERR,__FUNCTION__," Error. The specified file path and name does not exist: %s", f_source.c_str());
  } else if (0 != (l_statRetVal = lstat(f_source.c_str(), &l_stat))) {
    DEB_LOGGERSERVICELOG(ZONE_ERR,__FUNCTION__," Error. stat() for file or directory returned error: %s -> %d", f_source.c_str(), l_statRetVal);
  } else if (0 != S_ISDIR(l_stat.st_mode)) {
    l_eStatus = copyDirectory(f_source, f_destination);
  } else if (0 != S_ISLNK(l_stat.st_mode)) {
    l_eStatus = copyDirectory(f_source, f_destination);
  } else if (0 != S_ISREG(l_stat.st_mode)) {
    l_eStatus = copyFile(f_source, f_destination);
  } else {
    DEB_LOGGERSERVICELOG(ZONE_ERR,__FUNCTION__," Error. specified file may be a symbolic link or other non 'regular' type. File: %s, mode: 0x%04X.",f_source.c_str(),l_stat.st_mode);
  }
  DEB_LOGGERSERVICELOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

static EFrameworkunifiedStatus copyFileToAccOffOnDir(std::vector<LogInfo>& list, SS_STORELOGS_OPE_TYPE type)
{
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;
  std::string f_destination = LOGGERSERVICELOG_TMP_OUTPUT_PATH;
  f_destination.append("/");
  f_destination.append(ACCOFFON_DIR_NAME);
  DIR * l_pDir = opendir(LOGGERSERVICELOG_TMP_OUTPUT_PATH);
  if (l_pDir == NULL) {
    DEB_LOGGERSERVICELOG(ZONE_ERR, __FUNCTION__, "Could not open directory: %s", LOGGERSERVICELOG_TMP_OUTPUT_PATH);
    l_eStatus = eFrameworkunifiedStatusFail;
    return l_eStatus;
  } else {
    closedir (l_pDir);
    if (mkdir(f_destination.c_str(), 0775) == -1) {
      DEB_LOGGERSERVICELOG(ZONE_ERR, __FUNCTION__, "Could not create directory: %s. Errno: %s", f_destination.c_str(),strerror(errno));
      l_eStatus = eFrameworkunifiedStatusFail;
      return l_eStatus;
    }
  } if (list.size() != 0) {
    for (UI_32 j = 0; j < list.size(); j++) {
      std::string dest_name = f_destination;
      dest_name.append("/");
      dest_name.append(list.at(j).dest_name);
      l_eFrameworkunifiedStatus = copyUntyped(list.at(j).file_path, dest_name);
      if (l_eFrameworkunifiedStatus != eFrameworkunifiedStatusOK) {
        DEB_LOGGERSERVICELOG(ZONE_WARN, __FUNCTION__, " Error. File %s failed to archive.", list.at(j).file_path.c_str());
        l_eStatus = l_eFrameworkunifiedStatus;
      }
    }
    copyLoggerservicelogFile(f_destination);
  }
  return l_eStatus;
}

ELOGGERSERVICELOGTYPE QueryLoggerservicelogType(std::string f_logname) {
//  if (f_logname.find("_ILG_RESET.tar.gz") != std::string::npos
  if (f_logname.find(SS_STORE_FILENAME_SYSILG_TERM) != std::string::npos) {
    return eLoggerservicelogTypeSysAbnormal;
  } else if (f_logname.find(SS_STORE_FILENAME__CWORD52__TERM) != std::string::npos) {
    return eLoggerservicelogType_CWORD52_;
  } else if (f_logname.find("_ILG_RESET.tar.gz") != std::string::npos

      || f_logname.find("_ERR.tar.gz") != std::string::npos) {
    return eLoggerservicelogTypeAbnormal;
  } else if (f_logname.find("_GRP_RELAUNCH.tar.gz") != std::string::npos) {
    return eLoggerservicelogTypeGrpRelaunch;
  } else if (f_logname.find(".tar.gz") != std::string::npos) {
    return eLoggerservicelogTypeNormal;
  } else {
    return eLoggerservicelogTypeOther;
  }
}

void SyncDir(std::string f_dir_path) {
  int fd = open(f_dir_path.c_str(), O_RDONLY | O_DIRECTORY | O_CLOEXEC);
#define OPE_ERR_MSG "%s %s failed. errno:%d,msg:%s"
  if (fd == -1) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, OPE_ERR_MSG, f_dir_path.c_str(), "open",
           errno, strerror(errno));
  } else {
    if (fsync(fd) == -1) {  // LCOV_EXCL_BR_LINE 5: fsync's error case
      // LCOV_EXCL_START 5: fsync's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, OPE_ERR_MSG, f_dir_path.c_str(), "fsync",
             errno, strerror(errno));
      // LCOV_EXCL_STOP
    }
    if (close(fd) == -1) {  // LCOV_EXCL_BR_LINE 5: close's error case
      // LCOV_EXCL_START 5: close's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, OPE_ERR_MSG, f_dir_path.c_str(), "close",
             errno, strerror(errno));
      // LCOV_EXCL_STOP
    }
  }
#undef OPE_ERR_MSG
}

// LCOV_EXCL_START 4: no space error case
size_t deleteRequestLogs(std::string f_emmcpath, ELOGGERSERVICELOGTYPE f_reqtype,
                         size_t f_reqsize, ELOGGERSERVICELOGTYPE f_deltype) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  DIR* l_dirp = NULL;
  std::vector<std::string> vect;
  std::vector<std::string>::iterator it;
  UI_32 l_count = 0;
  size_t l_totalsize = 0;

  ELOGGERSERVICELOGTYPE filetype;


  // create saved file list on eMMC
  l_dirp = opendir(f_emmcpath.c_str());
  if (l_dirp != NULL) {
    struct dirent l_dirent;
    struct dirent* next;
    while (0 == readdir_r(l_dirp, &l_dirent, &next) && next != NULL) {
      std::string l_filename = l_dirent.d_name;

//      if (QueryLoggerservicelogType(l_filename) == f_deltype) {
//        vect.push_back(l_filename);
//      }
      if ((l_filename == ".") && (l_filename == "..")) {
        continue;
      }
      filetype = QueryLoggerservicelogType(l_filename);
      if (f_deltype == eLoggerservicelogTypeAbnormal) {
        if ((filetype == eLoggerservicelogTypeAbnormal) || (filetype == eLoggerservicelogTypeSysAbnormal) ||
            (filetype == eLoggerservicelogType_CWORD52_)) {
          vect.push_back(l_filename);
        }
      } else if (filetype == f_deltype) {
        vect.push_back(l_filename);
      }

    }
    closedir(l_dirp);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " open failed eMMC path: %s",
           f_emmcpath.c_str());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return 0;
  }

  // get num of delete logs
  std::sort(vect.begin(), vect.end());
  for (it = vect.begin(); it != vect.end(); it++) {
    l_count++;

    std::string l_filename = *it;
    struct stat st_buf;
    std::string l_tmpstr = f_emmcpath;
    l_tmpstr.append(l_filename.c_str());
    if (stat(l_tmpstr.c_str(), &st_buf) != -1) {
      l_totalsize += st_buf.st_size;
    }

    if (l_totalsize > f_reqsize) {
      break;
    }
  }

  // start delete logs
  if ((l_totalsize > f_reqsize) || (f_reqtype == eLoggerservicelogTypeAbnormal)
      || (f_reqtype == eLoggerservicelogTypeGrpRelaunch)) {
    for (it = vect.begin(); it != vect.end() && l_count > 0; it++) {
      l_count--;

      std::string l_filename = *it;
      std::string l_tmpstr = f_emmcpath;
      l_tmpstr.append(l_filename.c_str());
      if (unlink(l_tmpstr.c_str()) != 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " unlink failed %s:%d,msg:%s",
               l_tmpstr.c_str(), errno, strerror(errno));
      }
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " del-file=%s total=%d", l_tmpstr.c_str(),
             l_totalsize);
    }

    if (f_reqtype == eLoggerservicelogTypeAbnormal) {
      removeSameNumFiles(it, f_emmcpath, vect);
    }

    SyncDir(f_emmcpath);
  } else {
    l_totalsize = 0;
    FRAMEWORKUNIFIEDLOG(
      ZONE_INFO,
      __FUNCTION__,
      " nothing to delete logs req-type=%d del-logtype=%d total=%d, log_count=%d",
      f_reqtype, f_deltype, l_totalsize, l_count);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_totalsize;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 4: no space error case
EFrameworkunifiedStatus forceDeleteOldLog(std::string f_archive_destination,
                             size_t f_req_size) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  std::string l_emmc_path;
  size_t l_delsize = 0;
  ELOGGERSERVICELOGTYPE l_logtype = eLoggerservicelogTypeOther;

  size_t l_pos = f_archive_destination.find_last_of('/');
  if (std::string::npos != l_pos) {
    l_emmc_path = f_archive_destination.substr(0, l_pos);
    l_emmc_path.append("/");
    l_logtype = QueryLoggerservicelogType(
                  f_archive_destination.substr(l_pos + 1,
                      f_archive_destination.size() - l_pos + 1));

    if ((l_logtype == eLoggerservicelogTypeSysAbnormal) || (l_logtype == eLoggerservicelogType_CWORD52_)) {
      l_logtype = eLoggerservicelogTypeAbnormal;
    }

  }

  if (l_logtype == eLoggerservicelogTypeOther) {
    // not found
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Invalid Parameter. dest=%s",
           f_archive_destination.c_str());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return eFrameworkunifiedStatusFail;
  }

  // remove dirty-file if exist
  if (unlink(f_archive_destination.c_str()) != 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " unlink failed %s:%d.",
           f_archive_destination.c_str(), errno);
  }

  // delete normal log
  l_delsize = deleteRequestLogs(l_emmc_path, l_logtype, f_req_size,
                                eLoggerservicelogTypeNormal);
  if (l_delsize >= f_req_size) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " delete normal logs success: req-size=%d, del-size=%d", f_req_size,
           l_delsize);
    l_eStatus = eFrameworkunifiedStatusOK;
  } else if (l_logtype == eLoggerservicelogTypeAbnormal) {
    // delete abnormal log if request type is abnormal
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " continue delete abnormal logs: req-size=%d, del-size=%d",
           f_req_size, l_delsize);
    size_t l_tmpsize = f_req_size;
    l_tmpsize -= l_delsize;
    l_delsize = deleteRequestLogs(l_emmc_path, l_logtype, l_tmpsize,
                                  eLoggerservicelogTypeAbnormal);
    if (l_delsize >= l_tmpsize) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             " delete abnormal logs success: req-size=%d, del-size=%d",
             f_req_size, l_delsize);
      l_eStatus = eFrameworkunifiedStatusOK;
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " abnormal log delete not complete. req-size=%d, del-size=%d",
             f_req_size, l_delsize);
    }
  } else if (l_logtype == eLoggerservicelogTypeGrpRelaunch) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " continue delete group relaunch logs: req-size=%d, del-size=%d",
           f_req_size, l_delsize);
    size_t l_tmpsize = f_req_size;
    l_tmpsize -= l_delsize;
    l_delsize = deleteRequestLogs(l_emmc_path, l_logtype, l_tmpsize,
                                  eLoggerservicelogTypeGrpRelaunch);
    if (l_delsize >= l_tmpsize) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             " delete group relaunch logs success: req-size=%d, del-size=%d",
             f_req_size, l_delsize);
      l_eStatus = eFrameworkunifiedStatusOK;
    } else {
      FRAMEWORKUNIFIEDLOG(
        ZONE_ERR, __FUNCTION__,
        " group relaunch log delete not complete. req-size=%d, del-size=%d",
        f_req_size, l_delsize);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " normal log del not complete. req-size=%d, del-size=%d", f_req_size,
           l_delsize);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

///////////////////////////////////////////////////////////////////////
/// Function SS_LoggerStoreDebugLogs
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SS_LoggerStoreLogs(SS_STORELOGS_OPE_TYPE type) {
  if (type < SS_STORELOGS_MAX) {
    return storeLogs(type);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error. Bad enum number %d", type);
    return eFrameworkunifiedStatusFail;
  }
}


EFrameworkunifiedStatus SS_LoggerStoreLogs_deleteOldLogAbnrm(const std::string& log_path,
                                                std::vector<std::string>& l_vector,
                                                const std::string& f_archive_destination,
                                                SI_32 max_num,
                                                UI_32 abnrm_total) {
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus;
  l_eFrameworkunifiedStatus = deleteOldLogAbnrm(log_path, l_vector, f_archive_destination, max_num, abnrm_total);
  return l_eFrameworkunifiedStatus;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// IsDirectory
/// Method to check if the entity is a directory.
////////////////////////////////////////////////////////////////////////////////////////////////////
static BOOL IsDirectory(std::string &f_cPath) {  // NOLINT (runtime/references)
  BOOL l_bReturn = FALSE;
  struct stat st_buf;

  if (-1 == stat(f_cPath.c_str(), &st_buf)) {  // LCOV_EXCL_BR_LINE 5: stat's error case  // NOLINT (runtime/references)  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 5: stat's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error: stat failed for path/file %s, errno %d", f_cPath.c_str(),
           errno);
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    // LCOV_EXCL_STOP
  } else if (S_ISDIR(st_buf.st_mode)) {
    l_bReturn = TRUE;  // return true if f_cPath is a directory
  }
  return l_bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// RemoveDirectory
/// Method to remove a directory.
////////////////////////////////////////////////////////////////////////////////////////////////////
static BOOL RemoveDirectory(std::string &f_cPath) {  // NOLINT (runtime/references)
  BOOL l_bReturn = FALSE;

  if (RemoveSubDirectory(f_cPath)) {  // LCOV_EXCL_BR_LINE 6: RemoveSubDirectory always return true
    // delete the parent directory
    if (0 == rmdir(f_cPath.c_str())) {  // LCOV_EXCL_BR_LINE 5: rmdir's error case
      l_bReturn = TRUE;
    }
  }
  return l_bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// RemoveSubDirectory
/// Method to remove a sub directory.
////////////////////////////////////////////////////////////////////////////////////////////////////
static BOOL RemoveSubDirectory(std::string &f_cPath) {  // NOLINT (runtime/references)
  BOOL l_bReturn = TRUE;

  if (!f_cPath.empty()) {  // LCOV_EXCL_BR_LINE 6: f_cPath can't be empty
    std::string l_cFilePath = "";
    struct dirent l_Dirent;
    struct dirent* next;
    DIR *l_pDir = NULL;

    l_pDir = opendir(f_cPath.c_str());
    if (NULL != l_pDir) {  // LCOV_EXCL_BR_LINE 5: opendir's error case
      if ('/' != f_cPath[f_cPath.length() - 1]) {  // LCOV_EXCL_BR_LINE 6: there must be no '/'
        f_cPath.append("/");
      }

      while (0 == readdir_r(l_pDir, &l_Dirent, &next) && next != NULL) {
        if (0 != std::strcmp(l_Dirent.d_name, ".")
            && 0 != std::strcmp(l_Dirent.d_name, "..")
            && 0 != std::strcmp(l_Dirent.d_name, "lost+found")) {
          l_cFilePath.assign(f_cPath);
          l_cFilePath.append(l_Dirent.d_name);  // concatenate the strings to get the complete f_cPath
          if (TRUE == IsDirectory(l_cFilePath)) {
            l_bReturn = RemoveDirectory(l_cFilePath);
          } else {
            // it's a file, we can use unlink
            if (unlink(l_cFilePath.c_str()) == -1) {  // LCOV_EXCL_BR_LINE 5: unlink's error case
              // LCOV_EXCL_START 5: unlink's error case
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              l_bReturn = FALSE;
              // LCOV_EXCL_STOP
            }
          }
        }
      }
      closedir(l_pDir);  // close the directory
      SyncDir(f_cPath);
    } else {
      // LCOV_EXCL_START 5: opendir's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pDir is NULL");
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 6: f_cPath can't be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Pathname empty.");
    l_bReturn = FALSE;
    // LCOV_EXCL_STOP
  }
  return l_bReturn;
}

static void CreateSaveFinFlag(SS_STORELOGS_OPE_TYPE type) {
  if (type == SS_STORELOGS_INTERFACEUNIFIEDLOG) {
    int fd;
    fd = open(SS_LOGGER_SAVE_INTERFACEUNIFIEDLOG_FLAG,
              O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0640);
    if (fd == -1) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error. Create %s. errno %d msg :%s",
             SS_LOGGER_SAVE_INTERFACEUNIFIEDLOG_FLAG, errno, strerror(errno));
    } else {
      fsync(fd);
      close(fd);
      std::string l_flagPath = SS_LOGGER_SAVE_INTERFACEUNIFIEDLOG_FLAG;
      size_t l_pos = l_flagPath.find_last_of('/');
      if (std::string::npos != l_pos) {  // LCOV_EXCL_BR_LINE 6: there must be '/' in l_flagPath
        std::string l_dirPath = l_flagPath.substr(0, l_pos);
        SyncDir(l_dirPath);
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "create saveflag finish.");
        fprintf(stderr, "%s:Completed\n", __FUNCTION__);
      }
    }
  }
}

static void deleteAccOffOnUntyped(std::string deletePath)
{
  struct stat  l_stat;
  SI_8  l_statRetVal;
  if (0 != access(deletePath.c_str(), F_OK)) {
    DEB_LOGGERSERVICELOG(ZONE_WARN, __FUNCTION__, "access error (%s)", deletePath.c_str());
  } else if (0 != (l_statRetVal = lstat(deletePath.c_str(), &l_stat))) {
    DEB_LOGGERSERVICELOG(ZONE_WARN, __FUNCTION__, "get stat error (%s)", deletePath.c_str());
  } else if (0 != S_ISDIR(l_stat.st_mode)) {
    deleteAccOffOnDir(deletePath);
  } else if (0 != S_ISREG(l_stat.st_mode)) {
    unlink(deletePath.c_str());
  }
}

static void deleteAccOffOnDir(std::string deletePath)
{
  DIR * l_pDir = opendir(deletePath.c_str());
  if (l_pDir != NULL) {
    struct dirent l_pDirent;
    struct dirent* next;
    while (0 == readdir_r(l_pDir,&l_pDirent,&next) && next != NULL) {
      if ( ( 0   != strcmp( l_pDirent.d_name, "."  ) ) &&
      ( 0   != strcmp( l_pDirent.d_name, ".." ))  &&
      ( 0   != strcmp( l_pDirent.d_name, "lost+found" ))  &&
      ( '.' != l_pDirent.d_name[0] ) ) {
        std::string l_deletePath = deletePath;
        l_deletePath.append("/");
        l_deletePath.append(l_pDirent.d_name);
        deleteAccOffOnUntyped(l_deletePath);
      }
    }
    closedir (l_pDir);
  }
  RemoveDirectory(deletePath);
}

static EFrameworkunifiedStatus storeLogs(SS_STORELOGS_OPE_TYPE type) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  std::string file_name;
  std::vector<LogInfo> list;
  std::vector<LoggerservicelogInfo> loggerservicelogList;
  TAR* tar_info = NULL;
  uint32_t time_info;
  uint8_t status;
  CfgInfo cfginfo;

  UI_32 l_counter = INVALID_FILENUMBER;


  // load ss_logger configuration file.
  getConfigInfo(cfginfo);


  if (type == SS_STORELOGS_SYS_ILLEGAL) {
    if (getFileNumberFromTempArchive(l_counter) != eFrameworkunifiedStatusOK) {
      // ------------------------------------ //
      // no made ILG_RESET.tar.gz             //
      // when is abnormal logging count over. //
      // ------------------------------------ //
      return eFrameworkunifiedStatusOK;
    }
  }


//  Clock_getSystemTimeY2K38(&time_info, &status);

//  file_name = getFileName(cfginfo.output_path, time_info, type);
//
//  // prepare log files for collectting
//  for (UI_32 i = 0; i < _countof(GetLogFuncTbl); i++) {
//    GetLogFuncTbl[i](type, list);
//  }
  file_name = getFileName(cfginfo.output_path, time_info, type, l_counter);

  if (type != SS_STORELOGS_SYS_ILLEGAL) {
    // prepare log files for collectting
    for (UI_32 i = 0; i < _countof(GetLogFuncTbl); i++) {
      GetLogFuncTbl[i](type, list);
    }
  }

  if (type == SS_STORELOGS_ACCOFFON) {
    copyFileToAccOffOnDir(list, type);
    DEB_LOGGERSERVICELOG(ZONE_FUNC, __FUNCTION__, "- : FrameworkunifiedStatus = %d", l_eFrameworkunifiedStatus);
    return l_eFrameworkunifiedStatus;
  } else if (type != SS_STORELOGS_ACCOFFON_PRESS) {
    // loggerservicelogList used with clear buffer
    getLoggerservicelogFile(type, list, loggerservicelogList);
  }

  // ---------------------------------------------------------------- //
  // Warning!!! Logs by using FRAMEWORKUNIFIEDLOG macro, are not saved from here!! //
  // You can saved logs by FRAMEWORKUNIFIEDLOG after finished to clear buffer.     //
  // ---------------------------------------------------------------- //

  if (list.size() != 0) {
    // delete old archive before create new one.
    deleteOldEmmcLog(cfginfo, file_name, type);

    l_eFrameworkunifiedStatus = openArchive(&tar_info, file_name);
    if (l_eFrameworkunifiedStatus == eFrameworkunifiedStatusOK) {
      if (type == SS_STORELOGS_ACCOFFON_PRESS) {
        std::string collectPath;
        collectPath = LOGGERSERVICELOG_TMP_OUTPUT_PATH;
        collectPath.append("/");
        collectPath.append(ACCOFFON_DIR_NAME);
        if (0 != access(collectPath.c_str(), F_OK)) {
          DEB_LOGGERSERVICELOG(ZONE_ERR,__FUNCTION__," Error. The specified file path and name does not exist: %s", collectPath.c_str());
          l_eFrameworkunifiedStatus = eFrameworkunifiedStatusFail;
          return l_eFrameworkunifiedStatus;
        } else {
          l_eFrameworkunifiedStatus = addAllFileToArchive(tar_info,collectPath);
        }
      } else {
        for (UI_32 j = 0; j < list.size(); j++) {
          l_eFrameworkunifiedStatus = addToArchive(tar_info, list.at(j).file_path, list.at(j).dest_name);
          if(l_eFrameworkunifiedStatus != eFrameworkunifiedStatusOK) {
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, " Error. File %s failed to archive.", list.at(j).file_path.c_str());
          }
        }
      }

//      l_eFrameworkunifiedStatus = closeArchive(tar_info);
      l_eFrameworkunifiedStatus = closeArchive(tar_info, type);

      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "finish archiving logs(%d)", type);

      // when finish archiving, clear frameworkunifiedlog buffer.
      if (type == SS_STORELOGS_ACCOFFON_PRESS) {
        deleteTmpAccOffOn((std::string)ACCOFFON_DIR_NAME);
        DEB_LOGGERSERVICELOG(ZONE_FUNC, __FUNCTION__, "- : FrameworkunifiedStatus = %d", l_eFrameworkunifiedStatus);
        return (l_eFrameworkunifiedStatus);
      } else {
        ClearLoggerservicelogBuffer(loggerservicelogList);
      }

      // create flag of archiving completed
      CreateSaveFinFlag(type);

      // -------------------------------------------- //
      // You can save logs by FRAMEWORKUNIFIEDLOG macro from here. //
      // -------------------------------------------- //
    }

    // remove temporary or unnecessary files.
    for (UI_32 k = 0; k < list.size(); k++) {
      if (list.at(k).tmp_file_delete == TRUE) {
        struct stat l_stat;
        if (0 != stat(list.at(k).file_path.c_str(), &l_stat)) {
          FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Error. stat(%s) errno. %d",
                 list.at(k).file_path.c_str(), errno);
        } else if (S_ISDIR(l_stat.st_mode)) {
          RemoveDirectory(list.at(k).file_path);
        } else {
          if (0 != unlink(list.at(k).file_path.c_str())) {  // LCOV_EXCL_BR_LINE 5: unlink's error case
            // LCOV_EXCL_START 5: unlink's error case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, " Error. unlink(%s) errno. %d",
                   list.at(k).file_path.c_str(), errno);
            // LCOV_EXCL_STOP
          }
        }
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "- : FrameworkunifiedStatus = %d", l_eFrameworkunifiedStatus);
  return (l_eFrameworkunifiedStatus);
}

static void getCommunicationlogFile(SS_STORELOGS_OPE_TYPE type,
                                    std::vector<LogInfo>& list) {  // NOLINT (runtime/references)

  LogInfo log_info;

  log_info.file_path = FRAMEWORKUNIFIEDLOG_RAMDISC_PATH;
  log_info.file_path.append("/");
  log_info.file_path.append(COMMUNICATION_DIR_NAME);
  log_info.dest_name = COMMUNICATION_DIR_NAME;
  log_info.tmp_file_delete = FALSE;
  list.push_back(log_info);
}

static void getExternallogFile(SS_STORELOGS_OPE_TYPE type,
                             std::vector<LogInfo>& list) {  // NOLINT (runtime/references)
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ExternalLogList ext_log;
  memset(&ext_log, 0, sizeof(&ext_log));

  CNSConfigReader *l_pReaderCfg = new CNSConfigReader();

  if (NULL == l_pReaderCfg) {  // LCOV_EXCL_BR_LINE 5: new's error case
    // LCOV_EXCL_START 5: new's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Unable to allocate CNSConfigReader object.");
    // LCOV_EXCL_STOP
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->Parse(LOGGERSERVICELOG_CONFIG_PATH))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. l_pReaderCfg->Parse(%s) returned: 0x%X", LOGGERSERVICELOG_CONFIG_PATH);
    } else if (eFrameworkunifiedStatusOK !=
        (l_eStatus = l_pReaderCfg->GetInt("LOGGING.ExternalLogNum", ext_log.num))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. l_pReaderCfg->GetInt(LOGGING.ExternalLogNum)");
    } else if (eFrameworkunifiedStatusOK !=
        (l_eStatus = l_pReaderCfg->GetInt("LOGGING.ExternalLogMax", ext_log.max))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. l_pReaderCfg->GetInt(LOGGING.EmmcOutputMax)");
    } else {
      if (ext_log.num > 0) {
        std::string str = "EXTERNAL_";
        UI_32 num = 0;

        if (ext_log.num <= ext_log.max) {
          num = ext_log.num;
        } else {
          num = ext_log.max;
        }

        ext_log.p_info = new ExternalLogInfo[num];

        // Load external log info
        for (UI_32 i = 0; i < num; i++) {
          std::string query = str + std::to_string(i+1);
          if (eFrameworkunifiedStatusOK !=
              (l_eStatus = l_pReaderCfg->GetString((query+".OutputPath"), ext_log.p_info[i].dirname))) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. l_pReaderCfg->GetString(EXTERNAL.OutputPath)");
          } else if (eFrameworkunifiedStatusOK !=
              (l_eStatus = l_pReaderCfg->GetString((query+".ExtLogPath"), ext_log.p_info[i].path))) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. l_pReaderCfg->GetString(EXTERNAL.ExtLogPath)");
          } else if (eFrameworkunifiedStatusOK !=
              (l_eStatus = l_pReaderCfg->GetBool((query+".Remove"), ext_log.p_info[i].remove))) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. l_pReaderCfg->GetBool(EXTERNAL.Remove)");
          } else {
            LogInfo log_info;
            log_info.file_path = ext_log.p_info[i].path;
            log_info.dest_name = ext_log.p_info[i].dirname;
            log_info.tmp_file_delete = ext_log.p_info[i].remove;

            list.push_back(log_info);
          }
        }

        delete[] ext_log.p_info;

      } else {
        // None external log
      }
    }
    delete l_pReaderCfg;
  }
}




static void getLoggerservicelogFile(SS_STORELOGS_OPE_TYPE type,
                          std::vector<LogInfo>& list,  // NOLINT (runtime/references)
                          std::vector<LoggerservicelogInfo>& loggerservicelogList) {  // NOLINT (runtime/references)
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_DumpSize = 0;
  std::string src_file_path;
  std::string dst_file_path;
  PCSTR file_name;
  UI_32 file_num;
  LogInfo log_info;
  LoggerservicelogInfo loggerservicelog_info;

  file_num = NSLogGetFrameworkunifiedlogFileTotalNum();
  for (UI_32 cont = 0; cont < file_num; cont++) {
    file_name = NSLogGetFrameworkunifiedlogFileName(cont);
    if (file_name == NULL) {
      break;
    }
    src_file_path = FRAMEWORKUNIFIEDLOG_RAMDISC_PATH;
    src_file_path.append("/");
    src_file_path.append(file_name);
    dst_file_path = LOGGERSERVICELOG_TMP_OUTPUT_PATH;
    dst_file_path.append("/");
    dst_file_path.append(file_name);

    int index;
    index = NSLogGetFrameworkunifiedlogIndex(src_file_path.c_str());
    CNSRingBuffer* l_pLoggerservicelog = new (std::nothrow) CNSRingBuffer(
      src_file_path, 0, index + LOCK_NSLOG_ACCES_IF_1);
    if (l_pLoggerservicelog == NULL) {  // LCOV_EXCL_BR_LINE 5: new's error case
      // LCOV_EXCL_START 5: new's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: FRAMEWORKUNIFIEDLOG RingBuffer handle is NULL %s",
             src_file_path.c_str());
      continue;
      // LCOV_EXCL_STOP
    } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pLoggerservicelog->Open())) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: FRAMEWORKUNIFIEDLOG RingBuffer Open failed %s %d",
             src_file_path.c_str(), l_eStatus);
    } else {
      if (eFrameworkunifiedStatusOK
          != (l_eStatus = l_pLoggerservicelog->DumpToFile(dst_file_path.c_str(),
                          &l_DumpSize))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: FRAMEWORKUNIFIEDLOG RingBuffer DumpToFile failed %s %d",
               src_file_path.c_str(), l_eStatus);
      } else {
        log_info.file_path = dst_file_path;
        log_info.dest_name = file_name;
        log_info.tmp_file_delete = TRUE;
        list.push_back(log_info);
        // save frameworkunifiedlog-info for using clear frameworkunifiedlog buffer
        loggerservicelog_info.path = src_file_path;
        loggerservicelog_info.index = index;
        loggerservicelogList.push_back(loggerservicelog_info);
      }
      (void) l_pLoggerservicelog->Close();
    }
    delete l_pLoggerservicelog;
  }
}

static void copyLoggerservicelogFile(std::string distPath)
{
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_DumpSize = 0;
  std::string src_file_path;
  std::string dst_file_path;
  PCSTR file_name;
  UI_32 file_num;

  file_num = NSLogGetFrameworkunifiedlogFileTotalNum();

  for (UI_32 cont = 0; cont < file_num; cont++) {
    file_name = NSLogGetFrameworkunifiedlogFileName(cont);

    if (file_name == NULL) {
      break;
    }

    src_file_path = FRAMEWORKUNIFIEDLOG_RAMDISC_PATH;
    src_file_path.append("/");
    src_file_path.append(file_name);

    dst_file_path = distPath;
    dst_file_path.append("/");
    dst_file_path.append(file_name);

    int index;
    index = NSLogGetFrameworkunifiedlogIndex(src_file_path.c_str());
    CNSRingBuffer* l_pLoggerservicelog = new (std::nothrow)CNSRingBuffer(src_file_path,0,index+LOCK_NSLOG_ACCES_IF_1);
    if (l_pLoggerservicelog == NULL) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FRAMEWORKUNIFIEDLOG RingBuffer handle is NULL %s", src_file_path.c_str());
      continue;
    } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pLoggerservicelog->Open())) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,"Error: FRAMEWORKUNIFIEDLOG RingBuffer Open failed %s %d", src_file_path.c_str(), l_eStatus);
    } else {
      if (eFrameworkunifiedStatusOK != (l_eStatus = l_pLoggerservicelog->DumpToFile(dst_file_path.c_str(), &l_DumpSize))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,"Error: FRAMEWORKUNIFIEDLOG RingBuffer DumpToFile failed %s %d", src_file_path.c_str(), l_eStatus);
      }
      (void)l_pLoggerservicelog->Close();
    }
    delete l_pLoggerservicelog;
  }
}

static void deleteTmpAccOffOn(std::string accOffOnDir)
{
  std::string l_tmpAccOffOnPath = LOGGERSERVICELOG_TMP_OUTPUT_PATH;
  l_tmpAccOffOnPath.append("/");
  l_tmpAccOffOnPath.append(accOffOnDir.c_str());
  DIR * l_pDir = opendir(l_tmpAccOffOnPath.c_str());
  if (l_pDir != NULL) {
    struct dirent l_pDirent;
    struct dirent* next;
    while (0 == readdir_r(l_pDir,&l_pDirent,&next) && next != NULL) {
      if ( ( 0   != strcmp( l_pDirent.d_name, "."  ) ) &&
      ( 0   != strcmp( l_pDirent.d_name, ".." ))  &&
      ( 0   != strcmp( l_pDirent.d_name, "lost+found" ))  &&
      ( '.' != l_pDirent.d_name[0] ) ) {
        std::string l_deletePath = l_tmpAccOffOnPath;
        l_deletePath.append("/");
        l_deletePath.append(l_pDirent.d_name);
        deleteAccOffOnUntyped(l_deletePath);
      }
    }
    closedir (l_pDir);
  }
  RemoveDirectory(l_tmpAccOffOnPath);
}

static void ClearLoggerservicelogBuffer(std::vector<LoggerservicelogInfo> &loggerservicelogList) {  // NOLINT (runtime/references)
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  std::vector<LoggerservicelogInfo>::iterator ite = loggerservicelogList.begin();

  if (ite == loggerservicelogList.end()) {
    // target not found...
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "nothing to clear");
  } else {
    for (ite = loggerservicelogList.begin(); ite != loggerservicelogList.end(); ite++) {
      CNSRingBuffer* l_pLoggerservicelog = new (std::nothrow) CNSRingBuffer(
        ite->path.c_str(), 0, ite->index + LOCK_NSLOG_ACCES_IF_1);
      if (l_pLoggerservicelog == NULL) {  // LCOV_EXCL_BR_LINE 5: new's error case
        // LCOV_EXCL_START 5: new's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: FRAMEWORKUNIFIEDLOG RingBuffer handle is NULL %s", ite->path.c_str());
        continue;
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pLoggerservicelog->Open())) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error: FRAMEWORKUNIFIEDLOG RingBuffer Open failed %s %d", ite->path.c_str(),
               l_eStatus);
      } else {
        if (eFrameworkunifiedStatusOK != (l_eStatus = l_pLoggerservicelog->ClearBuf())) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 "Error: FRAMEWORKUNIFIEDLOG RingBuffer ClearBuf failed %s %d",
                 ite->path.c_str(), l_eStatus);
        }
        (void) l_pLoggerservicelog->Close();
      }
      delete l_pLoggerservicelog;
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Completed");
  fprintf(stderr, "%s:Completed\n", __FUNCTION__);
}

static void createKernelLog(SS_STORELOGS_KERNEL_LOG_TYPE type) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  int len;
  int fd;
  char* buf;
  ssize_t wr_len;

  len = klogctl(SYSLOG_ACTION_SIZE_BUFFER, NULL, 0);
  if (len < 0) {  // LCOV_EXCL_BR_LINE 5: klogctl's error case
    // LCOV_EXCL_START 5: klogctl's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error. Failed to get syslog buffer size: %d, %s", errno,
           strerror(errno));
    // LCOV_EXCL_STOP
  } else {
    buf = reinterpret_cast<char*>(malloc(len));
    if (buf != NULL) {  // LCOV_EXCL_BR_LINE 5: malloc's error case
      len = klogctl(SYSLOG_ACTION_READ_ALL, buf, len);
      if ((len < 0) || (len == 0)) {  // LCOV_EXCL_BR_LINE 5: klogctl's error case
        // LCOV_EXCL_START 5: klogctl's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error. Failed to read syslog buffer: %d, %s", errno,
               strerror(errno));
        // LCOV_EXCL_STOP
      } else {
        std::string file_put_path = "";

        switch (type) {
        case SS_STORELOGS_KERNEL_LOG:
          file_put_path = LOGGERSERVICELOG_TMP_OUTPUT_PATH;
          file_put_path.append("/");
          file_put_path.append(KERNEL_LOG_FN);
          fd = open(file_put_path.c_str(),
                    O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
          break;
        case SS_STORELOGS_KERNEL_BOOT_LOG:
          file_put_path = FRAMEWORKUNIFIEDLOG_RAMDISC_PATH;
          file_put_path.append("/");
          file_put_path.append(KERNEL_BOOT_LOG_FN);
          fd = open(file_put_path.c_str(),
                    O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
          break;
        default:
          fd = -1;
          break;
        }

        if (fd == -1) {
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
        }
      }
      free(buf);
    } else {
      // LCOV_EXCL_START 5: malloc's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Memory allocation Failed %s",
             strerror(errno));
      // LCOV_EXCL_STOP
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

static void getKernellogFile(SS_STORELOGS_OPE_TYPE type,
                             std::vector<LogInfo>& list) {  // NOLINT (runtime/references)
  getLastKernelLog(list);

  getKernelLog(list);

  if (type != SS_STORELOGS_ACCOFFON) {
    getKernelBootLog(type, list);
  }
}

static void getLastKernelLog(std::vector<LogInfo>& list) {  // NOLINT (runtime/references)
  LogInfo log_info;

  log_info.file_path = PSTORE_DIR_PATH;
  log_info.dest_name = "kernellog";
  log_info.tmp_file_delete = FALSE;
  list.push_back(log_info);
}

static void getKernelLog(std::vector<LogInfo>& list) {  // NOLINT (runtime/references)
  createKernelLog(SS_STORELOGS_KERNEL_LOG);

  LogInfo log_info;

  std::string klog_path = LOGGERSERVICELOG_TMP_OUTPUT_PATH;
  klog_path.append("/");
  klog_path.append(KERNEL_LOG_FN);

  if (0 == access(klog_path.c_str(), F_OK)) {
    log_info.file_path = klog_path;
    log_info.dest_name = KERNEL_LOG_FN;
    log_info.tmp_file_delete = TRUE;
    list.push_back(log_info);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Info: %s not found.", klog_path.c_str());
  }
}

static void getKernelBootLog(SS_STORELOGS_OPE_TYPE type,
                             std::vector<LogInfo>& list) {  // NOLINT (runtime/references)
  createKernelLog(SS_STORELOGS_KERNEL_BOOT_LOG);

  LogInfo log_info;

  std::string klog_path = FRAMEWORKUNIFIEDLOG_RAMDISC_PATH;
  klog_path.append("/");
  klog_path.append(KERNEL_BOOT_LOG_FN);

  if (0 == access(klog_path.c_str(), F_OK)) {
    std::string klog_out_path = LOGGERSERVICELOG_TMP_OUTPUT_PATH;
    klog_out_path.append("/");
    klog_out_path.append(KERNEL_BOOT_LOG_FN);

    if (eFrameworkunifiedStatusOK == copyFile(klog_path.c_str(), klog_out_path.c_str())) {  // LCOV_EXCL_BR_LINE 6: copyFile must return ok  // NOLINT[whitespace/line_length]
      log_info.file_path = klog_out_path;
      log_info.dest_name = KERNEL_BOOT_LOG_FN;
      log_info.tmp_file_delete = TRUE;
      list.push_back(log_info);
    } else {
      // LCOV_EXCL_START 6: copyFile must return ok
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Info: copyFile src:%s dest:%s failed.",
             klog_path.c_str(), klog_out_path.c_str());
      // LCOV_EXCL_STOP
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Info: %s not found.", klog_path.c_str());
  }
}


//static std::string getFileName(std::string &path, uint32_t f_time,  // NOLINT (runtime/references)
//                               SS_STORELOGS_OPE_TYPE type) {
static std::string getFileName(std::string &path, uint32_t f_time,
                               SS_STORELOGS_OPE_TYPE type, UI_32 l_counter) {

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::stringstream l_nameStream;

//  UI_32 l_counter = 0;
  std::string l_time_str = "00000000_000000";
  std::string l_ret;
  char buffer[20];
  struct tm l_time_tmp;


//  getEmmcCounter(path, l_counter);
  if (l_counter == INVALID_FILENUMBER) {
    l_counter = 0;
    getEmmcCounter(path, l_counter);
  }


//  CLOCK_RETURN clock_ret = Clock_getLocalTimeY2K38(&f_time, &l_time_tmp);
//  if (clock_ret == CLOCK_OK) {
//    if (0 != strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &l_time_tmp)) {
//      l_time_str = buffer;
//    }
//  }

//  l_nameStream << std::setw(5) << std::setfill('0') << std::right << std::dec
//               << l_counter << '_';
  l_nameStream << std::setw(FILENUMBER_DIGIT_NUM) << std::setfill('0') << std::right << std::dec
               << l_counter << '_';


  switch (type) {
  case SS_STORELOGS_ILLEGAL:
    l_nameStream << std::setw(15) << l_time_str << "_ILG_RESET.tar.gz";
    break;
  case SS_STORELOGS_INTERFACEUNIFIEDLOG:
    l_nameStream << std::setw(15) << l_time_str << "_ACCOFF.tar.gz";
    break;
  case SS_STORELOGS_ACCOFFON:
  case SS_STORELOGS_ACCOFFON_PRESS:
    l_nameStream << std::setw(15) << l_time_str << "_ACCOFFON.tar.gz";
    break;

  case SS_STORELOGS_SYS_ILLEGAL:
    l_nameStream << std::setw(15) << l_time_str << SS_STORE_FILENAME_SYSILG_TERM;
    break;

  default:
    //Set the file name as usual for cases other than the anomaly log specification.
    l_nameStream << std::setw(15) << l_time_str << ".tar.gz";
    break;
  }

  l_ret = path;
  l_ret.append("/");
  l_ret.append(l_nameStream.str());

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_ret);
}

static void getEmmcCounter(std::string &path, UI_32& f_counter) {  // NOLINT (runtime/references)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::string l_checkFileName;
  std::vector<UI_32> l_vector;
  DIR *l_dirp;
  struct dirent *l_direntp;
  size_t l_pos;
  UI_32 l_count;
  UI_32 l_maxCount = 0;

  l_dirp = opendir(path.c_str());
  if (l_dirp != NULL) {
    for (;;) {
      l_direntp = readdir(l_dirp);
      if (l_direntp == NULL) {
        break;
      }
      l_checkFileName = l_direntp->d_name;
      l_pos = l_checkFileName.find(".tar.gz");
      if (std::string::npos != l_pos) {
        l_pos = l_checkFileName.find("_");
        if (std::string::npos != l_pos) {
          std::string l_numberString = l_checkFileName.substr(0, l_pos);
          l_count = static_cast<UI_32>(strtoul(l_numberString.c_str(), NULL, 10));
          l_vector.push_back(l_count);
          l_maxCount = std::max(l_count, l_maxCount);
        }
      }
    }
    closedir(l_dirp);
  }
  l_maxCount++;

//  if (l_maxCount >= 100000u) {
//    l_maxCount = 1;
  if (l_maxCount >= FILENUMBER_UPPER_LIMIT_OVER) {
    l_maxCount = FILENUMBER_LOWER_LIMIT;

    for (;;) {
      std::vector<UI_32>::iterator find_itr = std::find(l_vector.begin(),
                                              l_vector.end(),
                                              l_maxCount);
      if (find_itr == l_vector.end()) {
        break;
      }
      l_maxCount++;
    }
  }
  f_counter = l_maxCount;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

static void getConfigInfo(CfgInfo& f_cfginfo) {  // NOLINT (runtime/references)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  f_cfginfo.output_path = "";
  f_cfginfo.output_max = 0;

  CNSConfigReader *l_pReaderCfg = new CNSConfigReader();

  if (NULL == l_pReaderCfg) {  // LCOV_EXCL_BR_LINE 5: new's error case
    // LCOV_EXCL_START 5: new's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. Unable to allocate CNSConfigReader object.");
    // LCOV_EXCL_STOP
  } else {
    if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->Parse(LOGGERSERVICELOG_CONFIG_PATH))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. l_pReaderCfg->Parse(%s) returned: 0x%X",
             LOGGERSERVICELOG_CONFIG_PATH, l_eStatus);
    } else if (eFrameworkunifiedStatusOK
               != (l_eStatus = l_pReaderCfg->GetString("LOGGING.EmmcOutputPath",
                               f_cfginfo.output_path))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. l_pReaderCfg->GetString(LOGGING.EmmcOutputPath)");
    } else if (eFrameworkunifiedStatusOK
               != (l_eStatus = l_pReaderCfg->GetInt("LOGGING.EmmcOutputMax",
                               f_cfginfo.output_max))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. l_pReaderCfg->GetInt(LOGGING.EmmcOutputMax)");
    } else if (eFrameworkunifiedStatusOK
               != (l_eStatus = l_pReaderCfg->GetInt("LOGGING.EmmcOutputErrMax",
                               f_cfginfo.output_errmax))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. l_pReaderCfg->GetInt(LOGGING.EmmcOutputErrMax)");
    }
    delete l_pReaderCfg;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

static EFrameworkunifiedStatus deleteOldLog(std::string log_path,
                               std::vector<std::string>& l_vector,  // NOLINT (runtime/references)
                               std::string& f_archive_destination,  // NOLINT (runtime/references)
                               UI_32 max_num) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  size_t l_pos = f_archive_destination.find_last_of('/');

  if (std::string::npos != l_pos) {  // LCOV_EXCL_BR_LINE 6: '/' must exist in f_archive_destination
    std::sort(l_vector.begin(), l_vector.end());
    std::vector<std::string>::iterator itr = l_vector.begin();
    for (UI_32 i = static_cast<UI_32>(l_vector.size()); (max_num - 1) < i; i--, itr++) {
      if (itr == l_vector.end()) {  // LCOV_EXCL_BR_LINE 6: itr can't be l_vector.end()
        // LCOV_EXCL_START 6: itr can't be l_vector.end()
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        break;
        // LCOV_EXCL_STOP
      }
      std::string l_FilePath = log_path;
      std::string l_FileName = *itr;
      l_FilePath.append("/");
      l_FilePath.append(l_FileName.c_str());
      if (0 != unlink(l_FilePath.c_str())) {
        l_eStatus = eFrameworkunifiedStatusAccessError;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. delete old log file %s:%d.",
               l_FilePath.c_str(), errno);
      }
    }
    SyncDir(log_path);
  } else {
    // LCOV_EXCL_START 6: '/' must exist in f_archive_destination
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusFail;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. delete target not found. %s",
           f_archive_destination.c_str());
    // LCOV_EXCL_STOP
  }
  return l_eStatus;
}


static EFrameworkunifiedStatus getFileNumber(const std::string &filename,UI_32 &number) {
  char buf[16];
  size_t pos;
  size_t offset;
  size_t length;
  UI_32 temp_value;

  offset = filename.find_last_of(PATHNAME_SEPARATOR) + 1;
  pos = filename.find_first_of('_', offset);
  length = pos-offset;
  if (std::string::npos == pos) {
    return eFrameworkunifiedStatusInvldParam;
  } else if (sizeof(buf) <= length) {
    return eFrameworkunifiedStatusInvldParam;
  }

  char *endptr;
  int  error_number;

  memcpy(buf, filename.c_str() + offset, length);
  buf[length]  = '\x0';

  errno = EOK;
  temp_value = strtol(buf, &endptr, 10);
  error_number = errno;
  if ((error_number != EOK) || (*endptr != '\x0')) {
    return eFrameworkunifiedStatusInvldParam;
  }
  number = temp_value;
  return eFrameworkunifiedStatusOK;
}

static bool compareFileNumber(UI_32 filename1_number, const std::string &filename2) {
  EFrameworkunifiedStatus l_eStatus;
  bool       result;
  UI_32      filename2_number;

  l_eStatus = getFileNumber(filename2, filename2_number);
  if (l_eStatus != eFrameworkunifiedStatusOK) {
    return false;
  }
  result = (filename1_number == filename2_number);
  return result;
}

static EFrameworkunifiedStatus removeFiles(const std::string &log_path, std::vector<std::string> &l_vector,
                              size_t begin_pos, size_t end_pos) {
  std::string l_FilePath;
  size_t      i;
  int         result;
  EFrameworkunifiedStatus  l_eStatus     = eFrameworkunifiedStatusOK;
  EFrameworkunifiedStatus  l_ePartStatus = eFrameworkunifiedStatusOK;
  l_FilePath.reserve(SSLOGGER_PATHNAME);
  for (i = begin_pos; i < end_pos; i++) {
    l_FilePath = log_path + PATHNAME_SEPARATOR_STR + l_vector[i];
    result     = unlink(l_FilePath.c_str());
    if (0 != result) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. unlink() filename = %s ,errno = %d.", l_FilePath.c_str(), errno);
      l_ePartStatus  = eFrameworkunifiedStatusAccessError;
      if (l_ePartStatus != eFrameworkunifiedStatusOK) {
        l_eStatus  = l_ePartStatus;
      }
    }
  }
  return l_eStatus;
}

static EFrameworkunifiedStatus removeSameNumFiles(std::vector<std::string>::iterator &itr, const std::string &log_path,
                                     std::vector<std::string> &l_vector) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  if (l_vector.begin() == itr) {  // LCOV_EXCL_BR_LINE 4: itr is l_vector.begin() only when no space error case
    // LCOV_EXCL_START 4: itr is l_vector.begin() only when no space error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return eFrameworkunifiedStatusOK;
    // LCOV_EXCL_STOP
  }
  std::string &removed_filename  = *(itr - 1);
  UI_32        removed_filenumber;
  if (getFileNumber(removed_filename, removed_filenumber) != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 6: double check, must return ok  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 6: double check, must return ok
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, " Warning. Type mismatch filename. = %s", removed_filename.c_str());
    return eFrameworkunifiedStatusOK;
    // LCOV_EXCL_STOP
  }

  std::string l_FilePath;
  EFrameworkunifiedStatus  l_ePartStatus  = eFrameworkunifiedStatusOK;
  EFrameworkunifiedStatus  l_eStatus    = eFrameworkunifiedStatusOK;
  int         result;
  for (;itr < l_vector.end(); itr++) {
    const std::string &exist_filename = (*itr);
    if (compareFileNumber(removed_filenumber, exist_filename) == true) {
      l_FilePath = log_path + PATHNAME_SEPARATOR_STR + exist_filename;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Remove file = %s.", l_FilePath.c_str());
      result = unlink(l_FilePath.c_str());
      if (0 != result) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. unlink() filename = %s , errno = %d.", l_FilePath.c_str(), errno);
        l_ePartStatus = eFrameworkunifiedStatusAccessError;
        if (l_ePartStatus != eFrameworkunifiedStatusOK) {
          l_eStatus  = l_ePartStatus;
        }
      }
    } else {
      break;
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

static EFrameworkunifiedStatus deleteOldLogAbnrm(const std::string& log_path, std::vector<std::string>& l_vector,
                                    const std::string& f_archive_destination, SI_32 max_num, UI_32 abnrm_total) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  size_t l_pos = f_archive_destination.find_last_of(PATHNAME_SEPARATOR);
  if (std::string::npos == l_pos) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. delete target not found. %s", f_archive_destination.c_str());
    return eFrameworkunifiedStatusFail;
  }
  if (l_vector.size() == 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. empty array.");
    return eFrameworkunifiedStatusFail;
  }
  //FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " max_num = %d , vect_abnrm.size() = %d", max_num, vect_abnrm.size());

  size_t       i;
  EFrameworkunifiedStatus   l_ePartStatus = eFrameworkunifiedStatusOK;
  EFrameworkunifiedStatus   l_eStatus     = eFrameworkunifiedStatusOK;
  size_t       end_pos       = l_vector.size();
  SI_32        abnrm_count   = 0;
  UI_32        exist_filenumber;
  ELOGGERSERVICELOGTYPE  filetype;

  max_num = abnrm_total - max_num + 1;
  std::sort(l_vector.begin(), l_vector.end());
  for (i = 0; i < end_pos; i++) {
    const std::string &exist_filename = l_vector[i];
    if (getFileNumber(exist_filename, exist_filenumber) != eFrameworkunifiedStatusOK) {
      continue;
    }
    filetype = QueryLoggerservicelogType(exist_filename);
    if (filetype == eLoggerservicelogTypeAbnormal) {
      abnrm_count++;
      if (max_num <= abnrm_count) {
        end_pos  = i + 1;
        break;
      }
    }
  }
  l_ePartStatus = removeFiles(log_path, l_vector, 0, end_pos);
  if (l_ePartStatus != eFrameworkunifiedStatusOK) {
    l_eStatus  = l_ePartStatus;
  }
  if (end_pos < l_vector.size()) {
    std::vector<std::string>::iterator itr = l_vector.begin() + end_pos;
    l_ePartStatus = removeSameNumFiles(itr, log_path, l_vector);
    if (l_ePartStatus != eFrameworkunifiedStatusOK) {
      l_eStatus  = l_ePartStatus;
    }
  }
  SyncDir(log_path);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}


static EFrameworkunifiedStatus deleteOldEmmcLog(CfgInfo& f_cfginfo,  // NOLINT (runtime/references)
                                   std::string& f_archive_destination,  // NOLINT (runtime/references)
                                   SS_STORELOGS_OPE_TYPE type) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  DIR *l_dirp = NULL;
  struct dirent *l_direntp = NULL;
  std::vector<std::string> vect_abnrm;
  std::vector<std::string> vect_nrm;
  std::vector<std::string> vect_rlnc;
  std::string l_emmc_path = f_cfginfo.output_path;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  UI_32         abnrm_count = 0;

  if (SS_STORELOGS_SYS_ILLEGAL == type) {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return eFrameworkunifiedStatusOK;
  }


  l_dirp = opendir(l_emmc_path.c_str());
  if (l_dirp != NULL) {
    while ((l_direntp = readdir(l_dirp)) != NULL) {
      std::string l_findFileName = l_direntp->d_name;

//      if ((std::string::npos != l_findFileName.find("_ILG_RESET.tar.gz"))
//          || (std::string::npos != l_findFileName.find("_ERR.tar.gz"))) {
      if ((l_findFileName == ".") || (l_findFileName == "..")) {
        continue;
      } else if (std::string::npos != l_findFileName.find(SS_STORE_FILENAME_SYSILG_TERM)) {
        vect_abnrm.push_back(l_findFileName);
      } else if (std::string::npos != l_findFileName.find(SS_STORE_FILENAME__CWORD52__TERM)) {
        vect_abnrm.push_back(l_findFileName);
      } else if((std::string::npos != l_findFileName.find("_ILG_RESET.tar.gz")) ||
                (std::string::npos != l_findFileName.find("_ERR.tar.gz"))) {

        vect_abnrm.push_back(l_findFileName);

        abnrm_count++;

      } else if (std::string::npos
                 != l_findFileName.find("_GRP_RELAUNCH.tar.gz")) {
        vect_rlnc.push_back(l_findFileName);
      } else if (std::string::npos != l_findFileName.find(".tar.gz")) {
        vect_nrm.push_back(l_findFileName);
      }
    }
    closedir(l_dirp);
  }

  if (SS_STORELOGS_ILLEGAL == type) {
    if ((UI_32) f_cfginfo.output_errmax <= vect_abnrm.size()) {
      l_eStatus = deleteOldLog(l_emmc_path, vect_abnrm, f_archive_destination,
                               (UI_32) f_cfginfo.output_errmax);
    }
  } else {
    if ((UI_32) f_cfginfo.output_max <= vect_nrm.size()) {
      l_eStatus = deleteOldLog(l_emmc_path, vect_nrm, f_archive_destination,
                               (UI_32) f_cfginfo.output_max);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
