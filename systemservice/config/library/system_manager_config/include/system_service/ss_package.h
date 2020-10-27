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
 * @file ss_package.h
 * @brief \~english This file contains declaration of common package values
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup system_manager
 *  @ingroup system_service
 *  @{
 */
#ifndef CONFIG_LIBRARY_SYSTEM_MANAGER_CONFIG_INCLUDE_SYSTEM_SERVICE_SS_PACKAGE_H_
#define CONFIG_LIBRARY_SYSTEM_MANAGER_CONFIG_INCLUDE_SYSTEM_SERVICE_SS_PACKAGE_H_

// _CWORD71_ package name list
#define SS_PKG_MAIN_EMMC        "MAIN_EMMC"        // define MAIN_EMMC for main_emmc
#define SS_PKG_MAIN_PRODUCT_SI  "MAIN_PRODUCT_SI"  // define MAIN_PRODUCT_SI for main_product_si
#define SS_PKG_TARGETBOARD      "TARGETBOARD"      // define TARGETBOARD for target boar
#define SS_PKG_MAIN_LPF_SI      "MAIN_LPF_SI"      // define MAIN_LPF_SI for main_lpf_si

#define SS_PKG_NAVI_GPS         _Pragma ("GCC warning \"'SS_NAVI_GPS' macro is deprecated\"") "NAVI_GPS"
#define SS_PKG_SERIES_INFO      _Pragma ("GCC warning \"'SS_PKG_SERIES_INFO' macro is deprecated\"") "SERIES_INFO"

#define MSG_PSM_REBOOT_NOTIFY   0x11               // define ps_psmshadow reboot notice id

// AGL reserved version list.
#define SS_PKG_DIAG_CPU_INFO_001 "DIAG_CPU_INFO_001"
#define SS_PKG_DIAG_CPU_INFO_002 "DIAG_CPU_INFO_002"
#define SS_PKG_DIAG_CPU_INFO_003 "DIAG_CPU_INFO_003"
#define SS_PKG_DIAG_CPU_INFO_004 "DIAG_CPU_INFO_004"
#define SS_PKG_DIAG_CPU_INFO_005 "DIAG_CPU_INFO_005"
#define SS_PKG_DIAG_CPU_INFO_006 "DIAG_CPU_INFO_006"
#define SS_PKG_DIAG_CPU_INFO_007 "DIAG_CPU_INFO_007"
#define SS_PKG_DIAG_CPU_INFO_008 "DIAG_CPU_INFO_008"
#define SS_PKG_DIAG_CPU_INFO_009 "DIAG_CPU_INFO_009"
#define SS_PKG_DIAG_CPU_INFO_010 "DIAG_CPU_INFO_010"
#define SS_PKG_DIAG_CPU_INFO_011 "DIAG_CPU_INFO_011"
#define SS_PKG_DIAG_CPU_INFO_012 "DIAG_CPU_INFO_012"
#define SS_PKG_DIAG_CPU_INFO_013 "DIAG_CPU_INFO_013"
#define SS_PKG_DIAG_CPU_INFO_014 "DIAG_CPU_INFO_014"
#define SS_PKG_DIAG_CPU_INFO_015 "DIAG_CPU_INFO_015"

#endif  // CONFIG_LIBRARY_SYSTEM_MANAGER_CONFIG_INCLUDE_SYSTEM_SERVICE_SS_PACKAGE_H_
/** @}*/
/** @}*/
/** @}*/
