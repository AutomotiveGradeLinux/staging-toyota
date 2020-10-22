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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_NSMessageCenter
/// \brief
///
/// Header defines the messaging IF header that will be at the start of each message.
///
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGECENTER_INCLUDE_NS_MSGS_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGECENTER_INCLUDE_NS_MSGS_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/ns_shared_mem_if.h>

#define MC_EMSGT_VALID_TOKEN (0xC8 << 24)
#define MC_EMSGT_VALID_MASK (0xFF << 24)
#define MC_EMSGT_VALID(t) ((t & MC_EMSGT_VALID_MASK) == MC_EMSGT_VALID_TOKEN)  // NOLINT  (readability/nolint)

#define MC_EMSGT_SHM_MASK (0x1 << 0)
#define MC_EMSGT_IS_SHM(t) (t & MC_EMSGT_SHM_MASK)  // NOLINT  (readability/nolint)

#define MC_EMSGT_SYNC_MASK (0x1 << 1)
#define MC_EMSGT_IS_SYNC(t) (t & MC_EMSGT_SYNC_MASK)  // NOLINT  (readability/nolint)

#define MC_EMSGT_SYSINFO_MASK (0x1 << 2)
#define MC_EMSGT_HAS_SYSINFO(t) (t & MC_EMSGT_SYSINFO_MASK)  // NOLINT  (readability/nolint)

#pragma pack(1)
/// \brief brief description of Messages Header that's pass via Message Center.
/// Note: Don't remove #paragma pack(1) this is required for the
/// ARM processor, forces data structures to be on a byte boundary
/// all messages should be place in between  #pragma pack(1) and #pragma pack()
typedef struct tSysMsgHeader {
  UI_32   eType;      // The type of the message
  TMemID    memId;      // Unique MemID of the data stored in shared memory,
  // this is only valid if eType == IPC_MSG_SHM_TYPE
  struct {
    CHAR    source[MAX_QUEUE_NAME_SIZE];  // Task that's sending the message.
    UI_32   command;            // The command id of the message
    UI_32   length;             // The length of the complete message in contiguous memory
    // including the header and data block
    UI_32   sessionid;
  } info;
} SSystemMsgHeader, *PSSystemMsgHeader;

#pragma pack()

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGECENTER_INCLUDE_NS_MSGS_H_
