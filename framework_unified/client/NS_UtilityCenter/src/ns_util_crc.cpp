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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_UtilityCenter
/// \brief    This file contains implementation of APIs to calcuate 16-bit and 32-bit CRC checksum of file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_util_crc.h>
#include <stdio.h>

// buffer size for reading from file
#define MAX_BUFFER_SIZE 4096

// Static 32 bit CRC lookup table
UI_16 g_arr_crc16_table[256] = {
};

// Static 32 bit CRC lookup table
UI_32 g_arr_crc32_table[256] = {
};


////////////////////////////////////////////////////////////////////////////////////////////////
/// CalculateCRC16
/// This API calculates the 16 bit CRC checksum of a file
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CalculateCRC16(PCSTR f_c_file_name, UI_16 &f_ui_check_sum) {  // NOLINT (readability/nolint)
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  CHAR l_c_buffer[MAX_BUFFER_SIZE] = {0};
  size_t l_ui_bytes_read = 0;
  FILE *l_p_file = NULL;

  if (NULL != f_c_file_name) {
    f_ui_check_sum = 0xFFFF;

    // Open the file
    l_p_file = fopen(f_c_file_name, "rbe");

    if (NULL == l_p_file) {
      l_e_status = eFrameworkunifiedStatusFileLoadError;
    } else {
      l_ui_bytes_read = fread(l_c_buffer, 1, sizeof(l_c_buffer), l_p_file);

      while (0 != l_ui_bytes_read) {  // LCOV_EXCL_BR_LINE 11: not a branch
        for (UI_32 l_ui_bytes_cnt = 0; l_ui_bytes_cnt < l_ui_bytes_read; l_ui_bytes_cnt++) {  // LCOV_EXCL_BR_LINE 11: not a branch
          f_ui_check_sum = static_cast<UI_16>(
                g_arr_crc16_table[((f_ui_check_sum >> 8) ^ l_c_buffer[l_ui_bytes_cnt]) & 0xFF] ^ (f_ui_check_sum << 8));
        }

        l_ui_bytes_read = fread(l_c_buffer, 1, sizeof(l_c_buffer), l_p_file);
      }

      fclose(l_p_file);
      l_p_file = NULL;
    }

    f_ui_check_sum = static_cast<UI_16>(~f_ui_check_sum);
  } else {
    l_e_status = eFrameworkunifiedStatusInvldParam;
  }

  return l_e_status;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// CalculateCRC32
/// This API calculates the 32 bit CRC checksum of a file
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CalculateCRC32(PCSTR f_c_file_name, UI_32 &f_ui_check_sum) {  // NOLINT (readability/nolint)
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  CHAR l_c_buffer[MAX_BUFFER_SIZE] = {0};
  size_t l_ui_bytes_read = 0;
  FILE *l_p_file = NULL;

  if (NULL != f_c_file_name) {
    f_ui_check_sum = 0xFFFFFFFF;

    // Open the file
    l_p_file = fopen(f_c_file_name, "rbe");

    if (NULL == l_p_file) {
      l_e_status = eFrameworkunifiedStatusFileLoadError;
    } else {
      l_ui_bytes_read = fread(l_c_buffer, 1, sizeof(l_c_buffer), l_p_file);

      while (0 != l_ui_bytes_read) {  // LCOV_EXCL_BR_LINE 11: not a branch
        for (UI_32 l_ui_bytes_cnt = 0; l_ui_bytes_cnt < l_ui_bytes_read; l_ui_bytes_cnt++) {  // LCOV_EXCL_BR_LINE 11: not a branch
          f_ui_check_sum = (f_ui_check_sum >> 8)
            ^ g_arr_crc32_table[(l_c_buffer[l_ui_bytes_cnt] ^ f_ui_check_sum)  & 0xFF];
        }

        l_ui_bytes_read = fread(l_c_buffer, 1, sizeof(l_c_buffer), l_p_file);
      }

      fclose(l_p_file);
      l_p_file = NULL;
    }

    f_ui_check_sum = ~f_ui_check_sum;
  } else {
    l_e_status = eFrameworkunifiedStatusInvldParam;
  }

  return l_e_status;
}
