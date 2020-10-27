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

#ifndef POSITIONING_CLIENT_INCLUDE_VEHICLE_API_PRIVATE_H_
#define POSITIONING_CLIENT_INCLUDE_VEHICLE_API_PRIVATE_H_
/******************************************************************************
 * File name        :Vehicle_API_private.h
 * System name        :GPF
 * Subsystem name    :Vehicle I/F library
 ******************************************************************************/
#include <vehicle_service/POS_sensor_API.h>
#include "Vehicle_API.h"

/************************************************************************
*            Macro definitions                                                 *
************************************************************************/

/*----------------------------------------------------------------------*
 *    Shared Memory Related Extensions                                                     *
 *----------------------------------------------------------------------*/
#define VEHICLE_SHARE_LOCK           0xFF                            /* Locking */
#define VEHICLE_SHARE_UNLOCK         0x00                            /* Unlocking */

#define VEHICLE_SHARE_BLOCK_MNUM     1                               /* Number of shared memory control blocks */
#define VEHICLE_SHARE_BLOCK_DNUM     10                              /* Number of shared memory data blocks */
/* Number of shared memory blocks */
#define VEHICLE_SHARE_BLOCK_NUM      (VEHICLE_SHARE_BLOCK_MNUM+VEHICLE_SHARE_BLOCK_DNUM)

#define VEHICLE_SHARE_BLOCK_SIZE     512                             /* Shared Memory Block Size(byte) */
#define VEHICLE_SHARE_BLOCK_DSIZE    SENSOR_MSG_VSINFO_DSIZE         /* Shared Memory Data Size */
/* Size of shared memory allocation area */
#define VEHICLE_SHARE_SIZE           (VEHICLE_SHARE_BLOCK_SIZE * VEHICLE_SHARE_BLOCK_NUM)

#define VEHICLE_SEMAPHO_NAME         ("VEHICLE_SHARE_SEMAPHO")       /* Semaphore name */

#define VEHICLE_INVALID              0                               /* Disabled */
#define VEHICLE_EFFECTIVE            1                               /* Enabled */
#define VEHICLE_BIT31                0x80000000

/* ++ PastModel002 compliant_error compliant */
/* Message Data Size(byte) */
#define VEHICLE_VSINFO_DSIZE         504                             /* Vehicle sensor information        */
/* -- PastModel002 compliant_error compliant */

/*----------------------------------------------------------------------*
 *    Event Related Extensions                                                       *
 *----------------------------------------------------------------------*/
#define VEHICLE_EVENT_VAL_INIT       (VEHICLE_RET_ERROR_MIN-1)       /* Event initial value        */

/*----------------------------------------------------------------------*
 *    Shared Memory Related Extensions                                                       *
 *----------------------------------------------------------------------*/
#define VEHICLE_SHARE_NAME           ("POS_VEHICLE_SHARE_MEMORY")    /* Shared memory name */

/************************************************************************
*            Typedef definitions                                                *
************************************************************************/

/************************************************************************
*            Struct definitions                                                 *
************************************************************************/

/* ++ PastModel002 compliant_error compliant */

/************************************************************************
* TAG       : VEHICLE_MSG_VSINFO_DAT
* ABSTRACT  : Vehicle sensor information notification message(-> User)
************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER     hdr;                                   /* Message header    */
    VEHICLE_MSG_VSINFO_DAT     data;                                  /* Message data    */
} VEHICLE_MSG_VSINFO;

/* -- PastModel002 compliant_error compliant */

/************************************************************************
* TAG       : VEHICLE_SHARE_MNG
* ABSTRACT  : Shared memory management area
************************************************************************/
typedef struct {
    u_int8                     lock_info[VEHICLE_SHARE_BLOCK_NUM];    /* Usages    */
    u_int8                     reserve[501];
} VEHICLE_SHARE_MNG;

/************************************************************************
* TAG       : VEHICLE_SHARE_BLOCK_DAT
* ABSTRACT  : Shared memory data area(One block)
************************************************************************/
typedef struct {
    u_int16                    size;                                  /* Size of the data    */
    u_int8                     reserve[2];
    u_int8                     data[VEHICLE_SHARE_BLOCK_DSIZE];       /* Data        */
} VEHICLE_SHARE_BLOCK_DAT;

/************************************************************************
* TAG       : VEHICLE_SHARE_BLOCK_MNG
* ABSTRACT  : Areas of memory that are shared
************************************************************************/
typedef struct {
    VEHICLE_SHARE_MNG          mng;                                   /* Shared memory management information */
    VEHICLE_SHARE_BLOCK_DAT    data[VEHICLE_SHARE_BLOCK_DNUM];        /* Shared memory data portion */
} VEHICLE_SHARE;

/************************************************************************
*            Function prototype                                              *
************************************************************************/
RET_API VehicleDeleteEvent(EventID event_id);
RET_API VehicleLinkShareData(void **share_top, uint32_t *share_size, uint16_t *offset);
RET_API VehicleUnLinkShareData(VEHICLE_SHARE *share_top, u_int16 offset);
EventID VehicleCreateEvent(PNO pno);
void PosSetShareData(void *share_top, u_int16 offset, const void *data_src, u_int16 size_src);
RET_API VehicleSndMsg(PNO pno_src, PNO pno_dest, CID cid, u_int16 msg_len, const void *msg_data);

SENSOR_RET_API PosRegisterListenerProc(PCSTR notify_name, DID did, u_int8 ctrl_flg, u_int8 delivery_timing);

#endif  // POSITIONING_CLIENT_INCLUDE_VEHICLE_API_PRIVATE_H_ */
