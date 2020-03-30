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

/*******************************************************************************
 *    File name        :VehicleSens_SelectionItemList.cpp
 *    System name        :_CWORD107_
 *    Subsystem name    :Vehicle sensor process
 *    Program name    :Vehicle Sensor Selection Item List
 *    Module configuration    :VehicleSensInitSelectionItemList()            Vehicle sensor selection item list initialization function
 *                  :VehicleSensGetSelectionItemList()            Vehicle sensor selection item list acquisition method GET function
 *                  :VehicleSensGetSelectionItemListCanId()        Vehicle Sensor Selection Items List CANID GET Functions
 *                  :VehicleSensSetSelectionItemListCanId()        Vehicle Sensor Selection Items List CANID SET Functions
 *                  :VehicleSensCommWatchTblInit()                Disruption monitoring data management table initialization function
 *                  :VehicleSensCommWatchTblSave()                Disruption monitoring data management table storage function
 *                  :VehicleSensCommWatchTblRun()                Disruption monitoring data management table execution function
 ******************************************************************************/

//#include <positioning_hal.h>
#include <vehicle_service/positioning_base_library.h>

#include "VehicleSens_SelectionItemList.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static    VEHICLESENS_SELECTION_ITEM_LIST g_st_selection_itemlist[VEHICLESENS_SELECTION_ITEM_LIST_LEN];
static    VEHICLE_COMM_WATCH_TBL g_st_comm_watchtbl[VEHICLE_COMM_WATCHTBL_DID_NUM];

/*******************************************************************************
* MODULE    : VehicleSensInitSelectionItemList
* ABSTRACT  : Vehicle sensor selection item list initialization function
* FUNCTION  : Vehicle Sensor Selection Item List Initialization Process
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitSelectionItemList(void) {
//    u_int8    uc_get_method;

    VehicleSensCommWatchTblInit();

    /* Setting Acquisition Method (CAN/ Direct Line) for DataID in Vehicle Sensor Selection Item List.
       BackDoor, Adim, Rev sets the CAN/ direct line obtained from the FROM
    */
    memset(&g_st_selection_itemlist, 0x00, sizeof(g_st_selection_itemlist));

//    g_st_selection_itemlist[0].ul_did         = VEHICLE_DID_DESTINATION;
//    g_st_selection_itemlist[0].ul_canid         = VEHICLE_DID_BDB1S08;
//    g_st_selection_itemlist[0].uc_get_method     = VEHICLESENS_GETMETHOD_CAN;
//    g_st_selection_itemlist[1].ul_did         = VEHICLE_DID_HV;
//    g_st_selection_itemlist[1].ul_canid = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[1].uc_get_method  = VEHICLESENS_GETMETHOD_CAN;
//    g_st_selection_itemlist[2].ul_did         = VEHICLE_DID_2WD4WD;
//    g_st_selection_itemlist[2].ul_canid         = VEHICLE_DID_ENG1F03;
//    g_st_selection_itemlist[2].uc_get_method  = VEHICLESENS_GETMETHOD_CAN;
//    g_st_selection_itemlist[2].ul_did         = VEHICLE_DID_STEERING_WHEEL;
//    g_st_selection_itemlist[2].ul_canid         = VEHICLE_DID_BDB1S08;
//    g_st_selection_itemlist[2].uc_get_method  = VEHICLESENS_GETMETHOD_CAN;
//    g_st_selection_itemlist[3].ul_did         = VEHICLE_DID_VB;
//    g_st_selection_itemlist[3].uc_get_method  = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[4].ul_did         = VEHICLE_DID_IG;
//    g_st_selection_itemlist[4].uc_get_method  = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[5].ul_did         = VEHICLE_DID_MIC;
//    g_st_selection_itemlist[5].uc_get_method  = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[6].ul_did         = VEHICLE_DID_TEST;
//    g_st_selection_itemlist[6].uc_get_method  = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[7].ul_did         = VEHICLE_DID_VTRADAPTER;
//    g_st_selection_itemlist[7].uc_get_method  = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[8].ul_did         = VEHICLE_DID_AUXADAPTER;
//    g_st_selection_itemlist[8].uc_get_method  = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[9].ul_did         = VEHICLE_DID_BACKDOOR;
//    g_st_selection_itemlist[9].ul_canid         = VEHICLE_DID_BDB1S01;
//    uc_get_method = VEHICLESENS_GETMETHOD_NO_DETECTION;
//    g_st_selection_itemlist[9].uc_get_method = uc_get_method;
//    g_st_selection_itemlist[10].ul_did        = VEHICLE_DID_PKB;
//    g_st_selection_itemlist[10].ul_canid    = VEHICLE_DID_EPB1S01;
//    uc_get_method = VEHICLESENS_GETMETHOD_NO_DETECTION;
//    g_st_selection_itemlist[10].uc_get_method = uc_get_method;
//    g_st_selection_itemlist[11].ul_did         = VEHICLE_DID_ADIM;
//    g_st_selection_itemlist[11].ul_canid     = VEHICLE_DID_BDB1S01;
//    uc_get_method = VEHICLESENS_GETMETHOD_NO_DETECTION;
//    g_st_selection_itemlist[11].uc_get_method = uc_get_method;
//    g_st_selection_itemlist[12].ul_did         = VEHICLE_DID_ILL;
//    g_st_selection_itemlist[12].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[13].ul_did         = VEHICLE_DID_RHEOSTAT;
//    g_st_selection_itemlist[13].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[14].ul_did         = VEHICLE_DID_PANELTEMP;
//    g_st_selection_itemlist[14].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[15].ul_did         = VEHICLE_DID_SYSTEMP;
//    g_st_selection_itemlist[15].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[16].ul_did         = POSHAL_DID_SPEED_PULSE;
//    g_st_selection_itemlist[16].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[17].ul_did         = POSHAL_DID_SPEED_KMPH;
//    g_st_selection_itemlist[17].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[18].ul_did         = POSHAL_DID_GYRO;
//    g_st_selection_itemlist[18].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[19].ul_did         = POSHAL_DID_GSNS_X;
//    g_st_selection_itemlist[19].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[20].ul_did         = POSHAL_DID_GSNS_Y;
//    g_st_selection_itemlist[20].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[21].ul_did         = VEHICLE_DID_REV;
//    g_st_selection_itemlist[21].ul_canid     = VEHICLESENS_INVALID;
//    uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[21].uc_get_method = uc_get_method;
//    g_st_selection_itemlist[22].ul_did         = VEHICLE_DID_MINIJACK;
//    g_st_selection_itemlist[22].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[23].ul_did         = POSHAL_DID_GPS_ANTENNA;
//    g_st_selection_itemlist[23].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[24].ul_did         = POSHAL_DID_SNS_COUNTER;
//    g_st_selection_itemlist[24].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[25].ul_did         = VEHICLE_DID_GPS_COUNTER;
//    g_st_selection_itemlist[25].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[26].ul_did         = VEHICLE_DID_SIRF_BINARY;
//    g_st_selection_itemlist[26].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[27].ul_did         = VEHICLE_DID_RTC;
//    g_st_selection_itemlist[27].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[28].ul_did         = POSHAL_DID_GPS_VERSION;
//    g_st_selection_itemlist[28].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[29].ul_did         = VEHICLE_DID_SATELLITE_STATUS;
//    g_st_selection_itemlist[29].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[30].ul_did         = VEHICLE_DID_LOCATION;
//    g_st_selection_itemlist[30].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[31].ul_did         = VEHICLE_DID_BACKDOOR_LINE;
//    g_st_selection_itemlist[31].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[32].ul_did         = VEHICLE_DID_ADIM_LINE;
//    g_st_selection_itemlist[32].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[33].ul_did         = VEHICLE_DID_REV_LINE;
//    g_st_selection_itemlist[33].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[34].ul_did         = VEHICLE_DID_BACKDOOR_CAN;
//    g_st_selection_itemlist[34].ul_canid     = VEHICLE_DID_BDB1S01;
//    g_st_selection_itemlist[34].uc_get_method = VEHICLESENS_GETMETHOD_CAN;
//    g_st_selection_itemlist[35].ul_did         = VEHICLE_DID_ADIM_CAN;
//    g_st_selection_itemlist[35].ul_canid     = VEHICLE_DID_BDB1S01;
//    g_st_selection_itemlist[35].uc_get_method = VEHICLESENS_GETMETHOD_CAN;
//    g_st_selection_itemlist[36].ul_did         = VEHICLE_DID_REV_CAN;
//    g_st_selection_itemlist[36].ul_canid     = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[36].uc_get_method = VEHICLESENS_GETMETHOD_CAN;
//    /* ++ GPS _CWORD82_ support */
//    g_st_selection_itemlist[37].ul_did         = POSHAL_DID_GPS__CWORD82___CWORD44_GP4;
//    g_st_selection_itemlist[37].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[38].ul_did         = VEHICLE_DID_GPS__CWORD82__NMEA;
//    g_st_selection_itemlist[38].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[39].ul_did         = POSHAL_DID_GPS__CWORD82__FULLBINARY;
//    g_st_selection_itemlist[39].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    /* -- GPS _CWORD82_ support */
//
//#if CONFIG_SENSOR_EXT_VALID                /* Initial Sensor Response */
//    g_st_selection_itemlist[40].ul_did         = POSHAL_DID_GYRO_EXT;
//    g_st_selection_itemlist[40].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[41].ul_did         = POSHAL_DID_SPEED_PULSE_FST;
//    g_st_selection_itemlist[41].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[42].ul_did         = POSHAL_DID_GYRO_FST;
//    g_st_selection_itemlist[42].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    /* ++ PastModel002 support */
//    g_st_selection_itemlist[43].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_POSLLH;
//    g_st_selection_itemlist[43].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[44].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_STATUS;
//    g_st_selection_itemlist[44].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[45].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_TIMEUTC;
//    g_st_selection_itemlist[45].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[46].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_VELNED;
//    g_st_selection_itemlist[46].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[47].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_DOP;
//    g_st_selection_itemlist[47].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[48].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_TIMEGPS;
//    g_st_selection_itemlist[48].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[49].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_SVINFO;
//    g_st_selection_itemlist[49].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[50].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_CLOCK;
//    g_st_selection_itemlist[50].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[51].ul_did         = VEHICLE_DID_GPS_UBLOX_MON_HW;
//    g_st_selection_itemlist[51].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[52].ul_did         = POSHAL_DID_SPEED_PULSE_FLAG;
//    g_st_selection_itemlist[52].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//
//    g_st_selection_itemlist[53].ul_did         = VEHICLE_DID_GYRO_TROUBLE;
//    g_st_selection_itemlist[53].ul_canid     = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[53].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[54].ul_did         = VEHICLE_DID_SYS_GPS_INTERRUPT_SIGNAL;
//    g_st_selection_itemlist[54].ul_canid     = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[54].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[55].ul_did         = VEHICLE_DID_MAIN_GPS_INTERRUPT_SIGNAL;
//    g_st_selection_itemlist[55].ul_canid     = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[55].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[56].ul_did         = VEHICLE_DID_GYRO_CONNECT_STATUS;
//    g_st_selection_itemlist[56].ul_canid     = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[56].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    /* -- PastModel002 support */
//    g_st_selection_itemlist[57].ul_did         = POSHAL_DID_SPEED_PULSE_FLAG_FST;
//    g_st_selection_itemlist[57].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[58].ul_did         = POSHAL_DID_REV_FST;
//    g_st_selection_itemlist[58].ul_canid     = VEHICLESENS_INVALID;
//    uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[58].uc_get_method = uc_get_method;
//
//    g_st_selection_itemlist[59].ul_did         = POSHAL_DID_GPS_NMEA;
//    g_st_selection_itemlist[59].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[60].ul_did        = POSHAL_DID_GPS_TIME;
//    g_st_selection_itemlist[60].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[60].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[61].ul_did        = VEHICLE_DID_NAVIINFO_DIAG_GPS;
//    g_st_selection_itemlist[61].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[61].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[62].ul_did        = POSHAL_DID_GYRO_TEMP;
//    g_st_selection_itemlist[62].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[62].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[63].ul_did        = POSHAL_DID_GYRO_TEMP_FST;
//    g_st_selection_itemlist[63].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[63].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[64].ul_did        = POSHAL_DID_GSNS_X_FST;
//    g_st_selection_itemlist[64].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[64].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[65].ul_did        = POSHAL_DID_GSNS_Y_FST;
//    g_st_selection_itemlist[65].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[65].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//
//    g_st_selection_itemlist[66].ul_did        = VEHICLE_DID_LOCATION_LONLAT;
//    g_st_selection_itemlist[66].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[66].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[67].ul_did        = VEHICLE_DID_LOCATION_ALTITUDE;
//    g_st_selection_itemlist[67].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[67].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[68].ul_did        = VEHICLE_DID_MOTION_HEADING;
//    g_st_selection_itemlist[68].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[68].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[69].ul_did        = VEHICLE_DID_LOCATION_LONLAT_NAVI;
//    g_st_selection_itemlist[69].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[69].uc_get_method = VEHICLESENS_GETMETHOD_NAVI;
//    g_st_selection_itemlist[70].ul_did        = VEHICLE_DID_LOCATION_ALTITUDE_NAVI;
//    g_st_selection_itemlist[70].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[70].uc_get_method = VEHICLESENS_GETMETHOD_NAVI;
//    g_st_selection_itemlist[71].ul_did        = VEHICLE_DID_MOTION_HEADING_NAVI;
//    g_st_selection_itemlist[71].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[71].uc_get_method = VEHICLESENS_GETMETHOD_NAVI;
//    g_st_selection_itemlist[72].ul_did        = VEHICLE_DID_SETTINGTIME;
//    g_st_selection_itemlist[72].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[72].uc_get_method = VEHICLESENS_GETMETHOD_OTHER;
//    g_st_selection_itemlist[73].ul_did        = VEHICLE_DID_MOTION_SPEED;
//    g_st_selection_itemlist[73].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[73].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[74].ul_did        = VEHICLE_DID_MOTION_SPEED_NAVI;
//    g_st_selection_itemlist[74].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[74].uc_get_method = VEHICLESENS_GETMETHOD_NAVI;
//    g_st_selection_itemlist[75].ul_did        = VEHICLE_DID_MOTION_SPEED_INTERNAL;
//    g_st_selection_itemlist[75].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[75].uc_get_method = VEHICLESENS_GETMETHOD_INTERNAL;
//    g_st_selection_itemlist[76].ul_did        = POSHAL_DID_PULSE_TIME;
//    g_st_selection_itemlist[76].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[76].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[77].ul_did        = POSHAL_DID_GPS_TIME_RAW;
//    g_st_selection_itemlist[77].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[77].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[78].ul_did        = POSHAL_DID_GPS_WKNROLLOVER;
//    g_st_selection_itemlist[78].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[78].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[79].ul_did        = POSHAL_DID_GPS_CLOCK_DRIFT;
//    g_st_selection_itemlist[79].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[79].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[80].ul_did        = POSHAL_DID_GPS_CLOCK_FREQ;
//    g_st_selection_itemlist[80].ul_canid    = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[80].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//
//#else
//    g_st_selection_itemlist[40].ul_did         = VEHICLE_DID_GGA;
//    g_st_selection_itemlist[40].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[41].ul_did         = VEHICLE_DID_GLL;
//    g_st_selection_itemlist[41].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[42].ul_did         = VEHICLE_DID_GSA;
//    g_st_selection_itemlist[42].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[43].ul_did         = VEHICLE_DID_GSV;
//    g_st_selection_itemlist[43].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[44].ul_did         = VEHICLE_DID_RMC;
//    g_st_selection_itemlist[44].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[45].ul_did         = VEHICLE_DID_VTG;
//    g_st_selection_itemlist[45].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    /* ++ PastModel002 support */
//    g_st_selection_itemlist[46].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_POSLLH;
//    g_st_selection_itemlist[46].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[47].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_STATUS;
//    g_st_selection_itemlist[47].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[48].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_TIMEUTC;
//    g_st_selection_itemlist[48].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[49].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_VELNED;
//    g_st_selection_itemlist[49].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[50].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_DOP;
//    g_st_selection_itemlist[50].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[51].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_TIMEGPS;
//    g_st_selection_itemlist[51].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[52].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_SVINFO;
//    g_st_selection_itemlist[52].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[53].ul_did         = VEHICLE_DID_GPS_UBLOX_NAV_CLOCK;
//    g_st_selection_itemlist[53].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[54].ul_did         = VEHICLE_DID_GPS_UBLOX_MON_HW;
//    g_st_selection_itemlist[54].uc_get_method = VEHICLESENS_GETMETHOD_GPS;
//    g_st_selection_itemlist[55].ul_did         = POSHAL_DID_SPEED_PULSE_FLAG;
//    g_st_selection_itemlist[55].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//
//    g_st_selection_itemlist[56].ul_did         = VEHICLE_DID_GYRO_TROUBLE;
//    g_st_selection_itemlist[56].ul_canid     = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[56].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[57].ul_did         = VEHICLE_DID__CWORD56__GPS_INTERRUPT_SIGNAL;
//    g_st_selection_itemlist[57].ul_canid     = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[57].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[58].ul_did         = VEHICLE_DID__CWORD102__GPS_INTERRUPT_SIGNAL;
//    g_st_selection_itemlist[58].ul_canid     = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[58].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//    g_st_selection_itemlist[59].ul_did         = VEHICLE_DID_GYRO_CONNECT_STATUS;
//    g_st_selection_itemlist[59].ul_canid     = VEHICLESENS_INVALID;
//    g_st_selection_itemlist[59].uc_get_method = VEHICLESENS_GETMETHOD_LINE;
//
//    /* -- PastModel002 support */
//#endif
}

/*******************************************************************************
* MODULE    : VehicleSensGetSelectionItemList
* ABSTRACT  : Vehicle sensor_data acquisition method GET function
* FUNCTION  : Provide data acquisition methods
* ARGUMENT  : ul_did    Data ID
* NOTE      :
* RETURN    : VEHICLESENS_GETMETHOD_CAN            :CAN line
*             VEHICLESENS_GETMETHOD_LINE            :Direct Line
*             VEHICLESENS_GETMETHOD_NO_DETECTION    :Not downloaded
*             VEHICLESENS_GETMETHOD_GPS            :GPS
*             VEHICLESENS_GETMETHOD_NAVI            :Navi
*             VEHICLESENS_GETMETHOD_CLOCK            :Clock
*             VEHICLESENS_GETMETHOD_OTHER       :Others
******************************************************************************/
u_int8  VehicleSensGetSelectionItemList(DID ul_did) {
    int32 i;
    u_int8 uc_get_method = VEHICLESENS_GETMETHOD_NO_DETECTION;
    /* Ignore->MISRA-C++:2008 Rule 2-13-3 */    /* Ignore->MISRA-C++:2008 Rule 5-0-13, 5-0-14, 5-3-1 */
    if ( (ul_did & VEHICLESENS_BIT31_29) != 0 ) {
        /* For other than CAN frame data */
        for (i = 0; i < VEHICLESENS_SELECTION_ITEM_LIST_LEN; i++) {
            if (g_st_selection_itemlist[i].ul_did == ul_did) {
                uc_get_method = g_st_selection_itemlist[i].uc_get_method;
                break;
            }
        }
    } else {
        /* CAN frame data */
        uc_get_method = VEHICLESENS_GETMETHOD_CAN;
    }
    return uc_get_method;
}

/*******************************************************************************
* MODULE    : VehicleSensGetSelectionItemListCanId
* ABSTRACT  : Vehicle Sensor Selection Item List_CANID GET Function
* FUNCTION  : Provide CANID
* ARGUMENT  :
* NOTE      :
* RETURN    : ul_canid    CANID
******************************************************************************/
u_int32  VehicleSensGetSelectionItemListCanId(DID ul_did) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32 i;
    u_int32 ul_canid = VEHICLESENS_INVALID;

    for (i = 0; i < VEHICLESENS_SELECTION_ITEM_LIST_LEN; i++) {
        if (g_st_selection_itemlist[i].ul_did == ul_did) {
            if (VEHICLESENS_GETMETHOD_CAN == g_st_selection_itemlist[i].uc_get_method) {
                /* When the data source type is CAN communication */
                ul_canid = g_st_selection_itemlist[i].ul_canid;
            }
            break;
        }
    }
    return ul_canid;
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensSetSelectionItemListCanId
* ABSTRACT  : Vehicle Sensor Selection Item List_CANID SET Function
* FUNCTION  : Sets when the CANID associated with the specified DID is unconfirmed.
* ARGUMENT  : ul_canid    u-int32(CANID)
* NOTE      :
* RETURN    : TRUE        :Successful registration(Including when the CANID is fixed)
*           : FALSE    :Registration failure
******************************************************************************/
BOOL VehicleSensSetSelectionItemListCanId(DID ul_did, u_int32 ul_canid) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BOOL    b_ret     = TRUE;
    u_int8    uc_cnt    = 0;
    int32    uc_last_cnt = 0;

    u_int8    uc_effective_flg = VEHICLESENS_EFFECTIVE;

    for (uc_cnt = 0; uc_cnt < VEHICLE_COMM_WATCHTBL_DID_NUM; uc_cnt++) {
        if (g_st_comm_watchtbl[uc_cnt].ul_did == ul_did) {
            /* Obtain CANID determination flg */
            uc_effective_flg = g_st_comm_watchtbl[uc_cnt].uc_effective_flg;
            break;
        }
    }

    if (uc_cnt >= VEHICLE_COMM_WATCHTBL_DID_NUM) {
        return FALSE;
    }

    if (VEHICLESENS_EFFECTIVE != uc_effective_flg) {
        /* When the CANID is undetermined */
        b_ret = FALSE;
        for (uc_last_cnt = 0; uc_last_cnt < VEHICLESENS_SELECTION_ITEM_LIST_LEN; uc_last_cnt++) {
            if (g_st_selection_itemlist[uc_last_cnt].ul_did == ul_did) {
                /* Updating the CANID of the Vehicle Sensor Selection Items List */
                g_st_selection_itemlist[uc_last_cnt].ul_canid = ul_canid;
                /* To fix the CANID */
                g_st_comm_watchtbl[uc_cnt].uc_effective_flg = VEHICLESENS_EFFECTIVE;

                /* During CANID indoubt,When Vehicle API ""Vehicle Sensor Information Disruption Monitoring"" is called     */
                /* Register for disruption monitoring of pending CAN threads                                */
                if (0x00 < g_st_comm_watchtbl[uc_cnt].uc_vehicle_comm_watch_cnt) {
                    VehicleSensCommWatchTblRun(ul_did);
                }
                b_ret = TRUE;
                break;
            }
        }
    }
    return b_ret;
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensCommWatchTblInit
* ABSTRACT  : Disruption monitoring data management table initialization function
* FUNCTION  : Disruption monitoring data management table initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensCommWatchTblInit(void) {
    memset(&g_st_comm_watchtbl, 0x00, sizeof(g_st_comm_watchtbl));

    /* DID initialization */
//    g_st_comm_watchtbl[0].ul_did    = VEHICLE_DID_REV;
//    g_st_comm_watchtbl[1].ul_did    = VEHICLE_DID_REV_CAN;
}

/*******************************************************************************
* MODULE    : VehicleSensCommWatchTblSave
* ABSTRACT  : Disruption monitoring data management table storage function
* FUNCTION  : When the target CANID is undetermined, save the discontinuation monitoring data...
* ARGUMENT  :
* NOTE      :
* RETURN    : TRUE        : To fix the CANID
*           : FALSE    : CANID undetermined
******************************************************************************/
BOOL VehicleSensCommWatchTblSave(const VEHICLE_MSG_WATCH_STOPPAGE *pst_msg) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BOOL    b_ret             = TRUE;                            /* Function return value                        */
    u_int8    uc_cnt            = 0;                            /* Generic counters                        */
    u_int8    uc_last_cnt        = 0;                            /* Vehicle Sensor Information Disruption Monitoring Request Count    */
    u_int8    uc_get_method     = VEHICLESENS_GETMETHOD_CAN;    /* Acquisition method                         */
    u_int8    uc_effective_flg    = VEHICLESENS_EFFECTIVE;        /* CANID determination flg                        */


    uc_get_method = VehicleSensGetSelectionItemList(pst_msg->st_data.ul_did);

    /* If the retrieval method is CAN: Check if the CANID is fixed */
    if (VEHICLESENS_GETMETHOD_CAN == uc_get_method) {
        for ( uc_cnt = 0; uc_cnt < VEHICLE_COMM_WATCHTBL_DID_NUM; uc_cnt++ ) {
            if ( g_st_comm_watchtbl[uc_cnt].ul_did == pst_msg->st_data.ul_did ) {
                /* Obtain CANID determination flg */
                uc_effective_flg = g_st_comm_watchtbl[uc_cnt].uc_effective_flg;
                break;
            }
        }
    }

    if ( VEHICLESENS_EFFECTIVE != uc_effective_flg ) {
        /* Due to being asked for disruption monitoring of CANID indoubt data,Keep parameters required for disruption monitoring    */
        /* Returns success to the API user,No Disruption Monitoring Registration at this time                        */
        /* Ask the CAN thread to monitor for disruption when the CANID is fixed. */

        uc_last_cnt = g_st_comm_watchtbl[uc_cnt].uc_vehicle_comm_watch_cnt;
        if ( VEHICLE_COMM_WATCHTBL_DAT_NUM > uc_last_cnt ) {
            /* Destination PNO,Keep Disrupted Monitoring Time */
            g_st_comm_watchtbl[uc_cnt].st_comm_watch_dat[uc_last_cnt].us_pno        = pst_msg->st_data.us_pno;
            g_st_comm_watchtbl[uc_cnt].st_comm_watch_dat[uc_last_cnt].us_watch_time    = pst_msg->st_data.us_watch_time;

            /* Vehicle sensor information disruption monitoring request count is incremented. */
            uc_last_cnt++;
            g_st_comm_watchtbl[uc_cnt].uc_vehicle_comm_watch_cnt = uc_last_cnt;
        }
        b_ret = FALSE;
    }
    return b_ret;
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensCommWatchTblRun
* ABSTRACT  : Disruption monitoring data management table execution function
* FUNCTION  : Execute requested disruption monitoring when CANID is unconfirmed
* ARGUMENT  : ul_did    DID
* NOTE      :
* RETURN    : TRUE        : Normal completion
*           : FALSE    : ABENDs
******************************************************************************/
BOOL VehicleSensCommWatchTblRun(DID ul_did) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BOOL        b_ret        = TRUE;

    return b_ret;
}
// LCOV_EXCL_STOP
