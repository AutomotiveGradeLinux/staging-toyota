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
 *  VehicleSens_Did_MotionSpeed.cpp
 * @brief
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   Speed information data master GET processing
 *
 * @param[out]  *pst_data     - VEHICLESENS_DATA_MASTER Pointer to the data master acquisition destination
 * @param[in]  uc_get_method  - u_int8 acquisition method
 */
void VehicleSensGetMotionSpeed(VEHICLESENS_DATA_MASTER *pst_data, u_int8 uc_get_method) {
    switch (uc_get_method) {  // LCOV_EXCL_BR_LINE 6:other parameters cannot pass in
        case VEHICLESENS_GETMETHOD_INTERNAL:
        {
            /** When acquiring internal calculation data */
            VehicleSensGetMotionSpeedI(pst_data);
        }
        break;
        case VEHICLESENS_GETMETHOD_NAVI:
        {
            /** To acquire NAVI data */
            VehicleSensGetMotionSpeedN(pst_data);
            break;
        }
        default:
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            break;  // LCOV_EXCL_LINE 8: dead code
    }
}
