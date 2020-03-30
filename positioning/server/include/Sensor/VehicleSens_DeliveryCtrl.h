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

#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_DELIVERYCTRL_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_DELIVERYCTRL_H_
/******************************************************************************
 * File name        :VehicleSens_DeliveryCtrl.h
 * System name        :_CWORD107_
 * Subsystem name    :
 ******************************************************************************/

#include "Vehicle_API_Dummy.h"
#include "Sensor_API.h"
#include "Vehicle_API_private.h"
#include "Sensor_API_private.h"
#include "VehicleSens_SelectionItemList.h"
#include "VehicleSens_DataMaster.h"
#include "Dead_Reckoning_Local_Api.h"

/************************************************************************
*            Macro definitions                                                    *
************************************************************************/
#define    VEHICLESENS_CANID_EFFECTIVE        29        /* CAN ID Effective number   */
#define    VEHICLESENS_CANID_RESERVE        11        /* CAN ID Reserved number  */
#define    VEHICLESENS_SIGNAL                VEHICLESENS_SELECTION_ITEM_LIST_LEN    /* Number of vehicle signals  */
#define    VEHICLESENS_DELIVERY            10        /* Number of delivery destinations                */
/* CAN ID Maximum number            */
#define    VEHICLESENS_CANID_MAX            (VEHICLESENS_CANID_EFFECTIVE + VEHICLESENS_CANID_RESERVE)
/* Vehicle sensor information            */
#define    VEHICLESENS_DELIVERY_INFO        (VEHICLESENS_CANID_MAX + VEHICLESENS_SIGNAL)
/* Maximum number of vehicle sensor information    */
#define    VEHICLESENS_DELIVERY_INFO_MAX    (VEHICLESENS_DELIVERY_INFO * VEHICLESENS_DELIVERY)
/* Vehicle Sensor Information Valid Number    */
#define    VEHICLESENS_DID_EFFECTIVE        (VEHICLESENS_CANID_EFFECTIVE + VEHICLESENS_SIGNAL)
#define    VEHICLESENS_ACTION_TYPE_ADD        0        /* Vehicle sensor addition processing        */
#define    VEHICLESENS_ACTION_TYPE_UPDATE    1        /* Vehicle sensor update processing        */
#define    VEHICLESENS_LINK_INDEX_END        0xFFFF    /* End of the link index    */


#define    VEHICLESENS_PKG_DELIVERY_INFO_MAX    2560    /* Maximum number of vehicle sensor information  */
#define    VEHICLESENS_DELIVERY_METHOD_NORMAL    0        /* Delivery system normal delivery    */
#define    VEHICLESENS_DELIVERY_METHOD_PACKAGE    1        /* Delivery system package delivery */
#define    VEHICLESENS_PKG_EXT_SEND_MAX        10        /* Number of data master transmissions/onece  */
#define    VEHICLESENS_PKG_EXT_SEND_MAX_10DATA    100        /* Number of data masters transmitted (GYRO),SPEED)/once */

#define    VEHICLESENS_DELIVERY_MAX_SIZE            SENSOR_MSG_VSINFO_DSIZE        /* Maximum Size of Delivery       */
#define    VEHICLESENS_DELIVERY_FSTSNS_HDR_SIZE    8        /* Initial Sensor Data Delivery Data Header Size    */

/************************************************************************
*            Struct definitions                                                    *
************************************************************************/
/*********************************************************************
* TAG       : VEHICLESENS_DELIVERY_LIST_CANID
* ABSTRACT  : CAN data delivery registration request information
***********************************************************************/
typedef struct {
    u_int8    uc_can_num;                            /* CANID delivery registrations */
    u_int8    uc_reserve[3];
    u_int32    ul_can_id[VEHICLESENS_CANID_MAX];        /* Delivery registrationCANID */ /* CANIF_API deletion */
} VEHICLESENS_DELIVERY_LIST_CANID;

/*********************************************************************
* TAG       :VEHICLESENS_DELIVERY_CTRL_TBL_DATA
* ABSTRACT  : Structure of each data of the vehicle sensor delivery destination management table
***********************************************************************/
typedef struct {
    DID        ul_did;                /* Data ID                */
    PNO        us_pno;                /* Shipping PID            */
    u_int8    uc_chg_type;            /* Delivery timing            */
    u_int8    uc_ctrl_flg;            /* Delivery operation                */
    u_int16    us_link_idx;            /* Link index    */
    u_int16    us_pkg_start_idx;        /* Package Delivery Start Index    */
    u_int16    us_pkg_end_idx;        /* Package delivery end index        */
    u_int8    uc_method;            /* Delivery system                */
    u_int8    uc_reserve;            /* reserve                */
    /* Modify to store the destination service name TODO */
    /* Add handles as needed TODO */
} VEHICLESENS_DELIVERY_CTRL_TBL_DATA;

/*********************************************************************
* TAG       : VEHICLESENS_DELIVERY_CTRL_TBL
* ABSTRACT  : Vehicle Sensor Delivery Destination Management Table Structure
***********************************************************************/
typedef struct {
    u_int16        us_dnum;            /* Number of delivery destination management data items    */
    u_int8        uc_reserve[2];    /* Reserved                */
    /* Array of each data        */
    VEHICLESENS_DELIVERY_CTRL_TBL_DATA    st_ctrl_data[VEHICLESENS_DELIVERY_INFO_MAX];
} VEHICLESENS_DELIVERY_CTRL_TBL;

/*********************************************************************
* TAG       : VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA
* ABSTRACT  : Structure of each data of Vehicle Sensor Destination Management Table Management
***********************************************************************/
typedef struct {
    DID        ul_did;                    /* Data ID                */
    u_int16    us_start_idx;                /* Start index    */
    u_int16    us_end_idx;                /* End index    */
    u_int16    usdlvry_entry_num;        /* Number of registered shipping addresses        */
    u_int8    uc_reserve[2];            /* Reserved                */
} VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA;

/*********************************************************************
* TAG       : VEHICLESENS_DELIVERY_CTRL_TBL_MNG
* ABSTRACT  : Structure of Vehicle Sensor Delivery Destination Management Table Management
***********************************************************************/
typedef struct {
    u_int16        us_dnum;            /* Number of data items            */
    u_int8        uc_reserve[2];    /* Reserved                */
    /* Array of each data        */
    VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA     st_ctrl_tbl_mng_data[VEHICLESENS_DELIVERY_INFO];
} VEHICLESENS_DELIVERY_CTRL_TBL_MNG;

/*********************************************************************
* TAG       : VEHICLESENS_PKG_DELIVERY_TBL_MNG_DATA
* ABSTRACT  : Structure of each data of the vehicle sensor package delivery management table
***********************************************************************/
typedef struct {
    DID        ul_did;                    /* Data ID                    */
    u_int16    usdlvry_idx;                /* Delivery data index    */
    u_int8    uc_reserve[2];            /* Reserved                    */
} VEHICLESENS_PKG_DELIVERY_TBL_MNG_DATA;


/*********************************************************************
* TAG       : VEHICLESENS_PKG_DELIVERY_TBL_MNG
* ABSTRACT  : Structure of Vehicle Sensor Package Delivery Management Table
***********************************************************************/
typedef struct {
    u_int16        us_dnum;            /* Number of data items            */
    u_int8        uc_reserve[2];    /* Reserved                */
    /* Array of each data        */
    VEHICLESENS_PKG_DELIVERY_TBL_MNG_DATA     st_pkg_data[VEHICLESENS_PKG_DELIVERY_INFO_MAX];
} VEHICLESENS_PKG_DELIVERY_TBL_MNG;

/*********************************************************************
* TAG       : VEHICLESENS_DELIVERY_PNO_TBL
* ABSTRACT  : Vehicle Sensor Destination PNO Table
***********************************************************************/
typedef struct {
    PNO        us_pno;                /* Thread ID    */
    u_int16    us_pkg_start_idx;        /* Package Delivery Start Index    */
    u_int16    us_pkg_end_idx;        /* Package delivery end index        */
    u_int8    uc_method;            /* Delivery system                                */
    u_int8    uc_reserve;            /* reserve                                */
} VEHICLESENS_DELIVERY_PNO_TBL_DAT;

typedef struct {
    u_int16    us_dnum;                /* Number of data items    */
    u_int8    uc_reserve[2];        /* reserve        */
    VEHICLESENS_DELIVERY_PNO_TBL_DAT    st_pno_data[VEHICLESENS_DELIVERY_INFO_MAX];
} VEHICLESENS_DELIVERY_PNO_TBL;

/*********************************************************************
* TAG       : VEHICLESENS_DELIVERY_HEADER
* ABSTRACT  : Delivery data header
***********************************************************************/
typedef struct {
    DID        did;                                        /* Data ID            */
    u_int16    size;                                        /* Size of the data    */
    u_int8    rcv_flag;                                    /* Receive flag        */
    u_int8    sensor_cnt;                                    /* Sensor Counter    */
} VEHICLESENS_DELIVERY_HEADER;

/*********************************************************************
* TAG       : VEHICLESENS_DELIVERY_FORMAT
* ABSTRACT  : Delivery data format
***********************************************************************/
typedef struct {
    VEHICLESENS_DELIVERY_HEADER        header;                                    /* Header            */
    u_int8                            data[VEHICLESENS_DELIVERY_MAX_SIZE];    /* Data            */
} VEHICLESENS_DELIVERY_FORMAT;

/************************************************************************
*            Function prototype                                              *
************************************************************************/
void VehicleSensInitDeliveryCtrlTbl(void);
void VehicleSensInitDeliveryCtrlTblMng(void);
void VehicleSensInitPkgDeliveryTblMng(void);
VEHICLE_RET_API VehicleSensEntryDeliveryCtrl(const VEHICLE_MSG_DELIVERY_ENTRY *);
void VehicleSensAddDeliveryCtrlTbl(const VEHICLE_MSG_DELIVERY_ENTRY *);
void VehicleSensUpdateDeliveryCtrlTbl(VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA *);
void VehicleSensUpdatePkgDeliveryCtrlTbl(u_int16, u_int16);
void VehicleSensAddDeliveryCtrlTblMng(const VEHICLE_MSG_DELIVERY_ENTRY *);
void VehicleSensUpdateDeliveryCtrlTblMng(VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA *);
void VehicleSensAddPkgDeliveryTblMng(const SENSOR_MSG_DELIVERY_ENTRY *);
VEHICLE_RET_API VehicleSensEntryPkgDeliveryCtrl(const SENSOR_MSG_DELIVERY_ENTRY *, u_int8 uc_ext_chk);
VEHICLESENS_DELIVERY_PNO_TBL* VehicleSensMakeDeliveryPnoTbl(DID ul_did, u_int8 change_type);
void VehicleSensAddPnoTbl(u_int16);
u_int8 VehicleSensDeliveryGPS(DID ul_did, u_int8 uc_get_method, u_int8 uc_current_get_method, int32 pno_index,
                            u_int32* cid, VEHICLESENS_DATA_MASTER* stmaster,
                            const VEHICLESENS_DELIVERY_PNO_TBL *pst_pno_tbl);
u_int8 VehicleSensDeliveryFst(DID ul_did, u_int8 uc_get_method, int32 pno_index,
                            const VEHICLESENS_DELIVERY_PNO_TBL *pst_pno_tbl);
u_int8 VehicleSensDeliveryGyro(DID ul_did, u_int8 uc_current_get_method, int32 pno_index,
                            const VEHICLESENS_DELIVERY_PNO_TBL *pst_pno_tbl);
void VehicleSensDeliveryAntenna(DID ul_did, u_int8 uc_current_get_method, int32 pno_index,
                            const VEHICLESENS_DELIVERY_PNO_TBL *pst_pno_tbl);
u_int8 VehicleSensDeliveryOther(DID ul_did, u_int8 uc_current_get_method, int32 pno_index,
                            u_int32* cid,
                            VEHICLESENS_DATA_MASTER* stmaster,
                            const VEHICLESENS_DELIVERY_PNO_TBL *pst_pno_tbl);
void VehicleSensDeliveryProc(DID ul_did, u_int8 uc_chg_type, u_int8 uc_get_method);
//u_int8 VehicleSensFirstDeliverySens(PNO us_pno, DID ul_did, u_int8 uc_get_method,
//                                     VEHICLESENS_DATA_MASTER_FST* stmaster_fst,
//                                     VEHICLESENS_DATA_MASTER_FST* stmaster_fst_temp);
u_int8 VehicleSensFirstDeliveryOther(PNO us_pno, DID ul_did, u_int8 uc_get_method,
                                    u_int32* cid,
                                    VEHICLESENS_DATA_MASTER* stmaster);
void VehicleSensFirstDelivery(PNO us_pno, DID ul_did);
void VehicleSensFirstPkgDelivery(const SENSOR_MSG_DELIVERY_ENTRY_DAT *);
RET_API    VehicleSensCanDeliveryEntry(void);
void VehicleSensFirstPkgDelivery(const SENSOR_MSG_DELIVERY_ENTRY_DAT *);

#if CONFIG_SENSOR_EXT_VALID                /* Initial Sensor Support */
void VehicleSensFirstPkgDeliveryExt(const SENSOR_MSG_DELIVERY_ENTRY_DAT *pst_data);
#endif

#if CONFIG_HW_PORTSET_TYPE_C
void VehicleSensInitSeqNum(void);
void VehicleSensDivideDeliveryProc(PNO, const void *);
#endif

/* ++ PastModel002 support DR */
void VehicleSensInitDeliveryCtrlTblDR(void);
void VehicleSensInitDeliveryCtrlTblMngDR(void);
VEHICLE_RET_API VehicleSensEntryDeliveryCtrlDR(const DEADRECKONING_MSG_DELIVERY_ENTRY *);
void VehicleSensAddDeliveryCtrlTblDR(const DEADRECKONING_MSG_DELIVERY_ENTRY *);
void VehicleSensAddDeliveryCtrlTblMngDR(const DEADRECKONING_MSG_DELIVERY_ENTRY *);
void VehicleSensUpdateDeliveryCtrlTblDR(VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA *);
void VehicleSensUpdateDeliveryCtrlTblMngDR(VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA *);
void VehicleSensDeliveryProcDR(DID ul_did, u_int8 uc_chg_type, u_int8 uc_get_method);
VEHICLESENS_DELIVERY_PNO_TBL* VehicleSensMakeDeliveryPnoTblDR(DID ul_did, u_int8 change_type);
void VehicleSensAddPnoTblDR(u_int16 us_index);
void VehicleSensGetDebugDeliveryCtrlTbl(void* pbuf);
void VehicleSensGetDebugDeliveryCtrlTblMng(void* pbuf);
void VehicleSensGetDebugPkgDeliveryTblMng(void* pbuf);
void VehicleSensGetDebugDeliveryPnoTbl(void* pbuf);
/* -- PastModel002 supprt DR */
#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_DELIVERYCTRL_H_
