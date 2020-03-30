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
/// \ingroup  system_manager
/// \file     ss_system_manager_if.h
/// \brief    This file supports the System Manager client interface.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef SS_SYSTEM_MANAGER_IF_H_  // NOLINT (build/header_guard)
#define SS_SYSTEM_MANAGER_IF_H_

#include <native_service/frameworkunified_types.h>
#include <system_service/ss_sm_ram_access.h>
#include "system_service/ss_system_if.h"
#include "system_service/ss_error_event_common.h"
#include "system_service/ss_power_service.h"

/// \ingroup system_manager
/// \brief Boot mode information
typedef enum {
    e_SS_SM_BOOT_MODE_INVALID     = -1
  , e_SS_SM_BOOT_MODE_APPLICATION
  , e_SS_SM_BOOT_MODE_PROGRAMMING
}ESMBootModeInfo;

/// \ingroup system_manager
/// \brief Data reset mode information
typedef enum {
    e_SS_SM_DATA_RESET_MODE_NONE
  , e_SS_SM_DATA_RESET_MODE_USER
  , e_SS_SM_DATA_RESET_MODE_FACTORY
  , e_SS_SM_DATA_RESET_MODE_PROGUPDATE  // cannot use this
}ESMDataResetModeInfo;

/// \ingroup system_manager
/// \brief Program update status
typedef UI_32 SMProgUpdateState;
#define SS_SM_PROG_UPDATE_STATE_NONE            0x0000
#define SS_SM_PROG_UPDATE_STATE_UPDATED         0x0001
#define SS_SM_PROG_UPDATE_STATE_MAP_UPDATED     0x0002
#define SS_SM_PROG_UPDATE_STATE_MAPDIFF_UPDATED 0x0004

/// Set Next Wakeup type information
typedef enum {
    e_SS_SM_NEXT_WAKEUP_TYPE_NONE = 0
  , e_SS_SM_NEXT_WAKEUP_TYPE_COLD
  , e_SS_SM_NEXT_WAKEUP_TYPE_HOT
}ESMNextWakeupType;

/// \ingroup system_manager
/// \brief DRAM backup status
typedef enum {
    e_SS_SM_DRAM_BACKUP_UNSET = -1
  , e_SS_SM_DRAM_BACKUP_OK
  , e_SS_SM_DRAM_BACKUP_NG
}ESMDramBackupStatus;

/// \ingroup system_manager
/// \brief Reset status
typedef enum {
    e_SS_SM_RESET_STATUS_UNSET = -1
  , e_SS_SM_RESET_STATUS_NONE
  , e_SS_SM_RESET_STATUS_NG
  , e_SS_SM_RESET_STATUS_IMMEDIATE
}ESMResetStatus;

/// \ingroup system_manager
/// \brief Relaunch status
typedef enum {
    e_SS_SM_RELAUNCH_STATUS_NONE
  , e_SS_SM_RELAUNCH_STATUS_SAFE
  , e_SS_SM_RELAUNCH_STATUS_ERR
}EMRelaunchStatus;


/// \ingroup system_manager
/// \brief CPU Reset Reason
typedef enum {
    e_SS_SM_CPU_RESET_REASON_INVALID = -1
  , e_SS_SM_CPU_RESET_REASON_NORMAL  =  0
  , e_SS_SM_CPU_RESET_REASON_DATA_RESET
  , e_SS_SM_CPU_RESET_REASON_USER_FORCE_RESET
  , e_SS_SM_CPU_RESET_REASON_GENERIC
  , e_SS_SM_CPU_RESET_REASON_GENERIC_ERR = e_SS_SM_CPU_RESET_REASON_GENERIC
  , e_SS_SM_CPU_RESET_REASON_DSP_ERR
  , e_SS_SM_CPU_RESET_REASON_IMMRESET_NORMAL
  , e_SS_SM_CPU_RESET_REASON_CRITICAL_ERR  // DRAM backup will be discarded after reset
}ESMCpuResetReason;

/// \ingroup system_manager
/// \brief CPU Reset Info
typedef struct T_SystemManagerCpuResetInfo {
#define SS_SM_RESET_MSG_STR_SIZE 64
#define SS_SM_SUFFIX_STR_SIZE  32
    ESMCpuResetReason resetReason;
    CHAR              messageStr[SS_SM_RESET_MSG_STR_SIZE];
    CHAR              suffixStr[SS_SM_SUFFIX_STR_SIZE];
    T_SystemManagerCpuResetInfo
       () : resetReason(e_SS_SM_CPU_RESET_REASON_INVALID) {
    }
}TSystemManagerCpuResetInfo;


/// \ingroup system_manager
/// \brief Data Reset Reason
typedef enum {
    e_SS_SM_DATA_RESET_TYPE_USER = 0
  , e_SS_SM_DATA_RESET_TYPE_FACTORY  // AKA Shipment Mode
  , e_SS_SM_DATA_RESET_TYPE_CONFIGURATION
  , e_SS_SM_DATA_RESET_TYPE_PROGUPDATE    // cannot use this
}ESMDataResetType;

typedef enum {
    e_SS_SM_SVC_WAKEUP_STATUS_COMPLETE = 0x01
}ESMServiceWakeupStatus;

/// \ingroup system_manager
typedef struct T_SystemManagerBootModeRequestResponse {
  ESMBootModeInfo hostBootMode;
  EFrameworkunifiedStatus      eFrameworkunifiedStatus;
}TSystemManagerBootModeRequestResponse;

typedef enum {
    e_SS_SM_BOOT_INFO_MDUPDATE,

//    e_SS_SM_BOOT_INFO_SDUPDATE
    e_SS_SM_BOOT_INFO_SDUPDATE,
    e_SS_SM_BOOT_INFO_OPDTUPDATE

}ESMBootInfoType;

typedef struct {
    ESMBootInfoType type;
    UI_32           value;
}TSystemManagerBootInfoStruct;

/// \ingroup system_manager
/// \brief Data Reset Reason
typedef struct T_SS_BootModeListStruct {
  ESMBootModeInfo             hostBootMode;
  EPWR_SC__CWORD56__BOOT_MODE_TYPE _CWORD56_BootMode;
} TSS_BootModeListStruct;

typedef struct T_SystemManagerWakeupOrderInfo {
    char order_name[SS_SM_ORDER_NAME_MAX];
} TSystemManagerWakeupOrderInfo;

//****************************************************************************
/// \ingroup system_manager                                                   *
/// Data passed in the NTFY_SSSystemMgrPowerOnOff and                        *
/// NTFY_SSSystemMgrUserMode notification messages                           *

/// \ingroup system_manager
typedef struct T_SS_SM_UserModeOnOffNotification_Struct {                 // *
    BOOL                              isUserModeOn;                       // *
    EPWR_WAKEUP_FACTOR_TYPE           startupReason;                      // *
    EPWR_USER_MODE_CHANGE_REASON_TYPE userModeChangeReason;               // *
    T_SS_SM_UserModeOnOffNotification_Struct                              // *
       () : isUserModeOn(FALSE),                                          // *
            startupReason(epswfINVALID),                                  // *
            userModeChangeReason(epsumcrNOT_AVAILABLE) {                  // *
    }                                                                     // *
    T_SS_SM_UserModeOnOffNotification_Struct                              // *
        (BOOL                              f_bUserMode,                   // *
         EPWR_WAKEUP_FACTOR_TYPE           f_startupReason,               // *
         EPWR_USER_MODE_CHANGE_REASON_TYPE f_changeReason)                // *
           : isUserModeOn(f_bUserMode),                                   // *
             startupReason(f_startupReason),                              // *
             userModeChangeReason(f_changeReason) {                       // *
    }                                                                     // *
                                                                          // *
} T_SS_SM_UserModeOnOffNotification_StructType;                           // *
//                                                                           *
// End of Data passed as part of NTFY_SSSystemMgrPowerOnOff notification ... *
//****************************************************************************


//****************************************************************************
/// \ingroup system_manager
/**
 * \~english Parameters of the following message. \n
 *            SS_SM_START             'FrameworkunifiedOnStart'/'evStart' \n
 *            SS_SM_PRE_START         'FrameworkunifiedOnPreStart'/'evPreStart' \n
 *            SS_SM_BACKGROUND_START  'FrameworkunifiedOnBackgroundStart'/'evBackgroundStart' \n
 */
typedef struct T_SS_SM_START_DataStruct {
    EPWR_WAKEUP_FACTOR_TYPE           startupReason;        //!< \~english
    BOOL                              isUserModeOn;         //!< \~english
    ESMDataResetModeInfo              dataResetMode;        //!< \~english
    EPWR_SC_SECURITY_STATUS           securityStatus;       //!< \~english
    EPWR_SC_WAKEUP_TYPE               wakeupType;           //!< \~english
    ESMDramBackupStatus               dramBackupStatus;     //!< \~english
    ESMResetStatus                    resetStatus;          //!< \~english
    UI_32                             errResetCount;        //!< \~english
    /**
     *  \~english Constructor
     */
    T_SS_SM_START_DataStruct
       () : startupReason(epswfINVALID),
            isUserModeOn(FALSE),
            dataResetMode(e_SS_SM_DATA_RESET_MODE_NONE),
            securityStatus(epsssINVALID),
            wakeupType(epsstINVALID),
            dramBackupStatus(e_SS_SM_DRAM_BACKUP_UNSET),
            resetStatus(e_SS_SM_RESET_STATUS_UNSET),
            errResetCount(0) {}
    /**
     *  \~english Constructor
     */
    T_SS_SM_START_DataStruct
        (EPWR_WAKEUP_FACTOR_TYPE  f_startupReason,
         BOOL                     f_isUserModeOn,
         ESMDataResetModeInfo     f_dataResetMode,
         EPWR_SC_SECURITY_STATUS  f_securityStatus,
         EPWR_SC_WAKEUP_TYPE      f_wakeupType,
         ESMDramBackupStatus      f_dramBackupStatus,
         ESMResetStatus           f_resetStatus,
         UI_32                    f_resetCount)
           : startupReason(f_startupReason),
             isUserModeOn(f_isUserModeOn),
             dataResetMode(f_dataResetMode),
             securityStatus(f_securityStatus),
             wakeupType(f_wakeupType),
             dramBackupStatus(f_dramBackupStatus),
             resetStatus(f_resetStatus),
             errResetCount(f_resetCount) {}
} T_SS_SM_START_DataStructType;
//                                                                           *
// End of Data passed as part of SS_SM_START 'FrameworkunifiedOnStart'/'evStart' message  *
//****************************************************************************

//****************************************************************************
/// \ingroup system_manager
/// Data passed as part of SS_SM_STOP 'FrameworkunifiedOnStop'/'evStop' message            *
typedef struct T_SS_SM_STOP_DataStruct {                                  // *
  EPWR_SHUTDOWN_TRIGGER_TYPE     shutdownTrigger;                       // *
  EPWR_USER_MODE_TYPE            lastUserMode;                          // *
} T_SS_SM_STOP_DataStructType;                                            // *
//                                                                           *
// End of Data passed as part of SS_SM_STOP 'FrameworkunifiedOnStop'/'evStop' message  // *
//****************************************************************************

//****************************************************************************
/// \ingroup system_manager
/// Extended Parameter for FrameworkunifiedOnStart                                        *
typedef struct {                                                          // *
#define SS_SM_START_EXT_INFO_SIZE 64                                      // *
#define SS_SM_START_EXT_BODY_SIZE ( sizeof(BOOL) \
                                    + sizeof(EMRelaunchStatus) \
                                    + sizeof(BOOL) \
                                    + sizeof(BOOL) \
                                    )

#define SS_SM_START_EXT_RSV_SIZE  SS_SM_START_EXT_INFO_SIZE \
                                - SS_SM_START_EXT_BODY_SIZE               // *
        BOOL             isProgUpdated;                                   // *
        EMRelaunchStatus relaunchStatus;                                  // *
        BOOL             isMapUpdated;                                    // *
        BOOL             isMapDiffUpdated;                                // *
        uint8_t          reserved[SS_SM_START_EXT_RSV_SIZE];              // *
} T_SS_SM_START_ExtDataStructType;                                        // *
//                                                                           *
// End of Extended Parameter for FrameworkunifiedOnStart                                  *
//****************************************************************************

//****************************************************************************
/// \ingroup system_manager
/// Extended Parameter for FrameworkunifiedOnStop                                         *
typedef struct {                                                          // *
#define SS_SM_STOP_EXT_INFO_SIZE 64                                       // *
#define SS_SM_STOP_EXT_BODY_SIZE (sizeof(BOOL))                           // *
#define SS_SM_STOP_EXT_RSV_SIZE  SS_SM_STOP_EXT_INFO_SIZE \
                               - SS_SM_STOP_EXT_BODY_SIZE                 // *
        BOOL     isProgUpdated;                                           // *
        uint8_t  reserved[SS_SM_STOP_EXT_RSV_SIZE];                       // *
}T_SS_SM_STOP_ExtDataStructType;                                          // *
//                                                                           *
// End of Extended Parameter for FrameworkunifiedOnStop                                   *
//****************************************************************************

struct SMErrorEventNtfData {
#define SM_MODULE_NAME_SIZE 64
  EErrorEventType  EventType;
  CHAR             ModuleName[SM_MODULE_NAME_SIZE];
  BOOL             isNeedReboot;
};

//*************************************************************************
// Boot Micro Logging Protocol                                            *
//                                                                        *
enum eSMBootMicroResetReason {
    SS_SM_BOOT_MICRO_RESET_REASON_SELF_RESET = 0,
    SS_SM_BOOT_MICRO_RESET_REASON_USER_FORCE_RESET,
    SS_SM_BOOT_MICRO_RESET_REASON_DSP_RESET,
    SS_SM_BOOT_MICRO_RESET_REASON_HB_TIMEOUT
};
//                                                                        *
// End of Boot Micro Logging Protocol                                     *
//*************************************************************************


//****************************************************************************
/// \ingroup system_manager                                                        *
/// Error Event Logging enumerations                                         *
///
enum eSMUserLogType {
    e_SS_SM_CAPTURE_ALL_LOGS = 1,  //!< \~english Capture all logs
    e_SS_SM_SCREEN_CAPTURE,  //!< \~english Capture screenshot
    e_SS_SM_CAPTURE_INTERFACEUNIFIED_LOGS,  //!< \~english Capture application log
    e_SS_SM_USER_FORCE_RESET,  //!< \~english Capture logs are forcibily reset
    e_SS_SM_CAPTURE_DEV_LOGS,  //!< \~english Capture device log
    e_SS_SM_CAPTURE_MODULE_LOGS,  //!< \~english Capture debug log
    e_SS_SM_CAPTURE_DTC_LOGS,  //!< \~english Capture DTC event log
    e_SS_SM_CAPTURE_NAVI_LOGS,  //!< \~english Capture Navi event log
    e_SS_SM_CAPTURE_GROUP_RELAUNCH  //!< \~english Capture log by Group Relaunch
};
//                                                                           *
// Error Event Logging enumerations                                          *
//****************************************************************************

//****************************************************************************
/// \ingroup system_manager                                                        *
/// \brief Error Event Logging Info                                          *
typedef struct T_SystemManagerLoggingRequestInfo {
#define SS_SM_LOG_MSG_STR_SIZE 64
    eSMUserLogType    logType;
    CHAR              messageStr[SS_SM_LOG_MSG_STR_SIZE];
    CHAR              suffixStr[SS_SM_SUFFIX_STR_SIZE];
}TSystemManagerLoggingRequestInfo;
//
// Error Event Logging Info                                                  *
//****************************************************************************

//****************************************************************************
/// \ingroup system_manager                                                        *
/// \brief Error Event Clear Logs Info                                       *
typedef struct T_SystemManagerClearLogsInfo {
    void *rsv;
}TSystemManagerClearLogsInfo;
//
// Error Event Clear Logs Info                                               *
//****************************************************************************

#endif /* SS_SYSTEM_MANAGER_IF_H_ */  // NOLINT (build/header_guard)
