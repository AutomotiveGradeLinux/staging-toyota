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
 * @file ss_services.h
 * @brief \~english This file supports contains the queue names of all System Services processes.
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup interface_unified
 *  @ingroup system_service
 *  @{
 */
#ifndef SS_SERVICES_H_  // NOLINT (build/header_guard)
#define SS_SERVICES_H_

#include <agl_thread.h>


// Caution:ServiceName length mas is 15
// System_Services domain services   0123456789ABCDE
#define SS_DEV_DETECT_THD           "SS_Dev_Detect_Thd"
#define SS_DEV_DETECT_SRV           MN_SS_DEVDETECTSRV
#define SS_DEV_DETECT_BSP_THD       "SS_DeviceDetectErr"
#define SS_GROUP_LAUNCH_TRIGGER     "SS_GroupLaunchTrigger"
#define SERVICE_HEARTBEAT           "SS_HeartBeatService"
#define SERVICE_LOGGER              MN_SS_LOGGERSRV
#define SS_UDEV_DEV_DETECT_DLL      "SS_UDEV_DeviceDetect"
#define SS_PLM_SERVICE              "SS_PLMService"
#define SERVICE_POWER               MN_SS_POWERSERVICE
#define SS_RESOURCE_MONITOR_SERVICE "SS_ResourceMonitorService"
#define SERVICE_SOFTWAREUPDATE      "SS_SofUpdateSrv"
#define SERVICE_SYSMANAGER          MN_SS_SYSMANAGER
#define SS_WINSYS                   MN_SS_WINSYS
#define SS_RESOURCE_MANAGER         MN_SS_RESOURCEMGR
#define SS_TASK_MANAGER             MN_SS_TASKMANAGER
#define SERVICE_VUP                 MN_VUPSERVICE
#define SS_UPDATESERVICE            MN_SS_UPDATESERVICE

/// @}

#endif /* SS_SERVICES_H_ */  // NOLINT (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
