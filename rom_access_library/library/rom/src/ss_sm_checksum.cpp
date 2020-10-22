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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SystemManager
/// \brief    This file provides for checksum calculation.
///
///////////////////////////////////////////////////////////////////////////////
#include "ss_sm_checksum.h"

CSMChecksum::CSMChecksum() {}
CSMChecksum::~CSMChecksum() {}
UI_32 CSMChecksum::cal_checksum(const void* p_buf, size_t bufSize) {
  size_t num;
  UI_32 sum = 0;
  const UI_32 *p_calcBuf = reinterpret_cast<const UI_32 *>(p_buf);
  num = bufSize / sizeof(UI_32);

  for (size_t i = 0; i < num; i++) {
    sum += p_calcBuf[i];
  }
  return sum;
}

