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

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#include <native_service/ns_logger_if.h>
#include <system_service/ss_templates.h>

#include <system_service/ss_ver.h>

// For DEBUG
#define ZONE_INIT    ZONEMASK(10)
#define ZONE_FUNC    ZONEMASK(11)
#define ZONE_MEM     ZONEMASK(12)
#define ZONE_INFO    ZONEMASK(29)
#define ZONE_WARN    ZONEMASK(30)
#define ZONE_ERR     ZONEMASK(31)

#define ZONE_DEFAULT ZONE_FUNC

#define SSVER_TMPDIR "/tmp/ssver"

// FILE IO
class CPkgInfoIo {
 private:
  int m_fd;
  std::string m_path;
  static std::map<std::string, int> m_dbgCnt;  // Debug information for exclusive leakage check

  void openFD(const int32_t mode) {
    m_fd = open(m_path.c_str(), mode, 0664);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    if (m_fd == -1) {
      SS_ASERT_ERRNO(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h
      throw eFrameworkunifiedStatusFail;
    }

    // File lock to protect file accesses
    SS_ASERT_ERRNO(0 == flock(m_fd, LOCK_EX));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h
    // LCOV_EXCL_BR_START 11:Unexpected branch
    m_dbgCnt[m_path]++;
    SS_ASERT_LOG(m_dbgCnt[m_path] == 1, "%s:%d", m_path.c_str(),  // LCOV_EXCL_BR_LINE 15: marco define @ss_templates.h
                 m_dbgCnt[m_path]);
    // LCOV_EXCL_BR_STOP
  }

public:
  CPkgInfoIo(const std::string& pkgName)
    : m_fd(-1),
      m_path(SSVER_TMPDIR) {  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT,__FUNCTION__,"+:%s",pkgName.c_str());
    m_path += "/";
    m_path += pkgName;
 
     FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT,__FUNCTION__,"-");
   }

  ~CPkgInfoIo() {
    FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "+");

    // LCOV_EXCL_BR_START 11:Unexpected branch
    m_dbgCnt[m_path]--;
    SS_ASERT_LOG(m_dbgCnt[m_path] == 0, "%s:%d", m_path.c_str(),  // LCOV_EXCL_BR_LINE 15: marco define @ss_templates.h
                 m_dbgCnt[m_path]);
    SS_ASERT_ERRNO(0 == flock(m_fd, LOCK_UN));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h

    if (m_fd != -1) {
      SS_ASERT_ERRNO(0 == close(m_fd));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h
    }
    // LCOV_EXCL_BR_STOP
    FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "-");
  }

  void putData(SSVER_PkgInfo &info) {  //  NOLINT (readability/nolint)
    FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "+");
    openFD(O_RDWR|O_CREAT|O_CLOEXEC);
    ssize_t wsz;
    wsz = write(m_fd, &info, sizeof(info));
    if (wsz != sizeof(info)) {  // LCOV_EXCL_BR_LINE 5:std c lib error process
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed write(). wsz=%d size=%d errno=%d",  // LCOV_EXCL_LINE 5: glibc fail safe
             wsz, sizeof(info), errno);  // LCOV_EXCL_LINE 5:std c lib error process
      throw eFrameworkunifiedStatusFail;  // LCOV_EXCL_LINE 5:std c lib error process
    }
    FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "-");
  }

  void getData(SSVER_PkgInfo* p_info) {
    FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "+");
    openFD(O_RDONLY|O_CLOEXEC);

    ssize_t rsz;
    rsz = my_read(m_fd, p_info, sizeof(*p_info));
    if (rsz != sizeof(*p_info)) {  // LCOV_EXCL_BR_LINE 5:std c lib error process
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Failed read(). rsz=%d size=%d", rsz,  // LCOV_EXCL_LINE 5:std c lib error process
             sizeof(*p_info));  // LCOV_EXCL_LINE 5:std c lib error process
      p_info->version[0] = '\0';  // LCOV_EXCL_LINE 5:std c lib error process
      p_info->date[0] = '\0';  // LCOV_EXCL_LINE 5:std c lib error process
      throw eFrameworkunifiedStatusFail;  // LCOV_EXCL_LINE 5:std c lib error process
    }
    FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "-");
  }

  int my_read(int fd, void* buf, int size) {
    int done = 0;
    char *buffer = reinterpret_cast<char*>(buf);
    int sz = done;

    while (size > 0) {
      done = read(fd, buffer + done, size);
      if (done < 0) {  // LCOV_EXCL_BR_LINE 5:std c lib error process
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

        SS_ASERT_ERRNO(0);  // LCOV_EXCL_LINE 5:std c lib error process
        if (errno == EINTR) {  // LCOV_EXCL_LINE 5:std c lib error process
          continue;
        } else {
          return done;
        }
      } else if (done == 0) {  // LCOV_EXCL_BR_LINE 5:std c lib error process
        return sz;
      }
      size -= done;
      sz += done;
    }
    return sz;
  }
};

std::map<std::string, int> CPkgInfoIo::m_dbgCnt;  // Actual status

//==================================================================
//                  private
//==================================================================
void CSSVer::dump(void) const {  // LCOV_EXCL_START 7:debugging process
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

  std::string dumpStr;
  dumpStr = "VERINFO DUMP\n";

  for (SSVerPkgList::const_iterator ite = m_verList.begin();
      ite != m_verList.end(); ite++) {
    dumpStr += ite->first;
    dumpStr += "\n";
    dumpStr += ite->second.version;
    dumpStr += "\n";
    dumpStr += ite->second.date;
    dumpStr += "\n";
  }

  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "%s", dumpStr.c_str());
}
// LCOV_EXCL_STOP

//==================================================================
//                  PUBLIC
//==================================================================
CSSVer::CSSVer() {
  FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "+");
  DIR *dir = NULL;
  struct dirent dent, *next;
  struct stat statbuf;

  try {
    if (stat(SSVER_TMPDIR, &statbuf) != 0) {
      mkdir(SSVER_TMPDIR, 0775);
      // Because this process is not locked, if multiple processes process at the same time,
      // although it may have been created and fail, do not ASSERT it because it is harmless.
    }

    // Linux dependency codes
    if ((dir = opendir(SSVER_TMPDIR)) == NULL) {  // LCOV_EXCL_BR_LINE 5:std c lib error process
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

      SS_ASERT_ERRNO(0);  // LCOV_EXCL_LINE 5:std c lib error process
      throw eFrameworkunifiedStatusFail;
    }

    // LCOV_EXCL_BR_START 11:Unexpected branch
    while (0 == readdir_r(dir, &dent, &next) && next) {
      if (DT_REG == dent.d_type) {
        SSVER_PkgInfo info;
        std::string name(dent.d_name);
        std::string verInfo;

        CPkgInfoIo pkgIo(name);
        pkgIo.getData(&info);

        verInfo += "\nPACKAGE :";
        verInfo += name;
        verInfo += "\n";
        verInfo += "  VERSION:";
        verInfo += info.version;
        verInfo += "\n";
        verInfo += "  DATE   :";
        verInfo += info.date;
        verInfo += "\n";
        FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "%s", verInfo.c_str());

        m_verList.insert(std::make_pair(name, info));
      }
    }
    SS_ASERT_ERRNO(0 == closedir(dir));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h
    // LCOV_EXCL_BR_STOP
  } catch (...) {  // LCOV_EXCL_START 6:unable to reach
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    if (dir) {
      SS_ASERT_ERRNO(0 == closedir(dir));
    }
  }  // LCOV_EXCL_STOP
  FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "-");
}

CSSVer::~CSSVer() {
  FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "-");
}

EFrameworkunifiedStatus CSSVer::getPkgInfo(const std::string &name, SSVER_PkgInfo* p_info) {
  FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "+");
  EFrameworkunifiedStatus versionlibraryRet = eFrameworkunifiedStatusOK;
  try {
    if (p_info == NULL) {
      SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h
      throw eFrameworkunifiedStatusFail;
    }

    SSVerPkgList::iterator ite = m_verList.find(name);

    if (ite != m_verList.end()) {
      // cache hit
      *p_info = ite->second;
    } else {
      // cache miss
      std::string path(SSVER_TMPDIR);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
      path += "/";
      path += name;

      if (access(path.c_str(), F_OK) == 0) {
        // cache refresh
        CPkgInfoIo pkgIo(name);  // LCOV_EXCL_BR_LINE 11:Unexpected branch
        pkgIo.getData(p_info);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
        m_verList.insert(std::make_pair(name, *p_info));  // LCOV_EXCL_BR_LINE 11:except,C++ STL
      } else {
        throw eFrameworkunifiedStatusFileLoadError;
      }
    }
  } catch (EFrameworkunifiedStatus ee) {
    versionlibraryRet = ee;
  } catch (...) {  // LCOV_EXCL_START 6:unable to reach
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    versionlibraryRet = eFrameworkunifiedStatusFail;
  }  // LCOV_EXCL_STOP
  FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "-");
  return versionlibraryRet;
}

EFrameworkunifiedStatus CSSVer::setPkgInfo(const std::string &name, SSVER_PkgInfo &info) {
  FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "+");
  EFrameworkunifiedStatus versionlibraryRet = eFrameworkunifiedStatusOK;
  try {
    // LCOV_EXCL_BR_START 11:Unexpected branch
    SSVerPkgList::iterator ite = m_verList.find(name);

    if (ite == m_verList.end()) {
      m_verList.insert(std::make_pair(name, info));
      CPkgInfoIo pkgIo(name);
      pkgIo.putData(info);
    } else {
      ite->second = info;
      CPkgInfoIo pkgIo(name);
      pkgIo.putData(info);
    }
    // LCOV_EXCL_BR_STOP
  } catch (EFrameworkunifiedStatus ee) {
    versionlibraryRet = ee;
  } catch (...) {  // LCOV_EXCL_START 6:unable to reach
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h
    versionlibraryRet = eFrameworkunifiedStatusFail;
  }   // LCOV_EXCL_STOP

  FRAMEWORKUNIFIEDLOG(ZONE_DEFAULT, __FUNCTION__, "-");
  return versionlibraryRet;
}  // LCOV_EXCL_BR_LINE 10:for the last line

