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
* @file MDev_Gps_Main.cpp
*/

/*---------------------------------------------------------------------------*/
// Include files

#include "MDev_Gps_Main.h"
#include "positioning_common.h"
#include "MDev_Gps_Mtrx.h"
#include "MDev_Gps_TimerCtrl.h"
#include "MDev_Gps_Common.h"
#include "MDev_Gps_Nmea.h"
#include "MDev_GpsRecv.h"
#include "LineSensDrv_Thread.h"

/*---------------------------------------------------------------------------*/
// Global values

BOOL g_gps_rcv_thread_stop = FALSE;

/********************************************************************************
 * Matrix event translation table(Message input)
 ********************************************************************************/
const  TG_GPS_MSGEVTCHNG kGpsMsgchkC[MDEV_GPSMSGCHKC_MAX] = {
  /* ++ #GPF_60_024 */
  /* Matrix Event Translation Table for _CWORD82_ NMEA */
  {CID_GPS__CWORD82__REQUEST,             GPS_EVT_SENDREQ},                   /*  NMEA transmission request    */
  {CID_INI_INTERNAL_ACCOFF_START,   GPS_EVT_ACC_OFF},                   /*  In-function ACC-OFF instructions    */
  {CID_GPS_REQRESET,                GPS_EVT_RESETREQ},                  /*  GPS reset request      */
  {CID_NAVIINFO_DELIVER,            GPS_EVT_NAVI_LOCATIONINFO},         /*  Providing navigation information      */
  {CID_NAVIINFO_SPEED_DELIVER,      GPS_EVT_NAVI_SPEEDINFO},            /*  Navigation speed information service    */
  {CID_GPS_TIMESETTING,             GPS_EVT_TIMESETTING},               /*  GPS time setting instruction      */
  {CID_GPS_BACKUPDATA_LOAD,         GPS_EVT_BACKUPDATA_LOAD},           /*  Backup memory read request */
  {CID_THREAD_STOP_REQ,             GPS_EVT_STOPREQ},                   /*  Thread stop request           */
  {CID_GPSWEEKCOR_CNT_NOTIFICATION, GPS_EVT_WEEKCOR_CNT_NOTIFICATIO},   /*  GPS Week Adjustment Counter Notification   */
  {DAT_END,                         DAT_END}                            /*  Table end      */
  /* -- #GPF_60_024 */
};

/********************************************************************************
 * Matrix event translation table(Input Timeout [ Timer ID])
 ********************************************************************************/
const  TG_GPS_TIMEVTCHNG kGpsTimchkC[MDEV_PSTIMCHKC_MAX] = {
  {GPS_STARTUP_TIMER,       0, GPS_EVT_TIMEOUT_CYCLDAT},        /*  Start confirmation monitoring timer      */
  {GPS_CYCL_TIMER,          0, GPS_EVT_TIMEOUT_CYCLDAT},        /*  Periodic reception data monitoring timer  */
  {GPS_RECV_ACK_TIMER,      0, GPS_EVT_TIMEOUT_RSPDAT},         /*  ACK reception monitoring timer      */
  {GPS_NAVIFST_TIMER,       0, GPS_EVT_TIMEOUT_NAVI},           /*  Initial NAVI monitoring timer    */
  {GPS_NAVICYCLE_TIMER,     0, GPS_EVT_TIMEOUT_NAVI},           /*  NAVIGATION monitoring timer        */
  {GPS_NAVIDISRPT_TIMER,    0, GPS_EVT_TIMEOUT_NAVI},           /*  NAVIGATION Disruption Log Output Timer    */
  {GPS_DIAGCLK_GUARDTIMER,  0, GPS_EVT_TIMEOUT_DIAGCLKGUARD},   /*  Diagnosis provision time guard monitoring timer  */
  {GPS_NMEADATA_GUARDTIMER, 0, GPS_EVT_TIMEOUT_NMEADATAGUARD},  /*  NMEA data-providing guard monitoring timer  */
  {GPS_RECOVERY_TIMER,      0, GPS_EVT_TIMEOUT_RECOVERY},       /*  GPS recovery timer              */
  {GPS_RECEIVERERR_TIMER,   0, GPS_EVT_TIMEOUT_RECEIVERERR},    /*  GPS receiver anomaly detection timer   */
  {DAT_END,                 0, DAT_END}                         /*  Table end        */
};

/********************************************************************************
 * Processing Branch Table
 ********************************************************************************/
/********************************************************************************
 * TAG      : TG_GPS_JMP_TBL
 * ABSTRACT : GPS jump table
 * NOTE     : GPS Communication Management Matrix Table
 ********************************************************************************/
const  TG_GPS_JMP_TBL kGpsMatxC[GPS_STS_NUM][GPS_EVT_NUM] = {
  /* - - - - -  Confirming Start  - - - - - */
  {
    {&DevGpsInitStartSendReq},              /* Transmission request                */
    {&DevGpsInitStartGPSResetReq},          /* GPS reset request          */
    {&DevGpsInitStartRcvCyclCmd},           /* Cyclic reception command reception          */
    {&DevGpsNop},                           /* Receive Response Command            */
    {&DevGpsNop},                           /* Response monitoring timeout          */
    {&DevGpsInitStartCyclDataTimeOut},      /* Periodic reception data monitoring timeout    */
    {&DevGpsInitStartNaviDataTimeOut},      /* Navigation providing data monitoring timeout    */
    {&DevGpsInitStartDiagClkGuardTimeOut},  /* Diagnosis provision time guard monitoring timeout  */
    {&DevGpsInitStartAccOffStart},          /* In-function ACC-OFF instructions          */
    {&DevGpsInitStartNaviInfoDeliver},      /* Providing navigation information              */
    {&DevGpsInitStartNaviSpeedDeliver},     /* Navigation speed information service            */
    {&DevGpsInitStartSettingTime},          /* GPS time setting instruction            */
    {&DevGpsInitStartNmeaDataGuardTimeOut}, /* NMEA data-providing guard monitoring timeout */
    {&DevGpsInitStartBackupDataLoadReq},    /* Backup data read request           */
    {&DevGpsInitStartStopReq},              /* Thread stop request                      */
    {&DevGpsInitStartGpsweekcorcntNtf},     /* GPS Week Adjustment Counter Notification              */
    {&DevGpsInitStartRecoveryTimeOut},      /* GPS error monitoring timer out              */
    {&DevGpsInitStartGpsReceiverErrTimeOut} /* GPS receiver anomaly detection timeout    */
  },
  /* - - - - - -  In operation  - - - - - - */
  {
    {&DevGpsNormalSendReq},                 /* Transmission request                */
    {&DevGpsNormalGPSResetReq},             /* GPS reset request          */
    {&DevGpsNormalRcvCyclCmd},          /* Cyclic reception command reception          */
    {&DevGpsNop},                           /* Receive Response Command            */
    {&DevGpsNop},                           /* Response monitoring timeout          */
    {&DevGpsNormalCyclDataTimeOut},         /* Periodic reception data monitoring timeout    */
    {&DevGpsNormalNaviDataTimeOut},         /* Navigation providing data monitoring timeout    */
    {&DevGpsNormalDiagClkGuardTimeOut},     /* Diagnosis provision time guard monitoring timeout  */
    {&DevGpsNormalAccOffStart},             /* In-function ACC-OFF instructions          */
    {&DevGpsNormalNaviInfoDeliver},         /* Providing navigation information              */
    {&DevGpsNormalNaviSpeedDeliver},        /* Navigation speed information service            */
    {&DevGpsNormalSettingTime},             /* GPS time setting instruction            */
    {&DevGpsNormalNmeaDataGuardTimeOut},    /* NMEA data-providing guard monitoring timeout  */
    {&DevGpsNormalBackupDataLoadReq},       /* Backup data read request           */
    {&DevGpsNormalStopReq},                 /* Thread stop request                      */
    {&DevGpsNormalGpsweekcorcntNtf},        /* GPS Week Adjustment Counter Notification              */
    {&DevGpsNormalRecoveryTimeOut},         /* GPS error monitoring timer out              */
    {&DevGpsNormalGpsReceiverErrTimeOut}    /* GPS receiver anomaly detection timeout      */
  },
  /* - - - - --  Sending  - - - - - - */
  {
    {&DevGpsSendSendReq},                   /* Transmission request                */
    {&DevGpsSendGPSResetReq},               /* GPS reset request          */
    {&DevGpsSendRcvCyclCmd},                /* Cyclic reception command reception          */
    {&DevGpsSendRcvRspCmd},                 /* Receive Response Command            */
    {&DevGpsSendRspDataTimeOut},            /* Response monitoring timeout          */
    {&DevGpsSendCyclDataTimeOut},           /* Periodic reception data monitoring timeout    */
    {&DevGpsSendNaviDataTimeOut},           /* Navigation providing data monitoring timeout    */
    {&DevGpsSendDiagClkGuardTimeOut},       /* Diagnosis provision time guard monitoring timeout  */
    {&DevGpsSendAccOffStart},               /* In-function ACC-OFF instructions          */
    {&DevGpsSendNaviInfoDeliver},           /* Providing navigation information              */
    {&DevGpsSendNaviSpeedDeliver},          /* Navigation speed information service            */
    {&DevGpsSendSettingTime},               /* GPS time setting instruction            */
    {&DevGpsSendNmeaDataGuardTimeOut},      /* NMEA data-providing guard monitoring timeout  */
    {&DevGpsSendBackupDataLoadReq},         /* Backup data read request           */
    {&DevGpsSendStopReq},                   /* Thread stop request                      */
    {&DevGpsSendGpsweekcorcntNtf},          /* GPS Week Adjustment Counter Notification              */
    {&DevGpsSendRecoveryTimeOut},           /* GPS error monitoring timer out              */
    {&DevGpsSendGpsReceiverErrTimeOut}      /* GPS receiver anomaly detection timeout      */
  }
};

// Receive command analysis table(NMEA format)
const TG_GPS_CMD_ANA_TBL* kGpsCmdAnaTbl = kGpsCmdAnaTblUblox;

//GPS process pending buffer management table
TG_GPS_MSGRCV             g_gps_msg_rcvr;     /* Incoming message            */
TG_GPS_MNG                g_gps_mngr;         /* GPS process management information        */

// Global variable for GPS communication management
u_int16                   g_wsend_err;        /* Number of transmission errors          */
u_int16                   g_wcnct_err;        /* Number of connection errors          */
TG_GPS_OUTPUT_FORMAT      g_rcv_format;       /* Receive Format          */

// Global variable for the receive thread
TG_GPS_RECV_RcvData       g_gps_rcvdata;      /* Serial receive buffer    */
TG_GPS_RECV_RcvBuf        g_gps_rcvbuf;       /* Receive data storage buffer  */
TG_GPS_RECV_AnaDataBuf    g_gps_ana_databuf;  /* Analysis data buffer    */
TG_GPS_RECV_RcvFrameBuf   g_gps_rcv_framebuf; /* Receive frame buffer    */
TG_GPS_SAVEBUF            g_gps_save_cmdr;    /* Command pending buffer    */
u_int16                   g_wrecv_err;        /* Number of reception errors      */

/*---------------------------------------------------------------------------*/
// Functions

/********************************************************************************
 * MODULE      : DEV_Gps_MainThread
 * ABSTRACT    : Communication Management Thread Domain Processing
 * FUNCTION    : Receive a request from the host application and send a command to the serial port. 
 * ARGUMENT    : PVOID     pv....Thread Creation Arguments
 * NOTE        :
 * RETURN      : TRUE :Normal completion
 *               FALSE:ABENDs
 ********************************************************************************/
EFrameworkunifiedStatus DevGpsMainThread(HANDLE h_app) {
  BOOL ret = FALSE;

  PosSetupThread(h_app, ETID_POS_GPS);

  ret = DevGpsInit();

  if (TRUE == ret) {
    g_gps_rcv_thread_stop = FALSE;

    /* Start receiving message */
    DevGpsRcvMsg();
  } else {
    /* Initialization failed */
    POSITIONING_LOG("DevGpsMainThread Init  Fail \n");
  }

  return eFrameworkunifiedStatusFail;
}


/********************************************************************************
 * MODULE      : DEV_Gps_Init
 * ABSTRACT    : Communication management thread initialization processing
 * FUNCTION    : Initialize internal tables and serial ports 
 * ARGUMENT    : None
 * NOTE        :
 * RETURN      : TRUE :Normal completion
 *               FALSE:ABENDs
 ********************************************************************************/
BOOL DevGpsInit(void) {
  BOOL ret = FALSE;

  /* Global variable initialization(GPS Communication Management Thread Internal Use Variable) */
  /* Clear process management information    */
  memset(&g_gps_mngr, 0x00, sizeof(g_gps_mngr));          /* #05 */

  /* Clear the message receive buffer  */
  memset(&g_gps_msg_rcvr, 0x00, sizeof(g_gps_msg_rcvr));        /* #05 */

  /* Clear the pending buffer      */
  memset(&g_gps_save_cmdr, 0x00, sizeof(g_gps_save_cmdr));      /* #05 */

  /* Clear RTC backup-related data  */
  g_gps_mngr.rcv_cmd      = GPS_FORMAT_MIN;    /* #GPF_60_040 */
  g_gps_mngr.resp_cmd     = GPS_FORMAT_MIN;    /* #GPF_60_040 */
  g_gps_mngr.rcv_err_flag = FALSE;

  g_wsend_err   = 0;                /* Initialization of the number of transmission errors        */
  g_wcnct_err   = 0;                /* Connection error count initialization        */
  g_rcv_format  = GPS_FORMAT_MIN;   /* Initialize Receive Format #GPF_60_024*/

  /* Initialize the timer function initialization processing management table */
  DevGpsTimInit();

  /* Start confirmation monitoring timer setting(5Sec) */
  ret = DevGpsTimeSet(GPS_STARTUP_TIMER);

  /* GPS receiver anomaly detection timer setting(600Sec) */
  ret = DevGpsTimeSet(GPS_RECEIVERERR_TIMER);

  // Set command table
  kGpsCmdAnaTbl = kGpsCmdAnaTblUblox;

  /* State transition processing(Start Confirmation Monitor)  */
  ChangeStatusGpsCommon(GPS_STS_STARTUP);

  return(ret);
}

/******************************************************************************
@brief      DEV_Gps_RcvMsg<BR>
            Message reception processing
@outline    Receive a message and distribute processing into a matrix
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsRcvMsg(void) {
  RET_API   ret = RET_NORMAL;
  void*     p_rcv_msg = &g_gps_msg_rcvr;

  while (1) {
    p_rcv_msg = &g_gps_msg_rcvr;

    // Receive meaasge
    ret = _pb_RcvMsg(PNO_NAVI_GPS_MAIN, sizeof(g_gps_msg_rcvr), &p_rcv_msg, RM_WAIT);

    if (RET_RCVMSG == ret) {
      memcpy(&g_gps_msg_rcvr, p_rcv_msg, sizeof(g_gps_msg_rcvr));

      /* Message Event Check Processing  */
      DevGpsMsgEventCheck();

      if ((u_int32)NG != g_gps_mngr.event) {
        /* If the event check is successful    */
        /* Start Processing by Matrix      */
        (kGpsMatxC[g_gps_mngr.sts][g_gps_mngr.event].func)();
      }
    } else {
      /** MSG reception error */
      POSITIONING_LOG("DevGpsRcvMsg error ret[%d] \n", ret);
    }
  }
}

static inline void SetStopFlag(void) {
  g_gps_rcv_thread_stop = TRUE;
}

/********************************************************************************
 * MODULE      : DEV_Gps_MsgEventCheck
 * ABSTRACT    : Message Event Check Processing
 * FUNCTION    : Check received messages and convert them to events 
 * ARGUMENT    : None
 * NOTE        :
 * RETURN      : None
 ********************************************************************************/
void DevGpsMsgEventCheck(void) {
  u_int32 ul_cnt = 0;

  /* Set Error to Event No.      */
  g_gps_mngr.event = (u_int32)NG;

  /* For timeout notification */
  if (g_gps_msg_rcvr.header.hdr.cid == CID_TIMER_TOUT) {
    /* Get event number from sequence number */
    DevGpsTimEventCheck();
  } else if (g_gps_msg_rcvr.header.hdr.cid == CID_GPS_RECVDATA) {
    DevGpsCmdEventCheckNmea();
  } else {
    while (kGpsMsgchkC[ul_cnt].cid != DAT_END) {
      if (g_gps_msg_rcvr.header.hdr.cid == kGpsMsgchkC[ul_cnt].cid) {
        POSITIONING_LOG("DevGpsMsgEventCheck: cid = %d", g_gps_msg_rcvr.header.hdr.cid);

        /* Get event number */
        g_gps_mngr.event = kGpsMsgchkC[ul_cnt].event;

        if (GPS_EVT_STOPREQ == g_gps_mngr.event) {
          SetStopFlag();
          LineSensDrvThreadStopProcess();
        }

        break;
      }

      ul_cnt++;
    }
  }
}

/********************************************************************************
 * MODULE      : DEV_Gps_TimEventCheck
 * ABSTRACT    : Timeout-Event check processing
 * FUNCTION    : Check received timeout messages and convert them to events 
 * ARGUMENT    : None
 * NOTE        :
 * RETURN      : None
 ********************************************************************************/
void DevGpsTimEventCheck(void) {
  u_int8        uc_time_kind = 0;
  TimerToutMsg  st_rcv_msg;
  BOOL          ret = FALSE;

  // Set Error to Event No.
  g_gps_mngr.event = (u_int32)NG;

  // Get message
  memcpy(&st_rcv_msg, &g_gps_msg_rcvr, sizeof(TimerToutMsg));

  // Time judge kind
  ret = DevGpsTimeJdgKind((u_int16)st_rcv_msg.TimerSeq);

  if (TRUE == ret) {
    /* If the sequence numbers match,    */
    /* Set the conversion code to the event number  */
    uc_time_kind = (u_int8)((st_rcv_msg.TimerSeq >> 8) & 0x00FF);
    g_gps_mngr.event = kGpsTimchkC[uc_time_kind].event;
  }
}

/*---------------------------------------------------------------------------*/
/*EOF*/
