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
@file       VehicleDebug_API.cpp
@detail     VehicleDebug_API Functions
@lib        libVehicleDebug_API.so
******************************************************************************/

/*****************************************************************************
 *            Include                                                        *
 *****************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleDebug_API.h"
#include "VehicleDebug_API_private.h"

static VEHICLEDEBUG_RET_API VehicleDebugSndMsg(PNO pno_src,
                                               PNO pno_dest, CID cid, u_int16 msg_len, const void *msg_data);
static RET_API SensorGetLogSettingGetData(u_int32 *log_sw, u_int8 *severity);
static EventID SensorGetLogSettingCreateEvent(PNO pno);
static RET_API SensorGetLogSettingDeleteEvent(EventID event_id);

/*******************************************************************************
* MODULE    : SensorGetLogSettingGetData
* ABSTRACT  : Get Log Settings from Shared Memory
* FUNCTION  : Get log settings from shared memory
* ARGUMENT  : u_int32* log_sw    : Log type
*           : u_int8* severity    : Output level
* NOTE      :
* RETURN    : RET_NORMAL        : Successful acquisition
*           : RET_ERROR        : Failed to acquire
*           : RET_ERRPARAM        : Parameter error
******************************************************************************/
static RET_API SensorGetLogSettingGetData(u_int32 *log_sw, u_int8 *severity) {
    RET_API    ret = RET_NORMAL;
    RET_API    ret_api;                         /* API return value */
    VEHICLEDEBUG_MSG_LOGINFO_DAT *share_mem;    /* Store Shared Memory Address */
    u_int32    share_mem_size;                  /* Size of the linked shared memory */

    if (log_sw == NULL || severity == NULL) {
        ret = RET_ERRPARAM;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "log_sw == NULL or severity == NULL\r\n");
    } else {
        /* Link to shared memory */
        ret_api = _pb_LinkShareData(const_cast<char*>(LOG_SETTING_SHARE_MEMORY_NAME),
                                    reinterpret_cast<void **>(&share_mem), &share_mem_size);
        /* #QAC confirmation Rule11.4 Use structure for member reference(Cast according to shared memory link IF) */

        if (ret_api == RET_NORMAL) {
            /* Link to shared memory successful */
            if (share_mem_size == VEHICLEDEBUG_MSGBUF_DSIZE) {
                /* When the size of the linked shared memory is correct */

                /* Get log type/output level from shared memory */
                *log_sw = share_mem->log_sw;
                (void)memcpy(reinterpret_cast<void *>(severity),
                             (const void *)(share_mem->severity), sizeof(share_mem->severity));
            } else {
                /* The size of the linked shared memory is incorrect. */
                ret = RET_ERROR;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Bad shared memory size\r\n");
            }
        } else {
            /* Failed to link to shared memory */
            ret = RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't link shared memory\r\n");
        }
    }

    return ret;
}

/*******************************************************************************
* MODULE    : SensorGetLogSettingCreateEvent
* ABSTRACT  : Event Generation
* FUNCTION  : Generate an event
* ARGUMENT  : PNO pno          : Destination thread ID
* NOTE      :
* RETURN    : EventID event_id : Event ID (Event set failed if 0)
******************************************************************************/
static EventID SensorGetLogSettingCreateEvent(PNO pno) {
    EventID    event_id;          /* Event ID */
    int8       event_name[32];    /* Event name character string buffer */

    /* Initialization of event name character string buffer */
    (void)memset(reinterpret_cast<void *>(event_name), 0, sizeof(event_name));

    /* Event name creation */
    snprintf(event_name, sizeof(event_name), "VehicleDebug_%X", pno);

    /* Event Generation */
    event_id = _pb_CreateEvent(_CWORD64_EVENT_MANUALRESET_OFF, VEHICLEDEBUG_EVENT_VAL_INIT, event_name);

    return event_id;
}

/*******************************************************************************
 * MODULE    : SensorGetLogSettingDeleteEvent
 * ABSTRACT  : Event deletion processing
 * FUNCTION  : Delete events
 * ARGUMENT  : event_id    : Event ID of the event to delete
 * NOTE      :
 * RETURN    : RET_NORMAL    : Normal completion
 *           : RET_EV_NONE    : Specified event does not exist
 ******************************************************************************/
static RET_API SensorGetLogSettingDeleteEvent(EventID event_id) {
    return(_pb_DeleteEvent(event_id));
}

/*******************************************************************************
@brief      SensorGetLogSetting<BR>
            Log setting acquisition
@outline    Get the log settings.
@param[in]  PNO pno             : Destination thread ID
@param[out] u_int32* log_sw        : Log type
@param[out] u_int8* severity    : Output level
@return     SENSORMOTION_RET_API
@retval     VEHICLEDEBUG_RET_NORMAL       : Successful acquisition
@retval     VEHICLEDEBUG_RET_ERROR        : Acquisition failure error
*******************************************************************************/
VEHICLEDEBUG_RET_API SensorGetLogSetting(PNO pno, u_int32 *log_sw, u_int8 *severity) {
    VEHICLEDEBUG_RET_API            ret;
    RET_API                         ret_api;      /* API return value                */
    RET_API                         ret_tmp;
    EventID                         event_id;     /* Event ID                */
    int32                           event_val;    /* Event value                */
    SemID                           sem_id;       /* Semaphore ID                */
    VEHICLEDEBUG_MSG_LOGINFO_DAT    data;         /* Message data(Dummy)    */

    if (log_sw == NULL || severity == NULL) {
        ret = VEHICLEDEBUG_RET_ERROR;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "log_sw == NULL or severity == NULL\r\n");
    } else {
        /* Set of events */
        event_id = SensorGetLogSettingCreateEvent(pno);

        if (event_id != 0) {
            /* Event Set Success */

            /* Messaging(Notify VehicleSens_thread)                                */
            /* Because it uses shared memory for data retrieval,Message body does not need to be sent        */
            ret_api = VehicleDebugSndMsg(pno,
                                         PNO_VEHICLE_SENSOR,
                                         CID_VEHICLEDEBUG_LOG_GET,
                                         0,                     /* Message size is set to 0 */
                                         (const void *)&data);  /* Message data(Dummy) -> Errors by NULL */

            if (ret_api == RET_NORMAL) {
                /* Message transmission success */

                /* Wait for completion event */
                ret_api = _pb_WaitEvent(event_id,
                                        SAPI_EVWAIT_VAL,
                                        VEHICLEDEBUG_RET_ERROR_MIN, VEHICLEDEBUG_RET_NORMAL, &event_val, INFINITE);
                if (ret_api == RET_NORMAL) {
                    /* Return from Event Wait */
                    if (event_val == VEHICLEDEBUG_RET_NORMAL) {
                        /* Log setting acquisition function succeeded */

                        /* Get Semaphore ID */
                        sem_id = _pb_CreateSemaphore(const_cast<char *>(SENSOR_LOG_SETTING_SEMAPHO_NAME));

                        /* Because the return value of _pb_CreateSemaphore is not defined in #define,Evaluate directly */
                        if (sem_id != 0) {
                            /* Semaphore ID successfully acquired */
                            /* Semaphore Lock */
                            ret_api = _pb_SemLock(sem_id);

                            if (ret_api == RET_NORMAL) {
                                /* Semaphore lock successful */

                                /* Get Log Settings from Shared Memory */
                                ret_tmp = SensorGetLogSettingGetData(log_sw, severity);

                                if (ret_tmp == RET_NORMAL) {
                                    /* Log setting acquisition success */
                                    ret = VEHICLEDEBUG_RET_NORMAL;
                                } else {
                                    /* Log setting acquisition failure */
                                    ret = VEHICLEDEBUG_RET_ERROR;
                                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SensorGetLogSettingGetData Failed");
                                }

                                /* Semaphore unlock */
                                ret_api = _pb_SemUnlock(sem_id);
                                if (ret_api != RET_NORMAL) {
                                    /* Semaphore unlock failure */
                                    ret = VEHICLEDEBUG_RET_ERROR;
                                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemUnlock Failed");
                                }
                            } else {
                                /* Semaphore lock failed */
                                ret = VEHICLEDEBUG_RET_ERROR;
                                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemLock Failed");
                            }
                        } else {
                            /* Semaphore ID acquisition failure */
                            ret = VEHICLEDEBUG_RET_ERROR;
                            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "sem_id == 0");
                        }
                    } else {
                        /* Log setting acquisition function failed */
                        ret = VEHICLEDEBUG_RET_ERROR;
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "event_val != VEHICLEDEBUG_RET_NORMAL");
                    }
                } else {
                    /* _pb_WaitEvent failed */
                    ret = VEHICLEDEBUG_RET_ERROR;
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_WaitEvent Failed");
                }
            } else {
                /* Message transmission failure */
                ret = VEHICLEDEBUG_RET_ERROR;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "VehicleDebugSndMsg Failed");
            }
            /* Event deletion */
            (void)SensorGetLogSettingDeleteEvent(event_id);
        } else {
            /* Event set failed */
            ret = VEHICLEDEBUG_RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "event_id == 0");
        }
    }

    return ret;
}

/*******************************************************************************
@brief      SensorSetLogStatus<BR>
            Log Setting Request
@outline    Configure the log.
@param[in]  PNO     pno       : Destination thread ID
@param[in]  u_int32 log_sw     : Log type
                                 Correspond to the log output by each bit.
                                 0th bit : Location Log Output Settings(0:OFF 1:ON)
@param[in]  u_int8* severity  : Output level(Not used)
@param[out] None
@return     SENSORMOTION_RET_API
@retval     VEHICLEDEBUG_RET_NORMAL       : Setting success
@retval     VEHICLEDEBUG_RET_ERROR        : Setting failed
*******************************************************************************/
VEHICLEDEBUG_RET_API SensorSetLogStatus(PNO pno, u_int32 log_sw, u_int8 *severity) {
    RET_API    ret_api = RET_NORMAL;                       /* System API return value    */
    VEHICLEDEBUG_RET_API ret = VEHICLEDEBUG_RET_NORMAL;    /* Return value                 */
    VEHICLEDEBUG_MSG_LOGINFO_DAT data;                     /* Message data        */

    if (severity != NULL) {
        /* Message configuration */
        data.log_sw = log_sw;
        (void)memcpy(reinterpret_cast<void *>(&(data.severity[0])), (const void *)(severity), sizeof(data.severity));

        /* Messaging */
        ret_api = VehicleDebugSndMsg(pno,
                                     PNO_VEHICLE_SENSOR,
                                     CID_VEHICLEDEBUG_LOG_SET,
                                     (u_int16)sizeof(VEHICLEDEBUG_MSG_LOGINFO_DAT),
                                     (const void *)&data);
        if (ret_api == RET_NORMAL) {
            ret = VEHICLEDEBUG_RET_NORMAL;
        } else {
            ret = VEHICLEDEBUG_RET_ERROR;
        }
    } else {
        ret = VEHICLEDEBUG_RET_ERROR;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "severity == NULL");
    }

    return ret;
}

/*******************************************************************************
 * MODULE    : VehicleDebugSndMsg
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
static VEHICLEDEBUG_RET_API VehicleDebugSndMsg(PNO pno_src,
                                               PNO pno_dest, CID cid, u_int16 msg_len, const void *msg_data) {
    VEHICLEDEBUG_MSG_BUF    msg_buf;
    RET_API                 ret_api;

    if (msg_data != NULL) {
        /* Initialization message buffe */
        (void)memset(reinterpret_cast<void *>(&msg_buf), 0, sizeof(VEHICLEDEBUG_MSG_BUF));

        /*--------------------------------------------------------------*
         *    Create a message header                                        *
         *--------------------------------------------------------------*/
        msg_buf.hdr.hdr.sndpno         = pno_src;    /* source PNO    */
        msg_buf.hdr.hdr.cid            = cid;        /* Command ID    */
        msg_buf.hdr.hdr.msgbodysize    = msg_len;    /* Data size    */

        /*--------------------------------------------------------------*
         *    Create a message data                                *
         *--------------------------------------------------------------*/
        if (msg_len != 0) {
            (void)memcpy(reinterpret_cast<void *>(&msg_buf.data) , msg_data, sizeof(msg_buf.data));
        }
        /*--------------------------------------------------------------*
         * Send messages                                            *
         *--------------------------------------------------------------*/
        ret_api = _pb_SndMsg(pno_dest,                                               /* destination PNO    */
                             static_cast<u_int16>(sizeof(T_APIMSG_MSGBUF_HEADER) + msg_len),/* message size */
                             reinterpret_cast<void *><&msg_buf>,                     /* message buffer    */
                             0);                                                     /* Unused Argument    */
    } else {
        /* Argument error(Pointer to the data buffer) */
        ret_api = VEHICLEDEBUG_RET_ERROR_PARAM;
    }

    return ret_api;
}
