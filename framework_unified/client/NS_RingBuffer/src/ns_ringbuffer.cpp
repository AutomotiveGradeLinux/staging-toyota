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
/// \ingroup  tag_NSRingBuffer
/// \brief    This file contains implementation of class CNSRingBuffer.
///           This class provides API to open, read, write and close ring buffer
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include <native_service/ns_ringbuffer.h>
#include <native_service/cl_lock.h>

#include <string>

#define RBUF_HEADER_EXT "Hdr"
#define RBUF_RETRY_SLEEP (1000)
#define RBUF_RETRY_COUNT (8)
#define RBUF_PAGE_SIZE (4096)
#define RBUF_PAGE_ALIGN(p) ((p / RBUF_PAGE_SIZE) * RBUF_PAGE_SIZE)

////////////////////////////////////////////////////////////////////////////////////////////////
/// CNSRingBuffer
/// Parameterized Constructor of CNSRingBuffer class
////////////////////////////////////////////////////////////////////////////////////////////////
CNSRingBuffer::CNSRingBuffer(const std::string &f_cMappedFilePath, const UI_32 f_uiSize, int f_lid):
  m_cMappedFilePath(f_cMappedFilePath), m_uiRingBuffSize(f_uiSize), m_pRbufMtx(NULL), m_pRbufHdr(NULL), m_pRbuf(NULL),
  m_lid(f_lid), m_pLockAddr(NULL), m_siProcessLastWrtPage(-1) {  // LCOV_EXCL_BR_LINE 11: except branch
  m_cMappedFileHdrPath = f_cMappedFilePath;
  m_cMappedFileHdrPath.append(RBUF_HEADER_EXT);  // LCOV_EXCL_BR_LINE 11: except branch

  m_cRbufMtxName = f_cMappedFilePath;
  std::size_t found = m_cRbufMtxName.find_first_of('/');
  while (found != std::string::npos) {
    m_cRbufMtxName[found] = '_';
    found = m_cRbufMtxName.find_first_of('/', found + 1);
  }
  m_cRbufMtxName[0] = '/';
  pthread_mutex_init(&m_tOpenMutex, NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// CNSRingBuffer
/// Constructor of CNSRingBuffer class
////////////////////////////////////////////////////////////////////////////////////////////////
CNSRingBuffer::CNSRingBuffer():
  m_cMappedFilePath(""), m_uiRingBuffSize(0), m_pRbufMtx(NULL), m_pRbufHdr(NULL), m_pRbuf(NULL), m_lid(-1),
  m_pLockAddr(NULL), m_siProcessLastWrtPage(-1) {
  pthread_mutex_init(&m_tOpenMutex, NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CNSRingBuffer
/// Destructor of CNSRingBuffer class
////////////////////////////////////////////////////////////////////////////////////////////////
CNSRingBuffer::~CNSRingBuffer() {
  if (NULL != m_pRbufHdr) {
    // un-map the ring buffer object
    Close();

    m_pRbufHdr = NULL;
    m_pRbufMtx = NULL;
  }
  pthread_mutex_destroy(&m_tOpenMutex);
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Open
/// This function opens and maps the ring buffer object.
/// It creates the ring buffer if it does not exists.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSRingBuffer::Open() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusErrOther;

  pthread_mutex_lock(&m_tOpenMutex);

  if (NULL == m_pRbufMtx) {
    // Open ring buffer mutex
    if (eFrameworkunifiedStatusOK != (l_eStatus = MapSM(reinterpret_cast<PVOID *>(&m_pRbufMtx),
      m_cRbufMtxName, sizeof(NSRingBufferMtx)))) {
      if (ENOENT == errno) {
        errno = EOK;


//        if (eFrameworkunifiedStatusOK == CreateRBMutex()) {
        EFrameworkunifiedStatus l_eStatus1 = CreateRBMutex();
        if ((l_eStatus1 == eFrameworkunifiedStatusOK) || (l_eStatus1 == eFrameworkunifiedStatusDuplicate)) {

          l_eStatus = MapSM(reinterpret_cast<PVOID *>(&m_pRbufMtx), m_cRbufMtxName, sizeof(NSRingBufferMtx));
        }
      }
    } else {
      if (m_pRbufMtx->m_lid != m_lid) {
        // Different Lock ID in constructor parameter and mutex object
        l_eStatus = eFrameworkunifiedStatusInvldParam;
      }
    }
  }

  if (eFrameworkunifiedStatusOK == l_eStatus) {
    if (m_lid >= 0 && NULL == m_pLockAddr) {
      CL_LockProcessInit();
      if ((m_pLockAddr = CL_LockMap(m_lid)) == MAP_FAILED) {
        l_eStatus = eFrameworkunifiedStatusFail;
      }
    }
  }

  if (eFrameworkunifiedStatusOK == l_eStatus) {
    if (NULL == m_pRbufHdr) {
      // Open header ring buffer
      if (eFrameworkunifiedStatusOK != (l_eStatus = Map(reinterpret_cast<PVOID *>(&m_pRbufHdr),
        m_cMappedFileHdrPath, sizeof(NSRingBufferHdr)))) {
        if (ENOENT == errno) {    // ring buffer is not created yet
          errno = EOK;

          // Create ring buffer

//          if (eFrameworkunifiedStatusOK == CreateRBHeader()) {
          EFrameworkunifiedStatus l_eStatus1 = CreateRBHeader();
          if ((l_eStatus1 == eFrameworkunifiedStatusOK) || (l_eStatus1 == eFrameworkunifiedStatusDuplicate)) {

            // Retry to open
            l_eStatus = Map(reinterpret_cast<PVOID *>(&m_pRbufHdr),
              m_cMappedFileHdrPath, sizeof(NSRingBufferHdr));

//          }
          } else {
            l_eStatus = l_eStatus1;
          }

        }
      } else {

#if 0
        LockMtx();

        struct stat st;
        int ret = stat(m_cMappedFilePath.c_str(), &st);

        if (m_uiRingBuffSize == 0) {
          if (ret == 0) {   // LCOV_EXCL_BR_LINE 5:stat's error case.
            // LCOV_EXCL_START 5: stat's error case.
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            if ((UI_32)st.st_size != m_pRbufHdr->m_uiRingBufferSize || (UI_32)st.st_size < m_pRbufHdr->m_uiReadPtr
                || (UI_32)st.st_size < m_pRbufHdr->m_uiWritePtr) {
              memset(m_pRbufHdr, 0, sizeof(NSRingBufferHdr));
              m_pRbufHdr->m_uiRingBufferSize = static_cast<UI_32>(st.st_size);
              unlink(m_cMappedFilePath.c_str());
            }
            m_uiRingBuffSize = static_cast<UI_32>(st.st_size);
            // LCOV_EXCL_STOP
          }
        } else {
          if (m_uiRingBuffSize != m_pRbufHdr->m_uiRingBufferSize || m_uiRingBuffSize < m_pRbufHdr->m_uiReadPtr
              || m_uiRingBuffSize < m_pRbufHdr->m_uiWritePtr || (ret == 0 && m_uiRingBuffSize != (UI_32)st.st_size)) {
            memset(m_pRbufHdr, 0, sizeof(NSRingBufferHdr));
            m_pRbufHdr->m_uiRingBufferSize = m_uiRingBuffSize;
            unlink(m_cMappedFilePath.c_str());
          }
        }

        UnlockMtx();
#endif
        int ret;
        if ( (ret = LockMtx()) == 0) {
          struct stat st;
          int ret = stat(m_cMappedFilePath.c_str(), &st);

          if (m_uiRingBuffSize == 0) {
            if (ret == 0) {
              if ((UI_32)st.st_size != m_pRbufHdr->m_uiRingBufferSize || (UI_32)st.st_size < m_pRbufHdr->m_uiReadPtr ||
                  (UI_32)st.st_size < m_pRbufHdr->m_uiWritePtr) {
                memset(m_pRbufHdr, 0, sizeof(NSRingBufferHdr));
                m_pRbufHdr->m_uiRingBufferSize = static_cast<UI_32>(st.st_size);
                unlink(m_cMappedFilePath.c_str());
              }
              m_uiRingBuffSize = static_cast<UI_32>(st.st_size);
            }
          } else {
            if (m_uiRingBuffSize != m_pRbufHdr->m_uiRingBufferSize || m_uiRingBuffSize < m_pRbufHdr->m_uiReadPtr ||
                m_uiRingBuffSize < m_pRbufHdr->m_uiWritePtr || (ret == 0 && m_uiRingBuffSize != (UI_32)st.st_size)) {
              memset(m_pRbufHdr, 0, sizeof(NSRingBufferHdr));
              m_pRbufHdr->m_uiRingBufferSize = m_uiRingBuffSize;
              unlink(m_cMappedFilePath.c_str());
            }
          }

          UnlockMtx();
        } else {
          fprintf(stderr, "[%s] LockMtx error: %s\n", __PRETTY_FUNCTION__, strerror(ret));
          munmap(m_pRbufHdr, sizeof(NSRingBufferHdr));
          m_pRbufHdr = NULL;
          l_eStatus = eFrameworkunifiedStatusFail;
        }

      }
    }
  }

  if (eFrameworkunifiedStatusOK == l_eStatus && 0 != m_uiRingBuffSize) {
    if (NULL == m_pRbuf) {
      // Open ring buffer data buffer, create if not exists
      if (eFrameworkunifiedStatusOK != (l_eStatus = Map(reinterpret_cast<PVOID *>(&m_pRbuf),
        m_cMappedFilePath, m_uiRingBuffSize))) {
        if (ENOENT == errno) {   // ring buffer is not created yet
          // Create ring buffer

//          if (eFrameworkunifiedStatusOK == CreateRBDataBuffer()) {
          EFrameworkunifiedStatus l_eStatus1 = CreateRBDataBuffer();
          if ((l_eStatus1 == eFrameworkunifiedStatusOK) || (l_eStatus1 == eFrameworkunifiedStatusDuplicate)) {

            // Retry to open
            l_eStatus = Map(reinterpret_cast<PVOID *>(&m_pRbuf),
              m_cMappedFilePath, m_uiRingBuffSize);
          }
        }
      }
    }
  }

  pthread_mutex_unlock(&m_tOpenMutex);

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// IsOpen
/// This function is used to check whether the ring buffer is opened or not.
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CNSRingBuffer::IsOpen() {
  return NULL == m_pRbufHdr ? FALSE : TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Close
/// This function closes the ring buffer object.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSRingBuffer::Close() {
  if (NULL != m_pLockAddr) {
    CL_LockUnmap(m_pLockAddr);
  }

  EFrameworkunifiedStatus l_eStatus1 = UnMap(m_pRbufHdr, sizeof(NSRingBufferHdr));
  m_pRbufHdr = NULL;

  EFrameworkunifiedStatus l_eStatus2 = eFrameworkunifiedStatusOK;
  if (0 != m_uiRingBuffSize) {
    l_eStatus2 = UnMap(m_pRbuf, m_uiRingBuffSize);
    m_pRbuf = NULL;
  }

  EFrameworkunifiedStatus l_eStatus3 = UnMap(m_pRbufMtx, sizeof(NSRingBufferMtx));
  m_pRbufMtx = NULL;

  return (eFrameworkunifiedStatusOK != l_eStatus1 || eFrameworkunifiedStatusOK != l_eStatus2 ||
          eFrameworkunifiedStatusOK != l_eStatus3) ? eFrameworkunifiedStatusFail : eFrameworkunifiedStatusOK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Read
/// This function reads data from the ring buffer.
////////////////////////////////////////////////////////////////////////////////////////////////
SI_32 CNSRingBuffer::Read(PSTR f_pBuffer, const UI_32 f_uilength) {
  SI_32 l_iReadSize = -1;

  if ((NULL != f_pBuffer) && (NULL != m_pRbufHdr) && (0 != f_uilength)) {
    UI_32 l_uiDataSizeToRead = 0;

    // Remaining buffer size from read pointer to end of the buffer
    UI_32 l_uiRemainSize = 0;


#if 0
    LockMtx();

    // if ring buffer size is changed by some other process, remap the updated buffer size in this process
    // ring buffer size can only be changed if the initial size is 0.
    if (m_uiRingBuffSize != m_pRbufHdr->m_uiRingBufferSize) {
      if (eFrameworkunifiedStatusOK == Map(reinterpret_cast<PVOID *>(&m_pRbuf),
        m_cMappedFilePath, m_pRbufHdr->m_uiRingBufferSize)) {
        m_uiRingBuffSize = m_pRbufHdr->m_uiRingBufferSize;
      }
    }

    if (NULL != m_pRbuf) {
      l_uiRemainSize = m_uiRingBuffSize - m_pRbufHdr->m_uiReadPtr;

      // Round read data size depending on un-read data size in the buffer
      l_uiDataSizeToRead = m_pRbufHdr->m_uiUnReadSize < f_uilength ? m_pRbufHdr->m_uiUnReadSize : f_uilength;

      if (l_uiRemainSize < l_uiDataSizeToRead) {
        // Wrapping read
        memcpy(f_pBuffer, m_pRbuf + m_pRbufHdr->m_uiReadPtr, l_uiRemainSize);
        memcpy(f_pBuffer + l_uiRemainSize, m_pRbuf, l_uiDataSizeToRead - l_uiRemainSize);
        m_pRbufHdr->m_uiReadPtr = l_uiDataSizeToRead - l_uiRemainSize;
      } else {
        memcpy(f_pBuffer, m_pRbuf + m_pRbufHdr->m_uiReadPtr, l_uiDataSizeToRead);

        m_pRbufHdr->m_uiReadPtr += l_uiDataSizeToRead;

        // Read pointer is the end of the buffer
        if (m_pRbufHdr->m_uiReadPtr == m_uiRingBuffSize) {
          m_pRbufHdr->m_uiReadPtr = 0;
        }
      }

      m_pRbufHdr->m_uiUnReadSize -= l_uiDataSizeToRead;  // Update un-read data size

      l_iReadSize = l_uiDataSizeToRead;
    }

    UnlockMtx();
#endif
    int ret;
    if ( (ret = LockMtx()) == 0) {
      // if ring buffer size is changed by some other process, remap the updated buffer size in this process
      // ring buffer size can only be changed if the initial size is 0.
      if (m_uiRingBuffSize != m_pRbufHdr->m_uiRingBufferSize) {
        if (eFrameworkunifiedStatusOK == Map(reinterpret_cast<PVOID *>(&m_pRbuf),
          m_cMappedFilePath, m_pRbufHdr->m_uiRingBufferSize)) {
          m_uiRingBuffSize = m_pRbufHdr->m_uiRingBufferSize;
        }
      }

      if (NULL != m_pRbuf) {
        l_uiRemainSize = m_uiRingBuffSize - m_pRbufHdr->m_uiReadPtr;

        // Round read data size depending on un-read data size in the buffer
        l_uiDataSizeToRead = m_pRbufHdr->m_uiUnReadSize < f_uilength ? m_pRbufHdr->m_uiUnReadSize : f_uilength;

          if (l_uiRemainSize < l_uiDataSizeToRead) {
            // Wrapping read
            memcpy(f_pBuffer, m_pRbuf + m_pRbufHdr->m_uiReadPtr, l_uiRemainSize);
            memcpy(f_pBuffer + l_uiRemainSize, m_pRbuf, l_uiDataSizeToRead - l_uiRemainSize);
            m_pRbufHdr->m_uiReadPtr = l_uiDataSizeToRead - l_uiRemainSize;
          } else {
            memcpy(f_pBuffer, m_pRbuf + m_pRbufHdr->m_uiReadPtr, l_uiDataSizeToRead);

            m_pRbufHdr->m_uiReadPtr += l_uiDataSizeToRead;

            // Read pointer is the end of the buffer
            if (m_pRbufHdr->m_uiReadPtr == m_uiRingBuffSize) {
              m_pRbufHdr->m_uiReadPtr = 0;
            }
          }

          m_pRbufHdr->m_uiUnReadSize -= l_uiDataSizeToRead; // Update un-read data size

          l_iReadSize = l_uiDataSizeToRead;
        }

        UnlockMtx();
      } else {
        fprintf(stderr, "[%s] LockMtx error: %s\n", __PRETTY_FUNCTION__, strerror(ret));
        l_iReadSize = 0;
    }

  }

  return l_iReadSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Write
/// This function writes the data into the ring buffer.
////////////////////////////////////////////////////////////////////////////////////////////////
SI_32 CNSRingBuffer::Write(PCSTR f_pBuffer, const UI_32 f_uilength) {
  SI_32 l_iWriteSize = -1;

  // size available in buffer
  UI_32 l_uiRemainSize = 0;

  if (NULL != m_pRbufHdr && NULL != m_pRbuf && NULL != f_pBuffer && f_uilength <= m_uiRingBuffSize) {  // LCOV_EXCL_BR_LINE 11:except branch
#if 0
    LockMtx();

    UI_32 l_uiLastPage = RBUF_PAGE_ALIGN(m_pRbufHdr->m_uiWritePtr);
    l_uiRemainSize = m_uiRingBuffSize - m_pRbufHdr->m_uiWritePtr;

    if (m_siProcessLastWrtPage >= 0) {
      if ((UI_32)m_siProcessLastWrtPage != l_uiLastPage) {
        if (madvise(m_pRbuf + m_siProcessLastWrtPage, RBUF_PAGE_SIZE, MADV_DONTNEED) < 0) {
          fprintf(stderr, "[CNSRingBuffer::Write] madvise(%p) error: %s\n",
            m_pRbuf + m_siProcessLastWrtPage, strerror(errno));
        }
      }
    }

    // Write data to the buffer
    if (l_uiRemainSize < f_uilength) {
      // Wrapping write
      memcpy(m_pRbuf + m_pRbufHdr->m_uiWritePtr, f_pBuffer, l_uiRemainSize);
      memcpy(m_pRbuf, f_pBuffer + l_uiRemainSize, f_uilength - l_uiRemainSize);

      // Update the write pointer
      m_pRbufHdr->m_uiWritePtr = f_uilength - l_uiRemainSize;

      // The buffer is full of valid data
      m_pRbufHdr->m_bIsFull = TRUE;
    } else {
      memcpy(m_pRbuf + m_pRbufHdr->m_uiWritePtr, f_pBuffer, f_uilength);

      // Update the write pointer
      m_pRbufHdr->m_uiWritePtr += f_uilength;

      // Write pointer is the end of the buffer
      if (m_pRbufHdr->m_uiWritePtr == m_uiRingBuffSize) {
        m_pRbufHdr->m_uiWritePtr = 0;

        // The buffer is full of valid data
        m_pRbufHdr->m_bIsFull = TRUE;
      }
    }

    // Update un-read data size
    m_pRbufHdr->m_uiUnReadSize += f_uilength;

    // Set read pointer to be same as write pointer if write pointer exceeds the read pointer
    if (m_uiRingBuffSize < m_pRbufHdr->m_uiUnReadSize) {
      m_pRbufHdr->m_uiReadPtr = m_pRbufHdr->m_uiWritePtr;
      m_pRbufHdr->m_uiUnReadSize = m_uiRingBuffSize;
    }

    while (l_uiLastPage != RBUF_PAGE_ALIGN(m_pRbufHdr->m_uiWritePtr)) {
      if (madvise(m_pRbuf + l_uiLastPage, RBUF_PAGE_SIZE, MADV_DONTNEED) < 0) {
        fprintf(stderr, "[CNSRingBuffer::Write] madvise(%p) error: %s\n", m_pRbuf + l_uiLastPage, strerror(errno));
      }
      l_uiLastPage += RBUF_PAGE_SIZE;
      if (l_uiLastPage >= m_uiRingBuffSize) {
        l_uiLastPage = 0;
      }
    }
    m_siProcessLastWrtPage = (SI_32)l_uiLastPage;

    UnlockMtx();

    l_iWriteSize = f_uilength;
#endif
    int ret;
    if ( (ret = LockMtx()) == 0) {
      UI_32 l_uiLastPage = RBUF_PAGE_ALIGN(m_pRbufHdr->m_uiWritePtr);
      l_uiRemainSize = m_uiRingBuffSize - m_pRbufHdr->m_uiWritePtr;

      if (m_siProcessLastWrtPage >= 0) {
        if ((UI_32)m_siProcessLastWrtPage != l_uiLastPage) {
          if (madvise(m_pRbuf + m_siProcessLastWrtPage, RBUF_PAGE_SIZE, MADV_DONTNEED) < 0) {
            fprintf(stderr, "[CNSRingBuffer::Write] madvise(%p) error: %s\n",
                    m_pRbuf + m_siProcessLastWrtPage, strerror(errno));
          }
        }
      }

      // Write data to the buffer
      if (l_uiRemainSize < f_uilength) {
      // Wrapping write
        memcpy(m_pRbuf + m_pRbufHdr->m_uiWritePtr, f_pBuffer, l_uiRemainSize);
        memcpy(m_pRbuf, f_pBuffer + l_uiRemainSize, f_uilength - l_uiRemainSize);

        // Update the write pointer
        m_pRbufHdr->m_uiWritePtr = f_uilength - l_uiRemainSize;

        // The buffer is full of valid data
        m_pRbufHdr->m_bIsFull = TRUE;
      } else {
        memcpy(m_pRbuf + m_pRbufHdr->m_uiWritePtr, f_pBuffer, f_uilength);

        // Update the write pointer
        m_pRbufHdr->m_uiWritePtr += f_uilength;

        // Write pointer is the end of the buffer
        if (m_pRbufHdr->m_uiWritePtr == m_uiRingBuffSize) {
          m_pRbufHdr->m_uiWritePtr = 0;

          // The buffer is full of valid data
          m_pRbufHdr->m_bIsFull = TRUE;
        }
      }

      // Update un-read data size
      m_pRbufHdr->m_uiUnReadSize += f_uilength;

      // Set read pointer to be same as write pointer if write pointer exceeds the read pointer
      if (m_uiRingBuffSize < m_pRbufHdr->m_uiUnReadSize) {
        m_pRbufHdr->m_uiReadPtr = m_pRbufHdr->m_uiWritePtr;
        m_pRbufHdr->m_uiUnReadSize = m_uiRingBuffSize;
      }

      while (l_uiLastPage != RBUF_PAGE_ALIGN(m_pRbufHdr->m_uiWritePtr)) {
        if (madvise(m_pRbuf + l_uiLastPage, RBUF_PAGE_SIZE, MADV_DONTNEED) < 0) {
          fprintf(stderr, "[CNSRingBuffer::Write] madvise(%p) error: %s\n",
                  m_pRbuf + l_uiLastPage, strerror(errno));
        }
        l_uiLastPage += RBUF_PAGE_SIZE;
        if (l_uiLastPage >= m_uiRingBuffSize) {
          l_uiLastPage = 0;
        }
      }
      m_siProcessLastWrtPage = (SI_32)l_uiLastPage;

      UnlockMtx();

      l_iWriteSize = f_uilength;
    } else {
      fprintf(stderr, "[%s] LockMtx error: %s\n", __PRETTY_FUNCTION__, strerror(ret));
      l_iWriteSize = 0;
    }

  }

  return l_iWriteSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// DumpToFile
/// This function writes all the data in the buffer into provided file f_pPath.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSRingBuffer::DumpToFile(PCSTR f_pPath, PUI_32 f_uiDumpSize) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  ssize_t l_iSize = 0;

  SI_32 fd = -1;

  if (NULL == f_uiDumpSize) {
    return eFrameworkunifiedStatusNullPointer;
  }
  *f_uiDumpSize = 0;

  if (NULL != f_pPath) {
    if (NULL != m_pRbufHdr) {
      // Open file
      if (-1 != (fd = open(f_pPath, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0640))) {
        if (NULL != m_pRbuf && 0 != m_uiRingBuffSize) {

#if 0
          LockMtx();
          // Write buffer data to file
          if (m_pRbufHdr->m_bIsFull) {
            // Buffer has full of data (read data from write pointer)
            if (-1 != (l_iSize = write(fd, m_pRbuf + m_pRbufHdr->m_uiWritePtr,
              m_uiRingBuffSize - m_pRbufHdr->m_uiWritePtr))) {
              *f_uiDumpSize += static_cast<UI_32>(l_iSize);
            } else {
              l_eStatus = eFrameworkunifiedStatusErrOther;
            }
          }

          if (-1 != (l_iSize = write(fd, m_pRbuf, m_pRbufHdr->m_uiWritePtr))) {
            *f_uiDumpSize += static_cast<UI_32>(l_iSize);
          }

          UnlockMtx();
#endif
          int ret;
          if ( (ret = LockMtx()) == 0) {
            // Write buffer data to file
            if (m_pRbufHdr->m_bIsFull) {
              // Buffer has full of data (read data from write pointer)
              if (-1 != (l_iSize = write(fd, m_pRbuf + m_pRbufHdr->m_uiWritePtr,
                m_uiRingBuffSize - m_pRbufHdr->m_uiWritePtr))) {

//                *f_uiDumpSize += l_iSize;
                *f_uiDumpSize += static_cast<UI_32>(l_iSize);
              } else {
                l_eStatus = eFrameworkunifiedStatusErrOther;
              }
            }

            if (-1 != (l_iSize = write(fd, m_pRbuf, m_pRbufHdr->m_uiWritePtr))) {

//              *f_uiDumpSize += l_iSize;
              *f_uiDumpSize += static_cast<UI_32>(l_iSize);
            }

            UnlockMtx();
          } else {
            fprintf(stderr, "[%s] LockMtx error: %s\n", __PRETTY_FUNCTION__, strerror(ret));
          }

        } else if (NULL == m_pRbuf && 0 != m_uiRingBuffSize) {
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
SI_32 CNSRingBuffer::GetSize() {
  SI_32 l_uiReadSize = -1;

  if (NULL != m_pRbufHdr) {
    l_uiReadSize = m_pRbufHdr->m_uiUnReadSize;
  }

  return l_uiReadSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ClearBuf
/// This function clears the buffer.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSRingBuffer::ClearBuf() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pRbufHdr) {

#if 0
    LockMtx();

    // Initialize the r/w pointers
    m_pRbufHdr->m_uiReadPtr = 0;
    m_pRbufHdr->m_uiWritePtr = 0;
    m_pRbufHdr->m_uiUnReadSize = 0;
    m_pRbufHdr->m_bIsFull = FALSE;

    UnlockMtx();
#endif
    int ret;
    if ((ret = LockMtx()) == 0) {
      // Initialize the r/w pointers
      m_pRbufHdr->m_uiReadPtr = 0;
      m_pRbufHdr->m_uiWritePtr = 0;
      m_pRbufHdr->m_uiUnReadSize = 0;
      m_pRbufHdr->m_bIsFull = FALSE;

      UnlockMtx();
    } else {
      fprintf(stderr, "[%s] LockMtx error: %s\n", __PRETTY_FUNCTION__, strerror(ret));
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
EFrameworkunifiedStatus CNSRingBuffer::SetReadPtrToWritePtr() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pRbufHdr) {

#if 0
    LockMtx();

    // Initialize the r/w pointers
    m_pRbufHdr->m_uiReadPtr = m_pRbufHdr->m_uiWritePtr;
    m_pRbufHdr->m_uiUnReadSize = 0;

    UnlockMtx();
#endif
    int ret;
    if ( (ret = LockMtx()) == 0) {
      // Initialize the r/w pointers
      m_pRbufHdr->m_uiReadPtr = m_pRbufHdr->m_uiWritePtr;
      m_pRbufHdr->m_uiUnReadSize = 0;

      UnlockMtx();
    } else {
      fprintf(stderr, "[%s] LockMtx error: %s\n", __PRETTY_FUNCTION__, strerror(ret));
    }

  } else {
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  return l_eStatus;
}
//
////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateRBMutex
/// This function creates the shared memory object for mutex.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSRingBuffer::CreateRBMutex() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  // file descriptor of shared memory
  SI_32 l_siId = -1;

  // ring buffer mutex
  NSRingBufferMtx *l_pRbufMtx = NULL;

  if ((!m_cRbufMtxName.empty()) || (m_cRbufMtxName.size() <= NAME_MAX)) {
    // Try to create shared memory
    l_siId = shm_open(m_cRbufMtxName.c_str(), O_CREAT | O_EXCL | O_RDWR, 0640);

    if (-1 != l_siId) {
      // Set the size of shared memory
      if (-1 != ftruncate(l_siId, sizeof(NSRingBufferMtx))) {
        // Map the shared memory
        l_pRbufMtx = reinterpret_cast<NSRingBufferMtx *>(mmap(NULL,
          sizeof(NSRingBufferMtx), (PROT_READ | PROT_WRITE), MAP_SHARED, l_siId, 0));

        if (MAP_FAILED != l_pRbufMtx) {
          if (m_lid == -1) {
            // mutex attribute
            pthread_mutexattr_t l_tMtxAttr = {};

            // Initialize mutex
            pthread_mutexattr_init(&l_tMtxAttr);
            pthread_mutexattr_setpshared(&l_tMtxAttr, PTHREAD_PROCESS_SHARED);
            pthread_mutex_init(&l_pRbufMtx->m_tBufMutex, &l_tMtxAttr);
            pthread_mutexattr_destroy(&l_tMtxAttr);

            l_eStatus = eFrameworkunifiedStatusOK;
          } else {
            // CL_Lock
            l_eStatus = eFrameworkunifiedStatusOK;
          }
          l_pRbufMtx->m_lid = m_lid;

          // Once initialized un-map the shared memory
          munmap(l_pRbufMtx, sizeof(NSRingBufferMtx));
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
/// CreateRBHeader
/// This function creates the ring buffer object for header.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSRingBuffer::CreateRBHeader() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  // file descriptor of ring buffer
  SI_32 l_siFd = -1;

  // ring buffer headers
  NSRingBufferHdr *l_pRbufHdr = NULL;

  if (!m_cMappedFileHdrPath.empty()) {  // LCOV_EXCL_BR_LINE 11: except branch
    // Try to create ring buffer
    l_siFd = open(m_cMappedFileHdrPath.c_str(), O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, 0640);

    if (-1 != l_siFd) {
      // Set the size of ring buffer
      if (-1 != ftruncate(l_siFd, sizeof(NSRingBufferHdr))) {
        // Map the ring buffer
        l_pRbufHdr = reinterpret_cast<NSRingBufferHdr *>(mmap(NULL,
          sizeof(NSRingBufferHdr), (PROT_READ | PROT_WRITE), MAP_SHARED, l_siFd, 0));

        if (MAP_FAILED != l_pRbufHdr) {

#if 0
          LockMtx();

          // Initialize the r/w pointers
          l_pRbufHdr->m_uiReadPtr = 0;
          l_pRbufHdr->m_uiWritePtr = 0;
          l_pRbufHdr->m_uiUnReadSize = 0;
          l_pRbufHdr->m_bIsFull = FALSE;
          l_pRbufHdr->m_uiRingBufferSize = m_uiRingBuffSize;

          UnlockMtx();
#endif
          int ret;
          if ( (ret = LockMtx()) == 0) {
            // Initialize the r/w pointers
            l_pRbufHdr->m_uiReadPtr = 0;
            l_pRbufHdr->m_uiWritePtr = 0;
            l_pRbufHdr->m_uiUnReadSize = 0;
            l_pRbufHdr->m_bIsFull = FALSE;
            l_pRbufHdr->m_uiRingBufferSize = m_uiRingBuffSize;

            UnlockMtx();
            l_eStatus = eFrameworkunifiedStatusOK;
          } else {
            fprintf(stderr, "[%s] LockMtx error: %s\n", __PRETTY_FUNCTION__, strerror(ret));
            l_eStatus = eFrameworkunifiedStatusFail;
          }


          // Once initialized un-map the ring buffer
          munmap(l_pRbufHdr, sizeof(NSRingBufferHdr));
        }
      }

      close(l_siFd);
    } else if (EEXIST == errno) {
      // ring buffer is already created
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
/// CreateRBDataBuffer
/// This function creates the ring buffer object for data buffer.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSRingBuffer::CreateRBDataBuffer() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  // file descriptor of ring buffer
  SI_32 l_siFd = -1;

  if (!m_cMappedFilePath.empty()) {
    // Try to create ring buffer
    l_siFd = open(m_cMappedFilePath.c_str(), O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, 0640);

    if (-1 != l_siFd) {
      // Set the size of ring buffer
      if (-1 != ftruncate(l_siFd, m_uiRingBuffSize)) {
        l_eStatus = eFrameworkunifiedStatusOK;
      }

      close(l_siFd);
    } else if (EEXIST == errno) {
      // ring buffer is already created
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
/// MapRBHeader
/// This function open and maps in process space.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSRingBuffer::Map(PVOID *f_pRbuf, const std::string &f_cMappedFile, const UI_32 f_uiRbufSize) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  // file descriptor of ring buffer
  SI_32 l_siFd = -1;

  // ring buffer headers
  PVOID l_pRbuf = NULL;

  // Open ring buffer
  l_siFd = open(f_cMappedFile.c_str(), O_RDWR | O_CLOEXEC, NULL);

  if (-1 != l_siFd) {
    if (CheckSize(l_siFd, f_uiRbufSize) == eFrameworkunifiedStatusOK) {
      // Map the ring buffer into its memory space
      l_pRbuf = mmap(NULL, f_uiRbufSize, (PROT_READ | PROT_WRITE), MAP_SHARED, l_siFd, 0);

      if (MAP_FAILED != l_pRbuf) {
        *f_pRbuf = l_pRbuf;
        l_eStatus = eFrameworkunifiedStatusOK;
      }
    }

    close(l_siFd);
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// CheckSize
/// This function check mmap size
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSRingBuffer::CheckSize(int fd, off_t size) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  struct stat st;
  int i;

  for (i = 0; i < RBUF_RETRY_COUNT; i++) {
    if (fstat(fd, &st) < 0) {
      fprintf(stderr, "[CNSRingBuffer::CheckSize] fstat error: %s\n", strerror(errno));
      goto out;
    }
    if (st.st_size == size) {
      l_eStatus = eFrameworkunifiedStatusOK;
      break;
    }
    usleep(RBUF_RETRY_SLEEP);
  }
  if (i >= RBUF_RETRY_COUNT) {
    errno = ENOMEM;
  }

out:
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// MapSM
/// This function shm_open and maps the shared memory in process space.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSRingBuffer::MapSM(PVOID *f_pShMem, const std::string &f_cShmName, const UI_32 f_uiShmSize) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  // file descriptor of shared memory
  SI_32 l_siId = -1;

  // shared memory buffer headers
  PVOID l_pShmBuf = NULL;

  // Open shared memory
  l_siId = shm_open(f_cShmName.c_str(), O_RDWR, 0640);

  if (-1 != l_siId) {
    if (CheckSize(l_siId, f_uiShmSize) == eFrameworkunifiedStatusOK) {
      // Map the shared memory into its memory space
      l_pShmBuf = mmap(NULL, f_uiShmSize, (PROT_READ | PROT_WRITE), MAP_SHARED, l_siId, 0);

      if (MAP_FAILED != l_pShmBuf) {
        *f_pShMem = l_pShmBuf;
        l_eStatus = eFrameworkunifiedStatusOK;
      }
    }

    close(l_siId);
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// UnMap
/// This function unmaps object.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSRingBuffer::UnMap(PVOID f_pRbuf, const UI_32 f_uiRbufSize) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  // Un-map the ring buffer
  if (NULL != f_pRbuf) {
    if (0 != munmap(f_pRbuf, f_uiRbufSize)) {
      l_eStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}


//void CNSRingBuffer::LockMtx() {
//  if (m_lid == -1) {
//    pthread_mutex_lock(&m_pRbufMtx->m_tBufMutex);
//  } else {
//    CL_LockGet(m_pLockAddr);
//  }
//}
int CNSRingBuffer::LockMtx() {
  int ret = 0;

  if (m_lid == -1) {
    ret = pthread_mutex_lock(&m_pRbufMtx->m_tBufMutex);
  } else {
    ret = CL_LockGet(m_pLockAddr);
  }

  return ret;
}


void CNSRingBuffer::UnlockMtx() {
  if (m_lid == -1) {
    pthread_mutex_unlock(&m_pRbufMtx->m_tBufMutex);
  } else {
    CL_LockRelease(m_pLockAddr);
  }
}
