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

/**
 * @file
 *    positioning_application.cpp
 * @brief
 *   Module : POSITIONING
 *   Implements Vehicle service functionality
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_multithreading.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/ns_message_center_if.h>
#include <peripheral_service/ps_services.h>
#include <vehicle_service/POS_sensor_API.h>
#include <vehicle_service/POS_gps_API.h>
#include <system_service/ss_sm_client_if.h>
#include <vehicle_service/positioning_base_library.h>
//#include <stub/vehicle_notifications.h>
#include <peripheral_service/communication_notifications.h>
#include <other_service/VP_GetEnv.h>
#include <cstdlib>
#include <iostream>

#include "SensorLog.h"
#include "positioning_common.h"
#include "POS_private.h"
#include "Gps_API_private.h"
#include "POS_common_private.h"
#include "Vehicle_API.h"
#include "Vehicle_API_private.h"
#include "Sensor_API_private.h"
#include "Naviinfo_API.h"
#include "VehicleSensor_Thread.h"
#include "ClockGPS_Process_Proto.h"
#include "VehicleSens_Common.h"
#include "VehicleSens_DataMaster.h"
#include "VehicleSens_DeliveryCtrl.h"
#include "VehicleUtility.h"
#include "BackupMgrIf.h"
#include "ClockIf.h"
#include "CommUsbIf.h"
#include "DevDetectSrvIf.h"
#include "DiagSrvIf.h"
#include "PSMShadowIf.h"
#include "VehicleIf.h"
//#include "positioning_hal.h"
//#include "gps_hal.h"
#include "CommonDefine.h"

#include "VehicleIf.h"

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/
#define DATMOD_RETRY    (3) /* Number of shared memory generation retries */
#define DATMOD_PREINIT  (0) /* Shared Memory State Before Initialization */
#define PRIM_NAME_MAX   (32) /* Maximum Name Size */

/* Mask for managing various notification reception conditions */
#define NTFY_MSK_NONE                       (0x00)
/* Service availability notification */
#define NTFY_MSK_COMMUNICATION_AVAILABILITY (0x01)
#define NTFY_MSK_PS_COMMUSB_AVAILABILITY    (0x02)
#define NTFY_MSK_PS_PSMSHADOW_AVAILABILITY  (0x04)
#define NTFY_MSK_CLOCK_AVAILABILITY         (0x08)
#define NTFY_MSK_NS_BACKUPMGR_AVAILABILITY  (0x10)
#define NTFY_MSK_SS_DEVDETSRV_AVAILABILITY  (0x20)
#define NTFY_MSK_VS_VEHICLE_AVAILABILITY    (0x40)
/* Other Notices */
#define NTFY_MSK_PS_PSMSHADOW_INIT_COMP     (0x01) /* PSMShadow startup completion notice           */

/* Thread state */
#define THREAD_STS_NOEXIST                  (0x00)
#define THREAD_STS_CREATING                 (0x01)
#define THREAD_STS_CREATED                  (0x02)

/*
  Maximum message size
  Note : Set a value that is larger than the local thread's message reception buffer size.
*/
#define MAX_MSG_BUF_SIZE                    (2048)

#define POS_SNDMSG_DTSIZE_1     1       /* SndMSG data size 1Byte  */
#define POS_SNDMSG_DTSIZE_2     2       /* SndMSG data size 2Byte  */
#define POS_SNDMSG_DTSIZE_20     20     /* SndMSG data size of 20 bytes  */
#define POS_SNDMSG_DTSIZE_132     132     /* SndMSG data size: 132 bytes  */

/* PositioningLogFlag */
#define POSITIONINGLOG_FLAG_NAVI 9

/* Definition for thinning out sensor log at anomaly */
#define POSITIONINGLOG_SYS_1_ABNORMAL_DATA_NUM  3
#define POSITIONINGLOG_SYS_2_ABNORMAL_DATA_NUM  4
#define POSITIONINGLOG_SYS_3_ABNORMAL_DATA_NUM  4
#define POSITIONINGLOG_SYS_4_ABNORMAL_DATA_NUM 129
#define POSITIONINGLOG_SYS_ABNORMAL_DATA_NUM (POSITIONINGLOG_SYS_1_ABNORMAL_DATA_NUM + \
                                      POSITIONINGLOG_SYS_2_ABNORMAL_DATA_NUM + \
                                      POSITIONINGLOG_SYS_3_ABNORMAL_DATA_NUM + \
                                      POSITIONINGLOG_SYS_4_ABNORMAL_DATA_NUM)
#define POSITIONINGLOG_SYS_1_ABNORMAL_DATA_OFFSET 11
#define POSITIONINGLOG_SYS_2_ABNORMAL_DATA_OFFSET 32
#define POSITIONINGLOG_SYS_3_ABNORMAL_DATA_OFFSET 54
#define POSITIONINGLOG_SYS_4_ABNORMAL_DATA_OFFSET 114
#define POSITIONINGLOG_SYS_1_ABNORMAL_SET_DATA_OFFSET 0
#define POSITIONINGLOG_SYS_2_ABNORMAL_SET_DATA_OFFSET POSITIONINGLOG_SYS_1_ABNORMAL_DATA_NUM
#define POSITIONINGLOG_SYS_3_ABNORMAL_SET_DATA_OFFSET (POSITIONINGLOG_SYS_1_ABNORMAL_DATA_NUM + POSITIONINGLOG_SYS_2_ABNORMAL_DATA_NUM)
#define POSITIONINGLOG_SYS_4_ABNORMAL_SET_DATA_OFFSET (POSITIONINGLOG_SYS_1_ABNORMAL_DATA_NUM + \
                                               POSITIONINGLOG_SYS_2_ABNORMAL_DATA_NUM + \
                                               POSITIONINGLOG_SYS_3_ABNORMAL_DATA_NUM)
#define POSITIONINGLOG_SYS_OPC_OFFSET 9
#define POSITIONINGLOG_SYS_PULSE_TIME_NUM_OFFSET 114
#define POSITIONINGLOG_SYS_NORMAL_DATA 0xC1
#define POSITIONINGLOG_SYS_FST_DATA 0xF4
#define _pb_strcat(pdest, psrc, size)   (strncat(pdest, psrc, size) , (0))

enum LsDrvKind {
  LSDRV_GYRO,               /* Gyro output     */
  LSDRV_SPEED_PULSE,        /* Vehicle speed pulse       */
  LSDRV_SPEED_PULSE_FLAG,   /* Vehicle speed pulse flag  */
  LSDRV_SPEED_KMPH,         /* Vehicle speed(km/h)      */
  LSDRV_GYRO_EXT,           /* Gyro Extended Output  */
  LSDRV_REV,                /* REV information         */
  LSDRV_GYRO_TEMP,          /* Gyro Temperature      */
  LSDRV_GSENSOR_X,          /* G-sensor X-axis      */
  LSDRV_GSENSOR_Y,          /* G-sensor Y-axis      */
  LSDRV_PULSE_TIME,         /* Inter-Pulse Time      */
  LSDRV_SNS_COUNTER,        /* Sensor Counter    */
  LSDRV_GPS_INTERRUPT_FLAG, /* GPS interrupt flag */
  LSDRV_KINDS_MAX
};

// Vehicle sensor information notification message
typedef struct {
  uint32_t did;         // Data ID corresponding to vehicle sensor information
  uint16_t size;        // Data size of vehicle sensor information
  uint8_t rcv_flag;      // Vehicle sensor information reception flag
  uint8_t reserve;      // Reserved
  uint8_t data[VEHICLE_VSINFO_DSIZE];  // Vehicle sensor information
} VEHICLE_UNIT_MSG_VSINFO_DAT;

// Vehicle sensor information notification message
typedef struct {
  VEHICLE_UNIT_MSG_VSINFO_DAT data;  // Message data
} VEHICLE_UNIT_MSG_VSINFO;
/*---------------------------------------------------------------------------------*
 * Structre                                                                        *
 *---------------------------------------------------------------------------------*/
/*!
   @brief   Structure to create shared data
*/
typedef struct {
    char    share_data_name[PRIM_NAME_MAX]; /**< Shared data name         */
    u_int32 data_size;                    /**< Shared data size    */
} ST_SHAREDATA;

/*!
   @brief   Thread management information
*/
typedef struct {
    EnumTID_POS     id;           /**< Thread ID                                  */
    const int8_t*   p_name;        /**< Thread name                                  */
    PNO             pno;          /**< Process number                                */
    CbFuncPtr       routine;      /**< Start Routine                             */
    uint8_t         msk_available; /**< Dependent services Availability                    */
    uint8_t         msk_ntfy;      /**< Dependency notification                                   */
    uint8_t         msk_thread;    /**< Dependent threads                               */
    BOOL            is_depended;   /**< Positioning/Availability->TRUE change dependency */
    uint8_t         status;       /**< Thread activation state                             */
    uint8_t         order;        /**< Boot Sequence(Performance)                              */
    uint8_t         reserve[2];
} ST_THREAD_CREATE_INFO;


/* GPS fix count information */
typedef struct {
  uint32_t ul3d;                    /* 3D */
  uint32_t ul2d;                    /* 2D */
  uint32_t ul_else;                  /* Not fix */
  uint8_t dummy[4];                 /* Dummy */
} ST_GPS_FIX_CNT;

/*!
    @brief Structure that stores the time set by the time setting or the time updated(For GRADE1)
*/
typedef struct {
    u_int16 year; /* Year */
    u_int8 month; /* Month */
    u_int8 date;  /* Day */
    u_int8 hour;  /* Hour */
    u_int8 minute;/* Minutes */
    u_int8 second;/* Minutes */
    u_int8 flag;  /* Whether or not the time is set */
} ST_GPS_SET_TIME;

/*!
    @brief Structure for managing whether time setting is enabled or disabled(For GRADE2)
*/
typedef struct {
    u_int8 flag;        /* Whether or not the time is set */
    u_int8 reserve[3];
} ST_GPS_SET_TIME_FLAG;
/*---------------------------------------------------------------------------------*
 * Local Function Prototype                                                        *
 *---------------------------------------------------------------------------------*/
static EFrameworkunifiedStatus PositioningOnStartImpl(const HANDLE hApp, const EPWR_SC_WAKEUP_TYPE wakeupType,
                                 const ESMDataResetModeInfo dataResetMode);
static EFrameworkunifiedStatus PosNotifyCommunicationAvailability(HANDLE h_app);
static EFrameworkunifiedStatus PosNotifyCommUSBAvailability(HANDLE h_app);
static EFrameworkunifiedStatus PosNotifyPSMShadowAvailability(HANDLE h_app);
static EFrameworkunifiedStatus PosNotifyPSMShadowInitComp(HANDLE h_app);
static EFrameworkunifiedStatus PosNotifyClockAvailability(HANDLE h_app);
static EFrameworkunifiedStatus PosNotifyNSBackupMgrAvailability(HANDLE h_app);
static EFrameworkunifiedStatus PosNotifyDevDetectSrvAvailability(HANDLE h_app);
static EFrameworkunifiedStatus PosNotifyVehicleAvailability(HANDLE h_app);

static EFrameworkunifiedStatus PosStopThreadDummy(HANDLE h_app);
static void PosCreateSharedMemory(void);
static void PosCreateThread(HANDLE h_app);
static void PosStopThread(void);
static void PosBackupDataInit(void);

/* Message Dispatching Functions */
static EFrameworkunifiedStatus PosThreadCreateComp(HANDLE h_app);
static EFrameworkunifiedStatus PosThreadStopComp(HANDLE h_app);
static EFrameworkunifiedStatus PosPosifRegisterListenerPkgSensorData(HANDLE h_app);
static EFrameworkunifiedStatus PosPosifRegisterListenerSensorData(HANDLE h_app);
static EFrameworkunifiedStatus PosPosifReqGpsSetting(HANDLE h_app);
static EFrameworkunifiedStatus PosPosifSetGpsInfo(HANDLE h_app);
static EFrameworkunifiedStatus PosPosifGetGpsInfo(HANDLE h_app);
static EFrameworkunifiedStatus PosPosifSetData(HANDLE h_app);
static EFrameworkunifiedStatus PosPosifReqGpsReset(HANDLE h_app);
static EFrameworkunifiedStatus PosVehicleInfoRcv(HANDLE h_app);
static uint32_t PosGetMsg(HANDLE h_app, void** p_buf, uint32_t size);
static RET_API  PosSndMsg(PNO pno, CID cid, void* p_msg_body, uint32_t size);
static void PosOutputDebugDumpLog(uint8_t* p_buf);

/* Function scan for device insertion detection */
static EFrameworkunifiedStatus PosOnDevDetectOpenSessionAck(HANDLE h_app);
static EFrameworkunifiedStatus PosOnDevDetectCloseSessionAck(HANDLE h_app);
static EFrameworkunifiedStatus PosOnDevDetectEvent(HANDLE h_app);

/*---------------------------------------------------------------------------------*
 * Grobal Value                                                                    *
 *---------------------------------------------------------------------------------*/
/* Thread name */
static const int8_t kThreadNamePosMain[15]         = "POS_Main";
static const int8_t kThreadNamePosSens[15]         = "POS_Sens";
static const int8_t kThreadNamePosGps[15]          = "POS_Gps";
static const int8_t kThreadNamePosGpsRecv[15]     = "POS_Gps_Recv";
static const int8_t kThreadNamePosGpsRollover[15] = "POS_Gps_Rolovr";

/** Shared memory generation table */
static ST_SHAREDATA g_sharedata_tbl[] = {
    /* Shared data name to be generated,            Shared data size */
    { {VEHICLE_SHARE_NAME},                 512 * 11 }, /* Vehicle sensor information acquisition                     */
#if 0 /* Less than 0.1 SBU,Not used in _CWORD71_ */
    { {"SENSOR_SHARE_MEMORY"},              512 * 11 }, /* Vehicle sensor information Pkg acquisition                  */
    { {"GPS_INT_SIGNAL_SHARE_MEMORY"},      4        }, /* GPS Interrupt Signal Acquisition                      */
    { {"LOG_SETTING_SHARE_MEMORY"},         36       }, /* DR feature log acquisition                         */
    { {"GYRO_CONNECT_STTS_SHARE_MEMORY"},   4        }, /* Get Gyro Connection Status                   */
    { {"EPHEMERIS_NUM_SHARE_MEMORY"},       4        }, /* For acquiring effective ephemeris count at shutdown */
    { {"LOCALTIME_SHARE_MEMORY"},            12         }, /* Local time acquisition at shutdown     */
    { {"LONLAT_SHARE_MEMORY"},                8         }, /* Location acquisition at shutdown               */
#endif
    { {'\0'},                                  0         }  /* Termination */
};

/** Sub-thread creation table
   (1) Thread ID (Locally defined Enumeration)
   (2) Thread name
   (3) Process number
   (4) Start Routine
   (5) Dependent Availability
   (6) Dependency notification
   (7) Dependent threads * If there are dependent threads, do not create them until those threads are created.
   (8) Positioning/Availability->TRUE depending on change
   (9) Thread activation state (THREAD_STS_NOEXIST:Not started,THREAD_STS_CREATING:Starting,THREAD_STS_CREATED:Completion of the activation)
  (10) Boot Sequence(Performance) (0,1,2, ... Note : 0 = Initial value(Not started)) Time of termination,Be destroyed in the reverse order of startup
 */
static ST_THREAD_CREATE_INFO g_pos_thread_create_info_Grade1[] = {  // LCOV_EXCL_BR_LINE 11: unexpected branch
//    {    /* Pos_main */
//        ETID_POS_MAIN, /* (1) */
//        kThreadNamePosMain, /* (2) */
//        PNO_VEHICLE_SENSOR, /* (3) */
//        &VehicleSensThread, /* (4) */
//        (NTFY_MSK_NONE), /* (5) */
//        (NTFY_MSK_NONE), /* (6) */
//        0, /* (7) */
//        TRUE, /* (8) */
//        THREAD_STS_NOEXIST, /* (9) */
//        0 /* (10) */
//    },
//    {    /* Pos_sens */
//        ETID_POS_SENS, /* (1) */
//        kThreadNamePosSens, /* (2) */
//        PNO_LINE_SENS_DRV, /* (3) */
//        &StartLineSensorThreadPositioning, /* (4) */
//        (NTFY_MSK_PS_PSMSHADOW_AVAILABILITY), /* (5) */
//        (NTFY_MSK_NONE), /* (6) */
//        THREAD_STS_MSK_POS_MAIN, /* (7) */
//        FALSE, /* (8) */
//        THREAD_STS_NOEXIST, /* (9) */
//        0 /* (10) */
//    },
//    {    /* Pos_gps */
//        ETID_POS_GPS, /* (1) */
//        kThreadNamePosGps, /* (2) */
//        PNO_NAVI_GPS_MAIN, /* (3) */
//        &StartGpsMainThreadPositioning, /* (4) */
//        (NTFY_MSK_NONE), /* (5) */
//        (NTFY_MSK_NONE), /* (6) */
//        THREAD_STS_MSK_POS_MAIN, /* (7) */
//        TRUE, /* (8) */
//        THREAD_STS_NOEXIST, /* (9) */
//        0 /* (10) */
//    },
//    {    /* Pos_gps_recv */
//        ETID_POS_GPS_RECV, /* (1) */
//        kThreadNamePosGpsRecv, /* (2) */
//        PNO_NAVI_GPS_RCV, /* (3) */
//        &StartGpsRecvThreadPositioning, /* (4) */
//        (NTFY_MSK_NONE), /* (5) */
//        (NTFY_MSK_NONE), /* (6) */
//        THREAD_STS_MSK_POS_GPS, /* (7) */
//        FALSE, /* (8) */
//        THREAD_STS_NOEXIST, /* (9) */
//        0 /* (10) */
//    },
//    {    /* Pos_gps_rollover */
//        ETID_POS_GPS_ROLLOVER, /* (1) */
//        kThreadNamePosGpsRollover, /* (2) */
//        PNO_CLK_GPS, /* (3) */
//        &StartGpsRolloverThreadPositioning, /* (4) */
//        (NTFY_MSK_NS_BACKUPMGR_AVAILABILITY), /* (5) */
//        (NTFY_MSK_NONE), /* (6) */
//        THREAD_STS_MSK_POS_GPS, /* (7) */
//        FALSE, /* (8) */
//        THREAD_STS_NOEXIST, /* (9) */
//        0 /* (10) */
//    },
//    {   /* Termination */
//        ETID_POS_MAX, NULL, 0, NULL, NTFY_MSK_NONE, NTFY_MSK_NONE, 0, FALSE, THREAD_STS_NOEXIST, 0
//    },
};

/* State Management Variables */
static bool g_start_flg = false;       /** Start Processed Flag                                */
static EnumExeSts_POS g_exe_sts;       /** Positioning running status                                 */
static EnumSetupMode_POS g_setup_mode; /** Thread activation mode                                  */
static uint8_t g_last_thread_sts;      /** Latest internal thread activation state                            */
static uint8_t g_last_srv_sts;         /** Latest service availability                            */
static uint8_t g_last_ntfy_sts;           /** Receive state of latest notification                                    */
static uint8_t g_last_num_of_thread;   /** Number of Current Startup Threads                                  */

/** Sub-thread creation table */
static ST_THREAD_CREATE_INFO* g_pos_thread_create_info;

/** Interprocess message receive buffer */
static uint8_t g_rcv_msg_buf[MAX_MSG_BUF_SIZE];

/** Dispatcher Registration Callback Table */
static const FrameworkunifiedProtocolCallbackHandler kPositioningPcbhs[] = {  // LCOV_EXCL_BR_LINE 11: unexpected branch
    {CID_THREAD_CREATE_COMP,                &PosThreadCreateComp                  }, /* Thread start completion notification */
    {CID_THREAD_STOP_COMP,                  &PosThreadStopComp                    }, /* Thread stop completion notice */
    {CID_SENSORIF_PKG_DELIVERY_ENTRY_EXT,   &PosPosifRegisterListenerPkgSensorData},
    {CID_VEHICLEIF_DELIVERY_ENTRY,          &PosPosifRegisterListenerSensorData   },
    {CID_SENSORIF__CWORD82__REQUEST,              &PosPosifReqGpsSetting                },
    {CID_NAVIINFO_DELIVER,                  &PosPosifSetGpsInfo                   },
    {CID_VEHICLEIF_GET_VEHICLE_DATA,        &PosPosifGetGpsInfo                   },
    {CID_POSIF_SET_DATA,                    &PosPosifSetData                      },
    {CID_GPS_REQRESET,                      &PosPosifReqGpsReset                  },
};

static const FrameworkunifiedProtocolCallbackHandler kPositioningPcbhsVehicle[] = {  // LCOV_EXCL_BR_LINE 11: unexpected branch
    {CID_VEHICLESENS_VEHICLE_INFO,              &PosVehicleInfoRcv},
};

/** Dispatcher unregister command ID table */
static uint32_t g_positioning_cids[] = {
    CID_THREAD_CREATE_COMP,
    CID_THREAD_STOP_COMP,
    CID_SENSORIF_PKG_DELIVERY_ENTRY_EXT,
    CID_VEHICLEIF_DELIVERY_ENTRY,
    CID_SENSORIF__CWORD82__REQUEST,
    CID_NAVIINFO_DELIVER,
    CID_VEHICLEIF_GET_VEHICLE_DATA,
    CID_POSIF_SET_DATA,
    CID_GPS_REQRESET,
};

static uint32_t g_positioning_cids_vehicle[] = {
    CID_VEHICLESENS_VEHICLE_INFO,
};



/** Stop request flag for GPS reception thread */
BOOL g_thread_stop_req;

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   FrameworkunifiedOnInitialization<br>
 *   Sends message to Notification Service
 *
 *   Mm 21 API perform initialization.<br>
 *   Generatings shared memories used by Vehicle function block..<br>
 *   Creates a sub-thread of the Vehicle feature block..
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 *            eFrameworkunifiedStatusFail    ABENDs
 */
EFrameworkunifiedStatus FrameworkunifiedOnInitialization(HANDLE h_app) {
    RET_API ret_api;
    EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
    uint8_t* p_last_srv_sts = &g_last_srv_sts;
    uint8_t* p_last_thread_sts = &g_last_thread_sts;
    uint8_t* p_last_ntfy_sts = &g_last_ntfy_sts;
    ST_THREAD_CREATE_INFO** pp_thr_cre_info = &g_pos_thread_create_info;
    uint8_t* p_last_num_thr = &g_last_num_of_thread;
    BOOL* p_thr_stop_req = &g_thread_stop_req;
    EnumExeSts_POS* p_exe_sts = &g_exe_sts;
    EnumSetupMode_POS* pe_mode = &g_setup_mode;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    /* Global variable initialization */
    *p_last_srv_sts = 0;
    *p_last_thread_sts = 0;
    *p_last_ntfy_sts = 0;
    *pp_thr_cre_info = g_pos_thread_create_info_Grade1;
    *p_last_num_thr = 0;
    *p_thr_stop_req = FALSE;
    *p_exe_sts = EPOS_EXE_STS_STOP;
    *pe_mode = EPOS_SETUP_MODE_NORMAL;

    /* null check */
    if (h_app == NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "h_app is NULL");
        e_status = eFrameworkunifiedStatusFail;
    } else {
        /* Positioning Base API initialization */
        ret_api = _pb_Setup_CWORD64_API(h_app);
        if (ret_api != RET_NORMAL) {  // LCOV_EXCL_BR_LINE 4: can not return error
            // LCOV_EXCL_START 8: invalid
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                          "_pb_Setup_CWORD64_API ERROR!! [ret_api = %d]",
                          ret_api);

            e_status = eFrameworkunifiedStatusFail;
            // LCOV_EXCL_STOP
        } else {
            /* Availability at Positioning startup,Set internal thread activation state */
            if (ChkUnitType(UNIT_TYPE_GRADE1) == true) {  /* GRADE1 environment */
                *pp_thr_cre_info = g_pos_thread_create_info_Grade1;
                FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, 
                              "*pp_thr_cre_info = g_pos_thread_create_info_Grade1");
            } else if (ChkUnitType(UNIT_TYPE_GRADE2) == true) {
              /*
               *  Note.
               *  This feature branches processing depending on the unit type.
               */
            } else {
                FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "_pb_ChkUnitType UNKNOWN!!");
            }

            /* Shared Memory Creation */
            PosCreateSharedMemory();

            if (e_status == eFrameworkunifiedStatusOK) {
                /* Register callback functions for Positioning internals */
                e_status = FrameworkunifiedAttachCallbacksToDispatcher(h_app,
                                                        "NS_ANY_SRC",
                                                        kPositioningPcbhs,
                                                        _countof(kPositioningPcbhs));  // LCOV_EXCL_BR_LINE 4:nsfw error
                if (e_status != eFrameworkunifiedStatusOK) {
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                  "FrameworkunifiedAttachCallbacksToDispatcher ERROR!! [e_status = %d]", e_status);
                }

                (void)VehicleIfAttachCallbacksToDispatcher(kPositioningPcbhsVehicle,
                                                            _countof(kPositioningPcbhsVehicle));
            }

            if (e_status == eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                /* Positioning/Availability registration */
                e_status = FrameworkunifiedRegisterServiceAvailabilityNotification(h_app, POS_AVAILABILITY);
                if (eFrameworkunifiedStatusOK != e_status) {
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                  "FrameworkunifiedRegisterServiceAvailabilityNotification ERROR!! [e_status = %d]", e_status);
                }
            }

            if (e_status == eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                /* Positioning/Availability -> FALSE */
                e_status = FrameworkunifiedPublishServiceAvailability(h_app, FALSE);
                if (eFrameworkunifiedStatusOK != e_status) {
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                  "FrameworkunifiedPublishServiceAvailability ERROR!! [e_status = %d]", e_status);
                } else {
                    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "Positioning/Availability -> FALSE");
                }
            }

            /* Communication/Availability Changing notification registration */
            FrameworkunifiedSubscribeNotificationWithCallback(h_app, NTFY_Communication_Availability, &PosNotifyCommunicationAvailability);  // LCOV_EXCL_BR_LINE 6: not a branch  // NOLINT(whitespace/line_length)

            /* PS_CommUSB/Availability Changing notification registration */
            (void)CommUsbIfNotifyOnCommUSBAvailability(&PosNotifyCommUSBAvailability);  // LCOV_EXCL_BR_LINE 6: not a branch  // NOLINT(whitespace/line_length)

            /* PS_PSMShadow/Availability Changing notification registration */
            (void)PSMShadowIfNotifyOnPSMShadowAvailability(&PosNotifyPSMShadowAvailability);  // LCOV_EXCL_BR_LINE 6: not a branch  // NOLINT(whitespace/line_length)
            /* PS_PSMShadow Startup completion notification registration */
            (void)PSMShadowIfNotifyOnPSMShadowInitComp(&PosNotifyPSMShadowInitComp);  // LCOV_EXCL_BR_LINE 6: not a branch  // NOLINT(whitespace/line_length)

            /* Clock/Availability Changing notification registration */
            (void)ClockIfNotifyOnClockAvailability(&PosNotifyClockAvailability);  // LCOV_EXCL_BR_LINE 6: not a branch  // NOLINT(whitespace/line_length)


            /* Regist Vehilce Availability Notification */
            (void)VehicleIfNotifyOnVehicleAvailability(&PosNotifyVehicleAvailability);  // LCOV_EXCL_BR_LINE 6: not a branch  // NOLINT(whitespace/line_length)


            /* NS_BackupMgr/Availability Changing notification registration */
            (void)BackupMgrIfNotifyOnBackupMgrAvailability(&PosNotifyNSBackupMgrAvailability);  // LCOV_EXCL_BR_LINE 6: not a branch  // NOLINT(whitespace/line_length)

            /* DeviceDetectionServiceIf initialization */
            if (DevDetectSrvIfInitialize() == eFrameworkunifiedStatusOK) {
                /* SS_DevDetectSrv/Availability Changing notification registration */
                (void)DevDetectSrvIfNotifyOnDeviceDetectionAvailability(&PosNotifyDevDetectSrvAvailability);  // LCOV_EXCL_BR_LINE 6: not a branch  // NOLINT(whitespace/line_length)
            }

            (void)VehicleIfNotifyOnVehicleAvailability(&PosNotifyVehicleAvailability);  // LCOV_EXCL_BR_LINE 6: not a branch  // NOLINT(whitespace/line_length)
        }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");

    return e_status;
}

/**
 * @brief
 *   PositioningOnStartImpl
 */
static EFrameworkunifiedStatus PositioningOnStartImpl(const HANDLE hApp, const EPWR_SC_WAKEUP_TYPE wakeupType,
                                 const ESMDataResetModeInfo dataResetMode) {
    EnumExeSts_POS* p_exe_sts = &g_exe_sts;
    EnumSetupMode_POS* pe_mode = &g_setup_mode;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    if (g_start_flg == false) {
        g_start_flg = true;

        /* Running */
        *p_exe_sts = EPOS_EXE_STS_RUNNING;

        /* Cold start */
        if (wakeupType == epsstCOLDSTART) {
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "wakeupType:COLDSTART");

            *p_exe_sts = EPOS_EXE_STS_RUNNING_COLDSTART;

            /* Initialize GPS time setting information */
            PosBackupDataInit();
        } else {  /* Time of warm start */
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "wakeupType:WARMSTART");
        }

        /*  Time of factory initialization */
        if (dataResetMode == e_SS_SM_DATA_RESET_MODE_FACTORY) {
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "dataResetMode:FACTORYRESET");

            /* Set thread start mode to ""data reset start"" */
            *pe_mode = EPOS_SETUP_MODE_DATA_RESET;

            /* Initialize GPS time setting information */
            PosBackupDataInit();
        }

        PosCreateThread(hApp);
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");
    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   FrameworkunifiedOnStart
 */
EFrameworkunifiedStatus FrameworkunifiedOnStart(HANDLE hApp) {
    EFrameworkunifiedStatus ret = eFrameworkunifiedStatusFail;
    uint32_t size;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    size = PosGetMsg(hApp, reinterpret_cast<void**>(&g_rcv_msg_buf), MAX_MSG_BUF_SIZE);
    if (size != 0) {
        T_SS_SM_START_DataStructType* p_start_data;
        p_start_data = reinterpret_cast<T_SS_SM_START_DataStructType*>(g_rcv_msg_buf);

        ret = PositioningOnStartImpl(hApp, p_start_data->wakeupType, p_start_data->dataResetMode);
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");
    return ret;
}

/**
 * @brief
 *   FrameworkunifiedOnStop
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 *            eFrameworkunifiedStatusFail    ABENDs
 */
EFrameworkunifiedStatus FrameworkunifiedOnStop(HANDLE h_app) {
    EFrameworkunifiedStatus e_status;
    EnumExeSts_POS* p_exe_sts = &g_exe_sts;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    /* null check */
    if (h_app == NULL) {  // LCOV_EXCL_BR_LINE 200: can not NULL
        // LCOV_EXCL_START 8: invalid
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "h_app is NULL");
        // LCOV_EXCL_STOP
    } else {
        *p_exe_sts = EPOS_EXE_STS_STOP;

        e_status = FrameworkunifiedPublishServiceAvailability(h_app, FALSE);
        if (eFrameworkunifiedStatusOK != e_status) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                          "FrameworkunifiedPublishServiceAvailability ERROR!! [e_status = %d]",
                          e_status);
        } else {
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "Positioning/Availability -> FALSE");
        }

        PosStopThread();

        g_start_flg = false;
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");

    /* Return end response to SM at timing after completion of internal thread stop */
    return eFrameworkunifiedStatusFail;
}

/**
 * @brief
 *   FrameworkunifiedOnPreStart
 */
EFrameworkunifiedStatus FrameworkunifiedOnPreStart(HANDLE hApp) {
    EFrameworkunifiedStatus ret = eFrameworkunifiedStatusFail;
    uint32_t size;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    size = PosGetMsg(hApp, reinterpret_cast<void**>(&g_rcv_msg_buf), MAX_MSG_BUF_SIZE);
    if (size != 0) {
        T_SS_SM_START_DataStructType* p_start_data;
        p_start_data = reinterpret_cast<T_SS_SM_START_DataStructType*>(g_rcv_msg_buf);

        ret = PositioningOnStartImpl(hApp, p_start_data->wakeupType, p_start_data->dataResetMode);
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");
    return ret;
}

/**
 * @brief
 *   FrameworkunifiedOnPreStop
 */
EFrameworkunifiedStatus FrameworkunifiedOnPreStop(HANDLE hApp) {
    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");
    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");
    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   FrameworkunifiedOnBackgroundStart
 */
EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStart(HANDLE hApp) {
    EFrameworkunifiedStatus ret = eFrameworkunifiedStatusFail;
    uint32_t size;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    size = PosGetMsg(hApp, reinterpret_cast<void**>(&g_rcv_msg_buf), MAX_MSG_BUF_SIZE);
    if (size != 0) {
        T_SS_SM_START_DataStructType* p_start_data;
        p_start_data = reinterpret_cast<T_SS_SM_START_DataStructType*>(g_rcv_msg_buf);

        ret = PositioningOnStartImpl(hApp, p_start_data->wakeupType, p_start_data->dataResetMode);
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");
    return ret;
}

/**
 * @brief
 *   FrameworkunifiedOnBackgroundStop
 */
EFrameworkunifiedStatus FrameworkunifiedOnBackgroundStop(HANDLE hApp) {
    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");
    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");
    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   FrameworkunifiedOnDestroy (Not mounted)
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 *            eFrameworkunifiedStatusFail    ABENDs
 */
EFrameworkunifiedStatus FrameworkunifiedOnDestroy(HANDLE h_app) {  // LCOV_EXCL_START 14 Resident process, not called by NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");
    return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   FrameworkunifiedOnDebugDump
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 *            eFrameworkunifiedStatusFail    ABENDs
 */
EFrameworkunifiedStatus FrameworkunifiedOnDebugDump(HANDLE h_app) {  // LCOV_EXCL_START 7: debug code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    static uint8_t              buf_tmp[256];
    static uint8_t              buf_proc[128];
    static uint8_t              buf_thread[512];
    static uint8_t              buf_message[4][DEBUG_DUMP_MAX_SIZE];
    static uint8_t              buf_mutex[3][DEBUG_DUMP_MAX_SIZE];
    static uint8_t              buf_timer[DEBUG_DUMP_MAX_SIZE];
    static uint8_t              buf_event[9][DEBUG_DUMP_MAX_SIZE];
    static uint8_t              buf_memory[DEBUG_DUMP_MAX_SIZE];
    static uint8_t              buf_other[DEBUG_DUMP_MAX_SIZE];
    static uint8_t              buf_nand[256];
    static uint8_t              buf_ram[256];
    static uint8_t              buf_gps_format_fail[512];
    static uint8_t              buf_antenna[256];
    static uint8_t              buf_gps_info[DEBUG_DUMP_MAX_SIZE];
    static uint8_t              buf_navi_info[DEBUG_DUMP_MAX_SIZE];
    static uint8_t              buf_deli_ctrl_tbl[DEBUG_DUMP_MAX_SIZE];
    static uint8_t              buf_deli_ctrl_tbl_mng[DEBUG_DUMP_MAX_SIZE];
    static uint8_t              buf_pkg_deli_tbl_mng[DEBUG_DUMP_MAX_SIZE];
    static uint8_t              buf_deli_pno_tbl[DEBUG_DUMP_MAX_SIZE];
    static uint8_t              buf_sys[128];
    int32_t                      i;
    ST_THREAD_CREATE_INFO*       p_thr_cre_info = g_pos_thread_create_info;
    ST_GPS_FIX_CNT               st_gps_fix_cnt;
    ST_GPS_SET_TIME              st_gps_set_time;
    ST_GPS_SET_TIME_FLAG         st_gps_set_time_flag;
    uint8_t                      len_msg = 4;
    uint8_t                      len_mtx = 3;
    uint8_t                      len_evt = 9;
    EFrameworkunifiedStatus e_status;
    BOOL b_is_available;
    UNIT_TYPE e_type = UNIT_TYPE_NONE;
    u_int8                       sys_recv_flg;
    uint16_t                     wkn_rollover;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    memset(&buf_proc[0],           0x00, sizeof(buf_proc));
    memset(&buf_thread[0],         0x00, sizeof(buf_thread));
    memset(&buf_nand[0],           0x00, sizeof(buf_nand));
    memset(&buf_ram[0],            0x00, sizeof(buf_ram));
    memset(&buf_gps_format_fail[0],  0x00, sizeof(buf_gps_format_fail));
    memset(&buf_antenna[0],        0x00, sizeof(buf_antenna));
    memset(&buf_gps_info[0],        0x00, sizeof(buf_gps_info));
    memset(&buf_navi_info[0],       0x00, sizeof(buf_navi_info));
    memset(&buf_deli_ctrl_tbl[0],    0x00, sizeof(buf_deli_ctrl_tbl));
    memset(&buf_deli_ctrl_tbl_mng[0], 0x00, sizeof(buf_deli_ctrl_tbl_mng));
    memset(&buf_pkg_deli_tbl_mng[0],  0x00, sizeof(buf_pkg_deli_tbl_mng));
    memset(&buf_deli_pno_tbl[0],     0x00, sizeof(buf_deli_pno_tbl));
    memset(&buf_sys[0],            0x00, sizeof(buf_sys));

    for (i = 0; i < len_msg; i++) {
        memset(&buf_message[i][0], 0x00, sizeof(buf_message[i]));
    }
    for (i = 0; i < len_mtx; i++) {
        memset(&buf_mutex[i][0], 0x00, sizeof(buf_mutex[i]));
    }
    memset(&buf_timer[0],   0x00, sizeof(buf_timer));
    for (i = 0; i < len_evt; i++) {
        memset(&buf_event[i][0], 0x00, sizeof(buf_event[i]));
    }
    memset(&buf_memory[0],  0x00, sizeof(buf_memory));
    memset(&buf_other[0],   0x00, sizeof(buf_other));
    e_type = GetEnvSupportInfo();

    /* Availability status of related processes */
    snprintf(reinterpret_cast<char *>(&buf_proc[0]), sizeof(buf_proc),
            "Availability\n thread:0x%02x, srv:0x%02x, ntfy:0x%02x",
            g_last_thread_sts,  /* Latest internal thread activation state */
            g_last_srv_sts,     /* Latest service availability */
            g_last_ntfy_sts);   /* Receive state of latest notification         */

    /* Internal thread activation state */
    snprintf(reinterpret_cast<char *>(&buf_thread[0]), sizeof(buf_thread), "Thread");
    for (i = 0; i < ETID_POS_MAX; i++) {
        memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
        snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
                "\n [%d]id:%d, pno:0x%04x, name:%16s, sts:0x%02x, order:%d",
                i,
                p_thr_cre_info->id,       /* Thread ID */
                p_thr_cre_info->pno,      /* Process number */
                p_thr_cre_info->p_name,    /* Thread name */
                p_thr_cre_info->status,   /* Thread activation state */
                p_thr_cre_info->order);   /* Boot Sequence */
        _pb_strcat(reinterpret_cast<char *>(&buf_thread[0]), reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp));
        p_thr_cre_info++;
    }

    /* BASE API control data */
    /*   Message */
    (void)_pb_GetDebugMsgMngTbl(&buf_message[0][0], &len_msg);
    /*   Mutex */
    (void)_pb_GetDebugMutexMngTbl(&buf_mutex[0][0], &len_mtx);
    /*   Timer */
    (void)_pb_GetDebugTimerMngTbl(&buf_timer[0]);
    /*   Event */
    (void)_pb_GetDebugEventMngTbl(&buf_event[0][0], &len_evt);
    /*   Shared Memory */
    (void)_pb_GetDebugMemoryMngTbl(&buf_memory[0]);
    /*   Other */
    (void)_pb_GetDebugOtherMngTbl(&buf_other[0]);

    /* NAND data */
    snprintf(reinterpret_cast<char *>(&buf_nand[0]), sizeof(buf_nand), "NAND");
    /*   GPS fix count */
    memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
    memset(&st_gps_fix_cnt, 0x00, sizeof(st_gps_fix_cnt));

    e_status = BackupMgrIfBackupDataRd(D_BK_ID_POS_GPS_FIX_CNT,
                                        0,
                                        &st_gps_fix_cnt,
                                        sizeof(st_gps_fix_cnt), &b_is_available);
    snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
            "\n %20s rd:0x%08x av:%d, 3d:%d, 2d:%d, else:%d, dmy:0x%02x%02x%02x%02x",
            "GPS_FIX_CNT",
            e_status,
            b_is_available,
            st_gps_fix_cnt.ul3d,
            st_gps_fix_cnt.ul2d,
            st_gps_fix_cnt.ul_else,
            st_gps_fix_cnt.dummy[0], st_gps_fix_cnt.dummy[1], st_gps_fix_cnt.dummy[2], st_gps_fix_cnt.dummy[3]);
    _pb_strcat(reinterpret_cast<char *>(&buf_nand[0]), reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp));

    /* Data of the backup RAM */
    snprintf(reinterpret_cast<char *>(&buf_ram[0]), sizeof(buf_ram), "BackupRAM");
    /*   Set GPS date and time(Information) */
    memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
    (void)memset( reinterpret_cast<void *>(&st_gps_set_time), 0, (size_t)sizeof(st_gps_set_time) );

    e_status = BackupMgrIfBackupDataRd(D_BK_ID_POS_GPS_TIME_SET_INFO,
                                        0,
                                        &st_gps_set_time,
                                        sizeof(st_gps_set_time),
                                        &b_is_available);
    snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
            "\n %20s rd:0x%08x av:%d, %d/%d/%d %d:%d:%d  flag:0x%02x",
            "GPS_TIME_SET_INFO",
            e_status,
            b_is_available,
            st_gps_set_time.year,
            st_gps_set_time.month,
            st_gps_set_time.date,
            st_gps_set_time.hour,
            st_gps_set_time.minute,
            st_gps_set_time.second,
            st_gps_set_time.flag);
    _pb_strcat(reinterpret_cast<char *>(&buf_ram[0]), reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp));

    /*   Set GPS date and time(Flagging) */
    memset(&buf_tmp[0], 0x00, sizeof(buf_tmp));
    (void)memset( reinterpret_cast<void *>(&st_gps_set_time_flag), 0, (size_t)sizeof(st_gps_set_time_flag) );

    e_status = BackupMgrIfBackupDataRd(D_BK_ID_POS_GPS_TIME_SET_FLAG,
                                        0,
                                        &st_gps_set_time_flag,
                                        sizeof(st_gps_set_time_flag),
                                        &b_is_available);
    snprintf(reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp),
            "\n %20s rd:0x%08x av:%d, flag:0x%02x",
            "GPS_TIME_SET_FLAG",
            e_status,
            b_is_available,
            st_gps_set_time_flag.flag);
    _pb_strcat(reinterpret_cast<char *>(&buf_ram[0]), reinterpret_cast<char *>(&buf_tmp[0]), sizeof(buf_tmp));

    if (e_type == UNIT_TYPE_GRADE1) {
        /* GPS format anomaly counter  */ /* There is no lock control. */
        (void)DEVGpsGetDebugGpsFormatFailCnt(&buf_gps_format_fail[0]);

        /* GPS antenna connection status */ /* There is no lock control. */
        VEHICLESENS_DATA_MASTER st_sns_data = {0};
        (void)VehicleSensGetGpsAntenna(&st_sns_data, VEHICLESENS_GETMETHOD_LINE);
        snprintf(reinterpret_cast<char *>(&buf_antenna[0]), sizeof(buf_antenna),
            "Antenna\n sts:0x%02x",
            st_sns_data.uc_data[0]);

        /* GPS position time */ /* There is no lock control. */
        (void)VehicleSensGetDebugPosDate(&buf_gps_info[0], VEHICLESENS_GETMETHOD_GPS);
    }

    if (e_type == UNIT_TYPE_GRADE1) {
        /* Navigation position time */ /* There is no lock control. */
        (void)VehicleSensGetDebugPosDate(&buf_navi_info[0], VEHICLESENS_GETMETHOD_NAVI);
    }

    /* Delivery table */ /* There is no lock control. */
    (void)VehicleSensGetDebugDeliveryCtrlTbl(&buf_deli_ctrl_tbl[0]);
    (void)VehicleSensGetDebugDeliveryCtrlTblMng(&buf_deli_ctrl_tbl_mng[0]);
    (void)VehicleSensGetDebugPkgDeliveryTblMng(&buf_pkg_deli_tbl_mng[0]);
    (void)VehicleSensGetDebugDeliveryPnoTbl(&buf_deli_pno_tbl[0]);

    /* Initial Sensor Data Status from Sys */
    sys_recv_flg = LineSensDrvGetSysRecvFlag();
    snprintf(reinterpret_cast<char *>(&buf_sys[0]), sizeof(buf_sys),
            "Rx 1st Sensor Data %d\n", sys_recv_flg);

    /* Dump Information Out */
    PosOutputDebugDumpLog(&buf_proc[0]);
    PosOutputDebugDumpLog(&buf_thread[0]);
    for (i = 0; i < len_msg; i++) {
        PosOutputDebugDumpLog(&buf_message[i][0]);
    }
    for (i = 0; i < len_mtx; i++) {
        PosOutputDebugDumpLog(&buf_mutex[i][0]);
    }
    PosOutputDebugDumpLog(&buf_timer[0]);
    for (i = 0; i < len_evt; i++) {
        PosOutputDebugDumpLog(&buf_event[i][0]);
    }
    PosOutputDebugDumpLog(&buf_memory[0]);
    PosOutputDebugDumpLog(&buf_other[0]);
    PosOutputDebugDumpLog(&buf_nand[0]);
    PosOutputDebugDumpLog(&buf_ram[0]);
    if (e_type == UNIT_TYPE_GRADE1) {
        PosOutputDebugDumpLog(&buf_gps_format_fail[0]);
        PosOutputDebugDumpLog(&buf_antenna[0]);
        PosOutputDebugDumpLog(&buf_gps_info[0]);
    }
    if (e_type == UNIT_TYPE_GRADE1) {
        PosOutputDebugDumpLog(&buf_navi_info[0]);
    }
    PosOutputDebugDumpLog(&buf_deli_ctrl_tbl[0]);
    PosOutputDebugDumpLog(&buf_deli_ctrl_tbl_mng[0]);
    PosOutputDebugDumpLog(&buf_pkg_deli_tbl_mng[0]);
    PosOutputDebugDumpLog(&buf_deli_pno_tbl[0]);
    PosOutputDebugDumpLog(&buf_sys[0]);

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");
    return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   FrameworkunifiedCreateStateMachine (Not mounted)
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 *            eFrameworkunifiedStatusFail    ABENDs
 */
EFrameworkunifiedStatus FrameworkunifiedCreateStateMachine(HANDLE h_app) {  // LCOV_EXCL_START 8 : dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");
    return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief
 *   Common processing after thread startup
 *
 *   Thread naming,Create Message Queue,Thread activation response
 *
 * @param[in]  h_app Application handle
 * @param[in]  e_tid    Thread ID
 *
 * @return  Thread activation mode
 */
EnumSetupMode_POS PosSetupThread(HANDLE h_app, EnumTID_POS e_tid) {
    RET_API ret;
    ST_THREAD_CREATE_INFO* p_thr_cre_info = g_pos_thread_create_info;
    ST_THREAD_CREATE_INFO* p_info;
    ST_THREAD_SETUP_INFO st_setup_info;
    ST_THREAD_SETUP_INFO* pst_setup_info = &st_setup_info;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    p_info = p_thr_cre_info + e_tid;

    /* Application handle setting */
    _pb_SetAppHandle(h_app);

    /* Create Message Queue */
    _pb_CreateMsg(p_info->pno);

    /* Get Thread Startup Information */
    pst_setup_info->e_mode = EPOS_SETUP_MODE_NORMAL;
    (void)PosGetMsg(h_app, reinterpret_cast<void**>(&pst_setup_info), sizeof(ST_THREAD_SETUP_INFO));
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "[e_mode = %d]", pst_setup_info->e_mode);

    /* Issue thread creation completion notice */
    ret = _pb_SndMsg_Ext(POS_THREAD_NAME, CID_THREAD_CREATE_COMP, sizeof(EnumTID_POS), (const void*)&e_tid, 0);
    if (ret != RET_NORMAL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SndMsg_Ext ERROR!! [ret = %d]", ret);
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");

    return pst_setup_info->e_mode;
}

/**
 * @brief
 *   Common Processing at Thread Stop
 *
 *   Thread stop response,Thread destruction
 *
 * @param[in]  e_tid    Thread ID
 */
void PosTeardownThread(EnumTID_POS e_tid) {
    RET_API ret;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    /* Issue thread stop completion notice */
    ret = _pb_SndMsg_Ext(POS_THREAD_NAME, CID_THREAD_STOP_COMP, sizeof(EnumTID_POS), (const void*)&e_tid, 0);
    if (ret != RET_NORMAL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SndMsg_Ext ERROR!! [ret = %d]", ret);
    }

    /* Thread destruction */
    _pb_ExitThread((DWORD)0);

    /* don't arrive here */
    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");

    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return;  // LCOV_EXCL_LINE 8 : cannot reach here
}

#ifdef __cplusplus
}
#endif

/**
 * @brief
 *   FrameworkunifiedCreateChildThread dummy functions
 *
 * @param[in]  Application handle
 *
 * @return  eFrameworkunifiedStatusOK successful completion
 */
static EFrameworkunifiedStatus PosStopThreadDummy(HANDLE h_app) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");
    return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

/*---------------------------------------------------------------------------------*
 * Local Function                                                                  *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   Communication services Availability notification callback functions
 *
 * @param[in]  h_app    Application handle
 *
 * @return eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosNotifyCommunicationAvailability(HANDLE h_app) {
    BOOL isAvailable;
    uint8_t* pLastSrvSts = &g_last_srv_sts;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    isAvailable = FrameworkunifiedIsServiceAvailable(h_app);
    if (isAvailable == TRUE) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Communication/Availability -> TRUE");

        *pLastSrvSts |= NTFY_MSK_COMMUNICATION_AVAILABILITY;

        PosCreateThread(h_app);

        /* Sensor Log Initial Processing(First)*/
        SensLogInitialize(NULL);
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Communication/Availability -> FALSE");

        *pLastSrvSts = static_cast<uint8_t>(*pLastSrvSts & ~NTFY_MSK_COMMUNICATION_AVAILABILITY);
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   CommUSB services Availability notification callback functions
 *
 * @param[in]  h_app    Application handle
 *
 * @return eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosNotifyCommUSBAvailability(HANDLE h_app) {
    BOOL isAvailable;
    uint8_t* pLastSrvSts = &g_last_srv_sts;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    isAvailable = FrameworkunifiedIsServiceAvailable(h_app);
    if (isAvailable == TRUE) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PS_CommUSB/Availability -> TRUE");

        *pLastSrvSts |= NTFY_MSK_PS_COMMUSB_AVAILABILITY;

        PosCreateThread(h_app);
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PS_CommUSB/Availability -> FALSE");

        *pLastSrvSts = static_cast<uint8_t>(*pLastSrvSts & ~NTFY_MSK_PS_COMMUSB_AVAILABILITY);
    }

    /* Update CommUSB I/F availability */
    CommUsbIfSetAvailability(isAvailable);

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   PSMShadow services Availability notification callback functions
 *
 * @param[in]  h_app    Application handle
 *
 * @return eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosNotifyPSMShadowAvailability(HANDLE h_app) {
    BOOL isAvailable;
    uint8_t* pLastSrvSts = &g_last_srv_sts;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    isAvailable = FrameworkunifiedIsServiceAvailable(h_app);
    if (isAvailable == TRUE) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PS_PSMShadow/Availability -> TRUE");

        *pLastSrvSts |= NTFY_MSK_PS_PSMSHADOW_AVAILABILITY;

        PosCreateThread(h_app);
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PS_PSMShadow/Availability -> FALSE");

        *pLastSrvSts = static_cast<uint8_t>(*pLastSrvSts & ~NTFY_MSK_PS_PSMSHADOW_AVAILABILITY);
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   PSMShadow services Callback function for notifying completion of startup
 *
 * @param[in]  h_app    Application handle
 *
 * @return eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosNotifyPSMShadowInitComp(HANDLE h_app) {
    uint8_t* pLastNtfySts = &g_last_ntfy_sts;
    ST_THREAD_CREATE_INFO* pThrCreInfo = g_pos_thread_create_info;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    *pLastNtfySts |= NTFY_MSK_PS_PSMSHADOW_INIT_COMP;

    /* When the Pos_Sens thread is started */
    if (((pThrCreInfo + ETID_POS_SENS)->status) == THREAD_STS_CREATED) {
        /* External pin status request */
        LineSensDrvExtTermStsReq();
    }

    PosCreateThread(h_app);

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   Clock Services Availability Notification Callback Functions
 *
 * @param[in]  h_app Application handle
 *
 * @return eFrameworkunifiedStatusOK successful completion
 */
static EFrameworkunifiedStatus PosNotifyClockAvailability(HANDLE h_app) {
    BOOL isAvailable;
    uint8_t* pLastSrvSts = &g_last_srv_sts;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    isAvailable = FrameworkunifiedIsServiceAvailable(h_app);
    if (isAvailable == TRUE) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Clock/Availability -> TRUE");

        *pLastSrvSts |= NTFY_MSK_CLOCK_AVAILABILITY;

        PosCreateThread(h_app);
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Clock/Availability -> FALSE");

        *pLastSrvSts = static_cast<uint8_t>(*pLastSrvSts & ~NTFY_MSK_CLOCK_AVAILABILITY);
    }

    /* Update Clock I/F availability */
    ClockIfSetAvailability(isAvailable);

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   NS_BackupMgr service Availability notification callback function
 *
 * @param[in]  h_app Application handle
 *
 * @return eFrameworkunifiedStatusOK successful completion
 */
static EFrameworkunifiedStatus PosNotifyNSBackupMgrAvailability(HANDLE h_app) {
    EnumExeSts_POS* pExeSts = &g_exe_sts;
    uint8_t* pLastSrvSts = &g_last_srv_sts;
    ST_THREAD_CREATE_INFO* pThrCreInfo = g_pos_thread_create_info;
    EnumSetupMode_POS* peMode = &g_setup_mode;
    BOOL bIsAvailable;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    bIsAvailable = FrameworkunifiedIsServiceAvailable(h_app);

    /* Update BackupMgr I/F  availability */
    BackupMgrIfSetAvailability(bIsAvailable);

    if (bIsAvailable == TRUE) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "NS_BackupMgr/Availability -> TRUE");

        *pLastSrvSts |= NTFY_MSK_NS_BACKUPMGR_AVAILABILITY;

        /* Executing after cold start or during factory initialization*/
        if ((*pExeSts == EPOS_EXE_STS_RUNNING_COLDSTART) ||
            (*peMode == EPOS_SETUP_MODE_DATA_RESET)) {
            /* Backup RAM initialization */
            PosBackupDataInit();
        }

        /* When the GPS management thread is started */
        if (((pThrCreInfo + ETID_POS_GPS)->status) == THREAD_STS_CREATED) {
            /* Backup data read request to GSP management thread */
            (void)DEVGpsSndBackupDataLoadReq();
        }
        /* Enable Diag Code Writing */
        DiagSrvIfSetRegistrationPermission(TRUE);

        PosCreateThread(h_app);
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "NS_BackupMgr/Availability -> FALSE");

        *pLastSrvSts = static_cast<uint8_t>(*pLastSrvSts & ~NTFY_MSK_NS_BACKUPMGR_AVAILABILITY);
        /* Write prohibited dialog code */
        DiagSrvIfSetRegistrationPermission(FALSE);
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   SS_DevDetectSrv service Availability Callback Functions
 *
 * @param[in]  h_app Application handle
 *
 * @return eFrameworkunifiedStatusOK successful completion
 */
static EFrameworkunifiedStatus PosNotifyDevDetectSrvAvailability(HANDLE h_app) {
    EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
    BOOL isAvailable;
    BOOL bDummy;
    uint8_t* pLastSrvSts = &g_last_srv_sts;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    isAvailable = FrameworkunifiedIsServiceAvailable(h_app);

    /* Update DevDetectSrv I/F availability */
    DevDetectSrvIfSetAvailability(isAvailable);

    if (isAvailable == TRUE) {
        FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "SS_DevDetectSrv/Availability -> TRUE");
        *pLastSrvSts |= NTFY_MSK_SS_DEVDETSRV_AVAILABILITY;

        eStatus = DevDetectSrvIfOpenSessionRequest(&bDummy);
        if (eFrameworkunifiedStatusOK != eStatus) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                "DeviceDetectionServiceIf OpenSession ERROR!! [eStatus = %d]", eStatus);
        } else {
            eStatus = DevDetectSrvIfNotifyOnOpenSessionAck(&PosOnDevDetectOpenSessionAck, &bDummy);
            if (eFrameworkunifiedStatusOK != eStatus) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                    "DeviceDetectionServiceIf NotifyOnOpenSessionAck ERROR!! [eStatus = %d]", eStatus);
            }
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "SS_DevDetectSrv/Availability -> FALSE");
        *pLastSrvSts = static_cast<uint8_t>(*pLastSrvSts & ~NTFY_MSK_SS_DEVDETSRV_AVAILABILITY);
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}


/**
 * @brief
 *   Vehicle Availability notification callback functions
 *
 * @param[in]  h_app    Application handle
 *
 * @return eFrameworkunifiedStatusOK    Normal completion
 *
 */
static EFrameworkunifiedStatus PosNotifyVehicleAvailability(HANDLE h_app) {
  BOOL isAvailable;
  uint8_t* pLastSrvSts = &g_last_srv_sts;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

  isAvailable = FrameworkunifiedIsServiceAvailable(h_app);

  /* Update Vechile I/F Abailability */
  VehicleIf_SetAvailability(isAvailable);

  if (isAvailable == TRUE) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Vehicle/Availability -> TRUE");

    *pLastSrvSts |= NTFY_MSK_VS_VEHICLE_AVAILABILITY;

//    if (eFrameworkunifiedStatusFail == VehicleIfDeliveryEntry(VEHICLE_DID_SPEED)) {
//      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "VehicleIfDeliveryEntry SPEED ERROR");
//    }
//
//    if (eFrameworkunifiedStatusFail == VehicleIfDeliveryEntry(VEHICLE_DID_REV)) {
//      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "VehicleIfDeliveryEntry REV ERROR");
//    }

    if (eFrameworkunifiedStatusFail == VehicleIfDeliveryEntry(VEHICLE_DID_SPEED_PULSE_VEHICLE)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "VehicleIfDeliveryEntry SPEED PULSE ERROR");
    }

    PosCreateThread(h_app);

  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Vehicle/Availability -> FALSE");

    *pLastSrvSts &= ~NTFY_MSK_VS_VEHICLE_AVAILABILITY;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");

  return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   Shared Memory Creation for Positioning Function
 *
 * @return    RET_NORMAL    Normal completion
 *            RET_ERROR    ABENDs
 */
static void PosCreateSharedMemory(void) {
    RET_API    ret_api = RET_NORMAL;
    void        *mod_exec_dmy; /* Module data pointer(dummy) */
    int           retry; /* Retry counter */
    ST_SHAREDATA *p_shm_tbl;

    /* Configure Shared Data Generating Tables */
    p_shm_tbl = g_sharedata_tbl;

    while ( *(p_shm_tbl->share_data_name) != '\0' ) {
        for ( retry = 0; retry < DATMOD_RETRY; retry++ ) {
            /* Shared Memory Generation */
            ret_api = _pb_CreateShareData(p_shm_tbl->share_data_name, p_shm_tbl->data_size, &mod_exec_dmy);
            if (ret_api == RET_NORMAL) {  /* If successful */
                /* Set the shared memory status flag to ""Before initialization (0)"" */
                *reinterpret_cast<u_int32 *>(mod_exec_dmy) = DATMOD_PREINIT;

                break;
            } else {  /* In the event of failure */
                /* Error Handling */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                              "_pb_CreateShareData ERROR [ret_api:%d]",
                              ret_api);
            }
        }

        if (retry >= DATMOD_RETRY) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_CreateShareData failed more %d times.",
                                               DATMOD_RETRY);

            _pb_Exit();
            /* don't arrive here. */
        }

        /* Next shared memory generation */
        p_shm_tbl++;
    }

    return;
}

/**
 * @brief
 *   Positioning in-process thread creation
 *
 * @param[in]  hApp application handles
 */
static void PosCreateThread(HANDLE h_app) {
    HANDLE    threadHandle;
    EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
    EnumSetupMode_POS* peMode = &g_setup_mode;
    int32_t i;
    uint8_t* pLastSrvSts = &g_last_srv_sts;
    uint8_t* pLastThreadSts = &g_last_thread_sts;
    uint8_t* pLastNtfySts = &g_last_ntfy_sts;
    EnumExeSts_POS* pExeSts = &g_exe_sts;
    ST_THREAD_CREATE_INFO* pThrCreInfo = g_pos_thread_create_info;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    for (i = 0; i < ETID_POS_MAX; i++) {
        if ((pThrCreInfo->status == THREAD_STS_NOEXIST) && (pThrCreInfo->routine != NULL)) {
            FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, 
                          "i=%d, mskThread=0x%02x, *pLastThreadSts=0x%02x,"\
                          "mskAvailable=0x%02x, *pLastSrvSts=0x%02x, mskNtfy=0x%02x, *pLastNtfySts=0x%02x",
                          i,
                          pThrCreInfo->msk_thread,
                          *pLastThreadSts,
                          pThrCreInfo->msk_available,
                          *pLastSrvSts,
                          pThrCreInfo->msk_ntfy,
                          *pLastNtfySts);

            if ((*pExeSts != EPOS_EXE_STS_STOP)
                && ((((pThrCreInfo->msk_thread) & (*pLastThreadSts)) == pThrCreInfo->msk_thread)
                    || (pThrCreInfo->msk_thread == 0))
                && ((((pThrCreInfo->msk_available) & (*pLastSrvSts)) == pThrCreInfo->msk_available)
                    || (pThrCreInfo->msk_available == NTFY_MSK_NONE))
                && ((((pThrCreInfo->msk_ntfy) & (*pLastNtfySts)) == pThrCreInfo->msk_ntfy)
                    || (pThrCreInfo->msk_ntfy == NTFY_MSK_NONE))) {
                if (pThrCreInfo->pno == PNO_LINE_SENS_DRV || \
                    pThrCreInfo->pno == PNO_NAVI_GPS_MAIN || \
                    pThrCreInfo->pno == PNO_NAVI_GPS_RCV) {
                        (pThrCreInfo->routine)(h_app);
                } else {
                    /* Thread creation */
                    threadHandle = FrameworkunifiedCreateChildThread(h_app,
                                                       (PCSTR)(pThrCreInfo->p_name),
                                                       pThrCreInfo->routine,
                                                       &PosStopThreadDummy);
                    if (threadHandle == NULL) {  /* If the thread creation fails */
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                      "FrameworkunifiedCreateChildThread ERROR!! [tHandle=%p]",
                                      threadHandle);
                        _pb_Exit();
                        /* don't arrive here. */
                    } else {
                        /* Thread activation (Notify the startup mode) */
                        eStatus = FrameworkunifiedStartChildThread(h_app,
                                                      threadHandle,
                                                      sizeof(EnumSetupMode_POS),
                                                      reinterpret_cast<void*>(peMode));
                        if (eStatus != eFrameworkunifiedStatusOK) {
                            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                          "FrameworkunifiedStartChildThread ERROR!! [eStatus=%d, name=%s]",
                                          eStatus,
                                          pThrCreInfo->p_name);
                        } else {
                            pThrCreInfo->status = THREAD_STS_CREATING;
                            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "name=%s", pThrCreInfo->p_name);
                        }
                    }
                }
            }
        }

        pThrCreInfo++;
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");

    return;
}


/**
 * @brief
 *   Termination of Positioning in-process threads
 */
static void PosStopThread(void) {
    RET_API ret;
    ST_THREAD_CREATE_INFO* p_thr_cre_info = g_pos_thread_create_info;
    BOOL *p_thr_stop_req = &g_thread_stop_req;
    uint8_t uc_msg = 0;
    uint8_t uc_order = 0;
    PNO us_pno = 0;
    int32_t i;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    for (i = 0; i < ETID_POS_MAX; i++) {
        if (uc_order < p_thr_cre_info->order) {
            uc_order = p_thr_cre_info->order;
            us_pno = p_thr_cre_info->pno;
        }
        p_thr_cre_info++;
    }

    if (uc_order != 0) {
        /* Send Thread Termination Request */
        if (us_pno == PNO_NAVI_GPS_RCV) {
            /* Pos_gps_recv Only thread flag control */
            *p_thr_stop_req = TRUE;
        } else {
            ret = PosSndMsg(us_pno, CID_THREAD_STOP_REQ, &uc_msg, sizeof(uc_msg));
            if (ret != RET_NORMAL) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                              "PosSndMsg ERROR!! [ret = %d]",
                              ret);
            }
        }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");

    return;
}

/**
 * @brief
 *   Backup RAM initialization
 */
static void PosBackupDataInit(void) {
    UNIT_TYPE e_type = UNIT_TYPE_NONE;
    EFrameworkunifiedStatus e_status;
    BOOL b_is_available;
    ST_GPS_SET_TIME st_gps_set_time;
    ST_GPS_SET_TIME_FLAG st_gps_set_time_flag;

    (void)memset( reinterpret_cast<void *>(&st_gps_set_time_flag), 0, (size_t)sizeof(st_gps_set_time_flag) );
    (void)memset( reinterpret_cast<void *>(&st_gps_set_time), 0, (size_t)sizeof(st_gps_set_time) );

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    e_type = GetEnvSupportInfo();
    if (e_type == UNIT_TYPE_GRADE1) {
        /* Set GPS date and time */
        e_status = BackupMgrIfBackupDataWt(D_BK_ID_POS_GPS_TIME_SET_INFO,
                                            &st_gps_set_time,
                                            0,
                                            sizeof(st_gps_set_time),
                                            &b_is_available);
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "D_BK_ID_POS_GPS_TIME_SET_INFO:W:Clear");
        if (e_status != eFrameworkunifiedStatusOK) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                          "BackupMgrIfBackupDataWt ERROR!! [e_status=%d, b_is_available=%d]",
                          e_status,
                          b_is_available);
        }
    } else if (e_type == UNIT_TYPE_GRADE2) {
      /*
       *  Note.
       *  This feature branches processing depending on the unit type.
       */
    } else {
        /* No processing */
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");

    return;
}

/**
 * @brief
 *   Callback function for registering the dispatcher()
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosThreadCreateComp(HANDLE h_app) {
    EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
    uint8_t* pRcvMsg;
    uint32_t size;
    uint8_t* pLastThreadSts = &g_last_thread_sts;
    ST_THREAD_CREATE_INFO* pThrCreInfo = g_pos_thread_create_info;
    uint8_t* pLastNumThr = &g_last_num_of_thread;
    uint8_t* pLastSrvSts = &g_last_srv_sts;
    uint8_t* pLastNtfySts = &g_last_ntfy_sts;
    EnumTID_POS eTid;
    int32_t i;
    static BOOL isSetAvailable = FALSE;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    pRcvMsg = g_rcv_msg_buf;

    size = PosGetMsg(h_app, reinterpret_cast<void**>(&pRcvMsg), MAX_MSG_BUF_SIZE);
    if (size != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        (*pLastNumThr)++;

        eTid = *(reinterpret_cast<EnumTID_POS*>(pRcvMsg));

        FRAMEWORKUNIFIEDLOG(ZONE_17, __FUNCTION__, 
                      "Get message = [Sender:%s][CID:0x%X]",
                      (pThrCreInfo + eTid)->p_name,
                      CID_THREAD_CREATE_COMP);

        /* Thread Management Variable Updates */
        *pLastThreadSts = static_cast<uint8_t>(*pLastThreadSts | (0x1u << eTid));
        (pThrCreInfo + eTid)->status = THREAD_STS_CREATED;
        (pThrCreInfo + eTid)->order  = *pLastNumThr;

        /* Individual processing of started threads */
        switch (eTid) {
            case ETID_POS_SENS: /* When sensor driver thread startup is completed */
            {
                /* When PSMShadow startup completion notice has been received */
                if (((NTFY_MSK_PS_PSMSHADOW_INIT_COMP) & (*pLastNtfySts)) == NTFY_MSK_PS_PSMSHADOW_INIT_COMP) {
                    /* External pin status request */
                    LineSensDrvExtTermStsReq();
                }
                break;
            }
            case ETID_POS_GPS: /* When the GPS management thread has started */
            {
                /* NSBackupMgr/Availability=When TRUE notification has been received */
                if (((NTFY_MSK_NS_BACKUPMGR_AVAILABILITY) & (*pLastSrvSts)) == NTFY_MSK_NS_BACKUPMGR_AVAILABILITY) {
                    /* Backup data read request to GSP management thread */
                    (void)DEVGpsSndBackupDataLoadReq();
                }
                break;
            }
            default: /* Other thread startup completion time */
                break;
        }

        PosCreateThread(h_app);

        for (i = 0; i < ETID_POS_MAX; i++) {
            if ((pThrCreInfo->is_depended == TRUE) && (pThrCreInfo->status != THREAD_STS_CREATED)) {
                break; /* Positioning/Availability->TRUE condition does not meet */
            }
            pThrCreInfo++;
        }

        if ((i == ETID_POS_MAX) && (isSetAvailable == FALSE)) {
            /* Positionig/Availability -> TRUE */
            eStatus = FrameworkunifiedPublishServiceAvailability(h_app, TRUE);
            if (eFrameworkunifiedStatusOK != eStatus) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                              "FrameworkunifiedPublishServiceAvailability ERROR!! [eStatus = %d]",
                              eStatus);
            } else {
                FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "Positioning/Availability -> TRUE");
                isSetAvailable = TRUE;
            }
        }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}


/**
 * @brief
 *   Callback function for registering the dispatcher()
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosThreadStopComp(HANDLE h_app) {
    EFrameworkunifiedStatus eStatus;
    BOOL bIsAvailable;

    uint8_t* pRcvMsg;
    uint32_t size;
    uint8_t* pLastThreadSts = &g_last_thread_sts;
    ST_THREAD_CREATE_INFO* pThrCreInfo = g_pos_thread_create_info;
    uint8_t* pLastNumThr = &g_last_num_of_thread;
    BOOL *pThrStopReq = &g_thread_stop_req;
    EnumTID_POS eTid;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    pRcvMsg = g_rcv_msg_buf;

    size = PosGetMsg(h_app, reinterpret_cast<void**>(&pRcvMsg), MAX_MSG_BUF_SIZE);
    if (size != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        (*pLastNumThr)--;

        eTid = *(reinterpret_cast<EnumTID_POS*>(pRcvMsg));

        FRAMEWORKUNIFIEDLOG(ZONE_17, __FUNCTION__, 
                      "Get message = [Sender:%s][CID:0x%X]",
                      (pThrCreInfo + eTid)->p_name,
                      CID_THREAD_STOP_COMP);

        *pLastThreadSts = static_cast<uint8_t>(*pLastThreadSts & ~(0x1u << eTid));

        (pThrCreInfo + eTid)->status = THREAD_STS_NOEXIST;
        (pThrCreInfo + eTid)->order  = 0;

        if ((pThrCreInfo + eTid)->pno == PNO_NAVI_GPS_RCV) {
            *pThrStopReq = FALSE;
        }
    }

    PosStopThread();

    /* When all threads have stopped */
    if (*pLastThreadSts == 0) {
        /* Unregister callback function */
        eStatus = FrameworkunifiedDetachCallbacksFromDispatcher(h_app,
                                                   "NS_ANY_SRC",
                                                   (const PUI_32)g_positioning_cids,
                                                   _countof(g_positioning_cids), NULL);
        if (eStatus != eFrameworkunifiedStatusOK) {  /* In the event of failure */
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                          "PositioningDetachCallbacksToDispatcher Failed in FrameworkunifiedOnStop [eStatus:%d]",
                          eStatus);
        }

        /* Sensor log stop processing */
        SensLogTerminate();

        /* DeviceDetectionServiceIf termination process */
        eStatus = DevDetectSrvIfUnRegisterForDeviceDetectionEvent(SS_DEV_DETECT_ANY_USB_EV, &bIsAvailable);
        if (eStatus != eFrameworkunifiedStatusOK) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                          "DeviceDetectionServiceIf UnRegisterEvent ERROR!! [sts:%d, ava:%d]",
                          eStatus,
                          bIsAvailable);
        } else {
            eStatus = DevDetectSrvIfCloseSessionRequest(&bIsAvailable);
            if (eFrameworkunifiedStatusOK != eStatus) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                              "DeviceDetectionServiceIf CloseSession ERROR!! [sts=%d, ava:%d]",
                              eStatus,
                              bIsAvailable);
            }
        }

        (void)VehicleIfDetachCallbacksFromDispatcher((const PUI_32)g_positioning_cids_vehicle,
                                                            _countof(g_positioning_cids_vehicle));

        /* Releasing Base API Resources */
        _pb_Teardown_CWORD64_API();

        /* Stop processing completion response */
        SendInterfaceunifiedOnStopResponseToSystemManager(eFrameworkunifiedStatusOK);
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   Callback function for registering the dispatcher(POS_RegisterListenerPkgSensData)
 *
 *   Send a message to an internal thread.
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosPosifRegisterListenerPkgSensorData(HANDLE h_app) {
    RET_API ret;
    uint8_t* pRcvMsg;
    uint32_t size;
    EventID ulEventId;
    PCSTR pName;
    static const PCSTR pNone = "-";

    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "+");

    pRcvMsg = g_rcv_msg_buf;

    size = PosGetMsg(h_app, reinterpret_cast<void**>(&pRcvMsg), MAX_MSG_BUF_SIZE);
    if (size != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        pName = _pb_CnvPno2Name((reinterpret_cast<SENSOR_MSG_DELIVERY_ENTRY_DAT*>(pRcvMsg))->pno);
        if (pName == NULL) {
            pName = pNone;
        }
        FRAMEWORKUNIFIEDLOG(ZONE_17, __FUNCTION__, 
                      "Get message = [Sender:%s][CID:0x%X",
                      pName,
                      CID_SENSORIF_PKG_DELIVERY_ENTRY_EXT);

        /* Send Messages to Internal Thread */
        ret = PosSndMsg(PNO_VEHICLE_SENSOR, CID_SENSORIF_PKG_DELIVERY_ENTRY_EXT, pRcvMsg, size);
        if (ret != RET_NORMAL) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PosSndMsg ERROR!! [ret = %d]", ret);
            /* Event Generation */
            ulEventId = PosCreateEvent((reinterpret_cast<SENSOR_MSG_DELIVERY_ENTRY_DAT*>(pRcvMsg))->pno);
            if (ulEventId != 0) {
                /* Event publishing */
                ret = _pb_SetEvent(ulEventId, SAPI_EVSET_ABSOLUTE, SENSOR_RET_ERROR_INNER);
                if (ret != RET_NORMAL) {
                    /* Event issuance failure */
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SetEvent ERROR!! [ret = %d]", ret);
                }
                /* Event deletion */
                (void)PosDeleteEvent(ulEventId);
            } else {
                /* Event generation failure */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PosCreateEvent ERROR!!");
            }
        }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   Callback function for registering the dispatcher(POS_RegisterListenerSensData)
 *
 *   Send a message to an internal thread.
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosPosifRegisterListenerSensorData(HANDLE h_app) {
    RET_API ret;
    uint8_t* pRcvMsg;
    uint32_t size;
    EventID ulEventId;
    PCSTR pName;
    static const PCSTR pNone = "-";

    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "+");

    pRcvMsg = g_rcv_msg_buf;

    size = PosGetMsg(h_app, reinterpret_cast<void**>(&pRcvMsg), MAX_MSG_BUF_SIZE);
    if (size != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        pName = _pb_CnvPno2Name((reinterpret_cast<VEHICLE_MSG_DELIVERY_ENTRY_DAT*>(pRcvMsg))->pno);
        if (pName == NULL) {
            pName = pNone;
        }
        FRAMEWORKUNIFIEDLOG(ZONE_17, __FUNCTION__, 
                      "Get message = [Sender:%s][CID:0x%X]",
                      pName,
                      CID_VEHICLEIF_DELIVERY_ENTRY);

        /* Send Messages to Internal Thread */
        ret = PosSndMsg(PNO_VEHICLE_SENSOR, CID_VEHICLEIF_DELIVERY_ENTRY, pRcvMsg, size);
        if (ret != RET_NORMAL) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PosSndMsg ERROR!! [ret = %d]", ret);
            /* Event Generation */
            ulEventId = VehicleCreateEvent((reinterpret_cast<VEHICLE_MSG_DELIVERY_ENTRY_DAT*>(pRcvMsg))->pno);
            if (ulEventId != 0) {
                /* Event publishing */
                ret = _pb_SetEvent(ulEventId, SAPI_EVSET_ABSOLUTE, SENSOR_RET_ERROR_INNER);
                if (ret != RET_NORMAL) {
                    /* Event issuance failure */
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SetEvent ERROR!! [ret = %d]", ret);
                }
                /* Event deletion */
                (void)VehicleDeleteEvent(ulEventId);
            } else {
                /* Event generation failure */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "VehicleCreateEvent ERROR!!");
            }
        }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   Callback function for registering the dispatcher(POS_ReqGPSSetting)
 *
 *   Send a message to an internal thread.
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosPosifReqGpsSetting(HANDLE h_app) {
    RET_API ret;
    uint8_t* pRcvMsg;
    uint32_t size;
    uint8_t ucResult = SENSLOG_RES_FAIL;

    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "+");

    pRcvMsg = g_rcv_msg_buf;

    size = PosGetMsg(h_app, reinterpret_cast<void**>(&pRcvMsg), MAX_MSG_BUF_SIZE);
    if (size != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        FRAMEWORKUNIFIEDLOG(ZONE_17, __FUNCTION__, 
                      "Get message = [Sender:-][CID:0x%X]",
                      CID_SENSORIF__CWORD82__REQUEST);
        ucResult = SENSLOG_RES_SUCCESS;

        /* Send Messages to Internal Thread */
        ret = PosSndMsg(PNO_VEHICLE_SENSOR, CID_SENSORIF__CWORD82__REQUEST, pRcvMsg, size);
        if (ret != RET_NORMAL) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PosSndMsg ERROR!! [ret = %d]", ret);
        }
    }

    SensLogWriteInputData(SENSLOG_DATA_I_GPSSET, 0, 0, pRcvMsg, static_cast<uint16_t>(size),
                          ucResult, SENSLOG_ON, SENSLOG_ON);
    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   Callback function for registering the dispatcher(POS_SetGPSInfo)
 *
 *   Send a message to an internal thread.
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosPosifSetGpsInfo(HANDLE h_app) {
    RET_API ret;
    uint8_t* pRcvMsg;
    uint32_t size;
    uint8_t ucResult = SENSLOG_RES_FAIL;

    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "+");

    pRcvMsg = g_rcv_msg_buf;

    size = PosGetMsg(h_app, reinterpret_cast<void**>(&pRcvMsg), MAX_MSG_BUF_SIZE);
    if (size != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        FRAMEWORKUNIFIEDLOG(ZONE_17, __FUNCTION__, 
                      "Get message = [Sender:-][CID:0x%X]",
                      CID_NAVIINFO_DELIVER);
        ucResult = SENSLOG_RES_SUCCESS;

        /* Send Messages to Internal Thread */
        ret = PosSndMsg(PNO_NAVI_GPS_MAIN, CID_NAVIINFO_DELIVER, pRcvMsg, size);
        if (ret != RET_NORMAL) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PosSndMsg ERROR!! [ret = %d]", ret);
        }
    }
    SensLogWriteInputData(SENSLOG_DATA_I_GPSINF, 0, 0, pRcvMsg, static_cast<uint16_t>(size),
                         ucResult, SENSLOG_ON, SENSLOG_ON);
    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   Callback function for registering the dispatcher(POS_GetGPSInfo)
 *
 *   Send a message to an internal thread.
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosPosifGetGpsInfo(HANDLE h_app) {
    RET_API ret;
    uint8_t* pRcvMsg;
    uint32_t size;
    EventID ulEventId;
    PCSTR pName;
    static const PCSTR pNone = "-";

    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "+");

    pRcvMsg = g_rcv_msg_buf;

    size = PosGetMsg(h_app, reinterpret_cast<void**>(&pRcvMsg), MAX_MSG_BUF_SIZE);
    if (size != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        pName = _pb_CnvPno2Name((reinterpret_cast<VEHICLE_MSG_GET_VEHICLE_DATA_DAT*>(pRcvMsg))->pno);
        if (pName == NULL) {
            pName = pNone;
        }
        FRAMEWORKUNIFIEDLOG(ZONE_17, __FUNCTION__, 
                      "Get message = [Sender:%s][CID:0x%X]",
                      pName,
                      CID_VEHICLEIF_GET_VEHICLE_DATA);

        /* Send Messages to Internal Thread */
        ret = PosSndMsg(PNO_VEHICLE_SENSOR, CID_VEHICLEIF_GET_VEHICLE_DATA, pRcvMsg, size);
        if (ret != RET_NORMAL) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PosSndMsg ERROR!! [ret = %d]", ret);
            /* Event Generation */
            ulEventId = VehicleCreateEvent((reinterpret_cast<VEHICLE_MSG_GET_VEHICLE_DATA_DAT*>(pRcvMsg))->pno);
            if (ulEventId != 0) {
                /* Event publishing */
                ret = _pb_SetEvent(ulEventId, SAPI_EVSET_ABSOLUTE, POS_RET_ERROR_INNER);
                if (ret != RET_NORMAL) {
                    /* Event issuance failure */
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                  "_pb_SetEvent ERROR!! [ret = %d]",
                                  ret);
                }
                /* Event deletion */
                (void)VehicleDeleteEvent(ulEventId);
            } else {
                /* Event generation failure */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "VehicleCreateEvent ERROR!!");
            }
        }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   Callback function for registering the dispatcher(CID_POSIF_SET_DATA)
 *
 *   Send a message to an internal thread.
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosPosifSetData(HANDLE h_app) {
    RET_API ret;
    uint8_t* pRcvMsg;
    uint32_t size;
    EventID ulEventId;
    PCSTR pName;
    static const PCSTR pNone = "-";
    uint8_t ucResult = SENSLOG_RES_FAIL;

    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "+");

    pRcvMsg = g_rcv_msg_buf;

    size = PosGetMsg(h_app, reinterpret_cast<void**>(&pRcvMsg), MAX_MSG_BUF_SIZE);
//    if (size != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
//        ucResult = SENSLOG_RES_SUCCESS;
//
//        pName = _pb_CnvPno2Name((reinterpret_cast<POS_MSGINFO*>(pRcvMsg))->pno);
//        if (pName == NULL) {
//            pName = pNone;
//        }
//        FRAMEWORKUNIFIEDLOG(ZONE_17, __FUNCTION__, 
//                      "Get message = [Sender:%s][CID:0x%X][DID:0x%X]",
//                      pName,
//                      CID_POSIF_SET_DATA,
//                      (reinterpret_cast<POS_MSGINFO*>(pRcvMsg))->did);
//
//        /* Send Messages to Internal Thread */
//        ret = PosSndMsg(PNO_VEHICLE_SENSOR, CID_POSIF_SET_DATA, pRcvMsg, size);
//        if (ret != RET_NORMAL) {
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PosSndMsg ERROR!! [ret = %d]", ret);
//            if ((reinterpret_cast<POS_MSGINFO*>(pRcvMsg))->did == VEHICLE_DID_SETTINGTIME) {
//                /* GPS time setting(When waiting for completion of an event, an event is issued. */
//                /* Event Generation */
//                ulEventId = VehicleCreateEvent((reinterpret_cast<POS_MSGINFO*>(pRcvMsg))->pno);
//                if (ulEventId != 0) {
//                    /* Event publishing */
//                    ret = _pb_SetEvent(ulEventId, SAPI_EVSET_ABSOLUTE, POS_RET_ERROR_INNER);
//                    if (ret != RET_NORMAL) {
//                        /* Event issuance failure */
//                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
//                                      "_pb_SetEvent ERROR!! [ret = %d]",
//                                      ret);
//                    }
//                    /* Event deletion */
//                    (void)VehicleDeleteEvent(ulEventId);
//                } else {
//                    /* Event generation failure */
//                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "VehicleCreateEvent ERROR!!");
//                }
//            }
//        }
//    }
//    SensLogWriteInputData(SENSLOG_DATA_I_UNSPECIFIED,
//                          (reinterpret_cast<POS_MSGINFO*>(pRcvMsg))->did,
//                          0,
//                          pRcvMsg,
//                          static_cast<uint16_t>(size),
//                          ucResult,
//                          SENSLOG_ON,
//                          SENSLOG_ON);
    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   Callback function for registering the dispatcher(CID_GPS_REQRESET)
 *
 *   Send a message to an internal thread.
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosPosifReqGpsReset(HANDLE h_app) {
    RET_API ret;
    uint8_t* pRcvMsg;
    uint32_t size;
    EventID ulEventId;
    PCSTR pName;
    static const PCSTR pNone = "-";
    uint8_t ucResult = SENSLOG_RES_FAIL;

    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "+");

    pRcvMsg = g_rcv_msg_buf;

    size = PosGetMsg(h_app, reinterpret_cast<void**>(&pRcvMsg), MAX_MSG_BUF_SIZE);
    if (size != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
        ucResult = SENSLOG_RES_SUCCESS;

        pName = _pb_CnvPno2Name((reinterpret_cast<POS_RESETINFO*>(pRcvMsg))->snd_pno);
        if (pName == NULL) {
            pName = pNone;
        }
        FRAMEWORKUNIFIEDLOG(ZONE_17, __FUNCTION__, 
                      "Get message = [Sender:%s][CID:0x%X]",
                      pName,
                      CID_GPS_REQRESET);

        /* Send Messages to Internal Thread */
        ret = PosSndMsg(PNO_NAVI_GPS_MAIN, CID_GPS_REQRESET, pRcvMsg, size);
        if (ret != RET_NORMAL) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PosSndMsg ERROR!! [ret = %d]", ret);

            /* Event Generation */
            ulEventId = VehicleCreateEvent((reinterpret_cast<POS_RESETINFO*>(pRcvMsg))->snd_pno);
            if (0 != ulEventId) {
                /* Event publishing */
                ret = _pb_SetEvent(ulEventId, SAPI_EVSET_ABSOLUTE, POS_RET_ERROR_INNER);
                if (ret != RET_NORMAL) {
                    /* Event issuance failure */
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                  "_pb_SetEvent ERROR!! [ret = %d]",
                                  ret);
                }
                /* Event deletion */
                (void)VehicleDeleteEvent(ulEventId);
            } else {
                /* Event generation failure */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "VehicleCreateEvent ERROR!!");
            }
        }
    }

    SensLogWriteInputData(SENSLOG_DATA_I_GPSRST, 0, 0, pRcvMsg, static_cast<uint16_t>(size),
                          ucResult, SENSLOG_ON, SENSLOG_ON);
    FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, "-");

    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   Callback function for registering the dispatcher(CID_VEHICLESENS_VEHICLE_INFO)
 *
 *   Send a message to an internal thread.
 *
 * @param[in]  h_app    Application handle
 *
 * @return    eFrameworkunifiedStatusOK    Normal completion
 */
static EFrameworkunifiedStatus PosVehicleInfoRcv(HANDLE h_app) {
//  LSDRV_MSG_LSDATA_DAT_G line_sns_data;
  RET_API ret = RET_NORMAL;

//  memset(&line_sns_data, 0, sizeof(line_sns_data));
//  LSDRV_MSG_LSDATA_DAT_G *p_line_sns_data = &line_sns_data;
  VEHICLE_UNIT_MSG_VSINFO *p_vehicle_msg = NULL;
  uint32_t size = 0;
  uint16_t us_speed = 0;

  uint8_t* pRcvMsg = g_rcv_msg_buf;

  size = PosGetMsg(h_app, reinterpret_cast<void**>(&pRcvMsg), MAX_MSG_BUF_SIZE);
//  if (size != 0) {  // LCOV_EXCL_BR_LINE 4: nsfw error
//    memset(p_line_sns_data, 0x00, sizeof(line_sns_data));
//    p_vehicle_msg = (reinterpret_cast<VEHICLE_UNIT_MSG_VSINFO*>(pRcvMsg));
//    switch (p_vehicle_msg->data.did) {
//      // SPEED info
//      case VEHICLE_DID_SPEED:
//      {
//        p_line_sns_data->uc_data_num = LSDRV_KINDS_MAX;
//        p_line_sns_data->st_data[LSDRV_SPEED_KMPH].ul_did = POSHAL_DID_SPEED_KMPH;
//        p_line_sns_data->st_data[LSDRV_SPEED_KMPH].uc_size = POS_SNDMSG_DTSIZE_2;
//        p_line_sns_data->st_data[LSDRV_SPEED_KMPH].uc_sns_cnt = 0;
//
//        char p_env_variable[VP_MAX_LENGTH];
//
//        VP_GetEnv("VP__CWORD31__SPEED", p_env_variable);
//        if (0 == strcmp(p_env_variable, "direct")) {
//          int16_t speed = 0;
//          // To obtain the vehicle speed from Direct lanes,1 to 2 bytes of received data
//          memcpy(&speed, &p_vehicle_msg->data.data[0], sizeof(speed));
//          us_speed = static_cast<uint16_t>(abs(speed));
//          // Unit conversion [km/h] -> [0.01km/h]
//          us_speed = static_cast<uint16_t>(us_speed * 100);
//        } else if (0 == strcmp(p_env_variable, "CAN")) {
//          UINT16 speed = 0;
//          // To obtain the vehicle speed from CAN,2 to 3 bytes of received data
//          memcpy(&speed, &p_vehicle_msg->data.data[1], sizeof(speed));
//          us_speed = static_cast<uint16_t>(abs(speed));
//          // Unit conversion [km/h] -> [0.01km/h]
//          us_speed = static_cast<uint16_t>(us_speed * 100);
//        } else {
//          // In case of invalid value, the vehicle speed is acquired by CAN.
//          UINT16 speed = 0;
//          memcpy(&speed, &p_vehicle_msg->data.data[1], sizeof(speed));
//          us_speed = static_cast<uint16_t>(abs(speed));
//          // Unit conversion [km/h] -> [0.01km/h]
//          us_speed = static_cast<uint16_t>(us_speed * 100);
//        }
//
//        memcpy(&p_line_sns_data->st_data[LSDRV_SPEED_KMPH].uc_data[0], &us_speed, sizeof(us_speed));
//
//        ret = PosSndMsg(PNO_VEHICLE_SENSOR, CID_LINESENS_VEHICLE_DATA_G,
//                        reinterpret_cast<void*>(p_line_sns_data), sizeof(line_sns_data));
//
//        break;
//      }
//      // REV information
//      case VEHICLE_DID_REV:
//      {
//        p_line_sns_data->uc_data_num = LSDRV_KINDS_MAX;
//        p_line_sns_data->st_data[LSDRV_REV].ul_did = VEHICLE_DID_REV;
//        p_line_sns_data->st_data[LSDRV_REV].uc_size = POS_SNDMSG_DTSIZE_1;
//        p_line_sns_data->st_data[LSDRV_REV].uc_sns_cnt = 0;
//        p_line_sns_data->st_data[LSDRV_REV].uc_data[0] = p_vehicle_msg->data.data[0];
//
//        ret = PosSndMsg(PNO_VEHICLE_SENSOR, CID_LINESENS_VEHICLE_DATA_G,
//                            reinterpret_cast<void*>(p_line_sns_data), sizeof(line_sns_data));
//        break;
//      }
//
//      // Speed Pulse information
//      case VEHICLE_DID_SPEED_PULSE_VEHICLE:
//      {
//        p_line_sns_data->uc_data_num = LSDRV_KINDS_MAX;
//        p_line_sns_data->st_data[LSDRV_SPEED_PULSE].ul_did = POSHAL_DID_SPEED_PULSE;
//        p_line_sns_data->st_data[LSDRV_SPEED_PULSE].uc_size = sizeof(float);
//        p_line_sns_data->st_data[LSDRV_SPEED_PULSE].uc_sns_cnt = 0;
//
//        memcpy(&p_line_sns_data->st_data[LSDRV_SPEED_PULSE].uc_data[0],
//                                            &p_vehicle_msg->data.data[0], sizeof(float));
//
//        p_line_sns_data->st_data[LSDRV_PULSE_TIME].ul_did = POSHAL_DID_PULSE_TIME;
//        p_line_sns_data->st_data[LSDRV_PULSE_TIME].uc_size = sizeof(float);
//        p_line_sns_data->st_data[LSDRV_PULSE_TIME].uc_sns_cnt = 0;
//
//        memcpy(&p_line_sns_data->st_data[LSDRV_PULSE_TIME].uc_data[0],
//                       &p_vehicle_msg->data.data[sizeof(float)], sizeof(float));
//
//        ret = PosSndMsg(PNO_VEHICLE_SENSOR, CID_LINESENS_VEHICLE_DATA_G,
//                            reinterpret_cast<void*>(p_line_sns_data), sizeof(line_sns_data));
//
//        break;
//      }
//      default:
//        break;
//    }
//
//    if (ret != RET_NORMAL) {
//      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PosSndMsg ERROR!! [ret = %d]", ret);
//    }
//  }
  return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   Get Message
 *
 *   Get messages received by the dispatcher
 *
 * @param[in]  h_app  Application handle
 * @param[out] p_buf  Pointer to receive data storage buffer
 * @param[in]  Sizes of size receive data storage buffers
 *
 * @return    Received data size(0:Receiving error)
 */
static uint32_t PosGetMsg(HANDLE h_app, void** p_buf, uint32_t size) {
    EFrameworkunifiedStatus eStatus;
    uint32_t ulSize = 0;
    void* pRcvBuf;

    /* null check */
    if ((h_app == NULL) || (p_buf == NULL)) {  // LCOV_EXCL_BR_LINE 200: can not NULL
        // LCOV_EXCL_START 8: invalid
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                      "Argument ERROR!! [h_app = %p, p_buf = %p]",
                      h_app,
                      p_buf);
        // LCOV_EXCL_STOP
    } else {
        /* Check the size of received data */
        ulSize = FrameworkunifiedGetMsgLength(h_app);
        if (ulSize > size) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                          "Message size ERROR!! [ulSize = %d, maxsize = %d]",
                          ulSize,
                          size);
            ulSize = 0;
        } else {
            /* Obtain data */
            eStatus = FrameworkunifiedGetDataPointer(h_app, &pRcvBuf);
            if (eStatus == eFrameworkunifiedStatusOK) {
                *p_buf = pRcvBuf;
            } else if (eStatus == eFrameworkunifiedStatusInvldBufSize) {
                eStatus = FrameworkunifiedGetMsgDataOfSize(h_app, *p_buf, ulSize);
                /* When acquisition fails */
                if (eStatus != eFrameworkunifiedStatusOK) {
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                                  "FrameworkunifiedGetMsgDataOfSize ERROR [eStatus:%d]",
                                  eStatus);
                    ulSize = 0;
                }
            } else {
                /* When acquisition fails */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                              "FrameworkunifiedGetDataPointer ERROR [eStatus:%d]",
                              eStatus);
                ulSize = 0;
            }
        }
    }

    return ulSize;
}

/**
 * @brief
 *   Sending Messages for Internal Threads
 *
 *   Adds a header to the message received by the dispatcher and sends it to the internal thread..
 *
 * @param[in]  pno       PNO
 * @param[in]  cid      Command ID
 * @param[in]  p_msg_body  Sent message body
 * @param[in]  size         Send message size
 *
 * @return    RET_NORAML    Normal completion
 *            RET_ERROR    ABENDs
 */
static RET_API PosSndMsg(PNO pno, CID cid, void* p_msg_body, uint32_t size) {
    RET_API    ret = RET_NORMAL;
    T_APIMSG_MSGBUF_HEADER* p;
    static u_int8 sndMsgBuf[MAX_MSG_BUF_SIZE + sizeof(T_APIMSG_MSGBUF_HEADER)];

    if (p_msg_body == NULL) {  // LCOV_EXCL_BR_LINE 200: can not NULL
        // LCOV_EXCL_START 8: invalid
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR!! [p_msg_body = %p]", p_msg_body);
        ret = RET_ERROR;
        // LCOV_EXCL_STOP
    } else {
        p = reinterpret_cast<T_APIMSG_MSGBUF_HEADER*>(sndMsgBuf);

        /* Message header addition */
        p->hdr.cid = cid;
        p->hdr.msgbodysize = static_cast<uint16_t>(size);

        /* Copy of the data section */
        memcpy((p + 1), p_msg_body, size);

        /* Send a message to an internal thread */
        ret = _pb_SndMsg(pno, static_cast<uint16_t>(size + sizeof(T_APIMSG_MSGBUF_HEADER)), p, 0);
        /* When transmission fails */
        if (ret != RET_NORMAL) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SndMsg ERROR [ret = %d]", ret);
            ret = RET_ERROR;
        }
    }

    return ret;
}

/**
 * @brief
 *   SS_DevDetectSrv service OpenSessionAck Callback Functions
 *
 * @param  none
 *
 * @return eFrameworkunifiedStatusOK successful completion
 */
static EFrameworkunifiedStatus PosOnDevDetectOpenSessionAck(HANDLE h_app) {
    EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
    BOOL bIsAvailable;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    eStatus = DevDetectSrvIfDecodeOpenSessionResponse(&bIsAvailable);
    if (eFrameworkunifiedStatusOK != eStatus) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                      "DeviceDetectionServiceIf OpenSession ERROR!! [sts=%d, ava=%d]", eStatus, bIsAvailable);
    } else {
        eStatus = DevDetectSrvIfRegisterForDeviceDetectionEvent(SS_DEV_DETECT_ANY_USB_EV,
                                                                 &PosOnDevDetectEvent,
                                                                 NULL,
                                                                 &bIsAvailable);
        if (eFrameworkunifiedStatusOK != eStatus) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                          "DeviceDetectionServiceIf RegisterEvent ERROR!! [sts=%d, ava=%d]", eStatus, bIsAvailable);
        } else {
            eStatus = DevDetectSrvIfNotifyOnCloseSessionAck(&PosOnDevDetectCloseSessionAck, &bIsAvailable);
            if (eFrameworkunifiedStatusOK != eStatus) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                     "DeviceDetectionServiceIf CloseSessionAck ERROR!! [sts=%d, ava=%d]",
                              eStatus,
                              bIsAvailable);
            }
        }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");
    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   SS_DevDetectSrv service CloseSessionAck Callback Functions
 *
 * @param  none
 *
 * @return eFrameworkunifiedStatusOK successful completion
 */
static EFrameworkunifiedStatus PosOnDevDetectCloseSessionAck(HANDLE h_app) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    /* Sensor log stop processing */
    SensLogTerminate();

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");
    return eFrameworkunifiedStatusOK;
}

/**
 * @brief
 *   SS_DevDetectSrv service Event Callback Functions
 *
 * @param  none
 *
 * @return eFrameworkunifiedStatusOK successful completion
 */
static EFrameworkunifiedStatus PosOnDevDetectEvent(HANDLE h_app) {
    SS_MediaDetectInfo stMedia;
    EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
    uint32_t ulSize = 0;
    uint8_t mode;

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "+");

    /* Check the size of received data */
    ulSize = FrameworkunifiedGetMsgLength(h_app);
    if (ulSize > sizeof(SS_MediaDetectInfo)) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Message size ERROR!! [ulSize = %d, maxsize = %ld]",
                      ulSize, sizeof(SS_MediaDetectInfo));
        ulSize = 0;
    } else {
        /* Obtain data */
        eStatus = FrameworkunifiedGetMsgDataOfSize(h_app, &stMedia, ulSize);
        /* When acquisition fails */
        if (eStatus != eFrameworkunifiedStatusOK) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                          "FrameworkunifiedGetMsgDataOfSize ERROR [eStatus:%d]", eStatus);
            ulSize = 0;
        } else {
            if (eUSB == stMedia.dev_type) {
                if (TRUE == stMedia.bIsDeviceAvailable) {
                    /* Mount detection */
                    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, 
                        "USB mounted sts=[%d] path=[%s] fusedav=[%d]",
                        stMedia.bIsDeviceAvailable, stMedia.deviceMountpath, stMedia.bIsFuseDav);

                    /* NOTE: bIsFuseDav -> From the _CWORD80_'s point of view,                          */
                    /*       TRUE(With a USB flash drive inserted into the _CWORD84_,Be synchronized on the FuseDav)*/
                    /*       FALSE(USB memory is inserted into the _CWORD80_.)                        */

                    if (stMedia.bIsFuseDav == FALSE) {
                        /* Inserted into your USB port */
                        FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "SensLog Initialize start.");
                        /*  Get Debug/Release setting */
                        eStatus = NsLogGetFrameworkunifiedLogFlag(POSITIONINGLOG_FLAG_NAVI, &mode);
                        /* When acquisition fails */
                        if (eStatus != eFrameworkunifiedStatusOK) {
                            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                            "NsLogGetFrameworkunifiedLogFlag ERROR [eStatus:%d]", eStatus);
                        } else {
                            if (mode == FRAMEWORKUNIFIEDLOG_FLAG_MODE_DEBUG) {
                                /* Sensor Log Initial Processing(Normal)*/
                                SensLogInitialize(reinterpret_cast<uint8_t *>(stMedia.deviceMountpath));
                            }
                        }
                    } else {
                        /* TODO: Mounts (FuseDav synchronized) that are not inserted into your device's USB port are not supported. */
                        FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "Don't output SensLog.");
                    }
                } else {
                    /* Unmount detection */
                    if (stMedia.bIsFuseDav == FALSE) {
                        /* When it is unmounted to its own USB */
                        FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, 
                            "USB umounted sts=[%d] fusedav=[%d]",
                            stMedia.bIsDeviceAvailable, stMedia.bIsFuseDav);

                        /* Sensor log stop processing */
                        SensLogTerminate();
                    }
                }
            }
        }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, "-");
    return eStatus;
}

/**
 * @brief Debug dump log output function
 *
 * @param[in]    *p_buf Pointer to the output string
 */
static void PosOutputDebugDumpLog(uint8_t* p_buf) {  // LCOV_EXCL_START 7: uesd only in FrameworkunifiedOnDebugDump
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SSDEBUGDUMP("%s\n", p_buf);
    MilliSecSleep(1);
    return;
}
// LCOV_EXCL_STOP

