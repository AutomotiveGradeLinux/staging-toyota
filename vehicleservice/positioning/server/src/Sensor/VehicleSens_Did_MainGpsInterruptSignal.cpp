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
 *  File name    :VehicleSens_Did_MainGpsInterruptSignal.cpp
 *  System name    :PastModel002
 *  Subsystem name  :GPS process
 *  Program name  :MAIN GPS interrupt data master(VEHICLE_DID_MAIN_GPS_INTERRUPT_SIGNAL)
 *  Module configuration  :VehicleSensInitMainGpsInterruptSignal()  Vehicle sensor MAIN_GPS_INTERRUPT_SIGNAL initialization function
 *                  :VehicleSensSetMainGpsInterruptSignal()    Vehicle sensor MAIN_GPS_INTERRUPT_SIGNAL SET function
 *                  :VehicleSensGetMainGpsInterruptSignal()    Vehicle sensor MAIN_GPS_INTERRUPT_SIGNAL GET function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

#define VEHICLE_SENS_DID_MAIN_GPS_INTERRUPT_SIGNAL_DEBUG_FACTORY 0

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL  gstMainGpsInterruptSignal;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitMainGpsInterruptSignal
* ABSTRACT  : Vehicle sensor MAIN_GPS_INTERRUPT_SIGNAL initialization function
* FUNCTION  : MAIN_GPS_INTERRUPT_SIGNAL data master initialization processing
* ARGUMENT  : None
* NOTE      :
* RETURN    : None
******************************************************************************/
void VehicleSensInitMainGpsInterruptSignal(void) {
    (void)memset(reinterpret_cast<void *>(&(gstMainGpsInterruptSignal)),
        static_cast<int>(0x00), sizeof(VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL));
    gstMainGpsInterruptSignal.ul_did    = VEHICLE_DID_MAIN_GPS_INTERRUPT_SIGNAL;
    gstMainGpsInterruptSignal.us_size  = VEHICLE_DSIZE_MAIN_GPS_INTERRUPT_SIGNAL;
    gstMainGpsInterruptSignal.uc_data  = VEHICLE_DINIT_MAIN_GPS_INTERRUPT_SIGNAL;

#if VEHICLE_SENS_DID_MAIN_GPS_INTERRUPT_SIGNAL_DEBUG_FACTORY
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
        "#[FACTORY] gstMainGpsInterruptSignal.uc_data.uc_data == 0x%x\r\n", gstMainGpsInterruptSignal.uc_data);
#endif
}

/*******************************************************************************
* MODULE    : VehicleSensSetMainGpsInterruptSignal
* ABSTRACT  : Vehicle sensor MAIN_GPS_INTERRUPT_SIGNALE SET function
* FUNCTION  : Update the Main_GPS_INTERRUPT_SIGNAL data master
* ARGUMENT  : *pst_data : Pointer to received message data
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetMainGpsInterruptSignal(const SENSOR_MSG_GPSDATA_DAT *pst_data) {  // LCOV_EXCL_START 8: dead code.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8 uc_ret = VEHICLESENS_EQ;
    VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL *pst_master;

    if (pst_data == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "pst_data == NULL\r\n");
    } else {
        pst_master = &gstMainGpsInterruptSignal;

        /* Compare data master and received data */
        uc_ret = VehicleSensmemcmp(reinterpret_cast<void *>(&(pst_master->uc_data)),
            (const void *)(&(pst_data->uc_data)), (size_t)(pst_data->us_size));

        /* Received data is set in the data master. */
        pst_master->ul_did    = pst_data->ul_did;
        pst_master->us_size    = pst_data->us_size;
        pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
        (void)memcpy(reinterpret_cast<void *>(&(pst_master->uc_data)),
            (const void *)(&(pst_data->uc_data)), sizeof(pst_master->uc_data));

#if VEHICLE_SENS_DID_MAIN_GPS_INTERRUPT_SIGNAL_DEBUG_FACTORY
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
            "#[FACTORY] pst_data->ul_did == 0x%x", pst_data->ul_did);
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
            "#[FACTORY] gstMainGpsInterruptSignal.ul_did == 0x%x\r\n", gstMainGpsInterruptSignal.ul_did);
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
            "#[FACTORY] (u_int8)pst_data->us_size == 0x%x", (u_int8)pst_data->us_size);
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
            "#[FACTORY] gstMainGpsInterruptSignal.us_size == 0x%x\r\n", gstMainGpsInterruptSignal.us_size);
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
            "#[FACTORY] pst_data->uc_data == 0x%x", pst_data->uc_data[0]);
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
            "#[FACTORY] gstMainGpsInterruptSignal.uc_data == 0x%x\r\n", gstMainGpsInterruptSignal.uc_data);
#endif
    }

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensGetMainGpsInterruptSignal
* ABSTRACT  : Vehicle sensor MAIN_GPS_INTERRUPT_SIGNAL GET function
* FUNCTION  : Provide the MAIN_GPS_INTERRUPT_SIGNAL data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : None
******************************************************************************/
void VehicleSensGetMainGpsInterruptSignal(VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL *pst_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    const VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL *pst_master;

    if (pst_data == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "pst_data == NULL\r\n");
    } else {
        pst_master = &gstMainGpsInterruptSignal;

        /* Store the data master in the specified destination. */
        pst_data->ul_did    = pst_master->ul_did;
        pst_data->us_size    = pst_master->us_size;
        pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
        (void)memcpy(reinterpret_cast<void *>(&(pst_data->uc_data)),
            (const void *)(&(pst_master->uc_data)), sizeof(pst_data->uc_data));
    }
}
// LCOV_EXCL_STOP
