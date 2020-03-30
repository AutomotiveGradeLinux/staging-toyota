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

#include "system_service/ss_sm_boot_access.h"
#include <stdio.h>
#include <native_service/cl_lock.h>
#include <native_service/cl_lockid.h>
#include "system_service/nor_backup.h"
#include "ss_rom_access_if_romaccesslibrarylog.h"
#include "ss_rom_access_define.h"

#define BAI_MTD_DEV_RO "/dev/mtd1ro"  // NOR device for RO
#define BAI_MTD_DEV_RW "/dev/mtd1"    // NOR device for RW

#define BAI_SERCTOR_OFFSET  0
#define BAI_DATA_OFFSET     0

void* BOOT_AccessIf::g_m_plock = NULL;

BOOT_AccessIf::BOOT_AccessIf(BAI_OPEN_t type)
    : m_type(type) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (!(m_type == BAI_OPEN_RO  // LCOV_EXCL_BR_LINE 6: double check
      || m_type == BAI_OPEN_RW)) {  // LCOV_EXCL_BR_LINE 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    throw __LINE__;  // LCOV_EXCL_LINE 6: double check
  }

  if (g_m_plock == NULL) {
    g_m_plock = CL_LockMap(LOCK_BOOT_ACCESS_IF);

    if (g_m_plock == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 6:impossible to confirm because g_m_plock can not be changed
      ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 6:impossible to confirm because g_m_plock can not be changed
      // LCOV_EXCL_BR_START 6:impossible to confirm because g_m_plock can not be changed
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR, __FUNCTION__,
          "CL_LockMap Error, please check whether you call CL_LockProcessInit");
      // LCOV_EXCL_BR_STOP
    }
  }

  if (g_m_plock) {  // LCOV_EXCL_BR_LINE 6:impossible to confirm because g_m_plock can not be changed
    ROM_ACCESS_ASERT(0 == CL_LockGet(g_m_plock));  // LCOV_EXCL_BR_LINE 11:unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

BOOT_AccessIf::~BOOT_AccessIf() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (g_m_plock) {  // LCOV_EXCL_BR_LINE 6:impossible to confirm because g_m_plock can not be changed
    ROM_ACCESS_ASERT(0 == CL_LockRelease(g_m_plock));  // LCOV_EXCL_BR_LINE 11:unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

EFrameworkunifiedStatus BOOT_AccessIf::getBootInfo(LBM_NOR_t* p_bootInfo) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eRet = eFrameworkunifiedStatusOK;
  try {
    int ret;
    if (p_bootInfo == NULL) {
      ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 11:unexpected branch
      throw __LINE__;
    }
    ret = mtdn_backup_Read((const char *) BAI_MTD_DEV_RO, BAI_SERCTOR_OFFSET,
                           BAI_DATA_OFFSET, sizeof(LBM_NOR_t), p_bootInfo);  // LCOV_EXCL_BR_LINE 11:unexpected branch

    if (ret != 0) {
      ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 11:unexpected branch
      throw __LINE__;
    }
  } catch (...) {
    eRet = eFrameworkunifiedStatusFail;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eRet;
}

EFrameworkunifiedStatus BOOT_AccessIf::setBootInfo(LBM_NOR_t* p_bootInfo) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eRet = eFrameworkunifiedStatusOK;
  try {
    int ret;
    if (p_bootInfo == NULL) {
      ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 11:unexpected branch
      throw __LINE__;
    }
    if (m_type != BAI_OPEN_RW) {
      ROM_ACCESS_ASERT(0);  // LCOV_EXCL_BR_LINE 11:unexpected branch
      throw __LINE__;
    }
    ret = mtdn_backup_Write((const char *) BAI_MTD_DEV_RW, BAI_SERCTOR_OFFSET,
                            BAI_DATA_OFFSET, sizeof(LBM_NOR_t), p_bootInfo);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    if (ret != 0) {  // LCOV_EXCL_BR_LINE 5:c API error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ROM_ACCESS_ASERT(0);  // LCOV_EXCL_LINE 5:c API error case
      throw __LINE__;  // LCOV_EXCL_LINE 5:c API error case
    }
  } catch (...) {
    eRet = eFrameworkunifiedStatusFail;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eRet;
}

