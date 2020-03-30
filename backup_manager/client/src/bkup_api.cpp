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

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <native_service/ns_backup.h>
#include <native_service/ns_message_center_if.h>
#include <agl_thread.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "bkup_api.h"
#include "bkup_backupmanagerlog.h"
#include "bkup_util.h"

#define BKUP_RETRY_MAX (10)
#define BKUP_RETRY_TIMER (100 * 1000)

static __thread HANDLE g_bkup_msg_handle;
static __thread HANDLE g_bkup_response_handle;

static int32_t BkupApiCheckTagid(PCSTR tag_id) {
  if (tag_id == NULL) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "tag_id is NULL");
    // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
    return BKUP_RET_ERRPARAM;
  }
  if (strlen(tag_id) >= BKUP_MAX_ITEM_NAME) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "tag_id is long:%zd", strlen(tag_id));
    // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
    return BKUP_RET_ERRPARAM;
  }
  return BKUP_RET_NORMAL;
}

static int32_t BkupApiCall(void *snd_buf, size_t snd_size, void *rcv_buf, uint32_t rcv_size) {
  int32_t ret = BKUP_RET_ERROR;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusFail;
  uint32_t real_rcvlen = 0;
  static __thread char thread_name[16];

  if (g_bkup_msg_handle == NULL) {
    char invoker_name[24];

    if (prctl(PR_GET_NAME, thread_name) < 0) {  // LCOV_EXCL_BR_LINE 5:prctl's error case.
      // LCOV_EXCL_START 5:prctl's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "prctl(PR_GET_NAME):%s", strerror(errno));
      goto exit;
      // LCOV_EXCL_STOP 5:prctl's error case.
    }

    g_bkup_msg_handle = McOpenSender(SERVICE_BACKUP_MANAGER_MSG_HANDLE_THREAD);
    if (g_bkup_msg_handle == NULL) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "McOpenSender");
      goto exit;
      // LCOV_EXCL_STOP 4: NSFW error case
    }

    // LCOV_EXCL_BR_START 4: NSFW error case
    if ((e_status = McCreateInvokerName(thread_name, 0, invoker_name, sizeof(invoker_name))) != eFrameworkunifiedStatusOK) {
    // LCOV_EXCL_BR_STOP 4: NSFW error case
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "McCreateInvokerName:%d", e_status);
      McClose(g_bkup_msg_handle);
      g_bkup_msg_handle = NULL;
      goto exit;
      // LCOV_EXCL_STOP 4: NSFW error case
    }

    g_bkup_response_handle = McOpenSyncReceiver(invoker_name);
    if (g_bkup_response_handle == NULL) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "McOpenSyncReceiver");
      McClose(g_bkup_msg_handle);
      g_bkup_msg_handle = NULL;
      goto exit;
      // LCOV_EXCL_STOP 4: NSFW error case
    }
  }

  for (int retry_cnt = 0; retry_cnt < BKUP_RETRY_MAX; retry_cnt++) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
    e_status = McInvokeSync(g_bkup_msg_handle, thread_name, BACKUP_CID, static_cast<UI_32>(snd_size), snd_buf, 0,
                           g_bkup_response_handle, rcv_size, rcv_buf, &real_rcvlen);
    if (e_status != eFrameworkunifiedStatusFail) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      break;
    }
    usleep(BKUP_RETRY_TIMER);
  }

  if (e_status != eFrameworkunifiedStatusOK) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "McInvokeSync:%d", e_status);
    // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
    if (e_status == eFrameworkunifiedStatusInvldParam) {
      ret = BKUP_RET_ERRPARAM;
    } else if (e_status == eFrameworkunifiedStatusErrOther) {
      ret = BKUP_RET_ERRINIT;
    } else if (e_status == eFrameworkunifiedStatusExit) {
      ret = BKUP_RET_ERRTERM;
    } else if (e_status == eFrameworkunifiedStatusFileLoadError) {
      ret = BKUP_RET_ERRNOENT;
    } else if (e_status == eFrameworkunifiedStatusAccessError) {
      ret = BKUP_RET_ERRSIZE;
    } else {
      ret = BKUP_RET_ERROR;
    }
    goto exit;
  }
  if (rcv_size != real_rcvlen) {
    goto exit;
  }
  ret = BKUP_RET_NORMAL;

exit:
  return ret;
}

int32_t Backup_DataRd(PCSTR tag_id, uint32_t ui_offset, void *pv_buf, uint32_t ui_size) {
  int32_t ret = BKUP_RET_NORMAL;
  bkup_protocol_header_t hdr;

  if ((ret = BkupApiCheckTagid(tag_id)) != BKUP_RET_NORMAL) {
    goto exit;
  }
  if (pv_buf == NULL) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Buf is NULL");
    // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
    ret = BKUP_RET_ERRPARAM;
    goto exit;
  }

  memset(&hdr, 0, sizeof(hdr));
  hdr.command = BKUP_CMD_READ;
  BkupStrlcpy(hdr.item_name, tag_id, sizeof(hdr.item_name));
  hdr.offset = ui_offset;
  hdr.size = ui_size;

  ret = BkupApiCall(&hdr, sizeof(hdr), pv_buf, ui_size);

exit:
  return ret;
}

int32_t Backup_DataWt(PCSTR tag_id, void *pv_buf, uint32_t ui_offset, uint32_t ui_size) {
  int32_t ret;
  char *snd_buf = NULL;
  size_t snd_buf_size = 0;
  bkup_protocol_header_t *hdr;

  if ((ret = BkupApiCheckTagid(tag_id)) != BKUP_RET_NORMAL) {
    goto exit;
  }
  if (pv_buf == NULL) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Buf is NULL");
    // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
    ret = BKUP_RET_ERRPARAM;
    goto exit;
  }

  // LCOV_EXCL_BR_START 5:mmap's error case.
  if ((snd_buf = BkupAnonMmap(sizeof(bkup_protocol_header_t) + ui_size)) == MAP_FAILED) {
  // LCOV_EXCL_BR_STOP 5:mmap's error case.
    // LCOV_EXCL_START 5:mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mmap:%s", strerror(errno));
    ret = BKUP_RET_ERROR;
    goto exit;
    // LCOV_EXCL_STOP 5:mmap's error case.
  }
  snd_buf_size = sizeof(bkup_protocol_header_t) + ui_size;

  hdr = reinterpret_cast<bkup_protocol_header_t *>(snd_buf);
  hdr->command = BKUP_CMD_WRITE;
  BkupStrlcpy(hdr->item_name, tag_id, sizeof(hdr->item_name));
  hdr->offset = ui_offset;
  hdr->size = ui_size;
  memcpy(snd_buf + sizeof(bkup_protocol_header_t), pv_buf, ui_size);

  ret = BkupApiCall(snd_buf, snd_buf_size, NULL, 0);

exit:
  if (snd_buf) {
    munmap(snd_buf, snd_buf_size);
  }
  return ret;
}

int32_t Backup_DataFil(PCSTR tag_id, uint32_t ui_offset, uint8_t uc_pat, uint32_t ui_size) {
  int32_t ret;
  bkup_protocol_header_t hdr;

  if ((ret = BkupApiCheckTagid(tag_id)) != BKUP_RET_NORMAL) {
    goto exit;
  }

  memset(&hdr, 0, sizeof(hdr));
  hdr.command = BKUP_CMD_FILL;
  BkupStrlcpy(hdr.item_name, tag_id, sizeof(hdr.item_name));
  hdr.offset = ui_offset;
  hdr.size = ui_size;
  hdr.fill_patern = uc_pat;

  ret = BkupApiCall(&hdr, sizeof(hdr), NULL, 0);

exit:
  return ret;
}

int32_t Backup_DataSz(PCSTR tag_id, uint32_t *pui_size) {
  int32_t ret;
  bkup_protocol_header_t hdr;

  if ((ret = BkupApiCheckTagid(tag_id)) != BKUP_RET_NORMAL) {
    goto exit;
  }
  if (pui_size == NULL) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Buf is NULL");
    // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
    ret = BKUP_RET_ERRPARAM;
    goto exit;
  }

  memset(&hdr, 0, sizeof(hdr));
  hdr.command = BKUP_CMD_SIZE;
  BkupStrlcpy(hdr.item_name, tag_id, sizeof(hdr.item_name));

  ret = BkupApiCall(&hdr, sizeof(hdr), pui_size, sizeof(uint32_t));

exit:
  return ret;
}

int32_t Backup_DataRdByNumID(uint32_t num_id, uint32_t ui_offset, void *pv_buf, uint32_t ui_size) {
  int32_t ret = BKUP_RET_NORMAL;
  bkup_protocol_header_t hdr;

  if (pv_buf == NULL) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Buf is NULL");
    // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
    ret = BKUP_RET_ERRPARAM;
    goto exit;
  }

  memset(&hdr, 0, sizeof(hdr));
  hdr.command = BKUP_CMD_READ_NUM;
  hdr.num_id = num_id;
  hdr.offset = ui_offset;
  hdr.size = ui_size;

  ret = BkupApiCall(&hdr, sizeof(hdr), pv_buf, ui_size);

exit:
  return ret;
}

int32_t Backup_DataSzByNumID(uint32_t num_id, uint32_t *pui_size) {
  int32_t ret;
  bkup_protocol_header_t hdr;

  if (pui_size == NULL) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Buf is NULL");
    // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
    ret = BKUP_RET_ERRPARAM;
    goto exit;
  }

  memset(&hdr, 0, sizeof(hdr));
  hdr.command = BKUP_CMD_SIZE_NUM;
  hdr.num_id = num_id;

  ret = BkupApiCall(&hdr, sizeof(hdr), pui_size, sizeof(uint32_t));

exit:
  return ret;
}

int32_t Backup_DataChk(PCSTR tag_id) {
  int32_t ret;
  bkup_protocol_header_t hdr;

  if ((ret = BkupApiCheckTagid(tag_id)) != BKUP_RET_NORMAL) {
    goto exit;
  }

  memset(&hdr, 0, sizeof(hdr));
  hdr.command = BKUP_CMD_CHECK;
  BkupStrlcpy(hdr.item_name, tag_id, sizeof(hdr.item_name));

  ret = BkupApiCall(&hdr, sizeof(hdr), NULL, 0);

exit:
  return ret;
}

int32_t Backup_DataDel(PCSTR tag_id) {
  int32_t ret;
  bkup_protocol_header_t hdr;

  ret = BkupApiCheckTagid(tag_id);
  if (ret != BKUP_RET_NORMAL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "tag_id is invalid");
    goto exit;
  }

  memset(&hdr, 0, sizeof(hdr));
  hdr.command = BKUP_CMD_DELETE;
  BkupStrlcpy(hdr.item_name, tag_id, sizeof(hdr.item_name));

  ret = BkupApiCall(&hdr, sizeof(hdr), NULL, 0);

exit:
  return ret;
}
