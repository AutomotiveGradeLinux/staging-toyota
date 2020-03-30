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
 *  SensorLog.h
 * @brief
 *   Definition for Sensor Logging I/F Liblary
 */
#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_SENSORLOG_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_SENSORLOG_H_

#include <stdint.h>
#include <vehicle_service/positioning_base_library.h>
#include <vehicle_service/POS_define.h>

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/
/* NAV-SVINFO sizes(chnum=20) */
#define SENSLOG_GPS_SIZE_NAVSVINFO 248

/* Sensor log data type */
typedef enum {
    SENSLOG_DATA_I_UNSPECIFIED = 0,             /* Input:Not specified          */

    SENSLOG_DATA_I_SYS,                         /* Input:Sensor          */
    SENSLOG_DATA_I_SYS_STS,                     /* Input:External terminal status    */
    SENSLOG_DATA_I_GPS,                         /* Input:GPS(_CWORD82_/u-blox) */
    SENSLOG_DATA_I_LOC,                         /* Input:Location information        */
    SENSLOG_DATA_I_SPEED,                       /* Input:Vehicle speed            */
    SENSLOG_DATA_I_TIME,                        /* Input:Request to set GPS time */
    SENSLOG_DATA_I_GPSINF,                      /* Input:GPS information setting request */
    SENSLOG_DATA_I_GPSRST,                      /* Input:GPS reset request */
    SENSLOG_DATA_I_GPSSET,                      /* Input:GPS setting request     */
    SENSLOG_DATA_I_NAVSVINFO,                   /* Input:NAV-SVINFO(u-blox)*/
    SENSLOG_DATA_I_SYS_ABNORMAL,                /* Input:Sensor(When an error occurs)  */
    SENSLOG_DATA_I_COUNT                        /* Classified number               */
} SENSLOG_DATA_I;

typedef enum {
    SENSLOG_DATA_O_UNSPECIFIED = 0,             /* Output:Not specified          */

    SENSLOG_DATA_O_SYS,                         /* Output:Sensor          */
    SENSLOG_DATA_O_SYS_PKG,                     /* Output:Sensor(PKG)     */
    SENSLOG_DATA_O_GPS,                         /* Output:GPS(_CWORD82_/u-blox) */
    SENSLOG_DATA_O_LONLAT_N,                    /* Output:Latitude and longitude(NAVI)  */
    SENSLOG_DATA_O_LONLAT_G,                    /* Output:Latitude and longitude(GPS)   */
    SENSLOG_DATA_O_ALT,                         /* Output:Altitude            */
    SENSLOG_DATA_O_SPEED_N,                     /* Output:Vehicle speed(NAVI)      */
    SENSLOG_DATA_O_SPEED_P,                     /* Output:Vehicle speed(POS)       */
    SENSLOG_DATA_O_HEAD_N,                      /* Output:Orientation(NAVI)      */
    SENSLOG_DATA_O_HEAD_G,                      /* Output:Orientation(GPS)       */
    SENSLOG_DATA_O_TIME_SETREQ,                 /* Output:GPS time delivery request */
    SENSLOG_DATA_O_TIME,                        /* Output:GPS time         */
    SENSLOG_DATA_O_GPSINF,                      /* Output:GPS information         */
    SENSLOG_DATA_O_TIME_CS,                     /* Output:GPS time(For the ClockService) */
    SENSLOG_DATA_O_GPSRST,                      /* Output:GPS reset response */

    SENSLOG_DATA_O_COUNT                        /* Classified number               */
} SENSLOG_DATA_O;

/* Send/Receive result */
#define SENSLOG_RES_SUCCESS 0                  /* Send/Receive Success */
#define SENSLOG_RES_FAIL    1                  /* Send/Receive Abnormality */

/* Destination name(Internal use) */
#define SENSLOG_PNAME_CLOCK      "*Clock"      /* Clock Service */
#define SENSLOG_PNAME_COMMUSB    "*CommUSB"    /* CommUSB       */

#define SENSLOG_ON  1
#define SENSLOG_OFF 0

/*---------------------------------------------------------------------------------*
 * Typedef declaration                                                             *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Struct declaration                                                              *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Prototype Declaration                                                           *
 *---------------------------------------------------------------------------------*/
/* SensorLog public API */
#ifdef __cplusplus
extern "C" {
#endif
    void SensLogInitialize(uint8_t *p_mount_path);
    uint8_t SensLogGetNavSvinfoFlag(void);
    void SensLogWriteInputData(uint16_t us_data_type, DID ul_did, PNO us_pno, uint8_t *p_data, \
        uint16_t us_size, uint8_t uc_result, uint8_t u_write_flag, uint8_t u_write_abnormal_flag);
    void SensLogWriteOutputData(uint16_t us_data_type, DID ul_did, PNO us_pno, uint8_t *p_data, \
        uint16_t us_size, uint8_t uc_result);
    void SensLogTerminate(void);
    void SensLogStore(void);
#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_SENSORLOG_H_
