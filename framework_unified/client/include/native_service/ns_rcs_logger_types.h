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

/*
 * ns_rcs_logger_if.h
 */
/**
 * @file
 * @brief \~english This file provides logger type's structures
 *
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */
/** @addtogroup nsrcs
 *  @ingroup framework_unified
 *  @{
 */
#ifndef _NS_RCS_LOGGER_IF_H_  // NOLINT  (build/header_guard)
#define _NS_RCS_LOGGER_IF_H_

#include <native_service/ns_logger_if.h>

// protocol: NSRCS_SET_LOG_SETTINGS_REQ,
//       NSRCS_SET_LOG_MASK_REQ,
//       NSRCS_SET_LOG_OUT_OPT_REQ,
//       NSRCS_SET_LOG_SEVERITY_REQ
typedef struct _TNSRCS_SetLogSettingsReq {
  UI_8           m_ui8ClientId;
  TFrameworkunifiedZoneMask     m_pui32ZoneMask;
  UI_8           m_ui8OutputLogOption;
  TFrameworkunifiedLoggerSeverity m_si32Severity;
} TNSRCS_SetLogSettingsReq;

// protocol: NSRCS_SET_LOG_SETTINGS_RESP
typedef struct _TNSRCS_SetLogSettingsResp {
  UI_8           m_ui8ClientId;
  UI_8           m_ui8SetStatus;
  TFrameworkunifiedZoneMask     m_pui32ZoneMask;
  UI_8           m_ui8OutputLogOption;
  TFrameworkunifiedLoggerSeverity m_si32Severity;
} TNSRCS_SetLogSettingsResp;

// protocol: NSRCS_GET_LOG_SETTINGS_REQ
typedef struct _TNSRCS_GetLogSettingsReq {
  UI_8  m_ui8ClientId;
} TNSRCS_GetLogSettingsReq;

// protocol: NSRCS_GET_LOG_SETTINGS_RESP
typedef struct _TNSRCS_GetLogSettingsResp {
  UI_8           m_ui8ClientId;
  CHAR           m_cZoneList[BITS_IN_ZONE_MASK][ZONE_TEXT_SIZE];
  TFrameworkunifiedZoneMask     m_pui32ZoneMask;
  UI_8           m_ui8OutputLogOption;
  TFrameworkunifiedLoggerSeverity m_si32Severity;
} TNSRCS_GetLogSettingsResp;

// protocol: NSRCS_SET_LOG_MASK_RESP
typedef struct _TNSRCS_SetLogMaskResp {
  UI_8       m_ui8ClientId;
  UI_8       m_ui8SetStatus;
  TFrameworkunifiedZoneMask m_pui32ZoneMask;
} TNSRCS_SetLogMaskResp;

// protocol: NSRCS_SET_LOG_OUT_OPT_RESP
typedef struct _TNSRCS_SetLogOutOptResp {
  UI_8 m_ui8ClientId;
  UI_8 m_ui8SetStatus;
  UI_8 m_ui8OutputLogOption;
} TNSRCS_SetLogOutOptResp;

// protocol: NSRCS_SET_LOG_SEVERITY_RESP
typedef struct _TNSRCS_SetLogSeverityResp {
  UI_8  m_ui8ClientId;
  UI_8  m_ui8SetStatus;
  SI_32 m_si32Severity;
} TNSRCS_SetLogSeverityResp;

#endif /* _NS_RCS_LOGGER_IF_H_ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
