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
 *	FILE		:CAN_DeliveryData.cpp
 *	SYSTEM		:_CWORD107_
 *	SUBSYSTEM	:EXL process
 *	PROGRAM	:CAN thread CAN data delivery data management
 *	Module configuration	:CANDeliveryDataInit( )					CANDataDelivery Management Data Initialization Process
 *                    CAN_DeliveryCanidCheck( ) 				CAN ID registration and delivery registration checking process
 *                    CAN_DeliveryListCapacityCheck( ) 			CANDataDistribution destination management table free space confirmation processing
 *                    CAN_DeliveryCanidEntryCheck( )			CANDataChecking the status of delivery CAN ID control table entries
 *                    CANDeliveryEntry( )						CANDataDelivery registration process
 *                    CAN_SndListGenerate( ) 					CANData ID List Creation Process for Delivery
 ******************************************************************************/
#include "CAN_DeliveryData.h"

#include <string.h>  // NOLINT(build/include)
#include <native_service/frameworkunified_framework_if.h>
#include <other_service/strlcpy.h>

#include <utility>
#include <string>
#include <map>

#include "CAN_Thread.h"
#include "CAN_Delivery.h"
#include "Canif_API_Local.h"
//#include "can_hal.h"

/*************************************************/
/*           Global variables                      */
/*************************************************/
std::multimap<CANID, std::string> g_map_delivery_list;
std::multimap<uint16_t, std::string> g_map__CWORD29__delivery_list;

/*******************************************************************************
 * MODULE    : CANDeliveryDataInit
 * ABSTRACT  : CANDataDelivery Management Data Initialization Process
 * FUNCTION  : CANDataInitialize delivery management data for 
 * ARGUMENT  : void
 * NOTE      : 
 * RETURN    : void
 ******************************************************************************/
void CANDeliveryDataInit(void) {
  return;
}

/*******************************************************************************
 * MODULE    : CANDeliveryEntry
 * ABSTRACT  : CANDataDelivery registration process
 * FUNCTION  : CANDataRegister delivery 
 * ARGUMENT  : notifyId	: Addresses for delivery ID
 *             usCanNum		: Registered number
 *             *pulCanid	: CAN ID array pointers
 * NOTE      : 
 * RETURN    : void
 ******************************************************************************/
EFrameworkunifiedStatus CANDeliveryEntry(HANDLE h_app) {
  uint16_t i;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  CAN_DELIVERY_ENTRY rcv_msg;
  std::string notify_name;
  std::multimap<CANID, std::string>::iterator it;

  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg, sizeof(rcv_msg), eSMRRelease);  // LCOV_EXCL_BR_LINE 200: unexpect branch // NOLINT (whitespace/line_length)
  if (e_status != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN DeliveryEntry FrameworkunifiedGetMsgDataOfSize Error");  // LCOV_EXCL_LINE 4: NSFW error case // NOLINT (whitespace/line_length)
    goto exit;  // LCOV_EXCL_LINE 4: NSFW error case
  }

  for (i = 0; (i < rcv_msg.usCanNum) && (i < static_cast<uint16_t>(CAN_DELIVERY_CANID_ENTRY_MAX)); i++) {
    // Check for duplicate data
    // If the data to be registered is duplicated, an eFrameworkunifiedStatusOK is returned and the data is not registered.
    bool isDuplication = false;
    it = g_map_delivery_list.find(static_cast<std::multimap<CANID, std::string>::key_type>(rcv_msg.ulCanid[i]));
    for (; it != g_map_delivery_list.end(); ++it) {
      if (it->second == rcv_msg.notifyName) {
        isDuplication = true;
        break;
      }
    }
    if (isDuplication) {
      continue;
    }

    if ((uint32_t)g_map_delivery_list.size() >= CAN_DELIVERY_LIST_NUM) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN DeliveryEntry Table MAX size over");  // LCOV_EXCL_BR_LINE 15: marco defined in "native_service/ns_logger_if.h" // NOLINT (whitespace/line_length)
      e_status = eFrameworkunifiedStatusFail;
      break;
    }
    FRAMEWORKUNIFIEDLOG(ZONE_ENTRY, "", "[CAN] %04x %s", rcv_msg.ulCanid[i], rcv_msg.notifyName);  // LCOV_EXCL_LINE 4: NSFW error case // NOLINT (whitespace/line_length)
    notify_name = rcv_msg.notifyName;
    g_map_delivery_list.insert(std::make_pair(rcv_msg.ulCanid[i], notify_name));  // LCOV_EXCL_BR_LINE 200: unexpect branch // NOLINT (whitespace/line_length)
  }

exit:
  return e_status;
}

/*******************************************************************************
 * MODULE    : CAN_CWORD29_DeliveryEntry
 * ABSTRACT  : Process of registering the delivery of _CWORD29_ data
 * FUNCTION  : Register the delivery of _CWORD29_ data 
 * ARGUMENT  : h_app      : HANDLE
 * NOTE      :
 * RETURN    : EFrameworkunifiedStatus
 ******************************************************************************/
EFrameworkunifiedStatus CAN_CWORD29_DeliveryEntry(HANDLE h_app) {
  uint16_t i;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  CAN__CWORD29__DELIVERY_ENTRY rcv_msg;
  std::string notify_name;
  std::multimap<uint16_t, std::string>::iterator it;

  if (reinterpret_cast<HANDLE>(NULL) == h_app) {
    return eFrameworkunifiedStatusFail;
  }

  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, reinterpret_cast<PVOID>(&rcv_msg),
                                 static_cast<UI_32>(sizeof(CAN__CWORD29__DELIVERY_ENTRY)), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN DeliveryEntry FrameworkunifiedGetMsgDataOfSize Error");
    return e_status;
  }

  if (CAN_DELIVERY_OPC_ENTRY_MAX <= rcv_msg.usOpcNum) {
    e_status = eFrameworkunifiedStatusFail;
    return e_status;
  }

  for (i = 0; i < rcv_msg.usOpcNum; i++) {
    // Check for duplicate data
    // If the data to be registered is duplicated, an eFrameworkunifiedStatusOK is returned and the data is not registered.
    bool isDuplication = false;
    it = g_map__CWORD29__delivery_list.find(static_cast<std::multimap<uint16_t, std::string>::key_type>(rcv_msg.usOpc[i]));
    for (; it != g_map__CWORD29__delivery_list.end(); ++it) {
      if (it->second == rcv_msg.notifyName) {
        isDuplication = true;
        break;
      }
    }
    if (isDuplication) {
      continue;
    }

    if (static_cast<uint32_t>(g_map__CWORD29__delivery_list.size()) >= CAN_DELIVERY_LIST_NUM) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# _CWORD29_DeliveryEntry Table MAX size over");
      e_status = eFrameworkunifiedStatusFail;
      break;
    }
    FRAMEWORKUNIFIEDLOG(ZONE_ENTRY, "", "[OPC] %04x %s", rcv_msg.usOpc[i], rcv_msg.notifyName);
    notify_name = rcv_msg.notifyName;
    g_map__CWORD29__delivery_list.insert(std::make_pair(rcv_msg.usOpc[i], notify_name));
  }

  return e_status;
}

/*******************************************************************************
 * MODULE    : CANDeliveryBufferOut
 * ABSTRACT  : CAN shipping table log output processing
 * FUNCTION  : Output the CAN shipping table log 
 * ARGUMENT  : FILE *fp_log		: File pointer of the log output file
 * NOTE      : 
 * RETURN    : Thread ID
 ******************************************************************************/
void CANDeliveryBufferOut(FILE *fp_log) {  // LCOV_EXCL_START 8:dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  uint32_t usage_rate;
  std::multimap<CANID, std::string>::iterator it;

  if (NULL != fp_log) {
    /* Destination management table output */
    usage_rate = (((uint32_t)g_map_delivery_list.size() * 100) / CAN_DELIVERY_LIST_NUM);
    (void)fprintf(fp_log, "CAN DeliveryList_Buffer \n");
    (void)fprintf(fp_log, "BUFFER_Use: %04zu  BUFFER_Max: %04d  Usage_Rate: %04u \n",
                  g_map_delivery_list.size(), CAN_DELIVERY_LIST_NUM, usage_rate);
    if (usage_rate >= CAN_USAGE_RATE_THRESHOLD) {
      (void)fprintf(fp_log, "Warning: Buffer utilization exceeds a threshold.\n");
    }
    (void)fprintf(fp_log, "CanID:      notifyId:\n");

    for (it = g_map_delivery_list.begin(); it != g_map_delivery_list.end(); it++) {
      (void)fprintf(fp_log, "%08x    %s\n",
                    it->first,
                    it->second.c_str());
    }
  }
}
// LCOV_EXCL_STOP

#ifdef CAN_DEBUG
EFrameworkunifiedStatus CANAllDeleteDeliveryEntry(HANDLE h_app) {
  g_map_delivery_list.clear();
  g_map__CWORD29__delivery_list.clear();

  return eFrameworkunifiedStatusOK;
}  // LCOV_EXCL_BR_LINE 10 last Line
#endif
