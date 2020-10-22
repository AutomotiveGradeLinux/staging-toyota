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
/// \brief    This file contains implementation of class CNSSharedMem.
///           This class provides API to open, read, write and close shared memory
///
////////////////////////////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file ns_sharedmem.h
 * @brief \~english This file contains implementation of class CNSSharedMem.
 *         This class provides API to open, read, write and close shared memory
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
#ifndef __NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_SHAREDMEM_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_SHAREDMEM_H__

// Used only in nstest_sharedmem.
// set environment, CNSSharedMem::Open() always fail. ===> eFrameworkunifiedStatusErrOther
#define NSTEST_FAIL_SHAREDMEM_OPEN  "NSTEST_FAIL_SHAREDMEM_OPEN"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <pthread.h>
#include <native_service/frameworkunified_types.h>
#include <string>

// Structure of the header of the shared memory buffer
typedef struct _NSSharedBuffer {
  pthread_mutex_t m_tBufMutex;    // Mutex object to mutual exclusion
  pthread_cond_t  m_tCondVar;     // Condition Variable
  UI_32           m_uiReadPtr;    // Read pointer
  UI_32           m_uiWritePtr;   // Write pointer
  UI_32           m_uiUnReadSize;  // Data size not read yet
  BOOL            m_bIsFull;      // Flag indicating if the buffer has full of data
  UI_32           m_uiShMemSize;  // Size of the shared memory buffer
} NSSharedBufferHdr;

/**
 * @class CNSSharedMem
 * \~english @brief shared memory
 * \~english @par   Brief Introduction
 *        This class is used for handle shared memory.
 *
 */
class CNSSharedMem {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMem
  /// \~english @par Brief
  ///        Constructor for CNSSharedMem
  /// \~english @param[in] f_cSharedMemName
  ///        const std::string& - name of the shared memory
  /// \~english @param[in] f_uiSize
  ///        const UI_32 - size of shared memory
  /// \~english @retval none
  /// \~english @par Preconditons
  ///        None
  /// \~english @par Change of internal status
  ///        None
  /// \~english @par Conditions of processing failure
  ///        None
  /// \~english @par Detail
  ///        Create object of CNSSharedMem class.
  ///        This class don't used to open shared memory object or allocate memory.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  /// \~english @see ~CNSSharedMem
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNSSharedMem(const std::string &f_cSharedMemName, const UI_32 f_uiSize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMem
  /// \~english @par Brief
  ///        Destructor for CNSSharedMem.
  /// \~english @retval none
  /// \~english @par Preconditons
  ///       None
  /// \~english @par Change of internal status
  ///       - Change the state of shared memory to closed state
  /// \~english @par Conditions of processing failure
  ///       None
  /// \~english @par Detail
  ///       Closes the shared memory, if it is open.
  /// \~english @par
  ///         Please note the following points when using this API for application.
  ///         - Since the destructor does not delete the shared memory area, \n
  ///           the application must execute shm_unlink to delete the shared memory area.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  /// \~english @see CNSSharedMem(const std::string&, const UI_32), CNSSharedMem(), Close
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CNSSharedMem();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMem
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
  ///         It creates the shared memory if it does not exists.
  /// \~english @par Classification
  ///         Public
  /// \~english @par Type
  ///         Open Close
  /// \~english @see Close
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Open();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMem
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
  /// \ingroup CNSSharedMem
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
  ///         - Since the destructor does not delete the shared memory area, \n
  ///           the application must execute shm_unlink to delete the shared memory area.
  /// \~english @par Classification
  ///         public
  /// \~english @par Type
  ///         Open Close
  /// \~english @see Open
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Close();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMem
  /// \~english @par Brief
  //          Read data from shared memory.
  /// \~english @param [out] buffer
  ///         PSTR - pointer to the buffer in which the read data is stored
  /// \~english @param [in] f_uilength
  ///         UI_32 - length of the data buffer provided
  /// \~english @param [in] f_bBlock
  ///         BOOL - TRUE -  blocking call
  ///                FALSE - non blocking call
  /// \~english @retval SI_32 Returns The number of bytes actually read, or NS_SHM_ERROR if an error occurred
  /// \~english @retval NS_SHM_ERROR
  /// \~english @par Preconditons
  ///         - Open the shared memory object
  /// \~english @par Change of internal status
  ///         - None
  /// \~english @par Conditions of processing failure
  ///         - The shared memory object is not opened. [NS_SHM_ERROR]
  ///         - Pointor to buffer(buffer) is NULL. [NS_SHM_ERROR]
  ///         - Byte of data(f_uilength)is 0. [NS_SHM_ERROR]
  /// \~english @par Detail
  ///         This function reads data from the shared memory.
  ///         If this function is used as blocking call, then the calling thread will get blocked
  ///         until data is available for read. By default it is a blocking call.
  /// \~english @par Classification
  ///         public
  /// \~english @par Type
  ///         Open Close
  /// \~english @see Write
  ////////////////////////////////////////////////////////////////////////////////////////////////
  SI_32 Read(PSTR buffer, const UI_32 f_uilength, const BOOL f_bBlock = TRUE);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMem
  /// \~english @par Brief
  ///        This function writes the data into the shared memory.
  /// \~english @param[in] buffer
  ///        PCSTR - pointer to the buffer containing the data to be written
  /// \~english @param[in] f_uilength
  ///        const UI_32 - length of the data buffer to be written
  /// \~english @retval SI_32 Returns The number of bytes written, or NS_SHM_ERROR if an error occurred
  /// \~english @par Preconditons
  ///         - Open the shared memory object
  /// \~english @par Change of internal status
  ///         - None
  /// \~english @par Conditions of processing failure
  ///         - The shared memory object is not opened. [NS_SHM_ERROR]
  ///         - Pointor to buffer(buffer) is NULL. [NS_SHM_ERROR]
  ///         - Size of buffer(f_uilength) is larger than the size designated by constructor. [NS_SHM_ERROR]
  /// \~english @par Detail
  ///         - This function writes the data into the shared memory.
  /// \~english @par Classification
  ///         public
  /// \~english @par Type
  ///         Open Close
  /// \~english @see Read
  ////////////////////////////////////////////////////////////////////////////////////////////////
  SI_32 Write(PCSTR buffer, const UI_32 f_uilength);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMem
  /// \~english @par Brief
  ///           Dump data to file.
  /// \~english @param [in] f_pPath
  ///            PCSTR - file path.
  /// \~english @param[in] [in] f_uiDumpSize
  ///            PUI_32 - Returns The number of bytes written into file
  /// \~english @retval EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK on success,
  ///             eFrameworkunifiedStatusFileLoadError - file path is incorrect
  ///             eFrameworkunifiedStatusSemLockFail/
  ///             eFrameworkunifiedStatusSemLockFail - mutex locking/unlocking error
  ///             eFrameworkunifiedStatusFail - shared memory is not opened
  ///             eFrameworkunifiedStatusInvldParam - invalid param
  /// \~english @par Preconditons
  ///           -
  /// \~english @par Change of internal status
  ///           -None
  /// \~english @par Conditions of processing failure
  ///           - file path is incorrect.[eFrameworkunifiedStatusFileLoadError]
  ///           - mutex locking/unlocking error.[eFrameworkunifiedStatusSemLockFail/eFrameworkunifiedStatusSemLockFail]
  ///           - shared memory is not opened.[eFrameworkunifiedStatusFail]
  ///           - invalid param.[eFrameworkunifiedStatusInvldParam]
  /// \~english @par Detail
  ///           - This function writes all the data in the buffer into provided file f_pPath.
  ///           - This function does not changes the unread buffer.
  ///           - This function overwrites the file if it exists.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           sync only
  /// \~english @par
  ///           - eFrameworkunifiedStatus:Result
  ///               - eFrameworkunifiedStatusOK:Success
  ///               - Except eFrameworkunifiedStatusOK:Failure
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus DumpToFile(PCSTR f_pPath, PUI_32 f_uiDumpSize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMem
  /// \~english @par Brief
  ///        This function gets size of unread bytes.
  /// \~english @param None
  /// \~english @retval Except NS_SHM_ERROR - Returns The number of unread bytes
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
  /// \ingroup CNSSharedMem
  /// \~english @par Brief
  ///           Clear buffer.
  /// \~english @param None
  /// \~english @retval EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK on success,
  ///             eFrameworkunifiedStatusSemLockFail/
  ///             eFrameworkunifiedStatusSemLockFail - mutex locking/unlocking error
  ///             eFrameworkunifiedStatusFail - shared memory is not opened
  /// \~english @par Preconditons
  ///           -
  /// \~english @par Change of internal status
  ///           - set m_pShmHdr->m_uiReadPtr with 0;
  ///           - set m_pShmHdr->m_uiWritePtr with 0;
  ///           - set m_pShmHdr->m_uiUnReadSize with 0;
  ///           - set m_pShmHdr->m_bIsFull with FALSE;
  /// \~english @par Conditions of processing failure
  ///           - mutex locking/unlocking error.[eFrameworkunifiedStatusSemLockFail/eFrameworkunifiedStatusSemLockFail]
  ///           - shared memory is not opened.[eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///           - This function clears the shared memory buffer.if m_pShmHdr is null,return failure.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           sync only
  /// \~english @par
  ///           - eFrameworkunifiedStatus:Result
  ///               - eFrameworkunifiedStatusOK:Success
  ///               - Except eFrameworkunifiedStatusOK:Failure
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ClearBuf();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMem
  /// \~english @par Brief
  ///           This function sets the position of read ptr to write ptr in buffer.
  /// \~english @param None
  /// \~english @retval EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK on success,
  ///             eFrameworkunifiedStatusSemLockFail/
  ///             eFrameworkunifiedStatusSemLockFail - mutex locking/unlocking error
  ///             eFrameworkunifiedStatusFail - shared memory is not opened
  /// \~english @par Preconditons
  ///           -
  /// \~english @par Change of internal status
  ///           - set m_pShmHdr->m_uiReadPtr with m_pShmHdr->m_uiWritePtr;
  ///           - set m_pShmHdr->m_uiUnReadSize with 0;
  /// \~english @par Conditions of processing failure
  ///           - mutex locking/unlocking error.[eFrameworkunifiedStatusSemLockFail/eFrameworkunifiedStatusSemLockFail]
  ///           - shared memory is not opened.[eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///           - This function sets the position of read ptr to write ptr in buffer.\n
  ///             If m_pShmHdr of lock failure or unlock failure,result is error.
  /// \~english @par Classification
  ///           public
  /// \~english @par Type
  ///           sync only
  /// \~english @par
  ///           - eFrameworkunifiedStatus:Result
  ///               - eFrameworkunifiedStatusOK:Success
  ///               - Except eFrameworkunifiedStatusOK:Failure
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetReadPtrToWritePtr();

 protected:
  // no members in protected

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CNSSharedMem
  /// Constructor of CNSSharedMem class
  ///
  /// \return     None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNSSharedMem();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CreateSMHeader
  /// This function creates the shared memory object for header.
  ///
  /// \return EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK            shared memory created successfully
  ///             eFrameworkunifiedStatusDuplicate     shared memory already exists
  ///             eFrameworkunifiedStatusInvldParam    invalid shared memory name
  ///             eFrameworkunifiedStatusFail          on error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CreateSMHeader();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CreateSMDataBuffer
  /// This function creates the shared memory object for data buffer.
  ///
  /// \return EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK            shared memory created successfully
  ///             eFrameworkunifiedStatusDuplicate     shared memory already exists
  ///             eFrameworkunifiedStatusInvldParam    invalid shared memory name
  ///             eFrameworkunifiedStatusFail          on error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CreateSMDataBuffer();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// MapSMHeader
  /// This function open and maps the shared memory in process space.
  ///
  /// \param  [out] f_pShMem
  ///         PVOID* - address of the mapped memory
  /// \param  [in] f_cShmName
  ///         std::string - Name of the shared memory
  /// \param  [in] f_uiShmSize
  ///         UI_32 - Size of the shared memory
  ///
  /// \return EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK if shared memory opened successfully
  ///             eFrameworkunifiedStatusFail unable to open shared memory
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus MapSM(PVOID *f_pShMem, const std::string &f_cShmName, const UI_32 f_uiShmSize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// UnMapSM
  /// This function unmaps the shared memory object.
  ///
  /// \param  [in] f_pShMem
  ///         PCSTR - address of the memory to unmap
  /// \param  [in] f_uiShmSize
  ///         UI_32 - Size of the memory to be unmapped
  ///
  /// \return EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK if shared memory closed successfully
  ///             eFrameworkunifiedStatusFail if shared memory is not opened
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus UnMapSM(PVOID f_pShMem, const UI_32 f_uiShmSize);

  std::string m_cShmHdrName;      // name of the shared memory header

  std::string m_cShmName;         // name of the shared memory data

  UI_32 m_uiShmBuffSize;          // shared memory size

  NSSharedBufferHdr *m_pShmHdr;   // The pointer to the shared memory header

  PCHAR m_pShmBuff;               // The pointer to the shared memory data buffer
};

#endif /* __NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_SHAREDMEM_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
