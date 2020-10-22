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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSSharedMemory
/// \brief    This file contains implementation of class CNSSharedMemReader.
///           This class provides API to open, close and perform read operation on shared memory.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file ns_sharedmem_reader.h
 * @brief \~english This file contains implementation of class CNSSharedMemReader.
 *        This class provides API to open, close and perform read operation on shared memory.
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
#ifndef __NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_SHAREDMEM_READER_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_SHAREDMEM_READER_H__

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>
#include <string>

class CNSSharedMem;

/**
 * @class CNSSharedMemReader
 * \~english @brief read shared memory
 * \~english @par   Brief Introduction
 *        This class is used for read shared memory.
 *
 */
class CNSSharedMemReader {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemReader
  /// \~english @par Brief
  ///        Constructor for CNSSharedMemReader
  /// \~english @param[in] f_cSharedMemName
  ///        const std::string& - name of the shared memory
  /// \~english @param[in]  f_bBlock
  ///         BOOL - TRUE -  blocking call for Read() API,Thread will get blocked \n
  ///                        the thread until data is available for read.
  ///                FALSE - non blocking call for Read() API
  /// \~english @retval None
  /// \~english @par Preconditons
  ///           -None
  /// \~english @par Change of internal status
  ///           - None
  /// \~english @par Conditions of processing failure
  ///           - None
  /// \~english @par Detail
  ///           Create object of CNSSharedMemReader class.
  ///           This class don't used to open shared memory object or allocate memory.
  /// \~english @par Classification
  ///           - public
  /// \~english @par Type
  ///           - None
  /// \~english @see ~CNSSharedMemReader
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNSSharedMemReader(const std::string &f_cSharedMemName, const BOOL f_bBlock = TRUE);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemReader
  /// \~english @par Brief
  ///        Destructor for CNSSharedMemReader.
  /// \~english @param None
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           - Change the state of shared memory to closed state
  /// \~english @par Conditions of processing failure
  ///           -None
  /// \~english @par Detail
  ///       Closes the shared memory, if it is open.
  /// \~english @par
  ///         Please note the following points when using this API for application.
  ///         - Since the destructor does not delete the shared memory area, \n
  ///           the application must execute shm_unlink to delete the shared memory area.
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           -None
  /// \~english @see CNSSharedMemReader(const std::string&, const BOOL), CNSSharedMemReader(), Close
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CNSSharedMemReader();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemReader
  /// \~english @par Brief
  ///        This function opens and maps the shared memory object.
  /// \~english @retval eFrameworkunifiedStatusOK if shared memory opened successfully
  /// \~english @retval eFrameworkunifiedStatusFail unable to open shared memory
  /// \~english @retval eFrameworkunifiedStatusErrOther if shared memory already opened
  /// \~english @par Preconditons
  ///         None
  /// \~english @par Change of internal status
  ///         - Open shared memory object
  /// \~english @par Conditions of processing failure
  ///         - if shared memory already opened [eFrameworkunifiedStatusErrOther]
  ///         - Unable to open shared memory(shm_open) [eFrameworkunifiedStatusFail]
  ///         - Unable to create shared memory(mmap) [eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///         Opens a shared memory object and allocates it to a memory space.\n
  ///         It creates the shared memory if it does not exists.
  /// \~english @par Classification
  ///         Public
  /// \~english @par Type
  ///         Open Close
  /// \~english @see Close
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Open();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemReader
  /// \~english @par Brief
  ///        This function is used to check whether the shared memory buffer is opened or not.
  /// \~english @retval TRUE - Open
  /// \~english @retval FALSE - Not open
  /// \~english @par Preconditons
  ///         - None
  /// \~english @par Change of internal status
  ///         - None
  /// \~english @par Conditions of processing failure
  ///         - None
  /// \~english @par Detail
  ///         - Check whether the shared memory buffer is opened or not, and return the result.
  /// \~english @par Classification
  ///         public
  /// \~english @par Type
  ///         Open Close
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsOpen();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemReader
  /// \~english @par Brief
  ///        This function unmaps the shared memory object.
  /// \~english @retval eFrameworkunifiedStatusOK if shared memory closed successfully
  /// \~english @retval eFrameworkunifiedStatusFail if shared memory is not opened
  /// \~english @par Preconditons
  ///         - Open the shared memory object
  /// \~english @par Change of internal status
  ///         - Close the shared memory object
  /// \~english @par Conditions of processing failure
  ///         - The shared memory object is not opened. [eFrameworkunifiedStatusFail]
  ///         - Unable to unmap(munmap) the shared memory object [eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///         - This function unmaps the shared memory object from memory space.
  /// \~english @par
  ///         Please note the following points when using this API for application.
  ///         - Since the closing does not delete the shared memory area, \n
  ///           the application must execute shm_unlink to delete the shared memory area.
  /// \~english @par Classification
  ///         public
  /// \~english @par Type
  ///         Open Close
  /// \~english @see Open
  ////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Close();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemReader
  /// \~english @par Brief
  ///        This function reads data from the shared memory.
  /// \~english @param[in] buffer
  ///        PSTR - pointer to the buffer in which the read data is stored
  /// \~english @param[in] f_uilength
  ///        const UI_32 - length of the data buffer provided
  /// \~english @retval The number of bytes actually read, or NS_SHM_ERROR if an error occurred
  /// \~english @par Preconditons
  ///        - Open the shared memory object
  /// \~english @par Change of internal status
  ///        - None
  /// \~english @par Conditions of processing failure
  ///         - The shared memory object is not opened. [NS_SHM_ERROR]
  ///         - Pointor to buffer(buffer) is NULL. [NS_SHM_ERROR]
  ///         - Byte of data(f_uilength)is 0. [NS_SHM_ERROR]
  /// \~english @par Detail
  ///       If this constructor is used as blocking call, then the calling thread will get blocked\n
  ///       until data is available for read.
  /// \~english @par Classification
  ///         public
  /// \~english @par Type
  ///         Open Close
  /// \~english @see Write
  ////////////////////////////////////////////////////////////////////////////////////////////////
  SI_32 Read(PSTR buffer, const UI_32 f_uilength);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemReader
  /// \~english @par Brief
  ///        This function writes all the data in the buffer into provided file f_pPath.
  /// \~english @param[in] f_pPath
  ///        PCSTR - file path.
  /// \~english @param[in] f_uiDumpSize
  ///        PUI_32 - Returns The number of bytes written into file
  /// \~english @retval eFrameworkunifiedStatusOK - on success
  /// \~english @retval eFrameworkunifiedStatusNullPointer
  /// \~english @retval eFrameworkunifiedStatusInvldParam - invalid param
  /// \~english @retval eFrameworkunifiedStatusFail - shared memory is not open
  /// \~english @retval eFrameworkunifiedStatusFileLoadError - file path is incorrect
  /// \~english @retval eFrameworkunifiedStatusErrOther
  /// \~english @retval eFrameworkunifiedStatusSemUnlockFail
  /// \~english @par Preconditons
  ///        - File is opened.
  /// \~english @par Change of internal status
  ///        - None
  /// \~english @par Conditions of processing failure
  ///       - Pointor to dump size(f_uiDumpSize)is NULL [eFrameworkunifiedStatusNullPointer]
  ///       - Dump file path is NULL [eFrameworkunifiedStatusInvldParam]
  ///       - File is not opened [eFrameworkunifiedStatusFail]
  ///       - Failed to open dump file [eFrameworkunifiedStatusFileLoadError]
  ///       - Failed to access shared memory [eFrameworkunifiedStatusFail]
  ///       - Failed to write data into dump file [eFrameworkunifiedStatusErrOther]
  ///       - Failed to lock mutex [eFrameworkunifiedStatusSemLockFail]
  ///       - Failed to unlock mutex [eFrameworkunifiedStatusSemUnlockFail]
  /// \~english @par Detail
  ///       This function does not changes the unread buffer.
  /// \~english @par
  ///       This function overwrites the file if it exists.
  /// \~english @par Classification
  ///       public
  /// \~english @par Type
  ///       none
  /// \~english @see Open
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus DumpToFile(PCSTR f_pPath, PUI_32 f_uiDumpSize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemReader
  /// \~english @par Brief
  ///        This function gets size of unread .bytes.
  /// \~english @param None
  /// \~english @retval Except NS_SHM_ERROR - Returns The number of bytes written,
  /// \~english @retval NS_SHM_ERROR  -if an error occurred
  /// \~english @par Preconditons
  ///           -
  /// \~english @par Change of internal status
  ///           None
  /// \~english @par Conditions of processing failure
  ///           if m_pShmHdr is null.[NS_SHM_ERROR]
  /// \~english @par Detail
  ///           This function returns the number of unread bytes which can be read by Read().
  /// \~english @par Classification
  ///           Public
  /// \~english @par Type
  ///           sync only
  /// \~english @par
  ///           - l_uiReadSize:Result
  ///               - NS_SHM_ERROR:Failure
  ///               - Except NS_SHM_ERROR:Success
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////////////////////
  SI_32 GetSize();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemReader
  /// \~english @par Brief
  ///        This function sets the position of read ptr to write ptr in buffer.
  /// \~english @retval eFrameworkunifiedStatusOK - on success
  /// \~english @retval eFrameworkunifiedStatusFail - shared memory is not open
  /// \~english @retval eFrameworkunifiedStatusSemLockFail - mutex locking error
  /// \~english @retval eFrameworkunifiedStatusSemUnlockFail - mutex unlocking error
  /// \~english @par Preconditons
  ///        - File is opened.
  /// \~english @par Change of internal status
  ///        - None
  /// \~english @par Conditions of processing failure
  ///        - File is not opened [eFrameworkunifiedStatusFail]
  ///        - Failed to lock mutex [eFrameworkunifiedStatusSemLockFail]
  ///        - Failed to unlock mutex [eFrameworkunifiedStatusSemUnlockFail]
  /// \~english @par Detail
  ///           - Set the position of read ptr to write ptr in buffer.\n
  ///           - Initialize state of unreading bytes to 0.
  /// \~english @par Classification
  ///           - public
  /// \~english @par Type
  ///         Open Close
  /// \~english @see Open
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetReadPtrToWritePtr();

 protected:
  // no members in protected

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CNSSharedMemReader
  /// Constructor of CNSSharedMemReader class
  ///
  /// \return     None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNSSharedMemReader();

  CNSSharedMem *m_pShmReader;  // pointer to the CNSSharedMem object

  BOOL m_bBlock;              // blocking or non blocking read
};

#endif  /*__NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_SHAREDMEM_READER_H__*/  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
