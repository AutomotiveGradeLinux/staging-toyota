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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  ns_utility_sys_internal.hpp
/// @brief    This file contains declaration of common APIs for NS_UtilityCenter.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
//@{

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_UTILITYCENTER_INCLUDE_NS_UTILITY_SYS_INTERNAL_HPP_
#define FRAMEWORK_UNIFIED_CLIENT_NS_UTILITYCENTER_INCLUDE_NS_UTILITY_SYS_INTERNAL_HPP_

#include <native_service/frameworkunified_types.h>
#include <pthread.h>
#include <errno.h>

#include <native_service/ns_utility.hpp>

UI_32 utility_sys_rand();  // NOLINT (readability/nolint)

// Random Functions //////////////////////////////////////////////////////////

template< class T>
T randomValue(T min, T max) {
  const int32_t lmin = static_cast< int32_t >(min);
  const int32_t range = static_cast< int32_t >(max) - lmin + 1;
  if (range > 0) {
    return static_cast< T >(utility_sys_rand() % range + lmin);
  } else {
    return min;
  }
}

template< class T, UI_32 N >
T randomItem(const T(&arr)[ N ]) {
  return arr[ randomValue< UI_32 >(0, N - 1) ];
}


template< class T, class Lock > class CLockedData;
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T, Lock > &);
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T, Lock > &,  // NOLINT (readability/nolint)
                                                                                   typename FSig< Fn >::TArg2);  // NOLINT (readability/nolint)
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T, Lock > &,
                                                                                   typename FSig< Fn >::TArg2, typename FSig< Fn >::TArg3);
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T, Lock > &,
                                                                                   typename FSig< Fn >::TArg2, typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4);
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T, Lock > &,
                                                                                   typename FSig< Fn >::TArg2, typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4, typename FSig< Fn >::TArg5);
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T, Lock > &,
                                                                                   typename FSig< Fn >::TArg2, typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4, typename FSig< Fn >::TArg5,
                                                                                   typename FSig< Fn >::TArg6);
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T, Lock > &,
                                                                                   typename FSig< Fn >::TArg2, typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4, typename FSig< Fn >::TArg5,
                                                                                   typename FSig< Fn >::TArg6, typename FSig< Fn >::TArg7);
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T, Lock > &);  // NOLINT (readability/nolint)
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T, Lock > &,
                                                                                  typename FSig< Fn >::TArg2);
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T, Lock > &,
                                                                                  typename FSig< Fn >::TArg2, typename FSig< Fn >::TArg3);
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T, Lock > &,
                                                                                  typename FSig< Fn >::TArg2, typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4);
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T, Lock > &,
                                                                                  typename FSig< Fn >::TArg2, typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4, typename FSig< Fn >::TArg5);
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T, Lock > &,
                                                                                  typename FSig< Fn >::TArg2, typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4, typename FSig< Fn >::TArg5,
                                                                                  typename FSig< Fn >::TArg6);
template< class T, class Lock, class Fn > typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T, Lock > &,
                                                                                  typename FSig< Fn >::TArg2, typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4, typename FSig< Fn >::TArg5,
                                                                                  typename FSig< Fn >::TArg6, typename FSig< Fn >::TArg7);


template< class T, class Lock >
class CWithReadLock {
 public:
  typedef const T TData;

  CWithReadLock(CLockedData< T, Lock > &);
  ~CWithReadLock();

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T1, Lock1 > &);

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T1, Lock1 > &, typename FSig< Fn >::TArg2);

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T1, Lock1 > &, typename FSig< Fn >::TArg2,
                                                 typename FSig< Fn >::TArg3);

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T1, Lock1 > &, typename FSig< Fn >::TArg2,
                                                 typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4);

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T1, Lock1 > &, typename FSig< Fn >::TArg2,
                                                 typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4, typename FSig< Fn >::TArg5);

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T1, Lock1 > &, typename FSig< Fn >::TArg2,
                                                 typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4, typename FSig< Fn >::TArg5, typename FSig< Fn >::TArg6);

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithReadLock(Fn, CLockedData< T1, Lock1 > &, typename FSig< Fn >::TArg2,
                                                 typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4, typename FSig< Fn >::TArg5, typename FSig< Fn >::TArg6,
                                                 typename FSig< Fn >::TArg7);

 private:
  DISALLOW_COPY_AND_ASSIGN(CWithReadLock);

  const TData &data() const;  // NOLINT (readability/nolint)
  CLockedData< T, Lock > &m_LockedData;
};

template< class T, class Lock >
class CWithWriteLock {
 public:
  typedef T TData;

  CWithWriteLock(CLockedData< T, Lock > &);
  ~CWithWriteLock();

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T1, Lock1 > &);

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T1, Lock1 > &, typename FSig< Fn >::TArg2);

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T1, Lock1 > &, typename FSig< Fn >::TArg2,
                                                  typename FSig< Fn >::TArg3);

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T1, Lock1 > &, typename FSig< Fn >::TArg2,
                                                  typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4);

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T1, Lock1 > &, typename FSig< Fn >::TArg2,
                                                  typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4, typename FSig< Fn >::TArg5);

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T1, Lock1 > &, typename FSig< Fn >::TArg2,
                                                  typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4, typename FSig< Fn >::TArg5, typename FSig< Fn >::TArg6);

  template< class T1, class Lock1, class Fn >
  friend typename FSig< Fn >::RType WithWriteLock(Fn, CLockedData< T1, Lock1 > &, typename FSig< Fn >::TArg2,
                                                  typename FSig< Fn >::TArg3, typename FSig< Fn >::TArg4, typename FSig< Fn >::TArg5, typename FSig< Fn >::TArg6,
                                                  typename FSig< Fn >::TArg7);

 private:
  DISALLOW_COPY_AND_ASSIGN(CWithWriteLock);

  TData &data();  // NOLINT (readability/nolint)
  CLockedData< T, Lock > &m_LockedData;
};

/// \brief Represents a piece of data protected by a lock
///
/// Data wrapped with Can only be accessed using helper functions
/// WithReadLock and WithWriteLock
///
/// Example usage:
///
/// void ModifySharedData( int& n ) { ... modify n ... };
/// int ReadSharedData( const int& n ){ ... read n ... };
///
/// void DoSomething()
/// {
///   CLocked< int > my_data( -1 );  // initialize value to -1
///   WithWriteLock( ModifySharedData, my_data );
///   int val = WithReadLock( ReadSharedData, my_data );
/// }
///
/// NOTE: that data is locked for the duration of the time that helper functions
/// ModifySharedData, and ReadSharedData are called - once they return, the data becomes
/// unlocked automatically.
template< class T, class Lock = CMutex >
class CLockedData {
 public:
  typedef CWithReadLock< T, Lock > TReadLock;
  typedef CWithWriteLock< T, Lock > TWriteLock;

  CLockedData() {}
  template< class T1 > CLockedData(T1 t1) : m_data(t1) {}
  template< class T1, class T2 > CLockedData(T1 t1, T2 t2) : m_data(t1, t2) {}
  template< class T1, class T2, class T3 > CLockedData(T1 t1, T2 t2, T3 t3) : m_data(t1, t2, t3) {}
  template< class T1, class T2, class T3, class T4 > CLockedData(T1 t1, T2 t2, T3 t3, T4 t4) : m_data(t1, t2, t3,
                                                                                                        t4) {}
  template< class T1, class T2, class T3, class T4, class T5 > CLockedData(T1 t1, T2 t2, T3 t3, T4 t4,
                                                                           T5 t5) : m_data(t1, t2, t3, t4, t5) {}
  template< class T1, class T2, class T3, class T4, class T5, class T6 > CLockedData(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5,
                                                                                     T6 t6) : m_data(t1, t2, t3, t4, t5, t6) {}
  template< class T1, class T2, class T3, class T4, class T5, class T6, class T7 > CLockedData(T1 t1, T2 t2, T3 t3,
                                                                                               T4 t4, T5 t5, T6 t6, T7 t7) : m_data(t1, t2, t3, t4, t5, t6, t7) {}

  friend class CWithReadLock< T, Lock >;
  friend class CWithWriteLock< T, Lock >;

 private:
  T m_data;
  Lock m_lock;
};

// CWithReadLock and CWithWriteLock implementation //////////////////////////////////////////////

template< class T, class Lock >
CWithReadLock< T, Lock >::CWithReadLock(CLockedData< T, Lock > &ld) : m_LockedData(ld) {
  m_LockedData.m_lock.ReadLock();
}

template< class T, class Lock >
CWithReadLock< T, Lock >::~CWithReadLock() {
  m_LockedData.m_lock.Unlock();
}

template< class T, class Lock >
const typename CWithReadLock< T, Lock >::TData &CWithReadLock< T, Lock >::data() const {  // NOLINT (readability/nolint)
  return m_LockedData.m_data;
}

template< class T, class Lock >
CWithWriteLock< T, Lock >::CWithWriteLock(CLockedData< T, Lock > &ld) :  m_LockedData(ld) {
  m_LockedData.m_lock.WriteLock();
}

template< class T, class Lock >
CWithWriteLock< T, Lock >::~CWithWriteLock() {
  m_LockedData.m_lock.Unlock();
}

template< class T, class Lock >
typename CWithWriteLock< T, Lock >::TData &CWithWriteLock< T, Lock >::data() {  // NOLINT (readability/nolint)
  return m_LockedData.m_data;
}

// WithReadLock / WithWriteLock ///////////////////////////////////////////////

template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithReadLock(Fn f, CLockedData< T, Lock > &data) {
  typename CLockedData< T, Lock >::TReadLock rl(data);
  return f(rl.data());
}

template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithReadLock(Fn f, CLockedData< T, Lock > &data, typename FSig< Fn >::TArg2 t2) {
  typename CLockedData< T, Lock >::TReadLock rl(data);
  return f(rl.data(), t2);
}

template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithReadLock(Fn f, CLockedData< T, Lock > &data, typename FSig< Fn >::TArg2 t2,
                                        typename FSig< Fn >::TArg3 t3) {
  typename CLockedData< T, Lock >::TReadLock rl(data);
  return f(rl.data(), t2, t3);
}

template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithReadLock(Fn f, CLockedData< T, Lock > &data, typename FSig< Fn >::TArg2 t2,
                                        typename FSig< Fn >::TArg3 t3, typename FSig< Fn >::TArg4 t4) {
  typename CLockedData< T, Lock >::TReadLock rl(data);
  return f(rl.data(), t2, t3, t4);
}

template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithReadLock(Fn f, CLockedData< T, Lock > &data, typename FSig< Fn >::TArg2 t2,
                                        typename FSig< Fn >::TArg3 t3, typename FSig< Fn >::TArg4 t4, typename FSig< Fn >::TArg5 t5) {
  typename CLockedData< T, Lock >::TReadLock rl(data);
  return f(rl.data(), t2, t3, t4, t5);
}

template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithReadLock(Fn f, CLockedData< T, Lock > &data, typename FSig< Fn >::TArg2 t2,
                                        typename FSig< Fn >::TArg3 t3, typename FSig< Fn >::TArg4 t4, typename FSig< Fn >::TArg5 t5,
                                        typename FSig< Fn >::TArg6 t6) {
  typename CLockedData< T, Lock >::TReadLock rl(data);
  return f(rl.data(), t2, t3, t4, t5, t6);
}

template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithReadLock(Fn f, CLockedData< T, Lock > &data, typename FSig< Fn >::TArg2 t2,
                                        typename FSig< Fn >::TArg3 t3, typename FSig< Fn >::TArg4 t4, typename FSig< Fn >::TArg5 t5,
                                        typename FSig< Fn >::TArg6 t6, typename FSig< Fn >::TArg7 t7) {
  typename CLockedData< T, Lock >::TReadLock rl(data);
  return f(rl.data(), t2, t3, t4, t5, t6, t7);
}


template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithWriteLock(Fn f, CLockedData< T, Lock > &data) {
  typename CLockedData< T, Lock >::TWriteLock wl(data);
  return f(wl.data());
}

template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithWriteLock(Fn f, CLockedData< T, Lock > &data, typename FSig< Fn >::TArg2 t2) {
  typename CLockedData< T, Lock >::TWriteLock wl(data);
  return f(wl.data(), t2);
}

template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithWriteLock(Fn f, CLockedData< T, Lock > &data, typename FSig< Fn >::TArg2 t2,
                                         typename FSig< Fn >::TArg3 t3) {
  typename CLockedData< T, Lock >::TWriteLock wl(data);
  return f(wl.data(), t2, t3);
}

template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithWriteLock(Fn f, CLockedData< T, Lock > &data, typename FSig< Fn >::TArg2 t2,
                                         typename FSig< Fn >::TArg3 t3, typename FSig< Fn >::TArg4 t4) {
  typename CLockedData< T, Lock >::TWriteLock wl(data);
  return f(wl.data(), t2, t3, t4);
}

template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithWriteLock(Fn f, CLockedData< T, Lock > &data, typename FSig< Fn >::TArg2 t2,
                                         typename FSig< Fn >::TArg3 t3, typename FSig< Fn >::TArg4 t4, typename FSig< Fn >::TArg5 t5) {
  typename CLockedData< T, Lock >::TWriteLock wl(data);
  return f(wl.data(), t2, t3, t4, t5);
}

template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithWriteLock(Fn f, CLockedData< T, Lock > &data, typename FSig< Fn >::TArg2 t2,
                                         typename FSig< Fn >::TArg3 t3, typename FSig< Fn >::TArg4 t4, typename FSig< Fn >::TArg5 t5,
                                         typename FSig< Fn >::TArg6 t6) {
  typename CLockedData< T, Lock >::TWriteLock wl(data);
  return f(wl.data(), t2, t3, t4, t5, t6);
}

template< class T, class Lock, class Fn >
typename FSig< Fn >::RType WithWriteLock(Fn f, CLockedData< T, Lock > &data, typename FSig< Fn >::TArg2 t2,
                                         typename FSig< Fn >::TArg3 t3, typename FSig< Fn >::TArg4 t4, typename FSig< Fn >::TArg5 t5,
                                         typename FSig< Fn >::TArg6 t6, typename FSig< Fn >::TArg7 t7) {
  typename CLockedData< T, Lock >::TWriteLock wl(data);
  return f(wl.data(), t2, t3, t4, t5, t6, t7);
}


#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_UTILITYCENTER_INCLUDE_NS_UTILITY_SYS_INTERNAL_HPP_
//@}
