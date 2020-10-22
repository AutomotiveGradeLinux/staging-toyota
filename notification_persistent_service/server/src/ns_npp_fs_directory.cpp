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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NS_NPPService
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_NPPService
/// \brief    This file contains the implementation for class CFSDirectory.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef AGL_STUB
#include <other_service/PosixBasedOS001TimeApi.h>
#endif

#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef AGL_STUB
#include <cstring>
#endif
#include <string>

#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_types.h"
#include "ns_npp_fs_directory.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CFSDirectory
/// Constructor of CFSDirectory class
////////////////////////////////////////////////////////////////////////////////////////////////////
CFSDirectory::CFSDirectory() {  // LCOV_EXCL_START 8: all methods are static
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CFSDirectory
/// Destructor of CFSDirectory class
////////////////////////////////////////////////////////////////////////////////////////////////////
CFSDirectory::~CFSDirectory() {   // LCOV_EXCL_START 8: all methods are static
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateDirectory
/// Method to create a directory.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFSDirectory::CreateDirectory(const std::string &f_cdirpath) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus   l_estatus = eFrameworkunifiedStatusOK;

  if (!f_cdirpath.empty()) {
    PSTR     l_cTempDirPath;
    PSTR     l_cParsedDirPath;
    PCSTR      l_cCopypath = f_cdirpath.c_str();

    l_cTempDirPath =  const_cast<PSTR >(f_cdirpath.c_str());
    while (l_estatus == eFrameworkunifiedStatusOK && (l_cParsedDirPath = std::strchr(l_cTempDirPath, '/')) != 0) {
      if (l_cParsedDirPath != l_cTempDirPath) {
        /* Neither root nor double slash in path */
        *l_cParsedDirPath = '\0';
        if (0 != mkdir(l_cCopypath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) {  // LCOV_EXCL_BR_LINE 5: mkdir's error case  // NOLINT[whitespace/line_length]
          if (EEXIST != errno) {  // LCOV_EXCL_BR_LINE 5: errno can't be changed
            // LCOV_EXCL_START 5: errno can't be changed
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error Cannot Create directory %s", l_cCopypath);
            l_estatus = eFrameworkunifiedStatusFail;
            // LCOV_EXCL_STOP
          } else {
            // file already exist
          }
        }
        *l_cParsedDirPath = '/';
      }
      l_cTempDirPath = l_cParsedDirPath + 1;
    }
    if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 200: l_estatus must be eFrameworkunifiedStatusOK
      if (0 != mkdir(l_cCopypath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) {  // LCOV_EXCL_BR_LINE 5: mkdir's error case  // NOLINT[whitespace/line_length]
        if (EEXIST != errno) {  // LCOV_EXCL_BR_LINE 5: errno can't be changed
          // LCOV_EXCL_START 5: errno can't be changed
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error Cannot Create directory %s", l_cCopypath);
          l_estatus = eFrameworkunifiedStatusFail;
          // LCOV_EXCL_STOP
        } else {
          // file already exist
        }
      }
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Directory path is empty.");
    l_estatus = eFrameworkunifiedStatusFail;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// DoesDirecotryExist
/// Method to check if a directory exists.
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CFSDirectory::DoesDirecotryExist(const std::string &f_cdirpath) {
  DIR *l_pDirDescriptor = opendir(f_cdirpath.c_str());
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
BOOL CFSDirectory::IsDirectory(const std::string &f_cpath) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_bReturn = FALSE;

  if (!f_cpath.empty()) {  // LCOV_EXCL_BR_LINE 200: f_cpath can't be empty
    struct stat st_buf;

    if (0 == stat(f_cpath.c_str(), &st_buf)) {  // LCOV_EXCL_BR_LINE 5: stat's error case
      // Get the status of the file
      if (S_ISREG(st_buf.st_mode)) {
        l_bReturn = FALSE;  // return false if f_cpath is a regular file
      }
      if (S_ISDIR(st_buf.st_mode)) {
        l_bReturn = TRUE;  // return true if f_cpath is a directory
      }
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error retrieving status of path : %s", f_cpath.c_str());  // LCOV_EXCL_LINE 5: stat's error case  // NOLINT[whitespace/line_length]
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Directory path is empty.");  // LCOV_EXCL_LINE 200: f_cpath can't be empty
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// RemoveDirectory
/// Method to remove a directory.
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CFSDirectory::RemoveDirectory(std::string f_cpath, const UI_32 f_uidelay) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_bReturn = FALSE;

  if (!f_cpath.empty()) {  // LCOV_EXCL_BR_LINE 200: f_cpath can't be empty.
    std::string l_cFilePath = "";

    struct dirent *l_pdirent = NULL;
    DIR *l_pdir = NULL;

    l_pdir = opendir(f_cpath.c_str());  // LCOV_EXCL_BR_LINE 11:unexpected branch
    if (NULL != l_pdir) {
      if ('/' != f_cpath[f_cpath.length() - 1]) {
        f_cpath.append("/");
      }

      while (NULL != (l_pdirent = readdir(l_pdir))) {
        if (0 != std::strcmp(l_pdirent->d_name, ".") &&
            0 != std::strcmp(l_pdirent->d_name, "..")) {
          l_cFilePath.assign(f_cpath);
          l_cFilePath.append(l_pdirent->d_name);  // concatenate the strings to get the complete f_cpath

          if (TRUE == IsDirectory(l_cFilePath)) {
            (void)RemoveDirectory(l_cFilePath, f_uidelay);
          } else {
            // it's a file, we can use unlink
            unlink(l_cFilePath.c_str());

            if (0 < f_uidelay) {  // LCOV_EXCL_START 200: f_uidelay is 0
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
#ifdef AGL_PosixBasedOS001LEGACY_USED
              delay(f_uidelay);
#endif
            }
            // LCOV_EXCL_STOP
          }
        }
      }
      closedir(l_pdir);  // close the directory
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "l_pdir is NULL");
    }

    // delete the directory
    if (0 == rmdir(f_cpath.c_str())) {
      if (0 < f_uidelay) {  // LCOV_EXCL_START 200: f_uidelay is 0
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
#ifdef AGL_PosixBasedOS001LEGACY_USED
        delay(f_uidelay);
#endif
      }
      // LCOV_EXCL_STOP
      l_bReturn = TRUE;
    }
  } else {
    // LCOV_EXCL_START 200: f_cpath can't be empty.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Directory path is empty.");
    l_bReturn = FALSE;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bReturn;
}
