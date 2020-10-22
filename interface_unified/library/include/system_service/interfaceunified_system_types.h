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

/**
 * @file interfaceunified_system_types.h
 * @brief \~english This file supports the System Manager audio source types.
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup interface_unified
 *  @ingroup system_service
 *  @{
 */
#ifndef INTERFACEUNIFIED_SYSTEM_TYPES_H_  // NOLINT (build/header_guard)
#define INTERFACEUNIFIED_SYSTEM_TYPES_H_


typedef enum _SS_Sources {
  SS_SOURCE_NA = 0,
  SS_SOURCE_FM = 1,  //## To be integrated into SS_SOURCE_AMFM for _CWORD71_
  SS_SOURCE_AM = 2,  //## To be integrated into SS_SOURCE_AMFM for _CWORD71_

  SS_SOURCE_CD = 3,    // For Audio CDs
  SS_SOURCE_AUX = 4,

  SS_SOURCE_PHONE = 5,    // Will be deprecated soon, use specific Incoming or Outgoing call.
  SS_SOURCE_TA = 6,  // ## To be integrated into SS_SOURCE_AMFM for _CWORD71_
  SS_SOURCE_CHIMES = 7,
  SS_SOURCE_NAVI = 8,

  SS_SOURCE_MSC = 9,              ///< Any mass storage device (USB-MSC, SD, HDD etc.,)
  SS_SOURCE_MTP = 10,             ///< _CWORD104_, MTP protocol devices(like _CWORD28_ over USB)

  SS_SOURCE__CWORD57_ = 11,  // ## To be integrated into SS_SOURCE_USB and SS_SOURCE_USB2 for _CWORD71_

  SS_SOURCE__CWORD73__APP = 12,        ///< _CWORD73_ based audio application (_CWORD74_ over BT/USB) - _CWORD22_, _CWORD13_ etc.,
  SS_SOURCE__CWORD73__VIDEO_MODE = 13,  ///< _CWORD73_ Video Mode (_CWORD57_ playback)

  SS_SOURCE_STREAMING = 14,      ///< Streaming over Wi-fi
  SS_SOURCE_BT_AUDIO = 15,       ///< Bluetooth Audio
  SS_SOURCE_BT_SPP = 16,         ///< Bluetooth SPP based audio application (_CWORD28_/BB etc.,)

  SS_SOURCE_SPEECH = 17,         ///< Speech audio playback

  SS_SOURCE_USB = SS_SOURCE_MSC,  ///< will be deprecated soon

  SS_SOURCE__CWORD59_ = 18,
  SS_SOURCE_ADRM = 19,
  SS_SOURCE__CWORD60_ = 20,
  SS_SOURCE_ADRA = 21,


  SS_SOURCE_INCOMING_CALL = SS_SOURCE_PHONE,

  SS_SOURCE_PRIVATE_MODE_CALL = 24,

  SS_SOURCE_HANDSFREE_SPEAKING = 25,

  SS_SOURCE_FM_DAB = 29,
  SS_SOURCE_SD_CARD = 30,
  SS_SOURCE_SDARS = 31,
  SS_SOURCE_BROWSER_ENT = 32,
  SS_SOURCE_ML_ENT = 33,
  SS_SOURCE_ML_INFO = 34,

  SS_SOURCE_DATA_CD = 35,    // For Data CDs

  SS_SOURCE_TAS = 40,                   // TA on same frequency

  SS_SOURCE_DTV = 41,
  SS_SOURCE_DVD = 42,
  SS_SOURCE_BD  = 43,
  SS_SOURCE_DELIVERY_APP = 44,
  SS_SOURCE_ENTUNE_ENT = 45,
  SS_SOURCE_HDD_AUDIO = 46,
  SS_SOURCE_JOYFUL_TALK = 47,
  SS_SOURCE_CLEARANCE_SONAR = 48,  // Not used, To be deleted
  SS_SOURCE_DSRC = 49,
  SS_SOURCE_ENTUNE_INFO = 50,    // Not used, To be deleted
  SS_SOURCE_HELPNET = 51,
  SS_SOURCE_OTV = 52,
  SS_SOURCE_MIRACAST = 53,
  SS_SOURCE_RSE = 54,
  SS_SOURCE_INSIDER = 55,
  SS_SOURCE_MAIL = 56,
  SS_SOURCE_MAIL_RINGTONE = 57,
  SS_SOURCE_BD_REAR = 58,
  SS_SOURCE_HDMI_REAR = 59,
  SS_SOURCE_SD_CARD_REAR = 60,
  SS_SOURCE_MIRACAST_REAR = 61,
  SS_SOURCE_USB_REAR = 62,    // Not used, To be deleted
  SS_SOURCE_DLNA_REAR = 63,
  SS_SOURCE_BROWSER_INFO = 64,
  SS_SOURCE_DSRC_ENT = 65,
  SS_SOURCE_RECDATA_PLAY = 66,
// add Source  ##
  SS_SOURCE_AMFM = SS_SOURCE_FM,
  SS_SOURCE_DLNA = 67,
  SS_SOURCE_USB2 = 68,
// add interrupt audio source ##
  SS_SOURCE_VDSP = 69,
  SS_SOURCE_NAVI_VR = 70,
  SS_SOURCE_STARTUPBGM = 71,
  SS_SOURCE_STARTUPBGM_INIT = 72,
  SS_SOURCE_MAIL_INFO = 73,

  SS_SOURCE_DIAG = 74,
  SS_SOURCE_MAYDAY = 75,
  SS_SOURCE_DCM = 76,
  SS_SOURCE_SD_VIDEO = 77,

  SS_SOURCE_DELIVERY_APP_INFO = 78,

  SS_SOURCE__CWORD27__INFO = 86,

  // Declaration of special sources starting from 100
  SS_SOURCE_POWER_OFF = 100   // Being used by Mode
}SS_SOURCE, *PSS_SOURCE;


typedef enum _SS_Zones {
  SS_ZONE_NA = 0,
  SS_ZONE_DRIVER = 1,  ///< Zero/Driver zone
  SS_ZONE_REAR1,
  SS_ZONE_REAR2,
  SS_ZONE_REAR3,
  SS_ZONE_ALL
}SS_ZONE, *PSS_ZONE;


typedef enum E_AUDIO_PORT {
  SS_AUDIO_INPUT_PORT_1 = 0x01,      ///< Enum representing MCASP2
  SS_AUDIO_INPUT_PORT_2 = 0x02,      ///< Enum representing MCBSP
  SS_AUDIO_INPUT_PORT_3 = 0x03,    ///< Enum representing MCASP3
  SS_AUDIO_INPUT_PORT_4 = 0x04,      ///< Enum representing MCASP3
  SS_AUDIO_INPUT_PORT_5 = 0x05       ///< Enum representing other port
}SS_AUDIO_INPUT_PORT, *PSS_AUDIO_INPUT_PORT;

typedef enum _SS_Device_Context {
  SINGLE_CONTEXT = 0,
  MULTI_CONTEXT = 1
}SS_DEVICE_CONTEXT, *PSS_DEVICE_CONTEXT;

#endif /* INTERFACEUNIFIED_SYSTEM_TYPES_H_ */  // NOLINT (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
