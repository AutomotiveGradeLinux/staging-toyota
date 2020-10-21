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
#include <fcntl.h>
#include <unistd.h>
#include <linux/cdrom.h>
#include <sys/ioctl.h>
#include <deck_hal.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <stdio.h>
#include <string.h>
#include "deck_hal_deckhallog.h"

#define DECKHAL_THREAD "DeckHal"
#define DISC_PATH "/dev/sr0"
// static variables
static char g_notify_name[NOTIFY_NAME_MAX_SIZE + 1] = {0};
static bool g_register_status = false;  // deck_hal register status

// functions
EFrameworkunifiedStatus DeckNotifyInfo(const DECK_MSG_DELIVERY &DeliveryData);

EFrameworkunifiedStatus StopDiscRotation() {
    int fd;
    int ret;
    fd = open(DISC_PATH, O_RDONLY);
    if (fd < 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "open failed.");
        return eFrameworkunifiedStatusFail;
    }
    ret = ioctl(fd, CDROMSTOP);
    close(fd);
    if (ret != 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "ioctl failed.");
        return eFrameworkunifiedStatusFail;
    }
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus EjectDisc(DeckKind kind, DeckEjectMode mode) {
    int fd;
    int ret;
    fd = open(DISC_PATH, O_RDONLY);
    if (fd < 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "open failed.");
        return eFrameworkunifiedStatusFail;
    }
    ret = ioctl(fd, CDROM_LOCKDOOR);
    if (ret != 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "unlock disc failed.");
        close(fd);
        return eFrameworkunifiedStatusFail;
    }
    ret = ioctl(fd, CDROMEJECT);
    close(fd);
    if (ret != 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "eject disc failed.");
        return eFrameworkunifiedStatusFail;
    }
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus DeckHalSysSend(HANDLE h_app, PCSTR name, const DECK_CMD* cmd, uint8_t req_id) {
  EFrameworkunifiedStatus ret = eFrameworkunifiedStatusOK;

  if (NULL == h_app || NULL == name || NULL == cmd) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Input param is NULL\n");
    return eFrameworkunifiedStatusInvldParam;
  }

  if (NOTIFY_NAME_MAX_SIZE < strlen(name)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Input name length is overlapped\n");
    return eFrameworkunifiedStatusInvldParam;
  }
  // DeckHalSysSend aims to send command to micon,
  // since micon is stub now,so DeckHalSysSend is stub either.

  // deck_hal should notify chande mode status after received mode change info
  // from micon,since now micon is stub,notify change mode status to vehicle
  // immediately.
  // the data notify to vehile is just a example.
  DECK_MSG_DELIVERY data;
  data.cmd.cmdhdr.lgadr_from = 0x44;  // vehile:LSDRV_ADR_DVD_P
  data.cmd.cmdhdr.lgadr_to = 0xC8;  // vehicle:LSDRV_OPC_DRV_M_CH_A_DVD
  data.cmd.cmdhdr.opc = 0xC8;  // vehicle:LSDRV_OPC_DRV_M_CH_A_DVD
  DeckNotifyInfo(data);

  return ret;
}

EFrameworkunifiedStatus DeckHalRegister(HANDLE h_app, PCSTR notify_name) {
  if (NULL == h_app) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Input h_app is NULL\n");
    return eFrameworkunifiedStatusInvldParam;
  }

  if (NULL == notify_name) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Input notify_name is NULL\n");
    return eFrameworkunifiedStatusInvldParam;
  }

  if (strlen(notify_name) > NOTIFY_NAME_MAX_SIZE) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Input notify_name length is overlapped\n");
    return eFrameworkunifiedStatusInvldParam;
  }

  strncpy(g_notify_name, notify_name, NOTIFY_NAME_MAX_SIZE);

  // now micon is stub,in the future,should register to micon

  g_register_status = true;

  return eFrameworkunifiedStatusOK;
}

// DeckNotifyInfo aims to send deck infos to vehicle
// if micon is not stub in the future,after received meesage from micon
// or any other ways triggered by micon,deck_hal should call this function
// with different input data to send these data to vehicle.
EFrameworkunifiedStatus DeckNotifyInfo(const DECK_MSG_DELIVERY &DeliveryData) {
  EFrameworkunifiedStatus ret = eFrameworkunifiedStatusOK;
  HANDLE send_handle = NULL;

  if (g_register_status == true) {
    send_handle = McOpenSender(g_notify_name);
    if (send_handle == NULL) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedMcOpenSender to _CWORD121_Shadow Failed.");
      return eFrameworkunifiedStatusFail;
    }

    ret = McSend(send_handle, DECKHAL_THREAD, CID_DECK_SYS_DELIVERY,
                 sizeof(DECK_MSG_DELIVERY), &DeliveryData);

    if (eFrameworkunifiedStatusOK != ret) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Failed to send CID_DECK_SYS_DELIVERY.");
      ret = eFrameworkunifiedStatusFail;
    }

    if (send_handle != NULL) {
      McClose(send_handle);
      send_handle = NULL;
    }
  }

  return ret;
}
