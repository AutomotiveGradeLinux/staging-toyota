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

#ifndef POSITIONING_CLIENT_INCLUDE_SENSOR_API_PRIVATE_H_
#define POSITIONING_CLIENT_INCLUDE_SENSOR_API_PRIVATE_H_
/******************************************************************************
 * File name        :Sensor_API_private.h
 * System name        :GPF
 * Subsystem name    Vehicle I/F library
 ******************************************************************************/

/************************************************************************
*            Macro definitions                                          *
************************************************************************/
#define SENSOR_PUBLIC_DID_NUM                25

/*----------------------------------------------------------------------*
 *    Shared Memory Related Extensions                                                     *
 *----------------------------------------------------------------------*/
#define SENSOR_SHARE_LOCK                    0xFF           /* Locking */
#define SENSOR_SHARE_UNLOCK                  0x00           /* Unlocking */

#define SENSOR_SHARE_BLOCK_MNUM              1              /* Number of shared memory control blocks */
#define SENSOR_SHARE_BLOCK_DNUM              10             /* Number of shared memory data blocks */
/* Number of shared memory blocks */
#define SENSOR_SHARE_BLOCK_NUM               (SENSOR_SHARE_BLOCK_MNUM+SENSOR_SHARE_BLOCK_DNUM)

#define SENSOR_SHARE_BLOCK_SIZE              512            /* Shared Memory Block Size(byte) */
#define SENSOR_SHARE_HEAD_SIZE               40             /* Vehicle Sensor Information Header( (4)+1+3+16*2) */
/* Shared Memory Data Size */
#define SENSOR_SHARE_BLOCK_DSIZE             (SENSOR_SHARE_BLOCK_SIZE - SENSOR_SHARE_HEAD_SIZE)
/* Size of shared memory allocation area */
#define SENSOR_SHARE_SIZE                    (SENSOR_SHARE_BLOCK_SIZE * SENSOR_SHARE_BLOCK_NUM)

#define SENSOR_SEMAPHO_NAME                  ("SENSOR_SHARE_SEMAPHO")            /* Semaphore name */
#define GPS_INTERRUPT_SIGNAL_SEMAPHO_NAME    ("GPS_INT_SIGNAL_SHARE_SEMAPHO")    /* Semaphore name(MAX 32Byte) */
#define GYRO_CONNECT_STATUS_SEMAPHO_NAME     ("GYRO_CONNECT_STTS_SHARE_SEMAPHO") /* Semaphore name(MAX 32Byte) */
#define EPHEMERIS_NUM_SEMAPHO_NAME           ("EPHEMERIS_NUM_SHARE_SEMAPHO")     /* Semaphore name(MAX 32Byte) */
#define LOCALTIME_SEMAPHO_NAME               ("LOCALTIME_SHARE_SEMAPHO")         /* Semaphore name(MAX 32Byte) */

/*----------------------------------------------------------------------*
 *    Event Related Extensions                                                       *
 *----------------------------------------------------------------------*/
#define SENSOR_EVENT_VAL_INIT                (SENSOR_RET_ERROR_MIN - 1)    /* Event initial value */

/*----------------------------------------------------------------------*
 *    Shared Memory Related Extensions                                                     *
 *----------------------------------------------------------------------*/
#define SENSOR_SHARE_NAME                    ("SENSOR_SHARE_MEMORY")            /* Shared memory name */
#define GPS_INTERRUPT_SIGNAL_SHARE_NAME      ("GPS_INT_SIGNAL_SHARE_MEMORY")    /* Shared memory name(MAX 32Byte) */
#define GYRO_CONNECT_STATUS_SHARE_NAME       ("GYRO_CONNECT_STTS_SHARE_MEMORY") /* Shared memory name(MAX 32Byte) */
#define EPHEMERIS_NUM_SHARE_NAME             ("EPHEMERIS_NUM_SHARE_MEMORY")     /* Shared memory name(MAX 32Byte) */
#define LOCALTIME_SHARE_NAME                 ("LOCALTIME_SHARE_MEMORY")         /* Shared memory name(MAX 32Byte) */

/*-----------------------------------------------------------------------------
 * GPS local CID define
 *----------------------------------------------------------------------------*/
#define CID_DEV_REQGPS                       MAKECID(CID_DEVHIGH, 0x00)    /* GPS request          */
#define CID_DEV_REQRST                       MAKECID(CID_DEVHIGH, 0x01)    /* GPS reset request */

#define CID_GPS_SERIAL0                      (CID)0x0100    /* GPS reset request function definition    */
/* GPS reset request CID        */

/*---------------------------------------------------------------------------
 * GPS error log output
 ---------------------------------------------------------------------------*/
#define GPS_DBGMSG                           0         /* Product Shipping Log Output(WPF_DEBUGMSG)            */
#define GPS_RTLMSG                           1         /* Development test log output(WPF_RETAILMSG)        */

/*----------------------------------------------------------------------*
 *    for message                                                       *
 *----------------------------------------------------------------------*/
#define CID_SENSORIF_PKG_DELIVERY_ENTRY_EXT  0x0602    /* sensor extra package register command ID        */

/*----------------------------------------------------------------------*
 *    for JudgeDid                                                      *
 *----------------------------------------------------------------------*/
#define MODE_REGISTER_LISTENER_PKG_SENS_DATA 0         /* POS_RegisterListenerPkgSensData        */
#define MODE_REGISTER_LISTENER_SENSDATA      1         /* POS_RegisterListenerSensData        */
#define MODE_GET_SENSDATA                    2         /* POS_GetSensData        */

/************************************************************************
*            Typedef definitions                                                *
************************************************************************/

/************************************************************************
*            Struct definitions                                                 *
************************************************************************/

/************************************************************************
* TAG       : SENSOR_SHARE_MNG
* ABSTRACT  : Shared memory management area
************************************************************************/
typedef struct {
    u_int8                        lock_info[SENSOR_SHARE_BLOCK_NUM];    /* Usages    */
    u_int8                        reserve[501];                         /* 512-11    */
} SENSOR_SHARE_MNG;

/************************************************************************
* TAG       : SENSOR_SHARE_BLOCK_DAT_DAT
* ABSTRACT  : Shared memory data area(Data section details)
************************************************************************/
typedef struct {
    u_int8                        pkg_num;                              /* Number of packages    */
    u_int8                        reserve[3];
    u_int16                       offset[SENSOR_PKG_DELIVERY_MAX];      /* Offset value */
    u_int8                        data[SENSOR_SHARE_BLOCK_DSIZE];       /* Data portion        */
} SENSOR_SHARE_BLOCK_DAT_DAT;

/************************************************************************
* TAG       : SENSOR_SHARE_BLOCK_DAT
* ABSTRACT  : Shared memory data area(One block)
************************************************************************/
typedef struct {
    u_int16                       size;                                 /* Size of the data    */
    u_int8                        reserve[2];
    SENSOR_SHARE_BLOCK_DAT_DAT    data;                                 /* Data portion        */
} SENSOR_SHARE_BLOCK_DAT;

/************************************************************************
* TAG       : SENSOR_SHARE_BLOCK_MNG
* ABSTRACT  : Areas of memory that are shared
************************************************************************/
typedef struct {
    SENSOR_SHARE_MNG              mng;                                  /* Shared memory management information */
    SENSOR_SHARE_BLOCK_DAT        data[SENSOR_SHARE_BLOCK_DNUM];        /* Shared memory data portion */
} SENSOR_SHARE;

/**
 * @brief Return code list structure
 */
typedef struct {
/*
 *  Note.
 *  This feature needs to be defined according to the unit type.
 */
    BOOL                          GRADE2_ret;    /**< Return code in GRADE2 */
    BOOL                          GRADE1_ret;    /**< Return code in GARDE1 */
    DID                           did;           /**< DID                   */
} SENSOR_RET_PKG;


/************************************************************************
*            Function prototype                                            *
************************************************************************/
RET_API PosDeleteEvent(EventID event_id);
RET_API SensorLinkShareData(void **share_top, u_int32 *share_size, u_int16 *offset);
RET_API SensorUnLinkShareData(SENSOR_SHARE *share_top, u_int16 offset);

EventID PosCreateEvent(PNO pno);
void SensorSetShareData(void *share_top, u_int16 offset, const void *data_src, u_int16 size_src);
RET_API PosSndMsg(PNO pno_src, PNO pno_dest, CID cid, u_int16 msg_len, const void *msg_data);

BOOL SensorJudgeDid(DID did, uint8_t mode);

/* Function macro */
#define SENSOR_DID_JUDGE_REGLIS_PKG(did)    SensorJudgeDid(did, MODE_REGISTER_LISTENER_PKG_SENS_DATA)
#define SENSOR_DID_JUDGE_REGLIS(did)        SensorJudgeDid(did, MODE_REGISTER_LISTENER_SENSDATA)
#define SENSOR_DID_JUDGE_GET(did)           SensorJudgeDid(did, MODE_GET_SENSDATA)

#endif  // POSITIONING_CLIENT_INCLUDE_SENSOR_API_PRIVATE_H_ */
