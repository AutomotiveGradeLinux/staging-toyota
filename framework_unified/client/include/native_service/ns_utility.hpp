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
/// \ingroup  tag_NS_UtilityCenter
/// \brief    This file contains declaration of common APIs for NS_UtilityCenter.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file ns_utility.hpp
 * @brief \~english This file contains declaration of common APIs for NS_UtilityCenter.
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
/** @addtogroup native
 *  @ingroup framework_unified
 *  @{
 */
#ifndef __NSFRAMEWORK_NSUTILITY_NSUTILITY__  // NOLINT  (build/header_guard)
#define __NSFRAMEWORK_NSUTILITY_NSUTILITY__

#include <native_service/frameworkunified_types.h>
#include <new>
#include <exception>
#include <algorithm>
#include <iterator>

#ifdef AGL_STUB
#include <stdexcept>
#endif

// Macros ///////////////////////////////////////////////////////////////

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName& )

#define DEFINE_EXCEPTION( name, base ) \
class name : public base \
{ \
public: \
  name ( const char* str = "" ) : base ( str ) \
  {} \
}

// Helpful type definitions ////////////////////////////////////////////////
template <int v>
struct Int2Type {
  enum { value = v };  // NOLINT (readability/nolint)
};

template <class T>
struct Type2Type {
  typedef T Type;
};

// Helpful Fucntions /////////////////////////////////////////////////////////
template< class T, UI_32 N >
const T *ArrBeg(const T(&arr)[ N ]) {
  return &arr[ 0 ];
}

template< class T, UI_32 N >
const T *ArrEnd(const T(&arr)[ N ]) {
  return &arr[ 0 ] + N;
}

template< class O, class I >
O SimpleCast(I i) {
  return static_cast< O >(i);
}

template< class R, class B >
R UnalignedRet(B *b) {
  return *reinterpret_cast< R volatile * >(b);
}

template< class Cont, class Gen >
Cont genRange(UI_32 N, Gen genFn) {
  Cont c;
  std::generate_n(std::back_inserter(c), N, genFn);
  return c;
}

template< class OCont, class IIter, class MapFn >
OCont mapRange(const IIter &begin, const IIter &end, MapFn fn) {
  OCont c;
  std::transform(begin, end, std::inserter(c, c.begin()), fn);
  return c;
}
// Static Compile-time list creation /////////////////////////////////////////

#ifdef AGL_STUB
#else
////////////////////////
#include <_pack1.h>    //
//////////////////////////
#endif

template< class T, UI_32 N >
struct TList {
  T datum;
  TList < T, N - 1 > data;

  T *operator&() {  // NOLINT (readability/nolint)
    return reinterpret_cast< T * >(this);
  }
  const T *operator&() const {  // NOLINT (readability/nolint)
    return reinterpret_cast< const T * >(this);
  }
  static const UI_32 count = N;  // NOLINT (readability/nolint)

};

template< class T >
struct TList< T, 1 > {
  T datum;

  T *operator&() {  // NOLINT (readability/nolint)
    return reinterpret_cast< T * >(this);
  }
  const T *operator&() const {  // NOLINT (readability/nolint)
    return reinterpret_cast< const T * >(this);
  }
  static const UI_32 count = 1;  // NOLINT (readability/nolint)

};

#ifdef AGL_STUB
#else
/////////////////////////
#include <_packpop.h> //
///////////////////////
#endif

// Function Decomposition ///////////////////////////////////////////////////

template< class Sig >
struct FSig {
};

template<class R>
struct FSig< R(*)() > {
  typedef R RType;
  static const UI_32 argCount = 0;  // NOLINT (readability/nolint)
};

template<class R, class T1>
struct FSig< R(*)(T1) > {
  typedef R RType;
  typedef T1 TArg1;
  static const UI_32 argCount = 1;  // NOLINT (readability/nolint)
};

template<class R, class T1, class T2>
struct FSig< R(*)(T1, T2) > {
  typedef R RType;
  typedef T1 TArg1;
  typedef T2 TArg2;
  static const UI_32 argCount = 2;  // NOLINT (readability/nolint)
};

template<class R, class T1, class T2, class T3>
struct FSig< R(*)(T1, T2, T3) > {
  typedef R RType;
  typedef T1 TArg1;
  typedef T2 TArg2;
  typedef T3 TArg3;
  static const UI_32 argCount = 3;  // NOLINT (readability/nolint)
};

template<class R, class T1, class T2, class T3, class T4>
struct FSig< R(*)(T1, T2, T3, T4) > {
  typedef R RType;
  typedef T1 TArg1;
  typedef T2 TArg2;
  typedef T3 TArg3;
  typedef T4 TArg4;
  static const UI_32 argCount = 4;  // NOLINT (readability/nolint)
};

template<class R, class T1, class T2, class T3, class T4, class T5>
struct FSig< R(*)(T1, T2, T3, T4, T5) > {
  typedef R RType;
  typedef T1 TArg1;
  typedef T2 TArg2;
  typedef T3 TArg3;
  typedef T4 TArg4;
  typedef T5 TArg5;
  static const UI_32 argCount = 5;  // NOLINT (readability/nolint)
};

template<class R, class T1, class T2, class T3, class T4, class T5, class T6>
struct FSig< R(*)(T1, T2, T3, T4, T5, T6) > {
  typedef R RType;
  typedef T1 TArg1;
  typedef T2 TArg2;
  typedef T3 TArg3;
  typedef T4 TArg4;
  typedef T5 TArg5;
  typedef T6 TArg6;
  static const UI_32 argCount = 6;  // NOLINT (readability/nolint)
};

template<class R, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct FSig< R(*)(T1, T2, T3, T4, T5, T6, T7) > {
  typedef R RType;
  typedef T1 TArg1;
  typedef T2 TArg2;
  typedef T3 TArg3;
  typedef T4 TArg4;
  typedef T5 TArg5;
  typedef T6 TArg6;
  typedef T7 TArg7;
  static const UI_32 argCount = 7;  // NOLINT (readability/nolint)
};



// Accumulator definition //////////////////////////////////////////////////

/// \brief Accumultor type
///
/// An accumulator is an object that keeps an internal counter, and will increment its
/// counter by n, which defaults to 1, each time operator() is called on it.  Eample usage:
///
/// Accumulator< int > a( 42 );  // initialize a to 42
/// int v1 = a();   // <-- v1 == 43
/// int v2 = a();   // <-- v2 == 44
/// int v4 = a(10);  // <-- v4 == 54
/// int v5 = a();   // <-- v5 == 55
template<typename T>
class Accumulator {
 public:
  Accumulator(T n) : n(n) {}

  template<typename U>
  Accumulator(const Accumulator<U> &u) : n(u.n) {}

  T operator()(T i = 1) {  // NOLINT (readability/nolint)
    return operator()< T >(i);
  }

  template<typename U>
  T operator()(U i) {
    return n += i;
  }

  template< typename U >
  friend class Accumulator;

 private:
  T n;
};


/// \brief Accumulator utility function
///
/// Given a value n of type T, returns an accumulator of type Accumulator<T> initialized to value n
template<typename T>
Accumulator<T> MakeAccumulator(T n) {
  return Accumulator<T>(n);
}

// Rsrc Management helper class /////////////////////////////////////////////

template< class T >
class MemTraits {
 public:
  typedef T *Type;
  typedef std::bad_alloc Exception;
  static void Release(Type rsrc) {
    delete rsrc;
  }

  static BOOL BadValue(Type rsrc) {
    return NULL == rsrc;
  }
};

template< class RsrcTraits >
class RaiseExceptionPolicy {
 public:
  typedef typename RsrcTraits::Type Type;
  static Type check(Type t) {  // NOLINT (readability/nolint)
    if (RsrcTraits::BadValue(t)) {
      throw typename RsrcTraits::Exception();
    }

    return t;
  }
};

template< class RsrcTraits >
class CheckForErrorPolicy {  // no exceptions
 public:
  typedef typename RsrcTraits::Type Type;

  static Type check(Type t) {  // NOLINT (readability/nolint)
    return t;
  }

  static bool isValid(Type t) {  // NOLINT (readability/nolint)
    return ! RsrcTraits::BadValue(t);
  }
};

template < class T,
           class RsrcTraits                    = MemTraits< T >,
           template <class> class ErrorPolicy  = RaiseExceptionPolicy >
class ResourceMgr : public RsrcTraits {
 public:
  typedef typename RsrcTraits::Type Type;
  typedef ErrorPolicy< RsrcTraits > TFullErrorPolicy;

  ResourceMgr(Type rsrc) : m_rsrc(TFullErrorPolicy::check(rsrc)) {}

  ~ResourceMgr() {
    RsrcTraits::Release(m_rsrc);
  }

  operator Type() {  // NOLINT (readability/nolint)
    return m_rsrc;
  }

  operator const Type() const {  // NOLINT (readability/nolint)
    return m_rsrc;
  }

  bool isValid() const {  // NOLINT (readability/nolint)
    return TFullErrorPolicy::isValid(m_rsrc);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(ResourceMgr);

  Type m_rsrc;
};

// Functor Helper classes /////////////////////////////////////
template< class R >
class IFunctor {
 public:
  IFunctor() {}
  virtual ~IFunctor() {}

  virtual R operator()() const = 0;  // NOLINT (readability/nolint)
  virtual UI_32 size() const = 0;  // NOLINT (readability/nolint)
};

template< class TFn >
class CFunctor0 : public IFunctor< typename FSig< TFn >::RType > {
 public:
  CFunctor0(TFn fn) : m_function(fn) {}
  typename FSig< TFn >::RType operator()() const {  // NOLINT (readability/nolint)
    return m_function();
  }
  UI_32 size() const {  // NOLINT (readability/nolint)
    return sizeof(TFn);
  }

 private:
  TFn m_function;
};

template< class TFn, class Arg1 >
class CFunctor1 : public IFunctor< typename FSig< TFn >::RType > {
 public:

  CFunctor1(TFn fn, Arg1 arg1) : m_function(fn), m_arg1(arg1) {}
  typename FSig< TFn >::RType operator()() const {  // NOLINT (readability/nolint)
    return m_function(m_arg1);
  }
  UI_32 size() const {  // NOLINT (readability/nolint)
    return sizeof(TFn) + sizeof(Arg1);
  }

 private:
  TFn m_function;
  Arg1 m_arg1;
};

template< class TFn, class Arg1, class Arg2 >
class CFunctor2 : public IFunctor< typename FSig< TFn >::RType > {
 public:

  CFunctor2(TFn fn, Arg1 arg1, Arg2 arg2) : m_function(fn), m_arg1(arg1), m_arg2(arg2) {}
  typename FSig< TFn >::RType operator()() const {  // NOLINT (readability/nolint)
    return m_function(m_arg1, m_arg2);
  }
  UI_32 size() const {  // NOLINT (readability/nolint)
    return sizeof(TFn) + sizeof(Arg1) + sizeof(Arg2);
  }
 private:
  TFn m_function;
  Arg1 m_arg1;
  Arg2 m_arg2;
};

template< class TFn, class Arg1, class Arg2, class Arg3 >
class CFunctor3 : public IFunctor< typename FSig< TFn >::RType > {
 public:

  CFunctor3(TFn fn, Arg1 arg1, Arg2 arg2, Arg3 arg3) : m_function(fn), m_arg1(arg1), m_arg2(arg2), m_arg3(arg3) {}
  typename FSig< TFn >::RType operator()() const {  // NOLINT (readability/nolint)
    return m_function(m_arg1, m_arg2, m_arg3);
  }
  UI_32 size() const {  // NOLINT (readability/nolint)
    return sizeof(TFn) + sizeof(Arg1) + sizeof(Arg2) + sizeof(Arg3);
  }
 private:
  TFn m_function;
  Arg1 m_arg1;
  Arg2 m_arg2;
  Arg3 m_arg3;
};

template< class TFn, class Arg1, class Arg2, class Arg3, class Arg4 >
class CFunctor4 : public IFunctor< typename FSig< TFn >::RType > {
 public:
  CFunctor4(TFn fn, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) : m_function(fn), m_arg1(arg1), m_arg2(arg2),
    m_arg3(arg3), m_arg4(arg4) {}
  typename FSig< TFn >::RType operator()() const {  // NOLINT (readability/nolint)
    return m_function(m_arg1, m_arg2, m_arg3, m_arg4);
  }
  UI_32 size() const {  // NOLINT (readability/nolint)
    return sizeof(TFn) + sizeof(Arg1) + sizeof(Arg2) + sizeof(Arg3) + sizeof(Arg4);
  }
 private:
  TFn m_function;
  Arg1 m_arg1;
  Arg2 m_arg2;
  Arg3 m_arg3;
  Arg4 m_arg4;
};

template< class TFn, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5 >
class CFunctor5 : public IFunctor< typename FSig< TFn >::RType > {
 public:
  CFunctor5(TFn fn, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) : m_function(fn), m_arg1(arg1), m_arg2(arg2),
    m_arg3(arg3), m_arg4(arg4), m_arg5(arg5) {}
  typename FSig< TFn >::RType operator()() const {  // NOLINT (readability/nolint)
    return m_function(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5);
  }
  UI_32 size() const {  // NOLINT (readability/nolint)
    return sizeof(TFn) + sizeof(Arg1) + sizeof(Arg2) + sizeof(Arg3) + sizeof(Arg4) + sizeof(Arg5);
  }
 private:
  TFn m_function;
  Arg1 m_arg1;
  Arg2 m_arg2;
  Arg3 m_arg3;
  Arg4 m_arg4;
  Arg5 m_arg5;
};

template< class TFn, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6 >
class CFunctor6 : public IFunctor< typename FSig< TFn >::RType > {
 public:
  CFunctor6(TFn fn, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6) : m_function(fn), m_arg1(arg1),
    m_arg2(arg2), m_arg3(arg3), m_arg4(arg4), m_arg5(arg5), m_arg6(arg6) {}
  typename FSig< TFn >::RType operator()() const {  // NOLINT (readability/nolint)
    return m_function(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6);
  }
  UI_32 size() const {  // NOLINT (readability/nolint)
    return sizeof(TFn) + sizeof(Arg1) + sizeof(Arg2) + sizeof(Arg3) + sizeof(Arg4) + sizeof(Arg5) + sizeof(Arg6);
  }
 private:
  TFn m_function;
  Arg1 m_arg1;
  Arg2 m_arg2;
  Arg3 m_arg3;
  Arg4 m_arg4;
  Arg5 m_arg5;
  Arg6 m_arg6;
};

template< class TFn, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7 >
class CFunctor7 : public IFunctor< typename FSig< TFn >::RType > {
 public:
  CFunctor7(TFn fn, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7)
    : m_function(fn), m_arg1(arg1), m_arg2(arg2), m_arg3(arg3), m_arg4(arg4), m_arg5(arg5), m_arg6(arg6), m_arg7(arg7) {}
  typename FSig< TFn >::RType operator()() const {  // NOLINT (readability/nolint)
    return m_function(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7);
  }
  UI_32 size() const {  // NOLINT (readability/nolint)
    return sizeof(TFn) + sizeof(Arg1) + sizeof(Arg2) + sizeof(Arg3) + sizeof(Arg4) + sizeof(Arg5) + sizeof(Arg6) + sizeof(Arg7);
  }
 private:
  TFn m_function;
  Arg1 m_arg1;
  Arg2 m_arg2;
  Arg3 m_arg3;
  Arg4 m_arg4;
  Arg5 m_arg5;
  Arg6 m_arg6;
  Arg7 m_arg7;
};


template<class TFn>
CFunctor0<TFn> functor(TFn fn) {
  return CFunctor0<TFn>(fn);
}

template<class TFn, class Arg1>
CFunctor1<TFn, Arg1> functor(TFn fn, Arg1 arg1) {
  return CFunctor1<TFn, Arg1>(fn, arg1);
}

template<class TFn, class Arg1, class Arg2>
CFunctor2<TFn, Arg1, Arg2> functor(TFn fn, Arg1 arg1, Arg2 arg2) {
  return CFunctor2<TFn, Arg1, Arg2>(fn, arg1, arg2);
}

template<class TFn, class Arg1, class Arg2, class Arg3>
CFunctor3<TFn, Arg1, Arg2, Arg3> functor(TFn fn, Arg1 arg1, Arg2 arg2, Arg3 arg3) {
  return CFunctor3<TFn, Arg1, Arg2, Arg3>(fn, arg1, arg2, arg3);
}

template<class TFn, class Arg1, class Arg2, class Arg3, class Arg4>
CFunctor4<TFn, Arg1, Arg2, Arg3, Arg4> functor(TFn fn, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
  return CFunctor4<TFn, Arg1, Arg2, Arg3, Arg4>(fn, arg1, arg2, arg3, arg4);
}

template<class TFn, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
CFunctor5<TFn, Arg1, Arg2, Arg3, Arg4, Arg5> functor(TFn fn, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) {
  return CFunctor5<TFn, Arg1, Arg2, Arg3, Arg4, Arg5>(fn, arg1, arg2, arg3, arg4, arg5);
}

template<class TFn, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
CFunctor6<TFn, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> functor(TFn fn, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,
                                                           Arg5 arg5, Arg6 arg6) {
  return CFunctor6<TFn, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>(fn, arg1, arg2, arg3, arg4, arg5, arg6);
}

template<class TFn, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
CFunctor7<TFn, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> functor(TFn fn, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,
                                                                 Arg5 arg5, Arg6 arg6, Arg7 arg7) {
  return CFunctor7<TFn, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>(fn, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

#endif  // __NSFRAMEWORK_NSUTILITY_NSUTILITY__  NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
