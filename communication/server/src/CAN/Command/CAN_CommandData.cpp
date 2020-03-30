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
 * FILE  :CAN_CommandData.cpp
 * SYSTEM  :_CWORD107_
 * SUBSYSTEM :EXL process
 * PROGRAM :CAN Thread CAN Command Control Data Management
 *     :
 * Module configuration :CANCommandDeliveryEntryCheck()  CAN command delivery management table registration status confirmation processing
 *     :CANCommandDeliveryEntry()   CAN command delivery management table registration process
 *     :CANCommandDeliveryDataGet()   CAN command delivery management table data acquisition processing
 *     :CANCommandDeliveryDelete()   CAN command delivery management table deletion processing
 *     :CANCommandTxRsltEntryCheck()   CAN command transmission result management table registration status confirmation processing
 *     :CANCommandTxRsltEntry()    CAN command transmission result management table registration process
 *     :CANCommandTxRsltDataGet()   CAN command transmission result management table data acquisition processing
 *     :CANCommandTxRsltDelete()    CAN command transmission result management table deletion processing
 *     :CANCommandTxRsltCheck()    CAN command transmission result management table notification registration confirmation processing
 *     :CANCommandFuelCalcRstReqCheck()  CAN section flame reset request reception status confirmation processing
 *     :CANCommandFuelCalcRstReq()   CAN section flammability reset request reception processing
 *     :CANCommandDataInit()    CAN command management data initialization processing
 *     :
 ******************************************************************************/
#include "CAN_CommandData.h"

#include <string.h>  // NOLINT (build/include)
#include <native_service/frameworkunified_types.h>
#include <other_service/strlcpy.h>

#include <peripheral_service/Canif_API.h>
#include "CAN_Thread.h"

/*************************************************/
/*           Global variable                      */
/*************************************************/
static CAN_COMMAND_DELIVERY_LIST_DAT g_st_can_cmd_delivery_dat[CAN_COMMAND_DELIVERY_NUM]; /* CAN command delivery management table */  //NOLINT (readability/naming)
/* CAN command transmission result management table */
static CAN_COMMAND_TXRSLT_LIST_DAT g_st_can_cmd_transrslt_dat[CAN_COMMAND_TXRSLT_NUM];  //NOLINT (readability/naming)

/*******************************************************************************
 * MODULE    : CANCmdidConvertIndexTX
 * ABSTRACT  : CAN Command ID->Index conversion processing (during delivery)
 * FUNCTION  : Convert the CAN command ID to an index in the CAN command delivery management table 
 * ARGUMENT  : uc_cmd_id : CAN command ID
 *      : *uc_index : Pointer to the index
 * NOTE      : 
 * RETURN    : TRUE : Successful conversion
 *      : FALSE : Conversion failed
 ******************************************************************************/
static BOOL CANCmdidConvertIndexTX(uint8_t uc_cmd_id, uint8_t* uc_index) {
  BOOL b_ret = TRUE;

  switch (uc_cmd_id) {
    case CAN_CMDID_FUELCALC_RST_REQ_DELIVERY: /* Request for delivery of request for flame reset in CAN section */
      *uc_index = 0;
      break;

    case CAN_CMDID_STARTUP_FIN_REQ_TX: /* CAN start completion notification request */
      *uc_index = 1;
      break;

    case CAN_CMDID_MRST_INFO_REQ_TX: /* CAN Master Reset Information Notification Request */
      *uc_index = 2;
      break;

    case CAN_CMDID_VERSION_REQ_TX: /* CAN version request */
      *uc_index = 3;
      break;

    case CAN_CMDID_CONNECTION_NODE_REQ_TX: /* CAN Connection Node Notification Request */
      *uc_index = 4;
      break;

    default: /* IDs that are not subject to undefined ID or registration */
      b_ret = FALSE;
      break;
  }

  return b_ret;
}

/*******************************************************************************
 * MODULE    : CANCmdidConvertIndexRX
 * ABSTRACT  : CAN Command ID->Index conversion processing (at reception)
 * FUNCTION  : Convert the CAN command ID to an index in the CAN command delivery management table 
 * ARGUMENT  : uc_cmd_id : CAN command ID
 *        *uc_index : Pointer to the index
 * NOTE      : 
 * RETURN    : TRUE : Successful conversion
 *  FALSE : Conversion failed
 ******************************************************************************/
static BOOL CANCmdidConvertIndexRX(uint8_t uc_cmd_id, uint8_t* uc_index) {
  BOOL b_ret = TRUE;

  switch (uc_cmd_id) {
    case CAN_CMDID_FUELCALC_RST_REQ_RX: /* RECEIVE REQUEST FREE OF CAN SECTION */
      *uc_index = 0;
      break;

    case CAN_CMDID_STARTUP_FIN_RESP_RX: /* CAN start completion notification request transmission */
      *uc_index = 1;
      break;

    case CAN_CMDID_MRST_INFO_RESP_RX: /* CAN master reset information notification request transmission */
      *uc_index = 2;
      break;

    case CAN_CMDID_VERSION_RESP_RX: /* CAN Version Request Send */
      *uc_index = 3;
      break;

    case CAN_CMDID_CONNECTION_NODE_RESP_RX: /* CAN connection node notification request transmission */
      *uc_index = 4;
      break;

    default: /* IDs that are not subject to undefined ID or registration */
      b_ret = FALSE;
      break;
  }
  return b_ret;
}

/*******************************************************************************
 * MODULE    : CANCommandDeliveryEntryCheck
 * ABSTRACT  : CAN command delivery management table registration status confirmation processing
 * FUNCTION  : Confirm whether registration is possible in the CAN command delivery management table. 
 * ARGUMENT  : notifyId : Addresses for delivery ID
 *    uc_cmd_id : CAN command ID
 * NOTE      : 
 * RETURN    : TRUE : Can be registered
 *    FALSE : Cannot be stored
 ******************************************************************************/
BOOL CANCommandDeliveryEntryCheck(PCSTR notify_name, uint8_t uc_cmd_id) {
  BOOL ret = TRUE; /* Return value of this function */
  uint8_t uc_index;  /* Index that references the delivery management table */
  int32_t l_cnt;     /* Generic counters */
  CAN_COMMAND_DELIVERY_LIST_DAT* ptr;

  ret = CANCmdidConvertIndexTX(uc_cmd_id, &uc_index);

  if (TRUE == ret) {  // LCOV_EXCL_BR_LINE 6: double check
    ret = FALSE;
    ptr = &g_st_can_cmd_delivery_dat[uc_index];
    /* Search destination */
    for (l_cnt = 0; l_cnt < CAN_COMMAND_DELIVERY_SNDID_NUM; l_cnt++) {
      if ((strcmp(ptr->notify_name_list[l_cnt], notify_name) == 0) ||
          (ptr->notify_name_num == (uint8_t)l_cnt)) {
        /* When overwriting or registering a new entry */
        ret = TRUE;
        break;
      }
    }
  }
  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommandDeliveryEntry
 * ABSTRACT  : CAN command delivery management table registration process
 * FUNCTION  : Register in the CAN command delivery management table 
 * ARGUMENT  : notifyId : Addresses for delivery ID
 *    uc_cmd_id : CAN command ID
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANCommandDeliveryEntry(PCSTR notify_name, uint8_t uc_cmd_id) {
  BOOL b_cont = TRUE; /* Processing Continuation Flag */
  uint8_t uc_index;   /* Index that references the delivery management table */
  int32_t l_cnt;      /* Generic counters */
  CAN_COMMAND_DELIVERY_LIST_DAT* ptr;

  b_cont = CANCmdidConvertIndexTX(uc_cmd_id, &uc_index);

  if (TRUE == b_cont) {  // LCOV_EXCL_BR_LINE 6: double check
    ptr = &g_st_can_cmd_delivery_dat[uc_index];
    /* Search destination */
    for (l_cnt = 0; (l_cnt < CAN_COMMAND_DELIVERY_SNDID_NUM); l_cnt++) {
      if (strcmp(ptr->notify_name_list[l_cnt], notify_name) == 0) {
        /* For overwriting */
        b_cont = FALSE;
      } else if (ptr->notify_name_num == (uint8_t)l_cnt) {
        /* For new entry */
        strlcpy(ptr->notify_name_list[l_cnt], notify_name, sizeof(ptr->notify_name_list[l_cnt]));
        ptr->notify_name_num++;
        b_cont = FALSE;
      } else {
        /* Do nothing */
      }
      if (FALSE == b_cont) {
        break;
      }
    }
  }
  return;
}

/*******************************************************************************
 * MODULE    : CANCommandDeliveryDataGet
 * ABSTRACT  : CAN command delivery management table data acquisition processing
 * FUNCTION  : Retrieve registration data from CAN command delivery management table 
 * ARGUMENT  : uc_cmd_id  : CAN command ID
 *    pNotifyNameNum : Return Pointer to Number of Destination Registrations
 *    pNotifyIdList : Shipping list reference pointer
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANCommandDeliveryDataGet(uint8_t uc_cmd_id, CAN_COMMAND_DELIVERY_LIST_DAT* dst) {
  BOOL b_cont;      /* Processing Continuation Flag */
  uint8_t uc_index; /* Index that references the delivery management table */
  CAN_COMMAND_DELIVERY_LIST_DAT* src;

  b_cont = CANCmdidConvertIndexRX(uc_cmd_id, &uc_index);

  if (TRUE == b_cont) { /* For registration ID*/  // LCOV_EXCL_BR_LINE 6: double check
    src = &g_st_can_cmd_delivery_dat[uc_index];
    dst->notify_name_num = src->notify_name_num; /* Number of registered delivery destination IDs */
    memcpy(dst->notify_name_list, src->notify_name_list, sizeof(dst->notify_name_list));
  }
  return;
}

/*******************************************************************************
 * MODULE    : CANCommandDeliveryDelete
 * ABSTRACT  : CAN command delivery management table deletion processing
 * FUNCTION  : Delete registered data from CAN command delivery management table 
 * ARGUMENT  : uc_cmd_id  : CAN command ID
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANCommandDeliveryDelete(uint8_t uc_cmd_id) {
  BOOL b_cont = TRUE; /* Processing Continuation Flag */
  uint8_t uc_index;   /* Index that references the delivery management table */
  CAN_COMMAND_DELIVERY_LIST_DAT* ptr;

  b_cont = CANCmdidConvertIndexRX(uc_cmd_id, &uc_index);

  if (TRUE == b_cont) { /* For registration ID */  // LCOV_EXCL_BR_LINE 6: double check
    ptr = &g_st_can_cmd_delivery_dat[uc_index];
    /* Whether the control method is delivery control type or one-time communication type */
    if (((uint8_t)CAN_CMD_DELIVERY_CTRL == ptr->uc_ctrl) ||
        ((uint8_t)CAN_CMD_1TIME_TRANCE == ptr->uc_ctrl)) {
      /* Delete registered data */
      ptr->notify_name_num = 0;                                             /* Number of delivery destination ID registrations = 0 */
      memset((ptr->notify_name_list), 0x00, sizeof(ptr->notify_name_list)); /* Destination ID List = 0 */
    }
  }
  return;
}

/*******************************************************************************
 * MODULE    : CANCommandDeliveryDeletesingle
 * ABSTRACT  : CAN command delivery management table deletion processing
 * FUNCTION  : Delete registered data from CAN command delivery management table 
 * ARGUMENT  : uc_cmd_id  : CAN command ID
 *      : notifyId  : Service client ID
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANCommandDeliveryDeletesingle(uint8_t uc_cmd_id, PCSTR notify_name) {  // LCOV_EXCL_START 4: only be executed when NSFW error // NOLINT (whitespace/line_length)
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  BOOL b_cont = TRUE; /* Processing Continuation Flag */
  uint8_t uc_index;   /* Index that references the delivery management table */
  int i, j;
  CAN_COMMAND_DELIVERY_LIST_DAT* ptr;

  b_cont = CANCmdidConvertIndexRX(uc_cmd_id, &uc_index);

  if (TRUE == b_cont) {  // LCOV_EXCL_BR_LINE 6: double check
    ptr = &g_st_can_cmd_delivery_dat[uc_index];
    /* Delete registered data */
    for (i = 0; i < ptr->notify_name_num; i++) {
      if (strcmp(notify_name, ptr->notify_name_list[i]) != 0) {
        continue;
      }
      for (j = i; j < ptr->notify_name_num - 1; j++) {
        memmove(ptr->notify_name_list[j], ptr->notify_name_list[j + 1], sizeof(char) * MAX_NAME_SIZE_APP);
      }
      ptr->notify_name_num--;
    }
  }
  return;
}
// LCOV_EXCL_STOP 8: only be executed when NSFW error

/*******************************************************************************
 * MODULE    : CANCommandTxRsltEntryCheck
 * ABSTRACT  : CAN command transmission result management table registration status confirmation processing
 * FUNCTION  : Check that the CAN command can be registered in the transmission result management table. 
 * ARGUMENT  : uc_cmd_id  : CAN command ID
 *    pucCanRid : Resource ID return pointer from CAN
 * NOTE      : 
 * RETURN    : TRUE  : Can be registered
 *    FALSE  : Cannot be stored
 ******************************************************************************/
BOOL CANCommandTxRsltEntryCheck(uint8_t uc_cmd_id, uint8_t* uc_can_rid) {
  BOOL ret = FALSE; /* Return value of this function */
  uint8_t uc_index;   /* Index that references the communication result management table */

  /* CAN command transmission result management table retrieval */
  for (uc_index = 0; uc_index < (uint8_t)CAN_COMMAND_TXRSLT_NUM; uc_index++) {
    /* CAN command ID match? */
    if (uc_cmd_id == g_st_can_cmd_transrslt_dat[uc_index].uc_cmd_id) {
      /* RID = 0xFF (not registered)? */
      if ((uint8_t)CAN_RID_NOTUSE_CODE == g_st_can_cmd_transrslt_dat[uc_index].uc_rid) {
        ret = TRUE;
        *uc_can_rid = (uint8_t)(CAN_CMD_RID_MASK | (uint32_t)uc_index);
        break;
      }
    }
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommandTxRsltEntry
 * ABSTRACT  : CAN command transmission result management table registration process
 * FUNCTION  : Register in the CAN command transmission result management table 
 * ARGUMENT  : uc_can_rid : CAN_RID of registration destination
 *    notifyId  : Process ID to register
 *    uc_rid  : Resource ID to register
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANCommandTxRsltEntry(uint8_t uc_can_rid, PCSTR notify_name, uint8_t uc_rid) {
  uint8_t uc_index; /* Index that references the communication result management table */

  /* Table's registration index */
  uc_index = (uint8_t)((uint32_t)(~CAN_CMD_RID_MASK) & (uint32_t)uc_can_rid);

  /* Registration */
  strlcpy(g_st_can_cmd_transrslt_dat[uc_index].notify_name, notify_name,
   sizeof(g_st_can_cmd_transrslt_dat[uc_index].notify_name));
  g_st_can_cmd_transrslt_dat[uc_index].uc_rid = uc_rid;

  return;
}

/*******************************************************************************
 * MODULE    : CANCommandTxRsltDataGet
 * ABSTRACT  : CAN command transmission result management table data acquisition processing
 * FUNCTION  : Acquire registration data from CAN command transmission result management table 
 * ARGUMENT  : uc_can_rid : CAN_RID
 *    pNotifyId  : Destination ID Return Pointer
 *    pusRid  : Resource ID return pointer
 *    pusCmdid : CAN command ID return pointer
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANCommandTxRsltDataGet(uint8_t uc_can_rid, char* p_notify_name, uint8_t* puc_rid, uint8_t* puc_cmd_id) {
  uint8_t uc_index; /* Index that references the communication result management table */

  /* Index from which to retrieve data for the table */
  uc_index = (uint8_t)((uint32_t)(~CAN_CMD_RID_MASK) & (uint32_t)uc_can_rid);

  /* Data acquisition */
  strncpy(p_notify_name, g_st_can_cmd_transrslt_dat[uc_index].notify_name,
                sizeof(g_st_can_cmd_transrslt_dat[uc_index].notify_name));
  *puc_rid = g_st_can_cmd_transrslt_dat[uc_index].uc_rid;
  *puc_cmd_id = g_st_can_cmd_transrslt_dat[uc_index].uc_cmd_id;

  return;
}

/*******************************************************************************
 * MODULE    : CANCommandTxRsltDelete
 * ABSTRACT  : CAN command transmission result management table deletion processing
 * FUNCTION  : Delete registered data from the CAN command transmission result management table 
 * ARGUMENT  : uc_can_rid  : CAN_RID
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANCommandTxRsltDelete(uint8_t uc_can_rid) {
  uint8_t uc_index; /* Index that references the communication result management table */

  /* Drop index for the table */
  uc_index = (uint8_t)((uint32_t)(~CAN_CMD_RID_MASK) & (uint32_t)uc_can_rid);

  /* Delete */
  memset(g_st_can_cmd_transrslt_dat[uc_index].notify_name, 0x00,
  sizeof(g_st_can_cmd_transrslt_dat[uc_index].notify_name));
  g_st_can_cmd_transrslt_dat[uc_index].uc_rid = CAN_RID_NOTUSE_CODE;

  return;
}

/*******************************************************************************
 * MODULE    : CANCommandTxRsltCheck
 * ABSTRACT  : CAN command transmission result management table notification registration confirmation processing
 * FUNCTION  : Check if the notification is registered in the CAN command transmission result management table. 
 * ARGUMENT  : uc_can_rid  : CAN_RID
 * NOTE      : 
 * RETURN    : TRUE   : Registered
 *    FALSE   : No registration
 ******************************************************************************/
BOOL CANCommandTxRsltCheck(uint8_t uc_can_rid) {  // LCOV_EXCL_START 8: is called in CANCommandTxRslt, but CANCommandTxRslt is dead code // NOLINT (whitespace/line_length)
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  BOOL ret = FALSE; /* Return value of this function */
  uint8_t uc_index;   /* Index that references the communication result management table */

  /* Reference index of the table */
  uc_index = (uint8_t)((uint32_t)(~CAN_CMD_RID_MASK) & (uint32_t)uc_can_rid);

  /* Not out of range of the table */
  if (uc_index < (uint8_t)CAN_COMMAND_TXRSLT_NUM) {
    /* Registered? */
    if ((uint8_t)CAN_RID_NOTUSE_CODE != g_st_can_cmd_transrslt_dat[uc_index].uc_rid) {
      ret = TRUE;
    }
  }

  return (ret);
}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : CANCommandFuelCalcRstReqCheck
 * ABSTRACT  : CAN section flame reset request reception status confirmation processing
 * FUNCTION  : Confirm whether or not a CAN section flame reset request has been received 
 * ARGUMENT  : None
 * NOTE      : 
 * RETURN    : TRUE : Registered
 *    FALSE : Unregistered
 ******************************************************************************/
BOOL CANCommandFuelCalcRstReqCheck(void) {
  BOOL ret; /* Return value of this function */

  if ((uint8_t)CAN_CMD_RST_RCV == g_st_can_cmd_delivery_dat[0].uc_flag) {
    ret = TRUE; /* Registered */
  } else {
    ret = FALSE; /* Unregistered */
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommandFuelCalcRstReq
 * ABSTRACT  : CAN section flammability reset request reception processing
 * FUNCTION  : Set CAN section flame reset request as received 
 * ARGUMENT  : None
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANCommandFuelCalcRstReq(void) {
  /* Set the control flag (interval flammability reset) of the CAN command transmission result management table as received */
  g_st_can_cmd_delivery_dat[0].uc_flag = CAN_CMD_RST_RCV;

  return;
}

/*******************************************************************************
 * MODULE    : CANCommandDataInit
 * ABSTRACT  : CAN command management data initialization processing
 * FUNCTION  : Initializing CAN Command Management Data 
 * ARGUMENT  : None
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANCommandDataInit(void) {
  int32_t l_cnt1 = 0; /* Counter */
  int32_t l_cnt2 = 0;
  int32_t l_index = 0;
  CAN_COMMAND_DELIVERY_LIST_DAT* ptr_dl;
  CAN_COMMAND_TXRSLT_LIST_DAT* ptr_tl;

  /*** CAN command delivery management table initialization ***/
  /* Control method initialization */
  g_st_can_cmd_delivery_dat[0].uc_ctrl = CAN_CMD_DELIVERY_CTRL; /* Delivery control type */
  g_st_can_cmd_delivery_dat[1].uc_ctrl = CAN_CMD_1TIME_TRANCE;  /* One-time communication */
  g_st_can_cmd_delivery_dat[2].uc_ctrl = CAN_CMD_1TIME_TRANCE;
  g_st_can_cmd_delivery_dat[3].uc_ctrl = CAN_CMD_1TIME_TRANCE;
  g_st_can_cmd_delivery_dat[4].uc_ctrl = CAN_CMD_TRANCE; /* Continuous communication type */
  g_st_can_cmd_delivery_dat[5].uc_ctrl = CAN_CMD_TRANCE;
  for (l_cnt1 = 0; l_cnt1 < CAN_COMMAND_DELIVERY_NUM; l_cnt1++) {
    ptr_dl = &g_st_can_cmd_delivery_dat[l_cnt1];
    /* Control flag initialization */
    ptr_dl->uc_flag = CAN_CMD_RST_NOT_RCV;
    /* Initialization of number of registered delivery destination IDs */
    ptr_dl->notify_name_num = 0x00;
    /* Delivery ID list initialization */
    memset((ptr_dl->notify_name_list), 0x00, (sizeof(ptr_dl->notify_name_list)));
  }

  /*** Initialization of CAN command transmission result management table ***/
  for (l_cnt1 = 0; l_cnt1 < CAN_COMMAND_TXRSLT_NUM; l_cnt1++) {
    ptr_tl = &g_st_can_cmd_transrslt_dat[l_cnt1];
    /* Initialization of delivery destination ID */
    memset(ptr_tl->notify_name, 0x00, sizeof(ptr_tl->notify_name));
    /* Resource ID Initialization */
    ptr_tl->uc_rid = CAN_RID_NOTUSE_CODE;
  }

  /* CAN command ID initialization */
  for (l_cnt1 = 0; l_cnt1 < CAN_COMMAND_TXRSLT_NUM; l_cnt1++) {
    ptr_tl = &g_st_can_cmd_transrslt_dat[l_cnt1];
    switch (l_index) {
      case 0: /* CAN section flame reset response transmission */
        ptr_tl->uc_cmd_id = CAN_CMDID_FUELCALC_REQ_TX;
        break;
      case 1: /* CAN start completion notification request transmission */
        ptr_tl->uc_cmd_id = CAN_CMDID_STARTUP_FIN_REQ_TX;
        break;
      case 2: /* CAN master reset information notification request transmission */
        ptr_tl->uc_cmd_id = CAN_CMDID_MRST_INFO_REQ_TX;
        break;
      case 3: /* CAN Version Request Send */
        ptr_tl->uc_cmd_id = CAN_CMDID_VERSION_REQ_TX;
        break;
      case 4: /* CAN connection node notification request transmission */
        ptr_tl->uc_cmd_id = CAN_CMDID_CONNECTION_NODE_REQ_TX;
        break;
    }
    l_cnt2++;
    if (l_cnt2 == CAN_COMMAND_TXRSLT_SNDID_NUM) {
      l_cnt2 = 0;
      l_index++;
    }
  }
  return;
}
