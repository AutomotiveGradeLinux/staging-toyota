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
/// \brief    This file provides support for child process exit detection.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef SS_CRASH_DETECTOR_H_  // NOLINT
#define SS_CRASH_DETECTOR_H_

#include <string>

class CCrashDetector {
 public:
              CCrashDetector();
  virtual    ~CCrashDetector();
  EFrameworkunifiedStatus  CrashDetectorInitialize(HANDLE hApp);
  PCSTR       CrashDetectorGetThreadName();

  HANDLE      m_hSmQueue;

 private:
  SI_32       CrashDetectorGetThreadPriority();

  HANDLE      m_hApp;
  pthread_t   m_ThreadId;
  std::string m_ThreadName;
  SI_32       m_siPriority;
};

#endif  // SS_CRASH_DETECTOR_H_  // NOLINT
