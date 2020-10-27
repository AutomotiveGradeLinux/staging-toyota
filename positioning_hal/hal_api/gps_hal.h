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

#ifndef HAL_API_GPS_HAL_H_
#define HAL_API_GPS_HAL_H_

/**
 * @file gps_hal.h
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

typedef int32               RET_API;
typedef uint32_t            DID;

/*************************************************************************
 *  RET_API define
 *************************************************************************/
/*--- for message ---*/
#define POS_TIMESTS_NG  0x00
//!< \~english time has not been adjusted after GPS receiver reset(time input or master reset or CSF start)
#define POS_TIMESTS_RTC 0x01
//!< \~english time output from RTC Backup(have time adjustment)
#define POS_TIMESTS_OK  0x02   //!< \~english time adjustment completed

/* command ID */
#define CID_POSIF_REGISTER_LISTENER_GPS_TIME          0x0201
//!< \~english GPS time delivery command ID

#define POS_DID_GPS__CWORD82__SETINITIAL                    0x80000033
//!< \~english data ID of setting initial location and time to GPS

#define POS_DID_GPS__CWORD82__SETRMODEEX 0x80000035
//!< \~english data ID of setting GPS receiver mode(pull extension sentence)

#define POS_DID_GPS__CWORD82__SELSENT  0x80000036
//!< \~english data ID of setting command to GPS that output any sentence

/* NAVIINFO_RET_API */
#define  NAVIINFO_RET_NORMAL                  (0)   //!< \~english normal finish
#define  NAVIINFO_RET_ERROR_PARAM             (-1)  //!< \~english parameter error
#define  NAVIINFO_RET_ERROR_INNER             (-2)  //!< \~english internal error
#define  NAVIINFO_RET_ERROR_BUFFULL           (-3)  //!< \~english buffer full
#define  NAVIINFO_RET_ERROR_NOSUPPORT         (-4)  //!< \~english no support
#define  NAVIINFO_RET_ERROR_RESOURCE          (-5)  //!< \~english lack of resource

/* Positioning Level/GPS Information Positioning Status Information Definition */
#define  NAVIINFO_DIAG_GPS_FIX_STS_NON        (0)   //!< \~english position not fix
#define  NAVIINFO_DIAG_GPS_FIX_STS_2D         (1)   //!< \~english 2D position fix
#define  NAVIINFO_DIAG_GPS_FIX_STS_3D         (2)   //!< \~english 3D position fix

/* Reception status definition */
#define  NAVIINFO_DIAG_GPS_RCV_STS_NOTUSE     (0)   //!< \~english not used
#define  NAVIINFO_DIAG_GPS_RCV_STS_SEARCHING  (1)   //!< \~english searching
#define  NAVIINFO_DIAG_GPS_RCV_STS_TRACHING   (2)   //!< \~english tracking
#define  NAVIINFO_DIAG_GPS_RCV_STS_NOTUSEFIX  (3)   //!< \~english not used for position fix
#define  NAVIINFO_DIAG_GPS_RCV_STS_USEFIX     (4)   //!< \~english used for position fix

/* GPS Response Status Definition */
#define GPS_SENDOK                            0     //!< \~english send result OK
#define GPS_SENDNG                            1     //!< \~english send result NG

/* GPS reset request mode definition */
#define GPS_RST_COLDSTART                     0xff
//!< \~english GPS reset request(cold start)

#define RET_NORMAL    0      //!< \~english normally end

#define RET_RCVMSG    1      //!< \~english receive message only
#define RET_RCVSIG    2      //!< \~english receive signal only
#define RET_RCVBOTH   3      //!< \~english receive message and signal
#define RET_NOMSG     4      //!< \~english receive no message

#define RET_QUE       5      //!< \~english insert signal
#define RET_NOTQUE    6      //!< \~english do not insert signal
#define RET_PROCDOWN  7      //!< \~english sub process down found
#define RET_SLEEP     100    //!< \~english sleep time


#define RET_OSERROR   (-127)  //!< \~english OS system call error

#define RET_ERROR     (-1)
//!< \~english abnormally end(do not know reason)
#define RET_ERRPARAM  (-2)   //!< \~english parameter error
#define RET_ERRNOTRDY (-3)
//!< \~english port or semphore ID is not created
#define RET_ERRPROC   (-4)   //!< \~english error occurs in API process
#define RET_ERRTIMEOUT  (-5)
//!< \~english timeout occurs but process unfinished
#define RET_ERRMSGFULL  (-6)  //!< \~english message table full


#define RET_ERROVERFLW  (-7)
//!< \~english message size exceeds receive buffer size

#define RET_ERRINIT   (-8)   //!< \~english  abnormally initialization

#define RET_EV_NONE   (-20)  //!< \~english event does not exist
#define RET_EV_MAX    (-21)  //!< \~english event value exceeds max value
#define RET_EV_MIN    (-22)
//!< \~english event value is lower than min value

/* POS_RET_API */
#define POS_RET_NORMAL              0       //!< \~english normal finish
#define POS_RET_ERROR               (-1)    //!< \~english error occured
#define POS_RET_ERROR_DID           (-2)    //!< \~english data ID error
#define POS_RET_ERROR_INNER         (-3)    //!< \~english internal error
#define POS_RET_ERROR_PARAM         (-4)    //!< \~english parameter error
#define POS_RET_ERROR_BUFFULL       (-5)    //!< \~english buffer full error
#define POS_RET_ERROR_CREATE_EVENT  (-6)    //!< \~english create event error
#define POS_RET_ERROR_OUTOF_MEMORY  (-8)
//!< \~english share memory allocation size error

#define POS_RET_ERROR_SIZE          (-9)    //!< \~english memory size error
#define POS_RET_ERROR_TIMEOUT       (-10)   //!< \~english timeout error
#define POS_RET_ERROR_NOSUPPORT     (-11)   //!< \~english no support
#define POS_RET_ERROR_BUSY          (-12)   //!< \~english busy
#define POS_RET_ERROR_RESOURCE      (-13)   //!< \~english lack of resources
#define POS_RET_ERROR_MIN           (-13)   //!< \~english min value of error


#define NUM_OF_100msData (10)
//!< \~english number of gyro data every 100ms


#define CID_GPS_SERIAL1              (CID)0x0101
//!< \~english Defination of notification about Gps Data has been receiced

#define CID_GPS_SERIAL2  (CID)0x0102  //!< \~english notification about 1PPS interupt


#define CID_GPS_SERIAL3              (CID)0x0103
//!< \~english Defination of SIF2 Data

#define CID_GPS_SERIAL4              (CID)0x0104
//!< \~english Defination of _CWORD82_ command send

#define CID_GPS_SERIAL5              (CID)0x0105
//!< \~english Defination of _CWORD82_ command send

#define CID_GPS_SERIAL6              (CID)0x0106
//!< \~english Defination of u-blox request data

#define CID_GPS_SERIAL7              (CID)0x0107
//!< \~english Defination of u-blox ack data

#define CID_GPS_RECVDATA             (CID)(CID_GPS_BASE | CID_GPS_SERIAL1)
//!< \~english gps data receiced CID

#define CID_GPS_1PPSINT              (CID)(CID_GPS_BASE | CID_GPS_SERIAL2)
//!< \~english 1PPS interupt CID

#define CID_GPS_SIF2DATA             (CID)(CID_GPS_BASE | CID_GPS_SERIAL3)
//!< \~english SIF2 Data CID

/**
 * \~english @brief _CWORD82_ command send CID
 * \~english @brief Message structure\n
 * \~english @brief Message buffer header structure @ref T_APIMSG_MSGBUF_HEADER\n
 * \~english @brief Message header structure @ref T_APIMSG_HEADER\n
 * \~english @brief Message main structure @ref TgGpsSndData\n
 * \~english @brief Message data(Vehicle sensor information setting message) structure @ref VEHICLE_MSG_SEND_DAT\n
 */
#define CID_GPS__CWORD82__REQUEST          (CID)(CID_GPS_BASE | CID_GPS_SERIAL4)

#define CID_GPS_CYCLEDATA_PROV       (CID)(CID_GPS_BASE | CID_GPS_SERIAL5)
//!< \~english Gps cycle data CID

#define CID_GPS_RCV_REQUESTDATA      (CID)(CID_GPS_BASE | CID_GPS_SERIAL6)
//!< \~english u-blox request data CID

#define CID_GPS_RCV_ACKDATA          (CID)(CID_GPS_BASE | CID_GPS_SERIAL7)
//!< \~english u-blox ack data CID

#define CID_GPS_SERIAL8              (CID)0x0108
//!< \~english Defination of Time Setting request

#define CID_GPS_SERIAL9              (CID)0x0109
//!< \~english Defination of Time Setting ack

#define CID_GPS_SERIAL10             (CID)0x010A
//!< \~english Defination of back up data read request

#define CID_GPS_SERIAL11             (CID)0x010B
//!< \~english Defination gps of week count request

/**
 * \~english  @brief Defination of Time Setting request
 * \~english  @brief Message structure\n
 * \~english  @brief Message header structure @ref T_APIMSG_MSGBUF_HEADER\n
 * \~english  @brief Message body structure   @ref TgGpsSndData\n
 * \~english  @brief Positioning information structure @ref POS_MSGINFO\n
 * */
#define CID_GPS_TIMESETTING          (CID)(CID_GPS_BASE | CID_GPS_SERIAL8)

/**
 * \~english  @brief Defination of Time Setting ack
 * \~english  @brief Message structure\n
 * \~english  @brief Message body structure   @ref TG_GPS_RET_TIMESET_MSG\n
 */
#define CID_GPS_RETTIMESETTING       (CID)(CID_GPS_BASE | CID_GPS_SERIAL9)

/**
 * \~english @brief  Read backup data request CID
 * \~english @brief  When this command is received, the backup data is read.\n
 * \~english @brief  Message-Related Structures\n
 * \~english @brief  Message buffer header structure @ref T_APIMSG_MSGBUF_HEADER
 */
#define CID_GPS_BACKUPDATA_LOAD      (CID)(CID_GPS_BASE | CID_GPS_SERIAL10)

/**
 * \~english @brief Gps Week count request CID
 * \~english @brief Message structure\n
 * \~english @brief Message buffer header structure @ref T_APIMSG_MSGBUF_HEADER\n
 * \~english @brief Message header structure @ref T_APIMSG_HEADER\n
 * \~english @brief Message data(Gps Week correction count notify message) structure @ref GpsWeekCorCntMsg\n
 */
#define CID_GPSWEEKCOR_CNT_NOTIFICATION (CID)(CID_GPS_BASE | CID_GPS_SERIAL11)



/**
 * \~english @brief  GPS reset result delivery command ID
 * \~english @brief  If you want to catch above envents, use NSFW like below.
 * \~english @code  
 * l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(hThread, TN_POSITIONING_GPS, CID_POSIF_REQ_GPS_RESET, OnRecivePosResetMsg);
 *            @endcode  
 */
#define CID_POSIF_REQ_GPS_RESET                       0x0784

#define POSHAL_DID_GPS_CONNECT_ERROR   0x800000B5U
//!< \~english GPS receive error DID

#define GPS_READ_LEN                 316
//!< \~english Gps NMEA FULLBINARY max length

#define GPS_CMD_NMEA_GGA_SZ          (71U)        //!< \~english GGA
#define GPS_CMD_NMEA_DGGA_SZ         (75U)        //!< \~english double precison GGA
#define GPS_CMD_NMEA_VTG_SZ          (37U)        //!< \~english VTG
#define GPS_CMD_NMEA_RMC_SZ          (61U)        //!< \~english RMC
#define GPS_CMD_NMEA_DRMC_SZ         (67U)        //!< \~english double precison RMC
#define GPS_CMD_NMEA_GLL_SZ          (44U)        //!< \~english GLL
#define GPS_CMD_NMEA_DGLL_SZ         (50U)        //!< \~english double precison GLL
#define GPS_CMD_NMEA_GSA_SZ          (66U)        //!< \~english GSA
#define GPS_CMD_NMEA_GSV_SZ          (70U)        //!< \~english GSV
#define GPS_CMD_NMEA__CWORD44__GP_3_SZ   (78U)        //!< \~english _CWORD44_,GP,3
#define GPS_CMD_NMEA__CWORD44__GP_4_SZ   (25U)        //!< \~english _CWORD44_,GP,4
#define GPS_CMD_NMEA_P_CWORD82_F_GP_0_SZ   (50U)        //!< \~english _CWORD44_,GP,0
#define GPS_CMD_NMEA_P_CWORD82_J_GP_1_SZ   (31U)        //!< \~english _CWORD44_,GP,1
#define GPS_CMD_NMEA_P_CWORD82_I_GP_SZ     (71U)        //!< \~english P_CWORD82_I,GP
#define GPS_CMD_NMEA_P_CWORD82_E_GP_0_SZ   (50U)        //!< \~english P_CWORD82_E,GP,0
#define GPS_CMD_NMEA_P_CWORD82_J_GP_0_SZ   (63U)        //!< \~english P_CWORD82_J,GP,0
#define GPS_CMD_NMEA_P_CWORD82_E_GP_2_SZ   (21U)        //!< \~english P_CWORD82_E,GP,2
#define GPS_CMD_NMEA_P_CWORD82_G_GP_0_SZ   (34U)        //!< \~english P_CWORD82_G,GP,0
#define GPS_CMD_NMEA_P_CWORD82_J_GP_7_SZ   (37U)        //!< \~english P_CWORD82_J,GP,7
#define GPS_CMD_NMEA_P_CWORD82_J_GP_8_SZ   (45U)        //!< \~english P_CWORD82_J,GP,8
#define GPS_CMD_BINARY_SZ            (81U)        //!< \~english standard binary

#define GPS_CMD_FULLBIN_SZ   (316U)
//!< \~english FULL binary(GPS10 format)

#define GPS_CMD_FULLBIN_VINFO  (30U + 160U + 25U + 8U)
//!< \~english FULL bianry version

#define GPS_NMEA_MAX_SZ    (255U)  //!< \~english NMEA max size
#define GPS_DATASIZE_RTC   (13U)  //!< \~english RTC Data size
#define GPS_NMEARCVSTS_SZ   (1U)  //!< \~english received NMEA size

#define GPS_GPSANT_SZ       (1U)  //!< \~english gps antenna status size
#define GPS_SNSCNT_SZ       (1U)  //!< \~english Sensor Count value size


#define GPS_CMD_NMEA_PROV_SZ  (GPS_CMD_NMEA_DRMC_SZ + GPS_CMD_NMEA_GSA_SZ + \
                                    (GPS_CMD_NMEA_GSV_SZ*3) + GPS_CMD_NMEA__CWORD44__GP_3_SZ)
//!< \~english NMEA size

#define GPS_NMEA_SZ         (GPS_NMEARCVSTS_SZ + GPS_GPSANT_SZ + GPS_SNSCNT_SZ + GPS_CMD_NMEA_PROV_SZ)
//!< \~english NEMA size

#define GPS_FULLBIN_SZ      (GPS_GPSANT_SZ + GPS_SNSCNT_SZ + GPS_CMD_FULLBIN_SZ)
//!< \~english FULLBIN size

#define GPS__CWORD44_GP4_SZ     (GPS_GPSANT_SZ + GPS_SNSCNT_SZ + GPS_CMD_NMEA__CWORD44__GP_4_SZ)
//!< \~english _CWORD44_GP4 size

#define GPS_MSGDATA_SZ_MAX   (512U)  //!< \~english message maximum data size
#define GPS_TLGRM_LEN         253   //!< \~english message data length
#define GPS_MSG_VSINFO_DSIZE  1904  //!< \~english message body size

#define  POS_MSG_INFO_DSIZE       1028 //!< \~english message body maximum size

/**
 * @enum MdevGpsDataKind
 * \~english  enumeration of gps data kind
 */
typedef enum MdevGpsDataKind {
  MDEV_GPS_DATA_RAWDATA_NMEA   = 0,    //!< \~english raw NMEA data of GPS
  MDEV_GPS_DATA_RAWDATA_FULLBIN,       //!< \~english raw binary data of GPS
  MDEV_GPS_DATA_RAWDATA__CWORD44_GP4,      //!< \~english raw _CWORD44_ data of GPS
  MDEV_GPS_DATA_NAVISPEED,             //!< \~english speed data
  MDEV_GPS_DATA_CUSTOMDATA,            //!< \~english position information
  MDEV_GPS_DATA_GPSTIME,               //!< \~english GPS time
  MDEV_GPS_DATA_GPSTIME_RAW,           //!< \~english raw time of GPS
  MDEV_GPS_DATA_INITIAL                //!< \~english initialize data
} MDEV_GPS_DATA_KIND;

/**
 * @struct NAVIINFO_UTCTIME
 * \~english UTC time information structure
 */
typedef struct {
    uint16_t         year;         //!< \~english year
    uint8_t          month;        //!< \~english month
    uint8_t          date;         //!< \~english day
    uint8_t          hour;         //!< \~english hour
    uint8_t          minute;       //!< \~english minute
    uint8_t          second;       //!< \~english second
    uint8_t          reserved;     //!< \~english reserve
} NAVIINFO_UTCTIME;

/**
 * @struct SENSORLOCATION_LONLATINFO_DAT
 * \~english longitude and latitude information data
 *        - Get method(getMethod)
 *        - SENSOR_GET_METHOD_GPS - longitude and latitude from GPS
 *        - SENSOR_GET_METHOD_NAVI - longitude and latitude from Navigation
 *        - Synchrony count(SyncCnt)
 *        - Count for position data synchronous \n
 *          When delivery altitude and heading data, position data can be synchronized by SyncCnt.\n
 *          But the data of different method can not be synchronized by SyncCnt.\n
 *          Example 1: [LonLat from GPS] and [Heading from GPS] can be synchronized by SyncCnt.\n
 *          Example 2: [LonLat from GPS] and [LonLat from Navi] can not be synchronized by SyncCnt.\n
 *          Caution: The sensor count in sensor data delivery is another data.
 *        - Enable or not(isEnable) \n
 *          To describe this delivery message is whether can be used or not
 *        - 0 - not avaliable
 *        - not 0 - avaliable
 *        - when GPS data is specified,longitude and latitude is invalid at following condition:\n
 *          so [not avaliable] provieded
 *        - After system start, GPS unit has not received current location data and GPS unit \n
 *          status is not positioning fixed.
 *        - If it is not initialized status, certainly provide [avaliable] when Navigation data specified
 *        - If the status is [not avaliable], data following can not be guaranteed.
 *        - Position status(posSts)
 *        - It is valid only when "Get method is Navigation" and "evironment is _CWORD80_".(otherwise it will be set as 0)
 *        - Bit0 : GPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_GSP)
 *        - Bit1 : DGPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_DGPS)
 *        - Bit3 : MapMatching data used result(1:used, 0:not used, definition of \n
 *          maskbit:POS_LOC_INFO_USE_MAPMATCHING)
 *        - Position accuracy(posAcc)
 *        - Detected accruray of current position:1LSB=1m \n
 *        - It is valid only when "Get method is Navigation" and "evironment is _CWORD80_".(otherwise it will be set as 0)\n
 *          0000H:0m \n
 *          0001H:1m \n
 *          :\n
 *          FFFDH:65533m \n
 *          FFFEH:65534m and larger than 65534m \n
 *          FFFFH:no data
 *        - Longitude : (WGS-84)(10^ -7degree as 1) \n
 *          East longitude is positive value and west longitude is negative value.
 *        - Latitude : (WGS-84)(10^ -7degree as 1) \n
 *          North latitude positive value and south latitude is negative value.
 */
typedef struct {
  uint8_t   getMethod;   //!< \~english get method
  uint8_t   SyncCnt;     //!< \~english Synchrony count
  uint8_t   isEnable;    //!< \~english enable or not
  uint8_t   posSts;      //!< \~english position status
  uint16_t  posAcc;      //!< \~english position accuracy
  int32_t   Longitude;   //!< \~english longitude(10^-7degree)
  //!< \~english max: +180.0000000degree[east longitude]
  //!< \~english min: -170.9999999degree[west longitude]
  int32_t   Latitude;    //!< \~english latitude(10^-7degree)
  //!< \~english max: +90.0000000degree[north Latitude]
  //!< \~english min: -90.0000000degree[south Latitude]
} SENSORLOCATION_LONLATINFO_DAT;


/**
 * @struct SENSORLOCATION_ALTITUDEINFO_DAT
 * \~english altitude information data
 *        - Get method(getMethod)
 *        - SENSOR_GET_METHOD_GPS - altitude from GPS
 *        - SENSOR_GET_METHOD_NAVI - altitude from Navigation
 *        - Synchrony count(SyncCnt)
 *        - Count for position data synchronous \n
 *          When delivery altitude and heading data, position data can be synchronized by SyncCnt. \n
 *          But the data of different method can not be synchronized by SyncCnt.\n
 *          example 1: [longitude and latitude from GPS] and [heading from GPS] can be synchronized by SyncCnt.\n
 *          example 2: [longitude and latitude from GPS] and [longitude and latitude from Navi] can not be
 *          synchronized by SyncCnt. \n
 *          Caution: The sensor count in sensor data delivery is another data.
 *        - Enable or not(isEnable) \n
 *          To describe this delivery message is whether can be used or not.
 *        - 0 - not avaliable
 *        - not 0 - avaliable
 *        - when GPS data specified, Altitude is invalid at following condition(so [not avaliable] provieded):
 *        - Immediately after system start, GPS unit has not received current location data and GPS unit status \n
 *          is not positioning fix
 *        - If it is not initialization status, certainly provide [avaliable] when Navi data specified
 *        - If the status is [not avaliable], data following can not be guaranteed.
 *        - Altitude
 *        - altitude data(unit 0.01m)
 */
typedef struct {
  uint8_t   getMethod;    //!< \~english get method
  uint8_t   SyncCnt;      //!< \~english Synchrony count
  uint8_t   isEnable;     //!< \~english enable or not
  uint8_t   Reserved[3];  //!< \~english reserve
  int32_t   Altitude;     //!< \~english altitude(0.01m)
} SENSORLOCATION_ALTITUDEINFO_DAT;


/**
 * @struct SENSORMOTION_HEADINGINFO_DAT
 * \~english heading information data
 *        - Get method(getMethod)
 *        - SENSOR_GET_METHOD_GPS - Heading from GPS
 *        - SENSOR_GET_METHOD_NAVI - Heading from Navi
 *        - Synchrony count(SyncCnt)
 *        - Count for position data synchronous \n
 *          When delivery altitude and heading data, position data can be synchronized by this count. \n
 *          But the data of different method can not be synchronized by this count.\n
 *          example 1: [longitude and latitude from GPS] and [heading from GPS] can be synchronized by the count. \n
 *          example 2: [longitude and latitude from GPS] and [longitude and latitude from Navi] can not be \n
 *          synchronized by the count. \n
 *          Caution: The sensor count in sensor data delivery is another data.
 *        - Enable or not(isEnable) \n
 *          To describe this data is whether enable or not 
 *        - 0 - not avaliable
 *        - not 0 - avaliable
 *        - Heading is invalid at following condition when GPS data specified, so [not avaliable] provieded \n
 *        - Immediately after system start, GPS unit has not received current location data and GPS unit status \n
 *          is not positioning fix
 *        - If it is not initialization status, certainly provide [avaliable] when Navi data specified
 *        - If the status is [not avaliable], data following can not be guaranteed.
 *        - Position status(posSts)
 *        - It is valid only when "Get method is Navi" and "evironment is _CWORD80_".(otherwise it will be set as 0)
 *        - Bit0 : GPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_GSP)
 *        - Bit1 : DGPS data used result(1:used, 0:not used, definition of maskbit:POS_LOC_INFO_USE_DGPS)
 *        - Bit3 : MapMatching data used result(1:used, 0:not used, definition of \n
 *          maskbit:POS_LOC_INFO_USE_MAPMATCHING)
 *        - Heading
 *        - Heading data(0.01degree as 1, based on north and clockwise count)\n
 *          But in environment _CWORD95_/_CWORD101___CWORD84_,the heading from Navi is a approximate that separate 360 degree \n
 *          into 16 part. \n
 *          example: 0, 2300, 4500, ..., 31500, 33800
 */
typedef struct {
  uint8_t   getMethod;    //!< \~english get method
  uint8_t   SyncCnt;      //!< \~english Synchrony count
  uint8_t   isEnable;     //!< \~english enable or not
  uint8_t   posSts;       //!< \~english position status
  uint8_t   Reserved1[2];  //!< \~english reserve
  uint16_t  Heading;     //!< \~english heading(0.01degree)
  //!< \~english max: 359.99 degree
  //!< \~english min: 0.00 degree
  uint8_t   Reserved2[2];    //!< \~english reserve
} SENSORMOTION_HEADINGINFO_DAT;

/**
 * @struct NAVIINFO_DIAG_GPS_FIX_CNT
 * \~english position fix count data structure
 */
typedef struct {
  uint32_t    ulCnt3d;    //!< \~english position fix count:3D
  uint32_t    ulCnt2d;    //!< \~english position fix count:2D
  uint32_t    ulCntElse;  //!< \~english position fix count:not fix
} NAVIINFO_DIAG_GPS_FIX_CNT;

/**
 * @struct NAVIINFO_DIAG_GPS_FIX_XYZ
 * \~english longitude and latitude data structure
 */
typedef struct {
  int32_t     lLat;       //!< \~english GPS latitude
  int32_t     lLon;       //!< \~english GPS longitude
} NAVIINFO_DIAG_GPS_FIX_XYZ;

/**
 * @struct NAVIINFO_DIAG_GPS_FIX
 * \~english position fix information structure
 */
typedef struct {
  uint8_t                   ucFixSts;     //!< \~english fix status
  uint8_t                   ucReserve[3];  //!< \~english reserve
  NAVIINFO_DIAG_GPS_FIX_CNT stCnt;        //!< \~english fix count data
  NAVIINFO_DIAG_GPS_FIX_XYZ stWgs84;      //!< \~english longitude and latitude data
} NAVIINFO_DIAG_GPS_FIX;

/**
 * @struct NAVIINFO_DIAG_GPS_PRN
 * \~english satellite information structure
 *        - reception status(ucRcvSts)
 *        - NAVIINFO_DIAG_GPS_RCV_STS_NOTUSE : not used
 *        - NAVIINFO_DIAG_GPS_RCV_STS_SEARCHING : searching
 *        - NAVIINFO_DIAG_GPS_RCV_STS_TRACHING : tracking
 *        - NAVIINFO_DIAG_GPS_RCV_STS_NOTUSEFIX : not used for position fix
 *        - NAVIINFO_DIAG_GPS_RCV_STS_USEFIX : used for position fix
 */
typedef struct {
  uint8_t     ucRcvSts;     //!< \~english reception status
  uint8_t     ucPrn;        //!< \~english satellite No.
  uint8_t     ucelv;        //!< \~english satellite dramatic angle
  uint8_t     ucLv;         //!< \~english satellite signal level
  uint16_t    usAzm;        //!< \~english satellite azimuth
  uint8_t     ucReserve[2];  //!< \~english reserve
} NAVIINFO_DIAG_GPS_PRN;

/**
 * @struct NAVIINFO_DIAG_GPS_SAT
 * \~english all satellite information structure
 */
typedef struct {
  NAVIINFO_DIAG_GPS_PRN   stPrn[12];    //!< \~english all satellite information
} NAVIINFO_DIAG_GPS_SAT;

/**
 * @struct NAVIINFO_DIAG_GPS
 * \~english position fixed and satellite information structure
 */
typedef struct {
  NAVIINFO_DIAG_GPS_FIX   stFix;    //!< \~english position fixed information
  NAVIINFO_DIAG_GPS_SAT   stSat;    //!< \~english all satellite information
} NAVIINFO_DIAG_GPS;

/**
 * @struct NAVIINFO_NAVI_GPS
 * \~english other GPS related information structure
 *        - altitude(altitude) \n
 *          As the altitude is used as unit [0.01m] in internal, \n
 *          the altitude data range is (-21,474,839~21,474,839).
 *        - UTC(utc)
 *        - The time set after rollover.
 *        - date and time status(tdsts)
 *        - 0= time has not been adjusted after GPS receiver reset(time input or master reset or CSF start)
 *        - 1= time output from RTC Backup(have time adjustment result)
 *        - 2= time adjustment completed
 */
typedef struct {
  int32_t           altitude;     //!< \~english altitude
  uint16_t          speed;        //!< \~english speed
  uint16_t          heading;      //!< \~english heading
  NAVIINFO_UTCTIME  utc;          //!< \~english UTC time
  uint8_t           tdsts;        //!< \~english date and time status
  uint8_t           reserve[3];   //!< \~english reserve
} NAVIINFO_NAVI_GPS;

/**
 * @struct NAVIINFO_ALL
 * \~english Navi data structure
 */
typedef struct {
  uint8_t           ucSensorCnt;  //!< \~english sensor count
  uint8_t           reserve[3];   //!< \~english reserve
  NAVIINFO_DIAG_GPS stDiagGps;    //!< \~english position fix related information
  NAVIINFO_NAVI_GPS stNaviGps;    //!< \~english other GPS related information
} NAVIINFO_ALL;

/**
 * @struct POS_MSGINFO
 * \~english message delivery positioning sensor information
 */
typedef struct {
    DID              did;          //!< \~english data ID
    PNO              pno;          //!< \~english delivery destination PNO
    uint16_t         size;         //!< \~english data body size
    uint8_t          rcv_flag;      //!< \~english received flag
    uint8_t          reserve;      //!< \~english reserve
    uint8_t          data[POS_MSG_INFO_DSIZE];  //!< \~english data body
} POS_MSGINFO;

/**
 * @struct SENSOR_GPSTIME
 * \~english GPS time information
 *        - date amd time status(tdsts)
 *        - 0= time has not been adjusted after GPS receiver reset(time input or master reset or CSF start)
 *        - 1= time output from RTC Backup(have time adjustment result)
 *        - 2= time adjustment completed
 */
typedef struct {
  NAVIINFO_UTCTIME  utc;          //!< \~english UTC time
  uint8_t           tdsts;        //!< \~english time status
  uint8_t           reserve[3];   //!< \~english reserve
} SENSOR_GPSTIME;

#define SENSOR_MSG_GPSTIME SENSOR_GPSTIME  //!< \~english GPS time information
#define SENSOR_GPSTIME_RAW SENSOR_GPSTIME  //!< \~english GPS time information

/**
 * @struct MdevGpsCustomData
 * \~english  Struct of Gps custom data
 */
typedef struct MdevGpsCustomData {
  MDEV_GPS_DATA_KIND                  e_kind;      //!< \~english kind
  SENSORLOCATION_LONLATINFO_DAT       st_lonlat;   //!< \~english longtitude,latitude
  SENSORLOCATION_ALTITUDEINFO_DAT     st_altitude;  //!< \~english  altitude
  SENSORMOTION_HEADINGINFO_DAT        st_heading;  //!< \~english  headings
  SENSOR_MSG_GPSTIME                  st_gps_time;  //!< \~english  Gps Time
  NAVIINFO_DIAG_GPS                   st_diag_gps;  //!< \~english  Gps data from diag
} MDEV_GPS_CUSTOMDATA;

/**
 * @struct MdevGpsCustomDataMgs
 * \~english  Struct of Gps custom data message
 */
typedef struct MdevGpsCustomDataMgs {
  T_APIMSG_MSGBUF_HEADER      h_dr;                //!< \~english header
  MDEV_GPS_CUSTOMDATA         st_data;             //!< \~english  Gps Custom Data
} MDEV_GPS_CUSTOMDATA_MGS;

/**
 * @struct MdevGpsRtc
 * \~english  Struct of Gps RTC data
 */
typedef struct MdevGpsRtc {
  u_int32     ul_retsts;                         //!< \~english return status
  u_int8      uc_datalen;                        //!< \~english data length
  u_int8      uc_ctrlcode;                       //!< \~english control code
  u_int8      uc_bcd_year;                        //!< \~english year
  u_int8      uc_bcd_month;                       //!< \~english month
  u_int8      uc_bcd_day;                         //!< \~english day
  u_int8      uc_bcd_hour;                        //!< \~english hour
  u_int8      uc_bcd_min;                         //!< \~english minute
  u_int8      uc_bcd_sec;                         //!< \~english second
  u_int8      uc_status;                         //!< \~english status
  u_int8      u_reserve[3];                       //!< \~english reserve
} MDEV_GPS_RTC;

/**
 * @struct MdevGpsGpsTime
 * \~english  Struct of Gps Time
 */
typedef struct MdevGpsGpsTime {
  MDEV_GPS_DATA_KIND          e_kind;              //!< \~english kind
  MDEV_GPS_RTC                st_rtc_data;          //!< \~english Gps Time
} MDEV_GPS_GPSTIME;

/**
 * @struct MdevGpsGpsTimeMgs
 * \~english  Struct of Gps Time message
 */
typedef struct MdevGpsGpsTimeMgs {
  T_APIMSG_MSGBUF_HEADER      h_dr;                //!< \~english header
  MDEV_GPS_GPSTIME            st_data;             //!< \~english Gps Time Data
} MDEV_GPS_GPSTIME_MGS;

/**
 * @struct DevGpsUtcTime
 * \~english  Struct of Gps UTC time
 */
typedef struct DevGpsUtcTime {
  u_int16                   us_year;               //!< \~english Year
  u_int8                    uc_month;              //!< \~english month
  u_int8                    uc_date;               //!< \~english day
  u_int8                    uc_hour;               //!< \~english hour
  u_int8                    uc_minute;             //!< \~english minute
  u_int8                    uc_second;             //!< \~english second
  u_int8                    u_reserved;           //!< \~english reserve
} DEV_GPS_UTCTIME;

/**
 * @enum MdevGpsFixSts
 * \~english  enumeration of gps fix status
 */
typedef enum MdevGpsFixSts {
  MDEV_GPS_NOTFIX  = 0,                           //!< \~english not fix
  MDEV_GPS_FIX                                    //!< \~english fix
} MDEV_GPS_FIX_STS;

/**
 * @struct MdevGpsNmea
 * \~english  Struct of gps NMEA Data
 */
typedef struct MdevGpsNmea {
  u_int8      uc_nmea_data[GPS_MSG_VSINFO_DSIZE];   //!< \~english NMEA Data
} MDEV_GPS_NMEA;

/**
 * @struct MdevGpsFullbin
 * \~english  Struct of Gps full bin data
 */
typedef struct MdevGpsFullbin {
  u_int8      uc_fullbin_data[GPS_FULLBIN_SZ];      //!< \~english gps full bin data
  u_int8      u_reserve[3];                         //!< \~english reserve
} MDEV_GPS_FULLBIN;

/**
 * @struct MdevGps_CWORD44_gp4
 * \~english  Struct of Gps _CWORD44_GP4 data
 */
typedef struct MdevGps_CWORD44_gp4 {
  u_int8      uc__CWORD44_gp4_data[GPS__CWORD44_GP4_SZ];    //!< \~english gps _CWORD44_GP4 data
  u_int8      u_reserve;                            //!< \~english reserve
} MDEV_GPS__CWORD44_GP4;

/**
 * @struct MdevGpsCycledata
 * \~english  Struct of Gps cycle data
 */
typedef struct MdevGpsCycledata {
  MDEV_GPS_DATA_KIND  e_kind;          //!< \~english kind
  u_int8      uc_data[GPS_MSGDATA_SZ_MAX - sizeof(MDEV_GPS_DATA_KIND)];  //!< \~english data
} MDEV_GPS_CYCLEDATA;

/**
 * @struct MdevGpsCycledataMsg
 * \~english  Struct of Gps cycle data message
 */
typedef struct MdevGpsCycledataMsg {
  T_APIMSG_MSGBUF_HEADER      h_dr;              //!< \~english header
  MDEV_GPS_CYCLEDATA          st_data;           //!< \~english data
} MDEV_GPS_CYCLEDATA_MSG;

/**
 * @struct MdevGpsRawdataNmea
 * \~english  Struct of Gps NMEA Raw Data
 */
typedef struct MdevGpsRawdataNmea {
  MDEV_GPS_DATA_KIND          e_kind;            //!< \~english kind
  MDEV_GPS_NMEA               st_nmea_data;       //!< \~english NMEA Data
} MDEV_GPS_RAWDATA_NMEA;

/**
 * @struct MdevGpsRawdataNmeaMsg
 * \~english  Struct of Gps NMEA Raw Data message
 */
typedef struct MdevGpsRawdataNmeaMsg {
  T_APIMSG_MSGBUF_HEADER      h_dr;              //!< \~english header(see vs-positioning-base-library)
  MDEV_GPS_RAWDATA_NMEA       st_data;           //!< \~english data
} MDEV_GPS_RAWDATA_NMEA_MSG;

/**
 * @struct MdevGpsRawdataFullbin
 * \~english  Struct of Gps Full Bin Raw Data
 */
typedef struct MdevGpsRawdataFullbin {
  MDEV_GPS_DATA_KIND          e_kind;            //!< \~english kind
  MDEV_GPS_FULLBIN            st_fullbin_data;    //!< \~english Full Binary Data
} MDEV_GPS_RAWDATA_FULLBIN;

/**
 * @struct MdevGpsRawdataFullbinMsg
 * \~english  Struct of Gps Full Bin Raw Data message
 */
typedef struct MdevGpsRawdataFullbinMsg {
  T_APIMSG_MSGBUF_HEADER      h_dr;      //!< \~english header
  MDEV_GPS_RAWDATA_FULLBIN    st_data;   //!< \~english Gps Full Bin Data
} MDEV_GPS_RAWDATA_FULLBIN_MSG;

/**
 * @struct MdevGpsRawdata_CWORD44_gp4
 * \~english  Struct of Gps _CWORD44_GP4 Raw Data
 */
typedef struct MdevGpsRawdata_CWORD44_gp4 {
  MDEV_GPS_DATA_KIND          e_kind;            //!< \~english kind
  MDEV_GPS__CWORD44_GP4           st__CWORD44_gp4;       //!< \~english Gps _CWORD44_GP4 Data
} MDEV_GPS_RAWDATA__CWORD44_GP4;

/**
 * @struct MdevGpsRawdata_CWORD44_gp4Msg
 * \~english  Struct of Gps _CWORD44_GP4 Raw Data message
 */
typedef struct MdevGpsRawdata_CWORD44_gp4Msg {
  T_APIMSG_MSGBUF_HEADER      h_dr;     //!< \~english header
  MDEV_GPS_RAWDATA__CWORD44_GP4   st_data;  //!< \~english Gps _CWORD44_GP4 Raw Data
} MDEV_GPS_RAWDATA__CWORD44_GP4_MSG;

/**
 * @struct SENSORMOTION_SPEEDINFO_DAT
 * \~english speed information data
 *        - Get method(getMethod)
 *        - SENSOR_GET_METHOD_POS - The speed calculated in positioning based on speed pulse will be deliveried.
 *        - SENSOR_GET_METHOD_NAVI - speed from Navi
 *        - Synchrony count(SyncCnt)
 *        - 0 (not change).
 *        - Enable or not(isEnable) \n
 *          To describe this data is whether enable or not 
 *        - 0 - not avaliable
 *        - not 0 - avaliable
 *        - Speed is invalid at following condition when speed pulse specified, so [not avaliable] provieded
 *        - Immediately after system start, the sensor data have not been received from SYS micon
 *        - If it is not initialization status, certainly provide [avaliable] when Navi data specified
 *        - If the status is [not avaliable], data following can not be guaranteed.
 *        - Speed
 *        - speed data(unit 0.01m/sec)
 */
typedef struct {
  uint8_t   getMethod;    //!< \~english get method
  uint8_t   SyncCnt;      //!< \~english Synchrony count
  uint8_t   isEnable;     //!< \~english enable or not
  uint8_t   Reserved1[3];  //!< \~english reserve
  uint16_t  Speed;        //!< \~english speed(0.01m/sec)
  uint8_t   Reserved2[2];  //!< \~english reserve
} SENSORMOTION_SPEEDINFO_DAT;

/**
 * @struct MdevGpsNavispeed
 * \~english  Struct of Gps speed data
 */
typedef struct MdevGpsNavispeed {
  MDEV_GPS_DATA_KIND          e_kind;            //!< \~english kind
  u_int16                     us_speed_kmph;     //!< \~english Speed(km/h)
  u_int8                      u_reserve[2];       //!< \~english reserve
  SENSORMOTION_SPEEDINFO_DAT  st_speed;          //!< \~english speed data
} MDEV_GPS_NAVISPEED;

/**
 * @struct MdevGpsNavispeedMsg
 * \~english  Struct of Gps speed data message
 */
typedef struct MdevGpsNavispeedMsg {
  T_APIMSG_MSGBUF_HEADER      h_dr;              //!< \~english header
  MDEV_GPS_NAVISPEED          st_data;           //!< \~english Gps Speed Data
} MDEV_GPS_NAVISPEED_MSG;

/**
 * @struct TgGpsSndData
 * \~english  Struct of _CWORD82_ Gps send data
 */
typedef struct TgGpsSndData {
  uint8_t     reserve[4];                         //!< \~english reserve
  u_int16     us_size;                            //!< \~english send data length
  u_int8      ub_data[502];                       //!< \~english send data
} TG_GPS_SND_DATA;

/**
 * @struct TgGpsRetPram
 * \~english  Gps Response data
 */
typedef struct TgGpsRetPram {
  unsigned int  ret_status;               //!< \~english response flag
  unsigned char data_len;                 //!< \~english data size
  unsigned char ctrl_code;                //!< \~english control code
  unsigned char gps_data[GPS_TLGRM_LEN];  //!< \~english data contents
} TG_GPS_RET_PRAM;

/**
 * @struct SensorWknRollOverHal
 * \~english  Struct of GPS week number
 */
typedef struct SensorWknRollOverHal {
  uint16_t  us_wkn;     //!< \~english  GPS week number
} SENSOR_WKN_ROLLOVER_HAL;

/**
 * @struct POS_DATETIME
 * \~english GPS time information structure
 */
typedef struct {
    uint16_t         year;         //!< \~english year
    uint8_t          month;        //!< \~english month
    uint8_t          date;         //!< \~english day
    uint8_t          hour;         //!< \~english hour
    uint8_t          minute;       //!< \~english minute
    uint8_t          second;       //!< \~english second
    uint8_t          reserved;     //!< \~english reserve
} POS_DATETIME;

/**
 * @struct TG_GPS_RET_RESET
 * \~english GPS reset status
 */
typedef struct {
  unsigned long     ret_rst_status;   //!< \~english response GPS reset status
} TG_GPS_RET_RESET;

/**
 * @struct TG_GPS_RET_RESET_MSG
 * \~english GPS reset information message
 */
typedef struct {
  TG_GPS_RET_RESET    data;   //!< \~english GPS reset status data
} TG_GPS_RET_RESET_MSG;

/**
 * @struct VEHICLE_MSG_SEND_DAT
 * \~english Vehicle information setting message structure
 */
typedef struct {
    DID                       did;          //!< \~english Data ID
    u_int16                   size;         //!< \~english Data size
    u_int8                    data[502];    //!< \~english Data
} VEHICLE_MSG_SEND_DAT;

/**
 * @struct GpsWeekCorCntMsg
 * \~english Gps Week correction count notify message structure
 */
typedef struct GpsWeekCorCntMsg {
  T_APIMSG_MSGBUF_HEADER  stHead;          //!< \~english Message header
  u_int8                  gpsweekcorcnt;    //!< \~english Gps Week correction count
  int8                    dummy[3];         //!< \~english Alignment adjustment
} CLOCKGPS_GPSWEEKCOR_CNT_MSG;

/** @}*/  // end of positioning_hal
/** @}*/  // end of positioning

/*---------------------------------------------------------------------------*/
#endif  // HAL_API_GPS_HAL_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
