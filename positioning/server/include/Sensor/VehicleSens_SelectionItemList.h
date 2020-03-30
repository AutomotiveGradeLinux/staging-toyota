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

#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_SELECTIONITEMLIST_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_SELECTIONITEMLIST_H_
/****************************************************************************
 * File name        :VehicleSens_SelectionItemList.h
 * System name        :_CWORD107_
 * Subsystem name    :Vehicle sensor process
 ****************************************************************************/

#include "Vehicle_API_Dummy.h"
#include "Vehicle_API_private.h"
#include "VehicleSens_Common.h"
#include "VehicleIf.h"

/************************************************************************
*            Macro definitions                                                    *
************************************************************************/
#if CONFIG_SENSOR_EXT_VALID                /* Initial Sensor Support */
/* ++ Addition of DID9 related to PastModel002 supported_UBX commands */
/* ++ GPS _CWORD82_ support (To add the target DID 3) */
#define VEHICLESENS_SELECTION_ITEM_LIST_LEN    (81)            /* Number of data in Vehicle Sensor Selection Item List */
/* -- GPS _CWORD82_ support */
#else
#define VEHICLESENS_SELECTION_ITEM_LIST_LEN    (50 + 10)        /* Number of data in Vehicle Sensor Selection Item List  */
/* -- PastModel002 support */
#endif
/* Sum of bits 29 through 31*/
#define VEHICLESENS_BIT31_29    (VEHICLESENS_BIT31 | VEHICLESENS_BIT30 | VEHICLESENS_BIT29)

#define VEHICLESENS_ITEMLIST_APPLICATION        0x00               /* Application of electronic PF        */
#define VEHICLESENS_ITEMLIST_NON_APPLICATION    0x01               /* Electronic PF not applicable        */
#define VEHICLESENS_ITEMLIST_INVALID            0x02              /* Disabled                */

#define VEHICLE_COMM_WATCHTBL_DAT_NUM            64          /* Number of data held for communication discontinuation registration    */
#define VEHICLE_COMM_WATCHTBL_DID_NUM            2            /* Number of Disruption Monitoring Data Management        */

/************************************************************************
*            Struct definitions                                                    *
************************************************************************/

/*********************************************************************
* TAG       : VEHICLESENS_SELECTION_ITEM_LIST
* ABSTRACT  : Managing a list of vehicle information choices
***********************************************************************/

typedef struct {
    DID    ul_did;                    /* Data ID                */
    u_int32    ul_canid;            /* CAN ID                */    /* CANIF_API deletion */
    u_int8    uc_get_method;        /* Data acquisition source category        */
    u_int8    reserve[3];            /* reserve                */
} VEHICLESENS_SELECTION_ITEM_LIST;

/************************************************************************
* TAG       : VEHICLE_COMM_WATCH_TBL
* ABSTRACT  : Managing Vehicle Sensor Disruption Monitoring Data
************************************************************************/
typedef struct {
    PNO            us_pno;                                            /* Destination PNO                        */
    u_int16     us_watch_time;                                    /* Interruption monitoring time(Units of 100 ms)            */
} VEHICLE_COMM_WATCH_DAT;

typedef struct {
    DID                        ul_did;                                /* Data ID corresponding to vehicle sensor information    */
    u_int8                    uc_effective_flg;                        /* CANID Valid Flag                    */
    u_int8                    uc_vehicle_comm_watch_cnt;                /* Vehicle Sensor Information Disruption Monitoring Request Count    */
    VEHICLE_COMM_WATCH_DAT    st_comm_watch_dat[VEHICLE_COMM_WATCHTBL_DAT_NUM];    /* Communication disruption registration data    */
} VEHICLE_COMM_WATCH_TBL;

/* ++ PastModel002 support */
/************************************************************************
* TAG       : VEHICLE_MSG_WATCH_STOPPAGE
* ABSTRACT  : Vehicle Sensor Disruption Monitoring Message(-> Vehicle sensor)
************************************************************************/
typedef struct {
    DID            ul_did;                    /* Data ID corresponding to vehicle sensor information    */
    PNO            us_pno;                    /* Destination PNO                        */
    u_int16        us_watch_time;            /* Interruption monitoring time(Units of 100 ms)            */
    u_int16        us_offset;                /* Offset to shared memory storage area    */
    u_int16        us_size;                 /* Size of shared memory storage area            */
    EventID        ul_eventid;                /* Event ID                        */
} VEHICLE_MSG_WATCH_STOPPAGE_DAT;

typedef struct {
    T_APIMSG_MSGBUF_HEADER                st_hdr;        /* Message header    */
    VEHICLE_MSG_WATCH_STOPPAGE_DAT        st_data;        /* Message data    */
} VEHICLE_MSG_WATCH_STOPPAGE;

/* -- PastModel002 support */

/************************************************************************
*            Function prototype                                              *
************************************************************************/
void    VehicleSensInitSelectionItemList(void);
u_int8    VehicleSensGetSelectionItemList(DID);
u_int32    VehicleSensGetSelectionItemListCanId(DID);
BOOL    VehicleSensSetSelectionItemListCanId(DID did, u_int32);    /* CANIF_API deletion */
void    VehicleSensCommWatchTblInit(void);
BOOL    VehicleSensCommWatchTblSave(const VEHICLE_MSG_WATCH_STOPPAGE*);
BOOL    VehicleSensCommWatchTblRun(DID);

#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_SELECTIONITEMLIST_H_
