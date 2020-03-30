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
 *    File name        :Sensor_API.cpp
 *    System name        :GPF
 *    Subsystem name    :Sensor I/F library
 *    Program name    :SensorI/F API
 *    Module configuration    :SensorPkgDeliveryEntry()        Vehicle sensor information package delivery registration
 *                  :SensorGetSensorPkgData()        Vehicle sensor information package acquisition
 ******************************************************************************/

#include <stdio.h>
#include "Sensor_API.h"
#include "Sensor_API_private.h"
/* ++ GPS _CWORD82_ support */
#include "Vehicle_API_Dummy.h"
/* -- GPS _CWORD82_ support */

#include "Sensor_Common_API.h"
#include "Vehicle_API_private.h"
#include "Naviinfo_API.h"
#include <vehicle_service/POS_define.h>
#include <vehicle_service/POS_sensor_API.h>
#include "POS_private.h"
#include <vehicle_service/positioning_base_library.h>

/*************************************************/
/*           Global variable                      */
/*************************************************/
/********************************************************************************
 * TAG      :TG_GPS_REQ_RESET
 * ABSTRACT :GPS reset request
 * NOTE     :I/F information between host applications(Reset mode)
 ********************************************************************************/
/**
 * @brief    POS_RegisterListenerPkgSensData Return code list
 */
static const SENSOR_RET_PKG g_ret_list_reg_lis_pkg_sens_data[SENSOR_PUBLIC_DID_NUM] = {
    /* GRADE2   GRADE1     DID (Key)     */
    {TRUE,      FALSE,     POS_DID_SPEED_PULSE        },
    {FALSE,     FALSE,     POS_DID_SPEED_KMPH         },
    {TRUE,      FALSE,     POS_DID_GYRO               },
    {TRUE,      FALSE,     POS_DID_GSNS_X             },
    {TRUE,      FALSE,     POS_DID_GSNS_Y             },
    {FALSE,     FALSE,     POS_DID_GPS_ANTENNA        },
    {TRUE,      FALSE,     POS_DID_SNS_COUNTER        },
    {FALSE,     FALSE,     POS_DID_SPEED_PULSE_FST    },
    {FALSE,     FALSE,     POS_DID_GYRO_FST           },
    {FALSE,     FALSE,     POS_DID_GPS__CWORD82__NMEA       },
    {FALSE,     FALSE,     POS_DID_GPS__CWORD82___CWORD44_GP4   },
    {FALSE,     FALSE,     POS_DID_GPS__CWORD82__FULLBINARY },
    {FALSE,     FALSE,     POS_DID_GPS_NMEA           },
    {TRUE,      FALSE,     POS_DID_REV                },
    {FALSE,     FALSE,     POS_DID_REV_FST            },
    {TRUE,      FALSE,     POS_DID_GYRO_TEMP          },
    {FALSE,     FALSE,     POS_DID_GYRO_TEMP_FST      },
    {FALSE,     FALSE,     POS_DID_GSNS_X_FST         },
    {FALSE,     FALSE,     POS_DID_GSNS_Y_FST         },
    {TRUE,      FALSE,     POS_DID_PULSE_TIME         },
    {FALSE,     FALSE,     POS_DID_GPS_CLOCK_DRIFT    },
    {FALSE,     FALSE,     POS_DID_GPS_CLOCK_FREQ     },
    {FALSE,     FALSE,     VEHICLE_DID_GPS_TIME           }, /* For local use */
    {FALSE,     FALSE,     VEHICLE_DID_GPS_TIME_RAW       }, /* For local use */
    {FALSE,     FALSE,     VEHICLE_DID_GPS_WKNROLLOVER    }  /* For local use */
};

/**
 * @brief    POS_RegisterListenerSensData Return code list
 */
static const SENSOR_RET_PKG g_ret_list_reg_lis_sens_data[SENSOR_PUBLIC_DID_NUM] = {
    /* GRADE2   GRADE1     DID (Key)     */
    {TRUE,      TRUE,      POS_DID_SPEED_PULSE        },
    {FALSE,     FALSE,     POS_DID_SPEED_KMPH         },
    {TRUE,      FALSE,     POS_DID_GYRO               },
    {TRUE,      FALSE,     POS_DID_GSNS_X             },
    {TRUE,      FALSE,     POS_DID_GSNS_Y             },
    {TRUE,      TRUE,      POS_DID_GPS_ANTENNA        },
    {FALSE,     FALSE,     POS_DID_SNS_COUNTER        },
    {TRUE,      FALSE,     POS_DID_SPEED_PULSE_FST    },
    {TRUE,      FALSE,     POS_DID_GYRO_FST           },
    {TRUE,      FALSE,     POS_DID_GPS__CWORD82__NMEA       },
    {FALSE,     FALSE,     POS_DID_GPS__CWORD82___CWORD44_GP4   },
    {TRUE,      FALSE,     POS_DID_GPS__CWORD82__FULLBINARY },
    {FALSE,     TRUE,      POS_DID_GPS_NMEA           },
    {FALSE,     FALSE,     POS_DID_REV                },
    {TRUE,      FALSE,     POS_DID_REV_FST            },
    {TRUE,      FALSE,     POS_DID_GYRO_TEMP          },
    {TRUE,      FALSE,     POS_DID_GYRO_TEMP_FST      },
    {TRUE,      FALSE,     POS_DID_GSNS_X_FST         },
    {TRUE,      FALSE,     POS_DID_GSNS_Y_FST         },
    {FALSE,     FALSE,     POS_DID_PULSE_TIME         },
    {FALSE,     TRUE,      POS_DID_GPS_CLOCK_DRIFT    },
    {FALSE,     TRUE,      POS_DID_GPS_CLOCK_FREQ     },
    {FALSE,     FALSE,     VEHICLE_DID_GPS_TIME           }, /* For local use */
    {FALSE,     FALSE,     VEHICLE_DID_GPS_TIME_RAW       }, /* For local use */
    {FALSE,     FALSE,     VEHICLE_DID_GPS_WKNROLLOVER    }  /* For local use */
};

/**
 * @brief    POS_GetSensData Return code list
 */
static const SENSOR_RET_PKG g_ret_list_get_sens_data[SENSOR_PUBLIC_DID_NUM] = {
    /* GRADE2   GRADE1     DID (Key)     */
    {TRUE,      TRUE,      POS_DID_SPEED_PULSE        },
    {FALSE,     FALSE,     POS_DID_SPEED_KMPH         },
    {TRUE,      FALSE,     POS_DID_GYRO               },
    {TRUE,      FALSE,     POS_DID_GSNS_X             },
    {TRUE,      FALSE,     POS_DID_GSNS_Y             },
    {TRUE,      TRUE,      POS_DID_GPS_ANTENNA        },
    {FALSE,     FALSE,     POS_DID_SNS_COUNTER        },
    {FALSE,     FALSE,     POS_DID_SPEED_PULSE_FST    },
    {FALSE,     FALSE,     POS_DID_GYRO_FST           },
    {TRUE,      FALSE,     POS_DID_GPS__CWORD82__NMEA       },
    {FALSE,     FALSE,     POS_DID_GPS__CWORD82___CWORD44_GP4   },
    {TRUE,      FALSE,     POS_DID_GPS__CWORD82__FULLBINARY },
    {FALSE,     TRUE,      POS_DID_GPS_NMEA           },
    {FALSE,     FALSE,     POS_DID_REV                },
    {FALSE,     FALSE,     POS_DID_REV_FST            },
    {TRUE,      FALSE,     POS_DID_GYRO_TEMP          },
    {FALSE,     FALSE,     POS_DID_GYRO_TEMP_FST      },
    {FALSE,     FALSE,     POS_DID_GSNS_X_FST         },
    {FALSE,     FALSE,     POS_DID_GSNS_Y_FST         },
    {FALSE,     FALSE,     POS_DID_PULSE_TIME         },
    {FALSE,     TRUE,      POS_DID_GPS_CLOCK_DRIFT    },
    {FALSE,     TRUE,      POS_DID_GPS_CLOCK_FREQ     },
    {FALSE,     TRUE,      VEHICLE_DID_GPS_TIME           }, /* For local use */
    {FALSE,     TRUE,      VEHICLE_DID_GPS_TIME_RAW       }, /* For local use */
    {FALSE,     TRUE,      VEHICLE_DID_GPS_WKNROLLOVER    }  /* For local use */
};

/**
 * @brief
 *   DID-decision functions for Sensor API
 *
 *    Determines whether or not the DID specified by the public API corresponds to the DID.
 *
 * @param[in]     did    Data ID
 * @param[in]    mode    Operation mode 1:For Package Delivery Registration API<br>
 *                                 2:For Sensor Data Delivery Registration API<br>
 *                                 3:For Sensor Data Acquisition API
 *
 * @return    TRUE    Be supported
 *             FALSE    Not supported
 */
BOOL SensorJudgeDid(DID did, uint8_t mode) {
    BOOL                    ret = FALSE;
    UNIT_TYPE               type;
    const SENSOR_RET_PKG    *pkg_list = NULL;
    int32_t i;

    /* Set Return pakage list */
    switch (mode) {
        case MODE_REGISTER_LISTENER_PKG_SENS_DATA: /* POS_RegisterListenerPkgSensData */
        {
            pkg_list = g_ret_list_reg_lis_pkg_sens_data;
            break;
        }
        case MODE_REGISTER_LISTENER_SENSDATA: /* POS_RegisterListenerSensData */
        {
            pkg_list = g_ret_list_reg_lis_sens_data;
            break;
        }
        case MODE_GET_SENSDATA: /* POS_GetSensData */
        {
            pkg_list = g_ret_list_get_sens_data;
            break;
        }
        default:
            /* Error log */
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [mode = %d]", mode);
            break;
    }

    if (pkg_list != NULL) {
        /* Search Return code list for DID */
        for (i = 0; i < SENSOR_PUBLIC_DID_NUM; i++) {
            if (did == pkg_list[i].did) {
                break;
            }
        }

        if (i != SENSOR_PUBLIC_DID_NUM) {
            /* Get Unit type */
            type = GetEnvSupportInfo();
            switch (type) {
                case UNIT_TYPE_GRADE1:
                {
                    ret = pkg_list[i].GRADE1_ret;
                    break;
                }
                case UNIT_TYPE_GRADE2:
                {
                    /*
                     *  Note.
                     *  This feature branches processing depending on the unit type.
                     */
                    ret = pkg_list[i].GRADE2_ret;
                    break;
                }
                default:
                    /* Error log */
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                                  "GetEnvSupportInfo ERROR [type = %d]", type);
                    break;
            }
        } else {
            /* Error log */
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [did = %d]", did);
        }
    }

    return ret;
}

/**
 * @brief
 *    Vehicle Sensor Information Extended Package Delivery Registration
 *  Sensor information is delivered for the first time.,Extension Packaging and Registering for Delivery.
 *
 * @param[in]  hApp                    Application handle
 * @param[in]  notifyName            Destination thread name
 * @param[in]  ucPkgNum                Number of package data(1 to 16)
 * @param[in]  *pulDid                Pointer to an array of data IDs for vehicle information
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
 *            SENSOR_RET_ERROR_NOSUPPORT        Unsupported environment<br>
 *            SENSOR_RET_ERROR_INNER            Internal abnormality
 *
 */
SENSOR_RET_API POS_RegisterListenerPkgSensData(HANDLE hApp,
                                               PCSTR notifyName,
                                               uint8_t ucPkgNum,
                                               DID *pulDid, uint8_t ucCtrlFlg, uint8_t ucDeliveryTiming) {
    SENSOR_RET_API                   ret;            /* Return value */
    BOOL                             ret_b;
    RET_API                          ret_api;      /* System API return value */
    EventID                          event_id;     /* Event ID */
    int32                            event_val;    /* Event value */
    SENSOR_MSG_DELIVERY_ENTRY_DAT    data;         /* Message data */
    int32                            i;            /* Generic counters */
    PNO                              ch_pno;       /* Converted internal PNO    */
    UNIT_TYPE                        type;         /* Supported HW Configuration Type    */

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+");

    ret = SENSOR_RET_NORMAL;
    /* Check Handle */
    if (hApp == NULL) {
        /* NULL terminates with an abnormal parameter */
        ret = SENSOR_RET_ERROR_PARAM;
    }

    if (ret == SENSOR_RET_NORMAL) {
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

    if (ret == SENSOR_RET_NORMAL) {
        /* Check Delivery Control Designation */
        if (SENSOR_DELIVERY_REGIST != ucCtrlFlg) {
            /* Parameters other than delivery registration terminated abnormally. */
            ret = SENSOR_RET_ERROR_PARAM;
        } else if ((ucDeliveryTiming != SENSOR_DELIVERY_TIMING_CHANGE) &&
                   (ucDeliveryTiming != SENSOR_DELIVERY_TIMING_UPDATE)) {
            /* Check delivery timing */
            /* Change delivery timing,Terminate as a parameter error except update */
            ret = SENSOR_RET_ERROR_PARAM;
        } else if (notifyName == NULL) {
            /* Check Thread Name */
            /* NULL terminates with an abnormal parameter */
            ret = SENSOR_RET_ERROR_PARAM;
        } else if ((0 == ucPkgNum) || (SENSOR_PKG_DELIVERY_MAX < ucPkgNum)) {
            /* Check Package Data Count */
            /* Out-of-range is terminated as a parameter error. */
            ret = SENSOR_RET_ERROR_PARAM;
        } else if (pulDid == NULL) {
            /* Check Data ID */
            /* NULL terminates with an abnormal parameter */
            ret = SENSOR_RET_ERROR_PARAM;
        } else {
            /* Check if data ID is acceptable */
            for (i = 0; i < ucPkgNum; i++) {
                /* Judge DID */
                ret_b = SENSOR_DID_JUDGE_REGLIS_PKG(pulDid[i]);
                if (ret_b == FALSE) {
                    /* An unacceptable ID terminates with an abnormal parameter. */
                    ret = SENSOR_RET_ERROR_PARAM;
                    break;
                } else {
                    ret = SENSOR_RET_NORMAL;
                }
            }
        }
    }

    if (ret == SENSOR_RET_NORMAL) {
        /* Resource acquisition */
        if (VehicleGetResource() == TRUE) {

        /* Initialization */
        event_id = 0;
        event_val = 0;
        memset(reinterpret_cast<void *>(&data), 0, sizeof(SENSOR_MSG_DELIVERY_ENTRY_DAT));

        /* Get PNO from Thread Name */
        ch_pno = _pb_CnvName2Pno(notifyName);

        /* Event Generation */
        event_id = PosCreateEvent(ch_pno);

        if (0 != event_id) {
            /* Successful event generation */

            /*--------------------------------------------------------------*
             *    Send Vehicle Sensor Information Delivery Registration Message                    *
             *--------------------------------------------------------------*/
            /* Create Message Data */
            data.pno             = ch_pno;
            data.pkg_num         = ucPkgNum;
            data.delivery_timing = ucDeliveryTiming;
            data.ctrl_flg        = ucCtrlFlg;
            data.event_id        = event_id;
            for (i = 0; i < ucPkgNum; i++) {
                if (pulDid[i] == POS_DID_GYRO) {
                    /* Replaced because POS_DID_GYRO is treated internally as VEHICLE_DID_GYRO_EXT. */
                    data.did[i] = VEHICLE_DID_GYRO_EXT;
                } else {
                    data.did[i] = pulDid[i];
                }
            }

            /* Messaging */
            ret_api = PosSndMsg(ch_pno,
                                PNO_VEHICLE_SENSOR,
                                CID_SENSORIF_PKG_DELIVERY_ENTRY_EXT,
                                (u_int16)sizeof(SENSOR_MSG_DELIVERY_ENTRY_DAT),
                                (const void *)&data);

            if (RET_NORMAL == ret_api) {
                /* Message transmission processing is successful */
                /* Wait for completion event from vehicle sensor thread */
                ret_api = _pb_WaitEvent(event_id,
                                        SAPI_EVWAIT_VAL,
                                        SENSOR_RET_ERROR_MIN,
                                        SENSOR_RET_NORMAL,
                                        &event_val,
                                        POS_API_TIME_OUT_MS);
                if (RET_NORMAL != ret_api) {
                    /* Return an internal error */
                    ret = SENSOR_RET_ERROR_INNER;
                } else {
                    /* Return from Event Wait */
                    /* Set event value (processing result) as return value */
                    ret = (SENSOR_RET_API)event_val;
                }
            } else {
                /* Message transmission processing failed */
                /* Return an internal error */
                ret = SENSOR_RET_ERROR_INNER;
            }

            /* Event deletion */
            ret_api = PosDeleteEvent(event_id);
        } else {
            /* Event generation failure */
            ret = SENSOR_RET_ERROR_CREATE_EVENT;
        }
        } else {
            /* When resource shortage occurs, the system terminates with an insufficient resource error. */
            ret = SENSOR_RET_ERROR_RESOURCE;
        }
        /* Resource release */
        VehicleReleaseResource();
    }

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "- [ret = %d]", ret);

    return ret;
}

/*******************************************************************************
 * MODULE    : PosCreateEvent
 * ABSTRACT  : Event creation process
 * FUNCTION  : Generate an event
 * ARGUMENT  : pno        : Thread ID
 * NOTE      :
 * RETURN    : Non-zero        : Event ID
 *           : Zero            : Event generation failure
 ******************************************************************************/
EventID PosCreateEvent(PNO pno) {
    EventID    event_id;          /* Event ID */
    char       event_name[32];    /* Event name character string buffer */
    RET_API    ret_api;           /* System API return value */

    /* Initialization of event name character string buffer */
    (void)memset(reinterpret_cast<void *>(event_name), 0, sizeof(event_name));

    /* Event name creation */
    snprintf(event_name, sizeof(event_name), "SENSOR_%X", pno);    /* Ignore->MISRA-C++:2008 Rule 5-2-12 */

    /* Event Generation */
    event_id = _pb_CreateEvent(FALSE , 0, event_name);    /* Ignore->MISRA-C++:2008 Rule 5-2-12 */

    if (0 != event_id) {
        /* For successful event generation */

        /* Initialize the event */
        ret_api = _pb_SetEvent(event_id, SAPI_EVSET_ABSOLUTE, SENSOR_EVENT_VAL_INIT);
        if (RET_NORMAL != ret_api) {
            /* Event initialization failed */

            /* Delete Event and Return Event Generation Failed */
            ret_api = PosDeleteEvent(event_id);
            event_id = 0;
        }
    }

    return event_id;
}

/*******************************************************************************
 * MODULE    : PosDeleteEvent
 * ABSTRACT  : Event deletion processing
 * FUNCTION  : Delete events
 * ARGUMENT  : event_id    : Event ID of the event to delete
 * NOTE      :
 * RETURN    : RET_NORMAL    : Normal completion
 *           : RET_EV_NONE    : Specified event does not exist
 ******************************************************************************/
RET_API PosDeleteEvent(EventID event_id) {
    return(_pb_DeleteEvent(event_id));
}

/*******************************************************************************
 * MODULE    : SensorLinkShareData
 * ABSTRACT  : Link to shared memory
 * FUNCTION  : Link to shared memory
 * ARGUMENT  : **share_top    : Storage destination of shared memory top address
 *           : *share_size    : Storage destination of shared memory area size
 *           : *offset    : Offset storage destination to free shared memory area
 * NOTE      :
 * RETURN    : RET_NORMAL    : Normal completion
 *           : RET_ERROR    : There is no shared memory area.
 ******************************************************************************/
RET_API SensorLinkShareData(void **share_top, u_int32 *share_size, u_int16 *offset) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API         ret_api;    /* System API return value */
    SemID           sem_id;     /* Semaphore ID */
    SENSOR_SHARE    *share_top_tmp;
    int32           i;

    /* Initialization */
    ret_api = RET_ERROR;

    /* Create Semaphore */
    sem_id = _pb_CreateSemaphore(const_cast<char *>(SENSOR_SEMAPHO_NAME));
    if (0 != sem_id) {
        /* Semaphore Lock */
        ret_api = _pb_SemLock(sem_id);
        if (RET_NORMAL == ret_api) {
            /* Link to shared memory */
            ret_api = _pb_LinkShareData(const_cast<char *>(SENSOR_SHARE_NAME), share_top, share_size);
            if (RET_NORMAL == ret_api) {
                /* By searching the free shared memory area,Offset is calculated if there is free space. */
                share_top_tmp = reinterpret_cast<SENSOR_SHARE *>(*share_top);

                /* Because the first block of the shared memory area is the control area,Loop from i = 1 */
                for (i = 1; i < SENSOR_SHARE_BLOCK_NUM; i++) {
                    if (SENSOR_SHARE_UNLOCK == share_top_tmp->mng.lock_info[i]) {
                        break;
                    }
                }
                if (i < SENSOR_SHARE_BLOCK_NUM) {
                    /* Empty space */
                    /* Lock the block */
                    share_top_tmp->mng.lock_info[i] = SENSOR_SHARE_LOCK;

                    /* Calculate the offset to the block */
                    *offset = static_cast<u_int16>(i * SENSOR_SHARE_BLOCK_SIZE);

                    /* Normal completion */
                    ret_api = RET_NORMAL;
                } else {
                    /* No free space */
                    ret_api = RET_ERROR;
                }
            } else {
                /* Failed link to shared memory */
                ret_api = RET_ERROR;
            }
            /* Semaphore unlock */
            _pb_SemUnlock(sem_id);
        } else {
            /* Semaphore lock failed */
            ret_api = RET_ERROR;
        }
    } else {
        /* Semaphore creation failed */
        ret_api = RET_ERROR;
    }

    return ret_api;
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : SensorUnLinkShareData
 * ABSTRACT  : Unlinking shared memory
 * FUNCTION  : Unlink shared memory
 * ARGUMENT  : *share_top    : Start address of shared memory
 *           : offset        : Offset to shared memory free area
 * NOTE      :
 * RETURN    : RET_NORMAL    : Normal completion
 *           : RET_ERROR    : There is no shared memory area./semaphore error
 ******************************************************************************/
RET_API SensorUnLinkShareData(SENSOR_SHARE *share_top, u_int16 offset) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API    ret_api;    /* System API return value */
    SemID      sem_id;     /* Semaphore ID */
    int32      i;

    /* Initialization */
    ret_api = RET_ERROR;

    /* Create Semaphore */
    sem_id = _pb_CreateSemaphore(const_cast<char *>(SENSOR_SEMAPHO_NAME));
    if (0 != sem_id) {
        /* Semaphore Lock */
        ret_api = _pb_SemLock(sem_id);
        if (RET_NORMAL == ret_api) {
            /* Unlock the block */
            i = static_cast<int32>(offset) / SENSOR_SHARE_BLOCK_SIZE;
            share_top->mng.lock_info[i] = SENSOR_SHARE_UNLOCK;

            /* Semaphore unlock */
            _pb_SemUnlock(sem_id);

            /* Normal completion */
            ret_api = RET_NORMAL;
        } else {
            /* Semaphore lock failed */
            ret_api = RET_ERROR;
        }
    } else {
        /* Semaphore creation failed */
        ret_api = RET_ERROR;
    }

    return ret_api;
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : SensorSetShareData
 * ABSTRACT  : Write processing to shared memory
 * FUNCTION  : Write shared memory
 * ARGUMENT  : *share_top    : Start address of shared memory
 *           : offset        : Offsets to shared memory write destination
 *           : *data_src    :
             : size_src :
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void SensorSetShareData(void *share_top, u_int16 offset, const void *data_src, u_int16 size_src) {  // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SENSOR_SHARE_BLOCK_DAT *share_dat;

    /* Calculate Shared Memory Write Address */
    share_dat = reinterpret_cast<SENSOR_SHARE_BLOCK_DAT *>(reinterpret_cast<u_int8 *>(share_top) + offset);
    /* #QAC confirmation Rule11.4 Use structure for member reference(Cast according to shared memory link IF) */

    /* Clear Shared Memory */
    memset(reinterpret_cast<void *>(share_dat), 0, sizeof(SENSOR_SHARE_BLOCK_DAT));

    /* Set write size to shared memory */
    share_dat->size = size_src;

    /* Set specified data in shared memory */
    memcpy(reinterpret_cast<void *>(&share_dat->data), data_src, (size_t)size_src);
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : PosSndMsg
 * ABSTRACT  : Message transmission processing
 * FUNCTION  : Send a message to the specified PNO
 * ARGUMENT  : pno_src            : Source PNO
 *           : pno_dest        : Destination PNO
 *           : cid            : Command ID
 *           : msg_len            : Message data body length
 *           : *msg_len        : Pointer to message data
 * NOTE      :
 * RETURN    : RET_NORMAL        : Normal completion
 *           : RET_ERRNOTRDY    : Destination process is not wakeup
 *           : RET_ERRMSGFULL    : Message queue overflows
 *           : RET_ERRPARAM        : Buffer size error
 ******************************************************************************/
RET_API PosSndMsg(PNO pno_src, PNO pno_dest, CID cid, u_int16 msg_len, const void *msg_data) {
    SENSOR_INTERNAL_MSG_BUF   msg_buf;        /* message buffer */
    T_APIMSG_MSGBUF_HEADER    *msg_hdr;       /* Pointer to the message header */
    RET_API                   ret_api;        /* Return value */
    PCSTR                     thread_name;    /* Destination thread name                */

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");

    /* _CWORD71_ processing speed(Memset modification) */
    /* Initializing the header of the message buffer */
    memset(reinterpret_cast<void *>(&msg_buf.hdr), 0, sizeof(T_APIMSG_MSGBUF_HEADER));

    /* Get pointer to send buffer */
    msg_hdr = reinterpret_cast<T_APIMSG_MSGBUF_HEADER *>(reinterpret_cast<void *>(&msg_buf));

    /*--------------------------------------------------------------*
     *    Create message headers                                    *
     *--------------------------------------------------------------*/
    msg_hdr->hdr.sndpno         = pno_src;    /* Source PNO */
    msg_hdr->hdr.cid            = cid;        /* Command ID */
    msg_hdr->hdr.msgbodysize    = msg_len;    /* Message data body length */

    /*--------------------------------------------------------------*
     *    Create message data                                    *
     *--------------------------------------------------------------*/
    if ((0 != msg_data) && (0 != msg_len)) {    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
        /* Set the message data */
        memcpy(reinterpret_cast<void *>(msg_buf.data), msg_data, (size_t)msg_len);
    }
    /*--------------------------------------------------------------*
     * Send messages                                            *
     *--------------------------------------------------------------*/
    /* Get Thread Name from PNO */
    if (pno_dest <= SYS_PNO_MAX) {
        thread_name = POS_THREAD_NAME;
    } else {
        thread_name = _pb_CnvPno2Name(pno_dest);
    }

    if ((pno_dest <= SYS_PNO_MAX) && (pno_src <= SYS_PNO_MAX)) {
        /* Internal debug log output */
        FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "[LOG pno_dest = 0x%x]", pno_dest);

        /* Internal Process Transmission and Reception Messages */
        ret_api = _pb_SndMsg(pno_dest,
                             (u_int16)(sizeof(T_APIMSG_MSGBUF_HEADER) + msg_len),/* Ignore->MISRA-C++:2008 Rule 5-0-5 */
                             reinterpret_cast<void *>(&msg_buf), 0);
    } else {
        /* Internal debug log output */
        FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, 
                      "[LOG thread_name = %s, cid = 0x%x]", thread_name, cid);

        /* External Process Transmission and Reception Messages */
        ret_api = _pb_SndMsg_Ext(thread_name,
                                 cid,
                                 (u_int16)(msg_len),    /* Ignore->MISRA-C++:2008 Rule 5-0-5 */
                                 reinterpret_cast<void *>(&(msg_buf.data)), 0);
    }
    /* If RET_ERROR,Register a dialog if called from a Vehicle related thread */ /* Task_30332 */
    if (ret_api == RET_ERROR) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "[ERROR]");
    }

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "- [ret = %d]", ret_api);

    return ret_api;
}

/**
 * @brief
 *  Sensor information acquisition
 *
 *  Obtain sensor information
 *
 * @param[in] hApp          HANDLE   - Application handle
 * @param[in] did         DID      - Data ID for vehicle information
 * @param[in] dest_data     void*    - Pointer representing the storage destination of vehicle sensor information
 * @param[in] dest_size    uint16_t - Storage destination size of vehicle sensor information(byte)
 *
 * @return  0 or more                        Stored data size(Include illegal)<br>
 *          POS_RET_ERROR_CREATE_EVENT   Event generation failure<br>
 *          POS_RET_ERROR_OUTOF_MEMORY   Shared memory allocation failed<br>
 *          POS_RET_ERROR_SIZE           Storage destination size error<br>
 *          POS_RET_ERROR_DID            Unregistered ID<br>
 *          POS_RET_ERROR_NOSUPPORT      Unsupported environment
 *
 */
POS_RET_API POS_GetSensData(HANDLE hApp, DID did, void *dest_data, uint16_t dest_size)
{
    POS_RET_API     ret;                               /* Return value */
    UNIT_TYPE       type = UNIT_TYPE_NONE;            /* Supported HW Configuration Type    */
    BOOL            ret_b;

    /** NULL checking */
    if ((hApp == NULL) || (dest_data == NULL)) {
        /** Parameter error */
        ret = POS_RET_ERROR_PARAM;
    } else {
        /* Positioning Base API initialization */
        _pb_Setup_CWORD64_API(hApp);

        /* Supported HW Configuration Check */
        type = GetEnvSupportInfo();
        if (UNIT_TYPE_GRADE1 == type) {
            /* GRADE1 */
            ret = POS_RET_NORMAL;
        } else if (UNIT_TYPE_GRADE2 == type) {
          /*
           *  Note.
           *  This feature branches processing depending on the unit type.
           */
            ret = POS_RET_ERROR_NOSUPPORT;
        } else {
            /* Environment error */
            ret = POS_RET_ERROR_NOSUPPORT;
        }
    }

//    if (ret == NAVIINFO_RET_NORMAL) {
//        /* Judge DID*/
//        ret_b = SENSOR_DID_JUDGE_GET(did);
//        if (ret_b == FALSE) {
//            /* An unacceptable ID is regarded as a parameter error. */
//            ret = POS_RET_ERROR_PARAM;
//        } else {
//            /* Data acquisition process */
//            ret = PosGetProc(did, dest_data, dest_size);
//        }
//    }

    return ret;
}

