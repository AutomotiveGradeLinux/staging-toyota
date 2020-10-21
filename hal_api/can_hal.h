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
#ifndef HAL_API_CAN_HAL_H_
#define HAL_API_CAN_HAL_H_
/**
 * @file can_hal.h
 */

/** @addtogroup communication
 *  @{
 */
/** @addtogroup can_hal
 *  @ingroup communication
 *  @{
 */

#include <native_service/frameworkunified_types.h>
#include <string>

/**
 * \~english Max data length of nomarl can send
 */
#define CAN_NORMAL_MESSAGE_LEN (8)

/**
 * \~english Maximum data length
 */
#define CAN_MESSAGE_LEN (2048)

/**
 * \~english  Maximum CAN_ID data length
 */
#define CAN_NUM_MAX (125)

/**
 * l_eStatus = FrameworkUnifiedAttachCallbackToDispatcher(hApp, FRAMEWORKUNIFIED_ANY_SOURCE, CID_CANHAL_CMD_CAN_SEND_STATUS, CANHalSndStsProcess);
 *            @endcode
 * \~english  Command ID to send CAN data and get the send status
 */
#define CID_CANHAL_CMD_CAN_SEND_STATUS (0x0A03)

/**
 * l_eStatus = FrameworkUnifiedAttachCallbackToDispatcher(hApp, FRAMEWORKUNIFIED_ANY_SOURCE, CID_CANHAL_CMD_CAN_RECV, CANHalRcvProcess);
 *            @endcode
 * \~english  Command ID to receive the CAN data
 */
#define CID_CANHAL_CMD_CAN_RECV        (0x0A04)

/**
 * l_eStatus = FrameworkUnifiedAttachCallbackToDispatcher(hApp, FRAMEWORKUNIFIED_ANY_SOURCE, CID_CANHAL_CMD_CAN_READY, AnyCallback);
 *            @endcode
 * \~english  Command ID to receive Global CAN is ready.
 */
#define CID_CANHAL_CMD_CAN_READY                 (0x0A0C)

/**
 * l_eStatus = FrameworkUnifiedAttachCallbackToDispatcher(hApp, FRAMEWORKUNIFIED_ANY_SOURCE, CID_CANHAL_CMD_ERROR_NOTIFY, AnyCallback);
 *            @endcode
 * \~english  Command ID to receive can hal error.
 */
#define CID_CANHAL_CMD_ERROR_NOTIFY                 (0x0A10)

/**
 * \~english  No target address
 */
#define CAN_NTA_NONE (0x00)

/**
 * \~english Size of error message.
 */
#define CANHAL_ERROR_MESSAGE_LEN (2048)

/**
 * \~english  Return type of the API
 */
enum CANHAL_RET_API {
  CANHAL_RET_NORMAL = 0,  //!< \~english API Success
  CANHAL_RET_ERR_ERR,     //!< \~english API Failed
  CANHAL_RET_ERR_PARAM,   //!< \~english Parameter error
  CANHAL_RET_ERR_STATE,  //!< \~english Uninitialized(not called CanOpen)
};

/**
 * \~english  CAN type
 */
enum CanHalType {
    CAN_HAL_TYPE_CAN = 0,  //!< \~english CAN
    NR_CAN_HAL_TYPES       //!< \~english Number of CAN type
};

/**
 * \~english  device State
 */
enum DEVICE_STATE {
  DEVICE_STATE_UNAVAILABLE,    //!< \~english device unavailable
  DEVICE_STATE_AVAILABLE,      //!< \~english device available
};

/**
 * \~english  CAN Send Result
 */
enum CAN_SEND_RESULT {
    CAN_SEND_RESULT_FAILURE = 0,     //!< \~english Send failure
    CAN_SEND_RESULT_SUCCESS,         //!< \~english Send success
};

/**
 * \~english  structure of can data
 */
typedef struct CanMessageT {
    uint32_t can_id;                 //!< \~english Target CAN ID
    uint32_t dlc;                    //!< \~english Data length(1~2048)
    uint8_t  rid;                    //!< \~english Replay ID
    uint8_t  data[CAN_MESSAGE_LEN];  //!< \~english Data buffer
} CanMessage;

/**
 * \~english  structure of can send result
 */
typedef struct CanSendResultT {
  uint32_t        can_id;               //!< \~english CAN ID
  uint8_t         rid;                  //!< \~english RID
  CAN_SEND_RESULT result;              //!< \~english Send result
} CanSendResult;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup CanOpen
 * \~english @par Brief
 *        API to start the can_hal service
 * \~english @param [in] h_app
 *        HANDLE - Application handle
 * \~english @param [in] type
 *        CanHalType - CAN type
 * \~english @retval CANHAL_RET_NORMAL     :  API return OK
 * \~english @retval CANHAL_RET_ERR_PARAM  :  Paramter error
 * \~english @retval CANHAL_RET_ERR_ERR    :  Failed
 * \~english @par Prerequisite
 *        None
 * \~english @par Change of internal state
 *        - When calling of this API succeeds, the requested channel state is set to open.
 * \~english @par Conditions of processing failure
 *        - Input paramter is invalid.
 *        - Internal processing failure.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Method
 * \~english @par Detail
 *        - Perform device use preparation processing of the requested communication path.
 *        - Up to one process can use this library.
 * \~english @see
 *        Nothing
 */
CANHAL_RET_API CanOpen(HANDLE h_app, CanHalType type);

/**
 * \ingroup CanClose
 * \~english @par Brief
 *        API to stop the can_hal service
 * \~english @param [in] h_app
 *        HANDLE - Application handle
 * \~english @param [in] type
 *        CanHalType - CAN type
 * \~english @retval CANHAL_RET_NORMAL      :  API return OK
 * \~english @retval CANHAL_RET_ERR_PARAM   :  Paramter error
 * \~english @retval CANHAL_RET_ERR_ERR     :  Failed
 * \~english @retval CANHAL_RET_ERR_STATE  :  Failed because CanOpen is not called
 * \~english @par Prerequisite
 *        Communication path status is open
 * \~english @par Change of internal state
 *        - In the case of normal termination, the requested channel state is close.
 * \~english @par Conditions of processing failure
 *        - CanOpen is not called.
 *        - Input paramter is invalid.
 *        - Internal processing failure.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @par Detail
 *        - Perform device use finalize processing of the requested communication path.
 *        - Up to one process can use this library.
 * \~english @see
 *        None
 */
CANHAL_RET_API CanClose(HANDLE h_app, CanHalType type);

/**
 * \ingroup CanSend
 * \~english @par Brief
 *        API to send the Can data
 * \~english @param [in] h_app
 *        HANDLE - Application handle
 * \~english @param [in] message
 *        const CanMessage * - Pointer to the structure of
 *                                    the data which is sent to the CAN
 * \~english @param [in] type
 *        CanHalType - CAN type
 * \~english @retval CANHAL_RET_NORMAL      :  API return OK
 * \~english @retval CANHAL_RET_ERR_ERR     :  Failed
 * \~english @retval CANHAL_RET_ERR_PARAM   :  Paramter error
 * \~english @retval CANHAL_RET_ERR_STATE  :  Failed because CanOpen is not called
 * \~english @par Prerequisite
 *        - can_hal start successfully
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Input paramter is invalid.
 *        - CanOpen is not called.
 *        - Send data to the sending thread failed.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Method
 * \~english @par Detail
 *        - Check the sending data, if data is valid, then send the data
 *            to the sending thread; If data if invalid, then return error.
 *        - The API can only be used by 1 process.
 * \~english @see
 *        \ref CanSendResult
 */
CANHAL_RET_API CanSend(HANDLE h_app, const CanMessage *message, CanHalType type);

/**
 * \ingroup CanGetVersion
 * \~english @par Brief
 *        API to get the CAN micon version
 * \~english @param [in] h_app
 *        HANDLE - Application handle
 * \~english @param [out] p_version
 *        std::string * - Pointer to the version buffer
 * \~english @retval CANHAL_RET_NORMAL     : API return OK
 * \~english @retval CANHAL_RET_ERR_STATE : Failed because CanOpen is not called
 * \~english @retval CANHAL_RET_ERR_PARAM  : Paramter error
 * \~english @retval CANHAL_RET_ERR_ERR    : Failed
 * \~english @par Prerequisite
 *        - can_hal start successfully
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Input paramter is invalid.
 *        - CanOpen is not called.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @par Detail
 *        - This API gets the CAN version.
 *        - The API can only be used by 1 process.
 *        - p_version detail\n
 *          Information on the following pink shaded part 4 BYTE in the version information \n
 *          of the CAN microcomputer is stored in string and returned.\n
 *          <table>
 *          <tr>
 *          <th></th><th>7bit-0bit</th>
 *          </tr>
 *          <tr>
 *          <td>1BYTE</td><td>7bit:M/S 6-0bit:Message kind</td>
 *          </tr>
 *          <tr>
 *          <td>2BYTE</td><td>7bit-0bit Operation Code</td>
 *          </tr>
 *          <tr>
 *          <td>3BYTE</td><td bgcolor="pink">7bit-0bit Software kind</td>
 *          </tr>
 *          <tr>
 *          <td>4BYTE</td><td bgcolor="pink">7bit-0bit version(Higher order)</td>
 *          </tr>
 *          <tr>
 *          <td>5BYTE</td><td bgcolor="pink">7bit-0bit version(middle order)</td>
 *          </tr>
 *          <tr>
 *          <td>6BYTE</td><td bgcolor="pink">7bit-0bit version(lower order)</td>
 *          </tr>
 *          </table>
 * \~english @see
 *        None
 */
CANHAL_RET_API CanGetVersion(HANDLE h_app, std::string *p_version);

/** @}*/  // end of can_hal
/** @}*/  // end of communication
#ifdef __cplusplus
}
#endif
#endif  // HAL_API_CAN_HAL_H_
