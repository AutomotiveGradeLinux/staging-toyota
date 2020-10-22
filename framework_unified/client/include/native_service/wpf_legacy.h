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
/**
 * @file
 * @brief \~english This file define primary types to use
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
/** @addtogroup framework
 *  @ingroup framework_unified
 *  @{
 */
#ifndef __WPF_LEGACY_H__  // NOLINT  (build/header_guard)
#define __WPF_LEGACY_H__

// these are legacy definitions.
// these were defined at Windows_Def.h ... in the past.
// if you make new code , you should not use these.
// you should use definitions in frameworkunified_types.h.

#include <native_service/frameworkunified_types.h>


// Windows_Def.h
#define CALLBACK
#define INVALID_HANDLE_VALUE  ((void*)(-1))
#define INFINITE        0xFFFFFFFF

typedef int         INT;
typedef unsigned int    UINT;
typedef unsigned int    *PUINT;
#define VOID        void
typedef char        CHAR;
typedef short       SHORT;  // NOLINT  (readability/nolint)
typedef long        LONG;  // NOLINT  (readability/nolint)
typedef SHORT       *PSHORT;
typedef LONG        *PLONG;
typedef unsigned long   ULONG;  // NOLINT  (readability/nolint)
typedef ULONG       *PULONG;
typedef unsigned short    USHORT;  // NOLINT  (readability/nolint)
typedef USHORT        *PUSHORT;
typedef unsigned char   UCHAR;
typedef UCHAR       *PUCHAR;
typedef unsigned long       DWORD;  // NOLINT  (readability/nolint)
// defined in kernelmoduleevklibdd.h typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;  // NOLINT  (readability/nolint)
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL        *PBOOL;
typedef BOOL        *LPBOOL;
typedef BYTE        *PBYTE;
typedef BYTE        *LPBYTE;
typedef int         *PINT;
typedef int         *LPINT;
typedef WORD        *PWORD;
typedef WORD        *LPWORD;
typedef long        *LPLONG;  // NOLINT  (readability/nolint)
typedef DWORD       *PDWORD;
typedef DWORD       *LPDWORD;
typedef void        *PVOID;
typedef void        *LPVOID;
typedef const void      *LPCVOID;
typedef wchar_t       WCHAR;
typedef size_t        SIZE_T;
typedef DWORD   COLORREF;
typedef DWORD   *LPCOLORREF;
typedef CHAR *PCHAR, *LPCH, *PCH;
typedef CONST CHAR *LPCCH, *PCCH;
typedef CHAR *NPSTR, *LPSTR, *PSTR;
typedef PSTR *PZPSTR;
typedef CONST PSTR *PCZPSTR;
typedef CONST CHAR *LPCSTR, *PCSTR;
typedef PCSTR *PZPCSTR;
typedef void *HGDIOBJ;
typedef int INT_PTR, *PINT_PTR;
typedef unsigned int UINT_PTR, *PUINT_PTR;
typedef long LONG_PTR, *PLONG_PTR;  // NOLINT  (readability/nolint)
typedef unsigned long ULONG_PTR, *PULONG_PTR;  // NOLINT  (readability/nolint)
typedef unsigned long ULONGLONG;  // NOLINT  (readability/nolint)
typedef char CCHAR;
typedef DWORD LCID;
typedef PDWORD PLCID;
typedef WORD   LANGID;
typedef long LONGLONG;  // NOLINT  (readability/nolint)
typedef WORD                ATOM;

typedef pthread_mutex_t   CRITICAL_SECTION;
typedef CRITICAL_SECTION *LPCRITICAL_SECTION;

#define __readableTo(extent)
#define __nullterminated __readableTo(sentinel(0))

typedef __nullterminated WCHAR *NWPSTR, *LPWSTR, *PWSTR;
typedef __nullterminated PWSTR *PZPWSTR;
typedef __nullterminated CONST PWSTR *PCZPWSTR;
typedef __nullterminated CONST WCHAR *LPCWSTR, *PCWSTR;
typedef __nullterminated PCWSTR *PZPCWSTR;

#ifdef  UNICODE
#define wsprintf    swprintf
typedef WCHAR *PWCHAR, *LPWCH, *PWCH;
typedef LPWSTR PTSTR, LPTSTR;
typedef LPCWSTR PCTSTR, LPCTSTR;
#else /* UNICODE */
#define wsprintf    sprintf
typedef LPSTR PTSTR, LPTSTR, PUTSTR, LPUTSTR;
typedef LPCSTR PCTSTR, LPCTSTR, PCUTSTR, LPCUTSTR;
typedef WCHAR *LPWCH;
typedef WCHAR *PWCHAR;  // 110509 chg
#endif  /* UNICODE */

#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name

typedef struct _SYSTEMTIME {
  WORD wYear;
  WORD wMonth;
  WORD wDayOfWeek;
  WORD wDay;
  WORD wHour;
  WORD wMinute;
  WORD wSecond;
  WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

#endif /* __WPF_LEGACY_H__ */   // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
