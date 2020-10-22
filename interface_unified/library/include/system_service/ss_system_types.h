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
/// \ingroup  tag_SystemServicesIf
/// \brief    This file supports string vector type definitions.
///
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 */
#ifndef SS_SYSTEM_TYPES_H_  // NOLINT (build/header_guard)
#define SS_SYSTEM_TYPES_H_

#include <vector>
#include <string>


typedef std::string         SS_String;
typedef std::vector< std::string > StringList;
typedef std::vector< std::string >::iterator StringListIter;

#endif /* SS_SYSTEM_TYPES_H_ */  // NOLINT (build/header_guard)
