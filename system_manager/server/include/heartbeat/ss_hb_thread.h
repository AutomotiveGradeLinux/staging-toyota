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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SystemManager
/// \brief    This file provides support for the application heartbeat system.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef HEARTBEAT_WORKER_H_  // NOLINT
#define HEARTBEAT_WORKER_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_multithreading.h>
#include <system_service/ss_system_timer.h>

#include "ss_hb_session.h"
#include "ss_sm_config.h"


class CHeartBeatThread {
 public:
  explicit CHeartBeatThread(HANDLE f_hThread);
  virtual ~CHeartBeatThread();

  ///////////////////////////
  /// HBPublishAvailabilityStatus
  /// At HeartBeat sends NTFY_Availability with availability
  /// notification to alert client that HeartBeat is available/not available.
  ///
  /// \param [in] hThread
  ///         HANDLE - Handle of HeartBeat
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus HBPublishAvailabilityStatus(HANDLE hThread, BOOL f_AvailabiltyStatus);

  ///////////////////////////
  /// HBOnStartThread
  /// Creates and starts the HeartBeat timer
  ///
  /// \param [in] hThread
  ///         HANDLE - Handle of HeartBeat
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus HBOnStartThread(HANDLE hThread);


  ///////////////////////////
  /// HBOnStopThread
  /// Stops and deletes the HeartBeat timer
  ///
  /// \param [in] hThread
  ///         HANDLE - Handle of HeartBeat
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus HBOnStopThread(HANDLE hThread);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// HBOnPrintConnections
  /// Prints all active sessions connected to HeartBeat
  ///
  /// \param [in] hThread
  ///         HANDLE - Handle of HeartBeat
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus HBOnPrintConnections(HANDLE hThread);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// HBOnPrintStack
  /// Prints all sessions connected to HeartBeat
  ///
  /// \param [in] hThread
  ///         HANDLE - Handle of HeartBeat
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus HBOnPrintStack(HANDLE hThread);

  ///////////////////////////
  /// HBOnTimerExpiry
  /// When APP timer expires, this method sends the HeartBeat request to the client.
  ///
  /// \param [in] hThread
  ///         HANDLE - Handle of HeartBeat
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus HBOnTimerExpiry(HANDLE hThread);

  ///////////////////////////
  /// HBOnClientResponse
  /// When the response from a client is received, this method updates the map
  /// containing client status to indicate the response.
  ///
  ///
  /// \param [in] hThread
  ///         HANDLE - Handle of HeartBeat
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus HBOnClientResponse(HANDLE hThread);

  ///////////////////////////
  /// HBOnPeriodicStatusRequest
  /// Reply to Status request from System Manager
  ///
  /// \param [in] hThread
  ///         HANDLE - Handle of HeartBeat
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus HBOnPeriodicStatusRequest(HANDLE hThread);


  ///////////////////////////
  /// HBOnAvailCheckRequest
  /// Reply to service's availability condition request from System Manager
  ///
  /// \param [in] hThread
  ///         HANDLE - Handle of HeartBeat
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus HBOnAvailCheckRequest(HANDLE hThread);

  ///////////////////////////
  /// HBOnRemoveModule
  /// Upon receiving delete module notification from sysmgr, this method removes
  /// a client from the client session map.
  ///
  /// \param [in] hThread
  ///         HANDLE - Handle of HeartBeat
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus HBOnRemoveModule(HANDLE hApp);

  ///////////////////////////
  /// HBOnAppendModule
  /// Upon receiving append module notification from sysmgr, this method append
  /// a client from the client session map.
  ///
  /// \param [in] hThread
  ///         HANDLE - Handle of HeartBeat
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus HBOnAppendModule(HANDLE hApp);

  // Service notification handlers
  EFrameworkunifiedStatus HBSendRequest(HANDLE hApp);
  EFrameworkunifiedStatus HBCheckHeartBeatResponses(HANDLE hThread);
  EFrameworkunifiedStatus HBDeleteRegisteredClientEntry(PSTR pModuleName);

  EFrameworkunifiedStatus HeartBeatTimerInit(HANDLE hThread);
  EFrameworkunifiedStatus HeartBeatTimersDelete(void);

 private:
  CHeartBeatSessionHandler m_oSessionHandler;
  TimerCtrl                m_oTimerCtrl;
  UI_32                    m_HBTimerID;
  HBConfigParameters       m_HBConfigParams;
  SI_32                    m_NextChkIndex;
  HANDLE                   m_hThread;
};
#endif  // HEARTBEAT_WORKER_H_  // NOLINT
