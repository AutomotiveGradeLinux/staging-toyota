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

///////////////////////////////////////////////////////////////////////////////
/// brief    This file supports a generic timer abstraction.
///////////////////////////////////////////////////////////////////////////////

/**
 * @file ss_system_timer.h
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup system_manager
 *  @ingroup system_service
 *  @{
 */


#ifndef SS_SYSTEM_TIMER_H_  // NOLINT (build/header_guard)
#define SS_SYSTEM_TIMER_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <list>
#include <map>


const PCSTR TIMER_SERVICE_NAME = "TIMER";


/**
 * @class Timer
 * \~english @brief Timer
 * \~english @par   Brief Introduction
 *        Class to provide Timer function
 *
 */
class Timer {
 public:
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Timer
  /// \~english @par Summary
  ///       Default constructor of Timer class.
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       To generate a Timer class, and initialize member variables. \n
  ///       After the constructor, be sure to call the Initialize. \n
  /// \~english @see  ~Timer, Initialize
  ////////////////////////////////////////////////////////////////////////////////////
  Timer();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Timer
  /// \~english @par Summary
  ///       Constructor of Timer class.
  /// \~english @param  [in] hdl
  ///        HANDLE   - HANDLE for Application
  /// \~english @param  [in] id
  ///        UI_32   -  Timer ID corresponding to the callback function
  /// \~english @param  [in] CbFn
  ///        CbFuncPtr   - Pointer to a callback function to be called when the timer expires
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) has been
  ///       done.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       None
  /// \~english @par Detail
  ///       To generate a Timer class. \n
  ///       To initialize timer information structure of the member variables. \n
  ///       To initialize the member variables of the class with an argument. Therefore, there is no need to call the
  ///       Initialize() function when using this constructor.
  /// \~english @see  ~Timer, Initialize
  ////////////////////////////////////////////////////////////////////////////////////
  Timer(HANDLE hdl, UI_32 id, CbFuncPtr CbFn);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~Timer
  /// \~english @par Summary
  ///       Destructor of Timer class.
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       None
  /// \~english @par Detail
  ///       To delete a Timer class. \n
  ///       If the initialization is complete state, to end by calling the Shutdown().
  /// \~english @see  Timer
  ////////////////////////////////////////////////////////////////////////////////////
  virtual ~Timer();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Initialize
  /// \~english @par Summary
  ///       Initialization of the Timer class.
  /// \~english @param  [in] hApp
  ///        HANDLE   - HANDLE for Application
  /// \~english @param  [in] id
  ///        UI_32   -  Timer ID corresponding to the callback function
  /// \~english @param  [in] CbFn
  ///        CbFuncPtr   - Pointer to a callback function to be called when the timer expires
  /// \~english @retval TRUE  Initialization success
  /// \~english @retval FALSE Initialization failed
  /// \~english @par Preconditions
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) has been
  ///       done.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - Handle(hApp) that has been specified by the argument is NULL. [FALSE]
  ///       - Timer ID(id) that has been specified by the argument is 0. [FALSE]
  ///       - Pointer to a callback function that has been specified by the argument is NULL. [FALSE]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       To initialize the member variables of the following.
  ///       - HANDLE for Application
  ///       - Timer information structure
  ///       - Pointer to a callback function
  ///       - HANDLE of message queue
  ///       - Timer Object
  /// \~english @par
  ///       A timer class to initialization completion state.
  /// \~english @see  ~Timer, Shutdown
  ////////////////////////////////////////////////////////////////////////////////////
  BOOL Initialize(HANDLE hApp, UI_32 id, CbFuncPtr CbFn);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Reinitialize
  /// \~english @par Summary
  ///       - Reinitialize the timer by specifying a timer value information
  /// \~english @param [in] id
  ///       UI_32 - timer id that you want to reinitialize
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize().
  /// \~english @par Change of the internal state
  ///       - set m_tTi.iCmd with id
  ///       - using the result of function McOpenSender(FrameworkunifiedGetAppName(m_hApp)) to set m_hSnd
  ///       - create a timer and give the handle to m_hTmr
  ///       - set m_bInit with TRUE
  /// \~english @par Causes of failures
  ///       - None
  /// \~english @par Classification
  ///       - Public
  /// \~english @par Type
  ///       - Method only
  /// \~english @par Detail
  ///       Reinitialize the timer by specifying a timer value information, \n
  ///       create a new timer if it is not initialize.
  /// \~english @see Initialize
  ////////////////////////////////////////////////////////////////////////////////////
  void Reinitialize(UI_32 id);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Shutdown
  /// \~english @par Summary
  ///       The end of the Timer class.
  /// \~english @param  None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize().
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       In the case of initialization completion state, it is completed by the following process.\n
  ///       - Stop the Timer
  ///       - Delete Timer Object
  ///       - Set NULL to a timer object variable
  ///       - Close message queue
  ///       - Set NULL to a message queue HANDLE variable
  ///       - The timer class to uninitialized.
  /// \~english @par
  ///       \nIf you want to re-use this timer, call again Initialize() function.
  /// \~english @see  Timer, Initialize
  ////////////////////////////////////////////////////////////////////////////////////
  void Shutdown();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetTime
  /// \~english @par Summary
  ///       Set the timer value information.
  /// \~english @param  [in] ss
  ///        UI_32   -  Timeout expiration time(sec)
  /// \~english @param  [in] sms
  ///        UI_32   -  Timeout expiration time(nano sec)
  /// \~english @param  [in] rs
  ///        UI_32   - Timeout expiration time at the time of repeat(sec)
  /// \~english @param  [in] rms
  ///        UI_32   - Timeout expiration time at the time of repeat(nano sec)
  /// \~english @retval TRUE  Setting success
  /// \~english @retval FALSE Setting failed
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize().
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - Not been initialized. [FALSE]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       Set a timer value information specified by the argument to the timer information structure.
  /// \~english @see  Timer
  ////////////////////////////////////////////////////////////////////////////////////
  BOOL SetTime(UI_32 ss, UI_32 sms, UI_32 rs, UI_32 rms);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Start
  /// \~english @par Summary
  ///       The start of the timer by specifying a timer value information.
  /// \~english @param  [in] ss
  ///        UI_32   -  Timeout expiration time(sec)
  /// \~english @param  [in] sms
  ///        UI_32   -  Timeout expiration time(nano sec)
  /// \~english @param  [in] rs
  ///        UI_32   - Timeout expiration time at the time of repeat(sec)
  /// \~english @param  [in] rms
  ///        UI_32   - Timeout expiration time at the time of repeat(nano sec)
  /// \~english @retval TRUE  Stop success
  /// \~english @retval FALSE Stop failed
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize().
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - Not been initialized. [FALSE]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Method only
  /// \~english @par Detail
  ///       In the case of initialization completion state, start the timer. \n
  ///       After timer start, again, if you start the timer, it does not work guarantee.
  /// \~english @see  Stop
  ////////////////////////////////////////////////////////////////////////////////////
  BOOL Start(UI_32 ss, UI_32 sms, UI_32 rs, UI_32 rms);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Start
  /// \~english @par Summary
  ///       Stop the timer.
  /// \~english @param  None
  /// \~english @retval TRUE  Stop success
  /// \~english @retval FALSE Stop failed
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize().
  ///       - Set the timeout value in SetTime().
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - Not been initialized. [FALSE]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Method only
  /// \~english @par Detail
  ///       In the case of initialization completion state, start the timer. \n
  ///       After timer start, again, if you start the timer, it does not work guarantee.
  /// \~english @see  SetTime, Stop
  ////////////////////////////////////////////////////////////////////////////////////
  BOOL Start();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Stop
  /// \~english @par Summary
  ///       Stop the timer.
  /// \~english @param  None
  /// \~english @retval TRUE  Stop success
  /// \~english @retval FALSE Stop failed
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize().
  ///       - The timer has been started
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - Not been initialized. [FALSE]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Method only
  /// \~english @par Detail
  ///       In the case of initialization completion state, stop the timer.  \n
  ///       After the timer has stopped, again, if you stop the timer, it does not do anything to just processing
  ///       Internal error occurs.
  /// \~english @see  Start
  ////////////////////////////////////////////////////////////////////////////////////
  BOOL Stop();

 private:
  BOOL m_bInit;     /// flag that indicated if the class has been initialized or not
  HANDLE m_hApp;     /// refs to an applications hApp handle
  HANDLE m_hSnd;    /// refs to applications message queue
  HANDLE m_hTmr;    /// refs to a timer handle
  NSTimerInfo m_tTi;  /// timer parameters
  CbFuncPtr m_CbFn;  /// pointer to the callback method
};

/** @}*/  // end of Timer
/** @}*/  // end of system_manager
/** @}*/  // end of SystemService
/** @}*/  // end of BaseSystem

/**
 * @class TimerCtrl
 * \~english @brief TimerCtrl
 * \~english @par   Brief Introduction
 *        Class to provide TimerCtrl function
 *
 */
class TimerCtrl {
 public:
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup TimerCtrl
  /// \~english @par Summary
  ///       Default Constructor, called when the class is instantiated.
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///
  /// \~english @par Change of the internal state
  ///       - set m_hApp as NULL,m_nTimersMax as DEFAULT_NTIMERS
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       To generate a TimerCtrl class. \n
  ///       After the constructor, be sure to call the Initialize. \n
  /// \~english @see  ~TimerCtrl, Initialize
  ////////////////////////////////////////////////////////////////////////////////////
  TimerCtrl();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup TimerCtrl
  /// \~english @par Summary
  ///       Constructor, called when the class is instantiated.
  /// \~english @param  [in] ntimers
  ///           UI_32 - number of timers objects in the pool
  /// \~english @retval None
  /// \~english @par Preconditions
  ///
  /// \~english @par Change of the internal state
  ///       - initialize var m_hApp and m_nTimersMax
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       To generate a TimerCtrl class. \n
  ///       After the constructor, be sure to call the Initialize. \n
  /// \~english @see  ~TimerCtrl, Initialize
  ////////////////////////////////////////////////////////////////////////////////////
  TimerCtrl(UI_32 ntimers);  // NOLINT (runtime/explicit)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Initialize
  /// \~english @par Summary
  ///       Initialize the class after instantiation.
  /// \~english @param [in] hApp
  ///       HANDLE - HANDLE to you applications hApp.
  /// \~english @retval None
  /// \~english @par Preconditions
  ///
  /// \~english @par Change of the internal state
  ///       - initialize var m_hApp and m_aTimers
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       The function is to initialize the member val of the class with param hApp,\n
  ///       if hApp is not null, then set m_hApp with hApp and initialize m_aTimers.
  /// \~english @see  ~TimerCtrl, Initialize
  ////////////////////////////////////////////////////////////////////////////////////
  VOID Initialize(HANDLE hApp);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Shutdown
  /// \~english @par Summary
  ///       handle something when shut down timer control.
  /// \~english @param  None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize()..
  /// \~english @par Change of the internal state
  ///       - Clear m_aTimer,shut down timer in m_rTimer and delete it.clear m_rTimer.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       Completely shuts down the timer control, this method calls Stop, cleans up. You must call
  ///       Initialize again if you want reuse this class.
  /// \~english @see  ~TimerCtrl, Initialize
  ////////////////////////////////////////////////////////////////////////////////////
  VOID Shutdown();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CreateTimer
  /// \~english @par Summary
  ///       get a timer control from pool
  /// \~english @param [in] CbFn
  ///       CbFn - Pointer to a callback function to be called when the timer expires
  /// \~english @retval Timer id  - If id is in m_aTimers
  /// \~english @retval 0  - If id is not in m_aTimers
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize().
  /// \~english @par Change of the internal state
  ///       - insert one Timer to m_rTimers
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       gets a the first id from m_aTimers if m_aTimer is not empty, using this id to create a timer,\n
  ///       put this timer into m_rTimers,and return the id.
  /// \~english @see  DeleteTimer
  ////////////////////////////////////////////////////////////////////////////////////
  UI_32 CreateTimer(CbFuncPtr CbFn);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup DeleteTimer
  /// \~english @par Summary
  ///       delete a timer from m_rTimers using the param.
  /// \~english @param [in] id
  ///       UI_32 - timer id that you want to delete
  /// \~english @retval Timer id  -If id is in m_aTimers
  /// \~english @retval 0  - If id is not in m_aTimers
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize().
  /// \~english @par Change of the internal state
  ///       - delete one Timer to m_rTimers that id is the param.\n
  ///         and put id of the timer into m_aTimers.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       delete your timer and returns it's id to the pool of timers.
  /// \~english @see  CreateTimer
  ////////////////////////////////////////////////////////////////////////////////////
  UI_32 DeleteTimer(UI_32 id);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetTimer
  /// \~english @par Summary
  ///       Sets the timer timeout values
  /// \~english @param [in] id
  ///       UI_32 - timer id that you want to set
  /// \~english @param [in] ss
  ///       UI_32 - Start time in seconds
  /// \~english @param [in] sms
  ///       UI_32 - Start time in milliseconds
  /// \~english @param [in] rs
  ///       UI_32 - Repeat time in seconds
  /// \~english @param [in] rms
  ///       UI_32 - Repeat time in milliseconds
  /// \~english @retval Timer id  - If id is in m_aTimers
  /// \~english @retval 0 - If id is not in m_aTimers
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize().
  /// \~english @par Change of the internal state
  ///       - set the timer infomation in m_rTimers with ss, sms, rs, rms
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       using ss, sms, rs, rms to set the timer infomation, timer'is is param.
  /// \~english @see None 
  ////////////////////////////////////////////////////////////////////////////////////
  UI_32 SetTimer(UI_32 id, UI_32 ss, UI_32 sms, UI_32 rs, UI_32 rms);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup StartTimer
  /// \~english @par Summary
  ///       Sets the timer timeout values and starts the timer.
  /// \~english @param [in] id
  ///       UI_32 - timer id that you want to start
  /// \~english @param  [in] ss
  ///        UI_32   -  Timeout expiration time(sec)
  /// \~english @param  [in] sms
  ///        UI_32   -  Timeout expiration time(nano sec)
  /// \~english @param  [in] rs
  ///        UI_32   - Timeout expiration time at the time of repeat(sec)
  /// \~english @param  [in] rms
  ///        UI_32   - Timeout expiration time at the time of repeat(nano sec)
  /// \~english @retval Timer id  - If id is in m_aTimers
  /// \~english @retval 0  - If id is not in m_aTimers
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize().
  /// \~english @par Change of the internal state
  ///       - None
  /// \~english @par Causes of failures
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       In the case of initialization completion state, start the timer that id is the first param. \n
  ///       After timer start, again, if you start the timer, it does not work guarantee.
  /// \~english @see  Stop
  ////////////////////////////////////////////////////////////////////////////////////
  UI_32 StartTimer(UI_32 id, UI_32 ss, UI_32 sms, UI_32 rs, UI_32 rms);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup StartTimerMulti
  /// \~english @par Summary
  ///       The start of the timer by specifying a timer value information and reinitialize timer id.
  /// \~english @param [in] id
  ///       UI_32 - timer id that you want to start
  /// \~english @param  [in] ss
  ///        UI_32   -  Timeout expiration time(sec)
  /// \~english @param  [in] sms
  ///        UI_32   -  Timeout expiration time(nano sec)
  /// \~english @param  [in] rs
  ///        UI_32   - Timeout expiration time at the time of repeat(sec)
  /// \~english @param  [in] rms
  ///        UI_32   - Timeout expiration time at the time of repeat(nano sec)
  /// \~english @param [in] subId
  ///       UI_32 -  using subId to reinitialize timer id
  /// \~english @retval Timer id  - If id is in m_aTimers
  /// \~english @retval 0 - If id is not in m_aTimers
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize().
  /// \~english @par Change of the internal state
  ///       - None
  /// \~english @par Causes of failures
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       Finds the timer by specifying, shut down the timer, \n
  ///       and reinitialize it using new id of this timer and starts the timer
  /// \~english @see  Stop
  ////////////////////////////////////////////////////////////////////////////////////
  UI_32 StartTimerMulti(UI_32 id, UI_32 ss, UI_32 sms, UI_32 rs, UI_32 rms, UI_32 subId);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup StartTimer
  /// \~english @par Summary
  ///       Start the timer.
  /// \~english @param [in] id
  ///       UI_32 - timer id that you want to start
  /// \~english @retval Timer id  - If id is in m_aTimers
  /// \~english @retval 0  - If id is in not m_aTimers
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize().
  ///       - Set the timeout value in SetTimer().
  /// \~english @par Change of the internal state
  ///       - None
  /// \~english @par Causes of failures
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       In the case of initialization completion state, \n
  ///       start of the timer by specifying a timer value information.
  /// \~english @see  SetTime, Stop
  ////////////////////////////////////////////////////////////////////////////////////
  UI_32 StartTimer(UI_32 id);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup StopTimer
  /// \~english @par Summary
  ///       Stop the timer.
  /// \~english @param [in] id
  ///       UI_32 - timer id that you want to stop
  /// \~english @retval Timer id   - If id is in m_aTimers
  /// \~english @retval 0  - If id is not in m_aTimers
  /// \~english @par Preconditions
  ///       - That has been initialized in the Initialize().
  ///       - Set the timeout value in SetTimer().
  /// \~english @par Change of the internal state
  ///       - None
  /// \~english @par Causes of failures
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       In the case of start completion state, \n
  ///       stop of the timer by specifying a timer value information
  /// \~english @see  SetTimer, StartTimer
  ////////////////////////////////////////////////////////////////////////////////////
  UI_32 StopTimer(UI_32 id);

 private:
  HANDLE m_hApp;            ///
  UI_32 m_nTimersMax;          ///
  std::list<UI_32> m_aTimers;      ///
  std::map<UI_32, Timer*> m_rTimers;  ///
};


#endif /* SS_SYSTEM_TIMER_H_ */  // NOLINT (build/header_guard)
