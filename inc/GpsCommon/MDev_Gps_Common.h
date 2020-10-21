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

#ifndef  INC_GPSCOMMON_MDEV_GPS_COMMON_H_
#define  INC_GPSCOMMON_MDEV_GPS_COMMON_H_

/*---------------------------------------------------------------------------*/
// Include files

#include "positioning_def.h"
#include "MDev_Gps_Main.h"

/*---------------------------------------------------------------------------*/
// Define

#define NMEA_CHECKSUM_CHAR            '*'    /* *## (HEX) */
#define NMEA_DATA_SEPARATOR           ','

#define GPS_NMEA_FIELD_LEN_MAX        (GPS_NMEA_MAX_SZ)

// NMEA GGA Sentence
/* Fix Status */
#define GPS_NMEA_GGA_FIX_STATUS_NON   0 /* invalid */
#define GPS_NMEA_GGA_FIX_STATUS_GPS   1 /* GPS SPS Mode, Fix valid */
#define GPS_NMEA_GGA_FIX_STATUS_DGPS  2 /* DGPS SPS Mode, Fix valid */
#define GPS_NMEA_GGA_FIX_STATUS_DR    6 /* Estimated/Dead Reckoning */
/* Field No */
#define GPS_NMEA_FNO_GGA_FS           6
#define GPS_NMEA_FNO_GGA_MSL          9

/* number of SV Fields */
#define GPS_NMEA_NUM_GSA_SV           12
/* Fix Status */
#define GPS_NMEA_GSA_FIX_STS_NON      1
#define GPS_NMEA_GSA_FIX_STS_2D       2
#define GPS_NMEA_GSA_FIX_STS_3D       3
/* Field No */
#define GPS_NMEA_FNO_GSA_FS           2
#define GPS_NMEA_FNO_GSA_SV           3

/* number of Satellite Infomation Fields (sv+elv+az+cno) */
#define GPS_NMEA_NUM_GSV_SINFO        4
    /* Field No */
#define GPS_NMEA_FNO_GSV_NOSV         3
#define GPS_NMEA_FNO_GSV_SV           4
#define GPS_NMEA_FNO_GSV_ELV          5
#define GPS_NMEA_FNO_GSV_AZ           6
#define GPS_NMEA_FNO_GSV_CNO          7

#define GPS_NMEA_RMC_STS_INVALID      'V'
#define GPS_NMEA_RMC_STS_VALID        'A'
#define GPS_NMEA_RMC_IND_NORTH        'N'
#define GPS_NMEA_RMC_IND_EAST         'E'

/* Field No */
#define GPS_NMEA_FNO_RMC_UTC          1
#define GPS_NMEA_FNO_RMC_STATUS       2
#define GPS_NMEA_FNO_RMC_LATITUDE     3
#define GPS_NMEA_FNO_RMC_NS           4
#define GPS_NMEA_FNO_RMC_LONGITUDE    5
#define GPS_NMEA_FNO_RMC_EW           6
#define GPS_NMEA_FNO_RMC_SPEED        7
#define GPS_NMEA_FNO_RMC_COG          8
#define GPS_NMEA_FNO_RMC_DATE         9

// Diag Fix Count
#define GPS_DIAG_FIX_COUNT_3D         (0x01)
#define GPS_DIAG_FIX_COUNT_2D         (0x02)
#define GPS_DIAG_FIX_COUNT_ELSE       (0x04)

/* GPS time setting related */
#define GPS_SETTIME_RANGE             5   /* 5 seconds : Valid range of GPS time setting */
#define GPS_TIME_RANGE                10  /* 10 seconds : GPS time change range */

/* NAV-SVINFO logging status */
#define GPS_NAVSVINFO_STS_INIT        0
#define GPS_NAVSVINFO_STS_READY       1
#define GPS_NAVSVINFO_STS_DONE        2
#define GPS_NAVSVINFO_STS_SETTING     3

/* Time setting information  */
#define GPS_TIME_RX                   0   /* Time not set */
#define GPS_TIME_ROLOVR               1   /* First GPS reception(for rollover) */
#define GPS_TIME_SET                  2   /* Time setting in progress */

/*!
    @brief Satellite position information
*/
typedef struct GpsSatelliteInfoStr {
  uint8_t  sv;                    /* Satellite number       */
  uint8_t  elv;                   /* Height 0-99      */
  uint16_t az;                    /* Orientation 0-359     */
  uint8_t  cno;                   /* Signal strength       */
  uint8_t  sts;                   /* Reception status */
} GpsSatelliteInfo;

/*---------------------------------------------------------------------------*/
// Global values

extern BOOL g_gps_rcv_thread_stop;
extern uint8_t g_is_gpstime_sts;

/*---------------------------------------------------------------------------*/
// Prototype

void                ChangeStatusGpsCommon(u_int32 sts);
void                RtyResetGpsCommon(void);
void                SendRtyResetGpsCommon(void);
void                SendReqGpsCommon(void);
void                GPSResetReqGpsCommon(void);
void                CyclDataTimeOutGpsCommon(void);
RET_API             CheckFrontStringPartGpsCommon(const u_char *p_tartget, const u_char *p_start);
int32               JudgeFormatGpsCommon(u_char *p_send_data, u_int32 ul_length, TG_GPS_OUTPUT_FORMAT *p_format);
RET_API             CheckSumGpsCommon(const u_int8 p_uc_data[], u_int32 ul_length, TG_GPS_OUTPUT_FORMAT e_cmd_info);
u_int8              AtoHGpsCommon(u_int8 ascii);
RET_API             DevGpsSaveCmd(TG_GPS_SAVECMD *p_send_data);
void                SendSaveCmdGpsCommon(void);
void                DeleteSaveCmdGpsCommon(void);
void                DeleteAllSaveCmdGpsCommon(void);
void                RcvCyclCmdNmeaGpsCommon(u_int8 *p_uc_data, u_int32 ul_len, TG_GPS_OUTPUT_FORMAT s_output_format);
int32               CheckSendCmdGpsCommon(const u_char *p_rcv_data, u_int32 ul_length, TG_GPS_OUTPUT_FORMAT *p_format);
int32_t             GetStringFromNmeaGpsCommon(u_int8 field_number, u_int8 *p_src, char* p_dst, size_t size);
double              GetNumericFromNmeaGpsCommon(u_int8 field_number, u_int8 *p_src, BOOL *p_valid);
int32               GetIntegerFromNmeaGpsCommon(u_int8 field_number, u_int8 *p_src);
int32               GetLonLatFromNmeaGpsCommon(u_int8 field_number, u_int8 *p_src, BOOL *p_valid);
u_int16             GetHeadingFromNmeaGpsCommon(u_int8 field_number, u_int8 *p_src, BOOL *p_valid);
int32               GetAltitudeFromNmeaGpsCommon(u_int8 field_number, u_int8 *p_src, BOOL *p_valid);
u_int16             GetSpeedFromNmeaGpsCommon(u_int8 field_number, u_int8 *p_src, BOOL *p_valid);
void                StopThreadGpsCommon(void);
uint8_t             JudgeFormatOrderGpsCommon(TG_GPS_OUTPUT_FORMAT s_format_1, TG_GPS_OUTPUT_FORMAT s_format_2);
ENUM_GPS_NMEA_INDEX GetNmeaIdxGpsCommon(TG_GPS_OUTPUT_FORMAT s_output_format);
void                SendResetReqGpsCommon(void);


void                WriteGpsTimeToBackup(uint8_t flag, POS_DATETIME* pst_datetime);
void                DevGpsCommSettingTime(void);
RET_API             GpsSetPosBaseEvent(uint16_t snd_pno, int32_t event_val);
uint8_t             HardResetChkGpsCommon(void);
void                RcvCyclCmdExtGpsCommon(u_int8 *p_data, u_int32 len, TG_GPS_OUTPUT_FORMAT format);

BOOL                DevGpsRcvProcGpsResetResp(TG_GPS_RCV_DATA*);
void                DevGpsRcvProcTimeSetResp(TG_GPS_RCV_DATA*);
void                DevGpsRcvProcWknRolloverGetResp(TG_GPS_RCV_DATA* p_data);
void                DevGpsRcvProcNavSvinfoAddResp(TG_GPS_RCV_DATA* p_data);
void                DevGpsReadGpsTime(POS_DATETIME* p_st_datetime);
RET_API             DevGpsRcvProcCmnResp(TG_GPS_RCV_DATA* p_data, uint8_t cmd);
RET_API             DevGpsResetReq(PNO us_pno, RID uc_rid);
void                DevGpsSettingTime(POS_DATETIME* p_st_datetime);
int                 DevGpsYMD2JD(int y, int m, int d);
BOOL                DevGpsYMD2GPSTIME(const int32 y, const int32 m, const int32 d,
                                        const int32 hh, const int32 mm, const int32 ss,
                                        u_int16* gpsw, u_int32* gpsws);
void                DevGpsSetChkSum(u_int8* uc_buffer, u_int32 ul_length);
RET_API             DevGpsCatPollReq(uint8_t *p_ublox_data, uint16_t kind);
void                DevGpsSetGpsweekcorcnt(void);
RET_API             DevGpsNavTimeUTCAddReq(void);
RET_API             DevGpsWknRolloverGetReq(void);
BOOL                DevGpsCheckGpsTime(uint16_t set_gpsw, uint32_t set_gpsws,
                              uint16_t rcv_gpsw, uint32_t rcv_gpsws, int32_t offset_range);

void                DevGpsCnvLonLatNavi(SENSORLOCATION_LONLATINFO_DAT* p_lonlat,
                                                  u_int8 fix_sts, int32 lon, int32 lat);
void                DevGpsCnvAltitudeNavi(SENSORLOCATION_ALTITUDEINFO_DAT* p_altitude,
                                                  u_int8 fix_sts, int32 alt);
void                DevGpsCnvHeadingNavi(SENSORMOTION_HEADINGINFO_DAT* p_heading,
                                                  u_int8 fix_sts, u_int16 heading);
/*---------------------------------------------------------------------------*/
#endif  // INC_GPSCOMMON_MDEV_GPS_COMMON_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
