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

#ifndef BACKUP_MANAGER_SERVER_INCLUDE_BKUP_PARAM_H_
#define BACKUP_MANAGER_SERVER_INCLUDE_BKUP_PARAM_H_

typedef struct {
  int id;
  int size;
  void *opt;
  bool nand;
  bool cache_dram;
  bool backup_dram;
  bool sync;
  bool encrypt;
  int backup_cycle;
  char category_name[64];
} bkup_query_result_t;

int BckupParamInit(void);

int BkupParamGet(const char *item_name, bkup_query_result_t *result);
int BkupParamGetNumid(uint32_t num_id, bkup_query_result_t *result, char *item_name, size_t item_name_size);
void *BkupParamSetOpt(const char *item_name, void *opt);

#endif  // BACKUP_MANAGER_SERVER_INCLUDE_BKUP_PARAM_H_
