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
 * @file ns__CWORD77__common.h
 * @brief \~english This file has templet class declaration of CCallback and CResCallback
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
#ifndef _NS__CWORD77_COMMON_H_  // NOLINT  (build/header_guard)
#define _NS__CWORD77_COMMON_H_

#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns__CWORD77__types.h>

#include <cstdlib>
#include <iostream>
#include <string>

#define MAX_DATA_SIZE 512;
// using namespace std;


// typedef string ServiceName;


#define DELETEPTR(p) {if (p != NULL) {delete p; p = NULL;}}

typedef EFrameworkunifiedStatus(*_CWORD77_FuncPtr)(HANDLE hApp, BOOL bTImerExpiry);
typedef EFrameworkunifiedStatus(*ResponseServiceTo_CWORD77_)(HANDLE hApp, E_CWORD77_ResponseType ResponseType, std::string ServiceName);
typedef boost::function< EFrameworkunifiedStatus(HANDLE, std::string, std::string, UI_32, EFrameworkunifiedStatus) > SessionAckTo_CWORD77_;

// typedef EFrameworkunifiedStatus (*_CWORD77_TimeoutFuncPtr)(HANDLE hApp,UI_32 uiCmdId, E_CWORD77_MessageType MessageType);

template <typename C, EFrameworkunifiedStatus(C::*M)(HANDLE)>
class CCallback {
 public:
  static CallbackFunctionPtr set(void *pInst) {
    if (pInst == NULL) {
      throw;
    }
    if (ms_pInst != NULL) {
      // TODO(framework_unified): FRAMEWORKUNIFIEDLOG some warning, preferably outside the template (i.e. use a global),
      // to limit executable size, as this code will be replicated many times over
    }
    ms_pInst = pInst;
    return &CCallback::call;
  }
  static EFrameworkunifiedStatus call(HANDLE y) {
    if (ms_pInst == NULL) {
      return eFrameworkunifiedStatusFault;  // can't throw here
    }
    C *c = static_cast<C *>(ms_pInst);
    return (c->*M)(y);
  }
 private:
  static void *ms_pInst;
};



template <typename C, EFrameworkunifiedStatus(C::*M)(HANDLE)>
void *CCallback<C, M>::ms_pInst = NULL;



template <typename C, EFrameworkunifiedStatus(C::*M)(HANDLE, E_CWORD77_ResponseType, std::string)>
class CResCallback {
 public:
  static ResponseServiceTo_CWORD77_ set(void *pInst) {
    if (pInst == NULL) {
      throw;
    }
    if (ms_pInst != NULL) {
      // TODO(framework_unified): FRAMEWORKUNIFIEDLOG some warning, preferably outside the template (i.e. use a global),
      // to limit executable size, as this code will be replicated many times over
    }
    ms_pInst = pInst;
    return &CResCallback::call;
  }
  static EFrameworkunifiedStatus call(HANDLE y, E_CWORD77_ResponseType ResponseType, std::string ServiceName) {
    if (ms_pInst == NULL) {
      return eFrameworkunifiedStatusFault;  // can't throw here
    }
    C *c = static_cast<C *>(ms_pInst);
    return (c->*M)(y, ResponseType, ServiceName);
  }
 private:
  static void *ms_pInst;
};

template <typename C, EFrameworkunifiedStatus(C::*M)(HANDLE, E_CWORD77_ResponseType, std::string)>
void *CResCallback<C, M>::ms_pInst = NULL;


#endif  // _NS__CWORD77_COMMON_H_ NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
