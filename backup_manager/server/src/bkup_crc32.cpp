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

#include <cstdio>
#include "bkup_crc32.h"
#include "bkup_util.h"

/*
 * CRC-32C lookup table
 *
 * Polynomial 0x1EDC6F41
 */
static const uint32_t kBkupCrc32Lookup[4][256] = {
  {
  },
  {
  },
  {
  },
  {
  }
};

/**
 * compute CRC-32C (Slicing-by-4)
 */
uint32_t BkupCrc32(const void *data, size_t length) {
  uint32_t crc = 0xFFFFFFFF;
  const uint32_t *current = (const uint32_t *)data;

  while (length >= 4) {
    uint32_t one = *current++ ^ crc;
    crc = kBkupCrc32Lookup[0][(one >> 24) & 0xFF] ^
          kBkupCrc32Lookup[1][(one >> 16) & 0xFF] ^
          kBkupCrc32Lookup[2][(one >> 8) & 0xFF] ^
          kBkupCrc32Lookup[3][one & 0xFF];
    length -= 4;
  }

  const uint8_t *current_char = (const uint8_t *)current;
  while (length-- > 0) {
    crc = (crc >> 8) ^ kBkupCrc32Lookup[0][(crc & 0xFF) ^ *current_char++];
  }

  return ~crc;
}
