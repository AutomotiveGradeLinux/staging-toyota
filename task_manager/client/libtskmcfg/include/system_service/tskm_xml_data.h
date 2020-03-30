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
 * @file tskm_xml_data.h
 * @brief \~english This file provide api to operating task manager with XML data
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup task_manager
 *  @ingroup system_service
 *  @{
 */
#ifndef TASK_MANAGER_CLIENT_LIBTSKMCFG_INCLUDE_SYSTEM_SERVICE_TSKM_XML_DATA_H_
#define TASK_MANAGER_CLIENT_LIBTSKMCFG_INCLUDE_SYSTEM_SERVICE_TSKM_XML_DATA_H_

#include "system_service/tskm_type.h"
#include "system_service/tskm_svc.h"


#ifdef __cplusplus
extern "C" {
#endif

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_initServiceList
  /// \~english @par Summary
  ///           init started services list
  /// \~english @param  [out] p_svcs
  ///           p_svcs   - pointer of service list
  /// \~english @par
  ///           p_svcs   TSKM_SVCS_CTX_t*
  /// \~english @code
  ///           typedef struct {
  ///             uint32_t svcNum;           // service num
  ///             TSKM_SVC_CTX_t *svcList;   // services list pointer
  ///           } TSKM_SVCS_CTX_t;
  ///           @endcode
  /// \~english @param  [in] iFd
  ///           iFd      - the file descriptor return by inotify_init1().
  /// \~english @par
  ///           p_svcs  int
  /// \~english @retval 0   Success
  /// \~english @retval -1  Failed
  /// \~english @par Preconditions
  ///       -  called in the ctxCreate()
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -  if srvId in serviceList greater than TSKM_SVC_ID_MAX_SIZE.  [-1]
  /// \~english @par Classification
  ///       -  public
  /// \~english @par Type
  ///       - sync only
  /// \~english @par Detail
  ///      Init started services list\n
  ///      Get the started services list from static variable serviceList and init by input parameter iFd\n
  ///      If envirment is NFS, change the path after /tmp to CAP path.\n
  ///
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
int tskm_initServiceList(TSKM_SVCS_CTX_t* p_svcs, int iFd);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_initWakeupCtx
  /// \~english @par Summary
  ///           Get state of started step.
  /// \~english @param  [out] p_wakeup
  ///           p_wakeup   - pointer of TSKM_GSTEP_CTX_t
  /// \~english @par
  ///           p_wakeup  TSKM_GSTEP_CTX_t*
  /// \~english @code
  /// typedef struct {
  ///   uint32_t gstepIdx;            // step Index
  ///   uint32_t gstepNum;            // number of step
  ///   TSKM_GSTEP_t* gstep;          // step start/stop info
  ///   uint64_t compState;
  /// } TSKM_GSTEP_CTX_t;
  ///           @endcode
  /// \~english @param  [in] isVupMode
  ///           isVupMode  - version up flag
  /// \~english @par
  ///           isVupMode  BOOL
  ///           TRUE       version up mode
  ///           FALSE      not versoin up mode
  /// \~english @retval  void
  ///
  /// \~english @par Preconditions
  ///       -   called in ctxCreate()
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -  none
  /// \~english @par Classification
  ///       -  public
  /// \~english @par Type
  ///       -  sync only
  /// \~english @par Detail
  ///      Get state of started step.\n
  ///      if version up mode is TRUE, retrun the wakeupGstepVup.\n
  ///      if version up mode is FALSE, return the wakeupGstep.\n
  ///
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
void tskm_initWakeupCtx(TSKM_GSTEP_CTX_t* p_wakeup, BOOL isVupMode);


  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_initDownCtx
  /// \~english @par Summary
  ///           Get state of down step.
  /// \~english @param  [out] p_down
  ///           p_down   - pointer of TSKM_GSTEP_CTX_t
  /// \~english @par
  ///           p_wakeup  TSKM_GSTEP_CTX_t*
  /// \~english @code
  /// typedef struct {
  ///   uint32_t gstepIdx;            // step Index
  ///   uint32_t gstepNum;            // number of step
  ///   TSKM_GSTEP_t* gstep;          // step start/stop info
  ///   uint64_t compState;
  /// } TSKM_GSTEP_CTX_t;
  ///           @endcode
  /// \~english @param  [in] isVupMode
  ///           isVupMode  - version up mode flag
  /// \~english @par
  ///           isVupMode  BOOL
  ///           TRUE       version up mode
  ///           FALSE      not versoin up mode
  /// \~english @retval
  /// \~english @par Preconditions
  ///       -   called in ctxCreate().
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -  none
  /// \~english @par Classification
  ///       - public
  /// \~english @par Type
  ///       - sync only
  /// \~english @par Detail
  ///      Get state of down step.\n
  ///      if version up mode is TRUE, retrun the downGstepVup.\n
  ///      if version up mode is FALSE, return the downGstep.\n
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
void tskm_initDownCtx(TSKM_GSTEP_CTX_t* p_down, BOOL isVupMode);

#ifdef __cplusplus
}
#endif

#endif  // TASK_MANAGER_CLIENT_LIBTSKMCFG_INCLUDE_SYSTEM_SERVICE_TSKM_XML_DATA_H_
/** @}*/
/** @}*/
/** @}*/
