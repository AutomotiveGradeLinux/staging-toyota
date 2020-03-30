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
 *  File name    :VehicleSens_DataMasterMain.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master
 *  Module configuration  :VehicleSensInitDataMaster()      Vehicle data master initialization function
 *          :VehicleSensSetDataMasterCan()     Vehicle sensor data master CAN data set processing
 *          :VehicleSensSetDataMasterLineSens()  Vehicle sensor data master LineSensor data set process
 *          :VehicleSensSetDataMasterGps()    Vehicle Sensor Data Master GPS Data Set Processing
 *          :VehicleSensGetDataMaster()      Vehicle Sensor Data Master Get Processing
 *          :VehicleSensGetGpsDataMaster()    Vehicle Sensor Data Master GPS Data Get Processing
 *                  :VehicleSensGetGps_CWORD82_NmeaSensorCnt()  Vehicle sensor GPS_sensor counter GET function
 *                  :VehicleSensSetDataMaster_CWORD82_()    Vehicle sensor data master GPS data (_CWORD82_) set processing
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"
//#include "gps_hal.h"
#include "POS_private.h"
#include "DeadReckoning_main.h"
#include "VehicleSens_DeliveryCtrl.h"
#include "Vehicle_API.h"
#include "CommonDefine.h"
#include <vehicle_service/POS_common_API.h>
#include "SensorMotion_API.h"
#include "SensorLog.h"
#include "ClockIf.h"
#include "DiagSrvIf.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
VEHICLESENS_PKG_DELIVERY_TEMP_EXT  gstPkgTempExt;  // NOLINT(readability/nolint)
#endif

#define DR_DEBUG 0
#define GPS_DEBUG 0

/*************************************************/
/*           Function prototype                    */
/*************************************************/
//static uint8_t VehicleSensGetSensorCnt(void);


/*******************************************************************************
* MODULE    : VehicleSensInitDataMaster
* ABSTRACT  : Vehicle sensor data master initialization
* FUNCTION  : Initializing Vehicle Sensor Data Master
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void  VehicleSensInitDataMaster(void) {
    /* Vehicle sensor data master initialization */

    VehicleSensInitSpeedPulsel();
    VehicleSensInitSpeedKmphl();
    VehicleSensInitGyrol();
    VehicleSensInitGsnsXl();
    VehicleSensInitGsnsYl();
    VehicleSensInitRevl();
    VehicleSensInitGpsAntennal();
    VehicleSensInitSnsCounterl();
    VehicleSensInitGpsCounterg();
#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
    VehicleSensInitGyroRevl();
    VehicleSensInitSnsCounterExtl();
    VehicleSensInitGyroExtl();
    VehicleSensInitSpeedPulseExtl();
    VehicleSensInitRevExtl();
    VehicleSensInitGsnsXExtl();
    VehicleSensInitGsnsYExtl();
    VehicleSensInitGsnsXFstl();
    VehicleSensInitGsnsYFstl();
    VehicleSensInitGyroFstl();
    VehicleSensInitSpeedPulseFstl();
    VehicleSensInitSpeedPulseFlagFstl();
    VehicleSensInitRevFstl();
#endif
    /* ++ GPS _CWORD82_ support */
    VehicleSensInitGps_CWORD82_FullBinaryG();
    VehicleSensInitGps_CWORD82_NmeaG();
    VehicleSensInitGps_CWORD82__CWORD44_Gp4G();
    VehicleSensInitGpsNmeaG();
    /* -- GPS _CWORD82_ support */

    /* ++ PastModel002 support */
    VehicleSensInitNavVelnedG();
    VehicleSensInitNavTimeUtcG();
    VehicleSensInitNavTimeGpsG();
    VehicleSensInitNavSvInfoG();
    VehicleSensInitNavStatusG();
    VehicleSensInitNavPosllhG();
    VehicleSensInitNavClockG();
    VehicleSensInitNavDopG();
    VehicleSensInitMonHwG();

    VehicleSensInitSpeedPulseFlag();
    VehicleSensInitGpsInterruptFlag();

    VehicleSensInitGyroTrouble();
    VehicleSensInitMainGpsInterruptSignal();
    VehicleSensInitSysGpsInterruptSignal();
    VehicleSensInitGyroConnectStatus();
    VehicleSensInitGpsAntennaStatus();
    /* -- PastModel002 support */

    VehicleSensInitGyroTempFstl();
    VehicleSensInitGyroTempExtl();
    VehicleSensInitGyroTempl();
    VehicleSensInitPulseTimeExtl();
    VehicleSensInitPulseTimel();

    VehicleSensInitLocationLonLatG();
    VehicleSensInitLocationAltitudeG();
    VehicleSensInitMotionSpeedG();
    VehicleSensInitMotionHeadingG();
    VehicleSensInitNaviinfoDiagGPSg();
    VehicleSensInitGpsTimeG();
    VehicleSensInitGpsTimeRawG();
    VehicleSensInitWknRolloverG();
    VehicleSensInitLocationLonLatN();
    VehicleSensInitLocationAltitudeN();
    VehicleSensInitMotionSpeedN();
    VehicleSensInitMotionHeadingN();
    VehicleSensInitSettingTimeclock();
    VehicleSensInitMotionSpeedI();
    VehicleSensInitGpsClockDriftG();
    VehicleSensInitGpsClockFreqG();

    VehicleSens_InitLocationInfoNmea_n();
}

/*******************************************************************************
* MODULE    : VehicleSensSetDataMasterLineSens
* ABSTRACT  : Vehicle sensor data master LineSensor data set process
* FUNCTION  : Set LineSensor data
* ARGUMENT  : *pst_data      : LineSensor Vehicle Signal Notification Data
*           : p_data_master_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
//void  VehicleSensSetDataMasterLineSens(const LSDRV_LSDATA *pst_data,
//        PFUNC_DMASTER_SET_N p_data_master_set_n,
//        BOOL b_sens_ext) {
//    u_int8  uc_chg_type;
//
//    /*------------------------------------------------------*/
//    /*  Call the data set processing associated with the DID      */
//    /*  Call the data master set notification process      */
//    /*------------------------------------------------------*/
//    switch (pst_data->ul_did) {
//        case POSHAL_DID_SPEED_PULSE:
//        {
//            uc_chg_type = VehicleSensSetSpeedPulsel(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//
//            break;
//        }
//        case POSHAL_DID_GYRO:
//        {
//            uc_chg_type = VehicleSensSetGyrol(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            break;
//        }
//        case POSHAL_DID_REV:
//        {
//            uc_chg_type = VehicleSensSetRevl(pst_data);
//#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "VehicleSens_SetDataMaster POSHAL_DID_REV \r\n");
//#endif
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            /* ++ PastModel002 support DR */
//            VehicleSensDeliveryProcDR(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            /* -- PastModel002 support DR */
//            break;
//        }
//        case POSHAL_DID_GPS_ANTENNA:
//        {
//            uc_chg_type = VehicleSensSetGpsAntennal(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            break;
//        }
//        default:
//            break;
//    }
//}

/*******************************************************************************
* MODULE    : VehicleSensSetDataMasterLineSensG
* ABSTRACT  : Vehicle sensor data master LineSensor data set process
* FUNCTION  : Set LineSensor data
* ARGUMENT  : *pst_data      : LineSensor Vehicle Signal Notification Data
*           : p_data_master_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
//void  VehicleSensSetDataMasterLineSensG(const LSDRV_LSDATA_G *pst_data,
//        PFUNC_DMASTER_SET_N p_data_master_set_n,
//        BOOL b_sens_ext) {
//    u_int8  uc_chg_type;
//    SENSORMOTION_SPEEDINFO_DAT stSpeed;
//    const VEHICLESENS_DATA_MASTER* pst_master;
//    u_int16 usSP_KMPH = 0;            /* Vehicle speed(km/h)      */
//
//    /*------------------------------------------------------*/
//    /*  Call the data set processing associated with the DID      */
//    /*  Call the data master set notification process      */
//    /*------------------------------------------------------*/
//    switch (pst_data->ul_did) {
//        case POSHAL_DID_SPEED_PULSE:
//        {
//            uc_chg_type = VehicleSensSetSpeedPulselG(pst_data);
//#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "VehicleSens_SetDataMaster POSHAL_DID_SPEED_PULSE \r\n");
//#endif
//            if (b_sens_ext == TRUE) {
//                VehicleSensSetSpeedPulseExtlG(pst_data);
//            }
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            /* ++ PastModel002 support DR */
//            VehicleSensDeliveryProcDR(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            /* -- PastModel002 support DR */
//            break;
//        }
//        case POSHAL_DID_SPEED_KMPH:
//        {
//            uc_chg_type = VehicleSensSetSpeedKmphlG(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//
//            /* Set the data master of Motion Seepd (Internal) */
//            pst_master = (const VEHICLESENS_DATA_MASTER*)pst_data;
//            memset(&stSpeed, 0x00, sizeof(SENSORMOTION_SPEEDINFO_DAT));
//            /* When the notified size is ""0"",The vehicle speed cannot be calculated with the line sensor. */
//            if (pst_master->us_size == 0) {
//                stSpeed.isEnable  = FALSE;
//            } else {
//                stSpeed.isEnable  = TRUE;
//                memcpy(&usSP_KMPH, pst_master->uc_data, sizeof(u_int16));
//            }
//            stSpeed.getMethod = SENSOR_GET_METHOD_POS;
//            /* Unit conversion [0.01km/h] -> [0.01m/s] */
//            stSpeed.Speed     = static_cast<uint16_t>((1000 * usSP_KMPH) / 3600);
//
//            uc_chg_type = VehicleSensSetMotionSpeedI(&stSpeed);
//            (*p_data_master_set_n)(VEHICLE_DID_MOTION_SPEED_INTERNAL, uc_chg_type, VEHICLESENS_GETMETHOD_INTERNAL);
//            break;
//        }
//
//        case POSHAL_DID_GYRO:
//        {
//            uc_chg_type = VehicleSensSetGyrolG(pst_data);
//#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "VehicleSens_SetDataMaster POSHAL_DID_GYRO \r\n");
//#endif
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            /* ++ PastModel002 support DR */
//            VehicleSensDeliveryProcDR(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            /* -- PastModel002 support DR */
//            break;
//        }
//        case POSHAL_DID_SNS_COUNTER:
//        {
//#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "VehicleSens_SetDataMaster POSHAL_DID_SNS_COUNTER \r\n");
//#endif
//            uc_chg_type = VehicleSensSetSnsCounterlG(pst_data);
//            if (b_sens_ext == TRUE) {
//                VehicleSensSetSnsCounterExtlG(pst_data);
//            }
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            /* ++ PastModel002 support DR */
//            VehicleSensDeliveryProcDR(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            /* -- PastModel002 support DR */
//            break;
//        }
//        case POSHAL_DID_GYRO_EXT:    /* 3 to 14bit A/D value,0bit:REV */
//        {
//            uc_chg_type = VehicleSensSetGyroRevlG(pst_data);
//
//            if (b_sens_ext == TRUE) {
//                VehicleSensSetGyroExtlG(pst_data);
//            }
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//
//            break;
//        }
//
//        case POSHAL_DID_SPEED_PULSE_FLAG:
//        {
//            uc_chg_type = VehicleSensSetSpeedPulseFlag((const LSDRV_LSDATA_G *)pst_data);
//
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            /* ++ PastModel002 support DR */
//            VehicleSensDeliveryProcDR(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            /* -- PastModel002 support DR */
//
//            break;
//        }
//        case POSHAL_DID_GPS_INTERRUPT_FLAG:
//        {
//            uc_chg_type = VehicleSensSetGpsInterruptFlag((const LSDRV_LSDATA_G *)pst_data);
//
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//
//            break;
//        }
//        case POSHAL_DID_REV:
//        {
//            uc_chg_type = VehicleSensSetRevlG(pst_data);
//            if (b_sens_ext == TRUE) {
//                VehicleSensSetRevExtlG(pst_data);
//            }
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            break;
//        }
//        case POSHAL_DID_GYRO_TEMP:
//        {
//            uc_chg_type = VehicleSensSetGyroTemplG(pst_data);
//            if (b_sens_ext == TRUE) {
//                VehicleSensSetGyroTempExtlG(pst_data);
//            }
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            break;
//        }
//        case POSHAL_DID_GSNS_X:
//        {
//            uc_chg_type = VehicleSensSetGsnsXlG(pst_data);
//            if (b_sens_ext == TRUE) {
//                VehicleSensSetGsnsXExtlG(pst_data);
//            }
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            break;
//        }
//        case POSHAL_DID_GSNS_Y:
//        {
//            uc_chg_type = VehicleSensSetGsnsYlG(pst_data);
//            if (b_sens_ext == TRUE) {
//                VehicleSensSetGsnsYExtlG(pst_data);
//            }
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            break;
//        }
//        case POSHAL_DID_PULSE_TIME:
//        {
//            uc_chg_type = VehicleSensSetPulseTimelG(pst_data);
//            if (b_sens_ext == TRUE) {
//                VehicleSensSetPulseTimeExtlG(pst_data);
//            }
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            break;
//        }
//        default:
//            break;
//    }
//}

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*******************************************************************************
* MODULE    : VehicleSensSetDataMasterLineSensFst
* ABSTRACT  : Vehicle sensor data master LineSensor data set process
* FUNCTION  : Set LineSensor data
* ARGUMENT  : *pst_data      : LineSensor Vehicle Signal Notification Data
*           : p_data_master_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
//void  VehicleSensSetDataMasterLineSensFst(const LSDRV_LSDATA_FST *pst_data, PFUNC_DMASTER_SET_N p_data_master_set_n) {  // LCOV_EXCL_START 8: dead code  // // NOLINT(whitespace/line_length)
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    u_int8  uc_chg_type;
//
//    /*------------------------------------------------------*/
//    /*  Call the data set processing associated with the DID      */
//    /*  Call the data master set notification process      */
//    /*------------------------------------------------------*/
//    switch (pst_data->ul_did) {
//        case POSHAL_DID_GYRO_FST:    /* 3 to 14bit A/D value,0bit:REV */
//        {
//            uc_chg_type = VehicleSensSetGyroFstl(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            break;
//        }
//        case POSHAL_DID_SPEED_PULSE_FST:
//        {
//            uc_chg_type = VehicleSensSetSpeedPulseFstl(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            break;
//        }
//        default:
//            break;
//    }
//}
// LCOV_EXCL_STOP
/*******************************************************************************
* MODULE    : VehicleSensSetDataMasterLineSensFstG
* ABSTRACT  : Vehicle sensor data master LineSensor data set process
* FUNCTION  : Set LineSensor data
* ARGUMENT  : *pst_data      : LineSensor Vehicle Signal Notification Data
*           : p_data_master_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
//void  VehicleSensSetDataMasterLineSensFstG(const LSDRV_MSG_LSDATA_DAT_FST  *pst_data,
//        PFUNC_DMASTER_SET_N p_data_master_set_n) {
//    u_int8  uc_chg_type;
//
//    /* Internal debug log output */
//    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");
//
//    /*------------------------------------------------------*/
//    /*  Call the data set processing associated with the DID      */
//    /*  Call the data master set notification process      */
//    /*------------------------------------------------------*/
//    switch (pst_data->st_gyro.ul_did) {
//        case POSHAL_DID_GYRO_FST:
//        {
//            uc_chg_type = VehicleSensSetGyroFstG(&(pst_data->st_gyro));
//            if (pst_data->st_gyro.uc_partition_max == pst_data->st_gyro.uc_partition_num)
//            {
//                (*p_data_master_set_n)(pst_data->st_gyro.ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            }
//            break;
//        }
//        default:
//            break;
//    }
//
//    switch (pst_data->st_speed.ul_did) {
//        case POSHAL_DID_SPEED_PULSE_FST:
//        {
//            uc_chg_type = VehicleSensSetSpeedPulseFstG(&(pst_data->st_speed));
//            if (pst_data->st_speed.uc_partition_max == pst_data->st_speed.uc_partition_num)
//            {
//                (*p_data_master_set_n)(pst_data->st_speed.ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            }
//            break;
//        }
//        default:
//            break;
//    }
//
//    switch (pst_data->st_spd_pulse_flg.ul_did) {
//        case POSHAL_DID_SPEED_PULSE_FLAG_FST:
//        {
//            uc_chg_type = VehicleSensSetSpeedPulseFlagFstG(&(pst_data->st_spd_pulse_flg));
//            if (pst_data->st_spd_pulse_flg.uc_partition_max == pst_data->st_spd_pulse_flg.uc_partition_num)
//            {
//                (*p_data_master_set_n)(pst_data->st_spd_pulse_flg.ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            }
//            break;
//        }
//        default:
//            break;
//    }
//
//    switch (pst_data->st_rev.ul_did) {
//        case POSHAL_DID_REV_FST:
//        {
//            uc_chg_type = VehicleSensSetRevFstG(&(pst_data->st_rev));
//            if (pst_data->st_rev.uc_partition_max == pst_data->st_rev.uc_partition_num)
//            {
//                (*p_data_master_set_n)(pst_data->st_rev.ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            }
//            break;
//        }
//        default:
//            break;
//    }
//
//    switch (pst_data->st_gyro_temp.ul_did) {
//        case POSHAL_DID_GYRO_TEMP_FST:
//        {
//            uc_chg_type = VehicleSensSetGyroTempFstG(&(pst_data->st_gyro_temp));
//
//            if (pst_data->st_gyro_temp.uc_partition_max == pst_data->st_gyro_temp.uc_partition_num) {
//                (*p_data_master_set_n)(pst_data->st_gyro_temp.ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            }
//            break;
//        }
//        default:
//            break;
//    }
//
//    switch (pst_data->st_gsns_x.ul_did) {
//        case POSHAL_DID_GSNS_X_FST:
//        {
//            uc_chg_type = VehicleSensSetGsnsXFstG(&(pst_data->st_gsns_x));
//
//            if (pst_data->st_gsns_x.uc_partition_max == pst_data->st_gsns_x.uc_partition_num) {
//                (*p_data_master_set_n)(pst_data->st_gsns_x.ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            }
//            break;
//        }
//        default:
//            break;
//    }
//
//    switch (pst_data->st_gsns_y.ul_did) {
//        case POSHAL_DID_GSNS_Y_FST:
//        {
//            uc_chg_type = VehicleSensSetGsnsYFstG(&(pst_data->st_gsns_y));
//
//            if (pst_data->st_gsns_y.uc_partition_max == pst_data->st_gsns_y.uc_partition_num) {
//                (*p_data_master_set_n)(pst_data->st_gsns_y.ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//            }
//            break;
//        }
//        default:
//            break;
//    }
//
//    /* Internal debug log output */
//    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "-");
//}
#endif

/*******************************************************************************
* MODULE    : VehicleSensSetDataMasterGps
* ABSTRACT  : Vehicle Sensor Data Master GPS Data Set Processing
* FUNCTION  : Set GPS data
* ARGUMENT  : *pst_data      : GPS received message data
*           : p_data_master_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
//void  VehicleSensSetDataMasterGps(SENSOR_MSG_GPSDATA_DAT *pst_data,
//                                       PFUNC_DMASTER_SET_N p_data_master_set_n) {
//    u_int8  uc_chg_type;
//    SENSORLOCATION_LONLATINFO_DAT lstLonLat;
//    SENSORLOCATION_ALTITUDEINFO_DAT lstAltitude;
//    SENSORMOTION_HEADINGINFO_DAT lstHeading;
//    MDEV_GPS_CUSTOMDATA *pstCustomData;
//
////    RET_API   ret;
//
////    char* pEnvPositioning_CWORD86_ = NULL;
////    BOOL sndFlg;
//
//    VEHICLESENS_DATA_MASTER st_data;
//    u_int8 antennaState = 0;
//    u_int8 sensCount = 0;
//    u_int8 ucResult = SENSLOG_RES_SUCCESS;
//
//    EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
//    BOOL bIsAvailable;
//    PNO us_pno;
//
//    /* Antenna Connection Information */
//    (void)memset(reinterpret_cast<void *>(&st_data),  0, sizeof(st_data));
//    VehicleSensGetGpsAntennal(&st_data);
//    antennaState = st_data.uc_data[0];
//
//    /* Sensor Counter */
//    (void)memset(reinterpret_cast<void *>(&st_data),  0, sizeof(st_data));
//    VehicleSensGetSnsCounterl(&st_data);
//    /** Sensor Counter Value Calculation */
//    /** Subtract sensor counter according to data amount from sensor counter.(Rounded off) */
//    /** Communication speed9600bps = 1200byte/s,Sensor counter is 1 count at 100ms. */
//    sensCount = st_data.uc_data[0];
//
//    FRAMEWORKUNIFIEDLOG(ZONE_27, __FUNCTION__, 
//            "antennaState = %d, sensCount = %d", antennaState, sensCount);
//
//    /*------------------------------------------------------*/
//    /*  Call the data set processing associated with the DID      */
//    /*  Call the data master set notification process      */
//    /*------------------------------------------------------*/
//    switch (pst_data->ul_did) {  // LCOV_EXCL_BR_LINE 6:some DID is not used
//        case VEHICLE_DID_GPS_UBLOX_NAV_VELNED:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//#if (GPS_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//                "VehicleSens_SetDataMaster VEHICLE_DID_GPS_UBLOX_NAV_VELNED \r\n");
//#endif
//            uc_chg_type = VehicleSensSetNavVelnedG(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* ++ PastModel002 support DR */
//            VehicleSensDeliveryProcDR(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* -- PastModel002 support DR */
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_TIMEUTC:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//#if (GPS_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//                "VehicleSens_SetDataMaster VEHICLE_DID_GPS_UBLOX_NAV_TIMEUTC \r\n");
//#endif
//            uc_chg_type = VehicleSensSetNavTimeUtcG(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* ++ PastModel002 support DR */
//            VehicleSensDeliveryProcDR(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* -- PastModel002 support DR */
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_TIMEGPS:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//#if (GPS_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//                "VehicleSens_SetDataMaster VEHICLE_DID_GPS_UBLOX_NAV_TIMEGPS \r\n");
//#endif
//            uc_chg_type = VehicleSensSetNavTimeGpsG(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_SVINFO:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//#if (GPS_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//                "VehicleSens_SetDataMaster VEHICLE_DID_GPS_UBLOX_NAV_SVINFO \r\n");
//#endif
//            uc_chg_type = VehicleSensSetNavSvInfoG(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* ++ PastModel002 support DR */
//            VehicleSensDeliveryProcDR(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* -- PastModel002 support DR */
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_STATUS:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//#if (GPS_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//                "VehicleSens_SetDataMaster VEHICLE_DID_GPS_UBLOX_NAV_STATUS \r\n");
//#endif
//            uc_chg_type = VehicleSensSetNavStatusG(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* ++ PastModel002 support DR */
//            VehicleSensDeliveryProcDR(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* -- PastModel002 support DR */
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_POSLLH:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//#if (GPS_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//                "VehicleSens_SetDataMaster VEHICLE_DID_GPS_UBLOX_NAV_POSLLH \r\n");
//#endif
//            uc_chg_type = VehicleSensSetNavPosllhG(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* ++ PastModel002 support DR */
//            VehicleSensDeliveryProcDR(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* -- PastModel002 support DR */
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_CLOCK:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//#if (GPS_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//                "VehicleSens_SetDataMaster VEHICLE_DID_GPS_UBLOX_NAV_CLOCK \r\n");
//#endif
//            uc_chg_type = VehicleSensSetNavClockG(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* ++ PastModel002 support DR */
//            VehicleSensDeliveryProcDR(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* -- PastModel002 support DR */
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_DOP:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//#if (GPS_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//                "VehicleSens_SetDataMaster VEHICLE_DID_GPS_UBLOX_NAV_DOP \r\n");
//#endif
//            uc_chg_type = VehicleSensSetNavDopG(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* ++ PastModel002 support DR */
//            VehicleSensDeliveryProcDR(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            /* -- PastModel002 support DR */
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_MON_HW:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//#if (GPS_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "VehicleSens_SetDataMaster VEHICLE_DID_GPS_UBLOX_MON_HW \r\n");
//#endif
//            uc_chg_type = VehicleSensSetMonHwG(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_COUNTER:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//            uc_chg_type = VehicleSensSetGpsCounterg(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        /* GPS raw data(_CWORD82_ NMEA) */
//        case VEHICLE_DID_GPS__CWORD82__NMEA:
//        {
//            /* NMEA data */
//            FRAMEWORKUNIFIEDLOG(ZONE_27, __FUNCTION__, 
//                "VehicleSens_GpsDataDelivery VEHICLE_DID_GPS__CWORD82__NMEA -->");
//
//            /* Insert Antenna Connection Status and Sensor Counter */
//            pst_data->uc_data[1] = antennaState; /* Insert Antennas into 2byte Eyes */
//            /* Place counters at 3byte */
//            pst_data->uc_data[2] = static_cast<u_int8>(sensCount - (u_int8)(((GPS_NMEA_SZ * 10) / 1200) + 1));
//
//            uc_chg_type = VehicleSensSetGps_CWORD82_NmeaG(pst_data);  /* Ignore->MISRA-C++:2008 Rule 5-2-5 */
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            FRAMEWORKUNIFIEDLOG(ZONE_27, __FUNCTION__, 
//                "VehicleSens_GpsDataDelivery VEHICLE_DID_GPS__CWORD82__NMEA <--");
//            break;
//        }
//        /* GPS raw data(FullBinary) */
//        case POSHAL_DID_GPS__CWORD82__FULLBINARY:
//        {
//            /* FullBin data */
//            FRAMEWORKUNIFIEDLOG(ZONE_27, __FUNCTION__, 
//                "VehicleSens_GpsDataDelivery POSHAL_DID_GPS__CWORD82__FULLBINARY -->");
//
//            /* Insert Antenna Connection Status and Sensor Counter */
//            pst_data->uc_data[0] = antennaState; /* Insert Antennas into 1byte Eyes */
//            /* Place counters at 2byte */
//            pst_data->uc_data[1] = static_cast<u_int8>(sensCount - (u_int8)(((GPS_CMD_FULLBIN_SZ * 10) / 1200) + 1));
//
//            uc_chg_type = VehicleSensSetGps_CWORD82_FullBinaryG(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            FRAMEWORKUNIFIEDLOG(ZONE_27, __FUNCTION__, 
//                "VehicleSens_GpsDataDelivery POSHAL_DID_GPS__CWORD82__FULLBINARY <--");
//            break;
//        }
//        /* GPS raw data(Specific information) */
//        case POSHAL_DID_GPS__CWORD82___CWORD44_GP4:
//        {
//            /* GPS-specific information */
//            uc_chg_type = VehicleSensSetGps_CWORD82__CWORD44_Gp4G(pst_data);  /* Ignore->MISRA-C++:2008 Rule 5-2-5 */
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            break;
//        }
//        case POSHAL_DID_GPS_CUSTOMDATA:
//        {
//            pstCustomData = reinterpret_cast<MDEV_GPS_CUSTOMDATA*>(pst_data->uc_data);
//            /* Latitude/LongitudeInformation */
//            (void)memcpy(&lstLonLat, &(pstCustomData->st_lonlat), sizeof(SENSORLOCATION_LONLATINFO_DAT));
//            lstLonLat.SyncCnt = VehicleSensGetSensorCnt(); /* Synchronization counter setting */
//            uc_chg_type = VehicleSensSetLocationLonLatG(&lstLonLat);
//            (*p_data_master_set_n)(VEHICLE_DID_LOCATION_LONLAT, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//
//            /* Altitude information */
//            (void)memcpy(&lstAltitude, &(pstCustomData->st_altitude), sizeof(SENSORLOCATION_ALTITUDEINFO_DAT));
//            lstAltitude.SyncCnt = VehicleSensGetSensorCnt(); /* Synchronization counter setting */
//            uc_chg_type = VehicleSensSetLocationAltitudeG(&lstAltitude);
//            (*p_data_master_set_n)(VEHICLE_DID_LOCATION_ALTITUDE, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//
//            /* Bearing information */
//            (void)memcpy(&lstHeading, &(pstCustomData->st_heading), sizeof(SENSORMOTION_HEADINGINFO_DAT));
//            lstHeading.SyncCnt = VehicleSensGetSensorCnt(); /* Synchronization counter setting */
//            uc_chg_type = VehicleSensSetMotionHeadingG(&lstHeading);
//            (*p_data_master_set_n)(VEHICLE_DID_MOTION_HEADING, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//
//            /* GPS time information */
//            uc_chg_type = VehicleSensSetGpsTimeG(&(pstCustomData->st_gps_time));
//            (*p_data_master_set_n)(POSHAL_DID_GPS_TIME, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//
//            /* Set GPS time to CLOCK */
//            eStatus = ClockIfDtimeSetGpsTime(&(pstCustomData->st_gps_time), &bIsAvailable);
//            if ((bIsAvailable == TRUE) && (eStatus != eFrameworkunifiedStatusOK)) {
//                ucResult = SENSLOG_RES_FAIL;
//                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
//                    "ClockIfDtimeSetGpsTime ERROR!! [ret=%d, y=%d, m=%d, d=%d, h=%d, n=%d, s=%d, sts=%d]",
//                    eStatus, pstCustomData->st_gps_time.utc.year, pstCustomData->st_gps_time.utc.month,
//                    pstCustomData->st_gps_time.utc.date,
//                    pstCustomData->st_gps_time.utc.hour, pstCustomData->st_gps_time.utc.minute,
//                    pstCustomData->st_gps_time.utc.second, pstCustomData->st_gps_time.tdsts);
//            }
//            us_pno = _pb_CnvName2Pno(SENSLOG_PNAME_CLOCK);
//            SensLogWriteOutputData(SENSLOG_DATA_O_TIME_CS, 0, us_pno,
//                            reinterpret_cast<uint8_t *>(&(pstCustomData->st_gps_time)),
//                            sizeof(pstCustomData->st_gps_time), ucResult);
//
//            /* Diag Info */
//            uc_chg_type = VehicleSensSetNaviinfoDiagGPSg(&(pstCustomData->st_diag_gps));
//            (*p_data_master_set_n)(VEHICLE_DID_NAVIINFO_DIAG_GPS, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            break;
//        }
//        /* GPS raw data(NMEA except _CWORD82_) */
//        case POSHAL_DID_GPS_NMEA:
//        {
//            /* NMEA data */
//            uc_chg_type = VehicleSensSetGpsNmeaG(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            break;
//        }
//        /* GPS version(except _CWORD82_) */
//        case POSHAL_DID_GPS_VERSION:
//        {
//            VehicleSensSetGpsVersion(pst_data);
//            break;
//        }
//        /* Raw GPS time data */
//        case POSHAL_DID_GPS_TIME_RAW:
//        {
//            (void)VehicleSensSetGpsTimeRawG(reinterpret_cast<SENSOR_GPSTIME_RAW*>(pst_data->uc_data));
//            break;
//        }
//        case POSHAL_DID_GPS_WKNROLLOVER:
//        {
//            (void)VehicleSensSetWknRolloverG(reinterpret_cast<SENSOR_WKNROLLOVER*>(pst_data->uc_data));
//            break;
//        }
//        /* GPS clock drift */
//        case POSHAL_DID_GPS_CLOCK_DRIFT:
//        {
//            uc_chg_type = VehicleSensSetGpsClockDriftG(reinterpret_cast<int32_t*>(pst_data->uc_data));
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            break;
//        }
//        /* GPS clock frequency */
//        case POSHAL_DID_GPS_CLOCK_FREQ:
//        {
//            uc_chg_type = VehicleSensSetGpsClockFreqG(reinterpret_cast<uint32_t*>(pst_data->uc_data));
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//            break;
//        }
//
//        default:
//            break;
//    }
//}

/*******************************************************************************
 * MODULE    : VehicleSensSetDataMasterData
 * ABSTRACT  : Vehicle sensor data master common data set processing
 * FUNCTION  : Set vehicle data
 * ARGUMENT  : *pstMsg      : Received message data
 *           : p_data_master_set_n  : Data Master Set Notification(Callback function)
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
//void  VehicleSensSetDataMasterData(const POS_MSGINFO *pstMsg,
//                                        PFUNC_DMASTER_SET_N p_data_master_set_n) {
//    u_int8              uc_chg_type = 0;
//    const POS_POSDATA        *pstPosData = NULL;
//    const u_int16          *pucSpeed = 0;
//    SENSORLOCATION_LONLATINFO_DAT  stLonLat;
//    SENSORLOCATION_ALTITUDEINFO_DAT  stAltitude;
//    SENSORMOTION_SPEEDINFO_DAT    stSpeed;
//    SENSORMOTION_HEADINGINFO_DAT  stHeading;
//    const SENSOR_MSG_GPSTIME    *pstGpsTime;
//
////    RET_API   ret;
//
////    char* pEnvPositioning_CWORD86_ = NULL;
////    BOOL sndFlg;
//
//
//    EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
//    BOOL bIsAvailable;
//    u_int8 ucResult = SENSLOG_RES_SUCCESS;
//    PNO us_pno;
//
//    /*------------------------------------------------------*/
//    /*  Call the data set processing associated with the DID      */
//    /*  Call the data master set notification process      */
//    /*------------------------------------------------------*/
//    switch (pstMsg->did) {
//        case POSHAL_DID_GPS_CUSTOMDATA_NAVI:
//        {
//            pstPosData = (const POS_POSDATA *) & (pstMsg->data);
//
//            /* Latitude/LongitudeInformation */
//            if (((pstPosData->status & POS_LOC_INFO_LAT) == POS_LOC_INFO_LAT) ||
//                    ((pstPosData->status & POS_LOC_INFO_LON) == POS_LOC_INFO_LON)) {
//                memset(&stLonLat, 0x00, sizeof(stLonLat));
//                stLonLat.getMethod = SENSOR_GET_METHOD_NAVI;
//                stLonLat.SyncCnt   = VehicleSensGetSensorCnt();
//                stLonLat.isEnable  = SENSORLOCATION_STATUS_ENABLE;
//                stLonLat.isExistDR = 0x00;
//                stLonLat.DRStatus  = SENSORLOCATION_DRSTATUS_INVALID;
//                stLonLat.posSts    = pstPosData->posSts;
//                stLonLat.posAcc    = pstPosData->posAcc;
//                stLonLat.Longitude = pstPosData->longitude;
//                stLonLat.Latitude  = pstPosData->latitude;
//                uc_chg_type = VehicleSensSetLocationLonLatN((const SENSORLOCATION_LONLATINFO_DAT *)&stLonLat);
//                (*p_data_master_set_n)(VEHICLE_DID_LOCATION_LONLAT_NAVI, uc_chg_type, VEHICLESENS_GETMETHOD_NAVI);
//            }
//
//            /* Altitude information */
//            if ((pstPosData->status & POS_LOC_INFO_ALT) == POS_LOC_INFO_ALT) {
//                memset(&stAltitude, 0x00, sizeof(stAltitude));
//                stAltitude.getMethod = SENSOR_GET_METHOD_NAVI;
//                stAltitude.SyncCnt   = VehicleSensGetSensorCnt();
//                stAltitude.isEnable  = SENSORLOCATION_STATUS_ENABLE;
//                stAltitude.isExistDR = 0x00;
//                stAltitude.DRStatus  = SENSORLOCATION_DRSTATUS_INVALID;
//                stAltitude.Altitude  = pstPosData->altitude;
//                uc_chg_type = VehicleSensSetLocationAltitudeN((const SENSORLOCATION_ALTITUDEINFO_DAT *)&stAltitude);
//                (*p_data_master_set_n)(VEHICLE_DID_LOCATION_ALTITUDE_NAVI, uc_chg_type, VEHICLESENS_GETMETHOD_NAVI);
//            }
//
//            /* Bearing information */
//            if ((pstPosData->status & POS_LOC_INFO_HEAD) == POS_LOC_INFO_HEAD) {
//                memset(&stHeading, 0x00, sizeof(stHeading));
//                stHeading.getMethod = SENSOR_GET_METHOD_NAVI;
//                stHeading.SyncCnt   = VehicleSensGetSensorCnt();
//                stHeading.isEnable  = SENSORMOTION_STATUS_ENABLE;
//                stHeading.isExistDR = 0x00;
//                stHeading.DRStatus  = SENSORMOTION_DRSTATUS_INVALID;
//                stHeading.posSts    = pstPosData->posSts;
//                stHeading.Heading   = pstPosData->heading;
//                uc_chg_type = VehicleSensSetMotionHeadingN((const SENSORMOTION_HEADINGINFO_DAT *)&stHeading);
//                (*p_data_master_set_n)(VEHICLE_DID_MOTION_HEADING_NAVI, uc_chg_type, VEHICLESENS_GETMETHOD_NAVI);
//            }
//
//            ( *p_data_master_set_n )( POSHAL_DID_GPS_CUSTOMDATA_NAVI, VEHICLESENS_NEQ, VEHICLESENS_GETMETHOD_NAVI );
//
//            break;
//        }
//        case VEHICLE_DID_MOTION_SPEED_NAVI:
//        {
//            pucSpeed = (const u_int16 *) & (pstMsg->data);
//
//            /* Vehicle speed information */
//            memset(&stSpeed, 0x00, sizeof(stSpeed));
//            stSpeed.getMethod = SENSOR_GET_METHOD_NAVI;
//            stSpeed.SyncCnt   = VehicleSensGetSensorCnt();
//            stSpeed.isEnable  = SENSORMOTION_STATUS_ENABLE;
//            stSpeed.isExistDR = 0x00;
//            stSpeed.DRStatus  = SENSORMOTION_DRSTATUS_INVALID;
//            stSpeed.Speed     = *pucSpeed;
//            uc_chg_type = VehicleSensSetMotionSpeedN((const SENSORMOTION_SPEEDINFO_DAT *)&stSpeed);
//            (*p_data_master_set_n)(VEHICLE_DID_MOTION_SPEED_NAVI, uc_chg_type, VEHICLESENS_GETMETHOD_NAVI);
//            break;
//        }
//        case POSHAL_DID_GPS_TIME:
//        {
//            pstGpsTime = (const SENSOR_MSG_GPSTIME*)(pstMsg->data);
//            uc_chg_type = VehicleSensSetGpsTimeG((const SENSOR_MSG_GPSTIME *)pstGpsTime);
//            (*p_data_master_set_n)(POSHAL_DID_GPS_TIME, uc_chg_type, VEHICLESENS_GETMETHOD_GPS);
//
//            /* Set GPS time to CLOCK */
//            eStatus = ClockIfDtimeSetGpsTime(pstGpsTime, &bIsAvailable);
//            if ((bIsAvailable == TRUE) && (eStatus != eFrameworkunifiedStatusOK)) {
//                ucResult = SENSLOG_RES_FAIL;
//                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
//                            "ClockIfDtimeSetGpsTime ERROR!! [ret=%d, y=%d, m=%d, d=%d, h=%d, n=%d, s=%d, sts=%d]",
//                            eStatus, pstGpsTime->utc.year, pstGpsTime->utc.month, pstGpsTime->utc.date,
//                            pstGpsTime->utc.hour, pstGpsTime->utc.minute, pstGpsTime->utc.second, pstGpsTime->tdsts);
//            }
//            us_pno = _pb_CnvName2Pno(SENSLOG_PNAME_CLOCK);
//            SensLogWriteOutputData(SENSLOG_DATA_O_TIME_CS, 0, us_pno,
//                                (uint8_t *)(pstGpsTime),  // NOLINT(readability/casting)
//                                    sizeof(SENSOR_MSG_GPSTIME), ucResult);
//            break;
//        }
//        case VEHICLE_DID_SETTINGTIME:
//        {
//            /* GPS time information */
//            uc_chg_type = VehicleSensSetSettingTimeclock((const POS_DATETIME *) & (pstMsg->data));
//            (*p_data_master_set_n)(VEHICLE_DID_SETTINGTIME, uc_chg_type, VEHICLESENS_GETMETHOD_OTHER);
//            break;
//        }
//
//        case VEHICLE_DID_LOCATIONINFO_NMEA_NAVI:
//        {
//            uc_chg_type = VehicleSens_SetLocationInfoNmea_n((const POS_LOCATIONINFO_NMEA*)&(pstMsg->data));
//            (*p_data_master_set_n)(VEHICLE_DID_LOCATIONINFO_NMEA_NAVI, uc_chg_type, VEHICLESENS_GETMETHOD_NAVI );
//            break;
//        }
//
//        default:
//            break;
//    }
//    return;
//}

/*******************************************************************************
* MODULE    : VehicleSensSetDataMasterGyroTrouble
* ABSTRACT  : Vehicle Sensor Data Master Gyro Failure Status Setting Process
* FUNCTION  : Set a gyro fault condition
* ARGUMENT  : *pst_data      : Gyro Failure Status Notification Data
*           : p_data_master_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
//void  VehicleSensSetDataMasterGyroTrouble(const LSDRV_MSG_LSDATA_DAT_GYRO_TROUBLE *pst_data, PFUNC_DMASTER_SET_N p_data_master_set_n) {  // LCOV_EXCL_START 8: dead code.  // NOLINT(whitespace/line_length)
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    u_int8  uc_chg_type;
//
//    if ((pst_data != NULL) && (p_data_master_set_n != NULL)) {
//        if (pst_data->ul_did == VEHICLE_DID_GYRO_TROUBLE) {
//            uc_chg_type = VehicleSensSetGyroTrouble(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//        } else {
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "UNKNOWN DID(%x) \r\n", pst_data->ul_did);
//        }
//    } else {
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ARGUMENT is NULL\r\n");
//    }
//}
// LCOV_EXCL_STOP
/*******************************************************************************
* MODULE    : VehicleSensSetDataMasterSysGpsInterruptSignal
* ABSTRACT  : Vehicle Sensor Data Master SYS GPS Interrupt Signal Setting
* FUNCTION  : Setting SYS GPS Interrupt Signals
* ARGUMENT  : *pst_data            : SYS GPS interrupt notification
*           : p_data_master_set_shared_memory  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
//void  VehicleSensSetDataMasterSysGpsInterruptSignal(const LSDRV_MSG_LSDATA_DAT_GPS_INTERRUPT_SIGNAL *pst_data, PFUNC_DMASTER_SET_SHARED_MEMORY p_data_master_set_shared_memory) {  // LCOV_EXCL_START 8: dead code.  // NOLINT(whitespace/line_length)
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    u_int8  uc_chg_type;
//    if ((pst_data != NULL) && (p_data_master_set_shared_memory != NULL)) {
//        if (pst_data->ul_did == VEHICLE_DID_SYS_GPS_INTERRUPT_SIGNAL) {
//            uc_chg_type = VehicleSensSetSysGpsInterruptSignal(pst_data);
//            /* As SYS GPS interrupt signals are not registered for delivery,          */
//            /* Disposal quantity,To avoid risks,DeliveryProc shall not be called.  */
//            (*p_data_master_set_shared_memory)(pst_data->ul_did, uc_chg_type);
//        } else {
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "UNKNOWN DID(%x) \r\n", pst_data->ul_did);
//        }
//    } else {
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ARGUMENT is NULL\r\n");
//    }
//}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensSetDataMasterMainGpsInterruptSignal
* ABSTRACT  : Vehicle Sensor Data Master MAIN GPS Interrupt Signal Setting
* FUNCTION  : Setting MAIN GPS Interrupt Signals
* ARGUMENT  : *pst_data            : MAIN GPS interrupt notification
*           : p_data_master_set_shared_memory  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
//void  VehicleSensSetDataMasterMainGpsInterruptSignal(const SENSOR_MSG_GPSDATA_DAT *pst_data, PFUNC_DMASTER_SET_SHARED_MEMORY p_data_master_set_shared_memory) {  // LCOV_EXCL_START 8: dead code.  // NOLINT(whitespace/line_length)
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    u_int8  uc_chg_type;
//
//    if ((pst_data != NULL) &&
//            (p_data_master_set_shared_memory != NULL)) {
//        if (pst_data->ul_did == VEHICLE_DID_MAIN_GPS_INTERRUPT_SIGNAL) {
//            uc_chg_type = VehicleSensSetMainGpsInterruptSignal(pst_data);
//            /* As MAIN GPS interrupt signals are not registered for delivery,          */
//            /* Disposal quantity,To avoid risks,DeliveryProc shall not be called.  */
//            (*p_data_master_set_shared_memory)(pst_data->ul_did, uc_chg_type);
//        } else {
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "UNKNOWN DID(%x) \r\n", pst_data->ul_did);
//        }
//    } else {
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ARGUMENT is NULL\r\n");
//    }
//}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensSetDataMasterGyroConnectStatus
* ABSTRACT  : Vehicle Sensor Data Master Gyro Connection Status Setting Processing
* FUNCTION  : Set the state of the gyro connection
* ARGUMENT  : *pst_data      : Gyro Connect Status Notification Data
*           : p_data_master_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
//void  VehicleSensSetDataMasterGyroConnectStatus(const LSDRV_MSG_LSDATA_DAT_GYRO_CONNECT_STATUS *pst_data, PFUNC_DMASTER_SET_SHARED_MEMORY p_data_master_set_shared_memory) {  // LCOV_EXCL_START 8: dead code.  // NOLINT(whitespace/line_length)
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    u_int8  uc_chg_type;
//
//    if ((pst_data != NULL) &&
//            (p_data_master_set_shared_memory != NULL)) {
//        if (pst_data->ul_did == VEHICLE_DID_GYRO_CONNECT_STATUS) {
//            uc_chg_type = VehicleSensSetGyroConnectStatus(pst_data);
//            /* As MAIN GPS interrupt signals are not registered for delivery,          */
//            /* Disposal quantity,To avoid risks,DeliveryProc shall not be called.  */
//            (*p_data_master_set_shared_memory)(pst_data->ul_did, uc_chg_type);
//        } else {
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "UNKNOWN DID(%x) \r\n", pst_data->ul_did);
//        }
//    } else {
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ARGUMENT is NULL\r\n");
//    }
//}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensSetDataMasterGpsAntennaStatus
* ABSTRACT  : Vehicle Sensor Data Master GPS Antenna Connection Status Setting Processing
* FUNCTION  : Setting the GPS Antenna Connection Status
* ARGUMENT  : *pst_data      : GPS antenna connection status notification data
*           : p_data_master_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
//void  VehicleSensSetDataMasterGpsAntennaStatus(const LSDRV_MSG_LSDATA_DAT_GPS_ANTENNA_STATUS *pst_data, PFUNC_DMASTER_SET_N p_data_master_set_n) {  // LCOV_EXCL_START 8: dead code.  // NOLINT(whitespace/line_length)
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    u_int8  uc_chg_type;
//
//    if ((pst_data != NULL) && (p_data_master_set_n != NULL)) {
//        if (pst_data->ul_did == POSHAL_DID_GPS_ANTENNA) {
//            uc_chg_type = VehicleSensSetGpsAntennaStatus(pst_data);
//            (*p_data_master_set_n)(pst_data->ul_did, uc_chg_type, VEHICLESENS_GETMETHOD_LINE);
//        } else {
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "UNKNOWN DID(%x) \r\n", pst_data->ul_did);
//        }
//    } else {
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ARGUMENT is NULL\r\n");
//    }
//}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensGetDataMaster
* ABSTRACT  : Vehicle Sensor Data Master Get Processing
* FUNCTION  : Provide vehicle sensor data master
* ARGUMENT  : ul_did    : Data ID corresponding to the vehicle information
*           : uc_get_method  : Data collection way
*           :          VEHICLESENS_GETMETHOD_CAN:  CAN communication
*           :          VEHICLESENS_GETMETHOD_LINE:  LineSensor drivers
*           : *pst_data    : Pointer to the data master provider
* NOTE      :
* RETURN    : void
******************************************************************************/
void  VehicleSensGetDataMaster(DID ul_did,
                                    u_int8 uc_get_method,
                                    VEHICLESENS_DATA_MASTER *pst_data) {
//    /*------------------------------------------------------*/
//    /*  Call the data Get processing associated with the DID        */
//    /*------------------------------------------------------*/
//    switch (ul_did) {  // LCOV_EXCL_BR_LINE 6:some DID is not used
//            /*------------------------------------------------------*/
//            /*  Vehicle sensor data group                  */
//            /*------------------------------------------------------*/
//        case POSHAL_DID_SPEED_PULSE:
//        {
//            VehicleSensGetSpeedPulse(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_GYRO:
//        {
//            VehicleSensGetGyro(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_GYRO_EXT:    /* 3 to 14bit A/D value,0bit:REV */
//        {
//            VehicleSensGetGyroRev(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_GSNS_X:
//        {
//            VehicleSensGetGsnsX(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_GSNS_Y:
//        {
//            VehicleSensGetGsnsY(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_REV:
//        {
//            VehicleSensGetRev(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_SPEED_PULSE_FLAG:
//        {
//            /* Data acquisition not selected */
//            VehicleSensGetSpeedPulseFlag(pst_data);
//            break;
//        }
//        case POSHAL_DID_GPS_ANTENNA:
//        {
//            VehicleSensGetGpsAntenna(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_SNS_COUNTER:
//        {
//            VehicleSensGetSnsCounter(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_GPS_INTERRUPT_FLAG:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//            /* Data acquisition not selected */
//            VehicleSensGetGpsInterruptFlag(pst_data);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case POSHAL_DID_SPEED_KMPH:
//        {
//            VehicleSensGetSpeedKmph(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_GYRO_TEMP:
//        {
//            VehicleSensGetGyroTemp(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_PULSE_TIME:
//        {
//            VehicleSensGetPulseTime(pst_data, uc_get_method);
//            break;
//        }
//        case VEHICLE_DID_LOCATION_LONLAT:
//        case VEHICLE_DID_LOCATION_LONLAT_NAVI:
//        {
//            VehicleSensGetLocationLonLat(pst_data, uc_get_method);
//            break;
//        }
//        case VEHICLE_DID_LOCATION_ALTITUDE:
//        case VEHICLE_DID_LOCATION_ALTITUDE_NAVI:
//        {
//            VehicleSensGetLocationAltitude(pst_data, uc_get_method);
//            break;
//        }
//        case VEHICLE_DID_MOTION_SPEED_NAVI:
//        case VEHICLE_DID_MOTION_SPEED_INTERNAL:
//        {
//            VehicleSensGetMotionSpeed(pst_data, uc_get_method);
//            break;
//        }
//        case VEHICLE_DID_MOTION_HEADING:
//        case VEHICLE_DID_MOTION_HEADING_NAVI:
//        {
//            VehicleSensGetMotionHeading(pst_data, uc_get_method);
//            break;
//        }
//        case VEHICLE_DID_SETTINGTIME:
//        {
//            VehicleSensGetSettingTime(pst_data, uc_get_method);
//            break;
//        }
//
//        default:
//            break;
//    }
}

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*******************************************************************************
* MODULE    : VehicleSensGetDataMasterExt
* ABSTRACT  : Vehicle Sensor Data Master Get Processing(Initial delivery)
* FUNCTION  : Provide vehicle sensor data master
* ARGUMENT  : ul_did    : Data ID corresponding to the vehicle information
*           : uc_get_method  : Data collection way
*           :          VEHICLESENS_GETMETHOD_CAN:  CAN communication
*           :          VEHICLESENS_GETMETHOD_LINE:  LineSensor drivers
*           :          VEHICLESENS_GETMETHOD_NAVI:  Navi
*           :          VEHICLESENS_GETMETHOD_CLOCK:Clock
*           : *pst_data    : Pointer to the data master provider
* NOTE      :
* RETURN    : void
******************************************************************************/
void  VehicleSensGetDataMasterExt(DID ul_did,
                                        u_int8 uc_get_method,
                                        VEHICLESENS_DATA_MASTER_EXT *pst_data) {
    /*------------------------------------------------------*/
    /*  Call the data Get processing associated with the DID        */
    /*------------------------------------------------------*/
//    switch (ul_did) {
//            /*------------------------------------------------------*/
//            /*  Vehicle sensor data group                  */
//            /*------------------------------------------------------*/
//        case POSHAL_DID_SPEED_PULSE:
//        {
//            VehicleSensGetSpeedPulseExt(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_GSNS_X:
//        {
//            VehicleSensGetGsnsXExt(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_GSNS_Y:
//        {
//            VehicleSensGetGsnsYExt(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_SNS_COUNTER:
//        {
//            VehicleSensGetSnsCounterExt(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_GYRO_EXT:    /* 3 to 14bit A/D value,0bit:REV */
//        {
//            VehicleSensGetGyroExt(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_REV:
//        {
//            VehicleSensGetRevExt(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_GYRO_TEMP:
//        {
//            VehicleSensGetGyroTempExt(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_PULSE_TIME:
//        {
//            VehicleSensGetPulseTimeExt(pst_data, uc_get_method);
//            break;
//        }
//        default:  /* Ignore->MISRA-C++:2008 Rule 6-3-1, 6-4-1 */
//            break;
//    }
}

/*******************************************************************************
* MODULE    : VehicleSensGetDataMasterFst
* ABSTRACT  : Vehicle Sensor Data Master Get Processing(Initial transmission data)
* FUNCTION  : Provide vehicle sensor data master
* ARGUMENT  : ul_did    : Data ID corresponding to the vehicle information
*           : uc_get_method  : Data collection way
*           :          VEHICLESENS_GETMETHOD_CAN:  CAN communication
*           :          VEHICLESENS_GETMETHOD_LINE:  LineSensor drivers
*           : *pst_data    : Pointer to the data master provider
* NOTE      :
* RETURN    : void
******************************************************************************/
void  VehicleSensGetDataMasterFst(DID ul_did,
                                        u_int8 uc_get_method,
                                        VEHICLESENS_DATA_MASTER_FST *pst_data) {
    /*------------------------------------------------------*/
    /*  Call the data Get processing associated with the DID        */
    /*------------------------------------------------------*/
//    switch (ul_did) {
//            /*------------------------------------------------------*/
//            /*  Vehicle sensor data group                  */
//            /*------------------------------------------------------*/
//        case POSHAL_DID_GYRO_FST:    /* 3 to 14bit A/D value,0bit:REV */
//        {
//            VehicleSensGetGyroFst(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_SPEED_PULSE_FST:
//        {
//            VehicleSensGetSpeedPulseFst(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_SPEED_PULSE_FLAG_FST:
//        {
//            VehicleSensGetSpeedPulseFlagFst(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_REV_FST:
//        {
//            VehicleSensGetRevFst(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_GYRO_TEMP_FST:
//        {
//            VehicleSensGetGyroTempFst(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_GSNS_X_FST:
//        {
//            VehicleSensGetGsnsXFst(pst_data, uc_get_method);
//            break;
//        }
//        case POSHAL_DID_GSNS_Y_FST:
//        {
//            VehicleSensGetGsnsYFst(pst_data, uc_get_method);
//            break;
//        }
//        default:
//            break;
//    }
}
#endif

/*******************************************************************************
* MODULE    : VehicleSensGetGpsDataMaster
* ABSTRACT  : Vehicle Sensor Data Master GPS Data Get Processing
* FUNCTION  : Provide vehicle sensor data master GPS data
* ARGUMENT  : ul_did    : Data ID corresponding to the vehicle information
*           : uc_get_method  : Data collection way
*           :          VEHICLESENS_GETMETHOD_GPS:  GPS
*           : *pst_data    : Pointer to the data master provider
* NOTE      :
* RETURN    : void
******************************************************************************/
//void  VehicleSensGetGpsDataMaster(DID ul_did,
//                                        u_int8 uc_get_method,
//                                        SENSOR_MSG_GPSDATA_DAT *pst_data) {
//    /*------------------------------------------------------*/
//    /*  Call the data Get processing associated with the DID        */
//    /*------------------------------------------------------*/
//    switch (ul_did) {  // LCOV_EXCL_BR_LINE 6:some DID is not used
//            /*------------------------------------------------------*/
//            /*  GPS data group                      */
//            /*------------------------------------------------------*/
//
//        case VEHICLE_DID_GPS_UBLOX_NAV_VELNED:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//            VehicleSensGetNavVelnedG(pst_data);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_TIMEUTC:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//            VehicleSensGetNavTimeUtcG(pst_data);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_TIMEGPS:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//            VehicleSensGetNavTimeGpsG(pst_data);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_SVINFO:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//            VehicleSensGetNavSvInfoG(pst_data);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_STATUS:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//            VehicleSensGetNavStatusG(pst_data);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_POSLLH:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//            VehicleSensGetNavPosllhG(pst_data);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_CLOCK:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//            VehicleSensGetNavClockG(pst_data);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_NAV_DOP:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();
//            VehicleSensGetNavDopG(pst_data);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_UBLOX_MON_HW:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();
//            VehicleSensGetMonHwG(pst_data);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS_COUNTER:
//        {
//            // LCOV_EXCL_START 8 : dead code
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//            VehicleSensGetGpsCounterg(pst_data);
//            break;
//            // LCOV_EXCL_STOP
//        }
//        case VEHICLE_DID_GPS__CWORD82__NMEA:
//        {
//            VehicleSensGetGps_CWORD82_NmeaG(pst_data);
//            break;
//        }
//        case POSHAL_DID_GPS__CWORD82__FULLBINARY:
//        {
//            VehicleSensGetGps_CWORD82_FullBinaryG(pst_data);
//            break;
//        }
//        case POSHAL_DID_GPS__CWORD82___CWORD44_GP4:
//        {
//            VehicleSensGetGps_CWORD82__CWORD44_Gp4G(pst_data);
//            break;
//        }
//        case VEHICLE_DID_NAVIINFO_DIAG_GPS:
//        {
//            VehicleSensGetNaviinfoDiagGPSg(pst_data);
//            break;
//        }
//        case POSHAL_DID_GPS_TIME:
//        {
//            VehicleSensGetGpsTimeG(pst_data);
//            break;
//        }
//      case POSHAL_DID_GPS_TIME_RAW:
//    {
//            VehicleSensGetGpsTimeRawG(pst_data);
//      break;
//    }
//        case POSHAL_DID_GPS_NMEA:
//        {
//            VehicleSensGetGpsNmeaG(pst_data);
//            break;
//        }
//      case POSHAL_DID_GPS_WKNROLLOVER:
//    {
//            VehicleSensGetWknRolloverG(pst_data);
//            break;
//    }
//        case POSHAL_DID_GPS_CLOCK_DRIFT:
//        {
//            VehicleSensGetGpsClockDriftG(pst_data);
//            break;
//        }
//        case POSHAL_DID_GPS_CLOCK_FREQ:
//        {
//            VehicleSensGetGpsClockFreqG(pst_data);
//            break;
//        }
//        default:
//            break;
//    }
//}

/*******************************************************************************
* MODULE    : VehicleSensGetDataMasterGyroTrouble
* ABSTRACT  : Vehicle Sensor Data Master Gyro Failure Status Get Processing
* FUNCTION  : Provide a gyro fault condition
* ARGUMENT  : ul_did    : Data ID
*           : uc_get_method  : Data collection way(Not used)
*           :          VEHICLESENS_GETMETHOD_CAN  :  CAN communication
*           :          VEHICLESENS_GETMETHOD_LINE  :  LineSensor drivers
*           :          VEHICLESENS_GETMETHOD_GPS  :  GPS
*           : *pst_data    : Pointer to the data master provider
* NOTE      :
* RETURN    : void
******************************************************************************/
void  VehicleSensGetDataMasterGyroTrouble(DID ul_did, u_int8 uc_get_method, VEHICLESENS_DATA_MASTER_GYRO_TROUBLE *pst_data) {  // LCOV_EXCL_START 8: dead code.  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (pst_data != NULL) {
        if (ul_did == VEHICLE_DID_GYRO_TROUBLE) {
            VehicleSensGetGyroTrouble(pst_data);
        } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "UNKNOWN DID(%x) \r\n", ul_did);
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "pst_data == NULL\r\n");
    }
}

/*******************************************************************************
* MODULE    : VehicleSensGetDataMasterSysGpsInterruptSignal
* ABSTRACT  : Vehicle Sensor Data Master SYS GPS Interrupt Signal Get Processing
* FUNCTION  : Provide SYS GPS interrupt signals
* ARGUMENT  : ul_did    : Data ID
*           : uc_get_method  : Data collection way(Not used)
*           :          VEHICLESENS_GETMETHOD_CAN  :  CAN communication
*           :          VEHICLESENS_GETMETHOD_LINE  :  LineSensor drivers
*           :          VEHICLESENS_GETMETHOD_GPS  :  GPS
*           : *pst_data    : Pointer to the data master provider
* NOTE      :
* RETURN    : void
******************************************************************************/
void  VehicleSensGetDataMasterSysGpsInterruptSignal(DID ul_did,
        u_int8 uc_get_method,
        VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL *pst_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (pst_data != NULL) {
        if (ul_did == VEHICLE_DID_SYS_GPS_INTERRUPT_SIGNAL) {
            VehicleSensGetSysGpsInterruptSignal(pst_data);
        } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "UNKNOWN DID(%x) \r\n", ul_did);
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "pst_data == NULL\r\n");
    }
}

/*******************************************************************************
* MODULE    : VehicleSensGetDataMasterMainGpsInterruptSignal
* ABSTRACT  : Vehicle Sensor Data Master MAIN GPS Interrupt Signal Get Processing
* FUNCTION  : Provide MAIN GPS interrupt signals
* ARGUMENT  : ul_did    : Data ID
*           : uc_get_method  : Data collection way(Not used)
*           :          VEHICLESENS_GETMETHOD_CAN  :  CAN communication
*           :          VEHICLESENS_GETMETHOD_LINE  :  LineSensor drivers
*           :          VEHICLESENS_GETMETHOD_GPS  :  GPS
*           : *pst_data    : Pointer to the data master provider
* NOTE      :
* RETURN    : void
******************************************************************************/
void  VehicleSensGetDataMasterMainGpsInterruptSignal(DID ul_did,
        u_int8 uc_get_method,
        VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL *pst_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (pst_data != NULL) {
        if (ul_did == VEHICLE_DID_MAIN_GPS_INTERRUPT_SIGNAL) {
            VehicleSensGetMainGpsInterruptSignal(pst_data);
        } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "UNKNOWN DID(%x) \r\n", ul_did);
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "pst_data == NULL\r\n");
    }
}

/*******************************************************************************
* MODULE    : VehicleSensGetDataMasterGyroConnectStatus
* ABSTRACT  : Vehicle Sensor Data Master Gyro Connection Status Get Processing
* FUNCTION  : Provide the status of the gyro connection
* ARGUMENT  : ul_did    : Data ID
*           : uc_get_method  : Data collection way(Not used)
*           :          VEHICLESENS_GETMETHOD_CAN  :  CAN communication
*           :          VEHICLESENS_GETMETHOD_LINE  :  LineSensor drivers
*           :          VEHICLESENS_GETMETHOD_GPS  :  GPS
*           : *pst_data    : Pointer to the data master provider
* NOTE      :
* RETURN    : void
******************************************************************************/
void  VehicleSensGetDataMasterGyroConnectStatus(DID ul_did,
        u_int8 uc_get_method,
        VEHICLESENS_DATA_MASTER_GYRO_CONNECT_STATUS *pst_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (pst_data != NULL) {
        if (ul_did == VEHICLE_DID_GYRO_CONNECT_STATUS) {
            VehicleSensGetGyroConnectStatus(pst_data);
        } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "UNKNOWN DID(%x)\r\n", ul_did);
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "pst_data == NULL\r\n");
    }
}

/*******************************************************************************
* MODULE    : VehicleSensGetDataMasterGpsAntennaStatus
* ABSTRACT  : Vehicle Sensor Data Master GPS Antenna Connection Status Get Processing
* FUNCTION  : Provide GPS antenna connection status
* ARGUMENT  : ul_did    : Data ID
*           : uc_get_method  : Data collection way(Not used)
*           :          VEHICLESENS_GETMETHOD_CAN  :  CAN communication
*           :          VEHICLESENS_GETMETHOD_LINE  :  LineSensor drivers
*           :          VEHICLESENS_GETMETHOD_GPS  :  GPS
*           : *pst_data    : Pointer to the data master provider
* NOTE      :
* RETURN    : void
******************************************************************************/
void  VehicleSensGetDataMasterGpsAntennaStatus(DID ul_did,
        u_int8 uc_get_method,
        VEHICLESENS_DATA_MASTER_GPS_ANTENNA_STATUS *pst_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (pst_data != NULL) {
//        if (ul_did == POSHAL_DID_GPS_ANTENNA) {
//            VehicleSensGetGpsAntennaStatus(pst_data);
//        } else {
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "UNKNOWN DID(%x) \r\n", ul_did);
//        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "pst_data == NULL\r\n");
    }
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Sensor counter acquisition
 *
 * @return  Sensor Counter
 *
 */
//static uint8_t VehicleSensGetSensorCnt(void) {
//    VEHICLESENS_DATA_MASTER      st_data;
//    uint8_t sensCnt;
//
//    /* Synchronous counter acquisition */
//    VehicleSensGetSnsCounterl(&st_data);
//
//    sensCnt = st_data.uc_data[0];
//
//    return sensCnt;
//}
