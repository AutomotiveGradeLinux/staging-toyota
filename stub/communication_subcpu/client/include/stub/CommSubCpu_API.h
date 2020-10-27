/*
 * @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
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

#ifndef COMMUNICATION_SUBCPU_CLIENT_INCLUDE_PERIPHERAL_SERVICE_COMMSUBCPU_API_H_
#define COMMUNICATION_SUBCPU_CLIENT_INCLUDE_PERIPHERAL_SERVICE_COMMSUBCPU_API_H_

/**
 * @file CommSubCpu_API.h
 * @~english
 * @brief communication_subcpu API header
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup peripheral_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup communication_subcpu
 *  @ingroup peripheral_service
 *  @{
 */

#include <sys/types.h>
#include <native_service/frameworkunified_types.h>
#include <agldd/moduleid.h>
#include <stub/com_message_header.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*! @~english CommSubCpu I/F API return code  */
typedef int32_t     COMMSUBCPU_RET_API;

/*! @~english Thread name  */
#define LAN_SERVICE_SYSCOM_MAIN "DEV_SYSCOM_MAIN"

/**
 * \~english The ICR command receive notification
 */
#define CID_ICR_CMD_RCV     (0x0A00)

/**
 * \~english The ICR command send result notification
 */
#define CID_ICR_CMD_SND_CNF (0x0A01)

/*! @~english The MAX length of send command  */
#define ICRCMD_OPELAND_SND_MAX  (2048)
/*! @~english The MAX length of send ICR command  */
#define ICRCMD_OPELAND_MAX      (252)

/**
 * \~english ICR command format
 */
typedef struct {
    uint16_t    phyadr_from;        //!< \~english Physical address
    uint8_t service_type;           //!< \~english Communication type
    uint8_t lgadr_from;             //!< \~english Source logical address
    uint8_t lgadr_to;               //!< \~english Forwarding destination logical address
    uint8_t opc;                    //!< \~english Operation code
    uint8_t d_length;               //!< \~english Data size
    uint8_t data[ICRCMD_OPELAND_MAX];   //!< \~english Command data
    uint8_t reserve;                    //!< \~english Reserve
} T_ICR_CMD_DATA;

/**
 * \~english Send command structure
 */
typedef struct {
    uint16_t    phyadr_from;        //!< \~english Physical address
    uint8_t service_type;           //!< \~english Communication type
    uint8_t lgadr_from;             //!< \~english Source logical address
    uint8_t lgadr_to;               //!< \~english Forwarding destination logical address
    uint8_t opc;                    //!< \~english Operation code
    uint16_t    d_length;           //!< \~english Data size
    uint8_t data[ICRCMD_OPELAND_SND_MAX];   //!< \~english Command data
} T_ICR_CMD_SND_DATA;

/**
 * \~english ICR command data
 */
typedef struct {
    T_ICR_CMD_SND_DATA  cmd_data;       //!< \~english Command information
} T_ICR_CMD;

/**
 * \~english The message of command receive notification
 */
typedef struct {
    /*! @~english The header of message  */
    T_APIMSG_MSGBUF_HEADER_COMM msg_hdr;
    /*! @~english The message data for send result  */
    T_ICR_CMD_DATA          cmd_data;
} T_ICR_CMD_RCV;

/*!
 * @~english Status of message send result
 */
typedef enum {
    /*! @~english Send success */
    ICR_SND_SUCCESS = 0,
    /*! @~english Initialize error */
    ICR_SND_ERR_INIT,
    /*! @~english Send error */
    ICR_SND_ERR,
    /*! @~english Send error(retry out) */
    ICR_SND_ERR_RETRYOUT,
    /*! @~english Send error(buffer full) */
    ICR_SND_ERR_BUFFERFULL,
    /*! @~english Send error(micon abnormal) */
    ICR_SND_ERR_MC,
    /*! @~english The MAX */
    ICR_SND_ERR_MAX
} ICR_ERROR_CODE;

/*!
 * @~english Structure for message send status data
 */
typedef struct {
  /*! @~english Result of send message */
  ICR_ERROR_CODE  cnf_sts;
} T_ICR_CMDSNDCNF_STS;


 /**
 * \~english The send result notification
 */
typedef struct {
    /*! @~english The header of message  */
    T_APIMSG_MSGBUF_HEADER_COMM msg_hdr;
    /*! @~english The message data for send result  */
    T_ICR_CMDSNDCNF_STS     sts;
} T_ICR_CMDSNDCNF;


#define COMMSUBCPU_RET_NORMAL               0   //!< \~english Normal termination
#define COMMSUBCPU_RET_ERROR_CANCEL         1   //!< \~english Abnormal termination
#define COMMSUBCPU_RET_ERROR_PARAM          2   //!< \~english Abnormality of parameter
#define COMMSUBCPU_RET_ERROR_BUFFULL        3   //!< \~english Buffer FULL
#define COMMSUBCPU_RET_ERROR_OVERLAPPED     4   //!< \~english Entry overlapped
#define COMMSUBCPU_RET_ERROR_CREATE_EVENT   5   //!< \~english Event generation failure

/**
 * \ingroup CommSubCpu
 * \~english @par Brief
 *       CommSubCpu moudle init
 * \~english @retval COMMSUBCPU_RET_NORMAL : Success
 * \~english @par Prerequisite
 *       None
 * \~english @par Change of internal state
 *       - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *       None
 * \~english @par Classification
 *       Public
 * \~english @par Type
 *       Sync
 * \~english @see
 *       None
 */
COMMSUBCPU_RET_API CommSubCpu_Init();

/**
 * \ingroup CommSubCpu
 * \~english @par Brief
 *       CommSubCpu moudle deinitializes
 * \~english @retval COMMSUBCPU_RET_NORMAL : Success
 * \~english @par Prerequisite
 *       CommSubCpu moudle init completely
 * \~english @par Change of internal state
 *       - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *       None
 * \~english @par Classification
 *       Public
 * \~english @par Type
 *       Sync
 * \~english @see
 *       None
 */
COMMSUBCPU_RET_API CommSubCpu_DeInit();

/**
 * \ingroup CommSubCpu
 * \~english @par Brief
 *       Delivery registration of CommSubCpu command
 * \~english @param [in] name
 *       PCSTR - Receiver thread name
 * \~english @param [in] from
 *       uint8_t - Sender logical address
 * \~english @param [in] to
 *       uint8_t - Receiver logical address
 * \~english @param [in] opc
 *       uint8_t - Operation code
 * \~english @param [in] callback_id
 *       uint8_t - callback ID
 * \~english @param [in] cid
 *       uint16_t - CID
 * \~english @retval COMMSUBCPU_RET_NORMAL : Success
 * \~english @retval COMMSUBCPU_RET_ERROR_PARAM : Paramter error
 * \~english @par Prerequisite
 *       Availability of Communication is TRUE
 * \~english @par Change of internal state
 *       - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 * - Input parameter error
 *       - COMMSUBCPU_RET_ERROR_PARAM
 * \~english @par Classification
 *       Public
 * \~english @par Type
 *       Async
 * \~english @see
 *       None
 */
COMMSUBCPU_RET_API  CommSubCpu_DeliveryMsgEntry(PCSTR name, uint8_t from, uint8_t to, uint8_t opc,
                                    uint8_t  callback_id, uint16_t cid);

/**
 * \ingroup CommSubCpu
 * \~english @par Brief
 *       CommSubCpu command transmission request
 * \~english @param [in] name
 *       PCSTR - Sender thread name
 * \~english @param [in] snd_cmd
 *       T_ICR_CMD* - Pointer to send command
 * \~english @param [in] req_id
 *       uint8_t - RID of the results
 * \~english @retval COMMSUBCPU_RET_NORMAL : Success
 * \~english @retval COMMSUBCPU_RET_ERROR_PARAM : Paramter error
 * \~english @par Prerequisite
 *       Availability of Communication is TRUE
 * \~english @par Change of internal state
 *       - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 * - Input parameter error
 *       - COMMSUBCPU_RET_ERROR_PARAM
 * \~english @par Classification
 *       Public
 * \~english @par Type
 *       Async
 * \~english @see
 *       None
 */
COMMSUBCPU_RET_API CommSubCpu_Transmission(PCSTR name, T_ICR_CMD *snd_cmd, uint8_t req_id);

#ifdef __cplusplus
}
#endif /*__cplusplus    */

#endif  // COMMUNICATION_SUBCPU_CLIENT_INCLUDE_PERIPHERAL_SERVICE_COMMSUBCPU_API_H_
