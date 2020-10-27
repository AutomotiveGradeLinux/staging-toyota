/*
 * @copyright Copyright (c) 2018-2020 TOYOTA MOTOR CORPORATION.
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

#ifndef HAL_API_POSITIONING_HAL_H_
#define HAL_API_POSITIONING_HAL_H_

/**
 * @file positioning_hal.h
 */

/** @addtogroup positioning
 *  @{
 */
/** @addtogroup positioning_hal
 *  @ingroup positioning
 *  @{
 */

/*---------------------------------------------------------------------------*/
// Include files

#include <native_service/frameworkunified_types.h>
#include <vehicle_service/pos_message_header.h>
#include <vehicle_service/std_types.h>

/*---------------------------------------------------------------------------*/
// Value define

#define POSHAL_THREAD_NAME_SENS                         "POS_Sens"
//!< \~english Thread name for receive sensor
#define POSHAL_THREAD_NAME_GPS_MAIN                     "POS_Gps"
//!< \~english Thread name for GPS NMEA analysis and delivery message
#define POSHAL_THREAD_NAME_GPS_RECV                     "POS_Gps_Recv"
//!< \~english Thread name for receive GPS
#define POSHAL_THREAD_NAME_GPS_ROLOVR                   "POS_Gps_Rolovr"
//!< \~english Thread name for monitoring GPS rollover

#define POSHAL_DID_SNS_COUNTER                          0x8000001A
//!< \~english Data ID of sensor counter
#define POSHAL_DID_SPEED_PULSE                          0x80000012
//!< \~english Data ID of speed pulse
#define POSHAL_DID_SPEED_PULSE_FLAG                     0x80000076
//!< \~english Data ID of speed pulse flag
#define POSHAL_DID_SPEED_PULSE_FST                      0x80000028
//!< \~english Data ID of first time speed pulse
#define POSHAL_DID_SPEED_PULSE_FLAG_FST                 0x8000007D
//!< \~english Data ID of first time speed pulse flag
#define POSHAL_DID_SPEED_KMPH                           0x80000013
//!< \~english Data ID of KMPH speed
#define POSHAL_DID_PULSE_TIME                           0x8000003A
//!< \~english Data ID of pulse time
#define POSHAL_DID_GYRO_X                               0x80000014
//!< \~english Data ID of X axis of gyro
#define POSHAL_DID_GYRO_Y                               0x80000085
//!< \~english Data ID of Y axis of gyro
#define POSHAL_DID_GYRO_Z                               0x80000086
//!< \~english Data ID of Z axis ofgyro
#define POSHAL_DID_GYRO                                 POSHAL_DID_GYRO_X
//!< \~english Data ID of gyro
#define POSHAL_DID_GYRO_X_FST                           0x80000029
//!< \~english Data ID of first time X axis gyro
#define POSHAL_DID_GYRO_Y_FST                           0x80000043
//!< \~english Data ID of first time Y axis gyro
#define POSHAL_DID_GYRO_Z_FST                           0x80000023
//!< \~english Data ID of first time Z axis gyro
#define POSHAL_DID_GYRO_FST                             POSHAL_DID_GYRO_X_FST
//!< \~english Data ID of first time gyro
#define POSHAL_DID_GYRO_EXT                             0x80000027
//!< \~english Data ID of extend gyro
#define POSHAL_DID_GYRO_TEMP                            0x80000090
//!< \~english Data ID of gyro temperature
#define POSHAL_DID_GYRO_TEMP_FST                        0x80000091
//!< \~english Data ID of first time gyro temperature
#define POSHAL_DID_REV                                  0x80000017
//!< \~english Data ID of reverse signal
#define POSHAL_DID_REV_FST                              0x8000007E
//!< \~english Data ID of first time reverse signal
#define POSHAL_DID_GSNS_X                               0x80000015
//!< \~english Data ID of x axis gsensor
#define POSHAL_DID_GSNS_Y                               0x80000016
//!< \~english Data ID of Y axis gsensor
#define POSHAL_DID_GSNS_Z                               0x80000026
//!< \~english Data ID of Z axis gsensor
#define POSHAL_DID_GSNS_X_FST                           0x80000087
//!< \~english Data ID of first time x axis gsensor
#define POSHAL_DID_GSNS_Y_FST                           0x80000088
//!< \~english Data ID of first time Y axis gsensor
#define POSHAL_DID_GSNS_Z_FST                           0x80000089
//!< \~english Data ID of first time Z axis gsensor
#define POSHAL_DID_GPS_NMEA                             0x8000009AU
//!< \~english Data ID of GPS NMEA sentence
#define POSHAL_DID_GPS_ANTENNA                          0x80000019
//!< \~english Data ID of GPS antenna status
#define POSHAL_DID_GPS_CUSTOMDATA                       0x80000094U
//!< \~english Data ID of GPS custom data
#define POSHAL_DID_GPS_CUSTOMDATA_NAVI                  0x800000A0
//!< \~english Data ID of GPS custom data for navi
#define POSHAL_DID_GPS_TIME                             0x80000098
//!< \~english Data ID of GPS time
#define POSHAL_DID_GPS_TIME_RAW                         0x800000B1U
//!< \~english Data ID of GPS raw time
#define POSHAL_DID_GPS_VERSION                          0x8000001E
//!< \~english Data ID of GPS version
#define POSHAL_DID_GPS__CWORD82___CWORD44_GP4                     0x80000031U
//!< \~english Data ID of _CWORD82_ GPS _CWORD44_GP4 data
#define POSHAL_DID_GPS__CWORD82__FULLBINARY                   0x80000032U
//!< \~english Data ID of _CWORD82_ GPS full binary data
#define POSHAL_DID_GPS_WKNROLLOVER                      0x800000B2U
//!< \~english Data ID of GPS week counter
#define POSHAL_DID_GPS_CLOCK_DRIFT                      0x800000B3U
//!< \~english Data ID of GPS time drift data
#define POSHAL_DID_GPS_CLOCK_FREQ                       0x800000B4U
//!< \~english Data ID of GPS time frequency data
#define POSHAL_DID_GPS_INTERRUPT_FLAG                   0x80000077
//!< \~english Data ID of GPS interrupt flag

#define CONFIG_SENSOR_EXT_VALID 1  //!< \~english for first sensor

#define CID_LINESENS_VEHICLE_DATA 0x0300  //!< \~english vehicle notify ID

/**
 * \~english @brief LineSensor Vehicle Signal Notification
 * \~english @brief This command is sent when LineSensor vehicle signals are notified when sensor data is received.\n
 * \~english @brief Message related structure @ref LsdrvMsgLsdataG\n
 * \~english @brief Message buffer header structure @ref T_APIMSG_MSGBUF_HEADER\n
 * \~english @brief Message header structure @ref T_APIMSG_HEADER\n
 * \~english @brief Message body structure   @ref LsdrvMsgLsdataDatG\n
 * \~english @brief Message body (vehicle signal notification message information) structure @ref LsdrvLsdataG\n
 */
#define CID_LINESENS_VEHICLE_DATA_G   0x0303

/**
 * \~english @brief Gyro Failure Status Notification
 * \~english @brief This command is sent when giro failure status is notified.\n
 * \~english @brief Message related structure @ref LsdrvMsgLsdataGyroTrouble\n
 * \~english @brief Message buffer header structure @ref T_APIMSG_MSGBUF_HEADER\n
 * \~english @brief Message header structure @ref T_APIMSG_HEADER\n
 * \~english @brief Message body structure   @ref LsdrvMsgLsdataDatGyroTrouble\n
 */
#define CID_LINESENS_VEHICLE_DATA_GYRO_TROUBLE  0x0304

/**
 * \~english @brief Get SYS GPS Interrupt Signals
 * \~english @brief This command is sent when SYS GPS interrupt signal is acquired.\n
 * \~english @brief Message related structure @ref LsdrvMsgLsdataGpsInterruptSignal\n
 * \~english @brief Message buffer header structure @ref T_APIMSG_MSGBUF_HEADER\n
 * \~english @brief Message header structure @ref T_APIMSG_HEADER\n
 * \~english @brief Message body structure   @ref LsdrvMsgLsdataDatGpsInterruptSignal\n
 */
#define CID_LINESENS_VEHICLE_DATA_SYS_GPS_INTERRUPT_SIGNAL  0x0305


/**
 * \~english @brief Gyro Connection Status Notification
 * \~english @brief This command is sent when giro connection status is notified.\n
 * \~english @brief Message related structure @ref LsdrvMsgLsdataGyroConnectStatus\n
 * \~english @brief Message buffer header structure @ref T_APIMSG_MSGBUF_HEADER\n
 * \~english @brief Message header structure  @ref T_APIMSG_HEADER\n
 * \~english @brief Message body structure    @ref LsdrvMsgLsdataDatGyroConnectStatus\n
 */
#define CID_LINESENS_VEHICLE_DATA_GYRO_CONNECT_STATUS       0x0306


/**
 * \~english @brief GPS Antenna Connection Status Notification
 * \~english @brief This command is sent when the GPS antenna connection status is notified.\n
 * \~english @brief Message related structure @ref LsdrvMsgLsdataGpsAntennaStatus\n
 * \~english @brief Message buffer header structure @ref T_APIMSG_MSGBUF_HEADER\n
 * \~english @brief Message header structure @ref T_APIMSG_HEADER\n
 * \~english @brief Message body structure   @ref LsdrvMsgLsdataDatGpsAntennaStatus\n
 */
#define CID_LINESENS_VEHICLE_DATA_GPS_ANTENNA_STATUS        0x0307


/**
 * \~english @brief LineSensor Vehicle Signal Notification(Initial Sensor)
 * \~english @brief This command is sent when LineSensor vehicle signals (initial sensor) are notified.\n
 * \~english @brief Message related structure @ref LsdrvMsgLsdataFst\n
 * \~english @brief Message buffer header structure @ref T_APIMSG_MSGBUF_HEADER\n
 * \~english @brief Message header structure @ref T_APIMSG_HEADER\n
 * \~english @brief Message body structure    @ref LsdrvMsgLsdataDatFst\n
 * \~english @brief Gyro X-axis  @ref LsdrvLsdataFstGyroX\n
 * \~english @brief Gyro Y-axis  @ref LsdrvLsdataFstGyroY\n
 * \~english @brief Gyro Z-axis  @ref LsdrvLsdataFstGyroZ\n
 * \~english @brief Velocity information      @ref LsdrvLsdataFstSpeed\n
 * \~english @brief Vehicle speed pulse    @ref LsdrvLsdataFstSpeedPulseFlag\n
 * \~english @brief REV flag     @ref LsdrvLsdataFstRev\n
 * \~english @brief Gyro Temperature  @ref LsdrvLsdataFstGyroTemp\n
 * \~english @brief G-sensor X-axis   @ref LsdrvLsdataFstGsensorX\n
 * \~english @brief G-sensor Y-axis   @ref LsdrvLsdataFstGsensorY\n
 * \~english @brief G-sensor Z-axis   @ref LsdrvLsdataFstGsensorZ\n
 */
#define CID_LINESENS_VEHICLE_DATA_FST                       0x0302

/**
 * \~english @brief GPS data notification
 * \~english @brief This command is used to notify the data input from the GPS receiver device.\n
 * \~english @brief Message related structure @ref SensorMsgGpsdata\n
 * \~english @brief Message buffer header structure @ref T_APIMSG_MSGBUF_HEADER\n
 * \~english @brief Message header structure @ref T_APIMSG_HEADER\n
 * \~english @brief Message body structure   @ref SensorMsgGpsdataDat\n
 */
#define CID_GPS_DATA                                        0x0301

/**
 * \~english @brief Notification of effective ephemeris number at shutdown
 * \~english @brief This command is sent to notify the effective ephemeris number at shutdown.\n
 * \~english @brief Message related structure @ref SensorMsgGpsdata\n
 * \~english @brief Message buffer header structure @ref T_APIMSG_MSGBUF_HEADER\n
 * \~english @brief Message body structure  @ref SensorMsgGpsdata\n
 */
#define CID_EPH_NUM_NOTIFICATION                            0x0310


#define LSDRV_FSTSNS_SENSOR_FIRST_SAVE_NUM                  50
//!< \~english number of save first sensor

#define LSDRV_FSTSNS_SENSOR_FIRST_SEND_NUM                  30
//!< \~english number of send first sensor

#define LSDRV_FSTSNS_DETAILED_DATA_GYRO_NUM                 10
//!< \~english number of gyro including detail data

#define LSDRV_FSTSNS_DETAILED_DATA_GSENSOR_NUM              10
//!< \~english number of Gsns includeing detail data

#define LSDRV_FSTSNS_DSIZE_GYRO_X  (LSDRV_FSTSNS_SENSOR_FIRST_SEND_NUM * LSDRV_FSTSNS_DETAILED_DATA_GYRO_NUM * 2)
//!< \~english data size of X axis of gyro
#define LSDRV_FSTSNS_DSIZE_GYRO_Y  (LSDRV_FSTSNS_SENSOR_FIRST_SEND_NUM * LSDRV_FSTSNS_DETAILED_DATA_GYRO_NUM * 2)
//!< \~english data size of Y axis of gyro
#define LSDRV_FSTSNS_DSIZE_GYRO_Z  (LSDRV_FSTSNS_SENSOR_FIRST_SEND_NUM * LSDRV_FSTSNS_DETAILED_DATA_GYRO_NUM * 2)
//!< \~english data size of Z axis of gyro
#define LSDRV_FSTSNS_DSIZE_SPEED          (LSDRV_FSTSNS_SENSOR_FIRST_SEND_NUM * 2)
//!< \~english data size of speed
#define LSDRV_FSTSNS_DSIZE_SPEED_FLG      LSDRV_FSTSNS_SENSOR_FIRST_SEND_NUM
//!< \~english data size of speed pulse
#define LSDRV_FSTSNS_DSIZE_REV            LSDRV_FSTSNS_SENSOR_FIRST_SEND_NUM
//!< \~english data size of reverse
#define LSDRV_FSTSNS_DSIZE_GYRO_TEMP      (LSDRV_FSTSNS_SENSOR_FIRST_SEND_NUM * 2)
//!< \~english data size of gyro temperature
#define LSDRV_FSTSNS_DSIZE_GSENSOR_X  (LSDRV_FSTSNS_SENSOR_FIRST_SEND_NUM * LSDRV_FSTSNS_DETAILED_DATA_GSENSOR_NUM * 2)
//!< \~english data size of X axis of Gsns

#define LSDRV_FSTSNS_DSIZE_GSENSOR_Y  (LSDRV_FSTSNS_SENSOR_FIRST_SEND_NUM * LSDRV_FSTSNS_DETAILED_DATA_GSENSOR_NUM * 2)
  //!< \~english data size of Y axis of Gsns
#define LSDRV_FSTSNS_DSIZE_GSENSOR_Z  (LSDRV_FSTSNS_SENSOR_FIRST_SEND_NUM * LSDRV_FSTSNS_DETAILED_DATA_GSENSOR_NUM * 2)
//!< \~english data size of Z axis of Gsns

#define SENSOR_MSG_VSINFO_DSIZE  1904  //!< \~english message body max size


// Thread control Command ID
#define CID_THREAD_CREATE_COMP   (0x0001) //!< \~english Thread Start Complete Notify Command ID
#define CID_THREAD_STOP_REQ      (0x0002) //!< \~english Thread Stop Request Command ID
#define CID_THREAD_STOP_COMP     (0x0003) //!< \~english Thread Stop Complete Notify Command ID

// A vehicle signal of data size
#define LSDRV_SNDMSG_DTSIZE_1     1       //!< \~english SndMSG data size 1Byte
#define LSDRV_SNDMSG_DTSIZE_2     2       //!< \~english SndMSG data size 2Byte
#define LSDRV_SNDMSG_DTSIZE_20    20      //!< \~english SndMSG data size 20Byte
#define LSDRV_SNDMSG_DTSIZE_132   132     //!< \~english SndMSG data size 132Byte

// NMEA receive flag
#define POS_SNS_GPS_NMEA_GGA      (0x00000001U)  //!< \~english GGA sentence
#define POS_SNS_GPS_NMEA_GLL      (0x00000002U)  //!< \~english GLL sentence
#define POS_SNS_GPS_NMEA_GSA1     (0x00000004U)  //!< \~english GSA1 sentence
#define POS_SNS_GPS_NMEA_GSA2     (0x00000008U)  //!< \~english GSA2 sentence
#define POS_SNS_GPS_NMEA_GSA3     (0x00000010U)  //!< \~english GSA3 sentence
#define POS_SNS_GPS_NMEA_GSA4     (0x00000020U)  //!< \~english GSA4 sentence
#define POS_SNS_GPS_NMEA_GST      (0x00000040U)  //!< \~english GST sentence
#define POS_SNS_GPS_NMEA_RMC      (0x00000080U)  //!< \~english RMC sentence
#define POS_SNS_GPS_NMEA_VTG      (0x00000100U)  //!< \~english VTG sentence
#define POS_SNS_GPS_NMEA_GSV1     (0x00000200U)  //!< \~english GSV1 sentence
#define POS_SNS_GPS_NMEA_GSV2     (0x00000400U)  //!< \~english GSV2 sentence
#define POS_SNS_GPS_NMEA_GSV3     (0x00000800U)  //!< \~english GSV3 sentence
#define POS_SNS_GPS_NMEA_GSV4     (0x00001000U)  //!< \~english GSV4 sentence
#define POS_SNS_GPS_NMEA_GSV5     (0x00002000U)  //!< \~english GSV5 sentence
#define POS_SNS_GPS_NMEA_GSV6     (0x00004000U)  //!< \~english GSV6 sentence
#define POS_SNS_GPS_NMEA_GSV7     (0x00008000U)  //!< \~english GSV7 sentence
#define POS_SNS_GPS_NMEA_GSV8     (0x00010000U)  //!< \~english GSV8 sentence
#define POS_SNS_GPS_NMEA_GSV9     (0x00020000U)  //!< \~english GSV9 sentence
#define POS_SNS_GPS_NMEA_GSV10    (0x00040000U)  //!< \~english GSV10 sentence
#define POS_SNS_GPS_NMEA_GSV11    (0x00080000U)  //!< \~english GSV11 sentence
#define POS_SNS_GPS_NMEA_GSV12    (0x00100000U)  //!< \~english GSV12 sentence
#define POS_SNS_GPS_NMEA_GSV13    (0x00200000U)  //!< \~english GSV13 sentence
#define POS_SNS_GPS_NMEA_RESERVE1 (0x00400000U)  //!< \~english Reserved
#define POS_SNS_GPS_NMEA_RESERVE2 (0x00800000U)  //!< \~english Reserved


/**
 * @enum POS_SNS_GPS_NMEA_SNO
 * \~english type of NMEA
 */
typedef enum {
  POS_SNS_GPS_NMEA_SNO_GGA = 0,   //!< \~english GGA sentence
  POS_SNS_GPS_NMEA_SNO_GLL,       //!< \~english GLL sentence
  POS_SNS_GPS_NMEA_SNO_GSA1,      //!< \~english GSA1 sentence
  POS_SNS_GPS_NMEA_SNO_GSA2,      //!< \~english GSA2 sentence
  POS_SNS_GPS_NMEA_SNO_GSA3,      //!< \~english GSA3 sentence
  POS_SNS_GPS_NMEA_SNO_GSA4,      //!< \~english GSA4 sentence
  POS_SNS_GPS_NMEA_SNO_GST,       //!< \~english GST sentence
  POS_SNS_GPS_NMEA_SNO_RMC,       //!< \~english RMC sentence
  POS_SNS_GPS_NMEA_SNO_VTG,       //!< \~english VTG sentence
  POS_SNS_GPS_NMEA_SNO_GSV1,      //!< \~english GSV1 sentence
  POS_SNS_GPS_NMEA_SNO_GSV2,      //!< \~english GSV2 sentence
  POS_SNS_GPS_NMEA_SNO_GSV3,      //!< \~english GSV3 sentence
  POS_SNS_GPS_NMEA_SNO_GSV4,      //!< \~english GSV4 sentence
  POS_SNS_GPS_NMEA_SNO_GSV5,      //!< \~english GSV5 sentence
  POS_SNS_GPS_NMEA_SNO_GSV6,      //!< \~english GSV6 sentence
  POS_SNS_GPS_NMEA_SNO_GSV7,      //!< \~english GSV7 sentence
  POS_SNS_GPS_NMEA_SNO_GSV8,      //!< \~english GSV8 sentence
  POS_SNS_GPS_NMEA_SNO_GSV9,      //!< \~english GSV9 sentence
  POS_SNS_GPS_NMEA_SNO_GSV10,     //!< \~english GSV10 sentence
  POS_SNS_GPS_NMEA_SNO_GSV11,     //!< \~english GSV11 sentence
  POS_SNS_GPS_NMEA_SNO_GSV12,     //!< \~english GSV12 sentence
  POS_SNS_GPS_NMEA_SNO_GSV13,     //!< \~english GSV13 sentence
  POS_SNS_GPS_NMEA_SNO_RESERVE1,  //!< \~english Reserved
  POS_SNS_GPS_NMEA_SNO_RESERVE2,  //!< \~english Reserved

  POS_SNS_GPS_NMEA_SNO_MAX        //!< \~english MAX sentence(invalid)
} POS_SNS_GPS_NMEA_SNO;

/**
 * @enum LsDrvKind
 * \~english type of Sensor
 */
typedef enum {
  LSDRV_GYRO_X,             //!< \~english Gyro X-Axis
  LSDRV_GYRO_Y,             //!< \~english Gyro Y-Axis
  LSDRV_GYRO_Z,             //!< \~english Gyro Z-Axis
  LSDRV_SPEED_PULSE,        //!< \~english Speed pulse
  LSDRV_SPEED_PULSE_FLAG,   //!< \~english Speed pulse Flag
  LSDRV_SPEED_KMPH,         //!< \~english Speed km/h
  LSDRV_GYRO_EXT,           //!< \~english extend Gyro
  LSDRV_REV,                //!< \~english Reverse
  LSDRV_GYRO_TEMP,          //!< \~english Gyro temperature
  LSDRV_GSENSOR_X,          //!< \~english G-sensor X-Axis
  LSDRV_GSENSOR_Y,          //!< \~english G-sensor Y-Axis
  LSDRV_GSENSOR_Z,          //!< \~english G-sensor Z-Axis
  LSDRV_PULSE_TIME,         //!< \~english Pulse time
  LSDRV_SNS_COUNTER,        //!< \~english Sensor counter
  LSDRV_GPS_INTERRUPT_FLAG, //!< \~english GPS Interrupt Flag
  LSDRV_KINDS_MAX           //!< \~english MAX
} LsDrvKind;


/**
 * @enum EnumTidPos
 * \~english  Thread id notified to positioning when starting or terminating each thread is completed.
 */
typedef enum EnumTidPos {
  ETID_POS_MAIN = 0,
  //!< \~english Thread for vehicle sensor(unused in hal)
  ETID_POS_SENS,
  //!< \~english Thread for receive sensor
  ETID_POS_GPS,
  //!< \~english Thread for GPS NMEA analysis and delivery message
  ETID_POS_GPS_RECV,
  //!< \~english Thread for receive GPS
  ETID_POS_GPS_ROLLOVER,
  //!< \~english Thread for monitoring GPS rollover
  ETID_POS_MAX
  //!< \~english Max thread id
} EnumTID_POS;

/**
 * @struct LsdrvLsdata
 * \~english  Struct of LineSensor message
 */
typedef struct LsdrvLsdata {
  u_int32           ul_did;                                   //!< \~english data ID
  u_int8            uc_size;                                  //!< \~english data size
  u_int8            uc_rcvflag;                               //!< \~english receive flag
  u_int8            uc_sns_cnt;                               //!< \~english sensor count
  u_int8            u_reserve;                                //!< \~english reserve
  u_int8            uc_data[132];                             //!< \~english data contents
} LSDRV_LSDATA;

/**
 * @struct LsdrvLsdataG
 * \~english  Struct of LineSensorG message
 */
typedef struct LsdrvLsdataG {
  u_int32     ul_did;           //!< \~english data ID
  u_int8      uc_size;          //!< \~english data size
  u_int8      uc_rcv_flag;      //!< \~english receive flag
  u_int8      uc_sns_cnt;       //!< \~english sensor count
  u_int8      reserve;          //!< \~english reserve
  u_int8      uc_data[132];     //!< \~english data content
} LSDRV_LSDATA_G;

/**
 * @struct LsdrvLsdataFstGyroX
 * \~english  Struct of X axis of of gyro data(first sensor message)
 */
typedef struct LsdrvLsdataFstGyroX {
  u_int32              ul_did;                               //!< \~english data ID
  u_int16              uc_size;                              //!< \~english data size
  u_int8               uc_partition_max;                     //!< \~english total of partition
  u_int8               uc_partition_num;                     //!< \~english data number
  u_int8               uc_data[LSDRV_FSTSNS_DSIZE_GYRO_X];   //!< \~english data contents
} LSDRV_LSDATA_FST_GYRO_X;

/**
 * @struct LsdrvLsdataFstGyroY
 * \~english  Struct of Y axis of of gyro data(first sensor message)
 */
typedef struct LsdrvLsdataFstGyroY {
  u_int32              ul_did;                               //!< \~english data ID
  u_int16              uc_size;                              //!< \~english data size
  u_int8               uc_partition_max;                     //!< \~english total of partition
  u_int8               uc_partition_num;                     //!< \~english data number
  u_int8               uc_data[LSDRV_FSTSNS_DSIZE_GYRO_Y];   //!< \~english data contents
} LSDRV_LSDATA_FST_GYRO_Y;

/**
 * @struct LsdrvLsdataFstGyroZ
 * \~english  Struct of Z axis of of gyro data(first sensor message)
 */
typedef struct LsdrvLsdataFstGyroZ {
  u_int32              ul_did;                               //!< \~english data ID
  u_int16              uc_size;                              //!< \~english data size
  u_int8               uc_partition_max;                     //!< \~english total of partition
  u_int8               uc_partition_num;                     //!< \~english data number
  u_int8               uc_data[LSDRV_FSTSNS_DSIZE_GYRO_Z];   //!< \~english data contents
} LSDRV_LSDATA_FST_GYRO_Z;

/**
 * @struct LsdrvLsdataFstSpeed
 * \~english  Struct of speed data(first sensor message)
 */
typedef struct LsdrvLsdataFstSpeed {
  u_int32  ul_did;                               //!< \~english data ID
  u_int16  uc_size;                              //!< \~english data size
  u_int8   uc_partition_max;                      //!< \~english total of partition
  u_int8   uc_partition_num;                      //!< \~english data number
  u_int8   uc_data[LSDRV_FSTSNS_DSIZE_SPEED];     //!< \~english data contents
} LSDRV_LSDATA_FST_SPEED;

/**
 * @struct LsdrvLsdataFstSpeedPulseFlag
 * \~english  Struct of speed pulse data(first sensor message)
 */
typedef struct LsdrvLsdataFstSpeedPulseFlag {
  u_int32  ul_did;                                 //!< \~english data ID
  u_int16  uc_size;                                //!< \~english data size
  u_int8   uc_partition_max;                       //!< \~english total of partition
  u_int8   uc_partition_num;                       //!< \~english data number
  u_int8   uc_data[LSDRV_FSTSNS_DSIZE_SPEED_FLG];  //!< \~english data contents
} LSDRV_LSDATA_FST_SPEED_PULSE_FLAG;

/**
 * @struct LsdrvLsdataFstRev
 * \~english  Struct of reverse data(first sensor message)
 */
typedef struct LsdrvLsdataFstRev {
  u_int32  ul_did;                             //!< \~english data ID
  u_int16  uc_size;                            //!< \~english data size
  u_int8   uc_partition_max;                   //!< \~english total of partition
  u_int8   uc_partition_num;                   //!< \~english data number
  u_int8   uc_data[LSDRV_FSTSNS_DSIZE_REV];    //!< \~english data contents
} LSDRV_LSDATA_FST_REV;

/**
 * @struct LsdrvLsdataFstGyroTemp
 * \~english  Struct of gyro temperature data(first sensor message)
 */
typedef struct LsdrvLsdataFstGyroTemp {
  u_int32  ul_did;                                 //!< \~english data ID
  u_int16  uc_size;                                //!< \~english data size
  u_int8   uc_partition_max;                       //!< \~english total of partition
  u_int8   uc_partition_num;                       //!< \~english data number
  u_int8   uc_data[LSDRV_FSTSNS_DSIZE_GYRO_TEMP];  //!< \~english data contents
} LSDRV_LSDATA_FST_GYRO_TEMP;

/**
 * @struct LsdrvLsdataFstGsensorX
 * \~english  Struct of X axis of Gsns data(first sensor message)
 */
typedef struct LsdrvLsdataFstGsensorX {
  u_int32  ul_did;                                 //!< \~english data ID
  u_int16  uc_size;                                //!< \~english data size
  u_int8   uc_partition_max;                       //!< \~english total of partition
  u_int8   uc_partition_num;                       //!< \~english data number
  u_int8   uc_data[LSDRV_FSTSNS_DSIZE_GSENSOR_X];  //!< \~english data contents
} LSDRV_LSDATA_FST_GSENSOR_X;

/**
 * @struct LsdrvLsdataFstGsensorY
 * \~english  Struct of Y axis of Gsns data(first sensor message)
 */
typedef struct LsdrvLsdataFstGsensorY {
  u_int32  ul_did;                                 //!< \~english data ID
  u_int16  uc_size;                                //!< \~english data size
  u_int8   uc_partition_max;                       //!< \~english total of partition
  u_int8   uc_partition_num;                       //!< \~english data number
  u_int8   uc_data[LSDRV_FSTSNS_DSIZE_GSENSOR_Y];  //!< \~english data contents
} LSDRV_LSDATA_FST_GSENSOR_Y;

/**
 * @struct LsdrvLsdataFstGsensorZ
 * \~english  Struct of Z axis of Gsns data(first sensor message)
 */
typedef struct LsdrvLsdataFstGsensorZ {
  u_int32  ul_did;                                 //!< \~english data ID
  u_int16  uc_size;                                //!< \~english data size
  u_int8   uc_partition_max;                       //!< \~english total of partition
  u_int8   uc_partition_num;                       //!< \~english data number
  u_int8   uc_data[LSDRV_FSTSNS_DSIZE_GSENSOR_Z];  //!< \~english data contents
} LSDRV_LSDATA_FST_GSENSOR_Z;

/**
 * @struct LsdrvMsgLsdataDatFst
 * \~english  Struct of LineSensor data (first sensor message)
 */
typedef struct LsdrvMsgLsdataDatFst {
  LSDRV_LSDATA_FST_GYRO_X            st_gyro_x;         //!< \~english data of X axis of gyro
  LSDRV_LSDATA_FST_GYRO_Y            st_gyro_y;         //!< \~english data of Y axis of gyro
  LSDRV_LSDATA_FST_GYRO_Z            st_gyro_z;         //!< \~english data of Z axis of gyro
  LSDRV_LSDATA_FST_SPEED             st_speed;          //!< \~english data of speed
  LSDRV_LSDATA_FST_SPEED_PULSE_FLAG  st_spd_pulse_flg;  //!< \~english data of speed pulse
  LSDRV_LSDATA_FST_REV               st_rev;            //!< \~english data of reverse
  LSDRV_LSDATA_FST_GYRO_TEMP         st_gyro_temp;      //!< \~english data of gyro temperature
  LSDRV_LSDATA_FST_GSENSOR_X         st_gsns_x;         //!< \~english data of X axis of Gsns
  LSDRV_LSDATA_FST_GSENSOR_Y         st_gsns_y;         //!< \~english data of Y axis of Gsns
  LSDRV_LSDATA_FST_GSENSOR_Z         st_gsns_z;         //!< \~english data of Z axis of Gsns
} LSDRV_MSG_LSDATA_DAT_FST;

/**
 * @struct LsdrvMsgLsdataDat
 * \~english  Struct of LineSensor package data
 */
typedef struct LsdrvMsgLsdataDat {
  u_int8        uc_data_num;    //!< \~english data number
  u_int8        reserve[3];     //!< \~english reserve
  LSDRV_LSDATA  st_data[12];    //!< \~english data
} LSDRV_MSG_LSDATA_DAT;

/**
 * @struct LsdrvMsgLsdata
 * \~english  Struct of LineSensor message data
 */
typedef struct LsdrvMsgLsdata {
  T_APIMSG_MSGBUF_HEADER  st_head;    //!< \~english message header
  LSDRV_MSG_LSDATA_DAT    st_para;    //!< \~english data of gyro
} LSDRV_MSG_LSDATA;

/**
 * @struct LsdrvMsgLsdataDatG
 * \~english  Struct of LineSensorG package data
 */
typedef struct LsdrvMsgLsdataDatG {
  u_int8          uc_data_num;      //!< \~english data number
  u_int8          reserve[3];       //!< \~english reserve
  LSDRV_LSDATA_G  st_data[15];      //!< \~english data
} LSDRV_MSG_LSDATA_DAT_G;

/**
 * @struct LsdrvMsgLsdataG
 * \~english  Struct of LineSensorG message data
 */
typedef struct LsdrvMsgLsdataG {
  T_APIMSG_MSGBUF_HEADER  st_head;    //!< \~english message header
  LSDRV_MSG_LSDATA_DAT_G  st_para;    //!< \~english data of message
} LSDRV_MSG_LSDATA_G;

/**
 * @struct LsdrvMsgLsdataDatGyroTrouble
 * \~english  Struct of Gyro trouble
 */
typedef struct LsdrvMsgLsdataDatGyroTrouble {
  u_int32     ul_did;       //!< \~english data ID
  u_int8      uc_size;      //!< \~english data size
  u_int8      reserve;      //!< \~english reserve
  u_int8      uc_data;      //!< \~english data content
  u_int8      reserve2;     //!< \~english reserve
} LSDRV_MSG_LSDATA_DAT_GYRO_TROUBLE;

/**
 * @struct LsdrvMsgLsdataGyroTrouble
 * \~english  Struct of message about Gyro trouble
 */
typedef struct LsdrvMsgLsdataGyroTrouble {
    T_APIMSG_MSGBUF_HEADER            st_head;   //!< \~english message header
    LSDRV_MSG_LSDATA_DAT_GYRO_TROUBLE st_para;   //!< \~english data of message
} LSDRV_MSG_LSDATA_GYRO_TROUBLE;

/**
 * @struct LsdrvMsgLsdataDatGpsInterruptSignal
 * \~english  Struct of Gps interrupt signal data
 */
typedef struct LsdrvMsgLsdataDatGpsInterruptSignal {
  u_int32     ul_did;         //!< \~english data ID
  u_int8      uc_size;        //!< \~english data size
  u_int8      reserve;        //!< \~english reserve
  u_int8      uc_data;        //!< \~english data content
  u_int8      reserve2;       //!< \~english reserve
} LSDRV_MSG_LSDATA_DAT_GPS_INTERRUPT_SIGNAL;

/**
 * @struct LsdrvMsgLsdataGpsInterruptSignal
 * \~english  Struct of message about Gps interrupt signal
 */
typedef struct LsdrvMsgLsdataGpsInterruptSignal {
  T_APIMSG_MSGBUF_HEADER                    st_head;  //!< \~english message header
  LSDRV_MSG_LSDATA_DAT_GPS_INTERRUPT_SIGNAL st_para;  //!< \~english data of message
} LSDRV_MSG_LSDATA_GPS_INTERRUPT_SIGNAL;

/**
 * @struct LsdrvMsgLsdataDatGyroConnectStatus
 * \~english  Struct of Gyro connect status
 */
typedef struct LsdrvMsgLsdataDatGyroConnectStatus {
    u_int32     ul_did;       //!< \~english data ID
    u_int8      uc_size;      //!< \~english data size
    u_int8      reserve;      //!< \~english reserve
    u_int8      uc_data;      //!< \~english data content
    u_int8      reserve2;     //!< \~english reserve
} LSDRV_MSG_LSDATA_DAT_GYRO_CONNECT_STATUS;

/**
 * @struct LsdrvMsgLsdataGyroConnectStatus
 * \~english  Struct of message about Gyro connect status
 */
typedef struct LsdrvMsgLsdataGyroConnectStatus {
    T_APIMSG_MSGBUF_HEADER                    st_head;  //!< \~english message header
    LSDRV_MSG_LSDATA_DAT_GYRO_CONNECT_STATUS  st_para;  //!< \~english data of message
} LSDRV_MSG_LSDATA_GYRO_CONNECT_STATUS;

/**
 * @struct LsdrvMsgLsdataDatGpsAntennaStatus
 * \~english  Struct of Gps Antenna Status
 */
typedef struct LsdrvMsgLsdataDatGpsAntennaStatus {
  u_int32     ul_did;        //!< \~english data ID
  u_int8      uc_size;       //!< \~english data size
  u_int8      uc_rcv_flag;   //!< \~english receive flag
  u_int8      uc_sns_cnt;    //!< \~english sensor count
  u_int8      uc_data;       //!< \~english data content
} LSDRV_MSG_LSDATA_DAT_GPS_ANTENNA_STATUS;

/**
 * @struct LsdrvMsgLsdataGpsAntennaStatus
 * \~english  Struct of message about Gps Antenna Status
 */
typedef struct LsdrvMsgLsdataGpsAntennaStatus {
  T_APIMSG_MSGBUF_HEADER                  st_head;  //!< \~english message header
  LSDRV_MSG_LSDATA_DAT_GPS_ANTENNA_STATUS st_para;  //!< \~english data of message
} LSDRV_MSG_LSDATA_GPS_ANTENNA_STATUS;

/**
 * @struct LsdrvLsdataFst
 * \~english  Struct of sensor data (first sensor message)
 */
typedef struct LsdrvLsdataFst {
  u_int32     ul_did;             //!< \~english data ID
  u_int16     uc_size;            //!< \~english data size
  u_int8      uc_partition_max;   //!< \~english total of partition
  u_int8      uc_partition_num;   //!< \~english data number
  u_int8      uc_data[60];        //!< \~english data contents
} LSDRV_LSDATA_FST;

/**
 * @struct LsdrvMsgLsdataFst
 * \~english  Struct of message about sensor data (first sensor message)
 */
typedef struct LsdrvMsgLsdataFst {
  T_APIMSG_MSGBUF_HEADER    st_head;  //!< \~english message header
  LSDRV_MSG_LSDATA_DAT_FST  st_para;  //!< \~english data of message
} LSDRV_MSG_LSDATA_FST;

/**
 * @struct SensorMsgGpsdataDat
 * \~english  Struct of Gps data
 * \~english  @par Detail
 *       This data structure can have 7 types of data that identified by data ID.\n
 *       The contents are as follows.\n
 *       | data ID                           | content                                   | data format                   |
 *       |:----------------------------------|:------------------------------------------|:------------------------------|
 *       | @ref POSHAL_DID_GPS_NMEA          | NMEA data                                 | @ref MdevGpsNmea "MDEV_GPS_NMEA" |
 *       | @ref POSHAL_DID_GPS_CUSTOMDATA    | GPS custom data                           | @ref MdevGpsCustomDataMgs "MDEV_GPS_CUSTOMDATA" |
 *       | @ref POSHAL_DID_GPS_CLOCK_DRIFT   | clock drift                               | @ref int32_t                  |
 *       | @ref POSHAL_DID_GPS_CLOCK_FREQ    | clock frequency                           | @ref int32_t                  |
 *       | @ref POSHAL_DID_GPS_WKNROLLOVER   | GPS week number                           | @ref SensorWknRollOverHal     |
 *       | @ref POSHAL_DID_GPS_CONNECT_ERROR | GPS signal error(TRUE:Error, FALSE:Normal)| @ref BOOL                     |
 *       | @ref POSHAL_DID_GPS_TIME (_RAW)   | GPS time information                      | @ref SENSOR_GPSTIME           |
 */
typedef struct SensorMsgGpsdataDat {
  u_int32     ul_did;           //!< \~english data ID
  u_int16     us_size;          //!< \~english data size
  u_int8      uc_rcv_flag;      //!< \~english receive flag
  u_int8      uc_sns_cnt;       //!< \~english sensor count
  u_int8      uc_gps_cnt_flag;  //!< \~english GPS count
  u_int8      reserve[3];       //!< \~english reserve
  u_int8      uc_data[SENSOR_MSG_VSINFO_DSIZE];   //!< \~english data contents
} SENSOR_MSG_GPSDATA_DAT;

/**
 * @struct SensorMsgGpsdata
 * \~english  Struct of message about GPS data
 */
typedef struct SensorMsgGpsdata {
  T_APIMSG_MSGBUF_HEADER  st_head;
    //!< \~english message header(see vs-positioning-base-library)
  SENSOR_MSG_GPSDATA_DAT  st_para;
    //!< \~english data of message
} SENSOR_MSG_GPSDATA;

/**
 * @struct TG_GPS_RET_TIMESET_MSG
 * \~english  Struct of message about GPS time set response
 */
typedef struct {
  T_APIMSG_MSGBUF_HEADER  header;     //!< \~english message header
  uint64_t                status;     //!< \~english response status
} TG_GPS_RET_TIMESET_MSG;

/**
 * @struct TG_GPS_NMEA_SENTENCE_INFO
 * \~english  Struct of GPS NMEA sentence info
 */
typedef struct {
  uint8_t                 uc_size;    //!< \~english sentence size
  uint8_t                 reserve;    //!< \~english reserve
  uint16_t                us_offset;  //!< \~english sentence start offset
} TG_GPS_NMEA_SENTENCE_INFO;

/**
 * @struct TG_GPS_NMEA_INFO
 * \~english  Struct of GPS NMEA info
 */
typedef struct {
  uint32_t                ul_rcvsts;        //!< \~english NMEA receive info
  uint8_t                 reserve[4];       //!< \~english reserve
  TG_GPS_NMEA_SENTENCE_INFO st_nmea_sentence_info[POS_SNS_GPS_NMEA_SNO_MAX];
    //!< \~english sentence infomation
} TG_GPS_NMEA_INFO;

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_hal
/// \~english @par Brief
///        - start thread of NMEA analysis and delivery message
/// \~english @param [in] h_app
///        HANDLE - handle
///
/// \~english @retval eFrameworkunifiedStatusOK       Success
/// \~english @retval eFrameworkunifiedStatusFail     Failed
///
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - This API can not change internal state
/// \~english @par Conditions of processing failure
///        - None
/// \~english @par Detail
///        - start thread of NMEA analysis and delivery message
///        - the API can only be used by 1 process
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync
/// \~english @see
///        None
/////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus StartGpsMainThreadPositioning(HANDLE h_app);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_hal
/// \~english @par Brief
///        - start thread for receive GPS manager
/// \~english @param [in] h_app
///        HANDLE - handle
///
/// \~english @retval eFrameworkunifiedStatusOK       Success
/// \~english @retval eFrameworkunifiedStatusFail     Failed
///
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - This API can not change internal state.
/// \~english @par Conditions of processing failure
///        - None
/// \~english @par Detail
///        - start model for receive GPS manager
///        - the API can only be used by 1 process
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync
/// \~english @see
///        None
/////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus  StartGpsRecvThreadPositioning(HANDLE h_app);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_hal
/// \~english @par Brief
///        -   start thread for receive sensor
/// \~english @param [in] h_app
///        HANDLE - handle
///
/// \~english @retval eFrameworkunifiedStatusOK       Success
/// \~english @retval eFrameworkunifiedStatusFail     Failed
///
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - This API can not change internal state.
/// \~english @par Conditions of processing failure
///        - None
/// \~english @par Detail
///        - start thread for receive sensor
///        - the API can only be used by 1 process
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync
/// \~english @see
///        None
/////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus StartLineSensorThreadPositioning(HANDLE h_app);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup positioning_hal
/// \~english @par Brief
///        - start monitoring thread of GPS rollover
/// \~english @param [in] h_app
///        HANDLE - handle
///
/// \~english @retval eFrameworkunifiedStatusOK       Success
/// \~english @retval eFrameworkunifiedStatusFail     Failed
///
/// \~english @par Prerequisite
///        - None
/// \~english @par Change of internal state
///        - This API can not change internal state.
/// \~english @par Conditions of processing failure
///        - The parameter hApp is NULL [eFrameworkunifiedStatusFail]
///        - When fail to start thread. [return value of FrameworkunifiedStartChildThread]
/// \~english @par Detail
///        - start monitoring thread of GPS rollover
///        - the API can only be used by 1 process
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync
/// \~english @see
///        FrameworkunifiedCreateChildThread, FrameworkunifiedStartChildThread
/////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus  StartGpsRolloverThreadPositioning(HANDLE h_app);

/** @}*/  // end of positioning_hal
/** @}*/  // end of positioning

/*---------------------------------------------------------------------------*/
#endif  // HAL_API_POSITIONING_HAL_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
