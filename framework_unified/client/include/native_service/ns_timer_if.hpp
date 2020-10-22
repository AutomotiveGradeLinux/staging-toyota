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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NSTimer
/// \brief   Header file for Timer class.
///
//////////////////////////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file ns_timer_if.hpp
 * @brief \~english Header file for Timer class.
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
#ifndef NS_TIMER_IF_HPP_  // NOLINT  (build/header_guard)
#define NS_TIMER_IF_HPP_

#include <native_service/frameworkunified_types.h>
#include <native_service/ns_timer_if.h>

/**
 * @class NSTimer
 * \~english @brief Handle Timer
 * \~english @par   Brief Introduction
 *        Timer class is using to handle the timer.creating, starting, stopping,and so on.
 *
 */
class NSTimer {
 public:
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NSTimer
  /// \~english @par Brief
  ///        Creates a timer object with default behavior as single timeout timer.
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           -None
  /// \~english @par Change of internal status
  ///           None
  /// \~english @par Conditions of processing failure
  ///           -None
  /// \~english @par Detail
  ///           -To create the instance of this class.init the member var using m_hTimer, m_u64TimeInterval,
  ///           m_bRepeatTimer.\n
  ///            Default status,all time will be set 0.
  /// \~english @par Classification
  ///           Public
  /// \~english @par Type
  ///           -
  /// \~english @see ~NSTimer
  ////////////////////////////////////////////////////////////////////////////////////
  NSTimer();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NSTimer
  /// \~english @par Brief
  ///        Deletes a timer object.
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           -Instance of this class is created successfully.
  /// \~english @par Change of internal status
  ///           None
  /// \~english @par Conditions of processing failure
  ///           -None
  /// \~english @par Detail
  ///           -Destruct the instance of this class.
  /// \~english @par Classification
  ///           Public
  /// \~english @par Type
  ///           -None
  /// \~english @see NSTimer
  ////////////////////////////////////////////////////////////////////////////////////
  ~NSTimer();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NSTimer
  /// \~english @par Brief
  ///        This function overloads \ref SetNotifyMethod().
  ///        On timeout, sends the command id  to the message queue of specified application.
  ///        Resets previous notify method if any.
  /// \~english @param[in] notifyCmdId
  ///        UI_16 - command id.
  /// \~english @param[in] notifyToAppName
  ///        PCSTR - Send command id to the message queue of this application
  /// \~english @retval EFrameworkunifiedStatus - Returns eFrameworkunifiedStatusOK if the action is set otherwise eFrameworkunifiedStatusFail
  /// \~english @retval eFrameworkunifiedStatusOK - Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam - Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusFail - process error
  /// \~english @par Preconditons
  ///           -None
  /// \~english @par Change of internal status
  ///           None
  /// \~english @par Conditions of processing failure
  ///       - Pointer of AppName specified by argument(notifyToAppName) is NULL [eFrameworkunifiedStatusInvldParam]
  ///       - Bytes of AppName specified by argument(notifyToAppName) are larger then MAX_SERVICE_NAME(15byte)
  ///       [eFrameworkunifiedStatusInvldParam]
  ///       - There isn't enough buffer(malloc) to storage message queue name, when getting(McOpenSender) message queue
  ///       handle [eFrameworkunifiedStatusFail]
  ///       - Bytes of message queue name are larger then LIMIT_QUEUE_NAME_SIZE(16byte), when getting(McOpenSender)
  ///       message queue handle [eFrameworkunifiedStatusFail]
  ///       - Failed to get message queue(mq_open), when getting(McOpenSender) message queue handle [eFrameworkunifiedStatusFail]
  ///       - There isn't enough buffer(malloc) to storage message queue information, when getting(McOpenSender) message
  ///       queue handle [eFrameworkunifiedStatusFail]
  ///       - Failed to create(epoll_create1) file descriptor for epoll, when creating timer handle(NS_TimerCreate)
  ///       [eFrameworkunifiedStatusFail]
  ///       - Failed to create(eventfd) file descriptor for event notification, when creating timer
  ///       handle(NS_TimerCreate) [eFrameworkunifiedStatusFail]
  ///       - Failed to connect(epoll_ctl) file descriptors for epoll and event notification, when creating timer
  ///       handle(NS_TimerCreate) [eFrameworkunifiedStatusFail]
  ///       - Failed to create(pthread_create) timer thread for event notification, when creating timer
  ///       handle(NS_TimerCreate) [eFrameworkunifiedStatusFail]
  ///       - There isn't enough buffer(malloc) to storage timer handle structure, when creating timer
  ///       handle(NS_TimerCreate) [eFrameworkunifiedStatusFail]
  ///       - There isn't enough buffer(malloc) to storage timer information structure,, when creating timer
  ///       handle(NS_TimerCreate) [eFrameworkunifiedStatusFail]
  ///       - Message queue handle is INVALID_HANDLE(-1), when creating timer handle(NS_TimerCreate) [eFrameworkunifiedStatusFail]
  ///       - Checking code of message queue handle is not MQ_CHECK_CODE, when creating timer handle(NS_TimerCreate)
  ///       [eFrameworkunifiedStatusFail]
  ///       - Type of message queue handle is not send/receive, when creating timer handle(NS_TimerCreate)
  ///       [eFrameworkunifiedStatusFail]
  ///       - Failed to create(timerfd_create) file descriptor for timer, when creating timer handle(NS_TimerCreate)
  ///       [eFrameworkunifiedStatusFail]
  ///       - Failed to connect(epoll_ctl) file descriptors for epoll and timer, when creating timer
  ///       handle(NS_TimerCreate) [eFrameworkunifiedStatusFail]
  ///       - Timer handle state of instance has been deleted(DELETED_STATE), when creating timer handle(NS_TimerCreate)
  ///       [eFrameworkunifiedStatusFail]
  ///       - Failed to start/stop(timerfd_settime) timer, when creating timer handle(NS_TimerCreate) [eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///        This function overloads \ref SetNotifyMethod().
  ///        On timeout, sends the command id  to the message queue of specified application.
  ///        Create a timer handle(m_hTimer) of instance, if the time handle already exits, delete it then create a new
  ///        one.
  ///        If there is resource for timer notification before, reset it.
  /// \~english @par Classification
  ///           Public
  /// \~english @par Type
  ///           -None
  /// \~english @see McOpenSender, McClose, NS_TimerCreate
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetNotifyMethod(UI_16 notifyCmdId, PCSTR notifyToAppName);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NSTimer
  /// \~english @par Brief
  ///        Sets the timer as a repeat timer. Timer timeouts repeatedly as per the interval set.
  ///        Default timer fires only once.
  ///        Not effective, if the timer is running. Effective on next \ref Start() or \ref Start(seconds, msecs) call.
  /// \~english @param[in] repeatTimer
  ///        BOOL - TRUE -  set repeat timer
  ///          FALSE - set single timeout timer. (Default behavior)
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           -None
  /// \~english @par Change of internal status
  ///           -None
  /// \~english @par Conditions of processing failure
  ///           -None
  /// \~english @par Detail
  ///        Sets the timer as a repeat timer. Timer timeouts repeatedly as per the interval set.
  ///        Default timer fires only once.
  ///        Not effective, if the timer is running. Effective on next \ref Start() or \ref Start(seconds, msecs) call.
  /// \~english @par Classification
  ///           Public
  /// \~english @par Type
  ///           -None
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////
  VOID SetRepeatTimer(BOOL repeatTimer);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NSTimer
  /// \~english @par Brief
  ///        Set the interval of timeout. Interval is addition of seconds and milliseconds passed as an arguments.
  /// \~english @param[in] seconds
  ///        UI_32 - Time in seconds
  /// \~english @param[in] msecs
  ///        UI_64 - Time in milliseconds
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           -None
  /// \~english @par Change of internal status
  ///           None
  /// \~english @par Conditions of processing failure
  ///           -None
  /// \~english @par Detail
  ///           Set the interval of timeout. Interval is addition of seconds and milliseconds passed as an arguments.
  ///           The default value of interval time is 0.
  ///           If interval time is 0, stop timer.
  /// \~english @par Classification
  ///           Public
  /// \~english @par Type
  ///           -None
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////
  VOID SetTime(UI_32 seconds, UI_64 msecs);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NSTimer
  /// \~english @par Brief
  ///        This function overloads SetTime(). Set the interval of timeout. Interval is addition of hrs,
  ///        mins, seconds and milliseconds passed as an arguments.
  /// \~english @param[in] hrs
  ///        UI_32 - Time in hours
  /// \~english @param[in] mins
  ///        UI_64 - Time in minutes
  /// \~english @param[in] seconds
  ///        UI_32 - Time in seconds
  /// \~english @param[in] msecs
  ///        UI_64 - Time in milliseconds
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           -None
  /// \~english @par Change of internal status
  ///           None
  /// \~english @par Conditions of processing failure
  ///           -None
  /// \~english @par Detail
  ///        This function overloads SetTime(). Set the interval of timeout. Interval is addition of hrs,
  ///        mins, seconds and milliseconds passed as an arguments.
  ///        The default value of interval time is 0.
  ///        If interval time is 0, stop timer.
  /// \~english @par Classification
  ///           Public
  /// \~english @par Type
  ///           -None
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetTime(UI_32 hrs, UI_32 mins, UI_32 seconds, UI_64 msecs);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NSTimer
  /// \~english @par Brief
  ///        Starts or restarts timer with specified time interval. Interval is addition of seconds and
  ///        milliseconds passed as an arguments.
  /// \~english @param[in] seconds
  ///        UI_32 - Time in seconds
  /// \~english @param[in] msecs
  ///        UI_64 - Time in milliseconds
  /// \~english @retval EFrameworkunifiedStatus - Returns eFrameworkunifiedStatusOK on success otherwise eFrameworkunifiedStatusFail
  /// \~english @par Preconditons
  ///           - Create a timer handle of instance by SetNotifyMethod()
  /// \~english @par Change of internal status
  ///           None
  /// \~english @par Conditions of processing failure
  ///       - Timer handle of instance is NULL [eFrameworkunifiedStatusFail]
  ///       - Timer handle state of instance has been deleted(DELETED_STATE) [eFrameworkunifiedStatusFail]
  ///       - Failed to start/stop(timerfd_settime) timer [eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///        Starts or restarts timer with specified time interval. Interval is addition of seconds and
  ///        milliseconds passed as an arguments.
  ///        Interval time setted by this method will overwrite interval time setted by SetTime(UI_32, UI_64).
  ///        If interval time is 0, stop timer.
  /// \~english @par Classification
  ///           Public
  /// \~english @par Type
  ///           - None
  /// \~english @see SetTime(UI_32, UI_64), Start, Stop, SetNotifyMethod
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Start(UI_32 seconds, UI_64 msecs);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NSTimer
  /// \~english @par Brief
  ///        This function overloads Start(). Starts or restarts timer with interval specified using SetTime()
  ///        or last interval (if the timer was started before using Start(seconds, msecs)).
  /// \~english @retval EFrameworkunifiedStatus - Returns eFrameworkunifiedStatusOK on success otherwise eFrameworkunifiedStatusFail
  /// \~english @par Preconditons
  ///           - Create a timer handle of instance by SetNotifyMethod()
  /// \~english @par Change of internal status
  ///           None
  /// \~english @par Conditions of processing failure
  ///       - Timer handle of instance is NULL [eFrameworkunifiedStatusFail]
  ///       - Timer handle state of instance has been deleted(DELETED_STATE) [eFrameworkunifiedStatusFail]
  ///       - Failed to start/stop(timerfd_settime) timer [eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///       - Starts or restarts timer with interval specified using SetTime(UI_32, UI_64) or Start(UI_32, UI_64).
  ///         If interval time is not setted, default value is 0. If interval time is 0, stop timer.
  /// \~english @par Classification
  ///           Public
  /// \~english @par Type
  ///           - None
  /// \~english @see SetTime(UI_32, UI_64), Stop, SetNotifyMethod, WholeSeconds, MSToNS, NS_TimerSetTime
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Start();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NSTimer
  /// \~english @par Brief
  ///        Stops the timer
  /// \~english @retval EFrameworkunifiedStatus - Returns eFrameworkunifiedStatusOK on success otherwise eFrameworkunifiedStatusFail
  /// \~english @par Preconditons
  ///           - Create a timer handle of instance by SetNotifyMethod()
  /// \~english @par Change of internal status
  ///           None
  /// \~english @par Conditions of processing failure
  ///       - Timer handle of instance is NULL [eFrameworkunifiedStatusFail]
  ///       - Timer handle state of instance has been deleted(DELETED_STATE) [eFrameworkunifiedStatusFail]
  ///       - Failed to stop(timerfd_settime) timer [eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///           - Stops the timer
  /// \~english @par Classification
  ///           Public
  /// \~english @par Type
  ///           - None
  /// \~english @see Start, Start(UI_32, UI_64), SetNotifyMethod, NS_TimerSetTime
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Stop();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NSTimer
  /// \~english @par Brief
  ///        Returns timer running status
  /// \~english @retval BOOL - TRUE if timer is running else FALSE
  /// \~english @par Preconditons
  ///           -None
  /// \~english @par Change of internal status
  ///           None
  /// \~english @par Conditions of processing failure
  ///           -None
  /// \~english @par Detail
  ///           -Returns timer running status
  /// \~english @par Classification
  ///           Public
  /// \~english @par Type
  ///           -None
  /// \~english @see Start, Start(UI_32, UI_64), SetNotifyMethod, NS_TimerSetTime
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsRunning();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NSTimer
  /// \~english @par Brief
  ///           Returns timeout interval in milliseconds
  /// \~english @retval UI_64 - Timeout interval in milliseconds
  /// \~english @par Preconditons
  ///           -None
  /// \~english @par Change of internal status
  ///           None
  /// \~english @par Conditions of processing failure
  ///           -None
  /// \~english @par Detail
  ///           - Returns timeout interval in milliseconds
  /// \~english @par Classification
  ///           Public
  /// \~english @par Type
  ///           -None
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_64 GetInterval();

 private:

  HANDLE  m_hTimer;  ///< Corresponding NS timer handle

  UI_64   m_u64TimeInterval; ///< time interval in milliseconds

  BOOL    m_bRepeatTimer; ///< if it is a repeat timer. Default value is false.
};


#endif  // NS_TIMER_IF_HPP_  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
