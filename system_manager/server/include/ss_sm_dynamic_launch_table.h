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

#ifndef SS_SM_DYNAMIC_LAUNCH_TABLE_H_  // NOLINT
#define SS_SM_DYNAMIC_LAUNCH_TABLE_H_

#include "ss_sm_dynamic_launcher.h"

DynamicLaunchInfo g_arrDynamicLaunchTableCfg[] = {
  // TUNER
  { SS_SOURCE_FM,
      (const PCHAR)"FM", (const PCHAR)"Audio", (const PCHAR)"Radio",
      (const PCHAR)"HMI", (const PCHAR)"AviageTrigger", FALSE},
  { SS_SOURCE_AM,
      (const PCHAR)"AM", (const PCHAR)"Audio", (const PCHAR)"Radio",
      (const PCHAR)"HMI", (const PCHAR)"AviageTrigger", FALSE},
  { SS_SOURCE_FM_DAB,
      (const PCHAR)"FM_DAB", (const PCHAR)"Audio", (const PCHAR)"Radio",
      (const PCHAR)"HMI", (const PCHAR)"AviageTrigger", FALSE},
  { SS_SOURCE_SDARS,
      (const PCHAR)"SDARS", (const PCHAR)"Audio", (const PCHAR)"Radio",
      (const PCHAR)"HMI", (const PCHAR)"AviageTrigger", FALSE},
  // MEDIA
  { SS_SOURCE_SD_CARD,
      (const PCHAR)"SDCard", (const PCHAR)"Audio", (const PCHAR)"Multimedia",
      (const PCHAR)"HMI", (const PCHAR)"AviageTrigger", TRUE},
  { SS_SOURCE_MSC,
      (const PCHAR)"MCS", (const PCHAR)"Audio", (const PCHAR)"Multimedia",
      (const PCHAR)"HMI", (const PCHAR)"AviageTrigger", TRUE},
  { SS_SOURCE_MTP, (const PCHAR)"MTP",
      (const PCHAR)"Audio", (const PCHAR)"Multimedia", (const PCHAR)"HMI",
      (const PCHAR)"AviageTrigger", TRUE},
  { SS_SOURCE__CWORD57_,
      (const PCHAR)"_CWORD57_", (const PCHAR)"Audio", (const PCHAR)"Multimedia",
      (const PCHAR)"HMI", (const PCHAR)"AviageTrigger", TRUE},
  { SS_SOURCE__CWORD73__APP,
      (const PCHAR)"_CWORD57__APP", (const PCHAR)"Audio",
      (const PCHAR)"Multimedia", (const PCHAR)"HMI", (const PCHAR)"AviageTrigger", TRUE},
  { SS_SOURCE__CWORD73__VIDEO_MODE,
      (const PCHAR)"_CWORD57__VM", (const PCHAR)"Audio",
      (const PCHAR)"Multimedia", (const PCHAR)"HMI", (const PCHAR)"AviageTrigger", TRUE},
  { SS_SOURCE_USB,
      (const PCHAR)"USB", (const PCHAR)"Audio", (const PCHAR)"Multimedia",
      (const PCHAR)"HMI", (const PCHAR)"AviageTrigger", TRUE},
  { SS_SOURCE_DATA_CD,
      (const PCHAR)"DATA_CD", (const PCHAR)"Audio",
      (const PCHAR)"Multimedia", (const PCHAR)"HMI", (const PCHAR)"AviageTrigger", TRUE},
  { SS_SOURCE_CD,
      (const PCHAR)"AUDIO_CD", (const PCHAR)"Audio",
      (const PCHAR)"Multimedia", (const PCHAR)"HMI", (const PCHAR)"AviageTrigger", TRUE},
};

#endif  // SS_SM_DYNAMIC_LAUNCH_TABLE_H_  // NOLINT
