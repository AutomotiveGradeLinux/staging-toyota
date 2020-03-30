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
/// \ingroup  tag_NSRingBuffer
/// \brief    This file contains implementation of class CNSRingBuffer.
///           This class provides API to open, read, write and close ring buffer
///
////////////////////////////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file ns_ringbuffer.h
 * @brief \~english This file contains implementation of class CNSRingBuffer.
 *        This class provides API to open, read, write and close ring buffer
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
#ifndef __NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_RINGBUFFER_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_RINGBUFFER_H__

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <pthread.h>

#include <native_service/frameworkunified_types.h>

#include <string>

// Structure of the header of the ring buffer
typedef struct _NSRingBufferHdr {
  UI_32           m_uiReadPtr;        // Read pointer
  UI_32           m_uiWritePtr;       // Write pointer
  UI_32           m_uiUnReadSize;     // Data size not read yet
  BOOL            m_bIsFull;          // Flag indicating if the buffer has full of data
  UI_32           m_uiRingBufferSize;  // Size of the ring buffer
} NSRingBufferHdr;

// Structure of the mutex of the ring buffer
typedef struct _NSRingBufferMtx {
  pthread_mutex_t m_tBufMutex;        // Mutex object to mutual exclusion
  int m_lid;
} NSRingBufferMtx;


/**
 * @class CNSRingBuffer
 * \~english @brief CNSRingBuffer
 * \~english @par   Brief Introduction
 *        Class to provide class of ring buffer.
 */
class CNSRingBuffer {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRingBuffer
  /// \~english @par Brief
  ///        Constructor for CNSRingBuffer
  /// \~english @param[in] f_cMappedFilePath
  ///        const std::string& - path of the ring buffer mapped file
  /// \~english @param[in] f_uiSize
  ///        const UI_32 - size of ring buffer
  /// \~english @param[in] f_lid
  ///        int - Lock ID
  /// \~english @retval none
  /// \~english @par Preconditons
  ///         none
  /// \~english @par Change of internal status
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///         none
  /// \~english @par Detail
  ///       Create an instance of CNSRingBuffer class.\n
  ///       Initializes cass member (m_cMappedFilePath, m_uiRingBuffSize, m_pRbufMtx, m_pRbufHdr,
  ///       m_pRbuf, m_lid, m_pLockAddr, m_siProcessLastWrtPage, m_cRbufMtxName).\n
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       Constructor of CNSRingBuffer class.
  ///       variables (m_cMappedFilePath, m_uiRingBuffSize, m_pRbufMtx, m_pRbufHdr,
  ///       m_pRbuf, m_lid, m_pLockAddr, m_siProcessLastWrtPage, m_cRbufMtxName) to be initialization.
  /// \~english @see  ~CNSRingBuffer
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNSRingBuffer(const std::string &f_cMappedFilePath, const UI_32 f_uiSize, int f_lid = -1);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRingBuffer
  /// \~english @par Brief
  ///        Destructor for CNSRingBuffer.
  /// \~english @param none
  /// \~english @retval none
  /// \~english @par Preconditons
  ///       - The constructor creates an instance of the CNSRingBuffer class.
  /// \~english @par Change of internal status
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///         none
  /// \~english @par Detail
  ///       Closes the ring buffer, if it is open.
  /// \~english @par
  ///       Closes the ring buffer if it is open.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @see CNSRingBuffer
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CNSRingBuffer();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRingBuffer
  /// \~english @par Brief
  ///        This function opens and maps the ring buffer object.
  /// \~english @param none
  /// \~english @retval eFrameworkunifiedStatusOK
  /// \~english @retval eFrameworkunifiedStatusInvldParam
  /// \~english @retval eFrameworkunifiedStatusFail
  /// \~english @retval eFrameworkunifiedStatusErrOther
  /// \~english @par Preconditons
  ///         none
  /// \~english @par Change of internal status
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - Different Lock ID in constructor parameter and mutex object  [eFrameworkunifiedStatusInvldParam]
  ///       - If the path at the time of CNSRingBuffer class instantiation is empty [eFrameworkunifiedStatusFail]
  ///       - When the CNSRingBuffer class is instantiated with a path longer than 255 characters... [eFrameworkunifiedStatusFail]
  ///       - If accessing the information header file (shm_open/ mmap) fails [eFrameworkunifiedStatusFail]
  ///       - When accessing the ring buffer file (shm_open/ mmap) fails [eFrameworkunifiedStatusFail]
  ///       - Accessing a mutex file (shm_open/ mmap) fails [eFrameworkunifiedStatusFail]
  ///       - Alreadey opened [eFrameworkunifiedStatusErrOther]
  /// \~english @par Detail
  ///       It creates the ring buffer if it does not exists.
  ///       Opens the path information header, ring buffer, and mutex set at instance creation.
  /// \~english @par
  ///       Update ring buffer size for instances created with different sizes on the same path.
  /// \~english @par
  ///       Execute close() to close.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @see CNSRingBuffer, Close
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Open();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRingBuffer
  /// \~english @par Brief
  ///        This function is used to check whether the ring buffer is opened or not..
  /// \~english @param none
  /// \~english @retval TRUE - Open
  /// \~english @retval FALSE - Not open
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///         none
  /// \~english @par Detail
  ///       This function is used to check whether the ring buffer is opened or not.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @see CNSRingBuffer, Close
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsOpen();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRingBuffer
  /// \~english @par Brief
  ///        This function unmaps the ring buffer object.
  /// \~english @param none
  /// \~english @retval eFrameworkunifiedStatusOK
  /// \~english @retval eFrameworkunifiedStatusFail
  /// \~english @par Preconditons
  ///       - Open has been performed.
  /// \~english @par Change of internal status
  ///       - The internal state is not changed.
  /// \~english @par Conditions of processing failure
  ///       - not open [eFrameworkunifiedStatusFail]
  ///       - information header file munmap fail [eFrameworkunifiedStatusFail]
  ///       - ring buffer file munmap fail [eFrameworkunifiedStatusFail]
  ///       - minidesk file munmap fail [eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///       - This function unmaps the ring buffer object.
  /// \~english @par
  ///       Close processing is also performed when an instance is destructed to prevent leakage.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @see Open
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Close();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRingBuffer
  /// \~english @par Brief
  ///        This function reads data from the ring buffer.
  /// \~english @param [OUT] buffer
  ///         PSTR - pointer to the buffer in which the read data is stored
  /// \~english @param [IN] f_uilength
  ///         UI_32 - length of the data buffer provided
  /// \~english @retval SI_32 Returns The number of bytes actually read
  /// \~english @retval -1 error occurred
  /// \~english @par Preconditions
  ///       - Open().
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///       - not open [-1]
  /// \~english @par Detail
  ///       - This function reads data from the ring buffer.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @see Open
  ////////////////////////////////////////////////////////////////////////////////////////////////
  SI_32 Read(PSTR buffer, const UI_32 f_uilength);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRingBuffer
  /// \~english @par Brief
  ///        This function writes the data into the ring buffer.
  /// \~english @param [OUT] buffer
  ///         PSTR - pointer to the buffer containing the data to be written
  /// \~english @param [IN] f_uilength
  ///         UI_32 - length of the data buffer to be written
  /// \~english @retval SI_32 Returns The number of bytes written
  /// \~english @retval -1 error occurred
  /// \~english @par Preconditions
  ///       - Open().
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///       - not open [-1]
  /// \~english @par Detail
  ///       - This function writes the data into the ring buffer.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @see Open
  ////////////////////////////////////////////////////////////////////////////////////////////////
  SI_32 Write(PCSTR buffer, const UI_32 f_uilength);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRingBuffer
  /// \~english @par Brief
  ///        This function writes all the data in the buffer into provided file f_pPath.
  /// \~english @param[in] f_pPath
  ///        PCSTR - file path.
  /// \~english @param[in] f_uiDumpSize
  ///        PUI_32 - Returns The number of bytes written into file
  /// \~english @retval eFrameworkunifiedStatusOK
  /// \~english @retval eFrameworkunifiedStatusNullPointer
  /// \~english @retval eFrameworkunifiedStatusInvldParam
  /// \~english @retval eFrameworkunifiedStatusFail
  /// \~english @retval eFrameworkunifiedStatusFileLoadError
  /// \~english @retval eFrameworkunifiedStatusErrOther
  /// \~english @par Preconditions
  ///       - Open().
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///       - f_uiDumpSize is NULL [eFrameworkunifiedStatusNullPointer]
  ///       - dump file path is NULL [eFrameworkunifiedStatusInvldParam]
  ///       - not open  [eFrameworkunifiedStatusFail]
  ///       - dump file open fail [eFrameworkunifiedStatusFileLoadError]
  ///       - ring buffer access fail [eFrameworkunifiedStatusFail]
  ///       - dump file write fail [eFrameworkunifiedStatusErrOther]
  /// \~english @par Detail
  ///       This function does not changes the unread buffer.
  /// \~english @par
  ///       Please note the following points when using this API for application.
  ///       - Writing to a dump file is overwritten.
  /// \~english @par
  ///       This function overwrites the file if it exists.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @see Open
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus DumpToFile(PCSTR f_pPath, PUI_32 f_uiDumpSize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRingBuffer
  /// \~english @par Brief
  ///        This function returns the number of unread bytes which can be read by Read().
  /// \~english @retval SI_32 Returns The number of bytes which can be read by Read()
  /// \~english @retval -1 error occurred
  /// \~english @par Preconditions
  ///       - Open().
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///       - not open [-1]
  /// \~english @par Detail
  ///       - This function returns the number of unread bytes which can be read by Read().
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @see Open
  ////////////////////////////////////////////////////////////////////////////////////////////////
  SI_32 GetSize();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRingBuffer
  /// \~english @par Brief
  ///        This function clears the ring buffer.
  /// \~english @param none
  /// \~english @retval eFrameworkunifiedStatusOK
  /// \~english @retval eFrameworkunifiedStatusFail
  /// \~english @par Preconditions
  ///       - Open().
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///       - not open [eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///        This function clears the ring buffer.
  /// \~english @par
  ///        Please note the following points when using this API for application.
  ///        - Only read/write information cleared, and the size of the ring buffer itself is not changed.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @see Open
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ClearBuf();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRingBuffer
  /// \~english @par Brief
  ///        This function sets the position of read ptr to write ptr in buffer.
  /// \~english @param none
  /// \~english @retval eFrameworkunifiedStatusOK success
  /// \~english @retval eFrameworkunifiedStatusFail ring buffer is not open
  /// \~english @par Preconditions
  ///       - Open().
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///       - not open [eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///        This function sets the position of read ptr to write ptr in buffer.
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @see Open
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetReadPtrToWritePtr();

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CNSRingBuffer
  /// Constructor of CNSRingBuffer class
  ///
  /// \return     None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNSRingBuffer();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CreateRBMutex
  /// This function creates the ring buffer mutex object.
  ///
  /// \return EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK            ring buffer created successfully
  ///             eFrameworkunifiedStatusDuplicate     ring buffer already exists
  ///             eFrameworkunifiedStatusInvldParam    invalid ring buffer name
  ///             eFrameworkunifiedStatusFail          on error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CreateRBMutex();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CreateRBHeader
  /// This function creates the ring buffer object for header.
  ///
  /// \return EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK            ring buffer created successfully
  ///             eFrameworkunifiedStatusDuplicate     ring buffer already exists
  ///             eFrameworkunifiedStatusInvldParam    invalid ring buffer name
  ///             eFrameworkunifiedStatusFail          on error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CreateRBHeader();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CreateRBDataBuffer
  /// This function creates the ring buffer object for data buffer.
  ///
  /// \return EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK            ring buffer created successfully
  ///             eFrameworkunifiedStatusDuplicate     ring buffer already exists
  ///             eFrameworkunifiedStatusInvldParam    invalid ring buffer name
  ///             eFrameworkunifiedStatusFail          on error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CreateRBDataBuffer();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// MapRBHeader
  /// This function open and maps the file in process space.
  ///
  /// \param  [out] f_pRbuf
  ///         PVOID* - address of the mapped memory
  /// \param  [in] f_cMappedFile
  ///         std::string - Path of the ring buffer mapped file
  /// \param  [in] f_uiRbufSize
  ///         UI_32 - Size of the ring buffer
  ///
  /// \return EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK if ring buffer opened successfully
  ///             eFrameworkunifiedStatusFail unable to open ring buffer
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Map(PVOID *f_pRbuf, const std::string &f_cMappedFile, const UI_32 f_uiRbufSize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// MapRBHeader
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
  /// UnMapRB
  /// This function unmaps the object.
  ///
  /// \param  [in] f_pRbuf
  ///         PCSTR - address of the memory to unmap
  /// \param  [in] f_uiRbufSize
  ///         UI_32 - Size of the memory to be unmapped
  ///
  /// \return EFrameworkunifiedStatus
  ///             eFrameworkunifiedStatusOK if ring buffer closed successfully
  ///             eFrameworkunifiedStatusFail if ring buffer is not open
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus UnMap(PVOID f_pRbuf, const UI_32 f_uiRbufSize);

  EFrameworkunifiedStatus CheckSize(int fd, off_t size);


//  void LockMtx();
  int LockMtx();

  void UnlockMtx();

  std::string m_cRbufMtxName;       // name of the ring buffer mutex name

  std::string m_cMappedFileHdrPath;  // path of the ring buffer mapped file header

  std::string m_cMappedFilePath;    // path of the ring buffer mapped file

  UI_32 m_uiRingBuffSize;           // ring buffer size

  NSRingBufferMtx *m_pRbufMtx;      // The pointer to the ring buffer mutex

  NSRingBufferHdr *m_pRbufHdr;      // The pointer to the ring buffer header

  PCHAR m_pRbuf;                    // The pointer to the ring buffer data buffer

  pthread_mutex_t m_tOpenMutex;     // for multithread process

  int m_lid;                        // Lock ID
  void *m_pLockAddr;                // Lock Address

  SI_32 m_siProcessLastWrtPage;     // to hold page that each process wrote in last
};

#endif /* __NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_RINGBUFFER_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
