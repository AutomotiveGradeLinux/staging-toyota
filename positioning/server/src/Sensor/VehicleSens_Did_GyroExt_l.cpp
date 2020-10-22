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
 *  File name    :VehicleSens_Did_GyroExt_l.cpp
 *  System name    :Polaris
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor data master(POSHAL_DID_GYRO_EXT)
 *  Module configuration  :VehicleSensInitGyroExtl()    Vehicle Sensor GYRO (Initial Delivery) Initialization Functions
 *                  :VehicleSensSetGyroExtlG()    Vehicle Sensor GYRO (Initial Delivery) Set Functions
 *                  :VehicleSensGetGyroExtl()    Vehicle Sensor GYRO (Initial Delivery) Get Functions
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_DataMaster.h"

/* Ignore->MISRA-C++:2008 Rule 2-7-2 */
#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_EXT    gstGyroExt_l;    // NOLINT(readability/nolint)
static  VEHICLESENS_DATA_MASTER      gstGyroRev_l;    // NOLINT(readability/nolint)

/*******************************************************************************
* MODULE    : VehicleSensInitGyroRevl
* ABSTRACT  : Vehicle Sensor GYRO Initialization Functions(Extensions data)
* FUNCTION  : GYRO data master initialization process
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGyroRevl(void) {
    u_int16  *pus;

    memset(&gstGyroRev_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER));
    /* POSHAL_DID_GYRO initialized by POSHAL_DID_GYRO_X because POSHAL_DID_GYRO is internally replaced by POSHAL_DID_GYRO_EXT  */
    gstGyroRev_l.ul_did  = POSHAL_DID_GYRO_X;
    gstGyroRev_l.us_size  = VEHICLE_DSIZE_GYRO;

    pus    = reinterpret_cast<u_int16 *>(gstGyroRev_l.uc_data);
    pus[0]  = VEHICLE_DINIT_GYRO;  /* Ignore->MISRA-C++:2008 Rule 5-0-15 */
    pus[1]  = VEHICLE_DINIT_GYRO;  /* Ignore->MISRA-C++:2008 Rule 5-0-15 */
    pus[2]  = VEHICLE_DINIT_GYRO;  /* Ignore->MISRA-C++:2008 Rule 5-0-15 */
}

/*******************************************************************************
* MODULE    : VehicleSensInitGyroExtl
* ABSTRACT  : Vehicle Sensor GYRO Initialization Functions(Initial delivery)
* FUNCTION  : GYRO data master initialization process
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitGyroExtl(void) {
    u_int16  *pus;

    memset(&gstGyroExt_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER_EXT));
    /* POSHAL_DID_GYRO initialized by POSHAL_DID_GYRO_X because POSHAL_DID_GYRO is internally replaced by POSHAL_DID_GYRO_EXT  */
    gstGyroExt_l.ul_did    = POSHAL_DID_GYRO_X;
    gstGyroExt_l.us_size    = VEHICLE_DSIZE_GYRO_EXT_INIT;

    pus    = reinterpret_cast<u_int16 *>(gstGyroExt_l.uc_data);
    memset(reinterpret_cast<void*>(pus), VEHICLE_DINIT_GYRO, VEHICLE_DSIZE_GYRO_EXT);
}

/*******************************************************************************
* MODULE    : VehicleSensSetGyroRevl
* ABSTRACT  : Vehicle Sensor GYRO SET Functions
* FUNCTION  : Update the GYRO data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetGyroRevl(const LSDRV_LSDATA *pst_data) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstGyroRev_l;

    /* Compare data master and received data */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data,
                    pst_data->uc_data, pst_data->uc_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */

    /* Received data is set in the data master. */
    pst_master->ul_did    = POSHAL_DID_GYRO_X;
    pst_master->us_size    = pst_data->uc_size;  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
    pst_master->uc_rcvflag  = VEHICLE_RCVFLAG_ON;
    pst_master->uc_snscnt  = pst_data->uc_sns_cnt;
    memset(pst_master->uc_data, 0x00, sizeof(pst_master->uc_data));
    memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */

    return(uc_ret);
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensSetGyroRevlG
* ABSTRACT  : Vehicle Sensor GYRO SET Functions
* FUNCTION  : Update the GYRO data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
u_int8 VehicleSensSetGyroRevlG(const LSDRV_LSDATA_G *pst_data) {
    u_int8 uc_ret;
    VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstGyroRev_l;

    /* Compare data master and received data */
    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
    uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

    /* Received data is set in the data master. */
    pst_master->ul_did    = POSHAL_DID_GYRO_X;
    pst_master->us_size    = pst_data->uc_size;  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
    pst_master->uc_rcvflag = VEHICLE_RCVFLAG_ON;
    pst_master->uc_snscnt  = pst_data->uc_sns_cnt;
    memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */

    return(uc_ret);
}

/*******************************************************************************
* MODULE    : VehicleSensSetGyroExtlG
* ABSTRACT  : Vehicle Sensor GYRO SET Functions(Initial delivery)
* FUNCTION  : Update the GYRO data master
* ARGUMENT  : *pst_data : Pointer to the message data received by the direct line
* NOTE      :
* RETURN    : VEHICLESENS_EQ  : No data change
*             VEHICLESENS_NEQ  : Data change
******************************************************************************/
void VehicleSensSetGyroExtlG(const LSDRV_LSDATA_G *pst_data) {
  VEHICLESENS_DATA_MASTER_EXT *pst_master;
  u_int16 us_start = 0;
  u_int16 us_size = 0;
  u_int16 us_cnt = 0;

  pst_master = &gstGyroExt_l;
  us_size = static_cast<u_int16>(sizeof(u_int16) * 10);  /* Size of one data item: 2byte * 10 data items */

  /* Retrieve the location where the received one is stored */
  us_start = gstPkgTempExt.start_point[GyroExt];

  /* Stored in data master(Order of reception)*/
  if (us_start >= VEHICLE_DKEEP_MAX) {
      /* Store the latest one at position 0 */
      us_start = VEHICLE_DATA_POS_00;
      /* If you are discarding old data,,Set a flag */
      gstPkgTempExt.data_break = VEHICLE_SNS_BREAK;
  }
  pst_master->ul_did    = POSHAL_DID_GYRO_X;
  pst_master->uc_rcvflag    = VEHICLE_RCVFLAG_ON;
  for (us_cnt = 0; us_cnt < us_size; us_cnt++) {
      pst_master->uc_data[us_start * us_size + us_cnt] = (u_int8)(pst_data->uc_data[us_cnt]);
  }

  /* Update next storage start position and latest data storage position */
  us_start++;
  gstPkgTempExt.start_point[GyroExt] = us_start;

  /* Update data master size */
  if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
      /* Make the size of all extended data masters */
      pst_master->us_size = VEHICLE_DSIZE_GYRO_EXT;
  } else {
      /* Add the size of one received data item */
      pst_master->us_size = static_cast<u_int16>(pst_master->us_size + us_size);
  }
}

/*******************************************************************************
* MODULE    : VehicleSensGetGyroExtl
* ABSTRACT  : Vehicle Sensor GYRO GET Functions(Initial delivery)
* FUNCTION  : Provide a GYRO data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGyroExtl(VEHICLESENS_DATA_MASTER_EXT *pst_data) {
  const VEHICLESENS_DATA_MASTER_EXT *pst_master;
  uint16_t us_size = 0;
  uint16_t us_data_cnt = 0; // Number of data contained
  uint16_t us_loop_cnt = 0; // 64 over index

  /* Store the data master in the specified destination. */
  pst_master           = &gstGyroExt_l;
  pst_data->ul_did     = pst_master->ul_did;
  pst_data->us_size    = pst_master->us_size;
  pst_data->uc_rcvflag = pst_master->uc_rcvflag;

  us_size = static_cast<u_int16>(sizeof(u_int16) * 10); /* Size of one data item: 2byte * 10 data items */

  /* Checking whether the number of stored entries is looped */
  if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
    us_data_cnt = VEHICLE_DKEEP_MAX;
  } else {
    us_data_cnt = gstPkgTempExt.start_point[GyroExt];
  }

  /* Acquire data from the oldest data master */
  for (uint16_t us_cnt = 0; us_cnt < us_data_cnt; us_cnt++) {
    if (gstPkgTempExt.data_break == VEHICLE_SNS_BREAK) {
      /* Get information before loop */
      if (gstPkgTempExt.start_point[GyroExt] + us_cnt < VEHICLE_DKEEP_MAX) {
        memcpy(&pst_data->uc_data[us_cnt * us_size],
               &pst_master->uc_data[(gstPkgTempExt.start_point[GyroExt] + us_cnt) * us_size], us_size);
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

/*******************************************************************************
* MODULE    : VehicleSensGetGyroRevl
* ABSTRACT  : Vehicle Sensor GYRO GET Functions(Initial delivery)
* FUNCTION  : Provide a GYRO data master
* ARGUMENT  : *pst_data : Pointer to the data master acquisition destination
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetGyroRevl(VEHICLESENS_DATA_MASTER *pst_data) {
    const VEHICLESENS_DATA_MASTER *pst_master;

    pst_master = &gstGyroRev_l;

    /* Store the data master in the specified destination. */
    pst_data->ul_did    = pst_master->ul_did;
    pst_data->us_size    = pst_master->us_size;
    pst_data->uc_rcvflag  = pst_master->uc_rcvflag;
    pst_data->uc_snscnt  = pst_master->uc_snscnt;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
}
#endif
