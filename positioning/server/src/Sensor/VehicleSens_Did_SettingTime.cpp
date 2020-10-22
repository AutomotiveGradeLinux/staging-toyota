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
 *   VehicleSens_Did_SettingTime.cpp
 * @brief
 */
/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*---------------------------------------------------------------------------------*
 * Global Value                                                                    *
 *---------------------------------------------------------------------------------*/

/**
 * @brief
 *  GPS setting time information data master GET process
 *
 *  Provide the GPS setting time information data master
 *
 * @param[in]  u_int8                   uc_get_method : Acquisition method(NAVI)
 * @param[out] VEHICLESENS_DATA_MASTER *pst_data     : Pointer to the data master acquisition destination
 *
 * @return  none
 * @retval  none
 */
void VehicleSensGetSettingTime(VEHICLESENS_DATA_MASTER *pst_data, u_int8 uc_get_method) {
    switch (uc_get_method) {  // LCOV_EXCL_BR_LINE 6:other parameters cannot pass in
        case VEHICLESENS_GETMETHOD_OTHER:
        {
            /** Acquiring from Clock */
            VehicleSensGetSettingTimeclock(pst_data);
            break;
        }
        default:  /* Ignore->MISRA-C++:2008 Rule 6-3-1, 6-4-1 */
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            break;  // LCOV_EXCL_LINE 8: dead code
    }

    return;
}
