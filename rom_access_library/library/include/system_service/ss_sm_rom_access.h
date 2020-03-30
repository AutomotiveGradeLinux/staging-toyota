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

/**
 * @file ss_sm_rom_access.h
 * @brief This file provides API for get rom information from extension memory
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

#ifndef ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_SS_SM_ROM_ACCESS_H_
#define ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_SS_SM_ROM_ACCESS_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>

#include "system_service/ss_sm_boot_access.h"

/**
 * \~english boot mode information
 */
typedef enum {
  APPLICATION_MODE = 0,
  PROGRAMMING_MODE
}EBOOT_MODE;

/**
 * \~english active flash load information
 */
typedef enum {
  NEW_FLASHLOADER = 0,
  OLD_FLASHLOADER
}EACTIVE_FLASHLOADER;

/**
 * \~english user mode information
 */
typedef enum {
  USER_OFF = 0,
  USER_ON
}EUSER_MODE;

/**
 * \~english control mode information
 */
typedef enum {
  DISABLE_MODE = 0,
  ENABLE_MODE
}ECONTROL_MODE;

/**
 * \~english data reset mode information
 */
typedef enum {
  DATARESET_NONE = 0,
  DATARESET_USER,
  DATARESET_FACTORY
}EDATARESET_MODE;

/**
 * \~english last illegal reset mode information
 */
typedef enum {
  LAST_ILGRESET_NORMAL = 0,
  LAST_ILGRESET_NG
}ELASTILGRESET_MODE;

/**
 * \~english Program update state information
 */
typedef uint32_t EPROGUPDATE_STATE;

/**
 * \~english next wakeup type
 */
typedef enum {
  NEXT_WAKEUP_TYPE_NONE = 0,
  NEXT_WAKEUP_TYPE_COLD,
  NEXT_WAKEUP_TYPE_HOT,
}ENEXT_WAKEUP_TYPE;

/**
 * \~english DRAM backup state
 */
typedef enum {
  DRAM_BACKUP_STATE_OK = 0,
  DRAM_BACKUP_STATE_NG
}DRAM_BACKUP_STATE;

/**
 * \~english max length of rom product private
 */
#define ROM_PRODUCT_PRIVATE_MAX 128

/**
 * @class ROM_AccessIf
 * \~english @brief ROM_AccessIf
 * \~english @par   Brief Introduction
 *        Class to provide the function of ROM AccessIf
 *
 */
class ROM_AccessIf {
 private:
  BOOT_AccessIf* m_pBoot;
  static void*   g_m_plock;
  static void*   g_m_bakup_map;
  static bool    g_m_is_dram_valid;
  LBM_NOR_t      m_nor;
  bool           m_norIsDirty;

  EFrameworkunifiedStatus LBA_Read(UI_32 offset, UI_32* buffer, UI_32 word_count);

  EFrameworkunifiedStatus LBA_Write(UI_32 offset, UI_32 value);

  EFrameworkunifiedStatus LBA_Write(UI_32 offset, UI_32* value, UI_32 size);

  EFrameworkunifiedStatus GetDataResetMode_sub(EDATARESET_MODE* p_data_reset_mode);

 public:
/**
 * \ingroup ROM_AccessIf
 * \~english @par Summary:
 *       Obtain the access permission to SYS area (secondary storage area).
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
 * \~ english @par Type
 *       None
 * \~english @par Detail:
 *       Obtain the access permission to SYS area. \n
 *       When the access permission is locked, the API watis for Mutex until the access permission is released. \n
 *       This API is assumed to be called by the SystemManager.
 * \~english @see  ~ROM_AccessIf, CL_LockMap, CL_LockGet
 */
  ROM_AccessIf();

/**
 * \ingroup ~ROM_AccessIf
 * \~english @par Summary:
 *       Release the access permission to SYS area (secondary storage area).
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
 * \~ english @par Type
 *       None
 * \~english @par Detail:
 *       Release the access permission to SYS area.
 * \~english @see  ROM_AccessIf, CL_LockRelease
 */
  ~ROM_AccessIf();

/**
 * \ingroup Initialize
 * \~english @par Summary:
 *       Synchronize SYS area (secondary storage area) and the mirror data in DRAM
 * \~english @param None
 * \~english @retval eFrameworkunifiedStatusOK   Synchronization success
 * \~english @retval eFrameworkunifiedStatusFail   Synchronization failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If system call mmap failed. [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Prepare the access to SYS area. \n
 * \~english @see EL_mem_exram_mmap
 */
  EFrameworkunifiedStatus Initialize();

/**
 * \ingroup SetProductPrivate
 * \~english @par Summary:
 *       Write data information depending on the product specification to the secondary storage area.
 * \~english @param [in] buf[ROM_PRODUCT_PRIVATE_MAX]
 *       UI_8[]  -  Data information depending on the product specification
 * \~english @par
 *   Definition of ROM_PRODUCT_PRIVATE_MAX
 * \~english @code
 *   #define ROM_PRODUCT_PRIVATE_MAX 128
 *   @endcode
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If write data to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Write data information depending on the product specification to the secondary storage area. \n
 * \~english @see  GetProductPrivate
 */
  EFrameworkunifiedStatus SetProductPrivate(UI_8 buf[ROM_PRODUCT_PRIVATE_MAX]);

/**
 * \ingroup GetProductPrivate
 * \~english @par Summary:
 *       Read the data information depending on the product specification from the secondary storage area.
 * \~english @param [out] buf[ROM_PRODUCT_PRIVATE_MAX]
 *       UI_8[]  -  Storing destination of data information depending on the product specification
 * \~english @par
 *   Definition of ROM_PRODUCT_PRIVATE_MAX
 * \~english @code
 *   #define ROM_PRODUCT_PRIVATE_MAX 128
 *   @endcode
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Read data information depending on the product specification from the secondary storage area. \n
 * \~english @see   SetProductPrivate
 */
  EFrameworkunifiedStatus GetProductPrivate(UI_8 buf[ROM_PRODUCT_PRIVATE_MAX]);

/**
 * \ingroup SystemInitialize
 * \~english @par Summary:
 *       Initialize the handles used for ROM access.
 * \~english @param [in] bkup_state
 * \~english @par
 *       DRAM_BACKUP_STATE
 *       - DRAM_BACKUP_STATE_OK
 *       - DRAM_BACKUP_STATE_NG
 * \~english @retval eFrameworkunifiedStatusOK   Initialize the handles used for ROM access success
 * \~english @retval eFrameworkunifiedStatusFail   Initialize the handles used for ROM access failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If system call mmap failed. [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Initialize the handles used for ROM access. \n
 * \~english @see EL_mem_exram_mmap
 */
  EFrameworkunifiedStatus SystemInitialize(DRAM_BACKUP_STATE bkup_state);

/**
 * \ingroup GetBootMode
 * \~english @par Summary:
 *       Get boot mode information.
 * \~english @param [out] p_boot_mode
 *       EBOOT_MODE*  -  Destination buffer to store information of boot mode.
 * \~english @par
 *       EBOOT_MODE
 *       - APPLICATION_MODE
 *       - PROGRAMMING_MODE
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get boot mode information from the secondary storage area. \n
 * \~english @see   SetBootMode
 */
  EFrameworkunifiedStatus GetBootMode(EBOOT_MODE* p_boot_mode);

/**
 * \ingroup SetBootMode
 * \~english @par Summary:
 *       Set boot mode information.
 * \~english @param [in] boot_mode
 *       EBOOT_MODE  -  Boot mode information which write to the secondary storage area.
 * \~english @par
 *       EBOOT_MODE
 *       - APPLICATION_MODE
 *       - PROGRAMMING_MODE
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If write data to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Set boot mode information to the secondary storage area. \n
 * \~english @see   GetBootMode
 */
  EFrameworkunifiedStatus SetBootMode(EBOOT_MODE boot_mode);

/**
 * \ingroup GetSignature
 * \~english @par Summary:
 *       Get signature information.
 * \~english @param [out] p_sig_value
 *       UI_32  -  Destination buffer to store signature information
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If write signature information to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get signature information from the secondary storage area. \n
 * \~english @see  SetSignature
 */
  EFrameworkunifiedStatus GetSignature(UI_32* p_sig_value);

/**
 * \ingroup SetSignature
 * \~english @par Summary:
 *       Set signature information.
 * \~english @param [in] sig_value
 *       UI_32  -  Signature information which write to the secondary storage area.
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If write signature information to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Set signature information to the secondary storage area. \n
 * \~english @see  GetSignature
 */
  EFrameworkunifiedStatus SetSignature(UI_32 sig_value);

/**
 * \ingroup SetActiveFlashloader
 * \~english @par Summary:
 *       Set active flash loader information.
 * \~english @param [in] active_flash_loader
 *       EACTIVE_FLASHLOADER  -  Active flash loader information which write to secondary storage area.
 * \~english @par
 *       EBOOT_MODE
 *       - APPLICATION_MODE
 *       - PROGRAMMING_MODE
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If write data to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Set active flash loader information to the secondary storage area. \n
 * \~english @see   GetBootMode
 */
  EFrameworkunifiedStatus SetActiveFlashloader(EACTIVE_FLASHLOADER active_flash_loader);

/**
 * \ingroup GetLastUserMode
 * \~english @par Summary:
 *       Get last user mode information.
 * \~english @param [out] p_user_mode
 *       EUSER_MODE*  -  Destination buffer to store information of user mode.
 * \~english @par
 *       EUSER_MODE
 *       - USER_ON
 *       - USER_OFF
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get last user mode information from the secondary storage area. \n
 * \~english @see   SetLastUserMode
 */
  EFrameworkunifiedStatus GetLastUserMode(EUSER_MODE* p_user_mode);

/**
 * \ingroup SetLastUserMode
 * \~english @par Summary:
 *       Set last user mode information.
 * \~english @param [in] user_mode
 *       EUSER_MODE  -  User mode information which write to the secondary storage area.
 * \~english @par
 *       EUSER_MODE
 *       - USER_ON
 *       - USER_OFF
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If write data to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Set last user mode information to the secondary storage area. \n
 * \~english @see   GetLastUserMode
 */
  EFrameworkunifiedStatus SetLastUserMode(EUSER_MODE user_mode);

/**
 * \ingroup GetTransportMode
 * \~english @par Summary:
 *       Get transport mode information.
 * \~english @param [out] p_control_mode
 *       ECONTROL_MODE*  -  Destination buffer to store information of transport mode.
 * \~english @par
 *       ECONTROL_MODE
 *       - DISABLE_MODE
 *       - ENABLE_MODE
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get transport mode information from the secondary storage area. \n
 * \~english @see   SetTransportMode
 */
  EFrameworkunifiedStatus GetTransportMode(ECONTROL_MODE* p_control_mode);

/**
 * \ingroup SetTransportMode
 * \~english @par Summary:
 *       Set transport mode information.
 * \~english @param [in] control_mode
 *       ECONTROL_MODE  -  Transport mode information which write to the secondary storage area.
 * \~english @par
 *       ECONTROL_MODE
 *       - DISABLE_MODE
 *       - ENABLE_MODE
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If write data to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Set transport mode information to the secondary storage area. \n
 * \~english @see   GetTransportMode
 */
  EFrameworkunifiedStatus SetTransportMode(ECONTROL_MODE control_mode);

/**
 * \ingroup GetProductionMode
 * \~english @par Summary:
 *       Get production mode information.
 * \~english @param [out] p_control_mode
 *       ECONTROL_MODE*  -  Destination buffer to store information of production mode.
 * \~english @par
 *       ECONTROL_MODE
 *       - DISABLE_MODE
 *       - ENABLE_MODE
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get production mode information from the secondary storage area. \n
 * \~english @see   SetProductionMode
 */
  EFrameworkunifiedStatus GetProductionMode(ECONTROL_MODE* p_control_mode);

/**
 * \ingroup SetProductionMode
 * \~english @par Summary:
 *       Set transport mode information.
 * \~english @param [in] control_mode
 *       ECONTROL_MODE  -  Production mode information which write to the secondary storage area.
 * \~english @par
 *       ECONTROL_MODE
 *       - DISABLE_MODE
 *       - ENABLE_MODE
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If write data to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Set production mode information to the secondary storage area. \n
 * \~english @see   GetProductionMode
 */
  EFrameworkunifiedStatus SetProductionMode(ECONTROL_MODE control_mode);

/**
 * \ingroup GetLimpHomeCutOffReqMode
 * \~english @par Summary:
 *       Get limp home cut off request mode information.
 * \~english @param [out] p_control_mode
 *       ECONTROL_MODE*  -  Destination buffer to store information of limp home cut off request mode.
 * \~english @par
 *       ECONTROL_MODE
 *       - DISABLE_MODE
 *       - ENABLE_MODE
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get limp home cut off request mode information from the secondary storage area. \n
 * \~english @see   SetLimpHomeCutOffReqMode
 */
  EFrameworkunifiedStatus GetLimpHomeCutOffReqMode(ECONTROL_MODE* p_control_mode);

/**
 * \ingroup SetLimpHomeCutOffReqMode
 * \~english @par Summary:
 *       Set limp home cut off request mode information.
 * \~english @param [in] control_mode
 *       ECONTROL_MODE  -  Limp home cut off request mode information which write to the secondary storage area.
 * \~english @par
 *       ECONTROL_MODE
 *       - DISABLE_MODE
 *       - ENABLE_MODE
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If write data to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Set limp home cut off request mode information to the secondary storage area. \n
 * \~english @see   GetLimpHomeCutOffReqMode
 */
  EFrameworkunifiedStatus SetLimpHomeCutOffReqMode(ECONTROL_MODE control_mode);

/**
 * \ingroup GetDataResetMode
 * \~english @par Summary:
 *       Get reserved data reset mode information.
 * \~english @param [out] p_data_reset_mode
 *       EDATARESET_MODE*  -  Destination buffer to store information of reserved data reset mode.
 * \~english @par
 *       EDATARESET_MODE
 *       - DATARESET_NONE
 *       - DATARESET_USER
 *       - DATARESET_FACTORY
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get reserved data reset mode information from the secondary storage area. \n
 * \~english @see   SetDataResetMode
 */
  EFrameworkunifiedStatus GetDataResetMode(EDATARESET_MODE* p_data_reset_mode);

/**
 * \ingroup GetDataResetModeFast
 * \~english @par Summary:
 *       Get reserved data reset mode information faster.This api can get correct value before
 *       calling SetDataResetMode.So it shoud be used only immediately after boot.
 * \~english @param [out] p_data_reset_mode
 *       EDATARESET_MODE*  -  Destination buffer to store information of reserved data reset mode faster.
 * \~english @par
 *       EDATARESET_MODE
 *       - DATARESET_NONE
 *       - DATARESET_USER
 *       - DATARESET_FACTORY
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 *       - If system call mmap failed. [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get reserved data reset mode information faster from the secondary storage area.This api can get
 *       correct value before calling SetDataResetMode.So it shoud be used only immediately after boot.
 * \~english @see   SetDataResetMode
 */
  EFrameworkunifiedStatus GetDataResetModeFast(EDATARESET_MODE* p_data_reset_mode);

/**
 * \ingroup SetDataResetMode
 * \~english @par Summary:
 *       Set reserve data reset mode information.
 * \~english @param [in] data_reset_mode
 *       EDATARESET_MODE  -  Reserve data reset mode information which write to the secondary storage area.
 * \~english @par
 *       EDATARESET_MODE
 *       - DATARESET_NONE
 *       - DATARESET_USER
 *       - DATARESET_FACTORY
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If write data to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Set reserve data reset mode information to the secondary storage area. \n
 * \~english @see   GetDataResetMode
 */
  EFrameworkunifiedStatus SetDataResetMode(EDATARESET_MODE data_reset_mode);

/**
 * \ingroup GetResetCount
 * \~english @par Summary:
 *       Get reset count information.
 * \~english @param [out] p_reset_count
 *       UI_32*  -  Destination buffer to store information of reset count.
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get reset count from the secondary storage area. \n
 * \~english @see   SetResetCount
 */
  EFrameworkunifiedStatus GetResetCount(UI_32* p_reset_count);

/**
 * \ingroup SetResetCount
 * \~english @par Summary:
 *       Set reset count information.
 * \~english @param [in] reset_count
 *       UI_32  -  Reset count information which write to the secondary storage area.
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If write data to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Set reset count information to the secondary storage area. \n
 * \~english @see   GetResetCount
 */
  EFrameworkunifiedStatus SetResetCount(UI_32 reset_count);

/**
 * \ingroup GetLastIlgReset
 * \~english @par Summary:
 *       Get last illegal reset information.
 * \~english @param [out] p_last_ilg_reset
 *       ELASTILGRESET_MODE*  -  Destination buffer to store information of last illegal reset.
 * \~english @par
 *       ELASTILGRESET_MODE
 *       - LAST_ILGRESET_NORMAL
 *       - LAST_ILGRESET_NG
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get last illegal reset information from the secondary storage area. \n
 * \~english @see   SetLastIlgReset
 */
  EFrameworkunifiedStatus GetLastIlgReset(ELASTILGRESET_MODE* p_last_ilg_reset);

/**
 * \ingroup SetLastIlgReset
 * \~english @par Summary:
 *       Set last illegal reset information.
 * \~english @param [in] last_ilg_reset
 *       ELASTILGRESET_MODE  -  Last illegal reset information which write to the secondary storage area.
 * \~english @par
 *       ELASTILGRESET_MODE
 *       - LAST_ILGRESET_NORMAL
 *       - LAST_ILGRESET_NG
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If write data to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Set last illegal reset information to the secondary storage area. \n
 * \~english @see   GetLastIlgReset
 */
  EFrameworkunifiedStatus SetLastIlgReset(ELASTILGRESET_MODE last_ilg_reset);

/**
 * \ingroup GetProgUpdateState
 * \~english @par Summary:
 *       Get program update status information.
 * \~english @param [out] p_prog_update_state
 *       EPROGUPDATE_STATE*  -  Destination buffer to store information of program update status.
 * \~english @par
 *       typedef uint32_t EPROGUPDATE_STATE
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get program update status information from the secondary storage area. \n
 * \~english @see   SetProgUpdateState
 */
  EFrameworkunifiedStatus GetProgUpdateState(EPROGUPDATE_STATE* p_prog_update_state);

/**
 * \ingroup SetProgUpdateState
 * \~english @par Summary:
 *       Set program update status information.
 * \~english @param [in] prog_update_state
 *       EPROGUPDATE_STATE  -  Program update status information which write to the secondary storage area.
 * \~english @par
 *      typedef uint32_t EPROGUPDATE_STATE
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If write data to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Set program update status information to the secondary storage area. \n
 * \~english @see   GetProgUpdateState
 */
  EFrameworkunifiedStatus SetProgUpdateState(EPROGUPDATE_STATE prog_update_state);

/**
 * \ingroup GetErrLogCount
 * \~english @par Summary:
 *       Get error logging count information.
 * \~english @param [out] p_err_log_count
 *       UI_32*  -  Destination buffer to store information of error logging count.
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get error logging count information from the secondary storage area. \n
 * \~english @see   SetErrLogCount
 */
  EFrameworkunifiedStatus GetErrLogCount(UI_32* p_err_log_count);

/**
 * \ingroup SetErrLogCount
 * \~english @par Summary:
 *       Get error logging count information.
 * \~english @param [out] err_log_count
 *       UI_32*  -  Destination buffer to store information of error logging count.
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get error logging count information from the secondary storage area. \n
 * \~english @see   GetErrLogCount
 */
  EFrameworkunifiedStatus SetErrLogCount(UI_32 err_log_count);

/**
 * \ingroup GetNextWakeupType
 * \~english @par Summary:
 *       Get next wakeup type information.
 * \~english @param [out] p_next_wakeup_type
 *       ENEXT_WAKEUP_TYPE*  -  Destination buffer to store information of next wakeup type.
 * \~english @par
 *       ENEXT_WAKEUP_TYPE
 *       - NEXT_WAKEUP_TYPE_NONE
 *       - NEXT_WAKEUP_TYPE_COLD
 *       - NEXT_WAKEUP_TYPE_HOT
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get next wakeup type information from the secondary storage area. \n
 * \~english @see   SetNextWakeupType
 */
  EFrameworkunifiedStatus GetNextWakeupType(ENEXT_WAKEUP_TYPE *p_next_wakeup_type);

/**
 * \ingroup SetNextWakeupType
 * \~english @par Summary:
 *       Get next wakeup type information.
 * \~english @param [out] next_wakeup_type
 *       ENEXT_WAKEUP_TYPE*  -  Destination buffer to store information of next wakeup type.
 * \~english @par
 *       ENEXT_WAKEUP_TYPE
 *       - NEXT_WAKEUP_TYPE_NONE
 *       - NEXT_WAKEUP_TYPE_COLD
 *       - NEXT_WAKEUP_TYPE_HOT
 * \~english @retval eFrameworkunifiedStatusOK   Success
 * \~english @retval eFrameworkunifiedStatusFail   Failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~ english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Get next wakeup type information from the secondary storage area. \n
 * \~english @see   GetNextWakeupType
 */
  EFrameworkunifiedStatus SetNextWakeupType(ENEXT_WAKEUP_TYPE next_wakeup_type);
};

#endif  // ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_SS_SM_ROM_ACCESS_H_

/** @}*/  // end of rom_access_library
/** @}*/  // end of system_service
/** @}*/  // end of BaseSystem
