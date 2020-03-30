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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NPPService
/// \brief    The file contains definition of CBinaryAccesser class.
///       This class stores data in a file in binary format.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef AGL_STUB
#include <other_service/strlcpy.h>
#endif
#include <cerrno>
#include <string>
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_fs_directory.h"
#include "ns_npp_binary_accesser.h"
#include "ns_npp_persistent_data.h"
#include "ns_npp_persistent_accesser.h"

struct PersistFileHeder {
  char check_code[4];
  unsigned int crc;
};
#define NPP_CHECK_CODE "NPPD"

////////////////////////////////////////////////////////////////////////////////////////////////
/// CBinaryAccesser
/// Constructor of CBinaryAccesser class
////////////////////////////////////////////////////////////////////////////////////////////////
CBinaryAccesser::CBinaryAccesser() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_uiCurStrOffset = 0;
  m_uiCurStrSize = 0;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CBinaryAccesser
/// Destructor of CBinaryAccesser class
////////////////////////////////////////////////////////////////////////////////////////////////
CBinaryAccesser::~CBinaryAccesser() {   // LCOV_EXCL_START 14: Resident process, global instance not released
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////
/// PersistData
/// Persist data in persistent memory in a file in binary format.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CBinaryAccesser::PersistData(std::string f_cmemfilepath,
                                        Persistent_Notification_List_Type *f_vdata,
                                        EFrameworkunifiedNotificationType f_epersistenttype,
                                        EFrameworkunifiedPersistCategory f_epersistcategory) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  int l_OutFd;

  if (NULL != f_vdata) {  // LCOV_EXCL_BR_LINE 6: f_vdata can't be NULL
    // Open global file in write mode.
    if ((eFrameworkunifiedStatusOK == OpenFileForWriting(&l_OutFd, f_cmemfilepath))) {
      Persistent_Notification_List_Iterator l_itNotificationList = f_vdata->begin();

      // Iterate through a vector f_vdata
      for (; l_itNotificationList != f_vdata->end(); l_itNotificationList++) {
        if (eFrameworkunifiedPersistedStateVar == (*l_itNotificationList)->m_ePersistentType) {
          if (f_epersistcategory == (*l_itNotificationList)->m_ePersistCategory) {
            // write header and data
            if (eFrameworkunifiedStatusOK != WriteHeaderAndData((*l_itNotificationList),
                                                   (*l_itNotificationList)->m_pPersistentData->m_pMessage,
                                                   l_OutFd)) {
              l_estatus = eFrameworkunifiedStatusFail;
              break;
            }
          }
        } else {
          // write header and data
          if (eFrameworkunifiedStatusOK != WriteHeaderAndData((*l_itNotificationList),
                                                 (*l_itNotificationList)->m_pPersistentData->m_pMessage,
                                                 l_OutFd)) {
            l_estatus = eFrameworkunifiedStatusFail;
            break;
          }
        }
      }

      if (eFrameworkunifiedStatusOK != WriteFileHeaderAndDuplicate(l_OutFd, f_cmemfilepath)) {
        l_estatus = eFrameworkunifiedStatusFail;
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_PRD_INFO3, __FUNCTION__, "Bytes written in NAND memory, %ld, file=%s",
               static_cast<long int>(m_uiCurStrOffset - sizeof(PersistFileHeder)), f_cmemfilepath.c_str()); // NOLINT (runtime/int)
      }
      close(l_OutFd);
    } else {
      l_estatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to open file %s", f_cmemfilepath.c_str());
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// RetrieveData
/// Retrieve data in persistent memory in a file in binary format.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CBinaryAccesser::RetrieveData(std::string f_cmemfilepath,
                                         Persistent_Notification_List_Type *&f_vdata,
                                         EFrameworkunifiedPersistCategory f_epersistcategory) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  int l_InFd;

  if (NULL != f_vdata) {  // LCOV_EXCL_BR_LINE 6: f_vdata can't be NULL
    if ((eFrameworkunifiedStatusOK == (l_estatus = OpenFileForReading(&l_InFd, f_cmemfilepath)))) {
      l_estatus = FillNotificationList(l_InFd, f_vdata, f_epersistcategory);

      close(l_InFd);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to open file %s, status=%d", f_cmemfilepath.c_str(), l_estatus);
    }
  } else {
    // LCOV_EXCL_START 6: f_vdata can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "f_pdata is NULL");
    l_estatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// FillNotificationList
/// This function retrieve the notification data from file and fill it in map.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CBinaryAccesser::FillNotificationList(int f_infd,
                                                 Persistent_Notification_List_Type *&f_vdata,
                                                 EFrameworkunifiedPersistCategory f_epersistcategory) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL == f_vdata) {  // LCOV_EXCL_BR_LINE 6: f_vdata can't be NULL
    // LCOV_EXCL_START 6: f_vdata can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "f_pdata is NULL");
    l_estatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  } else {
    while (eFrameworkunifiedStatusOK == l_estatus) {
      CPersistDataHeader l_objCPersistDataHeader;
      CHAR *l_pData = NULL;

      // Read header and data.
      l_estatus = ReadHeaderAndData(l_objCPersistDataHeader, l_pData, f_infd);
      if (eFrameworkunifiedStatusOK == l_estatus) {
        CNotificationsToPersist *l_objCNotificationsToPersist = new(std::nothrow) CNotificationsToPersist();
        CPersistentData *l_objCPersistentData = new(std::nothrow) CPersistentData();

        // fill the appropriate values in  l_objCNotificationsToPersist
        if ((NULL != l_objCPersistentData) && (NULL != l_objCNotificationsToPersist)) {  // LCOV_EXCL_BR_LINE 5: new's error case  // NOLINT[whitespace/line_length]
          l_objCPersistentData->m_pMessage = new(std::nothrow) CHAR[l_objCPersistDataHeader.m_uiSize];
          if (NULL != l_objCPersistentData->m_pMessage) {  // LCOV_EXCL_BR_LINE 5: new's error case
            std::memcpy(l_objCPersistentData->m_pMessage, l_pData, l_objCPersistDataHeader.m_uiSize);
            l_objCPersistentData->m_uiMsgSize = l_objCPersistDataHeader.m_uiSize;
            l_objCNotificationsToPersist->m_ePersistentType = l_objCPersistDataHeader.m_ePersistentType;
            l_objCNotificationsToPersist->m_ePersistCategory = f_epersistcategory;

            l_objCNotificationsToPersist->m_cNotificationName = l_objCPersistDataHeader.m_cNotificationName;
            l_objCNotificationsToPersist->m_cPublisherName = l_objCPersistDataHeader.m_cPublisherName;

            l_objCNotificationsToPersist->m_pPersistentData = l_objCPersistentData;

            l_objCNotificationsToPersist->m_uiMaxMsgLength = l_objCPersistDataHeader.m_uiMaxMsgLength;
            f_vdata->push_back(l_objCNotificationsToPersist);
          } else {
            // LCOV_EXCL_START 5: new's error case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            std::memset(&l_objCPersistDataHeader, 0, sizeof(CPersistDataHeader));
            l_estatus = eFrameworkunifiedStatusNullPointer;
            break;
            // LCOV_EXCL_STOP
          }
        } else {
          // LCOV_EXCL_START 5: new's error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          std::memset(&l_objCPersistDataHeader, 0, sizeof(CPersistDataHeader));
          l_estatus = eFrameworkunifiedStatusNullPointer;
          break;
          // LCOV_EXCL_STOP
        }
      } else if (eFrameworkunifiedStatusErrOther == l_estatus) {
        // EOF
        l_estatus = eFrameworkunifiedStatusOK;
        break;
      } else {
        std::memset(&l_objCPersistDataHeader, 0, sizeof(CPersistDataHeader));
        break;
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
///
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CBinaryAccesser::OpenFileForReading(int *f_infd,
                                               std::string f_cfilepath) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // raw & raw.bak file unlink
  std::string l_cUnlinkPath(f_cfilepath);
  unlink(l_cUnlinkPath.c_str());
  l_cUnlinkPath.append(".bak");
  unlink(l_cUnlinkPath.c_str());

  // add .valid
  std::string l_cValidPath(f_cfilepath);
  l_cValidPath.append(".valid");

  if ((*f_infd = open(l_cValidPath.c_str(), O_RDONLY)) >= 0) {
    if (eFrameworkunifiedStatusOK == CheckFileHeader(*f_infd)) {
      struct stat st;
      if (fstat(*f_infd, &st) == 0) {  // LCOV_EXCL_BR_LINE 5: fstat's error case
        if (st.st_size == 0) {
          // If original file is 0byte, to confirm bak file
          int bak_fd;
          if (eFrameworkunifiedStatusOK == OpenBakFileForReading(&bak_fd, l_cValidPath)) {
            close(*f_infd);
            *f_infd = bak_fd;
          }
        }
      }
      l_estatus = eFrameworkunifiedStatusOK;
    } else {
      close(*f_infd);
      l_estatus = OpenBakFileForReading(f_infd, l_cValidPath);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "open(%s) fail: %s", l_cValidPath.c_str(), strerror(errno));
    l_estatus = OpenBakFileForReading(f_infd, l_cValidPath);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// OpenFileForWriting
///
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CBinaryAccesser::OpenFileForWriting(int *f_outfd,
                                               std::string f_cfilepath) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");


  std::string l_cPath(f_cfilepath);
  size_t l_ifound = l_cPath.rfind("/");
  std::string l_cOutDirPath = l_cPath.substr(0, l_ifound);

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Path for persistence %s ", l_cOutDirPath.c_str());

  // if parent directory path doesn't exists, create it
  if (!CFSDirectory::DoesDirecotryExist(l_cOutDirPath)) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s DOESN'T exist, Creating...", l_cOutDirPath.c_str());
    if (eFrameworkunifiedStatusOK != (l_estatus = CFSDirectory::CreateDirectory(l_cOutDirPath))) {  // LCOV_EXCL_BR_LINE 6: CreateDirectory always return ok  // NOLINT[whitespace/line_length]
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Failed to create %s ", l_cOutDirPath.c_str());  // LCOV_EXCL_LINE 6: CreateDirectory always return ok  // NOLINT[whitespace/line_length]
    }
  }

  // add .valid
  std::string l_cValidPath(f_cfilepath);
  l_cValidPath.append(".valid");

  if ((*f_outfd = open(l_cValidPath.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0660)) >= 0) {
    if (lseek(*f_outfd, sizeof(PersistFileHeder), SEEK_SET) == sizeof(PersistFileHeder)) {  // LCOV_EXCL_BR_LINE 5: lseek's error case  // NOLINT[whitespace/line_length]
      l_estatus = eFrameworkunifiedStatusOK;
      m_uiCurStrOffset = sizeof(PersistFileHeder);
    } else {
      // LCOV_EXCL_START 5: lseek's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "lseek fail: %s", strerror(errno));
      close(*f_outfd);
      *f_outfd = -1;
      // LCOV_EXCL_STOP
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "open(%s) fail: %s", l_cValidPath.c_str(), strerror(errno));
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// WriteHeaderAndData
/// Write header and data in a file.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CBinaryAccesser::WriteHeaderAndData(CNotificationsToPersist *f_pnotificationstopersist,
                                               PVOID f_pdata,
                                               int f_outfd) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;

  if ((NULL == f_pdata) || (NULL == f_pnotificationstopersist)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "f_pdata is NULL");
    l_estatus = eFrameworkunifiedStatusNullPointer;
  } else {
    CPersistDataHeader l_objCPersistDataHeader;

#ifdef AGL_PosixBasedOS001LEGACY_USED
    strlcpy(l_objCPersistDataHeader.m_cNotificationName, f_pnotificationstopersist->m_cNotificationName.c_str(),
            sizeof(l_objCPersistDataHeader.m_cNotificationName));
    strlcpy(l_objCPersistDataHeader.m_cPublisherName, f_pnotificationstopersist->m_cPublisherName.c_str(),
            sizeof(l_objCPersistDataHeader.m_cPublisherName));
#endif

    //  size check
    if (UINT32_MAX >= static_cast<size_t>(sizeof(CPersistDataHeader) + l_objCPersistDataHeader.m_uiSize)) {  // LCOV_EXCL_BR_LINE 5: the size is not bigger than UINT32_MAX  // NOLINT[whitespace/line_length]
      l_objCPersistDataHeader.m_uiOffset      = static_cast<UI_32>(m_uiCurStrOffset + sizeof(CPersistDataHeader));
      l_objCPersistDataHeader.m_uiSize      = f_pnotificationstopersist->m_pPersistentData->m_uiMsgSize;
      l_objCPersistDataHeader.m_ePersistentType   = f_pnotificationstopersist->m_ePersistentType;
      l_objCPersistDataHeader.m_uiMaxMsgLength  = f_pnotificationstopersist->m_uiMaxMsgLength;

        // write header
        if (write(f_outfd, (PCHAR)&l_objCPersistDataHeader, sizeof(CPersistDataHeader)) == sizeof(CPersistDataHeader)) {  // LCOV_EXCL_BR_LINE 5: write's error case  // NOLINT[whitespace/line_length]
          // write data
          if (write(f_outfd, (PCHAR)f_pdata,  // LCOV_EXCL_BR_LINE 5: write's error case
                    l_objCPersistDataHeader.m_uiSize) == (ssize_t)l_objCPersistDataHeader.m_uiSize) {
            // save offset of end of current record's data
            m_uiCurStrOffset += static_cast<UI_32>(sizeof(CPersistDataHeader) + l_objCPersistDataHeader.m_uiSize);
            l_estatus = eFrameworkunifiedStatusOK;
          } else {
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error while writing data: %s", strerror(errno));  // LCOV_EXCL_LINE 5: lseek's error case  // NOLINT[whitespace/line_length]
          }
        } else {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error while writing header: %s", strerror(errno));  // LCOV_EXCL_LINE 5: lseek's error case  // NOLINT[whitespace/line_length]
        }
      }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
///
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CBinaryAccesser::ReadHeaderAndData(CPersistDataHeader &f_objcpersistdataheader,
                                              CHAR *&f_pdata, int f_infd) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  static UI_32 l_uisPrevAllocSz = 0;
  ssize_t l_iRet;

  // read header
  l_iRet = read(f_infd, (PCHAR)&f_objcpersistdataheader, sizeof(f_objcpersistdataheader));
  if (l_iRet != sizeof(f_objcpersistdataheader)) {
    if (l_iRet < 0) {  // LCOV_EXCL_BR_LINE 5: read's error case
      // LCOV_EXCL_START 5: read's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error while reading header: %s", strerror(errno));
      l_estatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    } else {
      l_estatus = eFrameworkunifiedStatusErrOther;  // TODO(my_username): set specific error as eNotificationpersistentserviceStatusEOFReadched
    }
    std::memset(&f_objcpersistdataheader, 0, sizeof(CPersistDataHeader));
  } else {  // read data
    m_uiCurStrOffset += static_cast<UI_32>(sizeof(CPersistDataHeader));
    if (static_cast<int>(f_objcpersistdataheader.m_uiSize) > 0 &&
        m_uiCurStrOffset + f_objcpersistdataheader.m_uiSize <= m_uiCurStrSize) {
      // if f_pdata has space allocated and if the allocated size less than new size,
      // free and reallocate
      if ((NULL != f_pdata) && (l_uisPrevAllocSz < f_objcpersistdataheader.m_uiSize)) {  // LCOV_EXCL_BR_LINE 6: f_pdata must be null  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 6: f_pdata must be null
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        delete[] f_pdata;
        f_pdata = NULL;
        // LCOV_EXCL_STOP
      }

      if (NULL == f_pdata) {  // LCOV_EXCL_BR_LINE 200: f_pdata must be null
        f_pdata = new(std::nothrow) CHAR[f_objcpersistdataheader.m_uiSize];
      } else {
        // memory already allcoated.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      }

      if (NULL != f_pdata) {   // LCOV_EXCL_BR_LINE 6: f_pdata can't be null
        l_uisPrevAllocSz = f_objcpersistdataheader.m_uiSize;
        l_iRet = read(f_infd, (PCHAR)f_pdata, f_objcpersistdataheader.m_uiSize);
        if (l_iRet == (ssize_t)f_objcpersistdataheader.m_uiSize) {  // LCOV_EXCL_BR_LINE 5: read's error case
          m_uiCurStrOffset += f_objcpersistdataheader.m_uiSize;
          l_estatus = eFrameworkunifiedStatusOK;
        } else {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error while reading data: %s", strerror(errno));  // LCOV_EXCL_LINE 5: read's error case  // NOLINT[whitespace/line_length]
        }
      } else {
        // LCOV_EXCL_START 6: f_pdata can't be null
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Mem Allocation failure");
        l_estatus = eFrameworkunifiedStatusNullPointer;
        // LCOV_EXCL_STOP
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invlide data size: %u", f_objcpersistdataheader.m_uiSize);
      std::memset(&f_objcpersistdataheader, 0, sizeof(CPersistDataHeader));
      l_estatus = eFrameworkunifiedStatusFail;
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

EFrameworkunifiedStatus CBinaryAccesser::WriteFileHeaderAndDuplicate(int f_outfd, std::string f_cfilepath) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
  struct stat st;

  if (fstat(f_outfd, &st) == 0) {  // LCOV_EXCL_BR_LINE 5: fstat's error case
    if (st.st_size > static_cast<long>(sizeof(PersistFileHeder))) { // NOLINT (runtime/int)
      size_t body_size = static_cast<size_t>(st.st_size - sizeof(PersistFileHeder));
      char *buf = new(std::nothrow) char[body_size];
      if (buf != NULL) {  // LCOV_EXCL_BR_LINE 5: new's error case
        if (lseek(f_outfd, sizeof(PersistFileHeder), SEEK_SET) == sizeof(PersistFileHeder)) {  // LCOV_EXCL_BR_LINE 5: lseek's error case  // NOLINT[whitespace/line_length]
          if (read(f_outfd, buf, body_size) == static_cast<ssize_t>(body_size)) {  // LCOV_EXCL_BR_LINE 5: read's error case  // NOLINT[whitespace/line_length]
            PersistFileHeder f_header;
            std::memcpy(&f_header.check_code, NPP_CHECK_CODE, sizeof(f_header.check_code));
            f_header.crc = static_cast<UI_32>(CalcCRC(buf, static_cast<UI_32>(body_size)));
            if (lseek(f_outfd, 0, SEEK_SET) == 0) {  // LCOV_EXCL_BR_LINE 5: lseek's error case
              if (write(f_outfd, &f_header, sizeof(f_header)) == sizeof(f_header)) {  // LCOV_EXCL_BR_LINE 5: write's error case  // NOLINT[whitespace/line_length]
                eStatus = eFrameworkunifiedStatusOK;

                // file duplicate
                std::string bak_file_path = f_cfilepath;
                bak_file_path.append(".valid.bak");
                int bak_fd = open(bak_file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0660);
                if (bak_fd >= 0) {  // LCOV_EXCL_BR_LINE 5: open's error case
                  if (write(bak_fd, &f_header, sizeof(f_header)) == sizeof(f_header)) {  // LCOV_EXCL_BR_LINE 5: write's error case  // NOLINT[whitespace/line_length]
                    if (write(bak_fd, buf, body_size) != static_cast<ssize_t>(body_size)) {  // LCOV_EXCL_BR_LINE 5: write's error case  // NOLINT[whitespace/line_length]
                      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "write fail: %s", strerror(errno));  // LCOV_EXCL_LINE 5: write's error case  // NOLINT[whitespace/line_length]
                    }
                  } else {
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "write fail: %s", strerror(errno));  // LCOV_EXCL_LINE 5: write's error case  // NOLINT[whitespace/line_length]
                  }
                  close(bak_fd);
                } else {
                  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "open fail: %s", strerror(errno));  // LCOV_EXCL_LINE 5: open's error case  // NOLINT[whitespace/line_length]
                }
              } else {
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "write fail: %s", strerror(errno));  // LCOV_EXCL_LINE 5: write's error case  // NOLINT[whitespace/line_length]
              }
            } else {
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "lseek fail: %s", strerror(errno));  // LCOV_EXCL_LINE 5: lseek's error case  // NOLINT[whitespace/line_length]
            }
          } else {
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "read fail: %s", strerror(errno));  // LCOV_EXCL_LINE 5: read's error case
          }
        } else {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "lseek fail: %s", strerror(errno));  // LCOV_EXCL_LINE 5: lseek's error case
        }
        delete[] buf;
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Mem Allocation failure");  // LCOV_EXCL_LINE 5: new's error case
      }
    } else {
      // 0byte file through
      eStatus = eFrameworkunifiedStatusOK;
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "fstat fail: %s", strerror(errno));  // LCOV_EXCL_LINE 5: fstat's error case
  }

  return eStatus;
}

EFrameworkunifiedStatus CBinaryAccesser::CheckFileHeader(int f_infd) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
  struct stat st;

  if (fstat(f_infd, &st) == 0) {  // LCOV_EXCL_BR_LINE 5: fstat's error case
    if (st.st_size > static_cast<long>(sizeof(PersistFileHeder))) { // NOLINT (runtime/int)
      size_t body_size = static_cast<size_t>(st.st_size - sizeof(PersistFileHeder));
      char *buf = new(std::nothrow) char[body_size];
      if (buf != NULL) {  // LCOV_EXCL_BR_LINE 5: new's error case
        if (lseek(f_infd, sizeof(PersistFileHeder), SEEK_SET) == sizeof(PersistFileHeder)) {  // LCOV_EXCL_BR_LINE 5: lseek's error case  // NOLINT[whitespace/line_length]
          if (read(f_infd, buf, body_size) == static_cast<ssize_t>(body_size)) {
            UI_32 crc32 = static_cast<UI_32>(CalcCRC(buf, static_cast<UI_32>(body_size)));
            PersistFileHeder f_header;
            if (lseek(f_infd, 0, SEEK_SET) == 0) {  // LCOV_EXCL_BR_LINE 5: lseek's error case
              if (read(f_infd, &f_header, sizeof(f_header)) == sizeof(f_header)) {  // LCOV_EXCL_BR_LINE 5: read's error case  // NOLINT[whitespace/line_length]
                if (std::memcmp(&f_header.check_code, NPP_CHECK_CODE, sizeof(f_header.check_code)) == 0
                    && f_header.crc == crc32) {
                  m_uiCurStrOffset = sizeof(f_header);
                  m_uiCurStrSize = static_cast<UI_32>(st.st_size);
                  eStatus = eFrameworkunifiedStatusOK;
                } else {
                  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "file header fail: %02x %02x %02x %02x %#x:%#x",
                         f_header.check_code[0], f_header.check_code[1],
                         f_header.check_code[2], f_header.check_code[3], crc32, f_header.crc);
                }
              } else {
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "read fail: %s", strerror(errno));  // LCOV_EXCL_LINE 5: read's error case  // NOLINT[whitespace/line_length]
              }
            } else {
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "lseek fail: %s", strerror(errno));  // LCOV_EXCL_LINE 5: lseek's error case  // NOLINT[whitespace/line_length]
            }
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "read fail: %s", strerror(errno));
          }
        } else {
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "lseek fail: %s", strerror(errno));  // LCOV_EXCL_LINE 5: lseek's error case
        }
        delete[] buf;
      } else {
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Mem Allocation failure");  // LCOV_EXCL_LINE 5: new's error case
      }
    } else {
      // 0byte file through
      eStatus = eFrameworkunifiedStatusOK;
    }
  } else {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "fstat fail: %s", strerror(errno));  // LCOV_EXCL_LINE 5: fstat's error case
  }

  return eStatus;
}

EFrameworkunifiedStatus CBinaryAccesser::OpenBakFileForReading(int *f_infd, std::string f_cfilepath) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
  std::string bak_file_path = f_cfilepath;
  bak_file_path.append(".bak");

  *f_infd = open(bak_file_path.c_str(), O_RDONLY);
  if (*f_infd >= 0) {
    struct stat st;
    if (fstat(*f_infd, &st) == 0) {
      // Error if bak file is 0byte
      if (st.st_size > 0) {
        if (eFrameworkunifiedStatusOK == CheckFileHeader(*f_infd)) {
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s fail. bak file correct.", f_cfilepath.c_str());
          eStatus = eFrameworkunifiedStatusOK;
        } else {
          close(*f_infd);
          *f_infd = -1;
        }
      } else {
        close(*f_infd);
        *f_infd = -1;
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "fstat fail: %s", strerror(errno));
      close(*f_infd);
      *f_infd = -1;
    }
  }

  return eStatus;
}

/*
 * CRC-32C lookup table
 *
 * Polynomial 0x1EDC6F41
 */
static const UI_32 crc32_lookup[4][256] = {  // NOLINT (readability/naming)
  {
  },
  {
  },
  {
  },
  {
  }
};

UI_32 CBinaryAccesser::CalcCRC(PVOID f_pdata, UI_32 f_size) {
  UI_32 crc = 0xFFFFFFFF;
  const UI_32 *current = (const UI_32 *)f_pdata;

  while (f_size >= 4) {
    UI_32 one = *current++ ^ crc;
    crc = crc32_lookup[0][(one >> 24) & 0xFF] ^
          crc32_lookup[1][(one >> 16) & 0xFF] ^
          crc32_lookup[2][(one >> 8) & 0xFF] ^
          crc32_lookup[3][one & 0xFF];
    f_size -= 4;
  }

  const UI_8 *current_char = (const UI_8 *)current;
  while (f_size-- > 0) {
    crc = (crc >> 8) ^ crc32_lookup[0][(crc & 0xFF) ^ *current_char++];
  }

  return ~crc;
}
