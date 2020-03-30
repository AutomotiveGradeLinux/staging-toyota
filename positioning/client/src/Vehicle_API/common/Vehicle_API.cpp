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
 *    File name        :Vehicle_API.cpp
 *    System name        :GPF
 *    Subsystem name    :Vehicle I/F library
 *    Program name    :Vehicle I/F API
 *    Module configuration    :POS_RegisterListenerSensData()    Vehicle sensor information delivery registration
 ******************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include <stdio.h>
#include <vehicle_service/POS_sensor_API.h>
#include "Sensor_API_private.h"
#include "Vehicle_API_Dummy.h"
#include "Vehicle_API_private.h"
#include "POS_private.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/

/*******************************************************************************
 * initialize
******************************************************************************/
VEHICLE_RET_API VehicleInitialize(u_int32 (*sighand)()) {  // LCOV_EXCL_START 8:dead code // NOLINT(readability/nolint)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API _CWORD64_api_ret;
    VEHICLE_RET_API ret;

    _CWORD64_api_ret = _pb_Setup_CWORD64_API(NULL);

    if (_CWORD64_api_ret == RET_NORMAL) {
        ret = RET_NORMAL;
    } else {
        ret = RET_ERROR;
    }
    return ret;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *    Vehicle sensor information delivery registration
 *  Register delivery of vehicle sensor information
 *
 * @param[in]  hApp                    Application handle
 * @param[in]  notifyName            Destination thread name
 * @param[in]  ulDid                Pointer to an array of data IDs for vehicle information
 * @param[in]  ucCtrlFlg            Delivery control<br>
 *                Delivery registration: SENSOR_DELIVERY_REGIST<br>
 *                Delivery stop: SENSOR_DELIVERY_STOP        (Note: Not mounted)<br>
 *                Resume delivery: SENSOR_DELIVERY_RESTART    (Note: Not mounted)
 * @param[in]  ucDeliveryTiming        Delivery timing<br>
 *                Updating    : SENSOR_DELIVERY_TIMING_UPDATE<br>
 *                Changing    : SENSOR_DELIVERY_TIMING_CHANGE
 *
 * @return    SENSOR_RET_NORMAL                Successful registration<br>
 *            SENSOR_RET_ERROR_CREATE_EVENT    Event generation failure<br>
 *            SENSOR_RET_ERROR_PARAM            Parameter error<br>
 *            SENSOR_RET_ERROR_DID            Unregistered ID<br>
 *            SENSOR_RET_ERROR_BUFFULL        FULL of delivery registers<br>
 *            SENSOR_RET_ERROR_NOSUPPORT        Unsupported environment
 *
 */
SENSOR_RET_API POS_RegisterListenerSensData(HANDLE hApp,
                                            PCSTR notifyName, DID ulDid, u_int8 ucCtrlFlg, u_int8 ucDeliveryTiming) {
    SENSOR_RET_API    ret;     /* Return value */
    UNIT_TYPE         type;    /* Supported HW Configuration Type    */
    BOOL              ret_b;

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    /* Check Delivery Control Designation */
    ret = SENSOR_RET_NORMAL;
    /* Arguments & Support Configuration Check */
    if ((ucDeliveryTiming != SENSOR_DELIVERY_TIMING_CHANGE) &&
        (ucDeliveryTiming != SENSOR_DELIVERY_TIMING_UPDATE)) {
        /* Change delivery timing,Terminate as a parameter error except update */
        ret = SENSOR_RET_ERROR_PARAM;
    } else if (SENSOR_DELIVERY_REGIST != ucCtrlFlg) {
        /* Parameters other than delivery registration terminated abnormally. */
        ret = SENSOR_RET_ERROR_PARAM;
    } else if (hApp == NULL) {
        /* Check Handle */
        /* NULL terminates with an abnormal parameter */
        ret = SENSOR_RET_ERROR_PARAM;
    } else if (notifyName == NULL) {
        /* Check Thread Name */
        /* NULL terminates with an abnormal parameter */
        ret = SENSOR_RET_ERROR_PARAM;
    } else {
        /* Positioning Base API initialization */
        _pb_Setup_CWORD64_API(hApp);

        /* Supported HW Configuration Check */
        type = GetEnvSupportInfo();
        if (UNIT_TYPE_GRADE1 == type) {
            /* GRADE1 */
            ret = SENSOR_RET_NORMAL;
        } else if (UNIT_TYPE_GRADE2 == type) {
          /*
           *  Note.
           *  This feature branches processing depending on the unit type.
           */
            ret = SENSOR_RET_ERROR_NOSUPPORT;
        } else {
            /* Environment error */
            ret = SENSOR_RET_ERROR_NOSUPPORT;
        }
    }

    if (SENSOR_RET_NORMAL == ret) {
        /* Judge DID */
        ret_b = SENSOR_DID_JUDGE_REGLIS(ulDid);
        if (ret_b == FALSE) {
            /* An unacceptable ID is regarded as a parameter error. */
            ret = SENSOR_RET_ERROR_PARAM;
        } else {
            /* Delivery registration process */
            ret = PosRegisterListenerProc(notifyName, ulDid, ucCtrlFlg, ucDeliveryTiming);
        }
    }

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "- [ret = %d]", ret);

    return ret;
}

/*******************************************************************************
 * MODULE    : PosSetShareData
 * ABSTRACT  : Write processing to shared memory
 * FUNCTION  : Write shared memory
 * ARGUMENT  : *share_top    : Start address of shared memory
 *           : offset        : Offsets to shared memory write destination
 *           : *data_src    : Data
 *           : size_src    : Size
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void PosSetShareData(void *share_top, u_int16 offset, const void *data_src, u_int16 size_src) {
    VEHICLE_SHARE_BLOCK_DAT *share_dat;
    /* Calculate Shared Memory Write Address */
    share_dat = reinterpret_cast<VEHICLE_SHARE_BLOCK_DAT *>(reinterpret_cast<u_int8 *>(share_top) + offset);

    /* _CWORD71_ processing speed(Memset modification) */
    /* Clear Shared Memory(Unused area) */
    share_dat->reserve[0] = 0;
    share_dat->reserve[1] = 0;

    /* Set write size to shared memory */
    share_dat->size = size_src;

    /* Set specified data in shared memory */
    memcpy(reinterpret_cast<void *>(share_dat->data), data_src, (size_t)size_src);
}

/*******************************************************************************
* MODULE    : VehicleGetDrData
* ABSTRACT  : DR information acquisition
* FUNCTION  : Retrieves DR information (optional data) by returning to completion.
* ARGUMENT  : pno        : Thread ID
*           : did        : Data ID for DR information
*           : *dest_data    : Pointer to the storage destination of DR information
*           : dest_size    : Storage destination size of DR information(byte)
* NOTE      :
* RETURN    : Zero or more                            : Stored data size
*           : VEHICLE_RET_ERROR_CREATE_EVENT    : Event generation failure
*           : VEHICLE_RET_ERROR_OUTOF_MEMORY    : Shared memory allocation failed
*           : VEHICLE_RET_ERROR_SIZE            : Storage destination size error
*           : VEHICLE_RET_ERROR_DID            : Unregistered ID
******************************************************************************/
int32 VehicleGetDrData(PNO pno, DID did, void *dest_data, u_int16 dest_size) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    VEHICLE_RET_API    ret;                            /* Return value */
    RET_API            ret_api;                        /* System API return value */
    EventID            event_id;                       /* Event ID */
    int32              event_val;                      /* Event value */
    void               *share_top;                     /* Start address of shared memory */
    u_int32            share_size;                     /* Size of shared memory area */
    u_int16            offset;                         /* Offset to free shared memory area */
    VEHICLE_SHARE_BLOCK_DAT             *share_dat;    /* Address of free shared memory area */
    VEHICLE_MSG_GET_VEHICLE_DATA_DAT    data;          /* Message data */

    /* Initialization */
    event_id = 0;
    event_val = 0;
    memset(reinterpret_cast<void *>(&data), 0, sizeof(VEHICLE_MSG_GET_VEHICLE_DATA_DAT));

    /* Event Generation */
    event_id = VehicleCreateEvent(pno);

    if (0 != event_id) {
        /* Successful event generation */
        /* Allocate shared memory */
        ret_api = VehicleLinkShareData(reinterpret_cast<void **>(&share_top), &share_size, &offset);
        if (RET_NORMAL != ret_api) {
            /* Failed to allocate shared memory */
            ret = VEHICLE_RET_ERROR_OUTOF_MEMORY;
        } else {
            /* When the shared memory is allocated successfully */

            /* Calculate start address of free shared memory area */
            share_dat = reinterpret_cast<VEHICLE_SHARE_BLOCK_DAT *>(reinterpret_cast<u_int8 *>(share_top) + offset);

            /* Send vehicle sensor information acquisition message */
            data.did         = did;
            data.pno         = pno;
            data.offset      = offset;
            data.size        = VEHICLE_SHARE_BLOCK_DSIZE;
            data.event_id    = event_id;
            /* Messaging */

            ret_api = VehicleSndMsg(pno,
                                    PNO_VEHICLE_SENSOR,
                                    CID_VEHICLEIF_GET_DR_DATA,
                                    sizeof(VEHICLE_MSG_GET_VEHICLE_DATA_DAT),
                                    (const void *)&data);

            if (RET_NORMAL == ret_api) {
                /* Message transmission processing is successful */
                /* Wait for completion event from vehicle sensor thread */
                ret_api = _pb_WaitEvent(event_id,
                                        SAPI_EVWAIT_VAL,
                                        VEHICLE_RET_ERROR_MIN, VEHICLE_RET_NORMAL, &event_val, INFINITE);
                if (RET_NORMAL != ret_api) {
                    /* When not put in event wait state */
                    /* Return an event generation failure */
                    ret = VEHICLE_RET_ERROR_CREATE_EVENT;
                } else {
                    /* Return from Event Wait */

                    /* Link to shared memory */
                    ret_api = _pb_LinkShareData(const_cast<char *>(VEHICLE_SHARE_NAME), &share_top, &share_size);

                    /* Calculate the address of the shared memory storage area. */
                    share_dat = reinterpret_cast<VEHICLE_SHARE_BLOCK_DAT *>(reinterpret_cast<u_int8 *>(share_top)
                                + offset);

                    if (event_val < 0) {
                        /* Vehicle sensor information acquisition failure */
                        ret = (VEHICLE_RET_API)event_val;
                    } else if (RET_NORMAL != ret_api) {
                        /* Shared memory error */
                        ret = VEHICLE_RET_ERROR_OUTOF_MEMORY;
                    } else if (dest_size < share_dat->size) {
                        /* Storage destination size error */
                        ret = VEHICLE_RET_ERROR_SIZE;
                    } else {
                        /* Vehicle sensor information acquisition success */

                        /* Copy from shared memory to user memory */
                        memcpy(dest_data, share_dat->data, (size_t)share_dat->size);

                        /* Set Write Size to Return Value */
                        ret = static_cast<int32>(share_dat->size);
                    }
                }
            } else {
                /* Message transmission processing failed */
                /* Return an event generation failure */
                ret = VEHICLE_RET_ERROR_CREATE_EVENT;
            }
            /* Free shared memory */
            (void)VehicleUnLinkShareData(reinterpret_cast<VEHICLE_SHARE*>(share_top), offset);
        }

        /* Event deletion */
        ret_api = VehicleDeleteEvent(event_id);

    } else {
        /* Event generation failure */
        ret = VEHICLE_RET_ERROR_CREATE_EVENT;
    }
    return ret;
}
// LCOV_EXCL_STOP
