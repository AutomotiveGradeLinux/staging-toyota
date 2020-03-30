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

#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_DEADRECKONING_DELIVERYCTRL_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_DEADRECKONING_DELIVERYCTRL_H_
/******************************************************************************
 * File name    :DeadReckoning_DeliveryCtrl.h
 * System name    :PastModel002
 * Subsystem name  :
 ******************************************************************************/

#include "DeadReckoning_DataMaster.h"
#include "Dead_Reckoning_Local_Api.h"

/************************************************************************
*      Macro definitions                          *
************************************************************************/
#define  DEADRECKONING_SIGNAL               6    /* Number of vehicle signals        */
#define  DEADRECKONING_DELIVERY             10    /* Number of delivery destinations        */
/* Vehicle sensor information      */
#define  DEADRECKONING_DELIVERY_INFO        (DEADRECKONING_SIGNAL)
/* Maximum number of vehicle sensor information  */
#define  DEADRECKONING_DELIVERY_INFO_MAX    (DEADRECKONING_DELIVERY_INFO * DEADRECKONING_DELIVERY)
/* Vehicle Sensor Information Valid Number  */
#define  DEADRECKONING_DID_EFFECTIVE        (DEADRECKONING_SIGNAL)
#define  DEADRECKONING_ACTION_TYPE_ADD      0    /* Vehicle sensor addition processing    */
#define  DEADRECKONING_ACTION_TYPE_UPDATE   1    /* Vehicle sensor update processing    */

#define  DEADRECKONING_LINK_INDEX_END       0xFFFF  /* End of the link index  */

#define  DEADRECKONING_DELIVERY_METHOD_NORMAL  0    /* Delivery system normal delivery    */

/************************************************************************
*      Struct definitions                          *
************************************************************************/

/*********************************************************************
* TAG    :DEADRECKONING_DELIVERY_CTRL_TBL_DATA
* ABSTRACT  : Structure of each data of the vehicle sensor delivery destination management table
***********************************************************************/
typedef struct {
    DID      ul_did;          /* Data ID        */
    PNO      us_pno;          /* Shipping PID      */
    u_int8   uc_chg_type;      /* Delivery timing      */
    u_int8   uc_ctrl_flg;     /* Delivery operation        */
    u_int16  us_link_idx;      /* Link index  */
    u_int8   uc_method;       /* Delivery system        */
    u_int8   uc_reserve;      /* reserve        */
} DEADRECKONING_DELIVERY_CTRL_TBL_DATA;

/*********************************************************************
* TAG    : DEADRECKONING_DELIVERY_CTRL_TBL
* ABSTRACT  : Vehicle Sensor Delivery Destination Management Table Structure
***********************************************************************/
typedef struct {
    u_int16  us_num;         /* Number of delivery destination management data items  */
    u_int8   uc_reserve[2];   /* Reserved        */
    /* Array of each data    */
    DEADRECKONING_DELIVERY_CTRL_TBL_DATA  st_ctrl_data[DEADRECKONING_DELIVERY_INFO_MAX];
} DEADRECKONING_DELIVERY_CTRL_TBL;

/*********************************************************************
* TAG    : DEADRECKONING_DELIVERY_CTRL_TBL_MNG_DATA
* ABSTRACT  : Structure of each data of Vehicle Sensor Destination Management Table Management
***********************************************************************/
typedef struct {
    DID      ul_did;             /* Data ID        */
    u_int16  us_start_idx;        /* Start index  */
    u_int16  us_end_idx;          /* End index  */
    u_int16  us_dlvry_entry_num;   /* Number of registered shipping addresses    */
    u_int8   uc_reserve[2];      /* Reserved        */
} DEADRECKONING_DELIVERY_CTRL_TBL_MNG_DATA;

/*********************************************************************
* TAG    : DEADRECKONING_DELIVERY_CTRL_TBL_MNG
* ABSTRACT  : Structure of Vehicle Sensor Delivery Destination Management Table Management
***********************************************************************/
typedef struct {
    u_int16    us_num;      /* Number of data items      */
    u_int8     uc_reserve[2];  /* Reserved        */
    /* Array of each data    */
    DEADRECKONING_DELIVERY_CTRL_TBL_MNG_DATA   st_ctrl_tbl_mng_data[DEADRECKONING_DELIVERY_INFO];
} DEADRECKONING_DELIVERY_CTRL_TBL_MNG;

/*********************************************************************
* TAG    : DEADRECKONING_DELIVERY_PNO_TBL
* ABSTRACT  : Vehicle Sensor Destination PNO Table
***********************************************************************/
typedef struct {
    PNO        us_pno;        /* Thread ID  */
    u_int16    us_index;      /* Appropriate INDEX in the delivery control table      */
    u_int8     uc_method;      /* Delivery system                */
    u_int8     uc_reserve[3];    /* reserve    */
} DEADRECKONING_DELIVERY_PNO_TBL_DAT;

typedef struct {
    u_int16    us_num;        /* Number of data items  */
    u_int8     uc_reserve[2];    /* reserve    */
    DEADRECKONING_DELIVERY_PNO_TBL_DAT  st_pno_data[DEADRECKONING_DELIVERY_INFO_MAX];
} DEADRECKONING_DELIVERY_PNO_TBL;

/************************************************************************
*      Function prototype                        *
************************************************************************/

DEADRECKONING_DELIVERY_PNO_TBL* DeadReckoningMakeDeliveryPnoTbl(DID ul_did, u_int8 change_type);

DEAD_RECKONING_RET_API DeadReckoningEntryDeliveryCtrl(const DEADRECKONING_MSG_DELIVERY_ENTRY *);

void DeadReckoningInitDeliveryCtrlTbl(void);
void DeadReckoningInitDeliveryCtrlTblMng(void);

void DeadReckoningAddDeliveryCtrlTbl(const DEADRECKONING_MSG_DELIVERY_ENTRY *);
void DeadReckoningUpdateDeliveryCtrlTbl(DEADRECKONING_DELIVERY_CTRL_TBL_MNG_DATA *);
void DeadReckoningAddDeliveryCtrlTblMng(const DEADRECKONING_MSG_DELIVERY_ENTRY *);
void DeadReckoningUpdateDeliveryCtrlTblMng(DEADRECKONING_DELIVERY_CTRL_TBL_MNG_DATA *);

void DeadReckoningAddPnoTbl(u_int16);
void DeadReckoningDeliveryProc(DID ul_did, u_int8 uc_chg_type, u_int8 uc_get_method);
void DeadReckoningFirstDelivery(PNO us_pno, DID ul_did);

RET_API  DRManagerSndMsg(PNO us_pno_src, PNO us_pno_dest, CID us_cid, u_int16 us_msg_len, const void *p_msg_data);

#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_DEADRECKONING_DELIVERYCTRL_H_
