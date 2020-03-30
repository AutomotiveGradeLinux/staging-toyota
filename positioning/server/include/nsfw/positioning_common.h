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

/**
 * @file
 *	positioning_common.h
 * @brief
 *  Positioing NSFW Dependency Section Common Headers
 */
#ifndef POSITIONING_SERVER_INCLUDE_NSFW_POSITIONING_COMMON_H_
#define POSITIONING_SERVER_INCLUDE_NSFW_POSITIONING_COMMON_H_

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/
/* Thread control command ID */
#define CID_THREAD_CREATE_COMP        (0x0001) /* Thread start completion notification */
#define CID_THREAD_STOP_REQ           (0x0002) /* Thread termination request     */
#define CID_THREAD_STOP_COMP          (0x0003) /* Thread termination notice */

#define CID_EXTTERM_REQ               (0x0011) /* External pin status request     */

/* Internal thread activation status determination */
#define THREAD_STS_MSK_POS_MAIN            (0x01)
#define THREAD_STS_MSK_POS_SENS            (0x02)
#define THREAD_STS_MSK_POS_GPS             (0x04)
#define THREAD_STS_MSK_POS_GPS_RECV        (0x08)
#define THREAD_STS_MSK_POS_GPS_ROLLOVER    (0x10)


/*---------------------------------------------------------------------------------*
 * ENUMERATION                                                                     *
 *---------------------------------------------------------------------------------*/
/*!
   @brief   Thread ID
*/
typedef enum {
    ETID_POS_MAIN = 0,
    ETID_POS_SENS,
    ETID_POS_GPS,
    ETID_POS_GPS_RECV,
    ETID_POS_GPS_ROLLOVER,
    ETID_POS_MAX
} EnumTID_POS;

/*!
   @brief   Positioning operating status definitions
*/
typedef enum {
  EPOS_EXE_STS_STOP = 0,             /* Stopped                               */
  EPOS_EXE_STS_RUNNING,              /* Running (From FrameworkunifiedOnStart to FrameworkunifiedOnStop) */
  EPOS_EXE_STS_RUNNING_COLDSTART     /* Running after cold start             */
} EnumExeSts_POS;

/*!
   @brief   Positioning Thread Startup Modes
*/
typedef enum {
  EPOS_SETUP_MODE_NORMAL = 0,        /* Normal start           */
  EPOS_SETUP_MODE_DATA_RESET         /* Data reset start */ /* QAC 930 */
} EnumSetupMode_POS;

/*---------------------------------------------------------------------------------*
 * STRUCTURE                                                                       *
 *---------------------------------------------------------------------------------*/
/*!
   @brief   Thread activation information
*/
typedef struct {
  EnumSetupMode_POS e_mode;           /* Thread activation mode */
} ST_THREAD_SETUP_INFO;


//    Duplicate/Confrict countermeasure
/* NMEA commands ****************************/
#define GPS_NMEA_NUM_SENTENCE           16

/*!
    @brief GPS NMEA sentence information
*/
typedef struct GpsNmeaSentenceInfo {
  u_int8                  uc_size;              /* Size of the sentence          */
  u_int8                  reserve;              /* Reserved                    */
  u_int16                 us_offset;            /* Beginning of the sentence        */
} TG_GPS_NMEA_SENTENCE_INFO;

/*!
    @brief GPS NMEA information
*/
typedef struct GpsNmeaInfo {
  u_int32                   ul_rcvsts;                                    /* NMEA received data                */
  u_int8                    reserve[4];                                   /* Reserved                    */
  TG_GPS_NMEA_SENTENCE_INFO st_nmea_sentence_info[GPS_NMEA_NUM_SENTENCE]; /* Sentence information              */
} TG_GPS_NMEA_INFO;


/*---------------------------------------------------------------------------------*
 * Prototype                                                                       *
 *---------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
    EnumSetupMode_POS PosSetupThread(HANDLE h_app, EnumTID_POS e_tid);
    void PosTeardownThread(EnumTID_POS e_tid);
#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------------*
 * Extern                                                                          *
 *---------------------------------------------------------------------------------*/
extern BOOL g_thread_stop_req; /* Pos_Gps_Recv Thread Stop Flag */


#endif  // POSITIONING_SERVER_INCLUDE_NSFW_POSITIONING_COMMON_H_
