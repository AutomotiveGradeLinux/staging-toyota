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
 *  File name    :VehicleSens_Did_GsnsXExt_l.cpp
 *  System name    :_CWORD107_
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_GSNS_X)
 *  Module configuration  :VehicleSensInitGsnsXExtl()  Vehicle sensor GSNS_X initialization function
 *                  :VehicleSensSetGsnsXExtlG()  Vehicle sensor GSNS_X SET function
 *                  :VehicleSensGetGsnsXExtl()  Vehicle sensor GSNS_X GET function
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/* Ignore->MISRA-C++:2008 Rule 2-7-2 */
#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_EXT  gstGsnsXExt_l;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGsnsXExtl
* ABSTRACT  : Vehicle sensor GSNS_X initialization function
* FUNCTION  : GSNS_X data master initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGsnsXExtl(void) {
    memset(&gstGsnsXExt_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER_EXT));
    gstGsnsXExt_l.ul_did    = POSHAL_DID_GSNS_X;
    gstGsnsXExt_l.us_size  = VEHICLE_DSIZE_GSNS_X_EXT_INIT;
    gstGsnsXExt_l.uc_rcvflag  = VEHICLE_RCVFLAG_OFF;
}

/**
 * @brief
 *   Vehicle sensor GSNS_X SET function
 *
 *   Update the GSNS_X data master
 *
 * @param[in]  *pst_data : Pointer to the message data received by the direct line
 */
void VehicleSensSetGsnsXExtlG(const LSDRV_LSDATA_G *pst_data) {
    VEHICLESENS_DATA_MASTER_EXT *pst_master;
    u_int16 us_start = 0;
    u_int16 us_size = 0;
    u_int16 us_cnt = 0;

    pst_master = &gstGsnsXExt_l;
    us_size = static_cast<u_int16>(sizeof(u_int16) * 10);  /* Size of one data item: 2byte * 10 data items */

    /* Store the latest one in the internal data structure */
    us_start = gstPkgTempExt.start_point[GsnsX];  /* Location to store one received message    */
    /* Stored in data master(Order of reception)*/
    if (us_start >= VEHICLE_DKEEP_MAX) {
        /* Store the latest one at position 0 */
        us_start = VEHICLE_DATA_POS_00;
        /* If you are discarding old data,,Set a flag */
        gstPkgTempExt.data_break = VEHICLE_SNS_BREAK;
    }
    pst_master->ul_did    = pst_data->ul_did;
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    for (us_cnt = 0; us_cnt < us_size; us_cnt++) {
        pst_master->uc_data[us_start * us_size + us_cnt] = (u_int8)(pst_data->uc_data[us_cnt]);
    }

    /* Update next storage start position and latest data storage position */
    us_start++;
    gstPkgTempExt.start_point[GsnsX] = us_start;

    /* Update data master size */
    if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
        /* Make the size of all extended data masters */
        pst_master->us_size = VEHICLE_DSIZE_GSNS_X_EXT;
    } else {
        /* Add the size of one received data item */
        pst_master->us_size = static_cast<u_int16>(pst_master->us_size + us_size);
    }
}

/*******************************************************************************
* MODULE    : VehicleSensGetGsnsXExtl
* ABSTRACT  : Vehicle sensor GSNS_X GET function
* FUNCTION  : Provide the GSNS_X data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGsnsXExtl(VEHICLESENS_DATA_MASTER_EXT *pst_data) {
  const VEHICLESENS_DATA_MASTER_EXT *pst_master;
  uint16_t us_size = 0;
  uint16_t us_data_cnt = 0; // Number of data contained
  uint16_t us_loop_cnt = 0; // 64 over index

  /* Store the data master in the specified destination. */
  pst_master           = &gstGsnsXExt_l;
  pst_data->ul_did     = pst_master->ul_did;
  pst_data->us_size    = pst_master->us_size;
  pst_data->uc_rcvflag = pst_master->uc_rcvflag;

  us_size = static_cast<u_int16>(sizeof(u_int16) * 10); /* Size of one data item: 2byte * 10 data items */

  /* Checking whether the number of stored entries is looped */
  if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
    us_data_cnt = VEHICLE_DKEEP_MAX;
  } else {
    us_data_cnt = gstPkgTempExt.start_point[GsnsX];
  }

  /* Acquire data from the oldest data master */
  for (uint16_t us_cnt = 0; us_cnt < us_data_cnt; us_cnt++) {
    if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
      /* Get information before loop */
      if (gstPkgTempExt.start_point[GsnsX] + us_cnt < VEHICLE_DKEEP_MAX) {
        memcpy(&pst_data->uc_data[us_cnt * us_size],
               &pst_master->uc_data[(gstPkgTempExt.start_point[GsnsX] + us_cnt) * us_size], us_size);
      } else {
        memcpy(&pst_data->uc_data[us_cnt * us_size],
               &pst_master->uc_data[us_loop_cnt * us_size], us_size);
        us_loop_cnt++;
      }
    } else {
      memcpy(&pst_data->uc_data[us_cnt * us_size],
             &pst_master->uc_data[us_cnt * us_size], us_size);
    }
  }
}
#endif
