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
* @file MDev_Gps_Main.h
*/

#ifndef  INC_GPSCOMMON_MDEV_GPS_MAIN_H_
#define  INC_GPSCOMMON_MDEV_GPS_MAIN_H_

/*---------------------------------------------------------------------------*/
// Include files

#include "gps_hal.h"
#include "positioning_def.h"

// #include "gps_hal.h"
// #include "MDev_Gps_Nmea.h"
// #include "LineSensDrv_Api.h"

/*---------------------------------------------------------------------------*/
// Value define

#define OFF           0       /* OFF */
#define ON            1       /* ON */
#define OK            0       /* Normal */
#define NG            (-1L)   /* Abnormality */
#define DAT_END       0xff    /* Exit code */

// State
typedef enum GpsMatrixSts {
  GPS_STS_STARTUP = 0,        /* Startup confirmation */
  GPS_STS_NORMAL,             /* In operation */
  GPS_STS_SENT,               /* Command sent status */
  GPS_STS_NUM
}TG_GPS_MATRIX_STS;

// Event
typedef enum GpsMatrixEvt {
  GPS_EVT_SENDREQ = 0,              /* Transmission request              */
  GPS_EVT_RESETREQ,                 /* GPS reset request        */
  GPS_EVT_RECVCYCLDAT,              /* Cyclic reception command reception        */
  GPS_EVT_RECVRSPDAT,               /* Receive response command          */
  GPS_EVT_TIMEOUT_RSPDAT,           /* Response monitoring timeout        */
  GPS_EVT_TIMEOUT_CYCLDAT,          /* Periodic reception data monitoring timeout  */
  GPS_EVT_TIMEOUT_NAVI,             /* Navigation providing data monitoring timeout  */
  GPS_EVT_TIMEOUT_DIAGCLKGUARD,     /* Diagnosis provision time guard monitoring timeout*/
  GPS_EVT_ACC_OFF,                  /* In-function ACC-OFF instructions        */
  GPS_EVT_NAVI_LOCATIONINFO,        /* Providing navigation information            */
  GPS_EVT_NAVI_SPEEDINFO,           /* Navigation speed information service          */
  GPS_EVT_TIMESETTING,              /* GPS time setting instruction          */
  GPS_EVT_TIMEOUT_NMEADATAGUARD,    /* NMEA data-providing guard monitoring timeout*/
  GPS_EVT_BACKUPDATA_LOAD,          /* Backup data read request       */
  GPS_EVT_STOPREQ,                  /* Thread stop request                 */
  GPS_EVT_WEEKCOR_CNT_NOTIFICATIO,  /* GPS Week Adjustment Counter Notification            */
  GPS_EVT_TIMEOUT_RECOVERY,         /* GPS error monitoring reset timer      */
  GPS_EVT_TIMEOUT_RECEIVERERR,      /* GPS receiver anomaly detection timeout  */
  GPS_EVT_NUM
} TG_GPS_MATRIX_EVT;

typedef enum GpsOutputFormat {
  GPS_FORMAT_MIN = -1,        /* Minimum Receive Format(Initialization)  */
  GPS_FORMAT_BINARY,          /* Standard binary            */
  GPS_FORMAT_FULLBIN,         /* Full binary            */
  GPS_FORMAT_GGA,             /* GGA                */
  GPS_FORMAT_DGGA,            /* DGGA                */
  GPS_FORMAT_VTG,             /* VTG                */
  GPS_FORMAT_RMC,             /* RMC                */
  GPS_FORMAT_DRMC,            /* DRMC                */
  GPS_FORMAT_GLL,             /* GLL                */
  GPS_FORMAT_DGLL,            /* DGLL                */
  GPS_FORMAT_GSA,             /* GSA                */
  GPS_FORMAT_GSV1,            /* GSV(1)              */
  GPS_FORMAT_GSV2,            /* GSV(2)              */
  GPS_FORMAT_GSV3,            /* GSV(3)              */
  GPS_FORMAT_GSV4,            /* GSV(4)              */
  GPS_FORMAT_GSV5,            /* GSV(5)              */
  GPS_FORMAT_GST,             /* GST                */
  GPS_FORMAT__CWORD44__GP3,       /* _CWORD44_,GP,3            */
  GPS_FORMAT__CWORD44__GP4,       /* _CWORD44_,GP,4            */
  GPS_FORMAT_P_CWORD82_F_GP0,       /* P_CWORD82_F,GP,0            */
  GPS_FORMAT_P_CWORD82_J_GP1,       /* P_CWORD82_J,GP,1            */
  GPS_FORMAT_P_CWORD82_I_GP,        /* P_CWORD82_I,GP              */
  GPS_FORMAT_P_CWORD82_E_GP0,       /* P_CWORD82_E,GP,0            */
  GPS_FORMAT_P_CWORD82_J_GP0,       /* P_CWORD82_J,GP,0            */
  GPS_FORMAT_P_CWORD82_E_GP2,       /* P_CWORD82_E,GP,2            */
  GPS_FORMAT_P_CWORD82_G_GP0,       /* P_CWORD82_G,GP,0            */
  GPS_FORMAT_P_CWORD82_J_GP7,       /* P_CWORD82_J,GP,7            */
  GPS_FORMAT_P_CWORD82_J_GP8,       /* P_CWORD82_J,GP,8            */
  GPS_FORMAT_MON_VER,         /* MON-VER              */
  GPS_FORMAT_AID_INI,         /* AID-INI              */
  GPS_FORMAT_ACK_ACKNACK,     /* ACK-ACKNACK            */
  GPS_FORMAT_NAV_TIMEUTC,     /* NAV-TIMEUTC            */
  GPS_FORMAT_NAV_CLOCK,       /* NAV-CLOCK                        */
  GPS_FORMAT_RXM_RTC5,        /* RXM-RTC5                         */
  GPS_FORMAT_NAV_SVINFO,      /* NAV-SVINFO                       */
  GPS_FORMAT_CFG_NAVX5,       /* CFG-NAVX5            */
  GPS_FORMAT_NMEA,            /* NMEA (Sentence unspecified)      */
  GPS_FORMAT_UBX,             /* UBX Protocol (binary)      */
  GPS_FORMAT_MAX              /* Maximum Receive Format        */
} TG_GPS_OUTPUT_FORMAT;

// Return value
#define RETRY_OFF       0     /* No retry            */
#define RETRY_ON        1     /* Retry exists            */
#define RETRY_OUT       2     /* Retry out          */
#define NG_RETRYOUT     4     /* Transmission failure retry out      */

#define GPSRET_NOPCMD   (0)   /* Non notification target command  #GPF_60_024 */
#define GPSRET_SNDCMD   (1)   /* Notification target command    #GPF_60_024 */
#define GPSRET_CMDERR   (-1)  /* No applicable command    #GPF_60_024 */

// Number of Retries
#define SRSET_MAX       4     /* Maximum serial reset retry value  */
#define HRSET_MAX       3     /* Maximum value of hard reset retry    */
#define SNDNG_MAX       4     /* Maximum retry value of transmission failure      */

/* The actual number of retries is SNDNG_MAX-1. */
#define GPS_RECV_ERR_MAX 5    /* Maximum number of reception errors  #GPF_60_024  */

// Others
#define SAV_MAX                   11
#define GPS_ALTITUDE_INVALID_VAL  (-1000000)  /* Value when fix altitude is invalid */
#define GPS_HEADING_INVALID_VAL   36100       /* Value when measurement orientation is invalid */

#define GPS_RECVOK                0           /* Normal reception            #13 */
#define GPS_RECVNG                1           /* Reception error            #13 */
#define GPS_OVERRUN               2           /* Overrun detection        #13 */
#define GPS_PARITY                3           /* Parity detection          #13 */
#define GPS_FRAMING               4           /* Framing detection        #13 */

// For the communication management thread
#define  GPS_NON_RECV             0           /* Not received              */
#define  GPS_RECV_ACK             1           /* ACK received            */
#define  GPS_RECV_DATA            2           /* Receive response command          */

#define GPS_SEND_ERR_MAX          5           /* Maximum number of transmission errors        */
#define GPS_CNCT_ERR_MAX          5           /* Maximum number of connection errors        */

#define GPS_START_EVT             "GPS_EVT"   /* Interthread synchronization events      */
#define GPS_EVT_VAL               99          /* Event settings          */
#define GPS_MSGDAT_BUF_SZ         500         /* Receive message buffer size    */

// Definition for receive command analysis table  #GPF_60_024
#define SENTENCE_STR_MAX          12      /* Maximum string for judging sentence  */

#define SENSOR_MSG_VSINFO_DSIZE  1904  //!< \~english message body max size

/*---------------------------------------------------------------------------*/
// Structure

/********************************************************************************
 * TAG      :TG_GPS_MNG
 * ABSTRACT :GPS process management information
 * NOTE     :
 ********************************************************************************/
typedef struct GpsMng {
  u_int32               sts;            /* Matrix state          */
  u_int32               event;          /* Matrix event code      */
  u_int32               rcvsts;         /* Response reception status            */
  u_int32               sndcnt;         /* Number of transmission attempts          */
  u_int32               hrsetcnt;       /* Number of tries before hard reset  */
  u_int32               sndngcnt;       /* Number of failed-to-send attempts        */
  TG_GPS_OUTPUT_FORMAT  rcv_cmd;        /* Types of received GPS commands      */
  TG_GPS_OUTPUT_FORMAT  resp_cmd;       /* Types of GPS commands during response monitoring  */
  PNO                   resp_pno;       /* Destination PNO            */
  RID                   resp_rid;       /* RID to be notified when responding        */
  u_int8                resp_rst_flag;  /* Response command reset flag    */
  BOOL                  rcv_err_flag;   /* GPS receiver error detection flag        */
} TG_GPS_MNG;

/********************************************************************************
 * TAG      :TG_GPS_SAVECMD
 * ABSTRACT :GPS process pending command
 * NOTE     :
 ********************************************************************************/
typedef struct GpsSaveCmd {
  TG_GPS_SND_DATA       sndcmd;       /* Sending commands(_CWORD82_ command)    */
  PNO                   us_pno;       /* Result notification destination process number(Unused)  */
  RID                   uc_rid;       /* Result Notification Resource ID(Unused)    */
  u_int8                uc_rst;       /* Reset flag          */
  TG_GPS_OUTPUT_FORMAT  e_cmd_info;   /* Command information          */
} TG_GPS_SAVECMD;

/********************************************************************************
 * TAG      :TG_GPS_SAVEBUF
 * ABSTRACT :GPS process pending buffer management information
 * NOTE     :
 ********************************************************************************/
typedef struct GpsSaveBuf {
  u_int32         saveno;             /* Current pending index number    */
  u_int32         sendno;             /* Current send index number    */
  u_int32         bufsav;             /* Number of pending buffers used        */
  TG_GPS_SAVECMD  savebuf[SAV_MAX];   /* Pending buffer            */
} TG_GPS_SAVEBUF;

/********************************************************************************
 * TAG      :TG_GPS_MSGRCV
 * ABSTRACT :Receive message
 * NOTE     :
 ********************************************************************************/
typedef struct GpsMsgRcv {
  T_APIMSG_MSGBUF_HEADER  header;     /* Message header          */
  u_int8                  msgdat[SENSOR_MSG_VSINFO_DSIZE + 12];
} TG_GPS_MSGRCV;

/********************************************************************************
 * TAG      :TG_GPS_MSGEVTCHNG
 * ABSTRACT :GPS message event translation table
 * NOTE     :
 ********************************************************************************/
typedef struct GpsMsgEvtChng {
  u_int32     cid;            /* Matrix state          */
  u_int32     event;          /* Matrix event code      */
} TG_GPS_MSGEVTCHNG;

/********************************************************************************
 * TAG      :TG_GPS_TIMEVTCHNG
 * ABSTRACT :GPS timer ID and event translation table
 * NOTE     :
 ********************************************************************************/
typedef struct GpsTimEvtChng {
  u_int16     tim_id;         /* Timer ID information          */
  u_int16     reserve;        /* Reserved              */
  u_int32     event;          /* Matrix event code      */
} TG_GPS_TIMEVTCHNG;

/********************************************************************************
 * TAG      :Gps_Jmp_Tbl
 * ABSTRACT :GPS jump table
 * NOTE     :
 ********************************************************************************/
typedef struct GpsJmpTbl {
  void        (*func)( void );    /* Jump-to module          */
} TG_GPS_JMP_TBL;

/********************************************************************************
 * TAG      :TG_GPS_CMD_ANA_TBL
 * ABSTRACT :Receive command analysis table structure
 * NOTE     :
 ********************************************************************************/
typedef struct TgGpsCmdAnaTbl {
  u_char                c_sentence[SENTENCE_STR_MAX];   /* Sentence identifier    */
  u_int32               ul_length;                      /* Length of the command      */
  u_int32               ul_rcv_kind;                    /* Receiving type        */
  BOOL                  b_snd_cmd_flg;                  /* Command notification flag  */
  TG_GPS_OUTPUT_FORMAT  e_rcv_format;                   /* Receive Format    */
} TG_GPS_CMD_ANA_TBL;

/********************************************************************************
 * TAG      :TG_GPS_TIME_BCD
 * ABSTRACT :Time information (BCD) structure
 * NOTE     :
 ********************************************************************************/
typedef struct GpsTimeBCD {
  u_int8          uc_year;      /* Year information    */
  u_int8          uc_month;     /* Month information    */
  u_int8          uc_day;       /* Day information    */
  u_int8          uc_hour;      /* Time information    */
  u_int8          uc_min;       /* Minute information    */
  u_int8          uc_sec;       /* Second information    */
} TG_GPS_TIME_BCD;

/*---------------------------------------------------------------------------*/
// Global values

#define MDEV_GPSMSGCHKC_MAX 10
#define MDEV_PSTIMCHKC_MAX  11
extern const TG_GPS_MSGEVTCHNG    kGpsMsgchkC[MDEV_GPSMSGCHKC_MAX];     /* Ignore -> MISRA-C:2004 Rule 8.12 */
extern const TG_GPS_TIMEVTCHNG    kGpsTimchkC[MDEV_PSTIMCHKC_MAX];      /* Ignore -> MISRA-C:2004 Rule 8.12 */
extern const TG_GPS_JMP_TBL       kGpsMatxC[GPS_STS_NUM][GPS_EVT_NUM];

#define MDEV_GPSCMDANATBLNMEA_MAX 20
extern const TG_GPS_CMD_ANA_TBL*  kGpsCmdAnaTbl;
extern const TG_GPS_CMD_ANA_TBL   kGpsCmdAnaTblUblox[MDEV_GPSCMDANATBLNMEA_MAX];

extern TG_GPS_SAVEBUF             g_gps_save_cmdr;
extern TG_GPS_MSGRCV              g_gps_msg_rcvr;
extern TG_GPS_MNG                 g_gps_mngr;

extern u_int16                    g_wsend_err;        /* Number of transmission errors          */
extern u_int16                    g_wcnct_err;        /* Number of connection errors          */
extern TG_GPS_OUTPUT_FORMAT       g_rcv_format;       /* Receive Format          */

/*---------------------------------------------------------------------------*/
// Prototype

EFrameworkunifiedStatus  DevGpsMainThread(HANDLE h_app);
BOOL        DevGpsInit(void);
void        DevGpsRcvMsg(void);
void        DevGpsMsgEventCheck(void);
void        DevGpsTimEventCheck(void);
void        DevGpsGpsVersionCheck(void);

/*---------------------------------------------------------------------------*/
#endif  // INC_GPSCOMMON_MDEV_GPS_MAIN_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
