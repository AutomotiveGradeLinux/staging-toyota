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

#define SS_PKG_SPI_LOADER       "SPI_LOADER"       // define SPI_LOADER for spi_loader
#define SS_PKG_UBOOT            "UBOOT"            // define UBOOT for uboot
#define SS_PKG_SECURE_LOADER    "SECURE_LOADER"    // define SECURE_LOADER for secure_loader

#define SS_PKG_SYS_UCOM         "SYS_UCOM"         // define SYS_UCOM for sys_ucom
#define SS_PKG_CAN_UCOM         "CAN_UCOM"         // define CAN_UCOM for can_ucom
#define SS_PKG__CWORD52__UCOM       "_CWORD52__UCOM"       // define _CWORD52__UCOM for _CWORD52__ucom
#define SS_PKG__CWORD52__DATA       "_CWORD52__DATA"       // define _CWORD52__DATA for _CWORD52__data
#define SS_PKG__CWORD52__FIXEQ      "_CWORD52__FIXEQ"      // define _CWORD52__FIXEQ for _CWORD52__fixeq

#define SS_PKG_MAIN_EMMC        "MAIN_EMMC"        // define MAIN_EMMC for main_emmc
#define SS_PKG_TARGETBOARD      "TARGETBOARD"      // define TARGETBOARD for target boar

#define SS_PKG_MAIN_PRODUCT_SI  "MAIN_PRODUCT_SI"  // define MAIN_PRODUCT_SI for main_product_si
#define SS_PKG_MAIN_COMP_SI     "MAIN_COMP_SI"     // define MAIN_COMP_SI for main_comp_si
#define SS_PKG_MAIN_LPF_SI      "MAIN_LPF_SI"      // define MAIN_LPF_SI for main_lpf_si

#define SS_PKG_FACTORY_FROM     "FACTORY_FROM"     // define FACTORY_FROM for factory_from
#define SS_PKG_FACTORY_FROM2    "FACTORY_FROM2"    // define FACTORY_FROM2 for factory_from2
#define SS_PKG_FACTORY_EEPROM   "FACTORY_EEPROM"   // define FACTORY_EEPROM for factory_eeprom
#define SS_PKG_DECK             "DECK"             // define DECK for deck
#define SS_PKG_REAGION_CODE     "REAGION_CODE"     // define REAGION_CODE for reagion_code
#define SS_PKG__CWORD8__DB     "_CWORD8__DB"     // define _CWORD8__DB for _CWORD8__db
#define SS_PKG_CAPACITIVE_TP    "CAPACITIVE_TP"    // define CAPACITIVE_TP for capacitive_tp
#define SS_PKG__CWORD52___CWORD32_     "_CWORD52___CWORD32_"     // define _CWORD52___CWORD32_ for _CWORD52___CWORD32_
#define SS_PKG_RADIO_PARAMETER  "RADIO_PARAMETER"  // define RADIO_PARAMETER for radio_parameter
#define SS_PKG_VR_DATA          "VR_DATA"          // define VR_DATA for vr_data
#define SS_PKG_NAVI_GPS         "NAVI_GPS"         // define NAVI_GPS for navi_gps
#define SS_PKG_DTV_MAIN         "DTV_MAIN"         // define DTV_MAIN for dtv_main
#define SS_PKG_DTV_EEPROM       "DTV_EEPROM"       // define DTV_EEPROM for dtv_eeprom
#define SS_PKG_DTV_BOOT         "DTV_BOOT"         // define DTV_BOOT for dtv_boot
#define SS_PKG_DTV_STATION_DB   "DTV_STATION_DB"   // define DTV_STATION_DB for dtv_station_db
#define SS_PKG_XM_TYPE_ID       "XM_TYPE_ID"       // define XM_TYPE_ID for xm_type_id
#define SS_PKG_XM_HW            "XM_HW"            // define XM_HW for xm_hw
#define SS_PKG_XM_SW            "XM_SW"            // define XM_SW for xm_sw
#define SS_PKG_XM_SXI           "XM_SXI"           // define XM_SXI for xm_sxi
#define SS_PKG_XM_BB            "XM_BB"            // define XM_BB for xm_bb
#define SS_PKG_XM_HDEC          "XM_HDEC"          // define XM_HDEC for xm_hdec
#define SS_PKG_XM_RF            "XM_RF"            // define XM_RF for xm_rf
#define SS_PKG_XM_SPL           "XM_SPL"           // define XM_SPL for xm_spl
#define SS_PKG_XM_AREA          "XM_AREA"          // define XM_AREA for xm_area
#define SS_PKG_NAVI_PROGRAM     "NAVI_PROGRAM"     // define NAVI_PROGRAM for navi_program
#define SS_PKG_NAVI_MAP         "NAVI_MAP"         // define NAVI_MAP for navi_map
#define SS_PKG_PHASE_INFO       "PHASE_INFO"       // define PHASE_INFO for phase_info

#define SS_PKG_SERIES_INFO      "SERIES_INFO"
#define SS_PKG_FPGA             "FPGA"             // define FPGA for fpga
#define SS_PKG_VP_CONFIG          "VP_VEHICLEPARAMETERLIBRARY"          // define VP_VEHICLEPARAMETERLIBRARY for vp_vehicleparameterlibrary
#define SS_PKG_VP__CWORD31_        "VP__CWORD31_"        // define VP__CWORD31_ for vp__CWORD31_
#define SS_PKG_PREINSTALL_DATA  "PREINSTALL_DATA"  // define PREINSTALL_DATA for preinstall_data

#define MSG_PSM_REBOOT_NOTIFY   0x11               // define ps_psmshadow reboot notice id

#endif  // CONFIG_LIBRARY_SYSTEM_MANAGER_CONFIG_INCLUDE_SYSTEM_SERVICE_SS_PACKAGE_H_
/** @}*/
/** @}*/
/** @}*/
