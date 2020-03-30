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
/**
 * @file resm_type.h
 * @brief \~english This file contains declaration of common enum and structures for resm library
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup resource_manager
 *  @ingroup system_service
 *  @{
 */
#ifndef RESOURCE_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_RESM_TYPE_H_
#define RESOURCE_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_RESM_TYPE_H_

#include <agl_thread.h>

#define NTFY_ResourceMgr_Availability MN_SS_RESOURCEMGR"/Availability"

#define RESM_ERR_t      int32_t
#define RESM_E_OK       (0)
#define RESM_E_PAR      (-1)
#define RESM_E_NG       (-2)

// Event flags (bit pattern)
#define RESM_EV_t           uint32_t
#define RESM_EV_NOP         (0x00)
#define RESM_EV_MEM         (0x01)
#define RESM_EV_NAND_STATUS (0x02)

#define RESM_RSV_t        int32_t

typedef enum {
  RESM_NAND_WRITE_ENABLE = 0,
  RESM_NAND_WRITE_DISABLE = -1
} RESM_NAND_WRITE_STATUS_t;

typedef struct {
  RESM_EV_t reqEvent;
  struct {
    uint32_t restMemThresh;
  } prm;
} RESM_REQ_EVENT_t;

#define RESM_INET_IF_MAX 5      /* tentative */
#define HWADDR_LEN (6)
typedef struct {
  uint32_t ifNum;
  struct {
    char name[64];
    uint64_t rxSize;
    uint64_t txSize;
    uint8_t hwaddr[HWADDR_LEN];
  } ifInfo[RESM_INET_IF_MAX];
} RESM_INET_STATUS_t;

typedef struct {
  uint32_t restMemSize;
  RESM_NAND_WRITE_STATUS_t nandWriteStatus;
  RESM_INET_STATUS_t inetStatus;
} RESM_STATUS_t;

#endif  // RESOURCE_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_RESM_TYPE_H_
/** @}*/
/** @}*/
/** @}*/
