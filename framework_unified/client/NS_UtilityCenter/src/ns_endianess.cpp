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


#include <native_service/ns_endianess.h>

UI_16 ConvertEndian_UI16(UI_16 f_value) {  // NOLINT (readability/nolint)
  UI_16 l_ret = static_cast<UI_16>((f_value << 8) | (f_value >> 8));
  return l_ret;
}


SI_16 ConvertEndian_SI16(SI_16 f_value) {  // NOLINT (readability/nolint)
  SI_16 l_ret = static_cast<SI_16>((f_value << 8) | ((f_value >> 8) & 0xFF));
  return l_ret;
}


UI_32 ConvertEndian_UI32(UI_32 f_value) {  // NOLINT (readability/nolint)
  UI_32 l_ret = ((f_value << 8) & 0xFF00FF00) | ((f_value >> 8) & 0xFF00FF);
  return (l_ret << 16) | (l_ret >> 16);
}

SI_32 ConvertEndian_SI32(SI_32 f_value) {  // NOLINT (readability/nolint)
  SI_32 l_ret = ((f_value << 8) & 0xFF00FF00) | ((f_value >> 8) & 0xFF00FF);
  return (l_ret << 16) | ((l_ret >> 16) & 0xFFFF);
}

UI_64 ConvertEndian_UI64(UI_64 f_value) {  // // NOLINT (readability/nolint)
  UI_64 l_ret = ((f_value << 8) & 0xFF00FF00FF00FF00ULL) | ((f_value >> 8) & 0x00FF00FF00FF00FFULL);
  l_ret = ((l_ret << 16) & 0xFFFF0000FFFF0000ULL) | ((l_ret >> 16) & 0x0000FFFF0000FFFFULL);
  return (l_ret << 32) | (l_ret >> 32);
}

SI_64 ConvertEndian_SI64(SI_64 f_value) {  // NOLINT (readability/nolint)
  SI_64 l_ret = ((f_value << 8) & 0xFF00FF00FF00FF00ULL) | ((f_value >> 8) & 0x00FF00FF00FF00FFULL);
  l_ret = ((l_ret << 16) & 0xFFFF0000FFFF0000ULL) | ((l_ret >> 16) & 0x0000FFFF0000FFFFULL);
  return (l_ret << 32) | ((l_ret >> 32) & 0xFFFFFFFFULL);
}
