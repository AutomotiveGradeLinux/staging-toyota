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
/**
* @file LineSensDrv_Api.cpp
*/

/*---------------------------------------------------------------------------*/
// Include files

#include "LineSensDrv_Api.h"
#include "positioning_def.h"

/*---------------------------------------------------------------------------*/
// Macro definitions

#define LSDRV_GPS_DATA_TOP_TH           5       // Threshold for determining the beginning of GPS data
#define LSDRV_SENS_DATA_RCV_WAIT_TIME   400     // Sensor data wait time of GPS reception flag ON(Total)
#define LSDRV_SENS_DATA_RCV_WAIT_TERM   50      // Sensor data wait time of GPS reception flag ON(1 time)

#define LSDRV_EST_GPS_CNT_ARRAY_NUM     3U      // Number of arrays for holding estimated GPS counter
#define LSDRV_EST_GPS_CNT_ENABLE_TH     5U      // Estimated GPS counter enable/disable determination threshold
#define LSDRV_EST_GPS_CNT_ADD_VALUE     10U     // Estimated GPS counter update addition value

// GPS data reception cycle:1sec sensor counters: 100ms

// for debug
#define LINE_SENS_DRV_API_DEBUG_SWITCH  0       // 0:OFF 1:ON

/*---------------------------------------------------------------------------*/
// Global variable

static HANDLE  g_gps_irq_mutex          = NULL;             // GPS received flag Mutex handles
static BOOL    g_rcv_pps_int            = FALSE;            // PPS interrupt reception flag  (GPS->_CWORD102_)
static BOOL    g_rcv_gps_irq            = FALSE;            // GPS reception flag    (GPS->_CWORD56_)
static u_int8  g_rcv_gps_sens_cnt_tmp   = 0;                // Sensor counter when GPS reception flag is ON(for retention)
static u_int8  g_rcv_gps_sens_cnt       = 0;                // Sensor counter when GPS reception flag is ON
static u_int8  g_gps_sens_cnt_top       = 0;                // Sensor counter when first GPS data is received
static u_int8  g_est_gps_cnt[LSDRV_EST_GPS_CNT_ARRAY_NUM];  // Array for storing estimated GPS counter values
static BOOL    g_est_gps_cnt_available  = FALSE;            // Estimated GPS counter value enable/disable judgment

typedef struct VehicleSensDataMaster {
  DID     ul_did;           // Data ID
  u_int16 us_size;          // Size of the data
  u_int8  uc_rcv_flag;      // Receive flag
  u_int8  uc_sns_cnt;       // Sensor counter
  u_int8  uc_data[132];     // Vehicle sensor data
} VEHICLESENS_DATA_MASTER;


/*******************************************************************************
 * MODULE    : DeliveryLineSensorDataPositioning
 * ABSTRACT  : LineSensor vehicle signaling notification messages sending process
 * FUNCTION  : Send LineSensor vehicle signalling notification messages
 * ARGUMENT  : *pstSendBuf:Transmitted data
 *           : uc_data_num  :Number of sent data
 * NOTE      :
 * RETURN    : None
 ******************************************************************************/
void DeliveryLineSensorDataPositioning(LSDRV_MSG_LSDATA_G* pst_send_buf, u_int8 uc_data_num) {
  if (pst_send_buf != NULL) {
    /* Initializing sent messages */
    memset(reinterpret_cast<void *>(&(pst_send_buf->st_head)), 0, sizeof(T_APIMSG_MSGBUF_HEADER));

    /* Message Header Settings */
    pst_send_buf->st_head.hdr.sndpno      = PNO_LINE_SENS_DRV;                /* Source process number */
    pst_send_buf->st_head.hdr.cid         = CID_LINESENS_VEHICLE_DATA_G;      /* Command ID */
    pst_send_buf->st_head.hdr.msgbodysize = sizeof(LSDRV_MSG_LSDATA_DAT_G);   /* Message data body length */

    /* Message data is already set */
    pst_send_buf->st_para.uc_data_num       = uc_data_num;

    /* Send messages */
    (void)_pb_ZcSndMsg(PNO_VEHICLE_SENSOR, sizeof( LSDRV_MSG_LSDATA_G ), 0);
  }
}

/*******************************************************************************
 * MODULE    : LineSensDrvApi_Initialize
 * ABSTRACT  : LineSensDrvApi initialization process
 * FUNCTION  : LineSensDrvApi initialization process
 * ARGUMENT  : -
 * NOTE      :
 * RETURN    : -
 ******************************************************************************/
BOOL LineSensDrvApiInitialize(void) {
  BOOL ret = TRUE;

  g_gps_irq_mutex = _pb_CreateMutex(NULL, FALSE, MUTEX_GPS_IRQ_FLG);

  if (g_gps_irq_mutex == 0) {
    ret = FALSE;
  } else {
    g_rcv_gps_irq = FALSE;
    g_rcv_gps_sens_cnt_tmp = 0;
    LineSensDrvApiInitEstGpsCnt();
    ret = TRUE;
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : LineSensDrvApi_InitEstGpsCnt
 * ABSTRACT  : Estimated GPS counter related parameter initialization processing
 * FUNCTION  : Estimated GPS counter related parameter initialization processing
 * ARGUMENT  : -
 * NOTE      :
 * RETURN    : -
 ******************************************************************************/
void LineSensDrvApiInitEstGpsCnt(void) {
  /* Initializing process */
  g_rcv_pps_int = FALSE;
  g_rcv_gps_sens_cnt = 0;
  g_gps_sens_cnt_top = 0;

  (void)memset(reinterpret_cast<void *>(&g_est_gps_cnt[0]), 0, sizeof(g_est_gps_cnt));
  g_est_gps_cnt_available  = FALSE;

  return;
}

/*---------------------------------------------------------------------------*/
/*EOF*/
