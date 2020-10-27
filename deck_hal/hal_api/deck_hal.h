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

/*!-----------------------------------------------------------------------------
 * @file    deck_hal.h
 * @~english
 * @brief   deck_hal API head
 *
 */

#ifndef HAL_API_DECK_HAL_H_
#define HAL_API_DECK_HAL_H_

#include <native_service/frameworkunified_types.h>

/**
 * @file deck_hal.h
 */

/** @addtogroup interface_unified
 *  @{
 */
/** @addtogroup deck_hal
 *  @ingroup interface_unified
 *  @{
 */
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*! @~english Maximum length of command data division */
#define DECK_CMD_SIZE_CMDDATA_MAX  252
/*! @~english Maximum length of notification destination */
#define NOTIFY_NAME_MAX_SIZE 15

// CID_DECK_SYS_DELIVERY is for deck_hal to report correspinding message to vehicle,
// the value 0x40 can be changed in the future
/**
 * \~english @brief  DECK SYS Command-Delivery Notification
 * \~english @brief  If you want to receive this DECK SYS command-delivery notification, use the NSFW IF as follows: 
 * \~english @code
 * l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(hApp, DECKHAL_THREAD, CID_DECK_SYS_DELIVERY, LineSensDrvRcvDeckHal);
 *            @endcode
 */
#define CID_DECK_SYS_DELIVERY 0x40

/*!
 * @~english
 * @brief Disc eject mode
*/
typedef enum {
  /*! @~english  normal eject   */
  DECK_EJECT_NORMAL,
  /*! @~english  forced eject   */
  DECK_EJECT_FORCED
} DeckEjectMode;

/*!
 * @~english
 * @brief Disc mode
*/
typedef enum {
  /*! @~english  CD    */
  DECK_KIND_CD,
  /*! @~english  DVD    */
  DECK_KIND_DVD,
  /*! @~english  Blue-lay disc    */
  DECK_KIND_BD
} DeckKind;

/*------------------------------------------------------------------------------
 * typedefs
 *----------------------------------------------------------------------------*/
/**
 * \~english  Deck-Sys command header structure
 */
typedef struct {
  /*! @~english Source logical address  */
  uint8_t  lgadr_from;
  /*! @~english Forwarding destination logical address  */
  uint8_t  lgadr_to;
  /*! @~english OP code  */
  uint8_t  opc;
  /*! @~english Reserve  */
  uint8_t  reserve;
} DeckCmdhdr;

/**
 * \~english  Deck-Sys command body structure
 */
typedef struct DeckCmd {
  /*! @~english Deck-Sys command header  */
    DeckCmdhdr cmdhdr;
  /*! @~english Data length  */
    uint16_t data_length;
  /*! @~english Reserve  */
    uint16_t reserve;
  /*! @~english Command data  */
    uint8_t data[DECK_CMD_SIZE_CMDDATA_MAX];
} DeckCmd;

/*!
 * @~english
 * @brief Structure for message header
 */
typedef struct {
  uint16_t replyid;      //!< \~english Send Result ID
  uint16_t sndpno;       //!< \~english Send process No
  uint16_t respno;       //!< \~english Response No
  uint16_t cid;          //!< \~english Command ID
  uint16_t msgbodysize;  //!< \~english Message body size
  uint8_t rid;           //!< \~english Resource ID
  uint8_t reserve;       //!< \~english Reserve
  uint8_t filler[2];     //!< \~english filler
} DECK_HEADER_COMM;

/*!
 * @~english
 * @brief Structure for buffer message header
 */
typedef struct {
  uint32_t signo;            //!< \~english Signal No
  DECK_HEADER_COMM hdr;  //!< \~english Message Header
} DECK_MSGBUF_HEADER_COMM;

/**
 * \~english Delivery message structure
 */
typedef struct {
  /*! @~english Header part  */
  DECK_MSGBUF_HEADER_COMM hdr;
  /*! @~english Physical address  */
  uint16_t phyadr_from;
  /*! @~english Communication type  */
  uint8_t service_type;
  /*! @~english Reserve    */
  uint8_t reserve;
  /*! @~english Data section  */
  DeckCmd cmd;
} DECK_MSG_DELIVERY;

/*!
 * @~english
 * @brief Structure for send command data
 */
typedef struct {
  /*! @~english Physical address */
  uint16_t  phyadr_from;
  /*! @~english Communication type */
  uint8_t   service_type;
  /*! @~english Forwarding former logic address */
  uint8_t   lgadr_from;
  /*! @~english Forwarding site logic address */
  uint8_t   lgadr_to;
  /*! @~english Operation code */
  uint8_t   opc;
  /*! @~english Data length (size in command data */
  uint8_t   d_length;
  /*! @~english Command data */
  uint8_t   data[DECK_CMD_SIZE_CMDDATA_MAX];
  /*! @~english reserve */
  uint8_t   reserve;
} DECK_CMD_DATA;

/*!
 * @~english
 * @brief Structure for send command
 */
typedef struct {
  /*! @~english Command information */
  DECK_CMD_DATA   cmd_data;
} DECK_CMD;

/*------------------------------------------------------------------------------
 * function declaration
 *----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StopDiscRotation
/// \~english @par Brief
///        Stop disc rotation
/// \~english @param [in] -
/// \~english @param [out] -
/// \~english @retval eFrameworkunifiedStatusOK OK
/// \~english @retval eFrameworkunifiedStatusFail Abnormal Error
/// \~english @par Prerequisite
///       - Disc was inserted
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - Error occurs when call open or ioctl function.[eFrameworkunifiedStatusFail]
/// \~english @par Classification
///         Public
/// \~english @par Type
///         Sync
/// \~english @par Detail
///       - Stop disc rotation
///       - Whatever disc is rotating or stoped, return successed.
///       - The API can be used by multi-process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////////////

EFrameworkunifiedStatus StopDiscRotation();

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EjectDisc
/// \~english @par Brief
///        Eject disc
/// \~english @param [in]  kind
///       DeckKind       - kind of deck.
/// \~english @param [in]  mode
///       DeckEjectMode       - eject mode(normal/forcedly)
/// \~english @param [out] -
/// \~english @retval eFrameworkunifiedStatusOK OK
/// \~english @retval eFrameworkunifiedStatusFail Abnormal Error
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - Error occurs when call open or ioctl function.[eFrameworkunifiedStatusFail]
/// \~english @par Classification
///         Public
/// \~english @par Type
///         Sync
/// \~english @par Detail
///       - Eject disc
///       - Whatever disc is Ejecting or Unejected, return successed.
///       - The API can be used by multi-process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus EjectDisc(DeckKind kind, DeckEjectMode mode);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DeckHalSysSend
/// \~english @par Brief
///        Send Disc Sys-Command to MICON
/// \~english @param [in] h_app
///        HANDLE   - Handle of caller (Handle to get in Dispatcher generation
///                  (FrameworkunifiedCreateDispatcherWithoutLoop etc.))
/// \~english @param [in] name
///        PCSTR    - Name to notification of send result destination thread name
/// \~english @param [in] cmd
///        DECK_CMD - Send command pointer
/// \~english @param [in] req_id
///        uint8_t - Resource ID
/// \~english @param [out] -
/// \~english @retval eFrameworkunifiedStatusOK Normal termination
/// \~english @retval eFrameworkunifiedStatusInvldParam Abnormal parameter
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - h_app is NULL.[eFrameworkunifiedStatusInvldParam]
///       - name is NULL or the length of name is longer than 15 bytes.
///         [eFrameworkunifiedStatusInvldParam]
///       - Send command pointer(cmd) is null.[eFrameworkunifiedStatusInvldParam]
/// \~english @par Classification
///         Public
/// \~english @par Type
///         Sync
/// \~english @par Detail
///       - Send SYS command to MICON
///       - Now only usb disc exists,MICON disc is stub now.
///       - The API can only be used by 1 process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus DeckHalSysSend(HANDLE h_app, PCSTR name, const DECK_CMD* cmd, uint8_t req_id);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DeckHalRegister
/// \~english @par Brief
///        Register the deck_hal command to receive from MICON
/// \~english @param [in] h_app
///        HANDLE   - Handle of caller (Handle to get in Dispatcher generation
///                  (FrameworkunifiedCreateDispatcherWithoutLoop etc.))
/// \~english @param [in] notify_name
///        PCSTR    - Name to notification destination thread name
/// \~english @param [out] -
/// \~english @retval eFrameworkunifiedStatusOK Normal termination
/// \~english @retval eFrameworkunifiedStatusInvldParam Abnormal parameter
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - h_app is NULL.[eFrameworkunifiedStatusInvldParam]
///       - notify_name is NULL or the length of notify_name is longer than 15 bytes.
///         [eFrameworkunifiedStatusInvldParam]
/// \~english @par Classification
///         Public
/// \~english @par Type
///         Sync
/// \~english @par Detail
///       - After call this API, the user can received dech_hal command from MICON.
///       - Now MICON is stub,so that deck_hal won't register to MICON.
///       - The API can only be used by 1 process.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus DeckHalRegister(HANDLE h_app, PCSTR notify_name);

#ifdef __cplusplus
}
#endif /*__cplusplus  */

/** @}*/  // end of deck_hal
/** @}*/  // end of interface_unified

#endif  // HAL_API_DECK_HAL_H_
