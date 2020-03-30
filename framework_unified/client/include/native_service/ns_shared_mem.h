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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSSharedMemory
/// \brief Shared interface file for SharedMemory Service
/// Detail description of interface and usage
/// \ref http://ptcframeworkunifiedaccbuild2/TikiWiki/tiki-index.php?page=Shared+Memory
///      http://ptcframeworkunifiedaspsql01/sites/ptcsweng/SW%20Engineering%20Document%20Library/
///             Projects/Connected%20Color%20Radio%20(CCR)/Architecture/Framework/
///             CCR%20Messaging.vsd
///
/// Declares the external APIs to Zone Player Service.
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file ns_shared_mem.h
 * @brief \~english Shared interface file for SharedMemory Service
 *
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */
/** @addtogroup native
 *  @ingroup framework_unified
 *  @{
 */

#ifndef __CCR_NATIVESERVICES_MESSAGECENTER_SHAREDMEMORY_H__  // NOLINT  (build/header_guard)
#define __CCR_NATIVESERVICES_MESSAGECENTER_SHAREDMEMORY_H__


#include <native_service/frameworkunified_types.h>          ///< Header file for Template
#include <stdint.h>
#ifdef AGL_STUB
#else
#include <devctl.h>
#include <sys/neutrino.h>
#endif

typedef struct _pulse_t TPosixHdr;  ///> Header for system-provided messages

enum ESharedMemCommand {
  eSharedMemCommandWrite = 1,
  eSharedMemCommandRead = 2,
  eSharedMemCommandSize = 3,
  eSharedMemCommandDelete = 4,
};

#define NS_SHAREDMEM_NAME "NS_SharedMem"
#define NS_SHAREDMEM_SOCK_QUEUE 10
DEFINE_HANDLE_TYPE(SSHMemID, TShMemID);

#ifdef AGL_STUB
#else
#include <_pack1.h>
#endif
struct SMsgHeader {
  uint16_t cmd;  // Command to the NS_SharedMem
  uintptr_t id;  // NS_SharedMem ID
  UI_32 size;   // The size of the data to write to NS_SharedMem
};

struct SReplyHeader {
  EFrameworkunifiedStatus status;  // write result
  UI_32 size;     // The size of the data to read from NS_SharedMem
};
#ifdef AGL_STUB
#else
#include <_packpop.h>
#endif

#endif  // __CCR_NATIVESERVICES_MESSAGECENTER_SHAREDMEMORY_H__  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
