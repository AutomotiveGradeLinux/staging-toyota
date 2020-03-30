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
 *  File name    :VehicleSens_Did_GsnsX.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(VEHICLE_DID_GSNS_X)
 *  Module configuration  :VehicleSensGetGsnsX()    Vehicle sensor GSNS_X GET function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/

/*******************************************************************************
* MODULE    : VehicleSensGetGsnsX
* ABSTRACT  : Vehicle sensor GSNS_X GET function
* FUNCTION  : Provide the GSNS_X data master
* ARGUMENT  : *pst_data    : Pointer to the data master acquisition destination
*             uc_get_method  : Acquisition method(Direct Line or CAN)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGsnsX(VEHICLESENS_DATA_MASTER *pst_data, u_int8 uc_get_method) {
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
            VehicleSensGetGsnsXl(pst_data);
            break;
        }
        default:  /* Ignore->MISRA-C++:2008 Rule 6-3-1, 6-4-1 */
            break;
    }
}

/* Ignore->MISRA-C++:2008 Rule 2-7-2 */
#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*******************************************************************************
* MODULE    : VehicleSensGetGsnsXExt
* ABSTRACT  : Vehicle sensor GSNS_X GET function
* FUNCTION  : Provide the GSNS_X data master
* ARGUMENT  : *pst_data    : Pointer to the data master acquisition destination
*             uc_get_method  : Acquisition method(Direct Line or CAN)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGsnsXExt(VEHICLESENS_DATA_MASTER_EXT *pst_data, u_int8 uc_get_method) {
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
            VehicleSensGetGsnsXExtl(pst_data);
            break;
        }
        default:  /* Ignore->MISRA-C++:2008 Rule 6-3-1, 6-4-1 */
            break;
    }
}
#endif
/* Ignore->MISRA-C++:2008 Rule 2-7-2 */

/**
 * @brief
 *   Vehicle sensor GSNS_X GET function
 *
 *   Provide the GSNS_X data master
 *
 * @param[in]  *pst_data: Pointer to the data master acquisition destination
 * @param[in]  uc_get_method: Acquisition method(Direct Line or CAN)
 */
//void VehicleSensGetGsnsXFst(VEHICLESENS_DATA_MASTER_FST *pst_data, u_int8 uc_get_method) {
//    switch (uc_get_method) {    // LCOV_EXCL_BR_LINE 6:VEHICLESENS_GETMETHOD_CAN cannot to pass in
//        case VEHICLESENS_GETMETHOD_CAN:
//        {
//            /* When acquiring from CAN data */
//            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//            break;  // LCOV_EXCL_LINE 8: dead code
//        }
//        case VEHICLESENS_GETMETHOD_LINE:
//        {
//            /* To acquire from LineSensor */
//            VehicleSensGetGsnsXFstl(pst_data);
//            break;
//        }
//        default:
//            break;
//    }
//}

