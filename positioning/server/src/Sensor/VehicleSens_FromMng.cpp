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
 *    File name        :VehicleSens_Thread.cpp
 *    System name        :_CWORD107_
 *    Subsystem name    :Vehicle sensor process
 *    Program name    :Vehicle sensor FROM control
 *    Module configuration    :VehicleSensBackUpDataInit    Vehicle sensor backup initialization processing
 *                    :VehicleSensFromSetProc        Vehicle sensor FROM SET process
 *                  :VehicleSensFromGetProc        Vehicle sensor FROM GET process
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_FromMng.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static    VEHICLESENS_NON_VOLATILE_DATA g_st_backup_data;

/*******************************************************************************
* MODULE    : VehicleSensBackUpDataInit
* ABSTRACT  : Vehicle sensor backup initialization processing
* FUNCTION  : Initialize the backup data.
* ARGUMENT  : init_sts
* NOTE      :
* RETURN    : ret
******************************************************************************/
int32 VehicleSensBackUpDataInit(int32 init_sts) {
    int32    ret = SRAM_CHK_OK;
    if (init_sts == INI_BUPCHK_SRAM_INIT) {
        ret = VehicleSensBackupInit();

        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "VehicleSensBackupChk SRAM_CHK\r\n");
    } else {
        ret = VehicleSensBackupChk();

        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "VehicleSensBackupChk SRAM_CHK\r\n");
    }
    return ret;
}

/*******************************************************************************
* MODULE    : VehicleSensBackupInit
* ABSTRACT  : Vehicle sensor backup initialization processing(Cold start)
* FUNCTION  : Initialize the backup data.
* ARGUMENT  :
* NOTE      :
* RETURN    : ret
******************************************************************************/
int32 VehicleSensBackupInit(void) {
    int32    ret = SRAM_CHK_OK;

    /* Initialize non-volatile data*/
    memset(&g_st_backup_data, 0x00, sizeof(VEHICLESENS_NON_VOLATILE_DATA));
//    g_st_backup_data.uc_hv            = VEHICLE_SNS_NONHV;                        /* Conveyor car            */
    g_st_backup_data.uc_hv_status    = VEHICLESENS_FROM_STATUS_NOT_DECISION;        /* Cold-start    */
    /* Write to the FROM */
    return ret;
}

/*******************************************************************************
* MODULE    : VehicleSensBackupChk
* ABSTRACT  : Vehicle sensor backup check process
* FUNCTION  : Check the backup data
* ARGUMENT  :
* NOTE      :
* RETURN    : ret
******************************************************************************/
int32 VehicleSensBackupChk(void) {
    int32    ret = SRAM_CHK_OK;
    return ret;
}

/*******************************************************************************
* MODULE    : VehicleSensFromSetProc
* ABSTRACT  : Vehicle sensor FROM SET process
* FUNCTION  : Updating FROM Data.
* ARGUMENT  : ul_did        :Data ID
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensFromSetProc(DID ul_did) {

    u_int8 uc_get_method;


    VEHICLESENS_DATA_MASTER st_master;
    /* Obtain the data acquisition method.*/
    uc_get_method = VehicleSensGetSelectionItemList(ul_did);
    if (VEHICLESENS_GETMETHOD_GPS < uc_get_method) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
            "VEHICLESENS_DELIVERYCTRL: VehicleSensGetSelectionItemList error :%d\r\n", uc_get_method);
    }

    /* Acquire the applicable data information from the data master..*/
    (void)memset (reinterpret_cast<void *>(&st_master), 0, sizeof(VEHICLESENS_DATA_MASTER));
    VehicleSensGetDataMaster(ul_did, uc_get_method, &st_master);

//    switch (ul_did) {
//        case VEHICLE_DID_HV:
//        {
//            g_st_backup_data.uc_hv             = st_master.uc_data[0];
//            g_st_backup_data.uc_hv_status     = VEHICLESENS_FROM_STATUS_DECISION;
//            break;
//        }
//        default:
//            break;
//    }
    /* Updating FROM Data*/
}

/*******************************************************************************
* MODULE    : VehicleSensFromGetProc
* ABSTRACT  : Vehicle sensor FROM GET process
* FUNCTION  : Get FROM datum.
* ARGUMENT  : ul_did        :Data ID
* NOTE      :
* RETURN    : ret
******************************************************************************/
void VehicleSensFromGetProc(DID ul_did, VEHICLESENS_FROM_INFO *p_st_from_data) {
    const VEHICLESENS_NON_VOLATILE_DATA *p_st_backup_data;
    p_st_backup_data = &g_st_backup_data;
//    switch (ul_did) {
//        case VEHICLE_DID_HV:
//        {
//            p_st_from_data->uc_from_value    = p_st_backup_data->uc_hv;
//            p_st_from_data->uc_from_status    = p_st_backup_data->uc_hv_status;
//            break;
//        }
//        default:
//            break;
//    }
}
