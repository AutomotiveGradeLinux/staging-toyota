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
 * @file cl_lockid.h
 * @brief Definition of LockID(LID)
 *
 */

#ifndef _cl_lockid_h_  // NOLINT(build/header_guard)
#define _cl_lockid_h_  // NOLINT(build/header_guard)

/**
 * @file cl_lockid.h
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup common_library
 *  @ingroup native_service
 *  @{
 */

/*
 * Numbering rules
 * Assign LID corresponding to the Lock name specified when requesting shared memories.
 */

typedef enum {
//  LID_TEST_1 = 0,
//  LID_TEST_2,
  LOCK_ANA_LOG_SEM = 0, /* Exclusive Control to store application log on kernel unmanaged RAM*/
  LOCK_POS_MTX_1,     /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_2,     /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_3,     /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_4,     /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_5,     /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_6,     /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_7,     /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_8,     /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_9,     /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_10,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_11,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_12,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_13,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_14,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_15,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_16,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_17,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_18,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_19,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_20,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_21,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_22,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_23,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_24,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_25,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_26,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_27,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_28,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_29,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_30,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_31,    /* Exclusive Control between POSITIONING and others */
  LOCK_POS_MTX_32,    /* Exclusive Control between POSITIONING and others */
  LOCK_CLOCK_SEM_TZ,    /* Access control to shared memory for TimeZone */
  LOCK_SEC_DATA_PROTECT,  /* Exclusive control to shared memory for the application that use the security chip. */
  LOCK_OS_SEM_ID__CWORD68_,  /* Exclusive Control for SSL library. */
  LOCK_ROM_ACCESS_IF,   /* Exclusive access control for system manager nv area */
  LOCK_BOOT_ACCESS_IF,  /* Exclusive access control for system manager nor area */
  LOCK_RAM_ACCESS_IF,   /* Exclusive access control for system manager drambackup area */
  LOCK_HRDS_1,      /* Exclusive Control between DataService and others */
  LOCK_HRDS_2,      /* Exclusive Control between DataService and others */
  LOCK_HRDS_3,      /* Exclusive Control between DataService and others */
  LOCK_HRDS_4,      /* Exclusive Control between DataService and others */
  LOCK_HRDS_5,      /* Exclusive Control between DataService and others */
  LOCK_HRDS_6,      /* Exclusive Control between DataService and others */
  LOCK_HRDS_7,      /* Exclusive Control between DataService and others */
  LOCK_HRDS_8,      /* Exclusive Control between DataService and others */
  LOCK_LOGG_ACCESS_IF,          /* Exclusive access control for NsLoggerService nv area */
  LOCK__CWORD76__HMI,     /* Exclusive Control between _CWORD76_Service and HMI's */
  LOCK__CWORD76___CWORD58_,      /* Exclusive Control between _CWORD76_Service and _CWORD58_Service */
  LOCK_DIAGCODE_MEM,    /* Exclusive Control to diag code memory area */
  LOCK_MOUNTER_MOUNT,   /* Exclusive Control for Mount */
  LOCK_NOR_ERCNT,     /* Exclusive access control for NOR erase count information */
  LOCK_RS_WNG_HIS_LIST_DATA,  /* Exclusive Control between RemoteService and RS-HMI. */
  LOCK_RS_WNG_SCREEN_DATA,  /* Exclusive Control between RemoteService and RS-HMI. */
  LOCK_RS_SCREEN_DATA,    /* Exclusive Control between RemoteService and RS-HMI. */
  LOCK_NSLOG_ACCES_IF_1,    /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_2,    /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_3,    /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_4,    /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_5,    /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_6,    /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_7,    /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_8,    /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_9,    /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_10,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_11,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_12,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_13,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_14,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_15,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_16,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_17,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_18,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_19,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_20,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_21,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_22,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_23,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_24,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_25,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_26,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_27,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_28,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_29,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_30,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_31,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_32,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_33,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_34,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_35,   /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_36,         /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_37,         /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_38,         /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_39,         /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_40,         /* Exclusive Control between writing to log file */
  LOCK_NSLOG_ACCES_IF_41,         /* Exclusive Control between writing to log file */

  LOCK_NSLOG_ACCES_IF_42,         /* Exclusive Control between writing to log file */

  LOCK_NSLOG_ACCES_IF_43,         /* Exclusive Control between writing to log file */
  LOCK_INFOSETTING_REV,   /* Exclusive control revision information for managing the settings */
  LOCK_SUBMENU_SELECT,    /* Exclusive control between writing the selection information of the sub-menu*/
  LOCK_DIAG_SEM1,     /* Exclusive Control between DiagService and others */
  LOCK_DIAG_SEM2,     /* Exclusive Control between Collect thread and Upload thread */
  LID_NUM         /* Number of LockID */
} ENUM_LOCK_ID;

/** @}*/  // end of common_library
/** @}*/  // end of NativeService
/** @}*/  // end of BaseSystem

#endif  // #ifndef _cl_lockid_h_  // NOLINT(build/header_guard)
