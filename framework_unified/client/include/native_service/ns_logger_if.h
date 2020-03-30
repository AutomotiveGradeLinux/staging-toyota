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


//////////////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// Declares the external APIs to Logger.
///
//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @brief \~english Declares the external APIs to Logger.
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup Native_service
 *  @{
 */
/** @addtogroup native
 *  @ingroup framework_unified
 *  @{
 */
#ifndef  __NATIVESERVICES_LOGGER_IF_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_LOGGER_IF_H__

#include <native_service/frameworkunified_types.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FRAMEWORKUNIFIEDLOG_MSGQ_NAME "/NSLog"
#define FRAMEWORKUNIFIEDLOG_SHAREDMEM_NAME "/frameworkunified_debug.log.1"
#define FRAMEWORKUNIFIEDLOG_SHAREDMEM_SIZE 5242880  // 5MB

#define MASTER_ZONE_COUNT  16U

#include <native_service/loglibrarylog_if.h>

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    typedef of ZONE
///////////////////////////////////////////////////////////////////////////////
typedef UI_32 TFrameworkunifiedZone;
// Note: if the BITS_IN_TZONE macro is getting modified do the corresponding changes
// in GET_ZONE_INDEX and GET_ZONE_BIT_MASK
#define BITS_IN_BYTE (8U)
#define BITS_IN_TZONE ((UI_32 )(sizeof( TFrameworkunifiedZone ) * BITS_IN_BYTE))
#define TZONE_COUNT ((UI_32)MASTER_ZONE_COUNT)
#define BITS_IN_ZONE_MASK ((UI_32)(BITS_IN_TZONE * TZONE_COUNT))
#define DEBUG_ZONE_COUNT  BITS_IN_ZONE_MASK

#define ZONE_MASK_ARRAY_ELTS ((UI_32)TZONE_COUNT)

#define USER_ZONES_COUNT BITS_IN_ZONE_MASK -10

#define ZONE_TEXT_SIZE  24

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    Array of mask options
///////////////////////////////////////////////////////////////////////////////
typedef TFrameworkunifiedZone TFrameworkunifiedZoneMask[ZONE_MASK_ARRAY_ELTS];

typedef enum _TFrameworkunifiedLoggerSeverity {
  FRAMEWORKUNIFIED_SEVERITY_LOW  = 0x0,  /// <all enable(default)
  FRAMEWORKUNIFIED_SEVERITY_DEBUG2,    /// <enable FRAMEWORKUNIFIEDLOG_DEBUG() and FRAMEWORKUNIFIEDLOG_DEBUG2() and all the follows
  FRAMEWORKUNIFIED_SEVERITY_DEBUG1,    /// <enable FRAMEWORKUNIFIEDLOG_DEBUG1() and all the follows
  FRAMEWORKUNIFIED_SEVERITY_INFO,      /// <enable FRAMEWORKUNIFIEDLOG_INFO() and all the follows
  FRAMEWORKUNIFIED_SEVERITY_WARN,      /// <enable FRAMEWORKUNIFIEDLOG_WARN() and all the follows
  FRAMEWORKUNIFIED_SEVERITY_ERROR,     /// <enable FRAMEWORKUNIFIEDLOG_ERROR() and all the follows
  FRAMEWORKUNIFIED_SEVERITY_FATAL,     /// <enable FRAMEWORKUNIFIEDLOG_FATAL() and FRAMEWORKUNIFIEDLOG_ALWAYS()
  FRAMEWORKUNIFIED_SEVERITY_ALWAYS     /// <enable FRAMEWORKUNIFIEDLOG_ALWAYS()
} TFrameworkunifiedLoggerSeverity;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    FRAMEWORKUNIFIEDLOG parameter
///////////////////////////////////////////////////////////////////////////////
typedef struct _FRAMEWORKUNIFIEDLOGPARAM {
  UI_8           uiLogOptions;                 /// <output options (ref. NS_LoggingMethod_t )
  CHAR           cZones[USER_ZONES_COUNT][ZONE_TEXT_SIZE];   /// <mask text    ZONE_TEXT_10 ... ZONE_TEXT_n
  TFrameworkunifiedZoneMask     uiZoneMask;                 /// <mask options ZONEMASK(10)...ZONEMASK(n) (ref. TFrameworkunifiedZone)
  TFrameworkunifiedLoggerSeverity eSeverity;                  /// <severity level(ref. TFrameworkunifiedLoggerSeverity)
} FRAMEWORKUNIFIEDLOGPARAM, *LPFRAMEWORKUNIFIEDLOGPARAM;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    FRAMEWORKUNIFIEDLOG output direction
/// \note     LMSGQ and LSLOGGER are disabled
///////////////////////////////////////////////////////////////////////////////
typedef enum _NS_LOGGER_METHODS {
  LPRINT    = 0x01,   /// <Print to the console(similar to printf)
  LMSGQ     = 0x02,   /// <Disabled
  LSLOGGER  = 0x04,   /// <Disabled
  LSHAREDMEM  = 0x08    /// <Output to shared memory. this is the default logging method.
} NS_LoggingMethod_t;

#pragma pack(1)        // *** DO NOT REMOVE ***

typedef struct tag_Change_Log_Parameters {
  TFrameworkunifiedZoneMask     uiZoneMask;
  UI_8           uiLogOptions;
  TFrameworkunifiedLoggerSeverity eSeverity;
} CHANGELOGPARAMS;

#pragma pack(0)        // *** DO NOT REMOVE ***

// RCS related
#define NS_RCS_LOGGER_PLUGIN_Q  "/NsRcsLoggerPlugin"

typedef enum _NS_RCS_LOGGER_SETTINGS {
  NS_RCS_LOGGER_PLUGIN_ADD_APPNAME = 0x500,
} NS_RCS_LOGGER_SETTINGS;

typedef struct _TNS_EnableRcsLogging {
  CHAR m_cAppName[MAX_NAME_SIZE_APP];
  BOOL m_bEnable;
} TNS_EnableRcsLogging;

// End RCS related

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLog
/// \~english @par Brief
///        Writes a log to the Native Services logging mechanism
/// \~english @param [in] p_lLine_i
///        const UI_16 - Line number(0 to 65535) that this log was was written on
/// \~english @param [in] f_uiZoneIndex
///        const UI_64 - Mask value(0 to 511) to check against
/// \~english @param [in] p_pstrClassName_i
///        PCSTR - Name of the class or method where the log was written
/// \~english @param [in] lpszFormat
///        PCSTR - string similar to that of a printf statement
/// \~english @param [in] arg list
///        ... - Data arguments(option)
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///         FRAMEWORKUNIFIEDLOG parameter is set throuth NsLogSetFrameworkunifiedLogParams
/// \~english @par Change of internal state
///         None
/// \~english @par Conditions of processing failure
///       - Real-time log output switch can be turn on by NsLogSetRealtimeLog() generally.
///         But in case of FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_FREEZE real-time log and/ramd/log/frameworkunifiedlog /will not print out
/// \~~english @par
///       - in the following conditions,real-time log will not print out:
///        - log output way is LPRINT but f_uiZoneIndex parameter is in [96 to 127]
///        - log output way is LPRINT but real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB and mq_open system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB and mq_send system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and socket system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and setsockopt system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and bind system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and inet_addr system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and sendto system call failed(retry 5 times)
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB_DISABLE
/// \~english @par
///        in the following conditions /ramd/log/frameworkunifiedlog/ log can not be print out
///        - log file for parameter f_uiZoneIndex and
///          log file for parameter FrameworkunifiedLogFlagId which is set by FRAMEWORKUNIFIEDLOG parameter can not be found
///        - the ring_buffer of log file(log file for FrameworkunifiedLogFlagId which is set by FRAMEWORKUNIFIEDLOG parameter when
///          log file is not exited) for parameter f_uiZoneIndex is not opened (CNSRingBuffer::IsOpen())
///        - the write-in operation to ring_buffer of log file for parameter f_uiZoneIndex
///          (log file for FrameworkunifiedLogFlagId which is set by FRAMEWORKUNIFIEDLOG parameter when file is not exited) is failed
/// \~english @par Detail
///         Writes a log to the Native Services logging mechanism
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
void NsLog(const UI_16 p_lLine_i,       // Line number where message was generated
           const UI_16 f_uiZoneIndex,
           PCSTR p_pstrClassName_i,
           PCSTR lpszFormat,
           ...)
#ifdef  __GNUC__
__attribute__((format(printf, 4, 5)))
#endif
;  // NOLINT  (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLog0
/// \~english @par Brief
///        Writes a log to the Native Services logging mechanism
/// \~english @param [in] p_lLine_i
///        const UI_16 - Line number(0 to 65535) that this log was was written on
/// \~english @param [in] f_uiZoneIndex
///        const UI_64 - Mask value(0 to 511) to check against
/// \~english @param [in] p_pstrClassName_i
///        PCSTR - Name of the class or method where the log was written
/// \~english @param [in] lpszFormat
///        PCSTR - string similar to that of a printf statement
/// \~english @param [in] arg list
///        ... - Data arguments(option)
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        FRAMEWORKUNIFIEDLOG parameter is set throuth NsLogSetFrameworkunifiedLogParams
/// \~english @par Change of internal state
///        None
/// \~english @par Conditions of processing failure
///       - Real-time log output switch can be turn on by NsLogSetRealtimeLog() generally.
///         But in case of FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_FREEZE real-time log and/ramd/log/frameworkunifiedlog /will not print out
/// \~~english @par
///       - in the following conditions,real-time log will not print out:
///        - log output way is LPRINT but f_uiZoneIndex parameter is in [96 to 127]
///        - log output way is LPRINT but real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB and mq_open system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB and mq_send system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and socket system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and setsockopt system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and bind system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and inet_addr system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and sendto system call failed(retry 5 times)
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB_DISABLE
/// \~english @par
///        in the following conditions /ramd/log/frameworkunifiedlog/ log can not be print out
///        - log file for parameter f_uiZoneIndex and
///          log file for parameter FrameworkunifiedLogFlagId which is set by FRAMEWORKUNIFIEDLOG parameter can not be found
///        - the ring_buffer of log file(log file for FrameworkunifiedLogFlagId which is set by FRAMEWORKUNIFIEDLOG parameter when
///          log file is not exited) for parameter f_uiZoneIndex is not opened (CNSRingBuffer::IsOpen())
///        - the write-in operation to ring_buffer of log file for parameter f_uiZoneIndex
///          (log file for FrameworkunifiedLogFlagId which is set by FRAMEWORKUNIFIEDLOG parameter when file is not exited) is failed
/// \~english @par Detail
///          Writes a log to the Native Services logging mechanism
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
void NsLog0(const UI_16 p_lLine_i,        // Line number where message was generated
            const UI_16 f_uiZoneIndex,
            PCSTR p_pstrClassName_i,
            PCSTR lpszFormat);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogTime
/// \~english @par Brief
///        Print out real-time in log using Native Service mechanism
/// \~english @param [in] f_uiZoneIndex
///        const UI_64 - Mask value(0 to 511) to check against
/// \~english @param [in] lpszFormat
///        PCSTR - string similar to that of a printf statement
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        parameter FRAMEWORKUNIFIEDLOG has already set using NsLogSetFrameworkunifiedLogParams
/// \~english @par Change of internal state
///       None
/// \~english @par Conditions of processing failure
///       - Real-time log output switch can be turn on by NsLogSetRealtimeLog() generally.
///         But in case of FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_FREEZE real-time log and/ramd/log/frameworkunifiedlog /will not print out
/// \~~english @par
///       - in the following conditions,real-time log will not print out:
///        - log output way is LPRINT but f_uiZoneIndex parameter is in [96 to 127]
///        - log output way is LPRINT but real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB and mq_open system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB and mq_send system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and socket system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and setsockopt system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and bind system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and inet_addr system call failed
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER and sendto system call failed(retry 5 times)
///        - log output way is LPRINT,real-time log output setting is
///          FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB_DISABLE
/// \~english @par
///        in the following conditions /ramd/log/frameworkunifiedlog/ log can not be print out
///        - log file for parameter f_uiZoneIndex and
///          log file for parameter FrameworkunifiedLogFlagId which is set by FRAMEWORKUNIFIEDLOG parameter can not be found
///        - the ring_buffer of log file(log file for FrameworkunifiedLogFlagId which is set by FRAMEWORKUNIFIEDLOG parameter when
///          log file is not exited) for parameter f_uiZoneIndex is not opened (CNSRingBuffer::IsOpen())
///        - the write-in operation to ring_buffer of log file for parameter f_uiZoneIndex
///          (log file for FrameworkunifiedLogFlagId which is set by FRAMEWORKUNIFIEDLOG parameter when file is not exited) is failed
/// \~english @par Detail
///        Print out real-time in log using Native Service mechanism
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
void NsLogTime(const UI_16 f_uiZoneIndex,
               PCSTR lpszFormat);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogData
/// \~english @par Brief
///        log print-out function
/// \~english @param [in] f_uiZoneIndex
///        const UI_64 - Mask value(0 to 511) to check against
/// \~english @param [in] data
///        PCSTR -log print-out data
/// \~english @param [in] size (0 to ULONG_MAX)
///        UI_32 - data size (0 to ULONG_MAX)
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        FRAMEWORKUNIFIEDLOG has already set by NsLogSetFrameworkunifiedLogParams
/// \~english @par Change of internal state
///        None
/// \~english @par Conditions of processing failure
///        -Process will failed under following conditions:
///         -Real-time log output switch can be turn on by NsLogSetRealtimeLog() generally.
///         But the set value is FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_FREEZE
///         -the log file for f_uiZoneIndex and log file for FrameworkunifiedLogFlagId which is set by FRAMEWORKUNIFIEDLOG parameter
///          are both not found
///        - the ring_buffer of log file for f_uiZoneIndex(using log file for FrameworkunifiedLogFlagId which is set by FRAMEWORKUNIFIEDLOG
///          if file is not exited)is not opened (CNSRingBuffer::IsOpen())
///        - write operation on ring_buffer of log file for f_uiZoneIndex
///          (using log file for FrameworkunifiedLogFlagId which is set by FRAMEWORKUNIFIEDLOG if file is not exited)is failed
/// \~english @par Detail
///         the difference bewteen NsLog and NsLogData:
///        - NsLog()will print out real-time log
///          NsLogData() will not print out real-time log but noly print out logs under /ramd/log/frameworkunifiedlog/
///        - NsLog() will add ZoneIndex, process name, real-time info to log but
///          NsLogData() only print out log
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
void NsLogData(const UI_16 f_uiZoneIndex,
               PCSTR data,
               UI_32 size);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogSetFrameworkunifiedLogParams
/// \~english @par Brief
///        Sets the FRAMEWORKUNIFIEDLOG parameter of your application\process based on
///        the value passed in.
/// \~english @param [in] p_FrameworkunifiedLogParams
///        FRAMEWORKUNIFIEDLOGPARAM * - FRAMEWORKUNIFIEDLOG parameter in process
/// \~english @par
///        FRAMEWORKUNIFIEDLOGPARAM Structure
/// \~english @code
///                 typedef struct _FRAMEWORKUNIFIEDLOGPARAM {
///                   UI_8 uiLogOptions;                              // output options (ref. NS_LoggingMethod_t )
///                   CHAR cZones[USER_ZONES_COUNT][ZONE_TEXT_SIZE];  // mask text ZONE_TEXT_10 ... ZONE_TEXT_n
///                   // mask options ZONEMASK(10)...ZONEMASK(n) (ref. TFrameworkunifiedZone)
///                   TFrameworkunifiedZoneMask uiZoneMask;
///                   TFrameworkunifiedLoggerSeverity eSeverity;                   // severity level(ref. TFrameworkunifiedLoggerSeverity)
///                  } FRAMEWORKUNIFIEDLOGPARAM, *LPFRAMEWORKUNIFIEDLOGPARAM;
///            @endcode
/// \~english @par
///        TFrameworkunifiedZoneMask Array
/// \~english @code
///                 typedef TFrameworkunifiedZone TFrameworkunifiedZoneMask[ZONE_MASK_ARRAY_ELTS];
///            @endcode
/// \~english @par
///        TFrameworkunifiedZone typedef
/// \~english @code
///                 UI_32 TFrameworkunifiedZone;  // 32bit
///            @endcode
/// \~english @par
///        TFrameworkunifiedLoggerSeverity Enum
/// \~english @code
///                  typedef enum _TFrameworkunifiedLoggerSeverity {
///                    FRAMEWORKUNIFIED_SEVERITY_LOW  = 0x0,  // all enable(default)
///                    FRAMEWORKUNIFIED_SEVERITY_DEBUG2,      // enable FRAMEWORKUNIFIEDLOG_DEBUG() and FRAMEWORKUNIFIEDLOG_DEBUG2() and all the follows
///                    FRAMEWORKUNIFIED_SEVERITY_DEBUG1,      // enable FRAMEWORKUNIFIEDLOG_DEBUG1() and all the follows
///                    FRAMEWORKUNIFIED_SEVERITY_INFO,        // enable FRAMEWORKUNIFIEDLOG_INFO() and all the follows
///                    FRAMEWORKUNIFIED_SEVERITY_WARN,        // enable FRAMEWORKUNIFIEDLOG_WARN() and all the follows
///                    FRAMEWORKUNIFIED_SEVERITY_ERROR,       // enable FRAMEWORKUNIFIEDLOG_ERROR() and all the follows
///                    FRAMEWORKUNIFIED_SEVERITY_FATAL,       // enable FRAMEWORKUNIFIEDLOG_FATAL() and FRAMEWORKUNIFIEDLOG_ALWAYS()
///                    FRAMEWORKUNIFIED_SEVERITY_ALWAYS       // enable FRAMEWORKUNIFIEDLOG_ALWAYS()
///                  } TFrameworkunifiedLoggerSeverity;
///            @endcode
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        - process will not conducted when p_FrameworkunifiedLogParams is NULL.
/// \~english @par Detail
///        set the FRAMEWORKUNIFIEDLOG of Application using the parameter set in parameter\n
///        ZONE_NS_WAR(8), ZONE_NS_ERR(9) will be put-out forcely in the function\n
///        log will be print out when ForceOutput is set.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
void NsLogSetFrameworkunifiedLogParams(FRAMEWORKUNIFIEDLOGPARAM *p_FrameworkunifiedLogParams);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogSetProcessName
/// \~english @par Brief
///        Sets the name of your application\process based on
///        the value passed in.
/// \~english @param [in] p_strProcessName_i
///        PCSTR - name of your application process.(string array ended by NULL below 20byte)
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        The specified Aplication names are not set in the following cases:
///        - When the Application name specified in the arguments (p_strProcessName_i) is NULL
///        - When the Application name specified in the arguments (p_strProcessName_i) is a NULL termination character string of 21 bytes or more
/// \~english @par Detail
///        -name in prarameter should be string array(less than 20byte) ended by NULL
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
void NsLogSetProcessName(PCSTR p_strProcessName_i);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogSetControlMask
/// \~english @par Brief
///        Sets the control word that should be logged
/// \~english @param [in] p_ulNSLogControl_i
///        TFrameworkunifiedZoneMask - Set mask to the new value.
/// \~english @par
///        TFrameworkunifiedZoneMask Array
/// \~english @code
///                 typedef TFrameworkunifiedZone TFrameworkunifiedZoneMask[ZONE_MASK_ARRAY_ELTS];
///            @endcode
/// \~english @par
///        TFrameworkunifiedZone typedef
/// \~english @code
///                 UI_32 TFrameworkunifiedZone;  // 32bit
///            @endcode
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        -process will not be conducted when p_ulNSLogControl_i is none
/// \~english @par Detail
///        -Sets the control word that should be logged
///         zone mask(16) will be rewrited using parameter value
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        NsLogGetControlMask
////////////////////////////////////////////////////////////////////////////////////////////
void NsLogSetControlMask(TFrameworkunifiedZoneMask p_ulNSLogControl_i);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogIsZoneSet
/// \~english @par Brief
///        check wheather the FRAMEWORKUNIFIEDLOG ZONE is set by your application process.
/// \~english @param [in] set_zone
///        UI_32 - checking zone_index
/// \~english @retval TRUE  zone is enable
/// \~english @retval FALSE zone is disable
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        If the parameter of set_zone is outside(set_zone < 0 or 511 > set_zone), it will return FALSE.
/// \~english @par Detail
///        check wheather the FRAMEWORKUNIFIEDLOG ZONE is set by your application process.\n
///        zone parameter first 4bit(0 to 15)is the index of zone mask array\n
///        second 5bit(0 to 31) is showing bit position of mask(9bit,511 for the most)\n
///        True will be returned when bit for zone mask is set as 1
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
BOOL NsLogIsZoneSet(UI_32 set_zone);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogGetControlMask
/// \~english @par Brief
///        Get the control word that you currently have set.
/// \~english @param [out] p_Zonemask_i
///        TFrameworkunifiedZoneMask - current mask value.
/// \~english @par
///        TFrameworkunifiedZoneMask Array
/// \~english @code
///                 typedef TFrameworkunifiedZone TFrameworkunifiedZoneMask[ZONE_MASK_ARRAY_ELTS];
///            @endcode
/// \~english @par
///        TFrameworkunifiedZone typedef
/// \~english @code
///                 UI_32 TFrameworkunifiedZone;  // 32bit
///            @endcode
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        - If p_Zonemask_i is NULL, no operation is performed.
/// \~english @par Detail
///        Get all 16 zone mask.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        NsLogSetControlMask
////////////////////////////////////////////////////////////////////////////////////////////
void NsLogGetControlMask(TFrameworkunifiedZoneMask p_Zonemask_i);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogSetLogMethod
/// \~english @par Brief
///        Sets the logging place, LPRINT, LSHAREDMEM or any combination of these.
///        If this is never called the default is LSHAREDMEM.
/// \~english @param [in] p_eMethod_i
///        UI_8 - Where you would like to log your data.
/// \~english @par
///        NS_LoggingMethod_t Enum
/// \~english @code
///           typedef enum _NS_LOGGER_METHODS {
///             LPRINT      = 0x01,        // Print to the console(similar to printf)
///             LMSGQ       = 0x02,        // Disabled
///             LSLOGGER    = 0x04,        // Disabled
///             LSHAREDMEM  = 0x08         // Output to shared memory. this is the default logging method.
///            } NS_LoggingMethod_t;
///            @endcode
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        -switch the out-put place for NsLog API
/// \~english @par Conditions of processing failure
///        - none
/// \~english @par Detail
///         Sets the logging place, LPRINT, LSHAREDMEM or any combination of these.
///        If this is never called the default is LSHAREDMEM.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        NsLogGetLogMethod
////////////////////////////////////////////////////////////////////////////////////////////
void NsLogSetLogMethod(UI_8 p_eMethod_i);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogGetLogMethod
/// \~english @par Brief
///        Get the logging method.
/// \~english @param None
/// \~english @retval - return the way you log your data.
///        UI_8 - Log data type(NS_LoggingMethod_t Enum).
/// \~english @par
///        NS_LoggingMethod_t Enum
/// \~english @code
///           typedef enum _NS_LOGGER_METHODS {
///             LPRINT      = 0x01,        // Print to the console(similar to printf)
///             LMSGQ       = 0x02,        // Disabled
///             LSLOGGER    = 0x04,        // Disabled
///             LSHAREDMEM  = 0x08         // Output to shared memory. this is the default logging method.
///            } NS_LoggingMethod_t;
///            @endcode
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - The internal state is not changed.
/// \~english @par Conditions of processing failure
///        - none
/// \~english @par Detail
///        Invoke callback function to get the logging method.\n
///        If callback function is null and expectedCalls is '0',
///        using default SetSeparator function to get the logging method. \n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        NsLogSetLogMethod
////////////////////////////////////////////////////////////////////////////////////////////
UI_8 NsLogGetLogMethod(void);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogInitialize
/// \~english @par Brief
///        Initialize the log level and real-time log out-put setting
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        if the getting process of FRAMEWORKUNIFIEDLOG CONFIG file data is failed
/// \~english @par Detail
///        Initialize the log level and real-time log out-put setting
///        if the getting process of FRAMEWORKUNIFIEDLOG CONFIG file data is failed,log
///        level and real-time log out-put setting will not be initialized
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
void NsLogInitialize(void);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogSetFrameworkunifiedLogFlag
/// \~english @par Brief
///           set LogLevel of FRAMEWORKUNIFIEDLOG flag
/// \~english @param [in] flag_id
///        UI_8 - index for FRAMEWORKUNIFIEDLOG flag(0 to FLAG_LIST(declared in FRAMEWORKUNIFIEDLOG CONFIG file)
/// \~english @param [in] mode
///        UI_8 - LogLevel
/// \~english @par
///        avaliable LogLevel
///          - FRAMEWORKUNIFIEDLOG_FLAG_MODE_RELEASE
///          - FRAMEWORKUNIFIEDLOG_FLAG_MODE_DEBUG
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///     in the following conditions FRAMEWORKUNIFIEDLOG flag will not be set
///        - mode  is not FRAMEWORKUNIFIEDLOG_FLAG_MODE_RELEASE, FRAMEWORKUNIFIEDLOG_FLAG_MODE_DEBUG
///        - the getting process of FRAMEWORKUNIFIEDLOG CONFIG file is failed(ns_logger_frameworkunifiedlog_xxx.cfg)
///        - flag_id is not declared in FRAMEWORKUNIFIEDLOG CONFIG file
/// \~english @par Detail
///        set loglevel for specific FRAMEWORKUNIFIEDLOG flag
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        NsLogGetFrameworkunifiedLogFlag
////////////////////////////////////////////////////////////////////////////////////////////
void NsLogSetFrameworkunifiedLogFlag(UI_8 flag_id, UI_8 mode);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogGetFrameworkunifiedLogFlag
/// \~english @par Brief
///           get LogLevel of FRAMEWORKUNIFIEDLOG flag
/// \~english @param [in] flag_id
///        UI_8 -  index for FRAMEWORKUNIFIEDLOG flag(0 to FLAG_LIST(declared in FRAMEWORKUNIFIEDLOG CONFIG file)
/// \~english @param [out]mode
///        UI_8 - LogLevel value
/// \return none
///        void - there is no return.
/// \~english @retval eFrameworkunifiedStatusOK   Success
/// \~english @retval eFrameworkunifiedStatusFail any sort of error occurred
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        -parameter mode is NULL[eFrameworkunifiedStatusFail]
///        -flag_id is not declared in FRAMEWORKUNIFIEDLOG CONFIG file [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        get FRAMEWORKUNIFIEDLOG flage for specific flag_id
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        NsLogSetFrameworkunifiedLogFlag
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NsLogGetFrameworkunifiedLogFlag(UI_8 flag_id, UI_8 *mode);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogSetRealtimeLog
/// \~english @par Brief
///            real-time log output setting
/// \~english @param [in] mode
///        UI_8 - set mode for Real time log
/// \~english @par
///        available setting for real-time log output
///        default value is REALTIME_LOG in FRAMEWORKUNIFIEDLOG CFG file
///        (FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF when FRAMEWORKUNIFIEDLOG CFG file get failed)
/// \~english @code
///                 #define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_UART        1     // : UARTOUT
///                 #define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB         2     // : USB OUT
///                 #define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB_DISABLE 0x82  // : USB OFF
///                 #define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER       3     // : Ether OUT
///                 #define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF         0     // : ALL OFF
///                 #define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_FREEZE      0xFF  // : FREEZE
///            @endcode
/// \~english @retval none
///        void - there is no return
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        -mode is not in FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_XXX above
/// \~english @par Detail
///        set the real-time log output using the setting value.\n
///        need to reboot during ROM start in order to switch to Ethernet output\n
///        /nv_system/frameworkunifiedlog.ether file will be generated
///        when mode FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER /nv_system/frameworkunifiedlog.ether\n
///        and /nv_system/frameworkunifiedlog.ether file will be delated when mode is not FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        NsLogGetRealtimeLog
////////////////////////////////////////////////////////////////////////////////////////////
void NsLogSetRealtimeLog(UI_8 mode);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogGetRealtimeLog
/// \~english @par Brief
///        get the setting of real-time log output
/// \~english @param [out] mode
///        UI_8 * -the setting of real-time log output
/// \~english @par
///        available settings of real-time log output
///        default value is REALTIME_LOG in FRAMEWORKUNIFIEDLOG CFG file
///        (FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF when FRAMEWORKUNIFIEDLOG CFG file get failed)
/// \~english @code
///                 #define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_UART        1     // : UARTOUT
///                 #define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB         2     // : USB OUT
///                 #define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB_DISABLE 0x82  // : USB OFF
///                 #define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_ETHER       3     // : Ether OUT
///                 #define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF         0     // : ALL OFF
///                 #define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_FREEZE      0xFF  // : FREEZE
///            @endcode
/// \~english @retval none
///        void - there is no return
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        - cannot get when mode is NULL
///        -FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF will be returned when the approach
///        of FRAMEWORKUNIFIEDLOG CONFIG file (ns_logger_frameworkunifiedlog_xxx.cfg)is failed
/// \~english @par Detail
///        get the setting of real-time log output
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        NsLogSetRealtimeLog
////////////////////////////////////////////////////////////////////////////////////////////
void NsLogGetRealtimeLog(UI_8 *mode);

// Deprecated API: Not use.
////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogSetSeverity
/// \~english @par Brief
///        Set log severity level
/// \~english @param [in] p_eLogSeverity_i
///        TFrameworkunifiedLoggerSeverity - Log severity
/// \~english @par
///        TFrameworkunifiedLoggerSeverity Enum
/// \~english @code
///           typedef enum _TFrameworkunifiedLoggerSeverity {
///             FRAMEWORKUNIFIED_SEVERITY_LOW  = 0x0,  /// <all enable(default)
///             FRAMEWORKUNIFIED_SEVERITY_DEBUG2,    /// <enable FRAMEWORKUNIFIEDLOG_DEBUG() and FRAMEWORKUNIFIEDLOG_DEBUG2() and all the follows
///             FRAMEWORKUNIFIED_SEVERITY_DEBUG1,    /// <enable FRAMEWORKUNIFIEDLOG_DEBUG1() and all the follows
///             FRAMEWORKUNIFIED_SEVERITY_INFO,      /// <enable FRAMEWORKUNIFIEDLOG_INFO() and all the follows
///             FRAMEWORKUNIFIED_SEVERITY_WARN,      /// <enable FRAMEWORKUNIFIEDLOG_WARN() and all the follows
///             FRAMEWORKUNIFIED_SEVERITY_ERROR,     /// <enable FRAMEWORKUNIFIEDLOG_ERROR() and all the follows
///             FRAMEWORKUNIFIED_SEVERITY_FATAL,     /// <enable FRAMEWORKUNIFIEDLOG_FATAL() and FRAMEWORKUNIFIEDLOG_ALWAYS()
///             FRAMEWORKUNIFIED_SEVERITY_ALWAYS     /// <enable FRAMEWORKUNIFIEDLOG_ALWAYS()
///            } TFrameworkunifiedLoggerSeverity;
///            @endcode
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(p_eLogSeverity_i).
/// \~english @par Detail
///        Invoke callback function to set log severity.\n
///        If callback function is null and expectedCalls is '0', using default function to set log severity.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        NsLogGetSeverity
////////////////////////////////////////////////////////////////////////////////////////////
void NsLogSetSeverity(TFrameworkunifiedLoggerSeverity p_eLogSeverity_i);

// Deprecated API: Not use.
////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogGetSeverity
/// \~english @par Brief
///        Get the log severity.
/// \~english @param none
/// \~english @retval Log Severity
///        TFrameworkunifiedLoggerSeverity - Log severity.
/// \~english @par
///        TFrameworkunifiedLoggerSeverity Enum
/// \~english @code
///           typedef enum _TFrameworkunifiedLoggerSeverity {
///             FRAMEWORKUNIFIED_SEVERITY_LOW  = 0x0,  /// <all enable(default)
///             FRAMEWORKUNIFIED_SEVERITY_DEBUG2,    /// <enable FRAMEWORKUNIFIEDLOG_DEBUG() and FRAMEWORKUNIFIEDLOG_DEBUG2() and all the follows
///             FRAMEWORKUNIFIED_SEVERITY_DEBUG1,    /// <enable FRAMEWORKUNIFIEDLOG_DEBUG1() and all the follows
///             FRAMEWORKUNIFIED_SEVERITY_INFO,      /// <enable FRAMEWORKUNIFIEDLOG_INFO() and all the follows
///             FRAMEWORKUNIFIED_SEVERITY_WARN,      /// <enable FRAMEWORKUNIFIEDLOG_WARN() and all the follows
///             FRAMEWORKUNIFIED_SEVERITY_ERROR,     /// <enable FRAMEWORKUNIFIEDLOG_ERROR() and all the follows
///             FRAMEWORKUNIFIED_SEVERITY_FATAL,     /// <enable FRAMEWORKUNIFIEDLOG_FATAL() and FRAMEWORKUNIFIEDLOG_ALWAYS()
///             FRAMEWORKUNIFIED_SEVERITY_ALWAYS     /// <enable FRAMEWORKUNIFIEDLOG_ALWAYS()
///            } TFrameworkunifiedLoggerSeverity;
///            @endcode
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - The internal state is not changed.
/// \~english @par Conditions of processing failure
///        none
/// \~english @par Detail
///        Invoke callback function to get log severity.\n
///        If callback function is null and expectedCalls is '0', using default function to get log severity.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        NsLogSetSeverity
////////////////////////////////////////////////////////////////////////////////////////////
TFrameworkunifiedLoggerSeverity NsLogGetSeverity(void);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLog_EvtCnt
/// \~english @par Brief
///        Event and count logger.
/// \~english @param [in] @param [in] Cnt_Id
///        UI_16 - ID of counter from comm layer
/// \~english @param [in] Evt_Id
///        UI_16 - ID of Event from comm layer
/// \~english @param [in] n
///        UI_8 - number of variable arguments inputted
/// \~english @param [in] arg list
///        ... - Data arguments(option)
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        none
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(Cnt_Id).
///       - It is an incorrect value(Evt_Id).
///       - It is an incorrect value(n).
/// \~english @par Detail
///        Invoke callback function with argument 'Cnt_Id', 'Evt_Id', 'n' to log.\n
///        If callback function is null and expectedCalls is '0', using default SetSeparator function to log. \n
///        In other situation, match and log 'Cnt_Id', 'Evt_Id', 'n'.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
// Deprecated API: Not use.
void NsLog_EvtCnt(      // Line number where message was generated
  UI_16 Cnt_Id,
  UI_16 Evt_Id, UI_8 n,
  ...);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLog_Evt
/// \~english @par Brief
///        Event logger mechanism.(Send data to SS event logger Queue)
/// \~english @param [in] Evt_Id
///        UI_16 - ID of Event from comm layer
/// \~english @param [in] nu
///        UI_8 - number of variable arguments inputted
/// \~english @param [in] ...
///        ... - Data arguments
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        - process nothing when Evt_id is in [0 to 0xFF]
///        - process nothing when nu is not 1 or 4
/// \~english @par Detail
///        -send the log event queue to SS(logger mechanism Event)
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Fire and Forget
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
// Deprecated API: Not use.
void NsLog_Evt(UI_16 Evt_Id, UI_8 nu, ...);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLog_Cnt
/// \~english @par Brief
///        Counter logger mechanism.(Send data to SS event logger Queue)
/// \~english @param [in] Cnt_Id
///        UI_16 - ID of Counter from comm layer
/// \~english @param [in] nu
///        UI_8 - number of variable arguments inputted
/// \~english @param [in] ...
///        ... - Data arguments
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        - process nothing when Cnt_Id is 0
///        - process nothing when nu is not 0 or 1
/// \~english @par Detail
///        send the log event queue to SS(logger mechanism counter)
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Fire and Forget
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
// Deprecated API: Not use.
void NsLog_Cnt(UI_16 Cnt_Id, UI_8 nu, ...);  // Not to use.

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogDetermineLogMethod
/// \~english @par Brief
///        Determines the control log method based on the string passed.
///        If the string passed doesn't match any of the key words:
///        slogger or msgr or console the method will return a value of
///        zero. This string may be in the form of the following:
///        "msgq|console" would cause the output to go to both the NSLog
///        message queue and the console.
/// \~english @param [in] output_type
///        PCSTR - The key words:
/// \~english @retval 1 ... 0xF Success
/// \~english @retval 0 the string passed doesn't match any of the key words
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        the string passed doesn't match any of the key words
/// \~english @par Detail
///        mask value retruned from application can be used as parameter in NsLogSetLogMethod()
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
UI_8 NsLogDetermineLogMethod(PCSTR output_type);

// Unused API.
// VOID NsLogEnableRcsLogging(BOOL f_bRCSEnableLogging);

// Private API.
////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogGetZoneTextList
/// \~english @par Brief
///        Get the zone data list.
/// \~english @param [out] f_cZoneList
///        CHAR [][ZONE_TEXT_SIZE] - zone text list(0 ... 511)
/// \~english @par
///        PUI_32 - Pointer to preallocated array of CHAR cZones[USER_ZONES_COUNT][ZONE_TEXT_SIZE];
/// \~english @retval none
///        VOID - There is no return
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(f_cZoneList) or f_cZoneList is empty.
///       - Some error occur when get value from f_cZoneList
/// \~english @par Detail
///        Invoke callback function to get zone text list.\n
///        If callback function is null and expectedCalls is '0', using default function to get zone text list.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        NsLogSetZones
////////////////////////////////////////////////////////////////////////////////////////////
VOID NsLogGetZoneTextList(CHAR f_cZoneList[][ZONE_TEXT_SIZE]);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogSetZones
/// \~english @par Brief
///            set FRAMEWORKUNIFIEDLOG ZONE
/// \~english @param [in] f_uiZoneCount
///        UI_32   - number of zone
/// \~english @param [in] ...
///        arg_list     - zone list (0 ... 511)
/// \~english @retval none
///        void - there is no return.
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        - zone value will not over 512
/// \~english @par Detail
///        set the specific bit to existed zone mask\n
///        first 4bit(0 to 15) in zone is the index of zone mask array\n
///        second 5bit(0 to 31)set the 9bit which shows the bit position of mask(blew 511)\n
///        eg:in the case of setting three zones=31,30,29, function should be called like NsLogSetZones(3,31,30,29) \n
///        operation may not be right when parameter shows in arg_list differs with array num in f_uiZoneCount
///
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Set Get
/// \~english @see
///        none
/////////////////////////////////////////////////////////////////////////////////////
void NsLogSetZones(UI_32 f_uiZoneCount, ...);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogParseZones
/// \~english @par Brief
///        Parse the FRAMEWORKUNIFIEDLOG ZONE.
/// \~english @param [out] p_FrameworkunifiedLogParams
///        FRAMEWORKUNIFIEDLOGPARAM * - Input/output FRAMEWORKUNIFIEDLOG parameter
/// \~english @param [in] f_uiZoneCount
///        UI_32   - Zone setting
/// \~english @param [in] ...
///        arg_list     - Zone lists (zone values are 0 to 511)
/// \~english @par
///        FRAMEWORKUNIFIEDLOGPARAM structures
/// \~english @code
/// typedef struct _FRAMEWORKUNIFIEDLOGPARAM {
///    UI_8         uiLogOptions;                             // Log output location (ref NS_LoggingMethod_t)
///    CHAR         cZones[USER_ZONES_COUNT][ZONE_TEXT_SIZE];  // ZONE_TEXT_10 ... ZONE_TEXT_n
///    TFrameworkunifiedZoneMask uiZoneMask;                               // ZONEMASK(10)...ZONEMASK(n) (ref TFrameworkunifiedZone)
///    TFrameworkunifiedLoggerSeverity eSeverity;                          // Logging level (ref TFrameworkunifiedLoggerSeverity)
/// } FRAMEWORKUNIFIEDLOGPARAM, *LPFRAMEWORKUNIFIEDLOGPARAM;
///            @endcode
/// \~english @retval none
/// \~english @par Prerequisite
///       - none
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - none
/// \~english @par Detail
///         In the FRAMEWORKUNIFIEDLOG parameter (p_FrameworkunifiedLogParams) specified in the arguments,\n
///         add (or) all masked values of the zone lists specified by the arguments.
/// \~english @par Classification
///          Public
/// \~english @see
///          none
/////////////////////////////////////////////////////////////////////////////////////
void NsLogParseZones(FRAMEWORKUNIFIEDLOGPARAM *p_FrameworkunifiedLogParams, UI_32 f_uiZoneCount, ...);

// Unused API.
// VOID NsRcsSendTraceData(PCHAR f_pData, UI_32 f_len);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NSLogGetFrameworkunifiedlogFileTotalNum
/// \~english @par Brief
///        get total number of log file
/// \~english @retval num
///        UI_32 - Total numger of frameworkunifiedlog files.
/// \~english @par Prerequisite
///        set log file using NsLogSetFrameworkunifiedLogParams
/// \~english @par Change of internal state
///        - none
/// \~english @par Conditions of processing failure
///        - none
/// \~english @par Detail
///        get total number of log file
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 NSLogGetFrameworkunifiedlogFileTotalNum(void);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NSLogGetFrameworkunifiedlogFileName
/// \~english @par Brief
///        get the name of log file
/// \~english @param [in] index
///        UI_32 - Index of a list of frameworkunifiedlog file names.(0 to  NSLogGetFrameworkunifiedlogFileTotalNum()-1)
/// \~english @retval file name -file with specific index is exsisted
/// \~english @retval NULL -file with specific index is not exsisted
/// \~english @par Prerequisite
///          set log file using NsLogSetFrameworkunifiedLogParams
/// \~english @par Change of internal state
///          - none
/// \~english @par Conditions of processing failure
///          file with specific index is not exsisted
/// \~english @par Detail
///         get the file name with specific index is not exsisted
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No macth
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
PCSTR NSLogGetFrameworkunifiedlogFileName(UI_32 index);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NSLogGetFrameworkunifiedlogFileName
/// \~english @par Brief
///          get index of log file
/// \~english @param [in] filename
///        PCSTR - Name of frameworkunifiedlog file.
/// \~english @retval 0 log file specific logfile is not exsisted
/// \~english @retval index of log file specific logfile is exsisted
/// \~english @par Prerequisite
///             set log file using NsLogSetFrameworkunifiedLogParams
/// \~english @par Change of internal state
///            - none
/// \~english @par Conditions of processing failure
///           process nothing when file name is NULL
///           process nothing when file name is not exsisted
/// \~english @par Detail
///           get index of log file
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
int NSLogGetFrameworkunifiedlogIndex(PCSTR filename);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsForceClose
/// \~english @par Brief
///        clear log forcely.
/// \~english @retval none
///        return void
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///       - none
/// \~english @par Conditions of processing failure
///       - none
/// \~english @par Detail
///       -clear log forcely.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////
VOID NsForceClose(void);

#ifdef __cplusplus
}
#endif


///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    extern FRAMEWORKUNIFIEDLOG parameter
///////////////////////////////////////////////////////////////////////////////
/// Global variable that's used by the Native Services Logger.
/// An application will setup this variable in their main,
/// values are user defined and provide user control of what
/// should and shouldn't get logged.
/// \code
/// FRAMEWORKUNIFIEDLOGPARAM g_FrameworkunifiedLogParams =
/// {
///   FRAMEWORKUNIFIEDLOGOPTIONS,
///     {
///       ZONE_TEXT_10, ZONE_TEXT_11, ZONE_TEXT_12,
///       ZONE_TEXT_13, ZONE_TEXT_14, ZONE_TEXT_15,
///       ZONE_TEXT_16, ZONE_TEXT_17, ZONE_TEXT_18,
///       ZONE_TEXT_19, ZONE_TEXT_20, ZONE_TEXT_21,
///       ZONE_TEXT_22, ZONE_TEXT_23, ZONE_TEXT_24,
///       ZONE_TEXT_25, ZONE_TEXT_26, ZONE_TEXT_27,
///     ZONE_TEXT_28, ZONE_TEXT_29, ZONE_TEXT_30,
///     ZONE_TEXT_31, ZONE_TEXT_32, ZONE_TEXT_33,
///     ZONE_TEXT_34, ZONE_TEXT_35, ZONE_TEXT_36,
///     ZONE_TEXT_37, ZONE_TEXT_38, ZONE_TEXT_39,
///     ZONE_TEXT_40, ZONE_TEXT_41, ZONE_TEXT_42,
///     ZONE_TEXT_43, ZONE_TEXT_44, ZONE_TEXT_45,
///     ZONE_TEXT_46, ZONE_TEXT_47, ZONE_TEXT_48,
///     ZONE_TEXT_49, ZONE_TEXT_50, ZONE_TEXT_51,
///     ZONE_TEXT_52, ZONE_TEXT_53, ZONE_TEXT_54,
///     ZONE_TEXT_55
///     },
///     FRAMEWORKUNIFIEDLOGZONES
/// };
/// \endcode
///

/// if FRAMEWORKUNIFIEDLOGZONES not defined in makefile
#ifndef FRAMEWORKUNIFIEDLOGZONES
///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    g_FrameworkunifiedLogParams
/// if FRAMEWORKUNIFIEDLOGZONES not defined in makefile
///////////////////////////////////////////////////////////////////////////////
#define FRAMEWORKUNIFIEDLOGZONES {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  FRAMEWORKUNIFIED_SET_ZONES
/// \~english @par Brief
///        This macro API register FRAMEWORKUNIFIEDLOGAPPZONES.
/// \~english @retval none
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///        - none
/// \~english @par Detail
///        in main() function use macro FRAMEWORKUNIFIED_SET_ZONES() before FrameworkunifiedDispatcher()
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        NsLogSetFrameworkunifiedLogParams
///////////////////////////////////////////////////////////////////////////////
#define FRAMEWORKUNIFIED_SET_ZONES() {UI_32 LogArr[] = {FRAMEWORKUNIFIEDLOGAPPZONES}; \
               NsLogParseZones(&g_FrameworkunifiedLogParams, (UI_32)_countof(LogArr), FRAMEWORKUNIFIEDLOGAPPZONES); \
               NsLogSetFrameworkunifiedLogParams(&g_FrameworkunifiedLogParams);}
#else
#warning "Zones defined by FRAMEWORKUNIFIEDLOGAPPZONES are overridden by FRAMEWORKUNIFIEDLOGZONES"
#define FRAMEWORKUNIFIED_SET_ZONES() NsLogSetFrameworkunifiedLogParams(&g_FrameworkunifiedLogParams)
#endif

#define ZONE_END UINT_MAX
// Unused API
// #define CHECK_SEVERITY_LEVEL(sev_level) ((sev_level) >= NsLogGetSeverity())
// If value of BITS_IN_TZONE changes in future then need to consider changes in the values 5 and 0x1F in below macros
// here 5 represents (sizeof(TFrameworkunifiedZone)+1) and 0x1F represents (BITS_IN_TZONE-1)
#define GET_ZONE_INDEX(zone) (((UI_32)zone) >> 5)
#define GET_ZONE_BIT_MASK(zone) ((TFrameworkunifiedZone)(1U << (((UI_32)zone) & 0x1F)))

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  ZONEMASK
/// \~english @par Brief
///       the Macro for zone index mask
/// \~english @param [in] n
///        zone index
/// \~english @retval num
///        UI_32 - zone index
/// \~english @par Prerequisite
///         - none
/// \~english @par Change of internal state
///          - none
/// \~english @par Conditions of processing failure
///          - none
/// \~english @par Detail
///           mask the zone index
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////////
#define ZONEMASK( n ) ((TFrameworkunifiedZone)( n ))

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  IS_ZONE_SET
/// \~english @par Brief
///        check wheather the FRAMEWORKUNIFIEDLOG ZONE is set by your application process.
/// \~english @param [in] set_zone
///         UI_32 - checking zone_index
/// \~english @retval TRUE  zone is enable
/// \~english @retval FALSE zone is disable
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///        - none
/// \~english @par Detail
///        Checks if the FRAMEWORKUNIFIEDLOG ZONE can be configured by Application processes.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        NsLogIsZoneSet
////////////////////////////////////////////////////////////////////////////////////////////////
#define IS_ZONE_SET(set_zone) (NsLogIsZoneSet(set_zone) == TRUE)

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  FRAMEWORKUNIFIEDLOG
/// \~english @par Brief
///        Developers should use the FRAMEWORKUNIFIEDLOG for logging.
/// \~english @param [in] zone
///         Defined zone in a process (e.g. ZONE_FUNC)
/// \~english @param [in] funcname
///         Name of the class or method where the log was written (e.g. "main")
/// \~english @param [in] print_fmt
///          string similar to that of a printf statement
/// \~english @param [in] args
///          Data arguments(option)
/// \~english @retval none
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///        - If print_fmt is NULL, no operation is performed.
/// \~english @par Detail
///        FRAMEWORKUNIFIEDLOG is simple macro to make it easy to log messages from any point in your code.
///        This macro has smarts that does fast checks to determine if your log should or shouldn't be printed.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        ZONEMASK, IS_ZONE_SET, NsLog, NsLog0
///////////////////////////////////////////////////////////////////////////////
#ifndef _FRAMEWORKUNIFIEDLOG_ZONE_FUNC_ENABLE_
#define FRAMEWORKUNIFIEDLOG(zone, funcname, print_fmt, args...)\
  (zone != ZONEMASK(11))?((IS_ZONE_SET(zone))?TEXT_ZONE(zone, funcname, print_fmt, ## args):((void)0)):((void)(0))
#else  // _FRAMEWORKUNIFIEDLOG_ZONE_FUNC_ENABLE_
#define FRAMEWORKUNIFIEDLOG(zone, funcname, print_fmt, args...)\
  ((IS_ZONE_SET(zone)) ? TEXT_ZONE(zone, funcname, print_fmt, ## args) : ((void)0))
#endif  // _FRAMEWORKUNIFIEDLOG_ZONE_FUNC_ENABLE_

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \~english @par Brief
///      This is the log output macro for clock display in milliseconds.
/// \~english @param [in] zone
///      Zone indexes to be checked
/// \~english @param [in] str
///      Log output string
/// \~english @retval none
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///        - none
/// \~english @par Detail
///     A simple macro for logging messages in milliseconds from any point in the source code.\n
///     The log is output in the following format. "Hours:Minutes:Seconds.Milliseconds/=log-output-string"
/// \~english @par Classification
///      Public
/// \~english @see
///      IS_ZONE_SET
///
////////////////////////////////////////////////////////////////////////////////////////////////
#define FRAMEWORKUNIFIEDLOG_TIME(zone, str)\
  ((IS_ZONE_SET(zone))?NsLogTime(zone, str):((void)(0)))

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \~english @par Brief
///      This is a simplified version of the log output macro.
/// \~english @param [in] zone
///      Zone indexes to be checked
/// \~english @param [in] data
///      Log output data
/// \~english @param [in] size
///      Size of the log output data
/// \~english @retval none
/// \~english @par Prerequisite
///        - none
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///        - none
/// \~english @par Detail
///     A simple macro for logging messages from any point in the source code.\n
///     \ref The macro FRAMEWORKUNIFIEDLOG can specify variable argument lists for log output data,
///     but the macro is a macro that specifies only data character strings.
/// \~english @par Classification
///      Public
/// \~english @see
///      IS_ZONE_SET, NsLogData
///
////////////////////////////////////////////////////////////////////////////////////////////////
#define FRAMEWORKUNIFIEDLOG_DATA(zone, data, size)\
  ((IS_ZONE_SET(zone))?NsLogData(zone, data, size):((void)(0)))

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    FRAMEWORKUNIFIEDLOG_EVTCNT
///////////////////////////////////////////////////////////////////////////////
/// Deprecated API. Not use.
#define FRAMEWORKUNIFIEDLOG_EVTCNT(zone, CntId, EvtId, n...) NsLog_EvtCnt(CntId, EvtId, ##n)

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    FRAMEWORKUNIFIEDLOG_EVT
///////////////////////////////////////////////////////////////////////////////
/// Event logger mechanism.(Send data to SS event logger Queue)
/// \note  The ZONE is not used.
///
/// \param [in] zone
///         Defined zone in a process (e.g. ZONE_FUNC)
/// \param [in] EvtId
///         UI_16 - ID of Event from comm layer
/// \param [in] n
///         UI_8 - number of variable arguments inputted
///////////////////////////////////////////////////////////////////////////////
/// Deprecated API. Not use.
#define FRAMEWORKUNIFIEDLOG_EVT(zone, EvtId, n...) NsLog_Evt(EvtId, ##n)

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    FRAMEWORKUNIFIEDLOG_CNT
///////////////////////////////////////////////////////////////////////////////
/// Counter logger mechanism.(Send data to SS event logger Queue)
/// \note  The ZONE is not used.
///
/// \param [in] zone
///         Defined zone in a process (e.g. ZONE_FUNC)
/// \param [in] CntId
///         UI_16 - ID of Event from comm layer
/// \param [in] n
///         UI_8 - number of variable arguments inputted
////////////////////////////////////////////////////////////////////////////////////////////
/// Deprecated API. Not use.
#define FRAMEWORKUNIFIEDLOG_CNT(zone, CntId, n...) NsLog_Cnt(CntId, ##n)  // Not to use

/// Deprecated API. Not use.
#define TEXT(funcname, args...) TEXT_ZONE(BITS_IN_ZONE_MASK, funcname, ## args)

// Unused API
// #define TEXT0(funcname, print_fmt ) NsLog(__LINE__, BITS_IN_ZONE_MASK, funcname, print_fmt)

#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
      _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
      _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
      _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
      _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
      _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
      _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, \
      _71, _72, _73, _74, _75, _76, _77, _78, NAME, ...) NAME
#define NsLogFunc(...)\
  GET_MACRO(__VA_ARGS__, \
      NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, \
      NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, \
      NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, \
      NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, \
      NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, \
      NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, \
      NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, \
      NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog, NsLog0)
#define TEXT_ZONE(zone_index, funcname, ...)\
  NsLogFunc(__VA_ARGS__)(__LINE__, zone_index, funcname, __VA_ARGS__)

/// Deprecated API. Not use.
#define TEXT_EVTCNT(CntId, EvtId, n...) NsLog_EvtCnt(CntId, EvtId, ##n)
/// Deprecated API. Not use
#define TEXT_EVT(EvtID, n...) NsLog_Evt(EvtID, ##n)
/// Deprecated API. Not use.
#define TEXT_CNT(CntId, n...) NsLog_Cnt(CntId, ##n)
/// Used to log data that contains trailing arguments \n
/// Example: FRAMEWORKUNIFIEDLOG0(ZONE_INFO, __FUNCTION__, "testers return value is [%d]!", rtnValue);


/// Deprecated API. Use FRAMEWORKUNIFIEDLOG instead.
#define FRAMEWORKUNIFIEDLOG0 FRAMEWORKUNIFIEDLOG


/// Note: These bits are reserved for Framework logging and debugging
#define ZONE_NS_FUNC    ZONEMASK(0)
#define ZONE_NS_DIS       ZONEMASK(1)
#define ZONE_NS__CWORD77_       ZONEMASK(2)
#define ZONE_NS_IMP_INFO    ZONEMASK(3)
#define ZONE_NS_SM_DEV_INFO ZONEMASK(4)
#define ZONE_NS_SM_USR_INFO ZONEMASK(5)
#define ZONE_NS_INFO    ZONEMASK(6)
#define ZONE_NS_DEBUG_DUMP  ZONEMASK(7)
#define ZONE_NS_WAR         ZONEMASK(8)
#define ZONE_NS_ERR         ZONEMASK(9)
#define ZONE_DEBUG_DUMP   ZONE_NS_DEBUG_DUMP

#define ZONE_TEXT_0 "NS_Func"
#define ZONE_TEXT_1 "NS_Dispatcher"
#define ZONE_TEXT_2 "NS__CWORD77_"
#define ZONE_TEXT_3 "NS_Reserved"
#define ZONE_TEXT_4 "NS_StateMachine_Dev"
#define ZONE_TEXT_5 "NS_StateMachine_Usr"
#define ZONE_TEXT_6 "NS_Info"
#define ZONE_TEXT_7 "NS_Debug_Dump"
#define ZONE_TEXT_8 "NS_Warning"
#define ZONE_TEXT_9 "NS_Error"

////
// Following is old definitions. Only for compile.
// Specific ZONE to use the ns_logger_frameworkunifiedlog.cfg
// If possible, following Specific ZONE difinitions shall be delete.
////
// Special ZONE(64)-(95)
/// Note: These bits are reserved for special ZONE
#define ZONE_PERFORMANCE                  ZONEMASK(64)
#define ZONE_SCREEN_TRANS                 ZONEMASK(65)
#define ZONE_SWDL                         ZONEMASK(66)
#define ZONE_SYSTEMDATA                   ZONEMASK(67)
#define ZONE_LOG_SH                       ZONEMASK(68)
#define ZONE_LOG_SYS                      ZONEMASK(69)
#define ZONE_LOG__CWORD52_                    ZONEMASK(70)
#define ZONE_SECURE                       ZONEMASK(71)
#define ZONE_CAN_FILTER                   ZONEMASK(72)
#define ZONE_COMM_PROCESS                 ZONEMASK(73)
#define ZONE_CONNECT_DEVICE               ZONEMASK(74)
#define ZONE_SP_ZONE_75                   ZONEMASK(75)
#define ZONE_SP_ZONE_76                   ZONEMASK(76)
#define ZONE_SP_ZONE_77                   ZONEMASK(77)
#define ZONE_SP_ZONE_78                   ZONEMASK(78)
#define ZONE_SP_ZONE_79                   ZONEMASK(79)
#define ZONE_SP_ZONE_80                   ZONEMASK(80)
#define ZONE_SP_ZONE_81                   ZONEMASK(81)
#define ZONE_SP_ZONE_82                   ZONEMASK(82)
#define ZONE_SP_ZONE_83                   ZONEMASK(83)
#define ZONE_SP_ZONE_84                   ZONEMASK(84)
#define ZONE_SP_ZONE_85                   ZONEMASK(85)
#define ZONE_SP_ZONE_86                   ZONEMASK(86)
#define ZONE_SP_ZONE_87                   ZONEMASK(87)
#define ZONE_SP_ZONE_88                   ZONEMASK(88)
#define ZONE_SP_ZONE_89                   ZONEMASK(89)
#define ZONE_SP_ZONE_90                   ZONEMASK(90)
#define ZONE_SP_ZONE_91                   ZONEMASK(91)
#define ZONE_SP_ZONE_92                   ZONEMASK(92)
#define ZONE_SP_ZONE_93                   ZONEMASK(93)
#define ZONE_SP_ZONE_94                   ZONEMASK(94)
#define ZONE_SP_ZONE_95                   ZONEMASK(95)

// Special ZONE(96)-(127) (console off)
/// Note: These bits are reserved for special ZONE
#define ZONE_COMM_SYS                     ZONEMASK(96)
#define ZONE_COMM_SH                      ZONEMASK(97)
#define ZONE_COMM_USB                     ZONEMASK(98)
#define ZONE_SP_ZONE_99                   ZONEMASK(99)
#define ZONE_SP_ZONE_100                  ZONEMASK(100)
#define ZONE_SP_ZONE_101                  ZONEMASK(101)
#define ZONE_SP_ZONE_102                  ZONEMASK(102)
#define ZONE_SP_ZONE_103                  ZONEMASK(103)
#define ZONE_SP_ZONE_104                  ZONEMASK(104)
#define ZONE_SP_ZONE_105                  ZONEMASK(105)
#define ZONE_SP_ZONE_106                  ZONEMASK(106)
#define ZONE_SP_ZONE_107                  ZONEMASK(107)
#define ZONE_SP_ZONE_108                  ZONEMASK(108)
#define ZONE_SP_ZONE_109                  ZONEMASK(109)
#define ZONE_SP_ZONE_110                  ZONEMASK(110)
#define ZONE_SP_ZONE_111                  ZONEMASK(111)
#define ZONE_SP_ZONE_112                  ZONEMASK(112)
#define ZONE_SP_ZONE_113                  ZONEMASK(113)
#define ZONE_SP_ZONE_114                  ZONEMASK(114)
#define ZONE_SP_ZONE_115                  ZONEMASK(115)
#define ZONE_SP_ZONE_116                  ZONEMASK(116)
#define ZONE_SP_ZONE_117                  ZONEMASK(117)
#define ZONE_SP_ZONE_118                  ZONEMASK(118)
#define ZONE_SP_ZONE_119                  ZONEMASK(119)
#define ZONE_SP_ZONE_120                  ZONEMASK(120)
#define ZONE_SP_ZONE_121                  ZONEMASK(121)
#define ZONE_SP_ZONE_122                  ZONEMASK(122)
#define ZONE_SP_ZONE_123                  ZONEMASK(123)
#define ZONE_SP_ZONE_124                  ZONEMASK(124)
#define ZONE_SP_ZONE_125                  ZONEMASK(125)
#define ZONE_SP_ZONE_126                  ZONEMASK(126)
#define ZONE_SP_ZONE_127                  ZONEMASK(127)

#endif /* __NATIVESERVICES_LOGGER_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
