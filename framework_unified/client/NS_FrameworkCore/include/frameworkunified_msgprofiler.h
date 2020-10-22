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
/// \ingroup  tag_NSFramework
/// \brief    File declares FrameworkunifiedMsgProfiler class which keeps profiler information for message dispatcher
///
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_MSGPROFILER_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_MSGPROFILER_H_

#include <native_service/frameworkunified_types.h>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Profiler class which keeps and manage profiler information for message dispatcher
///////////////////////////////////////////////////////////////////////////////////////////////////
class FrameworkunifiedMsgProfiler {
 public:
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// FrameworkunifiedMsgProfiler
  /// Parameterized constructor
  /// \param [in] f_cAppName
  ///     const std::string& - Name of the application or dispatcher
  ///
  /// \return none
  ///////////////////////////////////////////////////////////////////////////////////////////
  explicit FrameworkunifiedMsgProfiler(const std::string &f_cAppName);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// ~FrameworkunifiedMsgProfiler
  /// Class destructor
  ///
  /// \return none
  ///////////////////////////////////////////////////////////////////////////////////////////
  ~FrameworkunifiedMsgProfiler();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// GetAppName
  /// Returns application name corresponding to profiler
  ///
  /// \param [in] none
  ///
  /// \return std::string
  ///         std::string - Returns application name corresponding to profiler
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  std::string GetAppName();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// AddChildName
  /// Returns application name corresponding to profiler
  ///
  /// \param [in] f_cChildName
  ///      const std::string& - Add the name of child dispatcher profiler
  ///
  /// \return none
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  VOID AddChildName(const std::string &f_cChildName);

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// MsgReceived
  /// Updates the profiler information once the message is received.
  /// e.g. increaments msg count, save the time of msg received
  ///
  /// \param [in] none
  ///
  /// \return none
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  VOID MsgReceived();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// MsgProcessed
  /// Updates the profiler information once the message is processed.
  /// e.g. Updates the frequency of execution of message
  ///
  /// \param [in] none
  ///
  /// \return none
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  VOID MsgProcessed();

  ///////////////////////////////////////////////////////////////////////////////////////////
  /// PrintProfileInfo
  /// All the profiler information which FrameworkunifiedMsgProfiler class has prints it in message
  /// queue FRAMEWORKUNIFIED_NS_MSGPROFILERUTIL
  ///
  /// \param [in] f_hApp
  ///         HANDLE - Application framework handle
  ///
  /// \return status
  ///         EFrameworkunifiedStatus - success or failure
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus PrintProfileInfo(HANDLE f_hApp);

  static BOOL m_bMsgProfilerEnabled;  /// Flag is enabled when command line argument -q is passed.
  /// Otherwise no profiler information is maintained.

 private:
  ///////////////////////////////////////////////////////////////////////////////////////////
  /// GetClock
  /// GetClock of milliseconds
  ///
  /// \param [in] none
  ///
  /// \return UI_64
  ///         UI_64 - returns time in Milliseconds
  ///
  ///////////////////////////////////////////////////////////////////////////////////////////
  UI_64 GetClock();

  std::vector<std::string> m_vChildNames;  /// vector to save list of all child threads dispatcher
  std::string m_cAppName;  /// Dispacther application name
  UI_32 m_ui32TotalNoOfMsgsReceived;  /// Total number of messages received by the dispatcher

  // NOTE: Storing time in ClockCycle. So that it will not add overhead for converting it to miliseconds.
  //      It will get converted only when information is required to print.
  UI_64 m_ui64MinMsgProcessingTime;  // time is in clock cycles
  UI_64 m_ui64MaxMsgProcessingTime;  // time is in clock cycles

  UI_64 m_ui64TotalMsgExecutionTime;  /// total time spent in message execution
  UI_64 m_ui64LastMsgReceivedTime;  /// save the time when last message was received
  UI_64 m_ui64AppInitTime;  /// save the time when application got initialized
  BOOL  m_bLastMsgWasPrintProfile;  /// TRUE if the last message received
};

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_MSGPROFILER_H_
