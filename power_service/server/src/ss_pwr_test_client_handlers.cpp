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
/// \ingroup  tag_PowerService
/// \brief    This file supports the power service test client.
///
///////////////////////////////////////////////////////////////////////////////

#include "ss_power.h"
#include <system_service/ss_power_service_local.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <other_service/itoa.h>
#include <map>
#include <string>
#include "ss_power_powerservicelog.h"

using namespace std;  // NOLINT (build/namespaces)

#define SS_CRANK_STATE_STRING_LENGTH      5
#define SS_VOLTAGE_STATE_STRING_LENGTH      5
#define SS_MODULE_STATE_STRING_LENGTH      5
#define SS_HEX_BASE_VALUE            16

const CHAR ReeadyToWakeUpDescription[] = "_READY_TO_WAKEUP:";
const CHAR WakeUpInitiatedDescription[] = "_WAKEUP_INITIATED:";
const CHAR WakeUpCompletedDescription[] = "_WAKEUP_COMPLETED:";
const CHAR PowerOnCompletedDescription[] = "_POWER_ON_COMPLETED:";
const CHAR PowerOffInitiatedDescription[] = "_POWER_OFF_INITIATED:";
const CHAR PowerOffCompletedDescription[] = "_POWER_OFF_COMPLETED:";
const CHAR ShutdownInitiatedDescription[] = "_SHUTDOWN_INITIATED:";
const CHAR ShutdownCompletedDescription[] = "_SHUTDOWN_COMPLETED:";

const CHAR CrankInvalidDescription[] = "_INVALID:";
const CHAR CrankEntryDescription[] = "_ENTRY:";
const CHAR CrankExitDescription[] = "_EXIT:";

const CHAR VoltageInvalidDescription[] = "_INVALID:";
const CHAR VoltageNormalDescription[] = "_NORMAL:";
const CHAR VoltageLVI1Description[] = "_LVI1:";
const CHAR VoltageLVI2Description[] = "_LVI2:";

///////////////////////////////////////////////////////////////////////
/// AddCrankInformationToResponse
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::AddCrankInformationToResponse(CHAR *f_MessageResponse) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");
  CHAR l_Buffer[SS_CRANK_STATE_STRING_LENGTH] = { 0 };
  UI_32 l_Index = 0;

  std::map<ePowerSrvCrankStates, std::string> CrankMap;
  CrankMap[epscsINVALID] = CrankInvalidDescription;
  CrankMap[epscsENTRY] = CrankEntryDescription;
  CrankMap[epscsEXIT] = CrankExitDescription;

  // confirm that CrankState value would fit in buffer array
  if (m_CrankState < 0xFFFF) {  // LCOV_EXCL_BR_LINE 8: dead code
    itoa(m_CrankState, l_Buffer, SS_HEX_BASE_VALUE);
    while (l_Buffer[l_Index]) {
      l_Buffer[l_Index] = toupper(l_Buffer[l_Index]);
      l_Index++;
    }
  }

  if (MaxRespMsg > (strlen(f_MessageResponse) + strlen(CrankMap[m_CrankState].c_str()) + strlen(l_Buffer))) {  // LCOV_EXCL_BR_LINE 8: dead code  // NOLINT[whitespace/line_length]
    strcat(f_MessageResponse, l_Buffer);  // NOLINT (runtime/printf)
    strcat(f_MessageResponse, CrankMap[m_CrankState].c_str());  // NOLINT (runtime/printf)
  } else {
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        "Buffer Overrun Condition : Failed to add Crank State description to response");
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
///////////////////////////////////////////////////////////////////////
/// AddVoltageInformationToResponse
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::AddVoltageInformationToResponse(CHAR *f_MessageResponse) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");
  CHAR l_Buffer[SS_VOLTAGE_STATE_STRING_LENGTH] = { 0 };
  UI_32 l_Index = 0;

  std::map<ePowerSrvVoltageStates, std::string> VoltageMap;
  VoltageMap[epsvsINVALID] = VoltageInvalidDescription;
  VoltageMap[epsvsNORMAL] = VoltageNormalDescription;
  VoltageMap[epsvsLVI1] = VoltageLVI1Description;
  VoltageMap[epsvsLVI2] = VoltageLVI2Description;

  // confirm that VoltageState value would fit in buffer array
  if (m_VoltageState < 0xFFFF) {  // LCOV_EXCL_BR_LINE 8: dead code
    itoa(m_VoltageState, l_Buffer, SS_HEX_BASE_VALUE);
    while (l_Buffer[l_Index]) {
      l_Buffer[l_Index] = toupper(l_Buffer[l_Index]);
      l_Index++;
    }
  }

  if (MaxRespMsg > (strlen(f_MessageResponse) + strlen(VoltageMap[m_VoltageState].c_str()) + strlen(l_Buffer))) {  // LCOV_EXCL_BR_LINE 8: dead code  // NOLINT[whitespace/line_length]
    strcat(f_MessageResponse, l_Buffer);  // NOLINT (runtime/printf)
    strcat(f_MessageResponse, VoltageMap[m_VoltageState].c_str());  // NOLINT (runtime/printf)
  } else {
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        "Buffer Overrun Condition : Failed to add Voltage State description to response");
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
///////////////////////////////////////////////////////////////////////
/// AddStateInformationToResponse
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::AddStateInformationToResponse(CHAR *f_MessageResponse) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");
  CHAR l_Buffer[SS_MODULE_STATE_STRING_LENGTH] = { 0 };
  UI_32 l_Index = 0;

  std::map<SS_PSState, std::string> StateMap;   // State Map
  StateMap[SS_PS_READY_TO_WAKEUP] = ReeadyToWakeUpDescription;
  StateMap[SS_PS_WAKEUP_INITIATED] = WakeUpInitiatedDescription;
  StateMap[SS_PS_WAKEUP_COMPLETE] = WakeUpCompletedDescription;
  StateMap[SS_PS_POWER_ON_COMPLETE] = PowerOnCompletedDescription;
  StateMap[SS_PS_POWER_OFF_INITIATED] = PowerOffInitiatedDescription;
  StateMap[SS_PS_POWER_OFF_COMPLETE] = PowerOffCompletedDescription;
  StateMap[SS_PS_SHUTDOWN_INITIATED] = ShutdownInitiatedDescription;
  StateMap[SS_PS_SHUTDOWN_COMPLETE] = ShutdownCompletedDescription;

  // confirm that PowerState value would fit in buffer array
  if (m_PowerState < 0xFFFF) {  // LCOV_EXCL_BR_LINE 8: dead code
    itoa(m_PowerState, l_Buffer, SS_HEX_BASE_VALUE);
    while (l_Buffer[l_Index]) {
      l_Buffer[l_Index] = toupper(l_Buffer[l_Index]);
      l_Index++;
    }
  }

  if (MaxRespMsg > (strlen(f_MessageResponse) + strlen(StateMap[m_PowerState].c_str()) + strlen(l_Buffer))) {  // LCOV_EXCL_BR_LINE 8: dead code  // NOLINT[whitespace/line_length]
    strcat(f_MessageResponse, l_Buffer);  // NOLINT (runtime/printf)
    strcat(f_MessageResponse, StateMap[m_PowerState].c_str());  // NOLINT (runtime/printf)
  } else {
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        "Buffer Overrun Condition : Failed to add Module State description to response");
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// ConstructPwrStateResponse
///
///
//////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus Power::ConstructPwrStateResponse(CHAR *f_MessageResponse) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");

  if (f_MessageResponse == NULL) {  // LCOV_EXCL_BR_LINE 8: dead code
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return eFrameworkunifiedStatusFail;
  }

  if (eFrameworkunifiedStatusOK == (l_eStatus = AddStateInformationToResponse(f_MessageResponse))) {  // LCOV_EXCL_BR_LINE 8: dead code  // NOLINT[whitespace/line_length]
    if (eFrameworkunifiedStatusOK == (l_eStatus = AddVoltageInformationToResponse(f_MessageResponse))) {  // LCOV_EXCL_BR_LINE 8: dead code  // NOLINT[whitespace/line_length]
      if (eFrameworkunifiedStatusOK == (l_eStatus = AddCrankInformationToResponse(f_MessageResponse))) {  // LCOV_EXCL_BR_LINE 8: dead code  // NOLINT[whitespace/line_length]
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Response Constructed");
      }
    }
  }

  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
