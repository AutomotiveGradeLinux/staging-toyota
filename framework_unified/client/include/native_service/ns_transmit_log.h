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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSSharedMemory
/// \brief    This file contains declaration of wrapper C type API's for class CNSSharedMem
///           to read and write transmit log to shared memory.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file ns_transmit_log.h
 * @brief \~english This file contains declaration of wrapper C type API for reading and writing
 *        transmit log on shared memory.
 *
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
/** @addtogroup native
 *  @ingroup framework_unified
 *  @{
 */
#ifndef __NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_SHAREDMEM_LOG_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_SHAREDMEM_LOG_H__

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>

#ifdef __cplusplus
extern "C" {
#endif



#define TRANSMIT_LOG_SHAREDMEM_NAME         "/TransmitLogShBuf"
#define TRANSMIT_LOG_SHAREDMEM_SIZE         5242880         // 5MB

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NSSharedMemTransmitLogOpen
/// \~english @par Brief
///           Open the shared memory for transmit logging.
/// \~english @param[in]
/// \~english @retval eFrameworkunifiedStatusOK if shared memory is opened.
/// \~english @retval eFrameworkunifiedStatusNullPointer if memory allocation error while creating shared memory pointer.
/// \~english @retval eFrameworkunifiedStatusFail if shared memory not open
/// \~english @par Preconditons
///           - None
/// \~english @par Change of internal status
///           - Open shared memory object
/// \~english @par Conditions of processing failure
///           - Memory allocation error while creating shared memory pointer.[eFrameworkunifiedStatusNullPointer]
///           - If shared memory not open.[eFrameworkunifiedStatusFail]
/// \~english @par Detail
///            Open the shared memory for transmit logging.\n
///            If shared memory is not exist,then create a shared memory,and open.
/// \~english @par Classification
///           - public
/// \~english @par Type
///           - sync only
/// \~english @par
///           - eFrameworkunifiedStatus:Result
///               - eFrameworkunifiedStatusOK:Success
///               - Except eFrameworkunifiedStatusOK:Failure
/// \~english @see NSSharedMemTransmitLogClose
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSSharedMemTransmitLogOpen();

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NSSharedMemTransmitLogClose
/// \~english @par Brief
///           Close the transmit logging shared memory
/// \~english @param[in]
/// \~english @retval eFrameworkunifiedStatusOK if shared memory is closed
/// \~english @retval eFrameworkunifiedStatusNullPointer if shared memory pointer is NULL.
/// \~english @retval eFrameworkunifiedStatusFail if shared memory was not closed.
/// \~english @par Preconditons
///           - None
/// \~english @par Change of internal status
///           -Open shared memory object
/// \~english @par Conditions of processing failure
///           - shared memory pointer is NULL.[eFrameworkunifiedStatusNullPointer]
///           - If shared memory not close.[eFrameworkunifiedStatusFail]
/// \~english @par Detail
///            Close the transmit logging shared memory.\n
///            If shared memory closed failure or shared memory don't exist,then return failure.
/// \~english @par Classification
///           - public
/// \~english @par Type
///           - sync only
/// \~english @par
///           - eFrameworkunifiedStatus:Result
///               - eFrameworkunifiedStatusOK:Success
///               - Except eFrameworkunifiedStatusOK:Failure
/// \~english @see NSSharedMemTransmitLogOpen
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSSharedMemTransmitLogClose();

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NSSharedMemReadTransmitLog
/// \~english @par Brief
///           Reads transmit log from the shared memory buffer.
/// \~english @param [in] f_pBuffer
///           PSTR - pointer to the buffer in which data to be read.
/// \~english @param [in] f_uiLength
///           UI_32 - length of the data to be read.
/// \~english @param [in] f_bBlock
///           BOOL - TRUE -  blocking call
///                  FALSE - non blocking call
/// \~english @retval Except NS_SHM_ERROR - Indicates the number of bytes read.
/// \~english @retval NS_SHM_ERROR  -if shared memory is not opened.
/// \~english @par Preconditons
///           -None
/// \~english @par Change of internal status
///           None
/// \~english @par Conditions of processing failure
///           if g_pTransmitLogSharedBuf or f_pBuffer is null.[NS_SHM_ERROR]
/// \~english @par Detail
///           If this function is used as blocking call,\n
///           then the calling thread will get blocked until data is available for read.
/// \~english @par Classification
///           Public
/// \~english @par Type
///           sync only
/// \~english @par
///           - l_iReadSize:Result
///               - NS_SHM_ERROR:Failure
///               - Except NS_SHM_ERROR:Success
/// \~english @see NSSharedMemWriteTransmitLog
////////////////////////////////////////////////////////////////////////////////////////////////
SI_32 NSSharedMemReadTransmitLog(PSTR f_pBuffer, const UI_32 f_uiLength, const BOOL f_bBlock);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NSSharedMemWriteTransmitLog
/// \~english @par Brief
///           Write transmit log into the shared memory buffer.
/// \~english @param [in] f_pBuffer
///           PSTR - pointer to the buffer containing the data to be written.
/// \~english @param [in] f_uiLength
///           UI_32 - length of the data buffer to be written.
/// \~english @retval Except NS_SHM_ERROR - Indicates the number of bytes written,
/// \~english @retval NS_SHM_ERROR  -if shared memory is not opened.
/// \~english @par Preconditons
///           -None
/// \~english @par Change of internal status
///           None
/// \~english @par Conditions of processing failure
///           if g_pTransmitLogSharedBuf or f_pBuffer is null.[NS_SHM_ERROR]
/// \~english @par Detail
///           If this function is used as blocking call,\n
///           then the calling thread will get blocked until data is available for write.
/// \~english @par Classification
///           Public
/// \~english @par Type
///           sync only
/// \~english @par
///           - l_iReadSize:Result
///               - NS_SHM_ERROR:Failure
///               - Except NS_SHM_ERROR:Success
/// \~english @see NSSharedMemReadTransmitLog
////////////////////////////////////////////////////////////////////////////////////////////////
SI_32 NSSharedMemWriteTransmitLog(PCSTR f_pBuffer, const UI_32 f_uiLength);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NSSharedMemDumpTransmitLogToFile
/// \~english @par Brief
///           Dump the transmit logging shared memory to file
/// \~english @param [in] f_pPath
///           PCSTR - full file path
/// \~english @param [in] f_puiSize
///           PCSTR - Size of data dumped to file
/// \~english @retval eFrameworkunifiedStatusOK if shared memory is closed
/// \~english @retval eFrameworkunifiedStatusFileLoadError file path is incorrect
/// \~english @retval eFrameworkunifiedStatusSemLockFail/
///                   eFrameworkunifiedStatusSemUnLockFail - mutex locking/unlocking error
/// \~english @retval eFrameworkunifiedStatusFail shared memory is not open
/// \~english @par Preconditons
///           - None
/// \~english @par Change of internal status
///           -Open shared memory object
/// \~english @par Conditions of processing failure
///           - shared memory pointer is NULL.[eFrameworkunifiedStatusNullPointer]
///           - mutex locking/unlocking error.[eFrameworkunifiedStatusSemLockFail/eFrameworkunifiedStatusSemUnLockFail]
///           - shared memory is not open.[eFrameworkunifiedStatusFail]
///           - file path is incorrect.[eFrameworkunifiedStatusFileLoadError]
/// \~english @par Detail
///            Writes all the data in the buffer into provided file path.\n
///            Overwrites the file if it exists.
/// \~english @par Classification
///           - public
/// \~english @par Type
///           - sync only
/// \~english @par
///           - eFrameworkunifiedStatus:Result
///               - eFrameworkunifiedStatusOK:Success
///               - Except eFrameworkunifiedStatusOK:Failure
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSSharedMemDumpTransmitLogToFile(PCSTR f_pPath, PUI_32 f_puiDumpSize);

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NSSharedMemTransmitLogIsOpen
/// \~english @par Brief
///        This function is used to check whether the shared memory buffer is opened or not.
/// \~english @retval TRUE - Open
/// \~english @retval FALSE - Not open
/// \~english @par Preconditons
///           - None
/// \~english @par Change of internal status
///           - None
/// \~english @par Conditions of processing failure
///           - None
/// \~english @par Detail
///           - This function is used to check whether the shared memory buffer is opened or not.\n
///             Used only in nstest_sharedmem.
/// \~english @par Classification
///           - public
/// \~english @par Type
///         Sync only
/// \~english @see none
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL NSSharedMemTransmitLogIsOpen();

#ifdef __cplusplus
}
#endif

#endif /* __NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_SHAREDMEM_LOG_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
