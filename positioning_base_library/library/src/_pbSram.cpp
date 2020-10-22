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
 File name      : _pbSram.cpp
 System name    : 05 Integration Platform
 Subsystem name : System common functions
 Title          : System APIs
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

#include <vehicle_service/positioning_base_library.h>
#include "WPF_STD_private.h"


#ifdef    __cplusplus
extern "C" {
#endif
    /*
     Function prototype declarations
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    RET_API    SramDMWt(u_int32 mode);
    RET_API    SramDMWrkWt(u_int8 id, void *pbuf, u_int32 off, u_int16 size);
    RET_API    SramDMWrkRd(u_int8 id, u_int32 off, void *pbuf, u_int16 size);

#ifdef    __cplusplus
}
#endif

/*
 External function prototype declarations
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
extern RET_API    CreateAsyncWtThread(void);

/*
 Global Variable Definitions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
extern int            g_n_api_set_id;        /* ID variable for PSL registration */
extern void            *g_adr_diagmem_wrktop;    /* Diag memory temporary work area top address */

#ifdef CEPC_EM    /* Physical area (pseudo area) setting for CEPC and EM  */

/* Allocation size of temporary work for diagnosis memory */
#define SIZE_DIAGMEM_WRK        0x00001000

#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SramSetup
 * ABSTRACT  : SRAM Access-Initialization Process
 * NOTE      : When called by _sys_SramInit when _CWORD64_api.dll is attached, the SRAM physical area 
 *             is mapped to the shared area so that the SRAM physical area can be accessed.
 *             The first call after system startup checks the result of the previous SRAM write processing.
 *             If the power is off during system startup, the system performs continuous write processing.
 *             (Before the _CWORD100_ modularization of SRAM access functions,
 *              they were used to open ports, but now they do not need to be opened.)
 * ARGUMENT  : u_int32    mount_flg        HDD mount status flag
 *             u_int32    *mount_sts        HDD mount status return pointer
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
SramSetup(u_int32 mount_flg, u_int32 *mount_sts) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API        ret_sts = RET_OSERROR;
    return(ret_sts);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SramWt
 * ABSTRACT  : SRAM write process
 * NOTE      : Write the content of the buffer to the offset-position of the area of the specified SRAM ID.
 *             Temporary buffer is used for SRAM writing, and recovery from writing is considered.
 *             Therefore, the transfer data size must be less than or equal to the temporary buffer size.
 * ARGUMENT  : u_int8    id        SRAM area id
 *             void        *pbuf    Source buffer pointer
 *             u_int32    off        Destination SRAM offsets (bytes)
 *             u_int16    size    Transfer data size (bytes)
 * RETURN    : RET_API
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
SramWt(u_int8 id, void *pbuf, u_int32 off, u_int16 size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API        ret_sts = RET_OSERROR;
    return(ret_sts);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SramFil
 * ABSTRACT  : SRAM memory-fill process
 * NOTE      : Fill with the specified patterns from the offset position of
 *             the area of the specified SRAM ID.
 *             Temporary buffer is used for SRAM writing, and recovery from writing is considered.
 *             Therefore, it is effective that the fill size is less than or equal to the temporary buffer size.
 * ARGUMENT  : u_int8    id        SRAM area id
 *             u_int32    off        Fill destination SRAM offset (bytes)
 *             u_int8    pat        Fill pattern
 *             u_int16    size    Fill size (bytes)
 *
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
SramFil(u_int8 id, u_int32 off, u_int8 pat, u_int16 size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API        ret_sts = RET_OSERROR;

    ret_sts = PbSramFil32(id, off, pat, (u_int32)size);

    return(ret_sts);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : PbSramFil32
 * ABSTRACT  : SRAM memory-fill process
 * NOTE      : Fill with the specified patterns from the offset position of
 *             the area of the specified SRAM ID.
 *             Temporary buffer is used for SRAM writing, and recovery from writing is considered.
 *             Therefore, it is effective that the fill size is less than or equal to the temporary buffer size.
 * ARGUMENT  : u_int8    id        SRAM area id
 *             u_int32    off        Fill destination SRAM offset (bytes)
 *             u_int8    pat        Fill pattern
 *             u_int32    size    Fill size (bytes)
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
PbSramFil32(u_int8 id, u_int32 off, u_int8 pat, u_int32 size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API        ret_sts = RET_OSERROR;
    return(ret_sts);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SramRd
 * ABSTRACT  : SRAM read process
 * NOTE      : Write content in the area specified SRAM ID and offset position to buffer.
 * ARGUMENT  : u_int8    id        SRAM area ID
 *             u_int32    off        Source SRAM Offset (bytes)
 *             void        *pbuf    Destination buffer pointer
 *             u_int16    size    Transfer data size (bytes)
 * RETURN    : RET_API
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
SramRd(u_int8 id, u_int32 off, void *pbuf, u_int16 size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API        ret_sts = RET_OSERROR;

    ret_sts = PbSramRd32(id, off, pbuf, (u_int32)size);

    return(ret_sts);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : PbSramRd32
 * ABSTRACT  : SRAM read process
 * NOTE      : Write content in the area specified SRAM ID and offset position to buffer.
 * ARGUMENT  : u_int8    id        SRAM area ID
 *             u_int32    off        Source SRAM Offset (bytes)
 *             void        *pbuf    Destination buffer pointer
 *             u_int32    size    Transfer data size(bytes)
 * RETURN    : RET_API
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
PbSramRd32(u_int8 id, u_int32 off, void *pbuf, u_int32 size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API        ret_sts = RET_OSERROR;
    return(ret_sts);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SramSz
 * ABSTRACT  : Acquisition of SRAM ID size
 * NOTE      : Get the effective area size of the specified SRAM ID.
 * ARGUMENT  : u_int8    id        SRAM area ID
 *             u_int16    *psize    Size (bytes)
 * RETURN    : RET_API
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
SramSz(u_int8 id, u_int16 *psize) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API        ret_sts = RET_OSERROR;
    u_int32        size32 = 0;

    ret_sts = PbSramSz32(id, &size32);
    if (ret_sts == RET_NORMAL) {
        if (size32 <= 0x0000FFFF) {
            *psize = (u_int16)size32;
        } else {
            /* When the size of the specified ID is u_int16 or greater */
            ret_sts = RET_ERRPARAM;
        }
    }

    return(ret_sts);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : PbSramSz32
 * ABSTRACT  : Acquisition of SRAM ID size
 * NOTE      : Get the effective area size of the specified SRAM ID.
 * ARGUMENT  : u_int8    id        SRAM area ID
 *             u_int32    *psize    Size (bytes)
 * RETURN    : RET_API
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
PbSramSz32(u_int8 id, u_int32 *psize) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API        ret_sts = RET_OSERROR;
    return(ret_sts);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SramRWChk
 * ABSTRACT  : SRAM read/write check processing
 * NOTE      : Checking whether SRAM can be read/written correctly
 * ARGUMENT  : None
 * RETURN    : RET_API
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
SramRWChk(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API            ret_sts = RET_NORMAL;    /* Result    */
    return ret_sts;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : AsyncFWt
 * ABSTRACT  : Asynchronous Data HDD Backup Processing
 * NOTE      :
 * ARGUMENT  : void
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API AsyncFWt(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API        ret_sts = RET_OSERROR;
    /* _CWORD121_ compliant    ret_sts = Backup_AsyncForcibleWrite(); */
    return(ret_sts);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SramWtAccOff
 * ABSTRACT  : Asynchronous Data HDD Backup Process at ACC-OFF
 * NOTE      : Saving Asynchronous Data to Hard Drives at ACC-OFF
 * ARGUMENT  : void
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API SramWtAccOff(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API        ret_sts = RET_OSERROR;
    /* _CWORD121_ compliant    ret_sts = Backup_AccOffNotify(); */
    return(ret_sts);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DataWtWaitHDDSpinup
 * ABSTRACT  : HDD spin-up completion wait data save processing
 * NOTE      : Save to the HDD
 * ARGUMENT  : u_int16    para    Startup Identification Value
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
DataWtWaitHDDSpinup(u_int16 para) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API            ret_sts = RET_NORMAL;    /* Result    */
    return ret_sts;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SramLoadFromExtDev
 * ABSTRACT  : SRAM recovery processing
 * NOTE      : Restore Static data stored as a file on an externally attached device
 *             and the registry data to the HDD.
 * ARGUMENT  : u_int32        device        Device where the file to be restored resides
 *                                      SD card:EXTDEV_STRAGECARD
 *                                      PC card:EXTDEV_PCCARD
 *             RET_API        *err        Error code
 * RETURN    : Return only RET_ERROR (Just perform reset without returning of this function when normal end)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
SramLoadFromExtDev(ANA_RET_API *err) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API        ret_sts = RET_OSERROR;
    return(ret_sts);
}
// LCOV_EXCL_STOP

/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 End of File : _pbSram.cpp
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

