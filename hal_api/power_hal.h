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

/**
 * @file power_hal.h
 * @brief Interface and struct define for power_hal
 */

#ifndef HAL_API_POWER_HAL_H_
#define HAL_API_POWER_HAL_H_

#include <native_service/frameworkunified_types.h>

/** @addtogroup system_manager
 *  @{
 */
/** @addtogroup power_hal
 *  @ingroup system_manager
 *  @{
 */
/** @}*/  // end of power_hal
/** @}*/  // end of system_manager

/** @addtogroup power_supply_manager_shadow
 *  @{
 */
/** @addtogroup power_hal
 *  @ingroup power_supply_manager_shadow
 *  @{
 */

/**
 * \~english Definition of program update is not occured
 */
#define PWR_PROGUPDATE_STATE_NONE         (0x0)

/**
 * \~english Bit position of program update
 */
#define PWR_PROGUPDATE_STATE_UPDATED         (0x01)

/**
 * \~english Bit position of map update
 */
#define PWR_PROGUPDATE_STATE_MAP_UPDATED     (0x02)

/**
 * \~english Bit position of map difference update
 */
#define PWR_PROGUPDATE_STATE_MAPDIFF_UPDATED (0x04)

/**
 * \~english None reset state
 */
#define PWR_DATA_RESET_STATE_NONE       0

/**
 * \~english Reset state for initialization of user information
 */
#define PWR_DATA_RESET_STATE_USER       1

/**
 * \~english Reset state for factory initialization
 */
#define PWR_DATA_RESET_STATE_FACTORY    2

/*
 * @~english
 * @brief Command ID  power_hal -> User
 */
/*! @~english power_hal command receive notification */
#define CID_PWR_CMD_RCV      0x0103

/*! @~english power_hal command send result notification */
#define CID_PWR_CMD_SND_STS  0x0104

/**
 * \~english DRAM backup status
 */
enum PsmSysupDramT {
  /**
   * \~english normal value
   */
  PSM_SYSUP_DRAM_NORMAL,

  /**
   * \~english abnormal Soft Reset(software abnormality)
   */
  PSM_SYSUP_DRAM_SFTERRSR,

  /**
   * \~english need to discard backup dram
   */
  PSM_SYSUP_DRAM_DISCARD,

  /**
   * \~english max value
   */
  PSM_SYSUP_DRAM_MAX,
};

/**
 * \~english Start Status
 */
enum PsmStartStatusT {
  /**
   * \~english cold start
   */
  PSM_START_STATUS_FACTRESET,

  /**
   * \~english hot start
   */
  PSM_START_STATUS_NORMAL,

  /**
   * \~english max value
   */
  PSM_START_STATUS_MAX,
};

/**
 * \~english Communication state
 */
enum PsmComStsT {
  /**
   * \~english Communication normal
   */
  PSM_COM_STS_NORMAL,

  /**
   * \~english Communication abnormal
   */
  PSM_COM_STS_ABNORMAL,

  /**
   * \~english max value
   */
  PSM_COM_STS_MAX,
};

/**
 * \~english DRAM power supply abnormality check result
 */
enum PsmDramStsT {
  /**
   * \~english DRAM power supply abnormality check result normal
   */
  PSM_DRAM_STS_NORMAL,

  /**
   * \~english DRAM power supply abnormality check result abnormal
   */
  PSM_DRAM_STS_ABNORMAL,

  /**
   * \~english max value
   */
  PSM_DRAM_STS_MAX,
};

/**
 * \~english ACC customize setting.
 */
enum PsmAccCustomizeT {
  /**
   * \~english ACC customize setting OFF.
   */
  PSM_ACC_CUSTOMIZE_OFF,
  /**
   * \~english ACC customize setting ON.
   */
  PSM_ACC_CUSTOMIZE_ON
};

/**
 * \~english Reset history recording flag.
 */
#define PSM_RESET_HISTORY   (0x10000000)

/**
 * \~english Reset(Start-up) factor.
 */
enum PsmFactorT {
  /**
   * \~english none
   */
  PSM_FACTOR_NONE,
  /**
   * \~english Restart due to AGL.
   */
  PSM_FACTOR_AGL,
  /**
   * \~english Restart due to Tier1.
   */
  PSM_FACTOR_TIER1,
  /**
   * \~english Restart due to user.
   */
  PSM_FACTOR_USER,
  /**
   * \~english Restart due to AGL(Need to record reset history).
   */
  PSM_FACTOR_AGL_WITH_HISTORY = (PSM_RESET_HISTORY | PSM_FACTOR_AGL),
  /**
   * \~english Restart due to Tier1(Need to record reset history).
   */
  PSM_FACTOR_TIER1_WITH_HISTORY = (PSM_RESET_HISTORY | PSM_FACTOR_TIER1),
  /**
   * \~english Restart due to user(Need to record reset history).
   */
  PSM_FACTOR_USER_WITH_HISTORY = (PSM_RESET_HISTORY | PSM_FACTOR_USER)
};

/**
 * \~english Reset information type.
 */
enum PowerHalResetInfoId {
  /**
   * \~english Count of error log.
   */
  AGL_ERRLOG_COUNTER = 0,
  /**
   * \~english Abnormal reset count detected by AGL.
   */
  AGL_RESET_COUNTER,
  /**
   * \~english State of program update.
   */
  AGL_PROGUPDATE_STATE,
  /**
   * \~english Data reset state.
   */
  AGL_DATARESET_STATE,
  /**
   * \~english Flag of abnormal reset.
   */
  AGL_ILLRESET_FLAG,
  /**
   * \~english Max of reset information type.
   */
  AGL_RESERVED_LIMIT = 32
};

/**
 * \ingroup PsmNaviResetPower
 * \~english @par Brief
 *         API to do navi reset that is forced reset if end sequence of MainSoC is not completed.
 * \~english @param [in] h_app Handle for Application
 * \~english @param [in] reset_factor Reset factor
 * \~english @retval 0   :  Ok
 * \~english @retval -1 :  Failed
 * \~english @par Prerequisite
 *        - None
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Internal IO error. [-1]
 * \~english @par Detail
 *        - do navi reset process.
 *        - The API can only be used by 1 process
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        None
 */
int PsmNaviResetPower(HANDLE h_app, PsmFactorT reset_factor);

/**
 * \ingroup PsmClrRamJudgeFlgPower
 * \~english @par Brief
 *         API to do ram judge flag clear
 * \~english @param [in] h_app Handle for Application
 * \~english @retval 0   :  Ok
 * \~english @retval -1 :  Failed
 * \~english @par Prerequisite
 *        - None
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Internal IO error. [-1]
 * \~english @par Detail
 *        - do ram judge flag clear process.
 *        - The API can only be used by 1 process
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        None
 */
int PsmClrRamJudgeFlgPower(HANDLE h_app);

/**
 * \ingroup PsmGetDramBackupStatusPower
 * \~english @par Brief
 *         API to do get dram backup status
 * \~english @param [in] h_app Handle for Application
 * \~english @param [out] p_dram1 DRAM1 BackUp Status
 * \~english @param [out] p_dram2 DRAM2 BackUp Status
 * \~english @param [out] p_dram3 DRAM3 BackUp Status
 * \~english @retval 0   :  Ok
 * \~english @retval -1 :  Failed
 * \~english @par Prerequisite
 *        - None
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Internal IO error. [-1]
 * \~english @par Detail
 *        - do get dram backup status process.
 *        - The API can only be used by 1 process
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        None
 */
int PsmGetDramBackupStatusPower(HANDLE h_app, PsmSysupDramT *p_dram1, PsmSysupDramT *p_dram2, PsmSysupDramT *p_dram3);

/**
 * \ingroup PsmGetStartStatusPower
 * \~english @par Brief
 *         API to do get start status
 * \~english @param [in] h_app Handle for Application
 * \~english @param [out] p_start_sts start status
 * \~english @retval 0   :  Ok
 * \~english @retval -1 :  Failed
 * \~english @par Prerequisite
 *        - None
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Internal IO error. [-1]
 * \~english @par Detail
 *        - do get startup state process.
 *        - The API can only be used by 1 process
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        None
 */
int PsmGetStartStatusPower(HANDLE h_app, PsmStartStatusT *p_start_sts);

/**
 * \ingroup PsmGetSyscomStatusPower
 * \~english @par Brief
 *         API to do get communication status between mainsoc-sys
 * \~english @param [in] h_app Handle for Application
 * \~english @param [out] p_com_sts communication status between mainsoc-sys
 * \~english @retval 0   :  Ok
 * \~english @retval -1 :  Failed
 * \~english @par Prerequisite
 *        - None
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Internal IO error. [-1]
 * \~english @par Detail
 *        - do get communication status between mainsoc-sys process.
 *        - The API can only be used by 1 process
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        None
 */
int PsmGetSyscomStatusPower(HANDLE h_app, PsmComStsT *p_com_sts);

/**
 * \ingroup PsmGetDramPowerSupplyAbnormalityCheckResultPower
 * \~english @par Brief
 *         API to do get dram power supply abnormality check result
 * \~english @param [in] h_app Handle for Application
 * \~english @param [out] p_dram_sts DRAM power supply abnormality check result
 * \~english @retval 0   :  Ok
 * \~english @retval -1 :  Failed
 * \~english @par Prerequisite
 *        - None
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Internal IO error. [-1]
 * \~english @par Detail
 *        - do get dram power supply abnormality check result process.
 *        - The API can only be used by 1 process
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        None
 */
int PsmGetDramPowerSupplyAbnormalityCheckResultPower(HANDLE h_app, PsmDramStsT *p_dram_sts);

/**
 * \ingroup PsmGetAccCustomizeSetting
 * \~english @par Brief
 *         API to do get ACC customize setting
 * \~english @param [in] h_app Handle for Application
 * \~english @param [out] p_acc_customize ACC customize Setting
 * \~english @retval 0   :  Ok
 * \~english @retval -1 :  Failed
 * \~english @par Prerequisite
 *        - None
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Internal IO error. [-1]
 * \~english @par Detail
 *        - do get ACC customize setting.
 *        - The API can only be used by 1 process
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        None
 */
int PsmGetAccCustomizeSetting(HANDLE h_app, PsmAccCustomizeT* p_acc_customize);

/**
 * \ingroup PsmChgAccCustomizeSetting
 * \~english @par Brief
 *         API to do change ACC customize setting
 * \~english @param [in] h_app Handle for Application
 * \~english @param [in] acc_customize ACC customize Setting
 * \~english @retval 0   :  Ok
 * \~english @retval -1 :  Failed
 * \~english @par Prerequisite
 *        - None
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Internal IO error. [-1]
 * \~english @par Detail
 *        - do set ACC customize setting.
 *        - The API can only be used by 1 process
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        None
 */
int PsmChgAccCustomizeSetting(HANDLE h_app, PsmAccCustomizeT acc_customize);

/**
 * \ingroup PsmGetStartupFactor
 * \~english @par Brief
 *         API to do get start-up factor.
 * \~english @param [in] h_app Handle for Application
 * \~english @param [out] p_startup_factor Start-up factor
 * \~english @retval 0   :  Ok
 * \~english @retval -1 :  Failed
 * \~english @par Prerequisite
 *        - None
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Internal IO error. [-1]
 * \~english @par Detail
 *        - do get Start-up factor.
 *        - After the API call, start-up factor is clear.
 *        - The API can only be used by 1 process
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        None
 */
int PsmGetStartupFactor(HANDLE h_app, PsmFactorT* p_startup_factor);

/**
 * \ingroup PowerHalSetResetInfo
 * \~english @par Brief
 *         API to do set reset information.
 * \~english @param [in] id Reset information type.
 * \~english @param [in] val setting value.
 * \~english @retval 0   :  Ok
 * \~english @retval -1 :  Failed
 * \~english @par Prerequisite
 *        - None
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Internal IO error. [-1]
 *        - Save error to non-volatile memory. [-1]
 * \~english @par Detail
 *        - Do set value to Reset information type(\ref PowerHalResetInfoId).  \n
 *          \ref AGL_PROGUPDATE_STATE is not applicable(Only get value used by \ref PowerHalGetResetInfo). \n
 *          Domains of each reset information type are as below. \n
 *          \ref AGL_ERRLOG_COUNTER : 0 <= val <= UINT32_MAX \n
 *          \ref AGL_RESET_COUNTER : 0 <= val <= UINT32_MAX \n
 *          \ref AGL_DATARESET_STATE : It is set as below in each state of data reset. \n
 *           None reset state == \ref PWR_DATA_RESET_STATE_NONE \n
 *           Reset state for initialization of user information == \ref PWR_DATA_RESET_STATE_USER \n
 *           Reset state for factory initialization == \ref PWR_DATA_RESET_STATE_FACTORY \n
 *          \ref AGL_ILLRESET_FLAG : val = 0 or 1 (Abnormal reset is 1)
 *        - When the API is success, setting value is saved as non-volatile data.
 *        - Non-volatile memory is allocated \ref AGL_RESERVED_LIMIT for extendability.
 *        - Values setting by the API are saved when it is cold start(is_batterydiscon is true of PsmSysMcEVSysResUpChkPrmT notified from power_hal).
 *        - The API is completed within 1msec after the API is called.
 *        - The API can only be used by 1 process.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        PowerHalGetResetInfo
 */
int PowerHalSetResetInfo(enum PowerHalResetInfoId id, uint32_t val);

/**
 * \ingroup PowerHalGetResetInfo
 * \~english @par Brief
 *         API to do get setting value of reset information .
 * \~english @param [in] id Reset information type.
 * \~english @param [out] val Buffer of value.
 * \~english @retval 0   :  Ok
 * \~english @retval -1 :  Failed
 * \~english @par Prerequisite
 *        - None
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Internal IO error. [-1]
 *        - Read fail of value. [-1]
 * \~english @par Detail
 *        - Do get value from each reset information type(\ref PowerHalResetInfoId).
 *        - Value of \ref AGL_PROGUPDATE_STATE, low 3bits are assigned as below, \n
 *          and corresponding bit is set if state is updating. \n
 *           Low 1bit(\ref PWR_PROGUPDATE_STATE_UPDATED) : Program update \n
 *           Low 2bit(\ref PWR_PROGUPDATE_STATE_MAP_UPDATED) : Map update \n
 *           Low 3bit(\ref PWR_PROGUPDATE_STATE_MAPDIFF_UPDATED) : Map difference update \n
 *        - The API is completed within 1msec after the API is called.
 *        - Completeness of data getting by the API is assured by lower layer than power_hal, \n
 *          and return -1 as internal IO error when it is occurred abnormal error.
 *        - The API can only be used by 1 process.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        PowerHalSetResetInfo
 */
int PowerHalGetResetInfo(enum PowerHalResetInfoId id, uint32_t *val);

/** @}*/  // end of power_hal
/** @}*/  // end of power_supply_manager_shadow

#endif  // HAL_API_POWER_HAL_H_
