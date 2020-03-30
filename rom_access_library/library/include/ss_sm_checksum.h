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
/// \ingroup  tag_SystemManager
/// \brief    This file provides for checksum calculation.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SS_SM_CHECKSUM_H_
#define ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SS_SM_CHECKSUM_H_

#include <native_service/frameworkunified_types.h>
class CSMChecksum {
 public:
  CSMChecksum();
  ~CSMChecksum();
  UI_32 cal_checksum(const void* p_buf, size_t buf_size);

 private:
};
#endif  // ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SS_SM_CHECKSUM_H_
