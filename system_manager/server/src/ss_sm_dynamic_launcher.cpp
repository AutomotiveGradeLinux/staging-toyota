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

#include <string>
#include <sstream>
#include <iomanip>

#include "ss_sm_dynamic_launch_table.h"
#include "ss_sm_systemmanagerlog.h"

DynamicLaunchBase::DynamicLaunchBase() noexcept:
m_vOrderedGroup(),
m_iterOrderedGroup() {
}


DynamicLaunchBase::~DynamicLaunchBase() {
}

UI_32 DynamicLaunchBase::get_id() const {
    // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " DLB: Active Group ID is %d", *m_iterOrderedGroup);
    return *m_iterOrderedGroup;
}

UI_32 DynamicLaunchBase::advance_id() noexcept {
  if (!is_end()) {
    m_iterOrderedGroup++;
  }
  // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " DLB: Active Group ID is %d", *m_iterOrderedGroup);
  return *m_iterOrderedGroup;
}

UI_32 DynamicLaunchBase::decrement_id() noexcept {
  if (!is_begin()) {  // LCOV_EXCL_BR_LINE 5: stdlib error case.
    m_iterOrderedGroup--;
  }
  // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " DLB: Active Group ID is %d", *m_iterOrderedGroup);
  return *m_iterOrderedGroup;
}

BOOL DynamicLaunchBase::is_begin() {
  // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " DLB: Active Group ID is %d", *m_iterOrderedGroup);
  return (m_iterOrderedGroup == m_vOrderedGroup.begin());
}

BOOL DynamicLaunchBase::is_end() {
  // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " DLB: Active Group ID is %d", *m_iterOrderedGroup);
  DynamicOrderedIter l_it = m_iterOrderedGroup;
  l_it++;
  return (l_it == m_vOrderedGroup.end());
}

VOID DynamicLaunchBase::to_begin() {
  m_iterOrderedGroup = m_vOrderedGroup.begin();
  return;
}

VOID DynamicLaunchBase::to_end() {
  m_iterOrderedGroup = m_vOrderedGroup.end();
  m_iterOrderedGroup--;
  return;
}

VOID DynamicLaunchBase::print_info() noexcept {
  UI_32 idx = 1;
  std::stringstream l_logMsg;
  DynamicOrderedIter l_dyn_iter = m_vOrderedGroup.begin();
  for (; l_dyn_iter != m_vOrderedGroup.end(); l_dyn_iter++) {
    l_logMsg << std::endl << "Index: " << std::setw(2) << idx
            << " Group Id: " << std::setw(2) << *l_dyn_iter;

    idx++;
  }

  std::string l_logStr = l_logMsg.str();
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s", l_logStr.c_str());
}

DynamicLauncher::DynamicLauncher() noexcept:DynamicLaunchBase(),
m_nLaunchGroupId(0),
m_nLaunchBeforeGroupId(0),
m_nLaunchAfterGroupId(0),
m_nAviageTriggerId(0),
m_bDynamicLaunchEnabled(FALSE),  // by default until set by some one will this be disabled.
m_eSourceType(SS_SOURCE_NA) {  // invalid
}

DynamicLauncher::~DynamicLauncher() {
}

BOOL DynamicLauncher::dynamic_launch_enabled() const {  // LCOV_EXCL_START 8: Dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return m_bDynamicLaunchEnabled;
}
// LCOV_EXCL_STOP

VOID DynamicLauncher::configure_dynamic_launch(const SS_SOURCE & source_type,
        GroupLaunchMap & group_info, LaunchOrderedVector & order_info) noexcept {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  m_eSourceType = source_type;
  // "Dynamic Launch" is not used due to high dependency on a service table
  create_normal_launch_vector(group_info, order_info);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

BOOL DynamicLauncher::is_inGroupLaunchMap(UI_32& id,
      GroupLaunchMap & group_info) noexcept {  // LCOV_EXCL_START 8: Dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL Ret = FALSE;
  GroupLaunchMapIter l_grp_iter = group_info.find(id);
  if (l_grp_iter != group_info.end()) {
      Ret = TRUE;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return Ret;
}
// LCOV_EXCL_STOP

DynamicLaunchInfo * DynamicLauncher::get_dynamic_source_info(
        const SS_SOURCE & source_type, BOOL &source_trigger,
        BOOL &is_aviage_required) noexcept {  // LCOV_EXCL_START 8: Dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  UI_32 num_of_items = static_cast<UI_32>(_countof(g_arrDynamicLaunchTableCfg));
  DynamicLaunchInfo * point_to_this = NULL;
  for (UI_32 arr_idx = 0; arr_idx < num_of_items; arr_idx++) {
    if (g_arrDynamicLaunchTableCfg[arr_idx].source == source_type) {
      source_trigger = g_arrDynamicLaunchTableCfg[arr_idx].group_trigger;
      point_to_this = &g_arrDynamicLaunchTableCfg[arr_idx];
      if ((SS_SOURCE_FM == source_type) || (SS_SOURCE_AM == source_type)
            || (SS_SOURCE_FM_DAB == source_type)
            || (SS_SOURCE_SDARS == source_type)) {
        is_aviage_required = FALSE;
      }
      break;
    }
  }

  return point_to_this;
}
// LCOV_EXCL_STOP

UI_32 DynamicLauncher::get_id_using_group_name(PCSTR group_name,
        GroupLaunchMap & group_info) noexcept {  // LCOV_EXCL_START 8: Dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    GroupLaunchMapIter l_grp_iter = group_info.begin();
    UI_32 rtn_grp_id = 0;
    for (; l_grp_iter != group_info.end(); l_grp_iter++) {
        if (0 == strcmp(group_name, l_grp_iter->second.name.c_str())) {
            rtn_grp_id = l_grp_iter->second.id;
            break;
        }
    }

    return rtn_grp_id;
}
// LCOV_EXCL_STOP

VOID DynamicLauncher::create_normal_launch_vector(GroupLaunchMap & group_info,
        LaunchOrderedVector & order_info) noexcept {
  m_vOrderedGroup.clear();

  if (order_info.size() != 0) {  // LCOV_EXCL_BR_LINE 6: To initialize conditions for launching SystemManager
    // LCOV_EXCL_START 6: To initialize conditions for launching SystemManager
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LaunchOrderedIter l_ord_iter = order_info.begin();
    while (l_ord_iter != order_info.end()) {
      if (is_inGroupLaunchMap(*l_ord_iter, group_info) == TRUE) {
        m_vOrderedGroup.push_back(*l_ord_iter);
      }
      l_ord_iter++;
    }
    // LCOV_EXCL_STOP
  } else {
    GroupLaunchMapIter l_grp_iter = group_info.begin();
    while (l_grp_iter != group_info.end()) {
      m_vOrderedGroup.push_back(l_grp_iter->second.id);
      l_grp_iter++;
    }
  }
  m_iterOrderedGroup = m_vOrderedGroup.begin();
  m_bDynamicLaunchEnabled = TRUE;
  print_info();
}

VOID DynamicLauncher::copyDynOrderedVector(DynamicModuleStart & Start) noexcept {
  Start.setGroupVector(this->m_vOrderedGroup);
}

// DynamicModuleStart

DynamicModuleStart::DynamicModuleStart() noexcept:DynamicLaunchBase() {
}

DynamicModuleStart::~DynamicModuleStart() {
}

VOID DynamicModuleStart::setGroupVector(DynamicOrderedVector & Vector) noexcept {
    m_vOrderedGroup.assign(Vector.begin(), Vector.end());
    m_iterOrderedGroup = m_vOrderedGroup.begin();
}  // LCOV_EXCL_BR_LINE 10: Final line
