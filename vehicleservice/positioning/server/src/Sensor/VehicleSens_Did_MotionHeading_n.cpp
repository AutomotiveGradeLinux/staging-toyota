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
 *  VehicleSens_Did_MotionHeading_n.cpp
 * @brief
 */
/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"
#include "SensorMotion_API.h"
#include "CommonDefine.h"

/*---------------------------------------------------------------------------------*
 * Global Value                                                                    *
 *---------------------------------------------------------------------------------*/
static  VEHICLESENS_DATA_MASTER  gstMotionHeading_n;    // NOLINT(readability/nolint)

/**
 * @brief
 *  Orientation information data master initialization process(NAVI information)
 *
 *  Initialize the orientation information data master
 *
 * @param[in]   none
 * @param[out]  none
 * @return      none
 * @retval      none
 */
void VehicleSensInitMotionHeadingN(void) {
    SENSORMOTION_HEADINGINFO_DAT  st_heading;

    memset(&gstMotionHeading_n, 0x00, sizeof(VEHICLESENS_DATA_MASTER));

    /** Data ID setting */
    gstMotionHeading_n.ul_did   = VEHICLE_DID_MOTION_HEADING_NAVI;

    /** Data size setting */
    gstMotionHeading_n.us_size  = sizeof(SENSORMOTION_HEADINGINFO_DAT);

    /** Data content setting */
    memset(&st_heading, 0x00, sizeof(st_heading));
    st_heading.getMethod = SENSOR_GET_METHOD_NAVI;
    st_heading.SyncCnt   = 0x00;
    st_heading.isEnable  = SENSORMOTION_STATUS_DISABLE;
    st_heading.posSts    = 0x00;
    st_heading.Heading   = 0x00;
    memcpy(&gstMotionHeading_n.uc_data[0], &st_heading, sizeof(st_heading));

    return;
}

/**
 * @brief
 *  Compass Data Master SET Processing(NAVI information)
 *
 *  Update the orientation information data master
 *
 * @param[in]  VEHICLESENS_DATA_MASTER *pst_heading     : Pointer to the data master acquisition destination
 * @param[out] none
 * @return     u_int8
 * @retval     VEHICLESENS_EQ  : No data change
 * @retval     VEHICLESENS_NEQ : Data change
 */
u_int8 VehicleSensSetMotionHeadingN(const SENSORMOTION_HEADINGINFO_DAT *pst_heading) {
    u_int8            uc_ret;
    VEHICLESENS_DATA_MASTER    *pst_master;

    pst_master = &gstMotionHeading_n;

    /** With the contents of the current data master,Compare received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_heading, sizeof(SENSORMOTION_HEADINGINFO_DAT));

    /** Received data is set in the data master. */
    pst_master->ul_did    = VEHICLE_DID_MOTION_HEADING_NAVI;
    pst_master->us_size    = sizeof(SENSORMOTION_HEADINGINFO_DAT);
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    memcpy(pst_master->uc_data, pst_heading, sizeof(SENSORMOTION_HEADINGINFO_DAT));

    return(uc_ret);
}

/**
 * @brief
 *  Compass Data Master GET Processing(NAVI information)
 *
 *  Provide an orientation information data master
 *
 * @param[in]  none
 * @param[out] VEHICLESENS_DATA_MASTER *pst_data : Pointer to the data master acquisition destination
 * @return     none
 * @retval     none
 */
void VehicleSensGetMotionHeadingN(VEHICLESENS_DATA_MASTER *pst_data) {  // LCOV_EXCL_START 8: dead code.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstMotionHeading_n;

    /** Store the data master in the specified destination. */
    pst_data->ul_did      = pst_master->ul_did;
    pst_data->us_size     = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);

    return;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *  Compass Data Master GET Processing(NAVI information)
 *
 *  Providing orientation information data master with orientation and unit conversion
 *
 * @param[in]  none
 * @param[out] VEHICLESENS_DATA_MASTER *pst_data : Pointer to the data master acquisition destination
 * @return     none
 * @retval     none
 */
void VehicleSensGetMotionHeadingnCnvData(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;
    SENSORMOTION_HEADINGINFO_DAT   st_heading;
    int16 i_heading;

    pst_master = &gstMotionHeading_n;

    /* Perform the orientation conversion[-179 to +180] -> [0 to 359] */
    memcpy(&st_heading, pst_master->uc_data, sizeof(st_heading));
    i_heading = static_cast<int16>(st_heading.Heading);
    if (i_heading > 0) {
        i_heading = static_cast<int16>(360 - i_heading);
    } else {
        i_heading = static_cast<int16>(i_heading * -1);
    }
    /* Perform unit conversion[Once] -> [0.01 degree] */
    st_heading.Heading = (u_int16)(i_heading * 100);

    /** Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    memcpy(pst_data->uc_data, &st_heading, sizeof(st_heading));

    return;
}
