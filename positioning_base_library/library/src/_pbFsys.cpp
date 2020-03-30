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
 File name      : _pbFsys.cpp
 System name    : 05 Integration Platform
 Subsystem name : System common functions
 Title          : System API file access control related processes
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

#include <vehicle_service/positioning_base_library.h>
#include "WPF_STD_private.h"

/*
 Constants and structure definitions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define    ALLOC_SIZE                0x00200000
#define MAX_MUTEX_NAME_LEN        32

/* Flag structure for file system protection */
typedef struct {
    u_int8        file_write_flag;
    u_int8        dummy1;
    u_int8        recover_flag;
    u_int8        dummy2;
} FSYS_FLAG_STRUCT;

/* File system protection flag area control table */
typedef struct {
    TCHAR        mtx_name[MAX_MUTEX_NAME_LEN];    /* Mutex name */
    HANDLE       h_mutex;                            /* Mutex handles */
} FSYS_GLOBAL;

/*
 Global Variable Definitions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
FSYS_FLAG_STRUCT    *g_fsys_flag_top_addr;

/*
 External function prototype declaration
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifdef __cplusplus
extern "C" {
#endif
    BOOL    VirtualCopy(LPVOID lpv_dest, LPVOID lpv_src, DWORD cb_size, DWORD fdw_protect);
#ifdef __cplusplus
}
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : FileSystemCheckInit
 * ABSTRACT  : File system protection flag area and setting value initialization processing
 * NOTE      : Allocate a flag area for protecting the file system, create a Mutex 
 *           : for locking the area, and initialize the flags as follows.
 *           : File access flag        :File Access Permission State
 *           : FlashFS recovery status flag    :FlashFS access-prohibited status
 * ARGUMENT  : None
 * RETURN    : RET_API define
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
FileSystemCheckInit(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return (RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : FileSystemCheckTerm
 * ABSTRACT  : File system protection flag area release processing
 * NOTE      :
 * ARGUMENT  : None
 * RETURN    : None
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void
FileSystemCheckTerm(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SetFileAccessFlag
 * ABSTRACT  : File access flag setting process
 * NOTE      : Sets the write access state in the file access flag.Parameter
 *           : Set "Write prohibited state" at the time of parameter error.
 * ARGUMENT  : u_int8    status    : File access flag setting value
 *                                    WRITE_FLAG_OFF : Write prohibited state to file
 *                                    WRITE_FLAG_ON  : Write permission status for the file
 * RETURN    : None
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void
SetFileAccessFlag(u_int8 status) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : GetFileAccessFlag
 * ABSTRACT  : File access flag acquisition processing
 * NOTE      : Gets the write access status to a file from the file access flag
 * ARGUMENT  : u_int8    *status        Pointer for storing access status
 * RETURN    : RET_NORMAL        Normal completion Note : Always this value
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
GetFileAccessFlag(u_int8 *status) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    *status = WRITE_FLAG_ON;
    return (RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SetFFSRecoverFlag
 * ABSTRACT  : FLASH file system recovery processing status setting processing
 * NOTE      : Sets the status of FLASH file system recovery
 * ARGUMENT  : u_int8    status    : FLASH file system recovery process status setting
 *                                    RECOVER_OFF : Access authorization state
 *                                    RECOVER_ON  : Access prohibited status during recovery processing
 * RETURN    : None
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void
SetFFSRecoverFlag(u_int8 status) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : GetFFSRecoverFlag
 * ABSTRACT  : FLASH file system recovery processing status acquisition processing
 * NOTE      : Gets the status of FLASH file system recovery
 * ARGUMENT  : u_int8    *status        Pointer for storing recovery processing status
 * RETURN    : RET_API define
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
GetFFSRecoverFlag(u_int8 *status) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return (RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : GetFSAccessSts
 * ABSTRACT  : File system access status acquisition processing
 * NOTE      : Gets the access status to the file system
 * ARGUMENT  : u_int8    *status        Pointer for storing the file system access status
 * RETURN    : RET_API define
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
GetFSAccessSts(u_int8 *status) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    *status = FSNOACCESS;

    return (RET_NORMAL);
}
// LCOV_EXCL_STOP

/* End _pbFsys.cpp */
