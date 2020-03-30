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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SS_LoggerService
/// \brief    TODO
///
///////////////////////////////////////////////////////////////////////////////
#include "readerWriter/reader_writer.h"
#include <errno.h>
#include <loggerservicedebug_loggerservicelog.h>
#include <system_service/ss_templates.h>
#include <native_service/frameworkunified_thread_priority.h>
#include <string>

namespace ReaderWriter {

CReaderWriter::CReaderWriter()
    : m_pLoggerCfg(NULL),
      m_reader(NULL),
      m_writer(NULL),
      m_running(FALSE),
      m_thread(-1),
      m_readerName(""),
      m_paused(FALSE) {

  pthread_cond_init(&m_condVariable, NULL);
}

CReaderWriter::~CReaderWriter() {  // LCOV_EXCL_START 14:global instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (NULL != this->m_reader) {
    delete (this->m_reader);
    this->m_reader = NULL;
  }

  if (NULL != this->m_writer) {
    delete (this->m_writer);
    this->m_writer = NULL;
  }
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CReaderWriter::Initialize(CLoggerCfg* f_pLoggerCfg,
                                     EReaderType f_readerType,
                                     std::string f_readerName,
                                     UI_32 f_readerMaxSize,
                                     EWriterType f_writerType,
                                     std::string f_writer1Name,
                                     UI_32 f_writer1MaxSize,
                                     std::string f_writer2Name,
                                     UI_32 f_writer2MaxSize) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (NULL == (this->m_pLoggerCfg = f_pLoggerCfg)) {  // LCOV_EXCL_BR_LINE 6:Because the applicable variable cannot be changed from the external API
    // FRAMEWORKUNIFIEDLOG not needed... If this starts we won't be able to print loggerservicelogs anyways.
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  } else {
    this->m_reader = CReader::OpenReader(f_pLoggerCfg, f_readerType,  // LCOV_EXCL_BR_LINE 11:except,C++ STL
                                         f_readerName, f_readerMaxSize);
    this->m_writer = CWriter::OpenWriter(f_pLoggerCfg, f_writerType,  // LCOV_EXCL_BR_LINE 11:except,C++ STL
                                         f_writer1Name, f_writer1MaxSize,
                                         f_writer2Name, f_writer2MaxSize);

    if ((NULL == this->m_reader) || (NULL == this->m_writer)) {
      l_eStatus = eFrameworkunifiedStatusFail;

      this->Cleanup(this);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %d", l_eStatus);
  return (l_eStatus);
}

EFrameworkunifiedStatus CReaderWriter::Start(std::string f_readerName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusNullPointer;
  if (this->m_running == FALSE) {  // LCOV_EXCL_BR_LINE 6:Because m_running is always FALSE
    this->m_readerName = f_readerName;
    BOOL l_barInit =
        // LCOV_EXCL_BR_START 5:pthread cannot be passed because it cannot be turned mock
        (EOK == pthread_barrier_init(&m_barrier, NULL, 2)) ? TRUE : FALSE;
    if (l_barInit) {
        // LCOV_EXCL_BR_STOP
      pthread_attr_t attr;
      struct sched_param params;
      pthread_attr_init(&attr);
      pthread_attr_getschedparam(&attr, &params);
      SI_32 prio = frameworkunified::framework::CFrameworkunifiedThreadPriorities::GetPriority(
          f_readerName);
      if (-1 != prio) {  // LCOV_EXCL_BR_LINE 6:Because prio is always -1
        // LCOV_EXCL_START 6:Because the prio is not always -1
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        params.sched_priority = prio;
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedparam(&attr, &params);
        // LCOV_EXCL_STOP
      }

      if (EOK  == pthread_create(&m_thread, &attr, &CReaderWriter::ThreadFunctionWrapper, this)) {  // LCOV_EXCL_BR_LINE 5:pthread cannot be passed because it cannot be turned mock  // NOLINT[whitespace/line_length]
        this->m_running = TRUE;
        l_eStatus = eFrameworkunifiedStatusOK;
        pthread_barrier_wait(&this->m_barrier); /* Wait for thread to take off*/
      }
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusOK;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %d", l_eStatus);
  return (l_eStatus);
}

void* CReaderWriter::ThreadFunctionWrapper(void* param) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CReaderWriter* l_pObj = reinterpret_cast<CReaderWriter *>(param);

  l_pObj->ThreadFunction(l_pObj);
  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. ThreadFunction has exited");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return NULL;
}

void CReaderWriter::ThreadFunction(CReaderWriter* l_pObj) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  int l_oldState;

  pthread_setname_np(pthread_self(), this->m_readerName.c_str());
  pthread_cleanup_push(&CReaderWriter::CleanupWrapper, this);
    pthread_mutex_init(&m_writeMutex, NULL);
    pthread_mutex_init(&m_pauseMutex, NULL);
    int l_wait = pthread_barrier_wait(&m_barrier);

    if ((eFrameworkunifiedStatusOK == l_eStatus)
        // LCOV_EXCL_BR_START 5:pthread cannot be passed because it cannot be turned mock
        && (((PTHREAD_BARRIER_SERIAL_THREAD == l_wait) || (0 == l_wait)))) {
        // LCOV_EXCL_BR_STOP
      UI_8 data[MAX_QUEUE_MSG_SIZE + 1];
      while (TRUE) {
        if (TRUE == this->m_paused) {
          if (EOK == pthread_mutex_lock(&this->m_pauseMutex)) {  // LCOV_EXCL_BR_LINE 5:pthread cannot be passed because it cannot be turned mock
            (void)pthread_cond_wait(&m_condVariable, &m_pauseMutex);  // LCOV_EXCL_BR_LINE 5:pthread cannot be passed because it cannot be turned mock
            (void) pthread_mutex_unlock(&this->m_pauseMutex);  // LCOV_EXCL_BR_LINE 5:pthread cannot be passed because it cannot be turned mock
          }
        }
        if (!this->m_reader->IsOpen() || !this->m_writer->IsOpen()) {  // LCOV_EXCL_BR_LINE 6:As it is not always open
          (void)this->m_reader->Open();  // LCOV_EXCL_BR_LINE 11:Unexpected branch
          (void)this->m_writer->Open();  // LCOV_EXCL_BR_LINE 11:Unexpected branch
        }

        if (!this->m_reader->IsOpen() || !this->m_writer->IsOpen()) {

// ->Reduced frequency of accesses to IsOpen()
          usleep(1000*1000);  // LCOV_EXCL_BR_LINE 5:C function
        } else {
          while ((eFrameworkunifiedStatusOK == l_eStatus) && (FALSE == this->m_paused)) {
            SI_32 l_bytesRead = -1;
            SI_32 l_bytesWritten = -1;
            l_eStatus = this->m_reader->Read(data,
                MAX_QUEUE_MSG_SIZE,
                l_bytesRead);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
            if (eFrameworkunifiedStatusOK == l_eStatus) {
              pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,
                  &l_oldState);
              if (EOK == pthread_mutex_lock(&this->m_writeMutex)) {  // LCOV_EXCL_BR_LINE 5: c code error case
                l_eStatus = this->m_writer->Write(data,
                    (UI_32) l_bytesRead,
                    l_bytesWritten);
                (void) pthread_mutex_unlock(&this->m_writeMutex);
              }
              pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,
                  &l_oldState);
            }
          }
          l_eStatus = eFrameworkunifiedStatusOK;  // Reset l_eStatus
        }
      }
    }

    pthread_cleanup_pop(1);
}


void CReaderWriter::CleanupWrapper(void* param) {
  CReaderWriter* l_pObj = reinterpret_cast<CReaderWriter *>(param);

  l_pObj->Cleanup(l_pObj);
}

void CReaderWriter::Cleanup(CReaderWriter* l_pObj) {

//  (void) pthread_mutex_unlock(&this->m_pauseMutex);
//  (void) pthread_mutex_unlock(&this->m_writeMutex);


  if (this->m_reader != NULL) {  // LCOV_EXCL_BR_LINE 5: new will aways sucess, so m_reader can not be null
    delete (this->m_reader);
    this->m_reader = NULL;
  }
  if (this->m_writer != NULL) {  // LCOV_EXCL_BR_LINE 5: new will aways sucess, so m_writer can not be null
    delete (this->m_writer);
    this->m_writer = NULL;
  }
  this->m_running = FALSE;
}

EFrameworkunifiedStatus CReaderWriter::FlushCache(std::string f_filename) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (TRUE == this->m_pLoggerCfg->IsLoggingEnabled()) {  // LCOV_EXCL_BR_LINE 6: it can not be false
    if (EOK == pthread_mutex_lock(&this->m_writeMutex)) {  // LCOV_EXCL_BR_LINE 5:pthread cannot be passed because it cannot be turned mock
      if (this->m_writer != NULL) {  // LCOV_EXCL_BR_LINE 5: new will aways sucess, so m_writer can not be null
        l_eStatus = this->m_writer->FlushCache();
        LOG_STATUS_IF_ERRORED(l_eStatus, "this->m_writer->FlushCache()")
      }
      (void) pthread_mutex_unlock(&this->m_writeMutex);
    }

    UI_32 l_written;
    if (NULL != this->m_reader) {  // LCOV_EXCL_BR_LINE 6:Because it depends on the startup process
      l_eStatus = this->m_reader->ReadToFile(f_filename, l_written);
      /*Don't output error, if function not implemented*/
      l_eStatus =
          (l_eStatus == eFrameworkunifiedStatusNotImplemented) ? eFrameworkunifiedStatusOK : l_eStatus;
      LOG_STATUS_IF_ERRORED(l_eStatus,
                            "this->m_reader->ReadToFile(f_filename, l_written)");
    } else {
      l_eStatus = eFrameworkunifiedStatusNullPointer;
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CReaderWriter::UpdateLoggingParameters(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (EOK == pthread_mutex_lock(&this->m_writeMutex)) {  // LCOV_EXCL_BR_LINE 5:pthread cannot be passed because it cannot be turned mock
    if (this->m_writer != NULL) {
      l_eStatus = this->m_writer->UpdateLoggingParameters();
    }
    (void) pthread_mutex_unlock(&this->m_writeMutex);
  }
  BOOL l_enabled = this->m_pLoggerCfg->IsLoggingEnabled();
  if ((this->m_paused == FALSE) && (FALSE == l_enabled)) {
    this->Pause();
  } else if ((this->m_paused == TRUE) && (TRUE == l_enabled)) {
    this->Resume();
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "- : %d", l_eStatus);
  return (l_eStatus);
}

void CReaderWriter::Pause(void) {
  (void) pthread_mutex_lock(&this->m_pauseMutex);
  this->m_paused = TRUE;

  (void) pthread_mutex_unlock(&this->m_pauseMutex);
}

void CReaderWriter::Resume(void) {
  (void) pthread_mutex_lock(&this->m_pauseMutex);
  if (this->m_reader != NULL) {  // LCOV_EXCL_BR_LINE 5: new will aways sucess, so m_reader can not be null
    (void) this->m_reader->ResetPosition();
  }
  pthread_cond_signal(&m_condVariable);
  this->m_paused = FALSE;
  (void) pthread_mutex_unlock(&this->m_pauseMutex);
}
}  // namespace ReaderWriter
