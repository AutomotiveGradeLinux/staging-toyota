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

/****************************************************************************
 * FILE    Canif_API.h
 * TITLE   :Data header files needed to use the CAN I/F API
 ****************************************************************************/

#ifndef COMMUNICATION_CLIENT_CAN_INCLUDE_PERIPHERAL_SERVICE_CANIF_API_H_
#define COMMUNICATION_CLIENT_CAN_INCLUDE_PERIPHERAL_SERVICE_CANIF_API_H_

/**
 * @file Canif_API.h
 * @~english
 * @brief Canif API header
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup peripheral_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup communication
 *  @ingroup peripheral_service
 *  @{
 */
/** @addtogroup CAN
 *  @ingroup communication
 *  @{
 */

#include <native_service/frameworkunified_types.h>
#include <stdio.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <stub/com_message_header.h>

/**
 * \~english  Defination of Service name
 */
#define LAN_SERVICE_CAN "CAN_COM_PROT"

/**
 * \~english  Availability notification of CAN
 */
#define NTFY_Communication_CAN_ISAVAILABLE "Communication/CanIsAvailable"

/*********************************************************************
*                       Defining Return Values
***********************************************************************/
/* Normal system */
/**
 * \~english  Suceess
 */
#define CANIF_RET_NORMAL 1
/* Abnormal system */
/**
 * \~english  Parameter error
 */
#define CANIF_RET_ERROR_PARAM 2
/**
 * \~english  Buffer Full
 */
#define CANIF_RET_ERROR_BUFFULL 3
/**
 * \~english  CAN ID FULL
 */
#define CANIF_RET_ERROR_CANIDFULL 4
/**
 * \~english  ID unregister
 */
#define CANIF_RET_ERROR_UNDEF 5
/**
 * \~english  Thread ID unmatched
 */
#define CANIF_RET_ERROR_PID 6
/**
 * \~english  Timer error
 */
#define CANIF_RET_ERROR_TIMER 7
/**
 * \~english  Event Creat Error
 */
#define CANIF_RET_ERROR_CREATE_EVENT 8
/**
 * \~english  Error Cancel
 */
#define CANIF_RET_ERROR_CANCEL 9
/**
 * \~english  Null Pointer
 */
#define CANIF_RET_ERROR_NULL 10

/**
 * \~english  Flags for specifiy Delivery is echoback or not.
 */
enum CanIfEchoBackFlags {
  /**
  * \~english  Delivery is recv data
  */
  CANIF_PURERECV = 0,
  /**
  * \~english  Delivery is echo data
  */
  CANIF_ECHOBACK = 1
};

/**
 * \~english  Defination of Command ID ( CAN -> user )
 */
typedef enum _PS_CommunicationProtocol {
  /**
  * \~english  Delivery CAN Data
  */
  CID_CAN_DATA_DELIVERY = 0x0401,
  /**
  * \~english  CAN communication stop
  */
  CID_CAN_COMM_STOP,
  /**
  * \~english  CAN communication restore
  */
  CID_CAN_COMM_RESTORE,
  /**
  * \~english  CAN transmission result
  */
  CID_CAN_TX_RESULT,
  /**
  * \~english  Delivery CAN command
  */
  CID_CAN_CMD_DELIVERY,
  /**
  * \~english  CAN command transmission result
  */
  CID_CAN_CMD_TX_RESULT,
  /**
  * \~english  Delivery CAN data to CANGW
  */
  CID_CAN_CANGW_DELIVERY,
} PS_CommunicationProtocol;

/**
 * \~english  Defination of Command ID ( CANIF -> CAN )
 */
typedef enum _PS_Communication_InternalProtocol {
  /**
  * \~english  Delivery registration of CAN data 
  */
  CID_CANIF_DELIVERY_ENTRY = 0x0501,
  /**
  * \~english  Delivery deregistration of CAN data
  */
  CID_CANIF_DELIVERY_ERASE,
  /**
  * \~english  CAN data transmission start
  */
  CID_CANIF_TX_START,
  /**
  * \~english  CAN data communication stop watching 
  */
  CID_CANIF_COMM_WATCH,
  /**
  * \~english  CAN command control
  */
  CID_CANIF_CMD_CTRL,
  /**
  * \~english  CAN data transmission start
  */
  CID_CANIF_TX_BIT_START,
} PS_Communication_InternalProtocol;

/*********************************************************************
*                     Definitions of CAN command IDs
***********************************************************************/
/**
* \~english  Request CAN fuel calculator reset 
*/
#define CAN_CMDID_FUELCALC_RST_REQ_DELIVERY 0x00
/**
* \~english  Request CAN startup finished notification  
*/
#define CAN_CMDID_STARTUP_FIN_REQ_TX 0x01
/**
* \~english  Request CAN Master reset notification
*/
#define CAN_CMDID_MRST_INFO_REQ_TX 0x02
/**
* \~english  Request CAN version
*/
#define CAN_CMDID_VERSION_REQ_TX 0x03
/**
* \~english  Request CAN connected node notification
*/
#define CAN_CMDID_CONNECTION_NODE_REQ_TX 0x04
/**
* \~english  Request CAN BUS status notification
*   ToDo@Bus error I/F is not required when CanGetBusStatus() is deleted, so it is scheduled to be deleted.
*/
#define CAN_CMDID_BUS_STATUS_REQ_TX 0x05
/**
* \~english  Response of CAN fuel calculator reset
*/
#define CAN_CMDID_FUELCALC_REQ_TX 0x06
/**
* \~english  Receive CAN fuel calculator reset request
*/
#define CAN_CMDID_FUELCALC_RST_REQ_RX 0x07
/**
* \~english  Receive CAN startup finished notification
*/
#define CAN_CMDID_STARTUP_FIN_RESP_RX 0x08
/**
* \~english  Receive CAN Master reset notification
*/
#define CAN_CMDID_MRST_INFO_RESP_RX 0x09
/**
* \~english  Response of CAN version receive
*/
#define CAN_CMDID_VERSION_RESP_RX 0x0A
/**
* \~english  Response of CAN connection node receive
*/
#define CAN_CMDID_CONNECTION_NODE_RESP_RX 0x0B
/**
* \~english  Response of CAN bus status receive
*   ToDo@Bus error I/F is not required when CanGetBusStatus() is deleted, so it is scheduled to be deleted.
*/
#define CAN_CMDID_BUS_STATUS_RESP_RX 0x0C

/*********************************************************************
*                     Control Flag/Status Definition
***********************************************************************/
/* CANData Delivery control flag */
/**
* \~english  Delivery stop
*/
#define CAN_DELIVERY_STOP 0
/**
* \~english  Delivery restart
*/
#define CAN_DELIVARY_RESTART 1

/* CANData Transmission result notification status */
/**
* \~english  Success
*/
#define CAN_SUCCESS 0
/**
* \~english  Retryout
*/
#define CAN_RETRYOUT 1
/**
* \~english  Send buffer full
*/
#define CAN_BUFFERFUL 2

/* IG linkage type  */
/**
* \~english  IG cooperation off
*/
#define CAN_IG_COOPERATION_OFF 0
/**
* \~english  IG cooperation on
*/
#define CAN_IG_COOPERATION_ON 1

/* N_TA */
/**
* \~english  Invalid target address
*/
#define CAN_NTA_INVALID 0xFF

/* Others */
/**
* \~english  The code of resource ID which is not used 
*/
#define CAN_RID_NOTUSE_CODE 0xFF

/*********************************************************************
*                     Data size definition
***********************************************************************/
/**
* \~english  MAX CAN data size used to send/receive message
*/
#define CAN_DATA_SIZE 63
/**
* \~english  MAX CAN data size used to send message
*/
#define CAN_TXDATA_SIZE 8

/* Data size relationship */
/**
* \~english  MAX number of delivery entry
* \~english  ((528-16)-12)/4 = 125     12:event ID etc. , 4:size of CAN ID
*/
#define CAN_DELIVERY_CANID_ENTRY_MAX 125

/**
* \~english  OPC MAX number of delivery entry
*/
#define CAN_DELIVERY_OPC_ENTRY_MAX 255

/* Message size relationship */
/**
* \~english  MAX size of message buffer
*/
#define CAN_MSGBUF_MAX_SIZE 528
/**
* \~english  Size of message buffer header
*/
#define CAN_MSGBUF_HEADER_SIZE 16
/**
* \~english  MAX size of message buffer header
*/
#define CAN_MSGBUF_DATAMAX_SIZE (CAN_MSGBUF_MAX_SIZE - CAN_MSGBUF_HEADER_SIZE)

/**
* \~english  MAX size of CAN command data
*/
#define CANCMD_DAT_MAX 68

/**
* \~english  MAX number of CANGW transmission data
*/
#define CANGW_SND_NUM_MAX 0x16
/**
* \~english  MAX length of CANGW DLC
*/
#define CANGW_DLC_MAX_SIZE 0x08

/*********************************************************************
*                       Type definition
***********************************************************************/
/**
* \~english  CAN I/F API return code
*/
typedef int32_t CANIF_RET_API;
/**
* \~english  define of CAN ID type
*/
typedef uint32_t CANID;

/**
* \~english  data ID
*/
typedef uint32_t DID;

/**
* \~english  Data struct used to transfer CAN data
* \~english  (transfer data from CAN to user)
*/
typedef struct {
  /**
  * \~english  CAN ID
  */
  CANID can_id;
  /**
  * \~english  Data Length
  */
  uint8_t dlc;
  /**
  * \~english  reserve
  */
  uint8_t reserve[3];
  /**
  * \~english  DATA[0] ~ DATA[N]
  */
  uint8_t data[CAN_DATA_SIZE];
  /**
  * \~english  reserve
  */
  uint8_t reserve2;
} CAN_MSG_CANDATA_DAT;

/**
* \~english  Data struct used to transfer CAN message
* \~english  (transfer message from CAN to user)
*/
typedef struct {
  /**
  * \~english  header
  */
  T_APIMSG_MSGBUF_HEADER_COMM hdr;
  /**
  * \~english  data
  */
  CAN_MSG_CANDATA_DAT data;
  /**
  * \~english  echoback flag
  */
  enum CanIfEchoBackFlags echoback;
} CAN_MSG_CANDATA;

/**
* \~english  Data struct used to transfer CAN data to CANGW 
* \~english  (transfer data from CAN to user)
*/
typedef struct {
  /**
  * \~english  CAN ID (upper byte)
  */
  uint8_t can_id_high;
  /**
  * \~english  CAN ID (lower byte)
  */
  uint8_t can_id_low;
  /**
  * \~english  Data Length
  */
  uint8_t dlc;
  /**
  * \~english  data
  */
  uint8_t data[CANGW_DLC_MAX_SIZE];
} CAN_MSG_CANGWDATA_DAT;

/**
* \~english  Message struct used to transfer CAN data to CANGW
* \~english  (transfer message from CAN to user)
*/
typedef struct {
  /**
  * \~english  CAN data
  */
  CAN_MSG_CANGWDATA_DAT data[CANGW_SND_NUM_MAX];
  /**
  * \~english  Number of CAN data(max 22)
  */
  uint8_t num;
} CAN_MSG_CANGWDATA;

/**
* \~english  Data struct used to transfer CAN data
* \~english  (transfer data from user to CANIF API)
*/
typedef struct {
  /**
  * \~english  CAN ID
  */
  CANID can_id;
  /**
  * \~english  Data Length
  */
  uint8_t dlc;
  /**
  * \~english  reserve
  */
  uint8_t reserve[3];
  /**
  * \~english  DATA
  */
  uint8_t data[CAN_DATA_SIZE + 1];
} CAN_DATA;

/**
* \~english  Data struct used to transfer CAN data
* \~english  (transfer data from CANIF API to CAN)
*/
typedef struct {
  /**
  * \~english  Delivery target thread name
  */
  char notifyName[MAX_NAME_SIZE_APP];
  //  uint32_t  notifyId;    /* Addresses for delivery ID */
  /**
  * \~english  Thread ID used by _CWORD64_ 
  */
  uint32_t ulEventId;
  /**
  * \~english  Resource ID
  */
  uint8_t ucRid; /* Resources ID */
  /**
  * \~english  Cycle of send message(Unit of 100ms)
  */
  uint16_t usFreq;
  /**
  * \~english  CAN DATA
  */
  CAN_DATA stCandata;
} CAN_TRANSMISSION_START_MSG_DAT;

/**
* \~english  Data struct used to notify transmission result
* \~english  (transfer data from CAN API to user)
*/
typedef struct {
  /**
  * \~english  CAN ID
  */
  CANID ulCanid;
  /**
  * \~english  Transmission result status
  */
  uint8_t ucStatus;
  /**
  * \~english  Reserve
  */
  uint8_t reserve[3];
} CAN_MSG_SENDSTS_DAT;

/**
* \~english  Message struct used to notify transmission result
* \~english  (transfer message from CAN API to user)
*/
typedef struct {
  /**
  * \~english  header
  */
  T_APIMSG_MSGBUF_HEADER_COMM hdr;
  /**
  * \~english  Data
  */
  CAN_MSG_SENDSTS_DAT data;
} CAN_MSG_SENDSTS;

/**
* \~english  Data struct used for regular transmission stop
* \~english  (transfer data from CANIF API to CAN)
*/
typedef struct {
  /**
  * \~english  Delivery target thread name
  */
  char notifyName[MAX_NAME_SIZE_APP];
  //  uint32_t    notifyId;  /* Addresses for delivery ID */
  /**
  * \~english  Event ID that use for _CWORD64_
  */
  uint32_t ulEventId;
  /**
  * \~english  CAN ID
  */
  CANID ulCanid;
} CAN_FREQ_TRANS_STOP_MSG_DAT;

/**
* \~english  Data struct used for communication stop/restore
* \~english  (transfer message from CAN to user)
*/
typedef struct {
  /**
  * \~english  Data ID
  */
  DID ulDid;
} CAN_MSG_COMM_WATCHSTS_DAT;

/**
* \~english  Data struct used for communication stop/restore
* \~english  (transfer message from CAN to user)
*/
typedef struct {
  /**
  * \~english  Header
  */
  T_APIMSG_MSGBUF_HEADER_COMM hdr;
  /**
  * \~english  Data
  */
  CAN_MSG_COMM_WATCHSTS_DAT data;
} CAN_MSG_COMM_WATCHSTS;

/**
* \~english  Data struct used for delivery entry
* \~english  (transfer data from CANIF API to CAN)
*/
typedef struct {
  /**
  * \~english  Delivery target thread name
  */
  char notifyName[MAX_NAME_SIZE_APP];
  //  uint32_t    notifyId;        /* Addresses for delivery ID */
  /**
  * \~english  Event ID use for _CWORD64_
  */
  uint32_t ulEventId;
  /**
  * \~english  Number of CAN ID entry
  */
  uint16_t usCanNum;
  /**
  * \~english  CAN ID entry array
  */
  CANID ulCanid[CAN_DELIVERY_CANID_ENTRY_MAX];
} CAN_DELIVERY_ENTRY;

/**
* \~english  Data struct used for communication stop
* \~english  (transfer data from CANIF API to CAN)
*/
typedef struct {
  /**
  * \~english  Delivery target thread name
  */
  char notifyName[MAX_NAME_SIZE_APP]; /* Destination thread name */
  /**
  * \~english  Event ID
  */
  uint32_t ulEvtId;
  /**
  * \~english  CAN ID
  */
  CANID ulCanid;
  /**
  * \~english  Data ID
  */
  DID ulDid;
  /**
  * \~english  Watch time for commuication stop (Unit of 100ms)
  */
  uint16_t usWatchTime;
} CAN_COMM_WATCH_MSG_DAT;

/**
* \~english  Data struct used for CAN command control
* \~english  (transfer data from CANIF API to CAN)
*/
typedef struct {
  /**
  * \~english  Delivery target thread name
  */
  char notifyName[MAX_NAME_SIZE_APP];
  //  uint32_t  notifyId;  /* Transfer to: ID */
  /**
  * \~english  Event ID
  */
  uint32_t ulEvtId;
  /**
  * \~english  Resource ID
  */
  uint8_t ucRid;
  /**
  * \~english  CAN command ID
  */
  uint8_t ucCmdid;
} CAN_CMD_CTRL_MSG_DAT;

/**
* \~english  Data struct used for CAN command delivery
* \~english  (transfer data from CAN to user)
*/
typedef struct {
  /**
  * \~english  CAN command ID
  */
  uint8_t cmd_id;
  /**
  * \~english  Reserve
  */
  uint8_t reserve[3];
  /**
  * \~english  Data
  */
  uint8_t data[CANCMD_DAT_MAX];
} CAN_MSG_CANCMD_DAT;

/**
* \~english  Message struct used for CAN command delivery
* \~english  (transfer message from CAN to user)
*/
typedef struct {
  /**
  * \~english  Header
  */
  T_APIMSG_MSGBUF_HEADER_COMM hdr;
  /**
  * \~english  Data
  */
  CAN_MSG_CANCMD_DAT data;
} CAN_MSG_CANCMD;

/**
* \~english  Data struct used for CAN command transmission result
* \~english  (transfer data from CAN to user)
*/
typedef struct {
  /**
  * \~english  CAN command ID(same as delivery format)
  */
  uint32_t cmd_id;
  /**
  * \~english  Delivery result status
  */
  uint8_t status;
  /**
  * \~english  Reserve
  */
  uint8_t reserve[3];
} CAN_CMD_MSG_SENDSTS_DAT;

/**
* \~english  Data struct used for CAN command transmission result
* \~english  (transfer data from CAN to user)
*/
typedef struct {
  /**
  * \~english  Header
  */
  T_APIMSG_MSGBUF_HEADER_COMM hdr;
  /**
  * \~english  Data
  */
  CAN_CMD_MSG_SENDSTS_DAT data;
} CAN_CMD_MSG_SENDSTS;

/**
* \~english  Data struct for receiving message(work data)
*/
typedef struct {
  /**
  * \~english  Header
  */
  T_APIMSG_MSGBUF_HEADER_COMM stHead;
  /**
  * \~english  Data
  */
  uint8_t ucData[CAN_MSGBUF_DATAMAX_SIZE];
} CAN_MSG_DATA;

/**
* \~english  CAN data struct of communication stop registration
* \~english  (transfer data from CANIF API to CAN)
*/
typedef struct {
  /**
  * \~english  Delivery target thread name
  */
  char notifyName[MAX_NAME_SIZE_APP];
  /**
  * \~english  Event ID for _CWORD64_
  */
  uint32_t ulEvtId;
  /**
  * \~english  CAN ID
  */
  CANID ulCanid;
  /**
  * \~english  Data ID
  */
  DID ulDid;
  /**
  * \~english  Watch time for communication stop(Unit of 100ms)
  */
  uint16_t usWatchTime;
  /**
  * \~english  has IG coopration or not
  */
  uint8_t ucIg;
} CAN_COMM_WATCHEXT_MSG_DAT;

/**
* \~english  Mask data struct for CAN data
*/
typedef struct {
  /**
  * \~english  Mask data
  */
  uint8_t dat[CAN_TXDATA_SIZE];
} CAN_DATA_MASK;

/**
* \~english  Data struct used for CAN data transmission
*/
typedef struct {
  /**
  * \~english  Transmission data
  */
  uint8_t dat[CAN_TXDATA_SIZE];
} CAN_DATA_BIT;

/**
* \~english  Data struct used for CAN data transmission registration
*/
typedef struct {
  /**
  * \~english  CAN ID
  */
  CANID id;
  /**
  * \~english  Mask data
  */
  CAN_DATA_MASK mask;
  /**
  * \~english  Transmission data
  */
  CAN_DATA_BIT dat;
  /**
  * \~english  Cycle of regular transmission
  */
  uint32_t freq;
} CAN_TRANS_START_MSG_DAT;

/***********************************************************************
*                       CANIF API Functions Prototypes                        *
************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * MODULE    : Canif_DeliveryEntry
 ******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/// \ingroup Canif_DeliveryEntry
/// \~english @par Brief
///     Delivery registration of CAN data
/// \~english @param [in] h_app
///     HANDLE - Handle for application
/// \~english @param [in] notify_name
///     PCSTR - Delivery target thread name
/// \~english @param [in] can_num
///     uint8_t - Number of delivery registration CAN ID
/// \~english @param [in] p_can_id
///     CANID* - Pointer of delivery registration CAN ID array
/// \~english @retval CANIF_RET_NORMAL          Normality
/// \~english @retval CANIF_RET_ERROR_PARAM     Abnormality of parameter
/// \~english @retval CANIF_RET_ERROR_CANCEL    Abnormal termination
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///     - h_app is NULL.[CANIF_RET_ERROR_PARAM]
///     -notify_nameis NULL or the length ofnotify_nameis longer than 15 bytes.
///       [CANIF_RET_ERROR_PARAM]
///     - can_num is 0 or bigger than 125.[CANIF_RET_ERROR_PARAM]
///     - CAN ID pointer is null.[CANIF_RET_ERROR_PARAM]
///     - The upper 3bits of CAN ID are not 0.[CANIF_RET_ERROR_PARAM]
///     - malloc failed to get the message queue management information area
///       for transmission to communication service.  [CANIF_RET_ERROR_CANCEL]
///     - malloc failed to get the message queue name storage area for
///       transmission to communication service. [CANIF_RET_ERROR_CANCEL]
///     - mq_open failed to open the message queue for transmission
///       to communication service. [CANIF_RET_ERROR_CANCEL]
///     - The session message queue to communication service is full.
///       [CANIF_RET_ERROR_CANCEL]
///     - The transmission file descriptor of the session
///       message to communication service is invalid. [CANIF_RET_ERROR_CANCEL]
///     - The interruption by the system call (signal) occurred while
///       transmitting the session message to communication service.
///       [CANIF_RET_ERROR_CANCEL]
///     - Any error occurred in the transmission of the session message
///       to communication service. [CANIF_RET_ERROR_CANCEL]
///     - It failed to access to the shared memory for the transmission of
///       session message to communication service. [CANIF_RET_ERROR_CANCEL]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     - Delivery registration of CAN data. \n
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
CANIF_RET_API Canif_DeliveryEntry(HANDLE h_app, PCSTR notify_name,
                                  uint8_t can_num, CANID *p_can_id);

/*******************************************************************************
 * MODULE    : Canif_TransmissionStart
 ******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/// \ingroup Canif_TransmissionStart
/// \~english @par Brief
///     CAN data transmission starting
/// \~english @param [in] h_app
///     HANDLE - Handle for application
/// \~english @param [in] notify_name
///     PCSTR - Delivery target thread name
/// \~english @param [in] rid
///     uint8_t - Resource ID for CAN data transmission result notification
/// \~english @param [in] freq
///     uint16_t - Cycle of regular transmission
/// \~english @param [in] p_data
///     CAN_DATA* - Pointer of transmission data
/// \~english @retval CANIF_RET_NORMAL          Normality
/// \~english @retval CANIF_RET_ERROR_PARAM     Abnormality of parameter
/// \~english @retval CANIF_RET_ERROR_CANCEL    Abnormal termination
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///     - h_app is NULL.[CANIF_RET_ERROR_PARAM]
///     -notify_nameis NULL or the length ofnotify_nameis longer than 15 bytes.
///       [CANIF_RET_ERROR_PARAM]
///     - Transmission data pointer is null.[CANIF_RET_ERROR_PARAM]
///     - DLC size is bigger than 8 bytes.[CANIF_RET_ERROR_PARAM]
///     - The upper 3bits of CAN ID are not 0.[CANIF_RET_ERROR_PARAM]
///     - CAN ID is not defined in the _CWORD108_ bit asignment table
///       [CANIF_RET_ERROR_PARAM]
///     - malloc failed to get the message queue management information area
///       for transmission to communication service.  [CANIF_RET_ERROR_CANCEL]
///     - malloc failed to get the message queue name storage area for
///       transmission to communication service. [CANIF_RET_ERROR_CANCEL]
///     - mq_open failed to open the message queue for transmission
///       to communication service. [CANIF_RET_ERROR_CANCEL]
///     - The session message queue to communication service is full.
///       [CANIF_RET_ERROR_CANCEL]
///     - The transmission file descriptor of the session
///       message to communication service is invalid. [CANIF_RET_ERROR_CANCEL]
///     - The interruption by the system call (signal) occurred while
///       transmitting the session message to communication service.
///       [CANIF_RET_ERROR_CANCEL]
///     - Any error occurred in the transmission of the session message
///       to communication service. [CANIF_RET_ERROR_CANCEL]
///     - It failed to access to the shared memory for the transmission of
///       session message to communication service. [CANIF_RET_ERROR_CANCEL]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     ASync
/// \~english @par Detail
///     - The transmission of the CAN data starts(Regular/One). \n
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
CANIF_RET_API Canif_TransmissionStart(HANDLE h_app, PCSTR notify_name,
                                      uint8_t rid, uint16_t freq, CAN_DATA *p_data);

/*******************************************************************************
 * MODULE    : Canif_CommandCtrl
 ******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/// \ingroup Canif_CommandCtrl
/// \~english @par Brief
///     CAN command control
/// \~english @param [in] h_app
///     HANDLE - Handle for application
/// \~english @param [in] notify_name
///     PCSTR - Delivery target thread name
/// \~english @param [in] rid
///     uint8_t - Resource ID
/// \~english @param [in] cmd_id
///     uint32_t - CAN command ID(32bit)
/// \~english @retval CANIF_RET_NORMAL          Normality
/// \~english @retval CANIF_RET_ERROR_PARAM     Abnormality of parameter
/// \~english @retval CANIF_RET_ERROR_CANCEL    Abnormal termination
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///     - h_app is NULL.[CANIF_RET_ERROR_PARAM]
///     -notify_nameis NULL or the length ofnotify_nameis longer than 15 bytes.
///       [CANIF_RET_ERROR_PARAM]
///     - CAN command ID is invalid.[CANIF_RET_ERROR_PARAM]
///     - malloc failed to get the message queue management information area
///       for transmission to communication service.  [CANIF_RET_ERROR_CANCEL]
///     - malloc failed to get the message queue name storage area for
///       transmission to communication service. [CANIF_RET_ERROR_CANCEL]
///     - mq_open failed to open the message queue for transmission
///       to communication service. [CANIF_RET_ERROR_CANCEL]
///     - The session message queue to communication service is full.
///       [CANIF_RET_ERROR_CANCEL]
///     - The transmission file descriptor of the session
///       message to communication service is invalid. [CANIF_RET_ERROR_CANCEL]
///     - The interruption by the system call (signal) occurred while
///       transmitting the session message to communication service.
///       [CANIF_RET_ERROR_CANCEL]
///     - Any error occurred in the transmission of the session message
///       to communication service. [CANIF_RET_ERROR_CANCEL]
///     - It failed to access to the shared memory for the transmission of
///       session message to communication service. [CANIF_RET_ERROR_CANCEL]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     ASync
/// \~english @par Detail
///     - Transmission control of CAN command. \n
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
CANIF_RET_API Canif_CommandCtrl(HANDLE h_app, PCSTR notify_name, uint8_t rid,
                                uint32_t cmd_id);

/*******************************************************************************
 * MODULE    : Canif_CommWatch
 ******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/// \ingroup Canif_CommWatch
/// \~english @par Brief
///     CAN data regular transmission stop
/// \~english @param [in] h_app
///     HANDLE - Handle for application
/// \~english @param [in] notify_name
///     PCSTR - Delivery target thread name
/// \~english @param [in] can_id
///     CANID - CAN ID
/// \~english @param [in] did
///     DID - Data ID
/// \~english @param [in] watch_time
///     uint16_t - Communication watch suspension time(Unit of 100ms)
/// \~english @retval CANIF_RET_NORMAL          Normality
/// \~english @retval CANIF_RET_ERROR_PARAM     Abnormality of parameter
/// \~english @retval CANIF_RET_ERROR_CANCEL    Abnormal termination
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///     - h_app is NULL.[CANIF_RET_ERROR_PARAM]
///     -notify_nameis NULL or the length ofnotify_nameis longer than 15 bytes.
///       [CANIF_RET_ERROR_PARAM]
///     - The upper 3bits of CAN ID are not 0.[CANIF_RET_ERROR_PARAM]
///     - malloc failed to get the message queue management information area
///       for transmission to communication service.  [CANIF_RET_ERROR_CANCEL]
///     - malloc failed to get the message queue name storage area for
///       transmission to communication service. [CANIF_RET_ERROR_CANCEL]
///     - mq_open failed to open the message queue for transmission
///       to communication service. [CANIF_RET_ERROR_CANCEL]
///     - The session message queue to communication service is full.
///       [CANIF_RET_ERROR_CANCEL]
///     - The transmission file descriptor of the session
///       message to communication service is invalid. [CANIF_RET_ERROR_CANCEL]
///     - The interruption by the system call (signal) occurred while
///       transmitting the session message to communication service.
///       [CANIF_RET_ERROR_CANCEL]
///     - Any error occurred in the transmission of the session message
///       to communication service. [CANIF_RET_ERROR_CANCEL]
///     - It failed to access to the shared memory for the transmission of
///       session message to communication service. [CANIF_RET_ERROR_CANCEL]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     - CAN data regular transmission stop. \n
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
CANIF_RET_API Canif_CommWatch(HANDLE h_app, PCSTR notify_name, CANID can_id,
                              DID did, uint16_t watch_time);

////////////////////////////////////////////////////////////////////////////////
/// \ingroup Canif_TransStart
/// \~english @par Brief
///     Transmission CAN Command
/// \~english @param [in] h_app
///     HANDLE - Handle for application
/// \~english @param [in] can_id
///     CANID - CAN ID
/// \~english @param [in] mask
///     CAN_DATA_MASK* - Mask Data
/// \~english @param [in] dat
///     CAN_DATA_BIT* - Transmission Data
/// \~english @param [in] freq
///     uint32_t - Transmission Cycle
/// \~english @retval CANIF_RET_NORMAL          Normality
/// \~english @retval CANIF_RET_ERROR_PARAM     Abnormality of parameter
/// \~english @retval CANIF_RET_ERROR_CANCEL    Abnormal termination
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///     - h_app is NULL.[CANIF_RET_ERROR_PARAM]
///     - Transmission Cycle is bigger than 0xFFFF.[CANIF_RET_ERROR_PARAM]
///     - CAN ID is NULL.[CANIF_RET_ERROR_PARAM]
///     - Mask Data is NULL.[CANIF_RET_ERROR_PARAM]
///     - Transmission Data is NULL.[CANIF_RET_ERROR_PARAM]
///     - CAN ID is not defined in the _CWORD108_ bit asignment table
///       [CANIF_RET_ERROR_PARAM]
///     - The upper 3bits of CAN ID are not 0.[CANIF_RET_ERROR_PARAM]
///     - malloc failed to get the message queue management information area
///       for transmission to communication service.  [CANIF_RET_ERROR_CANCEL]
///     - malloc failed to get the message queue name storage area for
///       transmission to communication service. [CANIF_RET_ERROR_CANCEL]
///     - mq_open failed to open the message queue for transmission
///       to communication service. [CANIF_RET_ERROR_CANCEL]
///     - The session message queue to communication service is full.
///       [CANIF_RET_ERROR_CANCEL]
///     - The transmission file descriptor of the session
///       message to communication service is invalid. [CANIF_RET_ERROR_CANCEL]
///     - The interruption by the system call (signal) occurred while
///       transmitting the session message to communication service.
///       [CANIF_RET_ERROR_CANCEL]
///     - Any error occurred in the transmission of the session message
///       to communication service. [CANIF_RET_ERROR_CANCEL]
///     - It failed to access to the shared memory for the transmission of
///       session message to communication service. [CANIF_RET_ERROR_CANCEL]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     ASync
/// \~english @par Detail
///     - Start Transmission CAN Command API. \n
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
CANIF_RET_API Canif_TransStart(HANDLE h_app, CANID can_id, CAN_DATA_MASK *mask,
                               CAN_DATA_BIT *dat, uint32_t freq);

////////////////////////////////////////////////////////////////////////////////
/// \ingroup Canif_DeliveryEraseAll
/// \~english @par Brief
///     Delete all delivery entry
/// \~english @param [in] h_app
///     HANDLE - Handle for application
/// \~english @param [in] notify_name
///     PCSTR - Delivery target thread name
/// \~english @retval CANIF_RET_NORMAL          Normality
/// \~english @retval CANIF_RET_ERROR_PARAM     Abnormality of parameter
/// \~english @retval CANIF_RET_ERROR_CANCEL    Abnormal termination
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     - Change of internal state according to the API does not occur
/// \~english @par Conditions of processing failure
///     - h_app is NULL.[CANIF_RET_ERROR_PARAM]
///     -notify_nameis NULL or the length ofnotify_nameis longer than 15 bytes.
///       [CANIF_RET_ERROR_PARAM]
///     - malloc failed to get the message queue management information area
///       for transmission to communication service.  [CANIF_RET_ERROR_CANCEL]
///     - malloc failed to get the message queue name storage area for
///       transmission to communication service. [CANIF_RET_ERROR_CANCEL]
///     - mq_open failed to open the message queue for transmission
///       to communication service. [CANIF_RET_ERROR_CANCEL]
///     - The session message queue to communication service is full.
///       [CANIF_RET_ERROR_CANCEL]
///     - The transmission file descriptor of the session
///       message to communication service is invalid. [CANIF_RET_ERROR_CANCEL]
///     - The interruption by the system call (signal) occurred while
///       transmitting the session message to communication service.
///       [CANIF_RET_ERROR_CANCEL]
///     - Any error occurred in the transmission of the session message
///       to communication service. [CANIF_RET_ERROR_CANCEL]
///     - It failed to access to the shared memory for the transmission of
///       session message to communication service. [CANIF_RET_ERROR_CANCEL]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     - Delete all delivery entry. \n
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
CANIF_RET_API Canif_DeliveryEraseAll(HANDLE h_app, PCSTR notify_name);

/** @}*/  // end of CAN
/** @}*/  // end of communication
/** @}*/  // end of peripheral_service
/** @}*/  // end of BaseSystem

#if defined(__cplusplus)
}
#endif

#endif  // COMMUNICATION_CLIENT_CAN_INCLUDE_PERIPHERAL_SERVICE_CANIF_API_H_
