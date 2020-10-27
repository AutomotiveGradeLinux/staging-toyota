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
* @file MDev_GpsRecv.cpp
*/

/*---------------------------------------------------------------------------*/
// Include files

#include "MDev_GpsRecv.h"
#include "MDev_Gps_Common.h"
#include "positioning_common.h"
#include "MDev_Gps_API.h"
//extern "C" {
//#include <stub/gps_mng_api.h>
//}

/*---------------------------------------------------------------------------*/
// Global values

#define FTEN_DRIVER   0
#define TEST_HAL2

#if FTEN_DRIVER
static GpsMngApiObj g_gps_mng_obj;
#endif

/*---------------------------------------------------------------------------*/
// Functions

/*******************************************************************************
 * MODULE   : DEV_Gps_RecvThread
 * ABSTRACT : GPS communication management reception thread
 * FUNCTION : GPS communication management reception thread main
 * ARGUMENT : PVOID     pv....Thread creation arguments
 * NOTE     : -
 * RETURN   : TRUE
 ******************************************************************************/
EFrameworkunifiedStatus DevGpsRecvThread(HANDLE h_app) {
  int32 ret               = GPS_RCV_RET_NML;
  int32 gps_ret            = 0;
  BOOL* p_thread_stop_req = &g_gps_rcv_thread_stop;

  (void)PosSetupThread(h_app, ETID_POS_GPS_RECV);

  /* Initializing process */
  DevGpsRecvInit();

#if FTEN_DRIVER
  gps_ret = GpsMngApiOpen(&g_gps_mng_obj);
  if (gps_ret != GPS_CTL_RET_SUCCESS) {
    printf("[For test] GpsMngApiOpen open failed\n");
  }
#endif

  while (1) {
    /* Thread stop request is received */
    if (TRUE == *p_thread_stop_req) {
      /* Thread stop processing */
      DevGpsRecvThreadStopProcess();
    }

    ret = DevGpsRecvReadRcvData(&g_gps_rcvdata);

    if (GPS_RCV_RET_NML == ret) {
      /* For reception (normal) */
      DevGpsRecvRcvNormal();
    }
  }

  return eFrameworkunifiedStatusOK;
}

/*******************************************************************************
 * MODULE   : DEV_Gps_Recv_Init
 * ABSTRACT : Receive thread initialization processing
 * FUNCTION : Initialize the receive thread
 * ARGUMENT : -
 * NOTE     : -
 * RETURN   : -
 ******************************************************************************/
void DevGpsRecvInit(void) {
  /* Clears the receive buffer */
  DevGpsRecvClrRcvBuf();

  /* Clear receive error count */
  g_wrecv_err = 0;

  return;
}

/*******************************************************************************
 * MODULE   : DEV_Gps_Recv_ClrRcvBuf
 * ABSTRACT : Buffer clear processing
 * FUNCTION : Clears the receive buffer for serial reception.
 * ARGUMENT : -
 * NOTE     : -
 * RETURN   : -
 ******************************************************************************/
void  DevGpsRecvClrRcvBuf(void) {
  /*-----------------------------------------------------------------------*/
  /* Clear serial receive data storage buffer                                  */
  /*-----------------------------------------------------------------------*/
  memset(&g_gps_rcvdata, 0, sizeof(TG_GPS_RECV_RcvData));

  /*-----------------------------------------------------------------------*/
  /* Clear receive data storage buffer                                          */
  /*-----------------------------------------------------------------------*/
  memset(&g_gps_rcvbuf, 0, sizeof(TG_GPS_RECV_RcvBuf));

  /*-----------------------------------------------------------------------*/
  /* Clear analysis data buffer                                              */
  /*-----------------------------------------------------------------------*/
  memset(&g_gps_ana_databuf, 0, sizeof(TG_GPS_RECV_AnaDataBuf));

  /*-----------------------------------------------------------------------*/
  /* Clear receive frame buffer                                            */
  /*-----------------------------------------------------------------------*/
  memset(&g_gps_rcv_framebuf, 0, sizeof(TG_GPS_RECV_RcvFrameBuf));
}

/*******************************************************************************
 * MODULE   : DevGpsRecvJudgeStartSeqence
 * ABSTRACT : Start sequence determination process
 * FUNCTION : Determine whether or not the beginning of the data is the start sequence of the _CWORD82_ command
 * ARGUMENT : const u_char *buf_p   ... Pointer to analysis data
 *            u_int16  ana_size ... Size of data to be analyzed
 * NOTE     :
 * RETURN   : GPS__CWORD82__RET_START_SEQ_NONE ... No start sequence
 *            GPS__CWORD82__RET_START_SEQ_NMEA ... NMEA Start Sequences
 *            GPS__CWORD82__RET_START_SEQ_FULL ... Full Customization Start Sequence
 ******************************************************************************/
static int32 DevGpsRecvJudgeStartSeqence(const u_char *buf_p, u_int16 ana_size) {
  int32 ret = GPS__CWORD82__RET_START_SEQ_NONE;

  if ((ana_size >= 1) && (buf_p[0] == 0x24u)) {
    /* Ignore -> No applicable rules for MISRA-C */ /* 0x24: '$' QAC 285 */
    ret = GPS__CWORD82__RET_START_SEQ_NMEA;
  } else if ((ana_size >= 1) && (buf_p[0] == 0xB0)) {
    ret = GPS__CWORD82__RET_START_SEQ_FULL;
  } else if ((ana_size >= 1) && (buf_p[0] == 0xC6)) {
    /* #GPF_60_024 */
    ret = GPS__CWORD82__RET_START_SEQ_BIN;
  } else if ((ana_size >= 1) && (buf_p[0] == 0xB5)) {
    /* #GPF_60_024 */
    ret = GPS_UBLOX_RET_START_SEQ_UBX;
  } else {
    ret = GPS__CWORD82__RET_START_SEQ_NONE;
  }

  return ret;
}

/*******************************************************************************
 * MODULE   : DEV_Gps_Recv_SearchFrameData
 * ABSTRACT : Frame detection processing
 * FUNCTION : Find if a frame exists in the data
 * ARGUMENT : TG_GPS_RECV_AnaDataBuf *adbuf_p ... Pointer to the analysis data storage buffer
 *            u_int16 *ana_size; ... Analysis completion data size
 * NOTE     : Since it is assumed that the beginning of the frame is stored in the analysis start offset,
 *            when the beginning of the frame is detected in the middle of the buffer,
 *            the processing is terminated with the data up to that point as abnormal format data.
 * RETURN   : GPS_RCV_FRMSRCH_ERR_FORMAT ... Frame format error
 *            GPS_RCV_FRMSRCH_FIXED      ... Frame determination
 *            GPS_RCV_FRMSRCH_NOT_FIXED  ... Frame undetermined
 *            GPS_RCV_FRMSRCH_NO_DATA    ... No analysis data
 ******************************************************************************/
int32 DevGpsRecvSearchFrameData(const TG_GPS_RECV_AnaDataBuf *adbuf_p, u_int16 *ana_size) {
  int32   ret = GPS_RCV_FRMSRCH_ERR_FORMAT;          /* Return value(Frame format error) */
  /* ++ GPS _CWORD82_ support */
  int32   start_seq_type = GPS__CWORD82__RET_START_SEQ_NONE; /* Fully customized or NMEA */
  /* -- GPS _CWORD82_ support */
  u_int16 start_offset = 0;                           /* Analysis start offset */
  u_int16 i = 0;
  u_int16 d_size = 0;                                 /* Unanalyzed data size */

  /* Offset Initialization */
  start_offset = adbuf_p->offset;  /* Start of analysis */ /* Ignore -> No applicable rules for MISRA-C */

  /* Setting of unanalyzed data size */
  d_size = adbuf_p->datsize - start_offset;

  /* For size 0 */
  if (d_size == 0) {
    /* No analysis data */
    ret = GPS_RCV_FRMSRCH_NO_DATA;
    /* Set the analysis completion size to 0. */
    *ana_size = 0; /* Ignore -> No applicable rules for MISRA-C */
  } else {
    /* Since it is assumed that beginning of the frame is stored in the analysis start offset,         */
    /* determine if the start sequence is the first one. */

    /* ++ GPS _CWORD82_ support */
    start_seq_type = DevGpsRecvJudgeStartSeqence(&(adbuf_p->datbuf[start_offset]), d_size);

    if (start_seq_type != GPS__CWORD82__RET_START_SEQ_NONE) {
      /* -- GPS _CWORD82_ support */
      /* Set the frame as undetermined */
      ret = GPS_RCV_FRMSRCH_NOT_FIXED;

      /* ++ GPS _CWORD82_ support */
      /* Find end sequence */
      if (start_seq_type == GPS__CWORD82__RET_START_SEQ_NMEA) {
        for (i = 0; i < d_size; i++) {
          if (adbuf_p->datbuf[(start_offset + i)] == 0x0A) {
            /* If the end sequence is found, */
            /* end as frame fix. */
            ret = GPS_RCV_FRMSRCH_FIXED;

          /* Set the frame size for the analysis completion size. */
          *ana_size = i + 1;

          break;
          }
        }

        if (i == d_size) {
          if (i >= GPS__CWORD82__CMD_LEN_MAX) {
            /* If no end sequence is found, */
            /* frame format error. */
            ret = GPS_RCV_FRMSRCH_ERR_FORMAT;
            /* After that, searching for start sequence. */
          } else {
            /* Because the end sequence cannot be analyzed, */
            /* frame undetermined. */
            ret = GPS_RCV_FRMSRCH_NOT_FIXED;

            /* Set the size of unanalyzed data for the analysis completion size. */
            *ana_size = d_size;
          }
        }
      } else if (start_seq_type == GPS__CWORD82__RET_START_SEQ_FULL) {
        /* #GPF_60_024 */
        /* Is there one frame of data for full customization information? */
        if (d_size >= GPS__CWORD82__FULLBINARY_LEN) {
          /* Is there an end sequence? */
          if (adbuf_p->datbuf[( (start_offset + GPS__CWORD82__FULLBINARY_LEN) - 1)] == 0xDA) {
            /* Ignore -> MISRA-C:2004 Rule 12.1 */
            /* If an end sequence is found, */
            /* end as frame fix. */
            ret = GPS_RCV_FRMSRCH_FIXED;

            /* Set the frame size for the analysis completion size. */
            *ana_size = GPS__CWORD82__FULLBINARY_LEN;
          } else {
            /* If it is not an end sequence, */
            /* frame format error. */
            ret = GPS_RCV_FRMSRCH_ERR_FORMAT;
            /* Searching for Start Sequence */
          }
        } else {
          /* Because the end sequence cannot be analyzed, */
          /* frame undetermined. */
          ret = GPS_RCV_FRMSRCH_NOT_FIXED;

          /* Set the size of unanalyzed data for the analysis completion size. */
          *ana_size = d_size;
        }
      } else if (start_seq_type == GPS__CWORD82__RET_START_SEQ_BIN) {
        /* Is there data for one standard binary frame? */
        if (d_size >= GPS__CWORD82__NORMALBINARY_LEN) {
          /* Is there an end sequence? */
          if (adbuf_p->datbuf[((start_offset + GPS__CWORD82__NORMALBINARY_LEN) - 1)] == 0xDA) {
            /* Ignore -> MISRA-C:2004 Rule 12.1 */
            /* If an end sequence is found, */
            /* end as frame fix. */
            ret = GPS_RCV_FRMSRCH_FIXED;

            /* Set the frame size for the analysis completion size. */
            *ana_size = GPS__CWORD82__NORMALBINARY_LEN;
          } else {
            /* If it is not an end sequence, */
            /* frame format error. */
            ret = GPS_RCV_FRMSRCH_ERR_FORMAT;
            /* Searching for Start Sequence */
          }
        } else {
          /* Because the end sequence cannot be analyzed, */
          /* frame undetermined. */
          ret = GPS_RCV_FRMSRCH_NOT_FIXED;

          /* Set the size of unanalyzed data for the analysis completion size. */
          *ana_size = d_size;
        }
      } else if (start_seq_type == GPS_UBLOX_RET_START_SEQ_UBX) {
        /* TODO Checksum calculation using data from start_offset to d_size */
        /* TODO Check that the checksum is correct. (See test code.) */
        /* If the if checksums match, */
        /* end as frame fix. */
        ret = GPS_RCV_FRMSRCH_FIXED;

        /* Set the frame size for the analysis completion size. */
        *ana_size = d_size;
        } else {
      }
    /* -- #GPF_60_024 */
    } else {
      /* It is not a start sequence, so it is regarded as a frame format error. */
      ret = GPS_RCV_FRMSRCH_ERR_FORMAT;

      /* After that, searching for Start Sequence. */
    }

    /* If the frame format is abnormal, search for the start sequence. */
    if (ret == GPS_RCV_FRMSRCH_ERR_FORMAT) {
      POSITIONING_LOG("FORMAT ERROR [start_seq_type:%d]\n", start_seq_type);

      /* Assuming that the start sequence has not been found until the end, */
      /* the size of the analysis completed is set as the size of the unanalyzed data.  */
      *ana_size = d_size;

      /* ++ GPS _CWORD82_ support (Search from the second byte for safety (at least the first byte is checked at the beginning of the function)))*/
      for (i = start_offset + 1; i < (u_int32)(start_offset + d_size); i++) {
        /* for( i = (start_offset + 2); i < (u_int32)(start_offset + d_size); i++ ) */
        /* -- GPS _CWORD82_ support */
        /* Start Sequence? */
        /* ++ GPS _CWORD82_ support */
        if (DevGpsRecvJudgeStartSeqence(&(adbuf_p->datbuf[i]), d_size) != GPS__CWORD82__RET_START_SEQ_NONE) {
          /* if( (adbuf_p->datbuf[(i-1)] == GPS_CODE_START_SQ_HI) && */
          /*  (adbuf_p->datbuf[i] == GPS_CODE_START_SQ_LOW) ) */
          /* -- GPS _CWORD82_ support */
          /* In the case of a start sequence, the analysis is completed before that. #01 */
          *ana_size = (i - start_offset - 1);

          break;
        }
      }
    }
  }

  return ret;
}

/*******************************************************************************
 * MODULE : DEV_Gps_Recv_ReadRcvData
 * ABSTRACT : Data reception processing
 * FUNCTION : Read serial data
 * ARGUMENT : TG_GPS_RECV_RcvData* pst_rcv_data : Receive data read buffer
 * NOTE  : -
 * RETURN : T_ErrCodes Error code
 *    MCSUB_NML   Normal
 *    MCCOM_ERR_SYSTEM Abnormality
 ******************************************************************************/
int32 DevGpsRecvReadRcvData(TG_GPS_RECV_RcvData* pst_rcv_data) {
  int32 ret = GPS_RCV_RET_ERR;
  INT32 gps_ret = 0;

#if FTEN_DRIVER
  static int msg_kind = GPS_RCV_NMEA_GGA;

  // Serial data capture
  GpsMngApiDat gps_mng_data;

  memset(&gps_mng_data, 0, sizeof(gps_mng_data));
#ifdef TEST_HAL2
  if (msg_kind > GPS_RCV_NMEA_RMC) {
    msg_kind = GPS_RCV_NMEA_GGA;
  }
#else
  if (msg_kind == GPS_RCV_NMEA_GGA) {
    msg_kind = GPS_RCV_NMEA_RMC;
  } else if (msg_kind == GPS_RCV_NMEA_RMC) {
    msg_kind = GPS_RCV_NMEA_GGA;
  } else {
    msg_kind = GPS_RCV_NMEA_GGA;
  }
#endif

  gps_ret = GpsMngApiGetRcvMsg(&g_gps_mng_obj, msg_kind, &gps_mng_data);

#ifdef TEST_HAL2
    msg_kind++;
#endif

  pst_rcv_data->us_read_size = (u_int16)gps_mng_data.dataLength;

  if (pst_rcv_data->us_read_size >= GPS_RCV_MAXREADSIZE) {
    return GPS_RCV_RET_ERR_SYSTEM;
  }

  memcpy(pst_rcv_data->uc_read_data, gps_mng_data.data, pst_rcv_data->us_read_size);

  // Add '\0'
  pst_rcv_data->uc_read_data[pst_rcv_data->us_read_size] = '\0';

  if (GPS_CTL_RET_SUCCESS != gps_ret) {
    ret = GPS_RCV_RET_ERR_SYSTEM;
  } else {
    ret = GPS_RCV_RET_NML;
  }
#endif

  return ret;
}

/*******************************************************************************
 * MODULE : DEV_Gps_Recv_RcvNormal
 * ABSTRACT : Receive (normal) Receive processing
 * FUNCTION : Receive (normal) Processing at event reception
 * ARGUMENT : -
 * NOTE  :
 * RETURN : -
 ******************************************************************************/
void DevGpsRecvRcvNormal(void) {
  TG_GPS_RECV_RcvData*      pst_rcv_data = NULL;        /* Buffer for reading serial data */
  TG_GPS_RECV_RcvBuf*       pst_rcv_buf = NULL;         /* Receive data storage buffer */
  TG_GPS_RECV_AnaDataBuf*   pst_ana_data_buf = NULL;    /* Analysis data storage buffer */
  TG_GPS_RECV_RcvFrameBuf*  pst_rcv_frame_buf = NULL;   /* Receive frame buffer */
  int32                     i_ret = 0;                  /* Frame Detection Result */
  u_int16                   ana_size = 0;               /* Data analysis size storage */

  /* Initializing process */
  pst_ana_data_buf = &g_gps_ana_databuf;

  /* Fast _CWORD71_ processing(memset fix) */
  /* Initializes the offset that needs initialization in the receive data storage buffer. */
  pst_ana_data_buf->offset = 0;

  /* Serial continuous reception data analysis processing */
  pst_rcv_data = &g_gps_rcvdata;
  pst_rcv_buf = &g_gps_rcvbuf;
  memcpy(&(pst_ana_data_buf->datbuf[0]), &(pst_rcv_buf->r_buf[0]), pst_rcv_buf->r_size);
  memcpy(&(pst_ana_data_buf->datbuf[pst_rcv_buf->r_size]),
         &(pst_rcv_data->uc_read_data[0]),
         pst_rcv_data->us_read_size);

  pst_ana_data_buf->datsize = pst_rcv_buf->r_size + pst_rcv_data->us_read_size;

  /* Serial receive data analysis process */
  do {
    /* Frame detection processing */
    i_ret = DevGpsRecvSearchFrameData(pst_ana_data_buf, &ana_size);

    /* For frame determination */
    if (i_ret == GPS_RCV_FRMSRCH_FIXED) {
      /* Frames are stored in the receive frame buffer. */
      pst_rcv_frame_buf = &g_gps_rcv_framebuf;
      memset(pst_rcv_frame_buf, 0, sizeof(TG_GPS_RECV_RcvFrameBuf)); /* Ignore -> No applicable rules for MISRA-C */
      /* Ignore -> No applicable rules for MISRA-C */ /* QAC 3200 */
      memcpy(& (pst_rcv_frame_buf->buf[0]),
             &(pst_ana_data_buf->datbuf[pst_ana_data_buf->offset]),
             ana_size);
      pst_rcv_frame_buf->size = ana_size;

      /* Reception of the command */
      /* Send receipt notice (command) to main thread */
      /* Send Received Data to Main Thread */
      DevGpsRecvSndRcvData(pst_rcv_frame_buf);
    } else if (i_ret == GPS_RCV_FRMSRCH_NOT_FIXED) {
      /* Store unconfirmed data in the received data storage buffer, */
      /* and leave no unanalyzed data.                       */
      memset(pst_rcv_buf, 0, sizeof(TG_GPS_RECV_RcvBuf)); /* Ignore -> No applicable rules for MISRA-C */
      memcpy(& (pst_rcv_buf->r_buf[0]),
             &(pst_ana_data_buf->datbuf[pst_ana_data_buf->offset]),
             ana_size);
      pst_rcv_buf->r_size = ana_size;
      i_ret = GPS_RCV_FRMSRCH_NO_DATA;
    } else if (i_ret == GPS_RCV_FRMSRCH_ERR_FORMAT) {
      /* [Measures against resetting with 1S + _CWORD82_]From here */
      /* Clears the unanalyzed data stored in the receive data storage buffer, */
      /* and flag is set to "No Unparsed Data" so that data can be parsed from the beginning of the next. */
      memset(&(pst_rcv_buf->r_buf[0]), 0, pst_rcv_buf->r_size);
      pst_rcv_buf->r_size = 0;
      i_ret = GPS_RCV_FRMSRCH_NO_DATA;
      /* [Measures against resetting with 1S + _CWORD82_]Up to this point */
      /* Since this section discards garbage data, */
      /* not subject to diagnosis registration.     */
      /* Diagnosis registration check */
    } else if (i_ret == GPS_RCV_FRMSRCH_NO_DATA) {
      /* Ignore -> No applicable rules for MISRA-C */
      memset(pst_rcv_buf, 0, sizeof(TG_GPS_RECV_RcvBuf));
      /* Termination */
    } else {
      /* No unanalyzed data is assumed because it is impossible. */
      i_ret = GPS_RCV_FRMSRCH_NO_DATA;

      /* Ignore -> No applicable rules for MISRA-C */
      memset(pst_rcv_buf, 0, sizeof(TG_GPS_RECV_RcvBuf));
    }

    /* Update analysis data offset */
    pst_ana_data_buf->offset += ana_size;

    /* Repeat until no unanalyzed data is found. */
  } while (i_ret != GPS_RCV_FRMSRCH_NO_DATA);
}

/********************************************************************************
 * MODULE     : DEV_RcvDATA
 * ABSTRACT   : Acknowledgement
 * FUNCTION   : Send message notifications to the communication management thread 
 * ARGUMENT   : TG_GPS_RCV_DATA   *ptg_rcv_data....I/F information between the receiving thread 
 *                                                and the communication management thread
 * NOTE       :
 * RETURN     : RET_API  :RET_NORMAL:Normal completion
 *                       :RET_ERROR:ABENDs
 ********************************************************************************/
RET_API DevRcvData(const TG_GPS_RCV_DATA* ptg_rcv_data) {
  RET_API                 ret = RET_NORMAL; /* Return value */
  u_int16                 w_size = 0;        /* Data length setting */
  u_int16                 w_all_len = 0;      /* Sent message length */
  u_int16                 w_mode = 0;        /* Mode information */
  RID                     req_id = 0;       /* Resources ID */
  T_APIMSG_MSGBUF_HEADER  tg_header;            /* Message header */

  // Initialzie struct
  memset(&tg_header, 0, sizeof(tg_header));

  /* Transmitting buffer */
  u_int8 by_snd_buf[(sizeof(T_APIMSG_MSGBUF_HEADER) + sizeof(TG_GPS_RCV_DATA))];

  /* Calculation of transmission data length     */
  w_size = ptg_rcv_data->bydata_len + sizeof(ptg_rcv_data->dwret_status) + sizeof(ptg_rcv_data->bydata_len);

  /* <<Creation of message header section>>>>  */
  tg_header.signo           = 0;                  /* Signal information setting */
  tg_header.hdr.sndpno      = PNO_NAVI_GPS_RCV;   /* Source thread No. setting */
  tg_header.hdr.respno      = 0;                  /* Destination process No. */
  tg_header.hdr.cid         = CID_GPS_RECVDATA;   /* Command ID setting */
  tg_header.hdr.msgbodysize = w_size;              /* Message data length setting */
  tg_header.hdr.rid         = req_id;             /* Resource ID Setting */
  tg_header.hdr.reserve     = 0;                  /* Reserved area clear */

  memcpy(&by_snd_buf[ 0 ], &tg_header, sizeof(T_APIMSG_MSGBUF_HEADER));
  /* <<Creation of data section>>     */
  /* Copy data to send buffer  */
  memcpy(&by_snd_buf[ sizeof(T_APIMSG_MSGBUF_HEADER)], ptg_rcv_data, w_size);

  /* <<Messaging>>    */
  /* Calculation of Sent Message Length    */
  w_all_len = w_size + sizeof(T_APIMSG_MSGBUF_HEADER);

  /* Mode information(Reserved) */
  w_mode = 0;

  /* Message transmission request */
  ret = _pb_SndMsg(PNO_NAVI_GPS_MAIN, w_all_len, reinterpret_cast<void*>(by_snd_buf), w_mode);

  /* End of the process */
  return ret;
}

/********************************************************************************
 * MODULE   : DEV_Gps_Recv_SndRcvData
 * ABSTRACT : Receive data transmission processing
 * FUNCTION : Transmit received data
 * ARGUMENT : TG_GPS_RECV_RcvFrameBuf *frame_baf Receive frame buffer pointer
 * NOTE     : Fetches a command from the receive frame buffer and
 *            issue received data notifications to the main thread
 * RETURN   : None
 ********************************************************************************/
void DevGpsRecvSndRcvData(const TG_GPS_RECV_RcvFrameBuf* p_frame_buf) {
  TG_GPS_RCV_DATA tg_rcv_data;
  u_int16         w_cmd_len = 0;

  // Initialzie struct
  memset(&tg_rcv_data, 0, sizeof(tg_rcv_data));

  if (p_frame_buf != NULL) {
    w_cmd_len = p_frame_buf->size;

    if (w_cmd_len <= GPS_READ_LEN) {
      /* Ignore -> No applicable rules for MISRA-C */
      memset(&tg_rcv_data, 0, sizeof(TG_GPS_RCV_DATA));

      /* Status Settings */
      tg_rcv_data.dwret_status = GPS_RECVOK;

      /* Command length setting */
      tg_rcv_data.bydata_len = w_cmd_len;

      /* Receive data setting */
      /* Sending from the Beginning of the Sirf Binary #03 */
      memcpy(&tg_rcv_data.bygps_data[0], &p_frame_buf->buf[0], w_cmd_len);

      /* Issuance of reception notice */
      DevRcvData(&tg_rcv_data); /* Ignore -> No applicable rules for MISRA-C */
    }
  }
}

/**
 * @brief
 *   Pos_Gps_Recv Thread Stop Processing
 */
void DevGpsRecvThreadStopProcess(void) {
#if FTEN_DRIVER
  GpsMngApiClose(&g_gps_mng_obj);
#endif
  PosTeardownThread(ETID_POS_GPS_RECV);
  return;
}

/*---------------------------------------------------------------------------*/
/*EOF*/
