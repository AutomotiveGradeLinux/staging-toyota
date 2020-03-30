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
 * @file tskm_type.h
 * @brief \~english type of tskm
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup task_manager
 *  @ingroup system_service
 *  @{
 */
#ifndef TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_TSKM_TYPE_H_
#define TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_TSKM_TYPE_H_



#include <stdint.h>
#include <native_service/frameworkunified_types.h>
#include <system_service/ss_services.h>
#include "system_service/tskm_svcid.h"


#define TSKM_ERR_t  int32_t
#define TSKM_E_OK       0
#define TSKM_E_PAR      1
#define TSKM_E_STATE    2
#define TSKM_E_PERMIT   3
#define TSKM_E_ALREADY  4
#define TSKM_E_RETRY    5
#define TSKM_E_NG       6

#define TSKM_BOOL_t   int32_t
#define TSKM_TRUE     1
#define TSKM_FALSE    0

#define TSKM_RSV_t   int

typedef uint32_t TSKM_SVCID_t;
// Values are defined in tskm_svcid.h

typedef enum {
  TSKM_SVC_CMD_NOP,       // Do nothing
  TSKM_SVC_CMD_EXEC,      // Startup service
  TSKM_SVC_CMD_ENABLE,    // Allow service startup
  TSKM_SVC_CMD_DISABLE,   // Prohibit service startup
  TSKM_SVC_CMD_RESERVE,   // Reserve service startup
} TSKM_SVC_CMD_t;

typedef struct {
  TSKM_SVC_CMD_t cmd;
} TSKM_SVC_CTL_t;

typedef struct {
  TSKM_SVCID_t svcId;
  TSKM_BOOL_t isExecDisable;
} TSKM_SVC_INFO_t;

typedef enum {
  TSKM_BOOT_NORMAL,       // Normal start
  TSKM_BOOT_FACTRESET,    // start with factory default start
  TSKM_BOOT_USERRESET,    // start with resetting user info
  TSKM_BOOT_VERSIONUP,    // start with version up
} TSKM_BOOT_t;

typedef enum {
  TSKM_LOGGING_TYPE_MODULE_LOGS,   // logging request from the service
  TSKM_LOGGING_TYPE_GRP_RELAUNCH,  // logging by Group Relaunch
} TSKM_LOGGING_TYPE_NORMAL_t;

typedef enum {
  TSKM_ERROR_REBOOT_NORMAL,  // Anomaly detection
} TSKM_ERROR_REBOOT_TYPE_t;

#define TSKM_LOGGING_MSG_STR_SIZE 64

typedef struct {
  TSKM_LOGGING_TYPE_NORMAL_t type;                                   // Log type
  char messageStr[TSKM_LOGGING_MSG_STR_SIZE];  // Error message
} TSKM_LOGGING_INFO_t;

typedef struct {
  TSKM_ERROR_REBOOT_TYPE_t type;  // Reboot type
  TSKM_LOGGING_INFO_t log;        // Log info
} TSKM_ERROR_REBOOT_t;

typedef enum {
  TSKM_DATAINIT_TYPE_USER,  // Reset user info
} TSKM_DATAINIT_TYPE_t;

typedef struct {
  TSKM_DATAINIT_TYPE_t type;
  EFrameworkunifiedStatus (*onCompInit)(HANDLE hApp);
} TSKM_DATAINIT_t;

#define TSKM_ORDER_NAME_MAX 32

typedef struct {
  char orderName[TSKM_ORDER_NAME_MAX];
} TSKM_WAKEUP_ORDER_t;

#endif  // TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_TSKM_TYPE_H_
/** @}*/
/** @}*/
/** @}*/
