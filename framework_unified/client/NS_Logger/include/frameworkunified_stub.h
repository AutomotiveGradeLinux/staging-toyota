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

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_LOGGER_INCLUDE_FRAMEWORKUNIFIED_STUB_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_LOGGER_INCLUDE_FRAMEWORKUNIFIED_STUB_H_

/*#include <PosixBasedOS001legacy_types.h>*/  // for _UintXXt

#define _NTO_TRACE_USERFIRST        (0x00000000)

// from types.h
typedef char *__NTO_va_list;

// from sys/platform.h
#define __CLOCKADJUST \
    { \
      unsigned long       tick_count; /*NOLINT  (readability/nolint)*/\
      long            tick_nsec_inc;  /*NOLINT  (readability/nolint)*/\
    }

// from syspage.h
struct _clockadjust __CLOCKADJUST;
struct qtime_entry {
  _Uint64t          cycles_per_sec; /* for ClockCycle */
  _Uint64t volatile     nsec_tod_adjust;
  _Uint64t volatile     nsec;
  unsigned long       nsec_inc;  // NOLINT  (readability/nolint)
  unsigned long       boot_time; /* UTC seconds when machine booted */  // NOLINT  (readability/nolint)

  struct _clockadjust     adjust;
  unsigned long       timer_rate; /* times 10^timer_scale */  // NOLINT  (readability/nolint)
  long            timer_scale;  // NOLINT  (readability/nolint)
  unsigned long       timer_load;  // NOLINT  (readability/nolint)
  long            intr;  // NOLINT  (readability/nolint)
  unsigned long       epoch;  // NOLINT  (readability/nolint)
  unsigned long       flags;  // NOLINT  (readability/nolint)
  unsigned int        rr_interval_mul;
  unsigned long       spare0;  // NOLINT  (readability/nolint)
  _Uint64t volatile     nsec_stable;
  unsigned long       spare[4];  // NOLINT  (readability/nolint)
};

struct syspage_entry        *_syspage_ptr;

typedef struct {
  _Uint16t    entry_off;
  _Uint16t    entry_size;
} syspage_entry_info;

struct syspage_entry {
  _Uint16t      size;   /* size of syspage_entry */
  _Uint16t      total_size; /* size of system page */
  _Uint16t      type;
  _Uint16t      num_cpu;
  syspage_entry_info  system_private;
  syspage_entry_info  asinfo;
  syspage_entry_info  meminfo;
  syspage_entry_info  hwinfo;
  syspage_entry_info  cpuinfo;
  syspage_entry_info  cacheattr;
  syspage_entry_info  qtime;
  syspage_entry_info  callout;
  syspage_entry_info  callin;
  syspage_entry_info  typed_strings;
  syspage_entry_info  strings;
  syspage_entry_info  intrinfo;
  syspage_entry_info  smp;
  syspage_entry_info  pminfo;
  syspage_entry_info  mdriver;
  long        spare[2];  // NOLINT  (readability/nolint)
  union {
#if defined(SYSPAGE_TARGET_ALL) || defined(SYSPAGE_TARGET_X86)
    struct x86_syspage_entry  x86;
#endif
#if defined(SYSPAGE_TARGET_ALL) || defined(SYSPAGE_TARGET_PPC)
    struct ppc_syspage_entry  ppc;
#endif
#if defined(SYSPAGE_TARGET_ALL) || defined(SYSPAGE_TARGET_MIPS)
    struct mips_syspage_entry mips;
#endif
#if defined(SYSPAGE_TARGET_ALL) || defined(SYSPAGE_TARGET_ARM)
    struct arm_syspage_entry  arm;
#endif
#if defined(SYSPAGE_TARGET_ALL) || defined(SYSPAGE_TARGET_SH)
    struct sh_syspage_entry sh;
#endif
    struct {
      long            filler[20];  // NOLINT  (readability/nolint)
    } filler;
  } un;
};

#define _SYSPAGE_ENTRY( __base, __field )  /*NOLINT  (readability/nolint)*/\
  ((struct __field##_entry *)reinterpret_cast<void *>(reinterpret_cast<char *>(__base)\
  + (__base)->__field.entry_off))
#define SYSPAGE_ENTRY(__field)      _SYSPAGE_ENTRY(_syspage_ptr, __field)

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_LOGGER_INCLUDE_FRAMEWORKUNIFIED_STUB_H_
