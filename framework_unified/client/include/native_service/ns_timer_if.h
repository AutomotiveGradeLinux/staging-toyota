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

/**
 * @file ns_timer_if.h
 * @brief \~english APIs to create, delete and use Native Service timers .
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
#ifndef __NATIVESERVICES_TIMER_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_TIMER_H__

#include <native_service/frameworkunified_types.h>


#ifdef __cplusplus
extern "C" {
#endif

#define TIMER_QUE "TIMER"

#define MAX_SERVICE_NAME 15

/// \brief Timer Function Pointer definition
/// Detailed description of the class
typedef void (*TimerCb)(UI_16 cmd);

/// \brief Timer info, defines the initial
/// start of a timer, the repeat timer
/// values and the cmd id for a timer
typedef struct _NSTimerInfo {

//  UI_32 t_sec;
  time_t t_sec;
  UI_64 t_nsec;
  UI_16 iCmd;

//  UI_32 rpt_sec;
  time_t rpt_sec;
  UI_64 rpt_nsec;
} NSTimerInfo;

/// \brief Enum Types for valid Callback Mechanisms for
/// a NS_Timer
typedef enum _NSTimerCallbackMechanism {
  CALLBACK_MESSAGE
} eNSTimerCallbackMechanism;

/// \brief Helper methods that convert time provided in MS.
/// mseconds

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup WholeSeconds
/// \~english @par Brief
///        Converts a time value in milliseconds (ms) to a time value in secods (s).
/// \~english @param [in] ms
///        UI_32 - Time value in milliseconds(0 to ULONG_MAX)
/// \~english @retval Time value in seconds
/// \~english @par Preconditons
///       - none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - none
/// \~english @par Detail
///       This function converts a time value in ms to a time value in s and returns it.\n
///       For example, 12345ms obtains 12s as the conversion result.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Not applicable
/// \~english @see
///          none
///
/// \~english @par Brief
///        Get the number of whole seconds in the milsecond number that was passed.
/// \~english @param[in] ms
///        UI_32 - time value in mil seconds
/// \~english @retval Number of whole seconds in the number passed.
/// \~english @par Preconditons
/// \~english @par Change of internal status
/// \~english @par Conditions of processing failure
/// \~english @par Detail
/// \~english @par Classification
/// \~english @par Type
/// \~english @see
///         none
/////////////////////////////////////////////////////////////////////////////////////

//UI_32 WholeSeconds(UI_32 ms);
time_t WholeSeconds(UI_32 ms);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RemainderMs
/// \~english @par Brief
///        Get the number of remaining milseconds out of whole second for the number passed.
/// \~english @param[in] ms
///        UI_32 - time value in mil seconds
/// \~english @retval Number of remaining mil seconds in the number passed.
/// \~english @par Preconditons
///         none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///         none
/// \~english @par Detail
///       This function returns the time value in ms of the remainder from converting the time value in ms to the time value in s.\n
///       For example, 12345ms obtains 345ms as the conversion result.
/// \~english @par Classification
/// \~english @par Type
/// \~english @see
///         none
/////////////////////////////////////////////////////////////////////////////////////
UI_32 RemainderMs(UI_32 ms);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup MSToNS
/// \~english @par Brief
///        Converts mil seconds to nano seconds.
/// \~english @param[in] ms
///        UI_32 - time value in mil seconds
/// \~english @retval Number of nano seconds there are in mil seconds in the number passed.
/// \~english @par Preconditons
///         none
/// \~english @par Change of internal status
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///         none
/// \~english @par Detail
///       This function converts a time value in ms to a time value in ns and returns it.\n
///       For example, 1234ms obtains 1234000000ns as the conversion result.
/// \~english @par Classification
/// \~english @par Type
/// \~english @see
///         none
/////////////////////////////////////////////////////////////////////////////////////
UI_64 MSToNS(UI_32 ms);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_TimerCreate
/// \~english @par Brief
///        to create an instance of a timer object, and start it
/// \~english @param[in] timer_info
///        NSTimerInfo - timer value represents an absolute expiration timeout
/// \~english @param[in] cbMech
///        eNSTimerCallbackMechanism - CALLBACKMECHANISM_ENUM - specifies the callback mechanism associated with the
///        timer\n
///                                    CALLBACK_MESSAGE   will send a message to a message queue
/// \~english @param[in] sndMqHndl
///        HANDLE - implies a MESSAGE QUE HANDLE generated by McOpenSender() is specified
/// \~english @retval Handle to the timer object or NULL on failure
/// \~english @par Preconditons
///         none
/// \~english @par Change of internal status
/// - The internal state is not changed.
/// \~english @par Conditions of processing failure
/// - If HANDLE(sndMqHndl) is NULL, [NULL]
/// - If the Callback mechanism (cbMech) specified in the arguments is not CALLBACK_MESSAGE [NULL]
/// - If thread creation fails (reateTimerMonitoringThread(); returns eFrameworkunifiedStatusFail) [NULL]
/// - Unable to allocate memory for timer handle structures [NULL]
/// - Memory cannot be allocated for the timer-information struct. [NULL]
/// - When the message queue name of the handle (sndMqHndl) specified in the arguments is NULL,[NULL]
/// - If a system call (such as epoll_ctl()) is in error,[NULL]
/// \~english @par Detail
///   Creates an instance of a timer object, sends a CALLBACK_MESSAGE message to the message queue, and starts the timer.\n
///   When McOpenSender() is executed to execute NS_TimerCreate, the handles should be closed if not needed.
/// \~english @par Classification
/// \~english @par Type
/// \~english @see NS_TimerDelete,NS_TimerSetTime
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE NS_TimerCreate(NSTimerInfo timer_info, eNSTimerCallbackMechanism cbMech, HANDLE sndMqHndl);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_TimerDelete
/// \~english @par Brief
///        to delete an instance of a timer object
/// \~english @param[in] hTimer
///        HANDLE - a timer handle that was created via NS_TimerCreate
/// \~english @retval EFrameworkunifiedStatus indicates if the timer handle was delete successfully
/// \~english @retval eFrameworkunifiedStatusOK
/// \~english @retval eFrameworkunifiedStatusFail
/// \~english @retval eFrameworkunifiedStatusInvldParam
/// \~english @par Preconditons
/// - NS_TimerCreate has generated a HANDLE of timers
/// \~english @par Change of internal status
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - If HANDLE(hTimer) is NULL,[eFrameworkunifiedStatusInvldParam]
///     - Initializes the state. [eFrameworkunifiedStatusFail]
/// - Invalid timed objects for HANDLE(hTimer specified by arguments:[eFrameworkunifiedStatusInvldParam]
/// - If semaphoring fails, [eFrameworkunifiedStatusFail]
/// \~english @par Detail
/// If the timer is set, the timer is stopped.\n
/// Delete an instance of a timer object.
/// \~english @par Classification
/// \~english @par Type
/// \~english @see NS_TimerCreate, NS_TimerSetTime
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NS_TimerDelete(HANDLE hTimer);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_TimerSetTime
/// \~english @par Brief
///        to start or stop a timer by setting the time interval associated
///           with a timer, value of 0 in t_sec & t_nsec, will stop the timer.
/// \~english @param[in] hTimer
///        HANDLE - handle to timer that your specifying the timer to be set
/// \~english @param[in] timer_info
///        NSTimerInfo - timer value represents an absolute expiration timeout
/// \~english @retval EFrameworkunifiedStatus indicates if the timer info was set successfully
/// \~english @retval eFrameworkunifiedStatusOK
/// \~english @retval eFrameworkunifiedStatusInvldHandle
/// \~english @retval eFrameworkunifiedStatusFail
/// \~english @par Preconditons
///        the timer must have already been created with NS_TimerCreate
/// \~english @par Change of internal status
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - If HANDLE(hTimer) is NULL,[eFrameworkunifiedStatusInvldHandle]
///     - Initializes the state. [eFrameworkunifiedStatusFail]
/// - Retrieving timers fails [eFrameworkunifiedStatusFail]
/// \~english @par Detail
/// When the timer information (t_sec and t_nsec) is 0, the timer is stopped.\n
/// If it is not 0, the timer is started based on the timer information.\n
/// The hTimer must have been created with NS_TimerCreate.
/// \~english @par Classification
/// \~english @par Type
/// \~english @see NS_TimerCreate, NS_TimerGetTime
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NS_TimerSetTime(HANDLE hTimer, NSTimerInfo timer_info);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_TimerGetTime
/// \~english @par Brief
///        to retrieve the time until a timer expires
/// \~english @param[in] hTimer
///        HANDLE - handle to timer that your trying to get information about
/// \~english @param[out] timer_info
///        NSTimerInfo* - a structure to store the delay time until the timer expires
/// \~english @retval EFrameworkunifiedStatus indicates if the timer info was get successfully
/// \~english @retval eFrameworkunifiedStatusOK
/// \~english @retval eFrameworkunifiedStatusInvldHandle
/// \~english @retval eFrameworkunifiedStatusFail
/// \~english @par Preconditons
///     - NS_TimerCreate has generated a HANDLE of timers
/// \~english @par Change of internal status
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - If HANDLE(hTimer) is NULL,[eFrameworkunifiedStatusInvldHandle]
/// - If the timer_info specified in the arguments is NULL,... [eFrameworkunifiedStatusInvldHandle]
///     - Initializes the state. [eFrameworkunifiedStatusFail]
///     - If the timerdelete fails.... [eFrameworkunifiedStatusFail]
/// \~english @par Detail
/// Gets the time until the timer expires.
/// \~english @par Classification
/// \~english @par Type
/// \~english @see NS_TimerCreate, NS_TimerSetTime
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NS_TimerGetTime(HANDLE hTimer, NSTimerInfo *timer_info);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_TimerDebugOn
/// \~english @par Brief
///        to make a flag for the debugging process. for debug.
/// \~english @param[in] FlagState
///        BOOL - DebugFlag state
/// \~english @retval none
/// \~english @par Preconditons
///     none
/// \~english @par Change of internal status
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     none
/// \~english @par Detail
///     Sets the debug flag specified by the argument.\n
/// However, if the set flag is the same as the flag specified in the argument, no operation is performed.
/// \~english @par Classification
/// \~english @par Type
/// \~english @see none
////////////////////////////////////////////////////////////////////////////////////////////
void NS_TimerDebugOn(BOOL FlagState);

#ifdef __cplusplus
}
#endif
#endif /* __NATIVESERVICES_TIMER_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
