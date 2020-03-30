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
 * @file
 * @brief \~english This file contains the enum,struct and api of cl_monitor.
 */
#ifndef _cl_monitor_h_  // NOLINT(build/header_guard)
#define _cl_monitor_h_  // NOLINT(build/header_guard)

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///        Monitor init enum
/////////////////////////////////////////////////////////////////////////////////////
typedef enum {
  CL_MONITOR_INIT_SYSTEM = 0,
  CL_MONITOR_INIT_USER
} CL_MonitorInit_t;

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///        the struct of Monitor Entry
/////////////////////////////////////////////////////////////////////////////////////
typedef struct {
  uint16_t pid;
  uint8_t type;
  uint8_t state;

//  uint32_t timeout;
  time_t timeout;
  uint32_t id;
  uint32_t user_data;
} CL_MonitorEntry_t;

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///        Monitor type enum
/////////////////////////////////////////////////////////////////////////////////////
typedef enum {
  CL_MONITOR_TYPE_GENERIC = 0,
  CL_MONITOR_TYPE_RPC
} CL_MonitorType_t;

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///        Monitor state enum
/////////////////////////////////////////////////////////////////////////////////////
typedef enum {
  CL_MONITOR_STATE_SLEEP = 0,
  CL_MONITOR_STATE_RUN
} CL_MonitorState_t;


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///        the struct of Monitor Search
/////////////////////////////////////////////////////////////////////////////////////
typedef struct {
  CL_MonitorEntry_t *entry_list;
  int entry_num;
} CL_MonitorSearch_t;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file cl_monitor.h
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

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///     Monitor initialize
/// \~english @param [in] init_type
///     CL_MonitorInit_t - the type of initialzation
/// \~english @retval 0  Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     None
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
int CL_MonitorInit(CL_MonitorInit_t init_type);  // NOLINT(readability/nolint)


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///     Set Monitor Entry
/// \~english @param [in] type
///     CL_MonitorType_t - the type of monitor
/// \~english @param [in] state
///     CL_MonitorState_t - the state of monitor
/// \~english @param [in] timeout
///     uint32_t - the vlaue of timeout
/// \~english @param [in] user_data
///     uint32_t - the user data
/// \~english @retval 0  Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     None
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
int CL_MonitorSetEntry(CL_MonitorType_t type, uint32_t id,  // NOLINT(readability/nolint)
                       CL_MonitorState_t state, uint32_t timeout,
                       uint32_t user_data);


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///     Get Monitor Entry
/// \~english @param [in] type
///     CL_MonitorType_t - the type of monitor
/// \~english @param [in] id
///     uint32_t - the monitor id
/// \~english @param [out] entry
///     CL_MonitorEntry_t* - the pointer to monitor entry
/// \~english @retval 0  Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     None
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
int CL_MonitorGetEntry(CL_MonitorType_t type, uint32_t id, CL_MonitorEntry_t *entry);  // NOLINT(readability/nolint)


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///     Monitor searth intialize
/// \~english @param [in] serch
///     CL_MonitorSearch_t* - the pointor to monitory search
/// \~english @retval 0  Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     None
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
int CL_MonitorSearchInit(CL_MonitorSearch_t *serch);  // NOLINT(readability/nolint)


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///     Monitor searth destroy
/// \~english @param [in] serch
///     CL_MonitorSearch_t* - the pointor to monitory search
/// \~english @retval 0  Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     None
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
int CL_MonitorSearchDestroy(CL_MonitorSearch_t *serch);  // NOLINT(readability/nolint)


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///     Monitor searth timeout
/// \~english @param [in] serch
///     CL_MonitorSearch_t* - the pointor to monitory search
/// \~english @retval 0  Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     None
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
int CL_MonitorSearchTimeout(CL_MonitorSearch_t *search);  // NOLINT(readability/nolint)

/** @}*/  // end of common_library
/** @}*/  // end of NativeService
/** @}*/  // end of BaseSystem
#ifdef __cplusplus
}
#endif

#endif /* ifndef _cl_monitor_h_ */  // NOLINT(build/header_guard)
