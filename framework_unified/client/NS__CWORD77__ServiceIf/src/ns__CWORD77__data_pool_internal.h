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

////////////////////////////////////////////////////////////////////////////////
/// @file  ns__CWORD77__data_pool_internal.h
/// @brief
///
////////////////////////////////////////////////////////////////////////////////
//@{

#ifndef NS_NS__CWORD77__SERVICEIF_SRC_NS__CWORD77__DATA_POOL_INTERNAL_H_  // NOLINT  (readability/nolint)
#define NS_NS__CWORD77__SERVICEIF_SRC_NS__CWORD77__DATA_POOL_INTERNAL_H_

#include <native_service/ns__CWORD77__types.h>

////////////////////////////////////////////////////////////////////////////////////////////
///   SetRespDataIn_CWORD77_DataPool
/// API to set data associated with response into _CWORD77_ Data Pool
///
/// \param  [in] f_DataPoolKey
///     _CWORD77_DataPoolKey  - Data pool key
/// \param  [in] f_uiSize
///     UI_32  - size of the data
/// \param  [in] f_pData
///     PVOID  - void pointer to data
///
/// \return VOID - None
///
////////////////////////////////////////////////////////////////////////////////////////////
VOID SetRespDataIn_CWORD77_DataPool(const _CWORD77_DataPoolKey &f_DataPoolKey, UI_32 f_uiSize, const PVOID f_pData);

////////////////////////////////////////////////////////////////////////////////////////////
///   GetReqDataIn_CWORD77_DataPool
/// API to get data associated with request from _CWORD77_ Data Pool
///
/// \param  [in] f_DataPoolKey
///     _CWORD77_DataPoolKey  - Data pool key
/// \param  [in] f_uiSize
///     UI_32  - size of the data
/// \param  [in] f_pData
///     PVOID  - void pointer to data
///
/// \return EFrameworkunifiedStatus - Succes or Error
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus GetReqDataFrom_CWORD77_DataPool(const _CWORD77_DataPoolKey &f_DataPoolKey,  // NOLINT  (readability/nolint)
                                     UI_32 &f_uiSize,  // NOLINT  (readability/nolint)
                                     PVOID &f_pData);  // NOLINT  (readability/nolint)

#endif  // NS_NS__CWORD77__SERVICEIF_SRC_NS__CWORD77__DATA_POOL_INTERNAL_H_  // NOLINT  (readability/nolint)
//@}
