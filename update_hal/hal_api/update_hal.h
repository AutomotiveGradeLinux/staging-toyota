/*
 * @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
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

///////////////////////////////////////////////////////////////////////////////
/// \brief    This file provides support for Software Download.
///
///////////////////////////////////////////////////////////////////////////////

/**
 * @file update_hal.h
 */

/** @addtogroup update_service
 *  @{
 */
/** @addtogroup update_hal
 *  @ingroup update_service
 *  @{
 */

#ifndef INCLUDE_UPDATE_HAL_H_
#define INCLUDE_UPDATE_HAL_H_
#include <stdint.h>

/* status of VUP_COMPLETE */
#define VUP_SUCCESS       0        //!< \~english Success
#define VUP_FAIL          1        //!< \~english Failed

/* Inter process command codes */

/**
* \~english  Start all updates\n
*            Command ID that the user specifies when requesting update_hal to start all updates \n
*            This command should be sent using the interface_unified FrameworkunifiedSendMsg. \n
*            In this case, specify updater as the destination and UpdateDataInfo as the datatype. 
* \~english @attention Do not send this command from multiple processes or threads at the same time.
*/
#define VUP_REQ_VUPSTART     205

/**
* \~english  Start differential update\n
*            Command ID that the user specifies when requesting incremental update start from update_hal \n
*            This command should be sent using the interface_unified FrameworkunifiedSendMsg. \n
*            In this case, specify updater as the destination and UpdateDataInfo as the datatype. 
* \~english @attention Do not send this command from multiple processes or threads at the same time.
*/
#define VUP_REQ_DIFFVUPSTART 209

/**
* \~english  Copy of the updated side\n
*            Command ID that the user specifies when requesting update_hal to copy the update plane \n
*            This command should be sent using the interface_unified FrameworkunifiedSendMsg. \n
*            In this case, specify updater as the destination and UpdateDataInfo as the datatype. 
* \~english @attention Do not send this command from multiple processes or threads at the same time.
*/
#define VUP_REQ_COPY         216

/**
* \~english  Cancel update\n
*            Command ID that the user specifies when requesting update_hal to cancel the update \n
*            This command should be sent using the interface_unified FrameworkunifiedSendMsg. \n
*            At this time, specify updater as the destination. 
* \~english  @attention Do not send this command from multiple processes or threads at the same time.
*/
#define VUP_REQ_CANCEL       207

/**
* \~english   Update cancellation completion notice\n
*            Command ID that notifies the user that update cancellation processing has been completed from update_hal \n
*            When receiving the event of this command,
*            Use of interfaces provided by interface_unified (such as FrameworkunifiedAttachCallbackToDispatcher) 
* \~english  @attention Do not send this command from multiple processes or threads at the same time.
*/
#define VUP_RES_CANCEL       208

/**
* \~english   Progress status notification\n
*            Command ID that notifies the update progress status from update_hal to the user \n
*            When receiving the event of this command,
*            Use of interfaces provided by interface_unified (such as FrameworkunifiedAttachCallbackToDispatcher) 
* \~english  @attention Do not send this command from multiple processes or threads at the same time.
*/
#define VUP_PROGRESS         203

/**
* \~english   Update completion notice\n
*            Command ID that notifies the end of update from update_hal to the user \n
*            When receiving the event of this command,
*            Use of interfaces provided by interface_unified (such as FrameworkunifiedAttachCallbackToDispatcher) 
* \~english  @attention Do not send this command from multiple processes or threads at the same time.
*/
#define VUP_COMPLETE         204

#define UPDATE_START_TYPE_UPDATE       1    //!< \~english  Type of Update
#define UPDATE_START_TYPE_VERIFY       2    //!< \~english  Type of Verification

/* Data format of request to updater to exec update(UpdateService -> updater) */
// Formate is:
//   num_of_fileset x 1 : UI_32
//   fileset x N : UpdateDataInfo(struct) x N
#define KWI_MAX_MOD_FPATH 128                    //!< \~english Max file path of module in Detailed info
#define VUP_MAX_KWI_FNAME (KWI_MAX_MOD_FPATH+1)  //!< \~english 129 Max size of the string which to save the file path
#define KWI_SIZE_HASH        32                  //!< \~english HashSize length
#define VUP_MAX_KWI_DETAIL   15                  //!< \~english Max file path of Data infos

/**
* \~english Structure for starts update external update
*/
typedef struct _UpdateDataInfo {
  UI_32 ID;                           /*!< \~english ID originated by 0(ROOTFS:0, BOOT:1)*/
  char filename[VUP_MAX_KWI_FNAME];   /*!< \~english source file(device) name(fullpath)*/
  char srcname[VUP_MAX_KWI_FNAME];    /*!< \~english source file(device) name in diff update(fullpath)*/
  char destname[VUP_MAX_KWI_FNAME];   /*!< \~english destinamtion file(device) name(fullpath)*/
  UI_64 offset;                       /*!< \~english offset of source file*/
  UI_64 seek;                         /*!< \~english seek of destination file*/
  UI_32 size;                         /*!< \~english size of data*/
  char src_hash[KWI_SIZE_HASH];       /*!< \~english Original Data Hash(for diff update only)*/
  char dst_hash[KWI_SIZE_HASH];       /*!< \~english Restored Data Hash*/
  UI_32 num_of_div;                   /*!< \~english Num of Detail files*/
  UI_64 mod_offset;                   /*!< \~english offset of module in KWI file*/
  UI_64 div_offset;                   /*!< \~english offset of divide header in KWI file*/
} UpdateDataInfo;

/**
* \~english List of structure for starts update external update
*/
typedef struct _UpdateDataInfos {
  UI_32 units;                              /*!< \~english Num of Data Info*/
  UpdateDataInfo info[VUP_MAX_KWI_DETAIL];  /*!< \~english Data Infos*/
} UpdateDataInfos;

/**
* \~english Structure for notify update complete
*/
typedef union _ResUpdCompletion {
  // for Normal/SYS Updater
  struct {
    UI_32 type;                       /*!< \~english type*/
    UI_32 status;                     /*!< \~english result*/
  } inner;
  // for Outer Updater
  struct {
    UI_32 status;                     /*!< \~english result*/
    UI_32 need_reboot;                /*!< \~english Whether it is necessary to reboot system.*/
  } outer;
} ResUpdCompletion;


/** @}*/  // end of update_hal
/** @}*/  // end of update_service

#endif  // INCLUDE_UPDATE_HAL_H_
