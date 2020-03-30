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

#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_DATAMASTER_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_DATAMASTER_H_
/****************************************************************************
 * File name    :VehicleSens_DataMaster.h
 * System name    :_CWORD107_
 * Subsystem name  :Vehicle sensor process
 ****************************************************************************/

#include "Vehicle_API_Dummy.h"
#include "Vehicle_API_private.h"

//#include "positioning_hal.h"
#include "VehicleSens_Common.h"
#include "VehicleSens_FromMng.h"
#include "VehicleSens_SelectionItemList.h"
#include "CommonDefine.h"
#include <vehicle_service/POS_common_API.h>
#include <vehicle_service/POS_gps_API.h>
#include "Naviinfo_API.h"

/************************************************************************
*      Macro definitions                          *
************************************************************************/
/* Initial value */
#define  VEHICLE_DINIT_SP1P            0x0000
#define  VEHICLE_DINIT_SP1S            0x00
#define  VEHICLE_DINIT_SP1             0x0000
#define  VEHICLE_DINIT_ECOMODE         0x00
#define  VEHICLE_DINIT_NE1             0x0000
#define  VEHICLE_DINIT_R_ACCALL        0x00
#define  VEHICLE_DINIT_R_AC_WNG        0x00
#define  VEHICLE_DINIT_R_SW_CON        0x00
#define  VEHICLE_DINIT_R_BEEP          0x00
#define  VEHICLE_DINIT_I_RA_AUT        0x00
#define  VEHICLE_DINIT_I_RA_AC         0x00
#define  VEHICLE_DINIT_I_RA_LO         0x00
#define  VEHICLE_DINIT_I_RA_HI         0x00
#define  VEHICLE_DINIT_I_AUTO_D        0x00
#define  VEHICLE_DINIT_I_AUTO_P        0x00
#define  VEHICLE_DINIT_I_DEF           0x00
#define  VEHICLE_DINIT_I_RRDEF         0x00
#define  VEHICLE_DINIT_I_RFAUT         0x00
#define  VEHICLE_DINIT_I_REC           0x00
#define  VEHICLE_DINIT_I_FRS           0x00
#define  VEHICLE_DINIT_I_AC            0x00
#define  VEHICLE_DINIT_I_MHTR          0x00
#define  VEHICLE_DINIT_I_WIPD          0x00
#define  VEHICLE_DINIT_I_DUSY          0x00
#define  VEHICLE_DINIT_I_SWNG          0x00
#define  VEHICLE_DINIT_I_BLW_F         0x00
#define  VEHICLE_DINIT_I_OAUT_D        0x00
#define  VEHICLE_DINIT_I_OLET_D        0x00
#define  VEHICLE_DINIT_I_OAUT_P        0x00
#define  VEHICLE_DINIT_I_OLET_P        0x00
#define  VEHICLE_DINIT_ST_BMN_F        0x00
#define  VEHICLE_DINIT_I_AIRPR         0x00
#define  VEHICLE_DINIT_I_O2            0x00
#define  VEHICLE_DINIT_I_PLSM          0x00
#define  VEHICLE_DINIT_I_KAFUN         0x00
#define  VEHICLE_DINIT_I_SAFS          0x00
#define  VEHICLE_DINIT_R_ONSCRN        0x00
#define  VEHICLE_DINIT_I_HUM           0x00
#define  VEHICLE_DINIT_I_TMP_D         0x00
#define  VEHICLE_DINIT_I_TMP_P         0x00
#define  VEHICLE_DINIT_SL_ACDSP        0x00
#define  VEHICLE_DINIT_SL_DUSY         0x00
#define  VEHICLE_DINIT_SL_CIF          0x00
#define  VEHICLE_DINIT_SL_BLW_F        0x00
#define  VEHICLE_DINIT_SL_SRLVL        0x00
#define  VEHICLE_DINIT_SL_RR_IH        0x00
#define  VEHICLE_DINIT_SL_RAUTO        0x00
#define  VEHICLE_DINIT_SL_BLW_R        0x00
#define  VEHICLE_DINIT_SL_DST          0x00
#define  VEHICLE_DINIT_SL_WIPD         0x00
#define  VEHICLE_DINIT_SL_MRHT         0x00
#define  VEHICLE_DINIT_SL_AIRP         0x00
#define  VEHICLE_DINIT_SL_O2           0x00
#define  VEHICLE_DINIT_SL_PLSM         0x00
#define  VEHICLE_DINIT_SL_KAFUN        0x00
#define  VEHICLE_DINIT_SL_SWNG         0x00
#define  VEHICLE_DINIT_ST_CN_OK        0x00
#define  VEHICLE_DINIT_SL_RBLW         0x00
#define  VEHICLE_DINIT_SL_HUM          0x00
#define  VEHICLE_DINIT_R_INLVL         0x00
#define  VEHICLE_DINIT_I_INLVL         0x00
#define  VEHICLE_DINIT_I_AUT_RR        0x00
#define  VEHICLE_DINIT_I_AUT_RL        0x00
#define  VEHICLE_DINIT_I_CNT_IH        0x00
#define  VEHICLE_DINIT_I_RAUTO         0x00
#define  VEHICLE_DINIT_ST_BMN_R        0x00
#define  VEHICLE_DINIT_I_BLW_R         0x00
#define  VEHICLE_DINIT_R_RONSCR        0x00
#define  VEHICLE_DINIT_I_OLT_RR        0x00
#define  VEHICLE_DINIT_I_OLT_RL        0x00
#define  VEHICLE_DINIT_I_TMP_RR        0x00
#define  VEHICLE_DINIT_I_TMP_RL        0x00
#define  VEHICLE_DINIT_ACN_AMB         0x00
#define  VEHICLE_DINIT_AC_AMB05        0x00
#define  VEHICLE_DINIT_SOC_MON         0x00
#define  VEHICLE_DINIT_RDYIND          0x00
#define  VEHICLE_DINIT_HV_M2B          0x00
#define  VEHICLE_DINIT_HV_EP           0x00
#define  VEHICLE_DINIT_HV_EM           0x00
#define  VEHICLE_DINIT_RNRG            0x00
#define  VEHICLE_DINIT_HV_MP           0x00
#define  VEHICLE_DINIT_HV_MB           0x00
#define  VEHICLE_DINIT_HV_RG           0x00
#define  VEHICLE_DINIT_B_P             0x00
#define  VEHICLE_DINIT_B_R             0x00
#define  VEHICLE_DINIT_B_N             0x00
#define  VEHICLE_DINIT_B_D             0x00
#define  VEHICLE_DINIT_B_FC            0x0000
#define  VEHICLE_DINIT_R_DISP          0x00
#define  VEHICLE_DINIT_HVFLAG          0x00
#define  VEHICLE_DINIT_DVINF           0x00
#define  VEHICLE_DINIT_IN_FC           0x0000
#define  VEHICLE_DINIT_UNIT_0          0x00
#define  VEHICLE_DINIT_AS_SP           0x0000
#define  VEHICLE_DINIT_UNIT_1          0x00
#define  VEHICLE_DINIT_FC_SCL          0x00
#define  VEHICLE_DINIT_RANGE           0x0000
#define  VEHICLE_DINIT_UNIT_4          0x00
#define  VEHICLE_DINIT_TO_FC           0x0000
#define  VEHICLE_DINIT_UNIT_6          0x00
#define  VEHICLE_DINIT_AS_TM           0x0000
#define  VEHICLE_DINIT_ADIM_C          0x00
#define  VEHICLE_DINIT_LGCY            0x00
#define  VEHICLE_DINIT_BCTY            0x00
#define  VEHICLE_DINIT_RTRRQ           0x00
#define  VEHICLE_DINIT_DEST_BDB        0x00
#define  VEHICLE_DINIT_DS_PACK1        0x00
#define  VEHICLE_DINIT_DS_PACK2        0x00
#define  VEHICLE_DINIT_STRG_WHL        0x00
#define  VEHICLE_DINIT_TRIP_CNT        0x0000
#define  VEHICLE_DINIT_TIME_CNT        0x00000000
#define  VEHICLE_DINIT_TPM_VAR         0x00
#define  VEHICLE_DINIT_TPM_WA1         0x00
#define  VEHICLE_DINIT_TPM_TNUM        0x00
#define  VEHICLE_DINIT_TPM_REQ0        0x00
#define  VEHICLE_DINIT_TPM_TAN0        0x00
#define  VEHICLE_DINIT_TPM_FLPR        0x00
#define  VEHICLE_DINIT_TPM_FL0         0x00
#define  VEHICLE_DINIT_TPM_FRPR        0x00
#define  VEHICLE_DINIT_TPM_FR0         0x00
#define  VEHICLE_DINIT_TPM_RLPR        0x00
#define  VEHICLE_DINIT_TPM_RL0         0x00
#define  VEHICLE_DINIT_TPM_RRPR        0x00
#define  VEHICLE_DINIT_TPM_RR0         0x00
#define  VEHICLE_DINIT_TPM_SPPR        0x00
#define  VEHICLE_DINIT_TPM_SP0         0x00
#define  VEHICLE_DINIT_B_R2            0x00
#define  VEHICLE_DINIT_VSP0B           0x00
#define  VEHICLE_DINIT_EPBLOCK         0x00

#define  VEHICLE_DINIT_DESTINATION_DEST_BDB    0x00
#define  VEHICLE_DINIT_DESTINATION_DS_PACK1    0x00
#define  VEHICLE_DINIT_DESTINATION_DS_PACK2    0x00
#define  VEHICLE_DINIT_HV              0x00
#define  VEHICLE_DINIT_2WD4WD          0x00
#define  VEHICLE_DINIT_STEERING_WHEEL  0x00
#define  VEHICLE_DINIT_VB              0x00
#define  VEHICLE_DINIT_IG              0x00
#define  VEHICLE_DINIT_MIC             0x00
#define  VEHICLE_DINIT_TEST            0x00
#define  VEHICLE_DINIT_VTRADAPTER      0x00
#define  VEHICLE_DINIT_AUXADAPTER      0x00
#define  VEHICLE_DINIT_BACKDOOR        0x00
#define  VEHICLE_DINIT_PKB             0x00
#define  VEHICLE_DINIT_PKB_CAN         0x00
#define  VEHICLE_DINIT_ADIM            0x00
#define  VEHICLE_DINIT_ILL             0x00
#define  VEHICLE_DINIT_RHEOSTAT        0x00
#define  VEHICLE_DINIT_PANELTEMP       0x00
#define  VEHICLE_DINIT_SYSTEMP         0x0000
#define  VEHICLE_DINIT_SPEED_PULSE     0x0000
#define  VEHICLE_DINIT_SPEED_KMPH      0x0000
#define  VEHICLE_DINIT_GYRO            0x0000
#define  VEHICLE_DINIT_GSNS_X          0x0000
#define  VEHICLE_DINIT_GSNS_Y          0x0000
#define  VEHICLE_DINIT_REV             0x00
#define  VEHICLE_DINIT_VSC1S03_4       0x00
#define  VEHICLE_DINIT_VSC1S03_5       0x00
#define  VEHICLE_DINIT_VSC1S03_6       0x00
#define  VEHICLE_DINIT_VSC1S03_7       0x00
#define  VEHICLE_DINIT_ECO1S01_2       0x00
#define  VEHICLE_DINIT_ENG1F07_0       0x00
#define  VEHICLE_DINIT_ENG1F07_1       0x00
#define  VEHICLE_DINIT_ENG1S03_0       0x00
#define  VEHICLE_DINIT_ENG1S03_1       0x00
#define  VEHICLE_DINIT_ACN1S04_0       0x00
#define  VEHICLE_DINIT_ACN1S04_1       0x00
#define  VEHICLE_DINIT_ACN1S04_2       0x00
#define  VEHICLE_DINIT_ACN1S04_3       0x00
#define  VEHICLE_DINIT_ACN1S04_4       0x00
#define  VEHICLE_DINIT_ACN1S04_5       0x00
#define  VEHICLE_DINIT_ACN1S04_7       0x00
#define  VEHICLE_DINIT_ACN1S05_0       0x00
#define  VEHICLE_DINIT_ACN1S05_1       0x00
#define  VEHICLE_DINIT_ACN1S06_0       0x00
#define  VEHICLE_DINIT_ACN1S06_1       0x00
#define  VEHICLE_DINIT_ACN1S06_2       0x00
#define  VEHICLE_DINIT_ACN1S06_4       0x00
#define  VEHICLE_DINIT_ACN1S06_7       0x00
#define  VEHICLE_DINIT_ACN1S08_0       0x00
#define  VEHICLE_DINIT_ACN1S08_1       0x00
#define  VEHICLE_DINIT_ACN1S08_2       0x00
#define  VEHICLE_DINIT_ACN1S08_6       0x00
#define  VEHICLE_DINIT_ACN1S08_7       0x00
#define  VEHICLE_DINIT_ACN1S07_3       0x00
#define  VEHICLE_DINIT_ACN1S07_5       0x00
#define  VEHICLE_DINIT_EHV1S90_2       0x00
#define  VEHICLE_DINIT_EHV1S90_3       0x00
#define  VEHICLE_DINIT_EHV1S90_4       0x00
#define  VEHICLE_DINIT_EHV1S90_5       0x00
#define  VEHICLE_DINIT_EHV1S90_6       0x00
#define  VEHICLE_DINIT_EHV1S90_7       0x00
#define  VEHICLE_DINIT_ECT1S92_1       0x00
#define  VEHICLE_DINIT_ECT1S92_5       0x00
#define  VEHICLE_DINIT_ENG1S28_0       0x00
#define  VEHICLE_DINIT_ENG1S28_1       0x00
#define  VEHICLE_DINIT_BGM1S01_0       0x00
#define  VEHICLE_DINIT_ENG1F03_1       0x00
#define  VEHICLE_DINIT_ENG1F03_3       0x00
#define  VEHICLE_DINIT_MET1S01_5       0x00
#define  VEHICLE_DINIT_MET1S01_6       0x00
#define  VEHICLE_DINIT_MET1S01_7       0x00
#define  VEHICLE_DINIT_MET1S03_2       0x00
#define  VEHICLE_DINIT_MET1S03_3       0x00
#define  VEHICLE_DINIT_MET1S03_4       0x00
#define  VEHICLE_DINIT_MET1S04_4       0x00
#define  VEHICLE_DINIT_MET1S04_5       0x00
#define  VEHICLE_DINIT_MET1S04_6       0x00
#define  VEHICLE_DINIT_MET1S04_7       0x00
#define  VEHICLE_DINIT_MET1S05_5       0x00
#define  VEHICLE_DINIT_MET1S05_6       0x00
#define  VEHICLE_DINIT_MET1S05_7       0x00
#define  VEHICLE_DINIT_MET1S07_6       0x00
#define  VEHICLE_DINIT_MET1S07_7       0x00
#define  VEHICLE_DINIT_BDB1S01_4       0x00
#define  VEHICLE_DINIT_BDB1S01_5       0x00
#define  VEHICLE_DINIT_BDB1S03_4       0x00
#define  VEHICLE_DINIT_BDB1S08_2       0x00
#define  VEHICLE_DINIT_BDB1S08_3       0x00
#define  VEHICLE_DINIT_BDB1S08_4       0x00
#define  VEHICLE_DINIT_BDB1S08_5       0x00
#define  VEHICLE_DINIT_BDB1F03_2       0x00
#define  VEHICLE_DINIT_BDB1F03_3       0x00
#define  VEHICLE_DINIT_BDB1F03_4       0x00
#define  VEHICLE_DINIT_BDB1F03_5       0x00
#define  VEHICLE_DINIT_BDB1F03_6       0x00
#define  VEHICLE_DINIT_BDB1F03_7       0x00
#define  VEHICLE_DINIT_TPM1S02_2       0x00
#define  VEHICLE_DINIT_TPM1S02_5       0x00
#define  VEHICLE_DINIT_TPM1S03_2       0x00
#define  VEHICLE_DINIT_TPM1S03_3       0x00
#define  VEHICLE_DINIT_TPM1S03_4       0x00
#define  VEHICLE_DINIT_TPM1S03_5       0x00
#define  VEHICLE_DINIT_TPM1S03_6       0x00
#define  VEHICLE_DINIT_TPM1S03_7       0x00
#define  VEHICLE_DINIT_ENG1S92_3       0x00
#define  VEHICLE_DINIT_MMT1S52_0       0x00
#define  VEHICLE_DINIT_EPB1S01_0       0x00
#define  VEHICLE_DINIT_MINIJACK        0x00
#define  VEHICLE_DINIT_GPS_ANTENNA     0x00
#define  VEHICLE_DINIT_SNS_COUNTER     0x00
#define  VEHICLE_DINIT_GPS_COUNTER     0x00

/* ++ PastModel002 support */
#define  VEHICLE_DINIT_GPS_UBLOX_NAV_POSLLH    0x00
#define  VEHICLE_DINIT_GPS_UBLOX_NAV_STATUS    0x00
#define  VEHICLE_DINIT_GPS_UBLOX_NAV_TIMEUTC   0x00
#define  VEHICLE_DINIT_GPS_UBLOX_NAV_VELNED    0x00
#define  VEHICLE_DINIT_GPS_UBLOX_NAV_DOP       0x00
#define  VEHICLE_DINIT_GPS_UBLOX_NAV_TIMEGPS   0x00
#define  VEHICLE_DINIT_GPS_UBLOX_NAV_SVINFO    0x00
#define  VEHICLE_DINIT_GPS_UBLOX_NAV_CLOCK     0x00
#define  VEHICLE_DINIT_GPS_UBLOX_MON_HW        0x00

#define  VEHICLE_DINIT_SPEED_PULSE_FLAG      0x00
#define  VEHICLE_DINIT_GPS_INTERRUPT_FLAG    0x00

#define  VEHICLE_DINIT_GYRO_TROUBLE               0x00  /* Gyro failure status undefined(GYRO_UNFIXED) */
#define  VEHICLE_DINIT_MAIN_GPS_INTERRUPT_SIGNAL  0x00
#define  VEHICLE_DINIT_SYS_GPS_INTERRUPT_SIGNAL   0x00
#define  VEHICLE_DINIT_GYRO_CONNECT_STATUS        0x00
/* -- PastModel002 support */

/* Size of the data(byte) */
#define  VEHICLE_DSIZE_DESTINATION     0x00
#define  VEHICLE_DSIZE_HV              0x00
#define  VEHICLE_DSIZE_2WD4WD          0x00
#define  VEHICLE_DSIZE_STEERING_WHEEL  0x00
#define  VEHICLE_DSIZE_VB              0x00
#define  VEHICLE_DSIZE_IG              0x00
#define  VEHICLE_DSIZE_MIC             0x00
#define  VEHICLE_DSIZE_TEST            0x00
#define  VEHICLE_DSIZE_VTRADAPTER      0x00
#define  VEHICLE_DSIZE_AUXADAPTER      0x00
#define  VEHICLE_DSIZE_BACKDOOR        0x00
#define  VEHICLE_DSIZE_PKB             0x00
#define  VEHICLE_DSIZE_ADIM            0x00
#define  VEHICLE_DSIZE_ILL             0x00
#define  VEHICLE_DSIZE_RHEOSTAT        0x00
#define  VEHICLE_DSIZE_PANELTEMP       0x00
#define  VEHICLE_DSIZE_SYSTEMP         0x00
#define  VEHICLE_DSIZE_SPEED_PULSE     0x00
#define  VEHICLE_DSIZE_SPEED_KMPH      0x00
#define  VEHICLE_DSIZE_GYRO            0x00
#define  VEHICLE_DSIZE_GYRO_TEMP       0x00
#define  VEHICLE_DSIZE_PULSE_TIME      0x00
#define  VEHICLE_DSIZE_GSNS_X          0x00
#define  VEHICLE_DSIZE_GSNS_Y          0x00
#define  VEHICLE_DSIZE_REV             0x00
#define  VEHICLE_DSIZE_VSC1S03         0x00  /* Internal extensions */
#define  VEHICLE_DSIZE_ECO1S01         0x00
#define  VEHICLE_DSIZE_ENG1F07         0x00
#define  VEHICLE_DSIZE_ENG1S03         0x00
#define  VEHICLE_DSIZE_ACN1S04         0x00
#define  VEHICLE_DSIZE_ACN1S05         0x00
#define  VEHICLE_DSIZE_ACN1S06         0x00
#define  VEHICLE_DSIZE_ACN1S08         0x00
#define  VEHICLE_DSIZE_ACN1S07         0x00
#define  VEHICLE_DSIZE_EHV1S90         0x00
#define  VEHICLE_DSIZE_ECT1S92         0x00
#define  VEHICLE_DSIZE_ENG1S28         0x00
#define  VEHICLE_DSIZE_BGM1S01         0x00
#define  VEHICLE_DSIZE_ENG1F03         0x00
#define  VEHICLE_DSIZE_CAA1N01         0x00
#define  VEHICLE_DSIZE_MET1S01         0x00
#define  VEHICLE_DSIZE_MET1S03         0x00
#define  VEHICLE_DSIZE_MET1S04         0x00
#define  VEHICLE_DSIZE_MET1S05         0x00
#define  VEHICLE_DSIZE_MET1S07         0x00
#define  VEHICLE_DSIZE_BDB1S01         0x00
#define  VEHICLE_DSIZE_BDB1S03         0x00
#define  VEHICLE_DSIZE_BDB1S08         0x00
#define  VEHICLE_DSIZE_BDB1F03         0x00
#define  VEHICLE_DSIZE_TPM1S02         0x00
#define  VEHICLE_DSIZE_TPM1S03         0x00
#define  VEHICLE_DSIZE_ENG1S92         0x00
#define  VEHICLE_DSIZE_MMT1S52         0x00
#define  VEHICLE_DSIZE_EPB1S01         0x00
#define  VEHICLE_DSIZE_MINIJACK        0x00
#define  VEHICLE_DSIZE_GPS_ANTENNA     0x00
#define  VEHICLE_DSIZE_SNS_COUNTER     0x00
#define  VEHICLE_DSIZE_GPS_COUNTER     0x00
#define  VEHICLE_DSIZE_SIRF_BINARY     0
#define  VEHICLE_DSIZE_RTC             0x00
#define  VEHICLE_DSIZE_GPS_VERSION     0x00
#define  VEHICLE_DSIZE_SATELLITE_STATUS  0x00
#define  VEHICLE_DSIZE_LOCATION        0x00
/* ++ GPS _CWORD82_ support */
#define  VEHICLE_DSIZE_GPS__CWORD82___CWORD44_GP4  0x00
#define  VEHICLE_DSIZE_GPS__CWORD82__NMEA          0x00
#define  VEHICLE_DSIZE_GPS__CWORD82__FULLBINARY    0x00
/* -- GPS _CWORD82_ support */
#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
#define VEHICLE_DSIZE_DATA_MASTER_FST      0x00

//#define  VEHICLE_DSIZE_GYRO_FST         0x00
//#define  VEHICLE_DSIZE_SPEED_PULSE_FST  0x00
#define  VEHICLE_DSIZE_GSNSX_FST          0x00
#define  VEHICLE_DSIZE_GSNSY_FST          0x00
#define  VEHICLE_DSIZE_GYROTEMP_FST       0x00
#define  VEHICLE_DSIZE_SPEED_PULSE_FLAG_FST  0x00
#define  VEHICLE_DSIZE_REV_FST            0x00

#define  VEHICLE_DSIZE_GYRO_EXT          0x00
#define  VEHICLE_DSIZE_SPEED_PULSE_EXT   0x00
#define  VEHICLE_DSIZE_GSNS_X_EXT        0x00
#define  VEHICLE_DSIZE_GSNS_Y_EXT        0x00
#define  VEHICLE_DSIZE_SNS_COUNTER_EXT   0x00
#define  VEHICLE_DSIZE_GYRO_TEMP_EXT     0x00
#define  VEHICLE_DSIZE_REV_EXT           0x00
#define  VEHICLE_DSIZE_PULSE_TIME_EXT    0x00

#define  VEHICLE_DSIZE_GYRO_EXT_INIT         0x00  /* Gyro output extended data master registration number initial value    */
#define  VEHICLE_DSIZE_SPEED_PULSE_EXT_INIT  0x00  /* Vehicle speed expansion data master registration number initial value    */
#define  VEHICLE_DSIZE_GSNS_X_EXT_INIT       0x00  /* Initial value of G sensor (X-axis) extended data master registration number    */
#define  VEHICLE_DSIZE_GSNS_Y_EXT_INIT       0x00  /* Initial value of G sensor (Y-axis) expansion data master registration number    */
#define  VEHICLE_DSIZE_SNS_COUNTER_EXT_INIT  0x00  /* Initial value of register number of sensor counter extended data master  */
#define  VEHICLE_DSIZE_REV_EXT_INIT          0x00  /* REV extended data master registration number initial value        */
#define  VEHICLE_DSIZE_GYRO_TEMP_EXT_INIT    0x00  /* Gyro Temperature Extended Data Master Registration Number Initial Value    */
#define  VEHICLE_DSIZE_PULSE_TIME_EXT_INIT   0x00  /* Extended data master registration number initial value between pulse time    */

#define  VEHICLE_DSIZE_GYRO_TROUBLE              0x00
#define VEHICLE_DSIZE_MAIN_GPS_INTERRUPT_SIGNAL  0x00
#define  VEHICLE_DSIZE_SYS_GPS_INTERRUPT_SIGNAL  0x00
#define  VEHICLE_DSIZE_GYRO_CONNECT_STATUS       0x00

#define  VEHICLE_DKEEP_MAX            0    /* Internal data retention count      */

/* Data storage location */
#define  VEHICLE_DATA_POS_00            0x00  /* Data storage position 0 */
#define  VEHICLE_DATA_POS_01            0x00  /* Data storage position 1 */
#endif

#define  VEHICLE_DSIZE_GPS_FORMAT        0

/* ++ PastModel002 support */
/* GPS_u-blox data size(Up to 208 bytes + 1 byte of sensor counter) */
#define  VEHICLE_DSIZE_GPS_UBLOX_FORMAT      0
/* ++ PastModel002 support */

/* NMEA data size */
#define VEHICLE_DSIZE_GPS_NMEA_GGA        0    /* Positioning information(Fixed-length sentence 71 Byte) */
#define VEHICLE_DSIZE_GPS_NMEA_DGGA       0    /* Double precision GGA - Positioning information(Fixed-length sentence 75 Byte) */
#define VEHICLE_DSIZE_GPS_NMEA_VTG        0    /* Progress Direction,Velocity information(Fixed-length sentence 37 Byte) */
#define VEHICLE_DSIZE_GPS_NMEA_RMC        0    /* RMC - Minimal information(Fixed-length sentence 61 Byte) */
#define VEHICLE_DSIZE_GPS_NMEA_DRMC       0    /* Double RMC - Minimal information(Fixed-length sentence 67 Byte) */
#define VEHICLE_DSIZE_GPS_NMEA_GLL        0    /* GLL - Geographical locality information(Fixed-length sentence 44 Byte) */
#define VEHICLE_DSIZE_GPS_NMEA_DGLL       0    /* Double-precision GLL - Geographical locality information(Fixed-length sentence 50 Byte) */
/* GSA - DOP information and positioning satellite information(Fixed-length sentence 66 Byte) */
#define VEHICLE_DSIZE_GPS_NMEA_GSA        0
#define VEHICLE_DSIZE_GPS_NMEA_GSV_1      0    /* GSV - Visual satellite information(Fixed-length sentence 70 Byte) */
#define VEHICLE_DSIZE_GPS_NMEA_GSV_2      0    /* GSV - Visual satellite information(Fixed-length sentence 70 Byte) */
#define VEHICLE_DSIZE_GPS_NMEA_GSV_3      0    /* GSV - Visual satellite information(Fixed-length sentence 70 Byte) */
/* _CWORD44_,GP,3 - Visual satellite reception information */
#define VEHICLE_DSIZE_GPS_NMEA_PJRDC_GP_3    0
/* _CWORD44_,GP,4 - Receiver-specific information */
#define VEHICLE_DSIZE_GPS_NMEA_PJRDC_GP_4    0

/* FULLBINARY data size */
/* Fully customized output */
#define VEHICLE_DSIZE_GPS_FULLBINARY  0

#if CONFIG_HW_PORTSET_TYPE_C
#define  VEHICLE_DSIZE_NMEA           0
#define  VEHICLE_DSIZE_GGA            0x00
#define  VEHICLE_DSIZE_GLL            0x00
#define  VEHICLE_DSIZE_GSA            0x00
#define  VEHICLE_DSIZE_GSV            0x00
#define  VEHICLE_DSIZE_RMC            0x00
#define  VEHICLE_DSIZE_VTG            0x00
#endif

/* ++ PastModel002 support */
#define VEHICLE_DSIZE_GPS_UBLOX_HEADER_SIZE    0    /* UBX common header size */

#define VEHICLE_DSIZE_GPS_UBLOX_NAV_CLOCK      0
#define VEHICLE_DSIZE_GPS_UBLOX_NAV_DOP        0
#define VEHICLE_DSIZE_GPS_UBLOX_NAV_POSLLH     0
#define VEHICLE_DSIZE_GPS_UBLOX_NAV_STATUS     0
#define VEHICLE_DSIZE_GPS_UBLOX_NAV_SVINFO     0    /* 8 + 12*numCh(For variable,Define only the fixed part) */
#define VEHICLE_DSIZE_GPS_UBLOX_NAV_TIMEGPS    0
#define VEHICLE_DSIZE_GPS_UBLOX_NAV_TIMEUTC    0
#define VEHICLE_DSIZE_GPS_UBLOX_NAV_VELNED     0
#define VEHICLE_DSIZE_GPS_UBLOX_MON_HW         0

#define  VEHICLE_DSIZE_SPEED_PULSE_FLAG        0x00
#define  VEHICLE_DSIZE_GPS_INTERRUPT_FLAG      0x00

#define VEHICLE_DSIZE_GPS_UBLOX_NAV_SVINFO_CH_MAX     0  /* Maximum number of NAV_SVINFO channels */
#define VEHICLE_DSIZE_GPS_UBLOX_NAV_SVINFO_ALONE_MAX  0  /* NAV_SVINFO channel:Maximum Single Data Size */
/* -- PastModel002 support */

/* Data received*/
#define  VEHICLE_RCVFLAG_ON             0x00
#define  VEHICLE_RCVFLAG_OFF            0x00

/* Upper limit of number of data items stored between pulses */
#define VEHICLE_SNS_INFO_PULSE_NUM          0

/*------------------------------------------------------------------------------*
 * NMEA reception flag                               *
 -------------------------------------------------------------------------------*/
/* Not supported by UBX_Protocol */

/************************************************************************
*      Struct definitions                          *
************************************************************************/

/*********************************************************************
* TAG       : VEHICLESENS_DATA_MASTER
* ABSTRACT  : Vehicle sensor data master structure
***********************************************************************/
typedef struct {
    DID    ul_did;      /* Data ID      */
    u_int16  us_size;      /* Size of the data  */
    u_int8  uc_rcvflag;    /* Receive flag    */
    u_int8  uc_snscnt;    /* Sensor Counter  */
    u_int8  uc_data[132];  /* Vehicle sensor data  */
} VEHICLESENS_DATA_MASTER;

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
typedef struct {
    DID    ul_did;      /* Data ID      */
    u_int16  us_size;      /* Size of the data  */
    u_int8  uc_rcvflag;    /* Receive flag    */
    u_int8  uc_reserve;    /* Reserved      */
    u_int8  uc_data[8448];  /* Vehicle sensor data  */
} VEHICLESENS_DATA_MASTER_EXT;

typedef struct {
    DID    ul_did;                  /* Data ID          */
    u_int16  us_size;                  /* Size of the data      */
    u_int8  uc_rcvflag;                /* Receive flag        */
    u_int8  partition_flg;            /* Fragmented Transmission Flag      */
    u_int8  uc_data[VEHICLE_DSIZE_DATA_MASTER_FST];  /* Vehicle sensor data     */
} VEHICLESENS_DATA_MASTER_FST;

#endif

typedef struct {
    DID    ul_did;      /* Data ID      */
    u_int16  us_size;      /* Size of the data  */
    u_int8  uc_rcvflag;    /* Receive flag    */
    u_int8  uc_reserve;    /* Reserved      */
    u_int8  uc_data[VEHICLE_DSIZE_GPS_FORMAT];  /* Vehicle sensor data  */
} VEHICLESENS_DATA_MASTER_GPS_FORMAT;

typedef struct {
    DID    ul_did;      /* Data ID      */
    u_int16  us_size;      /* Size of the data  */
    u_int8  uc_rcvflag;    /* Receive flag    */
    u_int8  uc_reserve;    /* Reserved      */
    u_int8  uc_data[VEHICLE_DSIZE_SIRF_BINARY];  /* Vehicle sensor data  */
} VEHICLESENS_DATA_MASTER_GPS_BINARY;

/* ++ GPS _CWORD82_ support */
typedef struct {
    DID    ul_did;      /* Data ID      */
    u_int16  us_size;      /* Size of the data  */
    u_int8  uc_rcvflag;    /* Receive flag    */
    u_int8  uc_reserve;    /* Reserved      */
    u_int8  uc_data[VEHICLE_DSIZE_GPS__CWORD82__NMEA];  /* Vehicle sensor data  */
} VEHICLESENS_DATA_MASTER_GPS_NMEA_FORMAT;

/* -- GPS _CWORD82_ support */
#if CONFIG_HW_PORTSET_TYPE_C

typedef struct {
    DID    ul_did;              /* Data ID      */
    u_int16  us_size;              /* Size of the data  */
    u_int8  uc_rcvflag;            /* Receive flag    */
    u_int8  uc_reserve;            /* Reserved      */
    u_int8  uc_data[VEHICLE_DSIZE_NMEA];    /* Vehicle sensor data  */
} VEHICLESENS_DATA_MASTER_GPS_NMEA;
#endif

/* ++ PastModel002 support */
typedef struct {
    DID    ul_did;      /* Data ID        */
    u_int16  us_size;      /* Size of the data    */
    u_int8  uc_rcvflag;    /* Receive flag      */
    u_int8  uc_sensor_cnt;  /* Sensor Counter    */
    u_int8  uc_gpscnt_flag;  /* GPS counter flag  */
    u_int8  reserve[3];    /* reserve        */
    u_int8  uc_data[VEHICLE_DSIZE_GPS_UBLOX_FORMAT];  /* UBLOX_GPS data  */
} VEHICLESENS_DATA_MASTER_GPS_UBLOX_FORMAT;

typedef struct {
    DID    ul_did;      /* Data ID      */
    u_int16  us_size;      /* Size of the data  */
    u_int8  uc_rcvflag;    /* Receive flag    */
    u_int8  uc_reserve;    /* Reserved      */
    u_int8  uc_data;      /* Vehicle sensor data  */
} VEHICLESENS_DATA_MASTER_GYRO_TROUBLE;

typedef struct {
    DID    ul_did;      /* Data ID      */
    u_int16  us_size;      /* Size of the data  */
    u_int8  uc_rcvflag;    /* Receive flag    */
    u_int8  uc_reserve;    /* Reserved      */
    u_int8  uc_data;      /* Vehicle sensor data  */
} VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL;

typedef struct {
    DID    ul_did;      /* Data ID      */
    u_int16  us_size;      /* Size of the data  */
    u_int8  uc_rcvflag;    /* Receive flag    */
    u_int8  uc_reserve;    /* Reserved      */
    u_int8  uc_data;      /* Vehicle sensor data  */
} VEHICLESENS_DATA_MASTER_GYRO_CONNECT_STATUS;

typedef struct {
    DID    ul_did;      /* Data ID      */
    u_int16  us_size;      /* Size of the data  */
    u_int8  uc_rcvflag;    /* Receive flag    */
    u_int8  uc_sensor_cnt;  /* Sensor Counter  */
    u_int8  uc_data;      /* Vehicle sensor data  */
} VEHICLESENS_DATA_MASTER_GPS_ANTENNA_STATUS;

/* ++ PastModel002 support */

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*********************************************************************
* TAG       : VEHICLESENS_PKG_DELIVERY_TEMP_EXT
* ABSTRACT  : Structure of Vehicle Sensor Initial Delivery Management Table
***********************************************************************/
typedef struct {
    u_int16  us_gyro[10];      /* Gyro output        */
    u_int16  us_sp_pls[10];    /* Vehicle speed pulse        */
    u_int8  uc_sens_cnt;      /* Sensor Counter      */
    u_int8  uc_reserve[3];    /* reserve          */
} VEHICLESENS_PKG_DELIVERY_TEMP_TBL;


typedef struct {
    u_int16  start_point[8];                /* Sequence reference start position      */
    u_int16  end_point;                    /* Array registration completion position      */
    u_int16  data_break;                  /* All data undelivered flag    */
} VEHICLESENS_PKG_DELIVERY_TEMP_EXT;

/************************************************************************
*      External variable                          *
************************************************************************/
extern  VEHICLESENS_PKG_DELIVERY_TEMP_EXT  gstPkgTempExt;  // NOLINT(readability/nolint)
#endif

/************************************************************************
*      Function prototype                        *
************************************************************************/
void  VehicleSensInitDataMaster(void);
//void  VehicleSensSetDataMasterLineSens(const LSDRV_LSDATA *, PFUNC_DMASTER_SET_N func, BOOL);
//void  VehicleSensSetDataMasterLineSensG(const LSDRV_LSDATA_G *, PFUNC_DMASTER_SET_N func, BOOL);
//void  VehicleSensSetDataMasterGps(SENSOR_MSG_GPSDATA_DAT *, PFUNC_DMASTER_SET_N func);
void  VehicleSensGetDataMaster(DID ul_did, u_int8, VEHICLESENS_DATA_MASTER *);
//void  VehicleSensGetGpsDataMaster(DID ul_did, u_int8, SENSOR_MSG_GPSDATA_DAT *);
//void  VehicleSensSetDataMasterData(const POS_MSGINFO *, PFUNC_DMASTER_SET_N func);

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
//void  VehicleSensSetDataMasterLineSensFst(const LSDRV_LSDATA_FST *pst_data, PFUNC_DMASTER_SET_N p_data_master_set_n);
//void  VehicleSensSetDataMasterLineSensFstG(const LSDRV_MSG_LSDATA_DAT_FST *pst_data,
//        PFUNC_DMASTER_SET_N p_data_master_set_n);
void  VehicleSensGetDataMasterFst(DID ul_did, u_int8, VEHICLESENS_DATA_MASTER_FST *);
void  VehicleSensGetDataMasterExt(DID ul_did, u_int8, VEHICLESENS_DATA_MASTER_EXT *);
#endif

//void  VehicleSensSetDataMasterGyroTrouble(const LSDRV_MSG_LSDATA_DAT_GYRO_TROUBLE *pst_data,
//        PFUNC_DMASTER_SET_N p_data_master_set_n);
//void  VehicleSensSetDataMasterSysGpsInterruptSignal(const LSDRV_MSG_LSDATA_DAT_GPS_INTERRUPT_SIGNAL *pst_data,
//        PFUNC_DMASTER_SET_SHARED_MEMORY p_data_master_set_shared_memory);
//void  VehicleSensSetDataMasterMainGpsInterruptSignal(const SENSOR_MSG_GPSDATA_DAT *pst_data,
//        PFUNC_DMASTER_SET_SHARED_MEMORY p_data_master_set_shared_memory);
//void  VehicleSensSetDataMasterGyroConnectStatus(const LSDRV_MSG_LSDATA_DAT_GYRO_CONNECT_STATUS *pst_data,
//        PFUNC_DMASTER_SET_SHARED_MEMORY p_data_master_set_shared_memory);
//void  VehicleSensSetDataMasterGpsAntennaStatus(const LSDRV_MSG_LSDATA_DAT_GPS_ANTENNA_STATUS *pst_data,
//        PFUNC_DMASTER_SET_N p_data_master_set_n);

void  VehicleSensGetDataMasterGyroTrouble(DID ul_did, u_int8 uc_get_method,
        VEHICLESENS_DATA_MASTER_GYRO_TROUBLE *pst_data);
void  VehicleSensGetDataMasterSysGpsInterruptSignal(DID ul_did, u_int8 uc_get_method,
        VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL *pst_data);
void  VehicleSensGetDataMasterMainGpsInterruptSignal(DID ul_did, u_int8 uc_get_method,
        VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL *pst_data);
void  VehicleSensGetDataMasterGyroConnectStatus(DID ul_did, u_int8 uc_get_method,
        VEHICLESENS_DATA_MASTER_GYRO_CONNECT_STATUS *pst_data);
void  VehicleSensGetDataMasterGpsAntennaStatus(DID ul_did, u_int8 uc_get_method,
        VEHICLESENS_DATA_MASTER_GPS_ANTENNA_STATUS *pst_data);

void VehicleSensGetGsnsX(VEHICLESENS_DATA_MASTER *, u_int8);
void VehicleSensInitGsnsXl(void);
//u_int8 VehicleSensSetGsnsXl(const LSDRV_LSDATA *);
void VehicleSensGetGsnsXl(VEHICLESENS_DATA_MASTER *);
//u_int8 VehicleSensSetGsnsXlG(const LSDRV_LSDATA_G *);
//void VehicleSensGetGsnsXFst(VEHICLESENS_DATA_MASTER_FST *, u_int8);
void VehicleSensInitGsnsXFstl(void);
//u_int8 VehicleSensSetGsnsXFstG(const LSDRV_LSDATA_FST_GSENSOR_X *pst_data);
//void VehicleSensGetGsnsXFstl(VEHICLESENS_DATA_MASTER_FST *pst_data);

void VehicleSensGetGsnsY(VEHICLESENS_DATA_MASTER *, u_int8);
void VehicleSensInitGsnsYl(void);
//u_int8 VehicleSensSetGsnsYl(const LSDRV_LSDATA *);
void VehicleSensGetGsnsYl(VEHICLESENS_DATA_MASTER *);
//u_int8 VehicleSensSetGsnsYlG(const LSDRV_LSDATA_G *);
//void VehicleSensGetGsnsYFst(VEHICLESENS_DATA_MASTER_FST *, u_int8);
void VehicleSensInitGsnsYFstl(void);
//u_int8 VehicleSensSetGsnsYFstG(const LSDRV_LSDATA_FST_GSENSOR_Y *pst_data);
//void VehicleSensGetGsnsYFstl(VEHICLESENS_DATA_MASTER_FST *pst_data);

void VehicleSensGetGyro(VEHICLESENS_DATA_MASTER *, u_int8);
void VehicleSensInitGyrol(void);
//u_int8 VehicleSensSetGyrol(const LSDRV_LSDATA *);
//u_int8 VehicleSensSetGyrolG(const LSDRV_LSDATA_G *);
void VehicleSensGetGyrol(VEHICLESENS_DATA_MASTER *);

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
void VehicleSensGetSpeedPulseFst(VEHICLESENS_DATA_MASTER_FST *, u_int8);
void VehicleSensInitSpeedPulseFstl(void);
//u_int8 VehicleSensSetSpeedPulseFstl(const LSDRV_LSDATA_FST *);
//u_int8 VehicleSensSetSpeedPulseFstG(const LSDRV_LSDATA_FST_SPEED *);
void VehicleSensGetSpeedPulseFstl(VEHICLESENS_DATA_MASTER_FST *);

void VehicleSensGetGyroFst(VEHICLESENS_DATA_MASTER_FST *, u_int8);
void VehicleSensInitGyroFstl(void);
//u_int8 VehicleSensSetGyroFstl(const LSDRV_LSDATA_FST *);
//u_int8 VehicleSensSetGyroFstG(const LSDRV_LSDATA_FST_GYRO *);
void VehicleSensGetGyroFstl(VEHICLESENS_DATA_MASTER_FST *);

void VehicleSensGetSpeedPulseFlagFst(VEHICLESENS_DATA_MASTER_FST *, u_int8);
void VehicleSensInitSpeedPulseFlagFstl(void);
//u_int8 VehicleSensSetSpeedPulseFlagFstl(const LSDRV_LSDATA_FST *);
//u_int8 VehicleSensSetSpeedPulseFlagFstG(const LSDRV_LSDATA_FST_SPEED_PULSE_FLAG *);
void VehicleSensGetSpeedPulseFlagFstl(VEHICLESENS_DATA_MASTER_FST *);

void VehicleSensGetRevFst(VEHICLESENS_DATA_MASTER_FST *, u_int8);
void VehicleSensInitRevFstl(void);
//u_int8 VehicleSensSetRevFstl(const LSDRV_LSDATA_FST *);
//u_int8 VehicleSensSetRevFstG(const LSDRV_LSDATA_FST_REV *);
void VehicleSensGetRevFstl(VEHICLESENS_DATA_MASTER_FST *);

void VehicleSensGetGsnsXExt(VEHICLESENS_DATA_MASTER_EXT *, u_int8);
void VehicleSensInitGsnsXExtl(void);
//void VehicleSensSetGsnsXExtl(const LSDRV_LSDATA *);
//void VehicleSensSetGsnsXExtlG(const LSDRV_LSDATA_G *);
void VehicleSensGetGsnsXExtl(VEHICLESENS_DATA_MASTER_EXT *);

void VehicleSensGetGsnsYExt(VEHICLESENS_DATA_MASTER_EXT *, u_int8);
void VehicleSensInitGsnsYExtl(void);
//void VehicleSensSetGsnsYExtl(const LSDRV_LSDATA *);
//void VehicleSensSetGsnsYExtlG(const LSDRV_LSDATA_G *);
void VehicleSensGetGsnsYExtl(VEHICLESENS_DATA_MASTER_EXT *);

void VehicleSensGetGyroExt(VEHICLESENS_DATA_MASTER_EXT *, u_int8);
void VehicleSensInitGyroExtl(void);
//void VehicleSensSetGyroExtl(const LSDRV_LSDATA *);
//void VehicleSensSetGyroExtlG(const LSDRV_LSDATA_G *);
void VehicleSensGetGyroExtl(VEHICLESENS_DATA_MASTER_EXT *);

void VehicleSensGetGyroRev(VEHICLESENS_DATA_MASTER *, u_int8);
void VehicleSensInitGyroRevl(void);
//u_int8 VehicleSensSetGyroRevl(const LSDRV_LSDATA *);
//u_int8 VehicleSensSetGyroRevlG(const LSDRV_LSDATA_G *);
void VehicleSensGetGyroRevl(VEHICLESENS_DATA_MASTER *);

void VehicleSensGetSpeedPulseExt(VEHICLESENS_DATA_MASTER_EXT *, u_int8);
void VehicleSensInitSpeedPulseExtl(void);
//void VehicleSensSetSpeedPulseExtl(const LSDRV_LSDATA *);
//void VehicleSensSetSpeedPulseExtlG(const LSDRV_LSDATA_G *);
void VehicleSensGetSpeedPulseExtl(VEHICLESENS_DATA_MASTER_EXT *);

void VehicleSensGetSnsCounterExt(VEHICLESENS_DATA_MASTER_EXT *, u_int8);
void VehicleSensInitSnsCounterExtl(void);
//void VehicleSensSetSnsCounterExtl(const LSDRV_LSDATA *);
//void VehicleSensSetSnsCounterExtlG(const LSDRV_LSDATA_G *);
void VehicleSensGetSnsCounterExtl(VEHICLESENS_DATA_MASTER_EXT *);

void VehicleSensGetRevExt(VEHICLESENS_DATA_MASTER_EXT *pst_data, u_int8 uc_get_method);
void VehicleSensGetRevExtl(VEHICLESENS_DATA_MASTER_EXT *pst_data);
void VehicleSensInitRevExtl(void);
//void VehicleSensSetRevExtlG(const LSDRV_LSDATA_G *pst_data);

#endif

void VehicleSensGetSpeedKmph(VEHICLESENS_DATA_MASTER *, u_int8);
void VehicleSensInitSpeedKmphl(void);
//u_int8 VehicleSensSetSpeedKmphl(const LSDRV_LSDATA *);
//u_int8 VehicleSensSetSpeedKmphlG(const LSDRV_LSDATA_G *);
void VehicleSensGetSpeedKmphl(VEHICLESENS_DATA_MASTER *);

void VehicleSensGetGyroTemp(VEHICLESENS_DATA_MASTER *pst_data, u_int8 uc_get_method);
void VehicleSensInitGyroTempl(void);
//u_int8 VehicleSensSetGyroTemplG(const LSDRV_LSDATA_G *pst_data);
void VehicleSensGetGyroTempl(VEHICLESENS_DATA_MASTER *pst_data);

//void VehicleSensGetGyroTempFst(VEHICLESENS_DATA_MASTER_FST *pst_data, u_int8 uc_get_method);
void VehicleSensInitGyroTempFstl(void);
//u_int8 VehicleSensSetGyroTempFstG(const LSDRV_LSDATA_FST_GYRO_TEMP *pst_data);
//void VehicleSensGetGyroTempFstl(VEHICLESENS_DATA_MASTER_FST *pst_data);

//void VehicleSensGetGyroTempExt(VEHICLESENS_DATA_MASTER_EXT *pst_data, u_int8 uc_get_method);
void VehicleSensInitGyroTempExtl(void);
//void VehicleSensSetGyroTempExtlG(const LSDRV_LSDATA_G *pst_data);
//void VehicleSensGetGyroTempExtl(VEHICLESENS_DATA_MASTER_EXT *pst_data);

void VehicleSensGetPulseTime(VEHICLESENS_DATA_MASTER *pst_data, u_int8 uc_get_method);
void VehicleSensInitPulseTimel(void);
//u_int8 VehicleSensSetPulseTimelG(const LSDRV_LSDATA_G *pst_data);
void VehicleSensGetPulseTimel(VEHICLESENS_DATA_MASTER *pst_data);

//void VehicleSensGetPulseTimeExt(VEHICLESENS_DATA_MASTER_EXT *pst_data, u_int8 uc_get_method);
void VehicleSensInitPulseTimeExtl(void);
//void VehicleSensSetPulseTimeExtlG(const LSDRV_LSDATA_G *pst_data);
//void VehicleSensGetPulseTimeExtl(VEHICLESENS_DATA_MASTER_EXT *pst_data);

void VehicleSensGetRev(VEHICLESENS_DATA_MASTER *, u_int8);

void VehicleSensInitRevl(void);
//u_int8 VehicleSensSetRevl(const LSDRV_LSDATA *);
void VehicleSensGetRevl(VEHICLESENS_DATA_MASTER *);
//u_int8 VehicleSensSetRevlG(const LSDRV_LSDATA_G *pst_data);
void VehicleSensGetRevline(VEHICLESENS_DATA_MASTER *);

void VehicleSensGetSpeedPulse(VEHICLESENS_DATA_MASTER *, u_int8);
void VehicleSensInitSpeedPulsel(void);
//u_int8 VehicleSensSetSpeedPulsel(const LSDRV_LSDATA *);
//u_int8 VehicleSensSetSpeedPulselG(const LSDRV_LSDATA_G *);
void VehicleSensGetSpeedPulsel(VEHICLESENS_DATA_MASTER *);

void VehicleSensGetGpsAntenna(VEHICLESENS_DATA_MASTER *, u_int8);
void VehicleSensInitGpsAntennal(void);
//u_int8 VehicleSensSetGpsAntennal(const LSDRV_LSDATA *);
void VehicleSensGetGpsAntennal(VEHICLESENS_DATA_MASTER *);

void VehicleSensGetSnsCounter(VEHICLESENS_DATA_MASTER *, u_int8);
void VehicleSensInitSnsCounterl(void);
//u_int8 VehicleSensSetSnsCounterl(const LSDRV_LSDATA *);
//u_int8 VehicleSensSetSnsCounterlG(const LSDRV_LSDATA_G *);
void VehicleSensGetSnsCounterl(VEHICLESENS_DATA_MASTER *);

void VehicleSensInitGpsCounterg(void);
//u_int8 VehicleSensSetGpsCounterg(const SENSOR_MSG_GPSDATA_DAT *);
//void VehicleSensGetGpsCounterg(SENSOR_MSG_GPSDATA_DAT *);

/* ++ PastModel002 support */
/* U-BLOX_GPS MON-HW */
void VehicleSensInitMonHwG(void);
//u_int8 VehicleSensSetMonHwG(const SENSOR_MSG_GPSDATA_DAT *pst_data);
//void VehicleSensGetMonHwG(SENSOR_MSG_GPSDATA_DAT *pst_data);

/* U-BLOX_GPS NAV-CLOCK */
void VehicleSensInitNavClockG(void);
//u_int8 VehicleSensSetNavClockG(const SENSOR_MSG_GPSDATA_DAT *pst_data);
//void VehicleSensGetNavClockG(SENSOR_MSG_GPSDATA_DAT *pst_data);

/* U-BLOX_GPS NAV-DOP */
void VehicleSensInitNavDopG(void);
//u_int8 VehicleSensSetNavDopG(const SENSOR_MSG_GPSDATA_DAT *pst_data);
//void VehicleSensGetNavDopG(SENSOR_MSG_GPSDATA_DAT *pst_data);

/* U-BLOX_GPS NAV-POSLLH */
void VehicleSensInitNavPosllhG(void);
//u_int8 VehicleSensSetNavPosllhG(const SENSOR_MSG_GPSDATA_DAT *pst_data);
//void VehicleSensGetNavPosllhG(SENSOR_MSG_GPSDATA_DAT *pst_data);

/* U-BLOX_GPS NAV-STATUS */
void VehicleSensInitNavStatusG(void);
//u_int8 VehicleSensSetNavStatusG(const SENSOR_MSG_GPSDATA_DAT *pst_data);
//void VehicleSensGetNavStatusG(SENSOR_MSG_GPSDATA_DAT *pst_data);

/* U-BLOX_GPS NAV-SVINFO */
void VehicleSensInitNavSvInfoG(void);
//u_int8 VehicleSensSetNavSvInfoG(const SENSOR_MSG_GPSDATA_DAT *pst_data);
//void VehicleSensGetNavSvInfoG(SENSOR_MSG_GPSDATA_DAT *pst_data);

/* U-BLOX_GPS NAV-TIMEGPS */
void VehicleSensInitNavTimeGpsG(void);
//u_int8 VehicleSensSetNavTimeGpsG(const SENSOR_MSG_GPSDATA_DAT *pst_data);
//void VehicleSensGetNavTimeGpsG(SENSOR_MSG_GPSDATA_DAT *pst_data);

/* U-BLOX_GPS NAV-TIMEUTC */
void VehicleSensInitNavTimeUtcG(void);
//u_int8 VehicleSensSetNavTimeUtcG(const SENSOR_MSG_GPSDATA_DAT *pst_data);
//void VehicleSensGetNavTimeUtcG(SENSOR_MSG_GPSDATA_DAT *pst_data);

/* U-BLOX_GPS NAV-VELNED */
void VehicleSensInitNavVelnedG(void);
//u_int8 VehicleSensSetNavVelnedG(const SENSOR_MSG_GPSDATA_DAT *pst_data);
//void VehicleSensGetNavVelnedG(SENSOR_MSG_GPSDATA_DAT *pst_data);

/* DR SPEED_PULSE_FLAG */
void VehicleSensInitSpeedPulseFlag(void);
//u_int8 VehicleSensSetSpeedPulseFlag(const LSDRV_LSDATA_G *pst_data);
void VehicleSensGetSpeedPulseFlag(VEHICLESENS_DATA_MASTER *);

/* DR GPS_INTERRUPT_FLAG */
void VehicleSensInitGpsInterruptFlag(void);
//u_int8 VehicleSensSetGpsInterruptFlag(const LSDRV_LSDATA_G *pst_data);
void VehicleSensGetGpsInterruptFlag(VEHICLESENS_DATA_MASTER *);

/* GYRO_TROUBLE */
void VehicleSensInitGyroTrouble(void);
//u_int8 VehicleSensSetGyroTrouble(const LSDRV_MSG_LSDATA_DAT_GYRO_TROUBLE *pst_data);
void VehicleSensGetGyroTrouble(VEHICLESENS_DATA_MASTER_GYRO_TROUBLE *pst_data);

/* MAIN_GPS_INTERRUPT_SIGNAL */
void VehicleSensInitMainGpsInterruptSignal(void);
//u_int8 VehicleSensSetMainGpsInterruptSignal(const SENSOR_MSG_GPSDATA_DAT *pst_data);
void VehicleSensGetMainGpsInterruptSignal(VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL *pst_data);

/* SYS_GPS_INTERRUPT_SIGNAL */
void VehicleSensInitSysGpsInterruptSignal(void);
//u_int8 VehicleSensSetSysGpsInterruptSignal(const LSDRV_MSG_LSDATA_DAT_GPS_INTERRUPT_SIGNAL *pst_data);
void VehicleSensGetSysGpsInterruptSignal(VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL *pst_data);

/* GYRO_CONNECT_STATUS */
void VehicleSensInitGyroConnectStatus(void);
//u_int8 VehicleSensSetGyroConnectStatus(const LSDRV_MSG_LSDATA_DAT_GYRO_CONNECT_STATUS *pst_data);
void VehicleSensGetGyroConnectStatus(VEHICLESENS_DATA_MASTER_GYRO_CONNECT_STATUS *pst_data);

/* GPS_ANTENNA_STATUS */
void VehicleSensInitGpsAntennaStatus(void);
//u_int8 VehicleSensSetGpsAntennaStatus(const LSDRV_MSG_LSDATA_DAT_GPS_ANTENNA_STATUS *pst_data);
void VehicleSensGetGpsAntennaStatus(VEHICLESENS_DATA_MASTER_GPS_ANTENNA_STATUS *pst_data);

/* -- PastModel002 support */

/* GPS__CWORD82__FULLBINARY */
void VehicleSensInitGps_CWORD82_FullBinaryG(void);
//u_int8 VehicleSensSetGps_CWORD82_FullBinaryG(const SENSOR_MSG_GPSDATA_DAT *pst_data);
//void VehicleSensGetGps_CWORD82_FullBinaryG(SENSOR_MSG_GPSDATA_DAT *pst_data);

/* GPS__CWORD82__FULLBINARY */
void VehicleSensInitGps_CWORD82__CWORD44_Gp4G(void);
//u_int8 VehicleSensSetGps_CWORD82__CWORD44_Gp4G(const SENSOR_MSG_GPSDATA_DAT *pst_data);
//void VehicleSensGetGps_CWORD82__CWORD44_Gp4G(SENSOR_MSG_GPSDATA_DAT *pst_data);

/* GPS__CWORD82__NMEA */
void VehicleSensInitGps_CWORD82_NmeaG(void);
//u_int8 VehicleSensSetGps_CWORD82_NmeaG(const SENSOR_MSG_GPSDATA_DAT *pst_data);
//void VehicleSensGetGps_CWORD82_NmeaG(SENSOR_MSG_GPSDATA_DAT *pst_data);

/* GPS_NMEA */
void VehicleSensInitGpsNmeaG(void);
//u_int8 VehicleSensSetGpsNmeaG(const SENSOR_MSG_GPSDATA_DAT *pst_data);
//void VehicleSensGetGpsNmeaG(SENSOR_MSG_GPSDATA_DAT *pst_data);

/* LOCATION_LONLAT */
void VehicleSensGetLocationLonLat(VEHICLESENS_DATA_MASTER *pst_data, u_int8);
void VehicleSensInitLocationLonLatG(void);
//u_int8 VehicleSensSetLocationLonLatG(const SENSORLOCATION_LONLATINFO_DAT*);
void VehicleSensGetLocationLonLatG(VEHICLESENS_DATA_MASTER*);
void VehicleSensInitLocationLonLatN(void);
//u_int8 VehicleSensSetLocationLonLatN(const SENSORLOCATION_LONLATINFO_DAT*);
void VehicleSensGetLocationLonLatN(VEHICLESENS_DATA_MASTER*);
void VehicleSensGetLocationLonLatnUnitCnv(VEHICLESENS_DATA_MASTER*);

/* LOCATION_ALTITUDE */
void VehicleSensGetLocationAltitude(VEHICLESENS_DATA_MASTER *pst_data, u_int8);
void VehicleSensInitLocationAltitudeG(void);
//u_int8 VehicleSensSetLocationAltitudeG(const SENSORLOCATION_ALTITUDEINFO_DAT*);
void VehicleSensGetLocationAltitudeG(VEHICLESENS_DATA_MASTER*);
void VehicleSensInitLocationAltitudeN(void);
//u_int8 VehicleSensSetLocationAltitudeN(const SENSORLOCATION_ALTITUDEINFO_DAT*);
void VehicleSensGetLocationAltitudeN(VEHICLESENS_DATA_MASTER*);

/* MOTION_SPEED */
void VehicleSensGetMotionSpeed(VEHICLESENS_DATA_MASTER *pst_data, u_int8);
void VehicleSensInitMotionSpeedG(void);
//u_int8 VehicleSensSetMotionSpeedG(const SENSORMOTION_SPEEDINFO_DAT*);
void VehicleSensGetMotionSpeedG(VEHICLESENS_DATA_MASTER*);
void VehicleSensInitMotionSpeedN(void);
//u_int8 VehicleSensSetMotionSpeedN(const SENSORMOTION_SPEEDINFO_DAT*);
void VehicleSensGetMotionSpeedN(VEHICLESENS_DATA_MASTER*);
void VehicleSensInitMotionSpeedI(void);
//u_int8 VehicleSensSetMotionSpeedI(const SENSORMOTION_SPEEDINFO_DAT*);
void VehicleSensGetMotionSpeedI(VEHICLESENS_DATA_MASTER*);

/* MOTION_HEADING */
void VehicleSensGetMotionHeading(VEHICLESENS_DATA_MASTER *pst_data, u_int8);
void VehicleSensInitMotionHeadingG(void);
//u_int8 VehicleSensSetMotionHeadingG(const SENSORMOTION_HEADINGINFO_DAT*);
void VehicleSensGetMotionHeadingG(VEHICLESENS_DATA_MASTER*);
void VehicleSensInitMotionHeadingN(void);
//u_int8 VehicleSensSetMotionHeadingN(const SENSORMOTION_HEADINGINFO_DAT*);
void VehicleSensGetMotionHeadingN(VEHICLESENS_DATA_MASTER*);
void VehicleSensGetMotionHeadingnCnvData(VEHICLESENS_DATA_MASTER*);

/* GPS_TIME */
//void VehicleSensGetGpsTime(SENSOR_MSG_GPSDATA_DAT*, u_int8);
void VehicleSensInitGpsTimeG(void);
//u_int8 VehicleSensSetGpsTimeG(const SENSOR_MSG_GPSTIME*);
//void VehicleSensGetGpsTimeG(SENSOR_MSG_GPSDATA_DAT*);

/* GPS_TIME_RAW */
//void VehicleSensGetGpsTimeRaw(SENSOR_MSG_GPSDATA_DAT*, u_int8);
void VehicleSensInitGpsTimeRawG(void);
//u_int8 VehicleSensSetGpsTimeRawG(const SENSOR_GPSTIME_RAW*);
//void VehicleSensGetGpsTimeRawG(SENSOR_MSG_GPSDATA_DAT*);

/* GPS_WKNROLLOVER */
void VehicleSensGetWknRollover(VEHICLESENS_DATA_MASTER*, u_int8);
void VehicleSensInitWknRolloverG(void);
u_int8 VehicleSensSetWknRolloverG(const SENSOR_WKNROLLOVER*);
//void VehicleSensGetWknRolloverG(SENSOR_MSG_GPSDATA_DAT*);

/* DIAG_GPS */
void VehicleSensInitNaviinfoDiagGPSg(void);
//u_int8 VehicleSensSetNaviinfoDiagGPSg(const NAVIINFO_DIAG_GPS*);
//void VehicleSensGetNaviinfoDiagGPSg(SENSOR_MSG_GPSDATA_DAT*);

/* SETTINGTIME */
void VehicleSensGetSettingTime(VEHICLESENS_DATA_MASTER *, u_int8);
void VehicleSensInitSettingTimeclock(void);
//u_int8 VehicleSensSetSettingTimeclock(const POS_DATETIME*);
void VehicleSensGetSettingTimeclock(VEHICLESENS_DATA_MASTER*);

/* GPS_CLOCK_DRIFT */
//void VehicleSensGetGpsClockDrift(SENSOR_MSG_GPSDATA_DAT *, u_int8);
void VehicleSensInitGpsClockDriftG(void);
u_int8 VehicleSensSetGpsClockDriftG(const int32_t*);
//void VehicleSensGetGpsClockDriftG(SENSOR_MSG_GPSDATA_DAT*);

/* GPS_CLOCK_FREQ */
//void VehicleSensGetGpsClockFreq(SENSOR_MSG_GPSDATA_DAT *, u_int8);
void VehicleSensInitGpsClockFreqG(void);
u_int8 VehicleSensSetGpsClockFreqG(const uint32_t*);
//void VehicleSensGetGpsClockFreqG(SENSOR_MSG_GPSDATA_DAT*);

/* LOCATION INFORMATION (NMEA) */
void VehicleSens_GetLocationInfoNmea(VEHICLESENS_DATA_MASTER_GPS_FORMAT *, u_int8 );
void VehicleSens_InitLocationInfoNmea_n(void);
u_int8 VehicleSens_SetLocationInfoNmea_n( const POS_LOCATIONINFO_NMEA * );
void VehicleSens_GetLocationInfoNmea_n(VEHICLESENS_DATA_MASTER_GPS_FORMAT * );


#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_DATAMASTER_H_
