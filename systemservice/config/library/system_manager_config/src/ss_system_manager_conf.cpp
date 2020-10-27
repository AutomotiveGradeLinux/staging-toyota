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

      std::string cnfPath = "/usr/agl/conf/BS/ss/system_manager/rwdata/";
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

    // Set PRODUCT version information
    const char prdVersion[] = "/usr/agl/share/BS/ss/system_manager/rodata/version.txt";
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

    // PHASE_INFO
    const char phaseVersion[] = "/usr/agl/share/BS/ss/system_manager/rodata/PhaseInfo.txt";
    {
      configRet = setVerInfo_version_date(phaseVersion);
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


  // SM_CONF_LAUNCH_CUS
  if (eFrameworkunifiedStatusOK != setEvnVariableForLaunchCus(l_sEnvArea)) {
    SS_ASERT(0);
  }

  // SM_CONF_LAUNCH_TFF
  if (eFrameworkunifiedStatusOK
      != setEvnVariableForLaunchTFF(l_sEnvArea)) {
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

