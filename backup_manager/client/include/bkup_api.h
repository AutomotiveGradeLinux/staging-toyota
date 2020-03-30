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

#ifndef BACKUP_MANAGER_CLIENT_INCLUDE_BKUP_API_H_
#define BACKUP_MANAGER_CLIENT_INCLUDE_BKUP_API_H_

#define BACKUP_CID 0xBACC1D00

#define BKUP_MAX_ITEM_NAME 64

/**
 * media
 */
//#include <nv_hal.h>

/**
 * command
 */
typedef enum {
  BKUP_CMD_READ = 0,
  BKUP_CMD_WRITE,
  BKUP_CMD_FILL,
  BKUP_CMD_SIZE,
  BKUP_CMD_READ_NUM,
  BKUP_CMD_SIZE_NUM,
  BKUP_CMD_CHECK,
  BKUP_CMD_DELETE,
  BKUP_CMD_COPY_INNER,
  BKUP_CMD_MAX,
} bkup_protocol_command_t;

/**
 * api/process protocol
 */
typedef struct {
  bkup_protocol_command_t command;
  char item_name[64];
  uint32_t num_id;
  uint32_t offset;
  uint32_t size;
  char fill_patern;
//  enum NvHalMedia src_media;
} bkup_protocol_header_t;

#define SERVICE_BACKUP_MANAGER_MSG_HANDLE_THREAD "bkupmanagerthr"

#endif  // BACKUP_MANAGER_CLIENT_INCLUDE_BKUP_API_H_
