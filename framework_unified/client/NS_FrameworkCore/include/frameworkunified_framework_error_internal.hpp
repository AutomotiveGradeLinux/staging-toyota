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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
/// \brief    This file provides framework's API which are used by system services.
///////////////////////////////////////////////////////////////////////////////
//@{

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_FRAMEWORK_ERROR_INTERNAL_HPP_
#define FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_FRAMEWORK_ERROR_INTERNAL_HPP_

#include <stdexcept>
#include <native_service/frameworkunified_types.h>

namespace frameworkunified {
namespace framework {
namespace error {

class CSystemError : public std::runtime_error {
 public:
  explicit CSystemError(EFrameworkunifiedSystemError error, PCSTR errorMsg);
  const EFrameworkunifiedSystemError m_eSystemError;
};
}
}
}

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_FRAMEWORKCORE_INCLUDE_FRAMEWORKUNIFIED_FRAMEWORK_ERROR_INTERNAL_HPP_
//@}
