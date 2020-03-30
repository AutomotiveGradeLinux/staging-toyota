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
/// \ingroup  tag_PowerService
/// \brief    This file supports power service hysteresis management.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef POWER_SERVICE_SERVER_INCLUDE_SS_POWER_STATE_HYSTERESIS_H_
#define POWER_SERVICE_SERVER_INCLUDE_SS_POWER_STATE_HYSTERESIS_H_

#include "ss_power_config.h"

class PowerStateHysteresis {
 public:
  PowerStateHysteresis()
      : m_unTries(0) {
    bzero(&m_tHysteresisInfo, sizeof(m_tHysteresisInfo));
  }

  virtual ~PowerStateHysteresis() {  // LCOV_EXCL_START 14: do it when power off
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    m_unTries = 0;
  }
  // LCOV_EXCL_STOP 14: do it when power off
  explicit PowerStateHysteresis(PowerConfigParams::PowerOffInfo & info)  // NOLINT (runtime/references)
      : m_unTries(0),
        m_tHysteresisInfo(info) {
  }
  void set(PowerConfigParams::PowerOffInfo & info) {  // NOLINT (runtime/references)
    m_tHysteresisInfo = info;
  }
  BOOL getEnabled() const {
    return m_tHysteresisInfo.hysteresis.enabled;
  }
  UI_32 getTimeout() const {
    return m_tHysteresisInfo.hysteresis.timeout;
  }
  UI_32 getTries() const {
    return m_tHysteresisInfo.hysteresis.tries;
  }

  void clearTryCounter() {
    m_unTries = 0;
  }
  void bumbTryCounter() {
    m_unTries++;
  }
  BOOL maxTries() const {
    if (m_unTries < getTries())
      return FALSE;
    else
      return TRUE;
  }

 private:
  UI_32 m_unTries;
  PowerConfigParams::PowerOffInfo m_tHysteresisInfo;
};

#endif  // POWER_SERVICE_SERVER_INCLUDE_SS_POWER_STATE_HYSTERESIS_H_
