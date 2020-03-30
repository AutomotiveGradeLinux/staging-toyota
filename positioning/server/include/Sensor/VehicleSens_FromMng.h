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

#ifndef POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_FROMMNG_H_
#define POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_FROMMNG_H_
/****************************************************************************
 * File name        :VehicleSens_FromMng.h
 * System name        :_CWORD107_
 * Subsystem name    :Vehicle process
 ****************************************************************************/

#include "Vehicle_API_Dummy.h"
#include "Vehicle_API_private.h"
//#include "positioning_hal.h"
#include "VehicleSens_DataMaster.h"
#include "VehicleSens_Common.h"
#include "VehicleSens_SelectionItemList.h"
#include "INI_API.h"
#include "VehicleIf.h"

/************************************************************************
*            Macro definitions                                                    *
************************************************************************/
#define    VEHICLESENS_FROM_NOT_WRITE                0x00    /* Necessity of writing            */
#define    VEHICLESENS_FROM_WRITE                    0x01    /* No need to write            */

#define VEHICLESENS_FROM_STATUS_NOT_DECISION    0x00    /* FROM Statuses: Undetermined    */
#define VEHICLESENS_FROM_STATUS_DECISION        0x01    /* FROM Statuses: Confirm     */

/* Processing result */
#define SRAM_CHK_OK        (int)0    /* Successful completion of SRAM checking(No value change)  */
#define SRAM_CHK_CHG    (int)1    /* Successful completion of SRAM checking(With value change. Including initialization) */
#define SRAM_CHK_NG        (int)(-1)    /* SRAM checking error                    */

/************************************************************************
*            Struct definitions                                                    *
************************************************************************/
/*********************************************************************
* TAG       : VEHICLESENS_NON_VOLATILE_DATA
* ABSTRACT  : Vehicle type non-volatile data
***********************************************************************/
typedef struct {
    u_int8    uc_hv;                    /* hv            */
    u_int8    uc_hv_status;                /* Hv status    */
    u_int8    uc_reserve[14];            /* Reserved        */
} VEHICLESENS_NON_VOLATILE_DATA;

/*********************************************************************
* TAG       : VEHICLESENS_FROM_INFO
* ABSTRACT  : Structures for acquiring vehicle sensor FROM
***********************************************************************/
typedef struct {
    u_int8    uc_from_value;                        /* FROM values            */
    u_int8    uc_from_status;                        /* FROM Status Values    */
    u_int8    uc_reserve[2];                        /* Reserved            */
} VEHICLESENS_FROM_INFO;

/*********************************************************************
* TAG       :
* ABSTRACT  :
***********************************************************************/

/************************************************************************
*            Function prototype                                              *
************************************************************************/
int32 VehicleSensBackUpDataInit(int32);
int32 VehicleSensBackupInit(void);
int32 VehicleSensBackupChk(void);
void VehicleSensFromSetProc(DID ul_did);
void VehicleSensFromGetProc(DID ul_did, VEHICLESENS_FROM_INFO *p_st_from_data);

#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_VEHICLESENS_FROMMNG_H_
