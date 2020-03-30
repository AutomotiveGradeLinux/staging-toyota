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

#ifndef COMMUNICATION_SERVER_INCLUDE_CAN_DELIVERY_CAN_DELIVERYDATA_H_
#define COMMUNICATION_SERVER_INCLUDE_CAN_DELIVERY_CAN_DELIVERYDATA_H_
/******************************************************************************
 * FILE  :CAN_DeliveryData.h
 * SYSTEM  :_CWORD107_
 * SUBSYSTEM :
-----------------------------------------------------------------------------*/
#include <native_service/frameworkunified_types.h>
#include <peripheral_service/Canif_API.h>

/************************************************************************
*   Macro definitions             *
************************************************************************/
/* Data size relationship */
#define CAN_DELIVERY_LIST_NUM 400                                               /* Maximum number of delivery destination management tables */
#define CAN_DELIVERY_CANID_LIST_NUM 150 /* Delivery CAN ID control table max. */

#define CAN_EXRCV_DATA_NUM 22   /* CAN Extended Reception Notification CAN Maximum Number of Data */
#define CAN_EXRCV_DATA_SIZE 11  /* CAN Extended Reception Notification CAN Data Size */
#define CAN_EXRCV_CANNUM_SIZE 1 /* CAN Extended Reception Notification CAN Data Number Size */
#define CAN_EXRCV_DLC_MIN 1     /* CAN Extended Reception Notification DLC Minimum Value */
#define CAN_EXRCV_DLC_MAX 8     /* CAN Extended Reception Notification DLC Maximum Value */

/* Flag relationship */
#define CAN_DELIVERY_OFF 0 /* Stopping data delivery */
#define CAN_DELIVERY_ON 1  /* Data delivery in progress */

/* CANDataControl code relationship used in the delivery relationship */
#define CAN_DELIVERYLIST_STOP_CODE 0xFFFF /* Shipping Destination Management Table Stop Code */

#define CAN_CANIDLIST_EMPTY 0xFFFF /* Availability of CAN ID control table */
/************************************************************************
*   Struct definitions
************************************************************************/
/***************************************************
* TAG       : CAN_DELIVERY_DAT
* ABSTRACT  : Destination management table structure (1 item)
*             (CAN internal data management table)
****************************************************/
typedef struct {                      /* Type definition of delivery destination management data (1 item) */
  CANID ul_canid;                      /* CAN ID */
  char notify_name[MAX_NAME_SIZE_APP]; /* Destination thread name */
                                      // uint32_t  notifyId;  /* Addresses for delivery ID */
  uint8_t uc_delivery_on;               /* Delivery operation */
  uint8_t reserve1;                   /* Reserved */
  uint16_t us_link_id;                  /* Link ID */
  uint8_t reserve2[2];                /* Reserved */
} CAN_DELIVERY_DAT;

/***************************************************
* TAG       : CAN_DELIVERY_LIST_DAT
* ABSTRACT  : Destination management table structure (all)
*             (CAN internal data management table)
****************************************************/
typedef struct {                                  /* Type definition of the shipping management table */
  uint16_t us_entry_num;                            /* Registered number */
  uint8_t reserve[2];                             /* Reserved */
  CAN_DELIVERY_DAT st_list[CAN_DELIVERY_LIST_NUM]; /* Delivery destination management data */
} CAN_DELIVERY_LIST_DAT;

/***************************************************
* TAG       : CAN_DELIVERY_SPACE_DAT
* ABSTRACT  : Free space management structure in the destination management table (all)
*             (CAN internal data management table free space management table)
****************************************************/
typedef struct {                             /* Type definitions for free space management tables */
  uint16_t space_num;                         /* Number of free spaces */
  uint16_t index_list[CAN_DELIVERY_LIST_NUM]; /* Free space index list */
} CAN_DELIVERY_SPACE_DAT;

/***************************************************
* TAG       : CAN_CANID_DAT
* ABSTRACT  : Transport CAN ID control table structures (1 item)
*             (CAN internal data management table)
****************************************************/
typedef struct {      /* Defining the type of delivery destination CAN ID (1 item) */
  CANID ul_canid;      /* CAN ID */
  uint16_t us_start_id; /* Start ID */
  uint16_t us_end_id;   /* End ID */
  uint16_t us_data_num; /* Number of data items */
  uint8_t reserve[2]; /* Reserved */
} CAN_CANID_DAT;

/***************************************************
* TAG       : CAN_CANID_LIST_DAT
* ABSTRACT  : Delivery CAN ID control table structures (all)
*             (CAN internal data management table)
****************************************************/
typedef struct {                                     /* Type definitions for the target CAN ID administration table */
  uint16_t us_entry_num;                               /* Registered number */
  uint8_t reserve[2];                                /* Reserved */
  CAN_CANID_DAT st_list[CAN_DELIVERY_CANID_LIST_NUM]; /* Shipping CAN ID */
} CAN_CANID_LIST_DAT;

/***************************************************
* TAG       : CAN_DELIVERY_SND_DAT
* ABSTRACT  : Destination thread name for sending a message of delivery data
*             (CAN-internal Work)
****************************************************/
/* Type definition of the target thread name list data to which the message is sent */
typedef struct {
  int32_t i_num;                                              /* Number of messages sent */
  char notify_name[CAN_DELIVERY_LIST_NUM][MAX_NAME_SIZE_APP]; /* Destination thread name */
} CAN_DELIVERY_SND_DAT;

typedef struct {
  CAN_DELIVERY_LIST_DAT* p_dlvry_list;
  CAN_DELIVERY_SPACE_DAT* p_dlvry_space;
  CAN_CANID_LIST_DAT* p_canid_list;
  CAN_DELIVERY_SND_DAT* p_dlvry_snd;
} CAN_STRUCT_PTR;

/************************************************************************
*   Function prototype             *
************************************************************************/
void CANDeliveryDataInit(void);          /* CANDataDelivery Management Data Initialization Process */
EFrameworkunifiedStatus CANDeliveryEntry(HANDLE h_app); /* CANDataDelivery registration process */
EFrameworkunifiedStatus CAN_CWORD29_DeliveryEntry(HANDLE h_app);  /* Process of registering the delivery of _CWORD29_ data */
void CANDeliveryBufferOut(FILE* fp_log);   /* CAN shipping table log output processing */
#ifdef CAN_DEBUG
EFrameworkunifiedStatus CANAllDeleteDeliveryEntry(HANDLE h_app);
#endif

#endif  // COMMUNICATION_SERVER_INCLUDE_CAN_DELIVERY_CAN_DELIVERYDATA_H_
