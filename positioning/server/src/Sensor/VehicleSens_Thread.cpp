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
 *  File name    :VehicleSens_Thread.cpp
 *  System name    :GPF
 *  Subsystem name  :Vehicle sensor process
 *  Program name  :Vehicle sensor thread
 *  Module configuration  :VehicleSensThread()          Vehicle Sensor Thread Domain Functions
 *          :VehicleSensThreadInit()        Vehicle sensor thread initials
 *          :VehicleSensDeliveryEntry()      Vehicle sensor information delivery registration
 *          :VehicleSensGetVehicleData()      Vehicle sensor information acquisition
 *          :VehicleSensWatchStopPage()      Vehicle sensor interruption monitoring
 *          :VehicleSensPkgDeliveryEntry()    Vehicle sensor information package delivery registration
 *          :VehicleSensGetVehiclePkgData()    Vehicle sensor information package data acquisition
 *          :VehicleSensLineSensDataDelivery()  LineSensor Vehicle Signal Notification
 *          :VehicleSensGpsDataDelivery()      GPS data notification
 *          :VehicleSens_CanDataDelivery()      CAN data delivery notice
 *          :VehicleSensDataMasterSetN()      Data master set notification process(Callback function)
 *          :VehicleSensSetVehicleData()      GPS command request processing
 ******************************************************************************/
#include "VehicleSens_Thread.h"
#include <vehicle_service/positioning_base_library.h>
#include <other_service/VP_GetEnv.h>
#include "POS_private.h"
#include "positioning_common.h"
#include "SensorLog.h"
#include "VehicleUtility.h"
#include "VehicleSensor_Thread.h"

#include "VehicleIf.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static VEHICLE_MSG_BUF      g_wait_for_resp_msg;  /* Message waiting to receive a response */
static PFUNC_DMASTER_SET_N  g_wait_for_resp_set_n;  /* Response waiting data master set */
static BOOL                 g_sent_fst_pkg_delivery_ext; /* Initial expansion package data delivery status */

static VEHICLESENS_VEHICLE_SPEED_INFO g_vehicle_speed_info;

uint32_t gPseudoSecClockCounter = 0u;

/*************************************************/
/*           Function prototype                    */
/*************************************************/
static void VehicleSensInitDataDisrptMonitor(void);
static void VehicleSensDataDisrptMonitorProc(DID did);
static void VehicleSensRcvMsgTout(TimerToutMsg* rcv_msg);

static void VehilceSens_InitVehicleSpeed(void);
static void VehicleSens_StoreVehicleSpeed(VEHICLESENS_VEHICLE_SPEED_DAT* pVehicleSpeed);
static void VehicleSens_LoadVehicleSpeed(VEHICLESENS_VEHICLE_SPEED_INFO* pVehicleSpeedInfo);
static RET_API VehicleSens_CatNmeaSentenceFieldWithDelimiter(char* str1, const size_t size, const char* str2, const size_t n);
static RET_API VehicleSens_CatNmeaSentenceFieldWithoutDelimiter(char* str1, const size_t size, const char* str2, const size_t n);
static inline RET_API VehicleSens_GeneratePASCDFieldId(char* pascd, size_t size);
static inline RET_API VehicleSens_GeneratePASCDFieldTimestamp(char* pascd, size_t size);
static inline RET_API VehicleSens_GeneratePASCDFieldSensorType(char* pascd, size_t size);
static inline RET_API VehicleSens_GeneratePASCDFieldTransmissionState(char* pascd, size_t size);
static inline RET_API VehicleSens_GeneratePASCDFieldSlipDetect(char* pascd, size_t size);
static inline RET_API VehicleSens_GeneratePASCDFieldSampleCount(char* pascd, size_t size);
static inline RET_API VehicleSens_GeneratePASCDFieldTimeOffsetNSpeed(char* pascd, size_t size);
static inline RET_API VehicleSens_GeneratePASCDFieldChecksum(char* pascd, size_t size);
static inline RET_API VehicleSens_GeneratePASCDFieldCRLF(char* pascd, size_t size);
static RET_API VehicleSens_DeriveTransmissionStateFor_CWORD27_(VEHICLESENS_TRANSMISSION_PKG* pPkg);


/*******************************************************************************
* MODULE    : VehicleSensThread
* ABSTRACT  : Vehicle Sensor Thread Domain Functions
* FUNCTION  : Main processing
* ARGUMENT  : lpv_para  :
* NOTE      :
* RETURN    :
******************************************************************************/
EFrameworkunifiedStatus VehicleSensThread(HANDLE h_app) {
    RET_API  ret_api = RET_NORMAL;        /* Return Values of System API Functions  */
    T_APIMSG_MSGBUF_HEADER  *p;          /* Message header      */
    RET_API  ret_val;                /* Return value of initialization processing    */
    VEHICLE_MSG_DELIVERY_ENTRY  delivery_entry;

    static u_int8 msg_buf[MAX_MSG_BUF_SIZE]; /* message buffer  */

    void* p_msg_buf = &msg_buf;
    LSDRV_MSG_LSDATA_G** p_lsdrv_msg;
    VEHICLE_MSG_BUF** p_vehicle_msg;
    POS_MSGINFO *p_pos_msg;



    p_lsdrv_msg = reinterpret_cast<LSDRV_MSG_LSDATA_G**>(&p_msg_buf);
    p_vehicle_msg = reinterpret_cast<VEHICLE_MSG_BUF**>(&p_msg_buf);

    VehicleUtilityInitTimer();
    (void)PosSetupThread(h_app, ETID_POS_MAIN);

    memset(&(delivery_entry), 0, sizeof(VEHICLE_MSG_DELIVERY_ENTRY));

    /* Thread initialization process */
    ret_val = VehicleSensThreadInit();

    gPseudoSecClockCounter = 0u;

    if (RET_NORMAL == ret_val) {  // LCOV_EXCL_BR_LINE 6: always be RET_NORMAL
        while (1) {
            /* Message reception processing */
            p_msg_buf = &msg_buf;
            ret_api = _pb_RcvMsg(PNO_VEHICLE_SENSOR, sizeof(msg_buf), &p_msg_buf, RM_WAIT);

            /* Internal debug log output */
            FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, 
                          "[ret_api = 0x%x]",
                          ret_api);

            /* When the message is received successfully */
            if (ret_api == RET_RCVMSG) {
                p = reinterpret_cast<T_APIMSG_MSGBUF_HEADER *>(p_msg_buf);

                switch (p->hdr.cid) {  // LCOV_EXCL_BR_LINE 200: some DID is not used
                    case CID_VEHICLEIF_DELIVERY_ENTRY:
                    {
                        memcpy(&(delivery_entry), &(p_msg_buf), sizeof(VEHICLE_MSG_DELIVERY_ENTRY));

                        /* Sort by received DID */
                        switch (delivery_entry.data.did) {  // LCOV_EXCL_BR_LINE 200: DR DID is not used
                            case VEHICLE_DID_DR_ALTITUDE :
                            case VEHICLE_DID_DR_LATITUDE :
                            case VEHICLE_DID_DR_SPEED :
                            case VEHICLE_DID_DR_HEADING :
                            case VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL :
                            case VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR_LEVEL :
                            {
                                // LCOV_EXCL_START 8: dead code
                                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                                VehicleSensDrDeliveryEntry((const VEHICLE_MSG_DELIVERY_ENTRY *)p_msg_buf);
                                // LCOV_EXCL_STOP
                            }
                            break;
                            default:
                                /* Vehicle sensor information delivery registration */
                                VehicleSensDeliveryEntry((const VEHICLE_MSG_DELIVERY_ENTRY *)p_msg_buf);
                                break;
                        }
                        break;
                    }
                    case CID_VEHICLEIF_GET_VEHICLE_DATA:
                    {
                        /* Vehicle sensor information acquisition */
                        VehicleSensGetVehicleData((const VEHICLE_MSG_GET_VEHICLE_DATA *)p_msg_buf);
                        break;
                    }
                    case CID_LINESENS_VEHICLE_DATA:
                    {
                        /* LineSensor Vehicle Signal Notification */
                        VehicleSensLineSensDataDelivery((const LSDRV_MSG_LSDATA *)p_msg_buf,
                                                          (PFUNC_DMASTER_SET_N)VehicleSensDataMasterSetN);
                        break;
                    }
                    case CID_LINESENS_VEHICLE_DATA_G:
                    {
                        /* Data disruption monitoring process */
                        VehicleSensDataDisrptMonitorProc(
                            (reinterpret_cast<LSDRV_MSG_LSDATA_G*>(*p_lsdrv_msg))->st_para.st_data[0].ul_did);
                        VehicleSensLineSensDataDeliveryG((const LSDRV_MSG_LSDATA_G *)p_msg_buf,
                                                            (PFUNC_DMASTER_SET_N)VehicleSensDataMasterSetN);
                        break;
                    }
                    case CID_LINESENS_VEHICLE_DATA_GYRO_TROUBLE:
                    {
                        // LCOV_EXCL_START 8: dead code
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        /* Gyro Failure Status Notification */
                        VehicleSensLineSensDataDeliveryGyroTrouble((const LSDRV_MSG_LSDATA_GYRO_TROUBLE *)p_msg_buf,
                                (PFUNC_DMASTER_SET_N)VehicleSensDataMasterSetN);
                        break;
                        // LCOV_EXCL_STOP
                    }
                    case CID_LINESENS_VEHICLE_DATA_SYS_GPS_INTERRUPT_SIGNAL:
                    {
                        // LCOV_EXCL_START 8: dead code
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        /* SYS GPS interrupt notification */
                        VehicleSensLineSensDataDeliverySysGpsInterruptSignal(
                            (const LSDRV_MSG_LSDATA_GPS_INTERRUPT_SIGNAL *)p_msg_buf,
                            (PFUNC_DMASTER_SET_SHARED_MEMORY)VehicleSensDataMasterSetSharedMemory);
                        break;
                        // LCOV_EXCL_STOP
                    }
                    case CID_LINESENS_VEHICLE_DATA_GYRO_CONNECT_STATUS:
                    {
                        // LCOV_EXCL_START 8: dead code
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        /* Gyro Failure Status Notification */
                        VehicleSensLineSensDataDeliveryGyroConnectStatus(
                            (const LSDRV_MSG_LSDATA_GYRO_CONNECT_STATUS *)p_msg_buf,
                            (PFUNC_DMASTER_SET_SHARED_MEMORY)VehicleSensDataMasterSetSharedMemory);
                        break;
                        // LCOV_EXCL_STOP
                    }
                    case CID_LINESENS_VEHICLE_DATA_GPS_ANTENNA_STATUS:
                    {
                        // LCOV_EXCL_START 8: dead code
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        /* GPS antenna failure status notification */
                        VehicleSensLineSensDataDeliveryGpsAntennaStatus(
                            (const LSDRV_MSG_LSDATA_GPS_ANTENNA_STATUS *)p_msg_buf,
                            (PFUNC_DMASTER_SET_N)VehicleSensDataMasterSetN);
                        break;
                        // LCOV_EXCL_STOP
                    }
                    case CID_SENSORIF_PKG_DELIVERY_ENTRY_EXT:
                    {
                        /* Vehicle Sensor Information Extended Package Delivery Registration */
                        VehicleSensPkgDeliveryEntryExt((const SENSOR_MSG_DELIVERY_ENTRY *)p_msg_buf);
                        break;
                    }
                    case CID_LINESENS_VEHICLE_DATA_FST:
                    {
                        /* LineSensor Vehicle Initial Sensor Signal Notification */
                        VehicleSensLineSensDataDeliveryFstG((const LSDRV_MSG_LSDATA_FST *)p_msg_buf,
                                                            (PFUNC_DMASTER_SET_N)VehicleSensDataMasterSetN);
                        break;
                    }
                    case CID_GPS_DATA:
                    {
                        /* GPS information notification */
                        VehicleSensGpsDataDelivery(reinterpret_cast<SENSOR_MSG_GPSDATA *>(p_msg_buf),
                            (PFUNC_DMASTER_SET_N)VehicleSensDataMasterSetN,
                            (PFUNC_DMASTER_SET_SHARED_MEMORY)VehicleSensDataMasterSetSharedMemory);
                        break;
                    }
                    case CID_POSIF_SET_DATA:
                    {
                        p_pos_msg =
                        reinterpret_cast<POS_MSGINFO*>((reinterpret_cast<VEHICLE_MSG_BUF*>(*p_vehicle_msg))->data);
                        /* Data disruption monitoring process */
                        VehicleSensDataDisrptMonitorProc(p_pos_msg->did);

                        /* Data Setting Notification */
                        VehicleSensCommonDataDelivery((const VEHICLE_MSG_BUF *)p_msg_buf,
                                                      (PFUNC_DMASTER_SET_N)VehicleSensDataMasterSetN);
                        break;
                    }
                    case CID_GPS_RETTIMESETTING:
                    {
                        /* GPS time setting result notification */
                        VehicleSensGpsTimeDelivery((const VEHICLE_MSG_BUF *)p_msg_buf);
                        break;
                    }
                    case CID_DEAD_RECKONING_GPS_DATA :  /* GPS data distribution for DR */
                    case CID_DEAD_RECKONING_SENS_DATA :  /* Sensor Data Delivery for DR */
                    case CID_DEAD_RECKONING_SENS_FST_DATA :  /* Initial Sensor Data Delivery for DR */
                    {
                        // LCOV_EXCL_START 8: dead code
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        VehicleSensDrRcvMsg((const DEAD_RECKONING_RCVDATA *)p_msg_buf);
                        break;
                        // LCOV_EXCL_STOP
                    }
                    case CID_VEHICLEIF_GET_DR_DATA :
                    {
                        // LCOV_EXCL_START 8: dead code
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        /* Vehicle sensor information acquisition */
                        DeadReckoningGetDRData((const DEADRECKONING_MSG_GET_DR_DATA *)p_msg_buf);
                        break;
                        // LCOV_EXCL_STOP
                    }
                    case CID_DR_MAP_MATCHING_DATA :  /* Map matching information */
                    {
                        // LCOV_EXCL_START 8: dead code
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        DeadReckoningSetMapMatchingData((const DR_MSG_MAP_MATCHING_DATA *)p_msg_buf);
                        break;
                        // LCOV_EXCL_STOP
                    }
                    case CID_DR_CLEAR_BACKUP_DATA :  /* Clear backup data */
                    {
                        // LCOV_EXCL_START 8: dead code
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        DeadReckoningClearBackupData((const DR_MSG_CLEAR_BACKUP_DATA*)p_msg_buf);
                        break;
                        // LCOV_EXCL_STOP
                    }
                    case CID_VEHICLEDEBUG_LOG_GET : /* Log acquisition request */
                    {
                        // LCOV_EXCL_START 8: dead code
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        VehicleSensGetLog((const VEHICLEDEBUG_MSG_BUF *)p_msg_buf);
                        break;
                        // LCOV_EXCL_STOP
                    }
                    case CID_VEHICLEDEBUG_LOG_SET :  /* Log Setting Request */
                    {
                        // LCOV_EXCL_START 8: dead code
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        VehicleSensSetLog((const VEHICLEDEBUG_MSG_BUF *)p_msg_buf);
                        break;
                        // LCOV_EXCL_STOP
                    }
                    case CANINPUT_CID_LOCALTIME_NOTIFICATION : /* CAN information acquisition */
                    {
                        // LCOV_EXCL_START 8: dead code
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        VehicleSensWriteLocalTime((const CANINPUT_MSG_INFO*)p_msg_buf);
                        break;
                        // LCOV_EXCL_STOP
                    }
                    case CID_EPH_NUM_NOTIFICATION :  /* Set effective ephemeris count at shutdown */
                    {
                        // LCOV_EXCL_START 8: dead code
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        VehicleSensSetEphNumSharedMemory((const SENSOR_MSG_GPSDATA *)p_msg_buf);
                        break;
                        // LCOV_EXCL_STOP
                    }
                    case CID_SENSORIF__CWORD82__REQUEST:
                    {
                        // LCOV_EXCL_START 8: dead code
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        /* Requests to send GPS _CWORD82_ commands */
                        VehicleSensSetVehicleData((const VEHICLE_MSG_SEND *)p_msg_buf);
                        break;
                        // LCOV_EXCL_STOP
                    }
                    case CID_THREAD_STOP_REQ:
                    {
                        /* Thread stop processing */
                        VehicleSensThreadStopProcess();
                        break;
                    }
                    case CID_TIMER_TOUT:
                    {
                        /* Timeout notification reception processing */
                        VehicleSensRcvMsgTout(reinterpret_cast<TimerToutMsg*>(p_msg_buf));
                        break;
                    }
                    default:
                        break;
                }
            } else {
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "ret_api != RET_RCVMSG\r\n");
            }
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "VehicleSens: VehicleSens_MainThread Initial Error!! :%d", ret_val);
        _pb_Exit();
    }

    return eFrameworkunifiedStatusOK;
}

/*******************************************************************************
* MODULE    : VehicleSensThreadInit
* ABSTRACT  : Vehicle sensor thread initials
* FUNCTION  : Initial process
* ARGUMENT  : void
* NOTE      :
* RETURN    : CAN data delivery registration result
******************************************************************************/
RET_API VehicleSensThreadInit(void) {
    RET_API    ret_val = RET_NORMAL;

    /* Initialization of Vehicle Selection Item List Table */
    VehicleSensInitSelectionItemList();

    /* Vehicle sensor data master initialization */
    VehicleSensInitDataMaster();

    /* Initialization of shipping destination management table */
    VehicleSensInitDeliveryCtrlTbl();

    /* Initialization of shipping destination management table management information */
    VehicleSensInitDeliveryCtrlTblMng();

    /* Initialization of package delivery management table */
    VehicleSensInitPkgDeliveryTblMng();

    /* Initialization of non-volatile access function block */
    VehicleSensFromAccessInitialize();

    /* Clear message information waiting to receive a response */
    (void)memset(&g_wait_for_resp_msg, 0x00, sizeof(VEHICLE_MSG_BUF));
    g_wait_for_resp_set_n = NULL;
    g_sent_fst_pkg_delivery_ext = FALSE; /* Initial expansion package data undelivered */

    VehilceSens_InitVehicleSpeed(); /* for PASCD */

    /* Start of data interruption monitoring */
    VehicleSensInitDataDisrptMonitor();
    return ret_val;
}

/*******************************************************************************
 * MODULE    : VehicleSensDeliveryEntry
 * ABSTRACT  : Vehicle sensor information delivery registration
 * FUNCTION  :
 * ARGUMENT  : *msg  : message buffer
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void VehicleSensDeliveryEntry(const VEHICLE_MSG_DELIVERY_ENTRY *msg) {
    int32  event_val;
    EventID  event_id;

    /* Call the process of creating the delivery destination management table */
    event_val = static_cast<int32>(VehicleSensEntryDeliveryCtrl(msg));

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");

    /* Event Generation */
    event_id = VehicleCreateEvent(msg->data.pno);

    /* Publish Events */
    (void)_pb_SetEvent(event_id, SAPI_EVSET_ABSOLUTE, event_val);

    if (VEHICLE_RET_NORMAL == event_val) {
        /* Successful delivery registration */
        /* Deliver data for the first time */
        VehicleSensFirstDelivery((PNO)(msg->data.pno), (DID)(msg->data.did));
    }

    /* Event deletion */
    (void)VehicleDeleteEvent(event_id);

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "-");
}

/*******************************************************************************
* MODULE    : VehicleSensGetVehicleData
* ABSTRACT  : Vehicle sensor information acquisition
* FUNCTION  :
* ARGUMENT  : *msg  : message buffer
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetVehicleData(const VEHICLE_MSG_GET_VEHICLE_DATA *msg) {
    void    *share_top;            /* Start address of shared memory */
    u_int32    share_size;          /* Size of shared memory area */
    u_int8    get_method;          /* Data collection way */
    VEHICLESENS_DATA_MASTER    master;    /* Data master */
    RET_API    ret_api;
    int32    ret_val;
    int32    event_val;
    EventID    event_id;
    SENSOR_MSG_GPSDATA_DAT    gps_master;  /* GPS Data Master */

    /* Check the DID */
    ret_val = VehicleSensCheckDid(msg->data.did);
    if (VEHICLESENS_INVALID != ret_val) {  // LCOV_EXCL_BR_LINE 6:did always valid
        /* DID normal */

        /* Link to shared memory */
        ret_api = _pb_LinkShareData(const_cast<char *>(VEHICLE_SHARE_NAME), &share_top, &share_size);
        if (RET_NORMAL == ret_api) {
            /* Acquire the specified data from the data master. */
            get_method = VehicleSensGetSelectionItemList(msg->data.did);
            if ((VEHICLESENS_GETMETHOD_GPS == get_method) &&
                ((msg->data.did != VEHICLE_DID_LOCATION_LONLAT)   &&
                (msg->data.did != VEHICLE_DID_LOCATION_ALTITUDE) &&
                (msg->data.did != VEHICLE_DID_MOTION_HEADING))) {
                /* _CWORD71_ processing speed(Memset modification) */
                /* Retrieval of the data master fails.,Initialize size to 0 to prevent unauthorized writes */
                gps_master.us_size = 0;
                VehicleSensGetGpsDataMaster(msg->data.did, get_method, &gps_master);
                /* Check the data size */
                if (msg->data.size < gps_master.us_size) {
                    /* Shared memory error(Insufficient storage size) */
                    event_val = VEHICLE_RET_ERROR_OUTOF_MEMORY;
                } else {
                    /* Write data master to shared memory */
                    PosSetShareData(share_top,
                                    msg->data.offset, (const void *)&gps_master.uc_data, gps_master.us_size);

                    /* Set Successful Completion */
                    event_val = VEHICLE_RET_NORMAL;
                    SensLogWriteOutputData(SENSLOG_DATA_O_UNSPECIFIED,
                                            msg->data.did, msg->data.pno,
                                            reinterpret_cast<uint8_t *>(&(gps_master.uc_data[0])),
                                            gps_master.us_size, SENSLOG_RES_SUCCESS);
                }
            } else {
                (void)memset(reinterpret_cast<void *>(&master), 0, sizeof(VEHICLESENS_DATA_MASTER));
                VehicleSensGetDataMaster(msg->data.did, get_method, &master);

                /* Check the data size */
                if (msg->data.size < master.us_size) {
                    /* Shared memory error(Insufficient storage size) */
                    event_val = VEHICLE_RET_ERROR_OUTOF_MEMORY;
                } else {
                    /* Write data master to shared memory */
                    PosSetShareData(share_top,
                                    msg->data.offset, (const void *)master.uc_data, master.us_size);

                    /* Set Successful Completion */
                    event_val = VEHICLE_RET_NORMAL;
                    SensLogWriteOutputData(SENSLOG_DATA_O_UNSPECIFIED,
                                            msg->data.did, msg->data.pno,
                                            reinterpret_cast<uint8_t *>(&(master.uc_data[0])),
                                            master.us_size, SENSLOG_RES_SUCCESS);
                }
            }
        } else {
            /* Shared memory error */
            event_val = VEHICLE_RET_ERROR_OUTOF_MEMORY;
        }
    } else {
        /* DID error */
        event_val = VEHICLE_RET_ERROR_DID;
    }

    /* Event Generation */
    event_id = VehicleCreateEvent(msg->data.pno);

    /* Publish Events */
    ret_api = _pb_SetEvent(event_id, SAPI_EVSET_ABSOLUTE, event_val);
    if (RET_NORMAL != ret_api) {
        /* Event issuance failure */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SetEvent ERROR!! [ret_api = %d]", ret_api);
    }

    /* Event deletion */
    (void)VehicleDeleteEvent(event_id);

    return;
}

/*******************************************************************************
* MODULE    : VehicleSensWatchStopPage
* ABSTRACT  : Vehicle sensor interruption monitoring
* FUNCTION  :
* ARGUMENT  : *msg  : message buffer
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensWatchStopPage(const VEHICLE_MSG_WATCH_STOPPAGE *msg) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* Not used(Delete Used Functions After Investigation) */
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensPkgDeliveryEntry
* ABSTRACT  : Vehicle sensor information package delivery registration
* FUNCTION  :
* ARGUMENT  : *msg  : message buffer
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensPkgDeliveryEntry(const SENSOR_MSG_DELIVERY_ENTRY *msg) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32  event_val;
    EventID  event_id;

    /* Call the process of creating the delivery destination management table */
    event_val = static_cast<int32>(VehicleSensEntryPkgDeliveryCtrl(msg , VEHICLESENS_EXT_OFF));
    /* #Polaris_003 */

    /* Event Generation */
    event_id = PosCreateEvent(msg->data.pno);

    /* Publish Events */
    (void)_pb_SetEvent(msg->data.event_id, SAPI_EVSET_ABSOLUTE, event_val);

    if (VEHICLE_RET_NORMAL == event_val) {
        /* Successful delivery registration */
        /* Deliver package data for the first time */
        VehicleSensFirstPkgDelivery(&msg->data);
    }

    /* Event deletion */
    (void)PosDeleteEvent(event_id);
}
// LCOV_EXCL_STOP

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*******************************************************************************
* MODULE    : VehicleSensPkgDeliveryEntryExt
* ABSTRACT  : Vehicle Sensor Information Extended Package Delivery Registration
* FUNCTION  :
* ARGUMENT  : *msg  : message buffer
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensPkgDeliveryEntryExt(const SENSOR_MSG_DELIVERY_ENTRY *msg) {
    int32  event_val;

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");

    /* Call the process of creating the delivery destination management table */
    event_val = static_cast<int32>(VehicleSensEntryPkgDeliveryCtrl(msg , VEHICLESENS_EXT_ON));

    /* Event Generation */
    (void)PosCreateEvent(msg->data.pno);

    /* Publish Events */
    (void)_pb_SetEvent(msg->data.event_id, SAPI_EVSET_ABSOLUTE, event_val);

    if (VEHICLE_RET_NORMAL == event_val) {
        /* Successful delivery registration */
        /* Provide initial expansion package data delivery */
        VehicleSensFirstPkgDeliveryExt(&msg->data);

        /* Initial expansion package data delivered */
        g_sent_fst_pkg_delivery_ext = TRUE;

        FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, 
                      "FirstPkgDeliveryExt call.[%d]",
                      g_sent_fst_pkg_delivery_ext); /* TODO */
    }

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "-");
}

#else
/*******************************************************************************
* MODULE    : VehicleSensPkgDeliveryEntryError
* ABSTRACT  : Vehicle Sensor Information Extended Package Delivery Registration
* FUNCTION  :
* ARGUMENT  : *msg  : message buffer
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensPkgDeliveryEntryError(const SENSOR_MSG_DELIVERY_ENTRY *msg) {
    RET_API  ret_api;
    EventID  event_id;

    /* Event Generation */
    event_id = PosCreateEvent(msg->data.pno);

    /* Publish Event Set DID Error */
    ret_api = _pb_SetEvent(msg->data.event_id, SAPI_EVSET_ABSOLUTE, VEHICLE_RET_ERROR_DID);
}

#endif

/*******************************************************************************
* MODULE    : VehicleSensGetVehiclePkgData
* ABSTRACT  : Vehicle sensor information package data acquisition
* FUNCTION  :
* ARGUMENT  : *msg  : message buffer
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGetVehiclePkgData(const SENSOR_MSG_GET_SENSOR_DATA *msg) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensLineSensDataDelivery
* ABSTRACT  : LineSensor Vehicle Signal Notification
* FUNCTION  :
* ARGUMENT  : *msg      : message buffer
*           : p_datamaster_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensLineSensDataDelivery(const LSDRV_MSG_LSDATA *msg, PFUNC_DMASTER_SET_N p_datamaster_set_n) {
    int32  i;
    BOOL  sens_ext;

    sens_ext = TRUE;

    for (i = 0; i < msg->st_para.uc_data_num; i++) {
        /* Setting Vehicle Signal Data from LineSensor as Data Master */
        VehicleSensSetDataMasterLineSens((const LSDRV_LSDATA *) & (msg->st_para.st_data[i]),
                                          p_datamaster_set_n, sens_ext);
    }
}

/*******************************************************************************
* MODULE    : VehicleSensLineSensDataDeliveryG
* ABSTRACT  : LineSensor Vehicle Signal Notification
* FUNCTION  :
* ARGUMENT  : *msg      : message buffer
*           : p_datamaster_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensLineSensDataDeliveryG(const LSDRV_MSG_LSDATA_G *msg, PFUNC_DMASTER_SET_N p_datamaster_set_n) {
    int32  i;
    BOOL  sens_ext;

    sens_ext = TRUE;
    if (g_sent_fst_pkg_delivery_ext == TRUE) {
        /* Initial Expansion Package Data Delivery,Without storing extended data */
        sens_ext = FALSE;
    }

    for (i = 0; i < msg->st_para.uc_data_num; i++) {
        /* Setting Vehicle Signal Data from LineSensor as Data Master */
        VehicleSensSetDataMasterLineSensG((const LSDRV_LSDATA_G *) & (msg->st_para.st_data[i]),
                                              p_datamaster_set_n, sens_ext);
    }
}

/*******************************************************************************
* MODULE    : VehicleSensLineSensDataDeliveryGyroTrouble
* ABSTRACT  : Gyro Failure Status Notification
* FUNCTION  : Notify of a gyro failure condition
* ARGUMENT  : *msg      : message buffer
*           : p_datamaster_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensLineSensDataDeliveryGyroTrouble(const LSDRV_MSG_LSDATA_GYRO_TROUBLE *msg, PFUNC_DMASTER_SET_N p_datamaster_set_n) {  // LCOV_EXCL_START 8 : dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* Setting Gyro Failure Status Data from LineSensor to Data Master */
    VehicleSensSetDataMasterGyroTrouble((const LSDRV_MSG_LSDATA_DAT_GYRO_TROUBLE *)&(msg->st_para),
                                           p_datamaster_set_n);
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensLineSensDataDeliverySysGpsInterruptSignal
* ABSTRACT  : SYS GPS interrupt notification
* FUNCTION  : Notify SYS GPS interrupt signals
* ARGUMENT  : *msg            : message buffer
*           : p_datamaster_set_sharedmemory  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensLineSensDataDeliverySysGpsInterruptSignal(const LSDRV_MSG_LSDATA_GPS_INTERRUPT_SIGNAL *msg, PFUNC_DMASTER_SET_SHARED_MEMORY p_datamaster_set_sharedmemory) {  // LCOV_EXCL_START 8 :dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* Sets the SYS GPS interrupt data from the LineSensor to the data master. */
    VehicleSensSetDataMasterSysGpsInterruptSignal((const LSDRV_MSG_LSDATA_DAT_GPS_INTERRUPT_SIGNAL *)&(msg->st_para),
                                                    p_datamaster_set_sharedmemory);
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensLineSensDataDeliveryGyroConnectStatus
* ABSTRACT  : Gyro Connection Status Notification
* FUNCTION  : Notify the state of the gyro connection
* ARGUMENT  : *msg            : message buffer
*           : p_datamaster_set_sharedmemory  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensLineSensDataDeliveryGyroConnectStatus(const LSDRV_MSG_LSDATA_GYRO_CONNECT_STATUS *msg, PFUNC_DMASTER_SET_SHARED_MEMORY p_datamaster_set_sharedmemory) {  // LCOV_EXCL_START 8 : dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* Setting Gyro Connection Status Data from LineSensor to Data Master */
    VehicleSensSetDataMasterGyroConnectStatus((const LSDRV_MSG_LSDATA_DAT_GYRO_CONNECT_STATUS *)&(msg->st_para),
                                                p_datamaster_set_sharedmemory);
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensLineSensDataDeliveryGpsAntennaStatus
* ABSTRACT  : GPS Antenna Connection Status Notification
* FUNCTION  : Notify the GPS antenna connection status
* ARGUMENT  : *msg      : message buffer
*           : p_datamaster_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensLineSensDataDeliveryGpsAntennaStatus(const LSDRV_MSG_LSDATA_GPS_ANTENNA_STATUS *msg, PFUNC_DMASTER_SET_N p_datamaster_set_n) {  // LCOV_EXCL_START 8 : dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* Setting GPS Antenna Connection Status Data from LineSensor as Data Master */
    VehicleSensSetDataMasterGpsAntennaStatus((const LSDRV_MSG_LSDATA_DAT_GPS_ANTENNA_STATUS *)&(msg->st_para),
                                               p_datamaster_set_n);
}
// LCOV_EXCL_STOP

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*******************************************************************************
* MODULE    : VehicleSensLineSensDataDeliveryFst
* ABSTRACT  : LineSensor Vehicle Signal Notification(Initial Sensor)
* FUNCTION  :
* ARGUMENT  : *msg      : message buffer
*           : p_datamaster_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensLineSensDataDeliveryFst(const LSDRV_MSG_LSDATA_FST *msg, PFUNC_DMASTER_SET_N p_datamaster_set_n) {  // LCOV_EXCL_START 8 : dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP
#endif

#if CONFIG_SENSOR_EXT_VALID        /* Initial Sensor Support */
/*******************************************************************************
* MODULE    : VehicleSensLineSensDataDeliveryFstG
* ABSTRACT  : LineSensor Vehicle Signal Notification(Initial Sensor)
* FUNCTION  :
* ARGUMENT  : *msg      : message buffer
*           : p_datamaster_set_n  : Data Master Set Notification(Callback function)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensLineSensDataDeliveryFstG(const LSDRV_MSG_LSDATA_FST *msg, PFUNC_DMASTER_SET_N p_datamaster_set_n) {
    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "+");

    if (msg == NULL) {  // LCOV_EXCL_BR_LINE 6:msg cannot be null
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "msg == NULL\r\n");  // LCOV_EXCL_LINE 8: dead code
    } else {
        /* Set Vehicle Signal Data from LineSensor (Initial Sensor) as Data Master */
        VehicleSensSetDataMasterLineSensFstG((const LSDRV_MSG_LSDATA_DAT_FST *) & (msg->st_para),
                p_datamaster_set_n);
    }

    /* Internal debug log output */
    FRAMEWORKUNIFIEDLOG(ZONE_26, __FUNCTION__, "-");
}
#endif

/*******************************************************************************
* MODULE    : VehicleSensGpsDataDelivery
* ABSTRACT  : GPS data notification
* FUNCTION  :
* ARGUMENT  : *msg            : message buffer
*           : p_datamaster_set_n        : For Data Master Set Notification(Callback function) delivery
*           : p_datamaster_set_sharedmemory  : Data Master Set Notification (Callback Function) Shared Memory Write
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensGpsDataDelivery(SENSOR_MSG_GPSDATA *msg,
                                PFUNC_DMASTER_SET_N p_datamaster_set_n,
                                PFUNC_DMASTER_SET_SHARED_MEMORY p_datamaster_set_sharedmemory) {
    /* Setting GPS Data as Data Master */
    if (msg->st_para.ul_did == VEHICLE_DID_MAIN_GPS_INTERRUPT_SIGNAL) {  // LCOV_EXCL_BR_LINE 6:DID is not used
        // LCOV_EXCL_START 8: dead code
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        /* [PastModel002 Specifications] GPS->_CWORD102_ interrupt or not is obtained from GPS */
        VehicleSensSetDataMasterMainGpsInterruptSignal((const SENSOR_MSG_GPSDATA_DAT *)&(msg->st_para),
                                                         p_datamaster_set_sharedmemory);
        // LCOV_EXCL_STOP
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_27, __FUNCTION__, 
                      "VehicleSensGpsDataDelivery VehicleSensSetDataMasterGps() -->");
        VehicleSensSetDataMasterGps(reinterpret_cast<SENSOR_MSG_GPSDATA_DAT *>(&(msg->st_para)),
                                      p_datamaster_set_n);
        FRAMEWORKUNIFIEDLOG(ZONE_27, __FUNCTION__, 
                      "VehicleSensGpsDataDelivery VehicleSensSetDataMasterGps() <--");
    }
}

/*******************************************************************************
* MODULE    : VehicleSensDataMasterSetN
* ABSTRACT  : Data master set notification process(Callback function)
* FUNCTION  : Call the data delivery process
* ARGUMENT  : did    : Data ID
*           : chg_type  : Change type(no change/with change)
*           : get_method  : Acquisition method(Direct Line/CAN)
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensDataMasterSetN(DID did, u_int8 chg_type, u_int8 get_method) {

  u_int8 chgType;

  chgType = chg_type;

  switch (did) {
    case POSHAL_DID_SPEED_KMPH:
    {
      if (ChkUnitType(UNIT_TYPE_GRADE1) == TRUE) {
        /* For creating PASCD Sentence of NMEA */

        int ret;
        VEHICLESENS_VEHICLE_SPEED_DAT stVehicleSpeed;

        ret = clock_gettime(CLOCK_MONOTONIC, &(stVehicleSpeed.ts));
        if (ret != 0) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "clock_gettime error:%m");
        } else {
          VEHICLESENS_DATA_MASTER stData;
          SENSORMOTION_SPEEDINFO_DAT* pSpdInfo;

          VehicleSensGetMotionSpeed(&stData, VEHICLESENS_GETMETHOD_INTERNAL);  // LCOV_EXCL_BR_LINE 6:unexpected branch  //NOLINT (whitespace/line_length)
          pSpdInfo = (SENSORMOTION_SPEEDINFO_DAT*)(stData.uc_data);

          stVehicleSpeed.speed = pSpdInfo->Speed;

          VehicleSens_StoreVehicleSpeed(&stVehicleSpeed);  // LCOV_EXCL_BR_LINE 6:unexpected branch  //NOLINT (whitespace/line_length)
        }
      }

      break;
    }
    case POSHAL_DID_GPS_NMEA:
    {
      VehilceSens_InitVehicleSpeed();

      break;
    }
    default:
      break;
  }

  /* Call the data delivery process */
  VehicleSensDeliveryProc( did, chgType, get_method );
}

/*******************************************************************************
* MODULE    : VehicleSensDataMasterSetSharedMemory
* ABSTRACT  : Data master set notification process(Callback function)
* FUNCTION  : Call Write Shared Memory
* ARGUMENT  : did    : Data ID
*           : chg_type  : Whether or not data is updated
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensDataMasterSetSharedMemory(DID did, u_int8 chg_type) {  // LCOV_EXCL_START 8: dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* When there is no change, the shared memory write process is not called. */
    if (chg_type == VEHICLESENS_NEQ) {
        VehicleSensWriteSharedMemory(did);
    }
}
// LCOV_EXCL_STOP

/* ++ GPS _CWORD82_ support */
/*******************************************************************************
* MODULE    : VehicleSensSetVehicleData
* ABSTRACT  : GPS command request processing
* FUNCTION  : Transfer a GPS command request
* ARGUMENT  : *msg  : message buffer
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensSetVehicleData(const VEHICLE_MSG_SEND *msg) {
    u_int16    size;            /* Data length setting          */
    u_int16    all_len;          /* Sent message length          */
    u_int16    mode;            /* Mode information            */
    RID      req_id = 0;          /* Resources ID            */

    T_APIMSG_MSGBUF_HEADER  header;      /* Message header          */
    TG_GPS_SND_DATA      data;      /* Message body          */
    u_int8  snd_buf[(sizeof(T_APIMSG_MSGBUF_HEADER) + sizeof(TG_GPS_SND_DATA))];

    /* Message header generation */
    size = sizeof(data);
    header.signo = 0;            /* Signal information        */
    header.hdr.sndpno = PNO_VEHICLE_SENSOR;  /* Source thread number      */
    header.hdr.respno = 0;          /* Destination process No.      */
    header.hdr.cid = CID_GPS__CWORD82__REQUEST;  /* Command ID          */
    header.hdr.msgbodysize = size;      /* Message data length setting    */
    header.hdr.rid = req_id;          /* Resource ID Setting        */
    header.hdr.reserve = 0;          /* Reserved Area Clear      */

    /* Message body generating */
    data.us_size = msg->data.size;
    memcpy(&(data.ub_data[0]), &(msg->data.data[0]), msg->data.size);

    /* Reserved Area Clear      */
    data.reserve[0] = 0;
    data.reserve[1] = 0;
    data.reserve[2] = 0;
    data.reserve[3] = 0;

    /* Message generation */
    (void)memcpy(&snd_buf[0], &header, sizeof(header));
    (void)memcpy(&snd_buf[sizeof(header)], &data, sizeof(data));
    all_len = static_cast<u_int16>(size + sizeof(header));
    mode    = 0;

    FRAMEWORKUNIFIEDLOG(ZONE_27, __FUNCTION__, "VehicleSensSetVehicleData NMEA = %s", data.ub_data);
    (void)_pb_SndMsg(PNO_NAVI_GPS_MAIN, all_len, reinterpret_cast<void *>(&snd_buf[0]), mode);
}

/*******************************************************************************
 * MODULE    : VehicleSensDrDeliveryEntry
 * ABSTRACT  : Sensor Internal Information Delivery Registration for DR
 * FUNCTION  :
 * ARGUMENT  : *msg  : message buffer
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void VehicleSensDrDeliveryEntry(const VEHICLE_MSG_DELIVERY_ENTRY *msg) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32  event_val;
    EventID  event_id;

    /* Call the process of creating the delivery destination management table */
    event_val = static_cast<int32>(DeadReckoningEntryDeliveryCtrl((const DEADRECKONING_MSG_DELIVERY_ENTRY*)msg));
    /* #QAC confirmation Rule11.4 Member reference to suppress address reference error in data R/W */

    /* Event Generation */
    event_id = VehicleCreateEvent(msg->data.pno);

    /* Publish Events */
    (void)_pb_SetEvent(event_id, SAPI_EVSET_ABSOLUTE, event_val);

    if (VEHICLE_RET_NORMAL == event_val) {
        /* Successful delivery registration */
        /* Deliver data for the first time */
        DeadReckoningFirstDelivery((PNO)(msg->data.pno), (DID)(msg->data.did));
    }
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : VehicleSensGetLog
 * ABSTRACT  : Log acquisition request
 * FUNCTION  :
 * ARGUMENT  : *msg  : message buffer
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void VehicleSensGetLog(const VEHICLEDEBUG_MSG_BUF *msg) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    DeadReckoningGetLocationLogStatus(msg->hdr.hdr.sndpno);
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : VehicleSensSetLog
 * ABSTRACT  : Log Setting Request
 * FUNCTION  :
 * ARGUMENT  : *msg  : message buffer
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void VehicleSensSetLog(const VEHICLEDEBUG_MSG_BUF* msg) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return;
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : VehicleSensWriteLocalTime
 * ABSTRACT  : Set LOCALTIME to non-volatile
 * FUNCTION  :
 * ARGUMENT  : *msg  : message buffer
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void VehicleSensWriteLocalTime(const CANINPUT_MSG_INFO *msg) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOCALTIME  localtime = {0};
    int32    time_diff;
    RET_API    ret_write_api;
    static u_int8 cnt = 0U;    /* For update cycle count */
    if (msg != NULL) {
        memset(&time_diff, 0x00, sizeof(time_diff));  /* Coverity CID:18813 compliant */

        /* Determine that the 1s cycle (cnt == 1) has elapsed for 1 second.  */
        cnt++;

        /* When 5 seconds or more have elapsed since the last update    */
        /* Saving Time Information in Non-volatile Memory      */
        if (cnt >= NV_UPDATE_CYCLE_LOCALTIME) {
            /* Non-volatile write */
            ret_write_api = VehicleSensWriteNVLocaltime(&localtime, &time_diff);
            if (ret_write_api != RET_NORMAL) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "NV write error.");
            }
            cnt = 0U;
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ARGUMENT NULL");
    }
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensSetEphNumSharedMemory
* ABSTRACT  : Write effective ephemeris number to shared memory
* FUNCTION  :
* ARGUMENT  : *msg  : message buffer
* NOTE      :
* RETURN    : void
******************************************************************************/
void VehicleSensSetEphNumSharedMemory(const SENSOR_MSG_GPSDATA *msg) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API    ret_api;
    u_int8    ephemeris_num;

    if (msg != NULL) {
        ephemeris_num = msg->st_para.uc_data[0];

        ret_api = VehicleSensWriteDataValidEphemerisNum(ephemeris_num);

        if (ret_api != RET_NORMAL) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Share Memory write error.");
        }
    }

    return;
}
// LCOV_EXCL_STOP

/*******************************************************************************
* MODULE    : VehicleSensDrRcvMsg
* ABSTRACT  : Data delivery for DR,Write location information to shared memory
* FUNCTION  :
* ARGUMENT  : const EPHEMERIS_NUM_DATA_DAT * : Incoming message
* NOTE      :
* RETURN    : void
******************************************************************************/
void  VehicleSensDrRcvMsg(const DEAD_RECKONING_RCVDATA * msg) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    DEAD_RECKONING_LONLAT_INFO lonlat_info;

    if (msg != NULL) {
        DeadReckoningRcvMsg(msg, &lonlat_info);

        /* Process for storing location information in non-volatile memory during shutdown    */
        /* With a refresh interval of 1 second,Save only when location information is valid data    */
        if (lonlat_info.calc_called == TRUE) {
            static u_int8 cnt = 0U;  /* For update cycle count */

            /* Sensor data is 100ms cycle,Determine that (cnt == 10) has elapsed for 1 second  */
            /* Cnt increments to 10                  */
            if (cnt < NV_UPDATE_CYCLE_LONLAT) {
                cnt++;
            }

            /* When 1 second or more has elapsed since the last update and the location information is valid,    */
            /* Saving Location Information in Non-Volatile Memory                  */
            if ((cnt >= NV_UPDATE_CYCLE_LONLAT) && (lonlat_info.available == static_cast<u_int8>(TRUE))) {
                VehicleSensStoreLonlat(&(lonlat_info.lonlat));
                cnt = 0U;
            }
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ARGUMENT NULL");
    }

    return;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *  Data setting process
 *
 *  Setting Specified Data to Data Master
 *
 * @param[in]   const VEHICLE_MSG_BUF *msg       : message buffer
 * @param[out]  PFUNC_DMASTER_SET_N p_datamaster_set_n : Data Master Set Notification(Callback function)
 * @return      none
 * @retval      none
 */
void VehicleSensCommonDataDelivery(const VEHICLE_MSG_BUF *msg, PFUNC_DMASTER_SET_N p_datamaster_set_n) {
    const POS_MSGINFO  *pstPosMsg = (const POS_MSGINFO *) & (msg->data[0]);

    /* Individual processing for each data ID */
    switch (pstPosMsg->did) {
        case VEHICLE_DID_SETTINGTIME:
        {
            /* By checking the evacuation message information,Determine whether the GPS time has already been set and requested */
            if (NULL == g_wait_for_resp_set_n) {
                /* GPS time settable */
                /* GPS time setting data transmission */
                VehicleSensGpsTimeSndMsg(pstPosMsg);

                /* Save message information(Used when a response is received.)*/
                (void)memcpy(&g_wait_for_resp_msg, msg, sizeof(VEHICLE_MSG_BUF));
                g_wait_for_resp_set_n = p_datamaster_set_n;
            } else {
                /* GPS time setting process is already in progress:Reply BUSY to requesting processes */
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "SetGpsTime already.");
            }
            break;
        }
        default:
            /* Set the specified data in the data master */
            VehicleSensSetDataMasterData((const POS_MSGINFO *)msg->data, p_datamaster_set_n);
            break;
    }

    return;
}

/**
 * @brief
 *  GPS time setting data transmission process
 *
 *  Send the specified data to the GPS thread
 *
 * @param[in]   const POS_MSGINFO *pos_msg       : message buffer
 * @return      none
 * @retval      none
 */
void VehicleSensGpsTimeSndMsg(const POS_MSGINFO *pos_msg) {
    RET_API    ret_api = RET_NORMAL;    /* System API return value */
    u_int16    size = 0;          /* Data length setting */
    u_int16    all_len = 0;        /* Sent message length */
    u_int16    mode = 0;          /* Mode information */
    RID      req_id = 0;          /* Resources ID */
    T_APIMSG_MSGBUF_HEADER  header;      /* Message header */
    TG_GPS_SND_DATA      data;      /* Message body */
    u_int8    snd_buf[(sizeof(T_APIMSG_MSGBUF_HEADER) + sizeof(TG_GPS_SND_DATA))];

    memset(&header, 0x00, sizeof(T_APIMSG_MSGBUF_HEADER));
    memset(&data, 0x00, sizeof(TG_GPS_SND_DATA));

    /* Message header generation */
    size = sizeof(data);
    header.signo = 0;            /* Signal information          */
    header.hdr.sndpno = PNO_VEHICLE_SENSOR;  /* Source thread number      */
    header.hdr.respno = 0;          /* Destination process No.      */
    header.hdr.cid = CID_GPS_TIMESETTING;  /* Command ID          */
    header.hdr.msgbodysize = size;      /* Message data length setting  */
    header.hdr.rid = req_id;          /* Resource ID Setting        */

    /* Message body generating */
    data.us_size = pos_msg->size;
    memcpy(&(data.ub_data[0]), &(pos_msg->data[0]), pos_msg->size);

    /* Messaging */
    (void)memcpy(&snd_buf[0], &header, sizeof(header));
    (void)memcpy(&snd_buf[sizeof(header)], &data, sizeof(data));
    all_len = static_cast<u_int16>(size + sizeof(header));
    mode   = 0;
    ret_api = _pb_SndMsg(PNO_NAVI_GPS_MAIN, all_len, reinterpret_cast<void *>(&snd_buf[0]), mode);
    if (RET_NORMAL != ret_api) {
        /* Message transmission processing failed */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "send Message failed.");
    }

    return;
}

/**
 * @brief
 *  GPS time setting result notification process
 *
 *  Setting Specified Data to Data Master
 *
 * @param[in]   const VEHICLE_MSG_BUF *msg       : message buffer
 * @return      none
 * @retval      none
 */
void VehicleSensGpsTimeDelivery(const VEHICLE_MSG_BUF *msg) {
    int32              event_val = POS_RET_ERROR_INNER;  /* Event value */
    const TG_GPS_RET_TIMESET_MSG  *gps_ret_time;            /* GPS time setting response message */

    /* Determine the GPS time setting result */
    gps_ret_time = (const TG_GPS_RET_TIMESET_MSG *)msg;

    if (GPS_SENDOK == gps_ret_time->status) {
        event_val = POS_RET_NORMAL;
    } else {
        event_val = POS_RET_ERROR_TIMEOUT;
    }

    /* Set the specified data in the data master */
    if (POS_RET_NORMAL == event_val) {
        VehicleSensSetDataMasterData((const POS_MSGINFO *)&g_wait_for_resp_msg.data, g_wait_for_resp_set_n);
    }
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, 
                  "SetGpsTime Result[%d] EventVal[%d]",
                  static_cast<uint32_t>(gps_ret_time->status), static_cast<uint32_t>(event_val));

    /* Clear saved message information */
    (void)memset(&g_wait_for_resp_msg, 0x00, sizeof(VEHICLE_MSG_BUF));
    g_wait_for_resp_set_n = NULL;

    return;
}

/**
 * @brief
 *  Specified process No. event transmission processing
 *
 *  Send an event to the specified process No.
 *
 * @param[in]   uint16_t snd_pno   : Destination process number
 * @param[in]   int32_t  event_val : Sent event value
 * @return      RET_API
 * @retval      RET_NORMAL      Normal completion<br>
 *              RET_ERROR       Event generation failure<br>
 *              RET_ERRPARAM    Configuration mode error<br>
 *              RET_EV_NONE     Specified event does not exist<br>
 *              RET_EV_MAX      The set event value exceeds the maximum value<br>
 *              RET_EV_MIN      The set event value is below the minimum value.
 */
RET_API VehicleSensSendEvent(uint16_t snd_pno, int32_t event_val) {  // LCOV_EXCL_START 8 : dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API      ret_val = RET_ERROR;        /* Return value */
    EventID      event_id = 0;          /* Event ID */

    /* Event Generation */
    event_id = VehicleCreateEvent(snd_pno);
    if (0 != event_id) {
        /* Event publishing(Release Event Wait) */
        ret_val = _pb_SetEvent(event_id, SAPI_EVSET_ABSOLUTE, event_val);
        if (RET_NORMAL != ret_val) {
            /* Event issuance failure */
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "set Event failed.");
        }

        /* Event deletion */
        (void)VehicleDeleteEvent(event_id);
    } else {
        /* Event generation failure */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "create Event failed.");
    }

    return ret_val;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *  Pos_Main thread stop processing
 */
void VehicleSensThreadStopProcess(void) {
    /* Exit Process Implementation Point >>> */


    /* <<< Exit Process Implementation Point */

    /* Thread stop processing */
    PosTeardownThread(ETID_POS_MAIN);

    /* don't arrive here */
    return;
}

/**
 * @brief
 *  Initialization of data interruption monitoring process<br>
 *  1.Initial data reception monitoring timer issuance
 */
static void VehicleSensInitDataDisrptMonitor(void) {

    /* Initial sensor data reception monitoring timer */
    VehicleUtilitySetTimer(SNS_FST_TIMER);

    return;
}

/**
 * @brief
 *  Data disruption monitoring process<br>
 *  1.Stop timer for monitoring initial data reception<br>
 *  2.Stop timer for periodic data reception monitoring<br>
 *  3.Periodic data reception monitoring timer issuance
 *
 * @param[in]   did  Data type
 */
static void VehicleSensDataDisrptMonitorProc(DID did) {
    static BOOL is_rcv_sns_data = FALSE;

    switch (did) {
        case POSHAL_DID_GYRO_X:
        case POSHAL_DID_GYRO_Y:
        case POSHAL_DID_GYRO_Z:
        case POSHAL_DID_GSNS_X:
        case POSHAL_DID_GSNS_Y:
        case POSHAL_DID_GSNS_Z:
        case POSHAL_DID_SPEED_PULSE:
        case POSHAL_DID_REV:
        case POSHAL_DID_GPS_ANTENNA:
        case POSHAL_DID_GYRO_EXT:
        case POSHAL_DID_GYRO_TEMP:
        case POSHAL_DID_PULSE_TIME:
        case POSHAL_DID_SNS_COUNTER:
        {
            if (is_rcv_sns_data == FALSE) {
                /* Initial sensor data reception monitoring timer */
                VehicleUtilityStopTimer(SNS_FST_TIMER);
                is_rcv_sns_data = TRUE;

                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "is_rcv_sns_data=TRUE");
            }

            /* Cyclic sensor data reception monitoring timer stopped */
            VehicleUtilityStopTimer(SNS_CYCLE_TIMER);
            /* Cyclic sensor data reception monitoring timer setting */
            VehicleUtilitySetTimer(SNS_CYCLE_TIMER);
            /* Sensor data interruption log output timer */
            VehicleUtilityStopTimer(SNS_DISRPT_TIMER);

            break;
        }
        default:
        {
            /* nop */
        }
    }

    return;
}

/**
 * @brief
 *  Timeout message reception processing
 *
 * @param[in]   rcv_msg Incoming message
 */
static void VehicleSensRcvMsgTout(TimerToutMsg* rcv_msg) {
    uint8_t tim_kind = static_cast<uint8_t>(rcv_msg->TimerSeq >> 8);

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "+ ### TIMEOUT ### Seq=0x%04x", rcv_msg->TimerSeq);
    switch (tim_kind) {
        case SNS_FST_TIMER:
        case SNS_CYCLE_TIMER:
        case SNS_DISRPT_TIMER:
        {
            /* Sensor data interruption log output timer setting */
            VehicleUtilityStopTimer(SNS_DISRPT_TIMER);
            VehicleUtilitySetTimer(SNS_DISRPT_TIMER);
            break;
        }
        default:
        {
            /* nop */
        }
    }
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "-");
    return;
}


/**
 * @brief
 *  Initialize Vehilce Speed Information
 *
 * @details  This is for creating PASCD Sentence of NMEA. <br>
 *           You can initialize the buffer of Vehicle Speed Information.
 */
static void VehilceSens_InitVehicleSpeed(void) {
  (void)memset(&g_vehicle_speed_info, 0x00, sizeof(g_vehicle_speed_info));
  return;
}

/**
 * @brief
 *  Store Vehilce Speed Data
 *
 * @details  This is for creating PASCD Sentence of NMEA. <br>
 *           You can store Vehicle Speed Information for a cycle.
 *
 * @param[in]   VEHICLESENS_VEHICLE_SPEED_DAT* pVehicleSpeed : Speed [m/s] and TimeSpec
 */
static void VehicleSens_StoreVehicleSpeed(VEHICLESENS_VEHICLE_SPEED_DAT* pVehicleSpeed) {
  VEHICLESENS_VEHICLE_SPEED_INFO* pInfo = &g_vehicle_speed_info;


//  if (pInfo->sampleCount < VEHICLESENS_NMEA_PASCD_SAMPLECOUNT_MAX) {
  if (pInfo->sampleCount == 0) {
    (void)memcpy((pInfo->listSpd) + (pInfo->sampleCount), pVehicleSpeed, sizeof(VEHICLESENS_VEHICLE_SPEED_DAT));
    pInfo->sampleCount++;

  } else if (pInfo->sampleCount < VEHICLESENS_NMEA_PASCD_SAMPLECOUNT_MAX) {
    (void)_pb_memcpy((pInfo->listSpd) + (pInfo->sampleCount), pVehicleSpeed, sizeof(VEHICLESENS_VEHICLE_SPEED_DAT));

    VEHICLESENS_VEHICLE_SPEED_DAT* pS0 = &(pInfo->listSpd[0]);
    VEHICLESENS_VEHICLE_SPEED_DAT* pS = &(pInfo->listSpd[pInfo->sampleCount]);
    uint32_t ts_i; /* Interger Part of timestamp [s] */
    if (pS->ts.tv_nsec - pS0->ts.tv_nsec >= 0) {  // LCOV_EXCL_BR_LINE 200: can not less than zero
      ts_i = (pS->ts.tv_sec - pS0->ts.tv_sec) % VEHICLESENS_NMEA_PASCD_TS_MAX;
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ts_i = (pS->ts.tv_sec - pS0->ts.tv_sec - 1) % VEHICLESENS_NMEA_PASCD_TS_MAX;  // LCOV_EXCL_LINE 8: invalid
    }
    if (ts_i == 0) { /* Offset is 0 to 1 second */
        pInfo->sampleCount++;
    }

  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                  "WARNING: Num of stored datas is over 50. (%ld.%ld spd = %d)",
                  pVehicleSpeed->ts.tv_sec, pVehicleSpeed->ts.tv_nsec, pVehicleSpeed->speed );
  }

  return;
}

/**
 * @brief
 *  Load Vehicle Speed Information (Vehicle Speed Data x 50(max))
 *
 * @details  This is for creating PASCD Sentence of NMEA. <br>
 *           You can load Vehicle Speed Informations from the last initialization.
 *
 * @param[in]   VEHICLESENS_VEHICLE_SPEED_INFO* pVehicleSpeedInfo : Speeds [m/s] and TimeSpecs
 */
static void VehicleSens_LoadVehicleSpeed(VEHICLESENS_VEHICLE_SPEED_INFO* pVehicleSpeedInfo) {
  (void)memcpy(pVehicleSpeedInfo, &g_vehicle_speed_info, sizeof(g_vehicle_speed_info));
  return;
}

/**
 * @brief
 *  Concatenate NMEA Sentence Fields with Delimiter
 *
 * @details  This is for creating NMEA Sentence. <br>
 *           You can concatenate the two strings given as arguments 'str1' and 'str2'. <br>
 *           And at the time, it puts the delimiter between 'str1' and 'str2' automaticaly.
 *
 * @param[in]   char* str1         : destination
 * @param[in]   const size_t size  : buffer size of destination
 * @param[in]   const char* str2   : source
 * @param[in]   const size_t n     : copy size of source
 *
 * @return      RET_NORMAL         : success
 * @return      RET_ERROR          : failed
 */
static RET_API VehicleSens_CatNmeaSentenceFieldWithDelimiter(char* str1, const size_t size,
                                                             const char* str2, const size_t n) {
  RET_API ret_api = RET_NORMAL;

  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);
  size_t len3 = strlen(VEHICLESENS_NMEA_FIELDDELIMITER);
  size_t sn = n;

  if (sn > len2) {  // LCOV_EXCL_BR_LINE 200: can not exceed size
    sn = len2;
  }

  if (len1 + len3 + sn <= size - 1) {
    (void)strncat(str1, VEHICLESENS_NMEA_FIELDDELIMITER, len3); /* Add Delimiter (,) */
    (void)strncat(str1, str2, sn);
  } else {
    ret_api = RET_ERROR;

    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "ERROR: Buffer size is too small to connatenate. len1:%d len2:%d len3:%d n:%d", len1, len2, len3, n);
  }

  return ret_api;
}

/**
 * @brief
 *  Concatenate NMEA Sentence Fields without Delimiter
 *
 * @details This is for creating NMEA Sentence. <br>
 *          You can concatenate the two strings given as arguments 'str1' and 'str2'.
 *
 * @param[in]   char* str1          : destination
 * @param[in]   const size_t size   : buffer size of destination
 * @param[in]   const char* str2    : source
 * @param[in]   const size_t n      : copy size of source
 *
 * @return      RET_NORMAL         : success
 * @return      RET_ERROR          : failed
 */
static RET_API VehicleSens_CatNmeaSentenceFieldWithoutDelimiter(char* str1, const size_t size,
                                                                const char* str2, const size_t n) {
  RET_API ret_api = RET_NORMAL;

  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);
  size_t sn = n;

  if (sn > len2) {  // LCOV_EXCL_BR_LINE 200: can not exceed  size
    sn = len2;
  }

  if (len1 + sn <= size - 1) {
    (void)strncat(str1, str2, sn);
  } else {
    ret_api = RET_ERROR;

    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "ERROR: Buffer size is too small to connatenate. len1:%d len2:%d n:%d", len1, len2, n);
  }

  return ret_api;
}

/**
 * @brief
 *  Generate ID Field of PASCD Sentence
 *
 * @details  This is for creating PASCD Sentence of NMEA. <br>
 *           You can generate ID Field of PASCD Sentence and <br>
 *           concatenate it with the string given as argument 'pascd'. <br>
 *           It should be called in the specific order.
 *
 * @param[in/out]   char* pascd : buffer pointer for PASCD Sentence
 * @param[in]       size_t size : buffer size
 *
 * @return      RET_NORMAL         : success
 * @return      RET_ERROR          : failed
 */
static inline RET_API VehicleSens_GeneratePASCDFieldId(char* pascd, size_t size) {
  RET_API ret_api;

  ret_api = VehicleSens_CatNmeaSentenceFieldWithoutDelimiter(pascd, size, VEHICLESENS_NMEA_PASCD_ID, size);
  if (ret_api == RET_ERROR) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "ERROR: VehicleSens_CatNmeaSentenceFieldWithoutDelimiter:%d", ret_api);
  }

  return ret_api;
}

/**
 * @brief
 *  Generate Timestamp Field of PASCD Sentence
 *
 * @details  This is for creating PASCD Sentence of NMEA. <br>
 *           You can generate Timestamp Field of PASCD Sentence and <br>
 *           concatenate it with the string given as argument 'pascd'. <br>
 *           It should be called in the specific order.
 *
 * @param[in/out]   char* pascd : buffer pointer for PASCD Sentence
 * @param[in]       size_t size : buffer size
 *
 * @return      RET_NORMAL         : success
 * @return      RET_ERROR          : failed
 */
static inline RET_API VehicleSens_GeneratePASCDFieldTimestamp(char* pascd, size_t size) {
  RET_API ret_api;

  VEHICLESENS_VEHICLE_SPEED_INFO stVehicleSpeedInfo;

  uint32_t ts_i; /* Interger Part of timestamp [s] */
  uint32_t ts_f; /* Fractional Part of timestamp [ms] */
  char ts_ci[32]; /* ts_i in charactor */
  char ts_cf[32]; /* ts_f in charactor */

  VehicleSens_LoadVehicleSpeed(&stVehicleSpeedInfo);


#if 1 /* PASCD *//* Suppose that GPS NMEA data are updated 1 sec interval */
  ts_i = gPseudoSecClockCounter; /* Synchronize: GPS NMEA */
  ts_f = 0u;
  gPseudoSecClockCounter += 1u; /* PASCD *//* Suppose that GPS NMEA data are updated 1 sec interval */
  if (gPseudoSecClockCounter >= VEHICLESENS_NMEA_PASCD_TS_MAX) {
    gPseudoSecClockCounter = 0u;
  }
#else /* PASCD *//* Suppose that GPS NMEA data are updated 1 sec interval */

  ts_i = stVehicleSpeedInfo.listSpd[0].ts.tv_sec % VEHICLESENS_NMEA_PASCD_TS_MAX;
  ts_f = stVehicleSpeedInfo.listSpd[0].ts.tv_nsec;

#endif /* PASCD *//* Suppose that GPS NMEA data are updated 1 sec interval */

  (void)snprintf(ts_ci, sizeof(ts_ci), "%d", ts_i);
  (void)snprintf(ts_cf, sizeof(ts_cf), "%06d", ts_f);

  /* Integer Part */
  ret_api = VehicleSens_CatNmeaSentenceFieldWithDelimiter(pascd, size, ts_ci, VEHICLESENS_NMEA_PASCD_TS_INT_LEN_MAX);
  if (ret_api == RET_ERROR) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "ERROR: VehicleSens_CatNmeaSentenceFieldWithoutDelimiter:%d", ret_api);
  }

  /* Decimal Point */
  ret_api = VehicleSens_CatNmeaSentenceFieldWithoutDelimiter(pascd, size, VEHICLESENS_NMEA_DECIMALPOINT, size);
  if (ret_api == RET_ERROR) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "ERROR: VehicleSens_CatNmeaSentenceFieldWithoutDelimiter:%d", ret_api);
  }

  /* Fractional Part */
  ret_api = VehicleSens_CatNmeaSentenceFieldWithoutDelimiter(pascd, size,
                                                             ts_cf, VEHICLESENS_NMEA_PASCD_TS_FRA_LEN_MAX);
  if (ret_api == RET_ERROR) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "ERROR: VehicleSens_CatNmeaSentenceFieldWithDelimiter:%d", ret_api);
  }

  return ret_api;
}

/**
 * @brief
 *  Generate SensorType Field of PASCD Sentence
 *
 * @details  This is for creating PASCD Sentence of NMEA. <br>
 *           You can generate SensorType Field of PASCD Sentence and <br>
 *           concatenate it with the string given as argument 'pascd'. <br>
 *           It should be called in the specific order.
 *
 * @param[in/out]   char* pascd : buffer pointer for PASCD Sentence
 * @param[in]       size_t size : buffer size
 *
 * @return      RET_NORMAL         : success
 * @return      RET_ERROR          : failed
 */
static inline RET_API VehicleSens_GeneratePASCDFieldSensorType(char* pascd, size_t size) {
  RET_API ret_api;

  ret_api = VehicleSens_CatNmeaSentenceFieldWithDelimiter(pascd, size, VEHICLESENS_NMEA_PASCD_SENSORTYPE_C, size);
  if (ret_api == RET_ERROR) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "ERROR: VehicleSens_CatNmeaSentenceFieldWithDelimiter:%d", ret_api);
  }

  return ret_api;
}

/**
 * @brief
 *  Generate TransmissionState Field of PASCD Sentence
 *
 * @details  This is for creating PASCD Sentence of NMEA. <br>
 *           You can generate TransmissionState Field of PASCD Sentence and <br>
 *           concatenate it with the string given as argument 'pascd'. <br>
 *           It should be called in the specific order.
 *
 * @param[in/out]   char* pascd : buffer pointer for PASCD Sentence
 * @param[in]       size_t size : buffer size
 *
 * @return      RET_NORMAL         : success
 * @return      RET_ERROR          : failed
 */
static inline RET_API VehicleSens_GeneratePASCDFieldTransmissionState(char* pascd, size_t size) {
  RET_API ret_api = RET_NORMAL;
  EFrameworkunifiedStatus eStatus;

  uint8_t ucType; /* type of transmission */
  uint8_t ucShift;

#if 1 /* For Plus _CWORD27_ Gear Data Support 180115 */
  uint8_t ucPkb; /* state of parking brake */
#endif /* For Plus _CWORD27_ Gear Data Support 180115 */

  BOOL bIsAvailable;

  /* Get Type of Transmission */

//  eStatus = VehicleIf_GetTypeOfTransmission(&ucType, &bIsAvailable);
  eStatus = VehicleIf_GetTypeOfTransmission(&ucType, &ucPkb, &bIsAvailable);
  if ((bIsAvailable != true) || (eStatus != eFrameworkunifiedStatusOK)) {
    if (bIsAvailable == TRUE) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
      "ERROR: VehicleIf_GetTypeOfTransmission:%d", eStatus);
    }
    ret_api = RET_ERROR;
  }

  if (ret_api != RET_ERROR) {
    /* Get Shift Position */
    eStatus = VehicleIf_GetShiftPosition(&ucShift, &bIsAvailable);
    if ((bIsAvailable != true) || (eStatus != eFrameworkunifiedStatusOK)) {
      if (bIsAvailable == TRUE) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
        "ERROR: VehicleIf_GetShiftPosition:%d", eStatus);
      }
      ret_api = RET_ERROR;
    }
  }

  if (ret_api != RET_ERROR) {
    VEHICLESENS_TRANSMISSION_PKG tsmPkg;

    tsmPkg.type = ucType;
    tsmPkg.shift = ucShift;

#if 1 /* For Plus _CWORD27_ Gear Data Support 180115 */
    tsmPkg.pkb = ucPkb;
#endif /* For Plus _CWORD27_ Gear Data Support 180115 */


    ret_api = VehicleSens_DeriveTransmissionStateFor_CWORD27_(&tsmPkg);
    if (ret_api != RET_NORMAL) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
      "ERROR: VehicleSensDeriveTransmissionStateFor_CWORD27_:%d", ret_api);
    } else {
      ret_api = VehicleSens_CatNmeaSentenceFieldWithDelimiter(pascd, size, tsmPkg.state, size);
      if (ret_api == RET_ERROR) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
        "ERROR: VehicleSens_CatNmeaSentenceFieldWithDelimiter:%d", ret_api);
      }
    }
  }

  return ret_api;
}

/**
 * @brief
 *  Generate SlipDetect Field of PASCD Sentence
 *
 * @details  This is for creating PASCD Sentence of NMEA. <br>
 *           You can generate SlipDetect Field of PASCD Sentence and <br>
 *           concatenate it with the string given as argument 'pascd'. <br>
 *           It should be called in the specific order.
 *
 * @param[in/out]   char* pascd : buffer pointer for PASCD Sentence
 * @param[in]       size_t size : buffer size
 *
 * @return      RET_NORMAL         : success
 * @return      RET_ERROR          : failed
 */
static inline RET_API VehicleSens_GeneratePASCDFieldSlipDetect(char* pascd, size_t size) {
  RET_API ret_api;

  ret_api = VehicleSens_CatNmeaSentenceFieldWithDelimiter(pascd, size, VEHICLESNES_NMEA_PASCD_NOSLIP, size);
  if (ret_api == RET_ERROR) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "ERROR: VehicleSens_CatNmeaSentenceFieldWithDelimiter:%d", ret_api);
  }

  return ret_api;
}

/**
 * @brief
 *  Generate SampleCount Field of PASCD Sentence
 *
 * @details  This is for creating PASCD Sentence of NMEA. <br>
 *           You can generate SampleCount Field of PASCD Sentence and <br>
 *           concatenate it with the string given as argument 'pascd'. <br>
 *           It should be called in the specific order.
 *
 * @param[in/out]   char* pascd : buffer pointer for PASCD Sentence
 * @param[in]       size_t size : buffer size
 */
static inline RET_API VehicleSens_GeneratePASCDFieldSampleCount(char* pascd, size_t size) {
  RET_API ret_api;

  VEHICLESENS_VEHICLE_SPEED_INFO stVehicleSpeedInfo;
  char cSampleCount[32];

  VehicleSens_LoadVehicleSpeed(&stVehicleSpeedInfo);
  (void)snprintf(cSampleCount, sizeof(cSampleCount), "%d", stVehicleSpeedInfo.sampleCount);

  ret_api = VehicleSens_CatNmeaSentenceFieldWithDelimiter(pascd, size, cSampleCount, size);
  if (ret_api == RET_ERROR) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "ERROR: VehicleSens_CatNmeaSentenceFieldWithDelimiter:%d", ret_api);
  }

  return ret_api;
}

/**
 * @brief
 *  Generate TimeOffset and Speed Field of PASCD Sentence
 *
 * @details  This is for creating PASCD Sentence of NMEA. <br>
 *           You can generate TimeOffset and Speed Field of PASCD Sentence and <br>
 *           concatenate it with the string given as argument 'pascd'. <br>
 *           It should be called in the specific order.
 *
 * @param[in/out]   char* pascd : buffer pointer for PASCD Sentence
 * @param[in]       size_t size : buffer size
 *
 * @return      RET_NORMAL         : success
 * @return      RET_ERROR          : failed
 */
static inline RET_API VehicleSens_GeneratePASCDFieldTimeOffsetNSpeed(char* pascd, size_t size) {
  RET_API ret_api = RET_ERROR;

  VEHICLESENS_VEHICLE_SPEED_INFO stVehicleSpeedInfo;
  VEHICLESENS_VEHICLE_SPEED_DAT *pS0 = &(stVehicleSpeedInfo.listSpd[0]);

  int32_t i;

  VehicleSens_LoadVehicleSpeed(&stVehicleSpeedInfo);

  for (i = 0; i < stVehicleSpeedInfo.sampleCount; i++) {
    VEHICLESENS_VEHICLE_SPEED_DAT *pS = &(stVehicleSpeedInfo.listSpd[i]);

    /* timeoffset */
    uint32_t ts_i; /* Interger Part of timestamp [s] */
    uint32_t ts_f; /* Fractional Part of timestamp [us] */
    char ts_ci[32]; /* ts_i in charactor */
    char ts_cf[32]; /* ts_f in charactor */

    if (pS->ts.tv_nsec - pS0->ts.tv_nsec >= 0) {  // LCOV_EXCL_BR_LINE 200: can not less than zero
      ts_i = (pS->ts.tv_sec - pS0->ts.tv_sec) % VEHICLESENS_NMEA_PASCD_TS_MAX;
      ts_f = (pS->ts.tv_nsec - pS0->ts.tv_nsec) / 1000; /* [ns] -> [us] */
    } else {
      // LCOV_EXCL_START 8: invalid
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ts_i = (pS->ts.tv_sec - pS0->ts.tv_sec - 1) % VEHICLESENS_NMEA_PASCD_TS_MAX;
      ts_f = (1000000000 + pS->ts.tv_nsec - pS0->ts.tv_nsec) / 1000; /* [ns] -> [us] */
      // LCOV_EXCL_STOP
    }

    (void)snprintf(ts_ci, sizeof(ts_ci), "%d", ts_i);
    (void)snprintf(ts_cf, sizeof(ts_cf), "%06d", ts_f);

    /* Integer Part */
    ret_api = VehicleSens_CatNmeaSentenceFieldWithDelimiter(pascd, size, ts_ci, size);
    if (ret_api == RET_ERROR) {  // LCOV_EXCL_BR_LINE 200: can not exceed buffer size
    // LCOV_EXCL_START 8: invalid
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
      "ERROR: VehicleSens_CatNmeaSentenceFieldWithDelimiter:%d", ret_api);
      break;
    // LCOV_EXCL_STOP
    }

    /* Decimal Point */
    ret_api = VehicleSens_CatNmeaSentenceFieldWithoutDelimiter(pascd, size, VEHICLESENS_NMEA_DECIMALPOINT, size);
    if (ret_api == RET_ERROR) {  // LCOV_EXCL_BR_LINE 200: can not exceed buffer size
    // LCOV_EXCL_START 8: invalid
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
      "ERROR: VehicleSens_CatNmeaSentenceFieldWithoutDelimiter:%d", ret_api);
      break;
    // LCOV_EXCL_STOP
    }

    /* Fractional Part */
    ret_api = VehicleSens_CatNmeaSentenceFieldWithoutDelimiter(pascd, size,
                                                               ts_cf, VEHICLESENS_NMEA_PASCD_TO_FRA_LEN_MAX);
    if (ret_api == RET_ERROR) {  // LCOV_EXCL_BR_LINE 200: can not exceed buffer size
    // LCOV_EXCL_START 8: invalid
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
      "ERROR: VehicleSens_CatNmeaSentenceFieldWithoutDelimiter:%d", ret_api);
      break;
    // LCOV_EXCL_STOP
    }

    /* speed */
    uint16_t spd_i; /* Interger Part of speed [m/s] */
    uint16_t spd_f; /* Fractional Part of speed [mm/s] */
    char spd_ci[32]; /* spd_i in charactor */
    char spd_cf[32]; /* spd_f in charactor */

    spd_i = pS->speed / 100; /* [0.01m/s] -> [m/s] */
    spd_f = (pS->speed % 100) * 10; /* [0.01m/s] -> [mm/s] */

    (void)snprintf(spd_ci, sizeof(spd_ci), "%d", spd_i);
    (void)snprintf(spd_cf, sizeof(spd_cf), "%03d", spd_f);

    /* Integer Part */
    ret_api = VehicleSens_CatNmeaSentenceFieldWithDelimiter(pascd, size, spd_ci, size);
    if (ret_api == RET_ERROR) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
      "ERROR: VehicleSens_CatNmeaSentenceFieldWithDelimiter:%d", ret_api);
      break;
    }

    /* Decimal Point */
    ret_api = VehicleSens_CatNmeaSentenceFieldWithoutDelimiter(pascd, size, VEHICLESENS_NMEA_DECIMALPOINT, size);
    if (ret_api == RET_ERROR) {  // LCOV_EXCL_BR_LINE 200: can not exceed buffer size
    // LCOV_EXCL_START 8: invalid
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
      "ERROR: VehicleSens_CatNmeaSentenceFieldWithoutDelimiter:%d", ret_api);
      break;
    // LCOV_EXCL_STOP
    }

    /* Fractional Part */
    ret_api = VehicleSens_CatNmeaSentenceFieldWithoutDelimiter(pascd, size,
                                                               spd_cf, VEHICLESENS_NMEA_PASCD_SPD_FRA_LEN_MAX);
    if (ret_api == RET_ERROR) {  // LCOV_EXCL_BR_LINE 200: can not exceed buffer size
    // LCOV_EXCL_START 8: invalid
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
      "ERROR: VehicleSens_CatNmeaSentenceFieldWithoutDelimiter:%d", ret_api);
      break;
    // LCOV_EXCL_STOP
    }
  }

  return ret_api;
}

/**
 * @brief
 *  Generate Checksum Field of PASCD Sentence
 *
 * @details  This is for creating PASCD Sentence of NMEA. <br>
 *           You can generate Checksum Field of PASCD Sentence and <br>
 *           concatenate it with the string given as argument 'pascd'. <br>
 *           It should be called in the specific order.
 *
 * @param[in/out]   char* pascd : buffer pointer for PASCD Sentence
 * @param[in]       size_t size : buffer size
 *
 * @return      RET_NORMAL         : success
 * @return      RET_ERROR          : failed
 */
static inline RET_API VehicleSens_GeneratePASCDFieldChecksum(char* pascd, size_t size) {
  RET_API ret_api;

  size_t length;
  uint8_t chk = 0;
  char cChk[3];
  uint16_t i = 0;

  length = strnlen(pascd, size);

  /* Calculate Checksum (start with the 2th Bype except '$') */
  for (i = 1; i < length; i++) {
    chk ^= pascd[i];
  }
  (void)snprintf(cChk, sizeof(cChk), "%02X", chk);

  /* Set Astarisk before Checksum */
  ret_api = VehicleSens_CatNmeaSentenceFieldWithoutDelimiter(pascd, size, VEHICLESENS_NMEA_ASTARISK, size);
  if (ret_api == RET_ERROR) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "ERROR: VehicleSens_CatNmeaSentenceFieldWithoutDelimiter:%d", ret_api);
  }

  /* Set Checksum */
  ret_api = VehicleSens_CatNmeaSentenceFieldWithoutDelimiter(pascd, size, cChk, size);
  if (ret_api == RET_ERROR) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "ERROR: VehicleSens_CatNmeaSentenceFieldWithDelimiter:%d", ret_api);
  }

  return ret_api;
}

/**
 * @brief
 *  Generate CR & LF Field of PASCD Sentence
 *
 * @details  This is for creating PASCD Sentence of NMEA. <br>
 *           You can generate CRLF Field of PASCD Sentence and <br>
 *           concatenate it with the string given as argument 'pascd'. <br>
 *           It should be called in the specific order.
 *
 * @param[in/out]   char* pascd : buffer pointer for PASCD Sentence
 * @param[in]       size_t size : buffer size
 *
 * @return      RET_NORMAL         : success
 * @return      RET_ERROR          : failed
 */
static inline RET_API VehicleSens_GeneratePASCDFieldCRLF(char* pascd, size_t size) {
  RET_API ret_api;

  /* Set Carriage Return */
  ret_api = VehicleSens_CatNmeaSentenceFieldWithoutDelimiter(pascd, size, VEHICLESENS_NMEA_CR, size);
  if (ret_api == RET_ERROR) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "ERROR: VehicleSens_CatNmeaSentenceFieldWithDelimiter:%d", ret_api);
  }

  /* Set Line Feed */
  ret_api = VehicleSens_CatNmeaSentenceFieldWithoutDelimiter(pascd, size, VEHICLESENS_NMEA_LF, size);
  if (ret_api == RET_ERROR) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "ERROR: VehicleSens_CatNmeaSentenceFieldWithDelimiter:%d", ret_api);
  }

  return ret_api;
}

/**
 * @brief
 *  Derive Transmission State For _CWORD27_
 *
 * @details  This is for creating PASCD Sentence of NMEA. <br>
 *           You can derive transmissionState from the transmission type and <br>
 *           the shift positiong from Vehicle Service.
 *
 * @param[in/out] VEHICLESENS_TRANSMISSION_PKG* pPkg : source data set for Transmission State
 *
 * @return      RET_NORMAL         : success
 * @return      RET_ERROR          : failed
 */
static RET_API VehicleSens_DeriveTransmissionStateFor_CWORD27_(VEHICLESENS_TRANSMISSION_PKG* pPkg) {
  RET_API ret_api = RET_NORMAL;
  uint32_t i;

  static const VEHICLESENS_TRANSMISSION_PKG TmsLut[VEHICLEIF_TRANSMISSION_TYPE_NUM * VEHICLEIF_SHIFT_POSITION_NUM] = {

//    /* Transmission Type : MT */
//    { VEHICLEIF_TRANSMISSION_TYPE_MT,      VEHICLEIF_SHIFT_POSITION_U, VEHICLESENS_NMEA_PASCD_TMS_D },
//    { VEHICLEIF_TRANSMISSION_TYPE_MT,      VEHICLEIF_SHIFT_POSITION_R, VEHICLESENS_NMEA_PASCD_TMS_R },
//    { VEHICLEIF_TRANSMISSION_TYPE_MT,      VEHICLEIF_SHIFT_POSITION_P, VEHICLESENS_NMEA_PASCD_TMS_D },
//    { VEHICLEIF_TRANSMISSION_TYPE_MT,      VEHICLEIF_SHIFT_POSITION_N, VEHICLESENS_NMEA_PASCD_TMS_D },
//    { VEHICLEIF_TRANSMISSION_TYPE_MT,      VEHICLEIF_SHIFT_POSITION_D, VEHICLESENS_NMEA_PASCD_TMS_D },
//    /* Tranmission Type : AT */
//    { VEHICLEIF_TRANSMISSION_TYPE_AT,      VEHICLEIF_SHIFT_POSITION_U, VEHICLESENS_NMEA_PASCD_TMS_U },
//    { VEHICLEIF_TRANSMISSION_TYPE_AT,      VEHICLEIF_SHIFT_POSITION_R, VEHICLESENS_NMEA_PASCD_TMS_R },
//    { VEHICLEIF_TRANSMISSION_TYPE_AT,      VEHICLEIF_SHIFT_POSITION_P, VEHICLESENS_NMEA_PASCD_TMS_P },
//    { VEHICLEIF_TRANSMISSION_TYPE_AT,      VEHICLEIF_SHIFT_POSITION_N, VEHICLESENS_NMEA_PASCD_TMS_U },
//    { VEHICLEIF_TRANSMISSION_TYPE_AT,      VEHICLEIF_SHIFT_POSITION_D, VEHICLESENS_NMEA_PASCD_TMS_D },
//    /* Transmission Type : UNKNOWN */
//    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_U, VEHICLESENS_NMEA_PASCD_TMS_U },
//    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_R, VEHICLESENS_NMEA_PASCD_TMS_U },
//    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_P, VEHICLESENS_NMEA_PASCD_TMS_U },
//    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_N, VEHICLESENS_NMEA_PASCD_TMS_U },
//    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_D, VEHICLESENS_NMEA_PASCD_TMS_U },
    /* Transmission Type : MT */
    { VEHICLEIF_TRANSMISSION_TYPE_MT,      VEHICLEIF_SHIFT_POSITION_U, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_D },
    { VEHICLEIF_TRANSMISSION_TYPE_MT,      VEHICLEIF_SHIFT_POSITION_R, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_R },
    { VEHICLEIF_TRANSMISSION_TYPE_MT,      VEHICLEIF_SHIFT_POSITION_P, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_D },
    { VEHICLEIF_TRANSMISSION_TYPE_MT,      VEHICLEIF_SHIFT_POSITION_N, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_D },
    { VEHICLEIF_TRANSMISSION_TYPE_MT,      VEHICLEIF_SHIFT_POSITION_D, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_D },
    /* Tranmission Type : AT */
#if 1 /* For Plus _CWORD27_ Gear Data Support 180115 */
    { VEHICLEIF_TRANSMISSION_TYPE_AT,      VEHICLEIF_SHIFT_POSITION_U, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_U },
#else /* For Plus _CWORD27_ Gear Data Support 180115 */
    { VEHICLEIF_TRANSMISSION_TYPE_AT,      VEHICLEIF_SHIFT_POSITION_U, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_D },
#endif /* For Plus _CWORD27_ Gear Data Support 180115 */
    { VEHICLEIF_TRANSMISSION_TYPE_AT,      VEHICLEIF_SHIFT_POSITION_R, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_R },
    { VEHICLEIF_TRANSMISSION_TYPE_AT,      VEHICLEIF_SHIFT_POSITION_P, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_P },
    { VEHICLEIF_TRANSMISSION_TYPE_AT,      VEHICLEIF_SHIFT_POSITION_N, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_N }, /* However, the Vehicle does not notify you by Phase3. */
    { VEHICLEIF_TRANSMISSION_TYPE_AT,      VEHICLEIF_SHIFT_POSITION_D, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_D },
    /* Transmission Type : UNKNOWN */
#if 1 /* For Plus _CWORD27_ Gear Data Support 180115 */
    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_U, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_U },
    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_R, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_U },
    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_P, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_U },
    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_N, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_U },
    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_D, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_U },
#else /* For Plus _CWORD27_ Gear Data Support 180115 */
    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_U, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_D },
    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_R, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_D },
    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_P, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_D },
    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_N, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_D },
    { VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN, VEHICLEIF_SHIFT_POSITION_D, VEHICLEIF_PKB_UNKNOWN, VEHICLESENS_NMEA_PASCD_TMS_D },
#endif /* For Plus _CWORD27_ Gear Data Support 180115 */

  };

  for (i = 0; i < VEHICLEIF_TRANSMISSION_TYPE_NUM * VEHICLEIF_SHIFT_POSITION_NUM; i++) {
    if ((pPkg->type == TmsLut[i].type) && (pPkg->shift == TmsLut[i].shift)) {
      strncpy(pPkg->state, TmsLut[i].state, sizeof(pPkg->state));
      break;
    }
  }


#if 1 /* For Plus _CWORD27_ Gear Data Support 180115 */
  if ((pPkg->type == VEHICLEIF_TRANSMISSION_TYPE_MT) && (pPkg->pkb == VEHICLEIF_PKB_ON)) {
    strncpy(pPkg->state, VEHICLESENS_NMEA_PASCD_TMS_P, sizeof(VEHICLESENS_NMEA_PASCD_TMS_P));
  }
#endif /* For Plus _CWORD27_ Gear Data Support 180115 */


  if (i == VEHICLEIF_TRANSMISSION_TYPE_NUM * VEHICLEIF_SHIFT_POSITION_NUM) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
    "FORBIDDEN ERROR: Can't find Transmission State. type:%d shift:%d", pPkg->type, pPkg->shift);
    ret_api = RET_ERROR;
  }

  return ret_api;
}
