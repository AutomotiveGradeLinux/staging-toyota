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
/// \brief    This file contains implementation of class CNSSharedMemReader.
///           This class provides API to open, close and perform read operation on shared memory.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_sharedmem.h>
#include <native_service/ns_sharedmem_reader.h>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////
/// CNSSharedMemReader
/// Constructor of CNSSharedMemReader class
////////////////////////////////////////////////////////////////////////////////////////////////
CNSSharedMemReader::CNSSharedMemReader(): m_pShmReader(NULL), m_bBlock(TRUE) {
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// CNSSharedMemReader
/// Parameterized Constructor of CNSSharedMemReader class
/// This creates the shared memory object.
////////////////////////////////////////////////////////////////////////////////////////////////
CNSSharedMemReader::CNSSharedMemReader(const std::string &f_cSharedMemName, const BOOL f_bBlock): m_bBlock(f_bBlock) {
  m_pShmReader = new(std::nothrow) CNSSharedMem(f_cSharedMemName, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CNSSharedMemReader
/// Destructor of CNSSharedMemReader class.
/// Closes the shared memory, if it is open.
////////////////////////////////////////////////////////////////////////////////////////////////
CNSSharedMemReader::~CNSSharedMemReader() {
  if (NULL != m_pShmReader) {
    if (m_pShmReader->IsOpen()) {
      m_pShmReader->Close();
    }

    delete m_pShmReader;
    m_pShmReader = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Open
/// This function opens and maps the shared memory object.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMemReader::Open() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pShmReader) {
    l_eStatus = m_pShmReader->Open();
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// IsOpen
/// This function is used to check whether the shared memory buffer is opened or not.
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CNSSharedMemReader::IsOpen() {
  BOOL l_bOpen = FALSE;

  if (NULL != m_pShmReader) {
    l_bOpen = m_pShmReader->IsOpen();
  }

  return l_bOpen;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Close
/// This function unmaps the shared memory object.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMemReader::Close() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pShmReader) {
    l_eStatus = m_pShmReader->Close();
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Read
/// This function reads data from the shared memory.
////////////////////////////////////////////////////////////////////////////////////////////////
SI_32 CNSSharedMemReader::Read(PSTR buffer, const UI_32 f_uilength) {
  SI_32 l_iReadSize = NS_SHM_ERROR;

  if (NULL != m_pShmReader) {
    l_iReadSize = m_pShmReader->Read(buffer, f_uilength, m_bBlock);
  }

  return l_iReadSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// DumpToFile
/// This function writes all the data in the buffer into provided file f_pPath.
/// This function does not changes the unread buffer.
/// This function overwrites the file if it exists.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMemReader::DumpToFile(PCSTR f_pPath, PUI_32 f_uiDumpSize) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pShmReader) {
    l_eStatus = m_pShmReader->DumpToFile(f_pPath, f_uiDumpSize);
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// GetSize
/// This function returns the number of unread bytes which can be read by Read().
////////////////////////////////////////////////////////////////////////////////////////////////
SI_32 CNSSharedMemReader::GetSize() {
  UI_32 l_uiReadSize = NS_SHM_ERROR;

  if (NULL != m_pShmReader) {
    l_uiReadSize = m_pShmReader->GetSize();
  }

  return l_uiReadSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// SetReadPtrToWritePtr
/// This function sets the position of read ptr to write ptr in buffer.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMemReader::SetReadPtrToWritePtr() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pShmReader) {
    l_eStatus = m_pShmReader->SetReadPtrToWritePtr();
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}
