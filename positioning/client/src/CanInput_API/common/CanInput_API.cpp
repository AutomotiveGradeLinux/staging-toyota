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

/*******************************************************************************
 *    File name            : CanInput_API.cpp
 *    System name            : PastModel002
 *    Sub System name        : CanInput_API library
 *    Program name        : CanInput_API
 *    Module configuration: CanInputInitialize()        Initialization
 *                        : CanInputSndMsg()            Send a message
 ******************************************************************************/

/************************************************************************
 *            Include                                                   *
 ***********************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include "CanInput_API.h"
#include "CanInput_API_private.h"

/************************************************************************
*            Global variable                                            *
************************************************************************/

/************************************************************************
*            Definition                                                 *
************************************************************************/

/*******************************************************************************
 * MODULE    : CanInputInitialize
 * ABSTRACT  : Initialization
 * FUNCTION  : CanInput_API Initialization
 * ARGUMENT  : None
 * NOTE      :
 * RETURN    : CAN_INPUT_RET_NORMAL            : Successful completion
 *           : CAN_INPUT_RET_ERROR            : An error has occurred
 ******************************************************************************/
CAN_INPUT_RET_API CanInputInitialize(void) {
    RET_API              ret_sys;
    CAN_INPUT_RET_API    ret_api;

    /* _CWORD64_api Initialization */
    ret_sys = _pb_Setup_CWORD64_API(NULL);

    if (ret_sys == RET_NORMAL) {
        ret_api = CAN_INPUT_RET_NORMAL;
    } else {
        ret_api = CAN_INPUT_RET_ERROR;
    }

    return ret_api;
}

/*******************************************************************************
 * MODULE    : CanInputSndMsg
 * ABSTRACT  : Send a message
 * FUNCTION  : Send a message to VehicleSens_thread
 * ARGUMENT  : pno                            : Thread ID
 *           : cid                        : Command ID
 *           : msg_len                        : Data size
 *           : *msg_data                    : Pointer to the data buffer
 * NOTE      :
 * RETURN    : CAN_INPUT_RET_NORMAL            : Successful completion
 *           : CAN_INPUT_RET_ERROR_PARAM    : Parameter error
 *           : CAN_INPUT_RET_ERROR            : An error has occurred
 ******************************************************************************/
CAN_INPUT_RET_API CanInputSndMsg(PNO pno, CID cid, u_int16 msg_len, const void *msg_data) {
    CAN_INPUT_RET_API    ret_api;
    CANINPUT_MSG_INFO    msg_buf;


    if (cid == CANINPUT_CID_LOCALTIME_NOTIFICATION) {
        if (msg_len <= sizeof(msg_buf.data)) {
            RET_API    ret_sys;
            u_int16    msg_size;


            /* Initialization message buffer */
            (void)memset(reinterpret_cast<void *>(&msg_buf), 0, sizeof(msg_buf));

            /*--------------------------------------------------------------*
             *    Create a message header                                   *
             *--------------------------------------------------------------*/
            /* Set only the required data */
            msg_buf.hdr.hdr.sndpno         = pno;            /* source PNO         */
            msg_buf.hdr.hdr.cid            = cid;            /* Command ID         */
            msg_buf.hdr.hdr.msgbodysize    = msg_len;        /* Data size        */

            /*--------------------------------------------------------------*
             *    Create a message data                                        *
             *--------------------------------------------------------------*/
            if ((msg_len != 0) && (msg_data != NULL)) {
                (void)memcpy(reinterpret_cast<void *>(&msg_buf.data), msg_data, (size_t)msg_len);
            }

            msg_size = sizeof(T_APIMSG_MSGBUF_HEADER) + msg_len;

            /*--------------------------------------------------------------*
             * Send a message                                                *
             *--------------------------------------------------------------*/
            ret_sys = _pb_SndMsg(PNO_VEHICLE_SENSOR,                    /* destination PNO    */
                                 msg_size,                              /* message size        */
                                 reinterpret_cast<void *>(&msg_buf),    /* message buffer    */
                                 0);                                    /* Unused Argument    */

            if (ret_sys == RET_NORMAL) {
                ret_api = CAN_INPUT_RET_NORMAL;
            } else {
                ret_api = CAN_INPUT_RET_ERROR;
            }
        } else {
            /* Argument error(Data size) */
            ret_api = CAN_INPUT_RET_ERROR_PARAM;
        }
    } else {
        /* Argument error(Command ID) */
        ret_api = CAN_INPUT_RET_ERROR_PARAM;
    }

    return ret_api;
}

