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
 *	SYSTEM		:_CWORD107_
 *	Module configuration	:Canif_DeliveryEntry()			CANDataDelivery registration process
 *			  Canif_TransmissionStart()		CANDataTransmission start processing
 *			  Canif_TransmissionStop()		CANDataPeriodic transmission stop processing
 *			  Canif_CommWatch()			CANDataCommunication interruption monitoring processing (deprecated)
 *			  Canif_CommandCtrl()			CAN command control processing
 *			  CanifMsgHeaderGenerate()		Message header creation process
 *			  CanifSndMsg()			Message transmission processing
 *			  Canif_CommWatchExt()			CANDataCommunication interruption monitoring process
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @file    Canif_API.cpp
 * @~english
 * @brief   CAN I/F API module
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <other_service/strlcpy.h>

#include <native_service/frameworkunified_framework_if.h>

#include <peripheral_service/Canif_API.h>
#include "Canif_API_Local.h"
#include "com_error_type.h"

static __thread HANDLE g_hdl = NULL;
static __thread HANDLE g_mchdl = NULL;

CANIF_RET_API Canif_CWORD29_TransmissionCore(HANDLE h_app, PCSTR notify_name, const uint16_t opc,
                                          uint8_t rid, uint32_t data_size, uint8_t *p_data);

#define NOTIFY_NAME_MAX_SIZE 15
/*******************************************************************************
 * MODULE    : Canif_DeliveryEntry
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Delivery registration of CAN data
 *
 * @~english
 * Delivery registration of CAN data
 *
 * @~english
 * @note Details of process
 *   -# CNAID registration number check
 *   -# Delivery CAN ID check
 *   -# Creation of event
 *   -# Message sending to CAN
 *   -# Result notification event receive
 *   -# Deletion of event
 *
 * @~english
 * @return  Normality/Abnormality
 * @retval  <CANIF_RET_NORMAL>              Normality
 * @retval  <CANIF_RET_ERROR_PARAM>         Abnormality of parameter
 * @retval  <CANIF_RET_ERROR_CREATE_EVENT>  Failure of event create
 *
 * @~english
 * @param[in]     <notifyId>  Snd ID
 * @param[in]     <can_num>   Number of delivery registration CAN ID
 * @param[in]     <*p_can_id> Pointer of delivery registration CAN ID array
 *----------------------------------------------------------------------------*/
CANIF_RET_API Canif_DeliveryEntry(HANDLE h_app, PCSTR notify_name, uint8_t can_num, CANID *p_can_id) {
  return CanifDeliveryEntryCore(h_app, notify_name, can_num, p_can_id, CID_CANIF_DELIVERY_ENTRY);
}

CANIF_RET_API CanifDeliveryEntryCore(HANDLE h_app, PCSTR notify_name, uint8_t can_num, CANID *p_can_id, uint16_t cid) {
  CANIF_RET_API l_ret = CANIF_RET_NORMAL; /* Return value of this function */
  CAN_DELIVERY_ENTRY pst_delivery_entry;   /* CANDataDelivery registration structure */
  int32_t i;                             /* Generic counters */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  if (h_app == (HANDLE)NULL) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }
  if (p_can_id == reinterpret_cast<CANID *>(NULL)) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }

  if (notify_name == (PCSTR)NULL) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }
  if (NOTIFY_NAME_MAX_SIZE < strlen(notify_name)) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }

  /* Checking CNAID registrations*/
  if (((uint8_t)0 == can_num) || ((uint8_t)CAN_DELIVERY_CANID_ENTRY_MAX < can_num)) {
    return CANIF_RET_ERROR_PARAM;                                                     /* Parameter error */
  }
  for (i = 0; i < (int32_t)can_num; i++) {
    if (((CANID)CAN_ID_MASK_CODE & p_can_id[i]) != 0) { /* Parameter error when the upper 3 bits are set to numeric value */
      return CANIF_RET_ERROR_PARAM;
    }
  }

  /* Initialization of transmission data */
  memset(reinterpret_cast<void *>(&pst_delivery_entry), (int32_t)0x00, (size_t)sizeof(pst_delivery_entry));

  /* Delivery registration information setting */
  snprintf(pst_delivery_entry.notifyName, sizeof(pst_delivery_entry.notifyName),
           "%s", notify_name);  /* Destination thread name */
  pst_delivery_entry.usCanNum = (uint16_t)can_num;                       /* CAN ID count */
  memcpy(pst_delivery_entry.ulCanid, p_can_id, sizeof(CANID) * can_num); /* Contain the CAN ID */

  if (g_hdl == NULL) {
    g_hdl = FrameworkunifiedMcOpenSender(h_app, LAN_SERVICE_CAN);
  }
  /* Command delivery registration */
  e_status = FrameworkunifiedSendMsg(g_hdl, cid, sizeof(pst_delivery_entry), &pst_delivery_entry);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_ret = CANIF_RET_ERROR_CANCEL;
  // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    l_ret = CANIF_RET_NORMAL;
  }
  return l_ret;
}

/*******************************************************************************
 * MODULE    : Canif_Diag_CWORD29_DeliveryEntry
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Delivery registration of Phse5data
 *
 * @~english
 * Delivery registration of _CWORD29_data
 *
 * @~english
 * @note Details of process
 *   -# OPC registration number check
 *   -# Creation of event
 *   -# Message sending to CAN
 *   -# Result notification event receive
 *   -# Deletion of event
 *
 * @~english
 * @return  Normality/Abnormality
 * @retval  <CANIF_RET_NORMAL>              Normality
 * @retval  <CANIF_RET_ERROR_PARAM>         Abnormality of parameter
 * @retval  <CANIF_RET_ERROR_CREATE_EVENT>  Failure of event create
 *
 * @~english
 * @param[in]     <PCSTR>     Notification name
 * @param[in]     <can_num>   Number of delivery registration OPC
 * @param[in]     <*p_can_id> Pointer of delivery registration OPC array
 */
CANIF_RET_API Canif_Diag_CWORD29_DeliveryEntry(const HANDLE h_app, const PCSTR notify_name,
                                            const uint8_t opc_num, const uint16_t* const p_opc) {
  if (reinterpret_cast<HANDLE>(NULL) == h_app) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }

  if (reinterpret_cast<uint16_t *>(NULL) == p_opc) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }

  if (reinterpret_cast<PCSTR>(NULL) == notify_name) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }

  if ((size_t)NOTIFY_NAME_MAX_SIZE < strnlen(notify_name, (size_t)NOTIFY_NAME_MAX_SIZE+(size_t)1)) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }

  return Canif_CWORD29_DeliveryEntryCore(h_app, notify_name, opc_num, p_opc,
                                      static_cast<const uint16_t>(CID_CANIF__CWORD29__DELIVERY_ENTRY));
}

CANIF_RET_API Canif_CWORD29_DeliveryEntryCore(const HANDLE h_app, const PCSTR notify_name,
                                           const uint8_t opc_num, const uint16_t* const p_opc, const uint16_t cid) {
  CANIF_RET_API l_ret = CANIF_RET_NORMAL;         /* Return value of this function */
  CAN__CWORD29__DELIVERY_ENTRY pst_delivery_entry;   /* _CWORD29_ data-delivery registry structures */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  size_t copysize;

  /* OPC registration number check*/
  if (static_cast<uint8_t>(0) == opc_num) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }

  copysize = sizeof(uint16_t) * static_cast<const size_t>(opc_num);

  /* Initialization of transmission data */
  memset(reinterpret_cast<void *>(&pst_delivery_entry), (int32_t)0x00,
         static_cast<size_t>(sizeof(CAN__CWORD29__DELIVERY_ENTRY)));

  /* Delivery registration information setting */
  snprintf(reinterpret_cast<char *>(pst_delivery_entry.notifyName), (size_t)MAX_NAME_SIZE_APP,
           "%s", reinterpret_cast<const char *>(notify_name));         /* Destination thread name */
  pst_delivery_entry.usOpcNum = static_cast<uint16_t>(opc_num);        /* Number of OPCs */
  memcpy(reinterpret_cast<void *>(pst_delivery_entry.usOpc), reinterpret_cast<const void *>(p_opc),
         copysize);  /* Install OPC */

  if (reinterpret_cast<HANDLE>(NULL) == g_hdl) {
    g_hdl = FrameworkunifiedMcOpenSender(h_app, LAN_SERVICE_CAN);
  }
  /* Command delivery registration */
  e_status = FrameworkunifiedSendMsg(g_hdl, static_cast<UI_32>(cid), sizeof(CAN__CWORD29__DELIVERY_ENTRY),
                        reinterpret_cast<void *>(&pst_delivery_entry));
  if (eFrameworkunifiedStatusOK != e_status) {
    l_ret = CANIF_RET_ERROR_CANCEL;
  } else {
    l_ret = CANIF_RET_NORMAL;
  }
  return l_ret;
}

/*******************************************************************************
 * MODULE    : Canif_TransmissionStart
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    CAN data transmission starting
 *
 * @~english
 * The transmission of the CAN data starts(Regular/One)
 *
 * @~english
 * @note Details of process
 *   -# DLC check
 *   -# CAN ID check
 *   -# Creation of event
 *   -# Message sending to CAN
 *   -# Result notification event receive
 *   -# Deletion of event
 *
 * @~english
 * @return  Normality/Abnormality
 * @retval  <CANIF_RET_NORMAL>              Normality
 * @retval  <CANIF_RET_ERROR_PARAM>         Abnormality of parameter
 * @retval  <CANIF_RET_ERROR_CREATE_EVENT>  Failure of event create
 *
 * @~english
 * @param[in]     <notifyId>Snd ID
 * @param[in]     <rid>     Resource ID for CAN data transmission result notification
 * @param[in]     <freq>    Cycle of regular transmission
 * @param[in]     <*p_data> Pointer of transmission data
 *----------------------------------------------------------------------------*/
CANIF_RET_API Canif_TransmissionStart(HANDLE h_app, PCSTR notify_name, uint8_t rid, uint16_t freq, CAN_DATA *p_data) {
  CANIF_RET_API l_ret = CANIF_RET_NORMAL;               /* Return value of this function */
  CAN_TRANSMISSION_START_MSG_DAT pst_transmission_start; /* CANDataSending structure */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  if (h_app == (HANDLE)NULL) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }
  if (p_data == reinterpret_cast<CAN_DATA *>(NULL)) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }
  if (rid != (uint8_t)0xff) {
    if (notify_name == (PCSTR)NULL) {
      return CANIF_RET_ERROR_PARAM; /* Parameter error */
    }
    if (NOTIFY_NAME_MAX_SIZE < strlen(notify_name)) {
      return CANIF_RET_ERROR_PARAM; /* Parameter error */
    }
  }

  /* DLC check*/
  if ((uint8_t)CAN_TXDATA_SIZE < (p_data->dlc)) { /* Is the DLC over the transmission data size? */
    return CANIF_RET_ERROR_PARAM;
  }
  /* CAN ID checking */
  if (((CANID)CAN_ID_MASK_CODE & p_data->can_id) != 0) { /* Parameter error when the upper 3 bits are set to numeric value */
    return CANIF_RET_ERROR_PARAM;
  }

  /* CANDataInitialization of transmission start information */
  memset(reinterpret_cast<void *>(&pst_transmission_start), (int32_t)0x00, (size_t)sizeof(pst_transmission_start));

  /* CANDataTransmission start information setting */
  if (rid != (uint8_t)0xff) {
    snprintf(pst_transmission_start.notifyName, sizeof(pst_transmission_start.notifyName),
             "%s", notify_name); /* Destination thread name */
  } else {
    snprintf(pst_transmission_start.notifyName, sizeof(pst_transmission_start.notifyName), "%s", "");
  }

  pst_transmission_start.ucRid = rid;                              /* Resources ID */
  pst_transmission_start.usFreq = freq;                            /* Periodic transmission cycle */
  memcpy(reinterpret_cast<void *>(&pst_transmission_start.stCandata),
         reinterpret_cast<int8_t *>(p_data), (size_t)sizeof(CAN_DATA)); /* CANData */

  if (g_hdl == NULL) {
    g_hdl = FrameworkunifiedMcOpenSender(h_app, LAN_SERVICE_CAN);
  }

  /* CAN data transmission start */
  e_status = FrameworkunifiedSendMsg(g_hdl, CID_CANIF_TX_START, sizeof(pst_transmission_start), &pst_transmission_start);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_ret = CANIF_RET_ERROR_CANCEL;
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    l_ret = CANIF_RET_NORMAL;
  }
  return l_ret;
}

/*******************************************************************************
 * MODULE    : Canif_TransmissionStop
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    CAN data regular transmission stop
 *
 * @~english
 * CAN data regular transmission stop
 *
 * @~english
 * @note Details of process
 *   -# Creation of event
 *   -# Message sending to CAN
 *   -# Result notification event receive
 *   -# Deletion of event
 *
 * @~english
 * @return  Normality/Abnormality
 * @retval  <CANIF_RET_NORMAL>              Normality
 * @retval  <CANIF_RET_ERROR_CREATE_EVENT>  Failure of event create
 * @retval  <CANIF_RET_ERROR_CREATE_EVENT>  Event generation failure
 *
 * @~english
 * @param[in]     <notifyId>Snd ID
 * @param[in]     <can_id>  CAN ID
 *----------------------------------------------------------------------------*/
CANIF_RET_API Canif_TransmissionStop(HANDLE h_app, PCSTR notify_name, CANID can_id) {
  return CanifTransmissionStopCore(h_app, notify_name, can_id, CID_CANIF_TX_STOP);
}

CANIF_RET_API CanifTransmissionStopCore(HANDLE h_app, PCSTR notify_name, CANID can_id, uint16_t cid) {
  CANIF_RET_API l_ret = CANIF_RET_NORMAL;        /* Return value of this function */
  CAN_FREQ_TRANS_STOP_MSG_DAT pst_freq_trans_stop; /* CANDataPeriodic transmission stop structure */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  if (h_app == (HANDLE)NULL) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }
  /* CAN ID checking */
  if (((CANID)CAN_ID_MASK_CODE & can_id) != 0) {
    /* Parameter error when the upper 3 bits are set to numeric value */
    return CANIF_RET_ERROR_PARAM;
  }

  if (notify_name == (PCSTR)NULL) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }
  if (NOTIFY_NAME_MAX_SIZE < strlen(notify_name)) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }

  /* CANDataInitialization of periodic transmission stop information */
  memset(reinterpret_cast<void *>(&pst_freq_trans_stop), (int32_t)0x00, (size_t)sizeof(pst_freq_trans_stop));

  /* CANDataPeriodic transmission stop information setting */
  snprintf(pst_freq_trans_stop.notifyName, sizeof(pst_freq_trans_stop.notifyName),
           "%s", notify_name); /* Destination thread name */
  pst_freq_trans_stop.ulCanid = can_id;               /* CAN ID */

  if (g_hdl == NULL) {
    g_hdl = FrameworkunifiedMcOpenSender(h_app, LAN_SERVICE_CAN);
  }
  /* CAN data transmission start */
  e_status = FrameworkunifiedSendMsg(g_hdl, cid, sizeof(pst_freq_trans_stop), &pst_freq_trans_stop);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_ret = CANIF_RET_ERROR_CANCEL;
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    l_ret = CANIF_RET_NORMAL;
  }
  return l_ret;
}

/*******************************************************************************
 * MODULE    : Canif_CommWatch
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    CAN data regular transmission stop
 *
 * @~english
 * CAN data regular transmission stop
 *
 * @~english
 * @note Details of process
 *   -# Parameter check
 *   -# Creation of event
 *   -# Message sending to CAN
 *   -# Result notification event receive
 *   -# Deletion of event
 *
 * @~english
 * @return  Normality/Abnormality
 * @retval  <CANIF_RET_NORMAL>              Normality
 * @retval  <CANIF_RET_ERROR_PARAM>         Abnormality of parameter
 * @retval  <CANIF_RET_ERROR_CREATE_EVENT>  Failure of event create
 *
 * @~english
 * @param[in]     <pid>        Thread ID
 * @param[in]     <can_id>     CAN ID
 * @param[in]     <did>        Data ID
 * @param[in]     <watch_time> Communication watch suspension time(Unit of 100ms)
 *----------------------------------------------------------------------------*/
CANIF_RET_API Canif_CommWatch(HANDLE h_app, PCSTR notify_name, CANID can_id, DID did, uint16_t watch_time) {
  return CanifCommWatchCore(h_app, notify_name, can_id, did, CAN_IG_COOPERATION_OFF, watch_time, CID_CANIF_COMM_WATCH);
}

/*******************************************************************************
 * MODULE    : Canif_CommandCtrl
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    CAN command control
 *
 * @~english
 * Transmission control of CAN command
 *
 * @~english
 * @note Details of process
 *   -# Parameter check
 *   -# Creation of event
 *   -# Message sending to CAN
 *   -# Result notification event receive
 *   -# Deletion of event
 *
 * @~english
 * @return  Normality/Abnormality
 * @retval  <CANIF_RET_NORMAL>         Normality
 * @retval  <CANIF_RET_ERROR_PARAM>    Abnormality of parameter
 * @retval  <CANIF_RET_ERROR_CREATE_EVENT>  Failure of event create
 *
 * @~english
 * @param[in]     <notifyId> Snd ID
 * @param[in]     <rid>      Resource ID
 * @param[in]     <cmd_id>   CAN command ID(32bit)
 *----------------------------------------------------------------------------*/
CANIF_RET_API Canif_CommandCtrl(HANDLE h_app, PCSTR notify_name, uint8_t rid, uint32_t cmd_id) {
  /* Parameter anomaly check */
  if (((uint8_t)CAN_CMDID_FUELCALC_RST_REQ_DELIVERY != (uint8_t)cmd_id) && /* Request for delivery of request for flame reset in CAN section */
      ((uint8_t)CAN_CMDID_STARTUP_FIN_REQ_TX != (uint8_t)cmd_id) &&    /* CAN start completion notification request transmission */
      ((uint8_t)CAN_CMDID_MRST_INFO_REQ_TX != (uint8_t)cmd_id) &&      /* CAN master reset information notification request transmission */
      ((uint8_t)CAN_CMDID_VERSION_REQ_TX != (uint8_t)cmd_id) &&        /* CAN Version Request Send */
      ((uint8_t)CAN_CMDID_CONNECTION_NODE_REQ_TX != (uint8_t)cmd_id) &&  /* CAN connection node notification request transmission */
      ((uint8_t)CAN_CMDID_FUELCALC_REQ_TX != (uint8_t)cmd_id)) {         /* CAN section flame reset response transmission */
    return CANIF_RET_ERROR_PARAM; /* CAN Thread-> When it is not a CAN command to be sent to the SYS microcomputer */
  }

  return CanifCommandCtrlCore(h_app, notify_name, rid, cmd_id, CID_CANIF_CMD_CTRL);
}

CANIF_RET_API CanifCommandCtrlCore(HANDLE h_app, PCSTR notify_name, uint8_t rid, uint32_t cmd_id, uint16_t cid) {
  CANIF_RET_API l_ret = CANIF_RET_NORMAL; /* Return value of this function */
  CAN_CMD_CTRL_MSG_DAT pst_cmd_ctrl_msg;    /* CAN command control message data structure */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  /* Initialization of CAN command control information */
  memset(reinterpret_cast<void *>(&pst_cmd_ctrl_msg), (int32_t)0x00, (size_t)sizeof(pst_cmd_ctrl_msg));

  if (h_app == (HANDLE)NULL) {
    /* Parameter error */
    return CANIF_RET_ERROR_PARAM;
  }
  if (rid != 0xFF) {
    if (notify_name == (PCSTR)NULL) {
      /* Parameter error */
      return CANIF_RET_ERROR_PARAM;
    }
    if (NOTIFY_NAME_MAX_SIZE < strlen(notify_name)) {
      /* Parameter error */
      return CANIF_RET_ERROR_PARAM;
    }
  }

  /* Setting CAN command control information */
  snprintf(pst_cmd_ctrl_msg.notifyName, sizeof(pst_cmd_ctrl_msg.notifyName),
           "%s", notify_name); /* Destination thread name */
  pst_cmd_ctrl_msg.ucRid = rid;                    /* Resources ID */
  pst_cmd_ctrl_msg.ucCmdid = (uint8_t)cmd_id;               /* CAN command ID */

  if (g_hdl == NULL) {
    g_hdl = FrameworkunifiedMcOpenSender(h_app, LAN_SERVICE_CAN);
  }

  /* CAN data transmission start */
  e_status = FrameworkunifiedSendMsg(g_hdl, cid, sizeof(pst_cmd_ctrl_msg), &pst_cmd_ctrl_msg);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_ret = CANIF_RET_ERROR_CANCEL;
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    l_ret = CANIF_RET_NORMAL;
  }

  return l_ret;
}

CANIF_RET_API CanifCommWatchCore(HANDLE h_app, PCSTR notify_name, CANID can_id, DID did,
                                 uint8_t ig_cooperation, uint16_t watch_time, uint16_t cid) {
  CANIF_RET_API l_ret = CANIF_RET_NORMAL;     /* Return value of this function */
  CAN_COMM_WATCHEXT_MSG_DAT pst_comm_watch_msg; /* CANDataCommunication disruption registration structure */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  if (h_app == (HANDLE)NULL) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }
  if (notify_name == (PCSTR)NULL) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }
  if (NOTIFY_NAME_MAX_SIZE < strlen(notify_name)) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }

  /* Parameter anomaly check */
  if (((CANID)CAN_ID_MASK_CODE & can_id) != 0) { /* Parameter error when the upper 3 bits are set to numeric value */
    return CANIF_RET_ERROR_PARAM;                                                                 /* Parameter error */
  }

  /* CANDataInitialization of communication disruption monitoring information */
  memset(reinterpret_cast<void *>(&pst_comm_watch_msg), (int32_t)0x00, (size_t)sizeof(pst_comm_watch_msg));

  /* CANDataSetting of communication interruption monitoring information */
  strlcpy(pst_comm_watch_msg.notifyName, notify_name, sizeof(pst_comm_watch_msg.notifyName)); /* Destination thread name */
  pst_comm_watch_msg.ulCanid = can_id;                                       /* CAN ID */
  pst_comm_watch_msg.ulDid = did;                                            /* Data ID */
  pst_comm_watch_msg.ucIg = ig_cooperation;                                  /* IG linkage */
  pst_comm_watch_msg.usWatchTime = watch_time;                               /* Communication interruption monitoring time (in units of 100ms) */

  if (g_hdl == NULL) {
    g_hdl = FrameworkunifiedMcOpenSender(h_app, LAN_SERVICE_CAN);
  }
  /* CANDataCommunication disruption monitoring */
  e_status = FrameworkunifiedSendMsg(g_hdl, cid, sizeof(pst_comm_watch_msg), &pst_comm_watch_msg);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_ret = CANIF_RET_ERROR_CANCEL;
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    l_ret = CANIF_RET_NORMAL;
  }

  return l_ret;
}

/*******************************************************************************
 * MODULE    : Canif_Diag_CWORD29_Transmission
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Transmission of CAN data (_CWORD29_) transmission notification
 *
 * @~english
 * Transmission of CAN data (_CWORD29_) transmission notification
 *
 * @~english
 * @note Details of process
 *   -# Maximum check
 *   -# CAN ID check
 *   -# Creation of event
 *   -# Message sending to CAN
 *   -# Result notification event receive
 *   -# Deletion of event
 *
 * @~english
 * @return  Normality/Abnormality
 * @retval  <CANIF_RET_NORMAL>              Normality
 * @retval  <CANIF_RET_ERROR_PARAM>         Abnormality of parameter
 * @retval  <CANIF_RET_ERROR_BUFFULL>       Buffer fully
 * @retval  <CANIF_RET_ERROR_CREATE_EVENT>  Failure of event create
 *
 * @~english
 * @param[in]     <notifyId>  Snd ID
 * @param[in]     <opc>       OPC
 * @param[in]     <rid>       Resource ID
 * @param[in]     <data_size> Data size
 * @param[in]     <*p_data>   Pointer of transmission data
 */
CANIF_RET_API Canif_Diag_CWORD29_Transmission(HANDLE h_app, PCSTR notify_name, const uint16_t opc,
                                           uint8_t rid, uint32_t data_size, uint8_t *p_data) {
  return Canif_CWORD29_TransmissionCore(h_app, notify_name, opc, rid, data_size, p_data);
}

CANIF_RET_API Canif_CWORD29_TransmissionCore(HANDLE h_app, PCSTR notify_name, const uint16_t opc,
                                          uint8_t rid, uint32_t data_size, uint8_t *p_data) {
  CANIF_RET_API l_ret = CANIF_RET_NORMAL;              /* Return value of this function */
  CAN__CWORD29__TRANS_MSG pst__CWORD29__trans;         /* CANDataPointer to the send structure */
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  if (h_app == (HANDLE)NULL) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }

  if (rid != 0xFF) {
    if (notify_name == (PCSTR)NULL) {
      return CANIF_RET_ERROR_PARAM;
    }

    if (strlen(const_cast<char *>(notify_name)) > NOTIFY_NAME_MAX_SIZE) {
      return CANIF_RET_ERROR_PARAM;
    }
  }

  if (p_data == reinterpret_cast<uint8_t *>(NULL)) {
    return CANIF_RET_ERROR_PARAM; /* Parameter error */
  }

  /* Maximum Size Check */
  if (CAN__CWORD29__MEM_SZ < data_size) {
    return CANIF_RET_ERROR_PARAM;
  }

  /* CANDataInitialization of transmission start information */
  memset(reinterpret_cast<void *>(&pst__CWORD29__trans), (int32_t)0x00, (size_t)sizeof(pst__CWORD29__trans));

  /* CANDataTransmission start information setting */
  if (rid != (uint8_t)0xff) {
    snprintf(pst__CWORD29__trans.notifyName, sizeof(pst__CWORD29__trans.notifyName), "%s", notify_name);
  } else {
    snprintf(pst__CWORD29__trans.notifyName, sizeof(pst__CWORD29__trans.notifyName), "%s", "");
  }

  pst__CWORD29__trans.ucRid = rid;                              /* Resources ID */
  pst__CWORD29__trans.ulDataSize = data_size;
  pst__CWORD29__trans.opc = opc;
  memcpy(reinterpret_cast<void *>(&pst__CWORD29__trans.data), reinterpret_cast<int8_t *>(p_data), data_size);

  if (g_hdl == NULL) {
    g_hdl = FrameworkunifiedMcOpenSender(h_app, LAN_SERVICE_CAN);
  }

  /* CAN data transmission start */
  e_status = FrameworkunifiedSendMsg(g_hdl, CID_CANIF__CWORD29__TX_START, sizeof(pst__CWORD29__trans), &pst__CWORD29__trans);
  if (e_status != eFrameworkunifiedStatusOK) {
    l_ret = CANIF_RET_ERROR_CANCEL;
  } else {
    l_ret = CANIF_RET_NORMAL;
  }
  return l_ret;
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Transmission CAN Command
 *
 * @~english
 * Start Transmission CAN Command API
 *
 * @~english
 * @return  Return value
 * @retval  <CANIF_RET_NORMAL>            OK
 * @retval  <CANIF_RET_ERROR_CANCEL>      Abnormal termination
 * @retval  <CANIF_RET_ERROR_PARAM>       Parameter Error
 *
 * @~english
 * @param[in]     <h_app>    Application handle
 * @param[in]     <can_id>  CAN ID
 * @param[in]     <mask>    Mask Data
 * @param[in]     <dat>     Transmission Data
 * @param[in]     <freq>    Transmission Cycle
 *----------------------------------------------------------------------------*/
CANIF_RET_API Canif_TransStart(HANDLE h_app, CANID can_id, CAN_DATA_MASK *mask, CAN_DATA_BIT *dat, uint32_t freq) {
  CANIF_RET_API l_ret = CANIF_RET_NORMAL;
  CAN_TRANS_START_MSG_DAT pst_trans_start;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  /* Check parameter */
  if (h_app == (HANDLE)NULL || can_id == (CANID)NULL || mask == reinterpret_cast<CAN_DATA_MASK *>(NULL) ||
      dat == reinterpret_cast<CAN_DATA_BIT *>(NULL) || (uint32_t)0xFFFF < freq) {
    return CANIF_RET_ERROR_PARAM;
  }
  if (((CANID)CAN_ID_MASK_CODE & can_id) != 0) { /* Parameter error when the upper 3 bits are set to numeric value */
    return CANIF_RET_ERROR_PARAM;
  }

  memset(reinterpret_cast<void *>(&pst_trans_start), (int32_t)0x00, (size_t)sizeof(pst_trans_start));

  /* Create Transmission Data */
  pst_trans_start.id = can_id; /* CAN ID */
  pst_trans_start.freq = freq; /* Transmission Interval */
  memcpy(&pst_trans_start.mask.dat, mask->dat, sizeof(mask->dat));
  memcpy(&pst_trans_start.dat.dat, dat->dat, sizeof(dat->dat));

  if (g_hdl == NULL) {
    g_hdl = FrameworkunifiedMcOpenSender(h_app, LAN_SERVICE_CAN);
  }

  /* Start CAN Data transmission */
  e_status = FrameworkunifiedSendMsg(g_hdl, CID_CANIF_TX_BIT_START, sizeof(pst_trans_start), &pst_trans_start);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_ret = CANIF_RET_ERROR_CANCEL;
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    l_ret = CANIF_RET_NORMAL;
  }
  return l_ret;
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Stop periodic transmission of CAN Command
 *
 * @~english
 * Stop periodic Transmission of CAN Command API
 *
 * @~english
 * @return  Return value
 * @retval  <CANIF_RET_NORMAL>            OK
 * @retval  <CANIF_RET_ERROR_CANCEL>      Abnormal termination
 * @retval  <CANIF_RET_ERROR_PARAM>       Parameter Error
 *
 * @~english
 * @param[in]     <h_app>    Application handle
 * @param[in]     <can_id>  CAN ID
 *----------------------------------------------------------------------------*/
CANIF_RET_API Canif_TransStop(HANDLE h_app, CANID can_id) {
  CANIF_RET_API l_ret = CANIF_RET_NORMAL;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  /* Check parameter */
  if (h_app == (HANDLE)NULL || can_id == (CANID)NULL) {
    return CANIF_RET_ERROR_PARAM;
  }
  if (((CANID)CAN_ID_MASK_CODE & can_id) != 0) { /* Parameter error when the upper 3 bits are set to numeric value */
    return CANIF_RET_ERROR_PARAM;
  }

  if (g_hdl == NULL) {
    g_hdl = FrameworkunifiedMcOpenSender(h_app, LAN_SERVICE_CAN);
  }
  /* Start CAN ID transmission */
  e_status = FrameworkunifiedSendMsg(g_hdl, CID_CANIF_TX_BIT_STOP, sizeof(CANID), &can_id);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_ret = CANIF_RET_ERROR_CANCEL;
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    l_ret = CANIF_RET_NORMAL;
  }
  return l_ret;
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Transmission CAN Command(for OpeningMovie)
 *
 * @~english
 * CAN Command Send API
 *
 * @~english
 * @return  Return value
 * @retval  <CANIF_RET_NORMAL>            OK
 * @retval  <CANIF_RET_ERROR_CANCEL>      Abnormal termination
 * @retval  <CANIF_RET_ERROR_PARAM>       Parameter Error
 *
 * @~english
 * @param[in]     <can_id>  CAN ID
 * @param[in]     <mask>    Mask Data
 * @param[in]     <dat>     Transmission Data
 *----------------------------------------------------------------------------*/
CANIF_RET_API Canif_Send(CANID can_id, CAN_DATA_MASK *mask, CAN_DATA_BIT *dat) {
  CANIF_RET_API l_ret = CANIF_RET_NORMAL;
  CAN_TRANS_START_MSG_DAT pst_trans_start;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  /* Check parameter */
  if (can_id == (CANID)NULL || mask == NULL || dat == NULL) {
    return CANIF_RET_ERROR_PARAM;
  }
  if (((CANID)CAN_ID_MASK_CODE & can_id) != 0) { /* Parameter error when the upper 3 bits are set to numeric value */
    return CANIF_RET_ERROR_PARAM;
  }

  memset(reinterpret_cast<void *>(&pst_trans_start), (int32_t)0x00, (size_t)sizeof(pst_trans_start));

  /* Create Transmission Data */
  pst_trans_start.id = can_id; /* CAN ID */
  pst_trans_start.freq = 0;    /* Transmission Interval */
  memcpy(&pst_trans_start.mask.dat, mask->dat, sizeof(mask->dat));
  memcpy(&pst_trans_start.dat.dat, dat->dat, sizeof(dat->dat));

  if (g_mchdl == NULL) {
    g_mchdl = McOpenSender(LAN_SERVICE_CAN);
  }

  /* Start CAN Data transmission */
  e_status = McSend(g_mchdl, "ViewerApp", CID_CANIF_TX_BIT_START, sizeof(pst_trans_start), &pst_trans_start);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_ret = CANIF_RET_ERROR_CANCEL;
    // LCOV_EXCL_STOP 4: NSFW error case.
  } else {
    l_ret = CANIF_RET_NORMAL;
  }
  return l_ret;
}

#ifdef CAN_DEBUG
CANIF_RET_API Canif_Debug_Delete_AllDeliveryEntryList(HANDLE h_app) {
  CANIF_RET_API l_ret = CANIF_RET_NORMAL;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  UI_32 len = 0;

  /* Check parameter */
  if (h_app == (HANDLE)NULL) {
    return CANIF_RET_ERROR_PARAM;
  }

  if (g_hdl == NULL) {
    g_hdl = FrameworkunifiedMcOpenSender(h_app, LAN_SERVICE_CAN);
  }
  /* Start CAN ID transmission */
  e_status = FrameworkunifiedInvokeSync(g_hdl, CID_CANIF_DELETE_DELIVERY_ENTRY, 0, NULL, 0, NULL, &len);
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_ret = CANIF_RET_ERROR_CANCEL;  // LCOV_EXCL_LINE 4: NSFW error case.
  } else {
    l_ret = CANIF_RET_NORMAL;
  }
  return l_ret;
}
#endif
// LCOV_EXCL_BR_LINE 10:THE_END_LINE_OF_THE_FILE
