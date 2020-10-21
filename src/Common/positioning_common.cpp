/*
 * @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
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
* @file positioning_common.cpp
*/

/*---------------------------------------------------------------------------*/
// Include files

#include "positioning_common.h"
#include "MDev_Gps_Common.h"
#include "MDev_GpsRecv.h"
#include "LineSensDrv_Thread.h"

/*---------------------------------------------------------------------------*/
// Value define

#define DATMOD_RETRY                        (3) /* Number of shared memory generation retries */
#define DATMOD_PREINIT                      (0) /* Shared Memory State [Before initialization] */
#define PRIM_NAME_MAX                       (32) /* Maximum Name Size */
#define VEHICLE_SHARE_NAME                  ("POS_VEHICLE_SHARE_MEMORY")  /* Shared memory name */

/* Mask for managing various notification reception conditions */
#define NTFY_MSK_NONE                       (0x00)
/* Service availability notification */
#define NTFY_MSK_COMMUNICATION_AVAILABILITY (0x01)
#define NTFY_MSK_PS_COMMUSB_AVAILABILITY    (0x02)
#define NTFY_MSK_PS_PSMSHADOW_AVAILABILITY  (0x04)
#define NTFY_MSK_CLOCK_AVAILABILITY         (0x08)
#define NTFY_MSK_NS_BACKUPMGR_AVAILABILITY  (0x10)
#define NTFY_MSK_SS_DEVDETSRV_AVAILABILITY  (0x20)
/* Other Notices */
#define NTFY_MSK_PS_PSMSHADOW_INIT_COMP     (0x01)  /* PSMShadow startup completion notice           */
#define NTFY_MSK_PS_LANSERVER_DEVICE_UPDATE (0x02)  /* LanServer device configuration change notification       */
#define NTFY_MSK_PS_LANSERVER_WAKEUP_COMP   (0x04)  /* LanServer start completion notice */

/* Thread state */
#define THREAD_STS_NOEXIST                  (0x00)
#define THREAD_STS_CREATING                 (0x01)
#define THREAD_STS_CREATED                  (0x02)

/*---------------------------------------------------------------------------*/
// ENUMERATION

/*---------------------------------------------------------------------------*/
// STRUCTURE

typedef struct StThreadInfo {
  EnumTID_POS     e_id;           /**< Thread ID */
  const int8_t*   p_name;         /**< Thread name  */
  PNO             p_no;           /**< Process number */
  CbFuncPtr       cb_routine;     /**< Start routine  */
  uint8_t         msk_available;  /**< Dependent services Availability  */
  uint8_t         msk_ntfy;       /**< Dependent notification                 */
  uint8_t         msk_thread;     /**< Dependent threads              */
  BOOL            b_is_depended;  /**< Positioning/Availability->TRUE change dependency */
  uint8_t         uc_status;      /**< Thread activation state                             */
  uint8_t         uc_order;       /**< Boot Sequence(Performance)                             */
  uint8_t         u_reserve[2];
} ST_THREAD_CREATE_INFO;

typedef struct {
    char  share_data_name[PRIM_NAME_MAX]; /**< Shared data name   */
    u_int32 data_size;            /**< Shared data size */
} ST_SHAREDATA;

static const int8_t kThreadNamePosMain[15]          = "POS_Main";
static const int8_t kThreadNamePosSens[15]          = "POS_Sens";
static const int8_t kThreadNamePosGps[15]           = "POS_Gps";
static const int8_t kThreadNamePosGpsRecv[15]       = "POS_Gps_Recv";
static const int8_t kThreadNamePosGpsRollover[15]   = "POS_Gps_Rolovr";

static ST_THREAD_CREATE_INFO g_pos_thread_create_info[] = {
  { /* POS_Main */
    ETID_POS_MAIN, /* (1) */
    NULL, /* (2) */
    0, /* (3) */
    NULL, /* (4) */
    (NTFY_MSK_NONE), /* (5) */
    (NTFY_MSK_NONE), /* (6) */
    0, /* (7) */
    FALSE, /* (8) */
    THREAD_STS_NOEXIST, /* (9) */
    0 /* (10) */
  },
  { /* POS_sens */
    ETID_POS_SENS, /* (1) */
    kThreadNamePosSens, /* (2) */
    PNO_LINE_SENS_DRV, /* (3) */
    &LineSensDrvThread, /* (4) */
    (NTFY_MSK_NONE), /* (5) */
    (NTFY_MSK_NONE), /* (6) */
    0, /* (7) */
    FALSE, /* (8) */
    THREAD_STS_NOEXIST, /* (9) */
    0 /* (10) */
  },
  { /* POS_Gps */
    ETID_POS_GPS, /* (1) */
    kThreadNamePosGps, /* (2) */
    PNO_NAVI_GPS_MAIN, /* (3) */
    &DevGpsMainThread, /* (4) */
    (NTFY_MSK_NONE), /* (5) */
    (NTFY_MSK_NONE), /* (6) */
    0, /* (7) */
    TRUE, /* (8) */
    THREAD_STS_NOEXIST, /* (9) */
    0 /* (10) */
  },
  { /* POS_Gps_Recv */
    ETID_POS_GPS_RECV, /* (1) */
    kThreadNamePosGpsRecv, /* (2) */
    PNO_NAVI_GPS_RCV, /* (3) */
    &DevGpsRecvThread, /* (4) */
    (NTFY_MSK_NONE), /* (5) */
    (NTFY_MSK_NONE), /* (6) */
    THREAD_STS_MSK_POS_GPS, /* (7) */
    FALSE, /* (8) */
    THREAD_STS_NOEXIST, /* (9) */
    0 /* (10) */
  },
  { /* POS_Gps_Rolovr */
    ETID_POS_GPS_ROLLOVER, /* (1) */
    NULL, /* (2) */
    0, /* (3) */
    NULL, /* (4) */
    (NTFY_MSK_NONE), /* (5) */
    (NTFY_MSK_NONE), /* (6) */
    0, /* (7) */
    FALSE, /* (8) */
    THREAD_STS_NOEXIST, /* (9) */
    0 /* (10) */
  },
  { /* Termination */
    ETID_POS_MAX, /* (1) */
    NULL, /* (2) */
    0, /* (3) */
    NULL, /* (4) */
    NTFY_MSK_NONE, /* (5) */
    NTFY_MSK_NONE, /* (6) */
    0, /* (7) */
    FALSE, /* (8) */
    THREAD_STS_NOEXIST, /* (9) */
    0 /* (10) */
  },
};

static ST_SHAREDATA g_sharedata_tbl[] = {
    /* Shared data name to be generated,        Shared data size */
    { {VEHICLE_SHARE_NAME},         512 * 11 }, /* Vehicle sensor information acquisition                     */
#if 0 /* Not used in _CWORD71_ below */
    { {"SENSOR_SHARE_MEMORY"},        512 * 11 }, /* Vehicle sensor information Pkg acquisition                  */
    { {"GPS_INT_SIGNAL_SHARE_MEMORY"},    4    }, /* GPS interrupt signal acquisition                      */
    { {"LOG_SETTING_SHARE_MEMORY"},     36     }, /* DR feature log acquisition                         */
    { {"GYRO_CONNECT_STTS_SHARE_MEMORY"}, 4    }, /* Gyro connection status acquisition                   */
    { {"EPHEMERIS_NUM_SHARE_MEMORY"},   4    }, /* Effective ephemeris count acquisition at shutdown */
    { {"LOCALTIME_SHARE_MEMORY"},     12     }, /* Local time acquisition at shutdown     */
    { {"LONLAT_SHARE_MEMORY"},        8    }, /* Location acquisition at shutdown               */
#endif
    { {(int8)NULL},             0    }  /* Termination */
};


/*---------------------------------------------------------------------------*/
// Functions

static EFrameworkunifiedStatus PosStopThreadDummy(HANDLE h_app) {
  return eFrameworkunifiedStatusOK;
}

static uint32_t PosGetMsg(HANDLE h_app, void** p_buf, uint32_t ul_size) {
  EFrameworkunifiedStatus  e_status = eFrameworkunifiedStatusOK;
  uint32_t    tmp_size = 0;
  void*       p_rev_buf = NULL;

  if ((h_app == NULL) || (p_buf == NULL)) {
      POSITIONING_LOG("Argument ERROR!! [h_app = %p, pBuf = %p]", h_app , p_buf);
  } else {
    /* Check the size of received data */
    tmp_size = FrameworkunifiedGetMsgLength(h_app);
    if (tmp_size > ul_size) {
      POSITIONING_LOG("Message ul_size ERROR!! [tmp_size = %d, maxsize = %d]", tmp_size, ul_size);
      tmp_size = 0;
    } else {
      /* Obtain data */
      e_status = FrameworkunifiedGetDataPointer(h_app, &p_rev_buf);

      if (e_status == eFrameworkunifiedStatusOK) {
        *p_buf = p_rev_buf;
      } else if (e_status == eFrameworkunifiedStatusInvldBufSize) {
        e_status = FrameworkunifiedGetMsgDataOfSize(h_app, *p_buf, tmp_size);

        if (e_status != eFrameworkunifiedStatusOK) {
          POSITIONING_LOG("FrameworkunifiedGetMsgDataOfSize ERROR [e_status:%d]", e_status);
          tmp_size = 0;
        }
      } else {
        POSITIONING_LOG("FrameworkunifiedGetDataPointer ERROR [e_status:%d]", e_status);
        tmp_size = 0;
      }
    }
  }

  return tmp_size;
}

static void PosCreateSharedMemory(void) {
  RET_API ret_api = RET_NORMAL;
  void    *mod_exec_dmy; /* Module data pointer(dummy) */
  int         retry; /* Retry counter */
  ST_SHAREDATA *p_shm_tbl;

  /* Configure Shared Data Generating Tables */
  p_shm_tbl = g_sharedata_tbl;

  while (*(p_shm_tbl->share_data_name) != (int8)NULL) {
      for (retry = 0; retry < DATMOD_RETRY; retry++) {
          /* Shared Memory Generation */
          ret_api = _pb_CreateShareData(p_shm_tbl->share_data_name, p_shm_tbl->data_size, &mod_exec_dmy);
          if (ret_api == RET_NORMAL) {
              /* Set the shared memory status flag to "Before initialization (0)" */
              *reinterpret_cast<u_int32 *>(mod_exec_dmy) = DATMOD_PREINIT;
              break;
          } else {
              /* Error Handling */
              POSITIONING_LOG("_pb_CreateShareData ERROR [ret_api:%d]", ret_api);
          }
      }

      if (retry >= DATMOD_RETRY) {
          POSITIONING_LOG("_pb_CreateShareData failed more %d times.", DATMOD_RETRY);
          _pb_Exit();
          /* don't arrive here. */
      }

      /* Next shared memory generation */
      p_shm_tbl++;
  }
  return;
}

EFrameworkunifiedStatus PosInitialize(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  RET_API ret_api;
  ret_api = _pb_Setup_CWORD64_API(h_app);
  if (ret_api != RET_NORMAL) {
      POSITIONING_LOG("_pb_Setup_CWORD64_API ERROR!! [ret_api = %d]", ret_api);

      e_status = eFrameworkunifiedStatusFail;
  } else {
    PosCreateSharedMemory();
  }
  return e_status;
}

/**
 * @brief
 *   Common processing after thread startup
 *
 *   Thread naming, message queue creation, thread startup response
 *
 * @param[in]  h_app Application handle
 * @param[in]  eTid	Thread ID
 *
 * @return  Thread startup mode
 */
EnumSetupMode_POS PosSetupThread(HANDLE h_app, EnumTID_POS e_tid) {
  RET_API                 ret = RET_NORMAL;
  ST_THREAD_CREATE_INFO*  p_thread_info = g_pos_thread_create_info;
  ST_THREAD_CREATE_INFO*  p_info = p_thread_info + e_tid;
  ST_THREAD_SETUP_INFO    st_setup_info;
  ST_THREAD_SETUP_INFO*   pst_setup_info = &st_setup_info;
  /* Application handle setting */
  _pb_SetAppHandle(h_app);
  /* Create Message Queue */
  _pb_CreateMsg(p_info->p_no);
  /* Get thread startup information */
  pst_setup_info->mode = EPOS_SETUP_MODE_NORMAL;
  (void)PosGetMsg(h_app, reinterpret_cast<void**>(&pst_setup_info), sizeof(ST_THREAD_SETUP_INFO));

  POSITIONING_LOG("[mode = %d]", pst_setup_info->mode);

  /* Issue thread creation completion notice */
  ret = _pb_SndMsg_Ext(POS_THREAD_NAME,
                        CID_THREAD_CREATE_COMP,
                        sizeof(EnumTID_POS),
                        reinterpret_cast<const void*>(&e_tid),
                        0);

  if (ret != RET_NORMAL) {
    POSITIONING_LOG("_pb_SndMsg_Ext ERROR!! [ret = %d]", ret);
  }

  return pst_setup_info->mode;
}

/**
 * @brief
 *   Common processing at thread stop
 *
 *   Thread stop response, thread destruction
 *
 * @param[in]  e_tid	Thread ID
 */
void PosTeardownThread(EnumTID_POS e_tid) {
  RET_API e_ret = RET_NORMAL;
  ST_THREAD_CREATE_INFO*  p_thread_info = g_pos_thread_create_info;

  (p_thread_info + e_tid)->uc_status = THREAD_STS_NOEXIST;
  /* Issue thread stop completion notice */
  e_ret = _pb_SndMsg_Ext(POS_THREAD_NAME,
                           CID_THREAD_STOP_COMP,
                           sizeof(EnumTID_POS),
                           reinterpret_cast<const void*>(&e_tid),
                           0);

  if (e_ret != RET_NORMAL) {
    POSITIONING_LOG("_pb_SndMsg_Ext ERROR!! [e_ret = %d]", e_ret);
  }

  /* Thread destruction */
  _pb_ExitThread((DWORD)0);

  return;
}

/**
 * @brief
 *   Positioning in-process thread creation
 *
 * @param[in]  hApp Application handle
 */
EFrameworkunifiedStatus PosCreateThread(HANDLE h_app, int8_t index) {
  HANDLE                  thread_handle;
  EFrameworkunifiedStatus              e_status = eFrameworkunifiedStatusOK;
  ST_THREAD_CREATE_INFO*  p_thread_info = g_pos_thread_create_info;

  p_thread_info += index;

  static EnumSetupMode_POS g_setup_mode;

  if (NULL == h_app) {
    return eFrameworkunifiedStatusInvldHandle;
  }

  if (index < ETID_POS_MAX) {
    if ((p_thread_info->uc_status == THREAD_STS_NOEXIST) && (p_thread_info->cb_routine != NULL)) {
      /* Thread creation */
      thread_handle = FrameworkunifiedCreateChildThread(h_app,
                                           (PCSTR)(p_thread_info->p_name),
                                           p_thread_info->cb_routine,
                                           &PosStopThreadDummy);

      if (thread_handle == NULL) {
        POSITIONING_LOG("FrameworkunifiedCreateChildThread ERROR!! [tHandle=%p]", thread_handle);
      } else {
        e_status = FrameworkunifiedStartChildThread(h_app,
                                       thread_handle,
                                       sizeof(EnumSetupMode_POS),
                                       &g_setup_mode);

        if (e_status != eFrameworkunifiedStatusOK) {
          POSITIONING_LOG("FrameworkunifiedStartChildThread ERROR!! [e_status=%d, name=%s]", e_status, p_thread_info->p_name);
        } else {
          p_thread_info->uc_status = THREAD_STS_CREATING;
          POSITIONING_LOG("name=%s\n", p_thread_info->p_name);
        }
      }
    }
  }
  return e_status;
}

/*---------------------------------------------------------------------------*/
/*EOF*/
