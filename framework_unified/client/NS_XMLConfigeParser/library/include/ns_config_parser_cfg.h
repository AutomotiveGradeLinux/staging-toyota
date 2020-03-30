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
/// \ingroup MY 14 Platform Software Team
/// \brief   Internal Header for ns_config_parser_cfg.c
///
///  These functions are used to read  from and write to cfg file.
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_XMLCONFIGEPARSER_LIBRARY_INCLUDE_NS_CONFIG_PARSER_CFG_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_XMLCONFIGEPARSER_LIBRARY_INCLUDE_NS_CONFIG_PARSER_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////
#include <ns_config_parser_internal.h>
////////////////////////////////////////////////////////////////////////////////////////////
/// CFGGetValue:
/// Function : Traverse cfg file and read value corresponding to key.
///
/// \param [in] filePtr
///       FILE * - filePtr is a Pointer to a file descriptor.
/// \param [in] pKey
///       const char * -pKey is a Pointer to user key.
/// \param [in] pRetValue
///       char *  - pRetValue is a Pointer to value or values to be returned corresponding to user key.
/// \return status
///       int  - value returned depending upon user provided Key is valid or not.\n
///       returns 1 if user passed invalid key.\n
///       returns 0 if user passed valid key.\n
///
///////////////////////////////////////////////////////////////////////////////////////////
int CFGGetValue(FILE *file_ptr, const char *p_key, char *p_ret_value);

////////////////////////////////////////////////////////////////////////////////////////////
/// CFGUpdateOldValue:
/// Function : Traverse the cfg file and update the value corresponding to given key.
///
/// \param [in] hConfigFile
///       HANDLE       - hConfigFile is a handle to given file.
/// \param [in] pKey
///       const char * - pKey is a Pointer to user key.
/// \param [in] pValue
///       const char * - pValue is a Pointer to new value corresponding to user key.
/// \return status
///       int - returns 1 if user passed invalid key.\n
///             returns 0 if user passed valid key.\n
///                   returns -1 if memory allocation fail.\n
///             returns -6 if value not updated in file.\n
///
///////////////////////////////////////////////////////////////////////////////////////////
int  CFGUpdateOldValue(HANDLE h_config_file, const char *p_key, const char *p_value);

////////////////////////////////////////////////////////////////////////////////////////////
/// CFGUpdateOldValueWithNewValue
/// Function : Traverse the cfg file read value to given node,compare with old value,\n
///            if old value matched then write new value in place of old value.\n
///
/// \param [in] hConfigFile
///       HANDLE       - hConfigFile is a handle to given file.
/// \param [in] pKey
///       const char * - pKey is a Pointer to user key.
/// \param [in] pOldValue
///       const char * - pOldValue is a Pointer to Old Value corresponding to user key.
/// \param [in] pNewValue
///       const char * - pNewValue is a Pointer to New Value corresponding to user key.
/// \return status
///       int - returns 1 if user passed invalid key.\n
///             returns 0 if user passed valid key.\n
///                   returns -1 if memory allocation fail.\n
///             returns -6 if value not updated in file.\n
///
///////////////////////////////////////////////////////////////////////////////////////////

int  CFGUpdateOldValueWithNewValue(HANDLE h_config_file, const char *p_key,
                                   const char *p_old_value, const char *p_new_value);

#ifdef __cplusplus
extern "C" {
#endif

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_XMLCONFIGEPARSER_LIBRARY_INCLUDE_NS_CONFIG_PARSER_CFG_H_

// EOF
