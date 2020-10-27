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
 * @file rpc_id.h
 * @brief \~english   RPC library -- RPC_ID(port no) define
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup other_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup rpc_library
 *  @ingroup other_service
 *  @{
 */

/** @brief \~english define the RPC_ID used by program
 *  like #define XXX_RPC_ID ID
 *  but RPC_START_CLIENT() is not necessary
 *
 * RPC_ID as the following
 * - UI Application       : 50xxx
 * - Application Service  : 51xxx
 *   + HMI                : 511xx
 *   + AV                 : 512xx
 *   + Radio              : 513xx
 *   + Connectivity       : 514xx
 *   + Navigation/Location: 515xx
 *   + Vehicle            : 516xx
 *   + Communication      : 517xx
 *   + BasePF             : 518xx
 *   + System             : 519xx
 *   + Others             : 510xx
 * - Common Service       : 53xxx
 * - Extension Service    : 55xxx
 * - System/Device Driver : 56xxx
 *
 * - 49152~59999 are Reserved.
 * @note  \~english RPC_ID==port num ,
 *        when not call from RPClibrary, and not well-known(~1023)/registered(1024~49151)
 *        use the fixed port num to IP communicate, use 6xxxx port number.
 * @note  \~english when RPC_START_CLIENT,and distribute automatically.
 *        use the linux kernel port distribute function
 *        1024~4999 as the port num unused.
 */

#ifndef OTHERSERVICE_RPCID_H_  // NOLINT(build/header_guard)
#define OTHERSERVICE_RPCID_H_  // NOLINT(build/header_guard)


#define test_RPC_ID             49999
///< \~english sample(ID is used by RPClibrary test program)

/* UI Aapplication      : 50xxx */

/* Application Service  : 51xxx */
#define MODEMANAGER_RPC_ID      51100  /* ModeManager */
#define ACTIVITYMANAGER_RPC_ID  51101  /* ActivityManager */

/* Common Service       : 53xxx */
#define TIMERENTRYDRV_RPC_ID    53000  /* TimerEntryDrv */
#define TSKM_RPC_ID             53001  /* TaskManager */
#define MSGBRK_RPC_ID           53002  /* MessageBroker */
#define IPMANAGER_RPC_ID        53003  /* IPManager */
#define IPMANAGER_RPC_ID        53003  /* IPManager */
#define DEVICEMANAGER_RPC_ID    53004  /* DeviceManager */
#define CAN_RPC_ID              53005  /* Communication(CAN) */
#define _CWORD83__RPC_ID        53006  /* Communication(_CWORD83_) */
#define SENSOR_RPC_ID           53007  /* Vehicle(Sensor) */
#define GPS_RPC_ID              53008  /* Vehicle(GPS) */
#define RESMGR_RPC_ID           53009  /* ResourceManager */
#define GRAPHICS_RPC_ID         53010  /* Graphic */

/* Extension Service    : 55xxx */

/* System/Device Driver : 56xxx */
/** @}*/  // end of rpc_library
/** @}*/  // end of other_service
/** @}*/  // end of BaseSystem
#endif  // OTHERSERVICE_RPCID_H_
