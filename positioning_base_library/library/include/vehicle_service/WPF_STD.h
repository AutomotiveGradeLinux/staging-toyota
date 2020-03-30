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

#ifndef POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_VEHICLE_SERVICE_WPF_STD_H_
#define POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_VEHICLE_SERVICE_WPF_STD_H_

/**
 * @file WPF_STD.h
 * @brief Header file for system standard definitions
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup positioning_base_library
 *  @ingroup vehicle_service
 *  @{
 */

/*---------------------------------------------------------------------------------*
 * Incluce                                                                         *
 *---------------------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>
#include <wchar.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <native_service/wpf_legacy.h>
#include <vehicle_service/std_types.h>
#include <vehicle_service/pos_message_header.h>
#include <vehicle_service/sys_timerapi.h>
#include <vehicle_service/positioning_positioningbaselibrarylog.h>

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/
#define POS_THREAD_NAME "Positioning"    //!< \~english Positioning thread name

#ifdef  UNICODE
#define __TEXT(quote)     L##quote
#else    /* UNICODE */
#define __TEXT(quote)    (quote)         /* r_winnt */
#endif    /* UNICODE */

#define D_LTEXT(x)  __TEXT(x)
#define LTEXT(x)    D_LTEXT(x)

#define DEBUG_DUMP_MAX_SIZE 4096
//!< \~english FrameworkunifiedOnDebugDump max size

/* Sensor Log Type */
#define POS_SENSLOG_TYPE_NONE 0        //!< \~english sensor log type NONE
#define POS_SENSLOG_TYPE_SYS  1        //!< \~english sensor log type SYS
#define POS_SENSLOG_TYPE_GPS  2        //!< \~english sensor log type GPS
#define POS_SENSLOG_TYPE_NAV  3        //!< \~english sensor log type NAV
#define POS_SENSLOG_TYPE_CMD  4        //!< \~english sensor log type CMD

#define POS_SENSLOG_GET_ZONE(typ) \
    (((typ) == (POS_SENSLOG_TYPE_SYS))?(ZONE_POS_SYS_IN):\
     ((typ) == (POS_SENSLOG_TYPE_GPS))?(ZONE_POS_GPS_IN):\
     ((typ) == (POS_SENSLOG_TYPE_NAV))?(ZONE_POS_NAV_IN):\
     ((typ) == (POS_SENSLOG_TYPE_CMD))?(ZONE_POS_CMD_IN):\
     (0))
//!< \~english get sensor log zone code

#define POS_WRITE_SENSLOG(zone, data, len) (((zone) != (0))?((void)((FRAMEWORKUNIFIEDLOG_DATA(zone, data, len)))):((void)(0)))
//!< \~english write sensor log output
#define POS_SENSLOG(typ, data, len) (POS_WRITE_SENSLOG(POS_SENSLOG_GET_ZONE(typ), data, len))

/* HW configuration type */
/*
 *  Note.
 *  This feature needs to be defined by the vendor.
 */
#define UNIT_TYPE_NONE          0x00000000UL    //!< \~english type none
#define UNIT_TYPE_UNKNOWN       0x00000001UL    //!< \~english type unknown
#define UNIT_TYPE_GRADE1        0x00000002UL    //!< \~english type Grade1
#define UNIT_TYPE_GRADE2        0x00000004UL    //!< \~english type Grade2 

#define MAX_NAME_LEN            32              //!< \~english name max length
#define _CWORD31__NO_SIZE       16              //!< \~english _CWORD31_ No size
#define RESERVE1_SIZE           16              //!< \~english reserve 1
#define HELPNET_ID_SIZE         21              //!< \~english helpnet ID size
#define RESERVE2_SIZE           11              //!< \~english reserve 2
#define DATE_SIZE               32              //!< \~english make time size
#define SERIAL_NO_SIZE          3               //!< \~english serial No size
#define PRODUCT_NAME_SIZE       1               //!< \~english product name size

#define _pb_memcpy                      memcpy
#define _pb_memset                      memset

/*  RET_API define */
#define  RET_NORMAL    0        //!< \~english normal return
#define RET_ERROR   (-1)        //!< \~english error return
#define RET_ERRPARAM  (-2)      //!< \~english parameter error

#define SAPI_EVSET_ABSOLUTE 1   //!< \~english absolute vale set
#define SAPI_EVWAIT_VAL     1   //!< \~english event wait value set

/* sys common commandID definition */
#define    CID_GPS_BASE      (CID)0xF030
//!< \~english GPS

#define    CID_TIMER_BASE    (CID)0xF040
//!< \~english timer management

#define    CID_INITIAL_BASE  (CID)0xF100
//!< \~english initial

/* ResourceManager/CID definition */
#define CID_GPS_SERIAL0   (CID)0x0100
//!< \~english GPS reset request func definition

#define CID_GPS_REQRESET  (CID)(CID_GPS_BASE | CID_GPS_SERIAL0)
//!< \~english GPS reset request CID


/* sys common commandID detail definition */
#define    CID_COM_SERIAL4   (CID)0x0004
//!< \~english serial command ID 4

#define    CID_COM_SERIAL6   (CID)0x0006
//!< \~english serial command ID 6

/* initial CID definition */
#define  CID_INI_SYSTEMERR        (CID)(CID_INITIAL_BASE | CID_COM_SERIAL6)
//!< \~english system abnormality notify

/* timer control CID definition */
#define  CID_TIMER_TOUT        (CID)(CID_TIMER_BASE | CID_COM_SERIAL4)
//!< \~english timeout notify


/* PNO definition */
#define     PNO_NONE                         0x0000  //!< \~english initial value

#define     PNO__CWORD88___CWORD23_                  0x0100
//!< \~english _CWORD23_ primary thread
#define     PNO_GINI_MAIN                    0x0300
//!< \~english G initial
#define     PNO_CLK_GPS                      0x0316
//!< \~english clock GPS thread
#define     PNO_VEHICLE_SENSOR               0x0363
//!< \~english vehicle sensor thread
#define     PNO_LINE_SENS_DRV                0x0364
//!< \~english line sensor driver thread
#define     PNO_NAVI_GPS_MAIN                0x0440
//!< \~english GPS communication manager thread
#define     PNO_NAVI_GPS_RCV                 0x0441
//!< \~english GPS receive thread
#define     PNO__CWORD83_                          0x0900
//!< \~english _CWORD83_(Intersystem Communication Message)thread

#define     SYS_PNO_MIN                      PNO__CWORD88___CWORD23_
//!< \~english minimum PNO.(normal process)
#define     SYS_PNO_MAX                      PNO__CWORD83_
//!< \~english maximum PNO.(normal process)

/*---------------------------------------------------------------------------------*
 * Typedef declaration                                                             *
 *---------------------------------------------------------------------------------*/
typedef u_int32            EventID;
typedef int32              RET_API;   //!< \~english _CWORD64_API return code
typedef u_int32            event_id;
typedef event_id           SemID;     //!< \~english semaphore ID
typedef u_int16            CID;       //!< \~english command ID

typedef uint32_t           UNIT_TYPE;    //!< \~english unit type data type define
typedef int32              ANA_RET_API;

typedef char               TCHAR;

/**
 * @enum _RcvMsgMode
 * \~english  mode of receive message
 */
typedef enum {
    RM_WAIT,       //!< \~english wait until received message
    RM_CHECKRCV    //!< \~english do not wait message, return
} _RcvMsgMode;

/**
 * @enum
 * \~english definition of event reset mode
 */
enum {
    _CWORD64_EVENT_MANUALRESET_OFF = 0,              //!< \~english manual reset off
    _CWORD64_EVENT_MANUALRESET_ON,                   //!< \~english manual reset on
    _CWORD64_EVENT_MANUALRESET_MAX                   //!< \~english manual reset max value
};


/*---------------------------------------------------------------------------------*
 * Struct declaration                                                              *
 *---------------------------------------------------------------------------------*/
/**
 * @struct _SECURITY_ATTRIBUTES
 * \~english security attribute
 */
typedef struct _SECURITY_ATTRIBUTES {
  DWORD nLength;                         //!< \~english length
  LPVOID lpSecurityDescriptor;           //!< \~english data pointer
  BOOL bInheritHandle;                   //!< \~english inherited handle
} SECURITY_ATTRIBUTES;
typedef SECURITY_ATTRIBUTES *PSECURITY_ATTRIBUTES;   //!< \~english security attribute
typedef SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES;  //!< \~english security attribute

/*---------------------------------------------------------------------------------*
 * Prototype Declaration                                                           *
 *---------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Create event by specified name
///
/// \~english @param [in] ucManualReset
///     - u_int8  manual reset mode
/// \~english @param [in] lInitData
///     - int32  event value
/// \~english @param [in] cpEventName
///     - char*  event name
///
/// \~english @par
///     - manual reset mode(ucManualReset)
///       - _CWORD64_EVENT_MANUALRESET_OFF  manual reset OFF
///       - _CWORD64_EVENT_MANUALRESET_ON  manual reset ON
///
/// \~english @retval 0    event create failed
/// \~english @retval not 0 created event ID
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter cpEventName is NULL
///     - The parameter ucManualReset is more than or equal _CWORD64_EVENT_MANUALRESET_MAX
///     - The string of parameter cpEventName has no text
///     - The string length of parameter cpEventName is longer than MAX_EVENT_NAME_LEN
///     - The event is created in same process, but the count of thread reference to the event is reach to the max
///     - The event is created in system, but the count of process reference to the event is reach to the max
///     - Memory allocate failed for creating event table
///     - During create the event table, the event flag register failed
///     - The event table is full
///
/// \~english @par Detail
///     - Call this API to create event and return the created event ID.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
EventID _pb_CreateEvent(u_int8 ucManualReset, int32 lInitData, char *cpEventName);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Wait for the event specified by ID
///
/// \~english @param [in] eventID
///     - EventID  event ID
/// \~english @param [in] lWaitMode
///     - int32  event wait mode
/// \~english @param [in] lMinVal
///     - int32  event wait min value
/// \~english @param [in] lMaxVal
///     - int32  event wait max value
/// \~english @param [out] plEventVal
///     - int32*  event wait value output pointer
/// \~english @param [in] ulMillSecTime
///     - u_int32  timeout time(ms)
///
/// \~english @par
///     - event wait mode(lWaitMode)
///       - SAPI_EVWAIT_VAL
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_EV_NONE parameter error or specified event not exist
/// \~english @retval RET_ERRTIMEOUT timeout
/// \~english @retval RET_ERROR   other error
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter plEventVal is NULL
///     - The parameter eventID is more than MAX_PB_EVENTS
///     - The specified event has not been registered in event table
///     - The count of threads which is waiting this event is reach max
///     - The parameter ulMillSecTime is 0
///     - The parameter ulMillSecTime is INFINITE, and wait event flag failed
///     - The parameter ulMillSecTime is not INFINITE, and timeout
///     - The parameter ulMillSecTime is not INFINITE, and get event flag failed
///
/// \~english @par Detail
///     - Call this API to wait event.
///     - This API will return after timeout or this event been set.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_WaitEvent(EventID eventID, int32 lWaitMode, int32 lMinVal,
                        int32 lMaxVal, int32* plEventVal, u_int32 ulMillSecTime);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Set the value of event specified by ID
///
/// \~english @param [in] eventID
///     - EventID  event ID
/// \~english @param [in] lSetMode
///     - int32  event value set mode
/// \~english @param [in] iVal
///     - int32  event value
///
/// \~english @par
///     - event value set mode(lSetMode)
///       - SAPI_EVSET_ABSOLUTE  absolute value set(set parameter value)
///       - SAPI_EVSET_RELATE  offset set(offset of current value set)
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_ERRPARAM  parameter error
/// \~english @retval RET_EV_NONE specified event not exist
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter eventID is more than MAX_PB_EVENTS
///     - The specified event has not been registered in event table
///     - The parameter lSetMode is neither SAPI_EVSET_ABSOLUTE nor SAPI_EVSET_RELATE
///
/// \~english @par Detail
///     - Call this API to set event value.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_SetEvent(EventID eventID, int32 lSetMode, int32 lval);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Destroy the event specified by ID
///
/// \~english @param [in] eventID
///     - EventID  event ID
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_ERROR  event flag destroy failed
/// \~english @retval RET_EV_NONE specified event not exist
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///     (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter eventID is more than MAX_PB_EVENTS
///     - The specified event has not been registered in event table
///     - Event flag destroy failed
///
/// \~english @par Detail
///     - Call this API to destroy specified event.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_DeleteEvent(EventID eventID);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Initialization before call Base API(every process used Base API should call this API firstly).
///
/// \~english @param [in] hApp
///     - HANDLE  app Handle
///
/// \~english @retval RET_NORMAL normal return
///
/// \~english @par Precondition
///     - The creation/initialization(FrameworkunifiedCreateDispatcherWithoutLoop and etc.) of the dispatcher for App are completed.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to initialize Base API.
///     - Every process used Base API should call this API firstly.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_Setup_CWORD64_API(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Terminate current thread.
///
/// \~english @param [in] dwExitCode
///     - DWORD  exit code(not used)
///
/// \~english @retval None
///
/// \~english @par Precondition
///     - None
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to terminate current thread.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
VOID  _pb_ExitThread(DWORD dwExitCode);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Create the message queue
///
/// \~english @param [in] eventID
///     - EventID  process No
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_ERRPARAM  parameter error
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///        (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter pno is 0
///     - The name length of thread called this API is longer than MAX_QUEUE_NAME_SIZE
///
/// \~english @par Detail
///     - Call this API to create the message queue.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_CreateMsg(PNO pno);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Send message in process
///
/// \~english @param [in] pno
///     - PNO  process No
/// \~english @param [in] size
///     - u_int16  message size
/// \~english @param [in] msgbuf
///     - void*  message buffer
/// \~english @param [in] mode
///     - u_int16  not used
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_ERRPARAM  parameter error
/// \~english @retval RET_ERROR  other error
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter msgbuf is NULL
///     - The parameter pno is 0
///     - The message queue specified by parameter pno has not been created
///     - Get sender Handle failed
///     - Send message failed
///
/// \~english @par Detail
///     - Call this API to send message in process.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_SndMsg(PNO pno, u_int16 size, void* msgbuf, u_int16 mode);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Send message between process
///
/// \~english @param [in] name
///     - PCSTR  destination process name
/// \~english @param [in] cid
///     - CID  command ID
/// \~english @param [in] size
///     - u_int16  message size
/// \~english @param [in] msgbuf
///     - const void*  message buffer
/// \~english @param [in] mode
///     - u_int16  not used
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_ERRPARAM  parameter error
/// \~english @retval RET_ERROR  other error
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter name is NULL
///     - The parameter msgbuf is NULL
///     - The length of parameter name is longer than MAX_QUEUE_NAME_SIZE
///     - Get sender Handle failed
///     - Send message failed
///
/// \~english @par Detail
///     - Call this API to send message between process.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_SndMsg_Ext(PCSTR name, CID cid, u_int16 size, const void* msgbuf, u_int16 mode);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Create semaphore by specified name
///
/// \~english @param [in] semName
///     - char*  semaphore name
///
/// \~english @retval 0    semaphore create failed
/// \~english @retval not 0 created semaphore ID
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter semName is NULL
///     - The string of parameter semName has no text
///     - The string length of parameter semName is longer than MAX_SEMAPHORE_NAME_LEN
///     - Memory allocate failed for creating semaphore table
///     - The semaphore table is full
///
/// \~english @par Detail
///     - Call this API to create semaphore and return the created semaphore ID.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
SemID   _pb_CreateSemaphore(char* semName);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Semaphore lock
///
/// \~english @param [in] semID
///     - SemID  semaphore ID
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_OSERROR  error return
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///      (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter semName is 0
///     - The parameter semName is more than MAX_PB_SEMAPHORES
///     - The specified semaphore has not been registered in semaphore table
///     - The specified semaphore's mutex has not been created
///     - The specified semaphore's mutex lock failed
///
/// \~english @par Detail
///     - Call this API to get semaphore.
///     - This API will return after got semaphore.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_SemLock(SemID semID);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Semaphore unlock
///
/// \~english @param [in] semID
///     - SemID  semaphore ID
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_OSERROR  error return
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter semName is 0
///     - The parameter semName is more than MAX_PB_SEMAPHORES
///     - The specified semaphore has not been registered in semaphore table
///     - The specified semaphore's mutex has not been created
///     - The specified semaphore's mutex release failed
///
/// \~english @par Detail
///     - Call this API to relesae semaphore.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_SemUnlock(SemID semID);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Create Share Data
///
/// \~english @param [in] area_name
///     - char*  share data name
/// \~english @param [in] size
///     - u_int32  data size
/// \~english @param [out] mem_ptr
///     - void**  share memory pointer
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_ERROR  error return
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter area_name is NULL
///     - The parameter mem_ptr is NULL
///     - The string of parameter area_name has no text
///     - The string length of parameter area_name is longer than MAX_AREA_NAME_LEN
///     - The parameter size is 0
///     - The specified share data has already been created
///     - Memory allocate failed for creating share data
///     - The specified share data file create failed
///
/// \~english @par Detail
///     - Call this API to create share data.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_CreateShareData(char* area_name, u_int32 size, void** mem_ptr);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Link Share Data
///
/// \~english @param [in] area_name
///     - char*  share data name
/// \~english @param [out] mem_ptr
///     - void**  share memory pointer
/// \~english @param [in] size
///     - u_int32*  data size
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_ERROR  error return
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter area_name is NULL
///     - The parameter mem_ptr is NULL
///     - The string of parameter area_name has no text
///     - The string length of parameter area_name is longer than MAX_AREA_NAME_LEN
///     - The parameter size is NULL
///     - Memory allocate failed for linking share data
///     - The specified share data file open failed
///
/// \~english @par Detail
///     - Call this API to link share data.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_LinkShareData(char* area_name, void** mem_ptr, u_int32* size);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Start timer
///
/// \~english @param [in] SndPno
///     - PNO  source process No
/// \~english @param [in] TimerSeq
///     - u_int16  timer sequence No
/// \~english @param [in] TimerType
///     - u_int8  timer type
/// \~english @param [in] TimeOut
///     - u_int32  timeout time
///
/// \~english @par
///     - timer type(TimerType)
///       - TIMER_TYPE_SYN  fixed term timer
///       - TIMER_TYPE_USN  asynchronous timer
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_ERRPARAM  parameter error
/// \~english @retval RET_ERROR  other error
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - Initialization of positioning_base_library has not been done in current process
///     - The parameter TimerType is neither TIMER_TYPE_SYN nor TIMER_TYPE_USN
///     - The parameter TimeOut is 0
///     - The same timer has already been started
///     - Timer start failed
///
/// \~english @par Detail
///     - Call this API to start timer.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_ReqTimerStart(PNO SndPno, u_int16 TimerSeq, u_int8 TimeType, u_int32 TimeOut);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Stop timer
///
/// \~english @param [in] SndPno
///     - PNO  source process No
/// \~english @param [in] TimerSeq
///     - u_int16  timer sequence No
/// \~english @param [in] TimerType
///     - u_int8  timer type
///
/// \~english @par
///     - timer type(TimerType)
///       - TIMER_TYPE_SYN  fixed term timer
///       - TIMER_TYPE_USN  asynchronous timer
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_ERROR  error return
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - Initialization of positioning_base_library has not been done in current process
///     - Timer stop failed
///
/// \~english @par Detail
///     - Call this API to stop timer.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_TimerStop(PNO SndPno, u_int16 TimerSeq, u_int8 TimeType);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Create mutex
///
/// \~english @param [in] lpMutexAttributes
///     - LPSECURITY_ATTRIBUTES  not used
/// \~english @param [in] bInitialOwner
///     - BOOL  not used
/// \~english @param [in] lpName
///     - LPCTSTR  mutex name
///
/// \~english @retval NULL  error return
/// \~english @retval Except NULL  created mutex Handle
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter lpName is NULL
///     - The string length of pamareter lpName is longer than NAME_MAX
///     - Mutex lock map failed
///
/// \~english @par Detail
///     - Call this API to create mutex.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE _pb_CreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCTSTR lpName);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Get message management table dump info.
///
/// \~english @param [out] pBuf
///     - void*  output buffer
/// \~english @param [in/out] pLen
///     - uint8_t*  output buffer size
///
/// \~english @retval None
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to get message management table dump info.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
void _pb_GetDebugMsgMngTbl(void* pBuf, uint8_t* pLen);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Get mutex management table dump info.
///
/// \~english @param [out] pBuf
///     - void*  output buffer
/// \~english @param [in/out] pLen
///     - uint8_t*  output buffer size
///
/// \~english @retval None
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to get mutex management table dump info.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
void _pb_GetDebugMutexMngTbl(void* pBuf, uint8_t* pLen);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Get timer management table dump info.
///
/// \~english @param [out] pBuf
///     - void*  output buffer
///
/// \~english @retval None
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to get timer management table dump info.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
void _pb_GetDebugTimerMngTbl(void* pBuf);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Get event management table dump info.
///
/// \~english @param [out] pBuf
///     - void*  output buffer
/// \~english @param [in/out] pLen
///     - uint8_t*  output buffer size
///
/// \~english @retval None
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to get event management table dump info.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
void _pb_GetDebugEventMngTbl(void* pBuf, uint8_t* pLen);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Get memory management table dump info.
///
/// \~english @param [out] pBuf
///     - void*  output buffer
///
/// \~english @retval None
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to get memory management table dump info.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
void _pb_GetDebugMemoryMngTbl(void* pBuf);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Get common info management table dump info.
///
/// \~english @param [out] pBuf
///     - void*  output buffer
///
/// \~english @retval None
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to get common info management table dump info.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
void _pb_GetDebugOtherMngTbl(void* pBuf);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Get current app Handle.
///
/// \~english @param None
///
/// \~english @retval 0    get Handle failed
/// \~english @retval not 0 Handle
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - Current app Handle has not been registered
///
/// \~english @par Detail
///     - Call this API to get current app Handle.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE  _pb_GetAppHandle(void);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Set current app Handle.
///
/// \~english @param [in] hApp
///     - HANDLE  app Handle
///
/// \~english @retval None
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to get common info management table dump info.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
void  _pb_SetAppHandle(HANDLE hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Base API stop process(every process used Base API should call this API before stop).
///
/// \~english @param None
///
/// \~english @retval None
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to stop Base API.
///     - Every process used Base API should call this API before stop.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
void  _pb_Teardown_CWORD64_API(void);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Terminate current process.
///
/// \~english @param [in] pFunc
///     - char*  source function
/// \~english @param [in] line
///     - int  source line
///
/// \~english @retval None
///
/// \~english @par Precondition
///     - None
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to terminate current process.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
void  _pb_Exit_d(const char* pFunc, int line);
#define _pb_Exit() _pb_Exit_d(__func__, __LINE__)

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Receive message.
///
/// \~english @param [in] pno
///     - PNO  process No
/// \~english @param [in] size
///     - u_int16  message size
/// \~english @param [out] msgbuf
///     - void*  message buffer
/// \~english @param [in] mode
///     - u_int16  not used
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_ERRPARAM  parameter error
/// \~english @retval RET_ERROR  other error
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///       (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter msgbuf is NULL
///     - The parameter pno is 0
///     - The message queue specified by parameter pno has not been created
///     - Receive message failed
///     - Receive message successrully, but the received message size is longer than parameter size
///     - Get received message data failed
///
/// \~english @par Detail
///     - Call this API to receive message of specified pno.
///     - This API will return after received message.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_RcvMsg(PNO pno, u_int16 size, void** msgbuf, u_int16 mode);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Get send buffer for message transferring in process
///
/// \~english @param [in] pno
///     - PNO  process No
/// \~english @param [out] pSndBuf
///     - void**  send buffer
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_ERRPARAM  parameter error
/// \~english @retval RET_ERROR  other error
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
/// (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter pno is 0
///     - The message queue specified by parameter pno has not been created
///     - Get sender Handle failed
///     - Get send buffer failed
///
/// \~english @par Detail
///     - Call this API to get send buffer for message transferring in process.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_GetZcSndBuf(PNO pno, void** pSndBuf);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Send message in process
///
/// \~english @param [in] pno
///     - PNO  process No
/// \~english @param [in] size
///     - u_int16  message size
/// \~english @param [in] mode
///     - u_int16  not used
///
/// \~english @retval RET_NORMAL  normal return
/// \~english @retval RET_ERRPARAM  parameter error
/// \~english @retval RET_ERROR  other error
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
/// (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter pno is 0
///     - The message queue specified by parameter pno has not been created
///     - Get sender Handle failed
///     - Get send buffer failed
///     - Set send buffer failed
///     - Send message failed
///
/// \~english @par Detail
///     - Call this API to send message in process.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
RET_API _pb_ZcSndMsg(PNO pno, u_int16 size, u_int16 mode);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Convert process name to pno
///
/// \~english @param [in] name
///     - PCSTR  process name
///
/// \~english @retval 0    convert failed
/// \~english @retval not 0 converted PNO
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///  (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter name is NULL
///     - The size of parameter name is longer than THREAD_NAME_LEN_MAX
///
/// \~english @par Detail
///     - Call this API to convert process name to pno.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
PNO _pb_CnvName2Pno(PCSTR name);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Convert pno to process name
///
/// \~english @param [in] pno
///     - PNO  process No
///
/// \~english @retval NULL    convert failed
/// \~english @retval not NULL converted process name
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///  (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Failure condition
///     - The parameter pno has not been registered(not the return pno of _pb_CnvName2Pno).
///
/// \~english @par Detail
///     - Call this API to convert pno to process name.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Sync
///
////////////////////////////////////////////////////////////////////////////////////////////
PCSTR _pb_CnvPno2Name(PNO pno);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Get message resource
///
/// \~english @param None
///
/// \~english @retval TRUE    normal return
/// \~english @retval FALSE error(lack of resource)
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///  (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to get message resource.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
BOOL _pb_GetMsgResource(void);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Get mutex resource
///
/// \~english @param None
///
/// \~english @retval TRUE    normal return
/// \~english @retval FALSE error(lack of resource)
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///  (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to get mutex resource.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
BOOL _pb_GetMutexResource(void);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Get common resource
///
/// \~english @param None
///
/// \~english @retval TRUE    normal return
/// \~english @retval FALSE error(lack of resource)
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
/// (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to get common resource.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
BOOL _pb_GetOtherResource(void);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Release message resource
///
/// \~english @param None
///
/// \~english @retval None
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///  (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to release message resource.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
void _pb_ReleaseMsgResource(void);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Release mutex resource
///
/// \~english @param None
///
/// \~english @retval None
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///  (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to release mutex resource.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
void _pb_ReleaseMutexResource(void);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_base_library
/// \~english @par Brief
///     - Release common resource
///
/// \~english @param None
///
/// \~english @retval None
///
/// \~english @par Precondition
///     - positioning_base_library initialization are completed \n
///  (_pb_Setup_CWORD64_API has already been called) in current process.
///
/// \~english @par changes of the internal status
///     - There is no changes of the internal status by this API
///
/// \~english @par Detail
///     - Call this API to release common resource.
///        \n
///
/// \~english @par Classification
///        - Public
///
/// \~english @par Type
///        - Fire and Forget
///
////////////////////////////////////////////////////////////////////////////////////////////
void _pb_ReleaseOtherResource(void);

#ifdef __cplusplus
}
#endif

/** @}*/  // end of positioning_base_library
/** @}*/  // end of vehicle_service
/** @}*/  // end of BaseSystem
#endif  // POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_VEHICLE_SERVICE_WPF_STD_H_
