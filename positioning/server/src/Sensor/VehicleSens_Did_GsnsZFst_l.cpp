/*
 * @copyright Copyright (c) 2019-2020 TOYOTA MOTOR CORPORATION.
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
 *  VehicleSens_Did_GsnsZFst_l.cpp
 * @brief
 *  Vehicle sensor data master(POSHAL_DID_GSNS_Z_FST)
 */

#include "VehicleSens_DataMaster.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static  VEHICLESENS_DATA_MASTER_FST  g_st_gsnsz_fst_l;  // NOLINT(readability/nolint)

/**
 * @brief
 *   Vehicle sensor GSNS_Z initialization function
 *
 *   GSNS_Z data master initialization processing
 */
void VehicleSensInitGsnsZFstl(void) {
    memset(&g_st_gsnsz_fst_l, 0x00, sizeof(VEHICLESENS_DATA_MASTER_FST));
    g_st_gsnsz_fst_l.ul_did        = POSHAL_DID_GSNS_Z_FST;
    g_st_gsnsz_fst_l.us_size       = VEHICLE_DSIZE_GSNS_Z_EXT_INIT;
    g_st_gsnsz_fst_l.uc_rcvflag    = VEHICLE_RCVFLAG_OFF;
    g_st_gsnsz_fst_l.partition_flg = 0;
}

/**
 * @brief
 *   Vehicle sensor GSNS_Z SET function
 *
 *   Update the GSNS_Z data master
 *
 * @param[in]  *pst_data : Pointer to the message data received by the direct line
 *
 * @return  VEHICLESENS_EQ  No data change<br>
 *      VEHICLESENS_NEQ Data change
 */
u_int8 VehicleSensSetGsnsZFstG(const LSDRV_LSDATA_FST_GSENSOR_Z *pst_data) {
    static u_int8 uc_ret  = VEHICLESENS_EQ;
    VEHICLESENS_DATA_MASTER_FST *pst_master;

    u_int8  partition_max;    /* Total number of partitions */
    u_int8  partition_num;    /* Data number                */

    pst_master = &g_st_gsnsz_fst_l;

    partition_max = pst_data->uc_partition_max;
    partition_num = pst_data->uc_partition_num;

    if (partition_max == 1) {
        /* Compare data master and received data */
        uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

        /* Received data is set in the data master. */
        pst_master->ul_did        = pst_data->ul_did;
        pst_master->us_size       = pst_data->uc_size;
        pst_master->uc_rcvflag    = VEHICLE_RCVFLAG_ON;
        pst_master->partition_flg = 0;
        memset(pst_master->uc_data, 0x00, sizeof(pst_master->uc_data));
        memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);
    } else if (partition_max == 2) {
        if (partition_num == 1) {
            /* Compare data master and received data */
            uc_ret = VehicleSensmemcmp(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);

            /* Received data is set in the data master. */
            pst_master->ul_did      = pst_data->ul_did;
            pst_master->us_size      = static_cast<u_int16>(pst_master->us_size + pst_data->uc_size);
            pst_master->partition_flg  = 1;
            memcpy(pst_master->uc_data, pst_data->uc_data, pst_data->uc_size);
        } else if (partition_num == 2) {
            /* Compare data master and received data */
            if (uc_ret == VEHICLESENS_EQ) {
                uc_ret = VehicleSensmemcmp(&pst_master->uc_data[VEHICLE_DSIZE_GSNSZ_FST],
                    pst_data->uc_data, pst_data->uc_size);
            }

            /* Received data is set in the data master. */
            pst_master->ul_did        = pst_data->ul_did;
            pst_master->us_size       = static_cast<u_int16>(pst_master->us_size + pst_data->uc_size);
            pst_master->uc_rcvflag    = VEHICLE_RCVFLAG_ON;
            pst_master->partition_flg = 1;
            memcpy(&pst_master->uc_data[VEHICLE_DSIZE_GSNSZ_FST], pst_data->uc_data, pst_data->uc_size);
        } else {}
    } else {}

    return(uc_ret);
}

/**
 * @brief
 *   Vehicle sensor GSNS_Z GET function
 *
 *   Provide the GSNS_Z data master
 *
 * @param[in]  Pointer to the data master acquisition destination
 */
void VehicleSensGetGsnsZFstl(VEHICLESENS_DATA_MASTER_FST *pst_data) {
    const VEHICLESENS_DATA_MASTER_FST *pst_master;

    pst_master = &g_st_gsnsz_fst_l;

    /* Store the data master in the specified destination. */
    pst_data->ul_did        = pst_master->ul_did;
    pst_data->us_size       = pst_master->us_size;
    pst_data->uc_rcvflag    = pst_master->uc_rcvflag;
    pst_data->partition_flg = pst_master->partition_flg;
    memcpy(pst_data->uc_data, pst_master->uc_data, pst_master->us_size);
}
