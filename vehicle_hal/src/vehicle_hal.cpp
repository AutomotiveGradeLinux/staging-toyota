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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <agl_thread.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_dispatcher.h>
#include <native_service/frameworkunified_multithreading.h>
#include "vehicle_hal_frameworkunifiedlog.h"

extern "C" {
//  #include "carsignal_mng_api.h"
}

#include "vehicle_hal.h"

/**
* \~english environment variable define
*/
#define VEHICLEHAL_LINESENSKIND_ADIM_DATA                 "VEHICLEHAL_LINESENSKIND_ADIM"
/**
* \~english environment variable define
*/
#define VEHICLEHAL_LINESENSKIND_MIC_DATA                  "VEHICLEHAL_LINESENSKIND_MIC"
/**
* \~english environment variable define
*/
#define VEHICLEHAL_LINESENSKIND_VB_DATA                   "VEHICLEHAL_LINESENSKIND_VB"
/**
* \~english environment variable define
*/
#define VEHICLEHAL_LINESENSKIND_RHEOSTAT_DATA             "VEHICLEHAL_LINESENSKIND_RHEOSTAT"

#define VEHICLEHAL_LOWVB_VALUE  75

// send message flag
static HANDLE g_sendmsg_handle = NULL;

// work thread exit flag
static BOOL g_loopendflag = FALSE;

// polling thread exit flag
static BOOL g_polloopendflag = FALSE;

// isOpen flag
static BOOL g_isopenflag = FALSE;

// thread id
static pthread_t g_threadid = 0;

// hard object struct
//static VclCtlApiObj g_sclientobj;

// app name
static CHAR g_appname[MAX_NAME_SIZE_APP];

// line sense list
const UINT8 kLineSensList[VEHICLEHAL_LINESENSKIND_NUM] = {
//  CARSIGNAL_IG,
//  CARSIGNAL_PKB,
//  CARSIGNAL_REV,
//  CARSIGNAL_ILL,
};

static void *VehicleHalProcess(void *arg);

HANDLE g_vehiclehalext_thread;
HANDLE g_vehiclehalpol_thread;
EFrameworkunifiedStatus VehicleHalProcessExtStart(HANDLE happ);
EFrameworkunifiedStatus VehicleHalProcessExtStop(HANDLE happ);
EFrameworkunifiedStatus VehicleHalProcessRecvSpeed(HANDLE happ);

EFrameworkunifiedStatus VehicleHalProcessPolStart(HANDLE happ);
EFrameworkunifiedStatus VehicleHalProcessPolStop(HANDLE happ);

static const FrameworkunifiedProtocolCallbackHandler kVehicleHalSpd[] = { {
    CID_VEHICLEHAL_SPD, VehicleHalProcessRecvSpeed } };

#define VEHICLEHAL_SPEED "VehicleHalSpd"
#define VEHICLEHAL_EXT "VehicleHalExt"
#define VEHICLEHAL_POL "VehicleHalPol"

VEHICLEHAL_RET_TYPE VehicleHalStart(HANDLE happ) {
  if (NULL == happ) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Handle is NULL");
    return VEHICLEHAL_RET_ERR_PARAM;
  }

  // check open flag
  if (TRUE == g_isopenflag) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Already Opened");
    return VEHICLEHAL_RET_SUCCESS;
  }

//  memset(&g_sclientobj, 0, sizeof(g_sclientobj));

  // enter loop
  g_loopendflag = FALSE;

  g_polloopendflag = FALSE;

  // get app name
  PCSTR app_name = FrameworkunifiedGetAppName(happ);
  if (NULL == app_name) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Get App Name Failed");
    return VEHICLEHAL_RET_ERR_PARAM;
  }

  memset(g_appname, 0, sizeof(g_appname));
  memcpy(g_appname, app_name, sizeof(g_appname));
  g_appname[MAX_NAME_SIZE_APP] = '\0';

//  // Start VLC data input control API
//  UINT32 ret = VclCtlApiOpen(&g_sclientobj);
//  if ( CARSIGNAL_CTL_RET_SUCCESS != ret ) {
//    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Ten Api Open Failed");
//    return VEHICLEHAL_RET_ERR_HARDINIT;
//  }

  // Open message queue for sending
  g_sendmsg_handle = McOpenSender(g_appname);
  if (NULL == g_sendmsg_handle) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Open Mc Sender Failed");
//    VclCtlApiClose(&g_sclientobj);
    return VEHICLEHAL_RET_ERR_MCOPEN;
  }

  // create thread
  int iret = pthread_create(&g_threadid,
                          NULL,
                          VehicleHalProcess,
                          NULL);
  if ( 0 != iret ) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Create Thread Failed");
    McClose(g_sendmsg_handle);
//    VclCtlApiClose(&g_sclientobj);
    return VEHICLEHAL_RET_ERR_THREADSTART;
  }

  g_vehiclehalext_thread = FrameworkunifiedCreateChildThread(happ, VEHICLEHAL_EXT,
                                                VehicleHalProcessExtStart,
                                                VehicleHalProcessExtStop);

  EFrameworkunifiedStatus estatus = FrameworkunifiedStartChildThread(happ, g_vehiclehalext_thread, 0, NULL);
  if (eFrameworkunifiedStatusOK != estatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "FrameworkunifiedStartChildThread Failed");
    McClose(g_sendmsg_handle);
    return VEHICLEHAL_RET_ERR_THREADSTART;
  }

  g_vehiclehalpol_thread = FrameworkunifiedCreateChildThread(happ, VEHICLEHAL_POL,
                                                VehicleHalProcessPolStart,
                                                VehicleHalProcessPolStop);

  EFrameworkunifiedStatus estatus_pol = FrameworkunifiedStartChildThread(happ, g_vehiclehalpol_thread, 0, NULL);
  if (eFrameworkunifiedStatusOK != estatus_pol) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "FrameworkunifiedStartChildThread polling Failed");
    McClose(g_sendmsg_handle);
    return VEHICLEHAL_RET_ERR_THREADSTART;
  }


  // set open flag true
  g_isopenflag = TRUE;

  return VEHICLEHAL_RET_SUCCESS;
}

VEHICLEHAL_RET_TYPE VehicleHalStop(HANDLE happ) {
  if (NULL == happ) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Handle is NULL");
    return VEHICLEHAL_RET_ERR_PARAM;
  }

  if (NULL != g_vehiclehalext_thread) {
    EFrameworkunifiedStatus estatus = FrameworkunifiedDestroyChildThread(happ, g_vehiclehalext_thread);
    if (eFrameworkunifiedStatusOK != estatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "FrameworkunifiedDestroyChildThread Failed");
      return VEHICLEHAL_RET_ERR_PARAM;
    }
    g_vehiclehalext_thread = NULL;
  }

  // set open flag of polling to fasle
  g_polloopendflag = TRUE;

  if (NULL != g_vehiclehalpol_thread) {
    EFrameworkunifiedStatus estatus = FrameworkunifiedStopChildThread(happ, g_vehiclehalpol_thread, 0, NULL);
    if (eFrameworkunifiedStatusOK != estatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "FrameworkunifiedDestroyChildThread polling Failed");
      return VEHICLEHAL_RET_ERR_PARAM;
    }
    estatus = FrameworkunifiedDestroyChildThread(happ, g_vehiclehalpol_thread);
    if (eFrameworkunifiedStatusOK != estatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "FrameworkunifiedDestroyChildThread polling Failed");
      return VEHICLEHAL_RET_ERR_PARAM;
    }
    g_vehiclehalpol_thread = NULL;
  }

  // set open flag fasle
  g_isopenflag = FALSE;

  // exit loop
  g_loopendflag = TRUE;

  // destroy thead
  pthread_join(g_threadid, NULL);

  // Close message queue
  McClose(g_sendmsg_handle);

//  // End VLC data input control API
//  VclCtlApiClose(&g_sclientobj);

  return VEHICLEHAL_RET_SUCCESS;
}

void *VehicleHalProcess(void *arg) {
  // Line Sense Message
  VehicleHalLineSensMsg s_linesensmsg;
  memset( &s_linesensmsg, 0xFF, sizeof(s_linesensmsg) );

  // main loop
  while (TRUE != g_loopendflag) {
    char* p_env_vb = NULL;
    p_env_vb = getenv(VEHICLEHAL_LINESENSKIND_VB_DATA);
    if (NULL != p_env_vb) {
      // low voltage
      if (VEHICLEHAL_LOWVB_VALUE >= atoi(p_env_vb)) {
        s_linesensmsg.stdata[VEHICLEHAL_LINESENSKIND_LOWVOLTAGE] = 1;
      } else {
        s_linesensmsg.stdata[VEHICLEHAL_LINESENSKIND_LOWVOLTAGE] = 0;
      }
    }

    // loop to get line sense status
    for (UI_32 iloopnum = 0; iloopnum <= VEHICLEHAL_LINESENSKIND_ILL; ++iloopnum) {
      // line sense status
      UINT8 signalret = 0;

//      // get line sense status
//      UINT32 ret = VclCtlApiRcvVclData(&g_sclientobj, kLineSensList[iloopnum], &signalret);
//      if ( CARSIGNAL_CTL_RET_SUCCESS != ret ) {
//        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Get LineSens [%d]  status faild [%d]", kLineSensList[iloopnum], ret);
//        continue;
//      }

      // convert 0 to 1 or 1 to 0 for IG,REV according car signal api reference
      switch (kLineSensList[iloopnum]) {
//        case CARSIGNAL_IG:
//        case CARSIGNAL_REV:
//          signalret = 1 - signalret;
//          break;

        default:
          break;
      }

      // check status
      if (signalret != s_linesensmsg.stdata[iloopnum]) {
        s_linesensmsg.stdata[iloopnum] = signalret;
      }
    }

    char* p_env_adim = NULL;
    p_env_adim = getenv(VEHICLEHAL_LINESENSKIND_ADIM_DATA);
    if (NULL != p_env_adim) {
      s_linesensmsg.stdata[VEHICLEHAL_LINESENSKIND_ADIM] = atoi(p_env_adim);
    }

    char* p_env_mic = NULL;
    p_env_mic = getenv(VEHICLEHAL_LINESENSKIND_MIC_DATA);
    if (NULL != p_env_mic) {
      s_linesensmsg.stdata[VEHICLEHAL_LINESENSKIND_MIC] = atoi(p_env_mic);
    }

    // send line sens
    EFrameworkunifiedStatus eretval = McSend(g_sendmsg_handle, TN_LINE_SENS_READ, CID_VEHICLEHAL_LINESENS,
                                 sizeof(s_linesensmsg), (PVOID)&s_linesensmsg);
    if (eFrameworkunifiedStatusOK != eretval) {
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "McSend LineSens failed");
    }

    // wait 10ms
    usleep(10000);
  }
  return NULL;
}

EFrameworkunifiedStatus VehicleHalProcessExtStart(HANDLE happ) {
  EFrameworkunifiedStatus estatus = FrameworkunifiedAttachCallbacksToDispatcher(happ, VEHICLEHAL_SPEED,
                                                      kVehicleHalSpd,
                                                      static_cast<UI_32>(_countof(kVehicleHalSpd)));
  if (eFrameworkunifiedStatusOK != estatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__,
               "Error FrameworkunifiedAttachCallbacksToDispatcher Vehicle_Hal_Speed:%d", estatus);
    return estatus;
  }

  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus VehicleHalProcessExtStop(HANDLE happ) {
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus VehicleHalProcessRecvSpeed(HANDLE happ) {
  // Speed Pusle Message
  VehicleHalSpeedPulse speedpulsemsg;
  memset( &speedpulsemsg, 0xFF, sizeof(speedpulsemsg) );

  // get vehicle speed
  UINT16 speed = 0;
  EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusOK;

  estatus = FrameworkunifiedGetMsgDataOfSize(happ, &speed, sizeof(speed), eSMRRelease);
  if (eFrameworkunifiedStatusOK != estatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "failed FrameworkunifiedGetMsgDataOfSize() Status=[%d]", estatus);
    return estatus;
  }

  // calculate speed to speed pulse
  float speedpulse = static_cast<float>(speed * (4.0*637.0) / (60.0 * 60.0));
  if ( speedpulse != speedpulsemsg.pulse ) {
    speedpulsemsg.pulse = speedpulse;
    if (0.0 != speedpulse) {
      speedpulsemsg.period = static_cast<float>((1 / speedpulse) * 1000);
    } else {
      speedpulsemsg.period = 0;
    }
  }

  // send line sens
  EFrameworkunifiedStatus eretval = McSend(g_sendmsg_handle, TN_LINE_SENS_READ, CID_VEHICLEHAL_SPDPULSE,
                               sizeof(speedpulsemsg), (PVOID)&speedpulsemsg);
  if (eFrameworkunifiedStatusOK != eretval) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "McSend Speed Pulse failed");
  }

  eretval = McSend(g_sendmsg_handle, TN_LINE_SENS_READ, CID_VEHICLEHAL_SPD,
                                   sizeof(UINT16), (PVOID)&speed);
  if (eFrameworkunifiedStatusOK != eretval) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "McSend Speed failed");
  }

  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus VehicleHalProcessPolStart(HANDLE happ) {
  VehicleHalPol s_pol;
  memset( &s_pol, 0xFF, sizeof(s_pol) );

  while (!g_polloopendflag) {
    char* p_env_vb = NULL;
    char* p_env_rheostat = NULL;
    p_env_vb = getenv(VEHICLEHAL_LINESENSKIND_VB_DATA);
    p_env_rheostat = getenv(VEHICLEHAL_LINESENSKIND_RHEOSTAT_DATA);
    if (NULL != p_env_vb) {
      // covert voltage value
      s_pol.vb = atoi(p_env_vb);
    }

    if (NULL != p_env_rheostat) {
      // covert RHEOSTAT value
      s_pol.rheostat = atoi(p_env_rheostat);
    }
    // send vb and rheostat
    EFrameworkunifiedStatus eretval = McSend(g_sendmsg_handle, TN_LINE_SENS_READ, CID_VEHICLEHAL_LINESENS_POL,
                                   sizeof(s_pol), (PVOID)&s_pol);
    if (eFrameworkunifiedStatusOK != eretval) {
      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "McSend Vb and RHEOSTAT failed");
    }
    // sleep 700ms
    usleep(700000);
  }
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus VehicleHalProcessPolStop(HANDLE happ) {
  return eFrameworkunifiedStatusOK;
}
