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
 * @file
 *  VehicleSens_Did_PulseTime.cpp
 * @brief
 *  Vehicle sensor data master(VEHICLE_DID_PULSE_TIME)
 */

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/

/**
 * @brief
 *   Vehicle sensor pulse time GET function
 *
 *   Provide interpulse time data master
 *
 * @param[in]   *pst_data: Pointer to the data master acquisition destination
 * @param[in]   uc_get_method: Acquisition method(Direct Line or CAN)
 */
void VehicleSensGetPulseTime(VEHICLESENS_DATA_MASTER *pst_data, u_int8 uc_get_method) {
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
            VehicleSensGetPulseTimel(pst_data);
            break;
        }
        default:
            break;
    }
}

/**
 * @brief
 *   Vehicle sensor inter-pulse time (initial delivery) GET function
 *
 *   Provide interpulse time data master
 *
 * @param[in]   *pst_data: Pointer to the data master acquisition destination
 * @param[in]   uc_get_method: Acquisition method(Direct Line or CAN)
 */
void VehicleSensGetPulseTimeExt(VEHICLESENS_DATA_MASTER_EXT *pst_data, u_int8 uc_get_method) {
    switch (uc_get_method) {    // LCOV_EXCL_BR_LINE 6:VEHICLESENS_GETMETHOD_CAN cannot to pass in
        case VEHICLESENS_GETMETHOD_CAN:
        {
            /* When acquiring from CAN data */
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            break;  // LCOV_EXCL_LINE 8: dead code
        }

        case VEHICLESENS_GETMETHOD_LINE:
        {
            /* To acquire from LineSensor */
            VehicleSensGetPulseTimeExtl(pst_data);
            break;
        }
        default:
            break;
    }
}

/**
 * @brief
 *   Vehicle sensor pulse time (initial sensor) GET function
 *
 *   Provide interpulse time data master
 *
 * @param[in]   *pst_data: Pointer to the data master acquisition destination
 * @param[in]   uc_get_method: Acquisition method(Direct Line or CAN)
 */

void VehicleSensGetPulseTimeFst(VEHICLESENS_DATA_MASTER_FST *pst_data, u_int8 uc_get_method) {  // LCOV_EXCL_START 8: dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    switch (uc_get_method) {
        case VEHICLESENS_GETMETHOD_CAN:
        {
            /* When acquiring from CAN data */
            break;
        }
        case VEHICLESENS_GETMETHOD_LINE:
        {
            /* To acquire from LineSensor */
            break;
        }
        default:
            break;
    }
}
// LCOV_EXCL_STOP
