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
/// \ingroup  tag_SS_RomAccess
/// \brief    This file supports ROM Access.
///
///////////////////////////////////////////////////////////////////////////////
#include "system_service/ss_sm_rom_access.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>

//#include <stub/el_mem.h>
#include <native_service/cl_lock.h>
#include <native_service/cl_lockid.h>

#include "ss_rom_access_if_romaccesslibrarylog.h"
#include "ss_sm_checksum.h"
#include "ss_rom_access_define.h"

#define ROM_ACCESS_DIR  "/nv/romaccess"
#define ROM_ACCESS_FILE "romdata.dat"
const CHAR kDefaultRomDevice[] = ROM_ACCESS_DIR"/"ROM_ACCESS_FILE;
const CHAR kRecoveryRomDevice[] = ROM_ACCESS_DIR"/"ROM_ACCESS_FILE".old";
static const uint8_t kMagic[] = { 0xDE, 0xAD, 0xBE, 0xEF };

typedef enum {
  INDEX_BOOTFLAG = 0,
  INDEX_ACTIVE_FLASHLOADER,
  INDEX_PARTITIONS_RW_FLAG,
  INDEX_LAST_USER_MODE,
  INDEX_TRANSPORT_MODE,
  INDEX_PRODUCTION_MODE,
  INDEX_LIMP_HOME_CUT_OFF_REQUEST,
  INDEX_DATARESET_MODE,
  INDEX_RESET_COUNT,
  INDEX_LAST_ILGRESET,
  INDEX_PROGUPDATE_STATE,
  INDEX_ERR_LOG_COUNT,
  INDEX_NEXT_WAKEUP_TYPE,
  INDEX_SIGNATURE = 15,  // As far as possible, do not modify this index value!!!!!
  INDEX_PROG_STATE = 20,
  INDEX_PROG_STATE_END = 63,
  INDEX_PRODUCT_PRIVATE,  // 64
  INDEX_PRODUCT_PRIVATE_END = (INDEX_PRODUCT_PRIVATE + ROM_PRODUCT_PRIVATE_MAX
      - 1),
  INDEX_MAX
} eRomIndices;

void* ROM_AccessIf::g_m_plock = NULL;
void* ROM_AccessIf::g_m_bakup_map = MAP_FAILED;
bool ROM_AccessIf::g_m_is_dram_valid = false;

typedef struct {
  char magic_in[4];
  bool is_dirty;
  UI_32 data[INDEX_MAX];
  uint32_t checksum;
  char magic_out[4];
} ROM_SM_DRAM_INFO_t;

ROM_AccessIf::ROM_AccessIf()
    : m_pBoot(NULL),
      m_norIsDirty(false) {
  bzero(&m_nor, sizeof(m_nor));

  if (g_m_plock == NULL) {
    g_m_plock = CL_LockMap(LOCK_ROM_ACCESS_IF);
    // LCOV_EXCL_BR_START 6:impossible to confirm because g_m_plock can not be changed
    if (g_m_plock == MAP_FAILED) {
    // LCOV_EXCL_BR_STOP
      ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 6:impossible to confirm because g_m_plock can not be changed
      // LCOV_EXCL_BR_START 6:impossible to confirm because g_m_plock can not be changed
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR, __FUNCTION__,
          "CL_LockMap Error, please check whether you call CL_LockProcessInit");
      // LCOV_EXCL_BR_STOP
    }
  }
  // LCOV_EXCL_BR_START 6:impossible to confirm because g_m_plock can not be changed
  if (g_m_plock) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_BR_START 6:impossible to confirm because g_m_plock can not be changed
    if (0 != CL_LockGet(g_m_plock)) {
    // LCOV_EXCL_BR_STOP
      ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 6:impossible to confirm because g_m_plock can not be changed
    }
  }
}

ROM_AccessIf::~ROM_AccessIf() {
  if (m_pBoot) {
    if (m_norIsDirty) {
      ROM_ACCESS_ASERT(eFrameworkunifiedStatusOK == m_pBoot->setBootInfo(&m_nor));  // LCOV_EXCL_BR_LINE 11:unexpected branch
    }
    delete m_pBoot;  // LCOV_EXCL_BR_LINE 11:unexpected branch
    m_pBoot = NULL;
  }
  // LCOV_EXCL_BR_START 6:impossible to confirm because g_m_plock can not be changed
  if (g_m_plock) {
    ROM_ACCESS_ASERT(0 == CL_LockRelease(g_m_plock));
  }  // LCOV_EXCL_BR_STOP
}

EFrameworkunifiedStatus ROM_AccessIf::LBA_Read(UI_32 offset, UI_32* buffer,
                                  UI_32 word_count) {
  ROM_SM_DRAM_INFO_t* p_buf;
  // LCOV_EXCL_BR_START 6:impossible to confirm because g_m_bakup_map can not be changed
  if (g_m_bakup_map == MAP_FAILED) {
  // LCOV_EXCL_BR_STOP
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " g_m_bakup_map uninitialize");
    return eFrameworkunifiedStatusFail;
  }

  p_buf = static_cast<ROM_SM_DRAM_INFO_t*>(g_m_bakup_map);
  memcpy(buffer, &p_buf->data[offset], word_count * sizeof(UI_32));

  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus ROM_AccessIf::LBA_Write(UI_32 offset, UI_32 value) {
  UI_32 l_value = value;
  return LBA_Write(offset, &l_value, sizeof(UI_32));
}

EFrameworkunifiedStatus ROM_AccessIf::LBA_Write(UI_32 offset, UI_32* value, UI_32 size) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  const CHAR *l_file = kDefaultRomDevice;
  SI_32 fd = -1;
  ROM_SM_DRAM_INFO_t* p_buf;
  if (g_m_bakup_map == MAP_FAILED) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " g_m_bakup_map uninitialize");
    return eFrameworkunifiedStatusFail;
  }

  p_buf = static_cast<ROM_SM_DRAM_INFO_t*>(g_m_bakup_map);

  if (memcmp(&p_buf->data[offset], value, size) == 0) {
    // Not Write when the value does not change
    return eFrameworkunifiedStatusOK;
  }

  p_buf->is_dirty = true;
  memcpy(&p_buf->data[offset], value, size);

  CSMChecksum l_checksum;
  p_buf->checksum = l_checksum.cal_checksum(&(p_buf->data),  // LCOV_EXCL_BR_LINE 11:unexpected branch
                                            sizeof(p_buf->data));  // LCOV_EXCL_BR_LINE 11:unexpected branch

  if (0 != rename(kDefaultRomDevice, kRecoveryRomDevice)) {
    ROM_ACCESS_ERRNO(0);
  } else {
    int sync_fd;
    sync_fd = open(kRecoveryRomDevice, O_RDWR | O_DSYNC | O_CLOEXEC);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    // LCOV_EXCL_BR_START 5:It's impossible to mock open() function,so this line can not be passed.
    if (sync_fd != -1) {
    // LCOV_EXCL_BR_STOP
      ROM_ACCESS_ERRNO(0 == fsync(sync_fd));  // LCOV_EXCL_BR_LINE 11:unexpected branch
      ROM_ACCESS_ERRNO(0 == close(sync_fd));  // LCOV_EXCL_BR_LINE 11:unexpected branch
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ROM_ACCESS_ERRNO(0);  // LCOV_EXCL_LINE 5:It's impossible to mock open() function,so this line can not be passed.
    }
  }
  fd = open(l_file, O_CREAT | O_TRUNC | O_RDWR | O_DSYNC | O_CLOEXEC, 00664);  // LCOV_EXCL_BR_LINE 11:unexpected branch
  if (fd == -1) {
    ROM_ACCESS_ERRNO(0);
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: open(%s) returned -1", l_file);
    eStatus = eFrameworkunifiedStatusFail;
    goto ERROR;
  }

  /** Write the value of 32-bits at offset location */
  if (sizeof(p_buf->data) != write(fd, p_buf->data, sizeof(p_buf->data))) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
    // LCOV_EXCL_START 5:It's impossible to mock write() function,so this line can not be passed.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    ROM_ACCESS_ERRNO(0);
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: write(%s) returned -1", l_file);
    eStatus = eFrameworkunifiedStatusFail;
    goto ERROR;
    // LCOV_EXCL_STOP
  } else {
    ROM_ACCESS_ERRNO(0 == close(fd));  // LCOV_EXCL_BR_LINE 11:unexpected branch
    fd = -1;

    if (0 != unlink(kRecoveryRomDevice)) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
      // LCOV_EXCL_START 5:It's impossible to mock unlink() function,so this line can not be passed.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ROM_ACCESS_ERRNO(0);
      // LCOV_EXCL_STOP
    }

    int sync_fd;
    sync_fd = open(ROM_ACCESS_DIR, O_RDONLY);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    // LCOV_EXCL_BR_START 5:It's impossible to mock open() function,so this line can not be passed.
    if (sync_fd != -1) {
      // LCOV_EXCL_BR_STOP
      ROM_ACCESS_ERRNO(0 == fsync(sync_fd));  // LCOV_EXCL_BR_LINE 11:unexpected branch
      ROM_ACCESS_ERRNO(0 == close(sync_fd));  // LCOV_EXCL_BR_LINE 11:unexpected branch
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ROM_ACCESS_ERRNO(0);  // LCOV_EXCL_LINE 5:It's impossible to mock open() function,so this line can not be passed.
    }
  }
  p_buf->is_dirty = false;
  ERROR: if (fd != -1) {  // LCOV_EXCL_BR_LINE 5:It's impossible to mock open() function,so this line can not be passed.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    close(fd);  // LCOV_EXCL_LINE 5:It's impossible to mock write() function,so this line can not be passed.
  }
  // @todo memory value is not restored if non-volatile area writing fails after writing to memory
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::GetDataResetMode_sub(EDATARESET_MODE* pDataResetMode) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Read((UI_32) INDEX_DATARESET_MODE,
                             reinterpret_cast<UI_32 *>(pDataResetMode), 1))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_LINE 11:unexpected branch
           " Error: LBA_Read() errored: 0x%X", eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

// This function may be called in duplicate,
// so it is necessary to operate normally even if it is called in duplicate.
EFrameworkunifiedStatus ROM_AccessIf::SystemInitialize(DRAM_BACKUP_STATE bkupState) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

  ROM_ACCESS_STATIC_ASERT(sizeof(ROM_SM_DRAM_INFO_t) <= SS_SYS_AREA_ROM_MAX_SIZE);

  // Checksum calculations expect 4-bytes alignments
  ROM_ACCESS_STATIC_ASERT((sizeof(ROM_SM_DRAM_INFO_t) % sizeof(UI_32)) == 0);
  // LCOV_EXCL_BR_START 6:impossible to confirm because g_m_bakup_map can not be changed
  if (g_m_bakup_map == MAP_FAILED) {
  // LCOV_EXCL_BR_STOP
//    g_m_bakup_map = EL_mem_exram_mmap(
//        EL_MEM_TYPE_REGION_SYS,
//        EL_mem_getOffset(EL_MEM_ID_SYS_BOOT) + SS_SYS_AREA_ROM_OFFSET,
//        SS_SYS_AREA_ROM_MAX_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
//        EL_MEM_CACHE_INVALID);
    if (g_m_bakup_map == MAP_FAILED) {
      ROM_ACCESS_ERRNO(0);
      return eFrameworkunifiedStatusFail;
    }
  }

  if (bkupState
      == DRAM_BACKUP_STATE_NG) {
    // BACKUP NG
    ROM_SM_DRAM_INFO_t* p_buf;
    CSMChecksum l_checksum;

    p_buf = static_cast<ROM_SM_DRAM_INFO_t*>(g_m_bakup_map);
    bzero(&(p_buf->magic_in), sizeof(p_buf->magic_in));
    bzero(&(p_buf->magic_out), sizeof(p_buf->magic_out));
    g_m_is_dram_valid = false;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::Initialize() {
  const CHAR *l_file = kDefaultRomDevice;
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CSMChecksum l_checksum;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  if (g_m_bakup_map == MAP_FAILED) {
//    g_m_bakup_map = EL_mem_exram_mmap(  // LCOV_EXCL_BR_LINE 11:unexpected branch
//        EL_MEM_TYPE_REGION_SYS,
//        EL_mem_getOffset(EL_MEM_ID_SYS_BOOT) + SS_SYS_AREA_ROM_OFFSET,  // LCOV_EXCL_BR_LINE 11:unexpected branch
//        SS_SYS_AREA_ROM_MAX_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
//        EL_MEM_CACHE_INVALID);
    // LCOV_EXCL_BR_START 6:impossible to confirm because g_m_bakup_map can not be changed
    if (g_m_bakup_map == MAP_FAILED) {
    // LCOV_EXCL_BR_STOP
      ROM_ACCESS_ERRNO(0);
      return eFrameworkunifiedStatusFail;
    }
  }
  if (!g_m_is_dram_valid) {
    SI_32 fd = -1;
    ROM_SM_DRAM_INFO_t* p_buf;
    p_buf = static_cast<ROM_SM_DRAM_INFO_t*>(g_m_bakup_map);
    if ((p_buf->is_dirty)
        || (memcmp(kMagic, p_buf->magic_in, sizeof(kMagic)) != 0)
        || (memcmp(kMagic, p_buf->magic_out, sizeof(kMagic)) != 0)
        || ((l_checksum.cal_checksum(&(p_buf->data), sizeof(p_buf->data)) != p_buf->checksum))) {
        // Need to synchronize the NVs with the data on the DRAM
      struct stat stutInfo;
      if (0 != stat(ROM_ACCESS_DIR, &stutInfo)) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
        // LCOV_EXCL_BR_START 5:It's impossible to mock stat() function,so this line can not be passed.
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "%s not found", ROM_ACCESS_DIR);
        // LCOV_EXCL_BR_STOP
        ROM_ACCESS_ASERT(0 == mkdir(ROM_ACCESS_DIR, 0775));  // LCOV_EXCL_BR_LINE 11:unexpected branch
      }
      int expectSize = (INT32)(INDEX_MAX * sizeof(UI_32));
      int sigSize = (INT32)((INDEX_SIGNATURE + 1) * sizeof(UI_32));

      if (access(kRecoveryRomDevice, F_OK) == 0) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
        // LCOV_EXCL_BR_START 5:It's impossible to mock access() function,so this line can not be passed.
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "%s is found", kRecoveryRomDevice);
        // LCOV_EXCL_BR_STOP
        if (0 != unlink(kDefaultRomDevice)) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
          // LCOV_EXCL_BR_START 5:It's impossible to mock unlink() function,so this line can not be passed.
          FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Error:unlink(%s)", kDefaultRomDevice);
          // LCOV_EXCL_BR_STOP
        }
        if (0 != rename(kRecoveryRomDevice, kDefaultRomDevice)) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
          ROM_ACCESS_ERRNO(0);  // LCOV_EXCL_BR_LINE 11:unexpected branch
        }
      }
      if (0 == stat(kDefaultRomDevice, &stutInfo)  // LCOV_EXCL_BR_LINE 11:unexpected branch
          && stutInfo.st_size >= sigSize) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
        // Use this value if there are more sizes than SIGNITURE
    #define ROM_ACCESS_INFO_SIZE (sizeof(ROM_SM_DRAM_INFO_t) - (sizeof(UI_32)*INDEX_MAX))
    #define ROM_ACCESS_MAX_SIZE  (INT32)(SS_SYS_AREA_ROM_MAX_SIZE - ROM_ACCESS_INFO_SIZE)
        // Maximum size of RAM area - An upper size limit of the management info size
        if (stutInfo.st_size > ROM_ACCESS_MAX_SIZE) {
          ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 11:unexpected branch
          stutInfo.st_size = ROM_ACCESS_MAX_SIZE;
        }
        fd = open(l_file,  // LCOV_EXCL_BR_LINE 11:unexpected branch
                  O_RDWR | O_DSYNC | O_CLOEXEC);  // LCOV_EXCL_BR_LINE 11:unexpected branch
        // LCOV_EXCL_BR_START 5:It's impossible to mock open() function,so this line can not be passed.
        if (fd != -1) {
          int ret;
          if (stutInfo.st_size < expectSize) {
            // If the files are large, APPEND them
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "PADDING");
            ret = ftruncate(fd, expectSize);
            ROM_ACCESS_ERRNO(ret == 0);
            fsync(fd);
          } else if (stutInfo.st_size > expectSize) {
            // If the files are small, TRANC them
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "CUT");
            ret = ftruncate(fd, expectSize);
            ROM_ACCESS_ERRNO(ret == 0);
            fsync(fd);
          }
        }
        // LCOV_EXCL_BR_STOP
        } else {
        // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "%s size is not crrect",
               kDefaultRomDevice);
        // LCOV_EXCL_BR_STOP
        fd = open(l_file, O_CREAT | O_TRUNC | O_RDWR | O_DSYNC | O_CLOEXEC,
                  00664);  // LCOV_EXCL_BR_LINE 11:unexpected branch
        // LCOV_EXCL_BR_START 5:It's impossible to mock open() function,so this line can not be passed.
        if (fd != -1) {
          UI_32 buf[INDEX_MAX];
          bzero(buf, sizeof(buf));
          ROM_ACCESS_ASERT(sizeof(buf) == write(fd, buf, sizeof(buf)))
          fsync(fd);
        }
      // LCOV_EXCL_BR_STOP
      }
      // LCOV_EXCL_BR_START 5:It's impossible to mock open() function,so this line can not be passed.
      if (fd != -1) {
      // LCOV_EXCL_BR_STOP
        UI_32 read_buf[INDEX_MAX];
        if (-1 == lseek(fd, 0, SEEK_SET)) {
          // LCOV_EXCL_START 5:It's impossible to mock lseek() function,so this line can not be passed.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          ROM_ACCESS_ERRNO(0);
          // LCOV_EXCL_STOP
        } else {
          if (-1 == read(fd, read_buf, sizeof(read_buf))) {
            // LCOV_EXCL_START 5:It's impossible to mock read() function,so this line can not be passed.
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            ROM_ACCESS_ERRNO(0);
            // LCOV_EXCL_STOP
          } else {
            memcpy(p_buf->magic_in, kMagic, sizeof(kMagic));
            memcpy(&p_buf->data, read_buf, sizeof(p_buf->data));
            p_buf->checksum = l_checksum.cal_checksum(&(p_buf->data),
                                                      sizeof(p_buf->data));
            memcpy(p_buf->magic_out, kMagic, sizeof(kMagic));
            p_buf->is_dirty = false;

            // Synchronization of NV data with data on the DRAM completed
            g_m_is_dram_valid = true;
          }
        }
        close(fd);
      } else {
        // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error:open(%s)", l_file);
        // LCOV_EXCL_BR_STOP
      }
    } else {
      // No need to synchronize NV data with data on the DRAM
      g_m_is_dram_valid = true;
    }
  } else {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " alreay initialized");
    // LCOV_EXCL_BR_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::GetSignature(UI_32* pSigValue) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Read((UI_32) INDEX_SIGNATURE,
                                          reinterpret_cast<UI_32 *>(pSigValue), 1))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_LINE 11:unexpected branch
           " Error: LBA_Read() errored: 0x%X", eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::SetSignature(UI_32 sigValue) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Write(INDEX_SIGNATURE,
                       sigValue))) {
  // LCOV_EXCL_BR_STOP
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: LBA_Write(0x%X) errored: 0x%X",
           sigValue, eStatus);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::GetBootMode(EBOOT_MODE* pBootMode) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Read((UI_32) INDEX_BOOTFLAG,
                             reinterpret_cast<UI_32 *>(pBootMode), 1))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error: LBA_Read() errored: 0x%X", eStatus);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::SetBootMode(EBOOT_MODE bootMode) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Write(INDEX_BOOTFLAG, bootMode))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 11:unexpected branch
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: LBA_Write(%d) errored: 0x%X",
           bootMode, eStatus);
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::GetLastUserMode(EUSER_MODE* pUserMode) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Read((UI_32) INDEX_LAST_USER_MODE,
                                          reinterpret_cast<UI_32 *>(pUserMode), 1))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_LINE 11:unexpected branch
           " Error: LBA_Read() errored: 0x%X", eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::SetLastUserMode(EUSER_MODE userMode) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Write(INDEX_LAST_USER_MODE, userMode))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 11:unexpected branch
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: LBA_Write(%s) errored: 0x%X",
           userMode == USER_OFF ? "USER_OFF" : "USER_ON", eStatus);
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::GetTransportMode(ECONTROL_MODE* pControlMode) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Read((UI_32) INDEX_TRANSPORT_MODE,
                             reinterpret_cast<UI_32 *>(pControlMode), 1))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_LINE 11:unexpected branch
           " Error: LBA_Read() errored: 0x%X", eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::SetTransportMode(ECONTROL_MODE controlMode) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK
      != (eStatus = LBA_Write(INDEX_TRANSPORT_MODE, controlMode))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 11:unexpected branch
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: LBA_Write(%s) errored: 0x%X",
           controlMode == DISABLE_MODE ? "DISABLE_MODE" : "ENABLE_MODE",
           eStatus);
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}


EFrameworkunifiedStatus ROM_AccessIf::GetProductionMode(ECONTROL_MODE* pControlMode) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Read((UI_32) INDEX_PRODUCTION_MODE,
                             reinterpret_cast<UI_32 *>(pControlMode), 1))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_LINE 11:unexpected branch
           " Error: LBA_Read() errored: 0x%X", eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::SetProductionMode(ECONTROL_MODE controlMode) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK
      != (eStatus = LBA_Write(INDEX_PRODUCTION_MODE, controlMode))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 11:unexpected branch
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: LBA_Write(%s) errored: 0x%X",
           controlMode == DISABLE_MODE ? "DISABLE_MODE" : "ENABLE_MODE",
           eStatus);
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::GetLimpHomeCutOffReqMode(ECONTROL_MODE* pControlMode) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Read((UI_32) INDEX_LIMP_HOME_CUT_OFF_REQUEST,
                             reinterpret_cast<UI_32 *>(pControlMode), 1))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_LINE 11:unexpected branch
           " Error: LBA_Read() errored: 0x%X", eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::SetLimpHomeCutOffReqMode(ECONTROL_MODE controlMode) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK
      != (eStatus = LBA_Write(INDEX_LIMP_HOME_CUT_OFF_REQUEST, controlMode))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 11:unexpected branch
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: LBA_Write(%s) errored: 0x%X",
           controlMode == DISABLE_MODE ? "DISABLE_MODE" : "ENABLE_MODE",
           eStatus);
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::GetDataResetMode(EDATARESET_MODE* pDataResetMode) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  try {
    UI_32 sts;
    if (m_pBoot == NULL) {  // LCOV_EXCL_BR_LINE 6:impossible to confirm because m_pBoot can not be changed
      m_pBoot = new BOOT_AccessIf(BAI_OPEN_RW);  // LCOV_EXCL_BR_LINE 11:unexpected branch
      if (eFrameworkunifiedStatusOK != m_pBoot->getBootInfo(&m_nor)) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
        ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 11:unexpected branch
        throw eFrameworkunifiedStatusFail;
      }
    }

    if ((m_nor.sts1 == m_nor.sts2) || (m_nor.sts1 == m_nor.sts3)) {
      sts = m_nor.sts1;
    } else if (m_nor.sts2 == m_nor.sts3) {
      sts = m_nor.sts2;
      ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    } else {
      sts = SYSUP_CND_NORMAL;
      ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    }

    if (sts == SYSUP_CND_FACTRESET) {
      *pDataResetMode = DATARESET_FACTORY;
    } else {
      eStatus = GetDataResetMode_sub(pDataResetMode);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    }
  } catch (const EFrameworkunifiedStatus& e) {
    eStatus = e;
  } catch (...) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    eStatus = eFrameworkunifiedStatusFail;
  }  // LCOV_EXCL_STOP

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::GetDataResetModeFast(EDATARESET_MODE* pDataResetMode) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  void* p_map = MAP_FAILED;
  size_t mapSize = 0;
  try {
    LBM_RAM_t* p_lbm = NULL;
    mapSize = SS_SYS_AREA_BOOT_MAX_SIZE;

//    if (EL_mem_getLength(EL_MEM_ID_SYS_BOOT) < SS_SYS_AREA_BOOT_MAX_SIZE) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
//      mapSize = EL_mem_getLength(EL_MEM_ID_SYS_BOOT);  // LCOV_EXCL_BR_LINE 11:unexpected branch
//    }
    // LCOV_EXCL_BR_START 11:unexpected branch
//    p_map = reinterpret_cast<LBM_RAM_t*>(EL_mem_exram_mmap(EL_MEM_TYPE_REGION_SYS,
//                                           EL_mem_getOffset(EL_MEM_ID_SYS_BOOT),
//                                           mapSize, PROT_READ | PROT_WRITE,
//                                           MAP_SHARED, EL_MEM_CACHE_INVALID));
    // LCOV_EXCL_BR_STOP
    if (p_map == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 6:impossible to confirm because p_map can not be changed
      ROM_ACCESS_ERRNO(0);  // LCOV_EXCL_BR_LINE 6:impossible to confirm because p_map can not be changed
      throw eFrameworkunifiedStatusFail;
    }
    p_lbm = static_cast<LBM_RAM_t*>(p_map);
    if (p_lbm->sts == SYSUP_CND_FACTRESET) {
      *pDataResetMode = DATARESET_FACTORY;
    } else {
      eStatus = GetDataResetMode_sub(pDataResetMode);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    }
  } catch (const EFrameworkunifiedStatus& e) {
    ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    eStatus = e;
  } catch (...) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    ROM_ACCESS_ASERT(0);
    eStatus = eFrameworkunifiedStatusFail;
  }  // LCOV_EXCL_STOP

  if (p_map != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 6:impossible to confirm because p_map can not be changed
//    ROM_ACCESS_ERRNO(0 == EL_mem_exram_munmap(p_map,  // LCOV_EXCL_BR_LINE 11:unexpected branch
//                        mapSize));
  }
  return eStatus;  // LCOV_EXCL_BR_LINE 11:unexpected branch
}

EFrameworkunifiedStatus ROM_AccessIf::SetDataResetMode(EDATARESET_MODE dataResetMode) {
  EFrameworkunifiedStatus eStatus;
  EDATARESET_MODE l_mode = dataResetMode;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  try {
    if (m_pBoot == NULL) {  // LCOV_EXCL_BR_LINE 6:impossible to confirm because m_pBoot can not be changed
      // LCOV_EXCL_BR_START 11:unexpected branch
      m_pBoot = new BOOT_AccessIf(BAI_OPEN_RW);
      // LCOV_EXCL_BR_STOP
      if (eFrameworkunifiedStatusOK != m_pBoot->getBootInfo(&m_nor)) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
        ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 11:unexpected branch
        throw eFrameworkunifiedStatusFail;
      }
    }
    m_norIsDirty = true;
    if (dataResetMode == DATARESET_FACTORY) {
      m_nor.sts1 = m_nor.sts2 = m_nor.sts3 = SYSUP_CND_FACTRESET;
      l_mode = DATARESET_NONE;
    } else {
      m_nor.sts1 = m_nor.sts2 = m_nor.sts3 = SYSUP_CND_NORMAL;
    }

    // LCOV_EXCL_BR_START 11:unexpected branch
    if (eFrameworkunifiedStatusOK != (eStatus = LBA_Write(INDEX_DATARESET_MODE, l_mode))) {
    // LCOV_EXCL_BR_STOP
      // LCOV_EXCL_START 11:unexpected branch
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: LBA_Write() errored: 0x%X",
             eStatus);
      // LCOV_EXCL_STOP
    }
  } catch (const EFrameworkunifiedStatus& e) {
    ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    eStatus = e;
  } catch (...) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    ROM_ACCESS_ASERT(0);
    eStatus = eFrameworkunifiedStatusFail;
  }  // LCOV_EXCL_STOP

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::GetResetCount(UI_32* pResetCount) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Read((UI_32) INDEX_RESET_COUNT, reinterpret_cast<UI_32 *>(pResetCount),
                             1))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_LINE 11:unexpected branch
           " Error: LBA_Read() errored: 0x%X", eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
EFrameworkunifiedStatus ROM_AccessIf::SetResetCount(UI_32 resetCount) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  if (eFrameworkunifiedStatusOK != (eStatus =  // LCOV_EXCL_BR_LINE 11:unexpected branch
      LBA_Write(INDEX_RESET_COUNT, resetCount))) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: LBA_Write() errored: 0x%X",  // LCOV_EXCL_LINE 11:unexpected branch
           eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}
EFrameworkunifiedStatus ROM_AccessIf::GetLastIlgReset(ELASTILGRESET_MODE* pLastIlgReset) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Read((UI_32) INDEX_LAST_ILGRESET,
                             reinterpret_cast<UI_32 *>(pLastIlgReset), 1))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_LINE 11:unexpected branch
           " Error: LBA_Read() errored: 0x%X", eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::SetLastIlgReset(ELASTILGRESET_MODE lastIlgReset) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK
      != (eStatus = LBA_Write(INDEX_LAST_ILGRESET, lastIlgReset))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: LBA_Write() errored: 0x%X",  // LCOV_EXCL_LINE 11:unexpected branch
           eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::GetProgUpdateState(
    EPROGUPDATE_STATE* pProgUpdateState) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Read((UI_32) INDEX_PROGUPDATE_STATE,
                             reinterpret_cast<UI_32 *>(pProgUpdateState), 1))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_LINE 11:unexpected branch
           " Error: LBA_Read() errored: 0x%X", eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::SetProgUpdateState(EPROGUPDATE_STATE progUpdateState) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK
      != (eStatus = LBA_Write(INDEX_PROGUPDATE_STATE, progUpdateState))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: LBA_Write() errored: 0x%X",  // LCOV_EXCL_LINE 11:unexpected branch
           eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::GetErrLogCount(UI_32* pErrLogCount) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Read((UI_32) INDEX_ERR_LOG_COUNT,
                             reinterpret_cast<UI_32 *>(pErrLogCount), 1))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_LINE 11:unexpected branch
           " Error: LBA_Read() errored: 0x%X", eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::SetErrLogCount(UI_32 errLogCount) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (eFrameworkunifiedStatusOK != (eStatus =  // LCOV_EXCL_BR_LINE 11:unexpected branch
      LBA_Write(INDEX_ERR_LOG_COUNT, errLogCount))) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: LBA_Write() errored: 0x%X",  // LCOV_EXCL_LINE 11:unexpected branch
           eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::GetNextWakeupType(ENEXT_WAKEUP_TYPE *pNextWakeupType) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK != (eStatus = LBA_Read((UI_32) INDEX_NEXT_WAKEUP_TYPE,
                             reinterpret_cast<UI_32 *>(pNextWakeupType), 1))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_LINE 11:unexpected branch
           " Error: LBA_Read() errored: 0x%X", eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::SetNextWakeupType(ENEXT_WAKEUP_TYPE nextWakeupType) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK
      != (eStatus = LBA_Write(INDEX_NEXT_WAKEUP_TYPE, nextWakeupType))) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,  // LCOV_EXCL_LINE 11:unexpected branch
           " Error: LBA_Write() errored: 0x%X", eStatus);  // LCOV_EXCL_LINE 11:unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::SetActiveFlashloader(
    EACTIVE_FLASHLOADER activeFlashloader) {
  EFrameworkunifiedStatus eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK
      != (eStatus = LBA_Write(INDEX_ACTIVE_FLASHLOADER, activeFlashloader))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 11:unexpected branch
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error: LBA_Write(%s) errored: 0x%X",
        activeFlashloader == NEW_FLASHLOADER ?
            "NEW_FLASHLOADER" : "OLD_FLASHLOADER",
        eStatus);
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::SetProductPrivate(UI_8 buf[ROM_PRODUCT_PRIVATE_MAX]) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK
      != (eStatus = LBA_Write(INDEX_PRODUCT_PRIVATE, reinterpret_cast<UI_32*>(buf),
                              ROM_PRODUCT_PRIVATE_MAX / sizeof(UI_32)))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 11:unexpected branch
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: LBA_Write() errored: 0x%X",
           eStatus);
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus ROM_AccessIf::GetProductPrivate(UI_8 buf[ROM_PRODUCT_PRIVATE_MAX]) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // LCOV_EXCL_BR_START 11:unexpected branch
  if (eFrameworkunifiedStatusOK
      != (eStatus = LBA_Read(INDEX_PRODUCT_PRIVATE, reinterpret_cast<UI_32 *>(buf),
                             ROM_PRODUCT_PRIVATE_MAX / sizeof(UI_32)))) {
  // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 11:unexpected branch
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: LBA_Read() errored: 0x%X", eStatus);
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

  return eStatus;
}
