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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup tag_NS_NPPService
/// \ingroup  tag_NS_NPPService
///
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_NPPService
/// \brief    This class holds the message queue handles for notification subscribers
///
/// This is a singleton class which holds the message queue handles for notification subscribers.
/// This will ensure that NS_NPPService will hold only one handle per subscriber.
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <utility>
#include <string>
#include "ns_npp_handlelist.h"
#include "ns_npp_notificationpersistentservicelog.h"

// define static member of class CHandleList
CHandleList *CHandleList::m_psHandleList = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CHandleList
/// Class Constructor
////////////////////////////////////////////////////////////////////////////////////////////////////
CHandleList::CHandleList() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CHandleList
/// Class Destructor
////////////////////////////////////////////////////////////////////////////////////////////////////
CHandleList::~CHandleList() {   // LCOV_EXCL_START 14: Resident process, global instance not released
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetHandleList
/// This function is used to get the singleton instance of class.
////////////////////////////////////////////////////////////////////////////////////////////////////
CHandleList *CHandleList::GetHandleList() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // create the instance of class
  if (NULL == m_psHandleList) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Creating handle list.");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    m_psHandleList = new(std::nothrow) CHandleList();  // LCOV_EXCL_BR_LINE 11: unexpected branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return m_psHandleList;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ReleaseHandleList
/// This function is used to release the instance of class.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CHandleList::ReleaseHandleList() {   // LCOV_EXCL_START 100: not used
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != m_psHandleList) {
    delete m_psHandleList;
    m_psHandleList = NULL;
  } else {
    l_estatus = eFrameworkunifiedStatusNullPointer;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////
/// AddHandleInList
/// Add pair of subscriber name and corresponding handle in map.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CHandleList::AddHandleInList(std::string f_csubscribername, HANDLE f_hmqhandle) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  HandleListRetStatus_Type l_prRetValue;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_prRetValue = m_mHandleList.insert(make_pair(f_csubscribername, f_hmqhandle));
  if (false == l_prRetValue.second) {
    l_estatus = eFrameworkunifiedStatusDuplicate;
    FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Handle exists in list for %s.", f_csubscribername.c_str());
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Handle added in list for %s.", f_csubscribername.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }


  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}


////////////////////////////////////////////////////////////////////////////////////////////////
/// RemoveHandleFromList
/// Remove handle for subscriber from the list.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CHandleList::RemoveHandleFromList(std::string f_csubscribername) {   // LCOV_EXCL_START 100: not used
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  // TODO(my_username): Implementation of this function.
  // NOTE: Right now this feature is not required. Let the NS_NPPService hold one handle for
  //    each subscriber, even if all the notifications are unsubscribed by subscriber.
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////
/// GetSubscriberMqHandle
/// Get the message queue handle of a subscriber.
////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE CHandleList::GetSubscriberMqHandle(std::string f_csubscribername) {
  HANDLE l_hMqHandle = NULL;
  HandleList_Type::iterator l_itHandleList;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_itHandleList = m_mHandleList.find(f_csubscribername);
  if (l_itHandleList != m_mHandleList.end()) {
    l_hMqHandle = l_itHandleList->second;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_hMqHandle;
}
