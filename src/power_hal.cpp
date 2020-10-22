/*
 * @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
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

#include "power_hal.h"

int PsmNaviResetPower(HANDLE h_app, PsmFactorT reset_factor) {
  if (NULL == h_app) {
    return -1;
  }

  return 0;
}

int PsmClrRamJudgeFlgPower(HANDLE h_app) {
  if (NULL == h_app) {
    return -1;
  }

  return 0;
}

int PsmGetDramBackupStatusPower(HANDLE h_app, PsmSysupDramT *p_dram1, PsmSysupDramT *p_dram2, PsmSysupDramT *p_dram3) {
  if (NULL == h_app) {
    return -1;
  }

  return 0;
}

int PsmGetStartStatusPower(HANDLE h_app, PsmStartStatusT *p_start_sts) {
  if (NULL == h_app) {
    return -1;
  }

  return 0;
}

int PsmGetSyscomStatusPower(HANDLE h_app, PsmComStsT *p_com_sts) {
  if (NULL == h_app) {
    return -1;
  }

  return 0;
}

int PsmGetDramPowerSupplyAbnormalityCheckResultPower(HANDLE h_app, PsmDramStsT *p_dram_sts) {
  if (NULL == h_app) {
    return -1;
  }

  return 0;
}

int PsmGetAccCustomizeSetting(HANDLE h_app, PsmAccCustomizeT* p_acc_customize) {
  if (NULL == h_app) {
    return -1;
  }
  *p_acc_customize = PSM_ACC_CUSTOMIZE_ON;
  return 0;
}

int PsmChgAccCustomizeSetting(HANDLE h_app, PsmAccCustomizeT acc_customize) {
  if (NULL == h_app) {
    return -1;
  }

  return 0;
}

int PsmGetStartupFactor(HANDLE h_app, PsmFactorT* p_startup_factor) {
  if (NULL == h_app) {
    return -1;
  }
  *p_startup_factor = PSM_FACTOR_NONE;
  return 0;
}

int PowerHalSetResetInfo(enum PowerHalResetInfoId id, uint32_t val) {
  return 0;
}

int PowerHalGetResetInfo(enum PowerHalResetInfoId id, uint32_t *val) {
  return 0;
}
