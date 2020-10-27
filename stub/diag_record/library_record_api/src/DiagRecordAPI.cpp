/*
 * @copyright Copyright (c) 2019-2020 TOYOTA MOTOR CORPORATION.
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

#include "PFDREC_API.h"

RET_PFDREC_API PFDREC_FuncStartStop(E_PFDREC_FUNC status) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_DeleteData(PFDREC_CALLBACK_FUNC func) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_DeleteIndividual(E_PFDREC_CATEGORY_TYPE category) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_ComDataBackup(E_PFDREC_CATEGORY_TYPE category,
                                    time_t   time_info,
                                    uint32_t frame_num,
                                    uint8_t* data) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_ReadMemData(E_PFDREC_CATEGORY_TYPE category,
                                    uint32_t offset,
                                    uint32_t read_num,
                                    uint8_t* data,
                                    uint32_t* out_num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_GetDataInfo(E_PFDREC_CATEGORY_TYPE category,
                                    uint32_t* frame_num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_GetFuncStartStop(E_PFDREC_FUNC* status) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_OutputData(void) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_ReadData(E_PFDREC_CATEGORY_TYPE category,
                                uint64_t unique_id,
                                uint32_t read_num,
                                uint8_t* data,
                                uint32_t* out_num,
                                uint64_t* end_unique_id) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_SearchData(E_PFDREC_CATEGORY_TYPE category,
                                    time_t time_info,
                                    uint64_t* unique_id,
                                    uint8_t*  data) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}
RET_PFDREC_API PFDREC_OpenSession(HANDLE h_app) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_CloseSession(HANDLE h_app) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_OverwriteData(E_PFDREC_CATEGORY_TYPE category,
                                    uint32_t offset,
                                    uint8_t* data) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_SetExtractionMode(const E_PFDREC_CATEGORY_TYPE category,
                                        const E_PFDREC_EXTRACT_MODE mode) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_SetExtractionMode_Exp(const E_PFDREC_CATEGORY_TYPE category,
                                            const E_PFDREC_EXTRACT_MODE mode) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_ComDataBackupY2K38(E_PFDREC_CATEGORY_TYPE category,
                                            uint32_t time_info,
                                            uint32_t frame_num,
                                            uint8_t* data) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}

RET_PFDREC_API PFDREC_SearchDataY2K38(E_PFDREC_CATEGORY_TYPE category,
                                        uint32_t  time_info,
                                        uint64_t* unique_id,
                                        uint8_t*  data) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_PFDREC_NORMAL;
}
