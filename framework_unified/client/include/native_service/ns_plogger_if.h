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
/// Declares functions and macros related to performance logging
///
/// \ingroup  tag_NSLogger
/// @{
///
//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @brief \~english Declares functions and macros related to performance logging
 *
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */
/** @addtogroup native
 *  @ingroup framework_unified
 *  @{
 */

#ifndef NS_PLOGGER_IF_H_  // NOLINT  (build/header_guard)
#define NS_PLOGGER_IF_H_

#include <native_service/frameworkunified_types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NS_PLOGQ "/NSplog"
#define NS_SYSEVENTLOGQ "/NSSysEventLog"

/// Performance logging
typedef enum _EPLOG_TIME_FORMAT {
  EPLOG_TIME_FORMAT_USEC,  ///< microseconds
  EPLOG_TIME_FORMAT_MSEC,  ///< miliseconds
  EPLOG_TIME_FORMAT_SEC,   ///< seconds
} EPLOG_TIME_FORMAT;

typedef enum _EPLOG_OUTPUT_OPTION {
  EPLOG_SLOG     = 0x01,   ///< log to standard logging i.e. slogger
  EPLOG_MSGQ     = 0x02,   ///< log to message queue NSPlog
  EPLOG_TRACEEVENT = 0x04  ///< log to trace events
} EPLOG_OUTPUT_OPTION;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    Performance LOG parameter
///////////////////////////////////////////////////////////////////////////////
typedef struct _FRAMEWORKUNIFIEDLOGPPARAM {
  BOOL           bIsPLogEnabled;      ///< PLog enabled or disabled
  EPLOG_TIME_FORMAT  eTimeFormat;     ///< Time format when PLog is printed
  UI_64        ui64GapInuS;           ///< Gap in micro seconds
  UI_8           uiLogOutputOptions;  ///< Output log options
} FRAMEWORKUNIFIEDLOGPPARAM, *LFRAMEWORKUNIFIEDLOGPPARAM;

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    System event LOG parameter
///////////////////////////////////////////////////////////////////////////////
typedef struct _FRAMEWORKUNIFIEDLOGSYSEVENTPARAM {
  BOOL           bIsSysEventLogEnabled;    ///< SysEvent log enabled or disabled
} FRAMEWORKUNIFIEDLOGSYSEVENTPARAM, *LFRAMEWORKUNIFIEDLOGSYSEVENTPARAM;

extern FRAMEWORKUNIFIEDLOGPPARAM g_FrameworkunifiedLogPParams;
extern FRAMEWORKUNIFIEDLOGSYSEVENTPARAM g_FrameworkunifiedLogSysEventParams;

#define PLOG_TEXT(args...) NSLogPrintPerformanceLog(__LINE__, __FUNCTION__, ## args)
#define SYSEVENTLOG_TEXT(args...) NSLogSysEvent(__LINE__, __FUNCTION__, ## args)
#define IS_PLOG_ENABLED() NsLogIsPLogEnabled()
#define IS_SYSEVENTLOG_ENABLED() NSLogIsSysEventLogEnabled()

#ifdef __FRAMEWORKUNIFIEDLOG_NDEBUG_LEVEL2__
#define FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG(print_fmt, args...) ((void)0)
#else
///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG
///////////////////////////////////////////////////////////////////////////////
/// Prints a performance log in PosixBasedOS001 system log. These logs can be viewed by using utility sloginfo.
/// \note     This macro API is disable in Linux system
///           This macro API is disable if defined "__FRAMEWORKUNIFIEDLOG_NDEBUG_LEVEL2__"
///
/// \param [in] print_fmt
///      PCSTR - Input string
/// \param [in] args
///          string similar to that of a printf statement and data argments
///////////////////////////////////////////////////////////////////////////////
#define FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG(print_fmt, args...) ((IS_PLOG_ENABLED()) ? PLOG_TEXT(print_fmt, ## args): ((void)0))
#endif

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    FRAMEWORKUNIFIEDLOG_PERFORMANCE
///////////////////////////////////////////////////////////////////////////////
/// FRAMEWORKUNIFIEDLOG_PERFORMANCE is a logging API provided specifically for logging performance metrics.
/// \note     This macro API is disable in Linux system
///           It should be used in few locations within an application. (Max 5 times per application).
///           It should never be used in place of FRAMEWORKUNIFIEDLOG.
///
/// \param [in] print_fmt
///          string similar to that of a printf statement
/// \param [in] args
///          Data arguments
///////////////////////////////////////////////////////////////////////////////
#define FRAMEWORKUNIFIEDLOG_PERFORMANCE(print_fmt, args...) ((IS_PLOG_ENABLED()) ? PLOG_TEXT(print_fmt, ## args): ((void)0))

///////////////////////////////////////////////////////////////////////////////
/// FRAMEWORKUNIFIEDLOG_SYSTEMEVENT
///
/// \param [in] print_fmt
///          string similar to that of a printf statement
/// \param [in] args
///          Data arguments
///////////////////////////////////////////////////////////////////////////////
#define FRAMEWORKUNIFIEDLOG_SYSTEMEVENT(print_fmt, args...) ((IS_SYSEVENTLOG_ENABLED()) ?\
  SYSEVENTLOG_TEXT(print_fmt, ## args): ((void)0))

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NSLogPrintPerformanceLog
/// \~english @par Brief
///        This macro API is disable in Linux system
/// \~english @param [in]  f_ui16Line
///        const UI_16 - line number
/// \~english @param [in] f_cFuncName
///        PCSTR - Function name. It takes __PRETTY_FUNCTION__
/// \~english @param [in] print_fmt
///        PCSTR - Input string
/// \~english @retval none
/// \~english @par Prerequisite
///        - Work ony when AGL_STUB is disabled.
/// \~english @par Change of internal state
///        - The internal state is not changed.
/// \~english @par Conditions of processing failure
///        none
/// \~english @par Detail
///        Prints a performance log in PosixBasedOS001 system log. These logs can be viewed by using utility sloginfo.
///        Log is in format: "Plog: Time[us]:x or o (if gap in cs):Line:Function Input string"\n
///        - Outputs in time units (microseconds/milliseconds/second) that can be set by the NSLogSetPlogTimeFormat.
///        - The time data to be output is added to the gap data described in the/dev/shmem/Plog.gap files.
///        - This information is output to the log output destinations (system log, NSPlog message queue, and event trace)
///          that can be set by the NSLogSetPlogOutputOptions.(Default: NSPLog message queue)
///        - PLOG_TEXT, Use this API from the FRAMEWORKUNIFIEDLOG_PERFORMANCE and FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG macros.\n
///               Continued:\n
///                 - PLOG_TEXT("Service Availability Status: %d", IsAvailable);
///                 - FRAMEWORKUNIFIEDLOG_PERFORMANCE("Service Availability Status: %d", IsAvailable);
///                 - FRAMEWORKUNIFIEDLOG_PERFORMANCE_DEBUG("Service Availability Status: %d", IsAvailable);
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////////////////
VOID NSLogPrintPerformanceLog(const UI_16 f_ui16Line, PCSTR f_cFuncName, PCSTR __format, ...);

////////////////////////////////////////////////////////////////////////////////////////////////
/// NSLogEnablePLog
/// Enables/Disables the performance logging
///
/// \param [in] f_bEnable
///      BOOL - TRUE- Enable performance logging, FALSE - Disable
///
/// \return none
///
////////////////////////////////////////////////////////////////////////////////////////////////
VOID NSLogEnablePLog(BOOL f_bEnable);

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NsLogIsPLogEnabled
/// \~english @par Brief
///        Is Enables/Disables the performance logging
/// \~english @retval TRUE  current state of plog enable.
/// \~english @retval FALSE current state of plog disable.
/// \~english @par Prerequisite
///        - 
/// \~english @par Change of internal state
///        - The internal state is not changed.
/// \~english @par Conditions of processing failure
///        - 
/// \~english @par Detail
///        Get the Performance Log enable/disable status.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        NSLogEnablePLog
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL NsLogIsPLogEnabled(void);

////////////////////////////////////////////////////////////////////////////////////////////////
/// NSLogSetPlogTimeFormat
/// Set the time format for performance logging.
///
/// \param [in] f_ePlogTimeFormat
///      EPLOG_TIME_FORMAT - Time format for performance logging
///
/// \return none
///
////////////////////////////////////////////////////////////////////////////////////////////////
VOID NSLogSetPlogTimeFormat(EPLOG_TIME_FORMAT f_ePlogTimeFormat);

////////////////////////////////////////////////////////////////////////////////////////////////
/// NSLogSetPlogOutputOptions
/// Set the output options for performance logging.
///
/// \param [in] f_uiPlogOutputOption
///      UI_8 - Output option. Value is generated by or'ed value of enum EPLOG_OUTPUT_OPTION
///      e.g. EPLOG_SLOG | EPLOG_MSGQ
///
/// \return none
///
////////////////////////////////////////////////////////////////////////////////////////////////
VOID NSLogSetPlogOutputOptions(UI_8 f_uiPlogOutputOption);

////////////////////////////////////////////////////////////////////////////////////////////////
/// NSLogSysEvent()
/// Prints system event log in message queue NSSyseventlog.
///
/// \param [in] f_ui16Line
///      const UI_16 - line number
/// \param [in] f_cFuncName
///      PCSTR - Function name. It takes __PRETTY_FUNCTION__
/// \param [in] __format
///      PCSTR - Input string
///
/// \return none
///
////////////////////////////////////////////////////////////////////////////////////////////////
VOID NSLogSysEvent(const UI_16 f_ui16Line, PCSTR f_cFuncName, PCSTR __format, ...);

////////////////////////////////////////////////////////////////////////////////////////////////
/// NSLogEnableSysEventLog
/// Enables/Disables the system event logging
///
/// \param [in] f_bEnable
///      BOOL - TRUE- Enable system event logging, FALSE - Disable
///
/// \return none
///
////////////////////////////////////////////////////////////////////////////////////////////////
VOID NSLogEnableSysEventLog(BOOL f_bEnable);

////////////////////////////////////////////////////////////////////////////////////////////////
/// NSLogIsSysEventLogEnabled
/// Is Enables/Disables the system event logging
///
/// \return SysEventLogEnabled
///         BOOL - current state of sys event log(enable/disable).
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL NSLogIsSysEventLogEnabled(void);

#ifdef __cplusplus
}
#endif
#endif /* NS_PLOGGER_IF_H_ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/// @}
