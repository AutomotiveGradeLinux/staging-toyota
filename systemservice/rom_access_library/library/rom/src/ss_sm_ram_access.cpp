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

#include "system_service/ss_sm_ram_access.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <native_service/cl_lock.h>
#include <native_service/cl_lockid.h>
#include <stub/el_mem.h>
#include "system_service/ss_sm_boot_access.h"
#include "ss_rom_access_if_romaccesslibrarylog.h"
#include "ss_sm_checksum.h"
#include "ss_rom_access_define.h"



#define RAM_PATH     "/nv/BS/ss/rom_access_library/rwdata/ramdata.dat"
#define RAM_OLD_PATH RAM_PATH".old"
static const uint8_t kSigNature[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static const uint8_t kMagic[] = { 0xDE, 0xAD, 0xBE, 0xEF };

void* RAM_AccessIf::g_m_plock = NULL;
void* RAM_AccessIf::g_m_bakup_map = MAP_FAILED;
bool RAM_AccessIf::g_m_is_first_access = false;

typedef struct {
  char magic_in[4];
  RAM_SM_INFO_t data0;
  uint32_t checksum0;
  RAM_SM_INFO_t data1;
  uint32_t checksum1;
  char magic_out[4];
} RAM_SM_DRAM_INFO_t;

void RAM_AccessIf::initRamBackupEnable(RAM_WAKEUP_STATE wupState) {
  RAM_SM_DRAM_INFO_t* p_buf;
  CSMChecksum l_checksum;

  ROM_ACCESS_STATIC_ASERT(sizeof(RAM_SM_DRAM_INFO_t) <= SS_SYS_AREA_RAM_MAX_SIZE);
  // Checking sizes because checksum calculations expect 4 bytes alignments
  ROM_ACCESS_STATIC_ASERT((sizeof(RAM_SM_DRAM_INFO_t) % sizeof(UI_32)) == 0);

  if (g_m_bakup_map == MAP_FAILED) {
    g_m_bakup_map = EL_mem_exram_mmap(  // LCOV_EXCL_BR_LINE 11:unexpected branch
        EL_MEM_TYPE_REGION_SYS,
        EL_mem_getOffset(EL_MEM_ID_SYS_BOOT) + SS_SYS_AREA_RAM_OFFSET,  // LCOV_EXCL_BR_LINE 11:unexpected branch
        SS_SYS_AREA_RAM_MAX_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
        EL_MEM_CACHE_INVALID);
    // LCOV_EXCL_BR_START 6:impossible to confirm because g_m_bakup_map can not be changed
    if (g_m_bakup_map == MAP_FAILED) {
    // LCOV_EXCL_BR_STOP
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ROM_ACCESS_ERRNO(0);  // LCOV_EXCL_LINE 6:impossible to confirm because g_m_bakup_map can not be changed
      return;  // LCOV_EXCL_LINE 6:impossible to confirm because g_m_bakup_map can not be changed
    }
  }
  p_buf = static_cast<RAM_SM_DRAM_INFO_t*>(g_m_bakup_map);

  // LCOV_EXCL_BR_START 6:impossible to confirm because wupState can not be changed
  if ((RAM_WAKEUP_STATE_BACKUP_NG == wupState) ||
      (RAM_WAKEUP_STATE_BATTERY_DOWN == wupState) ||
      (memcmp(p_buf->magic_out, kMagic, sizeof(kMagic)) != 0) ||
      (memcmp(p_buf->magic_in, p_buf->magic_out, sizeof(p_buf->magic_in)) != 0)) {
  // LCOV_EXCL_BR_STOP
    // BACKUP NG
    g_m_is_first_access = false;
    bzero(p_buf, sizeof(RAM_SM_DRAM_INFO_t));
  }

  if (!g_m_is_first_access) {
    // LCOV_EXCL_BR_START 11:unexpected branch
    if ((memcmp(p_buf->data0.signature_in, kSigNature, sizeof(kSigNature)) == 0)
        && (memcmp(p_buf->data0.signature_out, kSigNature,
                   sizeof(kSigNature)) == 0)
        && (l_checksum.cal_checksum(&(p_buf->data0), sizeof(p_buf->data0)) == p_buf->checksum0)) {
    // LCOV_EXCL_BR_STOP
      // Use data0 content
      memcpy(&(p_buf->data1), &(p_buf->data0), sizeof(p_buf->data1));
      p_buf->checksum1 = p_buf->checksum0;
    // LCOV_EXCL_BR_START 11:unexpected branch
    } else if ((memcmp(p_buf->data1.signature_in, kSigNature, sizeof(kSigNature)) == 0)
        && (memcmp(p_buf->data1.signature_out, kSigNature,
                   sizeof(kSigNature)) == 0)
        && (l_checksum.cal_checksum(&(p_buf->data1), sizeof(p_buf->data1)) == p_buf->checksum1)) {
    // LCOV_EXCL_BR_STOP
      // Use data1 content
      memcpy(&(p_buf->data0), &(p_buf->data1), sizeof(p_buf->data0));
      p_buf->checksum0 = p_buf->checksum1;
    } else {
      // BACKUP NG or SIGNATURE CHECK or CHECK SUM NG
      bzero(p_buf, sizeof(RAM_SM_DRAM_INFO_t));
      memcpy(p_buf->magic_in, kMagic, sizeof(kMagic));
      // data0
      memcpy(p_buf->data0.signature_in, kSigNature, sizeof(kSigNature));
      memcpy(p_buf->data0.signature_out, kSigNature, sizeof(kSigNature));
      p_buf->checksum0 =
          l_checksum.cal_checksum(&(p_buf->data0), sizeof(p_buf->data0));  // LCOV_EXCL_BR_LINE 11:unexpected branch
      // data1
      memcpy(p_buf->data1.signature_in, kSigNature, sizeof(kSigNature));
      memcpy(p_buf->data1.signature_out, kSigNature, sizeof(kSigNature));
      p_buf->checksum1 =
          l_checksum.cal_checksum(&(p_buf->data1), sizeof(p_buf->data1));  // LCOV_EXCL_BR_LINE 11:unexpected branch
      memcpy(p_buf->magic_out, kMagic, sizeof(kMagic));
    }
  }
  memcpy(&m_buf, &(p_buf->data0), sizeof(p_buf->data0));
}

void RAM_AccessIf::finalRamBackupEnable(void) {
  RAM_SM_DRAM_INFO_t* p_buf;
  CSMChecksum l_checksum;
  // LCOV_EXCL_BR_START 6:impossible to confirm because g_m_bakup_map can not be changed
  if (g_m_bakup_map == MAP_FAILED) {
  // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    ROM_ACCESS_ASERT(0);  // LCOV_EXCL_LINE 6:impossible to confirm because g_m_bakup_map can not be changed
    return;  // LCOV_EXCL_LINE 6:impossible to confirm because g_m_bakup_map can not be changed
  }

  p_buf = static_cast<RAM_SM_DRAM_INFO_t*>(g_m_bakup_map);

  // kSigNature is cleared during writing
  bzero(m_buf.signature_in, sizeof(kSigNature));

  // data0
  memcpy(&(p_buf->data0), &m_buf, sizeof(p_buf->data0));
  memcpy(p_buf->data0.signature_in, kSigNature, sizeof(kSigNature));
  p_buf->checksum0 = l_checksum.cal_checksum(&(p_buf->data0),  // LCOV_EXCL_BR_LINE 11:unexpected branch
                                             sizeof(p_buf->data0));

  // data1
  memcpy(&(p_buf->data1), &m_buf, sizeof(p_buf->data1));
  memcpy(p_buf->data1.signature_in, kSigNature, sizeof(kSigNature));
  p_buf->checksum1 = l_checksum.cal_checksum(&(p_buf->data1),  // LCOV_EXCL_BR_LINE 11:unexpected branch
                                             sizeof(p_buf->data1));

  // Set kSigNature
  memcpy(m_buf.signature_in, kSigNature,
         sizeof(kSigNature));
}

RAM_AccessIf::RAM_AccessIf(
    RAM_WAKEUP_STATE wupState /*  = RAM_WAKEUP_STATE_DONT_CARE */)
    : m_buf(),
      m_dirty(false) {

  if (g_m_plock == NULL) {  // LCOV_EXCL_BR_LINE 6:impossible to confirm because g_m_plock can not be changed
    g_m_plock = CL_LockMap(LOCK_RAM_ACCESS_IF);
    if (g_m_plock == MAP_FAILED) {
      ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 6:impossible to confirm because g_m_plock can not be changed
      // LCOV_EXCL_BR_START 6:impossible to confirm because g_m_plock can not be changed
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR, __FUNCTION__,
          "CL_LockMap Error, please check whether you call CL_LockProcessInit");
      // LCOV_EXCL_BR_STOP
      g_m_plock = NULL;
    }
  }
  if (g_m_plock) {
    if (0 != CL_LockGet(g_m_plock)) {
      ROM_ACCESS_ASERT(0);
    }
  }

  if (RAM_WAKEUP_STATE_BATTERY_DOWN == wupState) {    // LCOV_EXCL_BR_LINE 13:
    // Models without DRAM backups are initialized when +B is down.
    // Deletes files because models with DRAM backups are also backwards compatible
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    unlink(RAM_PATH);  // LCOV_EXCL_LINE 13:
    unlink(RAM_OLD_PATH);  // LCOV_EXCL_LINE 13:
  }

  initRamBackupEnable(wupState);
  g_m_is_first_access = true;
}

RAM_AccessIf::~RAM_AccessIf() {
  if (m_dirty) {
    finalRamBackupEnable();
  }
  if (g_m_plock) {
    ROM_ACCESS_ASERT(0 == CL_LockRelease(g_m_plock));
  }

  return;
}

EFrameworkunifiedStatus RAM_AccessIf::getRamInfo(RAM_SM_INFO_t* p_bootInfo) {
  *p_bootInfo = m_buf;
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus RAM_AccessIf::setRamInfo(RAM_SM_INFO_t* p_bootInfo) {
  if (0 != memcmp(&m_buf, p_bootInfo, sizeof(m_buf))) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
    m_buf = *p_bootInfo;
    m_dirty = true;
  }

  return eFrameworkunifiedStatusOK;
}
