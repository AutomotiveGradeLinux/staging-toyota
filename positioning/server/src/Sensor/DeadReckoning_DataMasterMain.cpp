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
 *  File name    :DeadReckoning_DataMasterMain.cpp
 *  System name    :GPF
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle SW Data Master
 *  Module configuration  :DeadReckoningInitDataMaster()    Guess Navigation Data Master Initialization Function
 *          :DeadReckoningSetDataMaster()      Estimated Navigational Data Master SW Data Set Processing
 *          :DeadReckoningGetDataMaster()      Estimated Navigational Data Master Get Processing
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "DeadReckoning_DataMaster.h"

/*************************************************/
/*       Global variable                           */
/*************************************************/

#define DR_DEBUG 0

/*******************************************************************************
* MODULE   : DeadReckoningInitDataMaster
* ABSTRACT  : Initialization of Guess Navigation Data Master
* FUNCTION  : Initialize the estimated navigation data master
* ARGUMENT  : void
* NOTE    :
* RETURN   : void
******************************************************************************/
void DeadReckoningInitDataMaster(void) {  // LCOV_EXCL_START 8: dead code.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* Vehicle sensor data master initialization */
    DeadReckoningInitLongitudeDr();
    DeadReckoningInitLatitudeDr();
    DeadReckoningInitAltitudeDr();
    DeadReckoningInitSpeedDr();
    DeadReckoningInitHeadingDr();
    DeadReckoningInitSnsCounterDr();
    DeadReckoningInitGyroOffsetDr();
    DeadReckoningInitGyroScaleFactorDr();
    DeadReckoningInitGyroScaleFactorLevelDr();
    DeadReckoningInitSpeedPulseScaleFactorDr();
    DeadReckoningInitSpeedPulseScaleFactorLevelDr();
}

/*******************************************************************************
* MODULE   : DeadReckoning_SetDataMaster_Sub
* ABSTRACT  : Estimated Navigational Data Master SW Data Set Processing
* FUNCTION  : Set estimated navigation data
* ARGUMENT  : *p_data      : SW vehicle signal notification data
*       : p_datamaster_set_n  : Data Master Set Notification(Callback function)
* NOTE    :
* RETURN   : void
******************************************************************************/
void DeadReckoningSetDataMaster(const DEADRECKONING_DATA_MASTER *p_data,
                                PFUNC_DR_DMASTER_SET_N p_datamaster_set_n) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8  chg_type;

    static u_int8  gyro_parameter_chg_type      = 0;
    static u_int8  speedpulse_parameter_chg_type  = 0;

    /*------------------------------------------------------*/
    /*  Call the data set processing associated with the DID      */
    /*  Call the data master set notification process      */
    /*------------------------------------------------------*/
    switch (p_data->ul_did) {
        case VEHICLE_DID_DR_LONGITUDE:
        {
#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningSetDataMaster DID=[0x%08X] DSIZE=[%d] \r\n",
                          p_data->ul_did, p_data->us_size);
#endif
            chg_type = DeadReckoningSetLongitudeDr(p_data);
            /* Implementation of delivery process at LATITUDE updating timings */
            /* Since the order of transmission (updating) at the main receiver is fixed, there is no problem. */
            break;
        }
        case VEHICLE_DID_DR_LATITUDE:
        {
#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningSetDataMaster DID=[0x%08X] DSIZE=[%d] \r\n",
                          p_data->ul_did, p_data->us_size);
#endif
            chg_type = DeadReckoningSetLatitudeDr(p_data);
            (*p_datamaster_set_n)(p_data->ul_did, chg_type, DEADRECKONING_GETMETHOD_DR);
            break;
        }
        case VEHICLE_DID_DR_ALTITUDE:
        {
#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningSetDataMaster DID=[0x%08X] DSIZE=[%d] \r\n",
                          p_data->ul_did, p_data->us_size);
#endif
            chg_type = DeadReckoningSetAltitudeDr(p_data);
            (*p_datamaster_set_n)(p_data->ul_did, chg_type, DEADRECKONING_GETMETHOD_DR);
            break;
        }
        case VEHICLE_DID_DR_SPEED:
        {
#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningSetDataMaster DID=[0x%08X] DSIZE=[%d] \r\n",
                           p_data->ul_did, p_data->us_size);
#endif
            chg_type = DeadReckoningSetSpeedDr(p_data);
            (*p_datamaster_set_n)(p_data->ul_did, chg_type, DEADRECKONING_GETMETHOD_DR);
            break;
        }
        case VEHICLE_DID_DR_HEADING:
        {
#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningSetDataMaster DID=[0x%08X] DSIZE=[%d] \r\n",
                          p_data->ul_did, p_data->us_size);
#endif
            chg_type = DeadReckoningSetHeadingDr(p_data);
            (*p_datamaster_set_n)(p_data->ul_did, chg_type, DEADRECKONING_GETMETHOD_DR);
            break;
        }
        case VEHICLE_DID_DR_SNS_COUNTER:
        {
#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningSetDataMaster DID=[0x%08X] DSIZE=[%d] \r\n",
                          p_data->ul_did, p_data->us_size);
#endif
            chg_type = DeadReckoningSetSnsCounterDr(p_data);
            (*p_datamaster_set_n)(p_data->ul_did, chg_type, DEADRECKONING_GETMETHOD_DR);
            break;
        }
        case VEHICLE_DID_DR_GYRO_OFFSET:
        {
#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningSetDataMaster DID=[0x%08X] DSIZE=[%d] \r\n",
                          p_data->ul_did, p_data->us_size);
#endif
            chg_type = DeadReckoningSetGyroOffsetDr(p_data);
            /* Distribution processing not performed in this DID                                */
            /* Delivery processing is executed when VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL is updated.        */
            /* The GyroParameter order is defined by DeadReckoning_RcvMsg().              */
            if (chg_type == DEADRECKONING_NEQ) {
                gyro_parameter_chg_type = DEADRECKONING_NEQ;
            }
            break;
        }
        case VEHICLE_DID_DR_GYRO_SCALE_FACTOR:
        {
#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningSetDataMaster DID=[0x%08X] DSIZE=[%d] \r\n",
                          p_data->ul_did, p_data->us_size);
#endif
            chg_type = DeadReckoningSetGyroScaleFactorDr(p_data);
            /* Distribution processing not performed in this DID                                */
            /* Delivery processing is executed when VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL is updated.        */
            /* The GyroParameter order is defined by DeadReckoning_RcvMsg().              */
            if (chg_type == DEADRECKONING_NEQ) {
                gyro_parameter_chg_type = DEADRECKONING_NEQ;
            }
            break;
        }
        case VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL:
        {
#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningSetDataMaster DID=[0x%08X] DSIZE=[%d] \r\n",
                          p_data->ul_did, p_data->us_size);
#endif
            chg_type = DeadReckoningSetGyroScaleFactorLevelDr(p_data);
            if (gyro_parameter_chg_type == DEADRECKONING_NEQ) {
                chg_type        = DEADRECKONING_NEQ;
                gyro_parameter_chg_type  = DEADRECKONING_EQ;
            }
            (*p_datamaster_set_n)(p_data->ul_did, chg_type, DEADRECKONING_GETMETHOD_DR);
            break;
        }
        case VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR:
        {
#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningSetDataMaster DID=[0x%08X] DSIZE=[%d] \r\n",
                          p_data->ul_did, p_data->us_size);
#endif
            chg_type = DeadReckoningSetSpeedPulseScaleFactorDr(p_data);
            /* Distribution processing not performed in this DID                                */
            /* Delivery processing is executed when VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR_LEVEL is updated.    */
            /* The SpeedPulseParameter order is defined by DeadReckoning_RcvMsg().          */
            if (chg_type == DEADRECKONING_NEQ) {
                speedpulse_parameter_chg_type = DEADRECKONING_NEQ;
            }
            break;
        }
        case VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR_LEVEL:
        {
#if (DR_DEBUG == 1)  /* PastModel002_DR_VEHICLE  */
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningSetDataMaster DID=[0x%08X] DSIZE=[%d] \r\n",
                          p_data->ul_did, p_data->us_size);
#endif
            chg_type = DeadReckoningSetSpeedPulseScaleFactorLevelDr(p_data);
            if (speedpulse_parameter_chg_type == DEADRECKONING_NEQ) {
                chg_type            = DEADRECKONING_NEQ;
                speedpulse_parameter_chg_type  = DEADRECKONING_EQ;
            }
            (*p_datamaster_set_n)(p_data->ul_did, chg_type, DEADRECKONING_GETMETHOD_DR);
            break;
        }
        default:
            break;
    }
}

/*******************************************************************************
* MODULE   : DeadReckoningGetDataMaster
* ABSTRACT  : Estimated Navigational Data Master Get Processing
* FUNCTION  : Provide an estimated navigation data master
* ARGUMENT  : ul_did    : Data ID corresponding to the vehicle information
*       : *p_data    : Pointer to the data master provider
* NOTE    :
* RETURN   : void
******************************************************************************/
void DeadReckoningGetDataMaster(DID ul_did, DEADRECKONING_DATA_MASTER *p_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /*------------------------------------------------------*/
    /*  Call the data Get processing associated with the DID        */
    /*------------------------------------------------------*/
    switch (ul_did) {
        /*------------------------------------------------------*/
        /*  Vehicle sensor data group                                   */
        /*------------------------------------------------------*/
        case VEHICLE_DID_DR_LONGITUDE:
        {
            DeadReckoningGetLongitudeDr(p_data);
            break;
        }
        case VEHICLE_DID_DR_LATITUDE:
        {
            DeadReckoningGetLatitudeDr(p_data);
            break;
        }
        case VEHICLE_DID_DR_ALTITUDE:
        {
            DeadReckoningGetAltitudeDr(p_data);
            break;
        }
        case VEHICLE_DID_DR_SPEED:
        {
            DeadReckoningGetSpeedDr(p_data);
            break;
        }
        case VEHICLE_DID_DR_HEADING:
        {
            DeadReckoningGetHeadingDr(p_data);
            break;
        }
        case VEHICLE_DID_DR_SNS_COUNTER:
        {
            DeadReckoningGetSnsCounterDr(p_data);
            break;
        }
        case VEHICLE_DID_DR_GYRO_OFFSET:
        {
            DeadReckoningGetGyroOffsetDr(p_data);
            break;
        }
        case VEHICLE_DID_DR_GYRO_SCALE_FACTOR:
        {
            DeadReckoningGetGyroScaleFactorDr(p_data);
            break;
        }
        case VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL:
        {
            DeadReckoningGetGyroScaleFactorLevelDr(p_data);
            break;
        }
        case VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR:
        {
            DeadReckoningGetSpeedPulseScaleFactorDr(p_data);
            break;
        }
        case VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR_LEVEL:
        {
            DeadReckoningGetSpeedPulseScaleFactorLevelDr(p_data);
            break;
        }
        default:
            break;
    }
}
// LCOV_EXCL_STOP
