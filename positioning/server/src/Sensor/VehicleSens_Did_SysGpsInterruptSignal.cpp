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
 *  File name    :VehicleSens_Did_SysGpsInterruptSignal.cpp
 *  System name    :PastModel002
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(VEHICLE_DID_SYS_GPS_INTERRUPT_SIGNAL)
 *  Module configuration  :VehicleSensInitSysGpsInterruptSignal()    Vehicle sensor Sys_GPS_INTERRUPT_SIGNAL initialization function
 *                  :VehicleSensSetSysGpsInterruptSignal()    Vehicle Sensor Sys_GPS_INTERRUPT_SIGNAL SET Function
 *                  :VehicleSensGetSysGpsInterruptSignal()    Vehicle sensor Sys_GPS_INTERRUPT_SIGNAL GET function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

#define VEHICLE_SENS_DID_SYS_GPS_INTERRUPT_SIGNAL_DEBUG_FACTORY 0

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL  gstSysGpsInterruptSignal;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitSysGpsInterruptSignal
* ABSTRACT  : Vehicle sensor Sys_GPS_INTERRUPT_SIGNAL initialization function
* FUNCTION  : Sys_GPS_INTERRUPT_SIGNAL data master initialization processing
* ARGUMENT  : None
* NOTE      :
* RETURN    : None
******************************************************************************/
void VehicleSensInitSysGpsInterruptSignal(void) {
    (void)memset(reinterpret_cast<void *>(&(gstSysGpsInterruptSignal)), static_cast<int>(0x00),
                        sizeof(VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL));
    gstSysGpsInterruptSignal.ul_did  = VEHICLE_DID_SYS_GPS_INTERRUPT_SIGNAL;
//    gstSysGpsInterruptSignal.us_size  = VEHICLE_DSIZE_SYS_GPS_INTERRUPT_SIGNAL;
    gstSysGpsInterruptSignal.uc_data  = VEHICLE_DINIT_SYS_GPS_INTERRUPT_SIGNAL;

#if VEHICLE_SENS_DID_SYS_GPS_INTERRUPT_SIGNAL_DEBUG_FACTORY
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
        "#[FACTORY] gstSysGpsInterruptSignal.uc_data == 0x%x\r\n", gstSysGpsInterruptSignal.uc_data);
#endif
}

/*******************************************************************************
* MODULE    : VehicleSensSetSysGpsInterruptSignal
* ABSTRACT  : Vehicle sensor SYS_GPS_INTERRUPT_SIGNAL SET function
* FUNCTION  : Update the Sys_GPS_INTERRUPT_SIGNAL data master
* ARGUMENT  : *pst_data : Pointer to received message data
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
//u_int8 VehicleSensSetSysGpsInterruptSignal(const LSDRV_MSG_LSDATA_DAT_GPS_INTERRUPT_SIGNAL *pst_data) {  // LCOV_EXCL_START 8: dead code  // NOLINT(whitespace/line_length)
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    u_int8 uc_ret = VEHICLESENS_EQ;
//    VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL *pst_master;
//
//    if (pst_data == NULL) {
//        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "pst_data == NULL\r\n");
//    } else {
//        pst_master = &gstSysGpsInterruptSignal;
//
//        /* Compare data master and received data */
//        uc_ret = VehicleSensmemcmp(reinterpret_cast<void *>(&(pst_master->uc_data)),
//            (const void *)(&(pst_data->uc_data)), (size_t)(pst_data->uc_size));
//
//        /* Received data is set in the data master. */
//        pst_master->ul_did    = pst_data->ul_did;
//        pst_master->us_size    = (u_int16)pst_data->uc_size;
//        pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
//        (void)memcpy(reinterpret_cast<void *>(&(pst_master->uc_data)),
//            (const void *)(&(pst_data->uc_data)), sizeof(pst_master->uc_data));
//
//#if VEHICLE_SENS_DID_SYS_GPS_INTERRUPT_SIGNAL_DEBUG_FACTORY
//        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//            "#[FACTORY] pst_data->ul_did == 0x%x", pst_data->ul_did);
//        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//            "#[FACTORY] gstSysGpsInterruptSignal.ul_did == 0x%x\r\n", gstSysGpsInterruptSignal.ul_did);
//        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//            "#[FACTORY] (u_int8)pst_data->ucSize == 0x%x", (u_int8)pst_data->uc_size);
//        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//            "#[FACTORY] gstSysGpsInterruptSignal.us_size == 0x%x\r\n", gstSysGpsInterruptSignal.us_size);
//        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//            "#[FACTORY] pst_data->uc_data == 0x%x", pst_data->uc_data);
//        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//            "#[FACTORY] gstSysGpsInterruptSignal.uc_data == 0x%x\r\n", gstSysGpsInterruptSignal.uc_data);
//#endif
//    }
//
//    return(uc_ret);
//}

/*******************************************************************************
* MODULE    : VehicleSensGetSysGpsInterruptSignal
* ABSTRACT  : Vehicle sensor SYS_GPS_INTERRUPT_SIGNAL GET function
* FUNCTION  : Provide the SYS_GPS_INTERRUPT_SIGNAL data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : None
******************************************************************************/
void VehicleSensGetSysGpsInterruptSignal(VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL *pst_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    const VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL *pst_master;

    if (pst_data == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "pst_data == NULL\r\n");
    } else {
        pst_master = &gstSysGpsInterruptSignal;

        /* Store the data master in the specified destination. */
        pst_data->ul_did    = pst_master->ul_did;
        pst_data->us_size    = pst_master->us_size;
        pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
        (void)memcpy(reinterpret_cast<void *>(&(pst_data->uc_data)),
            (const void *)(&(pst_master->uc_data)), sizeof(pst_data->uc_data));
    }
}
// LCOV_EXCL_STOP
