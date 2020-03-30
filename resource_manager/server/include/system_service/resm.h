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
 * @file resm.h
 * @brief \~english This file contains declaration of APIs for resm library
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

#ifndef RESOURCE_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_RESM_H_
#define RESOURCE_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_RESM_H_

#include "system_service/resm_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RESM_Open
/// \~english @par Summary
///      Opens the session with resource manager.
/// \~english @param [in] p_prim
///       RESM_RSV_t* - Unused, NULL
/// \~english @param [out] p_ssnId
///       uint32_t*  - Pointer to session ID
/// \~english @par
///       RESM_RSV_t Variables
/// \~english @code
///            #define RESM_RSV_t        int32_t
///            #define RESM_RSV_NULL     (0x00)  // NULL
///            @endcode
/// \~english @retval RESM_E_OK Succeeded
/// \~english @retval RESM_E_PAR %Parameter error
/// \~english @retval RESM_E_NG Unexpected error
/// \~english @par Preconditions
///       - Availability of ResourceManager must be TRUE.
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       - p_ssnId is NULL. [RESM_E_PAR]
///       - The global variable for getting RPC_ID of program (rpcId) is NULL. [RESM_E_NG]
///       - The area for RPC ID(rpcId) is not gotten. [RESM_E_NG]
///       - The number of the thread able to register in a process exceeds limit. [RESM_E_NG]
///       - It failed to get the area for thread information. [RESM_E_NG]
///       - It failed to get the area for ID information storage. [RESM_E_NG]
///       - It failed to generate the datagram socket for API request receiving. [RESM_E_NG]
///       - It failed to bind a socket. [RESM_E_NG]
///       - It failed to get a socket name. [RESM_E_NG]
///       - The number of sessions exceeds the maximum value. [RESM_E_NG]
///       - There is no space in the thread information area for event registration. [RESM_E_NG]
///       - It failed to get the thread information area for event registration. [RESM_E_NG]
///       - The file descriptor for getting an event is created. [RESM_E_NG]
///       - The generated session ID exists (EEXIST error at ioctl). [RESM_E_NG]
///       - Any error occurred during registering an event flag (EEXIST error at ioctl). [RESM_E_NG]
///       - Any error occurred during processing poling setting (error at ioctl). [RESM_E_NG]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       None
/// \~english @par Detail
///      Opens a session. \n
///      An application can use another API by using the gotten ssnID.
/// \~english @see RESM_Close
////////////////////////////////////////////////////////////////////////////////////
RESM_ERR_t RESM_Open(const RESM_RSV_t* p_prim, uint32_t* p_ssnld);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RESM_Close
/// \~english @par Summary
///     Closes the session with resource manager.
/// \~english @param [in] ssnld
///       uint32_t  - Session ID
/// \~english @retval RESM_E_OK Succeeded
/// \~english @retval RESM_E_PAR %Parameter error
/// \~english @par Preconditions
///       - An application must finish getting a session ID by RESM_Open().
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       - ssnId exceeds the maximum value (EV_MAX_IDS_IN_THREAD). [RESM_E_PAR]
///       - The in-use flag of a global variable (g_resmgr.ssnInfo[ssnId].useFlag) is FALSE. [RESM_E_PAR]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       None
/// \~english @par Detail
///      Closes a session.
/// \~english @see RESM_Open
////////////////////////////////////////////////////////////////////////////////////
RESM_ERR_t RESM_Close(uint32_t ssnld);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RESM_ReqEvent
/// \~english @par Summary
///      Requests to issue an event.
/// \~english @param [in] ssnld
///       uint32_t  - Session ID
/// \~english @param [in] p_reqEvent
///       RESM_REQ_EVENT_t*  - Pointer to event
/// \~english @par
///       RESM_REQ_EVENT_t Structure
/// \~english @code
///            typedef struct {
///                RESM_EV_t reqEvent;          /* Event flag necessary to report */
///                struct {
///                    uint32_t restMemThresh;  /* Threshold of the remaining capacity of system memory */
///                } prm;
///            }RESM_REQ_EVENT_t;
///            @endcode
/// \~english @par
///       RESM_EV_t Variables
/// \~english @code
///            #define RESM_EV_t           uint32_t
///            #define RESM_EV_NOP         (0x00)
///            #define RESM_EV_MEM         (0x01)
///            #define RESM_EV_NAND_STATUS (0x02)
///            @endcode
/// \~english @par
///       - RESM_EV_NOP : No event notification
///       - RESM_EV_MEM : Event notification for memory
///       - RESM_EV_NAND_STATUS : Eventnotification for NAND state
/// \~english @par
///       Notes
///       - restMemThresh is valid only when RESM_EV_MEM is set to reqEvent. \n
///         ResourceManager issues RESM_EV_MEM event at the timing
///         when the remaining capacity of system memory fell below the value (unit is BYTE)
///         which an application specifies.
/// \~english @retval RESM_E_OK Succeeded
/// \~english @retval RESM_E_PAR %Parameter error
/// \~english @par Preconditions
///       - An application must finish getting a session ID by RESM_Open().
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       - p_reqEvent is NULL. [RESM_E_PAR]
///       - ssnId exceeds the maximum value (EV_MAX_IDS_IN_THREAD). [RESM_E_PAR]
///       - The in-use flag of a global variable (g_resmgr.ssnInfo[ssnId].useFlag) is FALSE. [RESM_E_PAR]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       None
/// \~english @par Detail
///      Requests to issue an event.
///      This API is called from the RPC library.
/// \~english @see RESM_GetEvent
////////////////////////////////////////////////////////////////////////////////////
RESM_ERR_t RESM_ReqEvent(uint32_t ssnld, const RESM_REQ_EVENT_t* p_reqEvent);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RESM_GetEventFd
/// \~english @par Summary
///      Gets FD of the event to receive from resource manager.
/// \~english @param [in] ssnld
///       uint32_t - Session ID
/// \~english @param [out] p_fd
///       int*  - Pointer to file descriptor
/// \~english @retval RESM_E_OK Succeeded
/// \~english @retval RESM_E_PAR %Parameter error
/// \~english @retval RESM_E_NG Unexpected error
/// \~english @par Preconditions
///       - An application must finish getting a session ID by RESM_Open().
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       - p_fd is NULL. [RESM_E_PAR]
///       - ssnId exceeds the maximum value (EV_MAX_IDS_IN_THREAD). [RESM_E_PAR]
///       - The in-use flag of a global variable (g_resmgr.ssnInfo[ssnId].useFlag) is FALSE. [RESM_E_PAR]
///       - ssnId rewrites bit for an event flag decision by invalid values. [RESM_E_NG]
///       - The file descriptor corresponding to ssnId is not found. [RESM_E_NG]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       None
/// \~english @par Detail
///      Gets FD of the event to receive from resource manager. \n
///      An application can do simultaneous wait of other events with the event of ResourceManager
///      by oversighting the gotten FD by select,poll etc.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
RESM_ERR_t RESM_GetEventFd(uint32_t ssnld, int* p_fd);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RESM_GetEvent
/// \~english @par Summary
///      Gets the event from resource manager.
/// \~english @param [in] ssnld
///       uint32_t - Session ID
/// \~english @param [out] p_evFlag
///       RESM_EV_t*  - Pointer to event
/// \~english @par
///       RESM_EV_t Variables
/// \~english @code
///            #define RESM_EV_t           uint32_t
///            #define RESM_EV_NOP         (0x00)
///            #define RESM_EV_MEM         (0x01)
///            #define RESM_EV_NAND_STATUS (0x02)
///            @endcode
/// \~english @par
///       - RESM_EV_NOP : No event notification
///       - RESM_EV_MEM : Event notification for memory
///       - RESM_EV_NAND_STATUS : Event notification for NAND state
/// \~english @retval RESM_E_OK Succeeded
/// \~english @retval RESM_E_PAR %Parameter error
/// \~english @retval RESM_E_NG Unexpected error
/// \~english @par Preconditions
///       - An application must finish getting a session ID by RESM_Open().
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       - p_evFlag is NULL. [RESM_E_PAR]
///       - ssnId exceeds the maximum value (EV_MAX_IDS_IN_THREAD). [RESM_E_PAR]
///       - The in-use flag of a global variable (g_resmgr.ssnInfo[ssnId].useFlag) is FALSE. [RESM_E_PAR]
///       - ssnId rewrites bit for an event flag decision by invalid values. [RESM_E_NG]
///       - The file descriptor corresponding to ssnId is not found. [RESM_E_NG]
///       - The flag of ssnId is not registered. (The returned value of ioctl is EOENT.) [RESM_E_NG]
///       - Any interruption occurred during getting an event. (The returned value of ioctl is EINTR.) [RESM_E_NG]
///       - Any error occurred during getting an event. (The returned value of ioctl is the error value
///         except for EOENT and EINTR.) [RESM_E_NG]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       None
/// \~english @par Detail
///      When an event flag is not set, this is blocked. \n
///      If an event flag is set, this stores event contents to p_evFlag and this ends. \n
///      If this ends, it clears all of event flags.
/// \~english @see RESM_ReqEvent
////////////////////////////////////////////////////////////////////////////////////
RESM_ERR_t RESM_GetEvent(uint32_t ssnld, RESM_EV_t* p_evFlag);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RESM_GetStatus
/// \~english @par Summary
///      Gets system state.
/// \~english @param [in] ssnld
///       uint32_t - Session ID
/// \~english @param [out] p_status
///       RESM_STATUS_t*  - Pointer to system state
/// \~english @par
///       RESM_STATUS_t Structure
/// \~english @code
///            typedef struct {
///                uint32_t restMemSize;                       /* Remaining memory length (KB)                       */
///                RESM_NAND_WRITE_STATUS_t  nandWriteStatus;  /* Access permission / Prohibition state to NAND flash */
///                RESM_INET_STATUS_t        inetStatus;       /* Network IF statistical information after start    */
///            } RESM_STATUS_t;
///            @endcode
/// \~english @par
///       enum RESM_NAND_WRITE_STATUS_t Variables
///       - RESM_NAND_WRITE_ENABLE : NAND access permission
///       - RESM_NAND_WRITE_DISABLE : NAND access prohibition
/// \~english @par
///       Notes
///       - When system records in NAND flash and it influences the lifetime of a device,
///         STATUS(nandWriteStatus) becomes RESM_NAND_WRITE_DISABLE. It is not in the state where WRITE fails. \n
/// \~english @par
///       RESM_INET_STATUS_t Structure
/// \~english @code
///            typedef struct {
///                uint32_t ifNum;                    /* Valid array at the number of interface and ifInfo */
///                struct {
///                    char    name[64];              /* Interface name                             */
///                    uint64_t  rxSize;              /* Receiving data length (KiB)                   */
///                    uint64_t  txSize;              /* Transmission data length (KiB)                */
///                    uint8_t   hwaddr[HWADDR_LEN];  /* Hardware address (MAC address)         */
///                } ifInfo[RESM_INET_IF_MAX];
///            }RESM_INET_STATUS_t;
///            @endcode
/// \~english @retval RESM_E_OK Succeeded
/// \~english @retval RESM_E_PAR %Parameter error
/// \~english @retval RESM_E_NG Unexpected error
/// \~english @par Preconditions
///       - An application must finish getting a session ID by RESM_Open().
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       - p_evFlag is NULL. [RESM_E_PAR]
///       - ssnId exceeds the maximum value (EV_MAX_IDS_IN_THREAD). [RESM_E_PAR]
///       - The in-use flag of a global variable (g_resmgr.ssnInfo[ssnId].useFlag) is FALSE. [RESM_E_PAR]
///       - It failed to open the device file for network. [RESM_E_NG]
///       - The number of the gotten network IF is 0. [RESM_E_NG]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       None
/// \~english @par Detail
///      Gets system state.
///      This API is called from the RPC library.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
RESM_ERR_t RESM_GetStatus(uint32_t ssnld, RESM_STATUS_t* p_status);

#ifdef __cplusplus
}
#endif

#endif  // RESOURCE_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_RESM_H_

/** @}*/  // end of ResourceManager
/** @}*/  // end of SystemService
/** @}*/  // end of BaseSystem
