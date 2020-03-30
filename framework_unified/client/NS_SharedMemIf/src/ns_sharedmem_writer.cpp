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
#include <native_service/ns_sharedmem_writer.h>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////
/// CNSSharedMemWriter
/// Constructor of CNSSharedMemWriter class
////////////////////////////////////////////////////////////////////////////////////////////////
CNSSharedMemWriter::CNSSharedMemWriter(): m_pShmWriter(NULL) {
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// CNSSharedMemWriter
/// Parameterized Constructor of CNSSharedMemWriter class
////////////////////////////////////////////////////////////////////////////////////////////////
CNSSharedMemWriter::CNSSharedMemWriter(const std::string &f_cSharedMemName, const UI_32 f_uiSize) {
  m_pShmWriter = new(std::nothrow) CNSSharedMem(f_cSharedMemName, f_uiSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CNSSharedMemWriter
/// Destructor of CNSSharedMemWriter class.
/// Closes the shared memory, if it is open.
////////////////////////////////////////////////////////////////////////////////////////////////
CNSSharedMemWriter::~CNSSharedMemWriter() {
  if (NULL != m_pShmWriter) {
    if (m_pShmWriter->IsOpen()) {
      m_pShmWriter->Close();
    }

    delete m_pShmWriter;
    m_pShmWriter = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Open
/// This function opens and maps the shared memory object.
/// It creates the shared memory if it does not exists.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMemWriter::Open() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pShmWriter) {
    l_eStatus = m_pShmWriter->Open();
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// IsOpen
/// This function is used to check whether the shared memory buffer is opened or not.
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CNSSharedMemWriter::IsOpen() {
  BOOL l_bOpen = FALSE;

  if (NULL != m_pShmWriter) {
    l_bOpen = m_pShmWriter->IsOpen();
  }

  return l_bOpen;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Close
/// This function unmaps the shared memory object.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMemWriter::Close() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pShmWriter) {
    l_eStatus = m_pShmWriter->Close();
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Write
/// This function writes the data into the shared memory.
////////////////////////////////////////////////////////////////////////////////////////////////
SI_32 CNSSharedMemWriter::Write(PCSTR buffer, const UI_32 f_uilength) {
  SI_32 l_iWriteSize = NS_SHM_ERROR;

  if ((NULL != m_pShmWriter) && (NULL != buffer) && (0 != f_uilength)) {
    l_iWriteSize = m_pShmWriter->Write(buffer, f_uilength);
  }

  return l_iWriteSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ClearBuf
/// This function clears the shared memory buffer.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNSSharedMemWriter::ClearBuf() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pShmWriter) {
    l_eStatus = m_pShmWriter->ClearBuf();
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}
