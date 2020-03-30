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

#include <string.h>

size_t strlcpy(char *dst, const char *src, size_t siz) {
  size_t ret = strlen(src);

  if (siz) {
    size_t len = (ret >= siz) ? siz - 1 : ret;
    memcpy(dst, src, len);
    dst[len] = '\0';
  }
  return ret;
}

size_t strlcat(char *dst, const char *src, size_t siz) {
  size_t dsiz = strlen(dst);
  size_t len = strlen(src);
  size_t res = dsiz + len;

  dst += dsiz;
  siz -= dsiz;
  if (len >= siz) {
    len = siz - 1;
  }
  memcpy(dst, src, len);
  dst[len] = '\0';
  return res;
}
