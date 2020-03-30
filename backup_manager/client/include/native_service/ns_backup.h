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
 * @file ns_backup.h
 * @brief backup_manager function header file
 */

#ifndef BACKUP_MANAGER_CLIENT_INCLUDE_NATIVE_SERVICE_NS_BACKUP_H_
#define BACKUP_MANAGER_CLIENT_INCLUDE_NATIVE_SERVICE_NS_BACKUP_H_

#include <native_service/frameworkunified_types.h>

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup backup_manager
 *  @ingroup native_service
 *  @{
 */

/**
 * \~english  Service availability notify
 */
#define NTFY_BackupMgr_Availability "NS_BackupMgr/Availability"

/**
 * \~english  Return value: terminated normally
 */
#define BKUP_RET_NORMAL      0

/**
 * \~english  Return value: abnormal termination(content not specified)
 */
#define BKUP_RET_ERROR      -1

/**
 * \~english  Return value: parameter error
 */
#define BKUP_RET_ERRPARAM   -2

/**
 * \~english  Return value: initializing
 */
#define BKUP_RET_ERRINIT    -3

/**
 * \~english  Return value: terminating
 */
#define BKUP_RET_ERRTERM    -4

/**
 * \~english  Return value: data does not exist
 */
#define BKUP_RET_ERRNOENT   -5

/**
 * \~english  Return value: data size error
 */
#define BKUP_RET_ERRSIZE    -6


#ifdef __cplusplus
extern  "C" {
#endif

/**
 * \ingroup Backup_DataRd
 * \~english @par Brief
 *        Read data from backup area
 * \~english @param [in] tagID
 *        PCSTR   - Area ID(character string of less than 64 bytes)
 * \~english @param [in] uiOffset
 *        uint32_t    - Offset from the beginning of the area(0~size of backup area identified by area ID)
 * \~english @param [out] pvBuf
 *        void *    - A pointer which point to buffer used to storage read data
 * \~english @param [in] uiSize
 *        uint32_t     - Read data size(0~size of backup area identified by area ID)
 * \~english @retval BKUP_RET_NORMAL     Terminated normally
 * \~english @retval BKUP_RET_ERRPARAM   Parameter error(Sync message transmission or reception(@ref McInvokeSync) error[eFrameworkunifiedStatusInvldParam])
 * \~english @retval BKUP_RET_ERRINIT    Initializing(Sync message send or receive(@ref McInvokeSync) error [eFrameworkunifiedStatusErrOther])
 * \~english @retval BKUP_RET_ERRTERM    Terminating(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusExit])
 * \~english @retval BKUP_RET_ERRNOENT   Data dose not exist(Sync message transmission or reception(@ref McInvokeSync) fails [eFrameworkunifiedStatusFileLoadError])
 * \~english @retval BKUP_RET_ERRSIZE    Data size error(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusAccessError])
 * \~english @retval BKUP_RET_ERROR      Abnormal termination(@ref McOpenSender failed, or sync message transmission or reception(@ref McInvokeSync) error other than the above)
 * \~english @par Prerequisite
 *        - None.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - If the area ID(tagID) specified by the argument is NULL [BKUP_RET_ERRPARAM]
 *        - If the area ID(tagID) specified by the argument is 64 bytes or more than 64 bytes [BKUP_RET_ERRPARAM]
 *        - If the buffer point(pvBuf) specified by the argument is NULL [BKUP_RET_ERRPARAM]
 *        - If system call(pctrl(PR_GET_NAME)) is error [BKUP_RET_ERROR]
 *        - If failed to open the message queue for transmission [BKUP_RET_ERROR]
 *        - If failed to genarate message queue name for synchronous communication [BKUP_RET_ERROR]
 *        - If failed to open the reception message queue for synchronous communication [BKUP_RET_ERROR]
 *        - Sync message transmission or reception(@ref McInvokeSync) error
 *          - If the return value is eFrameworkunifiedStatusInvldParam [BKUP_RET_ERRPARAM]
 *          - If the return value is eFrameworkunifiedStatusErrOther [BKUP_RET_ERRINIT]
 *          - If the return value is eFrameworkunifiedStatusExit [BKUP_RET_ERRTERM]
 *          - If the return value is eFrameworkunifiedStatusFileLoadError [BKUP_RET_ERRNOENT]
 *          - If the return value is eFrameworkunifiedStatusAccessError [BKUP_RET_ERRSIZE]
 *          - If the return value is eFrameworkunifiedStatusFail, it call will synchronization message transmission(@ref McInvokeSync) again after 100 milliseconds later
 *            If the return value is eFrameworkunifiedStatusFail, it will retry BKUP_RETRY_MAX(10) times
 *          - If the return value is other value [BKUP_RET_ERROR]
 *        - If the size specified by the argument does not match the actual read size [BKUP_RET_ERROR]
 * \~english @par Detail
          - Judge the type and access destination according to specified area ID and then read data.
 *
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        Backup_DataWt
 */
int32_t Backup_DataRd(PCSTR tag_id, uint32_t ui_offset, void *pv_buf, uint32_t ui_size);
/**
 * \ingroup Backup_DataWt
 * \~english @par Brief
 *        Write data to backup area
 * \~english @param [in] tagID
 *        PCSTR   - Area ID(character string of less than 64 bytes)
 * \~english @param [in] pvBuf
 *        void *  - A pointer which point to buffer used to storage write data
 * \~english @param [in] uiOffset
 *        uint32_t    - Offset from the beginning of the area(0~size of backup area identified by area ID)
 * \~english @param [in] uiSize
 *        uint32_t     - Write data size(0~size of backup area identified by area ID)
 * \~english @retval BKUP_RET_NORMAL    Terminated normally
 * \~english @retval BKUP_RET_ERRPARAM  Parameter error(Sync message transmission or reception(@ref McInvokeSync) error[eFrameworkunifiedStatusInvldParam])
 * \~english @retval BKUP_RET_ERRINIT   Initializing(Sync message send or receive(@ref McInvokeSync) error [eFrameworkunifiedStatusErrOther])
 * \~english @retval BKUP_RET_ERRTERM   Terminating(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusExit])
 * \~english @retval BKUP_RET_ERRNOENT  Data dose not exist(Sync message transmission or reception(@ref McInvokeSync) fails [eFrameworkunifiedStatusFileLoadError])
 * \~english @retval BKUP_RET_ERRSIZE   Data size error(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusAccessError])
 * \~english @retval BKUP_RET_ERROR     Abnormal termination(@ref McOpenSender failed, or sync message transmission or reception(@ref McInvokeSync) error other than the above)
 * \~english @par Prerequisite
 *        - None.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - If the area ID(tagID) specified by the argument is NULL [BKUP_RET_ERRPARAM]
 *        - If the area ID(tagID) specified by the argument is 64 bytes or more than 64 bytes [BKUP_RET_ERRPARAM]
 *        - If the buffer point(pvBuf) specified by the argument is NULL [BKUP_RET_ERRPARAM]
 *        - If system call(mmap()) is error [BKUP_RET_ERROR]
 *        - If system call(pctrl(PR_GET_NAME)) is error [BKUP_RET_ERROR]
 *        - If failed to open the message queue for transmission [BKUP_RET_ERROR]
 *        - If failed to genarate message queue name for synchronous communication [BKUP_RET_ERROR]
 *        - If failed to open the reception message queue for synchronous communication [BKUP_RET_ERROR]
 *        - Sync message transmission or reception(@ref McInvokeSync) error
 *          - If the return value is eFrameworkunifiedStatusInvldParam [BKUP_RET_ERRPARAM]
 *          - If the return value is eFrameworkunifiedStatusErrOther [BKUP_RET_ERRINIT]
 *          - If the return value is eFrameworkunifiedStatusExit [BKUP_RET_ERRTERM]
 *          - If the return value is eFrameworkunifiedStatusFileLoadError [BKUP_RET_ERRNOENT]
 *          - If the return value is eFrameworkunifiedStatusAccessError [BKUP_RET_ERRSIZE]
 *          - If the return value is eFrameworkunifiedStatusFail, it call will synchronization message transmission(@ref McInvokeSync) again after 100 milliseconds later
 *            If the return value is eFrameworkunifiedStatusFail, it will retry BKUP_RETRY_MAX(10) times
 *          - If the return value is other value [BKUP_RET_ERROR]
 *        - If the size specified by the argument does not match the actual read size [BKUP_RET_ERROR]
 * \~english @par Detail
 *        Judge the storage media type and access destination according to specified area ID and then write data.
 *        Calling @ref Backup_DataWt more than once for the same ID when writing data separately.
 *        Data integrity can not be guaranteed at power interruption.
 *        Data should be written at one time to guarantee data consistency.
 *        The work life of writing times depends on how the user writes,NS_BackupMgr doesn't guaranteed.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        Backup_DataRd
 */
int32_t Backup_DataWt(PCSTR tag_id, void *pv_buf, uint32_t ui_offset, uint32_t ui_size);


/**
 * \ingroup Backup_DataFil
 * \~english @par Brief
 *        Write data to backup area with the specified pattern
 * \~english @param [in] tagID
 *        PCSTR   - Area ID(character string of less than 64 bytes)
 * \~english @param [in] uiOffset
 *        uint32_t    - Offset from the beginning of the area(0~size of backup area identified by area ID)
 * \~english @param [in] ucPat
 *        uint8_t    - Write pattern data(0x00~0xff)
 * \~english @param [in] uiSize
 *        uint32_t     -  Write data size(0~size of backup area identified by area ID)
 * \~english @retval BKUP_RET_NORMAL     Terminated normally
 * \~english @retval BKUP_RET_ERRPARAM   Parameter error(Sync message transmission or reception(@ref McInvokeSync) error[eFrameworkunifiedStatusInvldParam])
 * \~english @retval BKUP_RET_ERRINIT    Initializing(Sync message send or receive(@ref McInvokeSync) error [eFrameworkunifiedStatusErrOther])
 * \~english @retval BKUP_RET_ERRTERM    Terminating(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusExit])
 * \~english @retval BKUP_RET_ERRNOENT   Data dose not exist(Sync message transmission or reception(@ref McInvokeSync) fails [eFrameworkunifiedStatusFileLoadError])
 * \~english @retval BKUP_RET_ERRSIZE    Data size error(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusAccessError])
 * \~english @retval BKUP_RET_ERROR      Abnormal termination(@ref McOpenSender failed, or sync message transmission or reception(@ref McInvokeSync) error other than the above)
 * \~english @par Prerequisite
 *        - None.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - If the area ID(tagID) specified by the argument is NULL [BKUP_RET_ERRPARAM]
 *        - If the area ID(tagID) specified by the argument is 64 bytes or more than 64 bytes [BKUP_RET_ERRPARAM]
 *        - If system call(pctrl(PR_GET_NAME)) is error [BKUP_RET_ERROR]
 *        - If failed to open the message queue for transmission [BKUP_RET_ERROR]
 *        - If failed to genarate message queue name for synchronous communication [BKUP_RET_ERROR]
 *        - If failed to open the reception message queue for synchronous communication [BKUP_RET_ERROR]
 *        - Sync message transmission or reception(@ref McInvokeSync) error
 *          - If the return value is eFrameworkunifiedStatusInvldParam [BKUP_RET_ERRPARAM]
 *          - If the return value is eFrameworkunifiedStatusErrOther [BKUP_RET_ERRINIT]
 *          - If the return value is eFrameworkunifiedStatusExit [BKUP_RET_ERRTERM]
 *          - If the return value is eFrameworkunifiedStatusFileLoadError [BKUP_RET_ERRNOENT]
 *          - If the return value is eFrameworkunifiedStatusAccessError [BKUP_RET_ERRSIZE]
 *          - If the return value is eFrameworkunifiedStatusFail, it call will synchronization message transmission(@ref McInvokeSync) again after 100 milliseconds later
 *            If the return value is eFrameworkunifiedStatusFail, it will retry BKUP_RETRY_MAX(10) times
 *          - If the return value is other value [BKUP_RET_ERROR]
 *        - If the size specified by the argument does not match the actual read size [BKUP_RET_ERROR]
 * \~english @par Detail
 *        Judge the type and access destination according to specified area ID and then fill it with the specified pattern.
 *        The work life of writing times depends on how the usrs writes,NS_BackupMgr doesn't guaranteed.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        none
 */
int32_t Backup_DataFil(PCSTR tag_id, uint32_t ui_offset, uint8_t uc_pat, uint32_t ui_size);


/**
 * \ingroup Backup_DataSz
 * \~english @par Brief
 *        Get the size of the backup area.
 * \~english @param [in] tagID
 *        PCSTR   - Area ID(character string of less than 64 bytes)
 * \~english @param [out] puiSize
 *        uint32_t *   -  A pointer which point to storage size
 * \~english @retval BKUP_RET_NORMAL     Terminated normally
 * \~english @retval BKUP_RET_ERRPARAM   Parameter error(Sync message transmission or reception(@ref McInvokeSync) error[eFrameworkunifiedStatusInvldParam])
 * \~english @retval BKUP_RET_ERRINIT    Initializing(Sync message send or receive(@ref McInvokeSync) error [eFrameworkunifiedStatusErrOther])
 * \~english @retval BKUP_RET_ERRTERM    Terminating(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusExit])
 * \~english @retval BKUP_RET_ERRNOENT   Data dose not exist(Sync message transmission or reception(@ref McInvokeSync) fails [eFrameworkunifiedStatusFileLoadError])
 * \~english @retval BKUP_RET_ERRSIZE    Data size error(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusAccessError])
 * \~english @retval BKUP_RET_ERROR      Abnormal termination(@ref McOpenSender failed, or sync message transmission or reception(@ref McInvokeSync) error other than the above)
 * \~english @par Prerequisite
 *        - None.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - If the area ID(tagID) specified by the argument is NULL [BKUP_RET_ERRPARAM]
 *        - If the area ID(tagID) specified by the argument is 64 bytes or more than 64 bytes [BKUP_RET_ERRPARAM]
 *        - If the pointer(puiSize) that stores the size specified by the argument is NULL [BKUP_RET_ERRPARAM]
 *        - If system call(pctrl(PR_GET_NAME)) is error [BKUP_RET_ERROR]
 *        - If failed to open the message queue for transmission [BKUP_RET_ERROR]
 *        - If failed to genarate message queue name for synchronous communication [BKUP_RET_ERROR]
 *        - If failed to open the reception message queue for synchronous communication [BKUP_RET_ERROR]
 *        - Sync message transmission or reception(@ref McInvokeSync) error
 *          - If the return value is eFrameworkunifiedStatusInvldParam [BKUP_RET_ERRPARAM]
 *          - If the return value is eFrameworkunifiedStatusErrOther [BKUP_RET_ERRINIT]
 *          - If the return value is eFrameworkunifiedStatusExit [BKUP_RET_ERRTERM]
 *          - If the return value is eFrameworkunifiedStatusFileLoadError [BKUP_RET_ERRNOENT]
 *          - If the return value is eFrameworkunifiedStatusAccessError [BKUP_RET_ERRSIZE]
 *          - If the return value is eFrameworkunifiedStatusFail, it call will synchronization message transmission(@ref McInvokeSync) again after 100 milliseconds later
 *            If the return value is eFrameworkunifiedStatusFail, it will retry BKUP_RETRY_MAX(10) times
 *          - If the return value is other value [BKUP_RET_ERROR]
 *        - If the size specified by the argument does not match the actual size [BKUP_RET_ERROR]
 * \~english @par Detail
 *         Judge the type and access destination according to specified area ID and acquire the size of the target data.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        none
 */
int32_t Backup_DataSz(PCSTR tag_id, uint32_t *pui_size);

/**
 * \ingroup Backup_DataRdByNumID
 * \~english @par Brief
 *        Read data from backup area
 * \~english @param [in] numID
 *        uint32_t   - Area ID specified by number
 * \~english @param [in] uiOffset
 *        uint32_t    - Offset from the beginning of the area(0~size of backup area identified by area ID)
 * \~english @param [out] pvBuf
 *        void *    - A pointer which point to buffer used to storage read data
 * \~english @param [in] uiSize
 *        uint32_t     - Read data size(0~size of backup area identified by area ID)
 * \~english @retval BKUP_RET_NORMAL     Terminated normally
 * \~english @retval BKUP_RET_ERRPARAM   Parameter error(Sync message transmission or reception(@ref McInvokeSync) error[eFrameworkunifiedStatusInvldParam])
 * \~english @retval BKUP_RET_ERRINIT    Initializing(Sync message send or receive(@ref McInvokeSync) error [eFrameworkunifiedStatusErrOther])
 * \~english @retval BKUP_RET_ERRTERM    Terminating(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusExit])
 * \~english @retval BKUP_RET_ERRNOENT   Data dose not exist(Sync message transmission or reception(@ref McInvokeSync) fails [eFrameworkunifiedStatusFileLoadError])
 * \~english @retval BKUP_RET_ERRSIZE    Data size error(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusAccessError])
 * \~english @retval BKUP_RET_ERROR      Abnormal termination(@ref McOpenSender failed, or sync message transmission or reception(@ref McInvokeSync) error other than the above)
 * \~english @par Prerequisite
 *        - None.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - If the area ID(tagID) specified by the argument is NULL [BKUP_RET_ERRPARAM]
 *        - If the area ID(tagID) specified by the argument is 64 bytes or more than 64 bytes [BKUP_RET_ERRPARAM]
 *        - If the buffer point(pvBuf) specified by the argument is NULL [BKUP_RET_ERRPARAM]
 *        - If system call(pctrl(PR_GET_NAME)) is error [BKUP_RET_ERROR]
 *        - If failed to open the message queue for transmission [BKUP_RET_ERROR]
 *        - If failed to genarate message queue name for synchronous communication [BKUP_RET_ERROR]
 *        - If failed to open the reception message queue for synchronous communication [BKUP_RET_ERROR]
 *        - Sync message transmission or reception(@ref McInvokeSync) error
 *          - If the return value is eFrameworkunifiedStatusInvldParam [BKUP_RET_ERRPARAM]
 *          - If the return value is eFrameworkunifiedStatusErrOther [BKUP_RET_ERRINIT]
 *          - If the return value is eFrameworkunifiedStatusExit [BKUP_RET_ERRTERM]
 *          - If the return value is eFrameworkunifiedStatusFileLoadError [BKUP_RET_ERRNOENT]
 *          - If the return value is eFrameworkunifiedStatusAccessError [BKUP_RET_ERRSIZE]
 *          - If the return value is eFrameworkunifiedStatusFail, it call will synchronization message transmission(@ref McInvokeSync) again after 100 milliseconds later
 *            If the return value is eFrameworkunifiedStatusFail, it will retry BKUP_RETRY_MAX(10) times
 *          - If the return value is other value [BKUP_RET_ERROR]
 *        - If the size specified by the argument does not match the actual read size [BKUP_RET_ERROR]
 * \~english @par Detail
 *        Judge the type and access destination according to specified area ID and then read data.
 *        Backup Non-volatile Data Read API for DIAG
 *        Use from the module other than DIAG is prohibited.
 *
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        Backup_DataRd
 */
int32_t Backup_DataRdByNumID(uint32_t num_id, uint32_t ui_offset, void *pv_buf, uint32_t ui_size);


/**
 * \ingroup Backup_DataSzByNumID
 * \~english @par Brief
 *        Get the size of the backup area.
 * \~english @param [in] numID
 *        uint32_t   - Area ID specified by number
 * \~english @param [out] puiSize
 *        uint32_t *   -  A pointer which point to storage size
 * \~english @retval BKUP_RET_NORMAL     Terminated normally
 * \~english @retval BKUP_RET_ERRPARAM   Parameter error(Sync message transmission or reception(@ref McInvokeSync) error[eFrameworkunifiedStatusInvldParam])
 * \~english @retval BKUP_RET_ERRINIT    Initializing(Sync message send or receive(@ref McInvokeSync) error [eFrameworkunifiedStatusErrOther])
 * \~english @retval BKUP_RET_ERRTERM    Terminating(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusExit])
 * \~english @retval BKUP_RET_ERRNOENT   Data dose not exist(Sync message transmission or reception(@ref McInvokeSync) fails [eFrameworkunifiedStatusFileLoadError])
 * \~english @retval BKUP_RET_ERRSIZE    Data size error(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusAccessError])
 * \~english @retval BKUP_RET_ERROR      Abnormal termination(@ref McOpenSender failed, or sync message transmission or reception(@ref McInvokeSync) error other than the above)
 * \~english @par Prerequisite
 *        - None.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - If the area ID(tagID) specified by the argument is NULL [BKUP_RET_ERRPARAM]
 *        - If the area ID(tagID) specified by the argument is 64 bytes or more than 64 bytes [BKUP_RET_ERRPARAM]
 *        - If the pointer(puiSize) that stores the size specified by the argument is NULL [BKUP_RET_ERRPARAM]
 *        - If system call(pctrl(PR_GET_NAME)) is error [BKUP_RET_ERROR]
 *        - If failed to open the message queue for transmission [BKUP_RET_ERROR]
 *        - If failed to genarate message queue name for synchronous communication [BKUP_RET_ERROR]
 *        - If failed to open the reception message queue for synchronous communication [BKUP_RET_ERROR]
 *        - Sync message transmission or reception(@ref McInvokeSync) error
 *          - If the return value is eFrameworkunifiedStatusInvldParam [BKUP_RET_ERRPARAM]
 *          - If the return value is eFrameworkunifiedStatusErrOther [BKUP_RET_ERRINIT]
 *          - If the return value is eFrameworkunifiedStatusExit [BKUP_RET_ERRTERM]
 *          - If the return value is eFrameworkunifiedStatusFileLoadError [BKUP_RET_ERRNOENT]
 *          - If the return value is eFrameworkunifiedStatusAccessError [BKUP_RET_ERRSIZE]
 *          - If the return value is eFrameworkunifiedStatusFail, it call will synchronization message transmission(@ref McInvokeSync) again after 100 milliseconds later
 *            If the return value is eFrameworkunifiedStatusFail, it will retry BKUP_RETRY_MAX(10) times
 *          - If the return value is other value [BKUP_RET_ERROR]
 *        - If the size specified by the argument does not match the actual size [BKUP_RET_ERROR]
 * \~english @par Detail
 *         Judge the type and access destination according to specified area ID and acquire the size of the target data.
 *         Get Non-volatile backup data size API for DIAG
 *         Use from the module other than DIAG is prohibited.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        Backup_DataSz
 */
int32_t Backup_DataSzByNumID(uint32_t num_id, uint32_t *pui_size);

/**
 * \ingroup Backup_DataChk
 * \~english @par Brief
 *        Check backup data.
 * \~english @param [in] tagID
 *        PCSTR   - Area ID(character string of less than 64 bytes)
 * \~english @retval BKUP_RET_NORMAL     Terminated normally
 * \~english @retval BKUP_RET_ERRPARAM   Parameter error(Sync message transmission or reception(@ref McInvokeSync) error[eFrameworkunifiedStatusInvldParam])
 * \~english @retval BKUP_RET_ERRINIT    Initializing(Sync message send or receive(@ref McInvokeSync) error [eFrameworkunifiedStatusErrOther])
 * \~english @retval BKUP_RET_ERRTERM    Terminating(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusExit])
 * \~english @retval BKUP_RET_ERRNOENT   Data dose not exist(Sync message transmission or reception(@ref McInvokeSync) fails [eFrameworkunifiedStatusFileLoadError])
 * \~english @retval BKUP_RET_ERRSIZE    Data size error(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusAccessError])
 * \~english @retval BKUP_RET_ERROR      Abnormal termination(@ref McOpenSender failed, or sync message transmission or reception(@ref McInvokeSync) error other than the above)
 * \~english @par Prerequisite
 *        - None.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - If the area ID(tagID) specified by the argument is NULL [BKUP_RET_ERRPARAM]
 *        - If the area ID(tagID) specified by the argument is 64 bytes or more than 64 bytes [BKUP_RET_ERRPARAM]
 *        - If system call(pctrl(PR_GET_NAME)) is error [BKUP_RET_ERROR]
 *        - If failed to open the message queue for transmission [BKUP_RET_ERROR]
 *        - If failed to genarate message queue name for synchronous communication [BKUP_RET_ERROR]
 *        - If failed to open the reception message queue for synchronous communication [BKUP_RET_ERROR]
 *        - Sync message transmission or reception(@ref McInvokeSync) error
 *          - If the return value is eFrameworkunifiedStatusInvldParam [BKUP_RET_ERRPARAM]
 *          - If the return value is eFrameworkunifiedStatusErrOther [BKUP_RET_ERRINIT]
 *          - If the return value is eFrameworkunifiedStatusExit [BKUP_RET_ERRTERM]
 *          - If the return value is eFrameworkunifiedStatusFileLoadError [BKUP_RET_ERRNOENT]
 *          - If the return value is eFrameworkunifiedStatusAccessError [BKUP_RET_ERRSIZE]
 *          - If the return value is eFrameworkunifiedStatusFail, it call will synchronization message transmission(@ref McInvokeSync) again after 100 milliseconds later
 *            If the return value is eFrameworkunifiedStatusFail, it will retry BKUP_RETRY_MAX(10) times
 *          - If the return value is other value [BKUP_RET_ERROR]
 *        - If the size specified by the argument does not match the actual read size [BKUP_RET_ERROR]
 * \~english @par Detail
 *        Judge the type and access destination according to specified area ID and check the target data.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        none
 */
int32_t Backup_DataChk(PCSTR tag_id);

/**
 * \ingroup Backup_DataDel
 * \~english @par Brief
 *        Delete backup data.
 * \~english @param [in] tag_id
 *        PCSTR   - Area ID(character string of less than 64 bytes)
 * \~english @retval BKUP_RET_NORMAL     Terminated normally
 * \~english @retval BKUP_RET_ERRPARAM   Parameter error(Sync message transmission or reception(@ref McInvokeSync) error[eFrameworkunifiedStatusInvldParam])
 * \~english @retval BKUP_RET_ERRINIT    Initializing(Sync message send or receive(@ref McInvokeSync) error [eFrameworkunifiedStatusErrOther])
 * \~english @retval BKUP_RET_ERRTERM    Terminating(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusExit])
 * \~english @retval BKUP_RET_ERRNOENT   Data dose not exist(Sync message transmission or reception(@ref McInvokeSync) fails [eFrameworkunifiedStatusFileLoadError])
 * \~english @retval BKUP_RET_ERRSIZE    Data size error(Sync message transmission or reception(@ref McInvokeSync) error [eFrameworkunifiedStatusAccessError])
 * \~english @retval BKUP_RET_ERROR      Abnormal termination(@ref McOpenSender failed, or sync message transmission or reception(@ref McInvokeSync) error other than the above)
 * \~english @par Prerequisite
 *        - None.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - If the area ID(tagID) specified by the argument is NULL [BKUP_RET_ERRPARAM]
 *        - If the area ID(tagID) specified by the argument is 64 bytes or more than 64 bytes [BKUP_RET_ERRPARAM]
 *        - If system call(pctrl(PR_GET_NAME)) is error [BKUP_RET_ERROR]
 *        - If failed to open the message queue for transmission [BKUP_RET_ERROR]
 *        - If failed to genarate message queue name for synchronous communication [BKUP_RET_ERROR]
 *        - If failed to open the reception message queue for synchronous communication [BKUP_RET_ERROR]
 *        - Sync message transmission or reception(@ref McInvokeSync) error
 *          - If the return value is eFrameworkunifiedStatusInvldParam [BKUP_RET_ERRPARAM]
 *          - If the return value is eFrameworkunifiedStatusErrOther [BKUP_RET_ERRINIT]
 *          - If the return value is eFrameworkunifiedStatusExit [BKUP_RET_ERRTERM]
 *          - If the return value is eFrameworkunifiedStatusFileLoadError [BKUP_RET_ERRNOENT]
 *          - If the return value is eFrameworkunifiedStatusAccessError [BKUP_RET_ERRSIZE]
 *          - If the return value is eFrameworkunifiedStatusFail, it call will synchronization message transmission(@ref McInvokeSync) again after 100 milliseconds later
 *            If the return value is eFrameworkunifiedStatusFail, it will retry BKUP_RETRY_MAX(10) times
 *          - If the return value is other value [BKUP_RET_ERROR]
 * \~english @par Detail
 *        Judge the type and access destination according to specified area ID and delete the target data.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        none
 */
int32_t Backup_DataDel(PCSTR tag_id);

#ifdef __cplusplus
}
#endif /*__cplusplus  */

/**@}*/  // end of backup_manager
/**@}*/  // end of native_service
/**@}*/  // end of BaseSystem

#endif  // BACKUP_MANAGER_CLIENT_INCLUDE_NATIVE_SERVICE_NS_BACKUP_H_
