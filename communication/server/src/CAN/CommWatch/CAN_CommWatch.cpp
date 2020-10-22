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

#include "CAN_CommWatch.h"

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <peripheral_service/Canif_API.h>
#include <utility>
#include <map>
#include <string>
#include "CAN_Thread.h"
#include "Thread_Common.h"

static CAN_CommWatchTable g_map_comm_watch_list_can;

void CANCommWatchInit(void) {
  g_map_comm_watch_list_can.clear();
  return;
}

static CAN_CommWatchTableIt CANCommWatchFind(CANID k, CAN_COMM_WATCH_VAL *val) {
  std::pair<CAN_CommWatchTableIt, CAN_CommWatchTableIt> range;
  CAN_CommWatchTableIt it;
  bool found = false;

  range = g_map_comm_watch_list_can.equal_range(k);
  for (it = range.first; it != range.second; it++) {
    if (!memcmp(&(it->second.notify_name),
                                  val->notify_name, sizeof(val->notify_name))) {
      found = true;
      break;
    }
  }

  if (!found)
    it = g_map_comm_watch_list_can.end();

  return it;
}

static bool CANCommWatchInsert(CANID k, CAN_COMM_WATCH_VAL *val) {
  CAN_CommWatchTableIt it = CANCommWatchFind(k, val);
  CAN_CommWatchTablePair p = std::make_pair((const CANID)k, *val);
  bool inserted = false;

  if (it == g_map_comm_watch_list_can.end()) {
    g_map_comm_watch_list_can.insert(p);
    inserted = true;
  }
  return inserted;
}

static void CANCommWatchUpdate(HANDLE h_app, CANID key,
                                             CAN_COMM_WATCH_VAL *val) {
  if (CANCommWatchInsert(key, val)) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "Can CommWatch : "
      "Entry (CANID=%x, DataID=%x, dst=%s, time=%d, cnt=%lu)",
      key, val->data_id,
      val->notify_name, val->set_time,
      g_map_comm_watch_list_can.size());
  } else {
    CAN_CommWatchTableIt it = CANCommWatchFind(key, val);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "Can CommWatch : Update the time "
      "(CANID=%x, DataID=%x, dst=%s, time:%d -> %d, cnt =%lu)",
      key, val->data_id, val->notify_name,
      it->second.set_time, val->set_time, 
      g_map_comm_watch_list_can.size());
    it->second.comm_watch_flag = val->comm_watch_flag;
    it->second.set_time = val->set_time;
    it->second.timer_cnt = val->timer_cnt;
    it->second.data_id = val->data_id;
  }

  return;
}

static void CANCommWatchStop(HANDLE h_app, CANID key,
                                           CAN_COMM_WATCH_VAL *val) {
  CAN_CommWatchTableIt it = CANCommWatchFind(key, val);
  if (it != g_map_comm_watch_list_can.end()) {
    g_map_comm_watch_list_can.erase(it);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "Can CommWatch : Stop the commwatch"
        "(CANID=%x, dst=%s, cnt=%lu)",
        key, val->notify_name, g_map_comm_watch_list_can.size());
  }

  return;
}

EFrameworkunifiedStatus CANCommWatch(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __func__, "@@@ Start communication CanCommWatch");

  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;
  CAN_COMM_WATCHEXT_MSG_DAT rcv_msg;
  CANID key;
  CAN_COMM_WATCH_VAL val;
  CAN_CommWatchTableIt it;

  memset(&rcv_msg, 0, sizeof(rcv_msg));
  memset(&val, 0, sizeof(val));
  e_status = FrameworkunifiedGetMsgDataOfSize(h_app, &rcv_msg, sizeof(rcv_msg), eSMRRelease);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedGetMsgDataOfSize Error(%d)", e_status);
    if (e_status == eFrameworkunifiedStatusInvldBufSize) {
      FrameworkunifiedClearMsgData(h_app);
    }
    goto cleanup;
  }

  // Create search data
  key = rcv_msg.ulCanid;
  snprintf(val.notify_name, sizeof(val.notify_name), "%s", rcv_msg.notifyName);
  val.data_id = rcv_msg.ulDid;
  val.comm_watch_flag = CAN_COMM_NORMAL;
  val.set_time = rcv_msg.usWatchTime;
  val.timer_cnt = rcv_msg.usWatchTime;
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__,
         "CAN CommWatch : dst=%s, CANID=%x, dataID=%x, time=%d",
         rcv_msg.notifyName, 
         rcv_msg.ulCanid, 
         rcv_msg.ulDid, rcv_msg.usWatchTime);

  if (0 == rcv_msg.usWatchTime) {
    CANCommWatchStop(h_app, key, &val);
  } else {
    CANCommWatchUpdate(h_app, key, &val);
  }
  e_status = eFrameworkunifiedStatusOK;
cleanup:
  return e_status;
}

static EFrameworkunifiedStatus CANCommWatchSndMsg(HANDLE h_app,
                                        CAN_COMM_WATCH_VAL *v, uint32_t cid) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;
  CAN_MSG_COMM_WATCHSTS msg;
  HANDLE h_client = NULL;

  memset(&msg, 0, sizeof(msg));
  msg.hdr.hdr.cid = (uint16_t)cid;
  msg.hdr.hdr.msgbodysize = sizeof(CAN_MSG_COMM_WATCHSTS_DAT);
  msg.hdr.hdr.rid = 0;
  msg.data.ulDid = v->data_id;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__,  "cid=%x msgbodysize=%x ulDid=%x",
         msg.hdr.hdr.cid, msg.hdr.hdr.msgbodysize, msg.data.ulDid);

  h_client = CommonFindSender(h_app, v->notify_name);
  if (!h_client){
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "CommonFindSender failed");
    goto cleanup;
  }

  e_status = FrameworkunifiedSendMsg(h_client, cid, sizeof(msg), &msg);
  if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__,
      "FrameworkunifiedSendMSg Error(e_status:%d to %s)",
      e_status, v->notify_name);
    goto cleanup;
  }

  e_status = eFrameworkunifiedStatusOK;
cleanup:
  return e_status;
}

static void CANCommWatchTimeoutCore(HANDLE h_app, CAN_COMM_WATCH_VAL *v) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;

  if (CAN_COMM_NORMAL != v->comm_watch_flag)
    return;

  if (v->timer_cnt != 0)
    v->timer_cnt--;

  if (v->timer_cnt != 0)
    return;

  e_status = CANCommWatchSndMsg(h_app, v, CID_CAN_COMM_STOP);
  if (eFrameworkunifiedStatusOK == e_status) {
     v->comm_watch_flag = CAN_COMM_STOP;
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "CANCommWatchSndMsg failed");
     // retry at next timeout.
  }
}

EFrameworkunifiedStatus CANCommWatchTimeout(HANDLE h_app) {
  CAN_CommWatchTableIt it;

  for (it = g_map_comm_watch_list_can.begin();
                            it != g_map_comm_watch_list_can.end(); it++) {
    CANCommWatchTimeoutCore(h_app, &(it->second));
  }

  return eFrameworkunifiedStatusOK;
}

static void CANCommWatchClearCore(HANDLE h_app, CANID id,
                                                CAN_COMM_WATCH_VAL *v) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;

  v->timer_cnt = v->set_time;

  if (CAN_COMM_STOP != v->comm_watch_flag)
    return;

  e_status = CANCommWatchSndMsg(h_app, v, CID_CAN_COMM_RESTORE);
  if (eFrameworkunifiedStatusOK == e_status) {
    v->comm_watch_flag = CAN_COMM_NORMAL;
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "Can CommWatch Clear : CANID=%x", id);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "CANCommWatchSndMsg failed");
     // retry at next timeout.
  }
}

void CANCommWatchClear(HANDLE h_app, CANID id) {
  std::pair<CAN_CommWatchTableIt, CAN_CommWatchTableIt> range;
  CAN_CommWatchTableIt it;

  range = g_map_comm_watch_list_can.equal_range(id);
  for (it = range.first; it != range.second; it++) {
    CANCommWatchClearCore(h_app, id, &(it->second));
  }
}
