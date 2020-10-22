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

/****************************************************************************
 * File name      : _sysCommon.h
 * System name    : System API
 * Subsystem name : Common system
 * Title          : Internal header for system API infrastructure management
 ****************************************************************************/
#ifndef  POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBCOMMON_H_
#define  POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBCOMMON_H_

#define  PBCOM_CONFIGFILE          "config"        /* Configuration file        */
#define  PBCOM_CONFIGSIZE_MAX      (1024)          /* Configuration file maximum size  */
#define  PBCOM_API_NORMAL          (0)             /* API processing result normal      */
#define  PBCOM_API_ERROR           (-1)            /* API processing result error      */
#define  PBCOM_UNICODE_NUL         (0x00)          /* NULL character codes      */
#define  PBCOM_UNICODE_LF          (0x0A)          /* Line feed character code      */
#define  PBCOM_UTOIMAX_NUM         (8)             /* Maximum font size    */
#define  PBCOM_CONFIG_INDEX_SCRWIDTH    "[ScreenWidth]"      /* Data information "Screen width"    */
#define  PBCOM_CONFIG_INDEX_SCRHEIGHT   "[ScreenHeight]"     /* Data information "Screen Height"    */

#endif   // POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBCOMMON_H_
