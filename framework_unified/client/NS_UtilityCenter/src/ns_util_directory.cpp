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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_NS_UtilityCenter
/// \brief   This file contains API implementation for handling file and folder functionalities.
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/ns_util_directory.h>

#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////
/// GetFileList
/// Get list of files present in a specific directory
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus GetFileList(TFileList *f_pv_tfile_list, PCSTR f_pc_path) {
  EFrameworkunifiedStatus l_e_status = eFrameworkunifiedStatusOK;

  DIR *l_p_dir = NULL;
  struct dirent *l_pt_dirent = NULL;

  if (NULL != f_pc_path && NULL != f_pv_tfile_list) {
    if (NULL != (l_p_dir  = opendir(f_pc_path))) {
      PCHAR l_c_file_name = NULL;
      size_t l_ui_str_length = 0;
      while (NULL != (l_pt_dirent = readdir(l_p_dir))) {  // LCOV_EXCL_BR_LINE 11: not a branch
        if (!((std::strcmp(l_pt_dirent->d_name, ".") == 0) || (std::strcmp(l_pt_dirent->d_name, "..") == 0))) {
          l_ui_str_length = std::strlen(l_pt_dirent->d_name);

          l_c_file_name = new(std::nothrow) CHAR[l_ui_str_length + 1];

          if (NULL != l_c_file_name) {  // LCOV_EXCL_BR_LINE 11: new's error case(c++)
            std::memset(l_c_file_name, 0, (sizeof(CHAR) * (l_ui_str_length + 1)));

            std::strncpy(l_c_file_name, l_pt_dirent->d_name, l_ui_str_length);

            f_pv_tfile_list->push_back(l_c_file_name);  // LCOV_EXCL_BR_LINE 11: not a branch

            delete[] l_c_file_name;  // LCOV_EXCL_BR_LINE 11: not a branch
            l_c_file_name = NULL;
          } else {
            l_e_status = eFrameworkunifiedStatusNullPointer;
          }
        }
      }
      closedir(l_p_dir);
    } else {
      l_e_status = eFrameworkunifiedStatusFail;
    }
  } else {
    l_e_status = eFrameworkunifiedStatusInvldParam;
  }

  return l_e_status;
}


////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateDirectory
/// Method to create a directory.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CreateDirectory(std::string f_c_dir_path) {
  EFrameworkunifiedStatus   l_e_status = eFrameworkunifiedStatusOK;

  if (!f_c_dir_path.empty()) {
    PSTR     l_c_temp_dir_path;
    PSTR     l_c_parsed_dir_path;
    PCSTR    l_c_copy_path = f_c_dir_path.c_str();

    l_c_temp_dir_path =  const_cast<PSTR >(f_c_dir_path.c_str());
    // LCOV_EXCL_BR_START 11: not a branch
    while (l_e_status == eFrameworkunifiedStatusOK && (l_c_parsed_dir_path = std::strchr(l_c_temp_dir_path, '/')) != 0) {
    // LCOV_EXCL_BR_STOP
      if (l_c_parsed_dir_path != l_c_temp_dir_path) {
        /* Neither root nor double slash in path */
        *l_c_parsed_dir_path = '\0';
        if (0 != mkdir(l_c_copy_path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) {
          if (EEXIST != errno) {  // LCOV_EXCL_BR_LINE 11: not a branch
            l_e_status = eFrameworkunifiedStatusFail;
          } else {
            // file already exist
            if (TRUE != DoesDirecotryExist(f_c_dir_path)) {  // LCOV_EXCL_BR_LINE 5: DoesDirecotryExist's error case
              l_e_status = eFrameworkunifiedStatusFail;
            }
          }
        }
        *l_c_parsed_dir_path = '/';
      }
      l_c_temp_dir_path = l_c_parsed_dir_path + 1;
    }
    if (eFrameworkunifiedStatusOK == l_e_status) {  // LCOV_EXCL_BR_LINE 11: not a branch
      if (0 != mkdir(l_c_copy_path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) {
        if (EEXIST != errno) {  // LCOV_EXCL_BR_LINE 11: not a branch
          l_e_status = eFrameworkunifiedStatusFail;
        } else {
          // file already exist
          if (TRUE != DoesDirecotryExist(f_c_dir_path)) {  // LCOV_EXCL_BR_LINE 5: DoesDirecotryExist's error case
            l_e_status = eFrameworkunifiedStatusFail;
          }
        }
      }
    }
  } else {
    l_e_status = eFrameworkunifiedStatusFail;
  }
  return l_e_status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// DoesDirecotryExist
/// Method to check if a directory exists.
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL DoesDirecotryExist(std::string f_c_dir_path) {
  BOOL l_b_directory_status = FALSE;

  DIR *l_p_dir_descriptor = opendir(f_c_dir_path.c_str());
  if (NULL != l_p_dir_descriptor) {
    closedir(l_p_dir_descriptor);
    l_b_directory_status =  TRUE;
  }
  return l_b_directory_status;
}

