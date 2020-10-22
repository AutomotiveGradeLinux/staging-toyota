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
/// \brief    This file supports error event logging.
///
///////////////////////////////////////////////////////////////////////////////
#include "ss_logger_error_event_archive.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libtar_listhash.h>
#include <libtar.h>
#include <zlib.h>
#include <stdio.h>
#include <boost/bind.hpp>
#include <native_service/frameworkunified_framework_if.h>
#include <system_service/ss_services.h>
#include <system_service/ss_templates.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "ss_logger_util.h"
#include "loggerservicedebug_loggerservicelog.h"
#include "ss_logger_fs_directory.h"

// GZF save variables
static gzFile g_gzFile;

CErrorEventArchive::CErrorEventArchive(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  m_pTarInfo = NULL;
  g_gzFile = NULL;
  m_gztype.openfunc = (openfunc_t) &CErrorEventArchive::gzOpenArchive;
  m_gztype.closefunc = (closefunc_t) &CErrorEventArchive::gzCloseArchive;
  m_gztype.readfunc = (readfunc_t) &CErrorEventArchive::gzReadArchive;
  m_gztype.writefunc = (writefunc_t) &CErrorEventArchive::gzWriteArchive;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

CErrorEventArchive::~CErrorEventArchive(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  if (NULL != m_pTarInfo) {  // LCOV_EXCL_BR_LINE 6: it will close after close
    // LCOV_EXCL_START 6: it will close after close
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    (void) closeArchive();
    m_pTarInfo = NULL;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

EFrameworkunifiedStatus CErrorEventArchive::openArchive(std::string f_pathAndName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL == m_pTarInfo) {  // LCOV_EXCL_BR_LINE 6: closeArchive will be set to null
    m_pathAndName = f_pathAndName;

    /* check if directory exists */
    size_t l_found = f_pathAndName.find_last_of('/');
    std::string l_path = f_pathAndName.substr(0, l_found);

    if (0 != access(l_path.c_str(), F_OK)) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Directory does not exist. Creating...");
      l_eFrameworkunifiedStatus = CFSDirectory::CreateDirectory(l_path);
    }

    /* create archive in tmp */
    m_tmpName = "/tmp/log.tar.gz";

    int l_tarRetVal = tar_open(&m_pTarInfo, const_cast<char*>(m_tmpName.c_str()),
                                &m_gztype,
                                O_WRONLY | O_CREAT,
                                TARMODE,
                                TAR_GNU);

    if ((l_tarRetVal != 0) || (m_pTarInfo == NULL) || (g_gzFile == NULL)) {  // LCOV_EXCL_BR_LINE 5: tar_open's error case.  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_START 5: tar_open's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Could not open TAR-Archive!");
      l_eFrameworkunifiedStatus = eFrameworkunifiedStatusAccessError;
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 6: closeArchive will be set to null
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Tar is still open");
    l_eFrameworkunifiedStatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eFrameworkunifiedStatus);
}

EFrameworkunifiedStatus CErrorEventArchive::addToArchive(std::string f_filePathAndName,
                                            std::string f_destinationName,
                                            UI_8 f_depth) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusFileLoadError;
  struct stat l_stat;
  int l_statRetVal;

  if (0 != access(f_filePathAndName.c_str(), F_OK)) {  // LCOV_EXCL_BR_LINE 200: file is aways exist
    // LCOV_EXCL_START 200: file is aways exist
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. The specified file path and name does not exist: %s",
           f_filePathAndName.c_str());
    // LCOV_EXCL_STOP
  } else if (0 != (l_statRetVal = stat(f_filePathAndName.c_str(), &l_stat))) {  // LCOV_EXCL_BR_LINE 5: stat's error case.  // NOLINT[whitespace/line_length]
    // LCOV_EXCL_START 5: stat's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. stat() for file or directory returned error: %s -> %d",
           f_filePathAndName.c_str(), l_statRetVal);
    // LCOV_EXCL_STOP
  } else if (0 != S_ISDIR(l_stat.st_mode)) {
    if (0 < f_depth) {  // LCOV_EXCL_BR_LINE 6: it aways 4
      l_eFrameworkunifiedStatus = addDirectoryToArchive(f_filePathAndName, f_destinationName,
                                           f_depth);
    } else {
      // LCOV_EXCL_START 6: it aways 4
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
             "Maximum recursive depth reached! File %s not added to archive.",
             f_filePathAndName.c_str());
      // LCOV_EXCL_STOP
    }
  } else if (0 != S_ISREG(l_stat.st_mode)) {  // LCOV_EXCL_BR_LINE 200: it can not be a symbolic link
    l_eFrameworkunifiedStatus = addFileToArchive(f_filePathAndName, f_destinationName);
  } else {
    // LCOV_EXCL_START 200: it can not be a symbolic link
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. specified file may be a symbolic link or other non 'regular' type. File: %s, mode: 0x%04X.",
        f_filePathAndName.c_str(), l_stat.st_mode);
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eFrameworkunifiedStatus);
}

// LCOV_EXCL_START 8: can not be called
EFrameworkunifiedStatus CErrorEventArchive::addToArchive(
    std::vector<std::string> f_filePathAndNameList) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  std::vector<std::string>::iterator l_vIter = f_filePathAndNameList.begin();
  while ((eFrameworkunifiedStatusOK == l_eFrameworkunifiedStatus)
      && (l_vIter != f_filePathAndNameList.end())) {
    l_eFrameworkunifiedStatus = addToArchive(*l_vIter, *l_vIter);
    l_vIter++;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eFrameworkunifiedStatus);
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CErrorEventArchive::closeArchive(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (0 != tar_append_eof(m_pTarInfo)) {  // LCOV_EXCL_BR_LINE 5: tar_append_eof's error case.
    // LCOV_EXCL_START 5: tar_append_eof's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "tar_append_eof()");
    l_eStatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }
  if (0 != tar_close(m_pTarInfo)) {  // LCOV_EXCL_BR_LINE 5: tar_close's error case.
    // LCOV_EXCL_START 5: tar_close's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Could not close archive");
    l_eStatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  } else {
    m_pTarInfo = NULL;
    g_gzFile = NULL;
  }

  if (eFrameworkunifiedStatusOK == l_eStatus) {  // LCOV_EXCL_BR_LINE 6: it aways be ok
#ifdef RELEASE_BUILD
    l_eStatus = CLoggerUtil::PathCheckAndCopyFile(m_tmpName, m_pathAndName);
#else
    l_eStatus = CLoggerUtil::CopyFile(m_tmpName, m_pathAndName);
#endif  // RELEASE_BUILD
    if (eFrameworkunifiedStatusOK == l_eStatus) {
      l_eStatus =
          (0 == remove(m_tmpName.c_str())) ? eFrameworkunifiedStatusOK : eFrameworkunifiedStatusFault;
      if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 5: remove's error case.
        // LCOV_EXCL_START 5: remove's error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(
            ZONE_WARN,
            __FUNCTION__,
            " Warning. Failed to remove temporary archive. Will overwrite on next event");
        // LCOV_EXCL_STOP
      }
    } else if (eFrameworkunifiedStatusErrNoEAGAIN == l_eStatus) {  // LCOV_EXCL_BR_LINE 200: write can not be ErrNoEAGAIN
      // LCOV_EXCL_START 200: write can not be ErrNoEAGAIN
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      // disk space full
      struct stat st_buf;
      size_t req_size;
      if (stat(m_tmpName.c_str(), &st_buf) != -1) {  // LCOV_EXCL_BR_LINE 5: stat's error case.
        req_size = st_buf.st_size;
        if (eFrameworkunifiedStatusOK
            == CLoggerUtil::forceDeleteOldLog(m_pathAndName, req_size)) {
#ifdef RELEASE_BUILD
          l_eStatus = CLoggerUtil::PathCheckAndCopyFile(m_tmpName, m_pathAndName);
#else
          l_eStatus = CLoggerUtil::CopyFile(m_tmpName, m_pathAndName);
#endif  // RELEASE_BUILD
          if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 200: PathCheckAndCopyFile can not be fail
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
        // LCOV_EXCL_START 5: stat's error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Warning. Failed to get tmp log file stat. dest=%s",
               m_pathAndName.c_str());
        // LCOV_EXCL_STOP
      }
      // LCOV_EXCL_STOP
    } else {
      /*should we copy lost archive to emmc??*/
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. Failed to copy to destination. %s", m_pathAndName.c_str());
    }
    remove(m_tmpName.c_str());
    m_tmpName = "";
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CErrorEventArchive::addFileToArchive(std::string f_filename,
                                                std::string f_destinationName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK; /*check if Tar is open and if file can be read */

  char path[PATH_MAX];

  if ((NULL != m_pTarInfo) && (0 != f_destinationName.length())  // LCOV_EXCL_BR_LINE 200: it will awalys be true
      && (realpath(f_filename.c_str(), path) != NULL)) {
    int l_tarRetVal = tar_append_file(m_pTarInfo, path,
                                      const_cast<char*>(f_destinationName. c_str()));
    if (0 != l_tarRetVal) {  // LCOV_EXCL_BR_LINE 5: tar_append_file's error case.
      // LCOV_EXCL_START 5: tar_append_file's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Could not add file \'%s\' to tar archive.", f_filename.c_str());
      l_eFrameworkunifiedStatus = eFrameworkunifiedStatusAccessError;
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 200: it will awalys be true
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Archive not opened or could not access file: \"%s\"",
           f_filename.c_str());
    l_eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eFrameworkunifiedStatus);
}

EFrameworkunifiedStatus CErrorEventArchive::addDirectoryToArchive(
    std::string f_path, std::string f_destinationName, UI_8 f_depth) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;
  struct dirent l_pDirent;
  struct dirent* next;

  DIR *l_pDir = opendir(f_path.c_str());
  if (l_pDir != NULL) {  // LCOV_EXCL_BR_LINE 5: opendir's error case.
    while (0 == readdir_r(l_pDir, &l_pDirent, &next) && next != NULL) {
      if ((0 != strcmp(l_pDirent.d_name, ".")) && /* Ignore special  . directory.                     */
      (0 != strcmp(l_pDirent.d_name, "..")) && /* Ignore special .. directory.                     */
      (0 != strcmp(l_pDirent.d_name, "lost+found")) && /* Ignore lost+found.                     */
      ('.' != l_pDirent.d_name[0])) {  /* Ignore hidden files                              */
        std::string l_extension = "/";
        std::string l_fileName = f_path;
        std::string l_destName = f_destinationName;
        l_extension.append(l_pDirent.d_name);
        l_fileName.append(l_extension);
        l_destName.append(l_extension);
        l_eFrameworkunifiedStatus = addToArchive(l_fileName, l_destName, static_cast<UI_8>(f_depth - 1));
      }
    }

    closedir(l_pDir);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eFrameworkunifiedStatus);
}

SI_32 CErrorEventArchive::gzOpenArchive(PCHAR f_pcPathname, SI_32 f_siOflags,
                                        SI_32 f_siMode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  SI_32 l_siFileDescriptor = -1;

  if (f_pcPathname) {  // LCOV_EXCL_BR_LINE 6: Pathname is awalys be set
    PCHAR l_pcGzoflags;

    switch (f_siOflags & O_ACCMODE) {  // LCOV_EXCL_BR_LINE 6: only O_WRONLY be set
      case O_WRONLY: {
        l_pcGzoflags = (PCHAR) "wb";
        break;
      }
      // LCOV_EXCL_START 6: only O_WRONLY be set
      case O_RDONLY: {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_pcGzoflags = (PCHAR) "rb";
        break;
      }
      // LCOV_EXCL_STOP
      case O_RDWR:
      default: {
        return -1;
      }
    }

    if (-1 == (l_siFileDescriptor =  // LCOV_EXCL_BR_LINE 5: open's error case.
        open(f_pcPathname, f_siOflags, f_siMode))) {
      // LCOV_EXCL_START 5: open's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
      return -1;
      // LCOV_EXCL_STOP
    }

    if ((f_siOflags & O_CREAT)  // LCOV_EXCL_BR_LINE 5: fchmod's error case.
        && fchmod(l_siFileDescriptor, f_siMode)) {
      // LCOV_EXCL_START 5: fchmod's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      close(l_siFileDescriptor);
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
      return -1;
      // LCOV_EXCL_STOP
    }

    g_gzFile = gzdopen(l_siFileDescriptor, l_pcGzoflags);
    if (!g_gzFile) {  // LCOV_EXCL_BR_LINE 5: gzdopen's error case.
      // LCOV_EXCL_START 5: gzdopen's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      close(l_siFileDescriptor);
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
      return -1;
      // LCOV_EXCL_STOP
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Source Path empty.");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (SI_32) l_siFileDescriptor;
}
SI_32 CErrorEventArchive::gzCloseArchive() {
  return gzclose(g_gzFile);
}

ssize_t CErrorEventArchive::gzWriteArchive(int fd, const void* buf,
                                           size_t count) {
  return gzwrite(g_gzFile, const_cast<void*>(buf), (unsigned int) count);
}

ssize_t CErrorEventArchive::gzReadArchive(int fd, void* buf, size_t count) {
  return gzread(g_gzFile, buf, (unsigned int) count);
}
