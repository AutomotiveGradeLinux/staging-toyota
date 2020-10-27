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
 * @file DTime_Api.h
 * @brief display time middle API header file.
 */
#ifndef CLOCK_CLIENT_DISPLAY_TIME_INCLUDE_VEHICLE_SERVICE_DTIME_API_H_
#define CLOCK_CLIENT_DISPLAY_TIME_INCLUDE_VEHICLE_SERVICE_DTIME_API_H_

/****************************************************************************
 * File name       : DTime_Api.h
 * System name     : 
 * Sub system name : Display time middle API header
 * Title           : Data Header Files Required for Using Display Time Middle
 ****************************************************************************/
#include <native_service/frameworkunified_types.h>
#include <stub/clock_notifications.h>

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup clock
 *  @ingroup vehicle_service
 *  @{
 */

/**
 * \~english commod ID (from display time middle)
 */
typedef enum _ClockServiceProtocol {
    CID_DTIME_NOTIFY    = 0x00000712,   //!< \~english Display time notice
} ClockServiceProtocol;

/******************************************************************************
* Structure : T_TimeData
* Function  : Time structure
* Note      : Stores the system/user time obtained from the Clock
 *******************************************************************************/
/**
 * \~english Time structure T_TimeData, store system time/user time acquired from clock.
 */
typedef struct _tag_T_TimeData {
    uint16_t       Year;        //!< \~english Year
    uint8_t        Month;       //!< \~english Month
    uint8_t        Day;         //!< \~english Day
    uint8_t        Hour;        //!< \~english Hour
    uint8_t        Minute;      //!< \~english Minute
    uint8_t        Second;      //!< \~english Second
    uint8_t        rsv;         //!< \~english Reserve
} T_TimeData;

/**
 * \~english +(plus)
 */
#define DTIME_TIMEDIFF_SIGN_PLUSE        FALSE
/**
 * \~english -(minus)
 */
#define DTIME_TIMEDIFF_SIGN_MINUS        TRUE

/**
 * \~english Time zone structure.
 */
typedef struct _tag_T_TimeDiff {
    BOOL       Sign;             //!< \~english Sign
    uint8_t    Hour_Diff;        //!< \~english Time difference (hour)
    uint8_t    Minute_Diff;      //!< \~english Time difference (minutes)
    uint8_t    rsv[2];           //!< \~english Reserve
} T_TimeDiff;

/**
 * \~english Time offset setting structure.
 */
typedef struct _tag_T_TimeOffset {
    int8_t    Hour_Offset;    //!< \~english Time difference (hour)
    int8_t    Minute_Offset;  //!< \~english Time difference (minutes)
    int8_t    rsv[2];         //!< \~english Reserve
} T_TimeOffset;

/**
 * \~english GPS automatic adjustment OFF (use system time).
 */
#define DTIME_SETTING_GPS_MANUAL        FALSE

/**
 * \~english GPS automatic adjustment Auto (use display time when GPS correction OFF).
 */
#define DTIME_SETTING_GPS_AUTO            TRUE

/**
 * \~english Notation in 12H.
 */
#define DTIME_SETTING_FORMAT_12H        FALSE

/**
 * \~english Notation in 24H.
 */
#define DTIME_SETTING_FORMAT_24H        TRUE

/**
 * \~english DST manual setting.
 */
#define DTIME_SETTING_DST_MANUAL        FALSE

/**
 * \~english DST automatic setting.
 */
#define DTIME_SETTING_DST_AUTO            TRUE

/**
 * \~english DST OFF
 */
#define DTIME_SETTING_DST_OFF            FALSE

/**
 * \~english DST ON
 */
#define DTIME_SETTING_DST_ON            TRUE

/**
 * \~english Time zone manual setting.
 */
#define DTIME_SETTING_TIMEZONE_MANUAL    FALSE

/**
 * \~english Time zone automatic setting.
 */
#define DTIME_SETTING_TIMEZONE_AUTO        TRUE

/**
 * \~english Display setting structure.
 */
typedef struct _tag_T_DisplaySetting {
    BOOL            GPS_Auto;          //!< \~english GPS automatic setting
    BOOL            Format;            //!< \~english  12H/24H display setting
    BOOL            DST_Auto;          //!< \~english DST automatic settting
    BOOL            DST_Setting;       //!< \~english DST_ON/OFF setting
    BOOL            TimeZone_Auto;     //!< \~english Time zone automatic setting
    T_TimeDiff      TimeZone_Setting;  //!< \~english Time zone time difference setting
    T_TimeOffset    TimeOffset;        //!< \~english Time offset
} T_DisplaySetting;

/**
 * \~english Display time invalid.
 */
#define DTIME_DISP_TIME_INVALID            FALSE

/**
 * \~english Display time valid.
 */
#define DTIME_DISP_TIME_VALID            TRUE

/**
 * \~english Invalid AM/PM.
 */
#define DTIME_DISP_MERIDIEM_INVALID        0

/**
 * \~english AM
 */
#define DTIME_DISP_MERIDIEM_AM            1

/**
 * \~english PM
 */
#define DTIME_DISP_MERIDIEM_PM            2

/**
 * \~english AM/PM unidentified in 12H display.
 */
#define DTIME_DISP_MERIDIEM_12H_INVALID    3

/**
 * \~english Display time structure.
 */
typedef struct _tag_T_DTimeData {
    BOOL    TimeValidity;    //!< \~english Validity of time display
    uint8_t    Meridiem;     //!< \~english AM/PM
    uint16_t   DYear;        //!< \~english Display date(year)
    uint8_t    DMonth;       //!< \~english Display date(month)
    uint8_t    DDay;         //!< \~english Display date(day)
    uint8_t    DHour;        //!< \~english Display time(hour)
    uint8_t    DMinute;      //!< \~english Display time(minute)
    uint8_t    rsv;          //!< \~english Reserve
} T_DTimeData;

/******************************************************************************
* Structure : T_DTIME_MSG_NOTIFY_DTIME
* Function  : Display time notification transmission data structure
* Note      :
 ******************************************************************************/
/**
 * \~english Display time notification transmission data structure T_DTIME_MSG_NOTIFY_DTIME.
 */
typedef struct _tag_T_DTIME_MSG_NOTIFY_DTIME {
    T_DTimeData            disp_time;        //!< \~english Display time
    T_DisplaySetting       disp_setting;     //!< \~english Display setting
    T_TimeData             org_time;         //!< \~english Time(origin data)
} T_DTIME_MSG_NOTIFY_DTIME;

/******************************************************************************
* Structure : T_DTIME_MSG_GPSTIME
* Function  : GPS time setting data structure
* Note      :
 ******************************************************************************/
/**
 * \~english GPS UTC time setting structure.
 */
typedef struct _tag_T_DTIME_GPSTIME {
    uint16_t           year;            //!< \~english Year
    uint8_t            month;           //!< \~english Month
    uint8_t            date;            //!< \~english Day
    uint8_t            hour;            //!< \~english Hour
    uint8_t            minute;          //!< \~english Minute
    uint8_t            second;          //!< \~english Second
    uint8_t            reserved;        //!< \~english Reserve
} T_DTIME_GPSTIME;

/**
 * \~english Time uncalibrated after receiver reset.
 */
#define DTIME_GPS_STATUS_INVALID    0x00

/**
 * \~english Time output by RTC backup (with time calibration results).
 */
#define DTIME_GPS_STATUS_RTC        0x01

/**
 * \~english Time calibration completed.
 */
#define DTIME_GPS_STATUS_CORRECT    0x02

/**
 * \~english GPS time setting data structure.
 */
typedef struct _tag_T_DTIME_MSG_GPSTIME {
    T_DTIME_GPSTIME      utc;          //!< \~english GPS UTC time
    uint8_t              tdsts;        //!< \~english Date and time status
    uint8_t              reserve[3];   //!< \~english Reserve
} T_DTIME_MSG_GPSTIME;

/** Other definitions *********************************/
/**
 * \~english AM
 */
#define DTIME_SET_MERIDIEM_AM        TRUE

/**
 * \~english PM
 */
#define DTIME_SET_MERIDIEM_PM        FALSE

/* Displayed Time Middle API Prototypes *************/
/**
 * \ingroup DTime_loadDisplaySetting
 * \~english @par Brief
 *      - Load display settings.
 * \~english @param [in]  hApp        Application handle
 * \~english @param [in]  Setting     Display setting
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Display time is available after this API is called by SettingService.\n
 *        (It is assumed to call it only once at startup)
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When storage area of the display setting is NULL [eFrameworkunifiedStatusFail]
 *      - When the time difference(hour)specified by the argument Setting->TimeZone_Setting.Hour_Diff\n
 *        is greater than the time difference(hour)upper limit(13)[eFrameworkunifiedStatusFail]
 *      - Incorrect time difference(minutes)specified by argument Setting->TimeZone_Setting.Minute_Diff\n
 *        (when the value is not 0, 15, 30, 45). [eFrameworkunifiedStatusFail]
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage synchronization communication fails. [eFrameworkunifiedStatusFail]
 *      - When processing of loading display setting fails in clock service. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Detail
 *      - Since this API is synchronous processing(completion recovery),\n
 *        it should be used in consideration of this.
 *      - When display time is used before this API is called by SettingService,\n
 *        even if DTime_registListener_DisplayTime() is used, display time notification can not be received.
 *      - (Since clock service does not publish display time notification.)
 *      - With this API, the following is information necessary for calculation of\n
 *        display time can be collectively set to Clock service.
 *      - GPS automatic setting, 12H/24H display setting, DST setting, time zone setting, time offset.
 *      - Set the time offset to 0 when GPS correction is OFF.
 *      - If the display time distribution has already been regitered,\n
 *        the Clock service will publish the display time notification immediately when this API is called.
 * \~english @see DTime_getDisplaySetting, DTime_regist_Listener_DisplayTime, FrameworkunifiedMcOpenSender, FrameworkunifiedInvokeSync
 */
EFrameworkunifiedStatus DTime_loadDisplaySetting(HANDLE h_app, T_DisplaySetting *setting);  /* Load display setting */

/**
 * \ingroup DTime_getDisplaySetting
 * \~english @par Brief
 *      - Load display settings.
 * \~english @param [in]  hApp        Application handle
 * \~english @param [in]  Setting     Display setting
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - This API should be called after DTime_loadDisplaySetting() is called.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When storage area of the display setting is NULL [eFrameworkunifiedStatusFail]
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage synchronization communication fails. [eFrameworkunifiedStatusFail]
 *      - When message length received by synchronous communication\n
 *        and length of storage buffer are different. [eFrameworkunifiedStatusFail]
 *      - When display setting on Clock service side is not loaded. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Detail
 *      - Since this API is synchronous processing(completion recovery),\n
 *        it should be used in consideration of this.
 *      - This API assumes that the Diag service will use it to acquire time difference information etc.
 *      - With this API, you can acquire the information on the right set at display time.\n
 *        GPS automatic setiting, 12H/24H display setting, DST setting, time zone setting, time offse.
 * \~english @see DTime_loadDisplaySetting, FrameworkunifiedMcOpenSender, FrameworkunifiedInvokeSync
 */
EFrameworkunifiedStatus DTime_getDisplaySetting(HANDLE h_app, T_DisplaySetting *setting);  /* Get display setting */

/**
 * \ingroup DTime_setGpsTime
 * \~english @par Brief
 *      - Set the GPS time on the clock.
 * \~english @param [in]  hApp        Application handle
 * \~english @param [in]  gps_time     GPS data setting
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the setting of storage area set by the GPS data/time setting is NULL. [eFrameworkunifiedStatusFail]
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - This API is assumed to be called when the positioning service acquires/updates the GPS time.
 *      - Note : Do not use POS_RegisterListenerGPStime() from Clock service.
 *      - The GPS time notification command to the external clock is transmitted\n
 *        when this API is called.
 *      - With this API, GPS time can be set to clock function.
 *        GPS date and time (year, month, day, hour, minute, second)\n
 *        GPS date and time status\n
 *        0 = Time not correct after receiver reset(time input or manager reset or CSF startup),\n
 *        1 = RTC backup according to time output(actual result of time correction),\n
 *        2 = Time correct done.
 * \~english @see FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_setGpsTime(HANDLE h_app, T_DTIME_MSG_GPSTIME *gps_time);  /* GPS time setting */

/**
 * \ingroup DTime_registerListener_DTime
 * \~english @par Brief
 *      - Register delivery of display time notification.
 * \~english @param [in]  hApp        Application handle
 * \~english @param [in]  notifyName     Delivery destination thread name. Note : Set it to 15 characters or less.
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the storage area specified by the destination thread name is NULL. [eFrameworkunifiedStatusFail]
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - The maximum number of registered listeners is 20.
 *      - If listeners are registered more than once from the same thread, count as one listener.\n
 *        however, in this case, the display time notification is immediately issued.
 *      - With this API, delivery of this display time notification to the thread\n
 *        designated as the argument is started.
 *      - When this API call is called, if the display setting is not loaded,\n
 *        you can not calculate the display time, so do not deliver the display time notification.
 *      - When this API is called, if the display setting has already been loaded,\n
 *        the Clock service immediately issues a display time notification.
 *      - A message is notified with a command "CID_DTIME_NOTIFY" when display time is deliveried.
 *      - The client receives the data of the T_DTIME_MSG_NOTIFY_DTIME structure in the above delivery message.
 *      - Message communication uses interprocess communication by NSFW.
 *      - Specify ClockMng thread (TN_CLOCK_MNG) as the sender when registering callback for receiving display time.
 *      - Display time is not delivered until SettingService calls DTime_loadDisplaySetting.
 * \~english @see DTime_UnregisterListener_DTime, FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_registerListener_DTime(HANDLE h_app, PCSTR notify_name);  /* Display time delivery registration */

/**
 * \ingroup DTime_UnregisterListener_DTime
 * \~english @par Brief
 *      - Release delivery of display time notification.
 * \~english @param [in]  hApp        Application handle
 * \~english @param [in]  notifyName     Delivery destination thread name. Note : Set it to 15 characters or less.
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the storage area specified by the destination thread name is NULL. [eFrameworkunifiedStatusFail]
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - With this API, releasing delivery registration of display time notification\n
 *        to the thread specified as argument is canceled.
 *      - Especially when the nonresident service is registering for delivery,\n
 *        be sure to cancel registration with this API in termination processing.
 * \~english @see DTime_registerListener_DTime, FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_UnregisterListener_DTime(HANDLE h_app, PCSTR notify_name);  /* Release display time delivery registration */

/**
 * \ingroup DTime_setAutoAdjust
 * \~english @par Brief
 *      - Set GPS automatic adjustment.
 * \~english @param [in]  hApp        Application handle
 * \~english @param [in]  Setting     Display setting.\n
 *                       DTIME_SETTING_GPS_AUTO: GPS automatic adjust AUTO(use display time when GPS correction OFF)\n
 *                       DTIME_SETTING_GPS_MANUAL: GPS automatic adjust OFF(use of system time)
 * \~english @param [in]  Offset      Time offset.\n
 *                                    GPS correction is ON: User set hour offset, minute offset.\n
 *                                    GPS correction is OFF: set zero
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the storage area of the time offset is NULL. [eFrameworkunifiedStatusFail]
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - This API is assumed to be called by SettingService.
 *      - With this API, it is possible to switch the absolute time\n
 *        (GPS time or display time in the case of GPS correction off) which is the source of display time.
 *      - When GPS correction is OFF, count from the default time (2017/1/1 1:00).
 *      - When GPS correction OFF, set the time offset to zero and set the time\n
 *        offset with this API when GPS correction is ON.
 *      - When this API is called, if the display time distribution has already been refistered,\n
 *        the Clock service immediately issues a display time notification.
 *      - However, when an external clock is connected, the display time is not issued immediately\n
 *        and the display time is delivered at the timing of receiving the "Time notification".
 *      - When an external clock is connected, since the data master at the offset time\n
 *        becomes an external clock, the following processing is performed.
 *      - If the received time offset is
 *      - 1) When it is more than 1 hour ahead of the offset of the external clock \n
 *        -> Set to add 1 hour offsets to external clock.
 *      - 2) If it is less than 1 hour, 1 minute or more after the offset of the external clock \n
 *        -> Set to add 1 minute offsets to external clock.
 *      - 3) When it is more than -1 hour earlier of the offset of the external clock \n
 *        -> Set the external clock to subtract 1 hour offsets.
 *      - 4) When it is less than 1 hour or more than -1 minute from the offset of the external clock \n
 *        -> Set the external clock to subtract 1 minute offsets.
 *      - 5) When both hours and minutes are zero\n
 *        -> Set the external clock to clear zero.
 *      - Do not reflect changes in offset when GPS correction OFF<-->ON is switched.
 * \~english @see FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_setAutoAdjust(HANDLE h_app, BOOL setting, T_TimeOffset *off_set);  /* GPS automatic adjustment */

/**
 * \ingroup DTime_increaseHour
 * \~english @par Brief
 *      - Adjust the time.
 * \~english @param [in]  hApp        Application handle
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - This API is assumed to be called by SettingService.
 *      - With this API, display time can be increased by one hour. Set the display\n
 *        time in the case of GPS correction off used when GPS correction OFF is set.
 *      - Note : The time offset when GPS correction ON is set by DTime_setAutoAdjust.
 *      - When this API is called, if the display time distribution has already been registered,\n
 *        the Clock service immediately issues a display time notification.
 *      - Note : However, when there is an external clock, and after receiving the time notification command,\n
 *        the display time notification is not issued immediately, and the display time is delivered at\n
 *        the timing of receiving the "time notification".
 *      - When an external clock is connected, set the absolute time hour (+) instruction to\n
 *        the external clock with the "setting SW information notification" command.
 * \~english @see DTime_decreaseHour, DTime_setAutoAdjust, FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_increaseHour(HANDLE h_app);  /* Adjust the time (hour increaase) */

/**
 * \ingroup DTime_decreaseHour
 * \~english @par Brief
 *      - Adjust time (decrease hour by 1 hour).
 * \~english @param [in]  hApp        Application handle
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - This API is assumed to be called by SettingService.
 *      - With this API, display time can be reduced by 1 hour. Set the display time\n
 *        in the case of GPS correction off used when GPS correction OFF is set.
 *      - Note : The time offset when GPS correction ON is set by DTime_setAutoAdjust.
 *      - When this API is called, if the display time distribution has already been registered,\n
 *        the Clock service immediately issues a display time notification.
 *      - Note : However, when there is an external clock, and after receiving the time notification command,\n
 *        the display time notification is not issued immediately, and the display time is delivered\n
 *        at the timing of receiving the "time notification".
 *      - When an external clock is connected, set the absolute time hour (-) instruction\n
 *        with the "setting SW information notification" command to the external clock.
 * \~english @see DTime_increaseHour, DTime_setAutoAdjust, FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_decreaseHour(HANDLE h_app);  /* Adjust the time (hour decrease) */

/**
 * \ingroup DTime_increaseMinute
 * \~english @par Brief
 *      - Adjust the time (increase time by 1 minute).
 * \~english @param [in]  hApp        Application handle
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - This API is assumed to be called by SettingService.
 *      - With this API, display time can be increased by one minute. Set the display time\n
 *        in the case of GPS correction off used when GPS correction OFF is set.
 *      - Note : The time offset when GPS correction ON is set by DTime_setAutoAdjust.
 *      - When this API is called, if the display time distribution has already been registered,\n
 *        the Clock service immediately issues a display time notification.
 *      - Note : However, when there is an external clock, and after receiving the time notification command,\n
 *        the display time notification is not issued immediately, and the display time is delivered\n
 *        at the timing of receiving the "time notification".
 *      - When an external clock is connected, set the absolute time minute (+) instruction\n
 *        to the external clock with the "setting SW information notification" command.
 * \~english @see DTime_decreaseMinute, DTime_setAutoAdjust, FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_increaseMinute(HANDLE h_app);  /* Adjust the time (minute increaase) */

/**
 * \ingroup DTime_decreaseMinute
 * \~english @par Brief
 *      - Adjust the time (decrease time by 1 minute).
 * \~english @param [in]  hApp        Application handle
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - This API is assumed to be called by SettingService.
 *      - With this API, display time can be reduced by 1 minute. Set the display time\n
 *        in the case of GPS correction off used when GPS correction OFF is set.
 *      - Note : The time offset when GPS correction ON is set by DTime_setAutoAdjust.
 *      - When this API is called, if the display time distribution has already been registered,\n
 *        the Clock service immediately issues a display time notification.
 *      - Note : However, when there is an external clock, and after receiving the time notification command,\n
 *        the display time notification is not issued immediately, and the display time is delivered\n
 *        at the timing of receiving the "time notification".
 *      - When an external clock is connected, set the absolute time minute (-) instruction\n
 *        with the "setting SW information notification" command to the external clock.
 * \~english @see DTime_increaseMinute, DTime_setAutoAdjust, FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_decreaseMinute(HANDLE h_app);  /* Adjust the time (minute decrease) */

/**
 * \ingroup DTime_clearMinute
 * \~english @par Brief
 *      - Adjust the time (set the time to 0 minute).
 * \~english @param [in]  hApp        Application handle
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - This API is assumed to be called by SettingService.
 *      - With this API, the display time can be set to 0 minutes.
 *      - When the display time is 30 to 59 minutes, increase the display time by one hour\n
 *        and then set the display time to 0 minutes.
 *      - If the display time is 0 to 29 minutes, set the dispaly time to 0 minutes only.
 *      - Set the internal offset to be used when GPS correction OFF.
 *      - Note : The time offset when GPS correction ON is set by DTime_setAutoAdjust.
 *      - When this API is called, if the display time distribution has already been registered,\n
 *        the Clock service immediately issues a display time notification.
 *      - Note : However, when there is an external clock, and after receiving the time notification command,\n
 *        the display time notification is not issued immediately, and the display time is delivered\n
 *        at the timing of receiving the "time notification".
 *      - When an external clock is connected, set the absolute time minute (-) instruction\n
 *        with the "setting SW information notification" command to the external clock.
 * \~english @see DTime_setAutoAdjust, FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_clearMinute(HANDLE h_app);  /* Adjust the time (minute clear) */

/**
 * \ingroup DTime_setAMPM
 * \~english @par Brief
 *      - Set AM/PM.
 * \~english @param [in]  hApp        Application handle
 * \~english @param [in]  Meridiem        AM/PM setting\n
 *                                        AM : DTIME_SET_MERIDIEM_AM(TRUE)\n
 *                                        PM : DTIME_SET_MERIDIEM_PM(FALSE)
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When message asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - This API is assumed to be called by SettingService.
 *      - With this API, AM/PM of display time can be switched.
 *      - When PM is selected with the display time (Hour of T_DTimeData) set to\n
 *        AM (0 to 11 o'clock), the display time is +12h.
 *      - When AM is selected at the display time (Hour of T_DTimeData) at PM (12 to 23 o'clock),\n
 *        it makes -12h at display time.
 *      - Set the internal offset to be used when GPS correction OFF.
 *      - Note : The time offset when GPS correction ON is set by DTime_setAutoAdjust.
 *      - When this API is called, if the display time distribution has already been registered,\n
 *        the Clock service immediately issues a display time notification.
 *      - Note : However, when there is an external clock, and after receiving the time notification command,\n
 *        the display time notification is not issued immediately, and the display time is delivered\n
 *        at the timing of receiving the "time notification".
 * \~english @see DTime_setAutoAdjust, FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_setAMPM(HANDLE h_app, BOOL meridiem);  /* AM/PM setting */

/**
 * \ingroup DTime_setFormat
 * \~english @par Brief
 *      - 12h/24h sets the display format.
 * \~english @param [in]  hApp        Application handle
 * \~english @param [in]  Setting        Display setting\n
 *                                       DTIME_SETTING_FORMAT_12H(FALSE): Notation in 12H\n
 *                                       DTIME_SETTING_FORMAT_24H(TRUE): Notation in 24H
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - This API is assumed to be called by SettingService.
 *      - With this API, it is possible to switch the display time 12H notation/24H notation.
 *      - When this API is called, if the display time distribution has already been registered,\n
 *        the Clock service immediately issues a display time notification.
 * \~english @see FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_setFormat(HANDLE h_app, BOOL setting);  /* format setting */

/**
 * \ingroup DTime_setTimeZone
 * \~english @par Brief
 *      - Set the time zone.
 * \~english @param [in]  hApp        Application handle
 * \~english @param [in]  Auto        Time zone automatic setting\n
 *                                    DTIME_SETTING_TIMEZONE_AUTO (TRUE): Time zone automatic setting\n
 *                                    DTIME_SETTING_TIMEZONE_MANUAL (FALSE): Time zone manual setting
 * \~english @param [in]  Diff        Time zone time difference setting
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the time zone time time difference setting storage area is NULL. [eFrameworkunifiedStatusFail]
 *      - When the time difference(hour)specified by the argument Diff->Hour_Diff\n
 *        is greater than the time difference(hour)upper limit value(13)[eFrameworkunifiedStatusFail]
 *      - When the time difference(minutes)specified by the argument Diff->Minute_Diff\n
 *        is incorrect (When the time difference (minutes) specified by the argument\n
 *        Diff->Minute_Diff is not 0, 15, 30, 45). [eFrameworkunifiedStatusFail]
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - This API is assumed to be called by SettingService.
 *      - With this API, you can set time zone automatic switching and time zone setting.
 *      - When this API is called, if the display time distribution has already been registered,\n
 *        the Clock service immediately issues a display time notification.
 *      - Note : However, when there is an external clock, and after receiving the time notification command,\n
 *        the display time notification is not issued immediately, and the display time is delivered\n
 *        at the timing of receiving the "time notification".
 *      - Depending on the time zone automatic setting of the second argument, set the third argument as follows.
 *      - MANUAL: User's setting value.
 *      - MANUAL/AUTO Time zones update each time the time zone setting changes.
 *      - Note : When MANUAL, it is not necessary to call this API each time "TZ status notification" is received.
 * \~english @see FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_setTimeZone(HANDLE h_app, BOOL b_auto, T_TimeDiff *diff);  /* time zone setting */

/**
 * \ingroup DTime_setDST
 * \~english @par Brief
 *      - Set DST.
 * \~english @param [in]  hApp        Application handle
 * \~english @param [in]  Auto        Time zone automatic setting\n
 *                                    DTIME_SETTING_DST_AUTO (TRUE): DST automatic setting\n
 *                                    DTIME_SETTING_DST_MANUAL (FALSE): DST manual setting
 * \~english @param [in]  Setting     Time zone time difference setting
 *                                    DTIME_SETTING_DST_ON (TRUE)\n
 *                                    DTIME_SETTING_DST_OFF (FALSE)
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - This API is assumed to be called by SettingService.
 *      - With this API, you can compete for automatic switching of DST, and can set ON/OFF.
 *      - When this API is called, the Clock service immediately issues a display time notification.
 *      - Note : However, when there is an external clock, and after receiving the time notification command,\n
 *        the display time notification is not issued immediately, and the display time is delivered\n
 *        at the timing of receiving the "time notification".
 *      - Depending on the DST automatic setting of the second argument, set the third argument as follows.
 *      - MANUAL: User's setting value.
 *      - MANUAL/AUTO update each DST ON/OFF setting if there is a change.
 *      - Note : When MANUAL, it is not necessary to call this API each time "DST status notification" is received.
 * \~english @see FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_setDST(HANDLE h_app, BOOL b_auto, BOOL setting);  /* DST setting */

/**
 * \ingroup DTime_notifySwOff
 * \~english @par Brief
 *      - Notify SW OFF information.
 * \~english @param [in]  h_app        Application handle
 * \~english @retval  eFrameworkunifiedStatusOK    success
 * \~english @retval  eFrameworkunifiedStatusFail  fail(including parameter error)
 * \~english @par Prerequisite
 *      - The availability of Clock service is TRUE.
 *      - Dispatcher for application is genereted/initialized(FrameworkunifiedCreateDispatcherWithoutLoop etc.).
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the generation of the message queue handle for transmission with\n
 *        the Clock service has failed. [eFrameworkunifiedStatusFail]
 *      - When messsage asynchronous communication fails. [eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Fire and Forget
 * \~english @par Detail
 *      - This API is assumed to be called by SettingService.
 *      - With this API, you can notify SW OFF informations of image operations to Extern clock(Meter Device).
 *      - Note : Do nothing while Extern clock doesn't exist.
 *      - After calling this API,the SW OFF will not be notified immediately,it will be notified while Meter exists.
 *      - while error occurs(API return eFrameworkunifiedStatusFail),need to recall this API.\n
 *   \n
 *      - Switch ON notification should be executed among the corresponding API of operations as followed.
 *        <table>
 *        <tr><th>API name(Switch ON notification)          </th><th>OFF notification neccessary/unneccessary         </th><th>note(reason of unneccessary)</th></tr>
 *        <tr><td>DTime_loadDisplaySetting        </td><td>unneccessary              </td><td>apart from button operation</td></tr>
 *        <tr><td>DTime_getDisplaySetting         </td><td>unneccessary              </td><td>apart from button operation</td></tr>
 *        <tr><td>DTime_setGpsTime                </td><td>unneccessary              </td><td>apart from button operation</td></tr>
 *        <tr><td>DTime_registerListener_DTime    </td><td>unneccessary              </td><td>apart from button operation</td></tr>
 *        <tr><td>DTime_UnregisterListener_DTime  </td><td>unneccessary              </td><td>apart from button operation</td></tr>
 *        <tr><td>DTime_setAutoAdjust             </td><td>neccessary                </td><td></td></tr>
 *        <tr><td>DTime_increaseHour              </td><td>neccessary                </td><td></td></tr>
 *        <tr><td>DTime_decreaseHour              </td><td>neccessary                </td><td></td></tr>
 *        <tr><td>DTime_increaseMinute            </td><td>neccessary                </td><td></td></tr>
 *        <tr><td>DTime_decreaseMinute            </td><td>neccessary                </td><td></td></tr>
 *        <tr><td>DTime_clearMinute               </td><td>neccessary                </td><td></td></tr>
 *        <tr><td>DTime_setAMPM                   </td><td>unneccessary              </td><td>extern clock is unneccessary</td></tr>
 *        <tr><td>DTime_setFormat                 </td><td>neccessary                </td><td></td></tr>
 *        <tr><td>DTime_setTimeZone               </td><td>unneccessary              </td><td>there is no extern clock SW ON/OFF notification</td></tr>
 *        <tr><td>DTime_setDST                    </td><td>unneccessary              </td><td>there is no extern clock SW ON/OFF notification</td></tr>
 *        </table>
 *   \n
 * \~english @see FrameworkunifiedMcOpenSender, FrameworkunifiedSendMsg
 */
EFrameworkunifiedStatus DTime_notifySwOff(HANDLE h_app);  /* SW OFF notification */

/** @}*/  // end of clock
/** @}*/  // end of vehicle_service
/** @}*/  // end of BaseSystem

#endif  // CLOCK_CLIENT_DISPLAY_TIME_INCLUDE_VEHICLE_SERVICE_DTIME_API_H_
