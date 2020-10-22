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

/*******************************************************************************
 *  File name    :VehicleSens_Did_Rev.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(VEHICLE_DID_REV)
 *  Module configuration  :VehicleSensGetRev()    Vehicle Sensor REV GET Functions
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/

/*******************************************************************************
* MODULE    : VehicleSensGetRev
* ABSTRACT  : Vehicle Sensor REV GET Functions
* FUNCTION  : Provide a REV data master
* ARGUMENT  : *pst_data    : Pointer to the data master acquisition destination
*             uc_get_method  : Acquisition method(Direct Line or CAN)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetRev(VEHICLESENS_DATA_MASTER *pst_data, u_int8 uc_get_method) {
    switch (uc_get_method) {  // LCOV_EXCL_BR_LINE 6:VEHICLESENS_GETMETHOD_CAN cannot to pass in
        case VEHICLESENS_GETMETHOD_CAN:
        {
            /* When acquiring from CAN data */
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            break;  // LCOV_EXCL_LINE 8: dead code
        }
        case VEHICLESENS_GETMETHOD_LINE:
        {
            /* To acquire from LineSensor */
            VehicleSensGetRevl(pst_data);
            break;
        }
        default:
            break;
    }
}

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*******************************************************************************
* MODULE    : VehicleSensGetRevFst
* ABSTRACT  : Vehicle sensor GET function
* FUNCTION  : Provide a data master
* ARGUMENT  : *pst_data    : Pointer to the data master acquisition destination
*             uc_get_method  : Acquisition method(Direct Line or CAN)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetRevFst(VEHICLESENS_DATA_MASTER_FST *pst_data, u_int8 uc_get_method) {
    switch (uc_get_method) {  // LCOV_EXCL_BR_LINE 6:VEHICLESENS_GETMETHOD_CAN cannot to pass in
        case VEHICLESENS_GETMETHOD_CAN:
        {
            /* When acquiring from CAN data */
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            break;  // LCOV_EXCL_LINE 8: dead code
        }
        case VEHICLESENS_GETMETHOD_LINE:
        {
            /* To acquire from LineSensor */
            VehicleSensGetRevFstl(pst_data);
            break;
        }
        default:
            break;
    }
}
#endif

/**
 * @brief
 *   Vehicle Sensor REV GET Functions
 *
 *   Provide a REV data master
 *
 * @param[in]  *pst_data: Pointer to the data master acquisition destination
 * @param[in]  uc_get_method: Acquisition method(Direct Line or CAN)
 *
 * @return none
 */
void VehicleSensGetRevExt(VEHICLESENS_DATA_MASTER_EXT *pst_data, u_int8 uc_get_method) {
    switch (uc_get_method) {  // LCOV_EXCL_BR_LINE 6:VEHICLESENS_GETMETHOD_CAN cannot to pass in
        case VEHICLESENS_GETMETHOD_CAN:
        {
            /* When acquiring from CAN data */
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            break;  // LCOV_EXCL_LINE 8: dead code
        }
        case VEHICLESENS_GETMETHOD_LINE:
        {
            /* To acquire from LineSensor */
            VehicleSensGetRevExtl(pst_data);
            break;
        }
        default:  /* Ignore->MISRA-C++:2008 Rule 6-3-1, 6-4-1 */
            break;
    }
}
