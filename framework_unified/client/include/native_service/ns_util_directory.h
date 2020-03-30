/**
 * @file ns_util_directory.h
 * @brief \~english This file contains declaration of ns_util_directory.
 *
 */
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
/// \brief    This file contains utility functions to handle file and folder operations.
///
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////



#ifndef  NS_NS_UTILITYCENTER_INC_NS_UTIL_DIRECTORY_H_  // NOLINT  (build/header_guard)
#define  NS_NS_UTILITYCENTER_INC_NS_UTIL_DIRECTORY_H_

#include <native_service/frameworkunified_types.h>
#include <vector>
#include <string>

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */
/** @addtogroup utility
 *  @ingroup framework_unified
 *  @{
 */

/**
 * \~english an abbreviation for std::vector<std::string>
 */
typedef std::vector< std::string > TFileList;

/**
 * \~english a abbreviation for std::vector<std::string>::iterator
 */
typedef TFileList::iterator TFileListIterator;

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Get the name of files existing in a specific directory
/// \~english @param [in] f_pc_path
///       PCSTR - the directory path
/// \~english @param [out] f_pv_tfile_list
///       TFileList  - store the name of the files in the specific directory
/// \~english @retval eFrameworkunifiedStatusOK : Success
/// \~english @retval eFrameworkunifiedStatusInvldParam : Invalid Param
/// \~english @retval eFrameworkunifiedStatusNullPointer : NULL Pointer
/// \~english @retval eFrameworkunifiedStatusFail : Abnormal Error
///
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - The pointer f_pv_tfile_list or f_pc_path is NULL. [eFrameworkunifiedStatusInvldParam]
///       - Failed to open the directory f_pc_path. [eFrameworkunifiedStatusFail]
///       - Failed to new a buffer for inner using. [eFrameworkunifiedStatusNullPointer]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function gets all the name of the file locating at the directory f_pc_path and stores \n
///       them in f_pv_tfile_list.
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus GetFileList(TFileList * f_pv_tfile_list, PCSTR f_pc_path);

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Check whether the directory is existing or not.
/// \~english @param [in] f_c_dir_path
///       std::string - the directory to be checked
/// \~english @retval TRUE: the directory is existing.
///                   FALSE: the directory is not existing.
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function checks whether the directory is existing or not.
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL DoesDirecotryExist(std::string f_c_dir_path);

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_UtilityCenter
/// \~english @par Brief
///       Create the directory f_c_dir_path.
/// \~english @param [in] f_c_dir_path
///       std::string - the directory to be created
/// \~english @retval eFrameworkunifiedStatusOK : Success
/// \~english @retval eFrameworkunifiedStatusFail : Abnormal Error
///
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - The input param f_c_dir_path is empty. [eFrameworkunifiedStatusFail]
///       - Faild to create(mkdir) the direcory. [eFrameworkunifiedStatusFail]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function creates the directory f_c_dir_path.
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CreateDirectory(std::string f_c_dir_path);


#ifdef __cplusplus
}
#endif

#endif  // NS_NS_UTILITYCENTER_INC_NS_UTIL_DIRECTORY_H_  NOLINT  (build/header_guard)

/** @}*/  // end of utility
/** @}*/  // end of framework_unified
/** @}*/  // end of native_service
/** @}*/  // end of BaseSystem
