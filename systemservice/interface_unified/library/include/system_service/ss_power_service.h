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
 * @file ss_power_service.h
 * @brief \~english This file supports the Power Service interface.
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup power_service
 *  @ingroup system_service
 *  @{
 */
#ifndef SS_POWER_SERVICE_H_  // NOLINT (build/header_guard)
#define SS_POWER_SERVICE_H_

#include <native_service/frameworkunified_types.h>
#include <map>
#include "system_service/ss_system_types.h"



typedef enum _PwLevelTypes {
  epspltUNKNOWN,
  epspltWAKEUP = 0x99,
  epspltNORMAL,
  epspltSHUTDOWN,
  epspltEMERGENCY,
} PowerSrvLevelType, EPWR_LEVEL_TYPE;


typedef enum {
  epsstUNKNOWN,
  epsstBASIC,
  epsstSUPERVISOR,
  epsstSYSTEM,
} PowerSrvSessionType, EPWR_SESSION_TYPE;


typedef enum {
  epscnINVALID     = 0xFF,
  epscnCANWAKEUP   = 0x07,  //< All modules up with Display Off and Audio Muted
  epscnCANSLEEP    = 0x08,  //< Communication and all modules shut down
} ePowerSrvCANStates, EPWR_CAN_STATE_TYPE;



// Not in use for PastModel003*1. Can be used for Platform
typedef enum {
  epswlINVALID     = 0xFF,
  epswlFULLRUN     = 0x0A,  //< Normal ignition on and full wakeup
  epswlLOCALWAKEUP = 0x0B,  //< Only certain modules are up.
} ePowerSrvWakeupLevels, EPWR_WAKEUP_LEVEL_TYPE;

typedef EPWR_WAKEUP_LEVEL_TYPE * PEPWR_WAKEUP_LEVEL_TYPE;

typedef enum {
  epswsINVALID     = 0xFF,
  epswsPWRON       = 0x00,  //< Enable Display and Audio
  epswsPWROFF      = 0x01,  //< Disable Audio and HMI
} ePowerSrvPowerStates, EPWR_POWER_STATE_TYPE;


typedef enum {
  epsvsINVALID    = 0xFF,
  epsvsNORMAL     = 0x20,
  epsvsLVI1       = 0x22,
  epsvsLVI2       = 0x24,
} ePowerSrvVoltageStates, EPWR_VOLTAGE_STATE_TYPE;

typedef EPWR_VOLTAGE_STATE_TYPE * PEPWR_VOLTAGE_STATE_TYPE;

typedef enum {
  epscsINVALID    = 0xFF,
  epscsENTRY      = 0xA0,
  epscsEXIT       = 0xA2,
} ePowerSrvCrankStates, EPWR_CRANK_STATE_TYPE;


//******************************************************************************
//  Startup Reason / EPWR_WAKEUP_FACTOR_TYPE typedefs                          *
//                                                                             *
/**
 * \~english
 */
typedef enum {
  epswfINVALID         = 0xFF,  //!< \~english
  epswfTESTACC         = 0x00,  //!< \~english
  epswfON_KEY          = 0x01,  //!< \~english
  epswfIGN_ACC         = 0x02,  //!< \~english
  epswfDR_OPEN_CLOSE   = 0x03,  //!< \~english
  epswfDX_ACTIVATION   = 0x04,  //!< \~english
  epswfPASS_ACTIVATION = 0x05,  //!< \~english
  epswfSPVACTIVATION   = 0x06,  //!< \~english
  epswfUSER_DATA_RESET = 0x07   //!< \~english
//
//  Carry-over enums from CCR
//
//  epswfPWRBUTTONON    = 0x0C,
//  epswfIGNON       = 0x0D,
//  epswfDISCEJECT      = 0x0F,
//  epswfSHOWTIME     = 0x11,
//  epswfPHONECALL     = 0x12,
//
//  End of Carry-over enums from CCR
//
} ePowerSrvWakeupFactors, EPWR_WAKEUP_FACTOR_TYPE;
typedef EPWR_WAKEUP_FACTOR_TYPE * PEPWR_WAKEUP_FACTOR_TYPE;
//                                                                             *
//  End of Startup Reason / EPWR_WAKEUP_FACTOR_TYPE                            *
//******************************************************************************

//****************************************************************************
// Shutdown Popup Protocol enums & typedefs                                  *
//                                                                           *
typedef enum {                                                            // *
  epsspPowerSave1        = 0x00,                                          // *
  epsspPowerSave2        = 0x01,                                          // *
  epsspPowerSave3        = 0x02,                                          // *
  epsspPowerSaveClr      = 0x03,                                          // *
  epsspLowVoltage1       = 0x04,                                          // *
  epsspLowVoltage2       = 0x05,                                          // *
  epsspLowVoltage3       = 0x06,                                          // *
  epsspLowVoltageClr     = 0x07,                                          // *
  epsspBattCouplingSW1   = 0x08,                                          // *
  epsspBattCouplingSW2   = 0x09,                                          // *
  epsspBattCouplingSW3   = 0x0A,                                          // *
  epsspBattCouplingSWClr = 0x0B,                                          // *
  epsspAbnormalTemp_1st  = 0x0C,                                          // *
  epsspAbnormalTemp_Clr  = 0x0D,                                          // *
  epsspLimpHome_1st      = 0x0E,                                          // *
  epsspLimpHome_2nd      = 0x0F,                                          // *
  epsspLimpHome_3rd      = 0x10,                                          // *
  epsspLimpHome_Clr      = 0x11,                                          // *
  epsspProdMd_1st        = 0x12,                                          // *
  epsspProdMd_Clr        = 0x13,                                          // *
  epsspTransMd_1st       = 0x14,                                          // *
  epsspTransMd_Clr       = 0x15,                                          // *
  epsspAllClr            = 0xFF                                           // *
} ePwrServiceShutdownPopupType, EPWR_SHUTDOWN_POPUP_TYPE;                 // *
                                                                          // *
//                                                                           *
// End of Shutdown Condition Protocol enums & typedefs                       *
//****************************************************************************

//****************************************************************************
// CPMShowPowerPopup Protocol enums & typedefs                               *
//                                                                           *
typedef enum {                                                            // *
  epssppNormal           = 0x00,                                          // *
  epssppCritical         = 0x01,                                          // *
  epssppAppCritical      = 0x02,                                          // *
  epssppAllClr           = 0xFF                                           // *
} ePwrServicePowerPopupType, EPWR_POWER_POPUP_TYPE;                       // *
                                                                          // *
//                                                                           *
// End of CPMShowPowerPopup Protocol enums & typedefs                        *
//****************************************************************************

//****************************************************************************
// Limp Home Cutoff Protocol enums & typedefs                                *
//                                                                           *
typedef enum {                                                            // *
  epslhcINVALID   = 0xFF,                                                 // *
  epslhcDISABLED  = 0x00,                                                 // *
  epslhcENABLED   = 0x01                                                  // *
} ePwrServiceLHCType, EPWR_LHC_TYPE;                                      // *
                                                                          // *
//                                                                           *
// End of Limp Home Cutoff Protocol enums & typedefs                         *
//****************************************************************************

//****************************************************************************
// Production Mode Protocol enums & typedefs                                 *
//                                                                           *
typedef enum {                                                            // *
  epspmINVALID   = 0xFF,                                                  // *
  epspmDISABLED  = 0x00,                                                  // *
  epspmENABLED   = 0x01                                                   // *
} ePwrServiceProdModeType, EPWR_PROD_MODE_TYPE;                           // *
                                                                          // *
//                                                                           *
// End of Production Mode Protocol enums & typedefs                          *
//****************************************************************************

//****************************************************************************
// Transport Mode Protocol enums & typedefs                                  *
//                                                                           *
typedef enum {                                                            // *
  epstmINVALID   = 0xFF,                                                  // *
  epstmDISABLED  = 0x00,                                                  // *
  epstmENABLED   = 0x01                                                   // *
} ePwrServiceTransportModeType, EPWR_TRANSPORT_MODE_TYPE;                 // *
                                                                          // *
//                                                                           *
// End of Transport Mode Protocol enums & typedefs                           *
//****************************************************************************

//****************************************************************************
// Last User Mode, User Mode , User Mode Change Reason Protocol enums &      *
// typedefs                                                                  *
//                                                                           *
/**
 * \~english
 */
typedef enum {
  epsumINVALID    = 0xFF,   //!< \~english
  epsumOFF        = 0x00,   //!< \~english
  epsumON         = 0x01    //!< \~english
} ePwrServiceUserModeType, EPWR_USER_MODE_TYPE;

/**
 * \~english State transition reason.
 */
typedef enum {
  epsumcrNOT_AVAILABLE  = 0x00,   //!< \~english Not available.
  epsumcrON_KEY         = 0x01,   //!< \~english
  epsumcrPARKING_B,               //!< \~english Parking(+B) Power state transition.
  epsumcrPRE_BA,                  //!< \~english Pre-boot(+BA) Power state transition.
  epsumcrNORMAL,                  //!< \~english Normal-boot Power state transition.
  epsumcrBACKGROUND_BA,           //!< \~english Background-boot(+BA) Power state transition.
} ePwrServiceUserModeChangeReasonType, EPWR_USER_MODE_CHANGE_REASON_TYPE;
                                                                          // *
//                                                                           *
// End of Last User Mode, User Mode Protocol enums & typedefs                *
//****************************************************************************

/**
 * \~english
 */
typedef struct _upinfo {
  EPWR_WAKEUP_LEVEL_TYPE   level;                           //!< \~english
  EPWR_WAKEUP_FACTOR_TYPE  factor;                          //!< \~english
  EPWR_USER_MODE_CHANGE_REASON_TYPE  userModeChangeReason;  //!< \~english State transition reason.
} upInfo;

/**
 * \~english
 */
typedef struct _wakeinfo {
  EPWR_POWER_STATE_TYPE   powerupType;  //!< \~english
  upInfo up;                            //!< \~english
} wakeInfo;

//****************************************************************************
// System Mode Info enums & typedefs                                         *
//                                                                           *
typedef enum {                                                            // *
  epssinfINVALID      = 0xFF,                                             // *
  epssinfNORMAL       = 0x00,                                             // *
  epssinfPROGRAMMING  = 0x08,                                             // *
}ePowerSrvSystemModeInfo, EPWR_SYSTEM_MODE_INFO;                          // *
                                                                          // *
typedef enum {                                                            // *
  epssusfINVALID                = 0xFF,                                   // *
  epssusSYSTEM_SERVICES_STARTED = 0x00,                                   // *
  epssusALL_SERVICES_LAUNCHED   = 0x01                                    // *
}ePowerSrvStartupStageType, EPWR_STARTUP_STAGE_TYPE;                      // *
                                                                          // *
typedef struct _SystemModeInfo {                                          // *
  EPWR_USER_MODE_TYPE      lastUserMode; // NOTE: Don't use this. just only remained for backward compatibility.
  EPWR_LHC_TYPE            limpHomeCutoff;                                // *
  EPWR_PROD_MODE_TYPE      productionMode;                                // *
  EPWR_TRANSPORT_MODE_TYPE transportMode;                                 // *
  EPWR_SYSTEM_MODE_INFO    systemMode;                                    // *
  EPWR_STARTUP_STAGE_TYPE  startupStage;                                  // *
} SystemModeInfo;                                                         // *
//                                                                           *
// End of System Mode Info enums & typedefs                                  *
//****************************************************************************

//****************************************************************************
// Startup Confirmation enums & typedefs                                     *
//                                                                           *
typedef enum {                                                            // *
  eps_CWORD56_bmINVALID          = 0xFF,                                       // *
  eps_CWORD56_bmAPPLICATION_MODE = 0x00,                                       // *
  eps_CWORD56_bmPROGRAMMING_MODE = 0x08                                        // *
} EPWR_SC__CWORD56__BOOT_MODE_TYPE;                                            // *
                                                                          // *
typedef enum {                                                            // *
  epsstINVALID     = 0xFF,                                                // *
  epsstWARMSTART   = 0x00,                                                // *
  epsstCOLDSTART   = 0x01                                                 // *
} EPWR_SC_WAKEUP_TYPE;                                                    // *
                                                                          // *
typedef enum {                                                            // *
  epsscrtINVALID      = 0xFF,                                             // *
  epsscrtNOT_REQUIRED = 0x00,                                             // *
  epsscrtREQUIRED     = 0x01                                              // *
} EPWR_SC_COLD_START_REQ_TYPE;                                            // *
                                                                          // *
typedef enum {                                                            // *
  epsssINVALID = 0xFF,                                                    // *
  epsssUNLOCK  = 0x00,                                                    // *
  epsssLOCK    = 0x01                                                     // *
} EPWR_SC_SECURITY_STATUS;                                                // *
                                                                          // *
typedef struct {                                                          // *
  EPWR_SC__CWORD56__BOOT_MODE_TYPE _CWORD56_BootMode;                               // *
  EPWR_SC_WAKEUP_TYPE         wakeupType;                                 // *
  EPWR_SC_COLD_START_REQ_TYPE coldStartRequest;                           // *
  EPWR_SC_SECURITY_STATUS     securityStatus;                             // *
  UI_32                       HWVersion;                                  // *
  UI_32                       matchedHardwareType;                        // *
  UI_32                       softwareType;                               // *
  UI_32                       imageType;                                  // *
  UI_32                       majorVersion;                               // *
  UI_32                       minorVersion;                               // *
  UI_32                       SWVersionYear;                              // *
  UI_32                       SWVersionWeek;                              // *
  UI_32                       SWVersionPatchLevel;                        // *
} StartupConfirmationMsgStrut, EPWR_SC_MSG_STRUCT;                        // *
//                                                                           *
// End of Startup Confirmation enums & typedefs                              *
//****************************************************************************

//****************************************************************************
// Power Request Message enums & typedefs                                    *
//                                                                           *
/**
 * _CWORD102_ Startup Information
 */
typedef enum {
  epsprm_CWORD102_siINVALID                = 0xFF,  //!< \~english
  epsprm_CWORD102_si_CWORD102__STARTUP_NORMAL      = 0x00,  //!< Normal Startup
  epsprm_CWORD102_si_CWORD102__STARTUP_AFTER_RESET = 0x01   //!< Startup after Irregular Reset
}ePowerSrv_CWORD102_StartupInfoType, EPWR__CWORD102__STARTUP_INFO_TYPE;
/**
 * Opening Type
 */
typedef enum {
  epsprmotINVALID               = 0xFF,   //!< \~english
  epsprmotTIMING_TYPE_1         = 0x00,   //!< CI/DI Show timing type1
  epsprmotTIMING_TYPE_2         = 0x01    //!< CI/DI Show timing type2
} ePowerSrvPwrReqMsgCIDIOpeningType, EPWR_OPENING_TYPE;
/**
 * Manual Reset Information
 */
typedef enum {
  epsprmriINVALID               = 0xFF,   //!< \~english
  epsprmriNORMAL                = 0x00,   //!< \~english
  epsprmriWAKEUP_AFTER_RESET    = 0x01    //!< \~english
} ePowerSrvPwrManResetInfoType, EPWR_MAN_RESET_INFO_TYPE;
/**
 * Last System Retention Request
 */
typedef enum {
  epsprlsrrINVALID               = 0xFF,  //!< \~english
  epsprlsrrLAST_SYSTEM_OFF       = 0x00,  //!< \~english
  epsprlsrrLAST_SYSTEM_ON        = 0x01   //!< \~english
} ePowerSrvTypeLastSysRetReq, EPWR_LAST_SYS_RET_REQ_TYPE;

// *
typedef struct {                                                          // *
  EPWR__CWORD102__STARTUP_INFO_TYPE          startupType;                         // *
  EPWR_OPENING_TYPE                  openingType;                         // *
  EPWR_WAKEUP_FACTOR_TYPE            startupReason;                       // *
  EPWR_USER_MODE_TYPE                lastUserMode;                        // *
  EPWR_MAN_RESET_INFO_TYPE           manResetInfo;                        // *
  EPWR_LAST_SYS_RET_REQ_TYPE         lastSystemRetentionReq;              // *
  EPWR_USER_MODE_TYPE                userMode;                            // *
} PowerRequestMsgStrut, EPWR_POWER_REQUEST_MSG_STRUCT;                    // *

/**
 * \~english structure for power request message
 */
typedef struct {                                              
  EPWR__CWORD102__STARTUP_INFO_TYPE          startupType;             //!< \~english
  EPWR_OPENING_TYPE                  openingType;             //!< \~english
  EPWR_WAKEUP_FACTOR_TYPE            startupReason;           //!< \~english
  EPWR_USER_MODE_TYPE                lastUserMode;            //!< \~english
  EPWR_MAN_RESET_INFO_TYPE           manResetInfo;            //!< \~english
  EPWR_LAST_SYS_RET_REQ_TYPE         lastSystemRetentionReq;  //!< \~english
  EPWR_USER_MODE_TYPE                userMode;                //!< \~english
  EPWR_USER_MODE_CHANGE_REASON_TYPE  userModeChangeReason;    //!< \~english State transition reason.
} PowerRequestMsgStrutWithUMCR, EPWR_POWER_REQUEST_MSG_STRUCT_WITH_UMCR;
//                                                                           *
// End of Power Request Message enums & typedefs                             *
//****************************************************************************



//****************************************************************************
// Shutdown Request Message enums & typedefs                                 *
//                                                                           *
//  ONS                                                                      *
//                                                                           *
typedef enum {                                                            // *
  epssdmonsINVALID             = 0xFF,                                    // *
  epssdmonsNO_ONS              = 0x00,                                    // *
  epssdmonsONS                 = 0x01                                     // *
} ePowerSrvONSType, EPWR_ONS_TYPE;                                        // *
//                                                                           *
//  Shutdown Trigger                                                         *
//                                                                           *
typedef enum {                                                            // *
  epssdmsdtINVALID             = 0xFF,                                    // *
  epssdmsdtTESTACC_OFF         = 0x00,                                    // *
  epssdmsdtON_KEY              = 0x01,                                    // *
  epssdmsdtIGN_LOCK            = 0x02,                                    // *
  epssdmsdtPWR_SAVE            = 0x03,                                    // *
  epssdmsdtTMP_STARTUP         = 0x04,                                    // *
  epssdmsdtDIAG_DEACTIVATION   = 0x05,                                    // *
  epssdmsdtABNORMAL_VOLTAGE    = 0x06,                                    // *
  epssdmsdtABNORMAL_TEMP       = 0x07,                                    // *
  epssdmsdtBATTERYCUTOFF       = 0x08,                                    // *
  epssdmsdtLIMPHOME            = 0x09,                                    // *
  epssdmsdtHU_CAN_ERROR        = 0x0A,                                    // *
  epssdmsdtBODY_CAN_ERROR      = 0x0B,                                    // *
  epssdmsdtTRANSPORT_MODE      = 0x0C,                                    // *
  epssdmsdtPRODUCTION_MODE     = 0x0D,                                    // *
  epssdmsdtIGN_OFF             = 0x0E,                                    // *
  epssdmsdtGENERIC_ERROR_RESET = 0x0F,                                    // *
  epssdmsdtFATAL_ERROR_RESET   = 0x10,                                    // *
  epssdmsdtUSER_DATA_RESET     = 0x11,                                    // *
  epssdmsdtFACTORY_DATA_RESET  = 0x12,                                    // *
  epssdmsdtFAST_SLEEP_MODE     = 0x13,                                    // *
  epssdmsdtNORMAL_RESET        = 0x14,                                    // *
  epssdmsdtPROGUPDATE_RESET    = 0x15,      // cannot use this
    //                                                                       *
    // Note to self: Make sure to update all enum-to-enum and                *
    // enum-to-string maps when changing these enums !                       *
} ePowerSrvPwrShutdownTriggerType, EPWR_SHUTDOWN_TRIGGER_TYPE;            // *
//                                                                           *
typedef struct {                                                          // *
  EPWR_ONS_TYPE                  ONS_Type;                                // *
  EPWR_SHUTDOWN_TRIGGER_TYPE     shutdownTrigger;                         // *
  EPWR_USER_MODE_TYPE            lastUserMode;                            // *
  EPWR_TRANSPORT_MODE_TYPE       transportMode;                           // *
  EPWR_PROD_MODE_TYPE            productionMode;                          // *
  EPWR_LHC_TYPE                  limpHomeCutoffRequest;                   // *
} ShutdownRequestMsgStrut, EPWR_SHUTDOWN_REQUEST_MSG_STRUCT;              // *
//                                                                           *
// End of Shutdown Request Message enums & typedefs                          *
//****************************************************************************

//****************************************************************************
// HeartBeat enums & typedefs                                                *
//                                                                           *
// HeartBeat Request Message                                                 *
typedef struct {                                                          // *
  UI_8  IntervalSec;                                                      // *
} EPWR_HB_REQ_MSG_STRUCT;                                                 // *
//                                                                           *
// HeartBeat Response Message                                                *
// Zero byte data message returned.                                          *
// End of HeartBeat enums & typedefs                                         *
//****************************************************************************

//****************************************************************************
// CPU Reset Reason enums & typedefs                                         *
typedef  enum {                                                           // *
    epsCpuResetReasonGeneric = 0                                          // *
  , epsCpuResetReasonFatalError                                           // *
  , epsCpuResetReasonUserDataReset                                        // *
  , epsCpuResetReasonFactoryDataReset                                     // *
  , epsCpuResetReasonUserForceReset                                       // *
  , epsCpuResetReasonShipmentModeReset                                    // *
  , epsCpuResetReasonHeadUnitReset                                        // *
  , epsCpuResetReasonNormalReset                                          // *
  , epsCpuResetReasonProgupdateReset    // cannot use this
} epsCpuResetReason;                                                      // *
//
typedef struct {
#define PWR_RESET_MSG_STR_SIZE 32
    epsCpuResetReason resetReason;                                        // *
    CHAR              messageStr[PWR_RESET_MSG_STR_SIZE];                 // *
} SS_Pwr_CpuResetMsgStruct;
//                                                                           *
// End of CPU Reset Reason enums & typedefs                                  *
//****************************************************************************

typedef struct {
  union _data {
    wakeInfo wake;

    struct _crank {
      EPWR_CRANK_STATE_TYPE   state;
    }crank;

    struct _voltage {
      EPWR_VOLTAGE_STATE_TYPE state;
    }voltage;

    struct _ws_complete {
      EPWR_WAKEUP_LEVEL_TYPE   level;
      EPWR_WAKEUP_FACTOR_TYPE  factor;
    }wk_shdwn_complete;

    struct _commwake {
      EPWR_CAN_STATE_TYPE   state;
    }commwake;

    struct _UserMode {
      EPWR_USER_MODE_TYPE    mode;
    } user_mode;

    struct _ShutdownPopup {
      EPWR_SHUTDOWN_POPUP_TYPE shutdownPopupEvent;
    } shutdownPopup;

    struct _PowerPopup {
      EPWR_POWER_POPUP_TYPE  powerPopupEvent;
    } powerPopup;

    StartupConfirmationMsgStrut  startupConfirmationMsg;
    PowerRequestMsgStrutWithUMCR powerRequestMsg;
    ShutdownRequestMsgStrut      shutdownRequestMsg;
  }data;
} Pwr_ServiceSetInterface;

#endif /* SS_POWER_SERVICE_H_ */  // NOLINT (build/header_guard)

/** @}*/  // end of power_service
/** @}*/  // end of system_service
/** @}*/  // end of BaseSystem
