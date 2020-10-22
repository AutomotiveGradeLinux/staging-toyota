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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
/// \brief
///       This file has class declaration of the CFrameworkunifiedAction. This is an interface class that defines
///     interfaces for implementing action
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_sm_eventdata.h
 * @brief \~english This file has class declaration of the CFrameworkunifiedAction. This is an interface class that defines
 *          interfaces for implementing action
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
 *  @ingroup framework_unified
 *  @{
 */
/** @addtogroup statemachine
 *  @ingroup framework
 *  @{
 */
#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NSFRAMEWORKCORE_STATEMACHINE_INC_FRAMEWORKUNIFIEDSMEVENTDATA_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NSFRAMEWORKCORE_STATEMACHINE_INC_FRAMEWORKUNIFIEDSMEVENTDATA_H__

#include <native_service/frameworkunified_types.h>
#include <native_service/ns_logger_if.h>
#include <boost/smart_ptr.hpp>


class CEventData {
 public:
  CEventData(UI_32 f_uiEventId): m_uiEventId(f_uiEventId) {  // NOLINT  (readability/nolint)
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "CEventData parameterized constructor");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  virtual ~CEventData() {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "CEventData destructor");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  // event id
  UI_32 m_uiEventId;

 private:
  CEventData() {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "CEventData constructor");
  }
};

#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_NSFRAMEWORKCORE_STATEMACHINE_INC_FRAMEWORKUNIFIEDSMEVENTDATA_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
