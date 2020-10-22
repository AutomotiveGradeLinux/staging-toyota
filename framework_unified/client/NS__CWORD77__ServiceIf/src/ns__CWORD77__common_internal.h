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
/// @defgroup tag_HMI_CWORD77_Controller HMI _CWORD77_ Controller
/// @file  ns__CWORD77__common_internal.h
/// @brief
///
////////////////////////////////////////////////////////////////////////////////
//@{

#ifndef NS_NS__CWORD77__SERVICEIF_SRC_NS__CWORD77__COMMON_INTERNAL_H_  // NOLINT  (readability/nolint)
#define NS_NS__CWORD77__SERVICEIF_SRC_NS__CWORD77__COMMON_INTERNAL_H_

#include <native_service/ns__CWORD77__types.h>
#include <cstdlib>
#include <iostream>
#include <string>

EFrameworkunifiedStatus Set_CWORD77_DataPoolKeyValue(std::string Key, std::string Value);
std::string Get_CWORD77_DataPoolKeyValue(std::string Key);

template <typename T>
UI_32 _CWORD77_GetMsgLength(HANDLE hApp) {
  UI_32 uiLength = FrameworkunifiedGetMsgLength(hApp);
  return (uiLength / sizeof(T));
}

template <typename T>
EFrameworkunifiedStatus _CWORD77_GetMsgData(HANDLE hApp, T *pData) {
  EFrameworkunifiedStatus  eStatus = eFrameworkunifiedStatusOK;
  UI_8 *pTemp = reinterpret_cast<UI_8 *>(pData);
  eStatus = FrameworkunifiedGetMsgDataOfSize(hApp, pTemp, sizeof(T));
  return eStatus;
}

#endif  // NS_NS__CWORD77__SERVICEIF_SRC_NS__CWORD77__COMMON_INTERNAL_H_  // NOLINT  (readability/nolint)
//@}
