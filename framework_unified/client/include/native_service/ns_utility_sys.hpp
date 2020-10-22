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

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief    This file contains declaration of common APIs for NS_UtilityCenter.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @file ns_utility_sys.hpp
 */

#ifndef __NSFRAMEWORK_NSUTILITY_NSUTILITYSYS__  // NOLINT  (build/header_guard)
#define __NSFRAMEWORK_NSUTILITY_NSUTILITYSYS__

#include <native_service/frameworkunified_types.h>
#include <native_service/ns_utility.hpp>
#include <pthread.h>
#include <errno.h>

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
/** @addtogroup native
 *  @ingroup framework_unified
 *  @{
 */

// Lock Wrapper ////////////////////////////////////////////////////////////////

/**
 * \~english define the lock_error class inherit from std::runtime_error
 */
DEFINE_EXCEPTION(lock_error, std::runtime_error);

/**
 * \~english define the lock_creation_error class inherit from lock_error
 */
DEFINE_EXCEPTION(lock_creation_error, lock_error);

/**
 * \~english define the lock_acquireread_error class inherit from lock_error
 */
DEFINE_EXCEPTION(lock_acquireread_error, lock_error);

/**
 * \~english define the lock_acquirewrite_error class inherit from lock_error
 */
DEFINE_EXCEPTION(lock_acquirewrite_error, lock_error);

/**
 * \~english define the lock_release_error class inherit from lock_error
 */
DEFINE_EXCEPTION(lock_release_error, lock_error);


/**
 * @class CMutex
 * @brief \~english Mutex Lock Class
 * @par   \~english Brief Introduction
 * \~english This class defines locking policy for Mutexes.
 *
 */
class CMutex {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Construct a CMutex object.
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function constructs a CMutex object.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CMutex();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Destruct the CMutex object.
  /// \~english @par Prerequisite
  ///       - Construct a CMutex object
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function destructs the CMutex object.
  /// \~english @see CMutex()
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CMutex();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Lock the CMutex object when reading.
  /// \~english @retval void
  ///
  /// \~english @par Prerequisite
  ///       - Construct a CMutex object
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function locks the CMutex object when reading.
  /// \~english @see CMutex(), WriteLock(), UnLock()
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void ReadLock();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Lock the CMutex object when writing.
  /// \~english @retval void
  ///
  /// \~english @par Prerequisite
  ///       - Construct a CMutex object
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function locks the CMutex object when writing.
  /// \~english @see CMutex(), ReadLock(), UnLock()
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void WriteLock();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Open the lock of the CMutex object.
  /// \~english @retval void
  ///
  /// \~english @par Prerequisite
  ///       - Construct a CMutex object
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function opens the lock of the CMutex object.
  /// \~english @see CMutex(), ReadLock(), WriteLock()
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void Unlock();

 private:

  pthread_mutex_t m_mtx;
};

/**
 * @class CRWLock
 * @brief \~english Read/Write Lock Class
 * @par   \~english Brief Introduction
 * \~english This class defines locking policy for reader/writer.
 *
 */
class CRWLock {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       - Construct a CRWLock object.
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function constructs a CRWLock object.
  /// \~english @see ~CRWLock()
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CRWLock();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Destruct the CRWLock object.
  /// \~english @par Prerequisite
  ///       - Construct a CRWLock object.
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function destructs the CRWLock object.
  /// \~english @see CRWLock()
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CRWLock();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Lock the CRWLock object when reading.
  /// \~english @retval void
  ///
  /// \~english @par Prerequisite
  ///       - Construct a CRWLock object.
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function locks the CRWLock object when reading.
  /// \~english @see CRWLock(), WriteLock(), UnLoack()
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void ReadLock();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Lock the CRWLock object when writing.
  /// \~english @retval void
  ///
  /// \~english @par Prerequisite
  ///       - Construct a CRWLock object.
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function locks the CRWLock object when writing.
  /// \~english @see CRWLock(), ReadLock(), UnLock()
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void WriteLock();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Open the lock of CRWLock object
  /// \~english @retval void
  ///
  /// \~english @par Prerequisite
  ///       - Construct a CRWLock object.
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function opens the lock of the CRWLock object.
  /// \~english @see CRWLock(), ReadLock(), WriteLock()
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void Unlock();

 private:
  pthread_rwlock_t m_rwl;
};

#endif  // __NSFRAMEWORK_NSUTILITY_NSUTILITYSYS__  // NOLINT  (build/header_guard)
/** @}*/  // end of native
/** @}*/  // end of framework_unified
/** @}*/  // end of native_service
/** @}*/  // end of BaseSystem
