/*
 * @copyright Copyright (c) 2019-2020 TOYOTA MOTOR CORPORATION.
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

/*
 * Information.
 * This source code is a sample source code .
 * Implementation of the function must be performed by the vendor.
 */

#include "stub/el_mem.h"

off_t EL_mem_getOffset(int mem_id) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return 0;
}

size_t EL_mem_getLength(int mem_id) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return 1;
}

int dummy[1] = {0};     /* dummy area for mmap() */

void *EL_mem_exram_mmap(int type, off_t offset , size_t length , int prot, int flags , int cache_flag) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return ((void *)dummy);
}

void *EL_mem_exram_mmap_simple(int mem_id , int prot, int flags , int cache_flag) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return ((void *)dummy);
}

int EL_mem_exram_munmap(void *addr, size_t length) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return 0;
}

u_int64_t EL_mem_getPhysicalAddress(int mem_id) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return 0;
}
