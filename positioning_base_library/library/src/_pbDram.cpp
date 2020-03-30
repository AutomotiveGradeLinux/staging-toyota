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

/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 File name      : _pbDram.cpp
 System name    : 05 Integration Platform
 Subsystem name : System common functions
 Title          : System API
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

#include <vehicle_service/positioning_base_library.h>
#include "WPF_STD_private.h"

/*
 Constants and structure definitions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define END                 0xFFFFFFFF              /* DRAM R/W check address table end flag */
/* Definitions for CRC calculations */
#define DIV_BYTE_DATA       0x000000FF
#define CRC_INIT            0x00000000
#define DATA_L_SHIFTNUM     8
#define DATA_R_SHIFTNUM     24

/*
 Global Variable Definitions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
extern int      g_n_api_set_id;                        /* ID variable for PSL registration */

/*
External function prototype declaration
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifdef __cplusplus
extern "C" {
#endif
    BOOL            VirtualCopy(LPVOID lpv_dest, LPVOID lpv_src, DWORD cb_size, DWORD fdw_protect);
#ifdef __cplusplus
};
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DramWt
 * ABSTRACT  : DRAM self-refresh area write process
 * NOTE      : Write the content of the buffer to the offset-position of the area of the specified DRAM ID.
 *             Perform CRC calculation and add it to the end of the area
 * ARGUMENT  : u_int8   id      DRAM Area IDs
 *             void     *pbuf   Source buffer pointer
 *             u_int32  off     Destination DRAM offsets(bytes)
 *             u_int16  size    Transfer data size(bytes)
 * RETURN    : RET_API defineed
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
DramWt(u_int8 id, void *pbuf, u_int32 off, u_int16 size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API ret = PbDramWt32(id, pbuf, off, (u_int32)size);
    return(ret);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : PbDramWt32
 * ABSTRACT  : DRAM self-refresh area write process
 * NOTE      : Write the content of the buffer to the offset-position of the area of the specified DRAM ID.
 *             Perform CRC calculation and add it to the end of the area
 * ARGUMENT  : u_int8   id      DRAM Area IDs
 *             void     *pbuf   Source buffer pointer
 *             u_int32  off     Destination DRAM offsets(bytes)
 *             u_int32  size    Transfer data size(bytes)
 * RETURN    : RET_API defined
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
PbDramWt32(u_int8 id, void *pbuf, u_int32 off, u_int32 size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DramRd
 * ABSTRACT  : DRAM self-refresh area read process
 * NOTE      : Write the offsets of the area of the specified DRAM ID to buffer.
 * ARGUMENT  : u_int8   id      DRAM Area IDs
 *             u_int32  off     Source DRAM Offsets(bytes)
 *             void     *pbuf   Destination buffer pointer
 *             u_int16  size    Transfer data size(bytes)
 * RETURN    : RET_API definef
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
DramRd(u_int8 id, u_int32 off, void *pbuf, u_int16 size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DramRd32
 * ABSTRACT  : DRAM self-refresh area read process
 * NOTE      : Write the offsets of the area of the specified DRAM ID to buffer.
 * ARGUMENT  : u_int8   id      DRAM Area IDs
 *             u_int32  off     Source DRAM Offset(bytes)
 *             void     *pbuf   Destination buffer pointer
 *             u_int32  size    Transfer data size(bytes)
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
DramRd32(u_int8 id, u_int32 off, void *pbuf, u_int32 size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DramSz
 * ABSTRACT  : DRAM ID size acquisition processing
 * NOTE      : Get the effective area size of the specified DRAM ID.
 * ARGUMENT  : u_int8   id      DRAM Area IDs
 *             u_int16  *psize  Size(bytes)
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
DramSz(u_int8 id, u_int16 *psize) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DramSz32
 * ABSTRACT  : DRAM ID size acquisition processing
 * NOTE      : Get the effective area size of the specified DRAM ID.
 * ARGUMENT  : u_int8   id      DRAM Area IDs
 *             u_int32  *psize  Size(bytes)
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
DramSz32(u_int8 id, u_int32 *psize) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DramFilCrc
 * ABSTRACT  : DRAM memory-fill process (add CRC-value)
 * NOTE      : Fill with the specified patterns from the offset position of the area of the specified DRAM ID.
 *             After filling, add result of the CRC calculation to the end of the region.
 * ARGUMENT  : u_int8   id      DRAM Area IDs
 *             u_int32  off     Fill destination DRAM offset(bytes)
 *             u_int8   pat     Fill pattern
 *             u_int16  size    Fill size(bytes)
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
DramFilCrc(u_int8 id, u_int32 off, u_int8 pat, u_int16 size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DramFil32Crc
 * ABSTRACT  : DRAM memory-fill process (CRC-value addition)
 * NOTE      : Fill with the specified patterns from the offset position of the area of the specified DRAM ID.
 *             After filling, perform CRC calculation and add it to the end of the region
 * ARGUMENT  : u_int8   id      DRAM Area IDs
 *             u_int32  off     Fill destination DRAM offset(bytes)
 *             u_int8   pat     Fill pattern
 *             u_int32  size    Fill size(bytes)
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
DramFil32Crc(u_int8 id, u_int32 off, u_int8 pat, u_int32 size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DramCrcChk
 * ABSTRACT  : DRAM self-refresh area CRC-check
 * NOTE      : Perform CRC calculation for the specified area and check whether the CRC value is normal.
 * ARGUMENT  : u_int8   id      DRAM Area IDs
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
DramCrcChk(u_int8 id) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DramRWChk
 * ABSTRACT  : DRAM read/write checking
 * NOTE      : Check whether DRAM can be read/written correctly
 * ARGUMENT  : None
 * RETURN    : RET_API define RET_NORMAL      : Normal status
 *                          RET_ERROR       : In case of read/write error
 *                          RET_OSERRER     : Virtual area mapping error
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
DramRWChk(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE   : Crc
 * ABSTRACT : Calculate sum of a given range
 * FUNCTION : RET_API Crc(u_int32 start_addr,u_int32 size, u_int32 *sum);
 * ARGUMENT : u_int32 start_addr Top address
 *            u_int32 size       Size
 *            u_int32* sum       Sum value storage
 * NOTE     : Calculate the 4 bytes sum of the size portion from start_addr and stores the result in sum.
 *            If start_addr and size are not 4-bytes boundaries, errors are returned.
 *            On error, -1 is stored in sum and returned.
 * RETURN   : RET_NORMAL Sum value calculation success
 *            RET_ERROR Sum value calculation failed
 ******************************************************************************/
RET_API
Crc(u_int32 start_addr, u_int32 size, u_int32 *sum) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : DramDestroy
 * ABSTRACT  : SRAM alternative DRAM self-refresh area destruction process
 * NOTE      : The SRAM replacement DRAM self-refresh area is destroyed by adding 1
 *             to the CRC value held by the area ID and destroying the CRC value.
 * ARGUMENT  : u_int8   id      DRAM Area IDs
 * RETURN    : RET_API defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
DramDestroy(u_int8 id) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE   : CrcEx
 * ABSTRACT : Calculate the sum of the CRC values in the given range and the CRC values passed in the arguments
 * FUNCTION : RET_API CrcEx(u_int32 start_addr, u_int32 size, u_int32 *sum, u_int32 addcrc);
 * ARGUMENT : u_int32 start_addr Top address
 *            u_int32 size       Size
 *            u_int32* sum       CRC value storage
 *            u_int32 addcrc     CRC value to add
 * NOTE     : When the 4-bytes-CRC value for the size is calculated from start_addr,
 *            Add the CRC value passed in the fourth argument to sum, and store it in sum.
 *			  Note:Because the address align is undefined, 1-byte calculation shall be performed.
 * RETURN   : RET_NORMAL Sum value calculation success
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
CrcEx(u_int32 start_addr, u_int32 size, u_int32 *sum, u_int32 addcrc) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP



/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 End of File : _pbDram.cpp
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/
