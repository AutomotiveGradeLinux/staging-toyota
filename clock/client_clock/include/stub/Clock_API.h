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
 * @file Clock_API.h
 * @brief System community functions header file.
 */

/*******************************************************************************
* $Header::                                                                   $
* $Revision::                                                                 $
*******************************************************************************/

 /******************************************************************************
 * file name            : Clock_API.h
 *  system name         : PND-A
 *  sub system name     : system common function group header file
 *  program name        :
****************************************************************************/
#ifndef CLOCK_CLIENT_CLOCK_INCLUDE_VEHICLE_SERVICE_CLOCK_API_H_
#define CLOCK_CLIENT_CLOCK_INCLUDE_VEHICLE_SERVICE_CLOCK_API_H_

#include <time.h>

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

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * \~english The time corrected by GPS.
 */
#define CLOCK_CORRECTED_TIME      (0U)

/**
 * \~english The time not corrected by GPS.
 */
#define CLOCK_NOT_CORRECTED_TIME  (1U)

/**
 * \~english The time synchronized by GPS RTC.
 */
#define CLOCK_RTC_TIME            (2U)

/**
 * \~english GPS time error.
 */
#define CLOCK_INVALID_TIME        (3U)

/**
 * \~english The max size memory for time zone.
 */
#define TIME_ZONE_MAX_SIZE        256

/**
 * \~english Return value define.
 */
typedef enum tagCLOCK_RETURN {
          CLOCK_OK,              //!< \~english Return successful
          CLOCK_ERROR,           //!< \~english Return failed
          CLOCK_ERROR_ARGUMENTS  //!< \~english Arguments error
} CLOCK_RETURN;
/*
 data type definition
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 \~english API function prototype defined
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * \ingroup Clock_getSystemTime
 * \~english @par Brief
 *      - Get the system time.
 * \~english @param [out]  time       Store system time data
 * \~english @param [out]  status     Store time data status\n
 *                                    Time status:\n
 *                                     Valid time  : CLOCK_CORRECTED_TIME\n
 *                                     Invalid time: CLOCK_INVALID_TIME\n
 *                                       NOTE:When can't communication with Clock service, return "CLOCK_INVALID_TIME"
 * \~english @retval  CLOCK_OK               success
 * \~english @retval  CLOCK_ERROR            fail
 * \~english @retval  CLOCK_ERROR_ARGUMENTS  arguments error
 * \~english @par Prerequisite
 *      - Clock service's availability is TRUE.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - The argument used for store system time data is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - The argument used for store time data status is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - Get system time error. [CLOCK_ERROR]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Detail
 *      - The API is sync(return after call end), please check it when used the API.
 *      - Call this API to get the system time and status.
 *      - NOTE:Local time can be got by calling Clock_getLocalTime which parameter is the result of this API.
 *      - The time returned by calling this API is UTC time, its range is from\n
 *        "1970-01-01 00:00:00" to "2038-01-19 03:14:07".
 *      - Clock_getSystemTimeY2K38 can be used to get time later than "2038-01-19 03:14:08".
 *      - Result : "System time and time status" to "System time and status" will be stored in arguments.
 * \~english @see Clock_getLocalTime, Clock_getSystemTimeY2K38, McOpenSender,\n
 *                McCreateInvokerName, McOpenSyncReceiver, McInvokeSync
 */
CLOCK_RETURN Clock_getSystemTime(struct timespec* time, uint8_t* status);

/**
 * \ingroup Clock_getLocalTime
 * \~english @par Brief
 *      - Get the local time.
 * \~english @param [in]   base_time       Pointer to store base time(seconds)
 * \~english @param [out]  local_time      Pointer to store local time
 * \~english @retval  CLOCK_OK               success
 * \~english @retval  CLOCK_ERROR            fail
 * \~english @retval  CLOCK_ERROR_ARGUMENTS  arguments error
 * \~english @par Prerequisite
 *      - Clock service's availability is TRUE.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - The argument used to store base time(seconds) is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - The argument used to store local time is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - Get local time failed. [CLOCK_ERROR]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Detail
 *      - The API is sync(return before call end), please check it when used the API.
 *      - Call this API to get local time.
 *      - The time returned by calling this API is UTC time, its range is from\n
 *        "1970-01-01 00:00:00" to "2038-01-19 03:14:07".
 *      - Clock_getSystemTimeY2K38 can be used to get time later than "2038-01-19 03:14:08".
 *      - Result : Local time will be stored in arguments.
 *      -          tm which is changed base on base_time will be returned when the timezone is not set.
 * \~english @see Clock_getSystemTime, Clock_getSystemTimeY2K38
 */
CLOCK_RETURN Clock_getLocalTime(const time_t* base_time, struct tm* local_time);

/**
 * \ingroup Clock_CnvSecToDate
 * \~english @par Brief
 *      - Convert specified elapsed seconds to date and time.\n
 *        The specified elapsed seconds are regarded as elapsed time from 1970/01/01 00:00:00.
 * \~english @param [in]   timep       The pointer to store the seconds
 * \~english @param [out]  result      The pointer to store date time
 * \~english @retval  CLOCK_OK               success
 * \~english @retval  CLOCK_ERROR            fail
 * \~english @retval  CLOCK_ERROR_ARGUMENTS  arguments error
 * \~english @par Prerequisite
 *      - Clock service's availability is TRUE.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - The pointer used to store the seconds from base date time is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - The pointer used to store the date time is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - Convert the seconds from base date time to date time failed. [CLOCK_ERROR]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Detail
 *      - The API is sync(return before call end), please check it when used the API.
 *      - Call this API to convert the seconds which is "1970-01-01 00:00:00" to date time.
 *      - The time returned by calling this API is UTC time, its range is from\n
 *        "1970-01-01 00:00:00" to "2038-01-19 03:14:07".
 *      - Clock_getSystemTimeY2K38 can be used to get time later than "2038-01-19 03:14:08".
 *      - Result :Convert the seconds to date time. The seconds is from "1970-01-01 00:00:00".
 * \~english @see Clock_CnvDateToSec, Clock_CnvSecToDateY2K38
 */
CLOCK_RETURN Clock_CnvSecToDate(const time_t* timep, struct tm *result);

/**
 * \ingroup Clock_CnvDateToSec
 * \~english @par Brief
 *      - Convert specified date and time to elapsed seconds from 1970/01/01 00:00:00.
 * \~english @param [in]   tm          The pointer to store date time to convert
 * \~english @param [out]  result      The pointer to store seconds from base time
 * \~english @retval  CLOCK_OK               success
 * \~english @retval  CLOCK_ERROR_ARGUMENTS  arguments error
 * \~english @par Prerequisite
 *      - Clock service's availability is TRUE.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - The pointer used to store the date time is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - The pointer used to store the seconds from base date time is NULL. [CLOCK_ERROR_ARGUMENTS]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Detail
 *      - The API is sync(return before call end), please check it when used the API.
 *      - Call this API to convert the seconds which from "1970-01-01 00:00:00" to date time.
 *      - The API is used when UTC time is in the range from "1970-01-01 00:00:00" to "2038-01-19 03:14:07".
 *      - For more then "2038-01-19 03:14:08", please use Clock_CnvDateToSecY2K38.
 *      - Result : Convert date time to seconds from "1970-01-01 00:00:00".
 * \~english @see Clock_CnvSecToDate, Clock_CnvDateToSecY2K38
 */
CLOCK_RETURN Clock_CnvDateToSec(const struct tm* tm, time_t* result);

/**
 * \ingroup Clock_setUserTime
 * \~english @par Brief
 *      - Set the display time when GPS synchronization setting is off.
 * \~english @param [in]   user_time       The pointer to store the time(second from base time)
 * \~english @retval  CLOCK_OK               success
 * \~english @retval  CLOCK_ERROR            fail
 * \~english @retval  CLOCK_ERROR_ARGUMENTS  arguments error
 * \~english @par Prerequisite
 *      - Clock service's availability is TRUE.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - The pointer used to store the time from base date time is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - Get thread name of caller failed. [CLOCK_ERROR]
 *      - Get handle of Clock service message queue failed. [CLOCK_ERROR]
 *      - Generate message queue name for sync call failed. [CLOCK_ERROR]
 *      - Generate message queue for receiving failed. [CLOCK_ERROR]
 *      - Message sync communicate failed. [CLOCK_ERROR]
 *      - Message buffer received sync communicate message is error. [CLOCK_ERROR]
 *      - Clock service handle failed. [CLOCK_ERROR]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Detail
 *      - The API is sync(return before call end), please check it when used the API.
 *      - This API called for user set time to user time when GPS corrected off.
 *      - Result : Set user time when GPS corrected off.
 * \~english @see Clock_getUserTime, Clock_addUserTimeOffset, McOpenSender,\n
 *                McCreateInvokerName, McOpenSyncReceiver, McInvokeSync
 */
CLOCK_RETURN Clock_setUserTime(const time_t* user_time);

/**
 * \ingroup Clock_getUserTime
 * \~english @par Brief
 *      - Get the display time when GPS synchronization setting is off.
 * \~english @param [out]   user_time       The pointer to store the time(second from base time)
 * \~english @param [out]   status          The pointer to store time status\n
 *                                           time status\n
 *                                           Valid time: CLOCK_CORRECTED_TIME\n
 *                                           Invalid time: CLOCK_INVALID_TIME\n
 * \~english @retval  CLOCK_OK               success
 * \~english @retval  CLOCK_ERROR            fail
 * \~english @retval  CLOCK_ERROR_ARGUMENTS  arguments error
 * \~english @par Prerequisite
 *      - Clock service's availability is TRUE.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When GPS corrected off, the pointer to store user time date is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - The pointer to store time status is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - The caller thread name get failed. [CLOCK_ERROR]
 *      - Get Clock service message queue handle failed. [CLOCK_ERROR]
 *      - Generate message queue name for sync call failed. [CLOCK_ERROR]
 *      - Generate message queue handle for sync call failed. [CLOCK_ERROR]
 *      - Message sync communicate failed. [CLOCK_ERROR]
 *      - Message buffer received sync communicate message is error. [CLOCK_ERROR]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Detail
 *      - The API is sync(return before call end), please check it when used the API.
 *      - Call the API, can get the status for user time invalid or valid, when GPS corrected off.
 *      - Note : Get local time, use Clock_getLocalTIme with arguments.\n
 *        The API used to get user time when GPS corrected off.
 *      - Result : Get user time when GPS corrected off.
 * \~english @see Clock_setUserTime, Clock_addUserTimeOffset, Clock_getLocalTime,\n
 *                 McOpenSender, McCreateInvokerName, McOpenSyncReceiver, McInvokeSync
 */
CLOCK_RETURN Clock_getUserTime(time_t* user_time, uint8_t* status);

/**
 * \ingroup Clock_addUserTimeOffset
 * \~english @par Brief
 *      - Adjust the display time when GPS synchronization setting is off.
 * \~english @param [in]   offset        The pointer to store offset for add(sub) user time.\n
 *                                       Note : Member has limit as follow. \n
 *                                       int tm_hour: hour(-12 to 12)\n
 *                                       int tm_min : minute(-59 to 59)\n
 *                                       year, month, day, second and so on not used, please set 0.
 * \~english @retval  CLOCK_OK               success
 * \~english @retval  CLOCK_ERROR            fail
 * \~english @retval  CLOCK_ERROR_ARGUMENTS  arguments error
 * \~english @par Prerequisite
 *      - Clock service's availability is TRUE.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - The pointer to store offset for add(sub) user time is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - The parameter offset->tm_sec set second not 0. [CLOCK_ERROR_ARGUMENTS]
 *      - The parameter offset->tm_mday set day not 0. [CLOCK_ERROR_ARGUMENTS]
 *      - The parameter offset->tm_mon set month not 0. [CLOCK_ERROR_ARGUMENTS]
 *      - The parameter offset->tm_year set year not 0. [CLOCK_ERROR_ARGUMENTS]
 *      - The parameter offset->tm_wday set day in week not 0. [CLOCK_ERROR_ARGUMENTS]
 *      - The parameter offset->tm_yday set day in year not 0. [CLOCK_ERROR_ARGUMENTS]
 *      - The parameter offset->tm_isdst set daylight save time not 0. [CLOCK_ERROR_ARGUMENTS]
 *      - The parameter offset->tm_hour set hour bigger then 12. [CLOCK_ERROR_ARGUMENTS]
 *      - The parameter offset->tm_hour set hour smaller then -12. [CLOCK_ERROR_ARGUMENTS]
 *      - The parameter offset->tm_min set minute bigger then 59. [CLOCK_ERROR_ARGUMENTS]
 *      - The parameter offset->tm_min set minute smaller then -59. [CLOCK_ERROR_ARGUMENTS]
 *      - The caller thread name get failed. [CLOCK_ERROR]
 *      - Get Clock service message queue handle failed. [CLOCK_ERROR]
 *      - Generate message queue name for sync call failed. [CLOCK_ERROR]
 *      - Generate message queue handle for sync call failed. [CLOCK_ERROR]
 *      - Message sync communicate failed. [CLOCK_ERROR]
 *      - Clock service handle failed. [CLOCK_ERROR]
 *      - Message buffer received sync communicate message is error. [CLOCK_ERROR]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Detail
 *      - The API is sync(return before call end), please check it when used the API.
 *      - The API used for add(sub) user time when user set GPS correct off.
 *      - Result : User time add(sub) offset when GPS correct off.
 * \~english @see Clock_setUserTime, Clock_getUserTime, McOpenSender, McCreateInvokerName,\n
 *                McOpenSyncReceiver, McInvokeSync
 */
CLOCK_RETURN Clock_addUserTimeOffset(const struct tm* offset);

/* Clock_getSystemTimeY2K38 can be used to get time later than "2038-01-19 03:14:08"(UTC). */
/**
 * \ingroup Clock_getSystemTimeY2K38
 * \~english @par Brief
 *      - Get the system time(2038 issue fixed).
 * \~english @param [out]   time       Store system time data
 * \~english @param [out]   status     Store time data status\n
 *                                    Time status:\n
 *                                     Valid time  : CLOCK_CORRECTED_TIME\n
 *                                     Invalid time: CLOCK_INVALID_TIME\n
 *                                     Note : When can't communication with Clock service, return "CLOCK_INVALID_TIME".
 * \~english @retval  CLOCK_OK               success
 * \~english @retval  CLOCK_ERROR            fail
 * \~english @retval  CLOCK_ERROR_ARGUMENTS  arguments error
 * \~english @par Prerequisite
 *      - Clock service's availability is TRUE.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the storage area of the system time data is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - When the time data status storage area is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - In case of system time acquisition failure. [CLOCK_ERROR]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Detail
 *      - The API is sync(return before call end), please check it when used the API.
 *      - Call the API, can get the status for system time invalid or valid.
 *      - Get local time, use Clock_getLocalTIme with arguments. The API used to get system time.
 *      - The API is used when UTC time is in the range from "1970-01-01 00:00:00" to "2016-01-19 03:14:07".
 *      - Result : System time and time status will be stored in arguments.
 * \~english @see Clock_getSystemTime, Clock_getLocalTimeY2K38, McOpenSender,\n
 *                McCreateInvokerName, McOpenSyncReceiver, McInvokeSync
 */
CLOCK_RETURN Clock_getSystemTimeY2K38(uint32_t* time, uint8_t* status);

/**
 * \ingroup Clock_getLocalTimeY2K38
 * \~english @par Brief
 *      - Get the local time(2038 issue fixed).
 * \~english @param [in]    base_time     Pointer to store base time(seconds)
 * \~english @param [out]   local_time    Pointer to store local time
 * \~english @retval  CLOCK_OK               success
 * \~english @retval  CLOCK_ERROR            fail
 * \~english @retval  CLOCK_ERROR_ARGUMENTS  arguments error
 * \~english @par Prerequisite
 *      - Clock service's availability is TRUE.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - The argument used to store base time(seconds) is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - The argument used to store local time is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - The argument named base_time bigger then up limit(4293400447) of fixed 2038 issue. [CLOCK_ERROR_ARGUMENTS]
 *      - Get local time failed. [CLOCK_ERROR]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Detail
 *      - The API is sync(return before call end), please check it when used the API.
 *      - Call this API to get local time.
 *      - Return local time by add time zone information to base time.
 *      - The API is used when UTC time is in the range from "1970-01-01 00:00:00" to "2106-01-19 03:14:07".
 *      - The parameter base_time bigger then up limit(4293400447), convert error.
 *      - Also, this API does not support day of the week (Member tm_wday acquired always stores -1.)\n
 *        what the caller sought from year/nomth/data information.
 *      - Result : Local time will be stored in arguments.
 *      -          If time zone not set, it will return change base_time type to tm
 * \~english @see Clock_getLocalTime, Clock_getSystemTimeY2K38
 */
CLOCK_RETURN Clock_getLocalTimeY2K38(const uint32_t* base_time, struct tm* local_time);

/**
 * \ingroup Clock_CnvSecToDateY2K38
 * \~english @par Brief
 *      - Convert specified elapsed seconds to date and time(2038 issue fixed).\n
 *        The specified elapsed seconds are regarded as elapsed time from 1970/01/01 00:00:00.
 * \~english @param [in]    time     Pointer to the storage destination
 * \~english @param [out]   result   Pointer to storage location of converted date/time
 * \~english @retval  CLOCK_OK               success
 * \~english @retval  CLOCK_ERROR            fail
 * \~english @retval  CLOCK_ERROR_ARGUMENTS  arguments error
 * \~english @par Prerequisite
 *      - Clock service's availability is TRUE.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the accumulated second storage area from the date conuting point to be converted is NULL.\n
 *        [CLOCK_ERROR_ARGUMENTS]
 *      - When the converted date/time storage area is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - When the regerence time (seconds) specified by the argument time is greater than\n
 *        the conuntermeasure upper limit value (4293400447) of the 2038 root problem. [CLOCK_ERROR_ARGUMENTS]
 *      - In case of a date/time conversion failure as a cumulative second from the specified date\n
 *        in the specified integration seconds. [CLOCK_ERROR]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Detail
 *      - The API is sync(return before call end), please check it when used the API.
 *      - Call this API to convert the specified integration seconds to the date and\n
 *        time as the integration seconds from "1970-01-01 00:00:00" at the date conuting point.
 *      - The API is used when UTC time is in the range from "1970-01-01 00:00:00" to "2106-01-19 03:14:07".
 *      - If the argument time exceeds the above upper limit(4293400447), it can not be converted properly.
 *      - Also, this API does not support day of the week (Member tm_wday acquired always stores -1.)\n
 *        what the caller sought from year/nomth/data information.
 *      - Result: Converts the specified integration seconds to the date and time\n
 *        as the integration seconds from "1970-01-01 00:00:00" at the date conuting point.
 * \~english @see Clock_CnvSecToDate, Clock_CnvDateToSecY2K38
 */
CLOCK_RETURN Clock_CnvSecToDateY2K38(const uint32_t* time, struct tm *result);

/**
 * \ingroup Clock_CnvDateToSecY2K38
 * \~english @par Brief
 *      - Convert specified date and time to elapsed seconds from 1970/01/01 00:00:00.(2038 issue fixed)
 * \~english @param [in]    tm       Pointer to the storage location of the date and time to be converted.
 * \~english @param [out]   result   Pointer to storage detination of accumulated seconds from date counting point.
 * \~english @retval  CLOCK_OK               success
 * \~english @retval  CLOCK_ERROR_ARGUMENTS  Invalide parameter
 * \~english @par Prerequisite
 *      - Clock service's availability is TRUE.
 * \~english @par Change of internal state
 *      - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *      - When the date time storage area to be converted is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - When the accumulated second storage area from the date conuting point is NULL. [CLOCK_ERROR_ARGUMENTS]
 *      - When the conversion result (result) is greater than the conuntermeasure upper\n
 *        limit value (4293400447) of the 2038 root problem. [CLOCK_ERROR_ARGUMENTS]
 * \~english @par Classification
 *      - Public
 * \~english @par Type
 *      - Sync
 * \~english @par Detail
 *      - The API is sync(return before call end), please check it when used the API.
 *      - Call this API to convert the specified time to integration seconds from "1970-01-01 00:00:00"\n
 *        (date conuting point).
 *      - The API is used when UTC time is in the range from "1970-01-01 00:00:00" to "2106-01-19 03:14:07".
 *      - The maximum value of result that can be obtained is 4293400447(sec).
 *      - Result: Converts the specified date and time to integration seconds from "1970-01-01 00:00:00"\n
 *        (date counting point).
 * \~english @see Clock_CnvDateToSec, Clock_CnvSecToDateY2K38
 */
CLOCK_RETURN Clock_CnvDateToSecY2K38(const struct tm* tm, uint32_t* result);

#ifdef  __cplusplus
}
#endif  // __cplusplus

/** @}*/  // end of clock
/** @}*/  // end of vehicle_service
/** @}*/  // end of BaseSystem

#endif  // CLOCK_CLIENT_CLOCK_INCLUDE_VEHICLE_SERVICE_CLOCK_API_H_
