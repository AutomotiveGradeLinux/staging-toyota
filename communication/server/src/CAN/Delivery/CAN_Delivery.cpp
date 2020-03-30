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
 *	Module configuration	:CANDataReceiveMsg()			CANDataProcess of Received Messages
 *                    CANDlcCheck()				DLC check process
 *                    CANDeliveryEntryMsgCheck()	CANDataDelivery registration message check process
 *                    CAN_DeliveryCtrlMsg()			CANDataDelivery control message processing				Not mounted
 *                    CAN_DeliveryCtrlMsgCheck()	CANDataDelivery control message check processing		Not mounted
 *                    CANCanidIfToUserCvt()		CAN ID Conversion Process 1
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @file    CAN_Delivery.cpp
 * @~english
 * @brief   CAN Delivery process
 */
#include "CAN_Delivery.h"
#include <native_service/frameworkunified_types.h>  // NOLINT(build/include)
#include <string.h>
#include "CAN_DeliveryData.h"
#include "CAN_CommWatch.h"
#include "CAN_TxMsg.h"

/*************************************************/
/*           Global variables                      */
/*************************************************/
const CANID kCulCanidCsumTbl[] = {/* CAN ID table that performs CSUM checks */
        (CANID)CAN_CSUM_CHECKTBL_STOP_CODE /* Stop code */
};

CAN_MSG_CANGWDATA g_cangw_msg_data; /* CANdata to be sent to the CANGW */
/*******************************************************************************
 * MODULE    : CANDataReceiveMsg
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    CAN data reply processing
 *
 * CAN data reply processing
 *
 * CANDataReceive processing 
 *
 * @~english
 * @note
 *   -# OPC check
 *   -# DLC check
 *   -# CSUM check
 *   -# Communication blackout clearness
 *   -# Filter processing
 *   -# ID list making for delivery
 *   -# Message sending
 *
 * @~english
 * @return  Data reception result
 * @retval  <RET_CAN_NORMAL>        Normality
 * @retval  <RET_CAN_ERROR_CANCEL>  Data annulment
 *
 * @~english
 * @param[in]     <pst_rcv_msg> Reception message
 */
RET_CAN CANDataReceiveMsg(HANDLE h_app, CAN_MSG_DATA *pst_rcv_msg) {
  RET_CAN ret = RET_CAN_NORMAL;  /* Return value of this function */
//  T_ICR_CMD_DATA *pst_msg_data;     /* Data part structure pointer */
  CANID can_id;                  /* CAN ID */
  uint8_t n_ta = CAN_NTA_INVALID; /* N_TA */
  uint8_t uc_dlc;                  /* DLC */
  uint8_t *puc_data_pos;            /* Data */
  uint8_t i;

//  pst_msg_data = reinterpret_cast<T_ICR_CMD_DATA *>(reinterpret_cast<void *>(&pst_rcv_msg->ucData[0]));
  g_cangw_msg_data.num = 0;

  /****** OPC check process ******/
//  switch (pst_msg_data->opc) {
//    case CAN_OPC_RX:           /* CANData reception notice */
#if 0
      /****** DLC check process ******/
      if (RET_CAN_NORMAL != (ret = CANDlcCheck(pst_msg_data))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Delivery DLC Error");
        CANDebugLogOut(CAN_DELIVERY_DLCERR, reinterpret_cast<void *>(pst_rcv_msg));
        break;
      }
#endif

//      can_id = CANCanidIfToUserCvt(&(pst_msg_data->data[0]));

//      if (pst_msg_data->opc == CAN_OPC_RX) {
//        uc_dlc = pst_msg_data->data[CAN_MSGBUF_DLC_POS];
//        puc_data_pos = &pst_msg_data->data[CAN_MSGBUF_DATA_POS];
//      }

//      ret = CANDeliveryRcvData(h_app, can_id, n_ta, uc_dlc, puc_data_pos, pst_msg_data->opc);
//      if (ret != RET_CAN_NORMAL) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
//        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Delivery Error");  // LCOV_EXCL_LINE 4: NSFW error case
//        CANDebugLogOut(CAN_DELIVERY_ERR, reinterpret_cast<void *>(pst_rcv_msg)); /* Delivery destination unregistered log output */  // LCOV_EXCL_LINE 4: NSFW error case // NOLINT (whitespace/line_length)
//      }
//      break;
//    case CAN_OPC_PAC_RX:
      /* Check CAN Data num */
//      if (0 >= pst_msg_data->data[0] || CAN_EXRCV_DATA_NUM < pst_msg_data->data[0]) {
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Delivery CAN Data num Error CAN Data num=%02X",
//        pst_msg_data->data[0]);  // LCOV_EXCL_BR_LINE 15: marco defined in "native_service/ns_logger_if.h" // NOLINT (whitespace/line_length)
//        break;
//      }
#if 0
      /****** DLC check process ******/
      if (RET_CAN_NORMAL != (ret = CANDlcCheck(pst_msg_data))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Delivery CAN Data size over");
        CANDebugLogOut(CAN_DELIVERY_DLCERR, reinterpret_cast<void *>(pst_rcv_msg));
      }

#endif
//      for (i = 0; i < pst_msg_data->data[0]; i++) {
//        can_id = CANPacCanidIfToUserCvt(&pst_msg_data->data[i * CAN_EXRCV_DATA_SIZE + CAN_MSGBUF_PAC_CANID_POS]);

//        uc_dlc = pst_msg_data->data[i * CAN_EXRCV_DATA_SIZE + CAN_MSGBUF_PAC_DLC_POS];
//        puc_data_pos = &pst_msg_data->data[i * CAN_EXRCV_DATA_SIZE + CAN_MSGBUF_PAC_DATA_POS];

//        if (RET_CAN_NORMAL != CANDeliveryRcvData(h_app, can_id, n_ta, uc_dlc, puc_data_pos, pst_msg_data->opc)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case // NOLINT (whitespace/line_length)
//          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//          ret = RET_CAN_ERROR_CANCEL;  // LCOV_EXCL_LINE 4: NSFW error case
//          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Delivery Error");  // LCOV_EXCL_LINE 4: NSFW error case
//          CANDebugLogOut(CAN_DELIVERY_ERR, reinterpret_cast<void *>(pst_rcv_msg)); /* Delivery destination unregistered log output */  // LCOV_EXCL_LINE 4: NSFW error case // NOLINT (whitespace/line_length)
//        }
//      }
//      break;
//    default:
//      ret = RET_CAN_ERROR_CANCEL;
//      break;
//  }
  // When the number of data to be delivered to CANGW_M is 1 or more, the data is sent to CANGW_M by CANDeliverySndMsgToCANGW.
  if (0 < g_cangw_msg_data.num) {
    // Sending CAN-data delivery messages to the CANGW
    if (FALSE == CANDeliverySndMsgToCANGW(h_app, &g_cangw_msg_data)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Delivery Error");  // LCOV_EXCL_LINE 4: NSFW error case
      ret = RET_CAN_ERROR_CANCEL;  // LCOV_EXCL_LINE 4: NSFW error case
    }
  }
  return (ret);
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Recieve data delivery before processing
 *
 * @~english
 * @return  Return value
 * @retval  <RET_CAN_NORMAL>            OK
 * @retval  <RET_CAN_ERROR_CANCEL>      Delivery Error
 *
 * @~english
 * @param[in]     <h_app>       Application handle
 * @param[in]     <ul_canid>    CAN ID
 * @param[in]     <n_ta>       N_TA
 * @param[in]     <uc_dlc>      DLC
 * @param[in]     <puc_data_pos> CAN Data
 * @param[in]     <opc>        OPC
 */
RET_CAN CANDeliveryRcvData(HANDLE h_app, CANID ul_canid, uint8_t n_ta,
                           uint8_t uc_dlc, uint8_t *puc_data_pos, uint8_t opc) {
  RET_CAN ret = RET_CAN_NORMAL;

  /****** Communication interruption clear processing ******/
  CANCommWatchClear(h_app, ul_canid);

  /****** CANDataSending a delivery message ******/
  if (FALSE == CANDeliverySndMsg(h_app, ul_canid, uc_dlc, puc_data_pos, opc, n_ta)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case // NOLINT (whitespace/line_length)
    /* CANDataDelivery message transmission processing */
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Delivery Error");  // LCOV_EXCL_LINE 4: NSFW error case
    ret = RET_CAN_ERROR_CANCEL;  // LCOV_EXCL_LINE 4: NSFW error case
  }

  return ret;
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Recieve _CWORD29_ data delivery before processing
 *
 * @~english
 * @return  Return value
 * @retval  <RET_CAN_NORMAL>            OK
 * @retval  <RET_CAN_ERROR_CANCEL>      Delivery Error
 *
 * @~english
 * @param[in]     <h_app>         Application handle
 * @param[in]     <us_opc>        OPC
 * @param[in]     <uc_dlc>        DLC
 * @param[in]     <puc_data_pos>  CAN Data
 */
RET_CAN CANDeliveryRcv_CWORD29_Data(HANDLE h_app, const uint16_t us_opc, uint32_t uc_dlc, uint8_t *puc_data_pos) {
  RET_CAN ret = RET_CAN_NORMAL;

  /****** CANDataSending a delivery message ******/
  if (FALSE == CANDelivery_CWORD29_SndMsg(h_app, us_opc, uc_dlc, puc_data_pos)) {
    /* CANDataDelivery message transmission processing */
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN Delivery Error");
    ret = RET_CAN_ERROR_CANCEL;
  }

  return ret;
}

/*******************************************************************************
 * MODULE    : CANDlcCheck
 ******************************************************************************/
/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    DLC check processing
 *
 * DLC check processing of received CAN data
 *
 * CANDataCheck the PROCESS 
 *
 * @~english
 * @note
 *   -# Length acquisition of header
 *   -# Positional change of DLC
 *   -# Data length calculation
 *   -# Data length comparison
 *
 * @~english
 * @param[in]     <pst_msg_data> Pointer of reference to data division of reception message
 */
//RET_CAN CANDlcCheck(const T_ICR_CMD_DATA *pst_msg_data) {  // LCOV_EXCL_START 8: dead code
//  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//  RET_CAN ret = RET_CAN_NORMAL; /* Return value of this function */
//  int32_t l_len1;                 /* For data length calculation */
//  int32_t l_len2;                 /* For data length calculation */
//  uint8_t offset = 0;            /* N_TA offset size */
//  uint8_t uc_dlc;                 /* DLC value */

  /* Length collection of headers */
//  l_len1 = (int32_t)pst_msg_data->d_length;

//  if ((uint8_t)CAN_OPC_PAC_RX == pst_msg_data->opc) {
//    l_len2 = (int32_t)(CAN_EXRCV_DATA_SIZE * pst_msg_data->data[0] + CAN_EXRCV_CANNUM_SIZE);

    /* DLC check of pacaging CAN data */
//    if (l_len1 != l_len2) {
//      ret = RET_CAN_ERROR_CANCEL; /* DLC error */
//    }
//  } else {
//    uc_dlc = (uint8_t)pst_msg_data->data[CAN_MSGBUF_DLC_POS];
    /* Data length considered from the DLC */
//    l_len2 = (int32_t)CAN_TRX_CANID_SIZE /* CAN ID sizes */
//            + (int32_t)CAN_TRX_DLC_SIZE /* Size of the DLC */
//            + (int32_t)offset           /* Size of N_TA */
//            + (int32_t)uc_dlc;           /* CANDataLength: DLC content */

    /* "Comparison of Length of headers and data length considered from DLCs */
    /* "Valid if the actual data length is equal to or less than the DLC */
//    if (l_len1 < l_len2) {
//      ret = RET_CAN_ERROR_CANCEL; /* DLC error */
//    } else if (uc_dlc > (uint8_t)CAN_DATA_SIZE) {
      /* CANDataDisabled when exceeding maximum size */
//      ret = RET_CAN_ERROR_CANCEL; /* DLC error */
//    } else {
      /* No DLC error */
//    }
//  }

//  return (ret);
//}
// LCOV_EXCL_STOP

/*******************************************************************************
 * MODULE    : CANCanidIfToUserCvt
 * ABSTRACT  : CAN ID Conversion Process 1
 * FUNCTION  : Convert CAN ID for interfaces to CAN ID for USER 
 * ARGUMENT  : *puc_operand	:Operand reference pointer
 * NOTE      :
 * RETURN    : CAN ID
 ******************************************************************************/
CANID CANCanidIfToUserCvt(uint8_t *puc_operand) {
  CANID can_id = 0; /* CAN ID */
  uint8_t *puc;      /* Generic pointer */
  uint32_t ul = 0;   /* General Variables */

  /*	Concept of conversion
	Input data format (8-bit data with the following structure)
		Base Address	+0	:ext_flag,    CAN ID[10:4]
						+1	:CAN ID[3:0], Empty[1:0], CAN ID[28:27]
						+2	:CAN ID[26:19]
						+3	:CAN ID[18:11]
	Output data format (32-bit data)
		Bit assignment		[31:29]		[28:0]
		CAN ID			Empty[2:0]	CAN ID[28:0]
*/

  puc = &puc_operand[CAN_MSGBUF_CANID_POS]; /* Get Base Address of CAN ID */
  ul = (uint32_t)puc[0] & 0x7F;            /* Contain [10:4] data */
  ul <<= 4;
  can_id |= (CANID)ul;
  ul = (uint32_t)puc[1] & 0xF0; /* Insert data of [3:0] */
  ul >>= 4;
  can_id |= (CANID)ul;
  ul = (uint32_t)puc[1] & 0x03; /* Populate [28:27] */
  ul <<= 27;
  can_id |= (CANID)ul;
  ul = (uint32_t)puc[2]; /* Contain [26:19] data */
  ul <<= 19;
  can_id |= (CANID)ul;
  ul = (uint32_t)puc[3]; /* Populate [18:11] */
  ul <<= 11;
  can_id |= (CANID)ul;

  return (can_id);
}

/*******************************************************************************
 * MODULE    : CANPacCanidIfToUserCvt
 * ABSTRACT  : CAN ID Conversion Process 1
 * FUNCTION  : Convert CAN ID for interfaces to CAN ID for USER 
 * ARGUMENT  : *puc_operand	:Operand reference pointer
 * NOTE      :
 * RETURN    : CAN ID
 ******************************************************************************/
CANID CANPacCanidIfToUserCvt(uint8_t *puc_operand) {
  CANID can_id = 0;
  uint8_t *puc;
  uint32_t ul = 0;

  puc = &puc_operand[0];
  ul = (uint32_t)puc[0] & 0x7F;
  ul <<= 4;
  can_id |= (CANID)ul;
  ul = (uint32_t)puc[1] & 0xF0;
  ul >>= 4;
  can_id |= (CANID)ul;

  return (can_id);
}
