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

/**
 * @file ss_sm_ram_access.h
 * @brief This file provides API for get ram information from extension memory
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup rom_access_library
 *  @ingroup system_service
 *  @{
 */

#ifndef  ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_SS_SM_RAM_ACCESS_H_
#define  ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_SS_SM_RAM_ACCESS_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>

/**
 * \~english max length of ram product private
 */
#define RAM_PRODUCT_PRIVATE_MAX (96)
/**
 * \~english max length of order file name
 */
#define SS_SM_ORDER_NAME_MAX    (32)

/**
 * \~english System management information
 */
typedef struct {
  char              signature_in[4];
  BOOL              isImmediateReset;
  BOOL              needRenotifyStartPrm;
  uint32_t          lastWakeupType;
  uint32_t          lastDramBackupStatus;
  uint32_t          lastResetStatus;
  char              productPrivate[RAM_PRODUCT_PRIVATE_MAX];
  char              order_name[SS_SM_ORDER_NAME_MAX];
  BOOL              isErrorReset;
  char              signature_out[4];
}RAM_SM_INFO_t;

/**
 * \~english System wakeup state type
 */
typedef enum {
  RAM_WAKEUP_STATE_DONT_CARE = 0,  /* \~english Don't care wakeup state*/
  RAM_WAKEUP_STATE_BACKUP_NG,      /* \~english Wakeup state is backup NG*/
  RAM_WAKEUP_STATE_BATTERY_DOWN    /* \~english Wakeup state is battery down*/
}RAM_WAKEUP_STATE;

/**
 * @class RAM_AccessIf
 * \~english @brief RAM_AccessIf
 * \~english @par   Brief Introduction
 *        Class to provide the function of RAM AccessIf
 *
 */
class RAM_AccessIf{
 private:
  static void*         g_m_plock;
  static void*         g_m_bakup_map;
  static bool          g_m_is_first_access;
  RAM_SM_INFO_t        m_buf;
  bool                 m_dirty;
  void initRamBackupEnable(RAM_WAKEUP_STATE wup_state);
  void finalRamBackupEnable(void);

 public:
/**
 * \ingroup RAM_AccessIf
 * \~english @par Summary:
 *        Obtain the access permission to the system management information.
 * \~english @param [in] wup_state
 *        RAM_WAKEUP_STATE   -   System maneger wakeup state type
 * \~english @par
 *           RAM_WAKEUP_STATE
 *           - RAM_WAKEUP_STATE_DONT_CARE       Don't care wakeup state\n
 *           - RAM_WAKEUP_STATE_BACKUP_NG       Wakeup state is backup NG\n
 *           - RAM_WAKEUP_STATE_BATTERY_DOWN    Wakeup state is battery down
 * \~english @retval None
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - None
 * \~english @par Classification:
 *       Public
 * \~english @par Type
 *       None
 * \~english @par Detail:
 *       Obtain the access permission to the system management information \n
 *       When the access permission is locked, the API waits for Mutex until the access permission is released. \n
 *       This API is supposed to be called by SystemManager.
 * \~english @see  ~RAM_AccessIf, CL_LockMap, CL_LockGet
 */
explicit RAM_AccessIf(RAM_WAKEUP_STATE wup_state = RAM_WAKEUP_STATE_DONT_CARE);

/**
 * \ingroup ~RAM_AccessIf
 * \~english @par Summary:
 *       Release the access permission to the system management information.
 * \~english @param None
 * \~english @retval None
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - None
 * \~english @par Classification:
 *       Public
 * \~english @par Type
 *       None
 * \~english @par Detail:
 *       Release the access permission to the system management information.\n
 * \~english @see  RAM_AccessIf, CL_LockRelease
 */
  ~RAM_AccessIf();

/**
 * \ingroup getRamInfo
 * \~english @par Summary:
 *       Read the system management information.
 * \~english @param  [out] p_boot_info
 *       RAM_SM_INFO_t *   -   Destination buffer to read the system management information.
 * \~english @par
 *           RAM_SM_INFO_t Structure body
 * \~english @code
 *           typedef struct {
 *             char              signature_in[4];
 *             BOOL              isImmediateReset;
 *             BOOL              needRenotifyStartPrm;
 *             uint32_t          lastWakeupType;
 *             uint32_t          lastDramBackupStatus;
 *             uint32_t          lastResetStatus;
 *             char              productPrivate[RAM_PRODUCT_PRIVATE_MAX];
 *             char              order_name[SS_SM_ORDER_NAME_MAX];
 *             BOOL              isErrorReset;
 *             char              signature_out[4];
 *           }RAM_SM_INFO_t;
 *           @endcode
 * \~english @retval eFrameworkunifiedStatusOK   Reading success
 * \~english @retval eFrameworkunifiedStatusFail Reading failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If p_boot_info is NULL  [eFrameworkunifiedStatusFail]
 *       - If read data from secondary storage area or /nv/BS/ss/rom_access_library/rwdata/ramdata.dat
 *         failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Copy the system information stored in the secondary storage area or
 *       in /nv/BS/ss/rom_access_library/rwdata/ramdata.dat to p_boot_info.  \n
 * \~english @see  setRamInfo
 */
  EFrameworkunifiedStatus getRamInfo(RAM_SM_INFO_t* p_boot_info);

/**
 * \ingroup setRamInfo
 * \~english @par Summary:
 *       Write the system management information.
 * \~english @param  [in] p_boot_info
 *       RAM_SM_INFO_t *   -   Original buffer to write the system management information
 * \~english @par
 *           RAM_SM_INFO_t Structure body
 * \~english @code
 *           typedef struct {
 *             char              signature_in[4];
 *             BOOL              isImmediateReset;
 *             BOOL              needRenotifyStartPrm;
 *             uint32_t          lastWakeupType;
 *             uint32_t          lastDramBackupStatus;
 *             uint32_t          lastResetStatus;
 *             char              productPrivate[RAM_PRODUCT_PRIVATE_MAX];
 *             char              order_name[SS_SM_ORDER_NAME_MAX];
 *             BOOL              isErrorReset;
 *             char              signature_out[4];
 *           }RAM_SM_INFO_t;
 *           @endcode
 * \~english @retval eFrameworkunifiedStatusOK   Write success
 * \~english @retval eFrameworkunifiedStatusFail Write failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If p_boot_info is NULL  [eFrameworkunifiedStatusFail]
 *       - If write data to secondary storage area or /nv/BS/ss/rom_access_library/rwdata/ramdata.dat
 *         failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Write the system manegement information stored in p_boot_info to the
 *       secondary storage area or /nv/BS/ss/rom_access_library/rwdata/ramdata.dat. \n
 * \~english @see  getRamInfo
 */
  EFrameworkunifiedStatus setRamInfo(RAM_SM_INFO_t* p_boot_info);
};

#endif  //  ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_SS_SM_RAM_ACCESS_H_

/** @}*/  // end of rom_access_library
/** @}*/  // end of system_service
/** @}*/  // end of BaseSystem
