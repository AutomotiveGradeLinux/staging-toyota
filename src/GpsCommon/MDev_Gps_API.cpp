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
* @file MDev_Gps_API.cpp
*/

/*---------------------------------------------------------------------------*/
// Include files

#include "MDev_Gps_API.h"
#include "positioning_hal.h"
#include "positioning_def.h"
#include "MDev_Gps_Nmea.h"

/*---------------------------------------------------------------------------*/
// Functions

/******************************************************************************
@brief      SendNmeaGps<BR>
            NMEA transmission process
@outline    Send NMEA to VehicleSensor Thread
@param[in]  TG_GPS_NMEA* : pstNmeaData ... NMEA data
@param[out] none
@return     RET_API
@retval     RET_NORMAL   : Normal completion
@retval     RET_ERROR    : ABENDs
*******************************************************************************/
RET_API SendNmeaGps(const MDEV_GPS_NMEA* p_nmea_data) {
  MDEV_GPS_RAWDATA_NMEA_MSG s_send_msg;
  SENSOR_MSG_GPSDATA        s_msg_buf;
  RET_API                   ret = RET_NORMAL;
  PNO                       u_pno = PNO_VEHICLE_SENSOR;

  /* Create GPS Data Notification Message */
  (void)memset(&s_send_msg, 0x00, sizeof(s_send_msg));

  /* Message header */
  s_send_msg.h_dr.hdr.sndpno = PNO_NAVI_GPS_MAIN;
  s_send_msg.h_dr.hdr.respno = 0x0000;
  s_send_msg.h_dr.hdr.cid = CID_GPS_DATA;
  s_send_msg.h_dr.hdr.msgbodysize = sizeof(MDEV_GPS_RAWDATA_NMEA);
  s_send_msg.h_dr.hdr.rid = 0x00;

  /* Message data */
  s_send_msg.st_data.e_kind = MDEV_GPS_DATA_RAWDATA_NMEA;
  (void)memcpy(&(s_send_msg.st_data.st_nmea_data), p_nmea_data, sizeof(s_send_msg.st_data.st_nmea_data));

  /* Create message buffer */
  (void)memset(&s_msg_buf, 0, sizeof(s_msg_buf));
  (void)memcpy(&(s_msg_buf.st_head.hdr), &(s_send_msg.h_dr.hdr), sizeof(T_APIMSG_HEADER));

  s_msg_buf.st_para.ul_did = POS_DID_GPS_NMEA;
  s_msg_buf.st_para.us_size = GPS_NMEA_SZ;

  (void)memcpy(&(s_msg_buf.st_para.uc_data), &(s_send_msg.st_data.st_nmea_data), s_msg_buf.st_para.us_size);

  ret = _pb_SndMsg(u_pno, sizeof(s_msg_buf), &s_msg_buf, 0);

  if (ret != RET_NORMAL) {
    POSITIONING_LOG("_pb_SndMsg ERROR!! [ret=%d]\n", ret);
    ret = RET_ERROR;
  }

  return ret;
}

/**
 * @brief
 *   GPS processing data transmission process
 *
 * @param[in]  pstGpsTime   SENSOR_MSG_GPSTIME - GPS time information
 * @param[in]  p_lonlat     SENSORLOCATION_LONLATINFO - Latitude and longitude information
 * @param[in]  p_altitude   SENSOR_LOCATION_ALTITUDEINFO - Altitude information
 * @param[in]  p_heading    SENSORMOTION_HEADINGINFO_DAT - Bearing information
 *
 * @return  RET_NORMAL    Normal completion
 *          RET_ERROR     ABENDs
 */
RET_API SendCustomGps(const SENSOR_MSG_GPSTIME* p_gps_time,
                      const SENSORLOCATION_LONLATINFO_DAT* p_lonlat,
                      const SENSORLOCATION_ALTITUDEINFO_DAT* p_altitude,
                      const SENSORMOTION_HEADINGINFO_DAT* p_heading,
                      const NAVIINFO_DIAG_GPS* p_diag_data) {
  SENSOR_MSG_GPSDATA    s_send_msg = {0};
  MDEV_GPS_CUSTOMDATA*  p_custom_data = NULL;
  RET_API               ret   = RET_NORMAL;
  PNO                   u_pno = PNO_VEHICLE_SENSOR;

  /** Create GPS Data Notification Message */
  /* Fast _CWORD71_ processing(memset fix) */
  /* Initializes an area whose value is undefined in the message buffer. */
  (void)memset(&s_send_msg.st_head, 0x00, sizeof(s_send_msg.st_head));

  /** Message header */
  s_send_msg.st_head.hdr.sndpno       = PNO_NAVI_GPS_MAIN;
  s_send_msg.st_head.hdr.respno       = 0x0000;
  s_send_msg.st_head.hdr.cid          = CID_GPS_DATA;
  s_send_msg.st_head.hdr.msgbodysize  = sizeof(MDEV_GPS_CUSTOMDATA);
  s_send_msg.st_head.hdr.rid          = 0x00;

  /** Message data */
  s_send_msg.st_para.ul_did           = VEHICLE_DID_GPS_CUSTOMDATA;
  s_send_msg.st_para.us_size          = sizeof(MDEV_GPS_CUSTOMDATA);
  p_custom_data                       = reinterpret_cast<MDEV_GPS_CUSTOMDATA *>(&(s_send_msg.st_para.uc_data));

  (void)memcpy(&(p_custom_data->st_lonlat),   p_lonlat,   sizeof(SENSORLOCATION_LONLATINFO_DAT));
  (void)memcpy(&(p_custom_data->st_altitude), p_altitude, sizeof(SENSORLOCATION_ALTITUDEINFO_DAT));
  (void)memcpy(&(p_custom_data->st_heading),  p_heading,  sizeof(SENSORMOTION_HEADINGINFO_DAT));
  (void)memcpy(&(p_custom_data->st_diag_gps),  p_diag_data,  sizeof(NAVIINFO_DIAG_GPS));
  (void)memcpy(&(p_custom_data->st_gps_time),  p_gps_time,  sizeof(SENSOR_MSG_GPSTIME));
  /* Messaging */
  ret = _pb_SndMsg(u_pno, sizeof(s_send_msg), reinterpret_cast<void*>(&s_send_msg), 0);

  if (ret != RET_NORMAL) {
    ret = RET_ERROR;
  }

  return ret;
}

/******************************************************************************
@brief      SendSpeedGps<BR>
            Rate information transmission processing
@outline    Sending speed information to vehicle sensor thread
@param[in]  SENSORMOTION_SPEEDINFO_DAT* : p_seed    ... Velocity information
@param[in]  u_int16                     : us_peed ... Vehicle speed(km/h)
@param[out] none
@return     RET_API
@retval     RET_NORMAL                  : Normal completion
@retval     RET_ERROR                   : ABENDs
*******************************************************************************/
RET_API SendSpeedGps(const SENSORMOTION_SPEEDINFO_DAT* p_seed, u_int16 us_peed) {
  MDEV_GPS_NAVISPEED_MSG  s_send_msg;
  RET_API                 ret = RET_NORMAL;
  PNO                     u_pno = PNO_VEHICLE_SENSOR;

  /** Create GPS Data Notification Message */
  (void)memset(&s_send_msg, 0x00, sizeof(s_send_msg));

  /** Message header */
  s_send_msg.h_dr.hdr.sndpno      = PNO_NAVI_GPS_MAIN;
  s_send_msg.h_dr.hdr.respno      = 0x0000;
  s_send_msg.h_dr.hdr.cid         = CID_GPS_DATA;
  s_send_msg.h_dr.hdr.msgbodysize = sizeof(MDEV_GPS_NAVISPEED);
  s_send_msg.h_dr.hdr.rid         = 0x00;

  /** Message data */
  s_send_msg.st_data.e_kind         = MDEV_GPS_DATA_NAVISPEED;
  s_send_msg.st_data.us_speed_kmph  = us_peed;

  (void)memcpy( &s_send_msg.st_data.st_speed, p_seed, sizeof(s_send_msg.st_data.st_speed) );

  /* Messaging */
  ret = _pb_SndMsg(u_pno, (sizeof(T_APIMSG_MSGBUF_HEADER) + sizeof(MDEV_GPS_NAVISPEED)), &s_send_msg, 0);

  if (ret != RET_NORMAL) {
    ret = RET_ERROR;
  }

  return ret;
}

/* ++ #GPF_60_103 */
/******************************************************************************
@brief      SendTimeGps<BR>
            Time information transmission processing
@outline    Send GPS time information to vehicle sensor thread
@param[in]  MDEV_GPS_RTC* : p_rtc    ... GPS time information
@param[out] none
@return     RET_API
@retval     RET_NORMAL    : Normal completion
@retval     RET_ERROR     : ABENDs
*******************************************************************************/
RET_API SendTimeGps(const MDEV_GPS_RTC* p_rtc) {
  MDEV_GPS_GPSTIME_MGS  s_send_msg;
  RET_API               ret = RET_NORMAL;
  PNO                   u_pno = PNO_VEHICLE_SENSOR;

  /** Create GPS data notification message */
  (void)memset(&s_send_msg, 0x00, sizeof(s_send_msg));

  /** Message header */
  s_send_msg.h_dr.hdr.sndpno      = PNO_NAVI_GPS_MAIN;
  s_send_msg.h_dr.hdr.respno      = 0x0000;
  s_send_msg.h_dr.hdr.cid         = CID_GPS_DATA;
  s_send_msg.h_dr.hdr.msgbodysize = sizeof(MDEV_GPS_GPSTIME);
  s_send_msg.h_dr.hdr.rid         = 0x00;

  /** Message data */
  s_send_msg.st_data.e_kind = MDEV_GPS_DATA_GPSTIME;

  (void)memcpy(&s_send_msg.st_data.st_rtc_data, p_rtc, sizeof(s_send_msg.st_data.st_rtc_data));

  /* Messaging */
  ret = _pb_SndMsg(u_pno, (sizeof(T_APIMSG_MSGBUF_HEADER) + sizeof(MDEV_GPS_GPSTIME)), &s_send_msg, 0);

  if (ret != RET_NORMAL) {
    ret = RET_ERROR;
  }

  return ret;
}

/**
 * @brief
 *   GPS clock drift transmit process
 *
 * @param[in]  drift  Clock drift
 *
 * @return  RET_NORMAL    Normal completion
 * @return  RET_ERROR    ABENDs
 */
RET_API SendClockDriftGps(int32_t drift) {
  SENSOR_MSG_GPSDATA  s_send_msg;
  RET_API             ret = RET_NORMAL;
  PNO                 u_pno = PNO_VEHICLE_SENSOR;

  /** Create GPS Data Notification Message */
  (void)memset(&s_send_msg, 0x00, sizeof(s_send_msg));

  /** Message header */
  s_send_msg.st_head.hdr.sndpno      = PNO_NAVI_GPS_MAIN;
  s_send_msg.st_head.hdr.respno      = 0x0000;
  s_send_msg.st_head.hdr.cid         = CID_GPS_DATA;
  s_send_msg.st_head.hdr.msgbodysize = sizeof(SENSOR_MSG_GPSDATA_DAT);
  s_send_msg.st_head.hdr.rid         = 0x00;

  /** Message data */
  s_send_msg.st_para.ul_did   = VEHICLE_DID_GPS_CLOCK_DRIFT;
  s_send_msg.st_para.us_size  = sizeof(int32_t);

  (void)memcpy(&(s_send_msg.st_para.uc_data), &drift, s_send_msg.st_para.us_size);

  /* Messaging */
  ret = _pb_SndMsg(u_pno, sizeof(s_send_msg), &s_send_msg, 0);

  if (ret != RET_NORMAL) {
    ret = RET_ERROR;
  }

  return ret;
}

/**
 * @brief
 *   GPS clock-frequency transmit process
 *
 * @param[in]  Freq Clocking frequencies
 *
 * @return  RET_NORMAL    Normal completion
 * @return  RET_ERROR    ABENDs
 */
RET_API SendClockFrequencyGps(uint32_t freq) {
  SENSOR_MSG_GPSDATA  s_send_msg;
  RET_API             ret = RET_NORMAL;
  PNO                 u_pno = PNO_VEHICLE_SENSOR;

  /** Create GPS Data Notification Message */
  (void)memset(&s_send_msg, 0x00, sizeof(s_send_msg));

  /** Message header */
  s_send_msg.st_head.hdr.sndpno      = PNO_NAVI_GPS_MAIN;
  s_send_msg.st_head.hdr.respno      = 0x0000;
  s_send_msg.st_head.hdr.cid         = CID_GPS_DATA;
  s_send_msg.st_head.hdr.msgbodysize = sizeof(SENSOR_MSG_GPSDATA_DAT);
  s_send_msg.st_head.hdr.rid         = 0x00;

  /** Message data */
  s_send_msg.st_para.ul_did   = VEHICLE_DID_GPS_CLOCK_FREQ;
  s_send_msg.st_para.us_size  = sizeof(uint32_t);

  (void)memcpy(&(s_send_msg.st_para.uc_data), &freq, s_send_msg.st_para.us_size);

  /* Messaging */
  ret = _pb_SndMsg(u_pno, sizeof(s_send_msg), &s_send_msg, 0);

  if (ret != RET_NORMAL) {
    ret = RET_ERROR;
  }

  return(ret);
}

/**
 * @brief
 *   GPS rollover standard week number transmission processing
 *
 * @param[in] *p_week_rollover GPS rollover base week number
 *
 * @return  RET_NORMAL    Normal completion
 * @return  RET_ERROR    ABENDs
 */
RET_API DevGpsSndWknRollover(const SensorWknRollOverHal* p_week_rollover) {
  SENSOR_MSG_GPSDATA  s_send_msg;
  RET_API             ret = RET_NORMAL;
  PNO                 u_pno = PNO_VEHICLE_SENSOR;

  /** Create GPS Data Notification Message */
  (void)memset( &s_send_msg, 0x00, sizeof(s_send_msg) );

  /** Message header */
  s_send_msg.st_head.hdr.sndpno      = PNO_NAVI_GPS_MAIN;
  s_send_msg.st_head.hdr.respno      = 0x0000;
  s_send_msg.st_head.hdr.cid         = CID_GPS_DATA;
  s_send_msg.st_head.hdr.msgbodysize = sizeof(SensorWknRollOverHal);
  s_send_msg.st_head.hdr.rid         = 0x00;

  /** Message data */
  s_send_msg.st_para.ul_did   = VEHICLE_DID_GPS_WKNROLLOVER;
  s_send_msg.st_para.us_size  = sizeof(SensorWknRollOverHal);

  (void)memcpy(&(s_send_msg.st_para.uc_data), p_week_rollover, s_send_msg.st_para.us_size);

  /* Messaging */
  ret = _pb_SndMsg(u_pno, sizeof(s_send_msg), &s_send_msg, 0);

  if (ret != RET_NORMAL) {
    ret = RET_ERROR;
  }

  return ret;
}

/******************************************************************************
@brief      DevGpsRstAnsSend<BR>
            Reset response issuance processing
@outline    Issue a reset response
@param[in]  PNO        : u_pno    ... Notify-To Process No.
@param[in]  RID        : uc_rid    ... Response resource ID
@param[in]  u_int32    : ul_rst_sts ... Response result
@param[out] none
@return     int32
@retval     RET_NORMAL : Normal
@retval     RET_ERROR  : Abnormality
*******************************************************************************/
int32 DevGpsRstAnsSend(PNO u_pno, RID uc_rid, u_int32 ul_rst_sts) {
  TG_GPS_RET_RESET_MSG  s_send_msg;
  RET_API               ret = RET_NORMAL;
  PCSTR                 l_thread_name;

  if (u_pno != PNO_NONE) {
    /** Create GPS Reset Notification Message */
    (void)memset(&s_send_msg, 0x00, sizeof(s_send_msg));

    /** Message data */
    s_send_msg.data.ret_rst_status = ul_rst_sts;

    /* Messaging */
    l_thread_name = _pb_CnvPno2Name(u_pno);
    /* External Process Transmission and Reception Messages */
    ret = _pb_SndMsg_Ext(l_thread_name,
                           CID_POSIF_REQ_GPS_RESET,
                           sizeof(s_send_msg.data),
                           reinterpret_cast<void *>(&(s_send_msg.data)),
                           0);
    if (ret != RET_NORMAL) {
      POSITIONING_LOG("GpsCommCtl # DevGpsRstAnsSend SndMsg Error ret[%d] pno[%03X]\n", ret, u_pno);
      ret = RET_ERROR;
    }
  }

  return ret;
}


/**
 * @brief      Time setting response issuance processing
 *
 * @param[in]  us_pno    Notify-To Process No.
 * @param[in]  uc_rid    Response resource ID
 * @param[in]  ul_rst_sts Response result
 *
 * @return     Processing result
 * @retval     RET_NORMAL : Normal
 * @retval     RET_ERROR  : Abnormality
 */
int32 DevGpsTimesetAnsSend(PNO us_pno, RID uc_rid, u_int32 ul_rst_sts) {
    TG_GPS_RET_TIMESET_MSG  st_snd_msg;
    RET_API         ret = RET_NORMAL;

    /** Create GPS Reset Notification Message */
    memset( &st_snd_msg, 0x00, sizeof(st_snd_msg) ); /* QAC 3200 */
    /** Message header */
    st_snd_msg.header.hdr.sndpno     = PNO_NAVI_GPS_MAIN;
    st_snd_msg.header.hdr.respno     = 0x0000;
    st_snd_msg.header.hdr.cid        = CID_GPS_RETTIMESETTING;
    st_snd_msg.header.hdr.msgbodysize = sizeof(st_snd_msg.status);
    st_snd_msg.header.hdr.rid        = uc_rid;
    /** Message data */
    st_snd_msg.status                = ul_rst_sts;

    /* Messaging */
    if (us_pno != PNO_NONE) {
        ret = _pb_SndMsg(us_pno, sizeof(st_snd_msg), &st_snd_msg, 0);

        if (ret != RET_NORMAL) {
            POSITIONING_LOG("DevGpsTimesetAnsSend SndMsg Error ret[%d] pno[%03X] \r\n", ret, us_pno);

            ret = RET_ERROR;
        }
    }

    return(ret);
}


/**
 * @brief
 *   GPS clock drift transmit process
 *
 * @param[in]  drift  Clock drift
 *
 * @return  RET_NORMAL    Normal completion
 * @return  RET_ERROR    ABENDs
 *
 */
RET_API DevSendGpsConnectError(BOOL is_err) {
  SENSOR_MSG_GPSDATA  s_send_msg;
  RET_API             ret = RET_NORMAL;
  PNO                 u_pno = PNO_VEHICLE_SENSOR;

  /** Create GPS Data Notification Message */
  (void)memset(&s_send_msg, 0x00, sizeof(s_send_msg));

  /** Message header */
  s_send_msg.st_head.hdr.sndpno      = PNO_NAVI_GPS_MAIN;
  s_send_msg.st_head.hdr.respno      = 0x0000;
  s_send_msg.st_head.hdr.cid         = CID_GPS_DATA;
  s_send_msg.st_head.hdr.msgbodysize = sizeof(SENSOR_MSG_GPSDATA_DAT);
  s_send_msg.st_head.hdr.rid         = 0x00;

  /** Message data */
  s_send_msg.st_para.ul_did   = POSHAL_DID_GPS_CONNECT_ERROR;
  s_send_msg.st_para.us_size  = sizeof(uint32_t);

  (void)memcpy(&(s_send_msg.st_para.uc_data), &is_err, s_send_msg.st_para.us_size);

  /* Messaging */
  ret = _pb_SndMsg(u_pno, sizeof(s_send_msg), &s_send_msg, 0);

  if (ret != RET_NORMAL) {
    ret = RET_ERROR;
  }

  return ret;
}


RET_API SndGpsTimeRaw(const SENSOR_GPSTIME_RAW* ps_gpstime_raw) {
    SENSOR_MSG_GPSDATA  st_snd_msg;
    RET_API       ret;
    PNO         _us_pno = PNO_VEHICLE_SENSOR;

    /** Create GPS Data Notification Message */
    (void)memset( &st_snd_msg, 0x00, sizeof(st_snd_msg) ); /* QAC 3200 */
    /** Message header */
    st_snd_msg.st_head.hdr.sndpno  = PNO_NAVI_GPS_MAIN;
    st_snd_msg.st_head.hdr.respno  = 0x0000;
    st_snd_msg.st_head.hdr.cid   = CID_GPS_DATA;
    st_snd_msg.st_head.hdr.msgbodysize = sizeof(SENSOR_MSG_GPSDATA_DAT);
    st_snd_msg.st_head.hdr.rid   = 0x00;
    /** Message data */
    st_snd_msg.st_para.ul_did   = VEHICLE_DID_GPS_TIME_RAW;
    st_snd_msg.st_para.us_size    = sizeof(SENSOR_GPSTIME_RAW);
    (void)memcpy(&(st_snd_msg.st_para.uc_data), ps_gpstime_raw, st_snd_msg.st_para.us_size); /* QAC 3200 */

    /* Messaging */
    ret = _pb_SndMsg( _us_pno, sizeof(st_snd_msg), &st_snd_msg, 0 );
    if (ret != RET_NORMAL) {
        POSITIONING_LOG("_pb_SndMsg ERROR!! [ret=%d]\n", ret);
        ret = RET_ERROR;
    }

    return(ret);
}

/*---------------------------------------------------------------------------*/
/*EOF*/
