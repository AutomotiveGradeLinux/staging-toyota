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
/*
 * Information.
 * This source code is a sample source code .
 * Implementation of the function must be performed by the vendor.
 */

#include <DiagCodeAPI.h>

DGCODE_RET_API Diag_SubscribeAvailability(HANDLE hApp, uint8_t availabilityId) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_NotifyAvailability(uint8_t availabilityId, BOOL bAvailability) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_PutDiagCode(HANDLE hApp, uint64_t err_id, uint16_t diagcode_code) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_DeleteDiagCode(HANDLE hApp, uint64_t err_id, uint16_t diagcode_code) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_PutDiagCode_Custom(HANDLE hApp, DGCODE_MEMINFO_CUSTOM* err_info) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_PutDTCInfo(const HANDLE hApp, const uint64_t dtc_id, const uint8_t test_result,
                               const DGCODE_DTC_SSR_INFO &ssr_info,
                               const uint16_t spply_code) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_PutRoBInfo(const HANDLE hApp, const uint64_t rob_id, const DGCODE_ROB_SSR_INFO* ssr_info) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_DeleteDTCInfo(const HANDLE hApp, const uint64_t err_id) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_DeleteRoBInfo(const HANDLE hApp, const uint64_t rob_id) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_GetDTCInfoList_CWORD29_(const DGCODE_GET_DTC_LIST_INFO* const get_info,
                                   DGCODE_DTC_LIST_INFO* const list_info) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_GetRoBInfoList(const DGCODE_GET_ROB_LIST_INFO* const get_info,
                                   DGCODE_ROB_LIST_INFO* const list_info) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_DeleteDTCInfoList(const DGCODE_DELETE_DTC_INFO* const del_info) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_DeleteRoBInfoList(const DGCODE_DELETE_ROB_INFO* const del_info) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_InitDiagCode(HANDLE hApp, uint8_t start_kind, CbFuncPtr fpOnCmd) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_SetDiagCodeMask(uint8_t onoff) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_SyncTimeStamp(uint32_t time_cnt, uint16_t trip_cnt) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_SyncDcmTime(const DGCODE_SSR_INFO_TIME_INFO * const hour_info) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_SyncOdometer(const uint32_t mileage, const uint8_t unit) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_InhibitPutDiagCode(void) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_GetDIDList(const uint64_t code, DGCODE_DID_LIST* const list) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_StartupAllStatusOfDTC(void) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}

DGCODE_RET_API Diag_ClearAllStatusOfDTC(void) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return DGCODE_RET_NORMAL;
}
