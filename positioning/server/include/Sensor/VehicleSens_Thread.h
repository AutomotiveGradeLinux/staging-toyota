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

#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_THREAD_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_THREAD_H_
/******************************************************************************
 * File name        :VehicleSens_Thread.h
 * System name        :_CWORD107_
 * Subsystem name    :
 ******************************************************************************/

#include "Vehicle_API_Dummy.h"
#include "Sensor_API.h"
#include "Vehicle_API_private.h"
#include "Sensor_API_private.h"
#include "VehicleSens_Common.h"
#include "VehicleSens_FromMng.h"
#include "VehicleSens_SelectionItemList.h"
#include "VehicleSens_DataMaster.h"
#include "VehicleSens_DeliveryCtrl.h"
#include "VehicleSens_SharedMemory.h"
#include "CanInput_API.h"
#include "CanInput_API_private.h"
#include "VehicleDebug_API.h"
#include "VehicleSens_FromAccess.h"
#include "ClockDataMng.h"
//#include "gps_hal.h"
//#include "positioning_hal.h"
#include "CommonDefine.h"

/* ++ DR support */
#include "DeadReckoning_main.h"
#include "Dead_Reckoning_Local_Api.h"
/* -- DR support */

#include "POS_common_private.h"

/************************************************************************
*            Macro definitions                                                 *
************************************************************************/

/* ++ Porting from PastModel002 enabled APIs */
#define CID_VEHICLEIF_GET_VEHICLE_DATA    0x0102    /* Vehicle sensor information acquisition CID        */
#define CID_VEHICLEIF_COMM_WATCH          0x0103    /* Vehicle Sensor Information Disruption Monitoring CID    */
#define CID_SENSORIF__CWORD82__REQUEST          0x0800    /* Vehicle sensor information setting CID        */

#define VEHICLE_RET_ERROR_OUTOF_MEMORY    (-8)      /* Shared memory allocation failure    */
#define VEHICLE_RET_ERROR_INVALID         (-10)     /* CANID undetermined        */

/* -- Porting from PastModel002 enabled APIs */


/* NMEA */
#define VEHICLESENS_NMEA_LF                      "\n"
#define VEHICLESENS_NMEA_CR                      "\r"
#define VEHICLESENS_NMEA_FIELDDELIMITER          ","
#define VEHICLESENS_NMEA_DECIMALPOINT            "."
#define VEHICLESENS_NMEA_ASTARISK                "*"

#define VEHICLESENS_NMEA_PASCD_LEN_MAX           256

#define VEHICLESENS_NMEA_PASCD_ID                "$PASCD"

#define VEHICLESENS_NMEA_PASCD_TS_MAX            86400
#define VEHICLESENS_NMEA_PASCD_TS_INT_LEN_MAX    6   /* max length of integer part of timestamp */
#define VEHICLESENS_NMEA_PASCD_TS_FRA_LEN_MAX    3   /* max length of fractional part of timestamp */

#define VEHICLESENS_NMEA_PASCD_SENSORTYPE_C      "C" /* Combined left and right wheel speed sensors */

#define VEHICLESENS_NMEA_PASCD_TMS_U             "U" /* Unkonwn */
#define VEHICLESENS_NMEA_PASCD_TMS_P             "P" /* Park */
#define VEHICLESENS_NMEA_PASCD_TMS_R             "R" /* Reverse */
#define VEHICLESENS_NMEA_PASCD_TMS_D             "D" /* Driving forword */
#define VEHICLESENS_NMEA_PASCD_TMS_N             "N" /* Neutral */

#define VEHICLESNES_NMEA_PASCD_SLIP              "1" /* 1 = a wheel speed slippage was detected */
#define VEHICLESNES_NMEA_PASCD_NOSLIP            "0" /* 0 = no slip was detected */

#define VEHICLESENS_NMEA_PASCD_TO_FRA_LEN_MAX    2   /* max length of fractional part of timeoffset */

#define VEHICLESENS_NMEA_PASCD_SPD_FRA_LEN_MAX   3   /* max length of fractional part of speed */

#define VEHICLESENS_NMEA_PASCD_SAMPLECOUNT_MAX   50

/************************************************************************
 *            Struct definitions                                                *
 ************************************************************************/

/*!
    @brief   Structure of Vehicle Speed and TimeSpec
 */
typedef struct {
    timespec ts;
    uint16_t speed; /* [0.01m/s] */
} VEHICLESENS_VEHICLE_SPEED_DAT;

/*!
    @brief   Structure of Vehilce Speeds and TimeSpecs
 */
typedef struct {
    VEHICLESENS_VEHICLE_SPEED_DAT listSpd[VEHICLESENS_NMEA_PASCD_SAMPLECOUNT_MAX];
    uint8_t sampleCount;
} VEHICLESENS_VEHICLE_SPEED_INFO;

/*!
    @brief   Structure for LUT to Derive Transmission State
 */
typedef struct {
    uint8_t type;     /* Transmission Type */
    uint8_t shift;    /* Shift Position from Vehicle */

    uint8_t pkb;      /* Parking Brake from Vehicle */
    char    state[8]; /* Transmission State for _CWORD27_ */
} VEHICLESENS_TRANSMISSION_PKG;

/************************************************************************
 * TAG       : VEHICLE_MSG_SEND_DAT
 * ABSTRACT  : Vehicle sensor information setting message(-> Vehicle sensor)
 ************************************************************************/



/************************************************************************
*            Function prototype                                            *
************************************************************************/
EFrameworkunifiedStatus VehicleSensThread(HANDLE h_app);
RET_API VehicleSensThreadInit(void);
void VehicleSensDeliveryEntry(const VEHICLE_MSG_DELIVERY_ENTRY *);
void VehicleSensGetVehicleData(const VEHICLE_MSG_GET_VEHICLE_DATA *);
void VehicleSensWatchStopPage(const VEHICLE_MSG_WATCH_STOPPAGE *);
void VehicleSensPkgDeliveryEntry(const SENSOR_MSG_DELIVERY_ENTRY *);
void VehicleSensGetVehiclePkgData(const SENSOR_MSG_GET_SENSOR_DATA *);
//void VehicleSensLineSensDataDelivery(const LSDRV_MSG_LSDATA *, PFUNC_DMASTER_SET_N);
//void VehicleSensLineSensDataDeliveryG(const LSDRV_MSG_LSDATA_G *, PFUNC_DMASTER_SET_N);
//void VehicleSensLineSensDataDeliveryGyroTrouble(const LSDRV_MSG_LSDATA_GYRO_TROUBLE *msg,
//                                                PFUNC_DMASTER_SET_N p_datamaster_set_n);
//void VehicleSensLineSensDataDeliverySysGpsInterruptSignal(const LSDRV_MSG_LSDATA_GPS_INTERRUPT_SIGNAL *msg,
//                                                PFUNC_DMASTER_SET_SHARED_MEMORY p_datamaster_set_sharedmemory);
//void VehicleSensLineSensDataDeliveryGyroConnectStatus(const LSDRV_MSG_LSDATA_GYRO_CONNECT_STATUS *msg,
//                                                      PFUNC_DMASTER_SET_SHARED_MEMORY p_datamaster_set_sharedmemory);
//void VehicleSensLineSensDataDeliveryGpsAntennaStatus(const LSDRV_MSG_LSDATA_GPS_ANTENNA_STATUS *msg,
//                                                     PFUNC_DMASTER_SET_N p_datamaster_set_n);

#if CONFIG_SENSOR_EXT_VALID                /* Initial Sensor Support */
void VehicleSensPkgDeliveryEntryExt(const SENSOR_MSG_DELIVERY_ENTRY *msg);
//void VehicleSensLineSensDataDeliveryFst(const LSDRV_MSG_LSDATA_FST *, PFUNC_DMASTER_SET_N);
//void VehicleSensLineSensDataDeliveryFstG(const LSDRV_MSG_LSDATA_FST *msg, PFUNC_DMASTER_SET_N p_datamaster_set_n);
#else
void VehicleSensPkgDeliveryEntryError(const SENSOR_MSG_DELIVERY_ENTRY *msg);
#endif
//void VehicleSensGpsDataDelivery(SENSOR_MSG_GPSDATA *msg,
//                                PFUNC_DMASTER_SET_N p_datamaster_set_n,
//                                PFUNC_DMASTER_SET_SHARED_MEMORY p_datamaster_set_sharedmemory);

void VehicleSensDataMasterSetN(DID did, u_int8 chg_type, u_int8 get_method);
void VehicleSensDataMasterSetSharedMemory(DID did, u_int8 chg_type);
/* ++ GPS _CWORD82_ support */
void VehicleSensSetVehicleData(const VEHICLE_MSG_SEND *);
/* -- GPS _CWORD82_ support */

void VehicleSensDrDeliveryEntry(const VEHICLE_MSG_DELIVERY_ENTRY *);
void VehicleSensGetLog(const VEHICLEDEBUG_MSG_BUF* msg);
void VehicleSensSetLog(const VEHICLEDEBUG_MSG_BUF* msg);
void VehicleSensWriteLocalTime(const CANINPUT_MSG_INFO *msg);
//void VehicleSensSetEphNumSharedMemory(const SENSOR_MSG_GPSDATA *msg);
void VehicleSensDrRcvMsg(const DEAD_RECKONING_RCVDATA *msg);

void VehicleSensCommonDataDelivery(const VEHICLE_MSG_BUF *msg, PFUNC_DMASTER_SET_N p_datamaster_set_n);
//void VehicleSensGpsTimeSndMsg(const POS_MSGINFO *pos_msg);
void VehicleSensGpsTimeDelivery(const VEHICLE_MSG_BUF *msg);
RET_API VehicleSensSendEvent(uint16_t snd_pno, int32_t event_val);
void VehicleSensThreadStopProcess(void);
#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_THREAD_H_
