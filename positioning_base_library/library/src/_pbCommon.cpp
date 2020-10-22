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

/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 File name      : _pbCommon.cpp
 System name    : 05 Integration Platform
 Subsystem name : System wide functions
 Title          : System API _CWORD64_ configuration processes
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vehicle_service/positioning_base_library.h>

#include "_pbCommon.h"
#include "WPF_STD_private.h"
#include "sysup_addr.h"
#include "tchar.h"
#include "sysup.h"

/*
 Constants and data type definitions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define BLOCK_SIZE          (1*1024*1024)
#define WDTC_REG_ADDR       (0x13500000 | ADDR_P2)  /* WDT clear register address */
#define SUM_ZERO            0
#define ALLOC_SIZE          0x00200000
#define EXIST_CLEAR      0x434c4f4b        /* Flag to indicate cleared Non-OS management area */

#define PRCM_BASE          0x48180000
#define PRM_RSTCTRL          0xA0

#define RST_GLOBAL_COLD_SW      (0x01 << 1)
#define RST_GLOBAL_WARM_SW      (0x01 << 0)

/*
 Internal function prototype declarations
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
u_int32  RomCheck(u_int32 *start_addr, u_int32 check_size);
u_int32  RomCheckEx(u_int32 *start_addr, u_int32 check_size);
uint64_t AtoxLong(char* str, unsigned int ascii_size);
void XtoaLong(uint64_t x, char* str);
#ifdef PBCOM_CONFIGREAD
static RET_API SysGetConfigSize(FILE*, u_int32*);
static RET_API SysReadConfigInfo(FILE*, TCHAR*, TCHAR*, u_int32);
static int SysUniToInt(TCHAR*, u_int32);
#endif   /* PBCOM_CONFIGREAD */

/*
 External function prototype declarations
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 Global Variable Definitions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifdef PSL_USED
extern int        g_n_api_set_id;  /* ID variable for PSL registration */
#endif   // PSL_USED

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : _CWORD64_NotOSAreaInit
 * ABSTRACT  : Initialization non-OS managed areas of _CWORD64_ 
 * NOTE      : This function initializes the non-OS control areas of _CWORD64_.
 * ARGUMENT  : None
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
_CWORD64_NotOSAreaInit(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
#ifndef CEPC_EM    /* If not emulated in CEPC */
    RET_API     ret;
    char        *cp_addr = NULL;
    u_int32     size;
    u_int32     *p_clear_flag;       /* Pointer for storing non-OS management area clear flag */
    LPVOID      notos_topaddr;

    /* Link of shared memory area to store the non-OS management area clear flag */
    ret = _pb_LinkShareData(const_cast<char*>("NOTOS_DATA"), reinterpret_cast<void**>(&cp_addr), &size);
    if (ret != RET_NORMAL) {
        /* Create a shared memory area to store the non-OS management area clear flag */
        Memset64pSync(cp_addr, 0, sizeof(p_clear_flag));    /* NULL initialization process of acquisition area */
    }

    p_clear_flag = reinterpret_cast<u_int32*>(cp_addr);

    /* Non-OS management area clear processing */
    /* Because an exception occurs when garbage data remains in the message pool at startup,
        Clear the entire non-OS management area as 0 Only for the first time after startup. */
    if (*p_clear_flag != EXIST_CLEAR) {
        /* Execute mapping to non-OS management area */
        ret = PbAccessPhysicalMem(TOP_ADDR_NOTOS,      /* Start address non-OS management area for of the _CWORD64_ */
                                      &notos_topaddr,      /* Start address of the mapping        */
                                      NOTOS_SISE,        /* Area-size              */
                                      0);

        /* failed mapping */
        if (ret != RET_NORMAL) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                   "_CWORD64_msg.lib:%s:LINE %d\r\n PbAccessPhysicalMem ERROR In " \
                                   "_CWORD64_NotOSAreaInit ... ret[%d] \r\n",
                           LTEXT(__FILE__), __LINE__, ret);
            _pb_Exit();
        }

        /* Non-OS management area clear processing */
        Memset64pSync(notos_topaddr, 0, NOTOS_SISE);

        /* Shared area release processing */
        ret = PbFreePhysicalMem(TOP_ADDR_NOTOS,      /* Start address of the _CWORD64_ non-OS management area    */
                                    notos_topaddr,      /* Start address of the mapped area    */
                                    NOTOS_SISE);      /* Size                */

        if (ret != RET_NORMAL) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                   "_CWORD64_msg.lib:%s:LINE %d\r\n PbFreePhysicalMem ERROR " \
                                   "In_CWORD64_NotOSAreaInit ... ret[%d] \r\n",
                           LTEXT(__FILE__), __LINE__, ret);
            _pb_Exit();
        }

        *p_clear_flag = EXIST_CLEAR;         /* Set non-OS management area clear flag */
    }
#endif

    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : _CWORD64_APIInit
 * ABSTRACT  : System initialization of _CWORD64_API
 * NOTE      : This function is called once from the initialization process
 *           : at system startup to perform the _CWORD64_API initialization process.
 * ARGUMENT  : u_int32 (*sighand)()     Completion of signal handling within APIs if NULL
 *           :                          Otherwise calls this first when a signal is raised
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
_CWORD64_APIInit(u_int32 (*sighand)()) {    // NOLINT(readability/nolint) API  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API    RetSts = RET_ERROR;

    if (sighand == NULL) {
        /* Parameter is set to an error other than NULL */
        RetSts = _pb_Setup_CWORD64_API(NULL);    /* Execute initialization processing for normal processing */
    }

    return(RetSts);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : _pb_GetPrgVer
 * ABSTRACT  : Get the program version
 * NOTE      : Get the program version of the mask ROM (BootLoader) and flash 
 *           : ROM (NK-image) into specified buffers.
 *           : In hardware-independent CEPC emulation environment, return the following version,
 *           : otherwise always return RET_NORMAL as the return value.
 *           : mask_ver=0x0102 0304 0506 0708
 *           : flash_ver1=0x0102 0304 0506 0708
 *           : flash_ver2=0x0102 0304 0506 0708
 *           : flash_ver3=0x0102 0304 0506 0708
 * ARGUMENT  : T_SYS_PRGVER     *pbuf   Destination buffer address
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
_pb_GetPrgVer(T_SYS_PRGVER *pbuf) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : _pb_UpManage
 * ABSTRACT  : Boot management process
 * NOTE      : Set the startup identifier in the SDRAM startup identification table (SDRAM) 
 *           : or TinyFugue area (FLASH) at the next startup.
 *           : Which one to set depends on the table type of the argument.
 *           : Soft reset is performed when reset is instructed by the parameter.
 *           : In hardware-independent CEPC emulation environment, return the RET_NORMAL as the return value without recording the startup identifier.
 * ARGUMENT  : u_int16  up_sts      :Startup Identifier Value
 *           : int      reset       :Reset instruction
 *           : u_int8  table    :Table Type
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
_pb_UpManage(u_int32 up_sts, int reset, u_int8 table) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* Is reset instructed? */
    if ((reset == SYS_RESET) || (reset == SYS_ONLY_RESET)) {
        PbReset();        /* Soft reset */
    }

    return RET_NORMAL;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : _pb_RomcheckMask
 * ABSTRACT  : Check the sum value of masked part
 * NOTE      : Check the sum value of the mask ROM (BOOTROM)
 * ARGUMENT  : None
 * RETURN    : RET_API  RET_NORMAL    :Sum value normal
 *                 REL_ERROR    :Sum value error
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
_pb_RomcheckMask(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : _pb_RomcheckApl
 * ABSTRACT  : Check sum value of APL part (NK image)
 * NOTE      : In hardware-independent CEPC emulations do nothing and return RET_NORMAL.
 * ARGUMENT  :
 * RETURN    : RET_API  RET_NORMAL    :Sum Value Normal
 *                 REL_ERROR    :Sum value error
 *            RET_OSERROR    :AccessPhysicalMem error
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
_pb_RomcheckApl(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : PbReset
 * ABSTRACT  : Hard reset processing
 * NOTE      : Manipulate the WDT control register to generate a hard reset
 *           : Hardware-independent CEPC emulations do nothing and return RET_NORMAL.
 * ARGUMENT  : void
 * RETURN    : None
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void
PbReset(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    /* Message log output processing */
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " PbReset:%s:LINE %d\r\n #### HARD RESET !! ####\r\n",
                   LTEXT(__FILE__), __LINE__);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : _pb_GetDeviceInfo
 * ABSTRACT  : Getting device information
 * NOTE      : Gets device information into the specified buffer.
 *           : In hardware-independent CEPC emulations, the following device
 *           : information is returned permanently. The return value is always RET_NORMAL.
 *           : _CWORD31__no="8611052060010001" mci_no="CNCS0200A" date="200301011200"
 * ARGUMENT  : T_SYS_PRGVER     *pbuf   Destination buffer address
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
_pb_GetDeviceInfo(T_SYS_DEVINFO *pbuf) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : _pb_WriteDeviceInfo
 * ABSTRACT  : Write device key information
 * NOTE      : Write device key information to flash memory
 * ARGUMENT  : u_char     *ucpData   Start address of write data (256 bytes)
 * RETURN    : int32      VUP_SUCCESS      Flash write complete
 *           :          VUP_FAILED      Flash write abend
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32
_pb_WriteDeviceInfo(u_char *ucpData) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : _pb_GetSramVer
 * ABSTRACT  : SRAM versioning process
 * NOTE      : Getting SRAM version
 *           : In hardware-independent CEPC emulations, return the following SRAM Version statically.
 *           : The return value is always RET_NORMAL.
 *           : SRAM Version=0x1112 1314
 * ARGUMENT  : char     *pbuf       Destination buffer address
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
_pb_GetSramVer(char *pbuf) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : _pb_GetDisplayInfo
 * ABSTRACT  : Getting display information
 * NOTE      : Get the screen size (width and height) into the buffer of the DISPLAY_INFO structure
 * ARGUMENT  : DISPLAY_INFO    *pSystemInfo  Screen size acquisition destination buffer address
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define DISPLAYINFO_SUBKEY_SIZE    56
#define DISPLAYINFO_VALNAME_SIZE  16
#define DISPLAYINFO_VAL_SIZE    8

RET_API
_pb_GetDisplayInfo(DISPLAY_INFO *pSystemInfo) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
#ifdef PBCOM_CONFIGREAD
    FILE*     fpConfigFile = NULL;            /* Configuration file handle    */
    TCHAR    tcData[DISPLAYINFO_VAL_SIZE] = {0};      /* Data section read buffer  */
    TCHAR    tcDataName[DISPLAYINFO_VALNAME_SIZE] = {0};  /* Data information storage buffer  */
    int    iRet = PBCOM_API_NORMAL;          /* API return value        */
#endif
    int    iHeight = 0;                /* Screen height information        */
    int    iWidth = 0;                  /* Screen width information        */
    RET_API  Retapi = RET_NORMAL;            /* Results of this Module process      */

    /************************************************************
     * Parameter check
     ************************************************************/
    if (NULL == pSystemInfo) {
        /* Parameter is set to error */
        Retapi = RET_ERRPARAM;
    }

    /************************************************************
     * Set the information in the configuration file
     ************************************************************/
#ifdef PBCOM_CONFIGREAD
    /************************************************************
     * Open configuration file
     ************************************************************/
    if (RET_NORMAL == Retapi) {
        fpConfigFile = fopen(PBCOM_CONFIGFILE, "r");
        if (NULL == fpConfigFile) {
            /* Set error in as a result  */
            Retapi = RET_ERROR;
        }
    }

    /************************************************************
     * Get screen width information from configuration file
     ************************************************************/
    if (RET_NORMAL == Retapi) {
        /* Buffer clear  */
        memset(tcData, 0x00, sizeof(tcData));
        memset(tcDataName, 0x00, sizeof(tcDataName));

        /* Data information setting  */
        memcpy(tcDataName, PBCOM_CONFIG_INDEX_SCRWIDTH, strlen(PBCOM_CONFIG_INDEX_SCRWIDTH));

        /************************************************************
         * Setting file information reading process
         ************************************************************/
        Retapi = SysReadConfigInfo(fpConfigFile, tcDataName, tcData, (u_int32)sizeof(tcData));

        /************************************************************
         * Conversion process from character string (UNICODE) to numeric
         ************************************************************/
        if (RET_NORMAL == Retapi) {
            /* Conversion from string to numeric  */
            iHeight = SysUniToInt(tcData, strlen(tcData));
        }
    }

    /************************************************************
     * Get screen height information from setting file
     ************************************************************/
    if (RET_NORMAL == Retapi) {
        /* Buffer clear  */
        memset(tcData, 0x00, sizeof(tcData));
        memset(tcDataName, 0x00, sizeof(tcDataName));

        /* Data information setting  */
        memcpy(tcDataName, PBCOM_CONFIG_INDEX_SCRHEIGHT, strlen(PBCOM_CONFIG_INDEX_SCRHEIGHT));

        /************************************************************
         * Setting file information reading process
         ************************************************************/
        Retapi = SysReadConfigInfo(fpConfigFile, tcDataName, tcData, (u_int32)sizeof(tcData));

        /************************************************************
         * Conversion process from character string (UNICODE) to numeric
         ************************************************************/
        if (RET_NORMAL == Retapi) {
            /* Conversion from string to numeric */
            iWidth = SysUniToInt(tcData, strlen(tcData));
        }
    }

    /************************************************************
     * Configuration file close
     ************************************************************/
    /* Executed only when the open is successful.  */
    if (NULL != fpConfigFile) {
        iRet = fclose(fpConfigFile);
        if (PBCOM_API_NORMAL != iRet) {
            Retapi = RET_ERROR;
        }
    }

    /************************************************************
     * Direct value setting
     ************************************************************/
#else
    iHeight = 480;
    iWidth = 800;
#endif
    /************************************************************
     * Display information setting
     ************************************************************/

    if (RET_NORMAL == Retapi) {
        /* Coverity CID: 18756 compliant */
        pSystemInfo->i_height_size = iHeight;
        pSystemInfo->i_width_size = iWidth;
    } /* Coverity CID: 18756 compliant */

    return Retapi;
}
// LCOV_EXCL_STOP

/******************************************************************************
* MODULE    : RomCheck
* ABSTRACT  : Calculate sum value within range of address specified by argument
* FUNCTION  : u_int32 RomCheck()
* ARGUMENT  : u_int32  *addr      Start address for which sum value is to be calculated
*             u_int32  check_size    Size for which the sum is to be calculated
* NOTE      :
* RETURN    : u_int32  sum        Sum value
******************************************************************************/
u_int32
RomCheck(u_int32 *start_addr, u_int32 check_size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int32     *addr_tmp;
    u_int32     *end_addr;
    u_int32     sum;

    end_addr = start_addr + check_size / sizeof(u_int32);
    addr_tmp = start_addr;

    for (sum = 0; addr_tmp < end_addr;) {
        sum += *(addr_tmp++);
    }
    return(sum);
}
// LCOV_EXCL_STOP

/******************************************************************************
* MODULE    : RomCheckEx
* ABSTRACT  : Calculate sum value in range of address on ROM specified by argument
* FUNCTION  : u_int32 RomCheckEx()
* ARGUMENT  : u_int32  *addr      Start address for which sum value is to be calculated
*             u_int32  check_size    Size for which the sum is to be calculated
* NOTE      : For single operation(For a one-bank configuration). APIs provided by FROM management
*              are used because FROM management must be exclusively controled.
*             The sum calculation unit is assumed to be the sector size of FlashROM.
* RETURN    : u_int32  sum        Sum value
*             When this function terminates with an error, "RET_ERROR" is returned.
******************************************************************************/
u_int32
RomCheckEx(u_int32 *start_addr, u_int32 check_size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(0);
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : AtoxLong
 * ABSTRACT    : Convert ASCII to hex
 * FUNCTION    : Convert a ASCII string (up to 8 characters) to a uint64_t hexadecimal string
 * NOTE      :
 * RETURN    : uint64_t :Converted hexadecimal number (up to 8 digits)
 * INPUT    : char *str -------------------- Pointer to a source ASCII string
 *           unsigned int ascii_size ------ Number of characters in source ASCII string
 * OUTPUT    :
 ******************************************************************************/
uint64_t AtoxLong(char* str, unsigned int ascii_size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    uint64_t  x = 0;    /* Return value */
    unsigned int  i;      /* For work */
    unsigned int  tmp;    /* For work */

    /* Check that the specified ASCII number of characters does not exceed the number of digits represented by int64_t (8 hexadecimal digits). */
    if (ascii_size > (sizeof(x)) * 2) {
        ascii_size = static_cast<unsigned int>(sizeof(x) * 2);
    }
    /* Convert one character at a time to hexadecimal */
    for (i = 0; i < ascii_size; i++) {
        /* Is a number? */
        tmp = *str - '0';
        if (tmp > 9) {
            /* Coverity CID: 18790 compliant */
            /* a-f? */
            tmp = *str - 'a' + 10;
            if ((tmp < 0x0a) || (tmp > 0x0f)) {
                /* A to F ? */
                tmp = *str - 'A' + 10;
                if ((tmp < 0x0a) || (tmp > 0x0f)) {
                    /* Stop when a non-hexadecimal value is found */
                    break;
                }
            }
        }

        /* Increase the digit */
        x <<= 4;
        /* Add to the bottom digit */
        x |= tmp;
        str++;
    }

    return x;
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : XtoaLong
 * ABSTRACT    : Convert HEX to ASCII
 * FUNCTION    : This function converts the 8-digit hexadecimal number from str[0] to the ASCII code and stores the code.
 * NOTE        : The string to be passed as *str must be at least 8 characters in length. * RETURN    :
 * INPUT    : uint64_t x ------------ Source hexadecimal number
 * OUTPUT    : char* str ------------------ Converted ASCII Strings (8 Characters)
 ******************************************************************************/
void XtoaLong(uint64_t x, char* str) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int    i;      /* For looping */
    int    size;    /* For looping */
    char  tmp;    /* Conversion temporary buffer */

    /* Maximum number of loops (for character string) */
    size = static_cast<int>(sizeof(int64_t) * 2);
    /* Convert eight digits */
    for (i = 0; i < size; i++) {
        tmp = static_cast<char>((x >> ((size - i - 1) * 4)) & 0x0F);

        /* For 0xA to 0xF */
        if (tmp > 0x09) {
            str[i] = static_cast<char>('A' + tmp - 0x0A);
        } else {
            str[i] = static_cast<char>('0' + tmp);
        }
    }
}
// LCOV_EXCL_STOP

/******************************************************************************
* MODULE     : _pb_SendSystemError
* ABSTRACT   : System error notification
* FUNCTION   : Notify the system error and reset the software.
* ARGUMENT   : PNO   pno      Error process No.
*              int32 errcode  Error code
* NOTE       :   _pb_SndMsg : Send message (System API)
*                _sys_GetPno : PNO to be gotten
* RETURN     : RET_API defined
******************************************************************************/
RET_API _pb_SendSystemError(PNO pno, int32 errcode) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    T_SYS_SYSTEMERROR        msg;  /* Message         */
    register  T_APIMSG_HEADER    *hdr;  /* Message header */
    RET_API   api_ret;

    /* Message Header Settings */
    hdr = &(msg.bhead.hdr);

    msg.bhead.signo    = 0;            /* Extension member            */
    hdr->sndpno      = 0;                /* Source Process No.       */
    hdr->respno      = 0;          /* Response destination process No.(None) */
    hdr->cid      = CID_INI_SYSTEMERR;   /* Command ID              */
    hdr->msgbodysize  = static_cast<uint16_t>(sizeof(T_SYS_SYSTEMERROR) - sizeof(T_APIMSG_MSGBUF_HEADER));
    /* Main unit size              */
    hdr->rid      = 0;          /* Resources ID (Dummy)      */
    hdr->reserve    = 0;
    msg.pno        = pno;          /* Error process No.       */
    msg.errcode      = errcode;         /* Error code            */

    /* Send Message */
    api_ret = _pb_SndMsg(PNO_GINI_MAIN, sizeof(msg), &msg, 0);

    return(api_ret);
}
// LCOV_EXCL_STOP

/******************************************************************************
* MODULE     : _pb_GetVupFileName
* ABSTRACT   : Getting the version upgrade file name
* FUNCTION   : Get the path name and file name of the version upgrade.
* ARGUMENT   : LPTSTR   filename      File name storage
*              u_char   media         Container media
* NOTE       :
* RETURN     : RET_API defined
******************************************************************************/
RET_API _pb_GetVupFileName(LPTSTR filename , u_char media) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API      api_ret = RET_NORMAL;  /* API return value */
    T_SYS_PRGVER  ver;        /* Version Information */
    RET_API      ret;        /* Internal function return value */
    u_char      kisyu;        /* Storage of model code */

    /* Get version */
    ret = _pb_GetPrgVer(&ver);

    if (ret == RET_NORMAL) {
        /* Extract model code from version */
        kisyu = ver.mask_ver[3];

        /* If the media is a disk */
        if (media == VUP_GETNAME_MEDIA_DISC) {
            _stprintf(filename, _T("%s%s%s%02x%s"),
                      VUP_DISC_PATHNAME, VUP_PATHNAME_SYSTEM, VUP_FNAME_TOP, kisyu, VUP_FNAME_BTM);
        } else {
            /* Media is USB */
            _stprintf(filename, _T("%s%s%s%02x%s"),
                      VUP_USB_PATHNAME, VUP_PATHNAME_SYSTEM, VUP_FNAME_TOP, kisyu, VUP_FNAME_BTM);
        }
    } else {
        api_ret = RET_ERROR;
    }

    return(api_ret);
}
// LCOV_EXCL_STOP

/******************************************************************************
* MODULE     : _pb_GetForceVupFileName
* ABSTRACT   : Getting the version upgrade file name
* FUNCTION   : Get the path name and file name of the version upgrade.
* ARGUMENT   : LPTSTR   filename      File name storage
*              u_char   media         Container media
* NOTE       :
* RETURN     : RET_API defined
******************************************************************************/
RET_API _pb_GetForceVupFileName(LPTSTR filename , u_char media) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API      api_ret = RET_NORMAL;  /* API return value */

    /* If the media is a disk */
    if (media == VUP_GETNAME_MEDIA_DISC) {
        _stprintf(filename, _T("%s%s%s"),
                  VUP_DISC_PATHNAME, VUP_PATHNAME_SYSTEM, VUP_FNAME_FORCEVUP);
    } else {
        /* Media is USB */
        _stprintf(filename, _T("%s%s%s"),
                  VUP_USB_PATHNAME, VUP_PATHNAME_SYSTEM, VUP_FNAME_FORCEVUP);
    }

    return(api_ret);
}
// LCOV_EXCL_STOP

#ifdef PBCOM_CONFIGREAD
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SysGetConfigSize
 * ABSTRACT  : Setting file size acquisition processing
 * NOTE      : Get file size.
 *           : When this Module is called, an area must be allocated at a higher level.
 *           : Parameter checking is not performed in this Module.
 * ARGUMENT  : FILE*  fpFile(File handle)
 *           : int*    piSize(File size storage area)
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static  RET_API
SysGetConfigSize(FILE* fpFile, u_int32* puiSize) {
    int64_t     lCurrentPos = 0;          /* File current position  */
    int64_t     lEndPos = 0;            /* End position of the file    */
    int      iRet = PBCOM_API_NORMAL;      /* API processing result      */
    RET_API    Retapi = RET_NORMAL;        /* Result of this Module process    */

    /************************************************************
     * Get current offset
     ************************************************************/
    lCurrentPos = ftell(fpFile);

    /* If the return value is abnormal, the processing result is set to abnormal.  */
    if (PBCOM_API_ERROR == lCurrentPos) {
        Retapi = RET_ERROR;
    }

    /************************************************************
     * Get end offset (size)
     *   -Set offset to end-of-file
     *   -Get end-of-file offset
     ************************************************************/
    if (RET_NORMAL == Retapi) {
        /* Set offset to end-of-file  */
        iRet = fseek(fpFile, 0L, SEEK_END);

        /* If the return value is abnormal, the processing result is set to abnormal.  */
        if (PBCOM_API_NORMAL != iRet) {
            Retapi = RET_ERROR;
        }
    }

    if (RET_NORMAL == Retapi) {
        /* Get end-of-file offset */
        lEndPos = ftell(fpFile);

        /* Set file size if the return value is normal  */
        if (PBCOM_API_ERROR != lEndPos) {
            *puiSize = (u_int32)lEndPos;
        } else {
            /* If the return value is abnormal, the processing result is set to abnormal.  */
            Retapi = RET_ERROR;
        }
    }

    /************************************************************
     * Revert current offset
     ************************************************************/
    if (RET_NORMAL == Retapi) {
        /* Revert offset and complete processing */
        iRet = fseek(fpFile, lCurrentPos, SEEK_SET);

        /* If the return value is abnormal, the processing result is set to abnormal.  */
        if (PBCOM_API_NORMAL != iRet) {
            Retapi = RET_ERROR;
        }
    }

    return Retapi;
}
#endif /* PBCOM_CONFIGREAD */

#ifdef PBCOM_CONFIGREAD
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SysReadConfigInfo
 * ABSTRACT  : Setting file information reading process
 * NOTE      : Read the data associated with the specified data information from the configuration file.
 *           : When this module is called, an area must be allocated at a higher level.
 *           : Parameter checking is not performed in this module.
 * ARGUMENT  : FILE*  fpConfig(File handle)
 *           : TCHAR*  tcDataIndex(Data information)
 *           : TCHAR*  tcDataBuf(Data read buffer)
 *           : int    tcDataSize(Data read size)
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static RET_API
SysReadConfigInfo(FILE* fpConfig, TCHAR* tcDataIndex, TCHAR* tcDataValue, u_int32 tcDataSize) {
    TCHAR  tcReadBuf[PBCOM_CONFIGSIZE_MAX] = {0};    /* Read buffer    */
    u_int32  uiDataPos = 0;                /* Data position    */
    u_int32  uiCpyPos = 0;                /* Copy position    */
    u_int32  uiFileSize = 0;                /* File size    */
    u_int32  uiReadSize = 0;                /* Data read size  */
    u_int32  uiDataNum = 0;                /* Number of read data    */
    int    iRet = 0;                  /* API processing result      */
    RET_API  Retapi = RET_NORMAL;            /* Result of this Module process    */

    /************************************************************
     * Get setting file size
     ************************************************************/
    /* Get setting file size  */
    Retapi = SysGetConfigSize(fpConfig, &uiFileSize);

    /* When the setting file size is gotten successfully  */
    if (RET_NORMAL == Retapi) {
        /* The maximum size of the configuration file is greater than or equal to the maximum size of the configuration file.  */
        if (uiFileSize > PBCOM_CONFIGSIZE_MAX) {
            /* Set the maximum value of the configuration file as the size */
            uiFileSize = PBCOM_CONFIGSIZE_MAX;
        }
    }

    /************************************************************
     * Read setting file
     ************************************************************/
    if (RET_NORMAL == Retapi) {
        /* Read data count  */
        uiDataNum = uiFileSize / sizeof(TCHAR);

        /* Set file pointer to the beginning of file  */
        rewind(fpConfig);

        /* Read the data  */
        uiReadSize = fread(tcReadBuf, sizeof(TCHAR), uiDataNum, fpConfig);

        /* When the data corresponding to the file size cannot be read  */
        if (uiReadSize != uiFileSize) {
            /* Set error in processing result  */
            Retapi = RET_ERROR;
        }
    }

    /************************************************************
     * Data information search & retrieval
     *   -Find data information from the configuration file you read
     *   -Read associated data if data information can be found 
     ************************************************************/
    if (RET_NORMAL == Retapi) {
        /* Set error to processing result (Set normal when search is normal)  */
        Retapi = RET_ERROR;

        for (uiDataPos = 0; uiDataPos < uiDataNum; uiDataPos++) {
            /* Determine file information  */
            iRet = memcmp(&tcReadBuf[uiDataPos], tcDataIndex, strlen(tcDataIndex));

            /************************************************************
             * If the applicable data information(Data information matched)
             ************************************************************/
            if (0 == iRet) {
                /* Move to data position (Move file information)*/
                uiDataPos += strlen(tcDataIndex) / sizeof(TCHAR);

                /* Set normal to processing result for data discovery  */
                Retapi = RET_NORMAL;

                /************************************************************
                 * Data read processing
                 *   -Read data from the read data size
                 *   -Assume that he end of the data has been reached when the following conditions are met
                 *      NULL character code
                 *      Line feed character code
                 *      The data end of the configuration file is exceeded.
                 ************************************************************/
                for (uiCpyPos = 0; uiCpyPos < tcDataSize; uiCpyPos++) {
                    if (uiDataNum > uiDataPos) {
                        /* Coverity CID: 18781 compliant */
                        /* If it is valid data  */
                        if ((PBCOM_UNICODE_NUL  != tcReadBuf[uiDataPos]) &&
                                (PBCOM_UNICODE_LF  != tcReadBuf[uiDataPos])) {
                            /* Data setting*/
                            tcDataValue[uiCpyPos] = tcReadBuf[uiDataPos];
                        } else {
                            /* Exit from this loop  */
                            break;
                        }
                    } else {
                      /* When the end of data is reached  */
                        /* Exit from this loop  */
                        break;
                    }
                    /* Move the data position by the set amount  */
                    uiDataPos++;
                }
                /************************************************************
                 * Completion of data information search
                 ************************************************************/
                break;
            } else {
                /************************************************************
                 * Not applicable data information(Data information mismatch)
                 ************************************************************/
                /************************************************************
                 * Move to next data information
                 *   -Determine supine data if following condition is met
                 *      Line feed character code
                 *   -End processing when the end of file is reached
                 ************************************************************/
                while (uiDataPos < uiDataNum) {
                    /* Data determination  */
                    if (PBCOM_UNICODE_LF == tcReadBuf[uiDataPos]) {
                        /*Next Data Detection  */
                        break;
                    }
                    /* Move the data by the determined amount  */
                    uiDataPos++;
                }
            }
        }
    }
    return Retapi;
}
#endif /* PBCOM_CONFIGREAD */

#ifdef PBCOM_CONFIGREAD
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SysUniToInt
 * ABSTRACT  : Converting from Unicode(string) to Int(number)
 * NOTE      : Convert from Unicode(string) to Int(number).
 *           : The number of characters that can be converted is defined in PBCOM_UTOIMAX_NUM.
 *           : When this module is called, an area must be allocated at a higher level.
 *           : Parameter checking is not performed in this module.
 * ARGUMENT  : TCHAR*  tcData(Character string storage area)
 *           : int    iDataSize(Character array size)
 * RETURN    : Number converted from character string
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static int SysUniToInt(TCHAR* tcData, u_int32 iDataSize) {
    int    iValue = 0;            /* Converted value    */
    u_int8  ucCount = 0;          /* Character string data position  */

    /************************************************************
     * Parameter check
     ************************************************************/
    if (PBCOM_UTOIMAX_NUM < iDataSize) {
        iDataSize = PBCOM_UTOIMAX_NUM;
    }

    /************************************************************
     * Conversion from string to number
     ************************************************************/
    for (ucCount = 0; ucCount < iDataSize; ucCount++) {
        /*If it is a number  */
        if ((tcData[ucCount] >= '0') &&
                (tcData[ucCount] <= '9')) {
            iValue = (iValue * 10) + (tcData[ucCount] - '0');
        } else {
          /* If it is not a digit  */
            /* End of data  */
            break;
        }
    }
    return iValue;
}
#endif /* PBCOM_CONFIGREAD */

/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 End of File : _sysCommon.cpp
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

