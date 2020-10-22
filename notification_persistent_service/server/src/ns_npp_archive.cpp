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
/// \brief    This file contains the implementation for archiving and extracting files and folders.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef AGL_STUB
#include <other_service/strlcpy.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include <dirent.h>
#include <zconf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libtar_listhash.h>
#include <string>
#include <utility>
#include <vector>

#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_fs_directory.h"
#include "ns_npp_copy_worker.h"


tartype_t g_gztype = { (openfunc_t) OpenArchive, (closefunc_t) CloseArchive,
                     (readfunc_t) ReadArchive, (writefunc_t) WriteArchive
                   };

GZFiles g_gzfiles;

// CHUNK Size 256 Kb
// Buffer size for feeding data to and pulling data from the zlib routines
const size_t kChunkSize = 1U << 18;
// buff size 32KB
const size_t kBufSize = 1U << 15;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CArchive
/// Constructor of CArchive class
////////////////////////////////////////////////////////////////////////////////////////////////////
CCopyWorker::CArchive::CArchive() {
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CArchive
/// Destructor of CArchive class
////////////////////////////////////////////////////////////////////////////////////////////////////
CCopyWorker::CArchive::~CArchive() {
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FolderOperation
/// Method to perform archive or extract operations
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::CArchive::Archive(NSP_CopyInfoCmd &f_tarchiveinfocmd,
                                          NSP_CopyStatusResponse &f_tarchivestatusresponse) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  std::string l_pSourcePath = f_tarchiveinfocmd.m_csourcepath;
  std::string l_pDestPath = f_tarchiveinfocmd.m_cdestinationpath;

  if (!l_pSourcePath.empty() || !l_pDestPath.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, l_pSourcePath and l_pDestPath can't be empty  // NOLINT[whitespace/line_length]
    size_t l_ifound = l_pDestPath.rfind("/");
    // get string from start to last occurrence of '/'. Which will return directory path
    std::string l_cOutDirPath = l_pDestPath.substr(0, l_ifound);
    // check for load file or release file
    if (LOADTYPE_RELEASE == f_tarchiveinfocmd.m_eloadtype) {
      if (CFSDirectory::DoesDirecotryExist(l_pSourcePath)) {
        // check if destination directory exist
        if (!CFSDirectory::DoesDirecotryExist(l_cOutDirPath)) {
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "%s DOESN'T exist, Creating...",
                 l_cOutDirPath.c_str());
          l_estatus = CFSDirectory::CreateDirectory(l_cOutDirPath);
        }
        // Create archive
        if (eFrameworkunifiedStatusOK == (l_estatus = CreateTar(l_pSourcePath, l_pDestPath,  // LCOV_EXCL_BR_LINE 6: CreateTar always return eFrameworkunifiedStatusOK  // NOLINT[whitespace/line_length]
                                                   f_tarchivestatusresponse))) {
          // sync tar file to persistent memory
          SI_32 l_iTarHandle = LIBTARFAIL;
          if (LIBTARFAIL != (l_iTarHandle = open(l_pDestPath.c_str() , O_RDONLY))) {
            struct stat l_objStat;
            if (-1 != fstat(l_iTarHandle , &l_objStat)) {  // LCOV_EXCL_BR_LINE 6: fstat always return ok
              FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO3, __FUNCTION__, "Bytes written in NAND memory, %ld, file=%s", l_objStat.st_size,
                     l_pDestPath.c_str());
            }

            close(l_iTarHandle);
          } else {
            l_estatus = eFrameworkunifiedStatusFail;
          }
        } else {
          // LCOV_EXCL_START 6: CreateTar always return eFrameworkunifiedStatusOK
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Create tar Failed.");
          l_estatus = eFrameworkunifiedStatusFail;
          // LCOV_EXCL_STOP
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "CP_WRK_FAILURE_SRC_NOT_FND for %s ",
               l_pSourcePath.c_str());
        l_estatus = eFrameworkunifiedStatusFail;
      }
    } else {
      // check if destination directory exist
      if (!CFSDirectory::DoesDirecotryExist(l_pDestPath)) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "%s DOESN'T exist, Creating...",
               l_pDestPath.c_str());
        l_estatus = CFSDirectory::CreateDirectory(l_pDestPath);
      }
      if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 6: l_estatus will be always eFrameworkunifiedStatusOK
        // Extract archive
        l_estatus = ExtractTar(l_pSourcePath, l_pDestPath, f_tarchivestatusresponse);
      }
    }
  } else {
    // LCOV_EXCL_START 6: double check, l_pSourcePath and l_pDestPath can't be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Source path or Dest Path empty.");
    l_estatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// CreateTar
/// Method to create tar
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::CArchive::CreateTar(const std::string &f_csourepath,
                                            const std::string &f_cdestpath,
                                            NSP_CopyStatusResponse &f_tarchivestatusresponse) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  std::string l_cDestPath = f_cdestpath;
  std::string l_pcRootDirPath = f_csourepath;

  libtar_list_t *l_pLibTarList = NULL;
  l_pLibTarList = libtar_list_new(LIST_QUEUE, NULL);
  libtar_list_add(l_pLibTarList, (void *)l_pcRootDirPath.c_str()); // NOLINT (readability/casting)

  if (!l_cDestPath.empty() || !l_pcRootDirPath.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, l_cDestPath and l_pcRootDirPath cannot be empty  // NOLINT[whitespace/line_length]
    if (eFrameworkunifiedStatusOK != Create(l_cDestPath, l_pcRootDirPath, l_pLibTarList)) {  // LCOV_EXCL_BR_LINE 6: Create always return eFrameworkunifiedStatusOK  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_START 6: Create always return eFrameworkunifiedStatusOK
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      // not copied properly so remove temporary file
      if (TRUE == CFSDirectory::RemoveDirectory(l_cDestPath)) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Delete persistent directory successful.");
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Delete persistent directory unsuccessful.");
      }
      l_estatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Archiving fails for %s to %s", l_pcRootDirPath.c_str(),
             l_cDestPath.c_str());
      // LCOV_EXCL_STOP
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "Folder archived %s ", l_cDestPath.c_str());
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,   "Source Path %s", l_pcRootDirPath.c_str());

      if (TRUE == CFSDirectory::RemoveDirectory(l_pcRootDirPath)) {  // LCOV_EXCL_BR_LINE 6: always return true
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Delete temporary directory successful.");
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Delete temporary directory unsuccessful.");  // LCOV_EXCL_LINE 6: always return true  // NOLINT[whitespace/line_length]
      }
      l_estatus = eFrameworkunifiedStatusOK;
    }
  } else {
    // LCOV_EXCL_START 6: double check, l_cDestPath and l_pcRootDirPath cannot be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Source path or Dest Path empty.");
    l_estatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }
  libtar_list_free(l_pLibTarList, NULL);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// ExtractTar
/// Method to extract tar
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::CArchive::ExtractTar(const std::string &f_csourepath,
                                             const std::string &f_cdestpath,
                                             NSP_CopyStatusResponse &f_tarchivestatusresponse) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  std::string l_pcTarFileDestPath = f_cdestpath;
  std::string l_pcRootDirPath = f_csourepath;

  if (!l_pcTarFileDestPath.empty() || !l_pcRootDirPath.empty()) {  // LCOV_EXCL_BR_LINE 6: double check, l_pcTarFileDestPath and l_pcRootDirPath can't be empty  // NOLINT[whitespace/line_length]
    if (eFrameworkunifiedStatusOK != Extract(l_pcRootDirPath, l_pcTarFileDestPath)) {
      // not copied properly so remove temporary file
      if (TRUE == CFSDirectory::RemoveDirectory(l_pcTarFileDestPath)) {  // LCOV_EXCL_BR_LINE 6: RemoveDirectory always return true  // NOLINT[whitespace/line_length]
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Delete retrieved directory successful.");
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Delete retrieved directory unsuccessful.");  // LCOV_EXCL_LINE 6: RemoveDirectory always return true  // NOLINT[whitespace/line_length]
      }

      l_estatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "ABORT WAS CALLED!");
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,   "Extraction fails for %s to %s",
             l_pcTarFileDestPath.c_str(), l_pcTarFileDestPath.c_str());
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "Folder extracted %s ", l_pcTarFileDestPath.c_str());
      l_estatus = eFrameworkunifiedStatusOK;
    }
  } else {
    // LCOV_EXCL_START 6: double check, l_pcTarFileDestPath and l_pcRootDirPath can't be empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Source path or Dest Path empty.");
    l_estatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Create
/// Method to create tar
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::CArchive::Create(std::string &f_pctarfiledestpath,
                                         std::string &f_pcrootdirpath,
                                         libtar_list_t *f_plibtarlist) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (FILEERROR == access(f_pcrootdirpath.c_str(), R_OK)) {  // LCOV_EXCL_BR_LINE 6: access always return ok
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return eFrameworkunifiedStatusFail;  // LCOV_EXCL_LINE 6: access always return ok
  }

  TAR *l_pTarInfo = NULL;
  CHAR l_cBuffer[MAX_PATH_LENGTH];
  libtar_listptr_t l_pTarListPtr;

  PCHAR l_pcTarFileDestPath = const_cast<PCHAR >(f_pctarfiledestpath.c_str());
  PCHAR l_pcRootDirPath = const_cast<PCHAR >(f_pcrootdirpath.c_str());

  CHAR l_temp[] = "/";

  if (l_pcTarFileDestPath || l_pcRootDirPath) {  // LCOV_EXCL_BR_LINE 6: double check, l_pcTarFileDestPath and l_pcRootDirPath can't be NULL  // NOLINT[whitespace/line_length]
    // open tar archive
    if (LIBTARFAIL == (tar_open(&l_pTarInfo, l_pcTarFileDestPath,  &g_gztype, O_WRONLY | O_CREAT,  // LCOV_EXCL_BR_LINE 6: tar_open always return ok  // NOLINT[whitespace/line_length]
                                TARMODE, TAR_GNU))) {
      // LCOV_EXCL_START 6: tar_open always return ok
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "tar_open(): %s", std::strerror(errno));
      l_estatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    }

    libtar_listptr_reset(&l_pTarListPtr);

    while (libtar_list_next(f_plibtarlist, &l_pTarListPtr) != LIBTARSUCCESS) {
      if (l_pcRootDirPath[0] != '/') {  // LCOV_EXCL_BR_LINE 6: l_pcRootDirPath must begin with '/'
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "%s", l_pcRootDirPath);  // LCOV_EXCL_LINE 6: l_pcRootDirPath must begin with '/'  // NOLINT[whitespace/line_length]
      } else {
#ifdef AGL_PosixBasedOS001LEGACY_USED
        strlcpy(l_cBuffer, l_pcRootDirPath, sizeof(l_cBuffer));
#endif
      }

      if (tar_append_tree(l_pTarInfo, l_cBuffer, l_temp) != LIBTARSUCCESS) {  // LCOV_EXCL_BR_LINE 6: tar_append_tree always return ok  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 6: tar_append_tree always return ok
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__,
               "tar_append_tree(\"%s\", \"%s\"): %s", l_cBuffer,
               l_temp, std::strerror(errno));
        tar_close(l_pTarInfo);
        return eFrameworkunifiedStatusFail;
        // LCOV_EXCL_STOP
      }
    }
    if (tar_append_eof(l_pTarInfo) != LIBTARSUCCESS) {  // LCOV_EXCL_BR_LINE 6: tar_append_eof always return ok
      // LCOV_EXCL_START 6: tar_append_eof always return ok
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "tar_append_eof(): %s", std::strerror(errno));
      tar_close(l_pTarInfo);
      return eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    }

    if (tar_close(l_pTarInfo) != LIBTARSUCCESS) {  // LCOV_EXCL_BR_LINE 6: tar_close always return ok
      // LCOV_EXCL_START 6: tar_close always return ok
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "tar_close(): %s", std::strerror(errno));
      l_estatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 6: double check, l_pcTarFileDestPath and l_pcRootDirPath can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Source path or Dest Path empty.");
    l_estatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// create
/// Method to extract tar
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::CArchive::Extract(std::string &f_pcrootdirpath, std::string &f_pctarfiledestpath) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (FILEERROR == access(f_pcrootdirpath.c_str(), R_OK)) {  // LCOV_EXCL_BR_LINE 6: access always return ok
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return eFrameworkunifiedStatusFail;  // LCOV_EXCL_LINE 6: access always return ok
  }

  PCHAR l_pcRootDirPath = const_cast<PCHAR >(f_pcrootdirpath.c_str());
  PCHAR l_pcTarFileDestPath = const_cast<PCHAR >(f_pctarfiledestpath.c_str());

  TAR *l_pTarInfo = NULL;

  if (l_pcRootDirPath || l_pcTarFileDestPath) {  // LCOV_EXCL_BR_LINE 6: double check, l_pcRootDirPath and l_pcTarFileDestPath can't be NULL  // NOLINT[whitespace/line_length]
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,   "Extraction open for %s to ", l_pcRootDirPath);

    if (LIBTARFAIL == (tar_open(&l_pTarInfo, l_pcRootDirPath, &g_gztype, O_RDONLY, LIBTARSUCCESS, TAR_GNU))) {  // LCOV_EXCL_BR_LINE 6: tar_open always return ok  // NOLINT[whitespace/line_length]
      // LCOV_EXCL_START 6: tar_open always return ok
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "tar_open(): %s", std::strerror(errno));
      l_estatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    }

    if (LIBTARSUCCESS != tar_extract_all(l_pTarInfo, l_pcTarFileDestPath)) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "tar_extract_all(): %s", std::strerror(errno));
      l_estatus = eFrameworkunifiedStatusFail;
    }

    if (LIBTARSUCCESS != tar_close(l_pTarInfo)) {  // LCOV_EXCL_BR_LINE 6: tar_close always return ok
      // LCOV_EXCL_START 6: tar_close always return ok
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "tar_close(): %s", std::strerror(errno));
      l_estatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 6: double check, l_pcRootDirPath and l_pcTarFileDestPath can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Source path or Dest Path empty.");
    l_estatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

SI_32 OpenArchive(PCHAR f_pcpathname, SI_32 f_sioflags, SI_32 f_simode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  gzFile l_gzfile = Z_NULL;
  SI_32 l_sifiledescriptor = LIBTARFAIL;
  if (f_pcpathname) {  // LCOV_EXCL_BR_LINE 6: double check, f_pcpathname can't be NULL
    PCHAR l_pcgzoflags;

    switch (f_sioflags & O_ACCMODE) {
      case O_WRONLY: {
          l_pcgzoflags = (PCHAR)"wb";
          break;
        }
      case O_RDONLY: {
          l_pcgzoflags = (PCHAR)"rb";
          break;
        }
      case O_RDWR:
      default: {
          // LCOV_EXCL_START 6: must be O_WRONLY or O_RDONLY
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          errno = EINVAL;
          return LIBTARFAIL;
          // LCOV_EXCL_STOP
        }
    }

    l_sifiledescriptor = open(f_pcpathname, f_sioflags, f_simode);

    if (LIBTARFAIL == l_sifiledescriptor) {  // LCOV_EXCL_BR_LINE 6: l_sifiledescriptor always be ok
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return LIBTARFAIL;  // LCOV_EXCL_LINE 6: l_sifiledescriptor always be ok
    }

    if (((f_sioflags & O_CREAT) && fchmod(l_sifiledescriptor, f_simode))) {  // LCOV_EXCL_BR_LINE 6: always be ok
      // LCOV_EXCL_START 6: always be ok
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      close(l_sifiledescriptor);
      return LIBTARFAIL;
      // LCOV_EXCL_STOP
    }

    l_gzfile = gzdopen(l_sifiledescriptor, l_pcgzoflags);
    if (!l_gzfile) {  // LCOV_EXCL_BR_LINE 6: l_gzfile always be not null
      // LCOV_EXCL_START 6: l_gzfile always be not null
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      close(l_sifiledescriptor);

      errno = ENOMEM;
      return LIBTARFAIL;
      // LCOV_EXCL_STOP
    } else {
      g_gzfiles.insert(std::make_pair(l_sifiledescriptor, l_gzfile));
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Source Path empty.");  // LCOV_EXCL_LINE 6: double check, f_pcpathname can't be NULL  // NOLINT[whitespace/line_length]
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_sifiledescriptor;
}

int CloseArchive(int fd) {
  int     ret;
  gzFile  gzfile = Z_NULL;

  /*  checks  */
  if (g_gzfiles.empty()) {  // LCOV_EXCL_BR_LINE 6: g_gzfiles always be not empty
    // LCOV_EXCL_START 6: g_gzfiles always be not empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    errno = EBADF;
    return -1;
    // LCOV_EXCL_STOP
  }

  GZFiles::iterator gzfiles_it = g_gzfiles.find(fd);
  if (gzfiles_it == g_gzfiles.end()) {  // LCOV_EXCL_BR_LINE 6: fd will always be found
    // LCOV_EXCL_START 6: fd will always be found
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    errno = EINVAL;
    return -1;
    // LCOV_EXCL_STOP
  }

  /*  call zlib */
  gzfile = gzfiles_it->second;
  ret = gzclose(gzfile);

  /*  remove gzFile* association from our list  */
  g_gzfiles.erase(gzfiles_it);

  return ret;
}

ssize_t ReadArchive(int fd, void *buf, size_t count) {
  gzFile  gzfile = Z_NULL;

  if (g_gzfiles.empty()) {  // LCOV_EXCL_BR_LINE 6: g_gzfiles always be not empty
    // LCOV_EXCL_START 6: g_gzfiles always be not empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    errno = EBADF;
    return -1;
    // LCOV_EXCL_STOP
  }

  GZFiles::iterator gzfiles_it = g_gzfiles.find(fd);
  if (gzfiles_it == g_gzfiles.end()) {  // LCOV_EXCL_BR_LINE 6: fd will always be found
    // LCOV_EXCL_START 6: fd will always be found
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    errno = EINVAL;
    return -1;
    // LCOV_EXCL_STOP
  }

  gzfile = gzfiles_it->second;
  return gzread(gzfile, buf, static_cast<UI_32>(count));
}

ssize_t WriteArchive(int fd, const void *buf, size_t count) {
  gzFile  gzfile = Z_NULL;

  if (g_gzfiles.empty()) {  // LCOV_EXCL_BR_LINE 6: g_gzfiles always be not empty
    // LCOV_EXCL_START 6: g_gzfiles always be not empty
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    errno = EBADF;
    return -1;
    // LCOV_EXCL_STOP
  }

  GZFiles::iterator gzfiles_it = g_gzfiles.find(fd);
  if (gzfiles_it == g_gzfiles.end()) {  // LCOV_EXCL_BR_LINE 6: fd will always be found
    // LCOV_EXCL_START 6: fd will always be found
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    errno = EINVAL;
    return -1;
    // LCOV_EXCL_STOP
  }

  gzfile = gzfiles_it->second;
  return gzwrite(gzfile, buf, static_cast<UI_32>(count));
}

// LCOV_EXCL_START 6: unused code
////////////////////////////////////////////////////////////////////////////////////////////
/// FileOperationUsingLibz
/// Method to determine whether to compress/decompress file using libz
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::CArchive::FileOperationUsingLibz(NSP_CopyInfoCmd &f_toperainfocmd,
                                                         NSP_CopyStatusResponse &f_toperarespstatus) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  std::string l_pSourcePath = f_toperainfocmd.m_csourcepath;
  std::string l_pDestPath = f_toperainfocmd.m_cdestinationpath;

  if (!l_pSourcePath.empty() || !l_pDestPath.empty()) {
    size_t l_ifound = l_pDestPath.rfind("/");
    // get string from start to last occurrence of '/'. Which will return directory path
    std::string l_cOutDirPath = l_pDestPath.substr(0, l_ifound);

    // check if destination directory exist
    if (!CFSDirectory::DoesDirecotryExist(l_cOutDirPath)) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "%s DOESN'T exist, Creating...",
             l_cOutDirPath.c_str());
      l_estatus = CFSDirectory::CreateDirectory(l_cOutDirPath);
    }

    // check for load file or release file
    if (LOADTYPE_RELEASE == f_toperainfocmd.m_eloadtype) {
      // Zip
      if (eFrameworkunifiedStatusOK == (l_estatus = CompressUsingZlib(l_pSourcePath, l_pDestPath, Z_BEST_COMPRESSION))) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "File Compression successful.");
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "File Compression failed.");
        f_toperarespstatus.m_bpersistencechk = FALSE;
        l_estatus = eFrameworkunifiedStatusFail;
      }
    } else if (LOADTYPE_LOAD == f_toperainfocmd.m_eloadtype) {
      // Unzip
      if (eFrameworkunifiedStatusOK == (l_estatus = DeCompressUsingZlib(l_pSourcePath, l_pDestPath))) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "File DeCompression successful.");
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "File DeCompression failed.");
        f_toperarespstatus.m_bpersistencechk = FALSE;
        l_estatus = eFrameworkunifiedStatusFail;
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "Wrong Request or No request for compression or decompression!");
      l_estatus = eFrameworkunifiedStatusFail;
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Source Path or Destination Path Empty.");
    l_estatus = eFrameworkunifiedStatusFail;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 6: unused code
////////////////////////////////////////////////////////////////////////////////////////////
/// CompressUsingZlib
/// Method to compress file using libz
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::CArchive::CompressUsingZlib(const std::string &f_csourepath,
                                                    const std::string &f_cdestpath,
                                                    SI_32 f_iziplevel) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (!f_csourepath.empty() || !f_cdestpath.empty()) {
    std::string l_cSourePath = f_csourepath;
    std::string l_cDestPath = f_cdestpath;

    std::string  l_TempFile = "";
    l_TempFile.append(l_cDestPath.c_str());
    l_TempFile.append(".nps_tmp");

    FILE *l_pSrcFilePtr =  fopen(l_cSourePath.c_str(), "rb");

    if (NULL != l_pSrcFilePtr) {
      FILE *l_pDestFilePtr =  fopen(l_TempFile.c_str(), "wb");
      if (NULL != l_pDestFilePtr) {
        setvbuf(l_pSrcFilePtr, NULL, _IOFBF, kBufSize);
        setvbuf(l_pDestFilePtr, NULL, _IOFBF, kBufSize);

        // structure used to pass information to and from the zlib routines
        z_stream l_tZstreamInfo = {};

        // Allocate deflate state
        l_tZstreamInfo.zalloc = Z_NULL;
        l_tZstreamInfo.zfree  = Z_NULL;
        l_tZstreamInfo.opaque = Z_NULL;

        // Initialize deflates
        SI_32 l_iDeflateStatus = deflateInit(&l_tZstreamInfo, f_iziplevel);

        // Deflate Flush Status to determine end of input file
        SI_32 l_iDeflateFlushState = Z_FINISH;

        std::vector< UI_8 > l_vInBuffer(kChunkSize);
        std::vector< UI_8 > l_vOutBuffer(kChunkSize);

        // pointer to the read bytes from the input file
        UI_8 *l_pReadBuffer =  &l_vInBuffer[0];
        // pointer to the next available byte space to write to output file
        UI_8 *l_pWriteBuffer = &l_vOutBuffer[0];

        size_t l_uiDeflateRetData = 0;

        if (Z_OK == l_iDeflateStatus) {
          do {
            // bytes read from input file
            l_tZstreamInfo.avail_in = static_cast<uInt>(fread(l_pReadBuffer, 1, kChunkSize, l_pSrcFilePtr));

            if (0 == ferror(l_pSrcFilePtr)) {
              // if the eof input file reached set the flush state to Z_FINISH
              // Z_NO_FLUSH to indicate that we still have uncompressed data
              l_iDeflateFlushState = feof(l_pSrcFilePtr) ? Z_FINISH : Z_NO_FLUSH;
              l_tZstreamInfo.next_in = l_pReadBuffer;

              // run deflate() on input until output buffer not full, finish
              // compression if all of source has been read in
              do {
                l_tZstreamInfo.avail_out = kChunkSize;
                l_tZstreamInfo.next_out = l_pWriteBuffer;
                // no bad return value
                l_iDeflateStatus = deflate(&l_tZstreamInfo, l_iDeflateFlushState);
                // state not clobbered
                if (Z_STREAM_ERROR == l_iDeflateStatus) {
                  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "zlib stream error");
                } else {
                  l_uiDeflateRetData = kChunkSize - l_tZstreamInfo.avail_out;
                  if ((fwrite(l_pWriteBuffer, 1, l_uiDeflateRetData, l_pDestFilePtr) != l_uiDeflateRetData) ||
                      // all data not written properly
                      (0 != ferror(l_pDestFilePtr))) {           // file error in writing
                    deflateEnd(&l_tZstreamInfo);
                    l_estatus = eFrameworkunifiedStatusFail;
                    l_tZstreamInfo.avail_out = 0;
                    l_iDeflateFlushState = Z_FINISH;
                  }
                }
              } while (0 == l_tZstreamInfo.avail_out);  //<< check if deflate() has no more output
            } else {                                    // by seeing that it did not fill the output buffer
              deflateEnd(&l_tZstreamInfo);              // leaving avail_out greater than zero >>//
              l_estatus = eFrameworkunifiedStatusFail;
            }

            // all input will be used
            if (0 != l_tZstreamInfo.avail_in) {
              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "All inputs in z-stream avail_in not used");
              l_estatus = eFrameworkunifiedStatusFail;
            }
            // done when last data in file processed
          } while (Z_FINISH != l_iDeflateFlushState);
          // stream will be complete
          if (Z_STREAM_END != l_iDeflateStatus) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "z-stream not completed");
            l_estatus = eFrameworkunifiedStatusFail;
          }

          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Compressed %lu bytes down to %lu bytes",
                 l_tZstreamInfo.total_in, l_tZstreamInfo.total_out);

          FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO3, __FUNCTION__, "Bytes written in NAND memory, %lu, file=%s", l_tZstreamInfo.total_out,
                 f_cdestpath.c_str());

          // clean up and return
          deflateEnd(&l_tZstreamInfo);
        } else {
          l_estatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "deflateInit Failed");
        }

        fclose(l_pDestFilePtr);

        if (0 != rename(l_TempFile.c_str(), l_cDestPath.c_str())) {
          l_estatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "Error renaming file %s to %s",
                 l_TempFile.c_str(), l_cDestPath.c_str());
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "Destination File Opening Error");
        l_estatus = eFrameworkunifiedStatusFail;
      }
      fclose(l_pSrcFilePtr);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __PRETTY_FUNCTION__, "Source File Opening Error");
      l_estatus = eFrameworkunifiedStatusFail;
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Source Path or Destination Path Empty.");
    l_estatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 6: unused code
////////////////////////////////////////////////////////////////////////////////////////////
/// DeCompressUsingZlib
/// Method to decompress file using libz
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CCopyWorker::CArchive::DeCompressUsingZlib(const std::string &f_csourepath,
                                                      const std::string &f_cdestpath) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (!f_csourepath.empty() || !f_cdestpath.empty()) {
    std::string l_cSourePath = f_csourepath;
    std::string l_cDestPath = f_cdestpath;

    std::string  l_TempFile = "";
    l_TempFile.append(l_cDestPath.c_str());
    l_TempFile.append(".nps_tmp");

    FILE *l_pSrcFilePtr =  fopen(l_cSourePath.c_str(), "rb");

    if (NULL != l_pSrcFilePtr) {
      FILE *l_pDestFilePtr =  fopen(l_TempFile.c_str(), "wb");
      if (NULL != l_pDestFilePtr) {
        setvbuf(l_pSrcFilePtr, NULL, _IOFBF, kBufSize);
        setvbuf(l_pDestFilePtr, NULL, _IOFBF, kBufSize);

        z_stream l_tZstreamInfo = {};

        // allocate inflate state
        l_tZstreamInfo.zalloc = Z_NULL;
        l_tZstreamInfo.zfree = Z_NULL;
        l_tZstreamInfo.opaque = Z_NULL;
        l_tZstreamInfo.avail_in = 0;
        l_tZstreamInfo.next_in = Z_NULL;

        SI_32 l_iInflateStatus = inflateInit(&l_tZstreamInfo);

        if (Z_OK == l_iInflateStatus) {
          std::vector< UI_8 > l_vInBuffer(kChunkSize);
          std::vector< UI_8 > l_vOutBuffer(kChunkSize);

          // pointer to the read bytes from the input file
          UI_8 *l_pReadBuffer =  &l_vInBuffer[0];
          // pointer to the next available byte space to write to output file
          UI_8 *l_pWriteBuffer = &l_vOutBuffer[0];

          size_t l_uiDeflateRetData = 0;

          do {
            l_tZstreamInfo.avail_in = static_cast<uInt>(fread(l_pReadBuffer, 1, kChunkSize, l_pSrcFilePtr));
            if (0 == ferror(l_pSrcFilePtr)) {
              if (0 == l_tZstreamInfo.avail_in) {
                break;
              }
              l_tZstreamInfo.next_in = l_pReadBuffer;
              // run inflate() on input until output buffer not full
              do {
                l_tZstreamInfo.avail_out = kChunkSize;
                l_tZstreamInfo.next_out = l_pWriteBuffer;
                l_iInflateStatus = inflate(&l_tZstreamInfo, Z_NO_FLUSH);

                // state clobbered
                if (Z_STREAM_ERROR == l_iInflateStatus) {
                  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "zlib stream error");
                  l_estatus = eFrameworkunifiedStatusFail;
                } else if (Z_NEED_DICT == l_iInflateStatus) {
                  l_iInflateStatus = Z_DATA_ERROR;
                  l_estatus = eFrameworkunifiedStatusFail;
                } else if (Z_MEM_ERROR == l_iInflateStatus) {
                  inflateEnd(&l_tZstreamInfo);
                  l_estatus = eFrameworkunifiedStatusFail;
                  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Out of Memory");
                } else {
                  l_uiDeflateRetData = kChunkSize - l_tZstreamInfo.avail_out;
                  // all data not written
                  if ((fwrite(l_pWriteBuffer, 1, l_uiDeflateRetData, l_pDestFilePtr) != l_uiDeflateRetData) ||
                      (0 != ferror(l_pDestFilePtr))) {      // or file writing error
                    inflateEnd(&l_tZstreamInfo);
                    l_estatus = eFrameworkunifiedStatusFail;
                  }
                }
              } while (0 == l_tZstreamInfo.avail_out);
            } else {
              inflateEnd(&l_tZstreamInfo);
              l_estatus = eFrameworkunifiedStatusFail;
            }

            // done when inflate() says it's done
          } while (Z_STREAM_END != l_iInflateStatus);

          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,   " De-compressed %lu bytes to %lu bytes",
                 l_tZstreamInfo.total_in, l_tZstreamInfo.total_out);
          // clean up and return
          inflateEnd(&l_tZstreamInfo);
        } else {
          l_estatus = eFrameworkunifiedStatusFail;
        }

        fclose(l_pDestFilePtr);

        if (0 != rename(l_TempFile.c_str(), l_cDestPath.c_str())) {
          l_estatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "Error renaming file %s to %s",
                 l_TempFile.c_str(), l_cDestPath.c_str());
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "Destination File Opening Error");
        l_estatus = eFrameworkunifiedStatusFail;
      }
      fclose(l_pSrcFilePtr);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "Source File Opening Error");
      l_estatus = eFrameworkunifiedStatusFail;
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Source Path or Destination Path Empty.");
    l_estatus = eFrameworkunifiedStatusFail;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
// LCOV_EXCL_STOP
