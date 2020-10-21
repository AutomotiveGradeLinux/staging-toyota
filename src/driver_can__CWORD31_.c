/*
 * @copyright Copyright (c) 2020 TOYOTA MOTOR CORPORATION.
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


#include <stdio.h>
#include <string.h>
#include "can_mng_api.h"

#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <linux/can/raw.h>
#include <net/if.h>


#define DUMP_DIR	"/nv/driver-can"
#define DUMP_RCVID	"/nv/driver-can/dump_rcvid"
#define CAN_DATA_SIZE (sizeof(CanData))

static int fd_can = -1;
static struct sockaddr_can tx_address;
static CanCtlRcvId RcvId;
const char* device_name = "vcan0";

static UINT32 getData(CanCtlRcvId*, CanCtlApiCmd*);
static UINT32 getCanData(unsigned char*);
static UINT32 copyEnableCandata(UINT16* , unsigned char*, unsigned char*);
static UINT32 isOpendDriver(void);

UINT32  CanCtlApiOpen(CanCtlApiObj* pClientObj)
{
  int err_no;
  int ret;

  CAN_MNG_API_LOGT("FUNC IN");

  /*----------------------------------------------------------------------*/
  /*  Clear Object                                                        */
  /*----------------------------------------------------------------------*/
  if (pClientObj != NULL) {
    memset(pClientObj, 0, sizeof(*pClientObj));
  }

  if (fd_can != -1) {
    CAN_MNG_API_LOGT("Already Opened");
    CAN_MNG_API_LOGT("FUNC OUT");
    return CAN_CTL_RET_SUCCESS;
  }

  // open socket
  CAN_MNG_API_LOGT("open socket start");
  struct ifreq ifr = {0};
  fd_can = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  err_no = errno;
  CAN_MNG_API_LOGT("open socket end");
  if (fd_can == -1) {
    CAN_MNG_API_LOGE("Socket Open Error.");
    CAN_MNG_API_LOGE("(errno[%d]).", err_no);
    return CAN_CTL_RET_ERR_ERR;
  }

  // ioctl(SIOCGIFINDEX)
  strcpy(ifr.ifr_name, device_name);
  ret = ioctl(fd_can, SIOCGIFINDEX, &ifr);
  err_no = errno;
  if (ret < 0) {
    CAN_MNG_API_LOGE("ioctl(SIOCGIFINDEX) Error.");
    CAN_MNG_API_LOGE("(errno[%d]).", err_no);
    (void)close(fd_can);
    fd_can = -1;
    return CAN_CTL_RET_ERR_ERR;
  }

  // bind
  tx_address.can_family = AF_CAN;
  tx_address.can_ifindex = ifr.ifr_ifindex;

  ret = bind(fd_can, (struct sockaddr *)&tx_address, sizeof(tx_address));
  err_no = errno;
  if (ret < 0) {
    CAN_MNG_API_LOGE("Socket Bind Error.");
    CAN_MNG_API_LOGE("(errno[%d]).", err_no);
    (void)close(fd_can);
    fd_can = -1;
    return -1;
  }

  // Initialize RcvId Map
  memset(&RcvId, 0, sizeof(RcvId));

  CAN_MNG_API_LOGT("FUNC OUT");

  return CAN_CTL_RET_SUCCESS;
}

UINT32  CanCtlApiClose(CanCtlApiObj* pClientObj)
{
  /*----------------------------------------------------------------------*/
  /*  Clear Object                                                        */
  /*----------------------------------------------------------------------*/
  if (pClientObj != NULL) {
    memset(pClientObj, 0, sizeof(*pClientObj));
  }

  if (fd_can != -1) {
    (void)close(fd_can);
  }

  // Initialize fd info
  fd_can = -1;

  CAN_MNG_API_LOGT("FUNC OUT");

  return CAN_CTL_RET_SUCCESS;
}

UINT32  CanCtlApiSndCmd(CanCtlApiObj* pClientObj, CanCtlApiCmd* pSndCmd)
{
  int i;
  printf("%s: call_id=0x%x len=%d\n", 
    __func__, pClientObj->call_id, pSndCmd->len);
  for (i = 0; i < pSndCmd->len; i++) {
    printf("[%d]0x%x ", i, pSndCmd->data[i]);
    if ((i != 0) && ((i % 7) == 0))
      printf("\n"); 
  }
  printf("\n");


  int ret;
  int err_no;

  CAN_MNG_API_LOGT("FUNC IN");
  /*----------------------------------------------------------------------*/
  /*  Check Input Value                                                   */
  /*----------------------------------------------------------------------*/
  if (NULL == pSndCmd)
  {
    CAN_MNG_API_LOGE("%s(%p) invalid parameter.", __func__, pSndCmd);
    return CAN_CTL_RET_ERR_PARAM;
  }

  ret = isOpendDriver();
  if (ret != CAN_CTL_RET_SUCCESS) {
    CAN_MNG_API_LOGE("Not Open Driver.");
    return CAN_CTL_RET_ERR_ERR;
  }

  struct can_frame frame = {0};

  frame.can_id = pSndCmd->data[1] & 0x7F;
  frame.can_id <<= 4;
  frame.can_id |= (pSndCmd->data[2] & 0xF0) >> 4;
  frame.can_dlc = pSndCmd->data[3];
  frame.data[0] = pSndCmd->data[4];
  frame.data[1] = pSndCmd->data[5];
  frame.data[2] = pSndCmd->data[6];
  frame.data[3] = pSndCmd->data[7];
  frame.data[4] = pSndCmd->data[8];
  frame.data[5] = pSndCmd->data[9];
  frame.data[6] = pSndCmd->data[10];
  frame.data[7] = pSndCmd->data[11];

  CAN_MNG_API_LOGT("write() CanDataExtSendNotif start size = %d", (int)sizeof(frame));
  ret = write(fd_can, &frame, sizeof(frame));
  err_no = errno;
  CAN_MNG_API_LOGE("write() ret = %d).", ret);

  CAN_MNG_API_LOGT("write() CanDataExtSendNotif end");
  if (ret == -1) {
    CAN_MNG_API_LOGE("write() error(errno[%d]).", err_no);
    return CAN_CTL_RET_ERR_ERR;
  }

  CAN_MNG_API_LOGT("FUNC OUT");

  return CAN_CTL_RET_SUCCESS;
}

UINT32  CanCtlApiSetRcvId(CanCtlApiObj* pClientObj, CanCtlRcvId* pRcvId)
{
  FILE* fp;
  size_t nmemb;

  CAN_MNG_API_LOGT("FUNC IN");
  /*----------------------------------------------------------------------*/
  /*  Check Input Value                                                   */
  /*----------------------------------------------------------------------*/
  if (NULL == pRcvId)
  {
    CAN_MNG_API_LOGE("%s(%p) invalid parameter.", __func__, pRcvId);
    return CAN_CTL_RET_ERR_PARAM;
  }

  struct stat st;
  int ret = 0;
  if (stat(DUMP_DIR, &st) != 0) {
    ret = mkdir(DUMP_DIR, 0755);
  }
  if (ret != 0) {
    CAN_MNG_API_LOGE("Can not Created RcvId Dump dir.");
    return CAN_CTL_RET_ERR_ERR;
  }
  CAN_MNG_API_LOGT("fopen(DUMP_RCVID) start");
  fp = fopen(DUMP_RCVID, "wb");
  CAN_MNG_API_LOGT("fopen(DUMP_RCVID) end");
  if (fp == NULL) {
    CAN_MNG_API_LOGE("Can not Opened RcvId Dump file.");
    return CAN_CTL_RET_ERR_ERR;
  }

  CAN_MNG_API_LOGT("fwrite(pRcvId) start");
  nmemb = fwrite(pRcvId, sizeof(CanCtlRcvId), 1, fp);
  CAN_MNG_API_LOGT("fwrite(pRcvId) end");
  if (nmemb != 1) {
    CAN_MNG_API_LOGE("RcvId Dump file write error.");
    (void)fclose(fp);
    return CAN_CTL_RET_ERR_ERR;
  }

  CAN_MNG_API_LOGT("fclose() start");
  (void)fclose(fp);
  CAN_MNG_API_LOGT("fclose() end");

  CAN_MNG_API_LOGT("FUNC OUT");

  return CAN_CTL_RET_SUCCESS;
}

UINT32  CanCtlApiRcvCmd(CanCtlApiObj* pClientObj, CanCtlApiCmd* pRcvCmd)
{
  int ret;
  int enable_ret;

  CanCtlRcvId InitialRcvId;
  CanCtlRcvId RcvId;
  FILE* fp;
  size_t nmemb;

  CAN_MNG_API_LOGT("FUNC IN");
  /*----------------------------------------------------------------------*/
  /*  Check Input Value                                                   */
  /*----------------------------------------------------------------------*/
  if (NULL == pRcvCmd)
  {
    CAN_MNG_API_LOGE("%s(%p) invalid parameter.", __func__, pRcvCmd);
    return CAN_CTL_RET_ERR_PARAM;
  }

  ret = isOpendDriver();
  if (ret != CAN_CTL_RET_SUCCESS) {
    CAN_MNG_API_LOGE("Not Open Driver.");
    return CAN_CTL_RET_ERR_ERR;
  }

  // Restore RcvId
  CAN_MNG_API_LOGT("fopen(DUMP_RCVID) start");
  fp = fopen(DUMP_RCVID, "rb");
  CAN_MNG_API_LOGT("fopen(DUMP_RCVID) end");
  if (fp == NULL) {
    CAN_MNG_API_LOGE("Can not Opened RcvId Dump file.");
    return CAN_CTL_RET_ERR_ERR;
  }

  CAN_MNG_API_LOGT("fread(RcvId) start");
  nmemb = fread(&RcvId, sizeof(CanCtlRcvId), 1, fp);
  CAN_MNG_API_LOGT("fread(RcvId) end");
  if (nmemb != 1) {
    CAN_MNG_API_LOGE("RcvId Dump file read error.");
    (void)fclose(fp);
    return CAN_CTL_RET_ERR_ERR;
  }

  CAN_MNG_API_LOGT("fclose() start");
  (void)fclose(fp);
  CAN_MNG_API_LOGT("fclose() end");

  memset(&InitialRcvId.id, 0, sizeof(InitialRcvId.id));

  if (memcmp(InitialRcvId.id, RcvId.id, sizeof(RcvId.id)) == 0) {
    CAN_MNG_API_LOGE("No RecvId Maps.");
    return CAN_CTL_RET_ERR_ERR;
  }

  enable_ret = getData(&RcvId, pRcvCmd);

  if (enable_ret != CAN_CTL_RET_SUCCESS) {
    CAN_MNG_API_LOGE("No data founds.");
    return enable_ret;
  }

  CAN_MNG_API_LOGT("FUNC OUT");

  return CAN_CTL_RET_SUCCESS;
}

static UINT32 getData(CanCtlRcvId* pRcvId, CanCtlApiCmd* pRcvCmd) {
  int ret;
  int total_cnt = 0;
  int copy_cnt = 0;
  int i;
  unsigned char can_data[CAN_CTL_CMD_LEN_MAX+1];
  unsigned char enable_can_data[CAN_CTL_CMD_LEN_MAX];
  unsigned char store_can_data[CAN_CTL_CMD_LEN_MAX];
  unsigned char* scd;


  UINT16 id_map[CAN_CTL_CMD_ID_HI_NUM];

  CAN_MNG_API_LOGT("FUNC IN");

  memcpy(id_map, pRcvId->id, sizeof(id_map));

  memset(store_can_data, 0, sizeof(store_can_data));
  scd = store_can_data;

  while(1) {
    memset(can_data, 0, sizeof(can_data));
    memset(enable_can_data, 0, sizeof(enable_can_data));
    ret = getCanData(can_data);
    if (ret != CAN_CTL_RET_SUCCESS) {
      // In case of error, read next data
      CAN_MNG_API_LOGE("getCanData error.");
      continue;
    }

    // Extract data of "CAN data extended reception notification 4"
    copy_cnt = copyEnableCandata(id_map, can_data, enable_can_data);
    if( copy_cnt == 0 ){
      continue;
    }

    // Store in work buffer
    for (i = 0; i < copy_cnt; i++) {
      if (total_cnt >= CAN_CTL_MAX_RCV_CAN_SIZE) {
        CAN_MNG_API_LOGE("buffer over");
        break;
      }
      memcpy(scd, &enable_can_data[CAN_DATA_SIZE * i], CAN_DATA_SIZE);
      scd = scd + CAN_DATA_SIZE;
      total_cnt++;
    }

    // Check work buffer overflow
    if (total_cnt >= CAN_CTL_MAX_RCV_CAN_SIZE) {
      CAN_MNG_API_LOGE("buffer over");
      break;
    }
  }

  // If data exists, set and return
  if (total_cnt != 0) {
    pRcvCmd->len = 1 + (total_cnt * CAN_DATA_SIZE);
    pRcvCmd->data[0] = total_cnt;
    memcpy( &(pRcvCmd->data[1]), store_can_data, (total_cnt * CAN_DATA_SIZE));
  } else {
  // If there is no data, return with length 0
    pRcvCmd->len = 0;
    memset( &(pRcvCmd->data[0]), 0, sizeof(UINT8) * CAN_DAT_LEN_MAX );
    CAN_MNG_API_LOGE("data not found");
  }

  CAN_MNG_API_LOGT("FUNC OUT");

  return CAN_CTL_RET_SUCCESS;
}

static UINT32 getCanData(unsigned char *can_data) {
  int err_no;
  int i;

  CAN_MNG_API_LOGT("FUNC IN");

  CAN_MNG_API_LOGE("recvfrom start");

  int nbytes = 0;
  struct can_frame frame = {0};
  socklen_t addrlen = sizeof(struct sockaddr_can);
  nbytes = recvfrom(fd_can, &frame, sizeof(frame), 0, (struct sockaddr*)&tx_address, &addrlen);
  err_no = errno;
  CAN_MNG_API_LOGE("recvfrom end ret = %d",nbytes);
  if (nbytes == -1) {
    CAN_MNG_API_LOGE("Not Read CAN Driver(errno[%d]).", err_no);
    return CAN_CTL_RET_ERR_ERR;
  } else if (nbytes != CAN_MTU) {
    CAN_MNG_API_LOGE("Receive Error size: %d.", nbytes);
    return CAN_CTL_RET_ERR_ERR;
  }

  CAN_MNG_API_LOGT("Recvfrom CAN Data(start)");
  can_data[0] = (frame.can_id & 0x7F0) >> 4;
  can_data[1] = (frame.can_id & 0x0F) << 4;
  can_data[2] = frame.can_dlc;
  for (i = 0; i < frame.can_dlc; i++) {
    can_data[3+i] = frame.data[i];
    CAN_MNG_API_LOGE(" 0x%02x", frame.data[i]);
  }
  CAN_MNG_API_LOGT("");
  CAN_MNG_API_LOGT("Recvfrom Data(end)");

  CAN_MNG_API_LOGT("FUNC OUT");
  return CAN_CTL_RET_SUCCESS;
}

static UINT32 copyEnableCandata(UINT16* id_map, unsigned char* can_data, unsigned char* enable_can_data) {
  int id;
  UINT16 val;
  unsigned char* ecd = enable_can_data;
  UINT32 count = 0;

  CAN_MNG_API_LOGT("FUNC IN");
  // Search data of "CAN data extended reception notification 4"
  id  = can_data[0];
  val = can_data[1] >> 4;  // Shift right 4 bits and determine search position

  CAN_MNG_API_LOGE("data id =%x", id);
  CAN_MNG_API_LOGE("data val=%x", val);
  CAN_MNG_API_LOGE("file val=%x", id_map[id]);

  // If the target data is found (If the bit is on ?)
  if ((id_map[id] & (0x01 << val)) != 0) {
    CAN_MNG_API_LOGT("matched.");
    CAN_MNG_API_LOGE("id_map[%u]", id_map[id]);

    // Store data in work buffer
    memcpy(ecd, &can_data[0], CAN_DATA_SIZE);
    count++;
  }

  CAN_MNG_API_LOGT("FUNC OUT");
  return count;
}

UINT32  CanCtlApiRcvSpd(CanCtlApiObj* pClientObj, CanCtlApiCmd* pRcvCmd)
{
  // Note.
  // If vendor needs the special implementation about receiving the vehicle speed,
  // it should be implemented by vendor.
  return CAN_CTL_RET_SUCCESS;
}

static UINT32 isOpendDriver() {
  CAN_MNG_API_LOGT("FUNC IN");

  if (fd_can == -1) {
      CAN_MNG_API_LOGE("Not Open CAN Driver.");
      return CAN_CTL_RET_ERR_ERR;
  }

  CAN_MNG_API_LOGT("FUNC OUT");
  return CAN_CTL_RET_SUCCESS;
}
