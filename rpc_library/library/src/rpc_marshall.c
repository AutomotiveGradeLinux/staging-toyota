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

/**
 * @file rpc_marshall.c
 * @brief RPC Library Internal Implementation--Argument Conversion During API Calls
 *
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <math.h>

#include <other_service/rpc.h>
#include "rpc_internal.h"

#include "apidef.tab.h"
#include "apidef.h"

/** @addtogroup RPClib_in
 * @{
 */
/** Structures for String Add Operations */
typedef struct {
  char *start;
  char *wp;
  unsigned int units;
  unsigned int remain;
} RpcString;

#ifdef STRETCH_STRING
#define _STRING_ALLOC_UNIT 512
#else
/*
 * Batch allocate total bytes of arguments + data to be added internally 
 * (maximal 6 bytes per argument)
 */
#define _STRING_ALLOC_UNIT \
  (RPC_MAX_API_ARG_TOTAL_SIZE + RPC_MAX_API_ARG_NUM * (2 + 4))
#endif

#define _ENOUGH_SPACE_FOR_ALL_TYPES \
  (sizeof(UINT64) > sizeof(double) ? sizeof(UINT64) : sizeof(double))

static RpcString* NewRpcString(void);
#ifdef STRETCH_STRING
static int StretchString(RpcString *str, UINT16 bytes);
#endif
static char *CopyString(RpcString *str, unsigned int *size);
static void DestroyString(RpcString *str);
static inline int AppendString(RpcString *str,
       const UINT8 *append, UINT16 applen);
static inline int MarshallUINT8(RpcString *str, UINT8 uc);
static inline int MarshallUINT16(RpcString *str, UINT16 uh);
static inline int MarshallUINT32(RpcString *str, UINT32 ui);
static inline int MarshallUINT64(RpcString *str, UINT64 ul);
static inline int Marshallfloat(RpcString *str, float f);
static inline int Marshalldouble(RpcString *str, double d);
static inline int MarshallUINT8Stream(RpcString *str, const UINT8 *buf,
          UINT16 bytes);
static inline int MarshallString(RpcString *str, char *buf,
          UINT16 bytes);
static inline int MarshallNullPointer(RpcString *str);
static inline int MarshallPointer(RpcString *str);

static RpcString *
NewRpcString(void) {
  RpcString *str;

  str = rpc_malloc(sizeof(RpcString));
  if (str == NULL) {  // LCOV_EXCL_START 5: fail safe for libc malloc
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return NULL;
  }  // LCOV_EXCL_STOP

  str->start = rpc_malloc(_STRING_ALLOC_UNIT);
  if (str->start == NULL) {  // LCOV_EXCL_START 5: fail safe for libc malloc
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    rpc_free(str);
    return NULL;
  }  // LCOV_EXCL_STOP
  str->start[0] = '\0';
  str->wp = str->start;
  str->units = 1;
  str->remain = _STRING_ALLOC_UNIT - 1;

  return str;
}

#ifdef STRETCH_STRING
static int
StretchString(RpcString *str, UINT16 bytes) {
  unsigned int oldunits = str->units;
  unsigned int units = 1 + (bytes / _STRING_ALLOC_UNIT) + oldunits;
  if (units == oldunits) {
    return 0;
  }
  unsigned int length = str->wp - str->start;
  char *newp = rpc_malloc(units * _STRING_ALLOC_UNIT);
  if (newp == NULL) {
    return -1;
  }
  memcpy(newp, str->start, length);
  rpc_free(str->start);
  str->start = newp;
  str->wp = str->start + length;
  *(str->wp) = '\0';
  str->units = units;
  str->remain += (units - oldunits) * _STRING_ALLOC_UNIT;
  return 0;
}
#endif  /* STRETCH_STRING */

static inline int
AppendString(RpcString *str, const UINT8 *append, UINT16 applen) {
  rpc_assert(append != NULL);  // LCOV_EXCL_BR_LINE 6: double check
#ifdef STRETCH_STRING
  if (applen > str->remain) {
    if (StretchString(str, applen - str->remain) < 0) {
      return -1;
    }
  }
#else
  rpc_assert(applen <= str->remain);  // LCOV_EXCL_BR_LINE 6: double check
#endif
  memcpy(str->wp, append, applen);
  str->wp += applen;
  str->remain -= applen;
  return 0;
}

static char *
CopyString(RpcString *str, unsigned int *size) {
  unsigned int length = (unsigned int)(str->wp - str->start);
  char *ret = rpc_malloc(length);
  // LCOV_EXCL_BR_START 5: fail safe for libc malloc
  if (ret == NULL) {
    return NULL;
  }
  // LCOV_EXCL_BR_STOP
  memcpy(ret, str->start, length);
  if (size != NULL) {
    *size = length;
  }

  return ret;
}

static void
DestroyString(RpcString *str) {
  rpc_free(str->start);
  rpc_free(str);
}

static inline int
MarshallUINT8(RpcString *str, UINT8 c) {
  UINT8 buf[1 + sizeof(c)];
  buf[0] = 'C';
  buf[1] = c;
  return AppendString(str, buf, sizeof(buf));
}

static inline int
MarshallUINT8Stream(RpcString *str, const UINT8 *buf, UINT16 bytes) {
  int ret = AppendString(str, (const UINT8 *)"B", 1);
  if (ret < 0) {  // LCOV_EXCL_BR_LINE 15: inline func rpc_marshall.c
    return -1;
  }
  return AppendString(str, buf, bytes);
}

static inline int
MarshallVararrayStream(RpcString *str, const UINT8 *buf, UINT16 bytes) {
  char head_str[1+4+1+1];  /* Area where symbol "V" + size is stored */
  sprintf(head_str, "V%03d ", bytes);
  head_str[1+4+1] = '\0';

  if (AppendString(str, (const UINT8 *)head_str, (UINT16)strlen(head_str)) < 0) {  // LCOV_EXCL_BR_LINE 15: inline func rpc_marshall.c
    return -1;
  }

  if (AppendString(str, buf, bytes) < 0) {  // LCOV_EXCL_BR_LINE 15: inline func rpc_marshall.c
    return -1;
  }
  return 0;
}

static inline int
MarshallString(RpcString *str, char *buf, UINT16 bytes) {
  char *p = buf;
  UINT16 count = 0;
  /* count the number of bytes in the argument */
  while(*p != '\0' && count < (UINT16)(bytes - 1)) {
    count++;
    p++;
  }

  char count_str[1+strlen("1024")+1+1];
  sprintf(count_str, "S%d ", count);

  if (AppendString(str, (const UINT8 *)count_str, (UINT16)strlen(count_str)) < 0 || AppendString(str, (const UINT8 *)buf, count) < 0) {  // LCOV_EXCL_BR_LINE 11: Unexpected branch // NOLINT(readability/nolint)
    return -1;
  }
  return 0;
}

static int
MarshallUINT16(RpcString *str, UINT16 uh) {
  UINT8 buf[1 + sizeof(uh)];
  buf[0] = 'H';
  memcpy(buf + 1, &uh, sizeof(uh));
  return AppendString(str, buf, sizeof(buf));
}

static inline int
MarshallUINT32(RpcString *str, UINT32 ui) {
  UINT8 buf[1 + sizeof(ui)];
  buf[0] = 'I';
  memcpy(buf + 1, &ui, sizeof(ui));
  return AppendString(str, buf, sizeof(buf));
}

static inline int
Marshallint(RpcString *str, int i) {
  return MarshallUINT32(str, (UINT32)i);
}

static inline int
MarshallUINT64(RpcString *str, UINT64 ul) {
  UINT8 buf[1 + sizeof(ul)];
  buf[0] = 'L';
  memcpy(buf + 1, &ul, sizeof(ul));
  return AppendString(str, buf, sizeof(buf));
}

static inline int
Marshallfloat(RpcString *str, float f) {
  UINT8 buf[1 + sizeof(f)];
  buf[0] = 'F';
  memcpy(buf + 1, &f, sizeof(f));
  return AppendString(str, buf, sizeof(buf));
}

static inline int
Marshalldouble(RpcString *str, double d) {
  UINT8 buf[1 + sizeof(d)];
  buf[0] = 'D';
  memcpy(buf + 1, &d, sizeof(d));
  return AppendString(str, buf, sizeof(buf));
}

static inline int
MarshallPointer(RpcString *str) {
  /* only to specify that a non-NULL pointer was delivered */
  return AppendString(str, (const UINT8 *)"P", 1);
}

static inline int
MarshallNullPointer(RpcString *str  /*, int code*/) {
  return AppendString(str, (const UINT8 *)"N", 1);
}

/** @ingroup RPClib_in
 */

#define MACROMarshallPointer(TYPE) \
  do { \
    TYPE *p = (TYPE *)temp; \
    error = Marshall##TYPE(str, *p); \
  } while(0)

#define MACROMarshallValue(TYPE, STACK_TYPE) \
  do { \
    TYPE value = (TYPE)va_arg(ap, STACK_TYPE); \
    error = Marshall##TYPE(str, value); \
  } while(0)

char *
RPC_marshall_arguments(unsigned int *size, int dont_marshall_out_pointer,
                       int num_args, ...) {
  rpc_assert(num_args <= RPC_MAX_API_ARG_NUM);  // LCOV_EXCL_BR_LINE 6: double check
  va_list ap;

  RpcString *str;
  str = NewRpcString();
  if (str == NULL) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc malloc
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return NULL;  // LCOV_EXCL_LINE 5: fail safe for libc malloc
  }

  va_start(ap, num_args);

  int ii, error;
  error = 0;

  for(ii = 0 ; ii < num_args && error == 0 ; ii++) {
    unsigned int code, is_vararray, is_pointer, in_out, out_only;
    UINT16 bytes;
    unsigned int val = va_arg(ap, unsigned int);

    RPC_marshall_flag flag;
    flag.all = ntohl(val);
    code = flag.bits.code;
    is_vararray = flag.bits.is_vararray;
    is_pointer = flag.bits.is_pointer;
    in_out = flag.bits.in_out;
    bytes = flag.bits.bytes;
    rpc_assert(bytes <= RPC_MAX_API_ARG_SIZE);  // LCOV_EXCL_BR_LINE 6: double check


    out_only = 0;
    if (in_out == RPC_OUT_ARG) {  /* OUT only argument */
      out_only = 1;
    }

    if (is_pointer) {  /* Argument passed by pointer */

      void *temp = (void *)va_arg(ap, void *);
      if (temp == NULL) {
        /* NULL pointer */
        error = MarshallNullPointer(str);
      } else if (dont_marshall_out_pointer && out_only) {
        /* Not refer to the contents of a pointer */
        error = MarshallPointer(str);
      } else {/* Marshall the contents of pointers */
        if (is_vararray) {
          /* Variable-length array */
          error = MarshallVararrayStream(str, temp, bytes);
        } else {
          switch(code) {
            case rpc_CHAR:
            case rpc_INT8:
            case rpc_UINT8:
              MACROMarshallPointer(UINT8);
              break;
            case rpc_INT16:
            case rpc_UINT16:
              MACROMarshallPointer(UINT16);
              break;
            case rpc_INT:
            case rpc_SINT:
            case rpc_UINT:
              MACROMarshallPointer(int);
              break;
            case rpc_INT32:
            case rpc_UINT32:
              MACROMarshallPointer(UINT32);
              break;
            case rpc_INT64:
            case rpc_UINT64:
              MACROMarshallPointer(UINT64);
              break;
            case rpc_FLOAT:
              MACROMarshallPointer(float);
              break;
            case rpc_DOUBLE:
              MACROMarshallPointer(double);
              break;
            case rpc_STRING:
              error = MarshallString(str, temp, bytes);
              break;
            case rpc_USER_DEFINED:
              error = MarshallUINT8Stream(str, temp, bytes);
              break;
            default:
              error = -1;
              break;
          }
        }
      }
    } else {/* Argument passed by value */

      /* Note: In this area, the code depends on the CPU architecture */

      switch(code) {
        case rpc_CHAR:
        case rpc_INT8:
        case rpc_UINT8:
          MACROMarshallValue(UINT8, unsigned int);
          break;
        case rpc_INT16:
        case rpc_UINT16:
          MACROMarshallValue(UINT16, unsigned int);
          break;
        case rpc_INT:
        case rpc_SINT:
        case rpc_UINT:
          MACROMarshallValue(int, int);  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_marshall.c
          break;
        case rpc_INT32:
        case rpc_UINT32:
          MACROMarshallValue(UINT32, UINT32);  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_marshall.c
          break;
        case rpc_INT64:
        case rpc_UINT64:
          MACROMarshallValue(UINT64, UINT64);  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_marshall.c
          break;
        case rpc_FLOAT:
          MACROMarshallValue(float, double);  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_marshall.c
          break;
        case rpc_DOUBLE:
          MACROMarshallValue(double, double);  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_marshall.c
          break;
        case rpc_USER_DEFINED:
          rpc_assert(bytes <= sizeof(UINT64));
          /* This area is very architecture-dependent! */
          if (bytes <= sizeof(UINT32)) {
            UINT32 value = (UINT32)va_arg(ap, UINT32);  // LCOV_EXCL_BR_LINE 15: macro defined in stdarg.h
            error = MarshallUINT8Stream(str, (UINT8 *)&value, bytes);
          } else if (bytes <= sizeof(UINT64)) {  // LCOV_EXCL_BR_LINE 6: double check
            UINT64 value = (UINT64)va_arg(ap, UINT64);  // LCOV_EXCL_BR_LINE 15: macro defined in stdarg.h
            error = MarshallUINT8Stream(str, (UINT8 *)&value, bytes);
          }
          break;
        default:
          error = -1;
          break;
      }
    }
  }
  va_end(ap);

  if (error != 0) {
    DestroyString(str);
    return NULL;
  }

  char *ret = CopyString(str, size);
  DestroyString(str);
  return ret;
}

static inline int DeMarshallUINT8(const char *from, UINT8 *ucp);
static inline int DeMarshallUINT16(const char *from, UINT16 *uhp);
static inline int DeMarshallUINT32(const char *from, UINT32 *uip);
static inline int DeMarshallUINT64(const char *from, UINT64 *ulp);
static inline int DeMarshallfloat(const char *from, float *fp);
static inline int DeMarshalldouble(const char *from, double *dp);
static inline int DeMarshallUINT8Stream(const char *from, UINT8 *buffer,
            UINT16 bytes);
static inline int DeMarshallString(const char *from, char *buffer,
           UINT16 bytes);

static inline int
DeMarshallUINT8(const char *from, UINT8 *ucp) {
  if (*from == 'C') {
    *ucp = *(UINT8 *)(from + 1);
    return 1 + sizeof(*ucp);
  } else {
    return -1;
  }
}

static inline int
DeMarshallUINT16(const char *from, UINT16 *uhp) {
  if (*from == 'H') {
    memcpy(uhp, from + 1, sizeof(*uhp));
    return 1 + sizeof(*uhp);
  } else {
    return -1;
  }
}

static inline int
DeMarshallUINT32(const char *from, UINT32 *uip) {
  if (*from == 'I') {
    memcpy(uip, from + 1, sizeof(*uip));
    return 1 + sizeof(*uip);
  } else {
    return -1;
  }
}

static inline int
DeMarshallint(const char *from, int *ip) {
  return DeMarshallUINT32(from, (UINT32 *)ip);
}

static inline int
DeMarshallUINT64(const char *from, UINT64 *ulp) {
  if (*from == 'L') {
    memcpy(ulp, from + 1, sizeof(*ulp));
    return 1 + sizeof(*ulp);
  } else {
    return -1;
  }
}

static inline int
DeMarshallfloat(const char *from, float *fp) {
  if (*from == 'F') {
    memcpy(fp, from + 1, sizeof(*fp));
    return 1 + sizeof(*fp);
  } else {
    return -1;
  }
}

static inline int
DeMarshalldouble(const char *from, double *dp) {
  if (*from == 'D') {
    memcpy(dp, from + 1, sizeof(*dp));

    return 1 + sizeof(*dp);
  } else {
    return -1;
  }
}

static inline int
DeMarshallUINT8Stream(const char *from, UINT8 *buffer, UINT16 bytes) {
  if (*from == 'B') {
    memcpy(buffer, from + 1, bytes);
    return (int)(1 + bytes);
  } else {
    return -1;
  }
}

static inline int
DeMarshallString(const char *from, char *buffer, UINT16 bytes) {
  if (*from == 'S') {
    char *start;
    long len = strtol(from + 1, &start, 10);
    if (len < 0 || len >= bytes) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc strtol
      return -1;
    }
    start++;  /* skip ' ' */
    int skip = (int)(start - from);
    memcpy(buffer, start, (size_t)len);
    buffer[len] = '\0';
    return skip + (int)len;
  } else {
    return -1;
  }
}

/* Variable-length array data */
static inline int
DemarshallVararrayInfo(const char *from, UINT16 *bytes/* OUT */) {
  if (*from == 'V') {
    char *end;
    long len = strtol((char *)(from + 1), &end, 10);
    if (len <= 0 || len > RPC_MAX_API_ARG_SIZE) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc strtol
      return -1;
    }
    *bytes = (UINT16)len;
    return (int)(end - from + 1);  /* skip ' ' */
  } else {
    return -1;
  }
}

/** @ingroup RPClib_in
 */

#define MACRODemarshall(TYPE) \
    do {\
  TYPE *p; \
  if (need_alloc && is_pointer) { \
    p = rpc_malloc(sizeof(TYPE)); \
    *(TYPE **)temp = p; \
  } else { \
    p = (TYPE *)temp; \
  } \
  if (p != NULL) { \
    ret = DeMarshall##TYPE(from, p); \
  } \
    } while(0)

int
RPC_demarshall_arguments(const char *from, unsigned int size,
       int need_alloc, int num_args, ...) {
  rpc_assert(num_args <= RPC_MAX_API_ARG_NUM);  // LCOV_EXCL_BR_LINE 6: double check
  va_list ap;
  va_start(ap, num_args);

  int ii, error;
  error = 0;
  int remain_len = (int)size;

  for(ii = 0 ; ii < num_args && error == 0 && remain_len > 0 ; ii++) {
    unsigned int code, is_pointer, is_vararray;
    UINT16 bytes;
    unsigned int val = va_arg(ap, unsigned int);
    RPC_marshall_flag flag;
    flag.all = ntohl(val);
    code = flag.bits.code;
    is_vararray = flag.bits.is_vararray;
    is_pointer = flag.bits.is_pointer;
    bytes = flag.bits.bytes;
    rpc_assert(bytes <= RPC_MAX_API_ARG_SIZE);  // LCOV_EXCL_BR_LINE 6: double check


    void *temp = va_arg(ap, void *);
    int ret = -1;

    if (*from == 'N') {  /* NULL pointer */
      if (bytes > 0 || is_pointer != 0) {
        if (need_alloc) {
          *(void **)temp = NULL;
        } else {
          /* do nothing */
        }
        ret = 1;
      }
    } else if (*from == 'P') {  /* Pointer(no content) */
      if (need_alloc) {
        if (bytes > 0) {  /* String type or user-defined type */
          *(void **)temp = rpc_malloc(bytes);
        } else if (is_pointer != 0) {  /* Other pointers */
          *(void **)temp = rpc_malloc(_ENOUGH_SPACE_FOR_ALL_TYPES);
        }
        ret = 1;
      }
    } else {  /* non-NULL pointer */
      if ( is_vararray ) {  /* Variable-length array */
        ret = DemarshallVararrayInfo(from, &bytes);
        if( ret < 0 ) {
          va_end(ap);
          return -1;  /* pgr0524 */
        }
      char *p;
      if (need_alloc) {
        p = rpc_malloc(sizeof(char)*bytes);
        *(char **)temp = p;
      } else {
        p = (char *)temp;  /* pgr0524 */
      }
      if (p != NULL) {
        memcpy(p, from + ret, bytes);
        ret += bytes;
      }
      } else {
        switch(code) {
          case rpc_CHAR:
          case rpc_INT8:
          case rpc_UINT8:
            MACRODemarshall(UINT8);  /* pgr0524 */  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_marshall.c
            break;
          case rpc_INT16:
          case rpc_UINT16:
            MACRODemarshall(UINT16);  /* pgr0524 */  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_marshall.c
            break;
          case rpc_INT:
          case rpc_SINT:
          case rpc_UINT:
            MACRODemarshall(int);  /* pgr0524 */  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_marshall.c
            break;
          case rpc_INT32:
          case rpc_UINT32:
            MACRODemarshall(UINT32);  /* pgr0524 */  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_marshall.c
            break;
          case rpc_INT64:
          case rpc_UINT64:
            MACRODemarshall(UINT64);  /* pgr0524 */  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_marshall.c
            break;
          case rpc_FLOAT:
            MACRODemarshall(float);  /* pgr0524 */  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_marshall.c
            break;
          case rpc_DOUBLE:
            MACRODemarshall(double);  /* pgr0524 */  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_marshall.c
            break;
          case rpc_USER_DEFINED: {
            UINT8 *p;
            if (need_alloc && is_pointer) {
              p = rpc_malloc(bytes);
              *(UINT8 **)temp = p;
            } else {
              p = (UINT8 *)temp;  /* pgr0524 */
            }
            if (p != NULL) {
              ret = DeMarshallUINT8Stream(from, p, bytes);
            }
            break;
          }
          case rpc_STRING: {
            char *p;
            if (need_alloc) {
              p = rpc_malloc(sizeof(char)*bytes);
              *(char **)temp = p;
            } else {
              p = (char *)temp;  /* pgr0524 */
            }
            if (p != NULL) {
              ret = DeMarshallString(from, p, bytes);
            }
            break;
          }
          default:
            break;
        }
      }
    }
    if (ret < 0) {
      error = 1;
    } else {
      remain_len -= ret;
      from += ret;
    }
  }
  va_end(ap);

  if (error) {
    return -1;  /* pgr0524 */
  } else {
    return 0;  /* pgr0524 */
  }
}

void
RPC_marshall_free(int num, ...) {
  va_list ap;
  va_start(ap, num);

  int i;
  for(i = 0 ; i < num ; i++) {
    void *ptr = va_arg(ap, void *);  // LCOV_EXCL_BR_LINE 15: macro defined in stdarg.h
    if (ptr != NULL) {
      rpc_free(ptr);
    }
  }
  va_end(ap);
}
