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

/**
 * @file ns__CWORD77__data_pool_if.h
 * @brief \~english This file provides api to operating _CWORD77_DataPool
 *
 */
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
/** @addtogroup _CWORD77_
 *  @ingroup framework_unified
 *  @{
 */
////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_HMI_CWORD77_Controller
/// \brief
///
////////////////////////////////////////////////////////////////////////////////
//@{
#ifndef _C_CWORD77_DATAPOOLIF_H_  // NOLINT  (build/header_guard)
#define _C_CWORD77_DATAPOOLIF_H_

#include <native_service/ns__CWORD77__types.h>

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_HMI_CWORD77_Controller
  /// \~english @par Brief
  ///        API to set data associated with request into _CWORD77_ Data Pool
  /// \~english @param  [in] f_DataPoolKey
  ///        _CWORD77_DataPoolKey  - Data pool key
  /// \~english @param  [in] f_uiSize
  ///        UI_32  - size of the data
  /// \~english @param  [in] f_pData
  ///        PVOID  - void pointer to data
  /// \~english @retval none
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///       - None.
  /// \~english @par Detail
  ///        API to set data associated with request into _CWORD77_ Data Pool
  /// \~english @par
  ///       It is maintained by executing C_CWORD77_Data::SetRespoData().
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          Set Get
  /// \~english @see GetRespDataFrom_CWORD77_DataPool, C_CWORD77_Data::SetRespoData
  ////////////////////////////////////////////////////////////////////////////////////////////
VOID SetReqDataIn_CWORD77_DataPool(const _CWORD77_DataPoolKey &f_DataPoolKey, UI_32 f_uiSize, PVOID f_pData);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_HMI_CWORD77_Controller
  /// \~english @par Brief
  ///        API to get data associated with response from _CWORD77_ Data Pool
  /// \~english @param  [in] f_DataPoolKey
  ///        _CWORD77_DataPoolKey  - Data pool key
  /// \~english @param  [out] f_uiSize
  ///        UI_32  - size of the data
  /// \~english @param  [out] f_pData
  ///        PVOID  - void pointer to data
  /// \~english @retval eFrameworkunifiedStatusOK    Success
  /// \~english @retval eFrameworkunifiedStatusFail  Error
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///       - C_CWORD77_Data Class instance fail or create fail [eFrameworkunifiedStatusFail]
  ///       - f_DataPoolKey is not registered [eFrameworkunifiedStatusFail]
  ///       - f_pData is NULL [eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///        API to get data associated with response from _CWORD77_ Data Pool
  /// \~english @par
  ///       Use SetRespNotfnDataFrom_CWORD77_DataPool() to keep the data.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          Set Get
  /// \~english @see SetRespNotfnDataFrom_CWORD77_DataPool, C_CWORD77_Data::GetRespoData
  ////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus GetRespDataFrom_CWORD77_DataPool(const _CWORD77_DataPoolKey &f_DataPoolKey,  // NOLINT  (readability/nolint)
                                      UI_32 &f_uiSize,  // NOLINT  (readability/nolint)
                                      PVOID &f_pData);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_HMI_CWORD77_Controller
  /// \~english @par Brief
  ///        API to set data associated with notification response into _CWORD77_ Data Pool
  /// \~english @param[in] f_cNotificationName
  ///        const std::string& - Name of the notification
  /// \~english @param[in] f_uiSize
  ///        UI_32 - size of the data
  /// \~english @param[in] f_pData
  ///        const PVOID - void pointer to data
  /// \~english @retval none
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///       - None.
  /// \~english @par Detail
  ///        API to set data associated with notification response into _CWORD77_ Data Pool
  /// \~english @par
  ///       Use GetRespNotfnDataFrom_CWORD77_DataPool() to get the held data.
  /// \~english @par
  ///       It is maintained by executing C_CWORD77_Data::SetRespoData().
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///          Set Get
  /// \~english @see GetRespNotfnDataFrom_CWORD77_DataPool, C_CWORD77_Data::SetRespoData
  ////////////////////////////////////////////////////////////////////////////////////////////
VOID SetRespNotfnDataIn_CWORD77_DataPool(const std::string &f_cNotificationName, UI_32 f_uiSize, const PVOID f_pData);

  ////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tag_HMI_CWORD77_Controller
  /// \~english @par Brief
  ///        API to set data associated with notification response into _CWORD77_ Data Pool
  /// \~english @param[in] f_cNotificationName
  ///        const std::string& - Name of the notification
  /// \~english @param[out] f_uiSize
  ///        UI_32& - size of the data
  /// \~english @param[out] f_pData
  ///        PVOID& - void pointer to data
  /// \~english @retval eFrameworkunifiedStatusOK    Success
  /// \~english @retval eFrameworkunifiedStatusFail  Fail
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///       - C_CWORD77_Data Class instance fail or create fail [eFrameworkunifiedStatusFail]
  ///       - f_cNotificationName is not registered [eFrameworkunifiedStatusFail]
  ///       - f_pData is NULL [eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///        API to set data associated with notification response into _CWORD77_ Data Pool
  /// \~english @par
  ///       Use SetRespNotfnDataFrom_CWORD77_DataPool() to keep the data.
  /// \~english @par
  ///       Get by executing C_CWORD77_Data::GetRespoData().
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///          Set Get
  /// \~english @see SetRespNotfnDataFrom_CWORD77_DataPool, C_CWORD77_Data::GetRespoData
  ////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus GetRespNotfnDataFrom_CWORD77_DataPool(const std::string &f_cNotificationName,  // NOLINT  (readability/nolint)
                                           UI_32 &f_uiSize,  // NOLINT  (readability/nolint)
                                           PVOID &f_pData);  // NOLINT  (readability/nolint)

#endif  // _C_CWORD77_DATAPOOLIF_H_ NOLINT  (build/header_guard)
//@}
/** @}*/
/** @}*/
/** @}*/
/** @}*/
