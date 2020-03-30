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
/**
 * @file frameworkunified_framework_error.hpp
 * @brief \~english This file provides framework's API which are used by system services.
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
/** @addtogroup framework
 *  @ingroup native_service
 *  @{
 */
#ifndef __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_ERROR_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_ERROR_H__

#include <native_service/frameworkunified_types.h>
#include <stdexcept>

namespace frameworkunified {
namespace framework {
namespace error {

class error : public std::runtime_error {
 public:
  explicit error(EFrameworkunifiedStatus error, PCSTR errorMsg);
 private:
  const EFrameworkunifiedStatus m_error;
};

}
}
}

#define DEFINE_FRAMEWORKUNIFIED_ERROR( name ) \
class name : public framework_unified::framework::error::error \
{ \
public: \
  explicit name ( EFrameworkunifiedStatus error, PCSTR str ) \
    : framework_unified::framework::error::error ( error, str ) \
  {} \
}

#endif  // __NATIVESERVICES_FRAMEWORK_FRAMEWORKUNIFIED_FRAMEWORK_ERROR_H__ NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
