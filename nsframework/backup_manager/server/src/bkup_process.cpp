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

#include <dirent.h>
#include <fcntl.h>
#include <libgen.h>
#include <inttypes.h>
#include <mqueue.h>
#include <agl_thread.h>
#include <poll.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <unistd.h>
#include <native_service/frameworkunified_framework_if.h>
#include <system_service/ss_system_manager_if.h>
#include <nv_hal.h>
#include <aglpath.h>
#include <cerrno>
#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <utility>
#include "bkup_api.h"
#include "bkup_backupmanagerlog.h"
#include "bkup_param.h"
#include "bkup_crc32.h"
#include "bkup_util.h"

#define BKUP_THREAD_NAND "NS_BkupNAND"
#define BKUP_THREAD_DELAY "NS_BkupDelay"
const char kBkupPrexixEncryption[] = "ENC_";
const size_t kBkupPrexixEncryptionSize = 4;

typedef void (* signal_handler)(int);
extern void SignalHandlerFuncForMsgHandleThread(int signum);
extern pthread_t g_delay_thread_id;
extern pthread_t g_nand_thread_id;

#define MAX_FD_EPOLL 8

/**
 * internal state
 */
typedef enum {
  BKUP_STATE_INIT = 0,
  BKUP_STATE_NORMAL,
  BKUP_STATE_TERM,
} bkup_state_t;
static bkup_state_t g_bkup_internal_state = BKUP_STATE_INIT;


/**
 * delay thread command
 */
typedef enum {
  BKUP_DELAY_CMD_TERM = 0,
  BKUP_DELAY_CMD_PRE_ACCOFF,
  BKUP_DELAY_CMD_REGIST,
} bkup_delay_cmd_t;


/**
 * inter thread communication message
 */
typedef union {
  struct {
    char client_name[16];
    uint32_t seq_id;
    char *rcv_buf;
    int rcv_size;
  } nand;
  struct {

//    bool terminate;
    bkup_delay_cmd_t cmd;

    int event_fd;
    char item_name[64];
  } delay;
} bkup_itc_message_t;

/**
 * reference response function
 */
typedef struct {
  int (*set_response)(void *, void *, int, EFrameworkunifiedStatus);
  void *opt;
} bkup_response_t;

/**
 * response MQ map(TLS)
 */
typedef std::map<std::string, HANDLE> g_bkup_response_mq_t;
static __thread g_bkup_response_mq_t *g_bkup_response_mq;

/**
 * NAND thread itc MQ
 */
static mqd_t g_bkup_nand_mqd = -1;

/**
 * Delay thread itc MQ
 */
static mqd_t g_bkup_delay_mqd = -1;

static int g_bkup_nand_tid;
static int g_bkup_delay_tid;

/**
 * prototype declared
 */
static void BkupApilog(bkup_response_t *response, enum NvHalMedia current_media,
                       bkup_protocol_header_t *hdr,
                       bkup_query_result_t *query_result, char *buf,
                       uint32_t crc32, EFrameworkunifiedStatus ret_value);

/**
 * check status of file spicefied by path
 */
static int BkupCheckStatFromNv(enum NvHalMedia media, const char *path,
                               int check_size);

/**
 * rwlock unlock
 */
static inline void BkupRwlockUnlock(pthread_rwlock_t *rwlock, const char *path);

/**
 * rwlock read lock
 */
static inline void BkupRwlockRdlock(pthread_rwlock_t *rwlock, const char *path);

/**
 * rwlock write lock
 */
static inline void BkupRwlockWrlock(pthread_rwlock_t *rwlock, const char *path);

/**\ingroup CheckNv
 * \~english @par Brief
 *      Check the data stored on the storage device and check the data size.
 * \~english @param [in] media
 *      enum NvHalMedia           - Media type
 * \~english @param [in] filename
 *      const char*               - Concatenated string of "category name/item name"
 * \~english @param [in] size
 *      uint32_t                  - Data size to be checked
 * \~english @retval eFrameworkunifiedStatusOK OK
 * \~english @retval eFrameworkunifiedStatusFileLoadError The file does not exist.
 * \~english @retval eFrameworkunifiedStatusAccessError Check size failed
 * \~english @retval eFrameworkunifiedStatusInvldParam Parameter error
 * \~english @par Preconditons
 *      - none
 * \~english @par Conditions of processing failure
 *      - If the media (media) specified in the parameter is smaller than NVHALMEDIA_CACHEDRAM or larger than NVHALMEDIA_NAND.\n
 *      [eFrameworkunifiedStatusInvldParam]
 *      - The identifier specified in the arguments is a NULL.[eFrameworkunifiedStatusInvldParam]
 *      - When the size specified in the arguments is 0.[eFrameworkunifiedStatusInvldParam]
 *      - Internal I/O handling errors (lstat).[eFrameworkunifiedStatusFileLoadError]
 *      - The size specified by the parameter does not correspond to the file size specified by the identifier.[eFrameworkunifiedStatusAccessError]
 */
static EFrameworkunifiedStatus CheckNv(enum NvHalMedia media, const char *filename, uint32_t size) {
  uint32_t size_;
  EFrameworkunifiedStatus ret;
  ret = GetSizeNv(media, filename, &size_);  // Get size
  if (ret != eFrameworkunifiedStatusOK) {  // If the GetSizeNv fails, the error is returned unchanged.
    return ret;
  }
  if (size_ != size) {  // Returns eFrameworkunifiedStatusAccessError if the sizes do not match.
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "invalid size:%d\n", size);
    return eFrameworkunifiedStatusAccessError;
  }
  return eFrameworkunifiedStatusOK;  // Returns eFrameworkunifiedStatusOK if size matches
}



/**
 * read file spicefied by id
 *
 * \retval  0 read success
 * \retval -1 crc error
 * \retval -2 read failed
 * \retval -3 invalid param
 */
static int32_t BkupBufferReadFromNv(const enum NvHalMedia media, const char* path,
                                    char *buf, size_t size, uint32_t *crc32) {
  int32_t ret = 0;
  EFrameworkunifiedStatus nv_ret = eFrameworkunifiedStatusFail;
  const uint32_t size_all = static_cast<uint32_t>(size + sizeof(uint32_t));
  void* buffer = MAP_FAILED;
  uint8_t* read_buffer = NULL;
  uint32_t calc_crc32, get_crc32;

  if (path == NULL) {  // LCOV_EXCL_BR_LINE 6:double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return -1;  // LCOV_EXCL_LINE 6:double check
  }

  if (!buf) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "buf is NULL");
    return -1;
  }
  if ((buffer = BkupAnonMmap(size_all)) == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5:mmap's error case.
    // LCOV_EXCL_START 5:mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mmap:map failed");
    ret = -1;
    goto exit;
    // LCOV_EXCL_STOP 5:mmap's error case.
  }

  read_buffer = reinterpret_cast<uint8_t*>(buffer);
  nv_ret = ReadNv(media, path, read_buffer, size_all);
  if (nv_ret == eFrameworkunifiedStatusOK) {
    const void* const src_buffer = reinterpret_cast<void *>(read_buffer + size);
    void* dest_buffer = reinterpret_cast<void *>(&get_crc32);
    memcpy(dest_buffer, src_buffer, sizeof(uint32_t));
    calc_crc32 = BkupCrc32(read_buffer, size);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    if (get_crc32 != calc_crc32) {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "invalid crc(%s):%#x:%#x", path, get_crc32, calc_crc32);
      // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
      ret = -1;
      goto exit;
    }
    if (crc32 != NULL) {
      *crc32 = get_crc32;
    }
    dest_buffer = reinterpret_cast<void *>(buf);
    memcpy(dest_buffer, buffer, size);
  } else if (nv_ret == eFrameworkunifiedStatusInvldParam) {  // LCOV_EXCL_BR_LINE 6:double check
    ret = -3;
  } else {
    ret = -2;
  }
exit:
  if (buffer != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5:mmap's error case.
    munmap(buffer, size_all);
  }

  return ret;
}

/*
 * buffer -> file write
 */
static int32_t BkupBufferWriteToNv(const enum NvHalMedia media, const char* path,
                                   char *buf, size_t size, uint32_t *crc32) {
  if (buf == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "buf is NULL");
    return eFrameworkunifiedStatusNullPointer;
  }
  int32_t ret = 0;
  EFrameworkunifiedStatus nv_ret = eFrameworkunifiedStatusFail;
  const uint32_t size_all = static_cast<uint32_t>(size + sizeof(uint32_t));
  char *buffer = reinterpret_cast<char *>(MAP_FAILED);
  uint32_t calc_crc32;

  if ((buffer = BkupAnonMmap(size_all)) == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5:mmap's error case.
    // LCOV_EXCL_START 5:mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mmap:%s", strerror(errno));
    ret = -1;
    goto exit;
    // LCOV_EXCL_STOP 5:mmap's error case.
  }

  calc_crc32 = BkupCrc32(buf, size);  // LCOV_EXCL_BR_LINE 11:unexpected branch
  if (crc32) {
    *crc32 = calc_crc32;
  }

  memcpy(buffer, buf, size);
  memcpy(buffer + size, &calc_crc32, sizeof(uint32_t));

  nv_ret = WriteNv(media, path, reinterpret_cast<uint8_t *>(buffer), size_all);
  if (nv_ret != eFrameworkunifiedStatusOK) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "write to nv(%s):%s", path, strerror(ret));
    // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
    ret = -1;
  }

exit:
  if (buffer != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5:mmap's error case.
    munmap(buffer, size_all);
  }

  return ret;
}

/**
 * inter media copy
 */
static int BkupMediaCopyFromNv(char *category_name, const char *item_name,
                               enum NvHalMedia src, enum NvHalMedia dst,
                               pthread_rwlock_t *src_rwlock,
                               pthread_rwlock_t *dst_rwlock) {
  if (category_name == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "category_name is NULL");
    return eFrameworkunifiedStatusNullPointer;
  }

  if (item_name == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "item_name is NULL");
    return eFrameworkunifiedStatusNullPointer;
  }
  int ret = -1;
  uint32_t crc32;
  char id_path[FILENAME_MAX] = {0};
  char *buffer = reinterpret_cast<char *>(MAP_FAILED);

  bkup_query_result_t query_result;
  BkupParamGet(item_name, &query_result);  // LCOV_EXCL_BR_LINE 11:unexpected branch

  char prefix_name[kBkupPrexixEncryptionSize + 1] = {0};
  if (query_result.encrypt != false) {
    strncpy(prefix_name, kBkupPrexixEncryption, kBkupPrexixEncryptionSize);
  }
  snprintf(id_path, FILENAME_MAX, "%s/%s%s", category_name, prefix_name, item_name);

  if ((buffer = BkupAnonMmap(query_result.size)) == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5:mmap's error case.
    // LCOV_EXCL_START 5:mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mmap:map failed");
    ret = eFrameworkunifiedStatusFail;
    goto exit;
    // LCOV_EXCL_STOP 5:mmap's error case.
  }

  BkupRwlockRdlock(src_rwlock, id_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch
  if ((ret = BkupBufferReadFromNv(src, id_path, buffer, query_result.size,
                                  &crc32)) >= 0) {
    // LCOV_EXCL_BR_START 11:unexpected branch
    BkupRwlockWrlock(dst_rwlock, id_path);
    ret = BkupBufferWriteToNv(dst, id_path, buffer, query_result.size, &crc32);
    BkupRwlockUnlock(dst_rwlock, id_path);
    // LCOV_EXCL_BR_STOP 11:unexpected branch
  }
  BkupRwlockUnlock(src_rwlock, id_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch

exit:
  if (buffer != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5:mmap's error case.
    munmap(buffer, query_result.size);
  }

  return ret;
}

/**
 * make backup path
 */
static int BkupMakeBkupPathToNvId(const char *category_name,
                                  const char *item_name, char **path_buf,
                                  int buf_size) {
  int ret = -1;
  char prefix_name[kBkupPrexixEncryptionSize + 1] = {0};

  if ((*path_buf = BkupAnonMmap(buf_size)) == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5:mmap's error case.
    // LCOV_EXCL_START 5:mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mmap:%s", strerror(errno));
    goto exit;
    // LCOV_EXCL_STOP 5:mmap's error case.
  }
  bkup_query_result_t query_result;
  if (BkupParamGet(item_name, &query_result) < 0) {  // LCOV_EXCL_BR_LINE 6:Excluded due to checking by upper-level function
    // LCOV_EXCL_START 6:Excluded due to checking by upper-level function
    AGL_ASSERT_NOT_TESTED();
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid item_name:%s", item_name);
    goto exit;
    // LCOV_EXCL_STOP 6:Excluded due to checking by upper-level function
  }
  if (query_result.encrypt != false) {
    strncpy(prefix_name, kBkupPrexixEncryption, kBkupPrexixEncryptionSize);
  }
  snprintf(*path_buf, static_cast<size_t>(buf_size), "%s/%s%s",
           category_name, prefix_name, item_name);
  ret = 0;

exit:
  return ret;
}

/**
 * delete data
 */
static int BkupDeleteDataFromNv(const enum NvHalMedia media, const char* const path) {
  int ret = 0;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  if (path == NULL) {  // LCOV_EXCL_BR_LINE 6:Excluded due to checking by upper-level function
    // LCOV_EXCL_START 6:Excluded due to checking by upper-level function
    AGL_ASSERT_NOT_TESTED();
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "path is NULL");
    ret = -1;
    return ret;
    // LCOV_EXCL_STOP 6:Excluded due to checking by upper-level function
  }
  const size_t path_size = static_cast<size_t>(std::string(path).length());
  if (path_size  > FILENAME_MAX) {  // LCOV_EXCL_BR_LINE 6:Excluded due to checking by upper-level function
    // LCOV_EXCL_START 6:Excluded due to checking by upper-level function
    AGL_ASSERT_NOT_TESTED();
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "path size error");
    ret = -1;
    return ret;
    // LCOV_EXCL_STOP 6:Excluded due to checking by upper-level function
  }

  uint32_t size = 0;
  e_status = GetSizeNv(media, path, &size);
  if (e_status == eFrameworkunifiedStatusFileLoadError) {
    // If the file does not exist, deleet it successfully
    return ret;
  } else if (e_status != eFrameworkunifiedStatusOK) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "GetSizeNv e_status:%d\n", e_status);
    ret = -1;
    return ret;
  } else {
    // no proc
  }

  e_status = DeleteNv(media, path);
  if (e_status != eFrameworkunifiedStatusOK) {
    ret = -1;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "DeleteNv e_status:%d\n", e_status);
  }
  return ret;
}

/**
 * get rwlock
 */
static pthread_rwlock_t *BkupGetRwlock(const char *item_name,
                                       enum NvHalMedia media,
                                       bkup_query_result_t *query_result) {
  int ret;
  pthread_rwlock_t *rwlock, *org_rwlock;
  pthread_rwlockattr_t attr;

  if (query_result == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "query_result is NULL");
    return NULL;
  }

  if (media != NVHALMEDIA_BACKUPDRAM || query_result->backup_dram == false || query_result->nand == false) {
    return NULL;
  }
  // backupDram && nand
  if (query_result->opt != NULL) {
    return reinterpret_cast<pthread_rwlock_t *>(query_result->opt);
  }

  if ((ret = pthread_rwlockattr_init(&attr)) != 0) {   // LCOV_EXCL_BR_LINE 5:pthread_rwlockattr_init's error case.
    // LCOV_EXCL_START 5:pthread_rwlockattr_init's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "pthread_rwlockattr_init:%s", strerror(ret));
    return NULL;
    // LCOV_EXCL_STOP 5:pthread_rwlockattr_init's error case.
  }

  // LCOV_EXCL_BR_START 5:pthread_rwlockattr_setkind_np's error case.
  if ((ret = pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP)) != 0) {
  // LCOV_EXCL_BR_STOP 5:pthread_rwlockattr_setkind_np's error case.
    // LCOV_EXCL_START 5:pthread_rwlockattr_setkind_np's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "pthread_rwlockattr_setkind_np:%s", strerror(ret));
    return NULL;
    // LCOV_EXCL_STOP 5:pthread_rwlockattr_setkind_np's error case.
  }

  // LCOV_EXCL_BR_START 5:malloc's error case.
  if ((rwlock = reinterpret_cast<pthread_rwlock_t *>(malloc(sizeof(pthread_rwlock_t)))) == NULL) {
  // LCOV_EXCL_BR_STOP 5:malloc's error case.
    // LCOV_EXCL_START 5:malloc's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "malloc failed");
    return NULL;
    // LCOV_EXCL_STOP 5:malloc's error case.
  }

  if ((ret = pthread_rwlock_init(rwlock, &attr)) != 0) {  // LCOV_EXCL_BR_LINE 5:pthread_rwlock_init's error case.
    // LCOV_EXCL_START 5:pthread_rwlock_init's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "pthread_rwlock_init:%s", strerror(ret));
    free(rwlock);
    return NULL;
    // LCOV_EXCL_STOP 5:pthread_rwlock_init's error case.
  }

  // LCOV_EXCL_BR_START 11:unexpected branch
  org_rwlock = reinterpret_cast<pthread_rwlock_t *>(BkupParamSetOpt(item_name, rwlock));
  // LCOV_EXCL_BR_STOP 11:unexpected branch
  if (org_rwlock == reinterpret_cast<void *>(-1)) {  // LCOV_EXCL_BR_LINE 6:org_rwlock will not be -1
    // LCOV_EXCL_START 6:org_rwlock will not be -1
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid item_name:%s", item_name != 0 ? item_name : NULL);
    free(rwlock);
    return NULL;
    // LCOV_EXCL_STOP 6:org_rwlock will not be -1
  } else if (org_rwlock != NULL) {  // LCOV_EXCL_BR_LINE 5:gcc builtin function's error case
    // conflict
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    free(rwlock);  // LCOV_EXCL_LINE 5:gcc  builtin function's error case
    rwlock = org_rwlock;  // LCOV_EXCL_LINE 5:gcc  builtin function's error case
  }

  query_result->opt = rwlock;
  return rwlock;
}

/**
 * rwlock read lock
 */
static inline void BkupRwlockRdlock(pthread_rwlock_t *rwlock, const char *path) {
  if (rwlock) {
    int ret;
    if ((ret = pthread_rwlock_rdlock(rwlock)) != 0) {  // LCOV_EXCL_BR_LINE 5:pthread_rwlock_rdlock's error case.
      // LCOV_EXCL_START 5:pthread_rwlock_rdlock's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "pthread_rwlock_rdlock(%s):%s", path, strerror(ret));
      // LCOV_EXCL_STOP 5:pthread_rwlock_rdlock's error case.
    }
  }
}

/**
 * rwlock write lock
 */
static inline void BkupRwlockWrlock(pthread_rwlock_t *rwlock, const char *path) {
  if (rwlock) {
    int ret;
    if ((ret = pthread_rwlock_wrlock(rwlock)) != 0) {  // LCOV_EXCL_BR_LINE 5:pthread_rwlock_wrlock's error case.
      // LCOV_EXCL_START 5:pthread_rwlock_wrlock's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "pthread_rwlock_wrlock(%s):%s", path, strerror(ret));
      // LCOV_EXCL_STOP 5:pthread_rwlock_wrlock's error case.
    }
  }
}

/**
 * rwlock unlock
 */
static inline void BkupRwlockUnlock(pthread_rwlock_t *rwlock, const char *path) {
  if (rwlock) {
    int ret;
    if ((ret = pthread_rwlock_unlock(rwlock)) != 0) {  // LCOV_EXCL_BR_LINE 5:pthread_rwlock_unlock's error case.
      // LCOV_EXCL_START 5:pthread_rwlock_unlock's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "pthread_rwlock_unlock(%s):%s", path, strerror(ret));
      // LCOV_EXCL_STOP 5:pthread_rwlock_unlock's error case.
    }
  }
}

/**
 * inter thread communication MQ open
 */
static mqd_t BkupItcMqopen(const char *mq_name, int flags) {
  struct mq_attr attr;

  if (mq_name == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "mq_name is NULL");
    return -1;
  }

  attr.mq_flags = 0;
  attr.mq_maxmsg = 256;
  attr.mq_msgsize = sizeof(bkup_itc_message_t);

  return mq_open(mq_name, flags, 0660, &attr);
}

/**
 * write & rename
 */
static int BkupWriteRename(const char *category_name, const char *item_name,
                           enum NvHalMedia media, char *buf, int size,
                           pthread_rwlock_t *rwlock) {
  int ret = -1;
  char *full_path = reinterpret_cast<char *>(MAP_FAILED);

  // LCOV_EXCL_BR_START 5:mmap's error case.
  if (BkupMakeBkupPathToNvId(category_name, item_name, &full_path, FILENAME_MAX) < 0) {
  // LCOV_EXCL_BR_STOP 5:mmap's error case.
    // LCOV_EXCL_START 5:mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    goto exit;
    // LCOV_EXCL_STOP 5:mmap's error case.
  }

  BkupRwlockWrlock(rwlock, full_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch
  if (BkupBufferWriteToNv(media, full_path, buf, size, NULL) < 0) {
    BkupRwlockUnlock(rwlock, full_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    goto exit;
  }
  BkupRwlockUnlock(rwlock, full_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch

  ret = size;

exit:
  if (full_path != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5:mmap's error case.
    munmap(full_path, FILENAME_MAX);
  }
  return ret;
}

/**
 * stat check
 *
 * \retval 0 file exists
 * \retval -1 file not exists
 * \retval -2 size error
 */
static int BkupCheckStatFromNv(enum NvHalMedia media, const char *path,
                               int check_size) {
  uint32_t size = check_size + static_cast<uint32_t>(sizeof(uint32_t));
  const EFrameworkunifiedStatus ret = CheckNv(media, path, size);

  if (path == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "path is NULL");
    return -1;
  }

  if (eFrameworkunifiedStatusOK == ret) {
    return 0;
  } else if (eFrameworkunifiedStatusAccessError == ret) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "CheckNv e_status:%d", ret);
    return -2;
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "CheckNv e_status:%d", ret);
    return -1;
  }
}

/**
 * media stat
 */
static int BkupMediaStat(const char *category_name, const char *item_name, enum NvHalMedia src, int size) {
  int ret = -1;
  char *path = reinterpret_cast<char *>(MAP_FAILED);

  // LCOV_EXCL_BR_START 5:mmap's error case.
  if (BkupMakeBkupPathToNvId(category_name, item_name, &path, FILENAME_MAX) < 0) {
  // LCOV_EXCL_BR_STOP 5:mmap's error case.
    // LCOV_EXCL_START 5:mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    goto exit;
    // LCOV_EXCL_STOP 5:mmap's error case.
  }
  ret = BkupCheckStatFromNv(src, path, size);  // LCOV_EXCL_BR_LINE 11:unexpected branch

exit:
  if (path != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5:mmap's error case.
    munmap(path, FILENAME_MAX);
  }
  return ret;
}

/**
 * stat check & read
 *
 * \retval 0 file exists
 * \retval -1 file not exists
 */
static int BkupCheckStatRead(enum NvHalMedia current_media, const char *path,
                             char *buf, int check_size, uint32_t *crc32,
                             pthread_rwlock_t *rwlock) {
  int ret;

  if ((ret = BkupCheckStatFromNv(current_media, path, check_size)) < 0) {
    return ret;
  }

  BkupRwlockRdlock(rwlock, path);

  if (BkupBufferReadFromNv(current_media, path, buf, check_size, crc32) < 0) {
    BkupRwlockUnlock(rwlock, path);
    return -1;
  }

  BkupRwlockUnlock(rwlock, path);
  return 0;
}

/**
 * valid check
 *
 * \retval 0 file invalid & do nothing
 * \retval -1 file invalid but create 0 fill file
 * \retval 1 file valid
 */
static int BkupCheckRead(const char *path, enum NvHalMedia current_media,
                         bkup_protocol_command_t command, char *rcv_buf,
                         char *read_buf, uint32_t *crc32,
                         bkup_query_result_t *query_result,
                         pthread_rwlock_t *rwlock) {
  int ret = -1;

  if (rcv_buf == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "rcv_buf is NULL");
    return 0;
  }
  if (query_result == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "query_result is NULL");
    return 0;
  }

  bkup_protocol_header_t *hdr = reinterpret_cast<bkup_protocol_header_t *>(rcv_buf);

  if (BkupCheckStatRead(current_media, path, read_buf, query_result->size, crc32, rwlock) < 0) {
    // file not exists
    if (current_media == NVHALMEDIA_NAND || query_result->nand == false) {
      goto exit;
    } else {
      if (command == BKUP_CMD_READ) {
        /*
         * Read
         */
        if (BkupCheckStatFromNv(NVHALMEDIA_NAND, path, query_result->size) == 0) {
          // do nothing
          ret = 0;
        } else {
          // take preference BackupDRAM or CacheDRAM
          ret = -1;
        }
      } else {
        /*
         * Write/Fill
         */
        pthread_rwlock_t *nand_rwlock = BkupGetRwlock(hdr->item_name, NVHALMEDIA_NAND, query_result);
        if (BkupCheckStatRead(NVHALMEDIA_NAND, path, read_buf, query_result->size, NULL, nand_rwlock) == 0) {
          // write back cache
          if (BkupWriteRename(query_result->category_name, hdr->item_name,
                              current_media, read_buf, query_result->size,
                              rwlock) >= 0) {
            ret = 1;
          } else {
            // copy error, do nothing
            ret = 0;
          }
        } else {
          // take preference BackupDRAM or CacheDRAM
          ret = -1;
        }
      }
      goto exit;
    }
  }
  ret = 1;
exit:
  return ret;
}

/**
 * read command
 */
static int BkupCmdRead(bkup_response_t *response, enum NvHalMedia current_media,
                       char *rcv_buf, bkup_query_result_t *query_result) {
  int ret = -1;
  if (response == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "response is NULL");
    return -1;
  }
  if (rcv_buf == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "rcv_buf is NULL");
    return -1;
  }
  if (query_result == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "query_result is NULL");
    return -1;
  }
  bkup_protocol_header_t *hdr = reinterpret_cast<bkup_protocol_header_t *>(rcv_buf);
  // LCOV_EXCL_BR_START 11:unexpected branch
  pthread_rwlock_t *rwlock = BkupGetRwlock(hdr->item_name, current_media, query_result);
  // LCOV_EXCL_BR_STOP 11:unexpected branch
  char *full_path = reinterpret_cast<char *>(MAP_FAILED);
  char *buf = reinterpret_cast<char *>(MAP_FAILED);
  size_t buf_size = 0;
  uint32_t crc32 = 0;
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;

  // LCOV_EXCL_BR_START 5:mmap's error case.
  if (BkupMakeBkupPathToNvId(query_result->category_name, hdr->item_name, &full_path, FILENAME_MAX) < 0) {
  // LCOV_EXCL_BR_STOP 5:mmap's error case.
    // LCOV_EXCL_START 5:mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    e_status = eFrameworkunifiedStatusFail;
    goto exit;
    // LCOV_EXCL_STOP 5:mmap's error case.
  }

  if ((buf = BkupAnonMmap(query_result->size)) == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5:mmap's error case.
    // LCOV_EXCL_START 5:mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mmap:%s", strerror(errno));
    e_status = eFrameworkunifiedStatusFail;
    goto exit;
    // LCOV_EXCL_STOP 5:mmap's error case.
  }
  buf_size = query_result->size;

  ret = BkupCheckRead(full_path, current_media, hdr->command, rcv_buf, buf,
                      &crc32, query_result, rwlock);  // LCOV_EXCL_BR_LINE 11:unexpected branch
  if (ret == -1) {
    // return all 0
    ret = hdr->size;
    goto exit;
  } else if (ret == 0) {
    // do nothing
    goto exit;
  }

  // success
  ret = hdr->size;

  // caching to memory
  if (current_media == NVHALMEDIA_NAND) {
    if (query_result->cache_dram) {
      if (BkupMediaStat(query_result->category_name, hdr->item_name,
                        NVHALMEDIA_CACHEDRAM, query_result->size) < 0) {
        // LCOV_EXCL_BR_START 11:unexpected branch
        pthread_rwlock_t *dst_rwlock =
            BkupGetRwlock(hdr->item_name, NVHALMEDIA_CACHEDRAM, query_result);
        // LCOV_EXCL_BR_STOP 11:unexpected branch
        BkupWriteRename(query_result->category_name, hdr->item_name,
                        NVHALMEDIA_CACHEDRAM, buf, query_result->size,
                        dst_rwlock);  // LCOV_EXCL_BR_LINE 11:unexpected branch
      }
    }
    if (query_result->backup_dram) {
      if (BkupMediaStat(query_result->category_name, hdr->item_name,
                        NVHALMEDIA_BACKUPDRAM, query_result->size) < 0) {
        // LCOV_EXCL_BR_START 11:unexpected branch
        pthread_rwlock_t *dst_rwlock =
            BkupGetRwlock(hdr->item_name, NVHALMEDIA_BACKUPDRAM, query_result);
        // LCOV_EXCL_BR_STOP 11:unexpected branch
        BkupWriteRename(query_result->category_name, hdr->item_name,
                        NVHALMEDIA_BACKUPDRAM, buf, query_result->size,
                        dst_rwlock);  // LCOV_EXCL_BR_LINE 11:unexpected branch
      }
    }
  }

exit:
  if (ret != 0 && buf != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case.
    // LCOV_EXCL_BR_START 11:unexpected branch
    BkupApilog(response, current_media, hdr, query_result, buf, crc32, e_status);
    // LCOV_EXCL_BR_STOP 11:unexpected branch
    // LCOV_EXCL_BR_START 4: NSFW error case
    if ((*response->set_response)(response->opt, buf + hdr->offset, hdr->size, e_status) < 0) {
    // LCOV_EXCL_BR_STOP 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ret = -1;  // LCOV_EXCL_LINE 4: NSFW error case
    }
  }

  if (full_path != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case.
    munmap(full_path, FILENAME_MAX);
  }
  if (buf != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case.
    munmap(buf, buf_size);
  }
  return ret;
}

/**
 * write command
 */
static int BkupCmdWrite(bkup_response_t *response,
                        enum NvHalMedia current_media, char *rcv_buf,
                        bkup_query_result_t *query_result) {
  int ret = -1;

  if (response == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "response is NULL");
    return -1;
  }
  if (rcv_buf == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "rcv_buf is NULL");
    return -1;
  }
  if (query_result == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "query_result is NULL");
    return -1;
  }
  bkup_protocol_header_t *hdr = reinterpret_cast<bkup_protocol_header_t *>(rcv_buf);
  // LCOV_EXCL_BR_START 11:unexpected branch
  pthread_rwlock_t *rwlock = BkupGetRwlock(hdr->item_name, current_media, query_result);
  // LCOV_EXCL_BR_STOP 11:unexpected branch
  char *full_path = reinterpret_cast<char *>(MAP_FAILED);
  char *buf = reinterpret_cast<char *>(MAP_FAILED);
  char *header = NULL;
  uint32_t crc32 = 0;
  int buf_size = 0;

  // LCOV_EXCL_BR_START 5: mmap's error case.
  if (BkupMakeBkupPathToNvId(query_result->category_name, hdr->item_name, &full_path, FILENAME_MAX) < 0) {
  // LCOV_EXCL_BR_STOP 5: mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    goto exit;  // LCOV_EXCL_LINE 5: mmap's error case.
  }

  if (hdr->offset != 0 || hdr->size != (uint32_t)query_result->size) {
    /*
     * partial write
     */
    if ((buf = BkupAnonMmap(query_result->size)) == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case.
      // LCOV_EXCL_START 5: mmap's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mmap:%s", strerror(errno));
      goto exit;
      // LCOV_EXCL_STOP 5: mmap's error case.
    }
    buf_size = query_result->size;

    // LCOV_EXCL_BR_START 11:unexpected branch
    ret = BkupCheckRead(full_path, current_media, hdr->command, rcv_buf, buf, NULL, query_result, rwlock);
    // LCOV_EXCL_BR_STOP 11:unexpected branch
    if (ret == 0) {
      // do nothing
      goto exit;
    }

    header = buf;
    memcpy(buf + hdr->offset, rcv_buf + sizeof(bkup_protocol_header_t), hdr->size);
    BkupRwlockWrlock(rwlock, full_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    if (BkupBufferWriteToNv(current_media, full_path, buf, query_result->size, &crc32) < 0) {
      BkupRwlockUnlock(rwlock, full_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch
      goto exit;
    }
    BkupRwlockUnlock(rwlock, full_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch
  } else {
    /*
     * whole write
     */
    header = rcv_buf + sizeof(bkup_protocol_header_t);
    BkupRwlockWrlock(rwlock, full_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    if (BkupBufferWriteToNv(current_media, full_path,
                rcv_buf + sizeof(bkup_protocol_header_t), hdr->size, &crc32) < 0) {
      BkupRwlockUnlock(rwlock, full_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch
      goto exit;
    }
    BkupRwlockUnlock(rwlock, full_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch
  }

  // success
  ret = hdr->size;

exit:
  EFrameworkunifiedStatus e_status = ret < 0 ? eFrameworkunifiedStatusFail : eFrameworkunifiedStatusOK;
  if (current_media == NVHALMEDIA_NAND) {
    if (query_result->cache_dram == false && query_result->backup_dram == false) {
      // LCOV_EXCL_BR_START 11:unexpected branch
      BkupApilog(response, current_media, hdr, query_result, header, crc32, e_status);
      // LCOV_EXCL_BR_STOP 11:unexpected branch
    }
    if ((*response->set_response)(response->opt, NULL, 0, e_status) < 0) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ret = -1;  // LCOV_EXCL_LINE 4: NSFW error case
    }
  } else {
    // LCOV_EXCL_BR_START 11:unexpected branch
    BkupApilog(response, current_media, hdr, query_result, header, crc32, e_status);
    // LCOV_EXCL_BR_STOP 11:unexpected branch
  }

  if (buf != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case.
    munmap(buf, buf_size);
  }
  if (full_path != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case.
    munmap(full_path, FILENAME_MAX);
  }
  return ret;
}

/**
 * fill command
 */
static int BkupCmdFill(bkup_response_t *response, enum NvHalMedia current_media,
                       char *rcv_buf, bkup_query_result_t *query_result) {
  int ret = -1;
  bkup_protocol_header_t *hdr = reinterpret_cast<bkup_protocol_header_t *>(rcv_buf);
  // LCOV_EXCL_BR_START 11:unexpected branch
  pthread_rwlock_t *rwlock = BkupGetRwlock(hdr->item_name, current_media, query_result);
  // LCOV_EXCL_BR_STOP 11:unexpected branch
  char *full_path = reinterpret_cast<char *>(MAP_FAILED);
  char *buf = reinterpret_cast<char *>(MAP_FAILED);
  uint32_t crc32 = 0;

  if (response == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "response is NULL");
    return -1;
  }
  if (rcv_buf == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "rcv_buf is NULL");
    return -1;
  }
  if (query_result == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "query_result is NULL");
    return -1;
  }
  // LCOV_EXCL_BR_START 5: mmap's error case.
  if (BkupMakeBkupPathToNvId(query_result->category_name, hdr->item_name, &full_path, FILENAME_MAX) < 0) {
  // LCOV_EXCL_BR_STOP 5: mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    goto exit;  // LCOV_EXCL_LINE 5: mmap's error case.
  }

  // LCOV_EXCL_BR_START 5: mmap's error case.
  if ((buf = BkupAnonMmap(query_result->size)) == MAP_FAILED) {
  // LCOV_EXCL_BR_STOP 5: mmap's error case.
    // LCOV_EXCL_START 5: mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mmap:%s", strerror(errno));
    goto exit;
    // LCOV_EXCL_STOP 5: mmap's error case.
  }

  if (hdr->offset != 0 || hdr->size != (uint32_t)query_result->size) {
    /*
     * partial fill
     */
    // LCOV_EXCL_BR_START 11:unexpected branch
    ret = BkupCheckRead(full_path, current_media, hdr->command, rcv_buf, buf, NULL, query_result, rwlock);
    // LCOV_EXCL_BR_STOP 11:unexpected branch
    if (ret == 0) {
      // do nothing
      goto exit;
    }
  }

  memset(buf + hdr->offset, hdr->fill_patern, hdr->size);
  BkupRwlockWrlock(rwlock, full_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch
  if (BkupBufferWriteToNv(current_media, full_path, buf, query_result->size, &crc32) < 0) {
    BkupRwlockUnlock(rwlock, full_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    goto exit;
  }
  BkupRwlockUnlock(rwlock, full_path);  // LCOV_EXCL_BR_LINE 11:unexpected branch

  // success
  ret = hdr->size;

exit:
  EFrameworkunifiedStatus e_status = ret < 0 ? eFrameworkunifiedStatusFail : eFrameworkunifiedStatusOK;
  if (current_media == NVHALMEDIA_NAND) {
    if (query_result->cache_dram == false && query_result->backup_dram == false) {
      // LCOV_EXCL_BR_START 11:unexpected branch
      BkupApilog(response, current_media, hdr, query_result, buf, crc32, e_status);
      // LCOV_EXCL_BR_STOP 11:unexpected branch
    }
    if ((*response->set_response)(response->opt, NULL, 0, e_status) < 0) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ret = -1;  // LCOV_EXCL_LINE 4: NSFW error case
    }
  } else {
    // LCOV_EXCL_BR_START 11:unexpected branch
    BkupApilog(response, current_media, hdr, query_result, buf, crc32, e_status);
    // LCOV_EXCL_BR_STOP 11:unexpected branch
  }

  if (buf != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case.
    munmap(buf, query_result->size);
  }
  if (full_path != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case.
    munmap(full_path, FILENAME_MAX);
  }
  return ret;
}

/**
 * copy command
 */
static int BkupCmdCopy(bkup_response_t *response, enum NvHalMedia current_media,
                       char *rcv_buf, bkup_query_result_t *query_result) {
  if (rcv_buf == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "rcv_buf is NULL");
    return -1;
  }
  if (query_result == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "query_result is NULL");
    return -1;
  }
  bkup_protocol_header_t *hdr =
      reinterpret_cast<bkup_protocol_header_t *>(rcv_buf);
  pthread_rwlock_t *src_rwlock =
      BkupGetRwlock(hdr->item_name, hdr->src_media, query_result);
  pthread_rwlock_t *dst_rwlock =
      BkupGetRwlock(hdr->item_name, current_media, query_result);

  int ret = BkupMediaCopyFromNv(query_result->category_name, hdr->item_name, hdr->src_media, current_media, src_rwlock,
                        dst_rwlock);

  if (response == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "response is NULL");
    return -1;
  }

  if (current_media == NVHALMEDIA_NAND) {
    // LCOV_EXCL_BR_START 4: NSFW error case
    if ((*response->set_response)(response->opt, NULL, 0, ret < 0 ? eFrameworkunifiedStatusFail : eFrameworkunifiedStatusOK) < 0) {
    // LCOV_EXCL_BR_STOP 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      ret = -1;  // LCOV_EXCL_LINE 4: NSFW error case
    }
  }
  return ret;
}

/**
 * check command
 */
static int BkupCmdCheck(bkup_response_t *response,
                        enum NvHalMedia current_media, char *rcv_buf,
                        bkup_query_result_t *query_result) {
  int ret = 0;
  bkup_protocol_header_t *hdr = reinterpret_cast<bkup_protocol_header_t *>(rcv_buf);
  char *full_path = reinterpret_cast<char *>(MAP_FAILED);

  if (rcv_buf == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "rcv_buf is NULL");
    return -1;
  }
  if (query_result == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "query_result is NULL");
    return -1;
  }
  // LCOV_EXCL_BR_START 5: mmap's error case.
  if (BkupMakeBkupPathToNvId(query_result->category_name, hdr->item_name, &full_path, FILENAME_MAX) < 0) {
  // LCOV_EXCL_BR_STOP 5: mmap's error case.
    // LCOV_EXCL_START 5: mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    ret = -3;
    goto exit;
    // LCOV_EXCL_STOP 5: mmap's error case.
  }
  ret = BkupCheckStatFromNv(current_media, full_path, query_result->size);  // LCOV_EXCL_BR_LINE 11:unexpected branch

exit:
  if (full_path != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case.
    munmap(full_path, FILENAME_MAX);
  }
  return ret;
}

/**
 * delete command
 */
static int BkupCmdDeleteData(bkup_response_t* const response,
                             const enum NvHalMedia current_media,
                             char* const rcv_buf,
                             bkup_query_result_t* const query_result) {
  int ret = -1;
  char *full_path = NULL;
  bkup_protocol_header_t* hdr = reinterpret_cast<bkup_protocol_header_t*>(NULL);
  pthread_rwlock_t* rwlock = reinterpret_cast<pthread_rwlock_t*>(NULL);
  if ((response == NULL) || (rcv_buf == NULL) || (query_result == NULL)) {  // LCOV_EXCL_BR_LINE 6:Excluded as no NULL is currently passed
    // LCOV_EXCL_START 6:Excluded as no NULL is currently passed
    AGL_ASSERT_NOT_TESTED();
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Resp or Buf or Query is NULL");
    goto exit;
    // LCOV_EXCL_STOP 6:Excluded as no NULL is currently passed
  }
  hdr = reinterpret_cast<bkup_protocol_header_t *>(rcv_buf);
  rwlock = BkupGetRwlock(hdr->item_name, current_media, query_result);
  if (BkupMakeBkupPathToNvId(query_result->category_name, hdr->item_name, &full_path, FILENAME_MAX) < 0) {  // LCOV_EXCL_BR_LINE 6:Excluded due to checking by upper-level function
    // LCOV_EXCL_START 6:Excluded due to checking by upper-level function
    AGL_ASSERT_NOT_TESTED();
    goto exit;
    // LCOV_EXCL_STOP 6:Excluded due to checking by upper-level function
  }
  BkupRwlockWrlock(rwlock, full_path);
  ret = BkupDeleteDataFromNv(current_media, full_path);
  if (ret != 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "BkupDeleteDataFromNv Error");
  }
  BkupRwlockUnlock(rwlock, full_path);

exit:
  if ((response != NULL) && (query_result != NULL)  && (hdr != NULL)) {  // LCOV_EXCL_BR_LINE 6:Excluded as no NULL is currently passed
    EFrameworkunifiedStatus e_status = (ret < 0) ? eFrameworkunifiedStatusFail : eFrameworkunifiedStatusOK;
    if (current_media == NVHALMEDIA_NAND) {
      if ((query_result->cache_dram == false) && (query_result->backup_dram == false)) {
        BkupApilog(response, current_media, hdr, query_result, NULL, 0, e_status);
      }
      if ((*response->set_response)(response->opt, NULL, 0, e_status) < 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Response Error");
        ret = -1;
      }
    } else {
      BkupApilog(response, current_media, hdr, query_result, NULL, 0, e_status);
    }
  }

  if (full_path != NULL) {  // LCOV_EXCL_BR_LINE 6:Excluded because checked by BkupMakeBkupPathToNvId()
    munmap(full_path, FILENAME_MAX);
  }
  return ret;
}

/**
 * common file handler
 */
static int BkupHandlerFile(bkup_response_t *response,
                           enum NvHalMedia current_media, char *rcv_buf,
                           bkup_query_result_t *query_result) {
  int ret = 0;
  bkup_protocol_header_t *hdr = reinterpret_cast<bkup_protocol_header_t *>(rcv_buf);

  if (rcv_buf == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "rcv_buf is NULL");
    return -1;
  }

  switch (hdr->command) {  // LCOV_EXCL_BR_LINE 6:double check
    case BKUP_CMD_READ:
      ret = BkupCmdRead(response, current_media, rcv_buf, query_result);
      break;

    case BKUP_CMD_WRITE:
      ret = BkupCmdWrite(response, current_media, rcv_buf, query_result);
      break;

    case BKUP_CMD_FILL:
      ret = BkupCmdFill(response, current_media, rcv_buf, query_result);
      break;

    case BKUP_CMD_COPY_INNER:
      ret = BkupCmdCopy(response, current_media, rcv_buf, query_result);
      break;

    case BKUP_CMD_DELETE:
      ret = BkupCmdDeleteData(response, current_media, rcv_buf, query_result);
      break;

    default:
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      break;  // LCOV_EXCL_LINE 6:double check
  }
  return ret;
}

/**
 * Main's response
 */
static int BkupSetResponseNsfw(void *opt, void *buf, int size, EFrameworkunifiedStatus ret_value) {
  EFrameworkunifiedStatus e_status;

  if (buf != NULL) {  // LCOV_EXCL_BR_LINE 6:double check
    // LCOV_EXCL_BR_START 4: NSFW error case
    if ((e_status = FrameworkunifiedSetSyncResponseData((HANDLE)opt, buf, size)) != eFrameworkunifiedStatusOK) {
    // LCOV_EXCL_BR_STOP 4: NSFW error case
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedSetSyncResponseData:%d", e_status);
      return -1;
      // LCOV_EXCL_STOP 4: NSFW error case
    }
  }
  return 0;
}

/**
 * write apilog
 */
static void BkupApilog(bkup_response_t *response, enum NvHalMedia current_media,
                       bkup_protocol_header_t *hdr,
                       bkup_query_result_t *query_result, char *buf,
                       uint32_t crc32, EFrameworkunifiedStatus ret_value) {
  const char *src;
  const char *cmd;
  char header[12] = "-- -- -- --";
  const char c[] = "0123456789abcdef";

  if (response == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "response is NULL");
    return;
  }
  if (hdr == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "hdr is NULL");
    return;
  }
  if (query_result == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "query_result is NULL");
    return;
  }
  if (response->set_response == BkupSetResponseNsfw) {
    src = FrameworkunifiedGetMsgSrc((HANDLE)response->opt);  // LCOV_EXCL_BR_LINE 11:unexpected branch
  } else {
    // LCOV_EXCL_BR_START 11:unexpected branch
    src = (reinterpret_cast<bkup_itc_message_t *>(response->opt))->nand.client_name;
    // LCOV_EXCL_BR_STOP 11:unexpected branch
  }

  switch (hdr->command) {  // LCOV_EXCL_BR_LINE 6:double check
    case BKUP_CMD_READ:
      cmd = "RD";
      break;
    case BKUP_CMD_WRITE:
      cmd = "WR";
      break;
    case BKUP_CMD_FILL:
      cmd = "FILL";
      break;
    case BKUP_CMD_READ_NUM:
      // LCOV_EXCL_START 6:double check
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      cmd = "RDN";
      break;
      // LCOV_EXCL_STOP 6:double check
    case BKUP_CMD_SIZE_NUM:
      // LCOV_EXCL_START 6:double check
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      cmd = "SZN";
      break;
      // LCOV_EXCL_STOP 6:double check
    case BKUP_CMD_DELETE:
      cmd = "DEL";
      break;
    default:
      // LCOV_EXCL_START 6:double check
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      cmd = "UNKNOWN";
      break;
      // LCOV_EXCL_STOP 6:double check
  }

  if (buf) {  // LCOV_EXCL_BR_LINE 6:double check
    int i;
    for (i = 0; i < 4; i++) {
      if (query_result->size > i) {
        header[i * 3] = c[*(buf + i) >> 4];
        header[i * 3 + 1] = c[*(buf + i) & 0xf];
      }
    }
  }

  // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
  FRAMEWORKUNIFIEDLOG(ZONE_ALSA, "", "%s/%s/%s/o:%d/s:%d/h:%s/c:%x/r:%d", src, cmd, hdr->item_name, hdr->offset, hdr->size,
         header, crc32, ret_value);
  // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
}

/**
 * Tweak command
 */
static int BkupTweakCommand(enum NvHalMedia current_media, char **rcv_buf,
                            int *rcv_size, bkup_query_result_t *query_result) {
  int ret = -1;
  bkup_protocol_header_t update_hdr;
  bkup_protocol_header_t *hdr = reinterpret_cast<bkup_protocol_header_t *>(*rcv_buf);

  if (current_media == NVHALMEDIA_NAND) {  // LCOV_EXCL_BR_LINE 6:current_media never will be NVHALMEDIA_NAND
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    ret = 0;  // LCOV_EXCL_LINE 6:current_media never will be NVHALMEDIA_NAND
    goto exit;  // LCOV_EXCL_LINE 6:current_media never will be NVHALMEDIA_NAND
  }
  if (rcv_size == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "rcv_size is NULL");
    return -1;
  }
  if (query_result == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "query_resul is NULL");
    return -1;
  }

  if ((hdr->command == BKUP_CMD_WRITE || hdr->command == BKUP_CMD_FILL) &&
      (hdr->offset != 0 || hdr->size != (uint32_t)query_result->size)) {
    // partial request
    if ((current_media == NVHALMEDIA_CACHEDRAM &&
         (query_result->nand || query_result->backup_dram)) ||
        (current_media == NVHALMEDIA_BACKUPDRAM && query_result->nand)) {
      memset(&update_hdr, 0, sizeof(update_hdr));
      BkupStrlcpy(update_hdr.item_name, hdr->item_name, sizeof(update_hdr.item_name));
      update_hdr.command = BKUP_CMD_COPY_INNER;
      update_hdr.src_media = current_media;
      munmap(*rcv_buf, *rcv_size);
      *rcv_size = 0;
      // LCOV_EXCL_BR_START 5: mmap's error case.
      if ((*rcv_buf = BkupAnonMmap(sizeof(bkup_protocol_header_t))) == MAP_FAILED) {
      // LCOV_EXCL_BR_STOP 5: mmap's error case.
        // LCOV_EXCL_START 5: mmap's error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mmap:%s", strerror(errno));
        goto exit;
        // LCOV_EXCL_STOP 5: mmap's error case.
      }
      *rcv_size = sizeof(bkup_protocol_header_t);
      memcpy(*rcv_buf, &update_hdr, sizeof(update_hdr));
    }
  }
  ret = 0;

exit:
  return ret;
}

/**
 * Main callback handler
 */
EFrameworkunifiedStatus BkupHandler(HANDLE h_app) {
  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  uint32_t rcv_size = 0;
  char *rcv_buf = NULL;
  bkup_protocol_header_t *hdr;
  bkup_query_result_t query_result;
  bool hold_rcv_buf = false;
  int ret = 0;
  bkup_response_t response_nsfw = {BkupSetResponseNsfw, h_app};

  if ((rcv_size = FrameworkunifiedGetMsgLength(h_app)) < sizeof(bkup_protocol_header_t)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedGetMsgLength: size fail:%d", rcv_size);
    e_status = eFrameworkunifiedStatusFail;
    goto exit;
    // LCOV_EXCL_STOP 4: NSFW error case.
  }

  rcv_buf = reinterpret_cast<char *>(mmap(NULL, rcv_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0));
  if (rcv_buf == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case.
    // LCOV_EXCL_START 5: mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    rcv_size = 0;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mmap:%s", strerror(errno));
    e_status = eFrameworkunifiedStatusInvldParam;
    goto exit;
    // LCOV_EXCL_STOP 5: mmap's error case.
  }

  // LCOV_EXCL_BR_START 4: NSFW error case.
  if ((e_status = FrameworkunifiedGetMsgDataOfSize(h_app, rcv_buf, rcv_size, eSMRRelease)) != eFrameworkunifiedStatusOK) {
  // LCOV_EXCL_BR_STOP 4: NSFW error case.
    // LCOV_EXCL_START 4: NSFW error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedGetMsgDataOfSize:%d", e_status);
    goto exit;
    // LCOV_EXCL_STOP 4: NSFW error case.
  }

  hdr = reinterpret_cast<bkup_protocol_header_t *>(rcv_buf);
  if (hdr->command < 0 || hdr->command >= BKUP_CMD_MAX) {  // LCOV_EXCL_BR_LINE 6:double check
    // LCOV_EXCL_START 6:double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid command:%d", hdr->command);
    e_status = eFrameworkunifiedStatusInvldParam;
    goto exit;
    // LCOV_EXCL_STOP 6:double check
  }

  if (hdr->command == BKUP_CMD_READ_NUM || hdr->command == BKUP_CMD_SIZE_NUM) {
    // LCOV_EXCL_BR_START 11:unexpected branch
    if (BkupParamGetNumid(hdr->num_id, &query_result, hdr->item_name, sizeof(hdr->item_name)) < 0) {
    // LCOV_EXCL_BR_STOP 11:unexpected branch
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid num_id:%d", hdr->num_id);
      // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
      e_status = eFrameworkunifiedStatusInvldParam;
      goto exit;
    }

    if (hdr->command == BKUP_CMD_READ_NUM) {
      hdr->command = BKUP_CMD_READ;
    }
  } else {
    if (BkupParamGet(hdr->item_name, &query_result) < 0) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid item_name:%s", hdr->item_name != 0 ? hdr->item_name : NULL);
      // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
      e_status = eFrameworkunifiedStatusInvldParam;
      goto exit;
    }
  }

  if (hdr->command == BKUP_CMD_READ || hdr->command == BKUP_CMD_WRITE || hdr->command == BKUP_CMD_FILL) {
    if (hdr->size == 0) {
      // do nothing
      goto exit;
    }
    if (hdr->size + hdr->offset > (uint32_t)query_result.size) {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid size+offset:%d+%d", hdr->size, hdr->offset);
      // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
      e_status = eFrameworkunifiedStatusInvldParam;
      goto exit;
    }
  }

  // LCOV_EXCL_BR_START 6: g_bkup_internal_state will not be BKUP_STATE_INIT
  if (g_bkup_internal_state == BKUP_STATE_INIT) {
  // LCOV_EXCL_BR_STOP 6: g_bkup_internal_state will not be BKUP_STATE_INIT
    // LCOV_EXCL_START 6: same as above
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    e_status = eFrameworkunifiedStatusErrOther;
    goto exit;
    // LCOV_EXCL_STOP 6: same as above
  }
  // LCOV_EXCL_BR_START 6: g_bkup_internal_state will not be BKUP_STATE_TERM
  if (g_bkup_internal_state == BKUP_STATE_TERM) {
  // LCOV_EXCL_BR_STOP 6: g_bkup_internal_state will not be BKUP_STATE_TERM
    // LCOV_EXCL_START 6: same as above
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    e_status = eFrameworkunifiedStatusExit;
    goto exit;
    // LCOV_EXCL_STOP 6: same as above
  }

  if (hdr->command == BKUP_CMD_SIZE || hdr->command == BKUP_CMD_SIZE_NUM) {
    // LCOV_EXCL_BR_START 11:unexpected branch
    BkupSetResponseNsfw(h_app, &query_result.size, sizeof(query_result.size), eFrameworkunifiedStatusOK);
    // LCOV_EXCL_BR_STOP 11:unexpected branch
  } else if (hdr->command == BKUP_CMD_CHECK) {
    if (query_result.nand) {
      // LCOV_EXCL_BR_START 11:unexpected branch
      ret = BkupCmdCheck(&response_nsfw, NVHALMEDIA_NAND, rcv_buf, &query_result);
      // LCOV_EXCL_BR_STOP 11:unexpected branch
    } else if (query_result.backup_dram) {
      // LCOV_EXCL_BR_START 11:unexpected branch
      ret = BkupCmdCheck(&response_nsfw, NVHALMEDIA_BACKUPDRAM, rcv_buf, &query_result);
      // LCOV_EXCL_BR_STOP 11:unexpected branch
    } else if (query_result.cache_dram) {
      // LCOV_EXCL_BR_START 11:unexpected branch
      ret = BkupCmdCheck(&response_nsfw, NVHALMEDIA_CACHEDRAM, rcv_buf, &query_result);
      // LCOV_EXCL_BR_STOP 11:unexpected branch
    } else {
      ret = -3;
    }

    if (ret == -1) {
      // not exists
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Not exists:%s", hdr->item_name != 0 ? hdr->item_name : NULL);
      // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
      e_status = eFrameworkunifiedStatusFileLoadError;
    } else if (ret == -2) {
      // size error
      e_status = eFrameworkunifiedStatusAccessError;
    } else if (ret == -3) {
      e_status = eFrameworkunifiedStatusFail;
    }
  } else {
    /*
     * Cache DRAM
     */
    if (query_result.cache_dram) {
      // LCOV_EXCL_BR_START 11:unexpected branch
      ret = BkupHandlerFile(&response_nsfw, NVHALMEDIA_CACHEDRAM, rcv_buf, &query_result);
      // LCOV_EXCL_BR_STOP 11:unexpected branch
      if (ret > 0) {
        if (hdr->command == BKUP_CMD_READ) {
          goto exit;
        }
        // LCOV_EXCL_BR_START 5: mmap's error case.
        if (BkupTweakCommand(NVHALMEDIA_CACHEDRAM, &rcv_buf,
                             reinterpret_cast<int *>(&rcv_size),
                             &query_result) < 0) {
        // LCOV_EXCL_BR_STOP 5: mmap's error case.
          // LCOV_EXCL_START 5: mmap's error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          e_status = eFrameworkunifiedStatusFail;
          goto exit;
          // LCOV_EXCL_STOP 5: mmap's error case.
        }
        hdr = reinterpret_cast<bkup_protocol_header_t *>(rcv_buf);
      }
    }

    /*
     * Backup DRAM
     */
    if (query_result.backup_dram) {
      // LCOV_EXCL_BR_START 11:unexpected branch
      ret = BkupHandlerFile(&response_nsfw, NVHALMEDIA_BACKUPDRAM, rcv_buf, &query_result);
      // LCOV_EXCL_BR_STOP 11:unexpected branch
      if (ret > 0) {
        if (hdr->command == BKUP_CMD_READ) {
          goto exit;
        }
        // LCOV_EXCL_BR_START 5: mmap's error case.
        if (BkupTweakCommand(NVHALMEDIA_BACKUPDRAM, &rcv_buf,
                             reinterpret_cast<int *>(&rcv_size),
                             &query_result) < 0) {
        // LCOV_EXCL_BR_STOP 5: mmap's error case.
          // LCOV_EXCL_START 5: mmap's error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          e_status = eFrameworkunifiedStatusFail;
          goto exit;
          // LCOV_EXCL_STOP 5: mmap's error case.
        }
        hdr = reinterpret_cast<bkup_protocol_header_t *>(rcv_buf);
      }
    }

    /*
     * NAND
     */
    if (query_result.nand) {
      bkup_itc_message_t snd_msg;

      if ((hdr->command == BKUP_CMD_READ) || (hdr->command == BKUP_CMD_DELETE) || (query_result.sync) || (ret == 0)) {
        /*
         * operation offload
         */
        const char *client_name;
        char system_info[MAX_SYS_INFO_SIZE];

        if (g_bkup_nand_mqd == -1) {
          // LCOV_EXCL_BR_START 5: mq_open's error case.
          if ((g_bkup_nand_mqd = BkupItcMqopen("/" BKUP_THREAD_NAND, O_WRONLY | O_CREAT | O_NONBLOCK | O_CLOEXEC)) < 0) {
          // LCOV_EXCL_BR_STOP 5: mq_open's error case.
            // LCOV_EXCL_START 5: mq_open's error case.
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mq_open:%s", strerror(errno));
            exit(EXIT_FAILURE);
            // LCOV_EXCL_STOP 5: mq_open's error case.
          }
        }

        if ((client_name = FrameworkunifiedGetMsgSrc(h_app)) == NULL) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
          // LCOV_EXCL_START 4: NSFW error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "invalid client_name");
          e_status = eFrameworkunifiedStatusFail;
          goto exit;
          // LCOV_EXCL_STOP 4: NSFW error case.
        }
        FrameworkunifiedGetSystemInfo(h_app, system_info);  // LCOV_EXCL_BR_LINE 11:unexpected branch.

        memset(&snd_msg, 0, sizeof(snd_msg));
        BkupStrlcpy(snd_msg.nand.client_name, client_name, sizeof(snd_msg.nand.client_name));
        snd_msg.nand.seq_id = static_cast<uint32_t>(strtoul(system_info, NULL, 16));
        snd_msg.nand.rcv_buf = rcv_buf;
        snd_msg.nand.rcv_size = rcv_size;

        // LCOV_EXCL_BR_START 5: mq_send's error case.
        if (mq_send(g_bkup_nand_mqd, reinterpret_cast<char *>(&snd_msg), sizeof(snd_msg), 0) < 0) {
        // LCOV_EXCL_BR_STOP 5: mq_send's error case.
          // LCOV_EXCL_START 5: mq_send's error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          int save_errno = errno;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mq_send:%s", strerror(errno));
          e_status = eFrameworkunifiedStatusFail;
          if (save_errno == EAGAIN) {
            kill(g_bkup_nand_tid, SIGABRT);
          }
          goto exit;
          // LCOV_EXCL_STOP 5: mq_send's error case.
        }
        FrameworkunifiedSetDeferredSyncResponse(h_app);  // LCOV_EXCL_BR_LINE 11:unexpected branch.
        hold_rcv_buf = true;
      } else {
        /*
         * delay write
         */
        if (g_bkup_delay_mqd == -1) {
          // LCOV_EXCL_BR_START 5: mq_open's error case.
          if ((g_bkup_delay_mqd = BkupItcMqopen("/" BKUP_THREAD_DELAY, O_WRONLY | O_CREAT | O_NONBLOCK | O_CLOEXEC)) < 0) {
          // LCOV_EXCL_BR_STOP 5: mq_open's error case.
            // LCOV_EXCL_START 5: mq_open's error case.
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mq_open:%s", strerror(errno));
            exit(EXIT_FAILURE);
            // LCOV_EXCL_STOP 5: mq_open's error case.
          }
        }

        memset(&snd_msg, 0, sizeof(snd_msg));

//        snd_msg.delay.terminate = false;
        snd_msg.delay.cmd = BKUP_DELAY_CMD_REGIST;

        BkupStrlcpy(snd_msg.delay.item_name, hdr->item_name, sizeof(snd_msg.delay.item_name));

        // LCOV_EXCL_BR_START 5: mq_send's error case.
        if (mq_send(g_bkup_delay_mqd, reinterpret_cast<char *>(&snd_msg), sizeof(snd_msg), 0) < 0) {
        // LCOV_EXCL_BR_STOP 5: mq_send's error case.
          // LCOV_EXCL_START 5: mq_send's error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          int save_errno = errno;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mq_send:%s", strerror(errno));
          e_status = eFrameworkunifiedStatusFail;
          if (save_errno == EAGAIN) {
            kill(g_bkup_delay_tid, SIGABRT);
          }
          goto exit;
          // LCOV_EXCL_STOP 5: mq_send's error case.
        }
      }
    }
  }

exit:
  if (hold_rcv_buf == false && rcv_buf != MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case
    munmap(rcv_buf, rcv_size);
  }
  return e_status;
}


/**
 * Power callback handler
 */
EFrameworkunifiedStatus bkup_power_handler(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  uint32_t rcv_size = 0;
  T_SS_SM_UserModeOnOffNotification_StructType power_status;

  if ((rcv_size = FrameworkunifiedGetMsgLength(hApp)) != sizeof(power_status)) {  // LCOV_EXCL_BR_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    // LCOV_EXCL_START 4: NSFW error case
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedGetMsgLength: size fail:%d", rcv_size);
    eStatus = eFrameworkunifiedStatusFail;
    goto exit;
    // LCOV_EXCL_STOP 4: NSFW error case
  }

  // LCOV_EXCL_BR_START 4: NSFW error case
  if ((eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, &power_status, rcv_size, eSMRRelease)) != eFrameworkunifiedStatusOK) {
  // LCOV_EXCL_BR_STOP 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    // LCOV_EXCL_START 4: NSFW error case
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "FrameworkunifiedGetMsgDataOfSize:%d", eStatus);
    goto exit;
    // LCOV_EXCL_STOP 4: NSFW error case
  }

  if (g_bkup_internal_state != BKUP_STATE_NORMAL) {  // LCOV_EXCL_BR_LINE 6:double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    // LCOV_EXCL_START 6:double check
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __func__, "Invalid state:%d", g_bkup_internal_state);
    goto exit;
    // LCOV_EXCL_STOP 6:double check
  }

  if (power_status.isUserModeOn == FALSE) {
    bkup_itc_message_t snd_msg;

    if (g_bkup_delay_mqd == -1) {
      // LCOV_EXCL_BR_START 5: mq_open's error case.
      if ((g_bkup_delay_mqd = BkupItcMqopen("/" BKUP_THREAD_DELAY, O_WRONLY|O_CREAT|O_NONBLOCK|O_CLOEXEC)) < 0) {
      // LCOV_EXCL_BR_STOP 5: mq_open's error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        // LCOV_EXCL_START 5: mq_open's error case.
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mq_open:%s", strerror(errno));
        eStatus = eFrameworkunifiedStatusFail;
        goto exit;
        // LCOV_EXCL_STOP 5: mq_open's error case.
      }
    }

    memset(&snd_msg, 0, sizeof(snd_msg));
    snd_msg.delay.cmd = BKUP_DELAY_CMD_PRE_ACCOFF;

    // LCOV_EXCL_BR_START 5: mq_send's error case.
    if (mq_send(g_bkup_delay_mqd, (char *)&snd_msg, sizeof(snd_msg), 0) < 0) {
    // LCOV_EXCL_BR_START 5: mq_send's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      // LCOV_EXCL_START 5: mq_send's error case.
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mq_send:%s", strerror(errno));
      eStatus = eFrameworkunifiedStatusFail;
      goto exit;
      // LCOV_EXCL_STOP 5: mq_send's error case.
    }
  }

exit:
  return eStatus;
}


/**
 * thread's response
 */
static int BkupSetResponseThread(void *opt, void *buf, int size, EFrameworkunifiedStatus ret_value) {
  HANDLE response_mq;
  char invoker_name[MAX_QUEUE_NAME_SIZE];
  if (opt == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "opt is NULL");
    return -1;
  }
  bkup_itc_message_t *msg = reinterpret_cast<bkup_itc_message_t *>(opt);
  EFrameworkunifiedStatus e_status;
  // LCOV_EXCL_BR_START 11:unexpected branch.
  McCreateInvokerName(msg->nand.client_name, 0, invoker_name, sizeof(invoker_name));
  // LCOV_EXCL_BR_STOP 11:unexpected branch.

  if (g_bkup_response_mq == NULL) {
    g_bkup_response_mq = new g_bkup_response_mq_t;  // LCOV_EXCL_BR_LINE 11:except,C++ operator
  }
  g_bkup_response_mq_t::iterator mq_it = g_bkup_response_mq->find(invoker_name);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
  if (mq_it != g_bkup_response_mq->end()) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    response_mq = mq_it->second;  // LCOV_EXCL_BR_LINE 11:except,C++ STL
  } else {
    if ((response_mq = McOpenSyncSender(invoker_name)) == NULL) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
      // LCOV_EXCL_START 4: NSFW error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mcOpenSyncSender");
      return -1;
      // LCOV_EXCL_STOP 4: NSFW error case.
    }
    g_bkup_response_mq->insert(std::make_pair(invoker_name, response_mq));  // LCOV_EXCL_BR_LINE 11:except,C++ STL
  }

  // LCOV_EXCL_BR_START 11:unexpected branch.
  e_status = McSendSyncResponse(response_mq, MN_NS_BACKUPMGR, BACKUP_CID, msg->nand.seq_id, ret_value, size, buf);
  // LCOV_EXCL_BR_STOP 11:unexpected branch.
  return (e_status == eFrameworkunifiedStatusOK) ? 0 : -1;  // LCOV_EXCL_BR_LINE 11:unexpected branch
}

/**
 * NAND Thread
 */
void *BkupNandThread(void *arg) {
  mqd_t mqd;
  struct pollfd fds[1];
  bkup_itc_message_t rcv_msg;
  bkup_protocol_header_t *hdr;
  bkup_query_result_t query_result;
  bkup_response_t response_thread = {BkupSetResponseThread, NULL};

  signal_handler p_signal = SignalHandlerFuncForMsgHandleThread;
  signal(SIGUSR1, p_signal);
  g_nand_thread_id = pthread_self();

  if (prctl(PR_SET_NAME, BKUP_THREAD_NAND) < 0) {  // LCOV_EXCL_BR_LINE 5: prctl's error case.
    // LCOV_EXCL_START 5: prctl's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "prctl(PR_SET_NAME):%s", strerror(errno));
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 5: prctl's error case.
  }

  // LCOV_EXCL_BR_START 5: mq_open's error case.
  if ((mqd = BkupItcMqopen("/" BKUP_THREAD_NAND, O_RDONLY | O_CREAT | O_CLOEXEC)) < -1) {
  // LCOV_EXCL_BR_STOP 5: mq_open's error case.
    // LCOV_EXCL_START 5: mq_open's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mq_open:%s", strerror(errno));
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 5: mq_open's error case.
  }
  g_bkup_nand_tid = static_cast<int>(syscall(__NR_gettid));

  fds[0].fd = mqd;
  fds[0].events = POLLIN;

  while (1) {
    int ret = poll(fds, sizeof(fds) / sizeof(struct pollfd), -1);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    if (ret < 0) {  // LCOV_EXCL_BR_LINE 5: poll's error case.
      // LCOV_EXCL_START 5: poll's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      if (errno == EINTR) {
        continue;
      }
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "poll errno:%s", strerror(errno));
      // LCOV_EXCL_STOP 5: poll's error case.
    } else {
      if ((fds[0].revents & POLLIN) != 0) {
        // LCOV_EXCL_BR_START 5: mq_receive's error case.
        if (mq_receive(mqd, reinterpret_cast<char *>(&rcv_msg), sizeof(rcv_msg), NULL) < 0) {
        // LCOV_EXCL_BR_STOP 5: mq_receive's error case.
          // LCOV_EXCL_START 5: mq_receive's error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          if (errno != EINTR) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mq_receive:%s", strerror(errno));
          }
          continue;
          // LCOV_EXCL_STOP 5: mq_receive's error case.
        }
        hdr = reinterpret_cast<bkup_protocol_header_t *>(rcv_msg.nand.rcv_buf);
        BkupParamGet(hdr->item_name, &query_result);  // LCOV_EXCL_BR_LINE 11:unexpected branch
        response_thread.opt = &rcv_msg;
        // LCOV_EXCL_BR_START 11:unexpected branch
        ret = BkupHandlerFile(&response_thread, NVHALMEDIA_NAND, rcv_msg.nand.rcv_buf, &query_result);
        // LCOV_EXCL_BR_STOP 11:unexpected branch

        munmap(rcv_msg.nand.rcv_buf, rcv_msg.nand.rcv_size);
      }
    }
  }

  return NULL;
}

/*
 * delay backup thread
 */
typedef struct {
  int fd;
  std::string name;
} bkup_delay_t;  // LCOV_EXCL_BR_LINE 11:except,C++ STL

static int BkupDelayTimerAdd(int efd, const char *item_name, time_t backup_cycle) {
  int ret = -1;
  struct epoll_event ev;
  bkup_delay_t *timer_ev = new bkup_delay_t;  // LCOV_EXCL_BR_LINE 11:except,C++ operator
  struct itimerspec itime;

  if ((timer_ev->fd = timerfd_create(CLOCK_MONOTONIC, 0)) < 0) {  // LCOV_EXCL_BR_LINE 5: timerfd_create's error case.
    // LCOV_EXCL_START 5: timerfd_create's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "timerfd_create:%s", strerror(errno));
    goto exit;
    // LCOV_EXCL_STOP 5: timerfd_create's error case.
  }
  timer_ev->name = item_name;  // LCOV_EXCL_BR_LINE 11:except,C++ STL

  ev.events = EPOLLIN;
  ev.data.ptr = timer_ev;
  if (epoll_ctl(efd, EPOLL_CTL_ADD, timer_ev->fd, &ev) < 0) {  // LCOV_EXCL_BR_LINE 5: epoll_ctl's error case.
    // LCOV_EXCL_START 5: epoll_ctl's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "epoll_ctl:%s", strerror(errno));
    goto exit;
    // LCOV_EXCL_STOP 5: epoll_ctl's error case.
  }

  memset(&itime, 0, sizeof(itime));
  itime.it_value.tv_sec = backup_cycle;
  itime.it_interval.tv_sec = backup_cycle;

  if (timerfd_settime(timer_ev->fd, 0, &itime, NULL) < 0) {  // LCOV_EXCL_BR_LINE 5: timerfd_settime's error case.
    // LCOV_EXCL_START 5: timerfd_settime's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "timerfd_settime:%s", strerror(errno));
    epoll_ctl(efd, EPOLL_CTL_DEL, timer_ev->fd, &ev);
    goto exit;
    // LCOV_EXCL_STOP 5: timerfd_settime's error case.
  }
  ret = 0;

exit:
  if (ret < 0) {  // LCOV_EXCL_BR_LINE 5: C API's error case.
    // LCOV_EXCL_START 5: C API's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (timer_ev->fd >= 0) {
      if (close(timer_ev->fd) < 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "timerfd_settime:%s", strerror(errno));
      }
    }
    delete timer_ev;
  }
  // LCOV_EXCL_STOP 5: C API's error case.
  return ret;
}

static uint64_t *g_bkup_syncreq;
static pthread_rwlock_t g_syncreq_rwlock = PTHREAD_RWLOCK_INITIALIZER;
#define BKUP_SYNCREQ_SIZE (4096)
#define BKUP_SYNCREQ_ID_MAX (BKUP_SYNCREQ_SIZE * 8 - 1)
#define BKUP_SYNCREQ_NAME "syncreq"

static int BkupSyncreqInit(void) {
  int ret = -1;
  char *map;

  map = reinterpret_cast<char *>(mmap(NULL, BKUP_SYNCREQ_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0));
  if (map == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    // LCOV_EXCL_START 5: mmap's error case.
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mmap failed");
    goto exit;
    // LCOV_EXCL_STOP 5: mmap's error case.
  }
  g_bkup_syncreq = reinterpret_cast<uint64_t *>(map);
  memset(g_bkup_syncreq, 0, BKUP_SYNCREQ_SIZE);

  pthread_rwlock_wrlock(&g_syncreq_rwlock);
  BkupBufferWriteToNv(NVHALMEDIA_BACKUPDRAM, BKUP_SYNCREQ_NAME,
                      reinterpret_cast<char *>(g_bkup_syncreq),
                      BKUP_SYNCREQ_SIZE, NULL);
  pthread_rwlock_unlock(&g_syncreq_rwlock);
  ret = 0;

exit:
  return ret;
}

static void BkupSyncreqTerm(void) {
  if (g_bkup_syncreq != NULL) {  // LCOV_EXCL_BR_LINE 6:g_bkup_syncreq will not be NULL
    munmap(g_bkup_syncreq, BKUP_SYNCREQ_SIZE);
    g_bkup_syncreq = NULL;
  }
}

static int BkupSyncreqSet(int id) {
  if (g_bkup_syncreq == NULL) {  // LCOV_EXCL_BR_LINE 6:g_bkup_syncreq will not be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return -1;  // LCOV_EXCL_LINE 5: mmap's error case.
  }
  if (id > BKUP_SYNCREQ_ID_MAX) {
    return -1;
  }
  if (!(*(g_bkup_syncreq + id / 64) & (1ULL << (id % 64)))) {
    *(g_bkup_syncreq + id / 64) |= (1ULL << (id % 64));
    pthread_rwlock_wrlock(&g_syncreq_rwlock);
    BkupBufferWriteToNv(NVHALMEDIA_BACKUPDRAM, BKUP_SYNCREQ_NAME,
                        reinterpret_cast<char *>(g_bkup_syncreq),
                        BKUP_SYNCREQ_SIZE, NULL);
    pthread_rwlock_unlock(&g_syncreq_rwlock);
  }
  return 0;
}

static int BkupSyncreqClear(int id) {
  if (g_bkup_syncreq == NULL) {  // LCOV_EXCL_BR_LINE 6:g_bkup_syncreq will not be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return -1;  // LCOV_EXCL_LINE 5: mmap's error case.
  }
  if (id > BKUP_SYNCREQ_ID_MAX) {
    return -1;
  }
  if (*(g_bkup_syncreq + id / 64) & (1ULL << (id % 64))) {
    *(g_bkup_syncreq + id / 64) &= ~(1ULL << (id % 64));
    pthread_rwlock_wrlock(&g_syncreq_rwlock);
    BkupBufferWriteToNv(NVHALMEDIA_BACKUPDRAM, BKUP_SYNCREQ_NAME,
                        reinterpret_cast<char *>(g_bkup_syncreq),
                        BKUP_SYNCREQ_SIZE, NULL);
    pthread_rwlock_unlock(&g_syncreq_rwlock);
  }

  return 0;
}

static inline int BkupPopcnt64(uint64_t x) {
  uint64_t n;

  n = (x >> 1) & 0x7777777777777777ULL;
  x = x - n;
  n = (n >> 1) & 0x7777777777777777ULL;
  x = x - n;
  n = (n >> 1) & 0x7777777777777777ULL;
  x = x - n;
  x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
  x = x * 0x0101010101010101ULL;
  return static_cast<int>(x >> 56);
}

static int BkupSyncreqSearch(void) {
  int i;

  if (g_bkup_syncreq == NULL) {  // LCOV_EXCL_BR_LINE 6:g_bkup_syncreq will not be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return -1;  // LCOV_EXCL_LINE 5: mmap's error case.
  }

  for (i = 0; i < static_cast<int>(BKUP_SYNCREQ_SIZE / sizeof(uint64_t)); i++) {
    if (*(g_bkup_syncreq + i) != 0) {
      uint64_t bits, mrb1;  // most right bit 1
      bits = *(g_bkup_syncreq + i);
      mrb1 = bits & (-bits);
      return i * 64 + BkupPopcnt64(mrb1 - 1);
    }
  }

  return -1;
}

static void BkupSyncreqDone(bool skip_cache) {
  int id;
  uint64_t start, end;

  start = BkupTimerReal();

  while ((id = BkupSyncreqSearch()) >= 0) {
    char item_name[64];
    bkup_query_result_t query_result;

    if (BkupParamGetNumid(id, &query_result, item_name, sizeof(item_name)) < 0) {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid num_id:%d", id);
      // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
    } else {
      if (query_result.cache_dram) {
        if (!skip_cache) {
          BkupMediaCopyFromNv(query_result.category_name, item_name,
                              NVHALMEDIA_CACHEDRAM, NVHALMEDIA_NAND, NULL,
                              NULL);
        }
      } else if (query_result.backup_dram) {
        // LCOV_EXCL_BR_START 11:unexpected branch
        BkupMediaCopyFromNv(query_result.category_name, item_name,
                            NVHALMEDIA_BACKUPDRAM, NVHALMEDIA_NAND, NULL, NULL);
        // LCOV_EXCL_BR_STOP 11:unexpected branch
      }
    }

    BkupSyncreqClear(id);  // LCOV_EXCL_BR_LINE 11:unexpected branch

  }

  end = BkupTimerReal();
  // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "spent:%" PRIu64 ".%06" PRIu64 "ms",
         static_cast<uint64_t>((end - start) / 1000000),
         static_cast<uint64_t>((end - start) % 1000000));
  // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
}

void BkupSyncreqRestart(void) {
  char *map;

  map = reinterpret_cast<char *>(mmap(NULL, BKUP_SYNCREQ_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0));
  if (map == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    // LCOV_EXCL_START 5: mmap's error case.
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mmap failed");
    goto exit;
    // LCOV_EXCL_STOP 5: mmap's error case.
  }
  g_bkup_syncreq = reinterpret_cast<uint64_t *>(map);

  if (BkupBufferReadFromNv(NVHALMEDIA_BACKUPDRAM, BKUP_SYNCREQ_NAME,
                           reinterpret_cast<char *>(g_bkup_syncreq),
                           BKUP_SYNCREQ_SIZE, NULL) < 0) {
    goto exit;
  }
  BkupSyncreqDone(true);
  BkupSyncreqTerm();

exit:
  return;
}

void *BkupDelayThread(void *arg) {
  mqd_t mqd;
  int efd;
  struct epoll_event ev;
  bkup_delay_t *mq_ev;
  bkup_itc_message_t rcv_msg;
  std::map<std::string, int> delay_req;  // LCOV_EXCL_BR_LINE 11:except,C++ STL

  signal_handler p_signal = SignalHandlerFuncForMsgHandleThread;
  signal(SIGUSR1, p_signal);
  g_delay_thread_id = pthread_self();


  if (prctl(PR_SET_NAME, BKUP_THREAD_DELAY) < 0) {  // LCOV_EXCL_BR_LINE 5: prctl's error case.
    // LCOV_EXCL_START 5: prctl's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "prctl(PR_SET_NAME):%s", strerror(errno));
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 5: prctl's error case.
  }

  // LCOV_EXCL_BR_START 5: mq_open's error case.
  if ((mqd = BkupItcMqopen("/" BKUP_THREAD_DELAY, O_RDONLY | O_CREAT | O_CLOEXEC)) < 0) {
  // LCOV_EXCL_BR_STOP 5: mq_open's error case.
    // LCOV_EXCL_START 5: mq_open's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mq_open:%s", strerror(errno));
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 5: mq_open's error case.
  }

  if ((efd = epoll_create(MAX_FD_EPOLL)) < 0) {  // LCOV_EXCL_BR_LINE 5: epoll_create's error case.
    // LCOV_EXCL_START 5: epoll_create's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "epoll_create:%s", strerror(errno));
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 5: epoll_create's error case.
  }
  g_bkup_delay_tid = static_cast<int>(syscall(__NR_gettid));

  mq_ev = new bkup_delay_t;  // LCOV_EXCL_BR_LINE 11:except,C++ operator
  mq_ev->fd = static_cast<int>(mqd);
  mq_ev->name = "mq";  // LCOV_EXCL_BR_LINE 11:except,C++ STL

  ev.events = EPOLLIN;
  ev.data.ptr = mq_ev;
  // LCOV_EXCL_BR_START 5: epoll_ctl's error case.
  if (epoll_ctl(efd, EPOLL_CTL_ADD, static_cast<int>(mqd), &ev) < 0) {
  // LCOV_EXCL_BR_STOP 5: epoll_ctl's error case.
    // LCOV_EXCL_START 5: epoll_ctl's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "epoll_ctl:%s", strerror(errno));
    exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP 5: epoll_ctl's error case.
  }

  while (1) {
    struct epoll_event events[MAX_FD_EPOLL];
    int nfds;

    nfds = epoll_wait(efd, events, MAX_FD_EPOLL, -1);  // LCOV_EXCL_BR_LINE 11:unexpected branch
    if (nfds < 0) {  // LCOV_EXCL_BR_LINE 5: epoll_wait's error case.
      // LCOV_EXCL_START 5: epoll_wait's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      if (errno != EINTR) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "epoll_wait:%s", strerror(errno));
      }
      continue;
      // LCOV_EXCL_STOP 5: epoll_wait's error case.
    }

    for (int i = 0; i < nfds; i++) {
      bkup_delay_t *cur_ev = reinterpret_cast<bkup_delay_t *>(events[i].data.ptr);

      if (cur_ev->fd == mqd) {
        /*
         * mq
         */
        // LCOV_EXCL_BR_START 5: mq_receive's error case.
        if (mq_receive(mqd, reinterpret_cast<char *>(&rcv_msg), sizeof(rcv_msg), NULL) < 0) {
        // LCOV_EXCL_BR_STOP 5: mq_receive's error case.
          // LCOV_EXCL_START 5: mq_receive's error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          if (errno != EINTR) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mq_receive:%s", strerror(errno));
          }
          continue;
          // LCOV_EXCL_STOP 5: mq_receive's error case.
        }

#if 0
        if (rcv_msg.delay.terminate) {
          // terminate request
          // shutdown sync
          BkupSyncreqDone(false);  // LCOV_EXCL_BR_LINE 11:unexpected branch

          for (std::map<std::string, int>::iterator i = delay_req.begin(); i != delay_req.end(); ++i) {
            i->second = 0;  // LCOV_EXCL_BR_LINE 11:except,C++ STL
          }

          // terminate finish response
          int64_t term_event = 1;
          // LCOV_EXCL_BR_START 5: write's error case.
          if (write(rcv_msg.delay.event_fd, &term_event, sizeof(term_event)) < 0) {
          // LCOV_EXCL_BR_STOP 5: write's error case.
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "write:%s", strerror(errno));  // LCOV_EXCL_LINE 5: write's error case.
          }
        } else {
          // delay request
          bkup_query_result_t query_result;
          BkupParamGet(rcv_msg.delay.item_name, &query_result);  // LCOV_EXCL_BR_LINE 11:unexpected branch

          if (query_result.backup_cycle > 0) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
            if (delay_req.find(rcv_msg.delay.item_name) != delay_req.end()) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
              delay_req[rcv_msg.delay.item_name]++;  // LCOV_EXCL_BR_LINE 11:except,C++ STL
            } else {
              // LCOV_EXCL_BR_START 11:unexpected branch
              BkupDelayTimerAdd(efd, rcv_msg.delay.item_name, query_result.backup_cycle);
              // LCOV_EXCL_BR_STOP 11:unexpected branch
              delay_req[rcv_msg.delay.item_name] = 1;  // LCOV_EXCL_BR_LINE 11:unexpected branch
            }

            if (BkupSyncreqSet(query_result.id) < 0) {
              // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid num_id:%d", query_result.id);
              // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
            }
          } else {
            // write to ACC OFF
            bkup_query_result_t query_result;
            if (BkupParamGet(rcv_msg.delay.item_name, &query_result) < 0) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
              // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid item_name:%s",
                  rcv_msg.delay.item_name != 0 ? rcv_msg.delay.item_name : NULL);
              // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
            } else {
              if (BkupSyncreqSet(query_result.id) < 0) {
                // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid num_id:%d", query_result.id);
                // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
              }
            }
          }
        }
#endif
        switch (rcv_msg.delay.cmd) {
          case BKUP_DELAY_CMD_TERM:
          case BKUP_DELAY_CMD_PRE_ACCOFF:
            /*
             * terminate or pre-acc_off request
             */
            // shutdown sync
            BkupSyncreqDone(false);

            for (std::map<std::string, int>::iterator i = delay_req.begin(); i != delay_req.end(); ++i) {
              i->second = 0;
            }

            if (rcv_msg.delay.cmd == BKUP_DELAY_CMD_TERM) {
              BkupSyncreqTerm();

              // terminate finish response
              int64_t term_event = 1;
              // LCOV_EXCL_BR_START 5: write's error case.
              if (write(rcv_msg.delay.event_fd, &term_event, sizeof(term_event)) < 0) {
              // LCOV_EXCL_BR_STOP 5: write's error case.
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "write:%s", strerror(errno));  // LCOV_EXCL_LINE 5: write's error case.
              }
            }
            break;

          case BKUP_DELAY_CMD_REGIST:
            // delay regist request
            bkup_query_result_t query_result;
            BkupParamGet(rcv_msg.delay.item_name, &query_result);

            if (query_result.backup_cycle > 0) {
              if (delay_req.find(rcv_msg.delay.item_name) != delay_req.end()) {
                delay_req[rcv_msg.delay.item_name]++;
              } else {
                BkupDelayTimerAdd(efd, rcv_msg.delay.item_name, query_result.backup_cycle);
                delay_req[rcv_msg.delay.item_name] = 1;
              }

              if (BkupSyncreqSet(query_result.id) < 0) {
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid num_id:%d", query_result.id);
              }
            } else {
              // wite to ACC OFF
              bkup_query_result_t query_result;
              if (BkupParamGet(rcv_msg.delay.item_name, &query_result) < 0) {  // LCOV_EXCL_BR_LINE 6:double check
                // LCOV_EXCL_START 6:double check
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid item_name:%s", rcv_msg.delay.item_name);
                // LCOV_EXCL_STOP 6:double check
              } else {
                if (BkupSyncreqSet(query_result.id) < 0) {
                  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid num_id:%d", query_result.id);
                }
              }
            }
            break;
          }

      } else {
        /*
         * timerfd
         */
        uint64_t exp;
        if (read(cur_ev->fd, &exp, sizeof(exp)) < 0) {  // LCOV_EXCL_BR_LINE 5: read's error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "read:%s", strerror(errno));  // LCOV_EXCL_LINE 5: read's error case.
        }

        if (delay_req[cur_ev->name] > 0) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
          bkup_query_result_t query_result;
          BkupParamGet(cur_ev->name.c_str(), &query_result);  // LCOV_EXCL_BR_LINE 11:unexpected branch

          if (query_result.cache_dram) {
            // LCOV_EXCL_BR_START 11:unexpected branch
            pthread_rwlock_t *src_rwlock = BkupGetRwlock(
                cur_ev->name.c_str(), NVHALMEDIA_CACHEDRAM, &query_result);
            pthread_rwlock_t *dst_rwlock = BkupGetRwlock(
                cur_ev->name.c_str(), NVHALMEDIA_NAND, &query_result);
            BkupMediaCopyFromNv(query_result.category_name,
                                cur_ev->name.c_str(), NVHALMEDIA_CACHEDRAM,
                                NVHALMEDIA_NAND, src_rwlock, dst_rwlock);
            BkupMediaCopyFromNv(query_result.category_name,
                                cur_ev->name.c_str(), NVHALMEDIA_CACHEDRAM,
                                NVHALMEDIA_NAND, NULL, NULL);
            // LCOV_EXCL_BR_STOP 11:unexpected branch
          } else if (query_result.backup_dram) {
            // LCOV_EXCL_BR_START 11:unexpected branch
            pthread_rwlock_t *src_rwlock = BkupGetRwlock(
                cur_ev->name.c_str(), NVHALMEDIA_BACKUPDRAM, &query_result);
            pthread_rwlock_t *dst_rwlock = BkupGetRwlock(
                cur_ev->name.c_str(), NVHALMEDIA_NAND, &query_result);
            BkupMediaCopyFromNv(query_result.category_name,
                                cur_ev->name.c_str(), NVHALMEDIA_BACKUPDRAM,
                                NVHALMEDIA_NAND, src_rwlock, dst_rwlock);
            // LCOV_EXCL_BR_STOP 11:unexpected branch
          }
          delay_req[cur_ev->name] = 0;  // LCOV_EXCL_BR_LINE 11:unexpected branch
          if (BkupSyncreqClear(query_result.id) < 0) {
            // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Invalid num_id:%d", query_result.id);
            // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
          }
        } else {
          // not update. timer delte
          // LCOV_EXCL_BR_START 5: epoll_ctl's error case.
          if (epoll_ctl(efd, EPOLL_CTL_DEL, cur_ev->fd, &events[i]) < 0) {
          // LCOV_EXCL_BR_STOP 5: epoll_ctl's error case.
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "epoll_ctl:%s", strerror(errno));  // LCOV_EXCL_LINE 5: epoll_ctl's error case.
          }
          if (close(cur_ev->fd) < 0) {  // LCOV_EXCL_BR_LINE 5: close's error case.
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "close:%s", strerror(errno));  // LCOV_EXCL_LINE 5: close's error case.
          }
          delay_req.erase(cur_ev->name);  // LCOV_EXCL_BR_LINE 11:unexpected branch
          delete cur_ev;  // LCOV_EXCL_BR_LINE 11:except,C++ operator
        }
      }
    }
  }

  return NULL;
}

/**
 * Terminate handler
 */
EFrameworkunifiedStatus BkupTerminateHandler(HANDLE h_app) {
  bkup_itc_message_t snd_msg;
  int event_fd;
  int64_t term_event;

  g_bkup_internal_state = BKUP_STATE_TERM;

  if (g_bkup_delay_mqd == -1) {
    // LCOV_EXCL_BR_START 5: mq_open's error case.
    if ((g_bkup_delay_mqd = BkupItcMqopen("/" BKUP_THREAD_DELAY, O_WRONLY | O_CREAT | O_NONBLOCK | O_CLOEXEC)) < 0) {
    // LCOV_EXCL_BR_STOP 5: mq_open's error case.
      // LCOV_EXCL_START 5: mq_open's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mq_open:%s", strerror(errno));
      return eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP 5: mq_open's error case.
    }
  }

  if ((event_fd = eventfd(0, 0)) < 0) {  // LCOV_EXCL_BR_LINE 5: eventfd's error case.
    // LCOV_EXCL_START 5: eventfd's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "eventfd:%s", strerror(errno));
    return eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP 5: eventfd's error case.
  }

  memset(&snd_msg, 0, sizeof(snd_msg));

//  snd_msg.delay.terminate = true;
  snd_msg.delay.cmd = BKUP_DELAY_CMD_TERM;

  snd_msg.delay.event_fd = event_fd;

  // LCOV_EXCL_BR_START 5: mq_send's error case.
  if (mq_send(g_bkup_delay_mqd, reinterpret_cast<char *>(&snd_msg), sizeof(snd_msg), 0) < 0) {
  // LCOV_EXCL_BR_STOP 5: mq_send's error case.
    // LCOV_EXCL_START 5: mq_send's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mq_send:%s", strerror(errno));
    return eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP 5: mq_send's error case.
  }

  while (1) {
    if (read(event_fd, &term_event, sizeof(term_event)) < 0) {  // LCOV_EXCL_BR_LINE 5: read's error case.
      // LCOV_EXCL_START 5: read's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      if (errno == EINTR) {
        continue;
      }
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "read:%s", strerror(errno));
      return eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP 5: read's error case.
    } else {
      break;
    }
  }
  if (close(event_fd) < 0) {  // LCOV_EXCL_BR_LINE 5: close's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "close:%s", strerror(errno));  // LCOV_EXCL_LINE 5: close's error case.
  }

  return eFrameworkunifiedStatusOK;
}

/**
 * Init handler
 */
EFrameworkunifiedStatus BkupInitHandler(HANDLE h_app) {
  BkupSyncreqRestart();
  BkupSyncreqInit();

  g_bkup_internal_state = BKUP_STATE_NORMAL;

  return eFrameworkunifiedStatusOK;
}
