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
 File name      : _pbOSCtrl.cpp
 System name    :
 Subsystem name :
 Title          :
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <vehicle_service/positioning_base_library.h>
#include "WPF_STD_private.h"
#include "_pbInternalProc.h"
#include "_pbSerial.h"
/* For CreateFile--> */
#include <sys/select.h>
#include <fcntl.h>
#include <strings.h>
#include <termios.h>
/* For CreateFile <-- */

#define GPS_FUNC_DEBUG_MSG__CWORD71__FILE 0

#if GPS_FUNC_DEBUG_MSG__CWORD71__FILE
#define FILE_OUT_LINE    50
static u_int8    file_buf[FILE_OUT_LINE][1024];
static u_int16    line_size_buf[FILE_OUT_LINE];

/* Log destination selection (Activate the one you want to output.) */
#define POS_GPS_R_LOGFILE_PATH "/nv/BS/vs/positioning_base_library/rwdata/Test_Pos_gps_read_data.txt"
#define POS_GPS_W_LOGFILE_PATH "/nv/BS/vs/positioning_base_library/rwdata/Test_Pos_gps_write_data.txt"

#endif /* GPS_FUNC_DEBUG_MSG__CWORD71__FILE */

/* For CreateFile--> */
#define BAUDRATE B9600
#define MODEMDEVICE "/dev/tty.gps"
/* For CreateFile <-- */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* MODULE    : ClearCommError
* ABSTRACT  : Get the communication error information and report the current status of the communication device.
* NOTE      :
* ARGUMENT  :
* RETURN    : BOOL defined
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Wait for replacement */
BOOL ClearCommError(HANDLE h_file, LPDWORD lp_errors, LPCOMSTAT lp_stat) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return 0;
// LCOV_EXCL_STOP
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* MODULE    : GetCommState
* ABSTRACT  : Store the current control settings of the specified communication device in the device control block (DCB struct).
* NOTE      :
* ARGUMENT  :
* RETURN    : BOOL defined
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Wait for replacement */ /* TODO IHFLOW OHFLOW declarations are missing */
BOOL GetCommState(HANDLE h_file, LPDCB lp_dcb) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return 1;
// LCOV_EXCL_STOP
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* MODULE    : GetCommTimeouts
* ABSTRACT  : Get the timeout parameters for all read and write operations performed on the specified communication device.
* NOTE      :
* ARGUMENT  :
* RETURN    : BOOL defined
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

BOOL GetCommTimeouts(HANDLE h_file, LPCOMMTIMEOUTS lp_comm_timeouts) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
#if 1    /* GPF  */
    BOOL bret = FALSE;
    DWORD dw_read = 0;
    DWORD dw_write = 0;

    if ((INVALID_HANDLE_VALUE != h_file) && (NULL != lp_comm_timeouts)) {
        /* Initialization */
        memset(lp_comm_timeouts, 0, sizeof(COMMTIMEOUTS));
        bret = SerialObjectTimeoutGet(h_file, &dw_read, &dw_write);
        if (TRUE == bret) {
            lp_comm_timeouts->write_total_timeout_constant = dw_write;
            if (0 == dw_read) {
                /* Return as 0? or return this setting? */
                lp_comm_timeouts->read_interval_timeout = INFINITE;
                lp_comm_timeouts->read_total_timeout_constant = 0;
                lp_comm_timeouts->read_total_timeout_multiplier = 0;
            } else {
                lp_comm_timeouts->read_total_timeout_constant = dw_read;
            }
        } else {
            /* not exist in the list */
            lp_comm_timeouts->write_total_timeout_constant = INFINITE;
            lp_comm_timeouts->read_total_timeout_constant = INFINITE;
            bret = TRUE;
        }
    }

    return bret;

#endif /* GPF  */
}
// LCOV_EXCL_STOP

/**
 * @brief
 *  Clear Communication Buffer
 *
 *  Discard all characters in the output buffer or input buffer of the specified resource
 *
 *
 * @param[in] h_file     Handle of the communication resource
 * @param[in] dw_flags   Operation to perform
 *
 * @return   Processing result
 * @retval   TRUE      processing succeeded
 * @retval   FALSE     Processing failed
 */
BOOL PurgeComm(HANDLE h_file, DWORD dw_flags) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BOOL ret = 0;
    int res = -1;
    /* h_file is FD,  and less than INT_MAX(0x7fffffff) */
    /* Flush received but unreceived data */
    res = tcflush(static_cast<int>((long)h_file), TCIFLUSH);  // NOLINT(runtime/int)
    if (res != -1) {
        ret = 1;
    }
    return ret;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* MODULE    : SetCommMask
* ABSTRACT  : Specify a set of events to monitor for a specific communication device.
* NOTE      :
* ARGUMENT  :
* RETURN    : BOOL defined
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

BOOL SetCommMask(HANDLE h_file, DWORD dw_evt_mask) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
#if 1    /* GPF  */
    BOOL bret = FALSE;

    bret = SerialObjectWaitmaskAdd(h_file, dw_evt_mask);
    return bret;
#endif /* GPF  */
}
// LCOV_EXCL_STOP

/**
 * @brief
 *
 * Communication setting
 *
 * Set the serial communication according to the setting information.
 *
 * @param[in] h_file     Communication handle
 * @param[in] lp_dcb     Serial port setting information
 *
 * @return   Processing result
 * @retval   TRUE      processing succeeded
 * @retval   FALSE     Processing failed
 */
BOOL SetCommState(HANDLE h_file, LPDCB lp_dcb) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    struct termios newtio;
#if 1
    bzero(&newtio, sizeof(newtio));
    cfmakeraw(&newtio);   /* RAW mode */
    newtio.c_cflag |= (BAUDRATE |  CS8 | CLOCAL | CREAD);/*Baud:9600*/
    newtio.c_lflag &= ~ECHO;
#endif
     /* h_file is FD,  and less than INT_MAX(0x7fffffff) */
    tcflush(static_cast<int>((long)h_file), TCIFLUSH);  // NOLINT(runtime/int)
    tcsetattr(static_cast<int>((long)h_file), TCSANOW, &newtio);  // NOLINT(runtime/int)

    return 1;
}
// LCOV_EXCL_STOP


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* MODULE    : SetCommTimeouts
* ABSTRACT  : Set the timeout parameter for all read and write operations performed on the specified communication device.
* NOTE      :
* ARGUMENT  :
* RETURN    : BOOL defined
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

BOOL SetCommTimeouts(HANDLE h_file, LPCOMMTIMEOUTS lp_comm_timeouts) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
#if 1    /* GPF  */
    BOOL bret = FALSE;
    DWORD dw_read_timeout = INFINITE;
    DWORD dw_write_timeout = INFINITE;

    /* Calculate timeout time */
    if ((INFINITE == lp_comm_timeouts->read_interval_timeout)
            && (0 == lp_comm_timeouts->read_total_timeout_constant)
            && (0 == lp_comm_timeouts->read_total_timeout_multiplier)) {
        /* Set without waiting */
        dw_read_timeout = 0;
    } else {
        dw_read_timeout = lp_comm_timeouts->read_total_timeout_constant;
    }


    dw_write_timeout = lp_comm_timeouts->write_total_timeout_constant;

    bret = SerialObjectTimeoutAdd(h_file, dw_read_timeout, dw_write_timeout);

    return bret;
#endif /* GPF  */
}
// LCOV_EXCL_STOP

/**
 * @brief
 *
 * Waiting for communication event
 *
 * Wait for an event to be generated for a specified handle
 * Note : Continue to wait indefinitely until data reception/error occurs
 *
 * @param[in]  h_file         Communication handle
 * @param[out] lp_evt_mask     Pointer to the variable to receive the event
 * @param[in]  lp_overlapped  OVERLAPPED Pointer to a struct[Note:Not used]
 *
 * @return   Processing result
 * @retval   TRUE     Processing succeeded
 * @retval   FALSE    Processing failed
 */
BOOL WaitCommEvent(HANDLE h_file, LPDWORD lp_evt_mask, LPOVERLAPPED lp_overlapped) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BOOL ret = 0;
    int res = -1;
    int fd;
    fd_set fds_set_err;
    fd_set fds_set;    // Set of file descriptor

    struct timeval tv;

    /* Monitor for 0.5 seconds */
    tv.tv_sec = 1;
    tv.tv_usec = 0;
     /* h_file is FD,  and less than INT_MAX(0x7fffffff) */
    fd = static_cast<int>((long)h_file);  // NOLINT(runtime/int)

    /* Initialization */
    FD_ZERO(&fds_set);
    FD_ZERO(&fds_set_err);

    FD_SET(fd, &fds_set);
    FD_SET(fd, &fds_set_err);

    res = select(fd + 1, &fds_set, NULL, &fds_set_err, &tv);

    if (res > 0) {
        if (FD_ISSET(fd, &fds_set)) {
            *lp_evt_mask = EV_RXCHAR;
            ret = 1;

        } else {
            *lp_evt_mask = EV_ERROR;
            ret = 0;
        }
    } else {
        ret = 0;
    }
    return ret;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *
 * Create file
 *
 * Create or open an object and return a handle which can be used to access the object
 *
 * @param lp_file_name                Not used
 * @param dw_desired_access           Not used
 * @param dw_share_mode               Not used
 * @param lp_security_attributes      Not used
 * @param dw_creation_disposition     Not used
 * @param dw_flags_and_attributes     Not used
 * @param h_template_file               Not used
 *
 * @return   Handle
 */
HANDLE CreateFile(LPCTSTR lp_file_name, DWORD dw_desired_access, DWORD dw_share_mode, LPSECURITY_ATTRIBUTES lp_security_attributes, DWORD dw_creation_disposition, DWORD dw_flags_and_attributes, HANDLE h_template_file) {  // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int fd;
    int loop = 1;

    while (loop == 1) {
        fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY);
        if (fd != -1) {
            break;
        }
        sleep(1);
    }
    /*  fd is FD,  and less than INT_MAX(0x7fffffff) and the return data type is HANDLE. */
    return (HANDLE)((long)(fd));  // NOLINT(runtime/int)
}
// LCOV_EXCL_STOP

/**
 * @brief
 *
 * Close file
 *
 * Close an open file
 *
 * @param[in] h_object   Handle
 *
 * @return   Processing result
 * @retval   TRUE     Processing succeeded
 * @retval   FALSE    Processing failed
 */
BOOL CloseFile(HANDLE h_object) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (h_object != reinterpret_cast<void*>(-1)) {
         /* h_object is FD,  and less than INT_MAX(0x7fffffff) */
        close(static_cast<int>((long)h_object));  // NOLINT(runtime/int)
    } else {
        /* nop */
    }
    return 0;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *
 * Write File
 *
 * Writing Data to a File
 *
 * @param[in]  h_file                  Handle
 * @param[in]  lp_buffer               buffer to write
 * @param[out] n_number_of_bytes_to_write  Maximum writing size
 * @param[out] lpNumberOfBytesWrite   Writing size
 * @param      lp_overlapped           Not used
 *
 * @return   Processing result
 * @retval   TRUE     Processing succeeded
 * @retval   FALSE    Processing failed
 */
BOOL WriteFile(HANDLE h_file, LPCVOID lp_buffer, DWORD n_number_of_bytes_to_write, LPDWORD lp_number_of_bytes_written, LPOVERLAPPED lp_overlapped) {  // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int res = -1;
    BOOL ret = 0;
    int fd;
    fd_set fds_set;    /* Set of file descriptors */
    fd_set fds_set_err;

    struct timeval tv;

#if GPS_FUNC_DEBUG_MSG__CWORD71__FILE
    static FILE*            fp = NULL;              /* For debugging            */
    int i;
#endif /* GPS_FUNC_DEBUG_MSG__CWORD71__FILE */

    /* monitor for 2 seconds.*/
    tv.tv_sec = 2;
    tv.tv_usec = 0;
     /* h_file is FD,  and less than INT_MAX(0x7fffffff) */
    fd = static_cast<int>((long)h_file);  // NOLINT(runtime/int)

    /* Initialization */
    FD_ZERO(&fds_set);
    FD_ZERO(&fds_set_err);

    FD_SET(fd, &fds_set);
    FD_SET(fd, &fds_set_err);

    res = select(fd + 1, &fds_set, NULL, &fds_set_err, &tv);
    if (res > 0) {
        if (FD_ISSET(fd, &fds_set)) {
             /* h_file is FD,  and less than INT_MAX(0x7fffffff) */
            res = static_cast<int>(write(static_cast<int>((long)h_file), lp_buffer,  // NOLINT(runtime/int)
                    n_number_of_bytes_to_write));
            *lp_number_of_bytes_written = res;

#if GPS_FUNC_DEBUG_MSG__CWORD71__FILE
            if (fp == NULL) {
                /* File initialization */
                fp = fopen(POS_GPS_W_LOGFILE_PATH, "w+");
            } else {
                fp = fopen(POS_GPS_W_LOGFILE_PATH, "a+");
            }
            for (i = 0; i < n_number_of_bytes_to_write; i++) {
                fprintf(fp, "%02x ", *(reinterpret_cast<char *>(lp_buffer) + i));
            }
            fprintf(fp, "\n");
            fclose(fp);
#endif /* GPS_FUNC_DEBUG_MSG__CWORD71__FILE */
        }
    } else {
    }

    if (res != -1) {
        ret = 1;
    }

    return ret;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* MODULE    : Wsprintf
* ABSTRACT  : Format a string and store the value in a buffer. If any of the arguments are passed,
*            Format according to the corresponding format specifier in the format control string and copies it to the output buffer.
* NOTE      :
* ARGUMENT  :
* RETURN    : int defined
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int Wsprintf(LPTSTR lp_out, LPCTSTR lp_fmt, ...) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
#if 1    /* GPF  */
    int rtn;
    va_list args;

    va_start(args, lp_fmt);
    /* Seems to be occured problems because the buffer size is not known. */
    rtn = vswprintf(reinterpret_cast<wchar_t*>(lp_out), 256, reinterpret_cast<const wchar_t*>(lp_fmt), args);
    va_end(args);

    return rtn;
#endif /* GPF  */
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* MODULE    : PbSystemTimeToFileTime
* ABSTRACT  : Convert the system date and time to 64-bit format file time.
* NOTE      :
* ARGUMENT  :
* RETURN    : BOOL defined
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL PbSystemTimeToFileTime(const SYSTEMTIME* lp_system_time, LPFILETIME lp_file_time) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return FALSE;
}
// LCOV_EXCL_STOP

/**
 * Function name : MunmapDeviceIo<br>
 *          Register unmapping<br>
 *
 * Overview   : Remove the mapping of a register to memory.<br>
 *
 * -# Release the mapping of the specified register to memory.
 *
 * @param h_dev      [I]Handle to the device object
 * @param map_size   [I]Size of mapped device I/O memory
 *
 * @return   RET_API      Processing result
 * @retval   RET_NORMAL   Processing succeeded
 * @retval   RET_ERROR    Processing failed
 */
/* Wait for replacement */
RET_API MunmapDeviceIo(HANDLE h_dev, u_int32 map_size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RET_NORMAL;
// LCOV_EXCL_STOP
}

/**
 * Function name : MmapDeviceIo<br>
 *          Register mapping<br>
 *
 * Overview   : Map registers to memory.<br>
 *
 * -# Map the specified register to memory.
 *
 * @param map_size   [I]Size of mapped device I/O memory
 * @param map_addr   [I]Mapped physical address
 *
 * @return   HANDLE    Processing result
 * @retval   Except NULL  Handle
 * @retval   NULL      Processing failed
 */
/* Wait for replacement */
HANDLE MmapDeviceIo(u_int32 map_size, u_int32 map_addr) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RET_NORMAL;
// LCOV_EXCL_STOP
}

/**
 * @brief
 *
 * Kill
 *
 * Exit program
 *
 * @param[in]  p_func         Calling function
 * @param[in]  line          Number of caller rows
 */
void _pb_Exit_d(const char* p_func, int line) {  // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)  // NOLINT(readability/nolint)  WPF_SYSAPI.h API
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int ret = -1;

    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FORBIDDEN ERROR [Called by:%s, Line:%d]", p_func, line);
    exit(ret);

    return;
}
// LCOV_EXCL_STOP


/* GPF001_sample_ttaka add end */

