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
 *  File name  :DeadReckoning_Main.cpp
 *  System name  :_CWORD107_
 *  Subsystem name :DeadReckoning Mains
 *  Program name :DeadReckoning Mains
 *  Module configuration :DeadReckoningInit()   Guessed navigation initialization processing
 *                  :DeadReckoningRcvMsg()  DR Component MSG Receive Processing
 ******************************************************************************/

//#include <positioning_hal.h>

#include "DeadReckoning_main.h"

#include "Sensor_Common_API.h"
#include "DeadReckoning_DataMaster.h"
#include "Dead_Reckoning_Local_Api.h"

#include "DeadReckoning_DbgLogSim.h"

#include "POS_private.h"

static RET_API DeadReckoningWriteSharedMemory(VEHICLEDEBUG_MSG_LOGINFO_DAT* loginfo);
static void DeadReckoningSetEvent(PNO pno, RET_API ret);
static void DeadReckoningLinkSharedMemory(char *shared_memory_name, void **share_addr);

#define DEAD_RECKONING_MAIN_DEBUG 0
#define DR_DEBUG 0
#define DR_DEBUG_ENG_MODE 0

/*************************************************/
/*                      Constant                     */
/*************************************************/

#define DEAD_RECKONING_BUF_SIZE 5
#define DR_MASK_WORD_L 0x00FF
#define DR_MASK_WORD_U 0xFF00

/*************************************************/
/*                Global variable                 */
/*************************************************/

/* Data receive confirmation flag */
BOOL g_gps_data_get_flg = FALSE;
BOOL g_sens_data_get_flg = FALSE;
BOOL g_fst_sens_data_get_flg = FALSE;

/* Reception flag for each data */
BOOL g_sens_data_get_sns_cnt_flg    = FALSE;
BOOL g_sens_data_get_gyro_flg     = FALSE;
BOOL g_sens_data_get_rev_flg     = FALSE;
BOOL g_sens_data_get_spdpulse_flg    = FALSE;
BOOL g_sens_data_get_spdpulse_chk_flg  = FALSE;

BOOL g_sens_data_get_gyro_fst_flg    = FALSE;
BOOL g_sens_data_get_rev_fst_flg    = FALSE;
BOOL g_sens_data_get_spdpulse_fst_flg   = FALSE;
BOOL g_sens_data_get_spdpulse_chk_fst_flg  = FALSE;

/* Receive data storage buffer */
/* [0]:SNS_COUNTER [1]:REV [2]:SPEED_PULSE_FLAG [3]:SPEED_PULSE [4]:GYRO */
DEAD_RECKONING_RCVDATA_SENSOR   g_sns_buf[DEAD_RECKONING_BUF_SIZE];
DEAD_RECKONING_SAVEDATA_SENSOR_FIRST g_fst_sns_buf;

/*******************************************************************************
 * MODULE    : DeadReckoningInit
 * ABSTRACT  : Guessed navigation initialization processing
 * FUNCTION  : Initialize inferred navigation processing
 * ARGUMENT  : None
 * NOTE      :
 * RETURN    : RET_NORMAL :Success in initialization
 *      RET_ERROR :Master Clear failed
 ******************************************************************************/
int32 DeadReckoningInit(void) {  // LCOV_EXCL_START 8: dead code.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RET_NORMAL;
}

/*******************************************************************************
 * MODULE    : DeadReckoningRcvMsg
 * ABSTRACT  : DR Component MSG Receive Processing
 * FUNCTION  : Receive the DR component MSG
 * ARGUMENT  : *msg  : message buffer
 * NOTE      :
 * RETURN    : None
 ******************************************************************************/
void DeadReckoningRcvMsg(const DEAD_RECKONING_RCVDATA *msg, DEAD_RECKONING_LONLAT_INFO *location_info) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//    if ((msg == NULL) || (location_info == NULL)) {
//        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "msg == NULL\r\n");
//    } else {
//        const SENSOR_MSG_GPSDATA_DAT   *rcv_gps_msg   = NULL;
//        const VEHICLESENS_DATA_MASTER   *rcv_sensor_msg  = NULL;
//        const VEHICLESENS_DATA_MASTER_FST  *rcv_sensor_msg_fst = NULL;
//
//        Struct_GpsData      send_gps_msg;
//        Struct_SensData      send_sensor_msg;
//
//        /* Initialization */
//        (void)memset(reinterpret_cast<void *>(&send_gps_msg),  0, sizeof(Struct_GpsData));
//        (void)memset(reinterpret_cast<void *>(&send_sensor_msg), 0, sizeof(Struct_SensData));
//
//        /* Flag is set to FALSE */
//        location_info->calc_called = static_cast<u_int8>(FALSE);
//        location_info->available = static_cast<u_int8>(FALSE);
//
//        if (CID_DEAD_RECKONING_GPS_DATA == msg->hdr.hdr.cid) {
//            rcv_gps_msg = (const SENSOR_MSG_GPSDATA_DAT *)(&(msg->data[0]));
//            /* #QAC confirmation Rule11.4 Use structure for member reference(Cast according to shared memory link IF) */
//
//            /* Receiving GPS Data for DR */
//            switch (rcv_gps_msg->ul_did) {
//                case  VEHICLE_DID_GPS_UBLOX_NAV_POSLLH :
//                case  VEHICLE_DID_GPS_UBLOX_NAV_STATUS :
//                case  VEHICLE_DID_GPS_UBLOX_NAV_TIMEUTC :
//                case  VEHICLE_DID_GPS_UBLOX_NAV_VELNED :
//                case  VEHICLE_DID_GPS_UBLOX_NAV_DOP :
//                case  VEHICLE_DID_GPS_UBLOX_NAV_SVINFO :
//                case  VEHICLE_DID_GPS_UBLOX_NAV_CLOCK :
//                {
//                    g_gps_data_get_flg = TRUE;
//                    break;
//                }
//                default:
//                    break;
//            }
//        } else if (CID_DEAD_RECKONING_SENS_DATA == msg->hdr.hdr.cid) {
//            rcv_sensor_msg = (const VEHICLESENS_DATA_MASTER *)(&(msg->data[0]));
//            /* #QAC confirmation Rule11.4 Use structure for member reference(Cast according to shared memory link IF) */
//
//            /* Sensor data reception for DR */
//            switch (rcv_sensor_msg->ul_did) {
//                case  POSHAL_DID_SNS_COUNTER :
//                {
//                    g_sns_buf[0].did = rcv_sensor_msg->ul_did;
//                    g_sns_buf[0].size = static_cast<u_int8>(rcv_sensor_msg->us_size);
//                    g_sns_buf[0].data[0] = rcv_sensor_msg->uc_data[0];
//                    g_sens_data_get_sns_cnt_flg = TRUE;
//                    break;
//                }
//                case  POSHAL_DID_REV :
//                {
//                    g_sns_buf[1].did = rcv_sensor_msg->ul_did;
//                    g_sns_buf[1].size = static_cast<u_int8>(rcv_sensor_msg->us_size);
//                    (void)memcpy(reinterpret_cast<void *>(&(g_sns_buf[1].data[0])),
//                                 (const void *)(&(rcv_sensor_msg->uc_data[0])), (size_t)rcv_sensor_msg->us_size);
//                    g_sens_data_get_rev_flg = TRUE;
//                    break;
//                }
//                case  POSHAL_DID_SPEED_PULSE_FLAG :
//                {
//                    g_sns_buf[2].did = rcv_sensor_msg->ul_did;
//                    g_sns_buf[2].size = static_cast<u_int8>(rcv_sensor_msg->us_size);
//                    (void)memcpy(reinterpret_cast<void *>(&(g_sns_buf[2].data[0])),
//                                 (const void *)(&(rcv_sensor_msg->uc_data[0])), (size_t)rcv_sensor_msg->us_size);
//                    g_sens_data_get_spdpulse_chk_flg = TRUE;
//                    break;
//                }
//                case  POSHAL_DID_SPEED_PULSE :
//                {
//                    g_sns_buf[3].did = rcv_sensor_msg->ul_did;
//                    g_sns_buf[3].size = static_cast<u_int8>(rcv_sensor_msg->us_size);
//                    (void)memcpy(reinterpret_cast<void *>(&(g_sns_buf[3].data[0])),
//                                 (const void *)(&(rcv_sensor_msg->uc_data[0])), (size_t)rcv_sensor_msg->us_size);
//                    g_sens_data_get_spdpulse_flg = TRUE;
//                    break;
//                }
//                case  POSHAL_DID_GYRO :
//                {
//                    g_sns_buf[4].did = rcv_sensor_msg->ul_did;
//                    g_sns_buf[4].size = static_cast<u_int8>(rcv_sensor_msg->us_size);
//                    (void)memcpy(reinterpret_cast<void *>(&(g_sns_buf[4].data[0])),
//                                 (const void *)(&(rcv_sensor_msg->uc_data[0])), (size_t)rcv_sensor_msg->us_size);
//                    g_sens_data_get_gyro_flg = TRUE;
//                    break;
//                }
//                default:
//                    break;
//            }
//        } else if (CID_DEAD_RECKONING_SENS_FST_DATA == msg->hdr.hdr.cid) {
//            u_int16 rev_data_size;
//
//            rcv_sensor_msg_fst = (const VEHICLESENS_DATA_MASTER_FST *)(&(msg->data[0]));
//            /* #QAC confirmation Rule11.4 Use structure for member reference(Cast according to shared memory link IF) */
//            rev_data_size = static_cast<u_int16>(msg->hdr.hdr.msgbodysize - VEHICLESENS_DELIVERY_FSTSNS_HDR_SIZE);
//
//#if DEAD_RECKONING_MAIN_DEBUG
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " DID = %08X, rev_data_size = %d ",
//                          rcv_sensor_msg_fst->ul_did, rev_data_size);
//#endif
//
//            /* Sensor data reception for DR */
//            switch (rcv_sensor_msg_fst->ul_did) {
//                case  POSHAL_DID_REV_FST :
//                {
//                    (void)memcpy(
//                        reinterpret_cast<void *>(&(g_fst_sns_buf.rev_data[g_fst_sns_buf.rev_rcv_size
//                                                                         / sizeof(g_fst_sns_buf.rev_data[0])])),
//                                        (const void *)(&(rcv_sensor_msg_fst->uc_data[0])),
//                                        (size_t)(rev_data_size));
//
//                    g_fst_sns_buf.rev_rcv_size = static_cast<u_int16>(
//                        g_fst_sns_buf.rev_rcv_size + rev_data_size);
//
//                    if (g_fst_sns_buf.rev_rcv_size == rcv_sensor_msg_fst->us_size) {
//                        g_sens_data_get_rev_fst_flg = TRUE;
//#if DEAD_RECKONING_MAIN_DEBUG
//                        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " REV receive flag = TRUE ");
//#endif
//                    }
//                    break;
//                }
//                case  POSHAL_DID_SPEED_PULSE_FLAG_FST :
//                {
//                    (void)memcpy(
//                    reinterpret_cast<void *>(&(g_fst_sns_buf.spd_pulse_check_data[g_fst_sns_buf.spd_pulse_check_rcv_size
//                                                                   / sizeof(g_fst_sns_buf.spd_pulse_check_data[0])])),
//                                        (const void *)(&(rcv_sensor_msg_fst->uc_data[0])),
//                                        (size_t)(rev_data_size));
//
//                    g_fst_sns_buf.spd_pulse_check_rcv_size = static_cast<u_int16>(
//                                    g_fst_sns_buf.spd_pulse_check_rcv_size + rev_data_size);
//
//                    if (g_fst_sns_buf.spd_pulse_check_rcv_size == rcv_sensor_msg_fst->us_size) {
//                        g_sens_data_get_spdpulse_chk_fst_flg = TRUE;
//#if DEAD_RECKONING_MAIN_DEBUG
//                        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " SPF receive flag = TRUE ");
//#endif
//                    }
//                    break;
//                }
//                case  POSHAL_DID_SPEED_PULSE_FST :
//                {
//                    (void)memcpy(
//                        reinterpret_cast<void *>(&(g_fst_sns_buf.spd_pulse_data[g_fst_sns_buf.spd_pulse_rcv_size
//                                                                        / sizeof(g_fst_sns_buf.spd_pulse_data[0])])),
//                                        (const void *)(&(rcv_sensor_msg_fst->uc_data[0])),
//                                        (size_t)(rev_data_size));
//
//                    g_fst_sns_buf.spd_pulse_rcv_size = static_cast<u_int16>(g_fst_sns_buf.spd_pulse_rcv_size + \
//                                                                                    rev_data_size);
//
//                    if (g_fst_sns_buf.spd_pulse_rcv_size == rcv_sensor_msg_fst->us_size) {
//                        g_sens_data_get_spdpulse_fst_flg = TRUE;
//#if DEAD_RECKONING_MAIN_DEBUG
//                        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " SP receive flag = TRUE ");
//#endif
//                    }
//                    break;
//                }
//                case  POSHAL_DID_GYRO_FST :
//                {
//                    (void)memcpy(
//                        reinterpret_cast<void *>(&(g_fst_sns_buf.gyro_data[g_fst_sns_buf.gyro_rcv_size
//                                                                          / sizeof(g_fst_sns_buf.gyro_data[0])])),
//                                        (const void *)(&(rcv_sensor_msg_fst->uc_data[0])),
//                                        (size_t)(rev_data_size));
//
//                    g_fst_sns_buf.gyro_rcv_size = static_cast<u_int16>(g_fst_sns_buf.gyro_rcv_size + rev_data_size);
//
//#if DEAD_RECKONING_MAIN_DEBUG
//                    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//                         " g_fst_sns_buf.gyro_rcv_size = %d, rcv_sensor_msg_fst->us_size = %d ",
//                        g_fst_sns_buf.gyro_rcv_size, rcv_sensor_msg_fst->us_size);
//#endif
//                    if (g_fst_sns_buf.gyro_rcv_size == rcv_sensor_msg_fst->us_size) {
//                        g_sens_data_get_gyro_fst_flg = TRUE;
//#if DEAD_RECKONING_MAIN_DEBUG
//                        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " GYRO receive flag = TRUE ");
//#endif
//                    }
//                    break;
//                }
//                default:
//                    break;
//            }
//        } else {
//            /* nop */
//        }
//
//        /* 4 data received? */
//        if ((g_sens_data_get_sns_cnt_flg == TRUE) &&
//                (g_sens_data_get_rev_flg == TRUE) &&
//                (g_sens_data_get_gyro_flg == TRUE) &&
//                (g_sens_data_get_spdpulse_flg == TRUE) &&
//                (g_sens_data_get_spdpulse_chk_flg == TRUE)) {
//            /* Sensor data acquisition flag ON */
//            g_sens_data_get_flg = TRUE;
//
//            /* Set all flags to FALSE */
//            g_sens_data_get_sns_cnt_flg   = FALSE;
//            g_sens_data_get_gyro_flg    = FALSE;
//            g_sens_data_get_rev_flg    = FALSE;
//            g_sens_data_get_spdpulse_flg   = FALSE;
//            g_sens_data_get_spdpulse_chk_flg  = FALSE;
//        }
//
//        /* 4 data received? */
//        if ((g_sens_data_get_rev_fst_flg == TRUE) &&
//                (g_sens_data_get_gyro_fst_flg == TRUE) &&
//                (g_sens_data_get_spdpulse_fst_flg == TRUE) &&
//                (g_sens_data_get_spdpulse_chk_fst_flg == TRUE)) {
//            /* Initial sensor data acquisition flag ON */
//            g_fst_sens_data_get_flg = TRUE;
//
//            /* Set all flags to FALSE */
//            g_sens_data_get_gyro_fst_flg    = FALSE;
//            g_sens_data_get_rev_fst_flg    = FALSE;
//            g_sens_data_get_spdpulse_fst_flg   = FALSE;
//            g_sens_data_get_spdpulse_chk_fst_flg  = FALSE;
//        }
//
//        DeadReckoningRcvMsgFstSens(msg, location_info, rcv_gps_msg, &send_gps_msg, &send_sensor_msg);
//    }
}

//void  DeadReckoningRcvMsgFstSens(const DEAD_RECKONING_RCVDATA *msg, DEAD_RECKONING_LONLAT_INFO *location_info,
//                            const SENSOR_MSG_GPSDATA_DAT  *rcv_gps_msg, Struct_GpsData* send_gps_msg,
//                            Struct_SensData* send_sensor_msg) {
//    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//       /* Verifying Debug Log Simulator Mode
//           Perform packet reading here to match some timing.
//           However,,Differ between GPS and SENSOR.
//        */
//    u_int8 fst_sens_send_num = 0U;
//    /* For GPS data */
//    if (g_gps_data_get_flg == TRUE) {
//#if (DR_DEBUG == 1) /* PastModel002_DR_VEHICLE */
//        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//             "DeadReckoningRcvMsg SEND GPS_DATA: DID[0x%08X] DSIZE[%d] MSG_SIZE[%d] SNS_CNT[%d] \r\n",
//            rcv_gps_msg.ulDid, rcv_gps_msg.ucData[4], msg->hdr.hdr.msgbodysize, rcv_gps_msg.ucSnsCnt);
//#endif
//
//        if (1) {
//            rcv_gps_msg = (const SENSOR_MSG_GPSDATA_DAT *)(&(msg->data[0]));
//            /* #QAC confirmation Rule11.4 Use structure for member reference(Cast according to shared memory link IF) */
//
//            /* Set and pass the data part except header/footer */
//            send_gps_msg->sens_cnt   = rcv_gps_msg->uc_sns_cnt;
//            send_gps_msg->sens_cnt_flag = rcv_gps_msg->uc_gps_cnt_flag;
//            send_gps_msg->gps_data_size = rcv_gps_msg->us_size;
//            send_gps_msg->did    = rcv_gps_msg->ul_did;
//            send_gps_msg->gps_data   = (const void *)(&rcv_gps_msg->uc_data[0]);
//        }
//
//        /* Providing GPS data to DR_Lib */
//
//        g_gps_data_get_flg = FALSE;
//
//    } else if (g_sens_data_get_flg == TRUE) {
//        Struct_DR_DATA      rcv_dr_data;
//        DR_CALC_INFO      rcv_dr_calc_info;
//        DEADRECKONING_DATA_MASTER   send_data_master;
//
//        if (1) {
//            /* Each data is stored in the data format for transmission. */
//            send_sensor_msg->sens_cnt_flag = 1U;
//            send_sensor_msg->sens_cnt = g_sns_buf[0].data[0];
//            send_sensor_msg->pulse_rev_tbl.reverse_flag = g_sns_buf[1].data[0];
//            send_sensor_msg->pulse_rev_tbl.pulse_flag = g_sns_buf[2].data[0];
//            send_sensor_msg->pulse_rev_tbl.pulse_sum_cyclic = static_cast<u_int16>(
//                    ((((u_int16)g_sns_buf[3].data[0] << 0) & DR_MASK_WORD_L) |
//                    (((u_int16)g_sns_buf[3].data[1] << 8) & DR_MASK_WORD_U)));
//            /* ToDo */
//            (void)memcpy(reinterpret_cast<void *>(&(send_sensor_msg->gyro_tbl.gyro_data[0])),
//                         (const void *)(&(g_sns_buf[4].data[0])), sizeof(u_int16) * NUM_OF_100msData);
//        }
//
//#if (DR_DEBUG == 1) /* PastModel002_DR_VEHICLE */
//        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningRcvMsg SEND SENSOR_COUNTER %d \r\n",
//                       send_sensor_msg->sens_cnt);
//#endif
//
//        /* When the sensor data are ready, Call the DR-calculation process. */
//        memset(&rcv_dr_data, 0x00, sizeof(rcv_dr_data));   /* Coverity CID:18805 compliant */
//        memset(&rcv_dr_calc_info, 0x00, sizeof(rcv_dr_calc_info)); /* Coverity CID:18806 compliant */
//
//        location_info->calc_called = static_cast<u_int8>(TRUE);
//        location_info->lonlat.latitude = static_cast<int32>(rcv_dr_data.latitude);
//        location_info->lonlat.longitude = static_cast<int32>(rcv_dr_data.longitude);
//
//        if (rcv_dr_data.dr_status != static_cast<u_int8>(SENSORLOCATION_DRSTATUS_INVALID)) {
//            location_info->available = static_cast<u_int8>(TRUE);
//        } else {
//            location_info->available = static_cast<u_int8>(FALSE);
//        }
//
//        /* Changing the order of registration and delivery of the out structure to the data master for DR */
//
//        send_data_master.ul_did = VEHICLE_DID_DR_SNS_COUNTER;
//        send_data_master.us_size = VEHICLE_DSIZE_DR_SNS_COUNTER;
//        send_data_master.uc_rcv_flag = 1;
//        send_data_master.dr_status = rcv_dr_data.dr_status;
//        (void)memcpy(reinterpret_cast<void *>(&(send_data_master.uc_data[0])),
//                     (const void *)(&(rcv_dr_data.positioning_time)), (size_t)VEHICLE_DSIZE_DR_SNS_COUNTER);
//        DeadReckoningSetDataMaster(&send_data_master, (PFUNC_DR_DMASTER_SET_N)DeadReckoningDeliveryProc);
//
//        send_data_master.ul_did = VEHICLE_DID_DR_LONGITUDE;
//        send_data_master.us_size = VEHICLE_DSIZE_LONGITUDE;
//        send_data_master.uc_rcv_flag = 1;
//        send_data_master.dr_status = rcv_dr_data.dr_status;
//        (void)memcpy(reinterpret_cast<void *>(&(send_data_master.uc_data[0])),
//                     (const void *)(&(rcv_dr_data.longitude)), (size_t)VEHICLE_DSIZE_LONGITUDE);
//        DeadReckoningSetDataMaster(&send_data_master, (PFUNC_DR_DMASTER_SET_N)DeadReckoningDeliveryProc);
//
//        send_data_master.ul_did = VEHICLE_DID_DR_LATITUDE;
//        send_data_master.us_size = VEHICLE_DSIZE_LATITUDE;
//        send_data_master.uc_rcv_flag = 1;
//        send_data_master.dr_status = rcv_dr_data.dr_status;
//        (void)memcpy(reinterpret_cast<void *>(&(send_data_master.uc_data[0])),
//                     (const void *)(&(rcv_dr_data.latitude)), (size_t)VEHICLE_DSIZE_LATITUDE);
//        DeadReckoningSetDataMaster(&send_data_master, (PFUNC_DR_DMASTER_SET_N)DeadReckoningDeliveryProc);
//
//        send_data_master.ul_did = VEHICLE_DID_DR_ALTITUDE;
//        send_data_master.us_size = VEHICLE_DSIZE_ALTITUDE;
//        send_data_master.uc_rcv_flag = 1;
//        send_data_master.dr_status = rcv_dr_data.dr_status;
//        (void)memcpy(reinterpret_cast<void *>(&(send_data_master.uc_data[0])),
//                     (const void *)(&(rcv_dr_data.altitude)), (size_t)VEHICLE_DSIZE_ALTITUDE);
//        DeadReckoningSetDataMaster(&send_data_master, (PFUNC_DR_DMASTER_SET_N)DeadReckoningDeliveryProc);
//
//        send_data_master.ul_did = VEHICLE_DID_DR_SPEED;
//        send_data_master.us_size = VEHICLE_DSIZE_SPEED;
//        send_data_master.uc_rcv_flag = 1;
//        send_data_master.dr_status = rcv_dr_data.dr_status;
//        (void)memcpy(reinterpret_cast<void *>(&(send_data_master.uc_data[0])),
//                     (const void *)(&(rcv_dr_data.rate)), (size_t)VEHICLE_DSIZE_SPEED);
//        DeadReckoningSetDataMaster(&send_data_master, (PFUNC_DR_DMASTER_SET_N)DeadReckoningDeliveryProc);
//
//        send_data_master.ul_did = VEHICLE_DID_DR_HEADING;
//        send_data_master.us_size = VEHICLE_DSIZE_HEADING;
//        send_data_master.uc_rcv_flag = 1;
//        send_data_master.dr_status = rcv_dr_data.dr_status;
//        (void)memcpy(reinterpret_cast<void *>(&(send_data_master.uc_data[0])),
//                     (const void *)(&(rcv_dr_data.heading)), (size_t)VEHICLE_DSIZE_HEADING);
//        DeadReckoningSetDataMaster(&send_data_master, (PFUNC_DR_DMASTER_SET_N)DeadReckoningDeliveryProc);
//
//        /* Data master setting,Data delivery(GyroParameter)           */
//        /* As it is registered for delivery with DID = VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL    */
//        /* Process in GyroOffset-> GyroScaleFactor-> GyroScelFactorLevel order(Processing order cannot be changed.) */
//        send_data_master.ul_did = VEHICLE_DID_DR_GYRO_OFFSET;
//        send_data_master.us_size = VEHICLE_DSIZE_GYRO_OFFSET;
//        send_data_master.uc_rcv_flag = DEADRECKONING_RCVFLAG_ON;
//        send_data_master.dr_status = 0U;       /* Not used */
//        (void)memcpy(reinterpret_cast<void *>(&(send_data_master.uc_data[0])),
//                     reinterpret_cast<void *>(&(rcv_dr_calc_info.gyro_offset)), (size_t)send_data_master.us_size);
//        DeadReckoningSetDataMaster(&send_data_master, (PFUNC_DR_DMASTER_SET_N)DeadReckoningDeliveryProc);
//
//        send_data_master.ul_did = VEHICLE_DID_DR_GYRO_SCALE_FACTOR;
//        send_data_master.us_size = VEHICLE_DSIZE_GYRO_SCALE_FACTOR;
//        send_data_master.uc_rcv_flag = DEADRECKONING_RCVFLAG_ON;
//        send_data_master.dr_status = 0U;       /* Not used */
//        (void)memcpy(reinterpret_cast<void *>(&(send_data_master.uc_data[0])),
//                     reinterpret_cast<void *>(&(rcv_dr_calc_info.gyro_scale_factor)),
//                     (size_t)send_data_master.us_size);
//        DeadReckoningSetDataMaster(&send_data_master, (PFUNC_DR_DMASTER_SET_N)DeadReckoningDeliveryProc);
//
//        send_data_master.ul_did = VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL;
//        send_data_master.us_size = VEHICLE_DSIZE_GYRO_SCALE_FACTOR_LEVEL;
//        send_data_master.uc_rcv_flag = DEADRECKONING_RCVFLAG_ON;
//        send_data_master.dr_status = 0U;       /* Not used */
//        (void)memcpy(reinterpret_cast<void *>(&(send_data_master.uc_data[0])),
//                     reinterpret_cast<void *>(&(rcv_dr_calc_info.gyro_scale_factor_level)),
//                     (size_t)(send_data_master.us_size));
//        DeadReckoningSetDataMaster(&send_data_master, (PFUNC_DR_DMASTER_SET_N)DeadReckoningDeliveryProc);
//
//        /* Data master setting,Data delivery(SpeedPulseParameter)         */
//        /* As it is registered for delivery with DID = VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR_LEVEL   */
//        /* Process in SpeedPulseScaleFactor-> SpeedPulseScaleFactorLevel order(Processing order cannot be changed.)  */
//        send_data_master.ul_did = VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR;
//        send_data_master.us_size = VEHICLE_DSIZE_SPEED_PULSE_SCALE_FACTOR;
//        send_data_master.uc_rcv_flag = DEADRECKONING_RCVFLAG_ON;
//        send_data_master.dr_status = 0U;       /* Not used */
//        (void)memcpy(reinterpret_cast<void *>(&(send_data_master.uc_data[0])),
//                     reinterpret_cast<void *>(&(rcv_dr_calc_info.speed_pulse_scale_factor)),
//                     (size_t)(send_data_master.us_size));
//        DeadReckoningSetDataMaster(&send_data_master, (PFUNC_DR_DMASTER_SET_N)DeadReckoningDeliveryProc);
//
//        send_data_master.ul_did = VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR_LEVEL;
//        send_data_master.us_size = VEHICLE_DSIZE_SPEED_PULSE_SCALE_FACTOR_LEVEL;
//        send_data_master.uc_rcv_flag = DEADRECKONING_RCVFLAG_ON;
//        send_data_master.dr_status = 0U;       /* Not used */
//        (void)memcpy(reinterpret_cast<void *>(&(send_data_master.uc_data[0])),
//                     reinterpret_cast<void *>(&(rcv_dr_calc_info.speed_pulse_scale_factor_level)),
//                     (size_t)(send_data_master.us_size));
//        DeadReckoningSetDataMaster(&send_data_master, (PFUNC_DR_DMASTER_SET_N)DeadReckoningDeliveryProc);
//
//        g_sens_data_get_flg = FALSE;
//    } else if (g_fst_sens_data_get_flg == TRUE) {
//#if (DR_DEBUG == 1) /* PastModel002_DR_VEHICLE */
//        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningRcvMsg FstSnsData -> DeadReckoningLibrary. \r\n");
//#endif
//        for (fst_sens_send_num = 0; fst_sens_send_num < g_fst_sns_buf.data_num; fst_sens_send_num++) {
//            /* Each data is stored in the data format for transmission. */
//            send_sensor_msg->sens_cnt_flag  = 0U;
//            send_sensor_msg->sens_cnt   = 0U;
//            send_sensor_msg->pulse_rev_tbl.reverse_flag  = g_fst_sns_buf.rev_data[fst_sens_send_num];
//            send_sensor_msg->pulse_rev_tbl.pulse_flag   = g_fst_sns_buf.spd_pulse_check_data[fst_sens_send_num];
//            send_sensor_msg->pulse_rev_tbl.pulse_sum_cyclic = g_fst_sns_buf.spd_pulse_data[fst_sens_send_num];
//            (void)memcpy(reinterpret_cast<void *>(&(send_sensor_msg->gyro_tbl.gyro_data[0])),
//                         (const void *)(&(g_fst_sns_buf.gyro_data[fst_sens_send_num * NUM_OF_100msData])),
//                         (size_t)((sizeof(g_fst_sns_buf.gyro_data[fst_sens_send_num])) * NUM_OF_100msData));
//
//#if (DR_DEBUG == 1) /* PastModel002_DR_VEHICLE */
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningRcvMsg send_sensor_msg.sens_cnt_flag %d \r\n",
//                          send_sensor_msg->sens_cnt_flag);
//            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "DeadReckoningRcvMsg SEND SENSOR_COUNTER %d \r\n",
//                          send_sensor_msg->sens_cnt);
//#endif
//
//            /* Sleep to reduce CPU load */
//            MilliSecSleep(DR_FST_SENS_CALC_SLEEP_TIME);
//
//            /* When the sensor data are ready, Call the DR-calculation process. */
//        }
//
//        g_sens_data_get_flg = FALSE;
//
//        g_fst_sens_data_get_flg = FALSE;
//
//    } else {
//        /* nop */
//    }
//}

/*******************************************************************************
* MODULE    : DeadReckoningGetDRData
* ABSTRACT  : Vehicle sensor information acquisition
* FUNCTION  :
* ARGUMENT  : *msg : message buffer
* NOTE      :
* RETURN    : void
******************************************************************************/
void DeadReckoningGetDRData(const DEADRECKONING_MSG_GET_DR_DATA *msg) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    void  *share_top;      /* Start address of shared memory */
    u_int32  share_size;     /* Size of shared memory area */
    RET_API  ret_api;
    int32  ret;
    int32  event_val = VEHICLE_RET_NORMAL;
    EventID  event_id;
    DEADRECKONING_DATA_MASTER  dr_master; /* GPS Data Master */

    DEADRECKONING_MSG_GET_DR_DATA msg_buf;

    /* Defines the data master for each API. */
    SENSORLOCATION_MSG_LONLATINFO_DAT msg_lonlat_info;
    SENSORLOCATION_MSG_ALTITUDEINFO_DAT msg_altitude_info;
    SENSORMOTION_MSG_SPEEDINFO_DAT  msg_speed_info;
    SENSORMOTION_MSG_HEADINGINFO_DAT msg_heading_info;

    (void)memset(reinterpret_cast<void *>(&msg_buf), 0, sizeof(DEADRECKONING_MSG_GET_DR_DATA));
    memcpy(&(msg_buf), msg, sizeof(DEADRECKONING_MSG_GET_DR_DATA));

    /* Check the DID */
    ret = DeadReckoningCheckDid(msg_buf.data.did);

    if (VEHICLESENS_INVALID != ret) {
        /* DID normal */

        /* Link to shared memory */
        ret_api = _pb_LinkShareData(const_cast<char *>(VEHICLE_SHARE_NAME), &share_top, &share_size);
        if (RET_NORMAL == ret_api) {
            /* Acquire the specified data from the data master. */
            (void)memset(reinterpret_cast<void *>(&dr_master), 0, sizeof(DEADRECKONING_DATA_MASTER));
            DeadReckoningGetDataMaster(msg_buf.data.did, &dr_master);

            /* Align data from the data master for API I/F */
            switch ((u_int32)(msg_buf.data.did)) {
                    /* Describes the process for each DID. */
                case VEHICLE_DID_DR_LATITUDE:
                {
                    (void)memset(reinterpret_cast<void *>(&msg_lonlat_info),
                                 0, sizeof(SENSORLOCATION_MSG_LONLATINFO_DAT));

                    /* Size storage(LATITUDE)  */
                    msg_lonlat_info.size = static_cast<u_int16>(sizeof(SENSORLOCATION_MSG_LONLATINFO_DAT));

                    /* DR status setting */
                    msg_lonlat_info.dr_status = dr_master.dr_status;

                    /* The DR enable flag is set to DR status. */
                    msg_lonlat_info.is_exist_dr = dr_master.dr_status;

                    /* Set the Latitude */
                    memcpy(&(msg_lonlat_info.latitude), &(dr_master.uc_data[0]), dr_master.us_size);

                    /* Obtain the data master Longitude */
                    (void)memset(reinterpret_cast<void *>(&dr_master), 0, sizeof(DEADRECKONING_DATA_MASTER));
                    DeadReckoningGetDataMaster(VEHICLE_DID_DR_LONGITUDE, &dr_master);

                    /* Set the Longitude */
                    memcpy(&(msg_lonlat_info.longitude), &(dr_master.uc_data[0]), dr_master.us_size);

                    /* Acquire data master SensorCnt */
                    (void)memset(reinterpret_cast<void *>(&dr_master), 0, sizeof(DEADRECKONING_DATA_MASTER));
                    DeadReckoningGetDataMaster(VEHICLE_DID_DR_SNS_COUNTER, &dr_master);

                    /* Set the SensorCnt */
                    memcpy(&(msg_lonlat_info.sensor_cnt), &(dr_master.uc_data[0]), dr_master.us_size);

                    /* Write data master to shared memory */
                    PosSetShareData(share_top,
                                    msg_buf.data.offset,
                                    (const void *)&msg_lonlat_info,
                                    sizeof(msg_lonlat_info));

                    /* Set Successful Completion */
                    event_val = VEHICLE_RET_NORMAL;

                    break;
                }
                case VEHICLE_DID_DR_ALTITUDE:
                {
                    (void)memset(reinterpret_cast<void *>(&msg_altitude_info),
                                 0, sizeof(SENSORLOCATION_MSG_ALTITUDEINFO_DAT));

                    msg_altitude_info.size = static_cast<u_int16>(sizeof(SENSORLOCATION_MSG_ALTITUDEINFO_DAT));
                    /* The DR enable flag is set to DR status. */
                    msg_altitude_info.is_exist_dr = dr_master.dr_status;
                    msg_altitude_info.dr_status = dr_master.dr_status;

                    /* Set the Speed */
                    memcpy(&(msg_altitude_info.altitude), &(dr_master.uc_data[0]), dr_master.us_size);

                    /* Acquire data master SensorCnt */
                    (void)memset(reinterpret_cast<void *>(&dr_master), 0, sizeof(DEADRECKONING_DATA_MASTER));
                    DeadReckoningGetDataMaster(VEHICLE_DID_DR_SNS_COUNTER, &dr_master);

                    /* Set the SensorCnt */
                    memcpy(&(msg_altitude_info.sensor_cnt), &(dr_master.uc_data[0]), dr_master.us_size);

                    /* Write data master to shared memory */
                    PosSetShareData(share_top,
                                    msg_buf.data.offset,
                                    (const void *)&msg_altitude_info,
                                    sizeof(msg_altitude_info));

                    /* Set Successful Completion */
                    event_val = VEHICLE_RET_NORMAL;

                    break;
                }
                case VEHICLE_DID_DR_SPEED:
                {
                    (void)memset(reinterpret_cast<void *>(&msg_speed_info),
                                 0, sizeof(SENSORMOTION_MSG_SPEEDINFO_DAT));

                    msg_speed_info.size = static_cast<u_int16>(sizeof(SENSORMOTION_MSG_SPEEDINFO_DAT));
                    /* The DR enable flag is set to DR status. */
                    msg_speed_info.is_exist_dr = dr_master.dr_status;
                    msg_speed_info.dr_status = dr_master.dr_status;

                    /* Set the Speed */
                    memcpy(&(msg_speed_info.speed), &(dr_master.uc_data[0]), dr_master.us_size);

                    /* Acquire data master SensorCnt */
                    (void)memset(reinterpret_cast<void *>(&dr_master), 0, sizeof(DEADRECKONING_DATA_MASTER));
                    DeadReckoningGetDataMaster(VEHICLE_DID_DR_SNS_COUNTER, &dr_master);
                    /* Set the SensorCnt */
                    memcpy(&(msg_speed_info.sensor_cnt), &(dr_master.uc_data[0]), dr_master.us_size);

                    /* Write data master to shared memory */
                    PosSetShareData(share_top,
                                          msg_buf.data.offset,
                                          (const void *)&msg_speed_info,
                                          sizeof(msg_speed_info));

                    /* Set Successful Completion */
                    event_val = VEHICLE_RET_NORMAL;

                    break;
                }
                case VEHICLE_DID_DR_HEADING:
                {
                    (void)memset(reinterpret_cast<void *>(&msg_heading_info),
                                 0, sizeof(SENSORMOTION_MSG_HEADINGINFO_DAT));

                    msg_heading_info.size = static_cast<u_int16>(sizeof(SENSORMOTION_MSG_HEADINGINFO_DAT));
                    /* The DR enable flag is set to DR status. */
                    msg_heading_info.is_exist_dr = dr_master.dr_status;
                    msg_heading_info.dr_status = dr_master.dr_status;

                    /* Set the Heading */
                    memcpy(&(msg_heading_info.heading), &(dr_master.uc_data[0]), dr_master.us_size);

                    /* Acquire data master SensorCnt */
                    (void)memset(reinterpret_cast<void *>(&dr_master), 0, sizeof(DEADRECKONING_DATA_MASTER));
                    DeadReckoningGetDataMaster(VEHICLE_DID_DR_SNS_COUNTER, &dr_master);
                    /* Set the SensorCnt */
                    memcpy(&(msg_heading_info.sensor_cnt), &(dr_master.uc_data[0]), dr_master.us_size);

                    /* Write data master to shared memory */
                    PosSetShareData(share_top,
                                          msg_buf.data.offset,
                                          (const void *)&msg_heading_info,
                                          sizeof(msg_heading_info));
                    /* Set Successful Completion */
                    event_val = VEHICLE_RET_NORMAL;

                    break;
                }
                /* Other than the above */
                default:
                    /* Do not edit. */
                    break;
            }

            /* Check the data size */
            if (msg_buf.data.size < dr_master.us_size) {
                /* Shared memory error(Insufficient storage size) */
                event_val = VEHICLE_RET_ERROR_OUTOF_MEMORY;
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
    event_id = VehicleCreateEvent(msg_buf.data.pno);

    /* Publish Events */
    ret_api = _pb_SetEvent(event_id, SAPI_EVSET_ABSOLUTE, event_val);
}

/*******************************************************************************
* MODULE    : DeadReckoningSetMapMatchingData
* ABSTRACT  : Map-Matching information setting
* FUNCTION  : Setting Map-Matching Information
* ARGUMENT  : *msg : message buffer
* NOTE      :
* RETURN    : void
******************************************************************************/
void DeadReckoningSetMapMatchingData(const DR_MSG_MAP_MATCHING_DATA *msg) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}

/*******************************************************************************
* MODULE    : DeadReckoningClearBackupData
* ABSTRACT  : Backup data clear function CALL
* FUNCTION  : Call the backup data clear function.
* ARGUMENT  : *msg : message buffer
* NOTE      :
* RETURN    : None
******************************************************************************/
void DeadReckoningClearBackupData(const DR_MSG_CLEAR_BACKUP_DATA *msg) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    char  event_name[32]; /* Event name character string buffer */
    EventID  event_id;   /* Event ID */
    int32  event_val;   /* Event value to set*/
    RET_API  ret_api;   /* System API return value */

    if (msg != NULL) {
        /* DR backup data initialization function call */
        event_val = RET_NORMAL;

        /* Initialization of event name character string buffer */
        (void)memset(reinterpret_cast<void *>(event_name), 0, sizeof(event_name));

        /* Event name creation */
        snprintf(event_name, sizeof(event_name), "DR_API_%X", msg->hdr.hdr.sndpno);
        /* Match DR_API.cpp side with name */

        /* Event Generation */
        event_id = _pb_CreateEvent(_CWORD64_EVENT_MANUALRESET_OFF, DR_EVENT_VAL_INIT, event_name);

        if (event_id != 0) {
            /* For successful event generation */
            /* Set the event */
            ret_api = _pb_SetEvent(event_id, SAPI_EVSET_ABSOLUTE, event_val);

            if (ret_api == RET_NORMAL) {
                /* Successful event set */
            } else {
                /* Event set failed */
                /* Delete Event and Return Event Generation Failed */
                ret_api = _pb_DeleteEvent(event_id);
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SetEvent Failed\r\n");
            }
        }
    }
}

/*******************************************************************************
* MODULE    : DeadReckoningSetEvent
* ABSTRACT  : Set of events
* FUNCTION  : Set event to return successful or unsuccessful log configuration retrieval
* ARGUMENT  : PNO     pno  : Caller Pno
*           : RET_API ret : Log setting acquisition Success/Fail
*           :                RET_NORMAL: Log setting acquisition success
*           :                RET_ERROR: Log setting acquisition failure
* NOTE      :
* RETURN    : None
******************************************************************************/
static void DeadReckoningSetEvent(PNO pno, RET_API ret) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    char  event_name[32]; /* Event name character string buffer */
    EventID  event_id;   /* Event ID */
    RET_API  ret_api;   /* System API return value */

    /* Initialization of event name character string buffer */
    (void)memset(reinterpret_cast<void *>(event_name), 0, sizeof(event_name));

    /* Event name creation */
    snprintf(event_name, sizeof(event_name), "VehicleDebug_%X", pno);
    /* Event name should match VehicleDebug_API.cpp */

    /* Event Generation */
    event_id = _pb_CreateEvent(_CWORD64_EVENT_MANUALRESET_OFF, VEHICLEDEBUG_EVENT_VAL_INIT, event_name);

    if (event_id != 0) {
        /* For successful event generation */
        /* Set the event */
        ret_api = _pb_SetEvent(event_id, SAPI_EVSET_ABSOLUTE, ret);

        if (ret_api == RET_NORMAL) {
            /* Successful event set */
        } else {
            /* Event set failed */
            /* Delete Event */
            ret_api = _pb_DeleteEvent(event_id);
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SetEvent Failed\r\n");
        }
    }
}

/*******************************************************************************
* MODULE    : DeadReckoningLinkSharedMemory
* ABSTRACT  : Shared memory link
* FUNCTION  : Link to shared memory
* ARGUMENT  : char *shared_memory_name : Name of shared memory to link
*           : void **share_addr        : Pointer to a variable that stores the address of the linked shared memory.
* NOTE      :
* RETURN    : None
******************************************************************************/
static void DeadReckoningLinkSharedMemory(char *shared_memory_name, void **share_addr) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API ret_api;
    void *pv_share_mem; /* Store Shared Memory Address */
    u_int32 share_mem_size; /* Size of the linked shared memory */

    if ((shared_memory_name != NULL) && (share_addr != NULL)) {
        /* Link to the handle storage area */
        ret_api = _pb_LinkShareData(shared_memory_name, &pv_share_mem, &share_mem_size);

        if (ret_api == RET_NORMAL) {
            /* If the link is successful */
            if (share_mem_size == VEHICLEDEBUG_MSGBUF_DSIZE) {
                /* When the size of the linked shared memory is correct */
                *share_addr = pv_share_mem; /* Set the address */
            } else {
                /* The size of the linked shared memory is incorrect. */
                *share_addr = NULL;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Bad shared memory size\r\n");
            }
        } else {
            /* If the link fails */
            *share_addr = NULL;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't link shared memory\r\n");
        }
    }
}

/*******************************************************************************
* MODULE    : DeadReckoningWriteSharedMemory
* ABSTRACT  : Write Shared Memory
* FUNCTION  : Write Shared Memory
* ARGUMENT  : VEHICLEDEBUG_MSG_LOGINFO_DAT* loginfo : Logging information
* RETURN    : RET_API : Whether writing to shared memory was successful
*           :         : RET_NORMAL Success
*           :         : RET_ERROR Failed
* NOTE      :
******************************************************************************/
static RET_API DeadReckoningWriteSharedMemory(VEHICLEDEBUG_MSG_LOGINFO_DAT* loginfo) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    static VEHICLEDEBUG_MSG_LOGINFO_DAT *share_addr = NULL; /* Store Shared Memory Address */
    static SemID   sem_id = 0;      /* ID of shared memory exclusive semaphore */

    RET_API ret = RET_NORMAL; /* Return of the functions */
    RET_API ret_api;    /* Return of the functions */

#if DEAD_RECKONING_MAIN_DEBUG
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Call %s\r\n", __func__);
#endif

    /* Get Semaphore ID */
    if (sem_id == 0) {
        sem_id = _pb_CreateSemaphore(const_cast<char *>(SENSOR_LOG_SETTING_SEMAPHO_NAME));
    }

    if (sem_id != 0) {
        /* Semaphore ID successfully acquired */
        ret_api = _pb_SemLock(sem_id); /* Semaphore Lock */

        if (ret_api == RET_NORMAL) {
            /* Semaphore lock successful */

            /* When the shared memory is not linked */
            if (share_addr == NULL) {
                /* Link to shared memory */
                DeadReckoningLinkSharedMemory(const_cast<char *>(LOG_SETTING_SHARE_MEMORY_NAME),
                                              reinterpret_cast<void **>(&share_addr));
                /* #QAC confirmation Rule11.4 Use structure for member reference(Cast according to shared memory link IF) */
            }

            if (share_addr != NULL) {
                /* The link to shared memory is successful. */
                /* Writing Data to Shared Memory */
                share_addr->log_sw = loginfo->log_sw;
                (void)memcpy(reinterpret_cast<void *>(share_addr->severity),
                             (const void *)(loginfo->severity), sizeof(share_addr->severity));
            } else {
                /* Failed to link to shared memory */
                ret = RET_ERROR;
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "DeadReckoningLinkSharedMemory Failed");
            }
            /* Semaphore unlock */
            (void)_pb_SemUnlock(sem_id);
        } else {
            /* Semaphore lock failed */
            ret = RET_ERROR;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_SemLock Failed");
        }
    } else {
        ret = RET_ERROR;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "sem_id == 0");
    }

    return ret;
}

/*******************************************************************************
* MODULE    : DeadReckoningGetLocationLogStatus
* ABSTRACT  : CALL of functions for acquiring logging settings
* FUNCTION  : Call the log setting acquisition function.
* ARGUMENT  : None
* NOTE      :
* RETURN    : void
******************************************************************************/
void DeadReckoningGetLocationLogStatus(PNO pno) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API       ret_api; /* System API return value */
    RET_API       ret;
    VEHICLEDEBUG_MSG_LOGINFO_DAT loginfo; /* Logging information */
    BOOL log_sw = FALSE;

    /* CALL of functions for acquiring logging settings */
    ret_api = RET_NORMAL;

    if (ret_api == RET_NORMAL) {
        /* Log setting acquisition function succeeded */
        loginfo.log_sw = (u_int32)(log_sw);

        /* Write to shared memory */
        ret = DeadReckoningWriteSharedMemory(&loginfo);

        /* Event publishing */
        DeadReckoningSetEvent(pno, ret);
    } else {
        /* Log setting acquisition function failed */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "GetLocationLogSetting Failed");

        /* Event publishing */
        DeadReckoningSetEvent(pno, RET_ERROR);
    }
}

/*******************************************************************************
* MODULE    : DeadReckoningSetLocationLogStatus
* ABSTRACT  : CALL of log-setting-request-function
* FUNCTION  : Call the log setting request function.
* ARGUMENT  : u_int32 log_sw : Log type
*           : u_int8 severity : Output level
* NOTE      :
* RETURN    : void
******************************************************************************/
void DeadReckoningSetLocationLogStatus(BOOL log_sw, u_int8 severity) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}

// LCOV_EXCL_STOP
