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
 * @file ss_sm_boot_access.h
 * @brief This file provides API for get boot information from nor flash
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

#ifndef ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_SS_SM_BOOT_ACCESS_H_
#define ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_SS_SM_BOOT_ACCESS_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>

// Top memory map of LINUX management outside domain (SYS area)
// Must not use it other than the model of C language specifications to refer to BOOT

#define LBM_UINT32 unsigned int
#define LBM_UINT16 uint16_t
#define LBM_UINT8  unsigned char
#define LBM_INT64  int64_t
#define LBM_INT32  unsigned int
#define LBM_INT16  uint16_t
#define LBM_INT8   unsigned char

// Each domain size
#define LBM_NOR_MIRROR_SIZE 0x10000   // MIRROR area of NOR
#define LBM_DRAM_SIZE       0x10000   // RAM area to share with BOOT in LINUX

#define LBM_NOR_BOOT_SIZE   128
#define LBM_NOR_TSKM_SIZE   128
#define LBM_NOR_VUP_SIZE    1024
#define LBM_NOR_ANA_SIZE    32
#define LBM_NOR_VEHI_SIZE   32
#define LBM_NOR_DEVMGR_SIZE 32


//********* Value of the field ****************
/* SDRAMstart identification */
#define SYSUP_DRAM_NORMAL (LBM_UINT32)0x4E524E52  /* normal value                             NRNR*/
#define SYSUP_DRAM_SFTERRSR (LBM_UINT32)0x73667366  /* abnormal Soft Reset(softwareabnormality) sfsf*/
#define SYSUP_DRAM_DISCARD (LBM_UINT32)0x44434443  /* need to discard backup                   DCDC*/

/* Start is in a state (navigator initial use)/System start state */
#define SYSUP_CND_FACTRESET (LBM_UINT16)0x434c  /* COLD START */
#define SYSUP_CND_NORMAL (LBM_UINT16)0x4853  /* HOT START  */

/* Start mode preservation table level */
#define SYSUP_MODE_NORMAL (LBM_UINT32)0x4E524E52  /* normal mode             */
#define SYSUP_MODE_VERUP (LBM_UINT32)0x56555655  /* versionup mode          */

/* Communication state between NAVI-SYS */
#define SUBCPU_STS_NOMAL (LBM_UINT32)0x00000000  /* Communication normal     */
#define SUBCPU_STS_COMNG (LBM_UINT32)0x88888888  /* Communication abnormal   */

/* SDRAM power supply abnormality detection */
#define SDRAM_STS_NOMAL (LBM_UINT32)0x11111111  /* backup power supply is normal   */
#define SDRAM_STS_BUPNG (LBM_UINT32)0x99999999  /* backup power supply is abnormal */

/* HWDT Outbreak */
#define HWDT_STS_NOMAL (LBM_UINT32)0x22222222  /* WDT normal                   */
#define HWDT_STS_WDTTO (LBM_UINT32)0xAAAAAAAA  /* WDT time-out outbreak        */

/* SWDT Outbreak */
#define SWDT_STS_NOMAL HWDT_STS_NOMAL         /* SWDT normal                  */
#define SWDT_STS_WDTTO HWDT_STS_WDTTO         /* SWDT time-out outbreak       */

/* Start side */
#define IFS_PROG1_CODE (LBM_UINT8)0xAA  /* Side PROG1              */
#define IFS_PROG2_CODE (LBM_UINT8)0x55  /* Side PROG2              */

/* Start infomation ID */
#define UPTBLE_ID_MAX (LBM_UINT8)0x20  /* LBM_UPTBLINFO Maximum value */
#define UPTBLE_ID_NORBOOT (LBM_UINT8)0x00  /* NOR-BOOT ID                 */
#define UPTBLE_ID_SCRLDR (LBM_UINT8)0x01  /* SecureLoader ID             */
#define UPTBLE_ID_NANDBOOT (LBM_UINT8)0x02  /* NAND-BOOT ID                */
#define UPTBLE_ID_ROOTFS (LBM_UINT8)0x03  /* Rootfs1 ID                  */
#define UPTBLE_ID_ROOTFS2 (LBM_UINT8)0x04  /* Rootfs2 ID                  */

/* Start status */
#define STUP_STS_NORMAL         (LBM_UINT32)0x00000000  /* normal status   */
/* Rollback of protectionism is necessary by abnormal reset repetition */
#define STUP_STS_ERROR_DETEC    (LBM_UINT32)0x22222222
#define STUP_STS_UPDATING       (LBM_UINT32)0x33333333  /* updating status */
/* not for use(will be removed) */
#define STUP_STS_SIGNVRFY_ERROR (LBM_UINT32)0x11111111

/* mdUpdate/sdUpdate */
#define UPDATE_NOTI_NONE       (LBM_UINT32)0x00000000 /* Notify is NOT required. */
#define UPDATE_NOTI_EXIST      (LBM_UINT32)0xEEEEEEEE /* Notify is required.     */

/* RAM Judge Flag */
#define RAMJUDGE_STS_NOMAL     (LBM_UINT32)0x44444444 /* RAM Judge Port is High */
#define RAMJUDGE_STS_NG        (LBM_UINT32)0xDDDDDDDD /* RAM Judge Port is Low  */

/* DRAM Self Refresh */
#define SELF_REFRESH_OK        (LBM_UINT32)0x55555555 /* DRAM Self Refresh is OK */
#define SELF_REFRESH_NG        (LBM_UINT32)0xCCCCCCCC /* DRAM Self Refresh is NG */

/* Release ROM type */
#define PRODUCTROM                  (LBM_UINT32)0x11111111
#define RELEASEROM                  (LBM_UINT32)0x22222222
#define RELEASEROM_REMAIN_USBBOOT   (LBM_UINT32)0x44444444
#define DEBUGROM                    (LBM_UINT32)0x88888888

/* Boot write Nor */
#define WRITE_NONE              (LBM_UINT32)0x00000000
#define WRITE_DONE              (LBM_UINT32)0x11111111


/* opdtLoadSatus */
#define DEFAULT_OPIMAGE_DRAM_BACKUPED   (LBM_UINT32)0x00000000
#define DEFAULT_OPIMAGE_FROM_EMMC       (LBM_UINT32)0xAAAAAAAA
#define SPECIFIC_OPIMAGE_DRAM_BACKUPED  (LBM_UINT32)0x11111111
#define SPECIFIC_OPIMAGE_FROM_EMMC      (LBM_UINT32)0xBBBBBBBB

/* opdtLoadFactor */
#define SPECIFIC_OPIMAGE_FLAG_NONE	(LBM_UINT32)0x00000000
#define SPECIFIC_OPIMAGE_FILE_NONE	(LBM_UINT32)0x11111111
#define SPECIFIC_OPIMAGE_FILESIZE_0	(LBM_UINT32)0x22222222
#define SPECIFIC_OPIMAGE_FILE_EXIST	(LBM_UINT32)0xEEEEEEEE


#pragma pack(push, 4)

// Memory map of the SYS area of the mem0

#define SS_SYS_AREA_BOOT_MAX_SIZE  (0x00010000UL)

#define SS_SYS_AREA_ROM_OFFSET    (0x00010000UL)
#define SS_SYS_AREA_ROM_MAX_SIZE  (0x00001000UL)

#define SS_SYS_AREA_RAM_OFFSET    (0x00011000UL)
#define SS_SYS_AREA_RAM_MAX_SIZE  (0x00001000UL)

//********* NOR area ****************
typedef struct LBM_boot_wpinfo_t {
  LBM_UINT32  currentside;  // IFS_PROG*_CODE
  LBM_UINT32  stupStsA;     // STUP_STS_*
  LBM_UINT32  stupStsB;     // STUP_STS_*
}LBM_UPTBLINFO;

typedef struct LBM_boot_t {
  LBM_UINT32  sts1;         // SYSUP_CND_*
  LBM_UINT32  btmode;       // SYSUP_MODE_*
  LBM_UINT32  sts2;         // SYSUP_CND_*
  LBM_UPTBLINFO upTbl[UPTBLE_ID_MAX];
  LBM_UINT32  sts3;         // SYSUP_CND_*
}LBM_NOR_t;

//********* DRAM area ****************
typedef struct _tmb_ram_t {
  LBM_UINT32  dram1;      // SYSUP_DRAM_*
  LBM_UINT32  sts;        // SYSUP_CND_*
  LBM_UINT32  upmode;     // SYSUP_MODE_*
  LBM_UINT32  dram2;      // SYSUP_DRAM_*
  LBM_UINT32  syscomSts;  // SUBCPU_STS_*
  LBM_UINT32  pwSts;      // SDRAM_STS_*
  LBM_UINT32  hwdtSts;    // HWDT_STS_*
  LBM_UINT32  dram3;      // SYSUP_DRAM_*
  LBM_UPTBLINFO upTbl[UPTBLE_ID_MAX];
  LBM_UINT32  sectorNo;   // NOR Shared SectorNo
  LBM_UINT32  mdUpdate;   // UPDATE_NOTI_* LanServer Update Notification
  LBM_UINT32  sdUpdate;   // UPDATE_NOTI_* SettingService Update Notification
  LBM_UINT32  ramjudgeSts;  // RAMJUDGE_STS_*
  LBM_UINT32  swdtSts;    // SWDT_STS_*
  LBM_UINT32  dram_self_refresh;  // DRAM Self Refresh
  LBM_UINT32  releaseNorType;
  LBM_UINT32  socCpuType;    // H2/E2/M2
  LBM_UINT32  socEsNumber;    // ES1x/ES2x/ES3x
  LBM_UINT32  spiBootDevice;    // NorDeviceID
  LBM_UINT32  bootNorWrite;    // uboot-api
  LBM_UINT32  naviDetFactor;

  LBM_UINT32  emmcTap;		//emmcTap
  LBM_UINT32  opdtUpdateReq;
  LBM_UINT32  opdtLoadSatus;
  LBM_UINT32  opdtLoadFactor;

}LBM_RAM_t;

#pragma pack(pop)


/**
* \~english Access type to the boot information
*/
typedef enum {
  BAI_OPEN_RO, /* \~english Access type for the Read only*/
  BAI_OPEN_RW  /* \~english Access type for the Read and Write*/
}BAI_OPEN_t;

/**
 * @class BOOT_AccessIf
 * \~english @brief BOOT_AccessIf
 * \~english @par   Brief Introduction
 *        Class to provide the function of BOOT AccessIf
 *
 */
class BOOT_AccessIf{
 private:
  static void* g_m_plock;
  BAI_OPEN_t   m_type;

 public:
 /**
 * \ingroup BOOT_AccessIf
 * \~english @par Summary:
 *        Obtain the access permission to the boot information (secondary storage area).
 * \~english @param [in] type
 *        BAI_OPEN_t   -   Access type to the boot information\n
 * \~english @par
 *           BAI_OPEN_t
 *           - BAI_MTD_DEV_RO      Access type for the Read only \n
 *           - BAI_MTD_DEV_RW      Access type for the Read and Write
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
 *       Obtain the access permission to the boot information \n
 *       When the access permission is locked, the API waits for Mutex until the access permission is released. \n
 *       This API is assumed to be called by SystemManager.
 * \~english @see  ~BOOT_AccessIf, CL_LockMap, CL_LockGet
 */
explicit BOOT_AccessIf(BAI_OPEN_t type);

 /**
 * \ingroup ~BOOT_AccessIf
 * \~english @par Summary:
 *       Release the access permission to the boot information (secondary storage area).
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
 *       Release the access permission to the boot information.\n
 * \~english @see  BOOT_AccessIf, CL_LockRelease
 */
  ~BOOT_AccessIf();

/**
 * \ingroup getBootInfo
 * \~english @par Summary:
 *       Read the boot information (secondary storage area).
 * \~english @param  [out] p_boot_info
 *       LBM_NOR_t *   -   Destination buffer to read the boot information
 * \~english @par
 *           LBM_NOR_t Structure body
 * \~english @code
 *           typedef struct LBM_boot_t{
 *             LBM_UINT32  sts1;             //SYSUP_CND_*
 *             LBM_UINT32  btmode;           //SYSUP_MODE_*
 *             LBM_UINT32  sts2;             //SYSUP_CND_*
 *             LBM_UPTBLINFO upTbl[UPTBLE_ID_MAX];
 *             LBM_UINT32  sts3;             //SYSUP_CND_*
 *           }LBM_NOR_t;
 *           @endcode
 *       Refer to "ss_boot_map.h" for the parameters to be set to the member of struct LBM_NOR_t.
 * \~english @retval eFrameworkunifiedStatusOK   Reading success
 * \~english @retval eFrameworkunifiedStatusFail Reading failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If p_boot_info is NULL  [eFrameworkunifiedStatusFail]
 *       - If read data from secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Copy the boot information stored in the secondary storage area to p_boot_info.  \n
 * \~english @see  setBootInfo
 */
  EFrameworkunifiedStatus getBootInfo(LBM_NOR_t* p_boot_info);

 /**
 * \ingroup setBootInfo
 * \~english @par Summary:
 *       Write the boot information (secondary storage area).
 * \~english @param  [in] p_boot_info
 *       LBM_NOR_t *   -   Original buffer to write the boot information
 * \~english @par
 *           LBM_NOR_tStructure body
 * \~english @code
 *           typedef struct LBM_boot_t{
 *             LBM_UINT32  sts1;             //SYSUP_CND_*
 *             LBM_UINT32  btmode;           //SYSUP_MODE_*
 *             LBM_UINT32  sts2;             //SYSUP_CND_*
 *             LBM_UPTBLINFO upTbl[UPTBLE_ID_MAX];
 *             LBM_UINT32  sts3;             //SYSUP_CND_*
 *           }LBM_NOR_t;
 *           @endcode
 *       Refer to "ss_boot_map.h" for the parameters to be set to the member of struct LBM_NOR_t.
 * \~english @retval eFrameworkunifiedStatusOK   Write success
 * \~english @retval eFrameworkunifiedStatusFail Write failed
 * \~english @par Precondition:
 *       - None
 * \~english @par Change in the internal status:
 *       - The change in the internal status does not occur by this API.
 * \~english @par Conditions of processing failure
 *       - If p_boot_info is NULL  [eFrameworkunifiedStatusFail]
 *       - BOOT_AccessIf Opened secondary storage area with BAI_MTD_DEV_RO  [eFrameworkunifiedStatusFail]
 *       - If write data to secondary storage area failed [eFrameworkunifiedStatusFail]
 * \~english @par Classification:
 *       Public
 * \~english @par Type
 *       Sync only(None communication)
 * \~english @par Detail:
 *       Write the boot information stored in p_boot_info to the secondary storage area. \n
 * \~english @see  getBootInfo
 */
  EFrameworkunifiedStatus setBootInfo(LBM_NOR_t* p_boot_info);
};

#endif  // ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_SS_SM_BOOT_ACCESS_H_

/** @}*/  // end of rom_access_library
/** @}*/  // end of system_service
/** @}*/  // end of BaseSystem
