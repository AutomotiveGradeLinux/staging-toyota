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
 *	FILE		:CAN_TransmissionData.cpp
 *	SYSTEM		:_CWORD107_
 *	SUBSYSTEM	:EXL process
 *	PROGRAM	:CAN Thread CAN Data Transmission Management Processing
 *	Module configuration	:CANTransmissionDataInit( )		CANDataTransmission management data initialization processing
 *                    CANFreqTransEntryCheck( )		CANDataPeriodic transmission management table registration destination confirmation processing
 *                    CANFreqTransEntry()				CANDataRegular transmission management table registration processing
 *                    CANFreqTransDataGet( )			Periodic transmission data acquisition processing
 *                    CANFreqTransCanidEntryCheck( )	CANDataPeriodic transmission control table registration status checking process (CAN ID retrieval)
 *                    CANFreqTransIdEntryCheck( )		Periodic transmission management table send destination ID confirmation processing
 *                    CANFreqTransStop( )				CANDataPeriodic transmission stop processing
 *                    CANFreqTransIndexEntryCheck( )	CANDataPeriodic transmission control table registration status checking process (INDEX retrieval)
 *                    CAN1TimeTransEntryCheck( )		CANDataConfirmation processing of the registration destination of the transmission result management table
 *                    CAN1TimeTransEntry( )			CANDataOne-time transmission result management table registration processing
 *                    CANTxRsltEntryCheck( )			CANDataTransmission result management table registration status confirmation processing
 *                    CANTxRsltDataGet( )				Transmission result data acquisition processing
 *                    CANTxRsltFin( )					CANDataTransmission result notification completion processing
 ******************************************************************************/
#include "CAN_TransmissionData.h"
#include <string.h>  // NOLINT(build/include)
#include <other_service/strlcpy.h>
#include <map>
#include "CAN_Thread.h"
#include "CAN_TxMsg.h"
#include "communication_communicationlog.h"

using std::map;

/*************************************************/
/*           Global variable                      */
/*************************************************/
static CAN_FREQ_TRANSMISSION_DAT  g_gst_freq_trans_dat[CAN_FREQ_TRSNSMISSION_LIST_NUM];  /* Periodic transmission management table */
  /* One-time transmission result management table */
static CAN_1TIME_TXRSLT_DAT g_gst1_time_trans_rslt_dat[CAN_1TIME_TRSNSMISSION_RSLT_NUM];  //NOLINT (readability/naming)
  /* _CWORD29_ send result control table */
static CAN__CWORD29__TXRSLT_DAT g_gst__CWORD29__trans_rslt_dat[CAN__CWORD29__TRSNSMISSION_RSLT_NUM];  //NOLINT (readability/naming)
map<CANID, CAN_TRANS_START_TABLE_VAL *> g_map_trans_data;  /* CAN Data management table */
std::multimap<CANID, CAN_TRANS_START_TABLE_VAL *> mapSubIDTransData; /* CAN SubID Data management table */
static CAN_INIT_TABLE Can_TransInitData_19PF[] = {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
    { (CANID)0x0000, (uint8_t)1, { 0x00 } }
};

/*******************************************************************************
 * MODULE    : :CANTransmissionDataInit
 * ABSTRACT  : CANDataTransmission management data initialization processing
 * FUNCTION  : CANDataInitializing Transmission Management Data 
 * ARGUMENT  : void
 * NOTE      : 
 * RETURN    : void
 ******************************************************************************/
void CANTransmissionDataInit(void) {
  int32_t i; /* Generic counters */
  int32_t cntMaxData;

  (void)memset(reinterpret_cast<void *>(&g_gst_freq_trans_dat), (int32_t)0, (size_t)sizeof(g_gst_freq_trans_dat));
  /* Periodic transmission management table */
  (void)memset(reinterpret_cast<void *>(&g_gst1_time_trans_rslt_dat),
               (int32_t)0, (size_t)sizeof(g_gst1_time_trans_rslt_dat));
  /* One-time transmission result management table */
  /* Fill the resource ID of the transmission result management table with an unused code */
  for (i = 0; i < (int32_t)CAN_1TIME_TRSNSMISSION_RSLT_NUM; i++) {
     g_gst1_time_trans_rslt_dat[i].uc_rid = (uint8_t)CAN_RID_NOTUSE_CODE;
  }

  (void)memset(reinterpret_cast<void *>(&g_gst__CWORD29__trans_rslt_dat),
               (int32_t)0, (size_t)sizeof(g_gst__CWORD29__trans_rslt_dat));
  /* _CWORD29_ send result control table */
  /* Fill the resource ID of the Phase send result management table with an unused resource ID */
  for (i = 0; i < (int32_t)CAN__CWORD29__TRSNSMISSION_RSLT_NUM; i++) {
    g_gst__CWORD29__trans_rslt_dat[i].uc_rid = (uint8_t)CAN_RID_NOTUSE_CODE;
  }

  /* Initialize Transmission data management table */
  cntMaxData = (int32_t)(sizeof(Can_TransInitData_19PF) / sizeof(Can_TransInitData_19PF[0]));
  for (i = 0; i < cntMaxData; i++) {
    g_map_trans_data[ Can_TransInitData_19PF[i].canid ] = &Can_TransInitData_19PF[i].val;
  }

  return;
}

/*******************************************************************************
 * MODULE    : CANFreqTransEntryCheck
 * ABSTRACT  : CANDataPeriodic transmission management table registration destination confirmation processing
 * FUNCTION  : CANDataReturns whether or not the periodic transmission management table can be registered and the index. 
 * ARGUMENT  : can_id			: CAN ID
 *             *puc_index		: Index (pointer output)
 * NOTE      : Can be overwritten with the same CAN ID
 * RETURN    : TRUE				: Can be registered
 *			   FALSE			: Cannot be stored
 ******************************************************************************/
BOOL CANFreqTransEntryCheck(CANID can_id, uint8_t *puc_index) {
  BOOL ret = FALSE; /* Return value of this function */
  int32_t i;         /* Generic counters */

  /* Can be overwritten by the same CAN ID detection */
  for (i = 0; i < (int32_t)CAN_FREQ_TRSNSMISSION_LIST_NUM; i++) {
    if (g_gst_freq_trans_dat[i].st_can_data.can_id == can_id) {
      ret = TRUE;            /* Can be registered */
      *puc_index = (uint8_t)i; /* Index acquisition */
      break;
    }
  }

  if (FALSE == ret) {
    /* If the same CAN ID is not found, free space is checked. */
    for (i = 0; i < (int32_t)CAN_FREQ_TRSNSMISSION_LIST_NUM; i++) {
      if ((uint16_t)0 == g_gst_freq_trans_dat[i].us_freq) {
        ret = TRUE;            /* Can be registered */
        *puc_index = (uint8_t)i; /* Index acquisition */
        break;
      }
    }
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANFreqTransEntry
 * ABSTRACT  : CANDataRegular transmission management table registration processing
 * FUNCTION  : CANDataRegister in the periodic transmission management table 
 * ARGUMENT  : uc_index          : Indexed
 *             *pst_msg_data      : Receive message data section reference pointer
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void CANFreqTransEntry(uint8_t uc_index, const CAN_TRANSMISSION_START_MSG_DAT *pst_msg_data) {
  strlcpy(g_gst_freq_trans_dat[uc_index].notify_name, pst_msg_data->notifyName,
          sizeof(g_gst_freq_trans_dat[uc_index].notify_name));
  g_gst_freq_trans_dat[uc_index].us_freq = pst_msg_data->usFreq; /* Periodic transmission period (in 100ms) */
  g_gst_freq_trans_dat[uc_index].uc_rid = pst_msg_data->ucRid;   /* Resources ID */
  if ((uint8_t)CAN_RID_NOTUSE_CODE == pst_msg_data->ucRid) {
      g_gst_freq_trans_dat[uc_index].uc_resp_num = 0; /* Notification of transmission results is not required. */
  } else {
      g_gst_freq_trans_dat[uc_index].uc_resp_num = (uint8_t)CAN_FREQ_TXRSLT_NUM; /* Number of transmission result notifications (number of remaining notifications) */
  }
  g_gst_freq_trans_dat[uc_index].st_can_data = pst_msg_data->stCandata; /* Transmitted data */

  return;
}

/*******************************************************************************
 * MODULE    : CANFreqTransDataGet
 * ABSTRACT  : Periodic transmission data acquisition processing
 * FUNCTION  : CANDataRetrieves data for the specified index from the periodic transmission management table 
 * ARGUMENT  : uc_index			: Indexed
 *             *pst_data			: Periodic transmission data
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void CANFreqTransDataGet(uint8_t uc_index, CAN_FREQ_TRANSMISSION_DAT *pst_data) {
  *pst_data = g_gst_freq_trans_dat[uc_index]; /* Copy to Output Pointer */
  return;
}

/*******************************************************************************
 * MODULE    : CANFreqTransCanidEntryCheck
 * ABSTRACT  : CANDataPeriodic transmission control table registration status checking process (CAN ID retrieval)
 * FUNCTION  : CANDataCheck the registration status of the periodic transmission management table 
 * ARGUMENT  : uc_index			: Indexed
 *           : can_id			: CAN ID
 * NOTE      :
 * RETURN    : TRUE				: Registered
 *			   FALSE			: No registration
 ******************************************************************************/
BOOL CANFreqTransCanidEntryCheck(uint8_t uc_index, CANID can_id) {
  BOOL ret = FALSE; /* Return value of this function */

  if ((uint16_t)0 != g_gst_freq_trans_dat[uc_index].us_freq) {
    if (can_id == g_gst_freq_trans_dat[uc_index].st_can_data.can_id) {
      ret = TRUE; /* Registered */
    }
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANFreqTransIdEntryCheck
 * ABSTRACT  : Periodic transmission management table send destination ID confirmation processing
 * FUNCTION  : CANDataCheck the destination ID in the periodic transmission management table 
 * ARGUMENT  : uc_index			: Indexed
 *           : notifyId		: Transfer to: ID
 * NOTE      :
 * RETURN    : TRUE				: Match
 *			   FALSE			: Mismatched
 ******************************************************************************/
BOOL CANFreqTransIdEntryCheck(uint8_t uc_index, PCSTR notify_name) {
  BOOL ret = FALSE; /* Return value of this function */

  if (strcmp(notify_name, g_gst_freq_trans_dat[uc_index].notify_name) == 0) {
    ret = TRUE; /* Match */
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANFreqTransStop
 * ABSTRACT  : CANDataPeriodic transmission stop processing
 * FUNCTION  : CANDataClears the data of the specified index in the periodic transmission management table. 
 * ARGUMENT  : uc_index			: Indexed
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void CANFreqTransStop(uint8_t uc_index) {
  CAN_FREQ_TRANSMISSION_DAT *pst; /* Structure pointer */

  pst = &g_gst_freq_trans_dat[uc_index];
  memset(reinterpret_cast<void *>(pst), 0x00, (size_t)sizeof(CAN_FREQ_TRANSMISSION_DAT));

  return;
}

/*******************************************************************************
 * MODULE    : CANFreqTransIndexEntryCheck
 * ABSTRACT  : CANDataPeriodic transmission control table registration status checking process (INDEX retrieval)
 * FUNCTION  : CANDataCheck the registration status of the specified index in the periodic transmission management table 
 * ARGUMENT  : uc_index			: Indexed
 * NOTE      :
 * RETURN    : TRUE				: Registered
 *			   FALSE			: No registration
 ******************************************************************************/
BOOL CANFreqTransIndexEntryCheck(uint8_t uc_index) {
  BOOL ret = FALSE; /* Return value of this function */

  if ((uint16_t)0 != g_gst_freq_trans_dat[uc_index].us_freq) {  // LCOV_EXCL_BR_LINE 6:Double check code. in CANFreqTransEntryCheck  //NOLINT (readability/naming)
    ret = TRUE; /* Registered */
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CAN1TimeTransEntryCheck
 * ABSTRACT  : CANDataConfirmation processing of the registration destination of the transmission result management table
 * FUNCTION  : CANDataWhether or not registration of the transmission results management table can be performed once, and the index is returned. 
 * ARGUMENT  : *puc_index		: Index (pointer output)
 * NOTE      : 
 * RETURN    : TRUE				: Can be registered
 *			   FALSE			: Cannot be stored
 ******************************************************************************/
BOOL CAN1TimeTransEntryCheck(uint8_t *puc_index) {
  BOOL ret = FALSE; /* Return value of this function */
  int32_t i;         /* Generic counters */

  for (i = 0; i < (int32_t)CAN_1TIME_TRSNSMISSION_RSLT_NUM; i++) {
    if ((uint8_t)CAN_RID_NOTUSE_CODE == g_gst1_time_trans_rslt_dat[i].uc_rid) {
      /* If the resource ID is an unused code, it is not registered. */
      ret = TRUE; /* Can be registered */
      *puc_index = (uint8_t)i;
      break;
    }
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CAN_CWORD29_TransEntryCheck
 * ABSTRACT  : CAN_CWORD29_ transmission-result-management-table-registration-destination-confirmation-processing
 * FUNCTION  : Returns whether or not the CAN_CWORD29_ send results control table can be registered and indexes. 
 * ARGUMENT  : *puc_index		: Index (pointer output)
 * NOTE      : 
 * RETURN    : TRUE				: Can be registered
 *			       FALSE			: Cannot be stored
 ******************************************************************************/
BOOL CAN_CWORD29_TransEntryCheck(uint8_t *puc_index) {
  BOOL ret = FALSE; /* Return value of this function */
  int32_t i;         /* Generic counters */

  for (i = 0; i < (int32_t)CAN__CWORD29__TRSNSMISSION_RSLT_NUM; i++) {
    if ((uint8_t)CAN_RID_NOTUSE_CODE == g_gst__CWORD29__trans_rslt_dat[i].uc_rid) {
      /* If the resource ID is an unused code, it is not registered. */
      ret = TRUE; /* Can be registered */
      *puc_index = (uint8_t)i;
      break;
    }
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CAN1TimeTransEntry
 * ABSTRACT  : CANDataOne-time transmission result management table registration processing
 * FUNCTION  : CANDataRegister in the transmission results management table once 
 * ARGUMENT  : uc_index			: Index to register
 *             *pst_msg_data		: Receive message data section reference pointer
 * NOTE      : 
 * RETURN    : EFrameworkunifiedStatus
 *             eFrameworkunifiedStatusOK         : Success
 *             eFrameworkunifiedStatusInvldParam : uc_index is invalid
 ******************************************************************************/
EFrameworkunifiedStatus CAN1TimeTransEntry(uint8_t uc_index, const CAN_TRANSMISSION_START_MSG_DAT *pst_msg_data) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  /* Register one transmission result management table */
  if (CAN_1TIME_TRSNSMISSION_RSLT_NUM > uc_index) {  // LCOV_EXCL_BR_LINE 6:Double check code. in CAN1TimeTransEntryCheck  //NOLINT (readability/naming)
    strlcpy(g_gst1_time_trans_rslt_dat[uc_index].notify_name, pst_msg_data->notifyName,
            sizeof(g_gst1_time_trans_rslt_dat[uc_index].notify_name));
    g_gst1_time_trans_rslt_dat[uc_index].ul_canid = pst_msg_data->stCandata.can_id;
    g_gst1_time_trans_rslt_dat[uc_index].uc_rid = pst_msg_data->ucRid;
  } else {
    e_status = eFrameworkunifiedStatusInvldParam;
  }
  return e_status;
}

/*******************************************************************************
 * MODULE    : CAN_CWORD29_TransEntry
 * ABSTRACT  : CAN_CWORD29_ transmission-result-management-table-registration-process
 * FUNCTION  : Register in the CAN_CWORD29_ transmission result management table 
 * ARGUMENT  : uc_index			: Index to register
 *             *pst_msg_data		: Receive message data section reference pointer
 * NOTE      : 
 * RETURN    : EFrameworkunifiedStatus
 *             eFrameworkunifiedStatusOK         : Success
 *             eFrameworkunifiedStatusInvldParam : uc_index is invalid
 ******************************************************************************/
EFrameworkunifiedStatus CAN_CWORD29_TransEntry(uint8_t uc_index, const CAN__CWORD29__TRANS_MSG *pst_msg_data) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  /* Register _CWORD29_ send result control table */
  if (CAN__CWORD29__TRSNSMISSION_RSLT_NUM > uc_index) {
    strlcpy(g_gst__CWORD29__trans_rslt_dat[uc_index].notify_name, pst_msg_data->notifyName,
            sizeof(g_gst__CWORD29__trans_rslt_dat[uc_index].notify_name));
    g_gst__CWORD29__trans_rslt_dat[uc_index].opc = pst_msg_data->opc;
    g_gst__CWORD29__trans_rslt_dat[uc_index].uc_rid = pst_msg_data->ucRid;
  } else {
    e_status = eFrameworkunifiedStatusInvldParam;
  }
  return e_status;
}

/*******************************************************************************
 * MODULE    : CANTxRsltEntryCheck
 * ABSTRACT  : CANDataTransmission result management table registration status confirmation processing
 * FUNCTION  : Periodic transmission, one transmission, result control for specified indexes
 *             Check the registration status of the table 
 * ARGUMENT  : uc_can_rid			: CAN RID (Resource-ID managed by the CAN thread)
 * NOTE      :
 * RETURN    : TRUE				: Registered
 *			   FALSE			: No registration
 ******************************************************************************/
BOOL CANTxRsltEntryCheck(uint8_t uc_can_rid) {  // LCOV_EXCL_START 8:this IF is called in CANIcrSndStsProcess, which will not be called // NOLINT (whitespace/line_length)
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  BOOL ret = FALSE; /* Return value of this function */
  uint8_t uc_index;   /* Indexed */
  /*** Periodic transmission ***/
  if ((uint8_t)CAN_TXRSLT_CANRID_1TIME_START > uc_can_rid) {
    uc_index = uc_can_rid;
    /* Checking for User unset code */
    if ((uint8_t)CAN_RID_NOTUSE_CODE != g_gst_freq_trans_dat[uc_index].uc_rid) {
      if ((uint8_t)0 != g_gst_freq_trans_dat[uc_index].uc_resp_num) {
        ret = TRUE; /* Registered */
      }
    }
  } else if ((uint8_t)CAN_TXRSLT_CANRID__CWORD29__START > uc_can_rid) {           /*** One-time transmission ***/
    uc_index = (uint8_t)((uint32_t)uc_can_rid - CAN_TXRSLT_CANRID_1TIME_START);
    if ((uint8_t)CAN_RID_NOTUSE_CODE != g_gst1_time_trans_rslt_dat[uc_index].uc_rid) {
    /* Checking for User unset code */
      ret = TRUE; /* Registered */
    }
  }

  return (ret);
}
// LCOV_EXCL_STOP 8

/*******************************************************************************
 * MODULE    : CANTxRsltDataGet
 * ABSTRACT  : Transmission result data acquisition processing
 * FUNCTION  : Retrieves the send result data for the specified index 
 * ARGUMENT  : uc_can_rid				: CAN RID (Resource-ID managed by the CAN thread)
 *             *pst_data				: Transmission result data (pointer output)
 * NOTE      : 
 * RETURN    : void
 ******************************************************************************/
void CANTxRsltDataGet(uint8_t uc_can_rid, CAN_1TIME_TXRSLT_DAT *pst_data) {
  uint8_t uc_index; /* Indexed */

  if ((uint8_t)CAN_TXRSLT_CANRID_1TIME_START > uc_can_rid) { /*** Periodic transmission ***/
    uc_index = uc_can_rid;
    strlcpy(pst_data->notify_name, g_gst_freq_trans_dat[uc_index].notify_name, sizeof(pst_data->notify_name));
    pst_data->ul_canid = g_gst_freq_trans_dat[uc_index].st_can_data.can_id;
    pst_data->uc_rid = g_gst_freq_trans_dat[uc_index].uc_rid;

  } else if ((uint8_t)CAN_TXRSLT_CANRID__CWORD29__START > uc_can_rid) { /*** One-time transmission ***/
    uc_index = (uint8_t)((uint32_t)uc_can_rid - CAN_TXRSLT_CANRID_1TIME_START);
    strlcpy(pst_data->notify_name,  g_gst1_time_trans_rslt_dat[uc_index].notify_name, sizeof(pst_data->notify_name));
    pst_data->ul_canid = g_gst1_time_trans_rslt_dat[uc_index].ul_canid;
    pst_data->uc_rid = g_gst1_time_trans_rslt_dat[uc_index].uc_rid;

  }
  return;
}

/*******************************************************************************
 * MODULE    : CAN_CWORD29_TxRsltDataGet
 * ABSTRACT  : Acquisition of _CWORD29_ Transmission Result Data
 * FUNCTION  : Retrieves the _CWORD29_ send results for the specified indexes 
 * ARGUMENT  : uc_can_rid       : CAN RID (Resource-ID managed by the CAN thread)
 *             *pst_data        : Transmission result data (pointer output)
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void CAN_CWORD29_TxRsltDataGet(const uint8_t uc_can_rid, CAN__CWORD29__TXRSLT_DAT* const pst_data) {
  uint32_t ul_index; /* Indexed */

  if (reinterpret_cast<CAN__CWORD29__TXRSLT_DAT *>(NULL) == pst_data) {
    return;
  }

  if (static_cast<uint32_t>(CAN_TXRSLT_CANRID__CWORD29__START) > uc_can_rid) {
    return;
  }

  ul_index = uc_can_rid - static_cast<uint32_t>(CAN_TXRSLT_CANRID__CWORD29__START);
  if (static_cast<uint32_t>(CAN__CWORD29__TRSNSMISSION_RSLT_NUM) <= ul_index) {
    return;
  }

  strlcpy(reinterpret_cast<char *>(pst_data->notify_name),
          reinterpret_cast<const char *>(g_gst__CWORD29__trans_rslt_dat[ul_index].notify_name), MAX_NAME_SIZE_APP);
  pst_data->opc = g_gst__CWORD29__trans_rslt_dat[ul_index].opc;
  pst_data->uc_rid = g_gst__CWORD29__trans_rslt_dat[ul_index].uc_rid;

  return;
}

/*******************************************************************************
 * MODULE    : CANTxRsltFin
 * ABSTRACT  : CANDataTransmission result notification completion processing
 * FUNCTION  : Determine and clear the registration of the specified index. 
 * ARGUMENT  : uc_can_rid				: CAN RID (Resource-ID managed by the CAN thread)
 * NOTE      : 
 * RETURN    : void
 ******************************************************************************/
void CANTxRsltFin(uint8_t uc_can_rid) {
  uint8_t uc_index; /* Indexed */

  if ((uint8_t)CAN_TXRSLT_CANRID_1TIME_START > uc_can_rid) { /*** Periodic transmission ***/
    uc_index = uc_can_rid;
    if (g_gst_freq_trans_dat[uc_index].uc_resp_num > (uint8_t)0) {
        g_gst_freq_trans_dat[uc_index].uc_resp_num--;                    /* Decrement the number of transmission result notifications */
      if ((uint8_t)0 == g_gst_freq_trans_dat[uc_index].uc_resp_num) {
          g_gst_freq_trans_dat[uc_index].uc_rid = 0;
      }
    }
  } else if ((uint8_t)CAN_TXRSLT_CANRID__CWORD29__START > uc_can_rid) { /*** One-time transmission ***/
    uc_index = (uint8_t)((uint32_t)uc_can_rid - CAN_TXRSLT_CANRID_1TIME_START);
    memset(g_gst1_time_trans_rslt_dat[uc_index].notify_name, 0x00,
           sizeof(g_gst1_time_trans_rslt_dat[uc_index].notify_name));
    g_gst1_time_trans_rslt_dat[uc_index].ul_canid = 0;
    g_gst1_time_trans_rslt_dat[uc_index].uc_rid = (uint8_t)CAN_RID_NOTUSE_CODE;
    /* Enter the User unset code */
  }

  return;
}

/*******************************************************************************
 * MODULE    : CAN_CWORD29_TxRsltFin
 * ABSTRACT  : _CWORD29_ transmission result notification completion process
 * FUNCTION  : Clearing and clearing _CWORD29_ registrations of specified indexes 
 * ARGUMENT  : uc_can_rid       : CAN RID (Resource-ID managed by the CAN thread)
 * NOTE      :
 * RETURN    : void
 ******************************************************************************/
void CAN_CWORD29_TxRsltFin(const uint8_t uc_can_rid) {
  uint32_t ul_index; /* Indexed */

  if (static_cast<uint32_t>(CAN_TXRSLT_CANRID__CWORD29__START) > uc_can_rid) {
    return;
  }

  ul_index = uc_can_rid - static_cast<uint32_t>(CAN_TXRSLT_CANRID__CWORD29__START);
  if (static_cast<uint32_t>(CAN__CWORD29__TRSNSMISSION_RSLT_NUM) <= ul_index) {
    return;
  }

  memset(reinterpret_cast<void *>(g_gst__CWORD29__trans_rslt_dat[ul_index].notify_name), 0x00,
         MAX_NAME_SIZE_APP);
  g_gst__CWORD29__trans_rslt_dat[ul_index].opc = 0;

  /* Enter the User unset code */
  g_gst__CWORD29__trans_rslt_dat[ul_index].uc_rid = (uint8_t)CAN_RID_NOTUSE_CODE;

  return;
}

/*******************************************************************************
 * MODULE    : CANTxRsltSndCheck
 * ABSTRACT  : CANDataTransmission result notification determination processing
 * FUNCTION  : CANDataDetermine whether to perform transmission result notification 
 * ARGUMENT  : uc_can_rid			: CAN RID (Resource-ID managed by the CAN thread)
 *             ul_sts			: Received transmission result status
 * NOTE      : 
 * RETURN    : TRUE				: Transmission result notification required
 *             FALSE			: No need to send result notification
 ******************************************************************************/
BOOL CANTxRsltSndCheck(uint8_t uc_can_rid, uint32_t ul_sts) {  // LCOV_EXCL_START 8:this IF is called in CANIcrSndStsProcess, which will not be called // NOLINT (whitespace/line_length)
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  BOOL ret = FALSE; /* Return value of this function */
  /*** Periodic transmission ***/
  if ((uint8_t)CAN_TXRSLT_CANRID_1TIME_START > uc_can_rid) {
    /* Always send result notification */
    ret = TRUE;
  } else if ((uint8_t)CAN_TXRSLT_CANRID__CWORD29__START > uc_can_rid) {/*** One-time transmission ***/
    /* Always send result notification */
    ret = TRUE;
  }

  return (ret);
}
// LCOV_EXCL_STOP 8

/*******************************************************************************
 * MODULE    : CANCycleTransBufferOut
 * ABSTRACT  : CAN periodic transmission table log output processing
 * FUNCTION  : Output the CAN periodic transmission table log 
 * ARGUMENT  : FILE *fp_log    : File pointer of the log output file
 * NOTE      : 
 * RETURN    : 
 ******************************************************************************/
void CANCycleTransBufferOut(FILE *fp_log) {  // LCOV_EXCL_START 8: for dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  int32_t usage_rate;
  uint16_t i;
  uint16_t data_cnt = 0;

  if (NULL != fp_log) {
    /* CAN periodic transmission table output */
    (void)fprintf(fp_log, "CAN CycleTrans Buffer \n");
    (void)fprintf(fp_log, "CanID:     Freq:  SndId:   Rid:   DLC:\n");
    for (i = 0; i < (uint16_t)CAN_FREQ_TRSNSMISSION_LIST_NUM; i++) {
      /* Data Registration Max */
      if ((uint16_t)0 != g_gst_freq_trans_dat[i].us_freq) {
        (void)fprintf(fp_log, "%08x   %04x   %s   %04x   %04x\n",
                      g_gst_freq_trans_dat[i].st_can_data.can_id,
                      g_gst_freq_trans_dat[i].us_freq,
                      g_gst_freq_trans_dat[i].notify_name,
                      g_gst_freq_trans_dat[i].uc_rid,
                      g_gst_freq_trans_dat[i].st_can_data.dlc);
        data_cnt++;
      }
    }
    usage_rate = (((int32_t)data_cnt * 100) / CAN_FREQ_TRSNSMISSION_LIST_NUM);
    (void)fprintf(fp_log, "BUFFER_Use: %04d  BUFFER_Max: %04d  Usage_Rate: %04d \n",
                  data_cnt, CAN_FREQ_TRSNSMISSION_LIST_NUM, usage_rate);
    if (usage_rate >= CAN_USAGE_RATE_THRESHOLD) {
      (void)fprintf(fp_log, "Warning: Buffer utilization exceeds a threshold.\n");
    }
  }
}
// LCOV_EXCL_STOP

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Merge and Update CAN Data
 *
 * @~english
 * @return  Return value
 * @retval  <TURE>        OK
 * @retval  <FALSE>       Failure
 *
 * @~english
 * @param[in]     <rcv_msg>   Start transmission message structure
 */
BOOL CANTransDataEntry(CAN_TRANS_START_MSG_DAT *rcv_msg) {
  CAN_TRANS_START_TABLE_VAL *tmp_trans_data;
  uint8_t mrg_data[CAN_TXDATA_SIZE] = {};
  CAN_DATA_BIT *mem_data;
  int i;
  BOOL ret = TRUE;

  ret = CANEntryTransCanidCheck(rcv_msg->id);
  if (FALSE == ret) {
    goto exit;
  }

  /* Get transmission data of corresponding CAN ID */
  tmp_trans_data = g_map_trans_data[rcv_msg->id];
  mem_data = &tmp_trans_data->dat;

  /* Merge transmission data */
  for (i = 0; i < tmp_trans_data->dlc; i++) {
    uint8_t mask = rcv_msg->mask.dat[i];
    uint8_t bit = rcv_msg->dat.dat[i];

    mrg_data[i] = mask & bit;
    mem_data->dat[i] = (uint8_t)((uint8_t)(mem_data->dat[i]) & (uint8_t)(~mask));
    mem_data->dat[i] |= mrg_data[i];
  }

  /* Update Transmission data management table */
  tmp_trans_data->dat = *mem_data;

exit:
  return ret;
}

/*!-----------------------------------------------------------------------------
 * @~english
* @brief    Transmission CAN Data(SubID) with Marge and Update Data
 *
 * @~english
 * @return  Return value
 * @retval  <TURE>        OK
 * @retval  <FALSE>       Failure
 *
 * @~english
 * @param[in]     <rcvMsg>   Start transmission message structure
 *----------------------------------------------------------------------------*/
BOOL CAN_SubIDTransStartTxMsg(HANDLE h_app, CAN_TRANS_START_MSG_DAT *rcvMsg)
{
  CAN_TRANS_START_TABLE_VAL *transData;
  CAN_DATA_BIT *memData;

  std::multimap<CANID, CAN_TRANS_START_TABLE_VAL *>::iterator it;
  // Find CANID Matching
  for (it = mapSubIDTransData.find(rcvMsg->id); it != mapSubIDTransData.end(); it++)
  {
    transData = it->second;

    // SubID matches
    if (transData->dat.dat[0] == (rcvMsg->dat.dat[0] & rcvMsg->mask.dat[0])) {
      // CANData(The 1byte eye is fixed to the SubID, so it should not be changed.)
      memData = &transData->dat;
      for (int i = 1; i < transData->dlc; i++) {
        memData->dat[i] &= ~rcvMsg->mask.dat[i];
        memData->dat[i] |= (rcvMsg->mask.dat[i] & rcvMsg->dat.dat[i]);
      }
      // CANDataSend
      return CANTransStartTxMsgCore(h_app, rcvMsg->id, transData->dlc, &transData->dat);
    }
  }

  // No matching CAN ID-SubID
  return FALSE;
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Get Transmission data
 *
 * @~english
 * @return  Return value
 * @retval  <TRUE>        Transmission success
 * @retval  <FALSE>       Transmission failed
 *
 * @~english
 * @param[in]     <id>    CAN ID
 */
BOOL CANTransStartTxMsg(HANDLE h_app, CANID id) {
  map<CANID, CAN_TRANS_START_TABLE_VAL *>::iterator it;
  CAN_TRANS_START_TABLE_VAL *trans_data;

  /* Get transmission data of corresponding CAN ID */
  it = g_map_trans_data.find(id);
  if (it == g_map_trans_data.end()) {
    return FALSE;
  }
  trans_data = it->second;

  /* Transmission CAN Data */
  return CANTransStartTxMsgCore(h_app, id, trans_data->dlc, &trans_data->dat);
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Check invalid CAN ID
 *
 * @~english
 * @return  Return value
 * @retval  <TURE>        OK
 * @retval  <FALSE>       Invalid CAN ID
 *
 * @~english
 * @param[in]     <id>   CAN ID
 */
BOOL CANEntryTransCanidCheck(CANID id) {
  map<CANID, CAN_TRANS_START_TABLE_VAL *>::iterator it;

  /* Get transmission data of corresponding CAN ID */
  it = g_map_trans_data.find(id);
  if (it == g_map_trans_data.end()) {
    return FALSE;
  }

  return TRUE;
}  // LCOV_EXCL_BR_LINE 10:THE_END_LINE_OF_THE_FILE
