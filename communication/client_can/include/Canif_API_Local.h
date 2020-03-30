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

/****************************************************************************
 * FILE		Canif_API_Local.h
 * SYSTEM	:_CWORD107_
 * SUBSYSTEM	:Local header for CAN I/F provided APIs
 * TITLE	:Data header files needed to use the CAN I/F API
 ****************************************************************************/
#ifndef COMMUNICATION_CLIENT_CAN_INCLUDE_CANIF_API_LOCAL_H_
#define COMMUNICATION_CLIENT_CAN_INCLUDE_CANIF_API_LOCAL_H_

/************************************************************************
*			Definition													*
************************************************************************/

/* CANDataControl code used in the transmission/reception relationship */
#define CAN_ID_MASK_CODE 0xE0000000UL  // CAN ID Mask Codes

/*********************************************************************
*                       Defining Return Values
***********************************************************************/
/* Defined in Canif_API.h */

/*********************************************************************
*                       Defining Event Generation
***********************************************************************/
#define CANIF_API_EVT_INIT 0                               /* Initial value of the event */
#define CANIF_API_ENT_EVT_MIN CANIF_RET_NORMAL             /* Minimum Event Wait */
#define CANIF_API_ENT_EVT_MAX CANIF_RET_ERROR_CREATE_EVENT /* Maximum value waiting for an event */

/*********************************************************************
*                       Type definition
***********************************************************************/

/***********************************************************************
*                 CANIF API Functions Prototypes                 *
************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/**
* \~english  Data struct used to transfer CAN data(_CWORD29_)
* \~english  (transfer data from CANIF API to CAN)
*/
typedef struct {
  /**
  * \~english  Delivery target thread name
  */
  char notifyName[MAX_NAME_SIZE_APP];
  /**
  * \~english  Resource ID
  */
  uint8_t ucRid;
  /**
  * \~english  Data size
  */
  uint32_t ulDataSize;
  /**
  * \~english  OPC
  */
  uint16_t  opc;
  /**
  * \~english  Data buffer
  */
  uint8_t  data[CAN__CWORD29__MEM_SZ];
} CAN__CWORD29__TRANS_MSG;

/* The most significant part is defined in Canif_API.h. */
CANIF_RET_API CanifDeliveryEntryCore(HANDLE h_app, PCSTR notify_name, uint8_t can_num, CANID *p_can_id, uint16_t cid);
CANIF_RET_API Canif_CWORD29_DeliveryEntryCore(const HANDLE h_app, const PCSTR notify_name,
                                           const uint8_t opc_num, const uint16_t* const p_opc, const uint16_t cid);
CANIF_RET_API CanifTransmissionStartCore(HANDLE h_app, PCSTR notify_name, uint8_t rid,
                                         uint16_t freq, CAN_DATA *p_data, uint16_t cid);
CANIF_RET_API CanifTransmissionStopCore(HANDLE h_app, PCSTR notify_name, CANID can_id, uint16_t cid);
CANIF_RET_API CanifCommandCtrlCore(HANDLE h_app, PCSTR notify_name, uint8_t rid, uint32_t cmd_id, uint16_t cid);
CANIF_RET_API CanifCommWatchCore(HANDLE h_app, PCSTR notify_name, CANID can_id, DID did,
                                 uint8_t ig_cooperation, uint16_t watch_time, uint16_t cid);

void CanifMsgHeaderGenerate(CAN_MSG_DATA *, uint16_t, uint16_t, uint16_t); /* Private Functions */
CANIF_RET_API CanifSndMsg(CAN_MSG_DATA *);                                  /* Private Functions */

CANIF_RET_API CanifCommWatchExtSndMsgData(uint16_t us_pid, CANID ul_canid, DID ul_did,
                                          uint8_t uc_ig, uint16_t us_watch_time); /* Private Functions */

#if defined(__cplusplus)
}
#endif
#endif  // COMMUNICATION_CLIENT_CAN_INCLUDE_CANIF_API_LOCAL_H_
