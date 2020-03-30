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
 *  File name    :VehicleSens_Did_RevFst_l.cpp
 *  System name    :PastModel002
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_REV_FST)
 *  Module configuration  :VehicleSensInitRevFstl()  Vehicle Sensor REV Initialization Functions
 *                  :VehicleSensSetRevFstl()    Vehicle Sensor REV SET Functions
 *                  :VehicleSensSetRevFstG()    Vehicle Sensor REV SET Functions
 *                  :VehicleSensGetRevFstl()    Vehicle Sensor REV GET Functions
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_FST  gstRevFst_l;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitRevFstl
* ABSTRACT  : Vehicle Sensor REV Initialization Functions
* FUNCTION  : REV data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitRevFstl(void) {
    (void)memset(reinterpret_cast<void *>(&gstRevFst_l), 0, sizeof (VEHICLESENS_DATA_MASTER_FST));
//    gstRevFst_l.ul_did      = POSHAL_DID_REV_FST;
    gstRevFst_l.us_size      = 0U;
    gstRevFst_l.uc_rcvflag    = 0U;
    gstRevFst_l.partition_flg  = 0;
}

/*******************************************************************************
* MODULE    : VehicleSensSetRevFstl
* ABSTRACT  : Vehicle Sensor REV SET Functions
* FUNCTION  : Update the REV data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
//u_int8 VehicleSensSetRevFstl(const LSDRV_LSDATA_FST *pst_data) {  // LCOV_EXCL_START 8 : dead code
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    u_int8 uc_ret;
//    VEHICLESENS_DATA_MASTER_FST *pst_master;
//
//    pst_master = &gstRevFst_l;
//
//    /* Compare data master and received data */
//    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);
//
//    /* Received data is set in the data master. */
//    pst_master->ul_did    = pst_data->ul_did;
//    pst_master->us_size    = pst_data->uc_size;
//    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
//    (void)memcpy(reinterpret_cast<void *>(pst_master->uc_data),
//        (const void *)(pst_data->uc_data), (size_t)(pst_data->uc_size));
//
//    return(uc_ret);
//}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensSetRevFstG
* ABSTRACT  : Vehicle Sensor REV SET Functions
* FUNCTION  : Update the REV data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
//u_int8 VehicleSensSetRevFstG(const LSDRV_LSDATA_FST_REV *pst_data) {
//    static u_int8 uc_ret = VEHICLESENS_EQ;
//    VEHICLESENS_DATA_MASTER_FST *pst_master;
//
//    pst_master = &gstRevFst_l;
//
//    u_int8  partition_max;    /* Total number of partitions            */
//    u_int8  partition_num;    /* Data number          */
//
//    partition_max = pst_data->uc_partition_max;
//    partition_num = pst_data->uc_partition_num;
//
//    if (partition_max == 1) {  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//        /* Compare data master and received data */
//        uc_ret = VehicleSensmemcmp(pst_master->uc_data,
//                pst_data->uc_data, pst_data->uc_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//
//        /* Received data is set in the data master. */
//        pst_master->ul_did      = pst_data->ul_did;
//        pst_master->us_size      = pst_data->uc_size;
//        pst_master->uc_rcvflag    = VEHICLE_RCVFLAG_ON;
//        pst_master->partition_flg  = 0;
//        memset(pst_master->uc_data, 0x00, sizeof(pst_master->uc_data));
//        memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//    } else if (partition_max == 2) {  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//        if (partition_num == 1) {  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            /* Compare data master and received data */
//            /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);
//
//            /* Received data is set in the data master. */
//            pst_master->ul_did      = pst_data->ul_did;
//            /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            pst_master->us_size = static_cast<u_int16>(pst_master->us_size + pst_data->uc_size);
//            pst_master->partition_flg  = 1;
//            memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//        } else if (partition_num == 2) {  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            /* Compare data master and received data */
//            if (uc_ret == VEHICLESENS_EQ) {
//                uc_ret = VehicleSensmemcmp(&pst_master->uc_data[VEHICLE_DSIZE_REV_FST],
//                            pst_data->uc_data, pst_data->uc_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            }
//
//            /* Received data is set in the data master. */
//            pst_master->ul_did      = pst_data->ul_did;
//            /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            pst_master->us_size = static_cast<u_int16>(pst_master->us_size + pst_data->uc_size);
//            pst_master->uc_rcvflag    = VEHICLE_RCVFLAG_ON;
//            pst_master->partition_flg  = 1;
//            memcpy(&pst_master->uc_data[VEHICLE_DSIZE_REV_FST],
//                            pst_data->uc_data, pst_data->uc_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//        } else {}
//    } else {}
//
//    return(uc_ret);
//}

/*******************************************************************************
* MODULE    : VehicleSensGetRevFstl
* ABSTRACT  : Vehicle Sensor REV GET Functions
* FUNCTION  : Provide a REV data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetRevFstl(VEHICLESENS_DATA_MASTER_FST *pst_data) {
    const VEHICLESENS_DATA_MASTER_FST *pst_master;

    pst_master = &gstRevFst_l;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    pst_data->partition_flg    = pst_master->partition_flg;
    (void)memcpy(reinterpret_cast<void *>(pst_data->uc_data),
        (const void *)(pst_master->uc_data), (size_t)(pst_master->us_size));
}

#endif
