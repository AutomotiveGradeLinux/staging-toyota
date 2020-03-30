/*
 * @copyright Copyright (c) 2019 TOYOTA MOTOR CORPORATION.
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
 * @file WPF_STD_private.h
 * @brief Header file for system standard definitions
 */

#ifndef POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_WPF_STD_PRIVATE_H_
#define POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_WPF_STD_PRIVATE_H_

/*---------------------------------------------------------------------------------*
 * Incluce                                                                         *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>

/* _CWORD64_ non-OS managed areas */
#define SDRAM__CWORD64__NOTOS_AREA 0x00000000
#define SDRAM__CWORD64__NOTOS_AREA_SIZE 0x0

#define BLVER_SIZE           0x00000008    /* BootLoader version information area size */
#define FROM_FIX_VER_SIZE    BLVER_SIZE    /* Mask(BootLoder)Version size    */

/* Start address of the _CWORD64_ non-OS management area */
#define TOP_ADDR_NOTOS      SDRAM__CWORD64__NOTOS_AREA
/* Size of the non-OS management area  */
#define NOTOS_SISE            SDRAM__CWORD64__NOTOS_AREA_SIZE

/* Reserved(32KB)Unavailable */
#define SRAMDRAM_RESERVE2_SIZE    0x00008000

#define WINAPI

#define STATUS_WAIT_0              ((DWORD)0x00000000L)
#define STATUS_ABANDONED_WAIT_0 ((DWORD)0x00000080L)
#define STATUS_TIMEOUT             ((DWORD)0x00000102L)
#define WAIT_TIMEOUT            STATUS_TIMEOUT
#define WAIT_OBJECT_0            ((STATUS_WAIT_0) + 0)
#define WAIT_ABANDONED            ((STATUS_ABANDONED_WAIT_0) + 0)
#define WAIT_FAILED                ((DWORD)0xFFFFFFFF)

#define SHARED_MEMORY_DIRECTORY        ("/tmp/shm/navigation/")

#define _pb_strcmp(pleft, pright)((strcmp(pleft, pright) == 0) ?(0) :(-1))

/* For heap allocation method control */
#define HEAP_ZERO_MEMORY            (0x00000008)

#define EV_RXCHAR           0x0001
#define EV_ERROR              0x0080

#define EV_DSR              0x0010

/* Systems API External Public Functions Prototype Definitions */
#define VUP_DISC_PATHNAME           _T("/Z01")                      /* VUP DISC paths */
#define VUP_USB_PATHNAME            _T("/U")                        /* VUP USB paths */
#define VUP_PATHNAME_SYSTEM         _T("/agl/system")               /* Storage path name */
#define VUP_FNAME_TOP               _T("/NA_D0_")                   /* VUPFile name(front) */
#define VUP_FNAME_BTM               _T(".bin")                      /* VUPFile name(rear) */
#define VUP_FNAME_FORCEVUP          _T("/forcevup.dat")             /* Forced VUP file name */

#define FROM_VARI_VER_SIZE  8
#define WRITE_FLAG_ON       0x01        /* Write permission status for the file */
#define FSNOACCESS          0x00        /* File system access disabled state */
#define VUP_GETNAME_MEDIA_DISC      0x01        /* VUP media:disc */

/* Function Code(Normal completion) */
#define RET_RCVMSG    1   /* Receive only message      */
#define RET_RCVSIG    2   /* Receive only signal         */
#define RET_RCVBOTH   3   /* Receive both messages and signals   */
#define RET_NOMSG   4   /* No received message */
/* User-side signal handling return values */
#define RET_QUE     5   /* Queue a signal */
#define RET_NOTQUE    6   /* Do not queue signals */
#define RET_PROCDOWN  7   /* Detect DOWN of children */
#define RET_SLEEP 100

/* Error code system  */
#define RET_OSERROR   (-127)  /* OS System call error  */
/* Actual error in _sys_GetOSErrorCode() */
/* Code can be retrieved     */
#define RET_ERRNOTRDY (-3)  /* Specified port ID and semaphore ID are not created.*/
#define RET_ERRPROC   (-4)  /* Error in API processing      */
#define RET_ERRTIMEOUT  (-5)  /* Timeout with Processing Not Complete  */
/*     (D type API only)   */
#define RET_ERRMSGFULL  (-6)  /* Message table full */
#define RET_ERROVERFLW  (-7)  /* Receive message is larger than the receive buffer size */
#define RET_ERRINIT   (-8)
#define RET_ERRREGQUE (-9)

#define RET_EV_NONE   (-20) /* No specified event */
#define RET_EV_MAX    (-21) /* Event value exceeds the specified maximum value */
#define RET_EV_MIN    (-22) /* The event value is less than the specified minimum value. */

#define SAPI_EVSET_RELATE   2


/*---------------------------------------------------------------------------------*
 * Typedef declaration                                                             *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Struct declaration                                                              *
 *---------------------------------------------------------------------------------*/
typedef struct FileTime {
    DWORD dw_low_date_time;
    DWORD dw_high_date_time;
} FILETIME;
typedef FILETIME *PFILETIME;
typedef FILETIME *LPFILETIME;

typedef struct Overlapped {
    ULONG_PTR internal;
    ULONG_PTR internal_high;
    union {
        struct {
            DWORD offset;
            DWORD offset_high;
        };

        PVOID pointer;
    };

    HANDLE  h_event;
} OVERLAPPED;
typedef OVERLAPPED *LPOVERLAPPED;

typedef struct ComStat {
    DWORD f_cts_hold : 1;
    DWORD f_dsr_hold : 1;
    DWORD f_rlsd_hold : 1;
    DWORD f_x_off_hold : 1;
    DWORD f_x_off_sent : 1;
    DWORD f_eof : 1;
    DWORD f_txim : 1;
    DWORD f_reserved : 25;
    DWORD cb_in_que;
    DWORD cb_out_que;
} COMSTAT;
typedef COMSTAT *LPCOMSTAT;

typedef struct Dcb {
    DWORD dcb_length;      /* sizeof(DCB)                     */
    DWORD baud_rate;       /* Baudrate at which running       */
    DWORD f_binary: 1;     /* Binary Mode (skip EOF check)    */
    DWORD f_parity: 1;     /* Enable parity checking          */
    DWORD f_out_x_cts_flow: 1; /* CTS handshaking on output       */
    DWORD f_out_x_dsr_flow: 1; /* DSR handshaking on output       */
    DWORD f_dtr_control: 2; /* DTR Flow control                */
    DWORD f_dsr_sensitivity: 1; /* DSR Sensitivity              */
    DWORD f_tx_continue_on_x_off: 1; /* Continue TX when Xoff sent */
    DWORD f_out_x: 1;       /* Enable output X-ON/X-OFF        */
    DWORD f_in_x: 1;        /* Enable input X-ON/X-OFF         */
    DWORD f_error_char: 1;  /* Enable Err Replacement          */
    DWORD f_null: 1;       /* Enable Null stripping           */
    DWORD f_rts_control: 2; /* Rts Flow control                */
    DWORD f_abort_on_error: 1; /* Abort all reads and writes on Error */
    DWORD f_dummy2: 17;    /* Reserved                        */
    WORD w_reserved;       /* Not currently used              */
    WORD x_on_lim;          /* Transmit X-ON threshold         */
    WORD x_off_lim;         /* Transmit X-OFF threshold        */
    BYTE byte_size;        /* Number of bits/byte, 4-8        */
    BYTE parity;          /* 0-4=None,Odd,Even,Mark,Space    */
    BYTE stop_bits;        /* 0,1,2 = 1, 1.5, 2               */
    char x_on_char;         /* Tx and Rx X-ON character        */
    char x_off_char;        /* Tx and Rx X-OFF character       */
    char error_char;       /* Error replacement char          */
    char eof_char;         /* End of Input character          */
    char evt_char;         /* Received Event character        */
    WORD w_reserved1;      /* Fill for now.                   */
} DCB;
typedef DCB *LPDCB;

typedef struct CommTimeouts {
    DWORD read_interval_timeout;          /* Maximum time between read chars. */
    DWORD read_total_timeout_multiplier;   /* Multiplier of characters.        */
    DWORD read_total_timeout_constant;     /* Constant in milliseconds.        */
    DWORD write_total_timeout_multiplier;  /* Multiplier of characters.        */
    DWORD write_total_timeout_constant;    /* Constant in milliseconds.        */
} COMMTIMEOUTS;
typedef COMMTIMEOUTS *LPCOMMTIMEOUTS;

typedef struct {
    DWORD            size;                            /* Allocated shared memory size                */
    DWORD            phy_addr;                        /* Physical address                            */
    HANDLE           h_map;                            /*  +8: Handle of file mapping object.    */
    void*            p_memory;                        /* Shared Memory Mapped Address            */
    DWORD            owner;                            /* +16: Owner flag.                        */
    HANDLE           h_heap;                            /* +20: Handle of heap for oneself.        */
} SHARED_MEMORY;

typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(
    LPVOID lpThreadParameter
);
typedef PTHREAD_START_ROUTINE     LPTHREAD_START_ROUTINE;

/* Program version storage */
typedef struct {
    u_int8  mask_ver[ FROM_FIX_VER_SIZE ];      /* BootLoader programming versions */
    u_int8  flash_ver1[ FROM_VARI_VER_SIZE ];   /* NK1 programming versions */
    u_int8  flash_ver2[ FROM_VARI_VER_SIZE ];   /* NK2 programming versions */
    u_int8  flash_ver3[ FROM_VARI_VER_SIZE ];   /* NK3 programming versions */
} T_SYS_PRGVER;

/* Device information storage */
typedef struct {
    u_int8  _CWORD31__no[ _CWORD31__NO_SIZE ];        /* _CWORD31_ product number */
    u_int8  reserve1[ RESERVE1_SIZE ];          /* RESERVE1 */
    u_int8  helpnet_id[ HELPNET_ID_SIZE ];      /* HELPNET ID */
    u_int8  reserve2[ RESERVE2_SIZE ];          /* RESERVE2 */
    u_int8  date[ DATE_SIZE ];                  /* Manufacturing date and time */
} T_SYS_DEVINFO;

/* Structure for storing display information */
typedef struct {
    int     i_width_size;                 /* Width */
    int     i_height_size;                /* Height */
} DISPLAY_INFO;

/* System error notification message structure */
typedef struct {
    T_APIMSG_MSGBUF_HEADER bhead;   /* Message buffer header */
    PNO             pno;            /* Process No.  */
    u_int8          dummy[2];       /* Dummy       */
    int32           errcode;        /* Error code */
} T_SYS_SYSTEMERROR;


/*---------------------------------------------------------------------------------*
 * Prototype Declaration                                                           *
 *---------------------------------------------------------------------------------*/
UNIT_TYPE PbGetEnvSupportInfo(void);
/* Configuration Functions  (_pbCommon.cpp)  */
void    PbReset(void);                                      /* Soft reset process */

/* Message Sending/Receiving Functions   (_pbMsg.cpp)   */
RET_API PbDeleteMsg(PNO pno);

/* SRAM Control Functions (_pbSram.cpp)   */
RET_API PbSramFil32(u_int8 id, u_int32 off, u_int8 pat, u_int32 size);
RET_API PbSramRd32(u_int8 id, u_int32 off, void *pbuf, u_int32 size);
RET_API PbSramSz32(u_int8 id, u_int32 *psize);

/* DRAM Control Functions (_pbDram.cpp)  */
RET_API PbDramWt32(u_int8 id, void *pbuf, u_int32 off, u_int32 size);

/* Semaphore(_pbSem.cpp) */
RET_API PbDeleteSemaphore(SemID sem_id);
DWORD   PbDeleteMutex(HANDLE h_mutex);
DWORD   PbMutexLock(HANDLE h_mutex, DWORD timeout);
BOOL    PbMutexUnlock(HANDLE h_mutex);

/* Shared Memory Access Functions(_pbMem.cpp) */
RET_API PbDeleteShareData(char* area_name);
/* Accessing Physical Memory Area Allocation Data */
RET_API PbAccessPhysicalMem(u_int32 addr, void **mem_ptr, u_int32 size, u_int32 mode);
/* Releases access to the physical memory area allocation data. */
RET_API PbFreePhysicalMem(u_int32 addr, void *mem_ptr, u_int32 size);

LPVOID  PbProcessHeapAlloc(DWORD dw_flags, SIZE_T dw_bytes);    /* Allocating Memory from the Process Heap            */
BOOL    PbProcessHeapFree(DWORD dw_flags, LPVOID lp_mem);       /* Free Memory Allocated from Process Heap      */

/* Functions Related to Analysis Support Functions(_pbOther.cpp) */
uint32_t PbGetTid(void);
uint32_t PbGetLocalTid(void);

/* Other Functions(_pbMisc.cpp) */
RET_API PbMilliSecSleep(u_int32 ul_mill_time);                  /* Sleeping of the calling process */

/* TODO As defined in _pbCSection--> */
VOID PbDeleteCriticalSection(LPCRITICAL_SECTION lp_critical_section);
VOID PbEnterCriticalSection(LPCRITICAL_SECTION lp_critical_section);
VOID PbInitializeCriticalSection(LPCRITICAL_SECTION lp_critical_section);
VOID PbLeaveCriticalSection(LPCRITICAL_SECTION lp_critical_section);

BOOL PbSystemTimeToFileTime(CONST SYSTEMTIME *lp_system_time, LPFILETIME lp_file_time);


/* Memory copy Bus release support */
void*  Memset64pSync(void* dest, int c, size_t count);

#endif  // POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_WPF_STD_PRIVATE_H_

