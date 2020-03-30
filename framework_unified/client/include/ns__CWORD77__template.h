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

////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_HMI_CWORD77_Controller
/// \brief
///
////////////////////////////////////////////////////////////////////////////////
//@{

#ifndef FRAMEWORK_UNIFIED_CLIENT_INCLUDE_NS__CWORD77__TEMPLATE_H_
#define FRAMEWORK_UNIFIED_CLIENT_INCLUDE_NS__CWORD77__TEMPLATE_H_

#include <iterator>
#include <algorithm>

/// Template for Map range transformation
template< class OCont, class IIter, class MapFn >
OCont mapRange(const IIter &begin, const IIter &end, MapFn fn) {
  OCont c;
  std::transform(begin, end, std::inserter(c, c.begin()), fn);
  return c;
}


/// Template to get Array begining
template< class T, UI_32 N >
const T *ArrBeg(const T(&arr)[ N ]) {
  return &arr[ 0 ];
}

/// Template to get Array ending
template< class T, UI_32 N >
const T *ArrEnd(const T(&arr)[ N ]) {
  return &arr[ 0 ] + N;
}

/// Template to convert structure to Map
template< class Cont, class T, class C, class TFn >
const Cont GetMap(const T *mapTable, C N, TFn transform) {
  return mapRange< Cont >(&mapTable[0], &mapTable[0] + N, transform);
}

ServiceResponseTableItem TransformServiceResponseTableIteminMap(const ServiceResponse &uch);

#endif  // FRAMEWORK_UNIFIED_CLIENT_INCLUDE_NS__CWORD77__TEMPLATE_H_
//@}
