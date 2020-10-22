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
 *  File name    :VehicleSens_SharedMemory.cpp
 *  System name    :PastModel002
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor shared memory management
 ******************************************************************************/

#include <vehicle_service/positioning_base_library.h>
#include "VehicleSens_SharedMemory.h"
#include "Sensor_API.h"
#include "VehicleSens_DataMaster.h"
#include "Sensor_API_private.h"
#include "SensorLocation_API.h"
#include "SensorLocation_API_private.h"

/********************************************************************************
 *      prototype declalation                          *
 ********************************************************************************/
static void VehicleSensLinkSharedMemory(char *shared_memory_name, void **p_share_addr);
static RET_API VehicleSensWriteDataGpsInterruptSignal(DID ul_did);
static RET_API VehicleSensWriteDataGyroConnectStatus(DID ul_did);
static RET_API VehicleSensWriteDataLocalTime(void);
static RET_API VehicleSensWriteDataLonLat(void);

/********************************************************************************
 *      Definition                              *
 ********************************************************************************/

/*******************************************************************************
* MODULE    : VehicleSensInitSharedMemory
* ABSTRACT  : Shared Memory Initialization
* FUNCTION  : Initialize shared memory
* ARGUMENT  : None
* NOTE      :
* RETURN    : RET_NORMAL  :Normal
*           : RET_ERROR  :Abnormality
******************************************************************************/
RET_API  VehicleSensInitSharedMemory(void) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API  l_ret;  /* Return of the functions */

    /* All shared memory initialization */
    l_ret = VehicleSensWriteDataGpsInterruptSignal(VEHICLE_DID_MAIN_GPS_INTERRUPT_SIGNAL);
    if (l_ret == RET_NORMAL) {
        l_ret = VehicleSensWriteDataGpsInterruptSignal(VEHICLE_DID_SYS_GPS_INTERRUPT_SIGNAL);
    }
    if (l_ret == RET_NORMAL) {
        l_ret = VehicleSensWriteDataGyroConnectStatus(VEHICLE_DID_GYRO_CONNECT_STATUS);
    }

    /* Initializes the effective ephemeris count when the shared memory is shut down. */
    if (l_ret == RET_NORMAL) {
        l_ret = VehicleSensWriteDataValidEphemerisNum(0);  /* Initialized with effective ephemeris number 0 */
    }

    /* Writes the value read from the non-volatile memory to the shared memory. */
    /* This process is executed only at startup.,After that, the shared memory will not be overwritten. */
    if (l_ret == RET_NORMAL) {
        l_ret = VehicleSensWriteDataLocalTime();
    }

    if (l_ret == RET_NORMAL) {
        l_ret = VehicleSensWriteDataLonLat();
    }

    return l_ret;
}

/*******************************************************************************
* MODULE    : VehicleSensLinkSharedMemory
* ABSTRACT  : Shared memory link
* FUNCTION  : Link to shared memory
* ARGUMENT  :
* NOTE      :
* RETURN    : None
******************************************************************************/
static void VehicleSensLinkSharedMemory(char *shared_memory_name, void **p_share_addr) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API  ret_api;
    void  *pv_share_mem;  /* Store Shared Memory Address */
    u_int32  ul_share_mem_size;  /* Size of the linked shared memory */

    /* Link to the handle storage area */
    ret_api = _pb_LinkShareData(shared_memory_name, &pv_share_mem, &ul_share_mem_size);

    if (ret_api == RET_NORMAL) {
        /* If the link is successful */
        *p_share_addr = pv_share_mem;  /* Set the address */
    } else {
        /* If the link fails */
        *p_share_addr = NULL;
    }
}

/*******************************************************************************
* MODULE    : VehicleSensWriteDataGpsInterruptSignal
* ABSTRACT  : Writing of data
* FUNCTION  : Writing Data to Shared Memory
* ARGUMENT  : DID : Data ID
* NOTE      :
* RETURN    : RET_NORMAL  :Normal
*           : RET_ERROR  :Abnormality
******************************************************************************/
static RET_API VehicleSensWriteDataGpsInterruptSignal(DID ul_did) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    static GPS_INTERRUPT  *gpsInterruptSharedAddr = NULL;  /* Store Shared Memory Address */
    static SemID      sem_id = 0;            /* ID of shared memory exclusive semaphore */

    VEHICLESENS_DATA_MASTER_GPS_INTERRUPT_SIGNAL  pst_data;

    RET_API l_ret = RET_NORMAL;  /* Return of the functions */
    RET_API l_ret_api;       /* Return of the functions */


    /* Get Semaphore ID */
    if (sem_id == 0) {
        sem_id = _pb_CreateSemaphore(const_cast<char *>(GPS_INTERRUPT_SIGNAL_SEMAPHO_NAME));
    }

    if (sem_id != 0) {
        /* Semaphore ID successfully acquired */
        l_ret_api = _pb_SemLock(sem_id);  /* Semaphore Lock */

        if (l_ret_api == RET_NORMAL) {
            /* Semaphore lock successful */

            /* When the shared memory is not linked */
            if (gpsInterruptSharedAddr == NULL) {
                /* Link to shared memory */
                /* #QAC confirmation Rule11.4 Use structure for member reference(Cast according to shared memory link IF) */
                VehicleSensLinkSharedMemory(const_cast<char *>(GPS_INTERRUPT_SIGNAL_SHARE_NAME),
                                              reinterpret_cast<void **>(&gpsInterruptSharedAddr));
            }

            if (gpsInterruptSharedAddr != NULL) {
                /* The link to shared memory is successful. */
                switch (ul_did) {
                    case VEHICLE_DID_MAIN_GPS_INTERRUPT_SIGNAL:
                    {
                        /* Get data from data master */
                        VehicleSensGetDataMasterMainGpsInterruptSignal(ul_did, 0, &pst_data);

                        /* Writing Data to Shared Memory */
                        gpsInterruptSharedAddr->_CWORD102__interrupt = pst_data.uc_data;
                        break;
                    }
                    case VEHICLE_DID_SYS_GPS_INTERRUPT_SIGNAL:
                    {
                        /* Get data from data master */
                        VehicleSensGetDataMasterSysGpsInterruptSignal(ul_did, 0, &pst_data);

                        /* Writing Data to Shared Memory */
                        gpsInterruptSharedAddr->_CWORD56__interrupt = pst_data.uc_data;
                        break;
                    }
                    default:
                        l_ret = RET_ERROR;
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "DID is unknown. \r\n");
                        break;
                }

            } else {
                l_ret = RET_ERROR;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "gpsInterruptSharedAddr == NULL \r\n");
            }

            /* Semaphore unlock */
            (void)_pb_SemUnlock(sem_id);
        } else {
            l_ret = RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemLock failed");
        }
    } else {
        l_ret = RET_ERROR;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "sem_id == 0");
    }

    return l_ret;
}

/*******************************************************************************
* MODULE    : VehicleSensWriteDataGyroConnectStatus
* ABSTRACT  : Writing of data
* FUNCTION  : Writing Data to Shared Memory
* ARGUMENT  : DID : Data ID
* NOTE      :
* RETURN    : RET_NORMAL  :Normal
*           : RET_ERROR  :Abnormality
******************************************************************************/
static RET_API VehicleSensWriteDataGyroConnectStatus(DID ul_did) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    static u_int8    *gyroConnectSharedAddr = NULL;  /* Store Shared Memory Address */
    static SemID    sem_id = 0;            /* ID of shared memory exclusive semaphore */

    VEHICLESENS_DATA_MASTER_GYRO_CONNECT_STATUS    pst_data;

    RET_API l_ret = RET_NORMAL;  /* Return of the functions */
    RET_API l_ret_api;       /* Return of the functions */

    /* Get Semaphore ID */
    if (sem_id == 0) {
        sem_id = _pb_CreateSemaphore(const_cast<char *>(GYRO_CONNECT_STATUS_SEMAPHO_NAME));
    }

    if (sem_id != 0) {
        /* Semaphore ID successfully acquired */
        l_ret_api = _pb_SemLock(sem_id);  /* Semaphore Lock */

        if (l_ret_api == RET_NORMAL) {
            /* Semaphore lock successful */

            /* When the shared memory is not linked */
            if (gyroConnectSharedAddr == NULL) {
                /* Link to shared memory */
                /* #QAC confirmation Rule11.4 Use structure for member reference(Cast according to shared memory link IF) */
                VehicleSensLinkSharedMemory(const_cast<char *>(GYRO_CONNECT_STATUS_SHARE_NAME),
                                              reinterpret_cast<void **>(&gyroConnectSharedAddr));
            }

            if (gyroConnectSharedAddr != NULL) {
                /* The link to shared memory is successful. */

                /* Get data from data master */
                VehicleSensGetDataMasterGyroConnectStatus(ul_did, 0, &pst_data);

                /* Writing Data to Shared Memory */
                *gyroConnectSharedAddr = pst_data.uc_data;

            } else {
                l_ret = RET_ERROR;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                    "gyroConnectSharedAddr == NULL \r\n");
            }

            /* Semaphore unlock */
            (void)_pb_SemUnlock(sem_id);
        } else {
            l_ret = RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemLock failed");
        }
    } else {
        l_ret = RET_ERROR;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "sem_id == 0");
    }

    return l_ret;
}

/*******************************************************************************
* MODULE    : VehicleSensWriteDataValidEphemerisNum
* ABSTRACT  : Write effective ephemeris number at shutdown
* FUNCTION  : Write effective ephemeris number at shutdown to shared memory
* ARGUMENT  : u_int8 valid_ephemer_isnum : Number of effective ephemeris written to shared memory during shutdown
* NOTE      :
* RETURN    : RET_NORMAL  :Normal
*           : RET_ERROR  :Abnormality
******************************************************************************/
RET_API VehicleSensWriteDataValidEphemerisNum(u_int8 valid_ephemer_isnum) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    static u_int8  *shared_addr = NULL;  /* Store Shared Memory Address */
    static SemID  sem_id = 0;      /* ID of shared memory exclusive semaphore */

    RET_API l_ret;    /* Return of the functions */
    RET_API l_ret_api;   /* Return of the functions */

    /* Get Semaphore ID */
    if (sem_id == 0) {
        sem_id = _pb_CreateSemaphore(const_cast<char *>(EPHEMERIS_NUM_SEMAPHO_NAME));
    }

    if (sem_id != 0) {
        /* Semaphore ID successfully acquired */
        l_ret_api = _pb_SemLock(sem_id);  /* Semaphore Lock */

        if (l_ret_api == RET_NORMAL) {
            /* Semaphore lock successful */

            /* When the shared memory is not linked */
            if (shared_addr == NULL) {
                /* Link to shared memory */
                /* #QAC confirmation Rule11.4 Use structure for member reference(Cast according to shared memory link IF) */
                VehicleSensLinkSharedMemory(const_cast<char *>(EPHEMERIS_NUM_SHARE_NAME),
                                             reinterpret_cast<void **>(&shared_addr));
            }

            if (shared_addr != NULL) {
                *shared_addr = valid_ephemer_isnum;
                l_ret = RET_NORMAL;

            } else {
                l_ret = RET_ERROR;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "shared_addr == NULL \r\n");
            }

            /* Semaphore unlock */
            l_ret_api = _pb_SemUnlock(sem_id);
            if (l_ret_api != RET_NORMAL) {
                /* Semaphore unlock failure */
                l_ret = RET_ERROR;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemUnlock failed");
            }
        } else {
            /* Semaphore ID acquisition failure */
            l_ret = RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemLock failed");
        }
    } else {
        l_ret = RET_ERROR;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "sem_id == 0");
    }

    return l_ret;
}

/*******************************************************************************
* MODULE    : VehicleSensWriteDataLocalTime
* ABSTRACT  : Writing Local Time at Shutdown
* FUNCTION  : Write local time on shutdown to shared memory
* ARGUMENT  : None
* NOTE      :
* RETURN    : RET_NORMAL  :Acquisition normal
*           : RET_ERROR  :Acquisition anomaly
******************************************************************************/
static RET_API VehicleSensWriteDataLocalTime(void) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    static LOCALTIME  *shared_addr = NULL;  /* Store Shared Memory Address */
    static SemID    sem_id = 0;      /* ID of shared memory exclusive semaphore */

    RET_API l_ret;    /* Return of the functions */
    RET_API l_ret_api;   /* Return of the functions */

    LOCALTIME LocalTime;

    /* Get Semaphore ID */
    if (sem_id == 0) {
        sem_id = _pb_CreateSemaphore(const_cast<char *>(LOCALTIME_SEMAPHO_NAME));
    }

    if (sem_id != 0) {
        /* Semaphore ID successfully acquired */
        l_ret_api = _pb_SemLock(sem_id);  /* Semaphore Lock */

        if (l_ret_api == RET_NORMAL) {
            /* Semaphore lock successful */

            /* When the shared memory is not linked */
            if (shared_addr == NULL) {
                /* Link to shared memory */
                /* #QAC confirmation Rule11.4 Use structure for member reference(Cast according to shared memory link IF) */
                VehicleSensLinkSharedMemory(const_cast<char *>(LOCALTIME_SHARE_NAME),
                                              reinterpret_cast<void **>(&shared_addr));
            }

            if (shared_addr != NULL) {
                /* The link to shared memory is successful. */

                /* Acquires data from the non-volatile memory and writes it to the shared memory. */
                l_ret_api = VehicleSensReadNVLocalTime(&LocalTime);
                if (l_ret_api == RET_NORMAL) {
                    *shared_addr = LocalTime;
                    l_ret = RET_NORMAL;
                } else {
                    /* When data acquisition from non-volatile memory fails,Set an invalid value */
                    (*shared_addr).status  = CLOCK_INVALID;
                    (*shared_addr).year  = 0xFFFFU; /* invalid */
                    (*shared_addr).month  = 255U; /* invalid */
                    (*shared_addr).day    = 255U; /* invalid */
                    (*shared_addr).hour  = 255U; /* invalid */
                    (*shared_addr).min    = 255U; /* invalid */
                    (*shared_addr).sec    = 255U; /* invalid */
                    l_ret = RET_NORMAL;
                }

            } else {
                l_ret = RET_ERROR;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "shared_addr == NULL \r\n");
            }

            /* Semaphore unlock */
            l_ret_api = _pb_SemUnlock(sem_id);
            if (l_ret_api != RET_NORMAL) {
                /* Semaphore unlock failure */
                l_ret = RET_ERROR;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemUnlock failed");
            }
        } else {
            /* Semaphore ID acquisition failure */
            l_ret = RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemLock failed");
        }
    } else {
        l_ret = RET_ERROR;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "sem_id == 0");
    }

    return l_ret;
}

/*******************************************************************************
* MODULE    : VehicleSensWriteDataLonLat
* ABSTRACT  : Write position at shutdown
* FUNCTION  : Write shutdown position to shared memory
* ARGUMENT  : None
* NOTE      :
* RETURN    : RET_NORMAL  :Successful acquisition
*           : RET_ERROR  :Failed to acquire
******************************************************************************/
static RET_API VehicleSensWriteDataLonLat(void) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    static LONLAT  *shared_addr = NULL;  /* Store Shared Memory Address */
    static SemID  sem_id = 0;      /* ID of shared memory exclusive semaphore */

    RET_API l_ret;    /* Return of the functions */
    RET_API l_ret_api;   /* Return of the functions */

    LONLAT lonlat;    /* Position */

    /* Get Semaphore ID */
    if (sem_id == 0) {
        sem_id = _pb_CreateSemaphore(const_cast<char *>(LONLAT_SEMAPHO_NAME));
    }

    if (sem_id != 0) {
        /* Semaphore ID successfully acquired */
        l_ret_api = _pb_SemLock(sem_id);  /* Semaphore Lock */

        if (l_ret_api == RET_NORMAL) {
            /* Semaphore lock successful */

            /* When the shared memory is not linked */
            if (shared_addr == NULL) {
                /* Link to shared memory */
                /* #QAC confirmation Rule11.4 Use structure for member reference(Cast according to shared memory link IF) */
                VehicleSensLinkSharedMemory(const_cast<char *>(LONLAT_SHARE_NAME),
                                              reinterpret_cast<void **>(&shared_addr));
            }

            if (shared_addr != NULL) {
                /* The link to shared memory is successful. */

                /* Acquires data from the non-volatile memory and writes it to the shared memory. */
                l_ret_api = VehicleSensReadNVLonLat(&lonlat);
                if (l_ret_api == RET_NORMAL) {
                    *shared_addr = lonlat;
                    l_ret = RET_NORMAL;
                } else {
                    /* When data acquisition from non-volatile memory fails */
                    (*shared_addr).latitude  = SENSORLOCATION_LATITUDE_INIT_VALUE;
                    (*shared_addr).longitude  = SENSORLOCATION_LONGITUDE_INIT_VALUE;
                    l_ret = RET_NORMAL;
                }

            } else {
                l_ret = RET_ERROR;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "shared_addr == NULL \r\n");
            }

            /* Semaphore unlock */
            l_ret_api = _pb_SemUnlock(sem_id);
            if (l_ret_api != RET_NORMAL) {
                /* Semaphore unlock failure */
                l_ret = RET_ERROR;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemUnlock failed");
            }
        } else {
            /* Semaphore ID acquisition failure */
            l_ret = RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemLock failed");
        }
    } else {
        l_ret = RET_ERROR;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "sem_id == 0");
    }

    return l_ret;
}

/*******************************************************************************
* MODULE    : VehicleSensWriteSharedMemory
* ABSTRACT  : Write Shared Memory
* FUNCTION  : Write Shared Memory
* ARGUMENT  : DID : Data ID
* RETURN    : None
* NOTE      :
******************************************************************************/
void VehicleSensWriteSharedMemory(DID ul_did) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    switch (ul_did) {
        case VEHICLE_DID_MAIN_GPS_INTERRUPT_SIGNAL:
        case VEHICLE_DID_SYS_GPS_INTERRUPT_SIGNAL:
        {
            (void)VehicleSensWriteDataGpsInterruptSignal(ul_did);
            break;
        }
        case VEHICLE_DID_GYRO_CONNECT_STATUS:
        {
            (void)VehicleSensWriteDataGyroConnectStatus(ul_did);
            break;
        }
        default:
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DID is unknown. \r\n");
            break;
    }
}
// LCOV_EXCL_STOP
