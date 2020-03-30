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

#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_DEADRECKONING_MAIN_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_DEADRECKONING_MAIN_H_
/****************************************************************************
 * File name        :DeadReckoning_main.h
 * System name        :PastModel002
 * Subsystem name    :
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>

#include "Sensor_Common_API.h"

#include "Dead_Reckoning_API.h"
#include "Dead_Reckoning_Local_Api.h"
#include "DeadReckoning_Common.h"
#include "DeadReckoning_DataMaster.h"
#include "DeadReckoning_DeliveryCtrl.h"

#include "Sensor_API.h"
#include "VehicleSens_DeliveryCtrl.h"
#include "VehicleDebug_API.h"
/************************************************************************
*            Macro definitions                                                 *
************************************************************************/
/* Return value */
#define RET_SWMNG_SUCCESS                             RET_NORMAL    /* Success of API processing                */
#define RET_SWMNG_ERROR                               RET_ERROR     /* API processing failed                */

/*** Rcv Message Relationships ***************************************************/
/* Size of data section */
#define DEAD_RECKONING_RCVMSG_MSGBUF                  512           /* RcvMSG Byte count in data section    */
#define DEAD_RECKONING_RCVDATA_SENSOR_MSGBUF          40            /* RcvMSG Byte count of Sensor data section     */
#define DEAD_RECKONING_RCVDATA_SENSOR_FIRST_MSGBUF    1000          /* RcvMSG Initial Byte count of sensor data section    */
#define DEAD_RECKONING_SENSOR_FIRST_NUM               175           /* Initial number of saved sensor data            */

/* Event initial value                                            */
/* DR_API_private. Synchronize with DR_EVENT_VAL_INIT in h    */
#define DR_EVENT_VAL_INIT                             (-101)

/* Event initial value                                                        */
/* DeadReckoning_main. Synchronize with h's VEHICELEDEBUG_EVENT_VAL_INIT    */
#define VEHICLEDEBUG_EVENT_VAL_INIT                   (-101)

/* PastModel002 sleep time[ms] for CPU load balancing in the first sensor data processing            */
#define DR_FST_SENS_CALC_SLEEP_TIME                   15

/************************************************************************
*            Struct definitions                                                 *
************************************************************************/

/*********************************************************************
* TAG       : DEAD_RECKONING_RCVDATA
* ABSTRACT  : Message receive buffer structure
***********************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER    hdr;
    u_int8                    data[DEAD_RECKONING_RCVMSG_MSGBUF];                               /* Data portion */
} DEAD_RECKONING_RCVDATA;

/*********************************************************************
* TAG       : DEAD_RECKONING_RCVDATA_SENSOR
* ABSTRACT  : LineSensor Vehicle Signal Notification Message Structure
***********************************************************************/
typedef struct {
    u_int32                   did;                                                              /* Data ID */
    u_int8                    size;                                                             /* Size of the data */
    u_int8                    reserve[3];                                                       /* Reserved */
    u_int8                    data[DEAD_RECKONING_RCVDATA_SENSOR_MSGBUF];                       /* Data content */
} DEAD_RECKONING_RCVDATA_SENSOR;

/*********************************************************************
* TAG       : DEAD_RECKONING_RCVDATA_SENSOR_FIRST
* ABSTRACT  : LineSensor Vehicle Signal Notification Message Structure
***********************************************************************/
typedef struct {
    u_int32                   did;                                                              /* Data ID */
    u_int16                   size;                                                             /* Size of the data */
    u_int8                    reserve[1];                                                       /* Reserved */
    u_int8                    num;                                                              /* Number of data */
    u_int8                    data[DEAD_RECKONING_RCVDATA_SENSOR_FIRST_MSGBUF];                 /* Data content */
} DEAD_RECKONING_RCVDATA_SENSOR_FIRST;

/*********************************************************************
* TAG       : DEAD_RECKONING_SAVEDATA_SENSOR_FIRST
* ABSTRACT  : LineSensor Vehicle Signal-Storage Message Structures
***********************************************************************/
typedef struct {
    u_int16                   gyro_data[DEAD_RECKONING_SENSOR_FIRST_NUM * NUM_OF_100msData];    /* Data content */
    u_int16                   spd_pulse_data[DEAD_RECKONING_SENSOR_FIRST_NUM];                  /* Data content */
    u_int8                    spd_pulse_check_data[DEAD_RECKONING_SENSOR_FIRST_NUM];            /* Data content */
    u_int8                    rev_data[DEAD_RECKONING_SENSOR_FIRST_NUM];                        /* Data content */
    u_int8                    data_num;                                                         /* Number of data */
    u_int8                    reserve[3];                                                       /* reserve */
    u_int16                   gyro_rcv_size;                                                    /* Receiving Size */
    u_int16                   spd_pulse_rcv_size;                                               /* Receiving Size */
    u_int16                   spd_pulse_check_rcv_size;                                         /* Receiving Size */
    u_int16                   rev_rcv_size;                                                     /* Receiving Size */
} DEAD_RECKONING_SAVEDATA_SENSOR_FIRST;

/*********************************************************************
* TAG       : DEAD_RECKONING_LONLAT_INFO
* ABSTRACT  : Get Position Information Structure
***********************************************************************/
typedef struct {
    u_int8                    calc_called;    /* Whether Calculate_DeadReckoningData was called */
    u_int8                    available;
    u_int8                    reserve[2];
    LONLAT                    lonlat;
} DEAD_RECKONING_LONLAT_INFO;

/************************************************************************
*            DeadReckoning_main function prototype                         *
************************************************************************/

void DeadReckoningRcvMsg(const DEAD_RECKONING_RCVDATA *msg, DEAD_RECKONING_LONLAT_INFO *location_info);
//void  DeadReckoningRcvMsgFstSens(const DEAD_RECKONING_RCVDATA *msg, DEAD_RECKONING_LONLAT_INFO *location_info,
//                            const SENSOR_MSG_GPSDATA_DAT  *rcv_gps_msg, Struct_GpsData* send_gps_msg,
//                            Struct_SensData* send_sensor_msg);
int32 DeadReckoningInit(void);
void DeadReckoningGetDRData(const DEADRECKONING_MSG_GET_DR_DATA *msg);
void DeadReckoningSetMapMatchingData(const DR_MSG_MAP_MATCHING_DATA *msg);
void DeadReckoningClearBackupData(const DR_MSG_CLEAR_BACKUP_DATA *msg);
void DeadReckoningGetLocationLogStatus(PNO pno);
void DeadReckoningSetLocationLogStatus(BOOL log_sw, u_int8 severity);

#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_DEADRECKONING_MAIN_H_ */
