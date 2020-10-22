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
 *  File name    :DeadReckoning_DeliveryCtrl.cpp
 *  System name    :GPF
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle Sensor Destination Management(
 *  Module configuration  DeadReckoningInitDeliveryCtrlTbl()        Vehicle sensor delivery destination management table initialization function
 *          DeadReckoningInitDeliveryCtrlTblMng()      Vehicle sensor delivery destination management table management area initialization function
 *          DeadReckoningEntryDeliveryCtrl()        Vehicle sensor delivery destination management registration function
 *          :DeadReckoningAddDeliveryCtrlTbl()        Vehicle sensor delivery destination management table addition function
 *          :DeadReckoningUpdateDeliveryCtrlTbl()      Vehicle sensor delivery destination management table update function
 *          :DeadReckoning_DeliveryEntry_Delete()      Vehicle sensor delivery destination management table deletion function
 *          :DeadReckoningAddDeliveryCtrlTblMng()      Vehicle sensor delivery destination management table management addition function
 *          :DeadReckoningUpdateDeliveryCtrlTblMng()    Vehicle sensor delivery destination management table management update function
 *          :DeadReckoning_DeleteDeliveryCtrlTblMng_Touch()  Vehicle sensor delivery destination management table management deletion (touch panel) function
 *          DeadReckoningMakeDeliveryPnoTbl()        Vehicle Sensor Destination PNO Table Creation Function
 *          :DeadReckoningAddPnoTbl()            Vehicle Sensor Destination PNO Table Addition Function
 *          :DeadReckoningDeliveryProc()          Vehicle Sensor Data Delivery Process
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "DeadReckoning_DeliveryCtrl.h"
#include "Vehicle_API.h"
#include "Vehicle_API_Dummy.h"
#include "Dead_Reckoning_Local_Api.h"

/*************************************************/
/*       Global variable            */
/*************************************************/
static  DEADRECKONING_DELIVERY_CTRL_TBL    g_delivery_dr_ctrltbl;
static  DEADRECKONING_DELIVERY_CTRL_TBL_MNG  g_delivery_dr_ctrltbl_mng;
static  DEADRECKONING_DELIVERY_PNO_TBL    g_delivery_dr_pnotbl;

/* Stored data count */
int32 g_delivery_dr_ctrl_num = 0;

/* PastModel002 support DR */
/*******************************************************************************
* MODULE   : DeadReckoningInitDeliveryCtrlTbl
* ABSTRACT  : Vehicle sensor delivery destination management table initialization function
* FUNCTION  : Delivery destination management table initialization processing
* ARGUMENT  : void
* NOTE    :
* RETURN   : void
******************************************************************************/
void DeadReckoningInitDeliveryCtrlTbl(void) {  // LCOV_EXCL_START 8: dead code.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    memset(&g_delivery_dr_ctrltbl, 0x00, sizeof(DEADRECKONING_DELIVERY_CTRL_TBL));
}

/*******************************************************************************
* MODULE   : DeadReckoningInitDeliveryCtrlTblMng
* ABSTRACT  : Vehicle sensor delivery destination management table management area initialization function
* FUNCTION  : Delivery destination management table management area initialization processing
* ARGUMENT  : void
* NOTE    :
* RETURN   : void
******************************************************************************/
void DeadReckoningInitDeliveryCtrlTblMng(void) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    memset(&g_delivery_dr_ctrltbl_mng, 0x00, sizeof(DEADRECKONING_DELIVERY_CTRL_TBL_MNG));
}

/*******************************************************************************
* MODULE   : DeadReckoningEntryDeliveryCtrl
* ABSTRACT  : Vehicle sensor delivery destination management registration function
* FUNCTION  : Shipping management table,Update the shipping management table management.
* ARGUMENT  : p_st_delivery_entry  : Pointer to the delivery registration information
* NOTE    :
* RETURN   : VEHICLE_RET_NORMAL  :Successful registration
******************************************************************************/
DEAD_RECKONING_RET_API DeadReckoningEntryDeliveryCtrl(const DEADRECKONING_MSG_DELIVERY_ENTRY *p_st_delivery_entry) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32 i;
    u_int8 uc_action_type  = DEADRECKONING_ACTION_TYPE_ADD;
    int32 uc_did_flag;
    DID ul_entry_did    = p_st_delivery_entry->data.did;
    DEADRECKONING_DELIVERY_CTRL_TBL_MNG_DATA  *p_st_existing_mng_data = NULL;
    /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    DEAD_RECKONING_RET_API ret = DEAD_RECKONING_RET_NORMAL;

    /* Check if the data ID exists. */
    uc_did_flag = DeadReckoningCheckDid(ul_entry_did);
    if (uc_did_flag == 0) {
        ret =  DEADRECKONING_RET_ERROR_DID;  /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    }

    /* Check the number of registered shipments. */
    if ((ret == DEAD_RECKONING_RET_NORMAL) &&/* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
            (g_delivery_dr_ctrltbl.us_num >= DEADRECKONING_DELIVERY_INFO_MAX)) {
        /* Return the FULL of delivery registrations*/
        ret =  DEADRECKONING_RET_ERROR_BUFFULL;  /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    }
    /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    if (ret == DEAD_RECKONING_RET_NORMAL) {
        /* By searching for the delivery registration of the relevant DID,Hold the address. */
        for (i = 0; i < g_delivery_dr_ctrltbl_mng.us_num; i++) {
            if (g_delivery_dr_ctrltbl_mng.st_ctrl_tbl_mng_data[i].ul_did == ul_entry_did) {
                uc_action_type = DEADRECKONING_ACTION_TYPE_UPDATE;
                p_st_existing_mng_data = &g_delivery_dr_ctrltbl_mng.st_ctrl_tbl_mng_data[i];
            }
        }

        /* Add to the shipping management table.*/
        DeadReckoningAddDeliveryCtrlTbl(p_st_delivery_entry);

        /* Processing when updating existing data*/
        if (uc_action_type == DEADRECKONING_ACTION_TYPE_UPDATE) {
            /* Update the shipping management table.*/
            DeadReckoningUpdateDeliveryCtrlTbl(p_st_existing_mng_data);

            /* Update the shipping destination management table management information.*/
            DeadReckoningUpdateDeliveryCtrlTblMng(p_st_existing_mng_data);
        } else { /* Newly added processing*/
            /* Add to the shipping management table management information.*/
            DeadReckoningAddDeliveryCtrlTblMng(p_st_delivery_entry);
        }
    }
    return ret; /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
}

/*******************************************************************************
* MODULE   : DeadReckoningAddDeliveryCtrlTbl
* ABSTRACT  : Vehicle sensor delivery destination management table addition function
* FUNCTION  : Add to the shipping management table.
* ARGUMENT  : *p_st_delivery_entry  : Pointer to the delivery registration information
* NOTE    :
* RETURN   : void
******************************************************************************/
void DeadReckoningAddDeliveryCtrlTbl(const DEADRECKONING_MSG_DELIVERY_ENTRY *p_st_delivery_entry) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    DEADRECKONING_DELIVERY_CTRL_TBL_DATA *p_st_ctrl_data;

    p_st_ctrl_data = &g_delivery_dr_ctrltbl.st_ctrl_data[g_delivery_dr_ctrltbl.us_num];
    p_st_ctrl_data->ul_did          = p_st_delivery_entry->data.did;
    p_st_ctrl_data->us_pno          = p_st_delivery_entry->data.pno;
    p_st_ctrl_data->uc_chg_type        = p_st_delivery_entry->data.delivery_timing;
    p_st_ctrl_data->uc_ctrl_flg        = p_st_delivery_entry->data.ctrl_flg;
    p_st_ctrl_data->us_link_idx        = DEADRECKONING_LINK_INDEX_END;
    p_st_ctrl_data->uc_method        = DEADRECKONING_DELIVERY_METHOD_NORMAL;

    g_delivery_dr_ctrltbl.us_num = static_cast<u_int16>(g_delivery_dr_ctrltbl.us_num + 1);
}

/*******************************************************************************
* MODULE   : DeadReckoningUpdateDeliveryCtrlTbl
* ABSTRACT  : Vehicle sensor delivery destination management table update function
* FUNCTION  : Update the shipping management table.
* ARGUMENT  : *p_st_existing_mng_data  : Pointer to the previous data information with the same data ID
* NOTE    :
* RETURN   : void
******************************************************************************/
void DeadReckoningUpdateDeliveryCtrlTbl(DEADRECKONING_DELIVERY_CTRL_TBL_MNG_DATA *p_st_existing_mng_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* Update Link Index Only.
       For the index of the value of us_end_idx that matches the data ID of the distribution target management table management information
       Make us_link_idx a registered index */
    g_delivery_dr_ctrltbl.st_ctrl_data[p_st_existing_mng_data->us_end_idx].us_link_idx =
        static_cast<u_int16>(g_delivery_dr_ctrltbl.us_num - 1);
}

/*******************************************************************************
* MODULE   : DeadReckoningAddDeliveryCtrlTblMng
* ABSTRACT  : Vehicle sensor delivery destination management table management addition function
* FUNCTION  : Add to the shipping management table management.
* ARGUMENT  : *p_st_delivery_entry  : Pointer to the delivery registration information
* NOTE    :
* RETURN   : void
******************************************************************************/
void DeadReckoningAddDeliveryCtrlTblMng(const DEADRECKONING_MSG_DELIVERY_ENTRY *p_st_delivery_entry) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    DEADRECKONING_DELIVERY_CTRL_TBL_MNG_DATA *p_st_ctr_mng_data;

    p_st_ctr_mng_data = &g_delivery_dr_ctrltbl_mng.st_ctrl_tbl_mng_data[g_delivery_dr_ctrltbl_mng.us_num];
    p_st_ctr_mng_data->ul_did    = p_st_delivery_entry->data.did;
    p_st_ctr_mng_data->us_start_idx  = static_cast<u_int16>(g_delivery_dr_ctrltbl.us_num - 1);
    p_st_ctr_mng_data->us_end_idx  = static_cast<u_int16>(g_delivery_dr_ctrltbl.us_num - 1);
    p_st_ctr_mng_data->us_dlvry_entry_num++;
    g_delivery_dr_ctrltbl_mng.us_num++;
    g_delivery_dr_ctrl_num++;
}

/*******************************************************************************
* MODULE   : DeadReckoningUpdateDeliveryCtrlTblMng
* ABSTRACT  : Vehicle sensor delivery destination management table management update function
* FUNCTION  : Update the shipping management table management.
* ARGUMENT  : *p_st_existing_mng_data  : Pointer to the previous data information with the same data ID
* NOTE    :
* RETURN   : void
******************************************************************************/
void DeadReckoningUpdateDeliveryCtrlTblMng(DEADRECKONING_DELIVERY_CTRL_TBL_MNG_DATA *p_st_existing_mng_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /*  Update only the end index and the number of registered shipping destinations. */
    p_st_existing_mng_data->us_end_idx  = static_cast<u_int16>(g_delivery_dr_ctrltbl.us_num - 1);
    p_st_existing_mng_data->us_dlvry_entry_num++;
}

/*******************************************************************************
* MODULE   : DeadReckoningMakeDeliveryPnoTbl
* ABSTRACT  : Vehicle sensor delivery destination PNO table creation function
* FUNCTION  : Create the shipping destination PNO table
* ARGUMENT  : ul_did    Data ID
*         Change_type Delivery Trigger
* NOTE    :
* RETURN   : DEADRECKONING_DELIVERY_PNO_TBL* Pointer to the shipping PNO table
******************************************************************************/
DEADRECKONING_DELIVERY_PNO_TBL* DeadReckoningMakeDeliveryPnoTbl(DID ul_did, u_int8 change_type) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32 i;
    u_int16 us_index = 0;
    u_int16 us_num = 0;

    /* Get the start index and count of the corresponding data ID. */
    for (i = 0; i < g_delivery_dr_ctrl_num; i++) {
        /* Stores the information of the corresponding DID.. */
        if (g_delivery_dr_ctrltbl_mng.st_ctrl_tbl_mng_data[i].ul_did == ul_did) {
            us_index  = g_delivery_dr_ctrltbl_mng.st_ctrl_tbl_mng_data[i].us_start_idx;
            us_num  = g_delivery_dr_ctrltbl_mng.st_ctrl_tbl_mng_data[i].us_dlvry_entry_num;
            break;
        }
    }

    /* Create a PNO list */
    g_delivery_dr_pnotbl.us_num = 0;
    if (change_type == DEADRECKONING_CHGTYPE_CHG) {
        /* Processing when delivery timing is changed*/
        for (i = 0; i < us_num; i++) {
            /* Functionalization by Increasing Structure Members */
            DeadReckoningAddPnoTbl(us_index);

            us_index  = g_delivery_dr_ctrltbl.st_ctrl_data[us_index].us_link_idx;
        }
    } else {
        /* Processing when delivery timing is update */
        for (i = 0; i < us_num; i++) {
            if (DEADRECKONING_DELIVERY_TIMING_UPDATE  == g_delivery_dr_ctrltbl.st_ctrl_data[us_index].uc_chg_type) {
                /* Functionalization by Increasing Structure Members */
                DeadReckoningAddPnoTbl(us_index);
            }
            us_index  = g_delivery_dr_ctrltbl.st_ctrl_data[us_index].us_link_idx;
        }
    }
    return (&g_delivery_dr_pnotbl);
}

/*******************************************************************************
* MODULE   : DeadReckoningAddPnoTbl
* ABSTRACT  : Vehicle Sensor Destination PNO Table Addition Function
* FUNCTION  : Add to the shipping PNO table.
* ARGUMENT  : us_index  : Index of the referenced destination management table
* NOTE    :
* RETURN   : void
******************************************************************************/
void DeadReckoningAddPnoTbl(u_int16 us_index) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int16  us_pno_tbl_idx;

    us_pno_tbl_idx = g_delivery_dr_pnotbl.us_num;
    g_delivery_dr_pnotbl.st_pno_data[us_pno_tbl_idx].us_pno    = g_delivery_dr_ctrltbl.st_ctrl_data[us_index].us_pno;
    /* Save the relevant INDEX in the delivery control table */
    g_delivery_dr_pnotbl.st_pno_data[us_pno_tbl_idx].us_index    = us_index;
    g_delivery_dr_pnotbl.st_pno_data[us_pno_tbl_idx].uc_method =
    g_delivery_dr_ctrltbl.st_ctrl_data[us_index].uc_method;
    g_delivery_dr_pnotbl.us_num++;
}

/*******************************************************************************
* MODULE   : DeadReckoningDeliveryProc
* ABSTRACT  : Vehicle Sensor Data Delivery Process
* FUNCTION  : Deliver data to a destination.
* ARGUMENT  : ul_did    :Data ID
*         uc_chg_type  :Delivery timing
*         uc_get_method  :Acquisition method
* NOTE    :
* RETURN   : void
******************************************************************************/
void DeadReckoningDeliveryProc(DID ul_did, u_int8 uc_chg_type, u_int8 uc_get_method) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32                i;          /* Generic counters           */
    DEADRECKONING_DATA_MASTER      st_master;      /* Data master of normal data      */


    /* Defines the data master for each API. */
    SENSORLOCATION_MSG_LONLATINFO_DAT  st_msg_lonlat_info;
    SENSORLOCATION_MSG_ALTITUDEINFO_DAT  st_msg_altitude_info;
    SENSORMOTION_MSG_SPEEDINFO_DAT    st_msg_speed_info;
    SENSORMOTION_MSG_HEADINGINFO_DAT  st_msg_heading_info;


    const DEADRECKONING_DELIVERY_PNO_TBL  *p_st_pno_tbl;          /* Vehicle Sensor Destination PNO Table Pointer  */

    /* Initialization */
    st_msg_lonlat_info.reserve[0]   = 0;
    st_msg_lonlat_info.reserve[1]   = 0;
    st_msg_lonlat_info.reserve[2]   = 0;
    st_msg_altitude_info.reserve[0] = 0;
    st_msg_altitude_info.reserve[1] = 0;
    st_msg_altitude_info.reserve[2] = 0;
    st_msg_speed_info.reserve       = 0;
    st_msg_heading_info.reserve     = 0;

    /* Obtain the shipping destination PNO */
    p_st_pno_tbl =
    reinterpret_cast<DEADRECKONING_DELIVERY_PNO_TBL*>(DeadReckoningMakeDeliveryPnoTbl(ul_did, uc_chg_type));

    if ((p_st_pno_tbl->us_num) > 0) {
        /* When there is a shipping destination PNO registration */
        /* Vehicle sensor information notification transmission process */
        for (i = 0; i < (p_st_pno_tbl->us_num); i++) {
            /* Acquire the applicable data information from the data master..*/
            DeadReckoningGetDataMaster(ul_did, &st_master);

            /* Align data from the data master for API I/F */
            switch (ul_did) {
                    /* Describes the process for each DID. */
                case VEHICLE_DID_DR_LATITUDE:
                {
                    /* Size storage(LONLAT)  */
                    st_msg_lonlat_info.size = (u_int16)sizeof(SENSORLOCATION_MSG_LONLATINFO_DAT);

                    /* DR status setting */
                    st_msg_lonlat_info.dr_status = st_master.dr_status;

                    /* The DR enable flag is set to enabled. */
                    st_msg_lonlat_info.is_exist_dr = SENSORLOCATION_EXISTDR_DR;

                    /* Set the Latitude */
                    (void)memcpy(reinterpret_cast<void *>(&(st_msg_lonlat_info.latitude)),
                        (const void *)(&(st_master.uc_data[0])), (size_t)(st_master.us_size));

                    /* Obtain the data master Longitude */
                    DeadReckoningGetDataMaster(VEHICLE_DID_DR_LONGITUDE, &st_master);

                    /* Set the Longitude */
                    (void)memcpy(reinterpret_cast<void *>(&(st_msg_lonlat_info.longitude)),
                        (const void *)(&(st_master.uc_data[0])), (size_t)(st_master.us_size));

                    /* Acquire data master SensorCnt */
                    DeadReckoningGetDataMaster(VEHICLE_DID_DR_SNS_COUNTER, &st_master);

                    /* Set the SensorCnt */
                    (void)memcpy(reinterpret_cast<void *>(&(st_msg_lonlat_info.sensor_cnt)),
                        (const void *)(&( st_master.uc_data[0])), (size_t)(st_master.us_size));

                    (void)DRManagerSndMsg(PNO_VEHICLE_SENSOR,
                                            p_st_pno_tbl->st_pno_data[i].us_pno,
                                            CID_VEHICLE_SENSORLOCATION_LONLAT,
                                            sizeof(SENSORLOCATION_MSG_LONLATINFO_DAT),
                                            (const void *)&st_msg_lonlat_info);
                    break;
                }
                case VEHICLE_DID_DR_ALTITUDE:
                {
                    /* Size storage(ALTITUDE)  */
                    st_msg_altitude_info.size = (u_int16)sizeof(SENSORLOCATION_MSG_ALTITUDEINFO_DAT);

                    /* The DR enable flag is set to enabled. */
                    st_msg_altitude_info.is_exist_dr = SENSORLOCATION_EXISTDR_DR;

                    /* DR status setting */
                    st_msg_altitude_info.dr_status = st_master.dr_status;


                    /* Set the Altitude */
                    (void)memcpy(reinterpret_cast<void *>(&(st_msg_altitude_info.altitude)),
                        (const void *)(&(st_master.uc_data[0])), (size_t)(st_master.us_size));

                    /* Acquire data master SensorCnt */
                    DeadReckoningGetDataMaster(VEHICLE_DID_DR_SNS_COUNTER, &st_master);

                    /* Set the SensorCnt */
                    (void)memcpy(reinterpret_cast<void *>(&(st_msg_altitude_info.sensor_cnt)),
                        (const void *)(&(st_master.uc_data[0])), (size_t)(st_master.us_size));

                    (void)DRManagerSndMsg(PNO_VEHICLE_SENSOR,
                                            p_st_pno_tbl->st_pno_data[i].us_pno,
                                            CID_VEHICLE_SENSORLOCATION_ALTITUDE,
                                            sizeof(SENSORLOCATION_MSG_ALTITUDEINFO_DAT),
                                            (const void *)&st_msg_altitude_info);

                    break;
                }
                case VEHICLE_DID_DR_SPEED:
                {
                    /* Size storage(SPEED)  */
                    st_msg_speed_info.size = (u_int16)sizeof(SENSORMOTION_MSG_SPEEDINFO_DAT);

                    /* The DR enable flag is set to enabled. */
                    st_msg_speed_info.is_exist_dr = SENSORMOTION_EXISTDR_DR;

                    /* DR status setting */
                    st_msg_speed_info.dr_status = st_master.dr_status;


                    /* Set the Speed */
                    (void)memcpy(reinterpret_cast<void *>(&(st_msg_speed_info.speed)),
                        (const void *)(&(st_master.uc_data[0])), (size_t)(st_master.us_size));

                    /* Acquire data master SensorCnt */
                    DeadReckoningGetDataMaster(VEHICLE_DID_DR_SNS_COUNTER, &st_master);
                    /* Set the SensorCnt */
                    (void)memcpy(reinterpret_cast<void *>(&(st_msg_speed_info.sensor_cnt)),
                        (const void *)(&(st_master.uc_data[0])), (size_t)(st_master.us_size));

                    (void)DRManagerSndMsg(PNO_VEHICLE_SENSOR,
                                            p_st_pno_tbl->st_pno_data[i].us_pno,
                                            CID_VEHICLE_SENSORMOTION_SPEED,
                                            sizeof(SENSORMOTION_MSG_SPEEDINFO_DAT),
                                            (const void *)&st_msg_speed_info);
                    break;
                }
                case VEHICLE_DID_DR_HEADING:
                {
                    /* Size storage(HEADING)  */
                    st_msg_heading_info.size = (u_int16)sizeof(SENSORMOTION_MSG_HEADINGINFO_DAT);

                    /* The DR enable flag is set to enabled. */
                    st_msg_heading_info.is_exist_dr = SENSORMOTION_EXISTDR_DR;

                    /* DR status setting */
                    st_msg_heading_info.dr_status = st_master.dr_status;

                    /* Set the Heading */
                    (void)memcpy(reinterpret_cast<void *>(&(st_msg_heading_info.heading)),
                        (const void *)(&(st_master.uc_data[0])), (size_t)(st_master.us_size));

                    /* Acquire data master SensorCnt */
                    DeadReckoningGetDataMaster(VEHICLE_DID_DR_SNS_COUNTER, &st_master);
                    /* Set the SensorCnt */
                    (void)memcpy(reinterpret_cast<void *>(&(st_msg_heading_info.sensor_cnt)),
                        (const void *)(&(st_master.uc_data[0])), (size_t)(st_master.us_size));

                    (void)DRManagerSndMsg(PNO_VEHICLE_SENSOR,
                                            p_st_pno_tbl->st_pno_data[i].us_pno,
                                            CID_VEHICLE_SENSORMOTION_HEADING,
                                            sizeof(SENSORMOTION_MSG_HEADINGINFO_DAT),
                                            (const void *)&st_msg_heading_info);
                    break;
                }
                case VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL:
                {
                    SENSORMOTION_MSG_GYROPARAMETERINFO_DAT  stMsgGyroParameterInfo;
                    /* Initialization */
                    stMsgGyroParameterInfo.reserve[0] = 0;
                    stMsgGyroParameterInfo.reserve[1] = 0;

                    /* GyroOffset/GyroScaleFactor/GyroScaleFactorLevel data master  */
                    /* setup must be completed before data delivery of this DID         */
                    /* The order of processing is defined by DeadReckoning_RcvMsg().,Be careful when changing              */

                    /* Size storage(GYROPARAMETER) */
                    stMsgGyroParameterInfo.size = (u_int16)sizeof(stMsgGyroParameterInfo);

                    /* GyroOffset acuisition/setting */
                    DeadReckoningGetDataMaster(VEHICLE_DID_DR_GYRO_OFFSET, &st_master);

                    (void)memcpy(reinterpret_cast<void *>(&(stMsgGyroParameterInfo.gyro_offset)),
                                        reinterpret_cast<void *>(&(st_master.uc_data[0])),
                                        sizeof(stMsgGyroParameterInfo.gyro_offset));

                    /* GyroScaleFactor acuisition/setting */
                    DeadReckoningGetDataMaster(VEHICLE_DID_DR_GYRO_SCALE_FACTOR, &st_master);

                    (void)memcpy(reinterpret_cast<void *>(&(stMsgGyroParameterInfo.gyro_scale_factor)),
                                        reinterpret_cast<void *>(&(st_master.uc_data[0])),
                                        sizeof(stMsgGyroParameterInfo.gyro_scale_factor));

                    /* GyroScaleFactorLevel acuisition/setting */
                    DeadReckoningGetDataMaster(VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL, &st_master);

                    (void)memcpy(reinterpret_cast<void *>(&(stMsgGyroParameterInfo.gyro_scale_factor_level)),
                                        reinterpret_cast<void *>(&(st_master.uc_data[0])),
                                        sizeof(stMsgGyroParameterInfo.gyro_scale_factor_level));

                    /* Data transmission */
                    (void)DRManagerSndMsg(PNO_VEHICLE_SENSOR,
                                            p_st_pno_tbl->st_pno_data[i].us_pno,
                                            CID_VEHICLE_SENSORMOTION_GYROPARAMETER,
                                            sizeof(stMsgGyroParameterInfo),
                                            (const void *)&stMsgGyroParameterInfo);
                }
                break;
                case VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR_LEVEL:
                {
                  SENSORMOTION_MSG_SPEEDPULSEPARAMETERINFO_DAT  stMsgSpeedPulseParameterInfo;

                  /* Initialization */
                  stMsgSpeedPulseParameterInfo.reserve[0] = 0;
                  stMsgSpeedPulseParameterInfo.reserve[1] = 0;
                  stMsgSpeedPulseParameterInfo.reserve[2] = 0;

                  /* GyroOffset/GyroScaleFactor/GyroScaleFactorLevel data master  */
                  /* setup must be completed before data delivery of this DID     */
                  /* The order of processing is defined by DeadReckoning_RcvMsg().,Be careful when changing              */

                  /* Size storage(SPEEDPULSEPARAMETER) */
                  stMsgSpeedPulseParameterInfo.size = (u_int16)sizeof(stMsgSpeedPulseParameterInfo);

                  /* SpeedPulseScaleFactor acuisition/setting */
                  DeadReckoningGetDataMaster(VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR, &st_master);

                  (void)memcpy(reinterpret_cast<void *>(&(stMsgSpeedPulseParameterInfo.speed_pulse_scale_factor)),
                                      reinterpret_cast<void *>(&(st_master.uc_data[0])),
                                      sizeof(stMsgSpeedPulseParameterInfo.speed_pulse_scale_factor));

                  /* SpeedPulseScaleFactorLevel acuisition/setting */
                  DeadReckoningGetDataMaster(VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR_LEVEL, &st_master);

                  (void)memcpy(reinterpret_cast<void *>(&(stMsgSpeedPulseParameterInfo.speed_pulse_scale_factor_level)),
                                      reinterpret_cast<void *>(&(st_master.uc_data[0])),
                                      sizeof(stMsgSpeedPulseParameterInfo.speed_pulse_scale_factor_level));

                    /* Data transmission */
                    (void)DRManagerSndMsg(PNO_VEHICLE_SENSOR,
                                            p_st_pno_tbl->st_pno_data[i].us_pno,
                                            CID_VEHICLE_SENSORMOTION_SPEEDPULSEPARAMETER,
                                            sizeof(stMsgSpeedPulseParameterInfo),
                                            (const void *)&stMsgSpeedPulseParameterInfo);
                }
                break;
                /* Other than the above */
                default:
                    /* Do not edit. */
                    break;
            }
        }
    }
}

/*******************************************************************************
 * MODULE    : DRManagerSndMsg
 * ABSTRACT  : Message transmission processing
 * FUNCTION  : Send a message to the specified PNO
 * ARGUMENT  : us_pno_src      : Source PNO
 *           : us_pno_dest    : Destination PNO
 *           : us_cid      : Command ID
 *           : us_msg_len      : Message data body length
 *           : *p_msg_data    : Pointer to message data
 * NOTE      :
 * RETURN    : RET_NORMAL    : Normal completion
 *           : RET_ERRNOTRDY  : Destination process is not wakeup
 *           : RET_ERRMSGFULL  : Message queue overflows
 *           : RET_ERRPARAM    : Buffer size error
 ******************************************************************************/
RET_API  DRManagerSndMsg(PNO us_pno_src, PNO us_pno_dest, CID us_cid, u_int16 us_msg_len, const void *p_msg_data) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    DEADRECKONING_MSG_BUF    st_msg_buf;    /* message buffer */
    T_APIMSG_MSGBUF_HEADER  *p_st_msg_hdr;    /* Pointer to the message header */
    RET_API          l_ret_api;    /* Return value */

    /* Message buffer initialization */
    memset(reinterpret_cast<void *>(&st_msg_buf), 0, sizeof(DEADRECKONING_MSG_BUF));

    /* Get pointer to send buffer */
    p_st_msg_hdr = reinterpret_cast<T_APIMSG_MSGBUF_HEADER *>(reinterpret_cast<void *>(&st_msg_buf));

    /*--------------------------------------------------------------*
     *  Create message headers                  *
     *--------------------------------------------------------------*/
    p_st_msg_hdr->hdr.sndpno    = us_pno_src;    /* Source PNO */
    p_st_msg_hdr->hdr.cid      = us_cid;    /* Command ID */
    p_st_msg_hdr->hdr.msgbodysize  = us_msg_len;    /* Message data body length */

    /*--------------------------------------------------------------*
     *  Create message data                  *
     *--------------------------------------------------------------*/
    if ((0 != p_msg_data) && (0 != us_msg_len)) {
        /* Set the message data */
        memcpy(reinterpret_cast<void *>(st_msg_buf.data), p_msg_data, (size_t)us_msg_len);
    }
    /*--------------------------------------------------------------*
     * Send messages                      *
     *--------------------------------------------------------------*/
    l_ret_api = _pb_SndMsg(us_pno_dest,
                            (u_int16)(sizeof(T_APIMSG_MSGBUF_HEADER) + us_msg_len),
                            reinterpret_cast<void *>(&st_msg_buf),
                            0);
    return (l_ret_api);
}


/*******************************************************************************
* MODULE    : DeadReckoningFirstDelivery
* ABSTRACT  : Vehicle Sensor Initial Data Delivery Process
* FUNCTION  : Deliver the initial data to the destination.
* ARGUMENT  : us_pno  :Addresses for delivery NO
*             ul_did  :Data ID
* NOTE      :
* RETURN    : void
******************************************************************************/
void DeadReckoningFirstDelivery(PNO us_pno, DID ul_did) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    DEADRECKONING_DATA_MASTER      st_master;      /* Data master of normal data      */

    /* Defines the data master for each API. */
    SENSORLOCATION_MSG_LONLATINFO_DAT  st_msg_lonlat_info;
    SENSORLOCATION_MSG_ALTITUDEINFO_DAT  st_msg_altitude_info;
    SENSORMOTION_MSG_SPEEDINFO_DAT    st_msg_speed_info;
    SENSORMOTION_MSG_HEADINGINFO_DAT  st_msg_heading_info;

    /* Initialization */
    st_msg_lonlat_info.reserve[0] = 0;
    st_msg_lonlat_info.reserve[1] = 0;
    st_msg_lonlat_info.reserve[2] = 0;
    st_msg_altitude_info.reserve[0] = 0;
    st_msg_altitude_info.reserve[1] = 0;
    st_msg_altitude_info.reserve[2] = 0;
    st_msg_speed_info.reserve = 0;
    st_msg_heading_info.reserve = 0;

    /* Align data from the data master for API I/F */
    switch (ul_did) {
            /* Describes the process for each DID. */
        case VEHICLE_DID_DR_LATITUDE:
        {
            DeadReckoningGetDataMaster(ul_did, &st_master);

            /* Size storage(LONLAT)  */
            st_msg_lonlat_info.size = (u_int16)sizeof(SENSORLOCATION_MSG_LONLATINFO_DAT);

            /* DR status setting */
            st_msg_lonlat_info.dr_status = st_master.dr_status;

            /* The DR enable flag is set to enabled. */
            st_msg_lonlat_info.is_exist_dr = SENSORLOCATION_EXISTDR_DR;

            /* Set the Latitude */
            memcpy(&(st_msg_lonlat_info.latitude), &(st_master.uc_data[0]), st_master.us_size);

            /* Obtain the data master Longitude */
            DeadReckoningGetDataMaster(VEHICLE_DID_DR_LONGITUDE, &st_master);

            /* Set the Longitude */
            memcpy(&(st_msg_lonlat_info.longitude), &(st_master.uc_data[0]), st_master.us_size);

            /* Acquire data master SensorCnt */
            DeadReckoningGetDataMaster(VEHICLE_DID_DR_SNS_COUNTER, &st_master);

            /* Set the SensorCnt */
            memcpy(&(st_msg_lonlat_info.sensor_cnt), &(st_master.uc_data[0]), st_master.us_size);

            (void)DRManagerSndMsg(PNO_VEHICLE_SENSOR,
                                    us_pno,
                                    CID_VEHICLE_SENSORLOCATION_LONLAT,
                                    sizeof(SENSORLOCATION_MSG_LONLATINFO_DAT),
                                    (const void *)&st_msg_lonlat_info);
            break;
        }
        case VEHICLE_DID_DR_ALTITUDE:
        {
            DeadReckoningGetDataMaster(ul_did, &st_master);

            /* Size storage(ALTITUDE)  */
            st_msg_altitude_info.size = (u_int16)sizeof(SENSORLOCATION_MSG_ALTITUDEINFO_DAT);

            /* The DR enable flag is set to enabled. */
            st_msg_altitude_info.is_exist_dr = SENSORLOCATION_EXISTDR_DR;

            /* DR status setting */
            st_msg_altitude_info.dr_status = st_master.dr_status;


            /* Set the Altitude */
            memcpy(&(st_msg_altitude_info.altitude), &(st_master.uc_data[0]), st_master.us_size);

            /* Acquire data master SensorCnt */
            DeadReckoningGetDataMaster(VEHICLE_DID_DR_SNS_COUNTER, &st_master);

            /* Set the SensorCnt */
            memcpy(&(st_msg_altitude_info.sensor_cnt), &(st_master.uc_data[0]), st_master.us_size);

            (void)DRManagerSndMsg(PNO_VEHICLE_SENSOR,
                                    us_pno,
                                    CID_VEHICLE_SENSORLOCATION_ALTITUDE,
                                    sizeof(SENSORLOCATION_MSG_ALTITUDEINFO_DAT),
                                    (const void *)&st_msg_altitude_info);

            break;
        }
        case VEHICLE_DID_DR_SPEED:
        {
            DeadReckoningGetDataMaster(ul_did, &st_master);

            /* Size storage(SPEED)  */
            st_msg_speed_info.size = (u_int16)sizeof(SENSORMOTION_MSG_SPEEDINFO_DAT);

            /* The DR enable flag is set to enabled. */
            st_msg_speed_info.is_exist_dr = SENSORMOTION_EXISTDR_DR;

            /* DR status setting */
            st_msg_speed_info.dr_status = st_master.dr_status;


            /* Set the Speed */
            memcpy(&(st_msg_speed_info.speed), &(st_master.uc_data[0]), st_master.us_size);

            /* Acquire data master SensorCnt */
            DeadReckoningGetDataMaster(VEHICLE_DID_DR_SNS_COUNTER, &st_master);
            /* Set the SensorCnt */
            memcpy(&(st_msg_speed_info.sensor_cnt), &(st_master.uc_data[0]), st_master.us_size);

            (void)DRManagerSndMsg(PNO_VEHICLE_SENSOR,
                                    us_pno,
                                    CID_VEHICLE_SENSORMOTION_SPEED,
                                    sizeof(SENSORMOTION_MSG_SPEEDINFO_DAT),
                                    (const void *)&st_msg_speed_info);
            break;
        }
        case VEHICLE_DID_DR_HEADING:
        {
            DeadReckoningGetDataMaster(ul_did, &st_master);

            /* Size storage(HEADING)  */
            st_msg_heading_info.size = (u_int16)sizeof(SENSORMOTION_MSG_HEADINGINFO_DAT);

            /* The DR enable flag is set to enabled. */
            st_msg_heading_info.is_exist_dr = SENSORMOTION_EXISTDR_DR;

            /* DR status setting */
            st_msg_heading_info.dr_status = st_master.dr_status;


            /* Set the Heading */
            (void)memcpy(reinterpret_cast<void *>(&(st_msg_heading_info.heading)),
             (const void *)(&(st_master.uc_data[0])), (size_t)(st_master.us_size));

            /* Acquire data master SensorCnt */
            DeadReckoningGetDataMaster(VEHICLE_DID_DR_SNS_COUNTER, &st_master);
            /* Set the SensorCnt */
            (void)memcpy(reinterpret_cast<void *>(&(st_msg_heading_info.sensor_cnt)),
                (const void *)(&(st_master.uc_data[0])), (size_t)(st_master.us_size));

            (void)DRManagerSndMsg(PNO_VEHICLE_SENSOR,
                                    us_pno,
                                    CID_VEHICLE_SENSORMOTION_HEADING,
                                    sizeof(SENSORMOTION_MSG_HEADINGINFO_DAT),
                                    (const void *)&st_msg_heading_info);
            break;
        }
        case VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL:
        {
            SENSORMOTION_MSG_GYROPARAMETERINFO_DAT  stMsgGyroParameterInfo;

            /* Initialization */
            stMsgGyroParameterInfo.reserve[0] = 0;
            stMsgGyroParameterInfo.reserve[1] = 0;

            /* Size storage(GYROPARAMETER) */
            stMsgGyroParameterInfo.size = (u_int16)sizeof(stMsgGyroParameterInfo);

            /* GyroOffset acuisition/setting */
            DeadReckoningGetDataMaster(VEHICLE_DID_DR_GYRO_OFFSET, &st_master);

            (void)memcpy(reinterpret_cast<void *>(&(stMsgGyroParameterInfo.gyro_offset)),
                                reinterpret_cast<void *>(&(st_master.uc_data[0])),
                                sizeof( stMsgGyroParameterInfo.gyro_offset));

            /* GyroScaleFactor acuisition/setting */
            DeadReckoningGetDataMaster(VEHICLE_DID_DR_GYRO_SCALE_FACTOR, &st_master);

            (void)memcpy(reinterpret_cast<void *>(&(stMsgGyroParameterInfo.gyro_scale_factor)),
                                reinterpret_cast<void *>(&(st_master.uc_data[0])),
                                sizeof(stMsgGyroParameterInfo.gyro_scale_factor));

            /* GyroScaleFactorLevel acuisition/setting */
            DeadReckoningGetDataMaster(VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL, &st_master);

            (void)memcpy(reinterpret_cast<void *>(&(stMsgGyroParameterInfo.gyro_scale_factor_level)),
                                reinterpret_cast<void *>(&(st_master.uc_data[0])),
                                sizeof(stMsgGyroParameterInfo.gyro_scale_factor_level));

            /* Data transmission */
            (void)DRManagerSndMsg(PNO_VEHICLE_SENSOR,
                                    us_pno,
                                    CID_VEHICLE_SENSORMOTION_GYROPARAMETER,
                                    sizeof(stMsgGyroParameterInfo),
                                    (const void *)&stMsgGyroParameterInfo);
        }
        break;
        case VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR_LEVEL:
        {
            SENSORMOTION_MSG_SPEEDPULSEPARAMETERINFO_DAT  stMsgSpeedPulseParameterInfo;

            /* Initialization */
            stMsgSpeedPulseParameterInfo.reserve[0] = 0;
            stMsgSpeedPulseParameterInfo.reserve[1] = 0;
            stMsgSpeedPulseParameterInfo.reserve[2] = 0;

            /* Size storage(SPEEDPULSEPARAMETER) */
            stMsgSpeedPulseParameterInfo.size = (u_int16)sizeof(stMsgSpeedPulseParameterInfo);

            /* SpeedPulseScaleFactor acuisition/setting */
            DeadReckoningGetDataMaster(VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR, &st_master);

            (void)memcpy(reinterpret_cast<void *>(&(stMsgSpeedPulseParameterInfo.speed_pulse_scale_factor)),
                                reinterpret_cast<void *>(&(st_master.uc_data[0])),
                                sizeof(stMsgSpeedPulseParameterInfo.speed_pulse_scale_factor));

            /* SpeedPulseScaleFactorLevel acuisition/setting */
            DeadReckoningGetDataMaster(VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR_LEVEL, &st_master);

            (void)memcpy(reinterpret_cast<void *>(&(stMsgSpeedPulseParameterInfo.speed_pulse_scale_factor_level)),
                                reinterpret_cast<void *>(&(st_master.uc_data[0])),
                                sizeof(stMsgSpeedPulseParameterInfo.speed_pulse_scale_factor_level));

            /* Data transmission */
            (void)DRManagerSndMsg(PNO_VEHICLE_SENSOR,
                                    us_pno,
                                    CID_VEHICLE_SENSORMOTION_SPEEDPULSEPARAMETER,
                                    sizeof(stMsgSpeedPulseParameterInfo),
                                    (const void *)&stMsgSpeedPulseParameterInfo);
        }
        break;
        /* Other than the above */
        default:
            /* Do not edit. */
            break;
    }
}
// LCOV_EXCL_STOP
