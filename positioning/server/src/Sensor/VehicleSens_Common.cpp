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

/*******************************************************************************
 *  File name    :VehicleSens_Common.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor common processing(VEHICLE_COMMON)
 *  Module configuration  :VehicleSensmemcmp()        Functions for Common Processing Memory Block Comparisons
 *                    VehicleSensCheckDid()      Common Processing Data ID Check Function
 *                    VehicleSensGetDataMasterOffset()  Get function for common processing data master offset value
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_Common.h"
#include "POS_private.h"
#include <system_service/ss_ver.h>
#include <system_service/ss_package.h>
#include "gps_hal.h"
#include "VehicleSens_DataMaster.h"


#define _pb_strcat(pdest, psrc, size)       (strncat(pdest, psrc, size) , (0))


/*************************************************/
/*           Global variable                      */
/*************************************************/
static const VEHICLESENS_DID_OFFSET_TBL kGstDidList[] = {
    /*      Data ID              Offset size    Reserved */
    {  VEHICLE_DID_DESTINATION,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_HV,              VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_STEERING_WHEEL,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_VB,              VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_IG,              VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_MIC,            VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_BACKDOOR,          VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_PKB,            VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_ADIM,            VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_ILL,            VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_RHEOSTAT,          VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_SYSTEMP,          VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_SPEED_PULSE,        VEHICLESENS_OFFSET_20WORD,    {0, 0} },
    {  POSHAL_DID_SPEED_PULSE_FLAG,      VEHICLESENS_OFFSET_20WORD,    {0, 0} },
    {  POSHAL_DID_SPEED_KMPH,          VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GYRO_X,            VEHICLESENS_OFFSET_20WORD,    {0, 0} },
    {  POSHAL_DID_GYRO_Y,            VEHICLESENS_OFFSET_20WORD,    {0, 0} },
    {  POSHAL_DID_GYRO_Z,            VEHICLESENS_OFFSET_20WORD,    {0, 0} },
    {  POSHAL_DID_GSNS_X,            VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GSNS_Y,            VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GSNS_Z,            VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_REV,            VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GPS_ANTENNA,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_SNS_COUNTER,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_GPS_COUNTER,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GPS_VERSION,        VEHICLESENS_OFFSET_GPS_FORMAT,  {0, 0} },
    {  VEHICLE_DID_LOCATION,          VEHICLESENS_OFFSET_GPS_FORMAT,  {0, 0} },
    /* ++GPS _CWORD82_ support */
    {  POSHAL_DID_GPS__CWORD82___CWORD44_GP4,      VEHICLESENS_OFFSET_GPS_FORMAT,  {0, 0} },
    {  VEHICLE_DID_GPS__CWORD82__NMEA,        VEHICLESENS_OFFSET_GPS_NMEA,  {0, 0} },
    {  POSHAL_DID_GPS_NMEA,                   VEHICLESENS_OFFSET_GPS_NMEA,  {0, 0} },
    {  POSHAL_DID_GPS__CWORD82__FULLBINARY,      VEHICLESENS_OFFSET_GPS_FORMAT,  {0, 0} },
    /* --GPS _CWORD82_ support */
    {  VEHICLE_DID_REV_LINE,          VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_REV_CAN,          VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
#if CONFIG_SENSOR_EXT_VALID            /* Initial Sensor Support */
    {  POSHAL_DID_GYRO_EXT,          VEHICLESENS_OFFSET_20WORD,    {0, 0} },
    {  POSHAL_DID_SPEED_PULSE_FST,      VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GYRO_X_FST,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GYRO_Y_FST,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GYRO_Z_FST,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_SPEED_PULSE_FLAG_FST,    VEHICLESENS_OFFSET_20WORD_FST,  {0, 0} },
    {  POSHAL_DID_REV_FST,          VEHICLESENS_OFFSET_20WORD_FST,  {0, 0} },
#endif
    /* ++ PastModel002 support */
    {  VEHICLE_DID_GPS_UBLOX_NAV_POSLLH,    VEHICLESENS_OFFSET_GPS_UBLOX,  {0, 0} },
    {  VEHICLE_DID_GPS_UBLOX_NAV_STATUS,    VEHICLESENS_OFFSET_GPS_UBLOX,  {0, 0} },
    {  VEHICLE_DID_GPS_UBLOX_NAV_TIMEUTC,    VEHICLESENS_OFFSET_GPS_UBLOX,  {0, 0} },
    {  VEHICLE_DID_GPS_UBLOX_NAV_VELNED,    VEHICLESENS_OFFSET_GPS_UBLOX,  {0, 0} },
    {  VEHICLE_DID_GPS_UBLOX_NAV_DOP,      VEHICLESENS_OFFSET_GPS_UBLOX,  {0, 0} },
    {  VEHICLE_DID_GPS_UBLOX_NAV_TIMEGPS,    VEHICLESENS_OFFSET_GPS_UBLOX,  {0, 0} },
    {  VEHICLE_DID_GPS_UBLOX_NAV_SVINFO,    VEHICLESENS_OFFSET_GPS_UBLOX,  {0, 0} },
    {  VEHICLE_DID_GPS_UBLOX_NAV_CLOCK,    VEHICLESENS_OFFSET_GPS_UBLOX,  {0, 0} },
    {  VEHICLE_DID_GPS_UBLOX_MON_HW,      VEHICLESENS_OFFSET_GPS_UBLOX,  {0, 0} },
    {  VEHICLE_DID_GYRO_TROUBLE,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_MAIN_GPS_INTERRUPT_SIGNAL,  VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_SYS_GPS_INTERRUPT_SIGNAL,  VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_GYRO_CONNECT_STATUS,    VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    /* -- PastModel002 support */
    {  POSHAL_DID_GPS_TIME,          VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GPS_TIME_RAW,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GPS_WKNROLLOVER,      VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_NAVIINFO_DIAG_GPS,      VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GYRO_TEMP,            VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GYRO_TEMP_FST,          VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GSNS_X_FST,            VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GSNS_Y_FST,            VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GSNS_Z_FST,            VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_PULSE_TIME,            VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_LOCATION_LONLAT,      VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_LOCATION_ALTITUDE,      VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_MOTION_SPEED,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_MOTION_HEADING,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_LOCATION_LONLAT_NAVI,    VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_LOCATION_ALTITUDE_NAVI,    VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_MOTION_SPEED_NAVI,      VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_MOTION_HEADING_NAVI,    VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_SETTINGTIME,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_MOTION_SPEED_INTERNAL,    VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GPS_CLOCK_DRIFT,      VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  POSHAL_DID_GPS_CLOCK_FREQ,        VEHICLESENS_OFFSET_NORMAL,    {0, 0} },
    {  0,                    0,                {0, 0} }    /* Termination code */
};

/*******************************************************************************
* MODULE    : VehicleSensmemcmp
* ABSTRACT  : Functions for Common Processing Memory Block Comparisons
* FUNCTION  : Memory block comparison processing
* ARGUMENT  : *vp_data1  : Comparison target address 1
*           : *vp_data2  : Comparison target address 2
*           : uc_size  : Comparison Size
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*           : VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensmemcmp(const void *vp_data1, const void *vp_data2, size_t uc_size) {
    u_int8 ret = VEHICLESENS_EQ;
    const u_int8 *ucp_data1 = (const u_int8 *)vp_data1;
    const u_int8 *ucp_data2 = (const u_int8 *)vp_data2;

    /* Loop by data size */
    while (uc_size > 0) {
        if (*ucp_data1 != *ucp_data2) {
            /* Data mismatch */
            ret = VEHICLESENS_NEQ;
            break;
        }
        ucp_data1++;
        ucp_data2++;
        uc_size--;
    }
    return( ret );
}

/*******************************************************************************
* MODULE    : VehicleSensCheckDid
* ABSTRACT  : Common Processing Data ID Check Function
* FUNCTION  : Check if the specified DID corresponds to the vehicle sensor information
* ARGUMENT  : ul_did  : Data ID
* NOTE      :
* RETURN    : VEHICLESENS_INVALID    :Disabled
*           : VEHICLESENS_EFFECTIVE  :Enabled
******************************************************************************/
int32 VehicleSensCheckDid(DID ul_did) {
    int32 i = 0;
    int32 ret = VEHICLESENS_INVALID;

    while (0 != kGstDidList[i].ul_did) {  // LCOV_EXCL_BR_LINE 200: did always valid
        if (kGstDidList[i].ul_did == ul_did) {
            /* DID enabled */
            ret = VEHICLESENS_EFFECTIVE;
            break;
        }
        i++;
    }
    return( ret );
}

/*******************************************************************************
* MODULE    : VehicleSensGetDataMasterOffset
* ABSTRACT  : Get function for common processing data master offset value
* FUNCTION  : Get the fixed offset value for a given DID
* ARGUMENT  : ul_did  : Data ID
* NOTE      :
* RETURN    : Offset value(Returns 0 if DID is invalid)
******************************************************************************/
u_int16  VehicleSensGetDataMasterOffset(DID ul_did) {
    int32    i = 0;              /* Generic counters      */
    u_int16    ret = 0;            /* Return value of this function    */

    while (0 != kGstDidList[i].ul_did) {  // LCOV_EXCL_BR_LINE 200: did always valid
        if (kGstDidList[i].ul_did == ul_did) {
            /* DID enabled */
            ret = kGstDidList[i].us_offset;
            break;
        }
        i++;
    }
    return( ret );
}

/* Ignore->MISRA-C++:2008 Rule 2-7-2 */
#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*******************************************************************************
* MODULE    : VehicleSensGetDataMasterExtOffset
* ABSTRACT  : Get function for common processing data master offset value
* FUNCTION  : Get the fixed offset value for the first package delivery of the specified DID
* ARGUMENT  : ul_did  : Data ID
* NOTE      :
* RETURN    : Offset value(Returns 0 for unspecified DID)
******************************************************************************/
u_int16  VehicleSensGetDataMasterExtOffset(DID ul_did) {
    u_int16    usRet = 0;            /* Return value of this function    */

    switch (ul_did) {
        case POSHAL_DID_GYRO_EXT:
        case POSHAL_DID_GYRO_X:
        case POSHAL_DID_GYRO_Y:
        case POSHAL_DID_GYRO_Z:
        case POSHAL_DID_GSNS_X:   /* Ignore->MISRA-C++:2008 Rule 2-7-2 */
        case POSHAL_DID_GSNS_Y:   /* Ignore->MISRA-C++:2008 Rule 2-7-2 */
        case POSHAL_DID_GSNS_Z:
        case POSHAL_DID_SPEED_PULSE:
        {
            /* Returns the extended package size (one data 2byte) at the time of initial delivery */
            usRet    = VEHICLESENS_OFFSET_10WORD_PKG_EXT;
            break;
        }
        case POSHAL_DID_SNS_COUNTER:
        case POSHAL_DID_REV:
        {
            /* Returns the extended package size (one data 1byte) at the time of initial delivery */
            usRet    = VEHICLESENS_OFFSET_BYTE_PKG_EXT;
            break;
        }
        case POSHAL_DID_GYRO_TEMP:
        {
            /* Returns the extended package size (one data 2byte) at the time of initial delivery */
            usRet    = VEHICLESENS_OFFSET_WORD_PKG_EXT;
            break;
        }
        case POSHAL_DID_PULSE_TIME:
        {
            /* Returns the expansion package size (132 bytes per data) at the time of initial delivery */
            usRet = VEHICLESENS_OFFSET_32LONG_PKG_EXT;
            break;
        }
        default:  /* Other than the above */  /* Ignore->MISRA-C++:2008 Rule 6-3-1, 6-4-1 */
            break;
    }
    return( usRet );
}
#endif
/* Ignore->MISRA-C++:2008 Rule 2-7-2 */

/**
 * @brief
 *  GPS version information setting request
 *
 *  Request to set GPS version information to SS
 *
 * @param[in]  pstData Pointer to received message data
 */
void VehicleSensSetGpsVersion(const SENSOR_MSG_GPSDATA_DAT *pstData) {
    static BOOL      isExistGpsVersion = FALSE;
    SSVER_PkgInfo    info;
    CSSVer           cssVer;
    UNIT_TYPE        eType = UNIT_TYPE_NONE;         /* Supported HW Configuration Type   */
    EFrameworkunifiedStatus       ret;

    if (isExistGpsVersion == FALSE) {
        memset(&info, 0x00, sizeof(info));
        /* Supported HW Configuration Check */
        eType = GetEnvSupportInfo();
        if (UNIT_TYPE_GRADE1 == eType) {  // LCOV_EXCL_BR_LINE 6:cannot be this env
            // LCOV_EXCL_START 8 : dead code
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            /* GRADE1 */
            memcpy(info.version, pstData->uc_data, sizeof(info.version));

            /* Calling setPkgInfo() */
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, 
                                            "--- VehicleUtility_SetGpsVersion setPkgInfo -->");
            ret = cssVer.setPkgInfo(SS_PKG_NAVI_GPS, info);
            if (ret == eFrameworkunifiedStatusOK) {
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, 
                            "--- VehicleUtility_SetGpsVersion setPkgInfo <-- GPSVersion = %s", info.version);
                isExistGpsVersion = TRUE; /* Update Flag */
            } else {
                /* Error log */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                            "Failed to set PkgInfo EpositioningStatus = %d", ret);
            }
            // LCOV_EXCL_STOP
        } else if ( UNIT_TYPE_GRADE2 == eType ) {
          /*
           *  Note.
           *  This feature branches processing depending on the unit type.
           */
        } else {
            /* nop */
        }
    }
    return;
}

/**
 * @brief
*  Acquisition of location and time information (dump)
 *
 * @param[out]     pBuf         Dump information
 * @param[in]      Uc_get_method Retrieval method
 */
void VehicleSensGetDebugPosDate(void* pBuf, u_int8 uc_get_method) {  // LCOV_EXCL_START 7: uesd only in FrameworkunifiedOnDebugDump
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    static uint8_t  buf[DEBUG_DUMP_MAX_SIZE];
    static uint8_t  bufTmp[256];
    VEHICLESENS_DATA_MASTER stSnsData;
    SENSORLOCATION_LONLATINFO_DAT *pstLonLat;
    SENSORLOCATION_ALTITUDEINFO_DAT *pstAltitude;
    SENSORMOTION_HEADINGINFO_DAT *pstHeading;
    SENSOR_MSG_GPSDATA_DAT stGpsData;
    SENSOR_MSG_GPSTIME *pstDateTimeGps;
    NAVIINFO_DIAG_GPS *pstDiagGps;
    uint8_t i;

    memset(&buf, 0x00, sizeof(buf));
    /* Title */
    switch ( uc_get_method ) {
        case VEHICLESENS_GETMETHOD_GPS:
            snprintf(reinterpret_cast<char *>(&buf), sizeof(buf), "GPS Info");
            break;
        case VEHICLESENS_GETMETHOD_NAVI:
            snprintf(reinterpret_cast<char *>(&buf), sizeof(buf), "Navi Info");
            break;
        default:
            /* nop */
            break;
    }

    /* Latitude,Longitude */
    VehicleSensGetLocationLonLat(&stSnsData, uc_get_method);
    pstLonLat = reinterpret_cast<SENSORLOCATION_LONLATINFO_DAT*>(stSnsData.uc_data);
    memset(&bufTmp[0], 0x00, sizeof(bufTmp));
    snprintf( reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp),
            "\n [LonLat] sync:%3d, Enable:%01d, Lon:%10d, Lat:%10d, PosSts:0x%02x, PosAcc:0x%04x",
            pstLonLat->SyncCnt,
            pstLonLat->isEnable,
            pstLonLat->Longitude,
            pstLonLat->Latitude,
            pstLonLat->posSts,
            pstLonLat->posAcc);
    _pb_strcat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp));

    /* Altitude */
    VehicleSensGetLocationAltitude(&stSnsData, uc_get_method);
    pstAltitude = reinterpret_cast<SENSORLOCATION_ALTITUDEINFO_DAT*>(stSnsData.uc_data);
    memset(&bufTmp[0], 0x00, sizeof(bufTmp));
    snprintf(reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp),
            "\n [Alt]    sync:%3d, Enable:%01d, Alt:%10d",
            pstAltitude->SyncCnt,
            pstAltitude->isEnable,
            pstAltitude->Altitude);
    _pb_strcat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp));

    /* Orientation */
    VehicleSensGetMotionHeading(&stSnsData, uc_get_method);
    pstHeading = reinterpret_cast<SENSORMOTION_HEADINGINFO_DAT*>(stSnsData.uc_data);
    memset(&bufTmp[0], 0x00, sizeof(bufTmp));
    snprintf(reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp),
            "\n [Head]   sync:%3d, Enable:%01d, Head:%5d, PosSts:0x%02x",
            pstHeading->SyncCnt,
            pstHeading->isEnable,
            pstHeading->Heading,
            pstHeading->posSts);
    _pb_strcat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp));

    switch ( uc_get_method ) {
        case VEHICLESENS_GETMETHOD_GPS:
            /* Satellite information */
            VehicleSensGetNaviinfoDiagGPSg(&stGpsData);
            pstDiagGps = reinterpret_cast<NAVIINFO_DIAG_GPS*>(stGpsData.uc_data);
            memset(&bufTmp[0], 0x00, sizeof(bufTmp));
            snprintf(reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp),
                    "\n [Diag]\n  FixSts:0x%02x",
                    pstDiagGps->stFix.ucFixSts);
            _pb_strcat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp));
            for (i = 0; i < 12; i++) {
                memset(&bufTmp[0], 0x00, sizeof(bufTmp));
                snprintf(reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp),
                        "\n  [%02d] RcvSts:0x%02x, prn:0x%02x, elv:0x%02x, Lv:0x%02x, azm:0x%04x",
                        i,
                        pstDiagGps->stSat.stPrn[i].ucRcvSts,
                        pstDiagGps->stSat.stPrn[i].ucPrn,
                        pstDiagGps->stSat.stPrn[i].ucelv,
                        pstDiagGps->stSat.stPrn[i].ucLv,
                        pstDiagGps->stSat.stPrn[i].usAzm);
                _pb_strcat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp));
            }

            /* Time */
            VehicleSensGetGpsTime(&stGpsData, VEHICLESENS_GETMETHOD_GPS);
            pstDateTimeGps = reinterpret_cast<SENSOR_MSG_GPSTIME*>(stGpsData.uc_data);
            memset(&bufTmp[0], 0x00, sizeof(bufTmp));
            snprintf(reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp),
                    "\n [Time] %04d/%02d/%02d %02d:%02d:%02d, sts:%d",
                    pstDateTimeGps->utc.year,
                    pstDateTimeGps->utc.month,
                    pstDateTimeGps->utc.date,
                    pstDateTimeGps->utc.hour,
                    pstDateTimeGps->utc.minute,
                    pstDateTimeGps->utc.second,
                    pstDateTimeGps->tdsts);
            _pb_strcat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp));
            break;
        case VEHICLESENS_GETMETHOD_NAVI:
            /* nop */
            break;
        default:
            /* nop */
            break;
    }
    memcpy(pBuf, &buf[0], sizeof(buf));
    return;
}
// LCOV_EXCL_STOP
