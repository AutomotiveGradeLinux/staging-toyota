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
 *  File name    :VehicleSens_Did_SpeedPulseFlagFst_l.cpp
 *  System name    :PastModel002
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_SPEED_PULSE_FLAG_FST)
 *  Module configuration  :VehicleSensInitSpeedPulseFlagFstl()    Vehicle sensor SPEED_PULSE_FLAG initialization function
 *                  :VehicleSensSetSpeedPulseFlagFstl()    Vehicle Sensor SPEED_PULSE_FLAG SET Function
 *                  :VehicleSensSetSpeedPulseFlagFstG()    Vehicle Sensor SPEED_PULSE_FLAG SET Function
 *                  :VehicleSensGetSpeedPulseFlagFstl()    Vehicle Sensor SPEED_PULSE_FLAG GET Function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/* #Polaris_003 START */
#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Response */
/*************************************************/
/*           Global variable                      */
/*************************************************/

/*******************************************************************************
* MODULE    : VehicleSensInitSpeedPulseFlagFstl
* ABSTRACT  : Vehicle sensor SPEED_PULSE_FLAG initialization function
* FUNCTION  : SPEED_PULSE_FLAG data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitSpeedPulseFlagFstl(void) {
    return;
}

/*******************************************************************************
* MODULE    : VehicleSensSetSpeedPulseFlagFstl
* ABSTRACT  : Vehicle Sensor SPEED_PULSE_FLAG SET Function
* FUNCTION  : Update the SPEED_PULSE_FLAG data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
//u_int8 VehicleSensSetSpeedPulseFlagFstl(const LSDRV_LSDATA_FST *pst_data) {  // LCOV_EXCL_START 8: dead code
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    return(VEHICLESENS_EQ);
//}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensSetSpeedPulseFlagFstG
* ABSTRACT  : Vehicle Sensor SPEED_PULSE_FLAG SET Function
* FUNCTION  : Update the SPEED_PULSE_FLAG data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
//u_int8 VehicleSensSetSpeedPulseFlagFstG(const LSDRV_LSDATA_FST_SPEED_PULSE_FLAG *pst_data) {
//    return VEHICLESENS_EQ;
//}

/*******************************************************************************
* MODULE    : VehicleSensGetSpeedPulseFlagFstl
* ABSTRACT  : Vehicle Sensor SPEED_PULSE_FLAG GET Function
* FUNCTION  : Provide the SPEED_PULSE_FLAG data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetSpeedPulseFlagFstl(VEHICLESENS_DATA_MASTER_FST *pst_data) {  // LCOV_EXCL_START 8: dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return;
}

#endif
// LCOV_EXCL_STOP
