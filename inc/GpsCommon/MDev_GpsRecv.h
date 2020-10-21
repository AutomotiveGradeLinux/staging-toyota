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
* @file MDev_GpsRecv.h
*/

#ifndef INC_GPSCOMMON_MDEV_GPSRECV_H_
#define INC_GPSCOMMON_MDEV_GPSRECV_H_

/*---------------------------------------------------------------------------*/
// Include files

#include "gps_hal.h"
#include "positioning_def.h"

/*---------------------------------------------------------------------------*/
// Define

/* ++ GPS _CWORD82_ support */
#define GPS__CWORD82__CMD_LEN_MAX         200
/* -- GPS _CWORD82_ support */

/* ++ GPS _CWORD82_ support */
#define GPS__CWORD82__RET_START_SEQ_NONE  0       /* _CWORD82_ Start Sequence Determination Result = None   */
#define GPS__CWORD82__RET_START_SEQ_NMEA  1       /* _CWORD82_ Start Sequence Determination Result = NMEA  */
#define GPS__CWORD82__RET_START_SEQ_FULL  2       /* _CWORD82_ Start Sequence Determination Result = Full customization */
#define GPS__CWORD82__RET_START_SEQ_BIN   3       /* _CWORD82_ Start Sequence Determination Result = Standard binary  #GPF_60_024 */

#define GPS_UBLOX_RET_START_SEQ_UBX 4       /* Ublox Start Sequence Decision Result = UBX Protocol (binary) */
#define GPS__CWORD82__FULLBINARY_LEN      GPS_CMD_FULLBIN_SZ  /* _CWORD82_ Full Customization Instruction Length     */

#define GPS__CWORD82__NORMALBINARY_LEN    81      /* _CWORD82_ standard binary instruction length       #GPF_60_024 */
/* -- GPS _CWORD82_ support */

/* Maximum size of serial data read */
#define GPS_RCV_MAXREADSIZE         (2048)  /* Changed from 256 -> 2048   #01 */

/* Receive data storage buffer size */
#define GPS_RCV_RCVBUFSIZE          512     /* Maximum size of the command to be received must be larger than the maximum size */

/* Max-size receive commands ID4:(188+8)byte */
/* Analysis data buffer size */
#define GPS_RCV_ANADATABUFSIZE      (GPS_RCV_MAXREADSIZE + GPS_RCV_RCVBUFSIZE)

/* Receive frame buffer size */
#define GPS_RCV_RCVFRAMEBUFSIZE     GPS_RCV_RCVBUFSIZE  /* Maximum size of the command to be received must be larger than the maximum size */

/* Reception notification data buffer size */
#define GPS_RCV_SNDCMD_SIZE_MAX

/* Common return values */
#define GPS_RCV_RET_ERR_EXT         (-4)    /* External cause error */
#define GPS_RCV_RET_ERR_SYSTEM      (-3)    /* System error */
#define GPS_RCV_RET_ERR             (-1)    /* Abnormality */
#define GPS_RCV_RET_NML             0       /* Normal */

// Reception thread reception frame detection result
#define GPS_RCV_FRMSRCH_ERR_FORMAT  (-1)    /* Frame format error */
#define GPS_RCV_FRMSRCH_FIXED       0       /* Frame determination   */
#define GPS_RCV_FRMSRCH_NOT_FIXED   1       /* Frame undetermined */
#define GPS_RCV_FRMSRCH_NO_DATA     2       /* No analysis data */

// Result of receive thread receive frame analysis
#define GPS_RCV_FRMANA_ERR_PRM      (-3)    /* Parameter error   */
#define GPS_RCV_FRMANA_ERR_FORMAT   (-2)    /* Frame format error */
#define GPS_RCV_FRMANA_ERR_CS       (-1)    /* Sumcheck error  */
#define GPS_RCV_FRMANA_OK           0       /* Normal data    */

/* Serial reception event determination result */
enum GpsRcvRet {
  GPS_RCV_REVT_NML = 0,   /* Receiving(Normal)       */
  GPS_RCV_REVT_ERR,       /* Receiving(Error)       */
  GPS_RCV_REVT_NUM        /* Number of events received      */
};

// Error logging
#define ERLG_SER_FRAMEERR     "Serial Framing Error"
#define ERLG_SER_OVERRUNERR   "Serial Overrun Error"
#define ERLG_SER_PARITYERR    "Serial Parity Error"
#define ERLG_RCVEV_JDGERR     "DEV_Gps_Recv_JdgSerEvt error"
#define ERLG_RCVDATA_SNDERR   "DEV_Gps_Recv_SndRcvData error"
#define ERLG_COMEVNT_WAITERR  "WaitCommEvent error"

/*---------------------------------------------------------------------------*/
// Struct

// Serial receive buffer structure
typedef struct GPSRecvData {
  u_int16   us_read_size;                       /* Serial data read data size */
  u_int8    uc_reserve[2];                      /* Preliminary */
  u_int8    uc_read_data[GPS_RCV_MAXREADSIZE];  /* Serial read data */
} TG_GPS_RECV_RcvData;

// Receive data storage buffer structure
typedef struct GPSRecvBuf {
  u_int16   r_size;                             /* Received size */
  u_int8    rsv[2];                             /* Reserved */
  u_int8    r_buf[GPS_RCV_RCVBUFSIZE];          /* Receive command data */
} TG_GPS_RECV_RcvBuf;

// Data analysis buffer
typedef struct GPSRecvAnaDataBuf {
  u_int16   datsize;                            /* Size of the data */
  u_int16   offset;                             /* Analysis start offset */
  u_int8    datbuf[GPS_RCV_ANADATABUFSIZE];     /* Analysis data */
} TG_GPS_RECV_AnaDataBuf;

// Receive frame buffer
typedef struct GPSRecvFrameBuf {
  u_int16   size;                               /* Frame-size */
  u_int8    rsv[2];                             /* Reserved */
  u_int8    buf[GPS_RCV_RCVFRAMEBUFSIZE];       /* Command data */
} TG_GPS_RECV_RcvFrameBuf;

/*---------------------------------------------------------------------------*/
// External variable

extern TG_GPS_RECV_RcvData      g_gps_rcvdata;      /* Serial receive buffer */
extern TG_GPS_RECV_RcvBuf       g_gps_rcvbuf;       /* Receive data storage buffer */
extern TG_GPS_RECV_AnaDataBuf   g_gps_ana_databuf;  /* Analysis data buffer */
extern TG_GPS_RECV_RcvFrameBuf  g_gps_rcv_framebuf; /* Receive frame buffer */
extern u_int16                  g_wrecv_err;        /* Number of reception errors */

/*---------------------------------------------------------------------------*/
// Prototype

/* Initializing process     */
void        DevGpsRecvInit(void);

/* Buffer clear processing   */
void        DevGpsRecvClrRcvBuf(void);

/* Receive (normal) receive processing   */
void        DevGpsRecvRcvNormal(void);

/* Data reception processing */
int32       DevGpsRecvReadRcvData(TG_GPS_RECV_RcvData* pst_rcv_data);

/* Frame detection processing   */
int32       DevGpsRecvSearchFrameData(const TG_GPS_RECV_AnaDataBuf *adbuf_p, u_int16 *ana_size);

/* Receive data transmission processing */
void        DevGpsRecvSndRcvData(const TG_GPS_RECV_RcvFrameBuf *p_frame_buf);

/* Acknowledgement      */
RET_API     DevRcvData(const TG_GPS_RCV_DATA* ptg_rcv_data);

/* Pos_Gps_Recv Thread Stop Processing */
void        DevGpsRecvThreadStopProcess(void);

EFrameworkunifiedStatus  DevGpsRecvThread(HANDLE h_app);

/*---------------------------------------------------------------------------*/
#endif  // INC_GPSCOMMON_MDEV_GPSRECV_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
