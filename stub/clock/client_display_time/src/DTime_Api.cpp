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

/*
 * Information.
 * This source code is a sample source code .
 * Implementation of the function must be performed by the vendor.
 */

#include <stub/DTime_Api.h>
#include <native_service/frameworkunified_framework_if.h>

#define DTIME_PARAM_NG      FALSE
#define DTIME_PARAM_OK      TRUE

/******************************************************************************
 *  API name : DTime_loadDisplaySetting
 *  Function : Load display setting
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_loadDisplaySetting(HANDLE h_app, T_DisplaySetting *setting ) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_getDisplaySetting
 *  Function : Get display setting
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_getDisplaySetting(HANDLE h_app, T_DisplaySetting *setting ) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_setGpsTime
 *  Function : GPS time setting
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_setGpsTime(HANDLE h_app, T_DTIME_MSG_GPSTIME *gps_time ) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_registerListener_DTime
 *  Function : Display time delivery registration
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_registerListener_DTime(HANDLE h_app, PCSTR notify_name) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_UnregisterListener_DTime
 *  Function : Release time delivery registration
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_UnregisterListener_DTime(HANDLE h_app, PCSTR notify_name) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_setAutoAdjust
 *  Function : GPS auto adjust setting
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_setAutoAdjust(HANDLE h_app, BOOL setting, T_TimeOffset *offset) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_increaseHour
 *  Function : Adjust the time (hour increaase)
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_increaseHour(HANDLE h_app) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_decreaseHour
 *  Function : Adjust the time (hour decrease)
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_decreaseHour(HANDLE h_app) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_increaseMinute
 *  Function : Adjust the time (minute increaase)
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_increaseMinute(HANDLE h_app) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_decreaseMinute
 *  Function : Adjust the time (minute decrease)
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_decreaseMinute(HANDLE h_app) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_clearMinute
 *  Function : Adjust the time (minute clear)
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_clearMinute(HANDLE h_app) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_setAMPM
 *  Function : AM/PM setting
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_setAMPM(HANDLE h_app, BOOL meridiem) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_setFormat
 *  Function : Format setting
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_setFormat(HANDLE h_app, BOOL setting ) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_setTimeZone
 *  Function : Time zone setting
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_setTimeZone(HANDLE h_app, BOOL b_auto, T_TimeDiff *diff ) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/******************************************************************************
 *  API name : DTime_setDST
 *  Function : DST setting
 *  Outline  :
 ******************************************************************************/
EFrameworkunifiedStatus  DTime_setDST(HANDLE h_app, BOOL b_auto, BOOL setting ) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}


/******************************************************************************
 *  API name : DTime_notifySwOff
 *  Function : SW OFF notification
 *  Outline  :
 *  Note     : Used to notify the SW OFF of on-screen operations.
 ******************************************************************************/
EFrameworkunifiedStatus DTime_notifySwOff(HANDLE h_app) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

/** Parameter check functions **/
BOOL CheckParamTimeZone(T_TimeDiff *diff) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DTIME_PARAM_OK;
}
