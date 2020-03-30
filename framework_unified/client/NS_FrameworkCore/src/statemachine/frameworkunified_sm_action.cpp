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

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
/// \brief
///       This file has class declaration of the CFrameworkunifiedAction. This is an interface class that defines
///     interfaces for implementing action
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_action.h>
#include <string>
///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedAction
/// Parameterized constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedAction::CFrameworkunifiedAction(std::string f_strName): m_strName(f_strName) {
}
