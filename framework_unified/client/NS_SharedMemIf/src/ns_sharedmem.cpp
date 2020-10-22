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

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include <native_service/ns_sharedmem.h>
#include <native_service/ns_transmit_log.h>

#include <string>

static CNSSharedMem *g_pTransmitLogSharedBuf = NULL;

#define SHM_HEADER_EXT      "Hdr"

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////////////////
/// NSSharedMemTransmitLogOpen
/// Open the shared memory for transmit logging
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSSharedMemTransmitLogOpen() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL == g_pTransmitLogSharedBuf) {
    // Create the instance
    g_pTransmitLogSharedBuf = new(std::nothrow) CNSSharedMem(TRANSMIT_LOG_SHAREDMEM_NAME, TRANSMIT_LOG_SHAREDMEM_SIZE);
  }

  if (NULL != g_pTransmitLogSharedBuf) {
    if (!g_pTransmitLogSharedBuf->IsOpen()) {
      // maps the shared memory buffer
      l_eStatus = g_pTransmitLogSharedBuf->Open();
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NSSharedMemTransmitLogClose
/// Close the transmit logging shared memory
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSSharedMemTransmitLogClose() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != g_pTransmitLogSharedBuf) {
    // un-map the shared memory object
    l_eStatus = g_pTransmitLogSharedBuf->Close();

    delete g_pTransmitLogSharedBuf;
    g_pTransmitLogSharedBuf = NULL;
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NSSharedMemReadTransmitLog
/// Reads transmit log from the shared memory buffer.
////////////////////////////////////////////////////////////////////////////////////////////
SI_32 NSSharedMemReadTransmitLog(PSTR f_pBuffer, const UI_32 f_uiLength, const BOOL f_bBlock) {
  // no. of bytes read
  SI_32 l_iReadSize = NS_SHM_ERROR;

  if (NULL != g_pTransmitLogSharedBuf && NULL != f_pBuffer) {
    // Writes log data into shared memory buffer
    l_iReadSize = g_pTransmitLogSharedBuf->Read(f_pBuffer, f_uiLength, f_bBlock);
  }

  return l_iReadSize;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NSSharedMemWriteTransmitLog
/// Write transmit log into the shared memory buffer.
////////////////////////////////////////////////////////////////////////////////////////////
SI_32 NSSharedMemWriteTransmitLog(PCSTR f_pBuffer, const UI_32 f_uiLength) {
  // no. of bytes read
  SI_32 l_iWriteSize = NS_SHM_ERROR;

  if (NULL != g_pTransmitLogSharedBuf && NULL != f_pBuffer) {
    // Writes log data into shared memory buffer
    l_iWriteSize = g_pTransmitLogSharedBuf->Write(f_pBuffer, f_uiLength);
  }

  return l_iWriteSize;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NSSharedMemDumpTransmitLogToFile
/// Dump transmit log from the shared memory buffer into file.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSSharedMemDumpTransmitLogToFile(PCSTR f_pPath, PUI_32 f_puiDumpSize) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  if (NULL != g_pTransmitLogSharedBuf && NULL != f_pPath) {
    // Writes log data into shared memory buffer
    l_eStatus = g_pTransmitLogSharedBuf->DumpToFile(f_pPath, f_puiDumpSize);
  }

  return l_eStatus;
}

// Used only in nstest_sharedmem.
#if defined(SHM_UNITTEST_ENABLE)
////////////////////////////////////////////////////////////////////////////////////////////
/// NSSharedMemTransmitLogIsOpen
////////////////////////////////////////////////////////////////////////////////////////////
BOOL NSSharedMemTransmitLogIsOpen() {
  if (g_pTransmitLogSharedBuf != NULL) {
    return g_pTransmitLogSharedBuf->IsOpen();
  } else {
    return FALSE;
  }
}
#endif

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
/// CNSSharedMem
/// Parameterized Constructor of CNSSharedMem class
////////////////////////////////////////////////////////////////////////////////////////////////
CNSSharedMem::CNSSharedMem(const std::string &f_cSharedMemName, const UI_32 f_uiSize):
  m_cShmName(f_cSharedMemName), m_uiShmBuffSize(f_uiSize), m_pShmHdr(NULL), m_pShmBuff(NULL) {
  m_cShmHdrName = f_cSharedMemName;
  m_cShmHdrName.append(SHM_HEADER_EXT);
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// CNSSharedMem
/// Constructor of CNSSharedMem class
////////////////////////////////////////////////////////////////////////////////////////////////
CNSSharedMem::CNSSharedMem():
  m_cShmName(""), m_uiShmBuffSize(0), m_pShmHdr(NULL), m_pShmBuff(NULL) {
  // do nothing
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CNSSharedMem
/// Destructor of CNSSharedMem class
////////////////////////////////////////////////////////////////////////////////////////////////
CNSSharedMem::~CNSSharedMem() {
  if (NULL != m_pShmHdr) {
    // un-map the shared memory object
    Close();

    m_pShmHdr = NULL;
  }

  // TODO(framework_unified): currently shared memory is not being unlinked,
  // we need to find a method where we can unlink the sharedmem
  // shm_unlink(m_cShmName.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Open
/// This function opens and maps the shared memory object.
/// It creates the shared memory if it does not exists.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMem::Open() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusErrOther;

// Used only in nstest_sharedmem.
#if defined(SHM_UNITTEST_ENABLE)
  if (getenv(NSTEST_FAIL_SHAREDMEM_OPEN) != NULL) {
    return l_eStatus;
  }
#endif

  if (NULL == m_pShmHdr) {
    // Open header shared memory
    if (eFrameworkunifiedStatusOK != (l_eStatus = MapSM(reinterpret_cast<PVOID *>(&m_pShmHdr),
      m_cShmHdrName, sizeof(NSSharedBufferHdr)))) {
      if (ENOENT == errno) {  // Shared memory is not created yet
        errno = EOK;

        // Create shared memory
        if (eFrameworkunifiedStatusOK == CreateSMHeader()) {
          // Retry to open
          l_eStatus = MapSM(reinterpret_cast<PVOID *>(&m_pShmHdr), m_cShmHdrName, sizeof(NSSharedBufferHdr));
        }
      }
    } else {
      pthread_mutex_lock(&m_pShmHdr->m_tBufMutex);

      // if shared memory buffer is created with size 0, then set new size in header if any change
      if (0 == m_pShmHdr->m_uiShMemSize && 0 < m_uiShmBuffSize) {
        m_pShmHdr->m_uiShMemSize = m_uiShmBuffSize;
      } else if (0 < m_pShmHdr->m_uiShMemSize && 0 == m_uiShmBuffSize) {
        m_uiShmBuffSize = m_pShmHdr->m_uiShMemSize;
      } else {
        // do nothing
      }

      pthread_mutex_unlock(&m_pShmHdr->m_tBufMutex);
    }
  }

  if (eFrameworkunifiedStatusOK == l_eStatus && 0 != m_uiShmBuffSize) {
    if (NULL == m_pShmBuff) {
      // Open shared memory data buffer, create if not exists
      if (eFrameworkunifiedStatusOK != (l_eStatus = MapSM(reinterpret_cast<PVOID *>(&m_pShmBuff), m_cShmName, m_uiShmBuffSize))) {
        if (ENOENT == errno) {  // Shared memory is not created yet
          // Create shared memory
          if (eFrameworkunifiedStatusOK == CreateSMDataBuffer()) {
            // Retry to open
            l_eStatus = MapSM(reinterpret_cast<PVOID *>(&m_pShmBuff), m_cShmName, m_uiShmBuffSize);
          }
        }
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusErrOther;
    }
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// IsOpen
/// This function is used to check whether the shared memory buffer is opened or not.
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CNSSharedMem::IsOpen() {
  return NULL == m_pShmHdr ? FALSE : TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Close
/// This function closes the shared memory object.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMem::Close() {
  EFrameworkunifiedStatus l_eStatus1 = UnMapSM(m_pShmHdr, sizeof(NSSharedBufferHdr));
  m_pShmHdr = NULL;

  EFrameworkunifiedStatus l_eStatus2 = eFrameworkunifiedStatusOK;
  if (0 != m_uiShmBuffSize) {
    l_eStatus2 = UnMapSM(m_pShmBuff, m_uiShmBuffSize);
    m_pShmBuff = NULL;
  }
  /*
   * todo
   * Even if an error occurs due to eFrameworkunifiedStatusNullPointer when UnMapSM fails,
   * the error type cannot be determined by the caller because it is rounded to eFrameworkunifiedStatusFail
   * if the API final determination is not OK.
   */
  return (eFrameworkunifiedStatusOK != l_eStatus1 || eFrameworkunifiedStatusOK != l_eStatus2) ? eFrameworkunifiedStatusFail : eFrameworkunifiedStatusOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Read
/// This function reads data from the shared memory.
////////////////////////////////////////////////////////////////////////////////////////////////
SI_32 CNSSharedMem::Read(PSTR f_pBuffer, const UI_32 f_uilength, const BOOL f_bBlock) {
  SI_32 l_iReadSize = NS_SHM_ERROR;

  if ((NULL != f_pBuffer) && (NULL != m_pShmHdr) && (0 != f_uilength)) {
    UI_32 l_uiDataSizeToRead = 0;

    // Remaining buffer size from read pointer to end of the buffer
    UI_32 l_uiRemainSize = 0;

    if (0 == pthread_mutex_lock(&m_pShmHdr->m_tBufMutex)) {
      if ((TRUE == f_bBlock) && (0 == m_pShmHdr->m_uiUnReadSize)) {
        pthread_cond_wait(&m_pShmHdr->m_tCondVar, &m_pShmHdr->m_tBufMutex);
      }

      // if shared memory buffer size is changed by some other process, remap the updated buffer size in this process
      // shared memory buffer size can only be changed if the initial size is 0.
      if (m_uiShmBuffSize != m_pShmHdr->m_uiShMemSize) {
        if (eFrameworkunifiedStatusOK == MapSM(reinterpret_cast<PVOID *>(&m_pShmBuff), m_cShmName, m_pShmHdr->m_uiShMemSize)) {
          m_uiShmBuffSize = m_pShmHdr->m_uiShMemSize;
        }
      }

      if (NULL != m_pShmBuff) {
        l_uiRemainSize = m_uiShmBuffSize - m_pShmHdr->m_uiReadPtr;

        // Round read data size depending on un-read data size in the buffer
        l_uiDataSizeToRead = m_pShmHdr->m_uiUnReadSize < f_uilength ? m_pShmHdr->m_uiUnReadSize : f_uilength;

        if (l_uiRemainSize < l_uiDataSizeToRead) {
          // Wrapping read
          memcpy(f_pBuffer, m_pShmBuff + m_pShmHdr->m_uiReadPtr, l_uiRemainSize);
          memcpy(f_pBuffer + l_uiRemainSize, m_pShmBuff, l_uiDataSizeToRead - l_uiRemainSize);
          m_pShmHdr->m_uiReadPtr = l_uiDataSizeToRead - l_uiRemainSize;
        } else {
          memcpy(f_pBuffer, m_pShmBuff + m_pShmHdr->m_uiReadPtr, l_uiDataSizeToRead);

          m_pShmHdr->m_uiReadPtr += l_uiDataSizeToRead;

          // Read pointer is the end of the buffer
          if (m_pShmHdr->m_uiReadPtr == m_uiShmBuffSize) {
            m_pShmHdr->m_uiReadPtr = 0;
          }
        }

        m_pShmHdr->m_uiUnReadSize -= l_uiDataSizeToRead;  // Update un-read data size

        l_iReadSize = l_uiDataSizeToRead;
      }

      pthread_mutex_unlock(&m_pShmHdr->m_tBufMutex);
    }
  }

  return l_iReadSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Write
/// This function writes the data into the shared memory.
////////////////////////////////////////////////////////////////////////////////////////////////
SI_32 CNSSharedMem::Write(PCSTR f_pBuffer, const UI_32 f_uilength) {
  SI_32 l_iWriteSize = NS_SHM_ERROR;

  // size available in buffer
  UI_32 l_uiRemainSize = 0;

  if (NULL != m_pShmHdr && NULL != m_pShmBuff && NULL != f_pBuffer && f_uilength <= m_uiShmBuffSize) {
    if (0 == pthread_mutex_lock(&m_pShmHdr->m_tBufMutex)) {
      l_uiRemainSize = m_uiShmBuffSize - m_pShmHdr->m_uiWritePtr;

      // Write data to the buffer
      if (l_uiRemainSize < f_uilength) {
        // Wrapping write
        memcpy(m_pShmBuff + m_pShmHdr->m_uiWritePtr, f_pBuffer, l_uiRemainSize);
        memcpy(m_pShmBuff, f_pBuffer + l_uiRemainSize, f_uilength - l_uiRemainSize);

        // Update the write pointer
        m_pShmHdr->m_uiWritePtr = f_uilength - l_uiRemainSize;

        // The buffer is full of valid data
        m_pShmHdr->m_bIsFull = TRUE;
      } else {
        memcpy(m_pShmBuff + m_pShmHdr->m_uiWritePtr, f_pBuffer, f_uilength);

        // Update the write pointer
        m_pShmHdr->m_uiWritePtr += f_uilength;

        // Write pointer is the end of the buffer
        if (m_pShmHdr->m_uiWritePtr == m_uiShmBuffSize) {
          m_pShmHdr->m_uiWritePtr = 0;

          // The buffer is full of valid data
          m_pShmHdr->m_bIsFull = TRUE;
        }
      }

      // Update un-read data size
      m_pShmHdr->m_uiUnReadSize += f_uilength;

      // Set read pointer to be same as write pointer if write pointer exceeds the read pointer
      if (m_uiShmBuffSize < m_pShmHdr->m_uiUnReadSize) {
        m_pShmHdr->m_uiReadPtr = m_pShmHdr->m_uiWritePtr;
        m_pShmHdr->m_uiUnReadSize = m_uiShmBuffSize;
      }

      pthread_cond_signal(&m_pShmHdr->m_tCondVar);

      pthread_mutex_unlock(&m_pShmHdr->m_tBufMutex);

      l_iWriteSize = f_uilength;
    }
  }

  return l_iWriteSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// DumpToFile
/// This function writes all the data in the buffer into provided file f_pPath.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMem::DumpToFile(PCSTR f_pPath, PUI_32 f_uiDumpSize) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  ssize_t l_iSize = 0;

  SI_32 fd = -1;

  if (NULL == f_uiDumpSize) {
    return eFrameworkunifiedStatusNullPointer;
  }
  *f_uiDumpSize = 0;

  if (NULL != f_pPath) {
    if (NULL != m_pShmHdr) {
      // Open file
      if (-1 != (fd = open(f_pPath, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, S_IRWXU | S_IRWXG | S_IRWXO))) {
        if (NULL != m_pShmBuff && 0 != m_uiShmBuffSize) {
          if (0 == pthread_mutex_lock(&m_pShmHdr->m_tBufMutex)) {
            // Write buffer data to file
            if (m_pShmHdr->m_bIsFull) {
              // Buffer has full of data (read data from write pointer)
              if (-1 != (l_iSize = write(fd, m_pShmBuff + m_pShmHdr->m_uiWritePtr,
                m_uiShmBuffSize - m_pShmHdr->m_uiWritePtr))) {
                *f_uiDumpSize += static_cast<UI_32>(l_iSize);
              } else {
                l_eStatus = eFrameworkunifiedStatusErrOther;
              }
            }

            if (-1 != (l_iSize = write(fd, m_pShmBuff, m_pShmHdr->m_uiWritePtr))) {
              *f_uiDumpSize += static_cast<UI_32>(l_iSize);
            }

            if (0 != pthread_mutex_unlock(&m_pShmHdr->m_tBufMutex)) {
              l_eStatus = eFrameworkunifiedStatusSemUnlockFail;
            }
          } else {
            l_eStatus = eFrameworkunifiedStatusSemLockFail;
          }
        } else if (NULL == m_pShmBuff && 0 != m_uiShmBuffSize) {
          l_eStatus = eFrameworkunifiedStatusFail;
        } else {
          // do nothing
        }

        // Sync the file to force I/O operation completed
        fsync(fd);

        close(fd);
      } else {
        l_eStatus = eFrameworkunifiedStatusFileLoadError;
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// GetSize
/// This function returns the number of unread bytes which can be read by Read().
////////////////////////////////////////////////////////////////////////////////////////////////
SI_32 CNSSharedMem::GetSize() {
  SI_32 l_uiReadSize = NS_SHM_ERROR;

  if (NULL != m_pShmHdr) {
    l_uiReadSize = m_pShmHdr->m_uiUnReadSize;
  }

  return l_uiReadSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ClearBuf
/// This function clears the buffer.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMem::ClearBuf() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pShmHdr) {
    if (0 == pthread_mutex_lock(&m_pShmHdr->m_tBufMutex)) {
      // Initialize the r/w pointers
      m_pShmHdr->m_uiReadPtr = 0;
      m_pShmHdr->m_uiWritePtr = 0;
      m_pShmHdr->m_uiUnReadSize = 0;
      m_pShmHdr->m_bIsFull = FALSE;

      if (0 != pthread_mutex_unlock(&m_pShmHdr->m_tBufMutex)) {
        l_eStatus = eFrameworkunifiedStatusSemUnlockFail;
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusSemLockFail;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// SetReadPtrToWritePtr
/// This function sets the position of read ptr to write ptr in buffer.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMem::SetReadPtrToWritePtr() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pShmHdr) {
    if (0 == pthread_mutex_lock(&m_pShmHdr->m_tBufMutex)) {  // LCOV_EXCL_BR_LINE 5: pthread_mutex_lock's error case
      // Initialize the r/w pointers
      m_pShmHdr->m_uiReadPtr = m_pShmHdr->m_uiWritePtr;
      m_pShmHdr->m_uiUnReadSize = 0;

      if (0 != pthread_mutex_unlock(&m_pShmHdr->m_tBufMutex)) {  // LCOV_EXCL_BR_LINE 5: pthread_mutex_unlock's error
        l_eStatus = eFrameworkunifiedStatusSemUnlockFail;
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusSemLockFail;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateSMHeader
/// This function creates the shared memory object for header.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMem::CreateSMHeader() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  // file descriptor of shared memory
  SI_32 l_siId = -1;

  // shared memory buffer headers
  NSSharedBufferHdr *l_pShmHdr = NULL;

  if ((!m_cShmHdrName.empty()) && (m_cShmHdrName.size() <= NAME_MAX)) {
    // Try to create shared memory
    l_siId = shm_open(m_cShmHdrName.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);

    if (-1 != l_siId) {
      // Set the size of shared memory
      if (-1 != ftruncate(l_siId, sizeof(NSSharedBufferHdr))) {
        // Map the shared memory
        l_pShmHdr = reinterpret_cast<NSSharedBufferHdr *>(mmap(NULL, sizeof(NSSharedBufferHdr),
          (PROT_READ | PROT_WRITE), MAP_SHARED, l_siId, 0));

        if (MAP_FAILED != l_pShmHdr) {
          // mutex attribute
          pthread_mutexattr_t l_tMtxAttr = {};
          pthread_condattr_t l_tCondAttr = {};

          // Initialize mutex
          pthread_mutexattr_init(&l_tMtxAttr);
          pthread_mutexattr_setpshared(&l_tMtxAttr, PTHREAD_PROCESS_SHARED);
          pthread_mutex_init(&l_pShmHdr->m_tBufMutex, &l_tMtxAttr);
          pthread_mutexattr_destroy(&l_tMtxAttr);

          pthread_condattr_init(&l_tCondAttr);
          pthread_condattr_setpshared(&l_tCondAttr, PTHREAD_PROCESS_SHARED);
          pthread_cond_init(&l_pShmHdr->m_tCondVar, &l_tCondAttr);
          pthread_condattr_destroy(&l_tCondAttr);

          pthread_mutex_lock(&l_pShmHdr->m_tBufMutex);

          // Initialize the r/w pointers
          l_pShmHdr->m_uiReadPtr = 0;
          l_pShmHdr->m_uiWritePtr = 0;
          l_pShmHdr->m_uiUnReadSize = 0;
          l_pShmHdr->m_bIsFull = FALSE;
          l_pShmHdr->m_uiShMemSize = m_uiShmBuffSize;

          pthread_mutex_unlock(&l_pShmHdr->m_tBufMutex);

          // Once initialized un-map the shared memory
          munmap(l_pShmHdr, sizeof(NSSharedBufferHdr));

          l_eStatus = eFrameworkunifiedStatusOK;
        }
      }

      close(l_siId);
    } else if (EEXIST == errno) {
      // Shared memory is already created
      l_eStatus = eFrameworkunifiedStatusDuplicate;
    } else {
      // do nothing
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateSMDataBuffer
/// This function creates the shared memory object for data buffer.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMem::CreateSMDataBuffer() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  // file descriptor of shared memory
  SI_32 l_siId = -1;

  if ((!m_cShmName.empty()) && (m_cShmName.size() <= NAME_MAX)) {
    // Try to create shared memory
    l_siId = shm_open(m_cShmName.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);

    if (-1 != l_siId) {
      // Set the size of shared memory
      if (-1 != ftruncate(l_siId, m_uiShmBuffSize)) {
        l_eStatus = eFrameworkunifiedStatusOK;
      }

      close(l_siId);
    } else if (EEXIST == errno) {
      // Shared memory is already created
      l_eStatus = eFrameworkunifiedStatusDuplicate;
    } else {
      // do nothing
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// MapSMHeader
/// This function open and maps the shared memory header in process space.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMem::MapSM(PVOID *f_pShMem, const std::string &f_cShmName, const UI_32 f_uiShmSize) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  // file descriptor of shared memory
  SI_32 l_siId = -1;

  // shared memory buffer headers
  PVOID l_pShmBuf = NULL;

  // Open shared memory
  l_siId = shm_open(f_cShmName.c_str(), O_RDWR, 0);

  if (-1 != l_siId) {
    // Map the shared memory into its memory space
    l_pShmBuf = mmap(NULL, f_uiShmSize, (PROT_READ | PROT_WRITE), MAP_SHARED, l_siId, 0);

    if (MAP_FAILED != l_pShmBuf) {
      *f_pShMem = l_pShmBuf;
      l_eStatus = eFrameworkunifiedStatusOK;
    }

    close(l_siId);
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// UnMapSM
/// This function unmaps the shared memory object.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMem::UnMapSM(PVOID f_pShMem, const UI_32 f_uiShmSize) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  // Un-map the shared memory
  if (NULL != f_pShMem) {
    if (0 != munmap(f_pShMem, f_uiShmSize)) {
      l_eStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}
