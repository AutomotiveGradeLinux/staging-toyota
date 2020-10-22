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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 File name      : _pbProcMng.cpp
 System name    :
 Subsystem name :
 Title          :
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <fcntl.h>
#include <sys/procfs.h>
#include <vehicle_service/positioning_base_library.h>
#include "WPF_STD_private.h"
#include "_pbInternalProc.h"
#include "_pbWaitforsingleobject.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : ExitProcess
 * ABSTRACT  : Terminate one process and all threads belonging to that process.
 * NOTE      :
 * ARGUMENT  :
 * RETURN    : VOID defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
VOID ExitProcess(UINT u_exit_code) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    exit(u_exit_code);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : GetCurrentProcess
 * ABSTRACT  : Get a pseudo handle for the current process.
 * NOTE      :
 * ARGUMENT  :
 * RETURN    : HANDLE defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* QAC 3460 */
HANDLE GetCurrentProcess(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return reinterpret_cast<HANDLE>(getpid());
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : GetCurrentProcessId
 * ABSTRACT  : Get the process identifier of the calling process.
 * NOTE      :
 * ARGUMENT  :
 * RETURN    : DWORD defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* QAC 3460 */
DWORD GetCurrentProcessId(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return (DWORD)getpid();
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : _pb_ExitThread
 * ABSTRACT  : Terminate a thread
 * NOTE      :
 * ARGUMENT  :
 * RETURN    : VOID defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
VOID _pb_ExitThread(DWORD dw_exit_code) {   // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)  // NOLINT(readability/nolint)  WPF_SYSAPI.h API
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* Delete handle type */
    WaitObjectDel(reinterpret_cast<HANDLE*>(pthread_self()));

    return pthread_exit (reinterpret_cast<void*>(dw_exit_code));
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : TerminateThread
 * ABSTRACT  : Terminate a thread
 * NOTE      :
 * ARGUMENT  :
 * RETURN    : BOOL defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL TerminateThread(HANDLE h_thread, DWORD dw_exit_code) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32    lret = EOK;
    BOOL    bret = TRUE;

    /* Delete handle type */
    WaitObjectDel(h_thread);

    /* When an error occurs */
    if (lret == EOK) {
        bret = FALSE;
    }

    return bret;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : GetExitCodeThread
 * ABSTRACT  : Get the exit status of the specified thread.
 * NOTE      :
 * ARGUMENT  :
 * RETURN    : BOOL defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL GetExitCodeThread(HANDLE h_thread, LPDWORD lp_exit_code) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return TRUE;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : WaitExitThread
 * ABSTRACT  : Wait for the thread to terminate.
 * NOTE      : It can only be used for a thread in the same process according to PosixBasedOS001.
 * ARGUMENT  :
 * RETURN    : BOOL defined
 *         RET_NORMAL        : Normal completion
 *         RET_ERROR        : ABEND
 *         RET_ERRTIMEOUT    : Timeout occurred
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API WaitExitThread(HANDLE h_handle, u_int32 ul_mill_seconds, u_int32* pul_exit_code) {  // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RET_NORMAL;    /* Coverity CID: 18759 compliant */
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : GetCurrentThread
 * ABSTRACT  : Get a pseudo handle for the current thread.
 * NOTE      :
 * ARGUMENT  :
 * RETURN    : HANDLE defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* QAC 3460 */
HANDLE GetCurrentThread(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return (HANDLE)pthread_self();
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : GetCurrentThreadId
 * ABSTRACT  : Get the thread identifier of the calling thread.
 * NOTE      :
 * ARGUMENT  :
 * RETURN    : DWORD defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* QAC 3460 */
DWORD GetCurrentThreadId(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return (DWORD)pthread_self();
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : GetThreadTimes
 * ABSTRACT  : Get time information about the specified thread.
 * NOTE      :
 * ARGUMENT  :
 * RETURN    : BOOL defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL GetThreadTimes(HANDLE h_process, HANDLE h_thread, LPFILETIME lp_creation_time, LPFILETIME lp_exit_time, LPFILETIME lp_kernel_time, LPFILETIME lp_user_time) {  // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return TRUE;    /* Coverity CID: 18765 compliant */
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : ResumeThread
 * ABSTRACT  : Decrease the thread suspend count by 1.
 * NOTE      : Always return 0 because no suspend operation is supported according to PosixBasedOS001.
 * ARGUMENT  :
 * RETURN    : DWORD defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
DWORD ResumeThread(HANDLE h_thread) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return 0;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SuspendThread
 * ABSTRACT  : Suspend execution of the specified thread.
 * NOTE      : Always return 0 because no suspend operation is supported according to PosixBasedOS001.
 * ARGUMENT  :
 * RETURN    : DWORD defined
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
DWORD SuspendThread(HANDLE h_thread) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return 0;
}
// LCOV_EXCL_STOP

