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

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_FRAMEWORK_UTILITY_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_FRAMEWORK_UTILITY_H_

#include <native_service/ns_utility.hpp>
#include "frameworkunified_framework_internal.h"

DEFINE_EXCEPTION(happ_error, std::runtime_error);

class HAppTraits {
 public:
  typedef HANDLE Type;
  typedef happ_error Exception;
  static void Release(Type rsrc) {
    FrameworkunifiedOnDestroyInternal(rsrc);
    FrameworkunifiedCloseDispatcher(rsrc);
  }

  static BOOL BadValue(Type rsrc) {
    return NULL == rsrc;
  }
};

typedef ResourceMgr< HANDLE, HAppTraits > THApp;

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_FRAMEWORK_UTILITY_H_
