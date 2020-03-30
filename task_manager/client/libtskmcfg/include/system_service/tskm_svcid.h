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
 * @file tskm_svcid.h
 * @brief \~english This file contains declaration of TSKM_SVC ID
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
/*
 * Service ID
 */
#ifndef TASK_MANAGER_CLIENT_LIBTSKMCFG_INCLUDE_SYSTEM_SERVICE_TSKM_SVCID_H_
#define TASK_MANAGER_CLIENT_LIBTSKMCFG_INCLUDE_SYSTEM_SERVICE_TSKM_SVCID_H_


#define TSKM_SVCID_NONE         0x00000000U
#define TSKM_SVCID_TE_RESIDENT  0x00000001U  // for test
#define TSKM_SVCID_TE_TRANSIENT 0x00000002U  // for test
#define TSKM_SVCID_ACTIVITYMGR  0x00000003U
#define TSKM_SVCID_SYSVUP       0x00000004U  // for vup
#define TSKM_SVCID__CWORD52_VUP     0x00000005U  // for vup
#define TSKM_SVCID_ROOTFSVUP    0x00000006U  // for vup
#define TSKM_SVCID_VUPPROGUI    0x00000007U  // for vup
#define TSKM_SVCID_NORVUP       0x00000008U  // for vup
#define TSKM_SVCID__CWORD58_         0x00000009U  // for _CWORD76_
#define TSKM_SVCID_SYSUPDATE    0x0000000AU  // for repro vup
#define TSKM_SVCID_NANDUPDATE   0x0000000BU  // for repro vup
#define TSKM_SVCID_BTPHONESRV   0x0000000CU
#define TSKM_SVCID_BTPBKSRV     0x0000000DU
#define TSKM_SVCID_BTMSGSRV     0x0000000EU
#define TSKM_SVCID_DTVVUPSRV    0x0000000FU  // fot peaks vup
#define TSKM_SVCID_XMVUPSRV     0x00000020U  // fot XM_outer_update vup

#endif  // TASK_MANAGER_CLIENT_LIBTSKMCFG_INCLUDE_SYSTEM_SERVICE_TSKM_SVCID_H_
/** @}*/
/** @}*/
/** @}*/
