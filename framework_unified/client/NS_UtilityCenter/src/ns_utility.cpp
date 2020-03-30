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

#include <native_service/ns_utility_if.h>
#include <native_service/ns_utility_sys.hpp>
#include <stdlib.h>
#include <ctime>
#include <cstring>
#include "ns_utility_sys_internal.hpp"
#include "ns_utility_if_internal.h"
// Random functions ////////////////////////////////////////////////////
void utility_sys_seedrand() {  // NOLINT (readability/nolint)
  static bool s_seeded = false;
  if (!s_seeded) {
    s_seeded = true;
    srand48(std::time(NULL));
  }
}

UI_32 utility_sys_rand() {  // NOLINT (readability/nolint)
  utility_sys_seedrand();
  return static_cast< UI_32 >(mrand48());
}

// Lock wrapper implementation /////////////////////////////////////

///////////////////////////////////////////
// Implements locking policy for mutexes //
///////////////////////////////////////////

// CMutex helper function
template< class TErr >
void MutexLock(pthread_mutex_t *mtx) {
  if (EOK != pthread_mutex_lock(mtx)) {
    throw TErr("");
  }
}

CMutex::CMutex() {
  pthread_mutex_t mtx_tmp = PTHREAD_MUTEX_INITIALIZER;
  std::memcpy(&m_mtx, &mtx_tmp, sizeof(m_mtx));
  if (EOK != pthread_mutex_init(&m_mtx, NULL)) {
    throw lock_creation_error();
  }
}

CMutex::~CMutex() {
  pthread_mutex_destroy(&m_mtx);
}

void CMutex::ReadLock() {
  MutexLock< lock_acquireread_error >(&m_mtx);
}

void CMutex::WriteLock() {
  MutexLock< lock_acquirewrite_error >(&m_mtx);
}

void CMutex::Unlock() {
  if (EOK != pthread_mutex_unlock(&m_mtx)) {
    throw lock_release_error("");
  }
}


/////////////////////////////////////////////////////////
// implements locking policy for reader / writer locks //
/////////////////////////////////////////////////////////
CRWLock::CRWLock() {
  pthread_rwlock_t rw_tmp = PTHREAD_RWLOCK_INITIALIZER;
  std::memcpy(&m_rwl, &rw_tmp, sizeof(m_rwl));
  if (EOK != pthread_rwlock_init(&m_rwl, NULL)) {
    throw lock_creation_error();
  }
}

CRWLock::~CRWLock() {
  pthread_rwlock_destroy(&m_rwl);
}

void CRWLock::ReadLock() {
  if (EOK != pthread_rwlock_rdlock(&m_rwl)) {
    throw lock_acquireread_error();
  }
}

void CRWLock::WriteLock() {
  if (EOK != pthread_rwlock_wrlock(&m_rwl)) {
    throw lock_acquirewrite_error();
  }
}

void CRWLock::Unlock() {
  if (EOK != pthread_rwlock_unlock(&m_rwl)) {
    throw lock_release_error();
  }
}

/////////////////////////////////////////////////////////
// implements GNU Builtins for counting leading zeros  //
/////////////////////////////////////////////////////////

// returns the number of leading zeros in a given 16bit value
SI_16 NS_CountLeadingZeros_16Bit(UI_16 f_ui_val) {  // NOLINT (readability/nolint)
  return static_cast<SI_16>((f_ui_val == 0) ? NS_INVALID_RETURN : __builtin_clz(f_ui_val));  // NOLINT (readability/nolint)
}

// returns the number of leading zeros in a given 32bit value
SI_16 NS_CountLeadingZeros_32Bit(UI_32 f_ui_val) {  // NOLINT (readability/nolint)
  return static_cast<SI_16>((f_ui_val == 0) ? NS_INVALID_RETURN : __builtin_clzl(f_ui_val));  // NOLINT (readability/nolint)
}

// returns the number of leading zeros in a given 64bit value
SI_16 NS_CountLeadingZeros_64Bit(UI_64 f_ui_val) {  // NOLINT (readability/nolint)
  return static_cast<SI_16>((f_ui_val == 0) ? NS_INVALID_RETURN : __builtin_clzll(f_ui_val));  // NOLINT (readability/nolint)
}

// returns the number of trailing zeros in a given 16bit value
SI_16 NS_CountTrailingZeros_16Bit(UI_16 f_ui_val) {  // NOLINT (readability/nolint)
  return static_cast<SI_16>((f_ui_val == 0) ? NS_INVALID_RETURN : __builtin_ctz(f_ui_val));  // NOLINT (readability/nolint)
}

// returns the number of trailing zeros in a given 32bit value
SI_16 NS_CountTrailingZeros_32Bit(UI_32 f_ui_val) {  // NOLINT (readability/nolint)
  return static_cast<SI_16>((f_ui_val == 0) ? NS_INVALID_RETURN : __builtin_ctzl(f_ui_val));  // NOLINT (readability/nolint)
}

// returns the number of trailing zeros in a given 64bit value
SI_16 NS_CountTrailingZeros_64Bit(UI_64 f_ui_val) {  // NOLINT (readability/nolint)
  return static_cast<SI_16>((f_ui_val == 0) ? NS_INVALID_RETURN : __builtin_ctzll(f_ui_val));  // NOLINT (readability/nolint)
}
