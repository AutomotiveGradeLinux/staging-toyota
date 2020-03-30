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

#ifndef TASK_MANAGER_SERVER_INCLUDE_TSKM_DEBUG_H_
#define TASK_MANAGER_SERVER_INCLUDE_TSKM_DEBUG_H_

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <native_service/ns_logger_if.h>
#include "system_service/tskm_svc.h"

#include "system_service/tskm_local_type.h"

#define TSKM_STATIC static

// for frameworkunifiedlog
#define ZONE_INIT         ZONEMASK(10)  // Reserved for lib
#define ZONE_FUNC         ZONEMASK(11)  // Reserved for lib
#define ZONE_MEM          ZONEMASK(12)  // Reserved for lib
#define ZONE_OUT          ZONEMASK(13)
#define ZONE_STATE        ZONEMASK(14)  // Basic state change
#define ZONE_15           ZONEMASK(15)
#define ZONE_16           ZONEMASK(16)
#define ZONE_17           ZONEMASK(17)
#define ZONE_18           ZONEMASK(18)
#define ZONE_19           ZONEMASK(19)
#define ZONE_20           ZONEMASK(20)
#define ZONE_21           ZONEMASK(21)
#define ZONE_22           ZONEMASK(22)
#define ZONE_23           ZONEMASK(23)
#define ZONE_24           ZONEMASK(24)
#define ZONE_25           ZONEMASK(25)
#define ZONE_26           ZONEMASK(26)
#define ZONE_27           ZONEMASK(27)
#define ZONE_28           ZONEMASK(28)
#define ZONE_INFO         ZONEMASK(29)  // Reserved for lib
#define ZONE_WARN         ZONEMASK(30)  // Reserved for lib
#define ZONE_ERR          ZONEMASK(31)  // Reserved for lib

#define ZONE_TEXT_10      "Init"
#define ZONE_TEXT_11      "Function"
#define ZONE_TEXT_12      "Memory"
#define ZONE_TEXT_13      ""
#define ZONE_TEXT_14      ""
#define ZONE_TEXT_15      ""
#define ZONE_TEXT_16      ""
#define ZONE_TEXT_17      ""
#define ZONE_TEXT_18      ""
#define ZONE_TEXT_19      ""
#define ZONE_TEXT_20      ""
#define ZONE_TEXT_21      ""
#define ZONE_TEXT_22      ""
#define ZONE_TEXT_23      ""
#define ZONE_TEXT_24      ""
#define ZONE_TEXT_25      ""
#define ZONE_TEXT_26      ""
#define ZONE_TEXT_27      ""
#define ZONE_TEXT_28      ""
#define ZONE_TEXT_29      "Info"
#define ZONE_TEXT_30      "Warning"
#define ZONE_TEXT_31      "Error"

#ifndef  FRAMEWORKUNIFIEDLOGOPTIONS
#define FRAMEWORKUNIFIEDLOGOPTIONS (LPRINT|LMSGQ)  // LPRINT , LMSGQ, LSLOGGER
#endif

#ifndef FRAMEWORKUNIFIEDLOGAPPZONES
#define FRAMEWORKUNIFIEDLOGAPPZONES ZONE_OUT, ZONE_WARN, ZONE_ERR, ZONE_STATE, ZONE_PERFORMANCE, ZONE_SYSTEMDATA
/*
 ZONE_NS_FUNC,\
  ZONE_NS_DIS,\
  ZONE_NS__CWORD77_,\
  ZONE_NS_IMP_INFO,\
  ZONE_NS_SM_DEV_INFO,\
  ZONE_NS_SM_USR_INFO,\
  ZONE_NS_INFO,\
  ZONE_NS_DEBUG_DUMP,\
  ZONE_NS_WAR       ,\
  ZONE_NS_ERR       ,\
  ZONE_DEBUG_DUMP
 */
#endif

#define TSKM_LOG_ERROR      0x00000001U
#define TSKM_LOG_STATE      0x00000002U
#define TSKM_LOG_DEBUG      0x00000004U
#define TSKM_LOG_API        0x00000008U
#define TSKM_LOG_PORTPF     0x00000010U
#define TSKM_LOG_FUNC       0x00000020U
#define TSKM_LOG_MSG        0x00000040U
#define TSKM_LOG_WARN       0x00000080U
#define TSKM_LOG_SVCSTATE   0x00000100U
#define TSKM_LOG_SYSTEMDATA 0x00000200U

// For debugging
#define TSKM_LOG_CUSTUM  (0 \
                        )
/*
 |TSKM_LOG_MSG    \
                         |TSKM_LOG_DEBUG  \
                         |TSKM_LOG_FUNC   \
                         |TSKM_LOG_PORTPF \
 */

typedef enum {
  TSKM_LOG_TYPE_TEXT,
  TSKM_LOG_TYPE_PLACE,
} TSKM_LOG_TYPE_t;

#define TSKM_DEFAULT_PRINTF(flag, fmt, ...) { \
    switch (flag) { \
        case TSKM_LOG_SYSTEMDATA: \
            FRAMEWORKUNIFIEDLOG(ZONE_SYSTEMDATA, __FUNCTION__, fmt, ##__VA_ARGS__); \
            break; \
        case TSKM_LOG_API: \
        case TSKM_LOG_STATE: \
            FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, fmt, ##__VA_ARGS__); \
            break; \
        case TSKM_LOG_ERROR: \
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, fmt, ##__VA_ARGS__); \
            break; \
        case TSKM_LOG_WARN: \
            FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, fmt, ##__VA_ARGS__); \
            break; \
        case TSKM_LOG_DEBUG: \
            FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, fmt, ##__VA_ARGS__); \
            break; \
        case TSKM_LOG_MSG: \
        case TSKM_LOG_FUNC: \
        case TSKM_LOG_PORTPF: \
        default: \
            FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, fmt, ##__VA_ARGS__); \
            break; \
    } \
  }

#define TSKM_PRINTF(flag, fmt, ...) { \
    if (TSKM_LOG_SVCSTATE == flag) { \
      /* Logs at startup and termination */ \
      FRAMEWORKUNIFIEDLOG(ZONE_PERFORMANCE, __FUNCTION__, fmt, ## __VA_ARGS__); \
    } else if (TSKM_LOG_CUSTUM & flag) { \
      /* Logging by STATE when CUSTUM is specified */ \
      FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, fmt, ## __VA_ARGS__); \
    } else { \
      TSKM_DEFAULT_PRINTF(flag, fmt, ## __VA_ARGS__); \
    } \
  }

#define TSKM_ASSERT(x)  \
          if (!(x)) { \
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "TSKM_ASSERT"); \
          }

#define TSKM_ASSERT_ERRNO(x)  \
          if (!(x)) { \
             FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "TSKM_ASSERT %d:%s", errno, strerror(errno)); \
          }

#define TSKM_ASSERT_PRINT(x, fmt, ...) \
          if (!(x)) { \
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "TSKM_ASSERT:" fmt, ## __VA_ARGS__); \
          }

#define TSKM_STATIC_ASSERT(expr) \
  { \
    char STATIC_ASSERTION_FAILED[(expr) ? 1 : -1]; \
    (void)STATIC_ASSERTION_FAILED; \
  }

#define TSKM_DUMP(addr, size) { \
    char* buf = (char*)addr;  /* NOLINT (readability/casting) */  \
    int ii; \
    printf("[DUMP] %s:%d addr:%p size:%d", __FUNCTION__, __LINE__, addr, size); \
    for (ii=0 ; ii < size ; ii++) { \
      if (ii%16 == 0) printf(" "); \
      if (ii%32 == 0) printf("\n");  \
      printf("%02x", buf[ii]);  \
    } \
    printf("\n"); \
  }

#define TSKM_FUNC_IN() TSKM_PRINTF(TSKM_LOG_FUNC, "%s:IN", __FUNCTION__)

#define TSKM_FUNC_OUT() TSKM_PRINTF(TSKM_LOG_FUNC, "%s:OUT", __FUNCTION__)

const char* tskm_convState2Str(TSKM_STATE_t state);
const char* tskm_convLocalStep2Str(TSKM_LOCAL_STEP_t localStep);
const char* tskm_convEvent2Str(TSKM_EVENT_t event);
const char* tskm_convInitCompId2Str(uint64_t compId);
const char* tskm_convTermCompId2Str(uint64_t compId);
const char* tskm_convSvcState2Str(TSKM_SVC_STATE_t state);
void tskm_svcsDump(TSKM_SVCS_CTX_t* p_svcs);

#endif  // TASK_MANAGER_SERVER_INCLUDE_TSKM_DEBUG_H_
