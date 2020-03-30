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
 *    File name        :VehicleSens_DeliveryCtrl.cpp
 *    System name        :GPF
 *    Subsystem name    :Vehicle sensor process
 *    Program name    :Vehicle Sensor Destination Management
 *    Module configuration    VehicleSensInitDeliveryCtrlTbl()            Vehicle sensor delivery destination management table initialization function
 *                    VehicleSensInitDeliveryCtrlTblMng()        Vehicle sensor delivery destination management table management area initialization function
 *                    VehicleSensInitPkgDeliveryTblMng()        Vehicle Sensor Package Delivery Management Table Initialization Function
 *                    VehicleSensEntryDeliveryCtrl()            Vehicle sensor delivery destination management registration function
 *                    VehicleSensAddDeliveryCtrlTbl()            Vehicle sensor delivery destination management table addition function
 *                    VehicleSensUpdateDeliveryCtrlTbl()        Vehicle sensor delivery destination management table update function
 *                    VehicleSensUpdatePkgDeliveryCtrlTbl()    Vehicle Sensor Delivery Destination Management Table Package Delivery Data Update Function
 *                    VehicleSensAddDeliveryCtrlTblMng()        Vehicle sensor delivery destination management table management addition function
 *                    VehicleSensUpdateDeliveryCtrlTblMng()    Vehicle sensor delivery destination management table management update function
 *                    VehicleSensAddPkgDeliveryTblMng()        Vehicle Sensor Package Delivery Management Table Additional Function
 *                    VehicleSensEntryPkgDeliveryCtrl()        Vehicle sensor package delivery management registration function
 *                    VehicleSensMakeDeliveryPnoTbl()            Vehicle Sensor Destination PNO Table Creation Function
 *                    VehicleSensAddPnoTbl()                    Vehicle Sensor Destination PNO Table Addition Function
 *                    VehicleSensDeliveryProc()                Vehicle Sensor Data Delivery Process
 *                    VehicleSensFirstDelivery()                Vehicle Sensor Initial Data Delivery Process
 *                    VehicleSensFirstPkgDelivery()            Vehicle Sensor Initial Package Data Delivery Process
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include <vehicle_service/POS_common_API.h>
#include <vehicle_service/POS_gps_API.h>
#include "VehicleSens_DeliveryCtrl.h"
#include "Dead_Reckoning_Local_Api.h"
#include "SensorLog.h"
#include "POS_private.h"

#define VEHICLE_SENS_DELIVERY_CTRL_DEBUG 0
#define VEHICLE_SENS_DELIVERY_CTRL_DEBUG_DIAG 0

#define GYRO_NORMAL     (0U)
#define GYRO_ERROR      (1U)
#define GYRO_UNFIXED    (2U)

#define _pb_strcat(pdest, psrc, size)  (strncat(pdest, psrc, size) , (0))

/*************************************************/
/*           Global variable                      */
/*************************************************/
static    VEHICLESENS_DELIVERY_CTRL_TBL        g_stdelivery_ctrl_tbl;
static    VEHICLESENS_DELIVERY_CTRL_TBL_MNG    g_stdelivery_ctrl_tbl_mng;
static    VEHICLESENS_PKG_DELIVERY_TBL_MNG    g_stpkgdelivery_tbl_mng;
static    VEHICLESENS_DELIVERY_PNO_TBL        g_stdelivery_pno_tbl;

/* ++ PastModel002 response DR */
static    VEHICLESENS_DELIVERY_CTRL_TBL        g_stdelivery_ctrl_tbl_dr;
static    VEHICLESENS_DELIVERY_CTRL_TBL_MNG    g_stdelivery_ctrl_tbl_mng_dr;
static    VEHICLESENS_DELIVERY_PNO_TBL        g_stdelivery_pno_tbl_dr;

/* -- PastModel002 response DR */

#if CONFIG_HW_PORTSET_TYPE_C
u_int16    gusSeqNum;                                    /* Sequence number for split transmission */
#endif

/*******************************************************************************
* MODULE    : VehicleSensInitDeliveryCtrlTbl
* ABSTRACT  : Vehicle sensor delivery destination management table initialization function
* FUNCTION  : Delivery destination management table initialization processing
* ARGUMENT  : void
* NOTE      :
******************************************************************************/
void VehicleSensInitDeliveryCtrlTbl(void) {
    memset(&g_stdelivery_ctrl_tbl, 0x00, sizeof(VEHICLESENS_DELIVERY_CTRL_TBL));
}

/*******************************************************************************
* MODULE    : VehicleSensInitDeliveryCtrlTblMng
* ABSTRACT  : Vehicle Sensor DR Internal Delivery Destination Management Table Management Area Initialization Function
* FUNCTION  : Delivery destination management table management area initialization processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitDeliveryCtrlTblMng(void) {
    memset(&g_stdelivery_ctrl_tbl_mng, 0x00, sizeof(VEHICLESENS_DELIVERY_CTRL_TBL_MNG));
}

/* ++ PastModel002 response DR */
/*******************************************************************************
* MODULE    : VehicleSensInitDeliveryCtrlTblDR
* ABSTRACT  : Vehicle sensor delivery destination management table initialization function
* FUNCTION  : DR distribution target management table initialization processing
* ARGUMENT  : void
* NOTE      :
******************************************************************************/
void VehicleSensInitDeliveryCtrlTblDR(void) {  // LCOV_EXCL_START 8: dead code.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    memset(&g_stdelivery_ctrl_tbl_dr, 0x00, sizeof(VEHICLESENS_DELIVERY_CTRL_TBL));
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensInitDeliveryCtrlTblMngDR
* ABSTRACT  : Vehicle sensor delivery destination management table management area initialization function
* FUNCTION  : Initialization processing for the management table area of the delivery destination management table for DR
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitDeliveryCtrlTblMngDR(void) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    memset(&g_stdelivery_ctrl_tbl_mng_dr, 0x00, sizeof(VEHICLESENS_DELIVERY_CTRL_TBL_MNG));
}
// LCOV_EXCL_STOP

/* -- PastModel002 response DR */

/*******************************************************************************
* MODULE    : VehicleSensInitPkgDeliveryTblMng
* ABSTRACT  : Vehicle Sensor Package Delivery Management Table Initialization Function
* FUNCTION  : Delivery Package Management Table Initialization Processing
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitPkgDeliveryTblMng(void) {
    memset(&g_stpkgdelivery_tbl_mng, 0x00, sizeof(VEHICLESENS_PKG_DELIVERY_TBL_MNG));
}

#if CONFIG_HW_PORTSET_TYPE_C
/*******************************************************************************
* MODULE    : VehicleSensInitSeqNum
* ABSTRACT  : Sequence number initialization function for split transmission
* FUNCTION  : Sequence number initialization processing for split transmission
* ARGUMENT  : void
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensInitSeqNum(void) {
    gusSeqNum = 0;
}
#endif

/*******************************************************************************
* MODULE    : VehicleSensEntryDeliveryCtrl
* ABSTRACT  : Vehicle sensor delivery destination management registration function
* FUNCTION  : Shipping management table,Update the shipping management table management.
* ARGUMENT  : pst_delivery_entry    : Pointer to the delivery registration information
* NOTE      :
* RETURN    : VEHICLE_RET_NORMAL    :Successful registration
******************************************************************************/
VEHICLE_RET_API VehicleSensEntryDeliveryCtrl(const VEHICLE_MSG_DELIVERY_ENTRY *pst_delivery_entry) {
    int32 i;
    u_int8 uc_action_type    = VEHICLESENS_ACTION_TYPE_ADD;
    int32 uc_did_flag;
    DID ulentry_did        = pst_delivery_entry->data.did;
    VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA    *pst_existing_mng_data    = NULL;
    VEHICLE_RET_API ret = VEHICLE_RET_NORMAL;  /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    BOOL bexist_flag = FALSE;

    /* Check if the data ID exists. */
    uc_did_flag = VehicleSensCheckDid(ulentry_did);
    if (uc_did_flag == 0) {  // LCOV_EXCL_BR_LINE 6:alway be 1
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

        ret = VEHICLE_RET_ERROR_DID;    /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */  // LCOV_EXCL_LINE 8: dead code  // NOLINT(whitespace/line_length)
    }

    /* Check the number of registered shipments. */
    if ((ret == VEHICLE_RET_NORMAL) && /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
            (g_stdelivery_ctrl_tbl.us_dnum >= VEHICLESENS_DELIVERY_INFO_MAX)) {
        /* Return the FULL of delivery registrations*/
        ret = VEHICLE_RET_ERROR_BUFFULL;    /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    }

    if (ret == VEHICLE_RET_NORMAL) {  /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        /* Duplicate delivery registration check*/
        for (i = 0; i < g_stdelivery_ctrl_tbl.us_dnum; i++) {
            if ((g_stdelivery_ctrl_tbl.st_ctrl_data[i].ul_did == ulentry_did) &&
                (g_stdelivery_ctrl_tbl.st_ctrl_data[i].us_pno == pst_delivery_entry->data.pno) &&
                (g_stdelivery_ctrl_tbl.st_ctrl_data[i].uc_method == VEHICLESENS_DELIVERY_METHOD_NORMAL)) {
                /* When the same shipping address (PNO) and shipping DID are already registered,Update registration information and exit */
                g_stdelivery_ctrl_tbl.st_ctrl_data[i].uc_chg_type = pst_delivery_entry->data.delivery_timing;
                bexist_flag = TRUE;
                break;
            }
        }

        if (bexist_flag == TRUE) {
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, 
                          "Duplicate DID=0x%x PNO=0x%x ChgType=%d",
                          ulentry_did,
                          pst_delivery_entry->data.pno,
                          pst_delivery_entry->data.delivery_timing);
        } else {
            /* By searching for the delivery registration of the relevant DID,Hold the address. */
            for (i = 0; i < g_stdelivery_ctrl_tbl_mng.us_dnum; i++) {
                if (g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data[i].ul_did == ulentry_did) {
                    uc_action_type = VEHICLESENS_ACTION_TYPE_UPDATE;
                    pst_existing_mng_data = &g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data[i];
                }
            }
            /* Add to the shipping management table.*/
            VehicleSensAddDeliveryCtrlTbl(pst_delivery_entry);

            /* Processing when updating existing data*/
            if (uc_action_type == VEHICLESENS_ACTION_TYPE_UPDATE) {
                /* Update the shipping management table.*/
                VehicleSensUpdateDeliveryCtrlTbl(pst_existing_mng_data);

                /* Update the shipping destination management table management information.*/
                VehicleSensUpdateDeliveryCtrlTblMng(pst_existing_mng_data);
            } else {  /* Newly added processing*/
                /* Add to the shipping management table management information.*/
                VehicleSensAddDeliveryCtrlTblMng(pst_delivery_entry);
            }
        }
    }
    return ret;    /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
}

/*******************************************************************************
* MODULE    : VehicleSensAddDeliveryCtrlTbl
* ABSTRACT  : Vehicle sensor delivery destination management table addition function
* FUNCTION  : Add to the shipping management table.
* ARGUMENT  : *pst_delivery_entry    : Pointer to the delivery registration information
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensAddDeliveryCtrlTbl(const VEHICLE_MSG_DELIVERY_ENTRY *pst_delivery_entry) {
    VEHICLESENS_DELIVERY_CTRL_TBL_DATA *pst_ctrl_data;

    pst_ctrl_data = &g_stdelivery_ctrl_tbl.st_ctrl_data[g_stdelivery_ctrl_tbl.us_dnum];
    pst_ctrl_data->ul_did                = pst_delivery_entry->data.did;
    pst_ctrl_data->us_pno                = pst_delivery_entry->data.pno;
    pst_ctrl_data->uc_chg_type            = pst_delivery_entry->data.delivery_timing;
    pst_ctrl_data->uc_ctrl_flg            = pst_delivery_entry->data.ctrl_flg;
    pst_ctrl_data->us_link_idx            = VEHICLESENS_LINK_INDEX_END;
    pst_ctrl_data->us_pkg_start_idx        = VEHICLESENS_LINK_INDEX_END;
    pst_ctrl_data->us_pkg_end_idx        = VEHICLESENS_LINK_INDEX_END;
    pst_ctrl_data->uc_method            = VEHICLESENS_DELIVERY_METHOD_NORMAL;
    g_stdelivery_ctrl_tbl.us_dnum = static_cast<u_int16>(g_stdelivery_ctrl_tbl.us_dnum + 1);
}

/*******************************************************************************
* MODULE    : VehicleSensUpdateDeliveryCtrlTbl
* ABSTRACT  : Vehicle sensor delivery destination management table update function
* FUNCTION  : Update the shipping management table.
* ARGUMENT  : *pstExistingMngData    : Pointer to the previous data information with the same data ID
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensUpdateDeliveryCtrlTbl(VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA *pst_existing_mng_data) {
    /* Ignore->MISRA-C++:2008 Rule 7-1-2 */
    /* Update Link Index Only.
       For indexes of usEndIdx values matching the data IDs in the target management table
       Making usLinkIdx an Index-Registered Index */
    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
    g_stdelivery_ctrl_tbl.st_ctrl_data[pst_existing_mng_data->us_end_idx].us_link_idx =
    static_cast<u_int16>(g_stdelivery_ctrl_tbl.us_dnum - 1);    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
}

/*******************************************************************************
* MODULE    : VehicleSensUpdatePkgDeliveryCtrlTbl
* ABSTRACT  : Vehicle Sensor Delivery Destination Management Table Package Delivery Data Update Function
* FUNCTION  : Updating Package Delivery Data in the Destination Management Table.
* ARGUMENT  : us_dnum    : Number of data items in the package delivery management table
*           : us_pkg_num    : Number of packages to create
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensUpdatePkgDeliveryCtrlTbl(u_int16 us_dnum, u_int16 us_pkg_num) {
    VEHICLESENS_DELIVERY_CTRL_TBL_DATA *pst_ctrl_data;
    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
    pst_ctrl_data = &g_stdelivery_ctrl_tbl.st_ctrl_data[g_stdelivery_ctrl_tbl.us_dnum - 1];
    pst_ctrl_data->us_pkg_start_idx    = us_dnum;
    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
    pst_ctrl_data->us_pkg_end_idx = static_cast<u_int16>(us_dnum + us_pkg_num - 1);
    pst_ctrl_data->uc_method        = VEHICLESENS_DELIVERY_METHOD_PACKAGE;
}

/*******************************************************************************
* MODULE    : VehicleSensAddDeliveryCtrlTblMng
* ABSTRACT  : Vehicle sensor delivery destination management table management addition function
* FUNCTION  : Add to the shipping management table management.
* ARGUMENT  : *pst_delivery_entry    : Pointer to the delivery registration information
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensAddDeliveryCtrlTblMng(const VEHICLE_MSG_DELIVERY_ENTRY *pst_delivery_entry) {
    VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA *pst_ctrl_mng_data;

    pst_ctrl_mng_data = &g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data[g_stdelivery_ctrl_tbl_mng.us_dnum];
    pst_ctrl_mng_data->ul_did        = pst_delivery_entry->data.did;
    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
    pst_ctrl_mng_data->us_start_idx = static_cast<u_int16>(g_stdelivery_ctrl_tbl.us_dnum - 1);
    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
    pst_ctrl_mng_data->us_end_idx = static_cast<u_int16>(g_stdelivery_ctrl_tbl.us_dnum - 1);
    pst_ctrl_mng_data->usdlvry_entry_num++;
    g_stdelivery_ctrl_tbl_mng.us_dnum++;
}

/*******************************************************************************
* MODULE    : VehicleSensUpdateDeliveryCtrlTblMng
* ABSTRACT  : Vehicle sensor delivery destination management table management update function
* FUNCTION  : Update the shipping management table management.
* ARGUMENT  : *pst_existing_mng_data    : Pointer to the previous data information with the same data ID
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensUpdateDeliveryCtrlTblMng(VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA *pst_existing_mng_data) {
    /*    Update only the end index and the number of registered shipping destinations. */
    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
    pst_existing_mng_data->us_end_idx = static_cast<u_int16>(g_stdelivery_ctrl_tbl.us_dnum - 1);
    pst_existing_mng_data->usdlvry_entry_num++;
}

/*******************************************************************************
* MODULE    : VehicleSensAddPkgDeliveryTblMng
* ABSTRACT  : Vehicle Sensor Package Delivery Management Table Additional Function
* FUNCTION  : Add to the shipping management table management.
* ARGUMENT  : *pst_pkg                : Pointer to package delivery registration information
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensAddPkgDeliveryTblMng(const SENSOR_MSG_DELIVERY_ENTRY *pst_pkg) {
    int32        i;                    /* Generic counters */

    /* Data ID registration */
    /* Registration of delivery data index */
    for (i = 0; i < (pst_pkg->data.pkg_num); i++) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
        g_stpkgdelivery_tbl_mng.st_pkg_data[g_stpkgdelivery_tbl_mng.us_dnum].ul_did = pst_pkg->data.did[i];
        /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
        g_stpkgdelivery_tbl_mng.st_pkg_data[g_stpkgdelivery_tbl_mng.us_dnum].usdlvry_idx = \
                                            static_cast<u_int16>(g_stpkgdelivery_tbl_mng.us_dnum + 1);
        g_stpkgdelivery_tbl_mng.us_dnum++;
    }
    /* The final delivery data index overwrites the terminating code */
    g_stpkgdelivery_tbl_mng.st_pkg_data[g_stpkgdelivery_tbl_mng.us_dnum - 1].usdlvry_idx = VEHICLESENS_LINK_INDEX_END;
}

/*******************************************************************************
* MODULE    : VehicleSensEntryPkgDeliveryCtrl
* ABSTRACT  : Vehicle sensor package delivery management registration function
* FUNCTION  : Shipping management table,Destination management table management,Update the package delivery management table.
* ARGUMENT  : *pst_pkg                : Pointer to package delivery registration information
* NOTE      :
* RETURN    : VEHICLE_RET_NORMAL    : Successful registration
******************************************************************************/
VEHICLE_RET_API VehicleSensEntryPkgDeliveryCtrl(const SENSOR_MSG_DELIVERY_ENTRY *pst_pkg ,
                                                 u_int8 uc_ext_chk) {   /* Ignore->MISRA-C++:2008 Rule 6-6-5 */
    int32                                    i;
    u_int16                                    us_size = 0;
    u_int8                                    uc_action_type    = VEHICLESENS_ACTION_TYPE_ADD;
    VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA    *pst_existing_mng_data    = NULL;
    VEHICLE_MSG_DELIVERY_ENTRY                st_delivery_entry;
    u_int16        us_boundary_adj;                    /* For boundary adjustment  */
    u_int16        us_next_offset;                    /* For size calculation  */
    VEHICLE_RET_API ret = VEHICLE_RET_NORMAL;  /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    BOOL bexist_flag = FALSE;

    /* Check if the data ID exists. */
    for (i = 0; i < (pst_pkg->data.pkg_num); i++) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
        if (VEHICLESENS_INVALID == VehicleSensCheckDid(pst_pkg->data.did[i])) {  // LCOV_EXCL_BR_LINE 200: always Valid
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            ret = VEHICLE_RET_ERROR_DID;  // // LCOV_EXCL_LINE 8 :dead code
            /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        }
    }

    /* Check the number of registered shipments. */
    if ((ret == VEHICLE_RET_NORMAL) &&  /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        (VEHICLESENS_DELIVERY_INFO_MAX <= g_stdelivery_ctrl_tbl.us_dnum)) {  /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
        /* Return the FULL of delivery registrations*/
        ret = VEHICLE_RET_ERROR_BUFFULL; /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    }

    /* Check the number of registered package shipments. */
    if ((ret == VEHICLE_RET_NORMAL) && /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        (VEHICLESENS_PKG_DELIVERY_INFO_MAX < (g_stpkgdelivery_tbl_mng.us_dnum + pst_pkg->data.pkg_num))) {
        /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
        /* Return the FULL of delivery registrations*/
        ret = VEHICLE_RET_ERROR_BUFFULL; /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    }

    if (ret == VEHICLE_RET_NORMAL) {  /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        /* Check that the size of the buffer to be delivered does not exceed the maximum size. */
        /* For boundary adjustment */
        us_boundary_adj = (u_int16)VEHICLESENS_BIT1 | (u_int16)VEHICLESENS_BIT0;
        for (i = 0; i < pst_pkg->data.pkg_num; i++) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
            
            /* Ignore->MISRA-C++:2008 Rule 2-7-2 */
#if CONFIG_SENSOR_EXT_VALID                /* Initial Sensor Support */
            if (uc_ext_chk == VEHICLESENS_EXT_OFF) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */  // LCOV_EXCL_BR_LINE 200: VEHICLESENS_EXT_OFF passed in function is dead code  // NOLINT(whitespace/line_length)
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                us_next_offset = VehicleSensGetDataMasterOffset(pst_pkg->data.did[i]);  // LCOV_EXCL_LINE 8: dead code
            } else {
                us_next_offset = VehicleSensGetDataMasterExtOffset(pst_pkg->data.did[i]);
            }
#else
            us_next_offset = VehicleSensGetDataMasterOffset(pst_pkg->data.did[i]);
#endif
            /* Ignore->MISRA-C++:2008 Rule 2-7-2 */
            /* Ignore->MISRA-C++:2008 Rule 5-0-5 Ignore->MISRA-C++:2008 Rule 5-0-13, 5-0-14, 5-3-1 */
            if ((us_next_offset & us_boundary_adj) != 0) {
                /* If you need to adjust */
                /* Mask Lower Bit Ignore->MISRA-C++:2008 Rule 5-0-5 Ignore->MISRA-C++:2008 Rule 5-0-21 */
                us_next_offset = static_cast<u_int16>(us_next_offset & ~us_boundary_adj);
                us_next_offset = static_cast<u_int16>(us_next_offset + (u_int16)VEHICLESENS_BIT2);  /* Add numbers */
            }
            us_size = static_cast<u_int16>(us_size + us_next_offset);    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
        }
        if (SENSOR_VSINFO_DSIZE < us_size) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
            /* Return the FULL of delivery registrations(Exceed the size of the buffer to be delivered due to the combination of packages)*/
            ret = VEHICLE_RET_ERROR_BUFFULL; /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        }
    }

    if (ret == VEHICLE_RET_NORMAL) {  /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        for (i = 0; i < g_stdelivery_ctrl_tbl.us_dnum; i++) {
            if ((g_stdelivery_ctrl_tbl.st_ctrl_data[i].us_pno == pst_pkg->data.pno) &&
               (g_stdelivery_ctrl_tbl.st_ctrl_data[i].uc_method == VEHICLESENS_DELIVERY_METHOD_PACKAGE)) {
                /* When the same shipping address (PNO) is already registered,Update registration information and exit */
                g_stdelivery_ctrl_tbl.st_ctrl_data[i].uc_chg_type = pst_pkg->data.delivery_timing;
                bexist_flag = TRUE;
                break;
            }
        }

        if (bexist_flag == TRUE) {
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Duplicate PNO=0x%x ChgType=%d",
                           pst_pkg->data.pno, pst_pkg->data.delivery_timing);
        } else {
            /* By searching for the delivery registration of the relevant DID,Hold the address. */
            for (i = 0; i < g_stdelivery_ctrl_tbl_mng.us_dnum; i++) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
                if (g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data[i].ul_did == pst_pkg->data.did[0]) {
                    uc_action_type = VEHICLESENS_ACTION_TYPE_UPDATE;
                    pst_existing_mng_data = &g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data[i];
                }
            }
            /* Type conversion,And copies of the data section(Because the header is unused,Not involved) */
            memset(reinterpret_cast<void *>(&st_delivery_entry),
                   static_cast<int32>(0),
                   (size_t)sizeof(VEHICLE_MSG_DELIVERY_ENTRY));
            st_delivery_entry.data.did            = pst_pkg->data.did[0];
            st_delivery_entry.data.pno            = pst_pkg->data.pno;
            st_delivery_entry.data.delivery_timing    = pst_pkg->data.delivery_timing;
            st_delivery_entry.data.ctrl_flg        = pst_pkg->data.ctrl_flg;
            st_delivery_entry.data.event_id        = pst_pkg->data.event_id;
            /* Add to the shipping management table.*/
            VehicleSensAddDeliveryCtrlTbl(&st_delivery_entry);

            /* Processing when updating existing data*/
            /* Ignore->MISRA-C++:2008 Rule 5-0-13, 5-0-14, 5-3-1 */
            if (uc_action_type == VEHICLESENS_ACTION_TYPE_UPDATE) {
                /* Update the shipping management table.*/
                VehicleSensUpdateDeliveryCtrlTbl(pst_existing_mng_data);

                /* Update the shipping destination management table management information.*/
                VehicleSensUpdateDeliveryCtrlTblMng(pst_existing_mng_data);
            } else {  /* Newly added processing*/
                /* Add to the shipping management table management information.*/
                VehicleSensAddDeliveryCtrlTblMng(&st_delivery_entry);
            }

            /* Updating Package Relationship Data in the Destination Management Table.*/
            /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
            VehicleSensUpdatePkgDeliveryCtrlTbl(g_stpkgdelivery_tbl_mng.us_dnum, pst_pkg->data.pkg_num);
            /* Add to the package delivery management table.*/
            VehicleSensAddPkgDeliveryTblMng(pst_pkg);
        }
    }

    return ret; /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
}

/*******************************************************************************
* MODULE    : VehicleSensMakeDeliveryPnoTbl
* ABSTRACT  : Vehicle sensor delivery destination PNO table creation function
* FUNCTION  : Create the shipping destination PNO table
* ARGUMENT  : ul_did      Data ID
*             Change_type Delivery Trigger
* NOTE      :
* RETURN    : VEHICLESENS_DELIVERY_PNO_TBL* Pointer to the shipping PNO table
******************************************************************************/
VEHICLESENS_DELIVERY_PNO_TBL* VehicleSensMakeDeliveryPnoTbl(DID ul_did, u_int8 change_type) {
    int32 i;
    u_int8 uc_ctrl_tbl_mng_data_list;
    u_int16 us_index    = 0;
    u_int16 us_dnum = 0;

    /* Get the start index and count of the corresponding data ID. */
    uc_ctrl_tbl_mng_data_list = static_cast<u_int8>(
        (sizeof(g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data)) /
        (sizeof(g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data[0])));
    for (i = 0; i < uc_ctrl_tbl_mng_data_list; i++) {
        /* Stores the information of the corresponding DID.. */
        if (g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data[i].ul_did == ul_did) {
            us_index    = g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data[i].us_start_idx;
            us_dnum    = g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data[i].usdlvry_entry_num;
            break;
        }
    }

    /* Create a PNO list */
    g_stdelivery_pno_tbl.us_dnum = 0;
    if (change_type == VEHICLESENS_CHGTYPE_CHG) {
        /* Processing when delivery timing is changed*/
        for (i = 0; i < us_dnum; i++) {
            /* Functionalization by Increasing Structure Members */
            VehicleSensAddPnoTbl(us_index);
            us_index    = g_stdelivery_ctrl_tbl.st_ctrl_data[us_index].us_link_idx;
        }
    } else {
        /* Processing when delivery timing is update */
        for (i = 0; i < us_dnum; i++) {
            if (VEHICLE_DELIVERY_TIMING_UPDATE  == g_stdelivery_ctrl_tbl.st_ctrl_data[us_index].uc_chg_type) {
                /* Functionalization by Increasing Structure Members */
                VehicleSensAddPnoTbl(us_index);
            }
            us_index    = g_stdelivery_ctrl_tbl.st_ctrl_data[us_index].us_link_idx;
        }
    }
    return(&g_stdelivery_pno_tbl);
}

/*******************************************************************************
* MODULE    : VehicleSensAddPnoTbl
* ABSTRACT  : Vehicle Sensor Destination PNO Table Addition Function
* FUNCTION  : Add to the shipping PNO table.
* ARGUMENT  : us_index    : Index of the referenced destination management table
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensAddPnoTbl(u_int16 us_index) {
    u_int16    us_pno_tbl_idx;

    us_pno_tbl_idx = g_stdelivery_pno_tbl.us_dnum;
    g_stdelivery_pno_tbl.st_pno_data[us_pno_tbl_idx].us_pno         = \
                                                        g_stdelivery_ctrl_tbl.st_ctrl_data[us_index].us_pno;
    g_stdelivery_pno_tbl.st_pno_data[us_pno_tbl_idx].us_pkg_start_idx = \
                                                        g_stdelivery_ctrl_tbl.st_ctrl_data[us_index].us_pkg_start_idx;
    g_stdelivery_pno_tbl.st_pno_data[us_pno_tbl_idx].us_pkg_end_idx   = \
                                                        g_stdelivery_ctrl_tbl.st_ctrl_data[us_index].us_pkg_end_idx;
    g_stdelivery_pno_tbl.st_pno_data[us_pno_tbl_idx].uc_method      = \
                                                        g_stdelivery_ctrl_tbl.st_ctrl_data[us_index].uc_method;
    g_stdelivery_pno_tbl.us_dnum++;
}

/*******************************************************************************
* MODULE    : VehicleSensDeliveryGPS
* ABSTRACT  : Vehicle Sensor Data Delivery Process
* FUNCTION  : Deliver data to a destination.
* ARGUMENT  : ul_did        :Data ID
*             uc_chg_type    :Delivery timing
*             uc_get_method  :Acquisition method
* NOTE      :
* RETURN    : void
******************************************************************************/
u_int8 VehicleSensDeliveryGPS(DID ul_did, u_int8 uc_get_method, u_int8 uc_current_get_method, int32 pno_index,
                            u_int32* cid,
                            VEHICLESENS_DATA_MASTER* stmaster,
                            const VEHICLESENS_DELIVERY_PNO_TBL *pst_pno_tbl) {
#if CONFIG_SENSOR_EXT_VALID                 /* Initial Sensor Support */
    SENSORLOCATION_MSG_LONLATINFO           *plonlat_msg;
    SENSORLOCATION_MSG_ALTITUDEINFO         *paltitude_msg;
    SENSORMOTION_MSG_HEADINGINFO            *pheading_msg;
#endif

//    SENSOR_MSG_GPSDATA_DAT            gps_master;
//    VEHICLESENS_DELIVERY_FORMAT        delivery_data;
//    u_int16                            length;
//    u_int16                            senslog_len;
//    RET_API                                 ret = RET_NORMAL;  /* API return value                       */
    u_int8                                  uc_result = SENSLOG_RES_SUCCESS; /* Send/Receive result      */

//    VehicleSensGetGpsDataMaster(ul_did, uc_current_get_method, &gps_master);
//
//    if (ul_did == POSHAL_DID_GPS_TIME) {
//        /* GPS time,Because there is no header in the message to be delivered,
//        Padding deliveryData headers */
//        (void)memcpy(reinterpret_cast<void*>(&delivery_data),
//                     reinterpret_cast<void*>(&gps_master.uc_data[0]), gps_master.us_size);
//        length = gps_master.us_size;
//        senslog_len = length;
//        *cid = CID_VEHICLESENS_VEHICLE_INFO_GPS_TIME;
//    } else if (ul_did == VEHICLE_DID_LOCATION_LONLAT) {
//        plonlat_msg = reinterpret_cast<SENSORLOCATION_MSG_LONLATINFO*>(&delivery_data);
//        /* Acquire the applicable data information from the data master..*/
//        VehicleSensGetDataMaster(ul_did, uc_current_get_method, stmaster);
//        (void)memcpy(reinterpret_cast<void*>(&(plonlat_msg->data)),
//                     reinterpret_cast<void*>(&(stmaster->uc_data[0])), stmaster->us_size);
//        length = (u_int16)(stmaster->us_size);
//        senslog_len = length;
//        *cid = CID_POSIF_REGISTER_LISTENER_LONLAT;
//    } else if (ul_did == VEHICLE_DID_LOCATION_ALTITUDE) {
//        paltitude_msg = reinterpret_cast<SENSORLOCATION_MSG_ALTITUDEINFO*>(&delivery_data);
//        /* Acquire the applicable data information from the data master..*/
//        VehicleSensGetDataMaster(ul_did, uc_get_method, stmaster);
//        (void)memcpy(reinterpret_cast<void*>(&(paltitude_msg->data)),
//                     reinterpret_cast<void*>(&stmaster->uc_data[0]), stmaster->us_size);
//        length = (u_int16)(stmaster->us_size);
//        senslog_len = length;
//        *cid = CID_POSIF_REGISTER_LISTENER_ALTITUDE;
//    } else if (ul_did == VEHICLE_DID_MOTION_HEADING) {
//        pheading_msg = reinterpret_cast<SENSORMOTION_MSG_HEADINGINFO*>(&delivery_data);
//        /* Acquire the applicable data information from the data master..*/
//        VehicleSensGetDataMaster(ul_did, uc_get_method, stmaster);
//        (void)memcpy(reinterpret_cast<void*>(&(pheading_msg->data)),
//                     reinterpret_cast<void*>(&stmaster->uc_data[0]), stmaster->us_size);
//        length = (u_int16)(stmaster->us_size);
//        senslog_len = length;
//        *cid = CID_POSIF_REGISTER_LISTENER_HEADING;
//    } else {
//        delivery_data.header.did                    = gps_master.ul_did;
//        delivery_data.header.size                = gps_master.us_size;
//        delivery_data.header.rcv_flag                = gps_master.uc_rcv_flag;
//        delivery_data.header.sensor_cnt            = gps_master.uc_sns_cnt;
//        (void)memcpy(reinterpret_cast<void *>(&delivery_data.data[0]),
//                     reinterpret_cast<void *>(&gps_master.uc_data[0]),
//                     (size_t)delivery_data.header.size);
//
//        length = static_cast<u_int16>((u_int16)sizeof(delivery_data.header) + \
//                                            delivery_data.header.size);
//        senslog_len = delivery_data.header.size;
//        *cid = CID_VEHICLESENS_VEHICLE_INFO;
//    }
//
//    /* Call Vehicle Sensor Information Notification Transmission Process.*/
//    ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                          pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                          static_cast<CID>(*cid),
//                          length,
//                          (const void *)&delivery_data);
//    uc_result = SENSLOG_RES_SUCCESS;
//    if (ret != RET_NORMAL) {
//        uc_result = SENSLOG_RES_FAIL;
//    }
//    if (*cid != CID_VEHICLESENS_VEHICLE_INFO) {
//        SensLogWriteOutputData(SENSLOG_DATA_O_UNSPECIFIED, ul_did,
//                                 pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                 reinterpret_cast<uint8_t *>(&delivery_data),
//                                 senslog_len, uc_result);
//    } else {
//        SensLogWriteOutputData(SENSLOG_DATA_O_UNSPECIFIED, ul_did,
//                                 pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                 reinterpret_cast<uint8_t *>(&(delivery_data.data[0])),
//                                 senslog_len, uc_result);
//    }
    return uc_result;
}

u_int8 VehicleSensDeliveryFst(DID ul_did, u_int8 uc_get_method, int32 pno_index,
                            const VEHICLESENS_DELIVERY_PNO_TBL *pst_pno_tbl) {
//    VEHICLESENS_DATA_MASTER_FST             st_master_fst;        /* Master of initial sensor data         */
//    VEHICLESENS_DATA_MASTER_FST             st_master_fst_temp;   /* For temporary storage                        */

    RET_API                                 ret = RET_NORMAL;  /* API return value                       */
    u_int8                                  uc_result = SENSLOG_RES_SUCCESS; /* Send/Receive result      */
//    (void)memset(reinterpret_cast<void *>(&st_master_fst),
//                 0,
//                 sizeof(VEHICLESENS_DATA_MASTER_FST));
//    (void)memset(reinterpret_cast<void *>(&st_master_fst_temp),
//                 0,
//                 sizeof(VEHICLESENS_DATA_MASTER_FST));
//    VehicleSensGetDataMasterFst(ul_did, uc_get_method, &st_master_fst);
//    if (st_master_fst.partition_flg == 1) {
//        /* When the partitioning flag is enabled,Or exceeds the size that can be sent,Perform split transmission */
//        memcpy(&st_master_fst_temp, &st_master_fst, sizeof(VEHICLESENS_DATA_MASTER_FST));
//        if ((ul_did == POSHAL_DID_GYRO_FST) ||    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                (ul_did == POSHAL_DID_GSNS_X_FST) ||
//                (ul_did == POSHAL_DID_GSNS_Y_FST)) {
//            /* 1st session */
//            /* Call Vehicle Sensor Information Notification Transmission Process.*/
//            /* Size of data that can be transmitted in one division(Same size definition used)*/
//            st_master_fst_temp.us_size = LSDRV_FSTSNS_DSIZE_GYRO;
//            ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                                  pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                  CID_VEHICLESENS_VEHICLE_INFO,
//                                  /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                                  static_cast<u_int16>(st_master_fst_temp.us_size + 8),
//                                  (const void *)&st_master_fst_temp);
//            uc_result = SENSLOG_RES_SUCCESS;
//            if (ret != RET_NORMAL) {
//                uc_result = SENSLOG_RES_FAIL;
//            }
//            SensLogWriteOutputData(SENSLOG_DATA_O_SYS,
//                                    ul_did,
//                                    pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                    reinterpret_cast<uint8_t *>(&(st_master_fst_temp.uc_data[0])),
//                                    st_master_fst_temp.us_size,
//                                    uc_result);
//
//            /* Second time */
//            /* Call Vehicle Sensor Information Notification Transmission Process.*/
//            (void)memset(reinterpret_cast<void *>(&st_master_fst_temp.uc_data[0]),
//                         0,
//                         sizeof(u_int8) * VEHICLE_DSIZE_DATA_MASTER_FST);
//            /* Size of the data - Transmission size(Same size definition used)Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            st_master_fst_temp.us_size = static_cast<u_int16>(st_master_fst.us_size - \
//                                                        LSDRV_FSTSNS_DSIZE_GYRO);
//            memcpy(&st_master_fst_temp.uc_data[0],
//                   &st_master_fst.uc_data[LSDRV_FSTSNS_DSIZE_GYRO],
//                   st_master_fst_temp.us_size);
//                   /* Ignore->MISRA-C++:2008 Rule 5-0-5 #Coverity_19347 */
//            ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                                  pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                  CID_VEHICLESENS_VEHICLE_INFO,
//                                  /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                                  static_cast<u_int16>(st_master_fst_temp.us_size + 8),
//                                  (const void *)&st_master_fst_temp);
//            uc_result = SENSLOG_RES_SUCCESS;
//            if (ret != RET_NORMAL) {
//                uc_result = SENSLOG_RES_FAIL;
//            }
//            SensLogWriteOutputData(SENSLOG_DATA_O_SYS,
//                                    ul_did,
//                                    pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                    reinterpret_cast<uint8_t *>(&(st_master_fst_temp.uc_data[0])),
//                                    st_master_fst_temp.us_size,
//                                    uc_result);
//        } else if (ul_did == POSHAL_DID_REV_FST) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            /* 1st session */
//            /* Call Vehicle Sensor Information Notification Transmission Process.*/
//            /* Size of data that can be transmitted in one division */
//            st_master_fst_temp.us_size = LSDRV_FSTSNS_DSIZE_REV;
//            ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                                  pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                  CID_VEHICLESENS_VEHICLE_INFO,
//                                  /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                                  static_cast<u_int16>(st_master_fst_temp.us_size + 8),
//                                  (const void *)&st_master_fst_temp);
//            uc_result = SENSLOG_RES_SUCCESS;
//            if (ret != RET_NORMAL) {
//                uc_result = SENSLOG_RES_FAIL;
//            }
//            SensLogWriteOutputData(SENSLOG_DATA_O_SYS,
//                                    ul_did,
//                                    pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                    reinterpret_cast<uint8_t *>(&(st_master_fst_temp.uc_data[0])),
//                                    st_master_fst_temp.us_size,
//                                    uc_result);
//
//            /* Second time */
//            /* Call Vehicle Sensor Information Notification Transmission Process.*/
//            (void)memset(reinterpret_cast<void *>(&st_master_fst_temp.uc_data[0]),
//                         0,
//                         sizeof(u_int8) * VEHICLE_DSIZE_DATA_MASTER_FST);
//            /* Size of the data - Transmission size Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            st_master_fst_temp.us_size = static_cast<u_int16>(st_master_fst.us_size - \
//                                                LSDRV_FSTSNS_DSIZE_REV);
//            /* Ignore->MISRA-C++:2008 Rule 5-0-5 #Coverity_19347 */
//            memcpy(&st_master_fst_temp.uc_data[0],
//                   &st_master_fst.uc_data[LSDRV_FSTSNS_DSIZE_REV],
//                   st_master_fst_temp.us_size);
//            ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                                  pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                  CID_VEHICLESENS_VEHICLE_INFO,
//                                  /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                                  static_cast<u_int16>(st_master_fst_temp.us_size + 8),
//                                  (const void *)&st_master_fst_temp);
//            uc_result = SENSLOG_RES_SUCCESS;
//            if (ret != RET_NORMAL) {
//                uc_result = SENSLOG_RES_FAIL;
//            }
//            SensLogWriteOutputData(SENSLOG_DATA_O_SYS,
//                                    ul_did,
//                                    pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                    reinterpret_cast<uint8_t *>(&(st_master_fst_temp.uc_data[0])),
//                                    st_master_fst_temp.us_size,
//                                    uc_result);
//        } else {
//            /* 1st session */
//            /* Call Vehicle Sensor Information Notification Transmission Process.*/
//            /* Size of data that can be transmitted in one division(Same size definition used)*/
//            st_master_fst_temp.us_size = LSDRV_FSTSNS_DSIZE_GYRO_TEMP;
//            ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                                  pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                  CID_VEHICLESENS_VEHICLE_INFO,
//                                  /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                                  static_cast<u_int16>(st_master_fst_temp.us_size + 8),
//                                  (const void *)&st_master_fst_temp);
//            uc_result = SENSLOG_RES_SUCCESS;
//            if (ret != RET_NORMAL) {
//                uc_result = SENSLOG_RES_FAIL;
//            }
//            SensLogWriteOutputData(SENSLOG_DATA_O_SYS,
//                                    ul_did,
//                                    pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                    reinterpret_cast<uint8_t *>(&(st_master_fst_temp.uc_data[0])),
//                                    st_master_fst_temp.us_size,
//                                    uc_result);
//            /* Second time */
//            /* Call Vehicle Sensor Information Notification Transmission Process.*/
//            (void)memset(reinterpret_cast<void *>(&st_master_fst_temp.uc_data[0]),
//                         0,
//                         sizeof(u_int8) * VEHICLE_DSIZE_DATA_MASTER_FST);
//            /* Size of the data - Transmission size(Same size definition used)Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            st_master_fst_temp.us_size = static_cast<u_int16>(st_master_fst.us_size - \
//                                                LSDRV_FSTSNS_DSIZE_GYRO_TEMP);
//            memcpy(&st_master_fst_temp.uc_data[0],
//                   &st_master_fst.uc_data[LSDRV_FSTSNS_DSIZE_GYRO_TEMP],
//                   st_master_fst_temp.us_size);  /* Ignore->MISRA-C++:2008 Rule 5-0-5 #Coverity_19347*/
//            ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                                  pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                  CID_VEHICLESENS_VEHICLE_INFO,
//                                  /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                                  static_cast<u_int16>(st_master_fst_temp.us_size + 8),
//                                  (const void *)&st_master_fst_temp);
//            uc_result = SENSLOG_RES_SUCCESS;
//            if (ret != RET_NORMAL) {
//                uc_result = SENSLOG_RES_FAIL;
//            }
//            SensLogWriteOutputData(SENSLOG_DATA_O_SYS,
//                                    ul_did,
//                                    pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                    reinterpret_cast<uint8_t *>(&(st_master_fst_temp.uc_data[0])),
//                                    st_master_fst_temp.us_size,
//                                    uc_result);
//        }
//    } else {
//        /* Call Vehicle Sensor Information Notification Transmission Process.*/
//        ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                              pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                              CID_VEHICLESENS_VEHICLE_INFO,
//                              /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                              static_cast<u_int16>(st_master_fst.us_size + 8),
//                              (const void *)&st_master_fst);
//        uc_result = SENSLOG_RES_SUCCESS;
//        if (ret != RET_NORMAL) {
//            uc_result = SENSLOG_RES_FAIL;
//        }
//        SensLogWriteOutputData(SENSLOG_DATA_O_SYS,
//                                ul_did,
//                                pst_pno_tbl->st_pno_data[pno_index].us_pno,
//                                reinterpret_cast<uint8_t *>(&(st_master_fst.uc_data[0])),
//                                st_master_fst.us_size,
//                                uc_result);
//    }

    return uc_result;
}

u_int8 VehicleSensDeliveryGyro(DID ul_did, u_int8 uc_current_get_method, int32 pno_index, const VEHICLESENS_DELIVERY_PNO_TBL *pst_pno_tbl) {  // LCOV_EXCL_START 8: dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API                                 ret = RET_NORMAL;  /* API return value                       */
    u_int8                                  uc_result = SENSLOG_RES_SUCCESS; /* Send/Receive result      */

    VEHICLESENS_DATA_MASTER_GYRO_TROUBLE    st_master_gyro_trouble;
    SENSORMOTION_MSG_GYROTROUBLEINFO_DAT    st_msg_gyro_trouble_info;

    /* Initialization */
    st_msg_gyro_trouble_info.reserve = 0;

    VehicleSensGetDataMasterGyroTrouble(ul_did, uc_current_get_method, &st_master_gyro_trouble);

    /* Size storage(GYROTROUBLE)  */
    st_msg_gyro_trouble_info.size = st_master_gyro_trouble.us_size;

    /* Set the GyroTrouble */
    (void)memcpy(reinterpret_cast<void *>(&(st_msg_gyro_trouble_info.gyro_trouble)),
                 reinterpret_cast<void *>(&(st_master_gyro_trouble.uc_data)),
                 sizeof(st_msg_gyro_trouble_info.gyro_trouble));

#if VEHICLE_SENS_DELIVERY_CTRL_DEBUG_DIAG
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                  "#[DIAG] ul_did                  = VEHICLE_DID_GYRO_TROUBLE");
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                  "#[DIAG] st_msg_gyro_trouble_info.size    = %d", st_msg_gyro_trouble_info.size);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                  "#[DIAG] st_msg_gyro_trouble_info.gyro_trouble    = 0x%08X \r\n",
                  st_msg_gyro_trouble_info.gyro_trouble);
#endif /* VEHICLE_SENS_DELIVERY_CTRL_DEBUG_DIAG */
    /* Since the undefined state is not a device specification,Do not deliver to the app side */
    if (st_msg_gyro_trouble_info.gyro_trouble != GYRO_UNFIXED) {

        /* Send GyroTrouble to API-caller */
        ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
                             pst_pno_tbl->st_pno_data[pno_index].us_pno,
                             CID_VEHICLE_SENSORMOTION_GYROTROUBLE,
                             sizeof(SENSORMOTION_MSG_GYROTROUBLEINFO_DAT),
                             (const void *)&st_msg_gyro_trouble_info);
        uc_result = SENSLOG_RES_SUCCESS;
        if (ret != RET_NORMAL) {
            uc_result = SENSLOG_RES_FAIL;
        }
        SensLogWriteOutputData(SENSLOG_DATA_O_SYS,
                                ul_did,
                                pst_pno_tbl->st_pno_data[pno_index].us_pno,
                                reinterpret_cast<uint8_t *>(&st_msg_gyro_trouble_info),
                                sizeof(SENSORMOTION_MSG_GYROTROUBLEINFO_DAT),
                                uc_result);
#if VEHICLE_SENS_DELIVERY_CTRL_DEBUG_DIAG
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "#[DIAG] GyroTrouble Delivery");
#endif /* VEHICLE_SENS_DELIVERY_CTRL_DEBUG_DIAG */
    }
    return uc_result;
}
// LCOV_EXCL_STOP

void VehicleSensDeliveryAntenna(DID ul_did, u_int8 uc_current_get_method, int32 pno_index, const VEHICLESENS_DELIVERY_PNO_TBL *pst_pno_tbl) {  // LCOV_EXCL_START 8 : dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    VEHICLESENS_DATA_MASTER_GPS_ANTENNA_STATUS    gps_antenna_status;
    VEHICLESENS_DELIVERY_FORMAT                    delivery_data;
    u_int16                                        length;

    VehicleSensGetDataMasterGpsAntennaStatus(ul_did, uc_current_get_method, &gps_antenna_status);

    delivery_data.header.did                    = gps_antenna_status.ul_did;
    delivery_data.header.size                = gps_antenna_status.us_size;
    delivery_data.header.rcv_flag                = gps_antenna_status.uc_rcvflag;
    delivery_data.header.sensor_cnt            = gps_antenna_status.uc_sensor_cnt;
    (void)memcpy(reinterpret_cast<void *>(&delivery_data.data[0]),
                 reinterpret_cast<void *>(&gps_antenna_status.uc_data),
                 (size_t)delivery_data.header.size);

    length = static_cast<u_int16>(static_cast<u_int32>(sizeof(delivery_data.header)) + delivery_data.header.size);

    /* Call Vehicle Sensor Information Notification Transmission Process.*/
    (void)VehicleSndMsg(PNO_VEHICLE_SENSOR,
                         pst_pno_tbl->st_pno_data[pno_index].us_pno,
                         CID_VEHICLESENS_VEHICLE_INFO,
                         length,
                         (const void *)&delivery_data);
}
// LCOV_EXCL_STOP

u_int8 VehicleSensDeliveryOther(DID ul_did, u_int8 uc_current_get_method, int32 pno_index,
                            u_int32* cid,  // NOLINT(readability/nolint)
                            VEHICLESENS_DATA_MASTER* stmaster,  // NOLINT(readability/nolint)
                            const VEHICLESENS_DELIVERY_PNO_TBL *pst_pno_tbl) {
    RET_API                                 ret = RET_NORMAL;  /* API return value                       */
    u_int8                                  uc_result = SENSLOG_RES_SUCCESS; /* Send/Receive result      */
/* Determine CID */
    if (ul_did == VEHICLE_DID_LOCATION_LONLAT_NAVI) {
        *cid = CID_POSIF_REGISTER_LISTENER_LONLAT;
    } else if (ul_did == VEHICLE_DID_LOCATION_ALTITUDE_NAVI) {  // LCOV_EXCL_BR_LINE 6:VEHICLE_DID_LOCATION_ALTITUDE_NAVI no API to pass in  // NOLINT(whitespace/line_length)
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        *cid = CID_POSIF_REGISTER_LISTENER_ALTITUDE;  // LCOV_EXCL_LINE 8: dead code
    } else if ((ul_did == VEHICLE_DID_MOTION_SPEED_NAVI) ||
            (ul_did == VEHICLE_DID_MOTION_SPEED_INTERNAL)) {
        *cid = CID_VEHICLE_SENSORMOTION_SPEED;
    } else if (ul_did == VEHICLE_DID_MOTION_HEADING_NAVI) {
        *cid = CID_POSIF_REGISTER_LISTENER_HEADING;
    } else if (ul_did == VEHICLE_DID_SETTINGTIME) {
        *cid = CID_POSIF_REGISTER_LISTENER_GPS_TIME_SET_REQ;
    } else {  // LCOV_EXCL_BR_LINE 6: cannot be this one
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        *cid = 0xFFFF; /* DID error */  // LCOV_EXCL_LINE 8: dead code
    }

    /* Send vehicle sensor information notification */
    if (*cid == 0xFFFF) {  // LCOV_EXCL_BR_LINE 6: cannot be this one
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        /* Error log */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unknown DID [%d]", ul_did);
    } else {
        /* Acquire the applicable data information from the data master..*/
        (void)memset(reinterpret_cast<void *>(stmaster), 0x00, sizeof(VEHICLESENS_DATA_MASTER));
        VehicleSensGetDataMaster(ul_did, uc_current_get_method, stmaster);

        /* Call Vehicle Sensor Information Notification Transmission Process.*/
        ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
                              pst_pno_tbl->st_pno_data[pno_index].us_pno,
                              static_cast<CID>(*cid),
                              (u_int16)(stmaster->us_size),
                              (const void *)&(stmaster->uc_data[0]));
        uc_result = SENSLOG_RES_SUCCESS;
        if (ret != RET_NORMAL) {
            uc_result = SENSLOG_RES_FAIL;
        }
        SensLogWriteOutputData(SENSLOG_DATA_O_UNSPECIFIED,
                                ul_did,
                                pst_pno_tbl->st_pno_data[pno_index].us_pno,
                                reinterpret_cast<uint8_t *>(&(stmaster->uc_data[0])),
                                stmaster->us_size,
                                uc_result);
    }

    return uc_result;
}
void VehicleSensDeliveryProc(DID ul_did, u_int8 uc_chg_type, u_int8 uc_get_method) {
    u_int8                                    uc_current_get_method;    /* Data collection way                    */
    int32                                    i;                    /* Generic counters                     */
    int32                                    j;                    /* Generic counters                     */
    VEHICLESENS_DATA_MASTER                    stmaster;            /* Data master of normal data            */
    const VEHICLESENS_DELIVERY_PNO_TBL        *pst_pno_tbl;            /* Vehicle Sensor Destination PNO Table Pointer    */
    SENSOR_PKG_MSG_VSINFO                    st_pkg_master;        /* Data master for package delivery */
    u_int8                                    uc_data_cnt;            /* Counter for the number of data  */
    u_int16                                    us_index;            /* Package delivery management table reference  */
    DID                                        ul_pkg_did;            /* DID for package data acquisition  */
    u_int16                                    us_offset;            /* For offset calculation      */
    u_int16                                    us_next_offset;        /* Next offset value      */
    u_int16                                    us_boundary_adj;        /* For boundary adjustment    */
#if CONFIG_SENSOR_EXT_VALID                    /* Initial Sensor Support */
    u_int32                                    cid;
    RET_API                                    ret = RET_NORMAL;    /* API return value                        */
    u_int8                                    uc_result = SENSLOG_RES_SUCCESS;    /* Send/Receive result        */
#endif

    /* Obtain the data acquisition method from the Vehicle Selection Item List */
    uc_current_get_method = VehicleSensGetSelectionItemList(ul_did);

    if (uc_current_get_method == uc_get_method) {
        /* When the data acquisition methods match (= delivery target) */

        /* Obtain the shipping destination PNO */
        pst_pno_tbl = (const VEHICLESENS_DELIVERY_PNO_TBL *)VehicleSensMakeDeliveryPnoTbl(ul_did, uc_chg_type);

        if ((pst_pno_tbl->us_dnum) > 0) {
            /* When there is a shipping destination PNO registration */
            /* For boundary adjustment */
            us_boundary_adj = (u_int16)VEHICLESENS_BIT1 | (u_int16)VEHICLESENS_BIT0;
            /* Vehicle sensor information notification transmission process */
            for (i = 0; i < (pst_pno_tbl->us_dnum); i++) {
                if (VEHICLESENS_DELIVERY_METHOD_PACKAGE == pst_pno_tbl->st_pno_data[i].uc_method) {
                    /* When the delivery method is the package method */
                    (void)memset(reinterpret_cast<void *>(&st_pkg_master), 0, sizeof(SENSOR_PKG_MSG_VSINFO));

                    us_index = pst_pno_tbl->st_pno_data[i].us_pkg_start_idx;
                    uc_data_cnt = 0U;
                    us_offset  = 0U;
                    for (j = 0; j < SENSOR_PKG_DELIVERY_MAX; j++) {
                        ul_pkg_did = g_stpkgdelivery_tbl_mng.st_pkg_data[us_index].ul_did;    /* Get DID            */
                        st_pkg_master.usOffset[uc_data_cnt] = us_offset;         /* Offset setting    */
                        uc_current_get_method = VehicleSensGetSelectionItemList(ul_pkg_did);  /* Data collection way    */
                        if (VEHICLESENS_GETMETHOD_GPS == uc_current_get_method) {
//                            VehicleSensGetGpsDataMaster(ul_pkg_did,
//                                        uc_current_get_method,
//                                    reinterpret_cast<SENSOR_MSG_GPSDATA_DAT *>(&st_pkg_master.ucData[us_offset]));
                        } else {
                                VehicleSensGetDataMaster(ul_pkg_did,
                                    uc_current_get_method,
                                    reinterpret_cast<VEHICLESENS_DATA_MASTER *>(&st_pkg_master.ucData[us_offset]));
                        }
                        uc_data_cnt++;                                                /* Data count increment    */
                        if ((us_index == pst_pno_tbl->st_pno_data[i].us_pkg_end_idx) ||
                        (VEHICLESENS_LINK_INDEX_END == g_stpkgdelivery_tbl_mng.st_pkg_data[us_index].usdlvry_idx)) {
                            st_pkg_master.ucDNum = uc_data_cnt;                       /* To save the number of data        */
                            break;
                        } else {
                            /* By creating the following processing contents,Need to obtain an offset value */
                            us_next_offset = VehicleSensGetDataMasterOffset(ul_pkg_did);  /* Next offset calculation */
                            /* Boundary adjustment of data size */
                            if ((us_next_offset & us_boundary_adj) != 0) {
                                /* If you need to adjust */
                                /* Mask Lower Bit */
                                us_next_offset = static_cast<u_int16>(us_next_offset & ~us_boundary_adj);
                                /* Add numbers */
                                us_next_offset = static_cast<u_int16>(us_next_offset + (u_int16)VEHICLESENS_BIT2);
                            }
                            us_offset = static_cast<u_int16>(us_offset +us_next_offset);
                            /* Get next index  */
                            us_index = g_stpkgdelivery_tbl_mng.st_pkg_data[us_index].usdlvry_idx;
                        }
                    }
//                    ret = PosSndMsg(PNO_VEHICLE_SENSOR,
//                                            pst_pno_tbl->st_pno_data[i].us_pno,
//                                            CID_SENSOR_PKG_INFO,
//                                            (u_int16)sizeof(SENSOR_PKG_MSG_VSINFO),
//                                            (const void *)&st_pkg_master);
//                    uc_result = SENSLOG_RES_SUCCESS;
//                    if (ret != RET_NORMAL) {
//                        uc_result = SENSLOG_RES_FAIL;
//                    }
//                    SensLogWriteOutputData(SENSLOG_DATA_O_SYS_PKG, 0, pst_pno_tbl->st_pno_data[i].us_pno,
//                                             reinterpret_cast<uint8_t *>(&st_pkg_master),
//                                             sizeof(SENSOR_PKG_MSG_VSINFO), uc_result);
                } else {
                    /* When the delivery system is normal */
                    /* Acquire the applicable data information from the data master..*/
                    if (VEHICLESENS_GETMETHOD_GPS == uc_current_get_method) {
//                        uc_result = VehicleSensDeliveryGPS(ul_did, uc_get_method, uc_current_get_method, i,
//                            &cid, &stmaster, pst_pno_tbl);
                    }
                    else if ((VEHICLESENS_GETMETHOD_NAVI == uc_get_method)  ||  // NOLINT(readability/braces)
                            (VEHICLESENS_GETMETHOD_OTHER == uc_get_method) ||
                            (VEHICLESENS_GETMETHOD_INTERNAL == uc_get_method)) {
//                        uc_result = VehicleSensDeliveryOther(ul_did, uc_current_get_method, i,
//                            &cid, &stmaster, pst_pno_tbl);
                    }

#if CONFIG_SENSOR_EXT_VALID                /* Initial Sensor Support */
                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    else if ((ul_did == POSHAL_DID_GYRO_FST) ||    // NOLINT(readability/braces)
//                              (ul_did == POSHAL_DID_REV_FST)  ||
//                              (ul_did == POSHAL_DID_GYRO_TEMP_FST) ||
//                              (ul_did == POSHAL_DID_GSNS_X_FST) ||
//                              (ul_did == POSHAL_DID_GSNS_Y_FST) ||
//                              (ul_did == POSHAL_DID_SPEED_PULSE_FST)) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                        /* Acquire the applicable data information from the data master for the initial sensor..*/
//                        uc_result = VehicleSensDeliveryFst(ul_did, uc_get_method, i, pst_pno_tbl);
//                    }
#endif
                    else if (ul_did == VEHICLE_DID_GYRO_TROUBLE) {  // LCOV_EXCL_BR_LINE 6:DID is not used
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//                        uc_result = VehicleSensDeliveryGyro(ul_did, uc_current_get_method, i, pst_pno_tbl);  // LCOV_EXCL_LINE 8: dead code  // NOLINT(whitespace/line_length)
                    }
                    else {    // NOLINT(readability/braces)
                        (void)memset(reinterpret_cast<void *>(&stmaster), 0x00, sizeof(stmaster));
                        VehicleSensGetDataMaster(ul_did, uc_current_get_method, &stmaster);

                        /* Call Vehicle Sensor Information Notification Transmission Process.*/
//                        ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                                              pst_pno_tbl->st_pno_data[i].us_pno,
//                                              CID_VEHICLESENS_VEHICLE_INFO,
//                                              (u_int16)sizeof(VEHICLESENS_DATA_MASTER),
//                                              (const void *)&stmaster);
//                        uc_result = SENSLOG_RES_SUCCESS;
//                        if (ret != RET_NORMAL) {
//                            uc_result = SENSLOG_RES_FAIL;
//                        }
//                        SensLogWriteOutputData(SENSLOG_DATA_O_SYS,
//                                                ul_did,
//                                                pst_pno_tbl->st_pno_data[i].us_pno,
//                                                reinterpret_cast<uint8_t *>(&(stmaster.uc_data[0])),
//                                                stmaster.us_size,
//                                                uc_result);
                    }
                }
            }
        }
    }
}

//u_int8 VehicleSensFirstDeliverySens(PNO us_pno, DID ul_did, u_int8 uc_get_method,
//                                     VEHICLESENS_DATA_MASTER_FST* stmaster_fst,
//                                     VEHICLESENS_DATA_MASTER_FST* stmaster_fst_temp) {
//    RET_API                     ret = RET_NORMAL;  /* API return value                       */
//    u_int8                      uc_result = SENSLOG_RES_SUCCESS; /* Send/Receive result      */
//    /* Acquire the applicable data information from the data master for the initial sensor..*/
//    (void)memset(reinterpret_cast<void *>(stmaster_fst), 0, sizeof(VEHICLESENS_DATA_MASTER_FST));
//    (void)memset(reinterpret_cast<void *>(stmaster_fst_temp),    0, sizeof(VEHICLESENS_DATA_MASTER_FST));
//    VehicleSensGetDataMasterFst(ul_did, uc_get_method, stmaster_fst);
//
//    /* Internal debug log output */
//    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, 
//                  "[LOG:POSHAL_DID_GYRO_FST,POSHAL_DID_SPEED_PULSE_FST]");
//
//    if (stmaster_fst->partition_flg == 1) {
//        /* When the partitioning flag is enabled,Or exceeds the size that can be sent,Perform split transmission */
//        memcpy(stmaster_fst_temp, stmaster_fst, sizeof(VEHICLESENS_DATA_MASTER_FST));
//        if ((ul_did == POSHAL_DID_GYRO_FST) ||    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                (ul_did == POSHAL_DID_GSNS_X_FST) ||
//                (ul_did == POSHAL_DID_GSNS_Y_FST)) {
//            /* Internal debug log output */
//            FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, 
//                          "[CALL:VehicleSndMsg:INPOSHAL_DID_GYRO_FST Partition]");
//
//            /* 1st session */
//            /* Call Vehicle Sensor Information Notification Transmission Process.*/
//            /* Size of data that can be transmitted in one division(Same size definition used) */
//            stmaster_fst_temp->us_size = LSDRV_FSTSNS_DSIZE_GYRO;
//            ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                                  us_pno,
//                                  CID_VEHICLESENS_VEHICLE_INFO,
//                                  /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                                  static_cast<u_int16>(stmaster_fst_temp->us_size + 8),
//                                  (const void *)stmaster_fst_temp);
//            uc_result = SENSLOG_RES_SUCCESS;
//            if (ret != RET_NORMAL) {
//                uc_result = SENSLOG_RES_FAIL;
//            }
//            SensLogWriteOutputData(SENSLOG_DATA_O_SYS,
//                                    ul_did,
//                                    us_pno,
//                                    reinterpret_cast<uint8_t *>(&(stmaster_fst_temp->uc_data[0])),
//                                    stmaster_fst_temp->us_size,
//                                    uc_result);
//
//            /* Second time */
//            /* Call Vehicle Sensor Information Notification Transmission Process.*/
//            (void)memset(reinterpret_cast<void *>(&stmaster_fst_temp->uc_data[0]),
//                         0,
//                         sizeof(u_int8) * VEHICLE_DSIZE_DATA_MASTER_FST);
//            /* Size of the data - Transmission size(Same size definition used) Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            stmaster_fst_temp->us_size = static_cast<u_int16>(stmaster_fst->us_size - LSDRV_FSTSNS_DSIZE_GYRO);
//            /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            memcpy(&stmaster_fst_temp->uc_data[0],
//                   &stmaster_fst->uc_data[LSDRV_FSTSNS_DSIZE_GYRO],
//                   stmaster_fst_temp->us_size);
//            ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                                  us_pno,
//                                  CID_VEHICLESENS_VEHICLE_INFO,
//                                  /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                                  static_cast<u_int16>(stmaster_fst_temp->us_size + 8),
//                                  (const void *)stmaster_fst_temp);
//            uc_result = SENSLOG_RES_SUCCESS;
//            if (ret != RET_NORMAL) {
//                uc_result = SENSLOG_RES_FAIL;
//            }
//            SensLogWriteOutputData(SENSLOG_DATA_O_SYS, ul_did, us_pno,
//                                    reinterpret_cast<uint8_t *>(&(stmaster_fst_temp->uc_data[0])),
//                                    stmaster_fst_temp->us_size, uc_result);
//        } else if (ul_did == POSHAL_DID_REV_FST) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            /* Internal debug log output */
//            FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, 
//                          "[CALL:VehicleSndMsg:INPOSHAL_DID_REV_FST Partition]");
//
//            /* 1st session */
//            /* Call Vehicle Sensor Information Notification Transmission Process.*/
//            stmaster_fst_temp->us_size = LSDRV_FSTSNS_DSIZE_REV;    /* Size of data that can be transmitted in one division */
//            ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                                  us_pno,
//                                  CID_VEHICLESENS_VEHICLE_INFO,
//                                  /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                                  static_cast<u_int16>(stmaster_fst_temp->us_size + 8),
//                                  (const void *)stmaster_fst_temp);
//            uc_result = SENSLOG_RES_SUCCESS;
//            if (ret != RET_NORMAL) {
//                uc_result = SENSLOG_RES_FAIL;
//            }
//            SensLogWriteOutputData(SENSLOG_DATA_O_SYS, ul_did, us_pno,
//                                   reinterpret_cast<uint8_t *>(&(stmaster_fst_temp->uc_data[0])),
//                                   stmaster_fst_temp->us_size, uc_result);
//
//            /* Second time */
//            /* Call Vehicle Sensor Information Notification Transmission Process.*/
//            (void)memset(reinterpret_cast<void *>(&stmaster_fst_temp->uc_data[0]),
//                         0,
//                         sizeof(u_int8) * VEHICLE_DSIZE_DATA_MASTER_FST);
//            /* Size of the data - Transmission size Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            stmaster_fst_temp->us_size = static_cast<u_int16>(stmaster_fst->us_size - LSDRV_FSTSNS_DSIZE_REV);
//            /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            memcpy(&stmaster_fst_temp->uc_data[0],
//                   &stmaster_fst->uc_data[LSDRV_FSTSNS_DSIZE_REV],
//                   stmaster_fst_temp->us_size);
//            ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                                  us_pno,
//                                  CID_VEHICLESENS_VEHICLE_INFO,
//                                  /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                                  static_cast<u_int16>(stmaster_fst_temp->us_size + 8),
//                                  (const void *)stmaster_fst_temp);
//            uc_result = SENSLOG_RES_SUCCESS;
//            if (ret != RET_NORMAL) {
//                uc_result = SENSLOG_RES_FAIL;
//            }
//            SensLogWriteOutputData(SENSLOG_DATA_O_SYS, ul_did, us_pno,
//                                    reinterpret_cast<uint8_t *>(&(stmaster_fst_temp->uc_data[0])),
//                                    stmaster_fst_temp->us_size, uc_result);
//        } else {
//            /* Internal debug log output */
//            FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, 
//                          "[CALL:Vehicle_SndMsg:POSHAL_DID_SPEED_PULSE_FST Partition]");
//
//            /* 1st session */
//            /* Call Vehicle Sensor Information Notification Transmission Process.*/
//            /* Size of data that can be transmitted in one division(Same size definition used) */
//            stmaster_fst_temp->us_size = LSDRV_FSTSNS_DSIZE_GYRO_TEMP;
//            ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                                    us_pno,
//                                    CID_VEHICLESENS_VEHICLE_INFO,
//                                    /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                                    static_cast<u_int16>(stmaster_fst_temp->us_size + 8),
//                                    (const void *)stmaster_fst_temp);
//            uc_result = SENSLOG_RES_SUCCESS;
//            if (ret != RET_NORMAL) {
//                uc_result = SENSLOG_RES_FAIL;
//            }
//            SensLogWriteOutputData(SENSLOG_DATA_O_SYS, ul_did, us_pno,
//                                    reinterpret_cast<uint8_t *>(&(stmaster_fst_temp->uc_data[0])),
//                                    stmaster_fst_temp->us_size, uc_result);
//
//            /* Second time */
//            /* Call Vehicle Sensor Information Notification Transmission Process.*/
//            (void)memset(reinterpret_cast<void *>(&stmaster_fst_temp->uc_data[0]),
//                         0,
//                         sizeof(u_int8) * VEHICLE_DSIZE_DATA_MASTER_FST);
//            /* Size of the data - Transmission size(Same size definition used) Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            stmaster_fst_temp->us_size = static_cast<u_int16>(stmaster_fst->us_size - LSDRV_FSTSNS_DSIZE_GYRO_TEMP);
//            /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            memcpy(&stmaster_fst_temp->uc_data[0],
//                   &stmaster_fst->uc_data[LSDRV_FSTSNS_DSIZE_GYRO_TEMP],
//                   stmaster_fst_temp->us_size);
//            ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                                  us_pno,
//                                  CID_VEHICLESENS_VEHICLE_INFO,
//                                  /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                                  static_cast<u_int16>(stmaster_fst_temp->us_size + 8),
//                                  (const void *)stmaster_fst_temp);
//            uc_result = SENSLOG_RES_SUCCESS;
//            if (ret != RET_NORMAL) {
//                uc_result = SENSLOG_RES_FAIL;
//            }
//            SensLogWriteOutputData(SENSLOG_DATA_O_SYS, ul_did, us_pno,
//                                    reinterpret_cast<uint8_t *>(&(stmaster_fst_temp->uc_data[0])),
//                                    stmaster_fst_temp->us_size, uc_result);
//        }
//    } else {
//        /* Internal debug log output */
//        FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "[CALL:VehicleSndMsg]");
//
//        /* Call Vehicle Sensor Information Notification Transmission Process.*/
//        ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                              us_pno,
//                              CID_VEHICLESENS_VEHICLE_INFO,
//                              /* Data size + data information Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                              static_cast<u_int16>(stmaster_fst->us_size + 8),
//                              (const void *)stmaster_fst);
//        uc_result = SENSLOG_RES_SUCCESS;
//        if (ret != RET_NORMAL) {
//            uc_result = SENSLOG_RES_FAIL;
//        }
//        SensLogWriteOutputData(SENSLOG_DATA_O_SYS, ul_did, us_pno,
//                                reinterpret_cast<uint8_t *>(&(stmaster_fst->uc_data[0])),
//                                stmaster_fst->us_size, uc_result);
//    }
//
//    return uc_result;
//}

u_int8 VehicleSensFirstDeliveryOther(PNO us_pno, DID ul_did, u_int8 uc_get_method,
                                    u_int32* cid,
                                    VEHICLESENS_DATA_MASTER* stmaster) {
    RET_API                     ret = RET_NORMAL;  /* API return value                       */
    u_int8                      uc_result = SENSLOG_RES_SUCCESS; /* Send/Receive result      */
    /* Determine CID */
    if (ul_did == VEHICLE_DID_LOCATION_LONLAT_NAVI) {
        *cid = CID_POSIF_REGISTER_LISTENER_LONLAT;
    } else if (ul_did == VEHICLE_DID_LOCATION_ALTITUDE_NAVI) {    // LCOV_EXCL_BR_LINE 6:VEHICLE_DID_LOCATION_ALTITUDE_NAVI no API to pass in  // NOLINT(whitespace/line_length)
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        *cid = CID_POSIF_REGISTER_LISTENER_ALTITUDE;  // LCOV_EXCL_LINE 8: dead code
    } else if ((ul_did == VEHICLE_DID_MOTION_SPEED_NAVI) ||
            (ul_did == VEHICLE_DID_MOTION_SPEED_INTERNAL)) {
        *cid = CID_VEHICLE_SENSORMOTION_SPEED;
    } else if (ul_did == VEHICLE_DID_MOTION_HEADING_NAVI) {
        *cid = CID_POSIF_REGISTER_LISTENER_HEADING;
    } else if (ul_did == VEHICLE_DID_SETTINGTIME) {
        *cid = CID_POSIF_REGISTER_LISTENER_GPS_TIME_SET_REQ;
    } else {  // LCOV_EXCL_BR_LINE 6: cannot be this one
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        *cid = 0xFFFF; /* DID error */  // LCOV_EXCL_LINE 8: dead code
    }

    /* Vehicle sensor information notification transmission */
    if (*cid == 0xFFFF) {  // LCOV_EXCL_BR_LINE 6: cannot be this one
        /* Error log */
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unknown DID [%d]", ul_did);  // LCOV_EXCL_LINE 8: dead code  // NOLINT(whitespace/line_length)
    } else {
        /* Acquire the applicable data information from the data master..*/
        (void)memset(reinterpret_cast<void *>(stmaster), 0x00, sizeof(VEHICLESENS_DATA_MASTER));
        VehicleSensGetDataMaster(ul_did, uc_get_method, stmaster);

        /* Call Vehicle Sensor Information Notification Transmission Process.*/
        ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
                              us_pno,
                              static_cast<CID>(*cid),
                              (u_int16)stmaster->us_size,
                              (const void *)&(stmaster->uc_data[0]));
        uc_result = SENSLOG_RES_SUCCESS;
        if (ret != RET_NORMAL) {
            uc_result = SENSLOG_RES_FAIL;
        }
        SensLogWriteOutputData(SENSLOG_DATA_O_UNSPECIFIED,
                                ul_did,
                                us_pno,
                                reinterpret_cast<uint8_t *>(&(stmaster->uc_data[0])),
                                stmaster->us_size,
                                uc_result);
    }

    return uc_result;
}
/*******************************************************************************
* MODULE    : VehicleSensFirstDelivery
* ABSTRACT  : Vehicle Sensor Initial Data Delivery Process
* FUNCTION  : Deliver the initial data to the destination.
* ARGUMENT  : us_pno    :Addresses for delivery NO
*             ul_did    :Data ID
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensFirstDelivery(PNO us_pno, DID ul_did) {
    u_int8 uc_get_method;
    VEHICLESENS_DATA_MASTER stmaster;
#if CONFIG_SENSOR_EXT_VALID                            /* Initial Sensor Support */
    VEHICLESENS_DATA_MASTER_FST stMasterFst;        /* Master of initial sensor data            */
    VEHICLESENS_DATA_MASTER_FST    stMasterFst_temp;    /* For temporary storage                        */
    u_int32                        cid;
    SENSORLOCATION_MSG_LONLATINFO    *pLonLatMsg;
    SENSORLOCATION_MSG_ALTITUDEINFO    *pAltitudeMsg;
    SENSORMOTION_MSG_HEADINGINFO    *pHeadingMsg;
    RET_API                        ret = RET_NORMAL;    /* API return value                        */
    u_int8                        uc_result = SENSLOG_RES_SUCCESS;    /* Send/Receive result        */
#endif

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+ [ul_did = 0x%x us_pno:0x%x]", ul_did, us_pno);

    /* Obtain the data acquisition method.*/
    uc_get_method = VehicleSensGetSelectionItemList(ul_did);

    if (VEHICLESENS_GETMETHOD_GPS == uc_get_method) {
//        SENSOR_MSG_GPSDATA_DAT            gps_master;
        VEHICLESENS_DELIVERY_FORMAT        delivery_data;
        u_int16                            length;
        u_int16                            senslog_len;

//        VehicleSensGetGpsDataMaster(ul_did, uc_get_method, &gps_master);

//        if (ul_did == POSHAL_DID_GPS_TIME) {
//            /* GPS time,Because there is no header in the message to be delivered,Padding deliveryData headers */
//            (void)memcpy(reinterpret_cast<void*>(&delivery_data),
//                         reinterpret_cast<void*>(&gps_master.uc_data[0]), gps_master.us_size);
//            length = gps_master.us_size;
//            senslog_len = length;
//            cid = CID_VEHICLESENS_VEHICLE_INFO_GPS_TIME;
//        } else if (ul_did == VEHICLE_DID_LOCATION_LONLAT) {
//            pLonLatMsg = reinterpret_cast<SENSORLOCATION_MSG_LONLATINFO*>(&delivery_data);
//            /* Acquire the applicable data information from the data master..*/
//            VehicleSensGetDataMaster(ul_did, uc_get_method, &stmaster);
//            (void)memcpy(reinterpret_cast<void*>(&(pLonLatMsg->data)),
//                         reinterpret_cast<void*>(&(stmaster.uc_data[0])), stmaster.us_size);
//            length = (u_int16)stmaster.us_size;
//            senslog_len = length;
//            cid = CID_POSIF_REGISTER_LISTENER_LONLAT;
//        } else if (ul_did == VEHICLE_DID_LOCATION_ALTITUDE) {
//            pAltitudeMsg = reinterpret_cast<SENSORLOCATION_MSG_ALTITUDEINFO*>(&delivery_data);
//            /* Acquire the applicable data information from the data master..*/
//            VehicleSensGetDataMaster(ul_did, uc_get_method, &stmaster);
//            (void)memcpy(reinterpret_cast<void*>(&(pAltitudeMsg->data)),
//                         reinterpret_cast<void*>(&stmaster.uc_data[0]), stmaster.us_size);
//            length = (u_int16)stmaster.us_size;
//            senslog_len = length;
//            cid = CID_POSIF_REGISTER_LISTENER_ALTITUDE;
//        } else if (ul_did == VEHICLE_DID_MOTION_HEADING) {
//            pHeadingMsg = reinterpret_cast<SENSORMOTION_MSG_HEADINGINFO*>(&delivery_data);
//            /* Acquire the applicable data information from the data master..*/
//            VehicleSensGetDataMaster(ul_did, uc_get_method, &stmaster);
//            (void)memcpy(reinterpret_cast<void*>(&(pHeadingMsg->data)),
//                         reinterpret_cast<void*>(&stmaster.uc_data[0]), stmaster.us_size);
//            length = (u_int16)stmaster.us_size;
//            senslog_len = length;
//            cid = CID_POSIF_REGISTER_LISTENER_HEADING;
//        } else {
//
//            delivery_data.header.did                    = gps_master.ul_did;
//            delivery_data.header.size                = gps_master.us_size;
//            delivery_data.header.rcv_flag                = gps_master.uc_rcv_flag;
//            delivery_data.header.sensor_cnt            = gps_master.uc_sns_cnt;
//            (void)memcpy(reinterpret_cast<void *>(&delivery_data.data[0]),
//                         reinterpret_cast<void *>(&gps_master.uc_data[0]),
//                         (size_t)delivery_data.header.size);
//
//            length = static_cast<u_int16>((u_int16)sizeof(delivery_data.header) + delivery_data.header.size);
//            senslog_len = delivery_data.header.size;
//            cid = CID_VEHICLESENS_VEHICLE_INFO;
//        }

        /* Call Vehicle Sensor Information Notification Transmission Process.*/
//        ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
//                              us_pno,
//                              static_cast<CID>(cid),
//                              length,
//                              (const void *)&delivery_data);
//        uc_result = SENSLOG_RES_SUCCESS;
//        if (ret != RET_NORMAL) {
//            uc_result = SENSLOG_RES_FAIL;
//        }
//        if (cid != CID_VEHICLESENS_VEHICLE_INFO) {
//            SensLogWriteOutputData(SENSLOG_DATA_O_UNSPECIFIED, ul_did, us_pno,
//                                     reinterpret_cast<uint8_t *>(&delivery_data), senslog_len, uc_result);
//        } else {
//            SensLogWriteOutputData(SENSLOG_DATA_O_UNSPECIFIED, ul_did, us_pno,
//                                   reinterpret_cast<uint8_t *>(&(delivery_data.data[0])), senslog_len, uc_result);
//        }
    }
    else if ((VEHICLESENS_GETMETHOD_NAVI == uc_get_method)  ||    // NOLINT(readability/braces)
            (VEHICLESENS_GETMETHOD_OTHER == uc_get_method) ||
            (VEHICLESENS_GETMETHOD_INTERNAL == uc_get_method)) {
        uc_result = VehicleSensFirstDeliveryOther(us_pno, ul_did, uc_get_method, &cid, &stmaster);
    }
#if CONFIG_SENSOR_EXT_VALID                /* Initial Sensor Support */
    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//    else if ((ul_did == POSHAL_DID_GYRO_FST) ||  // NOLINT(readability/braces)
//             (ul_did == POSHAL_DID_REV_FST) ||
//             (ul_did == POSHAL_DID_GYRO_TEMP_FST) ||
//             (ul_did == POSHAL_DID_GSNS_X_FST) ||
//             (ul_did == POSHAL_DID_GSNS_Y_FST) ||
//             (ul_did == POSHAL_DID_SPEED_PULSE_FST)) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//        /* Acquire the applicable data information from the data master for the initial sensor..*/
//        uc_result = VehicleSensFirstDeliverySens(us_pno, ul_did, uc_get_method, &stMasterFst, &stMasterFst_temp);
//    }
#endif
    else if (ul_did == VEHICLE_DID_GYRO_TROUBLE) {  // LCOV_EXCL_BR_LINE 8 : DID in not used
    // LCOV_EXCL_START 8: DID is not used
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        VEHICLESENS_DATA_MASTER_GYRO_TROUBLE    st_master_gyro_trouble;
        SENSORMOTION_MSG_GYROTROUBLEINFO_DAT    st_msg_gyro_trouble_info;

        /* Initialization */
        st_master_gyro_trouble.uc_reserve = 0;
        st_msg_gyro_trouble_info.reserve = 0;

        VehicleSensGetDataMasterGyroTrouble(ul_did, uc_get_method, &st_master_gyro_trouble);

        /* Size storage(GYROTROUBLE)  */
        st_msg_gyro_trouble_info.size = st_master_gyro_trouble.us_size;

        /* Set the GyroTrouble */
        (void)memcpy(reinterpret_cast<void *>(&(st_msg_gyro_trouble_info.gyro_trouble)),
                     reinterpret_cast<void *>(&(st_master_gyro_trouble.uc_data)),
                     sizeof(st_msg_gyro_trouble_info.gyro_trouble));

#if VEHICLE_SENS_DELIVERY_CTRL_DEBUG_DIAG
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "#[DIAG] ul_did         = VEHICLE_DID_GYRO_TROUBLE");
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                              "#[DIAG] st_msg_gyro_trouble_info.size = %d",
                              st_msg_gyro_trouble_info.size);
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                "#[DIAG] st_msg_gyro_trouble_info.gyro_trouble    = 0x%08X \r\n",
                st_msg_gyro_trouble_info.gyro_trouble);
#endif /* VEHICLE_SENS_DELIVERY_CTRL_DEBUG_DIAG */
        /* Since the undefined state is not a device specification,Do not deliver to the app for the first time */
        if (st_msg_gyro_trouble_info.gyro_trouble != GYRO_UNFIXED) {

            /* Send GyroTrouble to API-caller */
            ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
                                 us_pno,
                                 CID_VEHICLE_SENSORMOTION_GYROTROUBLE,
                                 sizeof(SENSORMOTION_MSG_GYROTROUBLEINFO_DAT),
                                 (const void *)&st_msg_gyro_trouble_info);
            uc_result = SENSLOG_RES_SUCCESS;
            if (ret != RET_NORMAL) {
                uc_result = SENSLOG_RES_FAIL;
            }
            SensLogWriteOutputData(SENSLOG_DATA_O_SYS, ul_did, us_pno,
                                    reinterpret_cast<uint8_t *>(&st_msg_gyro_trouble_info),
                                    sizeof(SENSORMOTION_MSG_GYROTROUBLEINFO_DAT),
                                    uc_result);
#if VEHICLE_SENS_DELIVERY_CTRL_DEBUG_DIAG
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "#[DIAG] GyroTrouble FirstDelivery");
#endif /* VEHICLE_SENS_DELIVERY_CTRL_DEBUG_DIAG */
        }
    // LCOV_EXCL_STOP
    }
    else {    // NOLINT(readability/braces)
        (void)memset(reinterpret_cast<void *>(&stmaster), 0x00, sizeof(stmaster));
        /* Acquire the applicable data information from the data master..*/
        VehicleSensGetDataMaster(ul_did, uc_get_method, &stmaster);

        /* Call Vehicle Sensor Information Notification Transmission Process.*/
        ret = VehicleSndMsg(PNO_VEHICLE_SENSOR,
                              us_pno,
                              CID_VEHICLESENS_VEHICLE_INFO,
                              (u_int16)sizeof(VEHICLESENS_DATA_MASTER),
                              (const void *)&stmaster);
        uc_result = SENSLOG_RES_SUCCESS;
        if (ret != RET_NORMAL) {
            uc_result = SENSLOG_RES_FAIL;
        }
        SensLogWriteOutputData(SENSLOG_DATA_O_SYS, ul_did, us_pno,
                                reinterpret_cast<uint8_t *>(&(stmaster.uc_data[0])),
                                stmaster.us_size, uc_result);
    }
    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "-");
}

/*******************************************************************************
* MODULE    : VehicleSensFirstPkgDelivery
* ABSTRACT  : Vehicle Sensor Initial Package Data Delivery Process
* FUNCTION  : Deliver the initial package data to the destination.
* ARGUMENT  : *pst_data    :Data portion pointer of the message buffer
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensFirstPkgDelivery(const SENSOR_MSG_DELIVERY_ENTRY_DAT *pst_data) {  // LCOV_EXCL_START 8: dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8                        uc_get_method;            /* Data collection way                */
    int32                        i;                        /* Generic counters                 */
    SENSOR_PKG_MSG_VSINFO        st_pkg_master;            /* Data master for package delivery    */
    DID                            ul_pkg_did;                /* DID for package data acquisition    */
    u_int16                        us_offset = 0;            /* For offset calculation                */
    u_int16                        us_next_offset;            /* Next offset value                */
    u_int16                        us_boundary_adj;            /* For boundary adjustment                */
    RET_API                        ret = RET_NORMAL;        /* API return value                    */
    u_int8                        uc_result = 0;            /* Send/Receive result                */

    (void)memset(reinterpret_cast<void *>(&st_pkg_master), 0, sizeof(SENSOR_PKG_MSG_VSINFO));
    /* For boundary adjustment */
    us_boundary_adj = (u_int16)VEHICLESENS_BIT1 | (u_int16)VEHICLESENS_BIT0;
    for (i = 0; i < pst_data->pkg_num; i++) {
        ul_pkg_did = pst_data->did[i];                    /* Get DID                */
        st_pkg_master.usOffset[i] = us_offset;                /* Offset setting        */
        /* Data collection way        */
        uc_get_method = VehicleSensGetSelectionItemList(ul_pkg_did);
        if (VEHICLESENS_GETMETHOD_GPS == uc_get_method) {
//            VehicleSensGetGpsDataMaster(ul_pkg_did,
//                                         uc_get_method,
//                                         reinterpret_cast<SENSOR_MSG_GPSDATA_DAT *>(&st_pkg_master.ucData[us_offset]));
        } else {
            VehicleSensGetDataMaster(ul_pkg_did,
                                      uc_get_method,
                                      reinterpret_cast<VEHICLESENS_DATA_MASTER *>(&st_pkg_master.ucData[us_offset]));
        }
        /* Next offset calculation    */
        /* Boundary adjustment of data size */
        us_next_offset = VehicleSensGetDataMasterOffset(ul_pkg_did);
        if ((us_next_offset & us_boundary_adj) != 0) {
            /* If you need to adjust */
            us_next_offset = static_cast<u_int16>(us_next_offset & ~us_boundary_adj);          /* Mask Lower Bit */
            us_next_offset = static_cast<u_int16>(us_next_offset + (u_int16)VEHICLESENS_BIT2);  /* Add numbers */
        }
        us_offset = static_cast<u_int16>(us_offset + us_next_offset);
    }

    st_pkg_master.ucDNum = pst_data->pkg_num;                /* To save the number of data            */
    ret = PosSndMsg(PNO_VEHICLE_SENSOR,
                      pst_data->pno,
                      CID_SENSOR_PKG_INFO,
                      (u_int16)sizeof(SENSOR_PKG_MSG_VSINFO),
                      (const void *)&st_pkg_master);
    uc_result = SENSLOG_RES_SUCCESS;
    if (ret != RET_NORMAL) {
        uc_result = SENSLOG_RES_FAIL;
    }
    SensLogWriteOutputData(SENSLOG_DATA_O_SYS_PKG, 0, pst_data->pno,
                            reinterpret_cast<uint8_t *>(&st_pkg_master),
                            sizeof(SENSOR_PKG_MSG_VSINFO),
                            uc_result);
}
// LCOV_EXCL_STOP

/* Ignore->MISRA-C++:2008 Rule 2-7-2 */
#if CONFIG_SENSOR_EXT_VALID                /* Initial Sensor Support */
/*******************************************************************************
* MODULE    : VehicleSensFirstPkgDeliveryExt
* ABSTRACT  : Vehicle Sensor Initial Expansion Package Data Delivery Process
* FUNCTION  : Deliver the initial expansion package data to the destination.
* ARGUMENT  : *pst_data    :Data portion pointer of the message buffer
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensFirstPkgDeliveryExt(const SENSOR_MSG_DELIVERY_ENTRY_DAT *pst_data) {
    u_int8                        ucGetMethod;            /* Data collection way                */
    int32                        i;                        /* Generic counters                 */
    static SENSOR_PKG_MSG_VSINFO    stPkgMaster;        /* Data master for package delivery    */
    DID                            ulPkgDid;                /* DID for package data acquisition    */
    u_int16                        usOffset = 0;            /* For offset calculation                */
    u_int16                        usNextOffset;            /* Next offset value                */
    u_int16                        usBoundaryAdj;            /* For boundary adjustment                */
    static VEHICLESENS_DATA_MASTER_EXT stExtDataTemp[8];/* Extended data master temporary storage area */
    u_int16                        usDataCnt[8] = {0};        /* For storing the number of data items                */
    u_int8                        ucDivideCnt;            /* Total number of partitions      */
    u_int8                        ucDivide100Cnt = 0;            /* Total number of 100 partitions      */
    u_int8                        ucDivideSendCnt;        /* Number of divided transmissions    */
    u_int16                        usDivideSendSize[8] = {0};    /* Split Send Size            */
    u_int16                        ucDivideSendPoint;        /* Split transmission data acquisition position   */
    u_int8                        ucDataBreak;            /* Storage area of all data undelivered flag  */
    RET_API                        ret = RET_NORMAL;        /* API return value                    */
    u_int8                        uc_result = 0;            /* Send/Receive result                */

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");

    /* For boundary adjustment */
    usBoundaryAdj = (u_int16)VEHICLESENS_BIT1 | (u_int16)VEHICLESENS_BIT0;
    /* #Polaris_004 START */    /* Ignore->MISRA-C++:2008 Rule 2-7-2 */
    (void)memset(reinterpret_cast<void *>(&stExtDataTemp), 0, sizeof(stExtDataTemp));
    for (i = 0; i < pst_data->pkg_num; i++) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
        ulPkgDid    = pst_data->did[i];                                /* Get DID                */
        ucGetMethod    = VehicleSensGetSelectionItemList(ulPkgDid);        /* Data collection way        */
        if (VEHICLESENS_GETMETHOD_GPS < ucGetMethod) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                    "VEHICLESENS_DELIVERYCTRL: VehicleSensGetSelectionItemList error :%d\r\n", ucGetMethod);
        }

//        if ((ulPkgDid == POSHAL_DID_GYRO_EXT)   ||    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                (ulPkgDid == POSHAL_DID_GSNS_X)      ||     /* Ignore->MISRA-C++:2008 Rule 2-7-2 */
//                (ulPkgDid == POSHAL_DID_GSNS_Y)      ||     /* Ignore->MISRA-C++:2008 Rule 2-7-2 */
//                (ulPkgDid == POSHAL_DID_SPEED_PULSE) ||    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                (ulPkgDid == POSHAL_DID_REV) ||
//                (ulPkgDid == POSHAL_DID_GYRO_TEMP) ||
//                (ulPkgDid == POSHAL_DID_PULSE_TIME) ||
//                (ulPkgDid == POSHAL_DID_SNS_COUNTER)) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//            /* Store in the extended data master information buffer */
//            VehicleSensGetDataMasterExt(ulPkgDid, ucGetMethod, &stExtDataTemp[i]);
//            /* Obtain the number of data items */
//            if ((ulPkgDid == POSHAL_DID_SNS_COUNTER) ||
//                    (ulPkgDid == POSHAL_DID_REV)) {
//                usDataCnt[i]        = stExtDataTemp[i].us_size;            /* 1data 1byte */
//                /* Store the transmission size for 10 items */
//                usDivideSendSize[i]    = 10;
//            } else if (ulPkgDid == POSHAL_DID_GYRO_TEMP) {
//                usDataCnt[i]        = stExtDataTemp[i].us_size / 2;        /* 1data 2byte */
//                /* Store the transmission size for 10 items */
//                usDivideSendSize[i]    = 20;
//            }
//            else if (ulPkgDid == POSHAL_DID_PULSE_TIME) {    // NOLINT(readability/braces)
//                usDataCnt[i]        = stExtDataTemp[i].us_size / 132;    /* 1data 132byte */
//                /* Store the transmission size for 10 items */
//                usDivideSendSize[i]    = 1320;
//            }
//            else {  // NOLINT(readability/braces)
//                usDataCnt[i]        = stExtDataTemp[i].us_size / 2;  /* 1data 2byte Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                /* Store the transmission size for 100 items */
//                usDivideSendSize[i]    = 200;
//
//                // divide cnt is 100
//                if (((usDataCnt[i] % VEHICLESENS_PKG_EXT_SEND_MAX_10DATA) > 0) || (usDataCnt[i] == 0)) {
//                    ucDivide100Cnt = static_cast<u_int8>((usDataCnt[i] / VEHICLESENS_PKG_EXT_SEND_MAX_10DATA) + 1);
//                } else {
//                    ucDivide100Cnt = static_cast<u_int8>(usDataCnt[i] / VEHICLESENS_PKG_EXT_SEND_MAX_10DATA);
//                }
//            }
//        }
    }
    /* All-data undelivered flag holding Ignore->MISRA-C ++: 2008 Rule 5-0-5 */
    ucDataBreak = static_cast<u_int8>(gstPkgTempExt.data_break);

    /* From the number of data items in the acquired buffer,Calculate the number of transmissions */
    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
    if (((usDataCnt[0] % VEHICLESENS_PKG_EXT_SEND_MAX) > 0) || (usDataCnt[0] == 0)) {
        /* If there is a remainder,,Division result + 1 */
        /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
        ucDivideCnt = static_cast<u_int8>((usDataCnt[0] / VEHICLESENS_PKG_EXT_SEND_MAX) + 1);
    } else {
        /* If there is no remainder,,The result of division is the total number of transmissions. */
        /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
        ucDivideCnt = static_cast<u_int8>(usDataCnt[0] / VEHICLESENS_PKG_EXT_SEND_MAX);
    }

    // if ucDivide100cnt is valid (greater than 0)
    ucDivideCnt = (ucDivide100Cnt > 0) ? ucDivide100Cnt : ucDivideCnt;

    ucDivideSendCnt        = 0;        /* Number of divided transmissions */
    while (ucDivideSendCnt < ucDivideCnt) {
        /* Clear send message buffer */
        (void)memset(reinterpret_cast<void *>(&stPkgMaster), 0, sizeof(SENSOR_PKG_MSG_VSINFO));
        usOffset    = 0;
        for (i = 0; i < pst_data->pkg_num; i++) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
            ulPkgDid                    = pst_data->did[i];    /* Get DID                */
            stPkgMaster.usOffset[i]        = usOffset;                /* Offset setting        */
            /*  copy Data ID of extended data master structure,Size of the data,Receive flag,Reserved */
            memcpy(reinterpret_cast<void *>(&stPkgMaster.ucData[usOffset]),
                   reinterpret_cast<void *>(&stExtDataTemp[i]), 8);
//            if ((ulPkgDid == POSHAL_DID_SNS_COUNTER) ||
//                    (ulPkgDid == POSHAL_DID_REV) ||
//                    (ulPkgDid == POSHAL_DID_PULSE_TIME) ||
//                    (ulPkgDid == POSHAL_DID_GYRO_TEMP)) {
//                if (usDataCnt[i] > VEHICLESENS_PKG_EXT_SEND_MAX) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    /* Calculate the data acquisition position */
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    ucDivideSendPoint = static_cast<u_int16>((u_int16)ucDivideSendCnt * usDivideSendSize[i]);
//                    /* Update the data size*/
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    stPkgMaster.ucData[usOffset + 4]        = (u_int8)usDivideSendSize[i];
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    stPkgMaster.ucData[usOffset + 5]        = (u_int8)(usDivideSendSize[i] >> 8);
//                    /* Create 10 divided transmission data of sensor counters of extended data master structure */
//                    memcpy(reinterpret_cast<void *>(&stPkgMaster.ucData[usOffset + 8]),
//                           reinterpret_cast<void *>(&stExtDataTemp[i].uc_data[ucDivideSendPoint]),
//                           usDivideSendSize[i]);    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    /* Subtract the number of created transmission data */
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    usDataCnt[i] = static_cast<u_int16>(usDataCnt[i] - VEHICLESENS_PKG_EXT_SEND_MAX);
//                } else {
//                    /* Calculate the data acquisition position */
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    ucDivideSendPoint = static_cast<u_int16>((u_int16)ucDivideSendCnt * usDivideSendSize[i]);
//                    /* Update the data size*/
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    stPkgMaster.ucData[usOffset + 4] = (u_int8)(stExtDataTemp[i].us_size - (u_int16)ucDivideSendPoint);
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 Ignore->MISRA-C++:2008 Rule 5-0-21 */
//                    stPkgMaster.ucData[usOffset + 5] = \
//                                (u_int8)((stExtDataTemp[i].us_size - (u_int16)ucDivideSendPoint) >> 8);
//                    /* Create the remaining divided send data of sensor counter of extended data master structure */
//                    memcpy(reinterpret_cast<void *>(&stPkgMaster.ucData[usOffset + 8]),
//                           reinterpret_cast<void *>(&stExtDataTemp[i].uc_data[ucDivideSendPoint]),
//                           stExtDataTemp[i].us_size - ucDivideSendPoint);    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    /* Since all buffers have been created, the number of data is set to 0. */
//                    usDataCnt[i] = 0;
//                }
//            } else {
//                if (usDataCnt[i] > VEHICLESENS_PKG_EXT_SEND_MAX_10DATA) {   /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    /* Calculate the data acquisition position */
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    ucDivideSendPoint = static_cast<u_int16>((u_int16)ucDivideSendCnt * usDivideSendSize[i]);
//                    /* Update the data size*/
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    stPkgMaster.ucData[usOffset + 4]        = (u_int8)usDivideSendSize[i];
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    stPkgMaster.ucData[usOffset + 5]        = (u_int8)(usDivideSendSize[i] >> 8);
//                    /* Create 100 divided transmission data of vehicle sensor data of extended data master structure */
//                    memcpy(reinterpret_cast<void *>(&stPkgMaster.ucData[usOffset + 8]),
//                           reinterpret_cast<void *>(&stExtDataTemp[i].uc_data[ucDivideSendPoint]),
//                           usDivideSendSize[i]);    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    /* Subtract the number of created transmission data */
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    usDataCnt[i] = static_cast<u_int16>(usDataCnt[i] - VEHICLESENS_PKG_EXT_SEND_MAX_10DATA);
//                } else {
//                    /* Calculate the data acquisition position */
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    ucDivideSendPoint = static_cast<u_int16>((u_int16)ucDivideSendCnt * usDivideSendSize[i]);
//                    /* Update the data size*/
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    stPkgMaster.ucData[usOffset + 4] = \
//                                (u_int8)(stExtDataTemp[i].us_size - (u_int16)ucDivideSendPoint);
//                    /* Ignore->MISRA-C++:2008 Rule 5-0-5 Ignore->MISRA-C++:2008 Rule 5-0-21 */
//                    stPkgMaster.ucData[usOffset + 5] = \
//                                (u_int8)((stExtDataTemp[i].us_size - (u_int16)ucDivideSendPoint) >> 8);
//                    /* Create the remaining divided transmission data of the vehicle sensor data of the extended data master structure. */
//                    memcpy(reinterpret_cast<void *>(&stPkgMaster.ucData[usOffset + 8]),
//                           reinterpret_cast<void *>(&stExtDataTemp[i].uc_data[ucDivideSendPoint]),
//                           stExtDataTemp[i].us_size - ucDivideSendPoint);    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
//                    /* Since all buffers have been created, the number of data is set to 0. */
//                    usDataCnt[i] = 0;
//                }
//            }
            /* Next offset calculation    */
            /* Boundary adjustment of data size */
            usNextOffset = VehicleSensGetDataMasterExtOffset(ulPkgDid);
            /* Ignore->MISRA-C++:2008 Rule 5-0-5 Ignore->MISRA-C++:2008 Rule 5-0-13, 5-0-14, 5-3-1 */
            if ((usNextOffset & usBoundaryAdj) != 0) {
                /* If you need to adjust */
                /* Mask Lower Bit Ignore->MISRA-C++:2008 Rule 5-0-5 Ignore->MISRA-C++:2008 Rule 5-0-21 */
                usNextOffset = static_cast<u_int16>(usNextOffset & ~usBoundaryAdj);
                usNextOffset = static_cast<u_int16>(usNextOffset + (u_int16)VEHICLESENS_BIT2);  /* Add numbers */
            }
            usOffset = static_cast<u_int16>(usOffset + usNextOffset);    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
        }
        stPkgMaster.ucDNum = pst_data->pkg_num;                /* To save the number of data                        */
        stPkgMaster.ucDataBreak = ucDataBreak;                 /* Set all data undelivered flag            */
        stPkgMaster.ucDivideCnt = ucDivideCnt;                 /* Set total number of partitions                        */
        /* Division number setting Ignore->MISRA-C++:2008 Rule 5-0-5 */
        stPkgMaster.ucDivideSendCnt = static_cast<uint8_t>(ucDivideSendCnt + 1);
        ret = PosSndMsg(PNO_VEHICLE_SENSOR,
                          pst_data->pno,
                          CID_SENSOR_PKG_INFO,
                          (u_int16)sizeof(SENSOR_PKG_MSG_VSINFO),
                          (const void *)&stPkgMaster);
        uc_result = SENSLOG_RES_SUCCESS;
        if (ret != RET_NORMAL) {
            uc_result = SENSLOG_RES_FAIL;
        }
        SensLogWriteOutputData(SENSLOG_DATA_O_SYS_PKG, 0, pst_data->pno,
                                reinterpret_cast<uint8_t *>(&stPkgMaster),
                                sizeof(SENSOR_PKG_MSG_VSINFO), uc_result);

        ucDivideSendCnt++;

        /* Package delivery (split) confirmation debug log output */
        FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, 
            "### SENS RECV # FST PKG DELIVERY : cnt[%d/7]", ucDivideSendCnt);
        if (7 <= ucDivideSendCnt) {
            FRAMEWORKUNIFIEDLOG(ZONE_INIT, __FUNCTION__, 
                "### SENS RECV # FST PKG DELIVERY : last sns_cnt[%d][%d][%d][%d]",
                stPkgMaster.ucData[8], stPkgMaster.ucData[9], stPkgMaster.ucData[10], stPkgMaster.ucData[11]);
        }
    }
    /* Ignore->MISRA-C++:2008 Rule 2-7-2 */

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "-");
}
#endif

/* ++ PastModel002 response DR */

/*******************************************************************************
 * MODULE    : VehicleSensEntryDeliveryCtrlDR
 * ABSTRACT  : Internal delivery destination management registration function for vehicle sensor DR
 * FUNCTION  : Internal distribution destination management table for DR,Update the shipping management table management.
 * ARGUMENT  : pst_delivery_entry    : Pointer to the delivery registration information
 * NOTE      :
 * RETURN    : VEHICLE_RET_NORMAL    :Successful registration
 ******************************************************************************/
VEHICLE_RET_API VehicleSensEntryDeliveryCtrlDR(const DEADRECKONING_MSG_DELIVERY_ENTRY *pst_delivery_entry) {  // LCOV_EXCL_START 8 : dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32 i;
    u_int8 uc_action_type    = VEHICLESENS_ACTION_TYPE_ADD;
    int32 uc_did_flag;
    DID ulentry_did        = pst_delivery_entry->data.did;
    VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA    *pst_existing_mng_data    = NULL;
    VEHICLE_RET_API ret = VEHICLE_RET_NORMAL;  /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */

    /* Check if the data ID exists. */
    uc_did_flag = VehicleSensCheckDid(ulentry_did);
    if (uc_did_flag == 0) {
        ret = VEHICLE_RET_ERROR_DID; /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    }

    /* Check the number of registered shipments. */
    if ((ret == VEHICLE_RET_NORMAL) &&/* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
            (g_stdelivery_ctrl_tbl_dr.us_dnum >= VEHICLESENS_DELIVERY_INFO_MAX)) {
        /* Return the FULL of delivery registrations*/
        ret = VEHICLE_RET_ERROR_BUFFULL;    /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
    }

    if (ret == VEHICLE_RET_NORMAL) {  /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
        /* By searching for the delivery registration of the relevant DID,Hold the address. */
        for (i = 0; i < g_stdelivery_ctrl_tbl_mng_dr.us_dnum; i++) {
            if (g_stdelivery_ctrl_tbl_mng_dr.st_ctrl_tbl_mng_data[i].ul_did == ulentry_did) {
                uc_action_type = VEHICLESENS_ACTION_TYPE_UPDATE;
                pst_existing_mng_data = &g_stdelivery_ctrl_tbl_mng_dr.st_ctrl_tbl_mng_data[i];
            }
        }

        /* Add to the shipping management table.*/
        VehicleSensAddDeliveryCtrlTblDR(pst_delivery_entry);
        /* Processing when updating existing data*/
        if (uc_action_type == VEHICLESENS_ACTION_TYPE_UPDATE) {
            /* Update the shipping management table.*/
            VehicleSensUpdateDeliveryCtrlTblDR(pst_existing_mng_data);

            /* Update the shipping destination management table management information.*/
            VehicleSensUpdateDeliveryCtrlTblMngDR(pst_existing_mng_data);
        } else {  /* Newly added processing*/
            /* Add to the shipping management table management information.*/
            VehicleSensAddDeliveryCtrlTblMngDR(pst_delivery_entry);
        }
    }
    return ret; /* _CWORD71_:QAC++4020:Multiple exit points found. MISRA-C++ Rule 6-6-5 */
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensMakeDeliveryPnoTblDR
* ABSTRACT  : Vehicle sensor internal delivery destination PNO table creation function for DR
* FUNCTION  : Create an internal delivery destination PNO table for DR
* ARGUMENT  : ul_did      Data ID
*             Change_type Delivery Trigger
* NOTE      :
* RETURN    : VEHICLESENS_DELIVERY_PNO_TBL* Pointer to the shipping PNO table
******************************************************************************/
VEHICLESENS_DELIVERY_PNO_TBL* VehicleSensMakeDeliveryPnoTblDR(DID ul_did, u_int8 change_type) {  // LCOV_EXCL_START 8 : dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32 i;
    u_int8 uc_ctrl_tbl_mng_data_list;
    u_int16 us_index    = 0;
    u_int16 us_dnum = 0;

    /* Get the start index and count of the corresponding data ID. */
    uc_ctrl_tbl_mng_data_list = static_cast<u_int8>(
       (sizeof(g_stdelivery_ctrl_tbl_mng_dr.st_ctrl_tbl_mng_data)) /
       (sizeof(g_stdelivery_ctrl_tbl_mng_dr.st_ctrl_tbl_mng_data[0])));

    for (i = 0; i < uc_ctrl_tbl_mng_data_list; i++) {
        /* Stores the information of the corresponding DID.. */
        if (g_stdelivery_ctrl_tbl_mng_dr.st_ctrl_tbl_mng_data[i].ul_did == ul_did) {
            us_index    = g_stdelivery_ctrl_tbl_mng_dr.st_ctrl_tbl_mng_data[i].us_start_idx;
            us_dnum    = g_stdelivery_ctrl_tbl_mng_dr.st_ctrl_tbl_mng_data[i].usdlvry_entry_num;
            break;
        }
    }

    /* Create a PNO list */
    (void)memset(reinterpret_cast<void *>(&g_stdelivery_pno_tbl_dr),
                 static_cast<int32>(0),
                 (size_t)sizeof(g_stdelivery_pno_tbl_dr));
    if (change_type == VEHICLESENS_CHGTYPE_CHG) {
        /* Processing when delivery timing is changed*/
        for (i = 0; i < us_dnum; i++) {
            /* Functionalization by Increasing Structure Members */
            VehicleSensAddPnoTblDR(us_index);
            us_index    = g_stdelivery_ctrl_tbl_dr.st_ctrl_data[us_index].us_link_idx;
        }
    } else {
        /* Processing when delivery timing is update */
        for (i = 0; i < us_dnum; i++) {
            if (VEHICLE_DELIVERY_TIMING_UPDATE  == g_stdelivery_ctrl_tbl_dr.st_ctrl_data[us_index].uc_chg_type) {
                /* Functionalization by Increasing Structure Members */
                VehicleSensAddPnoTblDR(us_index);
            }
            us_index    = g_stdelivery_ctrl_tbl_dr.st_ctrl_data[us_index].us_link_idx;
        }
    }

    return(&g_stdelivery_pno_tbl_dr);
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensAddPnoTblDR
* ABSTRACT  : Vehicle sensor DR internal delivery destination PNO table addition function
* FUNCTION  : Add to the internal DR shipping destination PNO table.
* ARGUMENT  : us_index    : Index of the referenced destination management table
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensAddPnoTblDR(u_int16 us_index) {  // LCOV_EXCL_START 8: dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int16    us_pno_tbl_idx;

    us_pno_tbl_idx = g_stdelivery_pno_tbl_dr.us_dnum;
    g_stdelivery_pno_tbl_dr.st_pno_data[us_pno_tbl_idx].us_pno      = \
                                                    g_stdelivery_ctrl_tbl_dr.st_ctrl_data[us_index].us_pno;
    g_stdelivery_pno_tbl_dr.st_pno_data[us_pno_tbl_idx].us_pkg_start_idx = \
                                                    g_stdelivery_ctrl_tbl_dr.st_ctrl_data[us_index].us_pkg_start_idx;
    g_stdelivery_pno_tbl_dr.st_pno_data[us_pno_tbl_idx].us_pkg_end_idx    = \
                                                    g_stdelivery_ctrl_tbl_dr.st_ctrl_data[us_index].us_pkg_end_idx;
    g_stdelivery_pno_tbl_dr.st_pno_data[us_pno_tbl_idx].uc_method        = \
                                                    g_stdelivery_ctrl_tbl_dr.st_ctrl_data[us_index].uc_method;
    g_stdelivery_pno_tbl_dr.us_dnum++;
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensAddDeliveryCtrlTblDR
* ABSTRACT  : Vehicle sensor DR internal delivery destination management table addition function
* FUNCTION  : Add to the DR internal shipping management table.
* ARGUMENT  : *pst_delivery_entry    : Pointer to the delivery registration information
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensAddDeliveryCtrlTblDR(const DEADRECKONING_MSG_DELIVERY_ENTRY *pst_delivery_entry) {  // LCOV_EXCL_START 8: dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    VEHICLESENS_DELIVERY_CTRL_TBL_DATA *pst_ctrl_data;

    pst_ctrl_data = &g_stdelivery_ctrl_tbl_dr.st_ctrl_data[g_stdelivery_ctrl_tbl_dr.us_dnum];
    pst_ctrl_data->ul_did                = pst_delivery_entry->data.did;
    pst_ctrl_data->us_pno                = pst_delivery_entry->data.pno;
    pst_ctrl_data->uc_chg_type            = pst_delivery_entry->data.delivery_timing;
    pst_ctrl_data->uc_ctrl_flg            = pst_delivery_entry->data.ctrl_flg;
    pst_ctrl_data->us_link_idx            = VEHICLESENS_LINK_INDEX_END;
    pst_ctrl_data->us_pkg_start_idx        = VEHICLESENS_LINK_INDEX_END;
    pst_ctrl_data->us_pkg_end_idx        = VEHICLESENS_LINK_INDEX_END;
    pst_ctrl_data->uc_method            = VEHICLESENS_DELIVERY_METHOD_NORMAL;
    g_stdelivery_ctrl_tbl_dr.us_dnum = static_cast<u_int16>(g_stdelivery_ctrl_tbl_dr.us_dnum + 1);
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensAddDeliveryCtrlTblMngDR
* ABSTRACT  : Internal delivery destination management table management addition function for vehicle sensor DR
* FUNCTION  : Add to the DR internal shipping management table management.
* ARGUMENT  : *pst_delivery_entry    : Pointer to the delivery registration information
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensAddDeliveryCtrlTblMngDR(const DEADRECKONING_MSG_DELIVERY_ENTRY *pst_delivery_entry) {  // LCOV_EXCL_START 8: dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA *pst_ctrl_mng_data;

    pst_ctrl_mng_data = &g_stdelivery_ctrl_tbl_mng_dr.st_ctrl_tbl_mng_data[g_stdelivery_ctrl_tbl_mng_dr.us_dnum];
    pst_ctrl_mng_data->ul_did        = pst_delivery_entry->data.did;
    pst_ctrl_mng_data->us_start_idx = static_cast<u_int16>(g_stdelivery_ctrl_tbl_dr.us_dnum - 1);
    pst_ctrl_mng_data->us_end_idx = static_cast<u_int16>(g_stdelivery_ctrl_tbl_dr.us_dnum - 1);
    pst_ctrl_mng_data->usdlvry_entry_num++;
    g_stdelivery_ctrl_tbl_mng_dr.us_dnum++;
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : VehicleSensUpdateDeliveryCtrlTblMngDR
 * ABSTRACT  : Internal delivery destination management table management update function for vehicle sensor DR
 * FUNCTION  : Update the internal delivery destination management table management for DR.
 * ARGUMENT  : *pst_existing_mng_data    : Pointer to the previous data information with the same data ID
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void VehicleSensUpdateDeliveryCtrlTblMngDR(VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA *pst_existing_mng_data) {  // LCOV_EXCL_START 8: dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /*    Update only the end index and the number of registered shipping destinations. */
    pst_existing_mng_data->us_end_idx = static_cast<u_int16>(g_stdelivery_ctrl_tbl_dr.us_dnum - 1);
    pst_existing_mng_data->usdlvry_entry_num++;
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensUpdateDeliveryCtrlTblDR
* ABSTRACT  : Vehicle sensor DR internal delivery destination management table update function
* FUNCTION  : Update the internal distribution destination management table for DR.
* ARGUMENT  : *pst_existing_mng_data    : Pointer to the previous data information with the same data ID
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensUpdateDeliveryCtrlTblDR(VEHICLESENS_DELIVERY_CTRL_TBL_MNG_DATA *pst_existing_mng_data) {  // LCOV_EXCL_START 8: dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* Update Link Index Only.
       For indexes of usEndIdx values matching the data IDs in the target management table
       Making usLinkIdx an Index-Registered Index */
    g_stdelivery_ctrl_tbl_dr.st_ctrl_data[pst_existing_mng_data->us_end_idx].us_link_idx =
        static_cast<u_int16>(g_stdelivery_ctrl_tbl_dr.us_dnum - 1);
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : VehicleSensDeliveryProcDR
 * ABSTRACT  : Internal Data Delivery Process for Vehicle Sensor DR
 * FUNCTION  : Deliver data to internal DR destinations.
 * ARGUMENT  : ul_did        :Data ID
 *             uc_chg_type    :Delivery timing
 *             uc_get_method  :Acquisition method
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void VehicleSensDeliveryProcDR(DID ul_did, u_int8 uc_chg_type, u_int8 uc_get_method) {
    return;
}

/**
 * @brief
 *   Obtain dump info(g_stdelivery_ctrl_tbl)
 *
 * @param[out]     pbuf    Dump information
 */
void VehicleSensGetDebugDeliveryCtrlTbl(void* pbuf) {  // LCOV_EXCL_START 7: uesd only in FrameworkunifiedOnDebugDump
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    static uint8_t  buf[DEBUG_DUMP_MAX_SIZE];
    static uint8_t  bufTmp[256];
    u_int16 i;

    memset(&buf, 0x00, sizeof(buf));
    snprintf(reinterpret_cast<char *>(&(buf)),
             32,
             "Delivery-Tbl\n DNum:%d", g_stdelivery_ctrl_tbl.us_dnum);
    for (i = 0; i < g_stdelivery_ctrl_tbl.us_dnum; i++) {
        if (i >= 30) {
            break;
        }
        memset(&bufTmp[0], 0x00, sizeof(bufTmp));
        snprintf(reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp),
                "\n [%02d] did:0x%08x, pno:0x%04x, chgT:0x%02x, ctrlFg:0x%02x, "\
                "lnkidx:0x%04x, pkgSidx:0x%04x, pkgEidx:0x%04x, Mth:0x%02x",
                i,
                g_stdelivery_ctrl_tbl.st_ctrl_data[i].ul_did,
                g_stdelivery_ctrl_tbl.st_ctrl_data[i].us_pno,
                g_stdelivery_ctrl_tbl.st_ctrl_data[i].uc_chg_type,
                g_stdelivery_ctrl_tbl.st_ctrl_data[i].uc_ctrl_flg,
                g_stdelivery_ctrl_tbl.st_ctrl_data[i].us_link_idx,
                g_stdelivery_ctrl_tbl.st_ctrl_data[i].us_pkg_start_idx,
                g_stdelivery_ctrl_tbl.st_ctrl_data[i].us_pkg_end_idx,
                g_stdelivery_ctrl_tbl.st_ctrl_data[i].uc_method);
        _pb_strcat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp));
    }
    memcpy(pbuf, &buf[0], sizeof(buf));
    return;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Obtain dump info(g_stdelivery_ctrl_tbl_mng)
 *
 * @param[out]     pbuf    Dump information
 */
void VehicleSensGetDebugDeliveryCtrlTblMng(void* pbuf) {  // LCOV_EXCL_START 7: uesd only in FrameworkunifiedOnDebugDump
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    static uint8_t  buf[DEBUG_DUMP_MAX_SIZE];
    static uint8_t  bufTmp[256];
    u_int16 i;

    memset(&buf, 0x00, sizeof(buf));
    snprintf(reinterpret_cast<char *>(&(buf)),
             32,
             "Delivery-TblMng\n DNum:%d",
             g_stdelivery_ctrl_tbl_mng.us_dnum);
    for (i = 0; i < g_stdelivery_ctrl_tbl_mng.us_dnum; i++) {
        if (i >= 60) {
            break;
        }
        memset(&bufTmp[0], 0x00, sizeof(bufTmp));
        snprintf(reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp),
                "\n [%02d] did:0x%08x, Sidx:0x%04x, Eidx:0x%04x, EntNum:0x%04x",
                i,
                g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data[i].ul_did,
                g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data[i].us_start_idx,
                g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data[i].us_end_idx,
                g_stdelivery_ctrl_tbl_mng.st_ctrl_tbl_mng_data[i].usdlvry_entry_num);
        _pb_strcat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp));
    }
    memcpy(pbuf, &buf[0], sizeof(buf));
    return;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Obtain dump info(g_stpkgdelivery_tbl_mng)
 *
 * @param[out]     pbuf    Dump information
 */
void VehicleSensGetDebugPkgDeliveryTblMng(void* pbuf) {  // LCOV_EXCL_START 7: uesd only in FrameworkunifiedOnDebugDump
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    static uint8_t  buf[DEBUG_DUMP_MAX_SIZE];
    static uint8_t  bufTmp[256];
    u_int16 i;

    memset(&buf, 0x00, sizeof(buf));
    snprintf(reinterpret_cast<char *>(&(buf)),
             32,
             "Delivery-PkgTblMng\n DNum:%d",
             g_stpkgdelivery_tbl_mng.us_dnum);
    for (i = 0; i < g_stpkgdelivery_tbl_mng.us_dnum; i++) {
        if (i >= 100) {
            break;
        }
        memset(&bufTmp[0], 0x00, sizeof(bufTmp));
        snprintf(reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp),
                "\n [%02d] did:0x%08x, Didx:0x%04x",
                i,
                g_stpkgdelivery_tbl_mng.st_pkg_data[i].ul_did,
                g_stpkgdelivery_tbl_mng.st_pkg_data[i].usdlvry_idx);
        _pb_strcat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp));
    }
    memcpy(pbuf, &buf[0], sizeof(buf));
    return;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Obtain dump info(g_stdelivery_pno_tbl)
 *
 * @param[out]     pbuf    Dump information
 */
void VehicleSensGetDebugDeliveryPnoTbl(void* pbuf) {  // LCOV_EXCL_START 7: uesd only in FrameworkunifiedOnDebugDump
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    static uint8_t  buf[DEBUG_DUMP_MAX_SIZE];
    static uint8_t  bufTmp[256];
    u_int16 i;

    memset(&buf, 0x00, sizeof(buf));
    snprintf(reinterpret_cast<char *>(&(buf)),
             32,
             "Delivery-PnoTbl\n DNum:%d",
             g_stdelivery_pno_tbl.us_dnum);
    for (i = 0; i < g_stdelivery_pno_tbl.us_dnum; i++) {
        if (i >= 60) {
            break;
        }
        memset(&bufTmp[0], 0x00, sizeof(bufTmp));
        snprintf(reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp),
                "\n [%02d] pno:0x%04x, pkgSidx:0x%04x, pkgEidx:0x%04x, Mth:0x%02x",
                i,
                g_stdelivery_pno_tbl.st_pno_data[i].us_pno,
                g_stdelivery_pno_tbl.st_pno_data[i].us_pkg_start_idx,
                g_stdelivery_pno_tbl.st_pno_data[i].us_pkg_end_idx,
                g_stdelivery_pno_tbl.st_pno_data[i].uc_method);
        _pb_strcat(reinterpret_cast<char *>(&buf[0]), reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp));
    }
    memcpy(pbuf, &buf[0], sizeof(buf));
    return;
}
// LCOV_EXCL_STOP
/* -- PastModel002 support DR */
