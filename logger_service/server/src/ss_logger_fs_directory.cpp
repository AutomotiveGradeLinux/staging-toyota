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
/// \brief    This file contains declaration of class CFSDirectory.
///
///////////////////////////////////////////////////////////////////////////////
#include "ss_logger_fs_directory.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string>
#include "loggerservicedebug_loggerservicelog.h"
#include "ss_logger_types.h"
#include "ss_logger_util.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CFSDirectory
/// Constructor of CFSDirectory class
////////////////////////////////////////////////////////////////////////////////////////////////////
CFSDirectory::CFSDirectory() {  // LCOV_EXCL_START 14:static instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CFSDirectory
/// Destructor of CFSDirectory class
////////////////////////////////////////////////////////////////////////////////////////////////////
CFSDirectory::~CFSDirectory() {  // LCOV_EXCL_START 14:static instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateDirectory
/// Method to create a directory.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFSDirectory::CreateDirectory(std::string &f_cDirPath) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (!f_cDirPath.empty()) {  // LCOV_EXCL_BR_LINE 6: f_cDirPath is aways not empty
    PSTR l_cTempDirPath;
    PSTR l_cParsedDirPath;
    PCSTR l_cCopypath = f_cDirPath.c_str();

    l_cTempDirPath = const_cast<PSTR>(f_cDirPath.c_str());
    while (l_eStatus == eFrameworkunifiedStatusOK
        && (l_cParsedDirPath = std::strchr(l_cTempDirPath, '/')) != 0) {
      if (l_cParsedDirPath != l_cTempDirPath) {
        /* Neither root nor double slash in path */
        *l_cParsedDirPath = '\0';
        if (0
            != mkdir(l_cCopypath,
                     S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) {
          if (EEXIST != errno) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error Cannot Create directory %s",
                   l_cCopypath);
            l_eStatus = eFrameworkunifiedStatusFail;
          } else {
            // file already exist
          }
        }
        *l_cParsedDirPath = '/';
      }
      l_cTempDirPath = l_cParsedDirPath + 1;
    }
    if (eFrameworkunifiedStatusOK == l_eStatus) {
      if (0
          != mkdir(l_cCopypath,
                   S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) {
        if (EEXIST != errno) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error Cannot Create directory %s",
                 l_cCopypath);
          l_eStatus = eFrameworkunifiedStatusFail;
        } else {
          // file already exist
        }
      }
    }
  } else {
    // LCOV_EXCL_START 6: f_cDirPath is aways not empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "String Empty.");
    l_eStatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// DoesDirectoryExist
/// Method to check if a directory exists.
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CFSDirectory::DoesDirectoryExist(std::string &f_cDirPath) {
  DIR* l_pDirDescriptor = opendir(f_cDirPath.c_str());
  if (NULL != l_pDirDescriptor) {
    closedir(l_pDirDescriptor);
    return TRUE;
  }
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// IsDirectory
/// Method to check if the entity is a directory.
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CFSDirectory::IsDirectory(std::string &f_cPath) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_bReturn = FALSE;

  if (!f_cPath.empty()) {  // LCOV_EXCL_BR_LINE 6: f_cPath can not be empty
    struct stat st_buf;
    if (-1 == stat(f_cPath.c_str(), &st_buf)) {  // LCOV_EXCL_BR_LINE 5: c code.
      // LCOV_EXCL_START 5: c code.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error: stat failed for path/file %s, errno %d", f_cPath.c_str(),
             errno);
      FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
      l_bReturn = FALSE;
      return l_bReturn;
      // LCOV_EXCL_STOP
    }

    // Get the status of the file
    if (S_ISREG(st_buf.st_mode)) {
      l_bReturn = FALSE;  // return false if f_cPath is a regular file
    }
    if (S_ISDIR(st_buf.st_mode)) {
      l_bReturn = TRUE;  // return true if f_cPath is a directory
    }
  } else {
    // LCOV_EXCL_START 6: f_cPath can not be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Pathname  empty.");
    l_bReturn = FALSE;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// RemoveDirectory
/// Method to remove a directory.
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CFSDirectory::RemoveDirectory(std::string &f_cPath) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_bReturn = FALSE;

  if (RemoveSubDirectory(f_cPath)) {
    // delete the parent directory
    if (0 == rmdir(f_cPath.c_str())) {  // LCOV_EXCL_BR_LINE 5: c code error case
      l_bReturn = TRUE;
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// RemoveSubDirectory
/// Method to remove a sub directory.
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CFSDirectory::RemoveSubDirectory(std::string &f_cPath) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_bReturn = TRUE;

  if (!f_cPath.empty()) {  // LCOV_EXCL_BR_LINE 6: f_cPath can not be empty
    std::string l_cFilePath = "";

    struct dirent l_Dirent;
    struct dirent* next;
    DIR *l_pDir = NULL;

    l_pDir = opendir(f_cPath.c_str());
    if (NULL != l_pDir) {  // LCOV_EXCL_BR_LINE 5: c code.
      if ('/' != f_cPath[f_cPath.length() - 1]) {
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
            if (unlink(l_cFilePath.c_str()) == -1) {
              l_bReturn = FALSE;
            }
          }
        }
      }
      closedir(l_pDir);  // close the directory
      CLoggerUtil::SyncDir(f_cPath);
    } else {
      // LCOV_EXCL_START 5: c code.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pDir is NULL");
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 6: f_cPath can not be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Pathname empty.");
    l_bReturn = FALSE;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bReturn;
}
