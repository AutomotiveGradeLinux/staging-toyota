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
/// \brief    This file provides support for System Manager process launching.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef SS_SM_LAUNCH_CONFIGURATION_H_  // NOLINT
#define SS_SM_LAUNCH_CONFIGURATION_H_

#include <native_service/frameworkunified_types.h>
#include <system_service/ss_services.h>
#include <system_service/ss_client_names.h>

typedef struct _DefaultModuleLaunchParams {
  const PCHAR name;
  const PCHAR binary_name;
  const PCHAR arguments;
  const PCHAR restart;
  UI_32       priority;
  BOOL        critical;
  UI_32       retry_cnt;
  const PCHAR logging_msk_str;
  BOOL        is_start_required;
  BOOL        shutdown_critical;
  UI_32       shutdown_wait_time;
} DefaultModuleLaunchParams;

typedef struct _DefaultGroupLaunchInfo {
  const PCHAR                name;
  UI_32                      id;
  DefaultModuleLaunchParams *modules;
} DefaultGroupLaunchInfo;


#define prefix

prefix DefaultModuleLaunchParams CoreModulesGroup[] = {
             // Queue name                           binary_name                arguments
  {(const PCHAR)SERVICE_NS_SHARED_MEM,
      (const PCHAR)"NS_SharedMem",      (const PCHAR)"", (const PCHAR)"me",
      10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0},
  {(const PCHAR)SERVICE_NS_NPP,         (const PCHAR)"NS_NPPService",
   (const PCHAR)"", (const PCHAR)"me",    20, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", FALSE, TRUE,  15},
  {(const PCHAR)SERVICE_LOGGER,         (const PCHAR)SERVICE_LOGGER,
   (const PCHAR)"", (const PCHAR)"me",    10, FALSE, 3, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0},
  {(const PCHAR)SERVICE__CWORD69_,           (const PCHAR)"_CWORD69_.exe",
   (const PCHAR)"", (const PCHAR)"me",    10, FALSE, 3, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0},
  {(const PCHAR)0,                      (const PCHAR)0,
   (const PCHAR)0,  (const PCHAR)0,        0, FALSE, 0, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0}
};

prefix DefaultModuleLaunchParams ShadowModulesGroup[] = {
             // Queue name                           binary_name                arguments
  {(const PCHAR)SERVICE_PS_IPC,         (const PCHAR)"PS_IPC",
   (const PCHAR)"", (const PCHAR)"group", 50, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0},
  {(const PCHAR)SERVICE_PSM_SHADOW,     (const PCHAR)"PS_PSMShadow",
   (const PCHAR)"", (const PCHAR)"group", 12, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_KEY_HANDLER,    (const PCHAR)"PS_KeyHandler",
   (const PCHAR)"", (const PCHAR)"group", 12, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_IPC_MP_SHADOW,  (const PCHAR)"PS_IPC_MP_Shadow",
   (const PCHAR)"", (const PCHAR)"group", 20, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)0,                      (const PCHAR)0,
   (const PCHAR)0,  (const PCHAR)0,        0, FALSE, 0, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0}
};

prefix DefaultModuleLaunchParams SystemModulesGroup[] = {
  {(const PCHAR)SERVICE_POWER,          (const PCHAR)SERVICE_POWER,
   (const PCHAR)"", (const PCHAR)"me",    12, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SS_DEV_DETECT_SRV,      (const PCHAR)SS_DEV_DETECT_SRV,
   (const PCHAR)"", (const PCHAR)"me",    12, FALSE, 3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SS_PLM_SERVICE,         (const PCHAR)SS_PLM_SERVICE,
   (const PCHAR)"", (const PCHAR)"me",    12, FALSE, 3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)0,                      (const PCHAR)0,
   (const PCHAR)0,  (const PCHAR)0,        0, FALSE, 0, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0}
};

prefix DefaultModuleLaunchParams _CWORD121_ModulesGroup[] = {
  {(const PCHAR)SERVICE_CAN_SHADOW,     (const PCHAR)"VS_CANShadow",
   (const PCHAR)"", (const PCHAR)"me",    10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_VS_DIAG_DISP,   (const PCHAR)"VS_DoIPServer",
   (const PCHAR)"", (const PCHAR)"me",    10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_VS_DIAG,        (const PCHAR)"VS_DiagService",
   (const PCHAR)"", (const PCHAR)"me",    10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_VS_VEHICLE,     (const PCHAR)"VS_VehicleService",
   (const PCHAR)"", (const PCHAR)"me",    10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_VS__CWORD85_,         (const PCHAR)"VS__CWORD85_Service",
   (const PCHAR)"", (const PCHAR)"me",    10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE__CWORD51__DR,       (const PCHAR)"_CWORD51__DR.exe",
   (const PCHAR)"", (const PCHAR)"me",    10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_VS_RVC,         (const PCHAR)"VS_RVCService",
   (const PCHAR)"", (const PCHAR)"me",    10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)0,                      (const PCHAR)0,
   (const PCHAR)0,  (const PCHAR)0,        0, FALSE, 0, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0}
};

prefix DefaultModuleLaunchParams AudioModulesGroup[] = {
  {(const PCHAR)SERVICE_AS_AUDIO,       (const PCHAR)"AS_AudioService",
   (const PCHAR)"", (const PCHAR)"me",    10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_AS_MODE,        (const PCHAR)"AS_ModeService",
   (const PCHAR)"", (const PCHAR)"me",    10, FALSE, 3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)0,                      (const PCHAR)0,
   (const PCHAR)0,  (const PCHAR)0,        0, FALSE, 0, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0}
};

prefix DefaultModuleLaunchParams RadioModulesGroup[] = {
  {(const PCHAR)SERVICE_TUNER,          (const PCHAR)"Radio",
   (const PCHAR)"", (const PCHAR)"me",    10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_DAB,            (const PCHAR)"Master_Tuner",
   (const PCHAR)"", (const PCHAR)"me",    10, FALSE, 3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_SDARS,          (const PCHAR)"Ext_SDARS_Service",
   (const PCHAR)"", (const PCHAR)"me",    10, FALSE, 3,
   (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)0,                      (const PCHAR)0,
   (const PCHAR)0,  (const PCHAR)0,        0, FALSE, 0, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0}
};

prefix DefaultModuleLaunchParams MultimediaModulesGroup[] = {
  {(const PCHAR)SERVICE_MM_ICD,         (const PCHAR)"MM_ICDService",
   (const PCHAR)"", (const PCHAR)"me",    10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_MM_MEDIA,       (const PCHAR)"MediaService",
   (const PCHAR)"", (const PCHAR)"me",    10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)0,                      (const PCHAR)0,
   (const PCHAR)0,  (const PCHAR)0,        0, FALSE, 0, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0}
};

prefix DefaultModuleLaunchParams HMIModulesGroup[] = {
  {(const PCHAR)SERVICE_HMI,            (const PCHAR)"GtfStartup",
   (const PCHAR)"/eb/gtf_5.2.1/models/PastModel004/gtfStartup.cfg", (const PCHAR)"me",
    13, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0},
  {(const PCHAR)SERVICE_HMI__CWORD77_,        (const PCHAR)"HMI__CWORD77_",
   (const PCHAR)"", (const PCHAR)"me",    10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_HMI__CWORD46_,      (const PCHAR)"HMI__CWORD46_Service",  (const PCHAR)"",
   (const PCHAR)"me",    10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)0,                      (const PCHAR)0,                   (const PCHAR)0,
   (const PCHAR)0,        0, FALSE, 0, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0}
};

prefix DefaultModuleLaunchParams NavigationModulesGroup[] = {
  {(const PCHAR)SERVICE_NAV_LOC,    (const PCHAR)"NAV_LocationService",   (const PCHAR)"", (const PCHAR)"me",
   10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_NAV,        (const PCHAR)"NAV_NavigationService", (const PCHAR)"", (const PCHAR)"me",
   10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)0,                  (const PCHAR)0,                       (const PCHAR)0,  (const PCHAR)0,
    0, FALSE, 0, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0}
};

prefix DefaultModuleLaunchParams SoftwareUpdateModulesGroup[] = {
  {(const PCHAR)SERVICE_SOFTWAREUPDATE, (const PCHAR)"SS_SoftwareUpdateService",
      (const PCHAR)"", (const PCHAR)"me", 10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_PS_SWDL_SHADOW, (const PCHAR)"PS_SoftwareUpdateShadow",
      (const PCHAR)"", (const PCHAR)"me", 10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)0,                      (const PCHAR)0,
   (const PCHAR)0,  (const PCHAR)0,     0, FALSE, 0, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0}
};

prefix DefaultModuleLaunchParams NetworkingModulesGroup[] = {
  {(const PCHAR)SERVICE__CWORD51__BT,      (const PCHAR)"_CWORD51__BT.exe",
   (const PCHAR)"", (const PCHAR)"me",  10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_NW_BT,         (const PCHAR)"NW_BluetoothService",
   (const PCHAR)"", (const PCHAR)"me",  10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_NW_CONNECTION, (const PCHAR)"NW_ConnectionService",
   (const PCHAR)"", (const PCHAR)"me",  10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_NW_PHONE,      (const PCHAR)"NW_PhoneService",
   (const PCHAR)"", (const PCHAR)"me",  10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_NW_PHONE_BOOK, (const PCHAR)"NW_PhoneBookService",
   (const PCHAR)"", (const PCHAR)"me",  10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)SERVICE_NW_MESSAGING,  (const PCHAR)"NW_MessagingService",
   (const PCHAR)"", (const PCHAR)"me",  10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)0,                     (const PCHAR)0,
   (const PCHAR)0,  (const PCHAR)0,      0, FALSE, 0, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0}
};

prefix DefaultModuleLaunchParams BrowserModulesGroup[] = {
  {(const PCHAR)SERVICE_BR_BROWSER,    (const PCHAR)"BR_BrowserService",
   (const PCHAR)"", (const PCHAR)"me",  10, TRUE,  3, (const PCHAR)"0x00000000, 0x00000000", TRUE,  FALSE,  0},
  {(const PCHAR)0,                     (const PCHAR)0,
   (const PCHAR)0,  (const PCHAR)0,      0, FALSE, 0, (const PCHAR)"0x00000000, 0x00000000", FALSE, FALSE,  0}
};

prefix DefaultGroupLaunchInfo g_arrLaunchTable[] = {
  {(const PCHAR)"CoreModules",     1, CoreModulesGroup           },
  {(const PCHAR)"ShadowModules",   2, ShadowModulesGroup         },
  {(const PCHAR)"SystemModules",   3, SystemModulesGroup         },
  {(const PCHAR)"_CWORD121_",            4, _CWORD121_ModulesGroup           },
  {(const PCHAR)"Audio",           5, AudioModulesGroup          },
  {(const PCHAR)"Radio",           6, RadioModulesGroup          },
  {(const PCHAR)"Multimedia",      7, MultimediaModulesGroup     },
  {(const PCHAR)"HMI",             8, HMIModulesGroup            },
  {(const PCHAR)"Navigation",      9, NavigationModulesGroup     },
  {(const PCHAR)"SoftwareUpdate", 10, SoftwareUpdateModulesGroup },
  {(const PCHAR)"Networking",     11, NetworkingModulesGroup     },
  {(const PCHAR)"Browser",        12, BrowserModulesGroup        },
};

#endif  // SS_SM_LAUNCH_CONFIGURATION_H_  // NOLINT
