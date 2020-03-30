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

#include <fstream>
#include <string.h>
#include <agl_thread.h>
#include <other_service/VP_GetEnv.h>

#include <system_service/ss_templates.h>
#include <system_service/ss_sm_ram_access.h>
#include <system_service/ss_ver.h>

#include "system_service/ss_package.h"
#include "system_service/ss_access_if_private.h"
#include "system_service/ss_system_manager_conf.h"

#ifndef ZONE_WARN
#define ZONE_WARN     ZONEMASK(30)
#endif
#ifndef ZONE_INFO
#define ZONE_INFO     ZONEMASK(29)
#endif

/* Environment variable name */
const CHAR BoardEnvVariable[] = "VEHICLEPARAMETERLIBRARY_BOARD";
const CHAR LaunchConfigFileNameEnvVariable[] = "APPS_CONFIG_FILENAME";
const CHAR AreaTypeEnvVariable[] = "VEHICLEPARAMETERLIBRARY_AREA";
const CHAR BrandTypeEnvVariable[] = "VEHICLEPARAMETERLIBRARY_BRAND";
const CHAR GradeEnvVariable[] = "VP_VEHICLEPARAMETERLIBRARY_grade";

/* Environment variable value */
const CHAR EnvValueYes[] = "y";
const CHAR EnvValueNo[] = "n";

const CHAR EnvValueAvailable[]   = "available";
const CHAR EnvValueUnavailable[] = "unavailable";

/*
 *  Note.
 *  Define areas, brands, and grades by the vendor.
 *  For example, an area is a destination of a product.
 *  Examples of areas : JP(Japan), EU, etc.
 */
const CHAR EnvValueArea1[] = "Area1";
const CHAR EnvValueArea2[] = "Area2";
const CHAR EnvValueArea3[] = "Area3";
const CHAR EnvValueArea4[] = "Area4";
const CHAR EnvValueBrand1[] = "Brand1";
const CHAR EnvValueBrand2[] = "Brand2";
const CHAR EnvValueGrade1[] = "Grade1";
const CHAR EnvValueGrade2[] = "Grade2";
const CHAR EnvValueGrade3[] = "Grade3";

/* Package information */
const CHAR PkgInfoGen2[] = "Gen2";


typedef struct T_SS_SM_CONF_CTX {
  EFrameworkunifiedStatus (*cbRebootNoticeFunc)(HANDLE hApp);
  T_SS_SM_CONF_CTX()
    : cbRebootNoticeFunc(NULL) {
  }
} T_SS_SM_CONF_CTX_INFO;

static T_SS_SM_CONF_CTX_INFO s_confCtx;

/***********************************************************************
 *  registProtectSvcs
 ***********************************************************************/
static void registProtectSvcs(std::vector<std::string> & protectedSvcs) {
  protectedSvcs.push_back(MN_NS_SHAREDMEM);
  protectedSvcs.push_back(MN_NS_NPPSERVICE);
  protectedSvcs.push_back(MN_NS_BACKUPMGR);
  protectedSvcs.push_back(MN_SS_LOGGERSRV);
  protectedSvcs.push_back(MN_COMMUNICATION);
  protectedSvcs.push_back(MN_PS_PSMSHADOW);
  protectedSvcs.push_back(MN_SS_POWERSERVICE);
}

/***********************************************************************
 *  registGroupRelaunchSvcs
 ***********************************************************************/
static void registGroupRelaunchSvcs(
  std::vector<std::string> & groupRelaunchSvcs) {
  groupRelaunchSvcs.push_back(MN_ENFORMSERVICE);
  groupRelaunchSvcs.push_back(MN_NAVIPROXY);
  groupRelaunchSvcs.push_back(MN_AWBPROXY);
  groupRelaunchSvcs.push_back(MN_TFFPROXY);
  groupRelaunchSvcs.push_back(MN_AWNPRIMARY);
  groupRelaunchSvcs.push_back(MN_AWMPRIMARY);
  groupRelaunchSvcs.push_back(MN_AWBPRIMARY);
  groupRelaunchSvcs.push_back(MN_AWTPRIMARY);
}

/***********************************************************************
 *  setEvnVariableForLaunchConfigFile
 ***********************************************************************/
static EFrameworkunifiedStatus setEvnVariableForLaunchConfigFile(std::string f_sEnvFile,
    std::string f_sEnvBrand,
    std::string f_sEnvGrade,
    BOOL f_bIsVup) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR VupLaunchConfigFileName[] = "sm_launch_vup.cfg";
  const CHAR LaunchConfigFileName1[] = "sm_launch.cfg";
  const CHAR LaunchConfigFileName2[] = "sm_launch.cfg";

  // If an environment variable is set, that value is used, so set only when it is not set
  if (f_sEnvFile.empty()) {
    const CHAR *l_pLaunchConfigFile = NULL;

    if (f_bIsVup) {
      l_pLaunchConfigFile = VupLaunchConfigFileName;
    } else {
      // Change the file read by Brand and Grade
      if ((f_sEnvBrand == EnvValueBrand2) && (f_sEnvGrade != EnvValueGrade1)) {
        l_pLaunchConfigFile = LaunchConfigFileName1;
      } else {
        l_pLaunchConfigFile = LaunchConfigFileName2;
      }

      std::string cnfPath = "/usr/agl/conf/systemmanager/";
      cnfPath += l_pLaunchConfigFile;
      if (access(cnfPath.c_str(), F_OK) != 0) {
        l_pLaunchConfigFile = LaunchConfigFileName1;
      }
    }

    if (0 != setenv(LaunchConfigFileNameEnvVariable, l_pLaunchConfigFile, 1)) {
      SS_ASERT_ERRNO(0);
      l_eStatus = eFrameworkunifiedStatusFail;
    }
  }

  return l_eStatus;
}

/***********************************************************************
 *  setEvnVariableForLaunchRadioHDArbiter
 ***********************************************************************/
static EFrameworkunifiedStatus setEvnVariableForLaunchRadioHDArbiter(
  std::string & f_sEnvBoard, std::string f_sEnvArea) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR LaunchRadioHDArbiterEnvVariable[] = "SM_CONF_LAUNCH_RADIOHDARBITER";
  const CHAR Board1[] = "Gen1Board1";
  const CHAR Board2[] = "Gen1Board2";

  const CHAR *l_pSetValue = EnvValueNo;

  BOOL l_bArea1 = (f_sEnvArea == EnvValueArea1) ? TRUE : FALSE;

  if (f_sEnvBoard.empty()) {
    // Set "n" if not set
    SS_ASERT_ERRNO(0);
  } else if ((Board1 == f_sEnvBoard) || (Board2 == f_sEnvBoard)) {
    l_pSetValue = (TRUE == l_bArea1) ? EnvValueYes : EnvValueNo;
  }

  if (0 != setenv(LaunchRadioHDArbiterEnvVariable, l_pSetValue, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

/***********************************************************************
 *  setEvnVariableForLaunchDtvService
 ***********************************************************************/
static EFrameworkunifiedStatus setEvnVariableForLaunchDtvService(std::string f_sEnvBrand,
    std::string f_sEnvArea,
    std::string f_sEnvGrade) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR LaunchDtvServiceEnvVariable[] = "SM_CONF_LAUNCH_DTVSERVICE";

  const CHAR *l_pSetValue = EnvValueNo;

  BOOL l_bBrand1 = (f_sEnvBrand == EnvValueBrand1) ? TRUE : FALSE;
  BOOL l_bArea2 = (f_sEnvArea == EnvValueArea2) ? TRUE : FALSE;
  BOOL l_bGrade1 =
    ((f_sEnvBrand == EnvValueBrand2) && (f_sEnvGrade == EnvValueGrade1)) ?
    TRUE : FALSE;

  if ((l_bBrand1 | l_bGrade1) && l_bArea2 ) {
    l_pSetValue = EnvValueYes;
  } else {
    l_pSetValue = EnvValueNo;
  }

  if (0 != setenv(LaunchDtvServiceEnvVariable, l_pSetValue, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

/***********************************************************************
 *  setEvnVariableForLaunchHandWriting
 ***********************************************************************/
static EFrameworkunifiedStatus setEvnVariableForLaunchHandWriting(std::string f_sEnvBrand,
    std::string f_sEnvArea) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR LaunchHandWritingEnvVariable[] = "SM_CONF_LAUNCH_HANDWRITING";

  const CHAR *l_pSetValue = EnvValueNo;

  BOOL l_bBrand1 = (f_sEnvBrand == EnvValueBrand1) ? TRUE : FALSE;
  BOOL l_bArea1 = (f_sEnvArea == EnvValueArea1) ? TRUE : FALSE;
  BOOL l_bArea3 = (f_sEnvArea == EnvValueArea3) ? TRUE : FALSE;

  if ((l_bBrand1 && l_bArea1) || (l_bBrand1 && l_bArea3)) {
    l_pSetValue = EnvValueYes;
  } else {
    l_pSetValue = EnvValueNo;
  }

  if (0 != setenv(LaunchHandWritingEnvVariable, l_pSetValue, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

/***********************************************************************
 *  setEvnVariableForLaunchCus
 ***********************************************************************/
static EFrameworkunifiedStatus setEvnVariableForLaunchCus(std::string f_sEnvArea) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR LaunchCusEnvVariable[] = "SM_CONF_LAUNCH_CUS";

  const CHAR *l_pSetValue =
    (EnvValueArea1 == f_sEnvArea || EnvValueArea2 == f_sEnvArea) ?
    EnvValueYes : EnvValueNo;

  if (0 != setenv(LaunchCusEnvVariable, l_pSetValue, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

/***********************************************************************
 *  setEvnVariableForLaunchLSP
 ***********************************************************************/
static EFrameworkunifiedStatus setEvnVariableForLaunchLSP(std::string f_sEnvBrand, std::string f_sEnvArea, std::string f_sEnvGrade){

  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR LaunchLSPEnvVariable[] = "SM_CONF_LAUNCH_LSP";

  const CHAR *l_pSetValue = EnvValueNo;

  BOOL l_bBrand2 = (f_sEnvBrand == EnvValueBrand2) ? TRUE : FALSE;
  BOOL l_bArea1 = (f_sEnvArea == EnvValueArea1) ? TRUE : FALSE;
  BOOL l_bGrade2 = (f_sEnvGrade == EnvValueGrade2) ? TRUE : FALSE;

  if(l_bBrand2 && l_bArea1) {
    l_pSetValue = EnvValueYes;
  } else if(l_bGrade2) {
    l_pSetValue = EnvValueYes;
  } else {
    l_pSetValue = EnvValueNo;
  }


  if (0 != setenv(LaunchLSPEnvVariable, l_pSetValue, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

/***********************************************************************
 *  setEvnVariableForLaunchTFF
 ***********************************************************************/
static EFrameworkunifiedStatus setEvnVariableForLaunchTFF(std::string f_sEnvArea) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR LaunchTFFEnvVariable[] = "SM_CONF_LAUNCH_TFF";

  const CHAR *l_pSetValue = EnvValueNo;

  BOOL l_bArea1 = (f_sEnvArea == EnvValueArea1) ? TRUE : FALSE;
  BOOL l_bArea2 = (f_sEnvArea == EnvValueArea2) ? TRUE : FALSE;

  if (l_bArea1 || l_bArea2) {
    l_pSetValue = EnvValueYes;
  } else {
    l_pSetValue = EnvValueNo;
  }

  if (0 != setenv(LaunchTFFEnvVariable, l_pSetValue, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

/***********************************************************************
 *  setEvnVariableForLaunchAwtPrimary
 ***********************************************************************/
static EFrameworkunifiedStatus
setEvnVariableForLaunchAwtPrimary(std::string f_sEnvGrade, std::string f_sEnvArea){
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR LaunchAwtPrimaryEnvVariable[] = "SM_CONF_LAUNCH_AWTPRIMARY";

  const CHAR *l_pSetValue = EnvValueNo;

  BOOL l_bArea1 = (f_sEnvArea == EnvValueArea1) ? TRUE : FALSE;
  BOOL l_bArea2 = (f_sEnvArea == EnvValueArea2) ? TRUE : FALSE;
  BOOL l_bGrade2 = (f_sEnvGrade == EnvValueGrade2) ? TRUE : FALSE;

  if (l_bArea1) {
    try {
      EFrameworkunifiedStatus ret = eFrameworkunifiedStatusOK;
      SSVER_PkgInfo PhaseInfo = { 0 };
      CSSVer cVersion;
      if (true != cVersion.empty()) {
        ret = cVersion.getPkgInfo(SS_PKG_PHASE_INFO, &PhaseInfo);
        if (ret == eFrameworkunifiedStatusOK) {
          if ('3' == PhaseInfo.version[0]) {
            l_pSetValue = EnvValueYes;
          } else {
            l_pSetValue = EnvValueNo;
          }
        } else {
          l_pSetValue = EnvValueNo;
        }
      } else {
        l_pSetValue = EnvValueNo;
      }
    } catch (EFrameworkunifiedStatus ee) {
      l_pSetValue = EnvValueNo;
    }
  } else if (l_bArea2) {
    if( l_bGrade2 ) {
        l_pSetValue = EnvValueNo;
    } else {
        l_pSetValue = EnvValueYes;
    }
  } else {
    l_pSetValue = EnvValueNo;
  }

  if (0 != setenv(LaunchAwtPrimaryEnvVariable, l_pSetValue, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

/***********************************************************************
 *  getEnvValiavleWithDefault
 ***********************************************************************/
static std::string getEnvValiavleWithDefault(const CHAR *f_pEnvName,
    const CHAR *f_pDefault) {
  std::string l_sReturnValue;
  char buf[VP_MAX_LENGTH];

  VP_GetEnv(f_pEnvName, buf);
  if (buf[0] == '\0') {
    l_sReturnValue = (NULL != f_pDefault) ? f_pDefault : "";
  } else {
    l_sReturnValue = buf;
  }
  return l_sReturnValue;
}


/***********************************************************************
 *  getPkgInfoWithDefault
 ***********************************************************************/
static std::string
getPkgInfoWithDefault(const CHAR *f_pPkgName, const CHAR *f_pDefault){
  std::string l_sReturnValue = (NULL != f_pDefault) ? f_pDefault : "";

  try {
    SSVER_PkgInfo pkgInfo = {0};
    CSSVer cVersion;

    if(!cVersion.empty()){
        EFrameworkunifiedStatus ret = cVersion.getPkgInfo(f_pPkgName, &pkgInfo);
        if((ret == eFrameworkunifiedStatusOK) && ('\0' != pkgInfo.version[0])){
            l_sReturnValue = pkgInfo.version;
        }
    }
  } catch(EFrameworkunifiedStatus ee){
    SS_ASERT(0);
  }

  return l_sReturnValue;
}


/***********************************************************************
 *  setEvnVariableForLaunchVrService
 ***********************************************************************/
static EFrameworkunifiedStatus setEvnVariableForLaunchVrService(std::string f_sEnvBrand,
    std::string f_sEnvArea) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR LaunchVrArea1Phase1EnvVariable[] = "SM_CONF_LAUNCH_VR_AREA1_PHASE1";
  const CHAR LaunchVrArea1Phase2EnvVariable[] = "SM_CONF_LAUNCH_VR_AREA1_PHASE2";
  const CHAR LaunchVrArea2Phase1EnvVariable[] = "SM_CONF_LAUNCH_VR_AREA2_PHASE1";
  const CHAR LaunchVrArea2Phase2EnvVariable[] = "SM_CONF_LAUNCH_VR_AREA2_PHASE2";

  const CHAR *l_pSetValueArea1Phase1 = EnvValueNo;
  const CHAR *l_pSetValueArea1Phase2 = EnvValueNo;
  const CHAR *l_pSetValueArea2Phase1 = EnvValueNo;
  const CHAR *l_pSetValueArea2Phase2 = EnvValueNo;

  BOOL l_bArea1 = (f_sEnvArea == EnvValueArea1) ? TRUE : FALSE;
  BOOL l_bArea2 = (f_sEnvArea == EnvValueArea2) ? TRUE : FALSE;

  char buf[VP_MAX_LENGTH] = { 0 };

  if (l_bArea1) {
    VP_GetEnv(VP_VEHICLEPARAMETERLIBRARY__CWORD6_, buf);
    if (strcmp(buf, "unavailable") == 0) {
      l_pSetValueArea1Phase1 = EnvValueYes;
    } else {
      l_pSetValueArea1Phase2 = EnvValueYes;
    }
  } else if (l_bArea2) {
    VP_GetEnv(VP_VEHICLEPARAMETERLIBRARY__CWORD6_, buf);
    if (strcmp(buf, "unavailable") == 0) {
      l_pSetValueArea2Phase1 = EnvValueYes;
    } else {
      l_pSetValueArea2Phase2 = EnvValueYes;
    }
  }

  if (0 != setenv(LaunchVrArea1Phase1EnvVariable, l_pSetValueArea1Phase1, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }
  if (0 != setenv(LaunchVrArea1Phase2EnvVariable, l_pSetValueArea1Phase2, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }
  if (0 != setenv(LaunchVrArea2Phase1EnvVariable, l_pSetValueArea2Phase1, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }
  if (0 != setenv(LaunchVrArea2Phase2EnvVariable, l_pSetValueArea2Phase2, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

/***********************************************************************
 *  setEvnVariableForLaunchMirrorLink
 ***********************************************************************/
static EFrameworkunifiedStatus setEvnVariableForLaunchMirrorLink(std::string f_sEnvBrand, std::string f_sEnvArea, std::string f_sSeriesInfo){
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR LaunchMirrorLinkEnvVariable[] = "SM_CONF_LAUNCH_MIRRORLINK";

  const CHAR *l_pSetValue = EnvValueNo;

  BOOL l_bBrand2 = (f_sEnvBrand == EnvValueBrand2) ? TRUE : FALSE;
  BOOL l_bArea3 = (f_sEnvArea == EnvValueArea3) ? TRUE : FALSE;
  BOOL l_bArea4 = (f_sEnvArea == EnvValueArea4) ? TRUE : FALSE;
  BOOL l_bGen2 = (f_sSeriesInfo == PkgInfoGen2) ? TRUE : FALSE;

  if(l_bGen2){
    l_pSetValue = EnvValueNo;
  } else {
    if((l_bBrand2 && l_bArea3) || (l_bBrand2 && l_bArea4)) {
      l_pSetValue = EnvValueYes;
    } else {
      l_pSetValue = EnvValueNo;
    }
  }


  if (0 != setenv(LaunchMirrorLinkEnvVariable, l_pSetValue, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

/***********************************************************************
 *  setEvnVariableForLaunchRadioService
 ***********************************************************************/
static EFrameworkunifiedStatus setEvnVariableForLaunchRadioService(std::string f_sEnvArea) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR LaunchRadioServiceEnvVariable[] = "SM_CONF_LAUNCH_RADIOSERVICE";

  const CHAR *l_pSetValue = EnvValueNo;

  BOOL l_bArea1 = (f_sEnvArea == EnvValueArea1) ? TRUE : FALSE;

  if (l_bArea1) {
    l_pSetValue = EnvValueYes;
  } else {
    l_pSetValue = EnvValueNo;
  }

  if (0 != setenv(LaunchRadioServiceEnvVariable, l_pSetValue, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

/***********************************************************************
 *  setEvnVariableForLaunchPLAYSRV
 ***********************************************************************/
static EFrameworkunifiedStatus
setEvnVariableForLaunchPLAYSRV(std::string f_sEnvArea, std::string f_sEnvBrand, std::string f_sEnvGrade, std::string f_sSeriesInfo){
  EFrameworkunifiedStatus   l_eStatus   = eFrameworkunifiedStatusOK;
  const CHAR LaunchPLAYSRVEnvVariable[] = "SM_CONF_LAUNCH_PLAYSRV";

  const CHAR  *l_pSetValue = EnvValueNo;
  BOOL l_bPhase1 = FALSE;
  BOOL l_bPhase2 = FALSE;
  BOOL l_bPhase3 = FALSE;

  BOOL l_bArea2 = (f_sEnvArea == EnvValueArea2) ? TRUE : FALSE;
  BOOL l_bArea1 = (f_sEnvArea == EnvValueArea1) ? TRUE : FALSE;
  BOOL l_bBrand2 = (f_sEnvBrand == EnvValueBrand2) ? TRUE : FALSE;
  BOOL l_bBrand1 = (f_sEnvBrand == EnvValueBrand1) ? TRUE : FALSE;
  BOOL l_bGrade1 = (f_sEnvGrade == EnvValueGrade1) ? TRUE : FALSE;
  BOOL l_bGrade2 = (f_sEnvGrade == EnvValueGrade2) ? TRUE : FALSE;
  BOOL l_bGrade3 = (f_sEnvGrade == EnvValueGrade3) ? TRUE : FALSE;
  BOOL l_bGen2 = (f_sSeriesInfo == PkgInfoGen2) ? TRUE : FALSE;

  //VEHICLEPARAMETERLIBRARY_PHASE
  EFrameworkunifiedStatus ret = eFrameworkunifiedStatusOK;
  SSVER_PkgInfo PhaseInfo = {0};
  CSSVer cVersion;
  if ( true != cVersion.empty() ) {
    ret = cVersion.getPkgInfo( SS_PKG_PHASE_INFO, &PhaseInfo );
    if ( ret == eFrameworkunifiedStatusOK ) {
      l_bPhase1 = ('1' == PhaseInfo.version[0]) ? TRUE : FALSE;
      l_bPhase2 = ('2' == PhaseInfo.version[0]) ? TRUE : FALSE;
      l_bPhase3 = ('3' == PhaseInfo.version[0]) ? TRUE : FALSE;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Gen2[%d] Area1[%d] Area2[%d] Brand1[%d] Brand2[%d] Grade1[%d] Grade2[%d] Grade3[%d] P1[%d] P2[%d] P3[%d] " ,
         l_bGen2, l_bArea1, l_bArea2, l_bBrand1, l_bBrand2, l_bGrade1, l_bGrade2, l_bGrade3,
         l_bPhase1, l_bPhase2, l_bPhase3);

  // Gen2
  if ( l_bGen2 ) {
    // Grade2
    if ( l_bGrade2 )
    {
      l_pSetValue = EnvValueNo;
    } else {
      l_pSetValue = EnvValueYes;
    }
  } else {
    // Brand1
    if ( l_bBrand1 ) {
      l_pSetValue = EnvValueYes;
    }
    // Brand2
    else if ( l_bBrand2 ) {
      // Area2
      if ( l_bArea2 ) {
        l_pSetValue = EnvValueYes;
      }
      // Area1
      else if ( l_bArea1 ) {
        //_CWORD95_
        if ( l_bGrade3 ) {
          // Phase3
          if ( l_bPhase3 ) {
            l_pSetValue = EnvValueYes;
          } else {
            l_pSetValue = EnvValueNo;
          }
        } else {
          l_pSetValue = EnvValueNo;
        }
      // Other
      } else {
        l_pSetValue = EnvValueNo;
      }
    // Other
    } else {
      l_pSetValue = EnvValueNo;
    }
  }
    
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "SetValue : %s" , l_pSetValue);
    
  if (0 != setenv(LaunchPLAYSRVEnvVariable, l_pSetValue, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

/***********************************************************************
 *  setEvnVariableForLaunch_CWORD27_
 ***********************************************************************/
static EFrameworkunifiedStatus
setEvnVariableForLaunch_CWORD27_(std::string f_sEnvArea, std::string f_sSeriesInfo){
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR Launch_CWORD27_EnvVariable[] = "SM_CONF_LAUNCH__CWORD27_";
  const CHAR *l_pSetValue = EnvValueNo;

  BOOL l_bArea1  = (f_sEnvArea == EnvValueArea1)  ? TRUE : FALSE;
  BOOL l_bGen2  = (f_sSeriesInfo == PkgInfoGen2) ? TRUE : FALSE;

  if (l_bGen2) {
    l_pSetValue = EnvValueYes;
  } else {
    if (l_bArea1) {
      try {
        EFrameworkunifiedStatus ret = eFrameworkunifiedStatusOK;
        SSVER_PkgInfo PhaseInfo = {0};
        CSSVer cVersion;
        if ( true != cVersion.empty() ) {
          ret = cVersion.getPkgInfo( SS_PKG_PHASE_INFO, &PhaseInfo );
          if ( ret == eFrameworkunifiedStatusOK ) {
            if ( '3' == PhaseInfo.version[0] ) {
              l_pSetValue = EnvValueYes;
            } else {
              l_pSetValue = EnvValueNo;
            }
          } else {
            l_pSetValue = EnvValueNo;
          }
        } else {
          l_pSetValue = EnvValueNo;
        }
      } catch ( EFrameworkunifiedStatus ee) {
        l_pSetValue = EnvValueNo;
      }
    } else {
      l_pSetValue = EnvValueNo;
    }
  }

  if (0 != setenv(Launch_CWORD27_EnvVariable, l_pSetValue, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}



/***********************************************************************
 *  setEvnVariableForLaunchVRBodyCtrl
 ***********************************************************************/
static EFrameworkunifiedStatus
setEvnVariableForLaunchVRBodyCtrl(std::string f_sEnvArea, std::string f_sSeriesInfo){
  EFrameworkunifiedStatus   l_eStatus   = eFrameworkunifiedStatusOK;
  const CHAR LaunchVRBodyCtrlEnvVariable[] = "SM_CONF_LAUNCH_VRBODYCTRL";

  const CHAR  *l_pSetValue = EnvValueNo;

  BOOL l_bArea2 = (f_sEnvArea == EnvValueArea2) ? TRUE : FALSE;
  BOOL l_bGen2 = (f_sSeriesInfo == PkgInfoGen2) ? TRUE : FALSE;

  if(l_bArea2 && l_bGen2) { 
    l_pSetValue = EnvValueYes;
  } else {
    l_pSetValue = EnvValueNo;
  }

  if(0 != setenv(LaunchVRBodyCtrlEnvVariable, l_pSetValue, 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

/***********************************************************************
 *  setEvnVariableForPkgSeriesInfo
 ***********************************************************************/
static EFrameworkunifiedStatus
setEvnVariableForPkgSeriesInfo(std::string f_sPkgSeriesInfo){
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  const CHAR PkgSeriesEnvVariable[] = "SM_CONF_PKG_SERIES";

  if (f_sPkgSeriesInfo.empty()) {
    SS_ASERT(0);
  }

  if (0 != setenv(PkgSeriesEnvVariable, f_sPkgSeriesInfo.c_str(), 1)) {
    SS_ASERT_ERRNO(0);
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

/***********************************************************************
 *  setVersionInfo subfunction(version,date settings)
 ***********************************************************************/
static EFrameworkunifiedStatus setVerInfo_version_date(const char* VerPath) {
  EFrameworkunifiedStatus configRet = eFrameworkunifiedStatusOK;
  CSSVer ver;
  SSVER_PkgInfo pkgInfo;

  bzero(&pkgInfo, sizeof(pkgInfo));
  std::string line;
  std::ifstream verFin(VerPath);
  if (verFin) {
    if (std::getline(verFin, line)) {
      snprintf(pkgInfo.version, sizeof(pkgInfo.version), "%s",
               line.c_str());
    }

    if (std::getline(verFin, line)) {
      snprintf(pkgInfo.date, sizeof(pkgInfo.date), "%s", line.c_str());
    }

    configRet = ver.setPkgInfo(SS_PKG_MAIN_LPF_SI, pkgInfo);
    SS_ASERT(configRet == eFrameworkunifiedStatusOK);
  } else {
    SS_ASERT_ERRNO(0);
  }
  return configRet;
}


/***********************************************************************
 *  setVersionInfo subfunction(version settings)
 ***********************************************************************/
static EFrameworkunifiedStatus setVerInfo_version(const char* VerPath) {
  EFrameworkunifiedStatus configRet = eFrameworkunifiedStatusOK;
  CSSVer ver;
  SSVER_PkgInfo pkgInfo;

  std::string line;
  std::ifstream verFin(VerPath);
  if (verFin) {
    bzero(&pkgInfo, sizeof(pkgInfo));
    if (std::getline(verFin, line)) {
      snprintf(pkgInfo.version, sizeof(pkgInfo.version), "%s",
               line.c_str());
    }
    configRet = ver.setPkgInfo(SS_PKG_SPI_LOADER, pkgInfo);
    SS_ASERT(configRet == eFrameworkunifiedStatusOK);
  } else {
    SS_ASERT_ERRNO(0);
  }
  return configRet;
}


/***********************************************************************
 *  Set version information
 ***********************************************************************/
static EFrameworkunifiedStatus setVersionInfo(void) {
  EFrameworkunifiedStatus configRet = eFrameworkunifiedStatusOK;

  try {
    CSSVer ver;
    SSVER_PkgInfo pkgInfo;
    const char targetEnv[] = "TARGET_BOARD";

    // HARDWARE
    snprintf(pkgInfo.version, sizeof(pkgInfo.version), "%s",
             getEnvValiavleWithDefault(targetEnv, NULL).c_str());
    pkgInfo.date[0] = '\0';
    configRet = ver.setPkgInfo(SS_PKG_TARGETBOARD, pkgInfo);
    SS_ASERT(configRet == eFrameworkunifiedStatusOK);

    // Set LPF version information
    const char pfVersion[] = "/version/pf/version.txt";
    {
      configRet = setVerInfo_version_date(pfVersion);
    }

    // Set COMP SI version information
    const char csiVersion[] = "/version/si/version.txt";
    {
      configRet = setVerInfo_version_date(csiVersion);
    }

    // Set PRODUCT version information
    const char prdVersion[] = "/version/product/version.txt";
    {
      std::string line;
      std::ifstream prdFin(prdVersion);
      if (prdFin) {
        bzero(&pkgInfo, sizeof(pkgInfo));
        if (std::getline(prdFin, line)) {
          snprintf(pkgInfo.version, sizeof(pkgInfo.version), "%s",
                   line.c_str());
        }
        configRet = ver.setPkgInfo(SS_PKG_MAIN_EMMC, pkgInfo);
        SS_ASERT(configRet == eFrameworkunifiedStatusOK);

        bzero(&pkgInfo, sizeof(pkgInfo));
        if (std::getline(prdFin, line)) {
          snprintf(pkgInfo.version, sizeof(pkgInfo.version), "%s",
                   line.c_str());
        }
        configRet = ver.setPkgInfo(SS_PKG_MAIN_PRODUCT_SI, pkgInfo);
        SS_ASERT(configRet == eFrameworkunifiedStatusOK);
      } else {
        SS_ASERT_ERRNO(0);
      }
    }

    // Set SPI_LOADER version information
    const char spiVersion[] = "/proc/device-tree/bootinfo/ver_spiloader";
    {
      configRet = setVerInfo_version(spiVersion);
    }

    // Set UBOOT version information
    const char ubootVersion[] = "/proc/device-tree/bootinfo/ver_uboot";
    {
      configRet = setVerInfo_version(ubootVersion);
    }

    // Set SECURE_LOADER version information
    const char secureVersion[] = "/proc/device-tree/bootinfo/ver_secureloader";
    {
      configRet = setVerInfo_version(secureVersion);
    }

    // PHASE_INFO
    const char phaseVersion[] = "/version/PhaseInfo/PhaseInfo.txt";
    {
      configRet = setVerInfo_version_date(phaseVersion);
    }

    // VP_VEHICLEPARAMETERLIBRARY
    {
      std::string l_sEnvVpConfigVer = getEnvValiavleWithDefault(VP_VEHICLEPARAMETERLIBRARY_VERSION,
                                    NULL);
      if (!l_sEnvVpConfigVer.empty()) {
        bzero(&pkgInfo, sizeof(pkgInfo));
        snprintf(pkgInfo.version, sizeof(pkgInfo.version), "%s",
                 l_sEnvVpConfigVer.c_str());

        configRet = ver.setPkgInfo(SS_PKG_VP_CONFIG, pkgInfo);
        SS_ASERT(configRet == eFrameworkunifiedStatusOK);
      } else {
        SS_ASERT(0);
      }
    }

    // VP__CWORD31_
    {
      std::string l_sEnvVp_CWORD31_Ver = getEnvValiavleWithDefault(
                                        VP__CWORD31__VERSION, NULL);
      if (!l_sEnvVp_CWORD31_Ver.empty()) {
        bzero(&pkgInfo, sizeof(pkgInfo));
        snprintf(pkgInfo.version, sizeof(pkgInfo.version), "%s",
                 l_sEnvVp_CWORD31_Ver.c_str());

        configRet = ver.setPkgInfo(SS_PKG_VP__CWORD31_, pkgInfo);
        SS_ASERT(configRet == eFrameworkunifiedStatusOK);
      }
        SS_ASERT(0);
    }
  } catch (EFrameworkunifiedStatus ee) {
    SS_ASERT(0);
    configRet = ee;
  }
  return configRet;
}

// If _CWORD83_ common header is added, replace with century definition
#define NTFY_PSMShadowService_Availability MN_PS_PSMSHADOW"/Availability"

EFrameworkunifiedStatus cbPsmNotificationFunc(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (MSG_PSM_REBOOT_NOTIFY != FrameworkunifiedGetMsgProtocol(hApp)) {
    SS_ASERT(0);
    goto ERROR;
  }

  if (NULL != s_confCtx.cbRebootNoticeFunc) {
    l_eStatus = s_confCtx.cbRebootNoticeFunc(hApp);
    if (l_eStatus != eFrameworkunifiedStatusOK) {
      SS_ASERT(0);
      goto ERROR;
    }
  } else {
    SS_ASERT(0);
    goto ERROR;
  }

ERROR:
  return l_eStatus;
}

EFrameworkunifiedStatus cbServiceNotificationFunc(HANDLE hApp) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  PCSTR l_availabilityName;
  BOOL l_serviceAvailability;

  l_availabilityName = FrameworkunifiedGetLastNotification(hApp);
  if (l_availabilityName == NULL) {
    SS_ASERT(0);
    goto ERROR;
  }
  l_serviceAvailability = FrameworkunifiedIsServiceAvailable(hApp);

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s : %s", l_availabilityName,
         l_serviceAvailability ? "TRUE" : "FALSE");

  // Since it is necessary to guarantee SYNC of RAMD at _CWORD71_
  // within 200msec after receiving reboot notice from SYS mcu,
  // do UMOUNT RAMD immediately after receiving reboot notice by SystemManager.
  if ((strcmp(NTFY_PSMShadowService_Availability, l_availabilityName) == 0)
      && (l_serviceAvailability)) {
    l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(hApp, FRAMEWORKUNIFIED_ANY_SOURCE,
                MSG_PSM_REBOOT_NOTIFY,
                cbPsmNotificationFunc);
    if (eFrameworkunifiedStatusOK != l_eStatus) {
      SS_ASERT(0);
      goto ERROR;
    }
  }

ERROR:
  return l_eStatus;
}

/***********************************************************************
 *  ss_sm_initHook
 ***********************************************************************/
EFrameworkunifiedStatus ss_sm_initHook(HANDLE hApp, const T_SS_SM_INIT_HOOK_IN_PARAM *inPrm,
                          T_SS_SM_INIT_HOOK_OUT_PARAM *outPrm) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  std::string l_sEnvFile;
  std::string l_sEnvBoard;
  std::string l_sEnvArea;
  std::string l_sEnvBrand;
  std::string l_sEnvGrade;

  std::string l_sPkgSeriesInfo;

  // Register non-recoverable services for OOM Killer
  registProtectSvcs(outPrm->protectedSvcs);

  // Register services for Group Relaunch
  registGroupRelaunchSvcs(outPrm->groupRelaunchSvcs);

  if (NULL == inPrm->cbRebootNoticeFunc) {
    SS_ASERT(0);
  } else {
    s_confCtx.cbRebootNoticeFunc = inPrm->cbRebootNoticeFunc;
  }

  /** Set Version information **/
  if (eFrameworkunifiedStatusOK != setVersionInfo()) {
    SS_ASERT(0);
  }

  /** Get environment variable **/

  // APPS_CONFIG_FILENAME (not set by default)
  l_sEnvFile = getEnvValiavleWithDefault(LaunchConfigFileNameEnvVariable, NULL);

  // VEHICLEPARAMETERLIBRARY_BOARD
  l_sEnvBoard = getEnvValiavleWithDefault(BoardEnvVariable, NULL);

  // VEHICLEPARAMETERLIBRARY_AREA
  l_sEnvArea = getEnvValiavleWithDefault(AreaTypeEnvVariable, EnvValueArea1);

  // VEHICLEPARAMETERLIBRARY_BRAND
  l_sEnvBrand = getEnvValiavleWithDefault(BrandTypeEnvVariable, NULL);

  // VEHICLEPARAMETERLIBRARY_grade
  l_sEnvGrade = getEnvValiavleWithDefault(GradeEnvVariable, NULL);


  /** Get Package information **/

  // SERIES_INFO
  l_sPkgSeriesInfo = getPkgInfoWithDefault(SS_PKG_SERIES_INFO, NULL);


  /** Set environment variable **/
  // APPS_CONFIG_FILENAME
  if (eFrameworkunifiedStatusOK
      != setEvnVariableForLaunchConfigFile(l_sEnvFile, l_sEnvBrand,
          l_sEnvGrade, inPrm->bIsVupMode)) {
    SS_ASERT(0);
  }

  // SM_CONF_PKG_SERIES
  if (eFrameworkunifiedStatusOK != setEvnVariableForPkgSeriesInfo(l_sPkgSeriesInfo)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH_RadioHDArbiter
  if (eFrameworkunifiedStatusOK
      != setEvnVariableForLaunchRadioHDArbiter(l_sEnvBoard, l_sEnvArea)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH_DTVSERVICE
  if (eFrameworkunifiedStatusOK
      != setEvnVariableForLaunchDtvService(l_sEnvBrand, l_sEnvArea, l_sEnvGrade)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH_HANDWRITING
  if (eFrameworkunifiedStatusOK
      != setEvnVariableForLaunchHandWriting(l_sEnvBrand, l_sEnvArea)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH_CUS
  if (eFrameworkunifiedStatusOK != setEvnVariableForLaunchCus(l_sEnvArea)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH_VR
  if (eFrameworkunifiedStatusOK
      != setEvnVariableForLaunchVrService(l_sEnvBrand, l_sEnvArea)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH_LSP
  if(eFrameworkunifiedStatusOK != setEvnVariableForLaunchLSP(l_sEnvBrand, l_sEnvArea, l_sEnvGrade)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH_MIRRORLINK
  if(eFrameworkunifiedStatusOK != setEvnVariableForLaunchMirrorLink(l_sEnvBrand, l_sEnvArea, l_sPkgSeriesInfo)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH_TFF
  if (eFrameworkunifiedStatusOK
      != setEvnVariableForLaunchTFF(l_sEnvArea)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH_AWTPRIMARY
  if(eFrameworkunifiedStatusOK != setEvnVariableForLaunchAwtPrimary(l_sEnvGrade, l_sEnvArea)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH_RADIOSERVICE
  if (eFrameworkunifiedStatusOK
      != setEvnVariableForLaunchRadioService(l_sEnvArea)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH_PLAYSRV
  if (eFrameworkunifiedStatusOK != setEvnVariableForLaunchPLAYSRV(l_sEnvArea, l_sEnvBrand, l_sEnvGrade, l_sPkgSeriesInfo)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH__CWORD27_
  if (eFrameworkunifiedStatusOK != setEvnVariableForLaunch_CWORD27_(l_sEnvArea, l_sPkgSeriesInfo)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH_VRBODYCTRL
  if(eFrameworkunifiedStatusOK != setEvnVariableForLaunchVRBodyCtrl(l_sEnvArea, l_sPkgSeriesInfo)) {
    SS_ASERT(0);
  }


  if (eFrameworkunifiedStatusOK
      != (l_eStatus = FrameworkunifiedSubscribeNotificationWithCallback(
                        hApp, NTFY_PSMShadowService_Availability, cbServiceNotificationFunc))) {
    FRAMEWORKUNIFIEDLOG(
      ZONE_ERR,
      __FUNCTION__,
      " Error: FrameworkunifiedSubscribeNotificationWithCallback(" "hApp, %s) errored: %d/'%s'",
      NTFY_PSMShadowService_Availability, l_eStatus, GetStr(l_eStatus).c_str());
  }

  return l_eStatus;
}

