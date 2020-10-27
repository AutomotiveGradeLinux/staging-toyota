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
* @file MDev_Gps_Common.h
*/

/*---------------------------------------------------------------------------*/

#include "MDev_Gps_Common.h"

#include <native_service/ns_backup.h>
#include <native_service/ns_backup_id.h>
#include "positioning_common.h"
#include "MDev_Gps_API.h"
#include "MDev_Gps_TimerCtrl.h"
#include "MDev_Gps_Nmea.h"
#include "MDev_GpsRecv.h"

/*---------------------------------------------------------------------------*/
// Value define

#define BK_ID_POS_GPS_TD_STS_SZ   1         /* Datetime Status-Size                */
#define BK_GPSFIXCNT_OFFSET_CHK   12        /* Fix Count-Offset-Check SRAM */
#define BK_GPSFIXCNT_RAM_OK       0         /* Fix Count RAM Status OK         */
#define BK_GPSFIXCNT_RAM_FILE_NG  1         /* Fix Count RAM Status FILE NG    */
#define BK_GPSFIXCNT_RAM_NG       2         /* Fix Count RAM Status NG         */

#define JULIAN_DAY                      (-32044.0f)   /* Be based on March 1, 4800 BC */
#define MODIFIED_JULIAN_DAY_OFFSET      (2400000.5f)
#define GPS_WS_MAX                      (7 * 24 * 60 * 60)  /* GPS time(Weekly seconds)*/
#define GET_METHOD_GPS          (1)  //!< \~english GPS

/* Presence information definitions DR */
#define SENSORLOCATION_EXISTDR_NODR              (0)    /* Without DR   */
#define SENSORLOCATION_EXISTDR_DR                (1)    /* There DR    */
/* DR state definition */
#define SENSORLOCATION_DRSTATUS_INVALID          (0)    /* Invalid            */
#define SENSORLOCATION_DRSTATUS_GPS_NODR         (1)    /* Information use GPS, not yet implemented DR   */
#define SENSORLOCATION_DRSTATUS_NOGPS_DR         (2)    /* No information available GPS, DR implementation */
#define SENSORLOCATION_DRSTATUS_GPS_DR           (3)    /* Information use GPS, DR implementation    */

#define  SENSORLOCATION_STATUS_DISABLE  (0)  //!< \~english Not available
#define  SENSORLOCATION_STATUS_ENABLE   (1)  //!< \~english Available
/*---------------------------------------------------------------------------*/
// Global values

uint8_t g_gps_reset_cmd_sending   = FALSE;  /* Reset command transmission in progress judgment flag     */
uint8_t g_is_gpstime_sts = FALSE;            /* Date/Time Status Fixed Value Setting Indication Flag   */
uint8_t g_gpstime_raw_tdsts = 0;
extern u_int8 g_gps_week_cor_cnt;
/*---------------------------------------------------------------------------*/
// Functions
void WriteGpsTimeToBackup(uint8_t flag, POS_DATETIME* pst_datetime) {
    int32_t ret;
    ST_GPS_SET_TIME buf;

    memset(reinterpret_cast<void *>(&buf), 0, (size_t)sizeof(buf));

    buf.flag   = flag;
    buf.year   = pst_datetime->year;
    buf.month  = pst_datetime->month;
    buf.date   = pst_datetime->date;
    buf.hour   = pst_datetime->hour;
    buf.minute = pst_datetime->minute;
    buf.second = pst_datetime->second;

    ret = Backup_DataWt(D_BK_ID_POS_GPS_TIME_SET_INFO, &buf, 0, sizeof(ST_GPS_SET_TIME));
    if (ret != BKUP_RET_NORMAL) {
      POSITIONING_LOG("Backup_DataWt ERROR!! [ret=%d]", ret);
    }

    return;
}



/********************************************************************************
 * MODULE      : ChangeStatusGpsCommon
 * ABSTRACT    : State transition processing
 * FUNCTION    : Changes the state of the GPS communication management process to the specified state 
 * ARGUMENT    : u_int32 sts : State of the transition destination
 * NOTE        :
 * RETURN      : None
 ********************************************************************************/
void ChangeStatusGpsCommon(u_int32 sts ) {
  /* Set the transition destination state in the management information state  */
  g_gps_mngr.sts = sts;
}

/********************************************************************************
 * MODULE      : RtyResetGpsCommon
 * ABSTRACT    : Reset retry counter(Periodic reception)
 * FUNCTION    : Reset the retry counter 
 * ARGUMENT    : None
 * NOTE        :
 * RETURN      : None
 ********************************************************************************/
void RtyResetGpsCommon(void) {
  g_gps_mngr.hrsetcnt = 0;      /* Initialization of the number of tries until a hard reset    */
  g_gps_mngr.sndngcnt = 0;      /* Initialization of the number of tries until serial reset  */
}

/******************************************************************************
@brief      SendRtyResetGpsCommon<BR>
            Reset retry counter(ACK response)
@outline    Reset the retry counter 
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void SendRtyResetGpsCommon(void) {
  g_gps_mngr.sndcnt   = 0;      /* Initialization of the number of transmission retries            */
}

uint8_t HardResetChkGpsCommon(void) {
  uint8_t      ret = RETRY_OFF;           /* Return value              */
  static uint8_t uc_hardreset_cnt = 0;      /* Number of hard resets      */

  g_gps_mngr.hrsetcnt++;        /* Add number of tries until hard reset  */

  if (g_gps_mngr.hrsetcnt >= HRSET_MAX) {
      // Todo For test don't have hardReset Method.
      // DEV_Gps_HardReset();

      g_gps_mngr.hrsetcnt = 0;     /* Initialization of the number of tries until a hard reset */
      /* Discard all pending commands */
      DeleteAllSaveCmdGpsCommon();
      /* Determination of the number of hard reset executions */
      uc_hardreset_cnt++;
      if (uc_hardreset_cnt >= 3) {
          ret = RETRY_OFF;      /* Set the return value (No retry: Specified number of hard resets completed) */
          uc_hardreset_cnt = 0;         /* Clear the number of hard resets */
      } else {
          ret = RETRY_OUT;      /* Set the return value (retry out: hardware reset) */
      }
  } else {         /* When the number of tries until the hard reset is less than the maximum value  */
      ret = RETRY_ON;         /* Set return value (with retry)     */
  }

  return (ret);            /* End of the process               */
}


/******************************************************************************
@brief      SendReqGpsCommon<BR>
            Common-Transmit Request Reception Matrix Function
@outline  Common processing when receiving a transmission request
@param[in]  none
@param[out]  none
@return     none
@retval     none
*******************************************************************************/
void SendReqGpsCommon(void) {
  RET_API               ret = RET_NORMAL;
  TG_GPS_SND_DATA*      p_rcv_data = NULL;
  TG_GPS_SAVECMD        s_send_data;
  TG_GPS_OUTPUT_FORMAT  s_output_format;

  p_rcv_data = reinterpret_cast<TG_GPS_SND_DATA*>(&(g_gps_msg_rcvr.msgdat[0]));  /* Incoming message structure */

  /** Transmit Data Format Check */
  ret = CheckSendCmdGpsCommon(p_rcv_data->ub_data, p_rcv_data->us_size, &s_output_format);

  /** Normal format */
  if (ret == RET_NORMAL) {
    memset( &s_send_data, 0x00, sizeof(s_send_data) );  /* Ignore -> MISRA-C:2004 Rule 16.10 */

    s_send_data.us_pno      = 0;                /* Result notification destination process number  */
    s_send_data.uc_rid      = 0;                /* Result Notification Resource ID    */
    s_send_data.uc_rst      = GPS_CMD_NOTRST;   /* Reset flag      */
    s_send_data.e_cmd_info  = s_output_format;  /* Command information        */
    memcpy( &s_send_data.sndcmd, p_rcv_data, sizeof(TG_GPS_SND_DATA) );  /* Sending commands */

    /* Command is suspended and terminated. */
    ret = DevGpsSaveCmd(&s_send_data);  /* #GPF_60_040 */

    if (ret != RET_NORMAL) {
        POSITIONING_LOG("GPS Command Reserve bufferFull ! \r\n");
    }
  } else {
    POSITIONING_LOG("# GpsCommCtl # GPS Command Format Error!! \r\n");
  }

  return;
}

/******************************************************************************
@brief      GPSResetReqGpsCommon<BR>
            Common-GPS reset request reception matrix function
@outline  Common processing at GPS reset reception
@param[in]  none
@param[out]  none
@return     none
@retval     none
*******************************************************************************/
void GPSResetReqGpsCommon(void) {
    RET_API         ret = RET_NORMAL;
    POS_RESETINFO   st_rcv_msg;

    memset(&st_rcv_msg, 0x00, sizeof(st_rcv_msg));  /* 2015/03/31 Coverity CID: 21530 support */

    if (g_gps_reset_cmd_sending == FALSE) {
      memcpy(&st_rcv_msg, reinterpret_cast<POS_RESETINFO *>(&(g_gps_msg_rcvr.msgdat)),
                              sizeof(POS_RESETINFO));
      ret = DevGpsResetReq(st_rcv_msg.respno, 0);
      if (ret == RET_NORMAL) {
            /* Set the reset command transmission judgment flag to transmission in progress */
            g_gps_reset_cmd_sending = TRUE;
            /* Send reset command(Normal response transmission)*/
            GpsSetPosBaseEvent(st_rcv_msg.sndpno, POS_RET_NORMAL);
        } else if (ret == RET_EV_NONE) {
            /* nop */
        } else {
            /* Send reset command(Internal Processing Error Response Transmission)*/
            GpsSetPosBaseEvent(st_rcv_msg.sndpno, POS_RET_ERROR_INNER);
        }
    } else {
      memcpy(&st_rcv_msg, reinterpret_cast<POS_RESETINFO*>(&(g_gps_msg_rcvr.msgdat)), sizeof(POS_RESETINFO));
      GpsSetPosBaseEvent(st_rcv_msg.sndpno, POS_RET_ERROR_BUSY);
    }
    return;
}

/******************************************************************************
@brief      CyclDataTimeOutGpsCommon<BR>
            Common-Receive data monitoring timeout matrix function
@outline  Common processing at reception cycle data monitoring timeout
@param[in]  none
@param[out]  none
@return     none
@retval     none
*******************************************************************************/
void CyclDataTimeOutGpsCommon(void) {
  g_wcnct_err++;              /* Count number of connection errors      */

  /* Stop all timers          */
  DevGpsTimeStop(GPS_STARTUP_TIMER);  /* Start confirmation monitoring timer      */  /* Ignore -> MISRA-C:2004 Rule 16.10 */
  DevGpsTimeStop(GPS_RECV_ACK_TIMER);  /* ACK reception monitoring timer      */  /* Ignore -> MISRA-C:2004 Rule 16.10 */

  /* Clear cyclic receive data up to now */
  DevGpsCycleDataClear();

  /* Start confirmation monitoring timer setting  */
  DevGpsTimeSet(GPS_STARTUP_TIMER);  /* Ignore -> MISRA-C:2004 Rule 16.10 */

  /* State-transition(Start Confirmation Monitor)    */
  ChangeStatusGpsCommon(GPS_STS_STARTUP);

  return;
}

/********************************************************************************
 * MODULE      : CheckFrontStringPartGpsCommon
 * ABSTRACT    : Message start character string determination processing
 * FUNCTION    : Check that the message starts with the specified string
 * ARGUMENT    : pubTarget    : Message
 *             : pubStart    : Character string
 * NOTE        : When target is equal to start, it is also judged to be TRUE.
 *             : "*" in the start is treated as a single-character wildcard.
 * RETURN      : RET_NORMAL  : Decision success
 *             : RET_ERROR  : Decision failure
 ********************************************************************************/
RET_API CheckFrontStringPartGpsCommon(const u_char *p_tartget, const u_char *p_start) {
  RET_API ret = RET_ERROR;

  while (((*p_tartget == *p_start)
    || ('*' == *p_start))  /* #GPF_60_024 */  /* Ignore -> No applicable rules for MISRA-C */
    &&  (*p_tartget != '\0')
    &&  (*p_start != '\0')) {
    p_tartget++;  /* Ignore -> MISRA-C:2004 Rule 17.4 */
    p_start++;    /* Ignore -> MISRA-C:2004 Rule 17.4 */
  }

  if (*p_start == '\0') {
    ret = RET_NORMAL;
  } else {
    ret = RET_ERROR;
  }

  return ret;
}

/******************************************************************************
@brief      JudgeFormatGpsCommon<BR>
            Format determination processing
@outline    Determine the format of the received GPS data
@param[in]  pubSndData    : Message to be judged
@param[in]  ul_length    : Message length
@param[out] peFormat    : Where to Return the Format
@input      none
@return     int32
@retval     GPSRET_SNDCMD  : Commands to be notified
@retval     GPSRET_NOPCMD  : User unset command
@retval     GPSRET_CMDERR  : No applicable command
*******************************************************************************/
int32  JudgeFormatGpsCommon(u_char *p_send_data, u_int32 ul_length, TG_GPS_OUTPUT_FORMAT *p_format) {
  int32     ret = GPSRET_NOPCMD; /* Return value    */
  u_int32   cnt = 0;              /* Ignore -> MISRA-C:2004 Rule 5.6 */

  for (cnt = 0; cnt < (u_int32)GPSCMDANATBL_MAX; cnt++) {
    /** End-of-table decision */
    if (CheckFrontStringPartGpsCommon((const u_char*)ENDMARK, kGpsCmdAnaTbl[cnt].c_sentence) == RET_NORMAL) {
      /** When there is no matching sentence, */
      /** set return value. */
      ret = GPSRET_NOPCMD;  /* Ignore -> MISRA-C:2004 Rule 3.1 */
      break;
    }

    /* ++ #GPF_60_040 */
    /* Sentence determination of received command */

    if (kGpsCmdAnaTbl[cnt].e_rcv_format == g_gps_mngr.rcv_cmd) {
      /* Format set */
      (*p_format) = g_gps_mngr.rcv_cmd;

      /* Notification judgment */
      if (kGpsCmdAnaTbl[cnt].b_snd_cmd_flg == TRUE) {
        /* Checksum implementation */
        if (CheckSumGpsCommon(reinterpret_cast<u_int8*>(p_send_data),
                              ul_length,
                              kGpsCmdAnaTbl[cnt].e_rcv_format) != RET_NORMAL) {
          /* Receive data anomaly */
          POSITIONING_LOG("# GpsCommCtl # GPS Data SUM ERROR! \r\n");

          g_wrecv_err++;

          /* Set data error in return value */
          ret = GPSRET_CMDERR;
        } else {
          /* Notification to vehicle sensor */
          ret = GPSRET_SNDCMD;
          g_wrecv_err = 0;
        }
      }

      break;
    }
  }

  return ret;
}


/******************************************************************************
@brief      CheckSumGpsCommon<BR>
            Checksum processing
@outline    Checking the integrity of GPS commands
@param[in]  none
@param[out] none
@input      u_int8          : p_uc_data[]   ... Receive data pointer
@input      u_int32         : ul_length    ... Data length
@input      u_int32         : e_cmd_info    ... Command information<BR>
                                              -FORMAT_NMEA     NMEA data<BR>
                                              -FORMAT_BIN      Standard binary<BR>
                                              -FORMAT_FULLBIN  FULL binaries
@return     RET_API
@retval     RET_NORMAL      : Normal completion
@retval     RET_ERROR       : ABENDs
*******************************************************************************/
RET_API CheckSumGpsCommon(const u_int8 p_uc_data[], u_int32 ul_length, TG_GPS_OUTPUT_FORMAT e_cmd_info) {
  RET_API l_ret = RET_ERROR;
  u_int32 i = 0;
  u_int8  uc_char = 0;
  u_int8  uc_sum = 0;
  u_int8  uc_cmp = 0;
  int32_t ret = 0;

  static u_int8 uc_work[UBX_CMD_SIZE_MAX];

  if ((GPS_FORMAT_MON_VER     == e_cmd_info)
    || (GPS_FORMAT_AID_INI     == e_cmd_info)
    || (GPS_FORMAT_ACK_ACKNACK == e_cmd_info)
    || (GPS_FORMAT_NAV_TIMEUTC == e_cmd_info)
    || (GPS_FORMAT_NAV_CLOCK   == e_cmd_info)
    || (GPS_FORMAT_RXM_RTC5    == e_cmd_info)
    || (GPS_FORMAT_NAV_SVINFO  == e_cmd_info)
    || (GPS_FORMAT_CFG_NAVX5   == e_cmd_info)) {
    (void)memcpy(uc_work, p_uc_data, ul_length - 2);
    DevGpsSetChkSum(uc_work, ul_length);

    ret = memcmp(uc_work, p_uc_data, ul_length);

    if (ret == 0) {
      l_ret = RET_NORMAL;
    } else {
    }
  } else {
    /** XOR each character between '$' and '*' */
    for ( i = 1; i < ul_length; i++ ) {
      if (p_uc_data[i] == (u_int8)'*') {
        /** '*'Detection */
        l_ret = RET_NORMAL;    /* #GPF_60_111 */
        break;
      } else {
        /** '*'Not detected */
        uc_char = p_uc_data[i];
        uc_sum ^= uc_char;
      }
    }

    /** When the position of '*' is within two characters following '*' (access check of the receive buffer range) */
    if ( (l_ret == RET_NORMAL) && ((GPS_READ_LEN - 2) > i) ) {
      /** Convert two characters following '*' to two hexadecimal digits */
      uc_cmp = (AtoHGpsCommon(p_uc_data[i + 1]) * 16) + AtoHGpsCommon(p_uc_data[i + 2]);

      /** Checksum result determination */
      if ( uc_cmp != uc_sum ) {
        /** Abnormality */
        l_ret = RET_ERROR;
      }
    } else {
      /** '*' Not detected, data length invalid */
      /** Abnormality */
      l_ret = RET_ERROR;
    }
  }

  return l_ret;
}

/******************************************************************************
@brief      AtoHGpsCommon<BR>
            ASCII-> hexadecimal conversions
@outline    Convert ASCII codes to hexadecimal
@param[in]  none
@param[out] none
@input      u_int8          : ascii ... ASCII code('0' ~ '9', 'a' ~ 'f', 'A' ~ 'F')
@return     u_int8
@retval     Hexadecimal number
*******************************************************************************/
u_int8  AtoHGpsCommon(u_int8 ascii) {
  u_int8 hex = 0;
  if ((ascii >= '0') && (ascii <= '9')) {
    hex = ascii - '0';
  } else if ((ascii >= 'a') && (ascii <= 'f')) {
    hex = (ascii - 'a') + 0xa;  /* Ignore -> MISRA-C:2004 Rule 12.1 */
  } else if ( (ascii >= 'A') && (ascii <= 'F') ) {
    hex = (ascii - 'A') + 0xa;  /* Ignore -> MISRA-C:2004 Rule 12.1 */
  } else {
    /* nop */
  }

  return hex;
}

/******************************************************************************
@brief      DevGpsSaveCmd<BR>
            Command pending processing
@outline    Temporarily save commands to be sent to GPS
@param[in]  TG_GPS_SND_DATA* pstSndMsg : Sending commands
@param[out] none
@return     RET_API
@retval     RET_NORMAL      : Normal completion
@retval     RET_ERROR       : ABENDs(buffer full)
*******************************************************************************/
RET_API DevGpsSaveCmd(TG_GPS_SAVECMD *p_send_data) {
  RET_API ret = RET_NORMAL;
  u_int32 savp = 0;            /* Holding buffer location storage      */

  /*  Pending buffer full        */
  if ( g_gps_save_cmdr.bufsav >= SAV_MAX ) {
    /* Return an abend */
    ret = RET_ERROR;
  } else {
    savp = g_gps_save_cmdr.saveno;      /* Get pending buffer position        */

    /* Copy data to pending buffer    */
    memset(&g_gps_save_cmdr.savebuf[savp], 0x00, sizeof(g_gps_save_cmdr.savebuf[savp]));
    memcpy(&g_gps_save_cmdr.savebuf[savp], p_send_data, sizeof(TG_GPS_SAVECMD));

    g_gps_save_cmdr.saveno++;        /* Pending index addition        */

    if (g_gps_save_cmdr.saveno >= SAV_MAX) {
      g_gps_save_cmdr.saveno = 0;      /* Reset Pending Index      */
    }

    g_gps_save_cmdr.bufsav++;        /* Number of pending buffers added        */
  }

  return ret;
}

/******************************************************************************
@brief      SendSaveCmdGpsCommon<BR>
            Pending command transmission processing
@outline    Send Pending Commands to GPS
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void SendSaveCmdGpsCommon(void) {
  u_int32           sndp = 0;             /* Holding buffer location storage  */
  // TG_GPS_SND_DATA*  p_send_data = NULL;   /* Pointer to the pending command */
  BOOL              b_ret = FALSE;        /* Command send return value       */

  /** Retrieves one pending command, if any, and sends it. */
  if (g_gps_save_cmdr.bufsav != 0) {
    /** Send position acquisition        */
    sndp = g_gps_save_cmdr.sendno;
    // p_send_data = reinterpret_cast<TG_GPS_SND_DATA*>(&(g_gps_save_cmdr.savebuf[sndp]));

    // todo For test No Uart
    // b_ret = DEV_Gps_CmdWrite( p_send_data->us_size, &(p_send_data->uc_data[0]) );/* #GPF_60_106 */
    if ( b_ret != TRUE ) {
      POSITIONING_LOG("DEV_Gps_CmdWrite fail. ret=[%d]\n", b_ret);
      // ucResult = SENSLOG_RES_FAIL;
    }

    /* ++ #GPF_60_040 */
    /** Configure Monitored Commands */
    g_gps_mngr.resp_cmd       = g_gps_save_cmdr.savebuf[ sndp ].e_cmd_info;
    g_gps_mngr.resp_pno       = g_gps_save_cmdr.savebuf[ sndp ].us_pno;
    g_gps_mngr.resp_rid       = g_gps_save_cmdr.savebuf[ sndp ].uc_rid;
    g_gps_mngr.resp_rst_flag  = g_gps_save_cmdr.savebuf[ sndp ].uc_rst;

    /** Perform response monitoring */
    DevGpsTimeSet(GPS_RECV_ACK_TIMER);  /* Ignore -> MISRA-C:2004 Rule 16.10 */
    /* State transition processing(sending)  */
    ChangeStatusGpsCommon(GPS_STS_SENT);
  } else {
    /** No monitored commands */
    g_gps_mngr.resp_cmd       = GPS_FORMAT_MIN;
    g_gps_mngr.resp_pno       = 0x0000;
    g_gps_mngr.resp_rid       = 0x00;
    g_gps_mngr.resp_rst_flag  = GPS_CMD_NOTRST;
  }

  return;
}

/* ++ #GPF_60_040 */
/******************************************************************************
@brief      DeleteSaveCmdGpsCommon<BR>
            Pending command deletion processing
@outline    Deleting a Pending Command
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DeleteSaveCmdGpsCommon(void) {
  u_int32 sndp = 0; /* Holding buffer location storage  */

  /** Delete one pending command, if any. */
  if (g_gps_save_cmdr.bufsav != 0) {
    /** Send position acquisition        */
    sndp = g_gps_save_cmdr.sendno;

    /** Clear Stored Data */
    memset(&g_gps_save_cmdr.savebuf[sndp], 0x00, sizeof(g_gps_save_cmdr.savebuf[sndp]));

    /** Transmit index addition    */
    g_gps_save_cmdr.sendno++;
    if ( g_gps_save_cmdr.sendno >= SAV_MAX ) {
      /** Transmit index MAX or higher  */
      /** Initialize transmission index      */
      g_gps_save_cmdr.sendno = 0;
    }

    /** Subtract pending buffers      */
    g_gps_save_cmdr.bufsav--;
  }

  return;
}

/******************************************************************************
@brief      DeleteAllSaveCmdGpsCommon<BR>
            Hold command abort processing
@outline    Discards all pending commands and returns a reset response
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DeleteAllSaveCmdGpsCommon(void) {
  u_int32 sndp = 0;     /* Holding buffer location storage  */
  PNO     us_pno = 0;   /* Ignore -> MISRA-C:2004 Rule 5.6 */
  RID     uc_rid = 0;   /* Ignore -> MISRA-C:2004 Rule 5.6 */

  while (g_gps_save_cmdr.bufsav != 0) {
    /** Determine whether it is a pending command that requires a response. */
    sndp = g_gps_save_cmdr.sendno;

    if (g_gps_save_cmdr.savebuf[ sndp ].uc_rst == GPS_CMD_RESET) {
      /** In the case of a reset request, a reset response (communication error) is notified. */
      us_pno = g_gps_save_cmdr.savebuf[ sndp ].us_pno;
      uc_rid = g_gps_save_cmdr.savebuf[ sndp ].uc_rid;
      DevGpsRstAnsSend(us_pno, uc_rid, GPS_SENDNG);  /* Ignore -> MISRA-C:2004 Rule 16.10 */
    }

    /** Delete */
    DeleteSaveCmdGpsCommon();
  }

  return;
}
/******************************************************************************
@brief      RcvCyclCmdNmeaGpsCommon<BR>
            Cyclic data (NMEA) notification process
@outline    Notifying VehicleSensor of Cyclic Data (NMEA) Reception
@param[in]  u_int8               : *p_uc_data ... Receive data pointer
@param[in]  u_int32              : ul_len    ... Received data length
@param[in]  TG_GPS_OUTPUT_FORMAT : s_output_format  ... Receive Format
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void RcvCyclCmdNmeaGpsCommon(u_int8 *p_uc_data, u_int32 ul_len, TG_GPS_OUTPUT_FORMAT s_output_format) {
  NAVIINFO_ALL  navilocinfo;            /* Navigation information */
  BOOL          bcycle_rcv_flag = FALSE;  /* Cyclic reception judgment result    */

  if (JudgeFormatOrderGpsCommon(s_output_format, g_rcv_format) == 0) {
    /** If a sentence is received before the last received sentence, */
    /** determine as the beginning of cyclic data. */
    bcycle_rcv_flag = TRUE;
  }

  if (bcycle_rcv_flag == TRUE) {
    DevGpsSndCycleDataNmea();

    /* Reset navigation information */
    memset(&navilocinfo, 0x00, sizeof(NAVIINFO_ALL));

    /* NMEA data analysis */
    DevGpsAnalyzeNmea(&navilocinfo);

    /** Clear Buffer */
    // DevGpsCycleDataClear();
  }

  DevGpsCycleDataSetNmea(p_uc_data, (u_int32)ul_len, GetNmeaIdxGpsCommon(s_output_format));

  /** Update receive format */
  g_rcv_format = s_output_format;

  return;
}


/**
 * @brief
 *  Cyclic data (UBX) notification processing
 *
 *  Notify vehicle sensor of reception of cyclic data (UBX)
 *
 * @param[in] u_int8               : *p_uc_data ... Receive data pointer
 * @param[in] u_int32              : ul_len    ... Received data length
 * @param[in] TG_GPS_OUTPUT_FORMAT : eFormat  ... Receive Format
 */
void RcvCyclCmdExtGpsCommon(u_int8 *p_uc_data, u_int32 ul_len, TG_GPS_OUTPUT_FORMAT e_format) {
  SENSOR_GPSTIME_RAW            st_gpstime_raw;
  TG_GPS_UBX_NAV_TIMEUTC_DATA   *pst_navtime_utc;
  BOOL b_validtow;        /* Valid Time of Week   */
  BOOL b_validwkn;        /* Valid Week of Number */
  BOOL b_validutc;         /* Valid UTC Time       */

  /* For NAV-TIMEUTC */
  if (e_format == GPS_FORMAT_NAV_TIMEUTC) {
    memset(&st_gpstime_raw, 0x00, sizeof(st_gpstime_raw));

    /* NAV-TIMEUTC analysis */
    pst_navtime_utc = reinterpret_cast<TG_GPS_UBX_NAV_TIMEUTC_DATA*>(p_uc_data + UBX_CMD_OFFSET_PAYLOAD);

    st_gpstime_raw.utc.year   = pst_navtime_utc->year;
    st_gpstime_raw.utc.month  = pst_navtime_utc->month;
    st_gpstime_raw.utc.date   = pst_navtime_utc->day;
    st_gpstime_raw.utc.hour   = pst_navtime_utc->hour;
    st_gpstime_raw.utc.minute = pst_navtime_utc->min;
    st_gpstime_raw.utc.second = pst_navtime_utc->sec;
    b_validtow = (BOOL)(((pst_navtime_utc->valid) & UBX_CMD_MSK_NAV_TIMEUTC_VALID_TOW)
                                  >> (uint8_t)(UBX_CMD_MSK_NAV_TIMEUTC_VALID_TOW / 2));
    b_validwkn = (BOOL)(((pst_navtime_utc->valid) & UBX_CMD_MSK_NAV_TIMEUTC_VALID_WKN)
                                                      >> (uint8_t)(UBX_CMD_MSK_NAV_TIMEUTC_VALID_WKN / 2));
    b_validutc = (BOOL)(((pst_navtime_utc->valid) & UBX_CMD_MSK_NAV_TIMEUTC_VALID_UTC)
                                                      >> (uint8_t)(UBX_CMD_MSK_NAV_TIMEUTC_VALID_UTC / 2));

    if ((b_validtow == TRUE) && (b_validwkn == TRUE)) {
          st_gpstime_raw.tdsts = 2; /* Calibrated */
          g_gpstime_raw_tdsts = 2;
    } else {
          st_gpstime_raw.tdsts = 0; /* Uncalibrated */
          g_gpstime_raw_tdsts = 0;
    }

    b_validutc = b_validutc;
    POSITIONING_LOG("year=%04d, month=%02d, date=%02d, hour=%02d, minute=%02d,"
                    " second=%02d, validTow=%d, validWkn=%d, validUtc=%d",
                    st_gpstime_raw.utc.year, st_gpstime_raw.utc.month, st_gpstime_raw.utc.date,
                    st_gpstime_raw.utc.hour, st_gpstime_raw.utc.minute, st_gpstime_raw.utc.second,
                    b_validtow, b_validwkn, b_validutc);
    /* Notify GPS time to vehicle sensor */
    SndGpsTimeRaw((const SENSOR_GPSTIME_RAW*)&st_gpstime_raw);
  } else {
    POSITIONING_LOG("Forbidden ERROR!![e_format=%d]", e_format);
  }

    /** Update Receive Format */
    g_rcv_format = e_format;

    return;
}

/******************************************************************************
@brief      CheckSendCmdGpsCommon<BR>
            Send command check processing
@outline    Check the format of the send command
@param[in]  const u_char*      pubRcvData  : Receive command
@param[in]  u_int32          ul_length  : Length of the command
@param[out] TG_GPS_OUTPUT_FORMAT*  peFormat  : Command format information
@return     int32
@retval     RET_NORMAL  : Normal
@retval     RET_ERROR  : Abnormality
*******************************************************************************/
int32 CheckSendCmdGpsCommon(const u_char *p_rcv_data, u_int32 ul_length, TG_GPS_OUTPUT_FORMAT *p_format) {
  u_int32 ret = RET_NORMAL;
  u_int32 cnt = 0;          /* Ignore -> MISRA-C:2004 Rule 5.6 */

  /** Analysis of received commands */
  for (cnt = 0; cnt < (u_int32)GPSCMDANATBL_MAX; cnt++) {
    /** End-of-table decision */
    if (CheckFrontStringPartGpsCommon((const u_char*)ENDMARK, kGpsCmdAnaTbl[cnt].c_sentence) == RET_NORMAL) {
      /** When there is no matching sentence */

      /** Return value setting */
      ret = RET_ERROR;  /* Ignore -> MISRA-C:2004 Rule 3.1 */
      break;
    }

    /** Sentence determination of received command */
    if (CheckFrontStringPartGpsCommon(p_rcv_data, kGpsCmdAnaTbl[cnt].c_sentence) == RET_NORMAL) {
      if (ul_length == kGpsCmdAnaTbl[cnt].ul_length) {
        /** When the sentences match */

        /** Reception type determination */
        if ( kGpsCmdAnaTbl[cnt].ul_rcv_kind == RCV_RESP ) {
          /** Response monitor format setting */
          *p_format = kGpsCmdAnaTbl[cnt].e_rcv_format;
        } else {
          /** Return value setting */
          ret = RET_ERROR;  /* Ignore -> MISRA-C:2004 Rule 3.1 */
        }

        break;  /* Ignore -> MISRA-C:2004 Rule 14.6 */
      }
    }
  }

  return ret;
}


/**
 * @brief
 *   Get a string of fields from a NMEA sentence
 *
 *   Operation is not guaranteed if a pointer other than a pointer to the NMEA sentence is specified. 
 *
 * @param[in]   field_number  Field No.
 * @param[in]   p_src  Pointers to NMEA sentences
 * @param[out]  p_dst  Pointer to the output area
 * @param[in]   size  Maximum output size
 *
 * @return Size
 */
int32_t GetStringFromNmeaGpsCommon(u_int8 field_number, u_int8* p_src, char* p_dst, size_t size ) {
  uint8_t*  ptr = NULL;
  /* int32_t numeric; */
  uint32_t  i = 0;
  int32_t   cnt = 0;
  int8_t    buf[GPS_NMEA_FIELD_LEN_MAX] = {0};

  /* Null check */
  if ((p_src == NULL) || (p_dst == NULL)) {
    POSITIONING_LOG("Argument ERROR [p_src=%p, p_dst=%p]", p_src, p_dst);
    return 0;
  }

  ptr = p_src;

  for (i = 0; i <= GPS_NMEA_MAX_SZ; i++) {
    if (cnt == field_number) {
      break;
    }

    if (*ptr == NMEA_DATA_SEPARATOR) {
      cnt++;
    }

    ptr++;
  }

  i = 0;
  if (*ptr != NMEA_DATA_SEPARATOR) {
    for (i = 0; i < (GPS_NMEA_FIELD_LEN_MAX - 1); i++) {
      buf[i] = *ptr;
      ptr++;

      if ((*ptr == NMEA_DATA_SEPARATOR) || (*ptr == NMEA_CHECKSUM_CHAR)) {
        i++;
        break;
      }
    }
  }

  buf[i] = '\0';
  strncpy(p_dst, (const char *)buf, size);

  return i;
}

/**
 * @brief
 *   Get the number (real type) of a field from a NMEA sentence
 *
 *   Operation is not guaranteed if a pointer other than a pointer to the NMEA sentence is specified. 
 *
 * @param[in]  field_number  Field No.
 * @param[in]  p_src  Pointers to NMEA sentences
 * @param[out] p_valid   Validity of numerical data
 *
 * @return  Real-number - double
 */
double_t GetNumericFromNmeaGpsCommon(u_int8 field_number, u_int8* p_src, BOOL* p_valid) {
  double_t  numeric = 0;
  char      buf[GPS_NMEA_FIELD_LEN_MAX] = {0};
  int32_t   ret = 0;

  /* Null check */
  if (p_src == NULL) {
    POSITIONING_LOG("Argument ERROR [p_src=%p]", p_src);
  }

  ret = GetStringFromNmeaGpsCommon(field_number, p_src, buf, GPS_NMEA_FIELD_LEN_MAX);

  if (ret > 0) {
    numeric = atof((const char *)buf);
    *p_valid = TRUE;
  } else {
    *p_valid = FALSE;
  }

  return numeric;
}

/**
 * @brief
 *   Get the integer value (integer type) of a field from a NMEA sentence
 *
 *   Operation is not guaranteed if a pointer other than a pointer to the NMEA sentence is specified. 
 *
 * @param[in]  field_number  Field No.
 * @param[in]  p_src  Pointers to NMEA sentences
 *
 * @return  Integer - int32
 */
int32 GetIntegerFromNmeaGpsCommon(u_int8 field_number, u_int8* p_src) {
  int32_t integer = 0;
  char    buf[GPS_NMEA_FIELD_LEN_MAX] = {0};

  /* Null check */
  if (p_src == NULL) {
    POSITIONING_LOG("Argument ERROR [p_src=%p]", p_src);
  }

  GetStringFromNmeaGpsCommon(field_number, p_src, buf, GPS_NMEA_FIELD_LEN_MAX);
  integer = atoi((const char *)buf);

  return integer;
}

/**
 * @brief
 *   Get the latitude/longitude of the given fi eld from the NMEA sentence
 *
 *   Operation is not guaranteed if a pointer other than a pointer to the NMEA sentence is specified. 
 *
 * @param[in]  field_number  Field No.
 * @param[in]  p_src  Pointers to NMEA sentences
 * @param[out] p_valid   Validity of the data
 *
 * @return Latitude and longitude [1/256 sec]
 */
int32_t GetLonLatFromNmeaGpsCommon(u_int8 field_number, u_int8 *p_src, BOOL *p_valid) {
  int32_t   result = 0;
  double_t  value = 0.0;
  int32_t   deg = 0;
  double_t  min = 0.0;

  /* Null check */
  if (p_src == NULL) {
    POSITIONING_LOG("Argument ERROR [p_src=%p]", p_src);
  }

  value = GetNumericFromNmeaGpsCommon(field_number, p_src, p_valid);

  if ((value < 0.0) || (value > 0.0)) {
    deg = (int32_t)(value / 100.0f);
    min = (double_t)(value - (deg * 100.0f));
    result = (int32_t)(((60.0f * 60.0f * 256.0f) * deg) + ((60.0f * 256.0f) * min));
  }

  return result;
}

/**
 * @brief
 *   Get the orientation of any fields from a NMEA sentence
 *
 *   Operation is not guaranteed if a pointer other than a pointer to the NMEA sentence is specified. 
 *
 * @param[in]  field_number  Field No.
 * @param[in]  p_src Pointers to NMEA sentences
 * @param[out] p_valid   Validity of the data
 *
 * @return Orientation[0.01 degree]
 */
u_int16 GetHeadingFromNmeaGpsCommon(u_int8 field_number, u_int8 *p_src, BOOL *p_valid) {
  u_int16   result = 0;
  double_t  value = 0.0;

  /* Null check */
  if (p_src == NULL) {
    POSITIONING_LOG("Argument ERROR [p_src=%p]", p_src);
  }

  value = GetNumericFromNmeaGpsCommon(field_number, p_src, p_valid);

  result = (u_int16)((value + 0.005f) * 100.0f);

  return result;
}


/**
 * @brief
 *   Get the orientation of any fields from a NMEA sentence
 *
 *   Operation is not guaranteed if a pointer other than a pointer to the NMEA sentence is specified. 
 *
 * @param[in]  field_number  Field No.
 * @param[in]  p_src Pointers to NMEA sentences
 * @param[out] p_valid   Validity of the data
 *
 * @return speed[0.01m/s]
 */
u_int16 GetSpeedFromNmeaGpsCommon(u_int8 field_number, u_int8 *p_src, BOOL *p_valid) {
  u_int16   result = 0;
  double_t  value = 0.0;

  /* Null check */
  if (p_src == NULL) {
    POSITIONING_LOG("Argument ERROR [p_src=%p]", p_src);
  }

  value = GetNumericFromNmeaGpsCommon(field_number, p_src, p_valid);

  // speed[0.01m/s]
  result = static_cast<u_int16>(value * 1.852 * 100 / 3.6);

  return result;
}

/**
 * @brief
 *   Get the altitude of certain fields from a NMEA sentence
 *
 *   Operation is not guaranteed if a pointer other than a pointer to the NMEA sentence is specified. 
 *
 * @param[in]  field_number  Field No.
 * @param[in]  p_src Pointers to NMEA sentences
 * @param[out] p_valid   Validity of the data
 *
 * @return Altitude [0.01m]
 */
int32 GetAltitudeFromNmeaGpsCommon(u_int8 field_number, u_int8 *p_src, BOOL *p_valid) {
  int32     result = 0;
  double_t  value = 0.0;

  /* Null check */
  if (p_src == NULL) {
    POSITIONING_LOG("Argument ERROR [p_src=%p]", p_src);
  }

  value = GetNumericFromNmeaGpsCommon(field_number, p_src, p_valid);

  result = static_cast<int32>((value + 0.005f) * 100.0f);

  return result;
}

/**
 * @brief
 *  GPS Control Thread Stop Processing
 */
void StopThreadGpsCommon(void) {
  /** Stop the start confirmation monitoring timer */
  (void)DevGpsTimeStop(GPS_STARTUP_TIMER);    /* Start confirmation monitoring timer      */

  /** Stop the periodic reception data monitoring timer */
  (void)DevGpsTimeStop(GPS_CYCL_TIMER);      /* Periodic reception data monitoring timer    */

  /** Stop the ACK reception monitoring timer */
  (void)DevGpsTimeStop(GPS_RECV_ACK_TIMER);    /* ACK reception monitoring timer      */

  /** Stop the Navigation Providing Monitor Timer */
  (void)DevGpsTimeStop(GPS_NAVIFST_TIMER);    /* Initial Navigation Monitoring Timer      */

  /** Stop the Navigation Providing Monitor Timer */
  (void)DevGpsTimeStop(GPS_NAVICYCLE_TIMER);    /* Navi monitoring timer        */

  /** Stop the Navigation Providing Monitor Timer */
  (void)DevGpsTimeStop(GPS_NAVIDISRPT_TIMER);    /* Navigation Monitoring Disruption Log Output Timer  */

  /** Stop Time Offering Monitor Timer */
  /* (void)DevGpsTimeStop(GPS_DIAGCLK_GUARDTIMER); */  /* Time Offering Monitoring Timer      */

  PosTeardownThread(ETID_POS_GPS);

  /* don't arrive here */
  return;
}

/**
 * @brief
 *  GPS format order determination process
 *
 * @param[in]  s_format_1 GPS format1
 * @param[in]  s_format_2 GPS format2
 * @return  0 Format 1 first<br>
 *          1 Format 2 first<br>
 *          2 Same as<br>
 *          3 Unable to determine
 */
uint8_t JudgeFormatOrderGpsCommon(TG_GPS_OUTPUT_FORMAT s_format_1, TG_GPS_OUTPUT_FORMAT s_format_2) {
  uint8_t ret = 3;
  u_int32 cnt = 0;

  if (s_format_1 == s_format_2) {
    ret = 2;
  } else if ((s_format_1 == GPS_FORMAT_MAX) || (s_format_2 == GPS_FORMAT_MIN)) {
    ret = 1;
  } else {
    for (cnt = 0; cnt < static_cast<u_int32>(GPSCMDANATBL_MAX); cnt++) {
      /** End-of-table decision */
      if (CheckFrontStringPartGpsCommon((const u_char*)ENDMARK, kGpsCmdAnaTbl[cnt].c_sentence) == RET_NORMAL) {
        /** There is no matching sentence */
        ret = 3;
        break;
      }

      if (kGpsCmdAnaTbl[cnt].e_rcv_format == s_format_1) {
        ret = 0;
        break;
      } else if (kGpsCmdAnaTbl[cnt].e_rcv_format == s_format_2) {
        ret = 1;
        break;
      }
    }
  }

  return ret;
}

/**
 * @brief
 *  Get NMEA index process
 *
 * @param[in]  iFmt GPS formats
 * @return  NMEA indexes
 */
ENUM_GPS_NMEA_INDEX GetNmeaIdxGpsCommon(TG_GPS_OUTPUT_FORMAT s_output_format) {
  ENUM_GPS_NMEA_INDEX ret = GPS_NMEA_INDEX_MAX;

  switch (s_output_format) {
  case GPS_FORMAT_GGA:
      /* GGA */
      ret = GPS_NMEA_INDEX_GGA;
      break;
  case GPS_FORMAT_DGGA:
      /* DGGA */
      ret = GPS_NMEA_INDEX_DGGA;
      break;
  case GPS_FORMAT_VTG:
      /* VTG */
      ret = GPS_NMEA_INDEX_VTG;
      break;
  case GPS_FORMAT_RMC:
      /* RMC */
      ret = GPS_NMEA_INDEX_RMC;
      break;
  case GPS_FORMAT_DRMC:
      /* DRMC */
      ret = GPS_NMEA_INDEX_DRMC;
      break;
  case GPS_FORMAT_GLL:
      /* GLL */
      ret = GPS_NMEA_INDEX_GLL;
      break;
  case GPS_FORMAT_DGLL:
      /* DGLL */
      ret = GPS_NMEA_INDEX_DGLL;
      break;
  case GPS_FORMAT_GSA:
      /* GSA */
      ret = GPS_NMEA_INDEX_GSA;
      break;
  case GPS_FORMAT_GSV1:
      /* GSV(1) */
      ret = GPS_NMEA_INDEX_GSV1;
      break;
  case GPS_FORMAT_GSV2:
      /* GSV(2) */
      ret = GPS_NMEA_INDEX_GSV2;
      break;
  case GPS_FORMAT_GSV3:
      /* GSV(3) */
      ret = GPS_NMEA_INDEX_GSV3;
      break;
  case GPS_FORMAT_GSV4:
      /* GSV(4) */
      ret = GPS_NMEA_INDEX_GSV4;
      break;
  case GPS_FORMAT_GSV5:
      /* GSV(5) */
      ret = GPS_NMEA_INDEX_GSV5;
      break;
  case GPS_FORMAT_GST:
      /* GST */
      ret = GPS_NMEA_INDEX_GST;
      break;
  case GPS_FORMAT__CWORD44__GP3:
      /* _CWORD44_,GP,3 */
      ret = GPS_NMEA_INDEX__CWORD44__GP_3;
      break;
  case GPS_FORMAT__CWORD44__GP4:
      /* _CWORD44_,GP,4 */
      ret = GPS_NMEA_INDEX__CWORD44__GP_4;
      break;
  case GPS_FORMAT_P_CWORD82_F_GP0:
      /* P_CWORD82_F,GP,0 */
      ret = GPS_NMEA_INDEX_P_CWORD82_F_GP_0;
      break;
  case GPS_FORMAT_P_CWORD82_J_GP1:
      /* P_CWORD82_J,GP,1 */
      ret = GPS_NMEA_INDEX_P_CWORD82_J_GP_1;
      break;
  default:
      /* nop */
      break;
  }
  return ret;
}

/**
 * @brief
 *  GPS reset request command transmission processing
 */
void SendResetReqGpsCommon(void) {
  T_APIMSG_MSGBUF_HEADER  s_msg_header;     /* Message header */
  POS_RESETINFO           s_send_msg;  /* Reset information     */
  u_int8                  uc_send_buf[(sizeof(T_APIMSG_MSGBUF_HEADER) + sizeof(POS_RESETINFO))]; /* Transmitting buffer */
  RET_API                 ret = RET_NORMAL;

  /* Send reset request */
  POSITIONING_LOG("GPS Reset Request");

  /* <<Creation of message header section>>>>      */
  s_msg_header.signo            = 0;                      /* Signal information setting                  */
  s_msg_header.hdr.sndpno       = PNO_NAVI_GPS_MAIN;      /* Source thread No. setting            */
  s_msg_header.hdr.respno       = PNO_NAVI_GPS_MAIN;      /* Destination process No.                */
  s_msg_header.hdr.cid          = CID_GPS_REQRESET;       /* Command ID setting                  */
  s_msg_header.hdr.msgbodysize  = sizeof(POS_RESETINFO);  /* Message data length setting        */
  s_msg_header.hdr.rid          = 0;                      /* Resource ID Setting                  */
  s_msg_header.hdr.reserve      = 0;                      /* Reserved area clear              */

  (void)memcpy(&uc_send_buf[ 0 ], &s_msg_header, sizeof(T_APIMSG_MSGBUF_HEADER));

  /* <<Creation of data section>>                  */
  s_send_msg.mode               = GPS_RST_COLDSTART;      /* Reset mode                    */
  s_send_msg.sndpno             = PNO_NAVI_GPS_MAIN;      /* Caller PNo.                        */
  s_send_msg.respno             = PNO_NAVI_GPS_MAIN;      /* Destination PNo.                        */
  s_send_msg.reserve[0]         = 0;
  s_send_msg.reserve[1]         = 0;
  s_send_msg.reserve[2]         = 0;

  /* Copy data to send buffer        */
  (void)memcpy( &uc_send_buf[ sizeof( T_APIMSG_MSGBUF_HEADER ) ], &s_send_msg, sizeof(POS_RESETINFO) );

  /* <<Messaging>>                */
  ret = _pb_SndMsg(PNO_NAVI_GPS_MAIN,  sizeof(uc_send_buf), reinterpret_cast<void *>(uc_send_buf), 0);

  if (ret != RET_NORMAL) {
      POSITIONING_LOG("GPS Reset Request Error[ret = %d]", ret);
  }
}



void DevGpsCommSettingTime(void) {
  TG_GPS_SND_DATA         *pst_rcv_data;
  RET_API                 ret = RET_NORMAL;

  pst_rcv_data = reinterpret_cast<TG_GPS_SND_DATA*>(&(g_gps_msg_rcvr.msgdat[0]));

  ret = DevGpsResetReq(PNO_NONE, 0);
  if (RET_NORMAL != ret) {
    POSITIONING_LOG("DevGpsResetReq failed.");
  } else {
    /* Time setting(u-blox) */
    DevGpsSettingTime(reinterpret_cast<POS_DATETIME *>(pst_rcv_data->ub_data));
  }
  return;
}

void DevGpsReadGpsTime(POS_DATETIME* pst_datetime) {
    int32_t ret;
    ST_GPS_SET_TIME buf;
    POS_DATETIME st_datetime;
    memset(&st_datetime, 0, sizeof(st_datetime));

    // if (isGpsTimeStatusProof == TRUE) {
        /* Clear the flag if the date and time status has already been calibrated. */
        // WriteGpsTimeToBackup(FALSE, &st_datetime);
        // POSITIONING_LOG("D_BK_ID_POS_GPS_TIME_SET_INFO:W:Clear");
    // } else {
        /* Read from backup RAM */
        ret = Backup_DataRd(D_BK_ID_POS_GPS_TIME_SET_INFO, 0, &buf, sizeof(ST_GPS_SET_TIME));
        if (ret == BKUP_RET_NORMAL) {
            pst_datetime->year   = buf.year;
            pst_datetime->month  = buf.month;
            pst_datetime->date   = buf.date;
            pst_datetime->hour   = buf.hour;
            pst_datetime->minute = buf.minute;
            pst_datetime->second = buf.second;

            /* Update of the date/time status fixed value setting indication flag */
            if (buf.flag == TRUE) {
                g_is_gpstime_sts = TRUE;
                POSITIONING_LOG("g_is_gpstime_sts = TRUE");
                POSITIONING_LOG("%d/%d/%d %d:%d:%d", buf.year,
                                                     buf.month,
                                                     buf.date,
                                                     buf.hour,
                                                     buf.minute,
                                                     buf.second);
            } else {
                g_is_gpstime_sts = FALSE;
                POSITIONING_LOG("g_is_gpstime_sts = FALSE");
            }
        } else {
            POSITIONING_LOG("Backup_DataRd ERROR!! [ret=%d]", ret);
        }
    // }
    return;
}

RET_API GpsSetPosBaseEvent(uint16_t ul_snd_pno, int32_t event_val) {
  RET_API     ret = RET_ERROR;       /* Return value */
  EventID     ul_eventid = 0;          /* Event ID */
  char        event_name[32];         /* Event name character string buffer */

  memset(reinterpret_cast<void *>(event_name), 0, sizeof(event_name));
  snprintf(event_name, sizeof(event_name), "VEHICLE_%X", ul_snd_pno);
  /* Event Generation */
  ul_eventid = _pb_CreateEvent(FALSE, 0, event_name);

  // initialize EventID
  if (0 != ul_eventid) {
    ret = _pb_SetEvent(ul_eventid, SAPI_EVSET_ABSOLUTE, VEHICLE_EVENT_VAL_INIT);
    if (RET_NORMAL != ret) {
      /* Delete event and return event generation failed */
      ret = _pb_DeleteEvent(ul_eventid);
      ul_eventid = 0;
    }
  }

  if (0 != ul_eventid) {
    /* Event publishing(Event wait release) */
    ret = _pb_SetEvent(ul_eventid, SAPI_EVSET_ABSOLUTE, event_val);
    if (RET_NORMAL != ret) {
        /* Event issuance failure */
        POSITIONING_LOG("set Event failed.");
    }
    /* Event deletion */
    _pb_DeleteEvent(ul_eventid);
  } else {
      /* Event generation failure */
      POSITIONING_LOG("create Event failed.");
  }
  return ret;
}

/**
 * @brief
 *  GPS reset response command reception processing
 *
 * @param[in]  p_data Pointer to the received GPS command
 */
BOOL DevGpsRcvProcGpsResetResp(TG_GPS_RCV_DATA* p_data) {
    RET_API ret;
    u_int32 ul_reset_sts = GPS_SENDNG;
    BOOL b_snd_flag = TRUE;
    uint8_t *p_gps_data;
    char date[6]; /* ddmmyy */
    char work[8]; /* Work buffer for converting String data */

    if (p_data == NULL) {
        POSITIONING_LOG("Argument ERROR!! [p_data = %p]", p_data);
    } else {
        p_gps_data = p_data->bygps_data;

        /* Checksum determination */
        ret = CheckSumGpsCommon(p_gps_data, p_data->bydata_len, GPS_FORMAT_RMC);
        if (ret == RET_NORMAL) {
            /* Get Date information */
            GetStringFromNmeaGpsCommon(GPS_NMEA_FNO_RMC_DATE, p_gps_data, date, 6);

            strncpy(work, &date[4], 2);

            if (atoi(work) == 0) {
                ul_reset_sts = GPS_SENDOK;  /* Reset Successful */
                /* Reissuing commands to a GPS device from here */
                 /* Send NAV-TIMEUTC sentence addition requests */
                DevGpsNavTimeUTCAddReq();
            }
            POSITIONING_LOG("reset flag = %d, year after reset = %s", ul_reset_sts, work);
        } else {
            b_snd_flag = FALSE; /* If the checksum is NG, wait for the next reply message without sending a callback. */
            POSITIONING_LOG("wrong checksum [ret = %d]", ret);
        }

        if (b_snd_flag == TRUE) {
            /** Drop the time status to an abnormal time */
            // Todo Suntec MACRO __CWORD71_ Disable, This Functions do nothing actrually.
            // DEV_Gps_SetTimeStatus_NG();

            /** Send Reset Response(Successful transmission) */
            DevGpsRstAnsSend(g_gps_mngr.resp_pno, g_gps_mngr.resp_rid, ul_reset_sts);

            POSITIONING_LOG("DEV_Gps_RstAndSend CALL!! [ul_reset_sts = %d]", ul_reset_sts);

            /** Clear reset command sending judgment flag */
            g_gps_reset_cmd_sending = FALSE;
        }
    }
    return b_snd_flag;
}

/**
 * @brief
 *  GPS time setting response command reception processing
 *
 * @param[in]  p_data Pointer to the received GPS command
 */
void DevGpsRcvProcTimeSetResp(TG_GPS_RCV_DATA* p_data) {
    u_int32 ul_reset_sts = GPS_SENDNG;
    BOOL b_snd_flag = TRUE;
    POS_DATETIME                    st_datetime;  /* Setting time */
    uint16_t set_gpsw;
    uint32_t set_gpsws;
    uint16_t rcv_gpsw;
    uint32_t rcv_gpsws;
    BOOL ret = FALSE;
    int32_t status;

    if (p_data == NULL) {
      POSITIONING_LOG("Argument ERROR!! [p_data = %p]", p_data);
    } else {
      /* Read from backup RAM */
      status = Backup_DataRd(D_BK_ID_POS_GPS_TIME_SET_INFO, 0, &st_datetime, sizeof(ST_GPS_SET_TIME));
      if (status == BKUP_RET_NORMAL) {
          /* Convert to GSP week number, GPS week second */
          DevGpsYMD2GPSTIME(static_cast<int32>(st_datetime.year),
                            static_cast<int32>(st_datetime.month),
                            static_cast<int32>(st_datetime.date), static_cast<int32>(st_datetime.hour),
                            static_cast<int32>(st_datetime.minute),
                            static_cast<int32>(st_datetime.second), &set_gpsw, &set_gpsws);

          memcpy(&rcv_gpsw,  &(p_data->bygps_data[24]), sizeof(uint16_t));
          memcpy(&rcv_gpsws, &(p_data->bygps_data[26]), sizeof(uint32_t));
          rcv_gpsws /= 1000; /* Convert week (ms) to week (s) */
          POSITIONING_LOG("recived gpsw[%d] gpsws[%d]\n", rcv_gpsw, rcv_gpsws);

          /* Check if GPS week number and GPS week second are within valid range */
          ret = DevGpsCheckGpsTime(set_gpsw, set_gpsws, rcv_gpsw, rcv_gpsws, GPS_SETTIME_RANGE);
          if (ret == TRUE) {
              /* Date/Time Status Fixed Value Setting Indication Flag ON */
              g_is_gpstime_sts = TRUE;
              POSITIONING_LOG("g_is_gpstime_sts = TRUE");

              /* Time Setting Successful : Update backup RAM time */
              ul_reset_sts = GPS_SENDOK;
              WriteGpsTimeToBackup(TRUE, &st_datetime);
              POSITIONING_LOG("SetGpsTime Success");
              POSITIONING_LOG("D_BK_ID_POS_GPS_TIME_SET_INFO:W:%d/%d/%d %d:%d:%d",
                                                    st_datetime.year,
                                                    st_datetime.month,
                                                    st_datetime.date,
                                                    st_datetime.hour,
                                                    st_datetime.minute,
                                                    st_datetime.second);
          } else {
              /* Time setting error (Difference in set time): Time deletion of backup RAM */
              memset(&st_datetime, 0, sizeof(st_datetime));
              WriteGpsTimeToBackup(FALSE, &st_datetime);
              POSITIONING_LOG("SetGpsTime failed\n");
              POSITIONING_LOG("D_BK_ID_POS_GPS_TIME_SET_INFO:W:Clear\n");
          }
      } else {
          /* Backup RAM read failed */
          b_snd_flag = FALSE;
          POSITIONING_LOG("Backup_DataRd ERROR!! [status=%d", status);
      }

      if (b_snd_flag == TRUE) {
          /** Send Time Set Response */
          DevGpsTimesetAnsSend(g_gps_mngr.resp_pno, g_gps_mngr.resp_rid, ul_reset_sts);

          POSITIONING_LOG("DEV_Gps_TimesetAndSend CALL!! [ul_reset_sts = %d]", ul_reset_sts);
      }
    }
    return;
}

/**
 * @brief
 *   GPS rollover standard week number acquisition response reception processing
 *
 * @param[in]  p_data Pointer to the received GPS command
 */
void DevGpsRcvProcWknRolloverGetResp(TG_GPS_RCV_DATA* p_data) {
  RET_API ret;
  SENSOR_WKN_ROLLOVER_HAL st_wkn_rollover;

  if (p_data == NULL) {
      POSITIONING_LOG("Argument ERROR!! [p_data = %p]", p_data);
  } else {
      /* Checksum determination */
      ret = CheckSumGpsCommon(p_data->bygps_data, p_data->bydata_len, GPS_FORMAT_CFG_NAVX5);
      if (ret == RET_NORMAL) {
          /* Version information extraction */
        (void)memcpy(&(st_wkn_rollover.us_wkn),
                            &(p_data->bygps_data[UBX_CMD_OFFSET_PAYLOAD + UBX_CMD_OFFSET_WKNROLLOVER]),
                            sizeof(st_wkn_rollover.us_wkn));
        POSITIONING_LOG("wknRollover=%d", st_wkn_rollover.us_wkn);

        // gwknRollover = st_wkn_rollover.us_wkn;

        /* Send reference week number to vehicle sensor */
        ret = DevGpsSndWknRollover(&st_wkn_rollover);
        if (ret != RET_NORMAL) {
          POSITIONING_LOG("VehicleUtility_pb_SndMsg ERROR!! [ret=%d]", ret);
        }
      } else {
          POSITIONING_LOG("CheckSumGpsCommon ERROR!! [ret = %d]", ret);
      }

      if (ret != RET_NORMAL) {
          /* GPS rollover standard week number acquisition request retransmission */
          ret = DevGpsWknRolloverGetReq();
          if (ret != RET_NORMAL) {
              POSITIONING_LOG("DevGpsWknRolloverGetReq ERROR!! [ret=%d]", ret);
          }
      }
  }

  return;
}

/**
 * @brief
 *   Receiving additional responses to NAV-SVINFO commands
 *
 * @param[in]  p_data Pointer to the received GPS command
 */
void DevGpsRcvProcNavSvinfoAddResp(TG_GPS_RCV_DATA* p_data) {
    RET_API ret;
    /* Common Response Command Reception Processing */
    ret = DevGpsRcvProcCmnResp(p_data, GPS_CMD_SENTENCEADD_NAVSVINFO);
    if (ret != RET_NORMAL) {
        POSITIONING_LOG("DEV_Gps_RcvProcCmnResp(ADD_NAVSVINFO) ERROR!!");
        /* Retransmission of requests to add NAV-SVINFO commands */
        (void)DevGpsNavTimeUTCAddReq();
    }
    return;
}


/**
 * @brief
 *  Common processing when a GPS receiver error is detected
 */
void GpsReceiverErrGpsCommon(void) {
  BOOL *pb_rcv_err = &(g_gps_mngr.rcv_err_flag); /* GSP receiver error detection status */

  /* Update GPS receiver anomaly detection flag */
  *pb_rcv_err = TRUE;

    DevSendGpsConnectError(TRUE);

    return;
}


/**
 * @brief
 *  GPS device setting response reception processing
 *
 * @param[in]  p_data Pointer to the received GPS command
 * @param[in]  cmd   Command in the configuration request
 * @return  RET_NORMAL Normal completion<br>
 *          RET_ERROR  ABENDs
 */
RET_API DevGpsRcvProcCmnResp(TG_GPS_RCV_DATA* p_data, uint8_t cmd) {
    RET_API ret = RET_ERROR;
    uint8_t is_ack; /* 1=ACK, 0=NACK */
    TG_GPS_UBX_ACK_DATA st_ack;
    uint8_t *puc_msg_class = &(st_ack.uc_msg_class);
    uint8_t *puc_msg_id    = &(st_ack.uc_msg_id);

    if (p_data == NULL) {
        POSITIONING_LOG("Argument ERROR!![p_data=%p]", p_data);
    } else {
        /* Checksum determination */
        ret = CheckSumGpsCommon(p_data->bygps_data, p_data->bydata_len, GPS_FORMAT_ACK_ACKNACK);
        if (ret == RET_NORMAL) {
          is_ack = p_data->bygps_data[UBX_CMD_OFFSET_ACKNAK];
          if (is_ack == 1) {
            /* Response command information extraction */
            (void)memcpy(&st_ack, &(p_data->bygps_data[UBX_CMD_OFFSET_PAYLOAD]), sizeof(st_ack));
            POSITIONING_LOG("msgClass=0x%02x, msgID=0x%02x", *puc_msg_class, *puc_msg_id);

            /* When the response wait command does not match the reception command */
            if (((cmd == GPS_CMD_SENTENCEADD_NMEAGST)    && ((*puc_msg_class != 0x06) || (*puc_msg_id != 0x01)))  ||
              ((cmd == GPS_CMD_SENTENCEADD_NAVTIMEUTC) && ((*puc_msg_class != 0x06) || (*puc_msg_id != 0x01)))  ||
              ((cmd == GPS_CMD_SENTENCEADD_NAVCLOCK)   && ((*puc_msg_class != 0x06) || (*puc_msg_id != 0x01)))  ||
              ((cmd == GPS_CMD_SENTENCEADD_RXMRTC5)    && ((*puc_msg_class != 0x06) || (*puc_msg_id != 0x01)))  ||
              ((cmd == GPS_CMD_SENTENCEADD_NAVSVINFO)  && ((*puc_msg_class != 0x06) || (*puc_msg_id != 0x01)))  ||
              ((cmd == GPS_CMD_AUTOMOTIVEMODE)         && ((*puc_msg_class != 0x06) || (*puc_msg_id != 0x24)))) {
              POSITIONING_LOG("Invalid ACK/NACK!! [cmd=%d, msgClass=0x%02x, msgID=0x%02x]\n",
                      cmd, *puc_msg_class, *puc_msg_id);
              ret = RET_ERROR;
            }
          } else {
            ret = RET_ERROR;
          }
        } else {
          POSITIONING_LOG("DEV_Gps_ChkSum ERROR!![ret=%d]", ret);
        }
    }
    return ret;
}


RET_API DevGpsResetReq(PNO us_pno, RID uc_rid) {
  RET_API                 ret;
  TG_GPS_SAVECMD          st_save_cmd;
  TG_GPS_SND_DATA*        pst_snd_data;
  uint8_t*                p_ublox_data;

                                   /*  SYNC_CHAR_1_2   CLASS  ID     length(L/U) */
  static const u_int8  csnd_cmd[] = {   0xB5,   0x62,   0x06,  0x04,  0x04,   0x00,
                                   /* navBbrMask(L/U) resetMode/res   CK_A  CK_B */
                                      0xFF,   0xFF,   0x00,   0x00,   0x00,  0x00 };

  pst_snd_data = &(st_save_cmd.sndcmd);
  p_ublox_data = pst_snd_data->ub_data;

  memset(&st_save_cmd, 0x00, sizeof(st_save_cmd));
  memcpy(p_ublox_data, csnd_cmd, UBX_CMD_SIZE_CFG_RST);

  /* Checksum assignment */
  DevGpsSetChkSum(p_ublox_data, UBX_CMD_SIZE_CFG_RST);

  pst_snd_data->us_size = UBX_CMD_SIZE_CFG_RST;
  st_save_cmd.us_pno = us_pno;                    /* Result notification destination process number  */
  st_save_cmd.uc_rid = uc_rid;                    /* Result Notification Resource ID     */
  st_save_cmd.uc_rst = GPS_CMD_RESET;             /* Reset flag         */
  st_save_cmd.e_cmd_info = GPS_FORMAT_RMC;        /* Command information           */

  /* Command is suspended and terminated. */
  ret = DevGpsSaveCmd(&st_save_cmd);
  if (ret != RET_NORMAL) {
      POSITIONING_LOG("# GpsCommCtl # GPS Command Reserve bufferFull !");

      /** When the pending buffer is full, a reset response (communication error) is returned. */
      DevGpsRstAnsSend(st_save_cmd.us_pno, st_save_cmd.uc_rid, GPS_SENDNG);
  }

  return ret;
}

void DevGpsSettingTime(POS_DATETIME* pst_datetime) {
    RET_API                        ret;
    TG_GPS_SAVECMD                 st_save_cmd;
    TG_GPS_SND_DATA*               pst_snd_data;
    uint8_t*                       p_ublox_data;
    TG_GPS_UBX_AID_INI_POLLED*     pst_ublox_data;

    uint16_t gpsw;
    uint32_t gpsws;

    if (pst_datetime == NULL) {
        POSITIONING_LOG("Argument ERROR!! [pstDataTime = %p]", pst_datetime);
    } else {
        pst_snd_data = &(st_save_cmd.sndcmd);
        p_ublox_data = pst_snd_data->ub_data;

        (void)memset(&st_save_cmd, 0x00, sizeof(st_save_cmd));

        /* Convert to GSP week number, GPS week second */
        (void)DevGpsYMD2GPSTIME(static_cast<int32>(pst_datetime->year),
                                static_cast<int32>(pst_datetime->month),
                                static_cast<int32>(pst_datetime->date),
                                static_cast<int32>(pst_datetime->hour),
                                static_cast<int32>(pst_datetime->minute),
                                static_cast<int32>(pst_datetime->second), &gpsw, &gpsws);


        /* Send command sequence generation */
        pst_ublox_data = reinterpret_cast<TG_GPS_UBX_AID_INI_POLLED*>(reinterpret_cast<void *>(p_ublox_data));
        pst_ublox_data->uc_sync_char1 = 0xB5;
        pst_ublox_data->uc_sync_char2 = 0x62;
        pst_ublox_data->uc_class     = 0x0B;
        pst_ublox_data->uc_id        = 0x01;
        pst_ublox_data->uc_length[0] = 0x30;
        pst_ublox_data->uc_length[1] = 0x00;
        pst_ublox_data->uc_flags[0]  = 0x02; /* Time setting: The second bit from the low order is 1. */

        (void)memcpy(pst_ublox_data->wn, &gpsw, sizeof(uint16_t));

        gpsws *= 1000;
        (void)memcpy(pst_ublox_data->tow, &gpsws, sizeof(uint32_t));

        /* Checksum assignment */
        DevGpsSetChkSum(reinterpret_cast<u_int8 *>(reinterpret_cast<void*>(pst_ublox_data)),
                                                                        UBX_CMD_SIZE_AID_INI);

        /* Concatenate Poll Request Command */
        ret = DevGpsCatPollReq(p_ublox_data, UBX_POLL_CMD_KIND_AID_INI);
        if (ret != RET_NORMAL) {
            POSITIONING_LOG("DEV_Gps_CatPollReqUblox ERROR!! [ret = %d]", ret);
        } else {
            pst_snd_data->us_size = UBX_CMD_SIZE_AID_INI_PLUS_POLL;
            st_save_cmd.us_pno = PNO_VEHICLE_SENSOR;   /* Result notification destination process number  */
            st_save_cmd.uc_rid = 0;                    /* Result Notification Resource ID      */
            st_save_cmd.uc_rst = GPS_CMD_TIMESET;      /* Response flag              */
            st_save_cmd.e_cmd_info = GPS_FORMAT_AID_INI;/* Command information            */

            /* Command is suspended and terminated. */
            ret = DevGpsSaveCmd(&st_save_cmd);
            if (ret != RET_NORMAL) {
                POSITIONING_LOG("DevGpsSaveCmd ERROR!! [ret = %d]", ret);
                /* Todo: retransmission by timer */
            }
            /* Update non-volatile of time setting  */
            WriteGpsTimeToBackup(FALSE, pst_datetime);
            /* Waiting for a GPS device to respond */
            g_is_gpstime_sts = FALSE;
            POSITIONING_LOG("Start SetGpsTime\n");
            POSITIONING_LOG("D_BK_ID_POS_GPS_TIME_SET_INFO:W:%d/%d/%d %d:%d:%d\n",
                         pst_datetime->year,
                         pst_datetime->month,
                         pst_datetime->date,
                         pst_datetime->hour,
                         pst_datetime->minute,
                         pst_datetime->second);
        }
    }
    return;
}

/**
 * @brief
 *  Converting from the Gregorian calendar to the Julian Day of Modification
 *
 * @param[in] y     year
 * @param[in] m     month
 * @param[in] d     day
 *
 * @return Modified Julian Date
 */
int DevGpsYMD2JD(int y, int m, int d) {
  double jd = JULIAN_DAY;
  double tmp;
  int mjd = 0;

  if (m <= 2) {  /* In January, February in December and 14th of the previous year */
      y--;
      m += 12;
  }
  y += 4800;
  m -= 3;
  d -= 1;

  tmp = static_cast<double>(y / 400);
  y %= 400;
  jd += tmp * 146097;

  tmp = static_cast<double>(y / 100);
  y %= 100;
  jd += tmp * 36524;

  tmp = static_cast<double>(y / 4);
  y %= 4;
  jd += tmp * 1461;
  jd += static_cast<double>(y * 365);

  tmp = static_cast<double>(m / 5);
  m %= 5;
  jd += tmp * 153;

  tmp = static_cast<double>(m / 2);
  m %= 2;
  jd += tmp * 61;
  jd += static_cast<double>(m * 31);
  jd += static_cast<double>(d);

  mjd = static_cast<int>(jd - MODIFIED_JULIAN_DAY_OFFSET); /* Julian Date-> Modified Julian Date */

  return mjd;
}

/**
 * @brief
 *  Converting from year, month, day, hour, minute and second (GPS hour) to GPS week and GPS week seconds
 *
 *  Converting y:year, m:month, d:day, hh:hour, mm:minute, ss:second (GPS hour) to GPS week/GPS week second
 *
 * @param[in] y     year
 * @param[in] m     month
 * @param[in] d     day
 * @param[in] hh    hour
 * @param[in] mm    munites
 * @param[in] ss    sec
 * @param[out] gpsw
 * @param[out] gpsws
 *
 * @return TRUE / FALSE
 */
BOOL DevGpsYMD2GPSTIME(const int32 y, const int32 m, const int32 d, const int32 hh,
                              const int32 mm, const int32 ss, u_int16* gpsw, u_int32* gpsws) {
    /*
       MJD  :Y year M month D date modified Julian date (Convert date to modified Julian date)
       GPSW : GPS week
       GPSWS: GPS week second
    */
    int mjd = DevGpsYMD2JD(y, m, d);

    *gpsw   = (uint16_t)((mjd - 44244) / 7);
    *gpsws  = (uint32_t)(((mjd - 44244 - (*gpsw * 7)) * 86400) +
                          (hh * 3600) + (mm * 60) + ss + 16); /* Add leap seconds ( 16 seconds) at 2015/1/1 */

    if (*gpsws >= GPS_WS_MAX) {
        (*gpsw)++;
        *gpsws -= GPS_WS_MAX;
    }

    POSITIONING_LOG("*gpsw = %d, *gpsws = %d", *gpsw, *gpsws);

    return TRUE;
}

/**
 * @brief
 *  Concatenating Poll Request commands to UBX commands
 *
 * @param[in/out]  *p_ublox_data     Pointer to the concatenated UBX command
 * @param[in]      kind          Additional command types
 *
 * @return RET_NORAML / RET_ERROR
 */
RET_API DevGpsCatPollReq(uint8_t *p_ublox_data, uint16_t kind) {
  RET_API ret_api = RET_NORMAL;
  TG_GPS_UBX_PACKET_HEADER          *pst_ubx_pkg_hdr;
  TG_GPS_UBX_COMMAND_NO_DATA        *pst_ubx_poll_req;
  uint32_t length;

                                         /* Header      ID          length      Checksum */
  static uint8_t c_aidini_poll_cmd[]  = {0xB5, 0x62, 0x0B, 0x01, 0x00, 0x00, 0x00, 0x00};
  static uint8_t c_cfgnav5_poll_cmd[] = {0xB5, 0x62, 0x06, 0x24, 0x00, 0x00, 0x00, 0x00};

  uint8_t* p_poll_cmd;
  uint16_t poll_cmd_length;

  if (p_ublox_data == NULL) {
    POSITIONING_LOG("Argument ERROR!! [p_ublox_data=%p]", p_ublox_data);
    ret_api = RET_ERROR;
  } else {
    if (kind == UBX_POLL_CMD_KIND_AID_INI) {
      p_poll_cmd   = c_aidini_poll_cmd;
      poll_cmd_length = UBX_CMD_SIZE_AID_INI_POLL;
    } else if (kind == UBX_POLL_CMD_KIND_CFG_NAV5) {
      p_poll_cmd   = c_cfgnav5_poll_cmd;
      poll_cmd_length = UBX_CMD_SIZE_CFG_NAV5_POLL;
    } else {
        POSITIONING_LOG("Argument ERROR!! [kind=%d]", kind);
        ret_api = RET_ERROR;
    }

    if (ret_api != RET_ERROR) {
      pst_ubx_pkg_hdr = reinterpret_cast<TG_GPS_UBX_PACKET_HEADER*>(reinterpret_cast<void*>(p_ublox_data));
      length = pst_ubx_pkg_hdr->us_length;

      if (length == 0) {
        POSITIONING_LOG("pst_ubx_pkg_hdr->us_length=0 ERROR!!");
        ret_api = RET_ERROR;
      } else {
        /* Add Poll Request Command */
        pst_ubx_poll_req = reinterpret_cast<TG_GPS_UBX_COMMAND_NO_DATA*>(reinterpret_cast<void*>(p_ublox_data +
                                                                                              length +
                                                                              sizeof(TG_GPS_UBX_COMMAND_NO_DATA)));

        memcpy(reinterpret_cast<void*>(pst_ubx_poll_req), reinterpret_cast<void*>(p_poll_cmd), poll_cmd_length);

        DevGpsSetChkSum(reinterpret_cast<u_int8 *>(reinterpret_cast<void*>(pst_ubx_poll_req)), poll_cmd_length);
      }
    }
  }

  return ret_api;
}

void DevGpsSetGpsweekcorcnt(void) {
    CLOCKGPS_GPSWEEKCOR_CNT_MSG gpsweekcor_cnt_msg;
    memcpy(&gpsweekcor_cnt_msg, &g_gps_msg_rcvr, sizeof(CLOCKGPS_GPSWEEKCOR_CNT_MSG));
    g_gps_week_cor_cnt = gpsweekcor_cnt_msg.gpsweekcorcnt;
    return;
}

RET_API DevGpsNavTimeUTCAddReq(void) {
  RET_API                 ret;
  TG_GPS_SAVECMD          st_save_cmd;
  TG_GPS_SND_DATA*        pst_snd_data;
  uint8_t*                p_ublox_data;

                                /*  SYNC_CHAR_1_2  CLASS  ID    length(L/U)   Payload                 CK_A    CK_B */
  static const u_int8 csnd_cmd[] = {   0xB5, 0x62,  0x06,  0x01, 0x03,   0x00, 0x01,   0x21,   0x01,   0x00,   0x00  };

  pst_snd_data = &(st_save_cmd.sndcmd);
  p_ublox_data = pst_snd_data->ub_data;

  memset(&st_save_cmd, 0x00, sizeof(st_save_cmd));

  /* Send command generation */
  memcpy(p_ublox_data, csnd_cmd, UBX_CMD_SIZE_CFG_MSG);

  /* Checksum assignment */
  DevGpsSetChkSum(p_ublox_data, UBX_CMD_SIZE_CFG_MSG);

  pst_snd_data->us_size = UBX_CMD_SIZE_CFG_MSG;
  st_save_cmd.us_pno = PNO_VEHICLE_SENSOR;             /* Result notification destination process number  */
  st_save_cmd.uc_rid = 0;                              /* Result notification resource ID      */
  st_save_cmd.uc_rst = GPS_CMD_SENTENCEADD_NAVTIMEUTC; /* Response flag            */
  st_save_cmd.e_cmd_info = GPS_FORMAT_ACK_ACKNACK;      /* Command information            */

  /* Command is suspended and terminated. */
  ret = DevGpsSaveCmd(&st_save_cmd);
  if (ret != RET_NORMAL) {
      POSITIONING_LOG("# GpsCommCtl # GPS Command Reserve bufferFull !");
  }

  return ret;
}

RET_API DevGpsWknRolloverGetReq(void) {
  RET_API                 ret;
  TG_GPS_SAVECMD          st_save_cmd;
  TG_GPS_SND_DATA*        pst_snd_data;
  uint8_t*                p_ublox_data;

                                      /*  SYNC_CHAR_1_2   CLASS   ID      length(L/U)     CK_A    CK_B */
  static const u_int8     csnd_cmd[] = {   0xB5,   0x62,   0x06,   0x23,   0x00,   0x00,   0x00,   0x00   };

  pst_snd_data = &(st_save_cmd.sndcmd);
  p_ublox_data = pst_snd_data->ub_data;

  (void)memset(&st_save_cmd, 0x00, sizeof(st_save_cmd)); /* QAC 3200 */

  /* Send command generation */
  (void)memcpy(p_ublox_data, csnd_cmd, UBX_CMD_SIZE_CFG_NAVX5_POLL); /* QAC 3200 */

  /* Checksum assignment */
  DevGpsSetChkSum(p_ublox_data, UBX_CMD_SIZE_CFG_NAVX5_POLL);

  pst_snd_data->us_size = UBX_CMD_SIZE_CFG_NAVX5_POLL;
  st_save_cmd.us_pno = PNO_VEHICLE_SENSOR;           /* Result notification destination process number  */
  st_save_cmd.uc_rid = 0;                            /* Result notification resource ID      */
  st_save_cmd.uc_rst = GPS_CMD_WKNROLLOVER;          /* Response flag            */
  st_save_cmd.e_cmd_info = GPS_FORMAT_CFG_NAVX5;      /* Command information            */

  /* Command is suspended and terminated. */
  ret = DevGpsSaveCmd(&st_save_cmd);
  if (ret != RET_NORMAL) {
      POSITIONING_LOG("# GpsCommCtl # GPS Command Reserve bufferFull !");
    /* If an error occurs at this point that is not expected due to a command queue full error, retransmission control is required. */
  }
  return ret;
}

/**
 * @brief
 *  GPS time setting validity check process
 *
 * @param[in]  set_gpsw     Set GPS week
 * @param[in]  set_gpsws    Set GPS Week
 * @param[in]  rcv_gpsw     Answer GPS Week
 * @param[in]  rcv_gpsws    Answer GPS Weeksec
 * @param[in]  offset_range  Tolerance time difference[sec]
 * @return     BOOL         TRUE:Effective time / FALSE:Invalid time
 */
BOOL DevGpsCheckGpsTime(uint16_t set_gpsw, uint32_t set_gpsws,
                        uint16_t rcv_gpsw, uint32_t rcv_gpsws, int32_t offset_range) {
  BOOL     ret = FALSE;
  int16_t  gap_gpsw = 0;
  int32_t  gap_gpsws = 0;

  /* Check if GPS week number and GPS week second are within valid range */
  /*  NG when Set Time > Receive Time          */
  gap_gpsw = (int16_t)(rcv_gpsw - set_gpsw); /* QAC 3758 */
  if (gap_gpsw == 0) {
      /* There is no difference between the set GPS week and the received GPS week */
      gap_gpsws = (int32_t)(rcv_gpsws - set_gpsws); /* QAC 3771 */
      if ((0 <= gap_gpsws) && (gap_gpsws <= offset_range)) {
        /* Setting of GPS time succeeded if the difference between GPS week seconds is within the range. */
        ret = TRUE;
      }
  } else if (gap_gpsw == 1) {
      /* Difference between the set GPS week and the received GPS week is 1 (over 7 weeks) */
      gap_gpsws = (int32_t)((rcv_gpsws + GPS_WS_MAX) - set_gpsws); /* QAC 3771 */
      if ((0 <= gap_gpsws) && (gap_gpsws <= offset_range)) {
            /* Setting of GPS time succeeded if the difference between GPS week seconds is within the range. */
            ret = TRUE;
      }
  } else {
      /* Set GPS time failed except above */
  }

  return ret;
}


/****************************************************************************
@brief      DevGpsCnvLonLatNavi<BR>
            Longitude/latitude information conversion process(Navigation-provided data)
@outline    Converts the latitude/longitude information provided by the navigation system into the format provided to the vehicle sensor.
@param[in]  u_int8                         : fix_sts  ... Current positioning status
@param[in]  int32                          : lon      ... Longitude information
@param[in]  int32                          : lat      ... Latitude-information
@param[out] SENSORLOCATION_LONLATINFO_DAT* : p_lonlat ... Latitude and longitude information after conversion
@return     none
@retval     none
*******************************************************************************/
void DevGpsCnvLonLatNavi(SENSORLOCATION_LONLATINFO_DAT* p_lonlat, u_int8 fix_sts, int32 lon, int32 lat) {
  int32 longtitude;
  int32 latitude;
  int64_t tmp;
  BOOL  b_ret_lon = TRUE;
  BOOL  b_ret_lat = TRUE;
  BOOL  b_fix_sts = TRUE;

  /** Provides latitude/longitude info for positioning status (2D/3D) */
  if ((NAVIINFO_DIAG_GPS_FIX_STS_2D == fix_sts ) ||
          (NAVIINFO_DIAG_GPS_FIX_STS_3D == fix_sts)) {
      /** Get longitude information */
      tmp = (int64_t)lon * 10000000;
      longtitude = static_cast<int32>(tmp / (int64_t)(256 * 60 * 60));
      if (longtitude == -1800000000) {
          /** West longitude 180 degrees -> East longitude 180 degrees */
          longtitude = +1800000000;
      }
      if ((longtitude > +1800000000) ||
              (longtitude < -1800000000)) {
          /** Longitude range error */
          b_ret_lon = FALSE;
      }
      /** Get latitude information */
      tmp = (int64_t)lat * 10000000;

      latitude = static_cast<int32>(tmp / (int64_t)(256 * 60 * 60));

      if ((latitude > +900000000) ||
              (latitude < -900000000)) {
          /** Latitude range error */
          b_ret_lat = FALSE;
      }
  } else {
      /** Does not provide latitude/longitude information in non-positioning state. */
      b_fix_sts = FALSE;
  }

  /** Provided data setting */
  p_lonlat->getMethod = GET_METHOD_GPS;
  p_lonlat->SyncCnt = 0x00;                /* Synchronization counter value(fixed to 0) */  /* Ignore -> No applicable rules for MISRA-C */
  /** Available/Unusable Decision */
  if ((b_fix_sts == TRUE) && (b_ret_lon == TRUE) && (b_ret_lat == TRUE)) {
    p_lonlat->isEnable  = SENSORLOCATION_STATUS_ENABLE;  /* Available status: Available */
    p_lonlat->Longitude = longtitude;            /* Longitude */  /* Ignore -> MISRA-C:2004 Rule 9.1 */
    p_lonlat->Latitude  = latitude;           /* Latitude */  /* Ignore -> MISRA-C:2004 Rule 9.1 */
  } else {
    p_lonlat->isEnable  = SENSORLOCATION_STATUS_DISABLE; /* Available status: Not available */
    p_lonlat->Longitude = 0;               /* Longitude 0 degree fixed */
    p_lonlat->Latitude  = 0;               /* Latitude fixed to 0 degrees */
  }
  return;
}


/****************************************************************************
@brief      DevGpsCnvAltitudeNavi<BR>
            Altitude information conversion process(Navigation-provided data)
@outline    Converts the altitude information provided by the navigation system into the format provided to the vehicle sensor.
@param[in]  u_int8                           : fix_sts    ... Current positioning status
@param[in]  int32                            : alt        ... Altitude information
@param[out] SENSORLOCATION_ALTITUDEINFO_DAT* : p_altitude ... Converted altitude information
@return     none
@retval     none
*******************************************************************************/
void DevGpsCnvAltitudeNavi(SENSORLOCATION_ALTITUDEINFO_DAT* p_altitude, u_int8 fix_sts, int32 alt) {
  int32 altitude;
  BOOL  b_fix_sts = TRUE;

  /** Provides altitude information when in Fix Status (2D/3D) */
  if (((NAVIINFO_DIAG_GPS_FIX_STS_2D == fix_sts) ||
          (NAVIINFO_DIAG_GPS_FIX_STS_3D == fix_sts)) &&
          (alt != GPS_ALTITUDE_INVALID_VAL)) {
      /* The unit of alt is [0.01m], so conversion is not necessary. */
      altitude = alt;
  } else {
      /** Does not provide altitude information in the non-positioning state. */
      b_fix_sts = FALSE;
  }
  /** Provided data setting */
  p_altitude->getMethod = GET_METHOD_GPS;
  p_altitude->SyncCnt = 0x00;                /* Synchronization counter value(fixed to 0) */  /* Ignore -> No applicable rules for MISRA-C */
  /** Available/Unusable Decision */
  if (b_fix_sts == TRUE) {
    p_altitude->isEnable = SENSORLOCATION_STATUS_ENABLE; /* Available status: Available */
    p_altitude->Altitude = altitude;            /* Altitude */  /* Ignore -> MISRA-C:2004 Rule 9.1 */
  } else {
    p_altitude->isEnable = SENSORLOCATION_STATUS_DISABLE;  /* Available status: Not available */
    p_altitude->Altitude = 0;                /* Altitude 0m fixed */
  }
  return;
}

/****************************************************************************
@brief      DevGpsCnvHeadingNavi<BR>
            Orientation information conversion processing(Navigation-provided data)
@outline    Converts the orientation information provided by the navigation system into the format provided to the vehicle sensor.
@param[in]  u_int8                        : fix_sts   ... Current positioning status
@param[in]  u_int16                       : head     ... Bearing information
@param[out] SENSORMOTION_HEADINGINFO_DAT* : p_heading ... Converted backward direction information
@return     none
@retval     none
*******************************************************************************/
void DevGpsCnvHeadingNavi(SENSORMOTION_HEADINGINFO_DAT* p_heading, u_int8 fix_sts, u_int16 head) {
  u_int16 us_heading;
  BOOL  b_fix_sts = TRUE;

  /** Provides orientation when in Fix Status (2D/3D) */
  if (((NAVIINFO_DIAG_GPS_FIX_STS_2D == fix_sts ) ||
          (NAVIINFO_DIAG_GPS_FIX_STS_3D == fix_sts)) &&
          (head != GPS_HEADING_INVALID_VAL)) {
      /** Orientation information conversion */
      us_heading = head;
      /* Units of head are [0.01 degree] so no conversion is necessary. */
      us_heading -= (u_int16)((us_heading / 36000) * 36000);
  } else {
      /** Azimuth information is not provided in the non-positioning state. */
      b_fix_sts = FALSE;
  }

  /** Provided data setting */
  p_heading->getMethod = GET_METHOD_GPS;
  p_heading->SyncCnt = 0x00;                 /* Synchronization counter value(fixed to 0) */  /* Ignore -> No applicable rules for MISRA-C */
  /** Available/Unusable Decision */
  if (b_fix_sts == TRUE) {
    p_heading->isEnable = SENSORLOCATION_STATUS_ENABLE;    /* Available status: Available */
    p_heading->Heading  = us_heading;           /* Orientation */  /* Ignore -> MISRA-C:2004 Rule 9.1 */
  } else {
    p_heading->isEnable = SENSORLOCATION_STATUS_DISABLE;   /* Available status: Not available */
    p_heading->Heading  = 0;               /* Azimuth 0 degree fixed */
  }

  return;
}
/*---------------------------------------------------------------------------*/
/*EOF*/
