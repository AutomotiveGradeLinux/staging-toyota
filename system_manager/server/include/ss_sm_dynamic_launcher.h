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
/// \ingroup  tag_SystemManager
/// \brief    This file provides support for System Manager process launching.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef SS_SM_DYNAMIC_LAUNCHER_H_  // NOLINT
#define SS_SM_DYNAMIC_LAUNCHER_H_

#include <native_service/frameworkunified_types.h>
#include <system_service/interfaceunified_system_types.h>
#include <vector>

#include "ss_sm_config.h"

using std::vector;


typedef struct _tDynamicLaunchParams {
  SS_SOURCE source;
  const PCHAR source_name;
  const PCHAR before_group_name;
  const PCHAR start_group_name;
  const PCHAR after_group_name;
  const PCHAR aviage_trigger_name;
  BOOL  group_trigger;
} DynamicLaunchInfo;


class DynamicLaunchBase {
 public:
  DynamicLaunchBase() noexcept;
  virtual ~DynamicLaunchBase();
  UI_32 get_id() const;
  UI_32 advance_id() noexcept;
  UI_32 decrement_id() noexcept;
  BOOL  is_begin();
  BOOL  is_end();
  VOID  to_begin();
  VOID  to_end();
  VOID  print_info() noexcept;

 protected:
  typedef std::vector<UI_32> DynamicOrderedVector;
  typedef DynamicOrderedVector::iterator DynamicOrderedIter;
  DynamicOrderedVector  m_vOrderedGroup;
  DynamicOrderedIter    m_iterOrderedGroup;
};

class DynamicModuleStart;

class DynamicLauncher: public  DynamicLaunchBase {
 public:
  DynamicLauncher() noexcept;
  virtual ~DynamicLauncher();
  BOOL  dynamic_launch_enabled() const;
  VOID  configure_dynamic_launch(const SS_SOURCE & source_type,  // NOLINT
                                 GroupLaunchMap & group_info, LaunchOrderedVector & order_info) noexcept;  // NOLINT
  VOID  copyDynOrderedVector(DynamicModuleStart &) noexcept;

 private:
  // Disable class copy constructors
  DynamicLauncher(const DynamicLauncher &) noexcept;
  DynamicLauncher & operator = (const DynamicLauncher &);

  // helper methods..
  DynamicLaunchInfo * get_dynamic_source_info(const SS_SOURCE & source_type,  // NOLINT
                                              BOOL &source_trigger, BOOL &is_aviage_required) noexcept;  // NOLINT
  UI_32 get_id_using_group_name(PCSTR group_name, GroupLaunchMap & group_info) noexcept;  // NOLINT
  VOID create_normal_launch_vector(GroupLaunchMap & group_info, LaunchOrderedVector & order_info) noexcept;  // NOLINT
  BOOL is_inGroupLaunchMap(UI_32& id, GroupLaunchMap & group_info) noexcept;  // NOLINT

  // Data Members
  UI_32 m_nLaunchGroupId;
  UI_32 m_nLaunchBeforeGroupId;
  UI_32 m_nLaunchAfterGroupId;
  UI_32 m_nAviageTriggerId;
  BOOL  m_bDynamicLaunchEnabled;
  SS_SOURCE m_eSourceType;
};

class DynamicModuleStart: public DynamicLaunchBase {
 public:
  DynamicModuleStart() noexcept;
  virtual ~DynamicModuleStart() noexcept;
  VOID setGroupVector(DynamicOrderedVector &) noexcept;
};

#endif  // SS_SM_DYNAMIC_LAUNCHER_H_  // NOLINT
