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
#define POS_GPS_R_LOGFILE_PATH "/nv/log/frameworkunifiedlog/Test_Pos_gps_read_data.txt"
#define POS_GPS_W_LOGFILE_PATH "/nv/log/frameworkunifiedlog/Test_Pos_gps_write_data.txt"

#endif /* GPS_FUNC_DEBUG_MSG__CWORD71__FILE */

static BOOL ReadFile_CWORD82_(HANDLE h_file, LPVOID lp_buffer, DWORD n_number_of_bytes_to_read, \
        LPDWORD lp_number_of_bytes_read, LPOVERLAPPED lp_overlapped);
static BOOL ReadFileUblox(HANDLE h_file, LPVOID lp_buffer, DWORD n_number_of_bytes_to_read, \
        LPDWORD lp_number_of_bytes_read, LPOVERLAPPED lp_overlapped);

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

#define MAX_READ_SIZE 2048
/**
 * @brief
 *
 * Read file
 *
 * Read data from a file (Wrapper Function)
 *
 * @param[in]  h_file                 Handle
 * @param[in]  lp_buffer              Buffer for reading
 * @param[out] n_number_of_bytes_to_read  Maximum read size
 * @param[out] lp_number_of_bytes_read   Read size
 * @param lp_overlapped               Not used
 *
 * @return   Processing result
 * @retval   TRUE     Processing succeeded
 * @retval   FALSE    Processing failed
 */
BOOL ReadFile(HANDLE h_file, LPVOID lp_buffer, DWORD n_number_of_bytes_to_read, LPDWORD lp_number_of_bytes_read, LPOVERLAPPED lp_overlapped) {  // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BOOL        ret = 0;
    UNIT_TYPE   e_type = UNIT_TYPE_NONE;                 /* Supported HW configuration type */

    e_type = PbGetEnvSupportInfo();
    if (UNIT_TYPE_GRADE1 == e_type) {
        ret = ReadFileUblox(h_file, lp_buffer, n_number_of_bytes_to_read, lp_number_of_bytes_read, lp_overlapped);
    } else if (UNIT_TYPE_GRADE2 == e_type) {
      /*
       *  Note.
       *  This feature branches processing depending on the unit type.
       */
        ret = ReadFile_CWORD82_(h_file, lp_buffer, n_number_of_bytes_to_read, lp_number_of_bytes_read, lp_overlapped);
    } else {
        ret = FALSE;
    }

    return ret;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *
 * Read file
 *
 * Read data from a file(Implementation Functions for _CWORD82_)
 *
 * @param[in]  h_file                 Handle
 * @param[in]  lp_buffer              Read buffer
 * @param[out] n_number_of_bytes_to_read  Maximum read size
 * @param[out] lp_number_of_bytes_read   Read size
 * @param lp_overlapped               Not used
 *
 * @return   Processing result
 * @retval   TRUE     Processing succeeded
 * @retval   FALSE    Processing failed
 */
BOOL ReadFile_CWORD82_(HANDLE h_file, LPVOID lp_buffer, DWORD n_number_of_bytes_to_read, LPDWORD lp_number_of_bytes_read, LPOVERLAPPED lp_overlapped) {  // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BOOL    ret = 0;
    static int     res = 0;
    DWORD       fdw_comm_mask;                    /* Communication event storage area               */

    static u_int8  ret_buf[MAX_READ_SIZE]    = {0};   /* Buffer for return */
    static u_int8 *wp       = ret_buf;

    static u_int8 work_buf[MAX_READ_SIZE];

    static u_int8 key = 0;   /* Primary key */
    static BOOL is_set_key = FALSE;   /* Key is set or not */
    static u_int8 key2 = 0;   /* Secondary key */
    static BOOL is_set_key2 = FALSE;   /* Key2 is set or not */
    static int32 work_bufcnt = 0;   /* Backup read pointer of work_buf */

    int32 i;
    int32 j;
    u_int8 val;
    BOOL ret_flg = FALSE;
    static BOOL is_detect_err = FALSE;

#if GPS_FUNC_DEBUG_MSG__CWORD71__FILE
    static  u_int16     file_line_size   = 0;
    static  FILE*       fp               = NULL;                /* For debugging            */
    u_int16             line_size = 0;
    u_int16             file_loop;
    u_int16             file_loop2;
#endif /* GPS_FUNC_DEBUG_MSG__CWORD71__FILE */

    for (i = 0; i < MAX_READ_SIZE; i++) {
        if (work_bufcnt == 0) /* Last read completed */ {
            res = -1;
            if (WaitCommEvent(h_file, (LPDWORD)&fdw_comm_mask, NULL) != FALSE) {
                 /* h_file is FD,  and less than INT_MAX(0x7fffffff) */
                res = static_cast<int>(read(static_cast<int>((long)h_file), work_buf,  // NOLINT(runtime/int)
                        n_number_of_bytes_to_read));
            }
        }

        if (res == -1) {
            break;
        } else {
            for (j = work_bufcnt; j < res; j++) {
                val = *(work_buf + j);
                *wp = val;

                if (is_detect_err == TRUE) {
                    /* If any errors are detected, skip the 1-byte so that the system can be restored to normal */
                    is_detect_err = FALSE;
                } else {
                    if ((key == val) && (is_set_key == TRUE)) {
                        ret_flg = TRUE;
                        *lp_number_of_bytes_read = (wp - ret_buf) + 1;   /* calculate total read size */

                        key = 0;
                        is_set_key = FALSE;
                        wp = ret_buf;
                        ret = TRUE;
                        break;
                    } else if ((val == 0x24) &&
                             ((key != 0xDA) || (is_set_key == FALSE)) &&
                             ((key2 != 0xBF) || (is_set_key2 == FALSE))) {
                        key = 0x0A;   /* End of NEMA sentence */
                        is_set_key = TRUE;
                        wp++;
                    } else if ((val == 0xB0) &&
                             ((key != 0x0A) || (is_set_key == FALSE))) {
                        key2 = 0xBF;
                        is_set_key2 = TRUE;
                        wp++;
                    } else if ((key2 == val) && (is_set_key2 == TRUE)) {
                        key = 0xDA;   /* End of _CWORD82_ Binary */
                        is_set_key = TRUE;
                        is_set_key2 = FALSE;
                        wp++;
                    } else if ((is_set_key == TRUE) || (is_set_key2 == TRUE)) {
                        wp++;
                    } else {
                        /* nothing */
                    }

                    /* PastModel008 Fix reset problem + */
                    if ((wp - ret_buf) + 1 >= MAX_READ_SIZE) {
                      ret_flg = TRUE;
                      *lp_number_of_bytes_read = (wp - ret_buf) + 1; /* calculate total read size */
                      wp = ret_buf;
//                      fprintf(fp, "detect buffer ret_buf full!! size = %d\n", *lp_number_of_bytes_read);
                      break;
                    }
                    /* PastModel008 Fix reset probrem - */
                }
            }

            if (j == res) /* When break was not performed during work_buf analysis */ {
                work_bufcnt = 0;
            }

            if (ret_flg == TRUE) {
                work_bufcnt = j;
                break;
            }
        }
    }


//    if ((i == MAX_READ_SIZE) || ((wp - ret_buf) + 1 > MAX_READ_SIZE)) {
    if ((i == MAX_READ_SIZE) || (*lp_number_of_bytes_read >= MAX_READ_SIZE)) { /* detect error */
        /* detect error */
        key = 0;
        is_set_key = FALSE;
        is_set_key2 = FALSE;
        is_detect_err = TRUE;
    } else {
        if (ret_flg == TRUE) {
            /* PastModel008 Fix reset problem + */
            if (*lp_number_of_bytes_read >= MAX_READ_SIZE) { /* PastModel008 Fix reset problem */
                ret = FALSE;
            } else {
            memcpy(lp_buffer, ret_buf, *lp_number_of_bytes_read);
            }
            /* PastModel008 Fix reset problem - */
        }
    }

#if GPS_FUNC_DEBUG_MSG__CWORD71__FILE
    for (file_loop = 0; file_loop < *lp_number_of_bytes_read; file_loop++) {
        file_buf[file_line_size][line_size++] = *(ret_buf + file_loop);
    }

    line_size_buf[file_line_size] = line_size;
    file_line_size++;

    if (file_line_size >= FILE_OUT_LINE) {
        if (fp == NULL) {
            /* File initialization */
            fp = fopen(POS_GPS_R_LOGFILE_PATH, "w+");
        } else {
            fp = fopen(POS_GPS_R_LOGFILE_PATH, "a+");
        }

        for (file_loop = 0; file_loop < FILE_OUT_LINE; file_loop++) {
            for (file_loop2 = 0; file_loop2 < line_size_buf[file_loop]; file_loop2++) {
                fprintf(fp, "%02x ", file_buf[file_loop][file_loop2]);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
        file_line_size = 0;
    }
#endif /* GPS_FUNC_DEBUG_MSG__CWORD71__FILE */

    return ret;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *
 * Read file
 *
 * Read data from a file(Implemented Function for ublox)
 *
 * @param[in]  h_file                 Handle
 * @param[in]  lp_buffer              Read buffer
 * @param[out] n_number_of_bytes_to_read  Maximum read size
 * @param[out] lp_number_of_bytes_read   Read size
 * @param lp_overlapped               Not used
 *
 * @return   Processing result
 * @retval   TRUE     processing succeeded
 * @retval   FALSE    Processing failed
 */
BOOL ReadFileUblox(HANDLE h_file, LPVOID lp_buffer, DWORD n_number_of_bytes_to_read, LPDWORD lp_number_of_bytes_read, LPOVERLAPPED lp_overlapped) {  // LCOV_EXCL_START 8:dead code  NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BOOL    ret = 0;
    static int     res = 0;
    DWORD       fdw_comm_mask;                    /* Communication event storage area */

    static u_int8  ret_buf[MAX_READ_SIZE]    = {0};   /* Buffer for return */
    static u_int8 *wp       = ret_buf;

    static u_int8 work_buf[MAX_READ_SIZE];

    static u_int8 key = 0;   /* Primary key */
    static BOOL is_set_key = FALSE;   /* Key is set or not */
    static BOOL is_set_key2 = FALSE;   /* Key2 is set or not */

    int32 i;
    int32 j;
    u_int8 val;
    BOOL ret_flg = FALSE;
    static BOOL is_detect_err = FALSE;
    static int32 ubx_size   = 0;   /* Support when Sentence span more than one read() */
    static int32 ubx_count  = 0;   /* Support when Sentence span more than one read() */
    static int32 work_bufcnt = 0;   /* Backup the read pointer of work_buf */

#if GPS_FUNC_DEBUG_MSG__CWORD71__FILE
    static  u_int16     file_line_size   = 0;
    static  FILE*       fp               = NULL;        /* For debugging */
    u_int16             line_size = 0;
    u_int16             file_loop;
    u_int16             file_loop2;
#endif /* GPS_FUNC_DEBUG_MSG__CWORD71__FILE */

    for (i = 0; i < MAX_READ_SIZE; i++) {
        if (work_bufcnt == 0) /* Last read completed */ {
            res = -1;
            if (WaitCommEvent(h_file, (LPDWORD)&fdw_comm_mask, NULL) != FALSE) {
                 /* h_file is FD,  and less than INT_MAX(0x7fffffff) */
                res = static_cast<int>(read(static_cast<int>((long)h_file), work_buf,  // NOLINT(runtime/int)
                        n_number_of_bytes_to_read));
            }
        }

        if (res == -1) {
            break;
        } else {
            for (j = work_bufcnt; j < res; j++) {
                val = *(work_buf + j);
                *wp = val;

                if (is_detect_err == TRUE) {
                    /* If any errors are detected, skip the 1Byte so that the system can be restored to normal */
                    is_detect_err = FALSE;
                } else {
                    if (((key == val) && (is_set_key == TRUE))
                        || ((ubx_count >= (7 + ubx_size)) && (is_set_key2 == TRUE))) {
                        /* At the end of the sentence */
                        ret_flg = TRUE;
                        *lp_number_of_bytes_read = (wp - ret_buf) + 1;   /* calculate total read size */

//                        fprintf(fp, "fixed UBX sentence ubx_size = %d, ubx_count = %d, *lp_number_of_bytes_read = %d,
//                                wp = %x, ret_buf = %x\n", ubx_size, ubx_count, *lpNumberOfBytesRead, wp, retBuf);
                        key = 0;
                        ubx_size = 0;    /* Clear when end of ubx is judged */
                        ubx_count = 0;   /* Clear when end of ubx is judged */
                        is_set_key = FALSE;
                        is_set_key2 = FALSE;
                        ret = TRUE;
                        wp = ret_buf;
                        break;
                    } else if ((val == 0x24) && (is_set_key == FALSE) && (is_set_key2 == FALSE)) {
                        /* At the beginning of the NMEA */
                        key = 0x0A;   /* End of NEMA sentence */
                        is_set_key = TRUE;
                        wp++;
                    } else if ((val == 0xB5) && (is_set_key == FALSE) && (is_set_key2 == FALSE)) {
                        /* At the beginning of the UBX */
                        is_set_key2 = TRUE;
                        wp++;
                    } else if ((ubx_count == 4) && (is_set_key2 == TRUE)) {
                        /* Payload size of the UBX(Lower byte) */
                        ubx_size = val;
                        wp++;
                    } else if ((ubx_count == 5) && (is_set_key2 == TRUE)) {
                        /* Payload size of the UBX(Upper byte) */
                        ubx_size += (val << 8);
                        wp++;
                    } else if ((is_set_key == TRUE) || (is_set_key2 == TRUE)) {
                        wp++;
                    } else {
                        /* nothing */
                    }

                    /* PastModel008 Fix reset problem + */
                    if ((wp - ret_buf) + 1 >= MAX_READ_SIZE) {
                        ret_flg = TRUE;
                        *lp_number_of_bytes_read = (wp - ret_buf) + 1; /* calculate total read size */
                        wp = ret_buf;
//                        fprintf(fp, "detect buffer ret_buf full!! size = %d\n", *lp_number_of_bytes_read);
                        break;
                    }
                    /* PastModel008 Fix reset problem - */

                    if (((ubx_count == 5) && (is_set_key2 == TRUE)) &&
                            ((ubx_size < 0) || (ubx_size > MAX_READ_SIZE))) {
                        is_set_key2 = FALSE;
                    }
                }
                if (is_set_key2 == TRUE) {
                    ubx_count++;
                }
            }
            /* When break was not performed during work_buf analysis */
            if (j == res) {
                work_bufcnt = 0;
            }
            if (ret_flg == TRUE) {
                work_bufcnt = j + 1;
                break;
            }
        }
    }

    /* detect error */

//    if ((i == MAX_READ_SIZE) || ((wp - ret_buf) + 1 > MAX_READ_SIZE)) {
    if ((i == MAX_READ_SIZE) || (*lp_number_of_bytes_read >= MAX_READ_SIZE)) { /* detect error */ /* PastModel008 Fix reset problem */  // NOLINT(whitespace/line_length)
        key = 0;
        ubx_count = 0;
        ubx_size = 0;
        is_set_key = FALSE;
        is_set_key2 = FALSE;
        is_detect_err = TRUE;
    } else {
        if (ret_flg == TRUE) {
            /* PastModel008 Fix reset problem + */
            if (*lp_number_of_bytes_read >= MAX_READ_SIZE) { /* PastModel008 Fix reset problem */
                ret = FALSE;
            } else {
            memcpy(lp_buffer, ret_buf, *lp_number_of_bytes_read);
            }
            /* PastModel008 Fix reset problem - */
        }
    }

#if GPS_FUNC_DEBUG_MSG__CWORD71__FILE
    for (file_loop = 0; file_loop < *lp_number_of_bytes_read; file_loop++) {
        file_buf[file_line_size][line_size++] = *(ret_buf + file_loop);
    }

    line_size_buf[file_line_size] = line_size;
    file_line_size++;

    if (file_line_size >= FILE_OUT_LINE) {
        if (fp == NULL) {
            /* File initialization */
            fp = fopen(POS_GPS_R_LOGFILE_PATH, "w+");
        } else {
            fp = fopen(POS_GPS_R_LOGFILE_PATH, "a+");
        }

        for (file_loop = 0; file_loop < FILE_OUT_LINE; file_loop++) {
            for (file_loop2 = 0; file_loop2 < line_size_buf[file_loop]; file_loop2++) {
                fprintf(fp, "%02x ", file_buf[file_loop][file_loop2]);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
        file_line_size = 0;
    }
#endif /* GPS_FUNC_DEBUG_MSG__CWORD71__FILE */

    return ret;
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

