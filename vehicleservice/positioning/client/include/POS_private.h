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

/**
 * @file
 *  POS_private.h
 * @brief
 *  Inner functions of Positoning API
 *
 */
#ifndef POSITIONING_CLIENT_INCLUDE_POS_PRIVATE_H_
#define POSITIONING_CLIENT_INCLUDE_POS_PRIVATE_H_

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include <vehicle_service/POS_define.h>
#include "Vehicle_API_Dummy.h"
#include "Vehicle_API_private.h"
#include "Naviinfo_API.h"
#include "POS_common_private.h"
#include <asm/unistd.h>
#include <other_service/VP_GetEnv.h>
#include <other_service/env_vehicleparameterlibrary.h>
#include "CommonDefine.h"
/*---------------------------------------------------------------------------------*
 * Function Prototype                                                              *
 *---------------------------------------------------------------------------------*/
/* Shared Library Value Area Check Function */
inline UNIT_TYPE GetEnvSupportInfo(void);
inline BOOL ChkUnitType(UNIT_TYPE type);
inline RET_API MunMapDeviceIo(HANDLE dev, uint32_t map_size);
inline RET_API MilliSecSleep(uint32_t mill_time);
inline POS_RET_API PosChkParam8(int8_t data, int8_t min, int8_t max, const char* fn, int32_t line);
inline POS_RET_API PosChkParam16(int16_t data, int16_t min, int16_t max, const char* fn, int32_t line);
inline POS_RET_API PosChkParam32(int32_t data, int32_t min, int32_t max, const char* fn, int32_t line);
inline POS_RET_API PosChkParamU8(uint8_t data, uint8_t min, uint8_t max, const char* fn, int32_t line);
inline POS_RET_API PosChkParamU16(uint16_t data, uint16_t min, uint16_t max, const char* fn, int32_t line);
inline POS_RET_API PosChkParamU32(uint32_t data, uint32_t min, uint32_t max, const char* fn, int32_t line);

/* Shared Intra-Library Core Functions */
inline POS_RET_API    PosSetProc(DID did, void *p_data, uint16_t size, uint8_t is_event);
inline POS_RET_API    PosGetProc(DID did, void *p_data, uint16_t dest_size);
inline SENSOR_RET_API PosRegisterListenerProc(PCSTR notify_name,
                                              DID did, u_int8 ctrl_flg, u_int8 delivery_timing);

/* General Functions in Shared Libraries */
inline EventID VehicleCreateEvent(PNO pno);
inline RET_API VehicleDeleteEvent(EventID event_id);
inline RET_API VehicleLinkShareData(void **share_top, uint32_t *share_size, uint16_t *offset);
inline RET_API VehicleUnLinkShareData(VEHICLE_SHARE *share_top, uint16_t offset);
inline RET_API VehicleSndMsg(PNO pno_src, PNO pno_dest, CID cid, uint16_t msg_len, const void *msg_data);
inline BOOL VehicleGetResource(void);
inline void VehicleReleaseResource(void);
inline uint32_t GetTid(void);
/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/
#define POS_CHKPARAM8(data, min, max)   PosChkParam8(data, min, max, __func__, __LINE__)
#define POS_CHKPARAM16(data, min, max)  PosChkParam16(data, min, max, __func__, __LINE__)
#define POS_CHKPARAM32(data, min, max)  PosChkParam32(data, min, max, __func__, __LINE__)
#define POS_CHKPARAMU8(data, min, max)  PosChkParamU8(data, min, max, __func__, __LINE__)
#define POS_CHKPARAMU16(data, min, max) PosChkParamU16(data, min, max, __func__, __LINE__)
#define POS_CHKPARAMU32(data, min, max) PosChkParamU32(data, min, max, __func__, __LINE__)
#define POS_API_TIME_OUT_MS  5000 /* Timeout period(ms) */

/*---------------------------------------------------------------------------------*
 * Inline Functions                                                                *
 *---------------------------------------------------------------------------------*/
UNIT_TYPE GetEnvSupportInfo(void) {
    UNIT_TYPE  ret_type = UNIT_TYPE_GRADE1;

    char       env_area[VP_MAX_LENGTH];
    char       env_grade[VP_MAX_LENGTH];
    char*      p_env_grade    = env_grade;
    char*      p_env_area     = env_area;

    /*
     *  Note.
     *  This feature branches processing depending on the area and grade type.
     */

    VP_GetEnv(VP_VEHICLEPARAMETERLIBRARY_AREA, p_env_area);

    if (0 == strcmp(p_env_area, "AREA1")) {
        ret_type = UNIT_TYPE_GRADE2;
    } else if (0 == strcmp(p_env_area, "AREA2")) {
        memset(&env_grade, 0x00, sizeof(env_grade));
        VP_GetEnv(VP_VEHICLEPARAMETERLIBRARY_GRADE, p_env_grade);
        if (0 == strcmp(p_env_grade, "_CWORD95_") ||
            0 == strcmp(p_env_grade, "_CWORD101_") ||
            0 == strcmp(p_env_grade, "_CWORD61_")) {
            ret_type = UNIT_TYPE_GRADE2;
        }
    } else {
        // NOP
    }

    return ret_type;
}

BOOL ChkUnitType(UNIT_TYPE type) {
    UNIT_TYPE type_temp;
    BOOL ret;

    type_temp = GetEnvSupportInfo();
    if ((type_temp & type) != 0) {
        ret = TRUE;
    } else {
        ret = FALSE;
    }

    return ret;
}

RET_API MunMapDeviceIo(HANDLE dev, uint32_t map_size) {
    return RET_NORMAL;
}

RET_API MilliSecSleep(uint32_t mill_time) {
    switch (mill_time) {
        case 0:
        {
            /* Discard the time slice */
            sched_yield();
            break;
        }
        case INFINITE:
        {
            /* Abort processing indefinitely */
            while (1) {
                sleep(INFINITE);
            }
        }
        default:
            /* Sleep for Specified Time */
            usleep(mill_time * 1000);
            break;
    }

    return RET_NORMAL;
}

/**
 * @brief
 *  Data Valid Value Determination(int8)
 *
 * @param[in] date  int8_t       Object data
 * @param[in] min   int8_t       Target Data Valid Value Range(Minimum value)
 * @param[in] max   int8_t       Target Data Valid Value Range(Maximum value)
 * @param[in] fn    const char*  Pointer to the function name
 * @param[in] line  int32_t      Number of lines
 *
 * @return  POS_RET_NORMAL       Within the valid range<br>
 *          POS_RET_ERROR        Out of scope<br>
 *          POS_RET_ERROR_PARAM  Argument error
 */
inline POS_RET_API PosChkParam8(int8_t data, int8_t min, int8_t max, const char* fn, int32_t line) {
    POS_RET_API ret = POS_RET_NORMAL;

    if (fn == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Argument ERROR!! fn=%p", fn);
        ret = POS_RET_ERROR_PARAM;
    } else {
        if ((data < min) || (max < data)) {
            ret = POS_RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, 
                          "%s/%d/Out of range!! data=%d[%d, %d]",
                          fn, line, data, min, max);
        }
    }

    return ret;
}

/**
 * @brief
 *  Data Valid Value Determination(int16)
 *
 * @param[in] date  int16_t      Object data
 * @param[in] min   int16_t      Target Data Valid Value Range(Minimum value)
 * @param[in] max   int16_t      Target Data Valid Value Range(Maximum value)
 * @param[in] fn    const char*  Pointer to the function name
 * @param[in] line  int32_t      Number of lines
 *
 * @return  POS_RET_NORMAL       Within the valid range<br>
 *          POS_RET_ERROR        Out of scope<br>
 *          POS_RET_ERROR_PARAM  Argument error
 */
inline POS_RET_API PosChkParam16(int16_t data, int16_t min, int16_t max, const char* fn, int32_t line) {
    POS_RET_API ret = POS_RET_NORMAL;

    if (fn == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Argument ERROR!! fn=%p", fn);
        ret = POS_RET_ERROR_PARAM;
    } else {
        if ((data < min) || (max < data)) {
            ret = POS_RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, 
                          "%s/%d/Out of range!! data=%d[%d, %d]",
                          fn, line, data, min, max);
        }
    }

    return ret;
}

/**
 * @brief
 *  Data Valid Value Determination(int32)
 *
 * @param[in] date  int32_t      Object data
 * @param[in] min   int32_t      Target Data Valid Value Range(Minimum value)
 * @param[in] max   int32_t      Target Data Valid Value Range(Maximum value)
 * @param[in] fn    const char*  Pointer to the function name
 * @param[in] line  int32_t      Number of lines
 *
 * @return  POS_RET_NORMAL       Within the valid range<br>
 *          POS_RET_ERROR        Out of scope<br>
 *          POS_RET_ERROR_PARAM  Argument error
 */
inline POS_RET_API PosChkParam32(int32_t data, int32_t min, int32_t max, const char* fn, int32_t line) {
    POS_RET_API ret = POS_RET_NORMAL;

    if (fn == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Argument ERROR!! fn=%p", fn);
        ret = POS_RET_ERROR_PARAM;
    } else {
        if ((data < min) || (max < data)) {
            ret = POS_RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, 
                          "%s/%d/Out of range!! data=%d[%d, %d]",
                          fn, line, data, min, max);
        }
    }

    return ret;
}

/**
 * @brief
 *  Data Valid Value Determination(unsigned int8)
 *
 * @param[in] date  uint8_t      Object data
 * @param[in] min   uint8_t      Target Data Valid Value Range(Minimum value)
 * @param[in] max   uint8_t      Target Data Valid Value Range(Maximum value)
 * @param[in] fn    const char*  Pointer to the function name
 * @param[in] line  int32_t      Number of lines
 *
 * @return  POS_RET_NORMAL       Within the valid range<br>
 *          POS_RET_ERROR        Out of scope<br>
 *          POS_RET_ERROR_PARAM  Argument error
 */
inline POS_RET_API PosChkParamU8(uint8_t data, uint8_t min, uint8_t max, const char* fn, int32_t line) {
    POS_RET_API ret = POS_RET_NORMAL;

    if (fn == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Argument ERROR!! fn=%p", fn);
        ret = POS_RET_ERROR_PARAM;
    } else {
        if ((data < min) || (max < data)) {
            ret = POS_RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, 
                          "%s/%d/Out of range!! data=%u[%u, %u]",
                          fn, line, data, min, max);
        }
    }

    return ret;
}

/**
 * @brief
 *  Data Valid Value Determination(unsigned int16)
 *
 * @param[in] date  uint16_t     Object data
 * @param[in] min   uint16_t     Target Data Valid Value Range(Minimum value)
 * @param[in] max   uint16_t     Target Data Valid Value Range(Maximum value)
 * @param[in] fn    const char*  Pointer to the function name
 * @param[in] line  int32_t      Number of lines
 *
 * @return  POS_RET_NORMAL       Within the valid range<br>
 *          POS_RET_ERROR        Out of scope<br>
 *          POS_RET_ERROR_PARAM  Argument error
 */
inline POS_RET_API PosChkParamU16(uint16_t data, uint16_t min, uint16_t max, const char* fn, int32_t line) {
    POS_RET_API ret = POS_RET_NORMAL;

    if (fn == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Argument ERROR!! fn=%p", fn);
        ret = POS_RET_ERROR_PARAM;
    } else {
        if ((data < min) || (max < data)) {
            ret = POS_RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, 
                          "%s/%d/Out of range!! data=%u[%u, %u]",
                          fn, line, data, min, max);
        }
    }

    return ret;
}

/**
 * @brief
 *  Data Valid Value Determination(unsigned int32)
 *
 * @param[in] date  uint32_t     Object data
 * @param[in] min   uint32_t     Target Data Valid Value Range(Minimum value)
 * @param[in] max   uint32_t     Target Data Valid Value Range(Maximum value)
 * @param[in] fn    const char*  Pointer to the function name
 * @param[in] line  int32_t      Number of lines
 *
 * @return  POS_RET_NORMAL       Within the valid range<br>
 *          POS_RET_ERROR        Out of scope<br>
 *          POS_RET_ERROR_PARAM  Argument error
 */
inline POS_RET_API PosChkParamU32(uint32_t data, uint32_t min, uint32_t max, const char* fn, int32_t line) {
    POS_RET_API ret = POS_RET_NORMAL;

    if (fn == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Argument ERROR!! fn=%p", fn);
        ret = POS_RET_ERROR_PARAM;
    } else {
        if ((data < min) || (max < data)) {
            ret = POS_RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, 
                          "%s/%d/Out of range!! data=%u[%u, %u]",
                          fn, line, data, min, max);
        }
    }

    return ret;
}

/**
 * @brief
 *  Data setting process(Internal processing)
 *
 *  Set the specified information
 *
 * @param[in] did      DID         - Data ID
 * @param[in] p_data    void*       - Configuration data
 * @param[in] size     uint16_t    - Setting data size
 * @param[in] is_event  uint8_t     - Event wait status(TRUE/FALSE)
 *
 * @return  POS_RET_NORMAL           Normal completion(Include illegal)<br>
 *          POS_RET_ERROR_PARAM      Parameter error<br>
 *          POS_RET_ERROR_INNER      Internal error
 */
inline POS_RET_API PosSetProc(DID did, void *p_data, uint16_t size, uint8_t is_event) {
    POS_RET_API    ret = POS_RET_NORMAL;    /* Return value of this function */
    RET_API      ret_api = RET_NORMAL;    /* API return value */
    POS_MSGINFO    snd_msg;          /* Message */
    EventID      event_id;          /* Event ID */
    int32_t      event_val;          /* Event value */
    PNO        pno;            /* Calling thread PNO */
    uint32_t    pid;            /* Process ID */
    uint32_t    tid;            /* Thread ID */
    char      name[128];

    /* Data size check */
    if (POS_MSG_INFO_DSIZE < size) {
        ret = POS_RET_ERROR_PARAM;
    } else {
        /* Resource acquisition */
        if (VehicleGetResource() == TRUE) {
            /* Message data */
            /* _CWORD71_ processing speed(Memset modification) */
            /* Initialization of areas that do not contain values */
            snd_msg.pno = 0;
            snd_msg.rcv_flag = 0;
            snd_msg.reserve = 0;

            snd_msg.did  = did;
            snd_msg.size  = size;
            memcpy(snd_msg.data, p_data, size);

            if (TRUE == is_event) {
                /* After requesting data setting,Wait for the setting completion(Event Wait) */

                /* Event Generation */
                pid = static_cast<uint32_t>(getpid());
                tid = GetTid();

                snprintf(name, sizeof(name), "PS_p%u_t%u", pid, tid);
                pno = _pb_CnvName2Pno(name);
                event_id = VehicleCreateEvent(pno);

                /* Set the source Pno of message data */
                snd_msg.pno = pno;

                if (0 != event_id) {
                    /* Successful event generation */

                    /* Send NAVI Sensor Data Setting to Vehicle Sensor */
                    ret_api = VehicleSndMsg(pno,
                                            PNO_VEHICLE_SENSOR,
                                            CID_POSIF_SET_DATA,

                                            (uint16_t)sizeof(POS_MSGINFO) - POS_MSG_INFO_DSIZE + snd_msg.size, /* variable length */
                                            (const void *)&snd_msg);

                    if (RET_NORMAL == ret_api) {
                        /* If the data setup process is successful,Wait for a completion event */
                        ret_api = _pb_WaitEvent(event_id,
                                                SAPI_EVWAIT_VAL,
                                                VEHICLE_RET_ERROR_MIN,
                                                VEHICLE_RET_NORMAL, &event_val, POS_API_TIME_OUT_MS);
                        if (RET_NORMAL != ret_api) {
                            /* Return an internal error */
                            ret = POS_RET_ERROR_INNER;
                        } else {
                            /* Return from Event Wait */
                            /* Set event value (processing result) as return value */
                            ret = (POS_RET_API)event_val;
                        }
                    } else {
                        /* Message transmission processing failed */
                        ret = POS_RET_ERROR_INNER;
                    }
                    /* Event deletion */
                    ret_api = VehicleDeleteEvent(event_id);
                } else {
                    /* Event generation failure */
                    ret = POS_RET_ERROR_INNER;
                }
            } else {
                /* After setting data,Immediate termination */

                /* Send NAVI Sensor Data Setting to Vehicle Sensor */
                ret_api = _pb_SndMsg_Ext(POS_THREAD_NAME,
                                         CID_POSIF_SET_DATA,
                                         sizeof(POS_MSGINFO), reinterpret_cast<void*>(&snd_msg), 0);
                if (ret_api != RET_NORMAL) {
                    /* Message transmission failure */
                    ret = POS_RET_ERROR_INNER;
                }
            }
        } else {
            /* Insufficient resource */
            ret = POS_RET_ERROR_RESOURCE;
        }
        /* Resource release */
        VehicleReleaseResource();
    }

    return ret;
}

/**
 * @brief
 *  Sensor information acquisition(Internal processing)
 *
 * @param[in] did         DID      - Data ID for vehicle information
 * @param[in] p_data     void*    - Pointer representing the storage destination of vehicle sensor information
 * @param[in] dest_size    uint16_t - Storage destination size of vehicle sensor information(byte)
 *
 * @return  0 or more                        Stored data size(Include illegal)<br>
 *          POS_RET_ERROR_CREATE_EVENT   Event generation failure<br>
 *          POS_RET_ERROR_OUTOF_MEMORY   Shared memory allocation failed<br>
 *          POS_RET_ERROR_SIZE           Storage destination size error<br>
 *          POS_RET_ERROR_DID            Unregistered ID<br>
 */
inline POS_RET_API PosGetProc(DID did, void *p_data, uint16_t dest_size) {
    POS_RET_API       ret;              /* Return value            */
    RET_API           ret_api;          /* System API return value        */
    EventID           event_id;         /* Event ID           */
    int32_t           event_val;        /* Event value           */
    void             *share_top;        /* Start address of shared memory       */
    uint32_t          share_size;       /* Size of shared memory area       */
    uint16_t          offset;                /* Offset to free shared memory area    */
    VEHICLE_SHARE_BLOCK_DAT *share_dat; /* Address of free shared memory area  */
    VEHICLE_MSG_GET_VEHICLE_DATA_DAT data; /* Message data     */
    PNO                  pno;           /* Calling thread PNO       */
    uint32_t    pid;          /* Process ID          */
    uint32_t    tid;          /* Thread ID            */
    char      name[128];

    /* Resource acquisition */
    if (VehicleGetResource() == TRUE) {
        /* Initialization */
        event_id = 0;
        event_val = 0;
        memset(reinterpret_cast<void *>(&data), 0, sizeof(VEHICLE_MSG_GET_VEHICLE_DATA_DAT));

        /* Event Generation */
        pid = static_cast<uint32_t>(getpid());
        tid = GetTid();

        snprintf(name, sizeof(name), "PG_p%u_t%u", pid, tid);
        pno = _pb_CnvName2Pno(name);
        event_id = VehicleCreateEvent(pno);

        if (0 != event_id) {
            /* Successful event generation */

            /* Allocate shared memory */
            ret_api = VehicleLinkShareData(reinterpret_cast<void **>(&share_top), &share_size, &offset);
            if (RET_NORMAL != ret_api) {
                /* Failed to allocate shared memory */
                ret = POS_RET_ERROR_OUTOF_MEMORY;
            } else { /* When the shared memory is allocated successfully */
                /* Calculate start address of free shared memory area */
                share_dat = reinterpret_cast<VEHICLE_SHARE_BLOCK_DAT *>(reinterpret_cast<u_int8 *>(share_top)
                            + offset);

                /* Send vehicle sensor information acquisition message */
                data.did        = did;
                data.pno        = pno;
                data.offset     = offset;
                data.size       = VEHICLE_SHARE_BLOCK_DSIZE;

                /* Messaging */
                ret_api = VehicleSndMsg(pno,
                                        PNO_VEHICLE_SENSOR,
                                        CID_VEHICLEIF_GET_VEHICLE_DATA,
                                        sizeof(VEHICLE_MSG_GET_VEHICLE_DATA_DAT), (const void *)&data);

                /* Message transmission processing is successful */
                if (RET_NORMAL == ret_api) {
                    /* Wait for completion event from vehicle sensor thread */
                    ret_api = _pb_WaitEvent(event_id,
                                            SAPI_EVWAIT_VAL,
                                            VEHICLE_RET_ERROR_MIN,
                                            VEHICLE_RET_NORMAL, &event_val, POS_API_TIME_OUT_MS);

                    if (RET_NORMAL != ret_api) {
                        /* Return an internal error */
                        ret = POS_RET_ERROR_INNER;
                    } else { /* Return from Event Wait */
                        /* Link to shared memory */
                        ret_api = _pb_LinkShareData(const_cast<char *>(VEHICLE_SHARE_NAME),
                                                    &share_top, &share_size);

                        /* Calculate the address of the shared memory storage area. */
                        share_dat = reinterpret_cast<VEHICLE_SHARE_BLOCK_DAT *>(reinterpret_cast<u_int8 *>(share_top)
                                    + offset);

                        if (event_val < 0) {
                            /* Vehicle sensor information acquisition failure */
                            ret = (VEHICLE_RET_API)event_val;
                        } else if (RET_NORMAL != ret_api) {
                            /* Shared memory error */
                            ret = POS_RET_ERROR_OUTOF_MEMORY;
                        } else if (dest_size < share_dat->size) {
                            /* Storage destination size error */
                            ret = POS_RET_ERROR_SIZE;
                        } else { /* Vehicle sensor information acquisition success */
                            /* Copy from shared memory to user memory */
                            memcpy(p_data, share_dat->data, (size_t)share_dat->size);

                            /* Set Write Size to Return Value */
                            ret = static_cast<int32>(share_dat->size);
                        }
                    }
                } else { /* Message transmission processing failed */
                    /* Return an event generation failure */
                    ret = POS_RET_ERROR_CREATE_EVENT;
                }
                /* Free shared memory */
                (void)VehicleUnLinkShareData(reinterpret_cast<VEHICLE_SHARE*>(share_top), offset);
            }
            /* Event deletion */
            ret_api = VehicleDeleteEvent(event_id);
        } else {
            /* Event generation failure */
            ret = POS_RET_ERROR_CREATE_EVENT;
        }
    } else {
        /* Insufficient resource */
        ret = POS_RET_ERROR_RESOURCE;
    }
    /* Resource release */
    VehicleReleaseResource();

    return ret;
}

/**
 * @brief
 *  Delivery registration process(Internal processing)
 *
 * @param[in]  notify_name      Destination thread name
 * @param[in]  did        Pointer to an array of data IDs for vehicle information
 * @param[in]  ctrl_flg      Delivery control<br>
 *        Delivery registration: SENSOR_DELIVERY_REGIST<br>
 *        Delivery stop: SENSOR_DELIVERY_STOP    (Note: Not mounted)<br>
 *        Resume delivery: SENSOR_DELIVERY_RESTART  (Note: Not mounted)
 * @param[in]  delivery_timing    Delivery timing<br>
 *        Updating  : SENSOR_DELIVERY_TIMING_UPDATE<br>
 *        Changing  : SENSOR_DELIVERY_TIMING_CHANGE
 *
 * @return  SENSOR_RET_NORMAL        Successful registration<br>
 *      SENSOR_RET_ERROR_CREATE_EVENT  Event generation failure<br>
 *      SENSOR_RET_ERROR_PARAM      Parameter error<br>
 *      SENSOR_RET_ERROR_DID      Unregistered ID<br>
 *      SENSOR_RET_ERROR_BUFFULL    FULL of delivery registers<br>
 *      SENSOR_RET_ERROR_INNER      Internal abnormality<br>
 */
inline SENSOR_RET_API PosRegisterListenerProc(PCSTR notify_name, DID did, u_int8 ctrl_flg, u_int8 delivery_timing) {
    SENSOR_RET_API          ret;      /* Return value */
    RET_API              ret_api;    /* System API return value */
    EventID              event_id;    /* Event ID */
    int32              event_val;    /* Event value */
    VEHICLE_MSG_DELIVERY_ENTRY_DAT  data;      /* Message data */
    PNO                pno;      /* Converted internal PNO  */

    /* Resource acquisition */
    if (VehicleGetResource() == TRUE) {
        /* Initialization */
        event_id = 0;
        event_val = 0;

        /* Get PNO from Thread Name */
        pno = _pb_CnvName2Pno(notify_name);

        /* Event Generation */
        event_id = VehicleCreateEvent(pno);

        if (0 != event_id) {
            /* Successful event generation */

            /*--------------------------------------------------------------*
             *  Send Vehicle Sensor Information Delivery Registration Message          *
             *--------------------------------------------------------------*/
            /* Create Message Data */
            data.did      = did;
            data.pno      = pno;
            data.delivery_timing  = delivery_timing;
            data.ctrl_flg  = ctrl_flg;
            data.event_id   = event_id;

            /* Messaging */
            ret_api = VehicleSndMsg(pno,
                                    PNO_VEHICLE_SENSOR,
                                    CID_VEHICLEIF_DELIVERY_ENTRY,
                                    (uint16_t)sizeof(VEHICLE_MSG_DELIVERY_ENTRY_DAT), (const void *)&data);

            if (RET_NORMAL == ret_api) {
                /* Message transmission processing is successful */
                /* Wait for completion event from vehicle sensor thread */
                ret_api = _pb_WaitEvent(event_id,
                                        SAPI_EVWAIT_VAL,
                                        VEHICLE_RET_ERROR_MIN,
                                        VEHICLE_RET_NORMAL, &event_val, POS_API_TIME_OUT_MS);
                if (RET_NORMAL != ret_api) {
                    /* Return an internal error */
                    ret = SENSOR_RET_ERROR_INNER;
                } else {
                    /* Return from Event Wait */
                    /* Set event value (processing result) as return value */
                    ret = (SENSOR_RET_API)event_val;
                }
            } else {
                /* Message transmission processing failed */
                /* Return an internal error */
                ret = SENSOR_RET_ERROR_INNER;
            }
            /* Event deletion */
            ret_api = VehicleDeleteEvent(event_id);
        } else {
            /* Event generation failure */
            ret = SENSOR_RET_ERROR_CREATE_EVENT;
        }
    } else {
        /* Insufficient resource */
        ret = SENSOR_RET_ERROR_RESOURCE;
    }
    /* Resource release */
    VehicleReleaseResource();

    return ret;
}

/*******************************************************************************
 * MODULE    : VehicleCreateEvent
 * ABSTRACT  : Event creation process
 * FUNCTION  : Generate an event
 * ARGUMENT  : pno    : Thread ID
 * NOTE      :
 * RETURN    : Non-zero    : Event ID
 *           : Zero      : Event generation failure
 ******************************************************************************/
inline EventID VehicleCreateEvent(PNO pno) {
    EventID    event_id;              /* Event ID */
    char    event_name[32];          /* Event name character string buffer */
    RET_API    ret_api;              /* System API return value */


    /* Initialization of event name character string buffer */
    memset(reinterpret_cast<void *>(event_name), 0, sizeof(event_name));
    /* Event name creation */
    snprintf(event_name, sizeof(event_name), "VEHICLE_%X", pno);

    /* Event Generation */
    event_id = _pb_CreateEvent(FALSE, 0, event_name);

    if (0 != event_id) {
        /* For successful event generation */

        /* Initialize the event */
        ret_api = _pb_SetEvent(event_id, SAPI_EVSET_ABSOLUTE, VEHICLE_EVENT_VAL_INIT);
        if (RET_NORMAL != ret_api) {
            /* Event initialization failed */
            /* Delete Event and Return Event Generation Failed */
            ret_api = VehicleDeleteEvent(event_id);
            event_id = 0;
        }
    }

    return(event_id);
}

/*******************************************************************************
 * MODULE    : VehicleDeleteEvent
 * ABSTRACT  : Event deletion processing
 * FUNCTION  : Delete events
 * ARGUMENT  : event_id  : Event ID of the event to delete
 * NOTE      :
 * RETURN    : RET_NORMAL  : Normal completion
 *           : RET_EV_NONE  : Specified event does not exist
 ******************************************************************************/
inline RET_API VehicleDeleteEvent(EventID event_id) {
    return(_pb_DeleteEvent(event_id));
}

/*******************************************************************************
 * MODULE    : VehicleLinkShareData
 * ABSTRACT  : Link to shared memory
 * FUNCTION  : Link to shared memory
 * ARGUMENT  : **share_top  : Storage destination of shared memory top address
 *           : *share_size  : Storage destination of shared memory area size
 *           : *offset  : Offset storage destination to free shared memory area
 * NOTE      :
 * RETURN    : RET_NORMAL  : Normal completion
 *           : RET_ERROR  : There is no shared memory area.
 ******************************************************************************/
inline RET_API VehicleLinkShareData(void **share_top, uint32_t *share_size, uint16_t *offset) {
    RET_API      ret_api;            /* System API return value */
    SemID      sem_id;              /* Semaphore ID */
    VEHICLE_SHARE  *share_top_tmp;
    int32      i;

    /* Initialization */
    ret_api = RET_ERROR;

    /* Create Semaphore */
    sem_id = _pb_CreateSemaphore(const_cast<char *>(VEHICLE_SEMAPHO_NAME));
    if (0 != sem_id) {
        /* Semaphore Lock */
        ret_api = _pb_SemLock(sem_id);
        if (RET_NORMAL == ret_api) {
            /* Link to shared memory */
            ret_api = _pb_LinkShareData(const_cast<char *>(VEHICLE_SHARE_NAME), share_top, share_size);
            if (RET_NORMAL == ret_api) {
                /* By searching the free shared memory area,Offset is calculated if there is free space. */
                share_top_tmp = reinterpret_cast<VEHICLE_SHARE *>(*share_top);

                /* Because the first block of the shared memory area is the control area,Loop from i = 1 */
                for (i = 1; i < VEHICLE_SHARE_BLOCK_NUM; i++) {
                    if (VEHICLE_SHARE_UNLOCK == share_top_tmp->mng.lock_info[i]) {
                        break;
                    }
                }
                if (i < VEHICLE_SHARE_BLOCK_NUM) {
                    /* Empty space */
                    /* Lock the block */
                    share_top_tmp->mng.lock_info[i] = VEHICLE_SHARE_LOCK;

                    /* Calculate the offset to the block */
                    *offset = static_cast<u_int16>(i * VEHICLE_SHARE_BLOCK_SIZE);

                    /* Normal completion */
                    ret_api = RET_NORMAL;
                } else {
                    /* No free space */
                    ret_api = RET_ERROR;
                }
            } else {
                /* Failed link to shared memory */
                ret_api = RET_ERROR;
            }
            /* Semaphore unlock */
            _pb_SemUnlock(sem_id);
        } else {
            /* Semaphore lock failed */
            ret_api = RET_ERROR;
        }
    } else {
        /* Semaphore creation failed */
        ret_api = RET_ERROR;
    }

    return(ret_api);
}

/*******************************************************************************
 * MODULE    : VehicleUnLinkShareData
 * ABSTRACT  : Unlinking shared memory
 * FUNCTION  : Unlink shared memory
 * ARGUMENT  : *share_top  : Start address of shared memory
 *           : offset    : Offset to shared memory free area
 * NOTE      :
 * RETURN    : RET_NORMAL  : Normal completion
 *           : RET_ERROR  : There is no shared memory area./semaphore error
 ******************************************************************************/
inline RET_API VehicleUnLinkShareData(VEHICLE_SHARE *share_top, uint16_t offset) {
    RET_API      ret_api;            /* System API return value */
    SemID      sem_id;              /* Semaphore ID */
    int32      i;

    /* Initialization */
    ret_api = RET_ERROR;

    /* Create Semaphore */
    sem_id = _pb_CreateSemaphore(const_cast<char *>(VEHICLE_SEMAPHO_NAME));
    if (0 != sem_id) {
        /* Semaphore Lock */
        ret_api = _pb_SemLock(sem_id);
        if (RET_NORMAL == ret_api) {
            /* Unlock the block */
            i = static_cast<int32>(offset) / VEHICLE_SHARE_BLOCK_SIZE;
            share_top->mng.lock_info[i] = VEHICLE_SHARE_UNLOCK;

            /* Semaphore unlock */
            _pb_SemUnlock(sem_id);

            /* Normal completion */
            ret_api = RET_NORMAL;
        } else {
            /* Semaphore lock failed */
            ret_api = RET_ERROR;
        }
    } else {
        /* Semaphore creation failed */
        ret_api = RET_ERROR;
    }

    return(ret_api);
}

/*******************************************************************************
 * MODULE    : VehicleSndMsg
 * ABSTRACT  : Message transmission processing
 * FUNCTION  : Send a message to the specified PNO
 * ARGUMENT  : pno_src      : Source PNO
 *           : pno_dest    : Destination PNO
 *           : cid      : Command ID
 *           : msg_len      : Message data body length
 *           : *msg_data    : Pointer to message data
 * NOTE      :
 * RETURN    : RET_NORMAL    : Normal completion
 *           : RET_ERRNOTRDY  : Destination process is not wakeup
 *           : RET_ERRMSGFULL  : Message queue overflows
 *           : RET_ERRPARAM    : Buffer size error
 ******************************************************************************/
inline RET_API VehicleSndMsg(PNO pno_src, PNO pno_dest, CID cid, uint16_t msg_len, const void *msg_data) {
    VEHICLE_MSG_BUF      msg_buf;    /* message buffer */
    T_APIMSG_MSGBUF_HEADER  *msg_hdr;    /* Pointer to the message header */
    RET_API          ret_api;    /* Return value */
    PCSTR          thread_name;  /* Destination thread name        */


    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, 
                  "+ [pno_src = 0x%x, pno_dest = 0x%x]", pno_src, pno_dest);

    /* _CWORD71_ processing speed(Memset modification) */
    /* Initializing the header of the message buffer */
    memset(reinterpret_cast<void *>(&msg_buf.hdr), 0, sizeof(T_APIMSG_MSGBUF_HEADER));

    /* Get pointer to send buffer */
    msg_hdr = reinterpret_cast<T_APIMSG_MSGBUF_HEADER *>(reinterpret_cast<void *>(&msg_buf));

    /*--------------------------------------------------------------*
     *  Create message headers                  *
     *--------------------------------------------------------------*/
    msg_hdr->hdr.sndpno    = pno_src;    /* Source PNO */
    msg_hdr->hdr.cid      = cid;    /* Command ID */
    msg_hdr->hdr.msgbodysize  = msg_len;    /* Message data body length */

    /*--------------------------------------------------------------*
     *  Create message data                  *
     *--------------------------------------------------------------*/
    if ((0 != msg_data) && (0 != msg_len)) {
        /* Set the message data */
        memcpy(reinterpret_cast<void *>(msg_buf.data), msg_data, (size_t)msg_len);
    }
    /*--------------------------------------------------------------*
     * Send messages                      *
     *--------------------------------------------------------------*/
    /* Get Thread Name from PNO */
    if (pno_dest <= SYS_PNO_MAX) {
        thread_name = POS_THREAD_NAME;
    } else {
        thread_name = _pb_CnvPno2Name(pno_dest);
    }

    if ((pno_dest <= SYS_PNO_MAX) && (pno_src <= SYS_PNO_MAX)) {
        /* Internal debug log output */
        FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "[LOG pno_dest = 0x%x]", pno_dest);

        /* Internal Process Transmission and Reception Messages */
        ret_api = _pb_SndMsg(pno_dest,
                             (uint16_t)(sizeof(T_APIMSG_MSGBUF_HEADER) + msg_len),
                             reinterpret_cast<void *>(&msg_buf), 0);
    } else {
        /* External Process Transmission and Reception Messages */
        ret_api = _pb_SndMsg_Ext(thread_name,
                                 cid,
                                 (uint16_t)(msg_len),  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
                                 reinterpret_cast<void *>(&(msg_buf.data)), 0);
    }
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "-");

    return(ret_api);
}

/**
 * @brief
 *  Resource Acquisition Decision
 *
 * @param[in] none
 *
 * @return  TRUE       Normal<br>
 *          FALSE      Abnormality(Insufficient resource)
 */
inline BOOL VehicleGetResource(void) {
    BOOL ret[4] = {TRUE, TRUE, TRUE, TRUE};
    uint8_t idx;

    ret[1] = _pb_GetMsgResource();
    ret[2] = _pb_GetMutexResource();
    ret[3] = _pb_GetOtherResource();

    for (idx = 1; idx < 4; idx++) {
        if (ret[idx] == FALSE) {
            ret[0] = FALSE;
        }
    }

    return ret[0];
}

/**
 * @brief
 *  Resource release
 *
 * @param[in] none
 *
 * @return  none
 */
inline void VehicleReleaseResource(void) {
    _pb_ReleaseMsgResource();
    _pb_ReleaseMutexResource();
    _pb_ReleaseOtherResource();

    return;
}

inline uint32_t GetTid(void) {
    return (uint32_t)syscall(__NR_gettid);
}


#endif  // POSITIONING_CLIENT_INCLUDE_POS_PRIVATE_H_
