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

/******************************************************************************
@file       DR_API.cpp
@detail     DR_API Functions
@lib        libDR_API.so
******************************************************************************/

/*****************************************************************************
 *            Include                                                        *
 *****************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include "DR_API.h"
#include "DR_API_private.h"

#define DR_API_DEBUG 0

static EventID DrCreateEvent(PNO pno);
static RET_API DrDeleteEvent(EventID event_id);
static RET_API DrSndMsg(PNO pno_src, PNO pno_dest, CID cid, u_int16 msg_len, const void *msg_data);

/******************************************************************************
@brief      DrSetMapMatchingData<BR>
            Map matching information setting
@outline    Set up map matching information.
@param[in]  PNO                   pno              : Thread ID
@param[in]  MAP_MATCHING_DATA*    map_matching_data  : Map matching information<BR>
@param[out] none
@return     DR_EXT_RET_API
@retval     DR_EXT_RET_NORMAL         : Normal completion
@retval     DR_EXT_RET_ERROR          : Parameter error
*******************************************************************************/
DR_EXT_RET_API DrSetMapMatchingData(PNO pno, MAP_MATCHING_DATA* map_matching_data) {
    DR_EXT_RET_API ret = DR_EXT_RET_NORMAL;    /* Return value of this function            */
    RET_API  ret_api   = RET_NORMAL;           /* System API return value    */

#if DR_API_DEBUG
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DR_API : DrSetMapMatchingData \r\n");
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "\r\n lat[0x%08X] lon[0x%08X] sts[0x%02X] \r\n",
                   map_matching_data->position_info.latitude,
                   map_matching_data->position_info.longitude,
                   map_matching_data->position_info.status);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "rate[0x%04X] sts[0x%02X] ort[0x%04X] sts[0x%02X] \r\n Sts[0x%08X] \r\n",
                   map_matching_data->rate_info.rate,
                   map_matching_data->rate_info.status,
                   map_matching_data->orient_info.orient,
                   map_matching_data->orient_info.status,
                   map_matching_data->status);
#endif

    /*--------------------------------------------------------------*
     *    Send map matching information                                *
     *--------------------------------------------------------------*/
    /* Messaging */
    ret_api = DrSndMsg(pno,
                       PNO_VEHICLE_SENSOR,
                       CID_DR_MAP_MATCHING_DATA,
                       (u_int16)sizeof(MAP_MATCHING_DATA), (const void *)map_matching_data);

    if (RET_NORMAL == ret_api) {
        ret = DR_EXT_RET_NORMAL;
    } else {
        ret = DR_EXT_RET_ERROR;
    }

#if DR_API_DEBUG
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DR_API : DrSetMapMatchingData return[0x%X] \r\n", ret);
#endif

    return ret;
}

/******************************************************************************
@brief      DrClearBackupData<BR>
            DR backup data initialization
@outline    Initialize the DR backup data.
@param[in]  PNO                   pno              : Thread ID
@param[out] none
@return     DR_EXT_RET_API
@retval     DR_EXT_RET_NORMAL         : Clear success
@retval     DR_EXT_RET_ERROR          : Clear failed
*******************************************************************************/
DR_EXT_RET_API DrClearBackupData(PNO pno) {
    EventID    event_id = 0;                   /* Event ID */
    int32      event_val = 0;                  /* Event value */
    RET_API    ret_api = RET_NORMAL;           /* System API return value */
    DR_EXT_RET_API ret = DR_EXT_RET_NORMAL;    /* Return value */
    u_char     data;                           /* Message data(Dummy)    */

    /* Event Generation */
    event_id = DrCreateEvent(pno);

    if (event_id != 0) {
        /* Successful event generation */
        /* Messaging(Notify VehicleSens_thread) */
        ret_api = DrSndMsg(pno,
                           PNO_VEHICLE_SENSOR,
                           CID_DR_CLEAR_BACKUP_DATA,
                           0U,                       /* Message size is set to 0          */
                           (const void *)&data);     /* Message data(Dummy) -> Errors by NULL    */
        if (ret_api == RET_NORMAL) {
            /* Message transmission process succeeded */
            /* Waiting for completion event from vehicle sensor thread */
            ret_api = _pb_WaitEvent(event_id,
                                    SAPI_EVWAIT_VAL,
                                    DR_RET_ERROR_MIN, DR_EXT_RET_NORMAL, &event_val, INFINITE);
            if (ret_api == RET_NORMAL) {
                /* Return from Event Wait */
                if (event_val == RET_NORMAL) {
                    /* Clear success */
                    ret = DR_EXT_RET_NORMAL;
                } else {
                    /* Clear failed */
                    ret = DR_EXT_RET_ERROR;
                }
            } else {
                /* Failed to wait for event */
                ret = DR_EXT_RET_ERROR;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_WaitEvent Failed");
            }
        } else {
            /* Message transmission failure */
            ret = DR_EXT_RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "DrSndMsg Failed");
        }
        /* Event deletion */
        (void)DrDeleteEvent(event_id);
    } else {
        /* Event generation failure */
        ret = DR_EXT_RET_ERROR;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "DrCreateEvent Failed");
    }

    return ret;
}

/*******************************************************************************
 * MODULE    : DrSndMsg
 * ABSTRACT  : Message transmission processing
 * FUNCTION  : Send a message to the specified PNO
 * ARGUMENT  : pno_src            : Source PNO
 *           : pno_dest        : Destination PNO
 *           : cid            : Command ID
 *           : msg_len            : Message data body length
 *           : *msg_data        : Pointer to message data
 * NOTE      :
 * RETURN    : RET_NORMAL        : Normal completion
 *           : RET_ERRNOTRDY    : Destination process is not wakeup
 *           : RET_ERRMSGFULL    : Message queue overflows
 *           : RET_ERRPARAM        : Buffer size error
 ******************************************************************************/
static RET_API DrSndMsg(PNO pno_src, PNO pno_dest, CID cid, u_int16 msg_len, const void *msg_data) {
    DR_MSG_BUF    msg_buf;
    RET_API       ret_api;

    if (msg_data != NULL) {
        /* Initialization message buffe */
        (void)memset(reinterpret_cast<void *>(&msg_buf), 0, sizeof(DR_MSG_BUF));

        /*--------------------------------------------------------------*
         *    Create a message header                                   *
         *--------------------------------------------------------------*/
        msg_buf.stHdr.hdr.sndpno         = pno_src;    /* source PNO    */
        msg_buf.stHdr.hdr.cid            = cid;        /* Command ID    */
        msg_buf.stHdr.hdr.msgbodysize    = msg_len;    /* Data size    */

        /*--------------------------------------------------------------*
         *    Create a message data                                *
         *--------------------------------------------------------------*/
        if (msg_len != 0) {
            (void)memcpy(reinterpret_cast<void *>(msg_buf.ucData), msg_data, (size_t)msg_len);
        }
        /*--------------------------------------------------------------*
         * Send messages                                          *
         *--------------------------------------------------------------*/
        ret_api = _pb_SndMsg(pno_dest,                                               /* destination PNO */
                             (u_int16)(sizeof(T_APIMSG_MSGBUF_HEADER) + msg_len),    /* message size */
                             reinterpret_cast<void *>(&msg_buf),                     /* message buffer */
                             0);                                                     /* Unused Argument */
    } else {
        /* Argument error(Pointer to the data buffer) */
        ret_api = RET_ERRPARAM;
    }

    return ret_api;
}

/*******************************************************************************
 * MODULE    : DrCreateEvent
 * ABSTRACT  : Event creation process
 * FUNCTION  : Generate an event
 * ARGUMENT  : pno        : Thread ID
 * NOTE      :
 * RETURN    : Non-zero        : Event ID
 *           : Zero            : Event generation failure
 ******************************************************************************/
static EventID DrCreateEvent(PNO pno) {
    EventID    event_id;          /* Event ID */
    int8       event_name[32];    /* Event name character string buffer */

    /* Initialization of event name character string buffer */
    (void)memset(reinterpret_cast<void *>(event_name), 0, sizeof(event_name));

    /* Event name creation */
    snprintf(event_name, sizeof(event_name), "DR_API_%X", pno);

    /* Event Generation */
    event_id = _pb_CreateEvent(_CWORD64_EVENT_MANUALRESET_OFF, DR_EVENT_VAL_INIT, event_name);

    if (event_id == 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_CreateEvent");
    }

    return event_id;
}

/*******************************************************************************
 * MODULE    : DrDeleteEvent
 * ABSTRACT  : Event deletion processing
 * FUNCTION  : Delete events
 * ARGUMENT  : event_id    : Event ID of the event to delete
 * NOTE      :
 * RETURN    : RET_NORMAL    : Normal completion
 *           : RET_EV_NONE    : Specified event does not exist
 ******************************************************************************/
static RET_API DrDeleteEvent(EventID event_id) {
    return (_pb_DeleteEvent(event_id));
}

