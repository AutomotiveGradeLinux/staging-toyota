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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SS_LoggerService
/// \brief    TODO
///
///////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_READER_WRITER_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_READER_WRITER_H_

#include <pthread.h>
#include <native_service/frameworkunified_types.h>
#include <string>
#include "readerWriter/reader_writer_cfg.h"
#include "readerWriter/reader/reader.h"
#include "readerWriter/writer/writer.h"

namespace ReaderWriter {

class CReaderWriter {
 public:
  CReaderWriter();
  virtual ~CReaderWriter();

  EFrameworkunifiedStatus Initialize(CLoggerCfg* f_pLoggerCfg, EReaderType f_readerType,
                        std::string f_readerName, UI_32 f_readerMaxSize,
                        EWriterType f_writerType, std::string f_writer1Name,
                        UI_32 f_writer1MaxSize);
  EFrameworkunifiedStatus Initialize(CLoggerCfg* f_pLoggerCfg, EReaderType f_readerType,
                        std::string f_readerName, UI_32 f_readerMaxSize,
                        EWriterType f_writerType, std::string f_writer1Name,
                        UI_32 f_writer1MaxSize, std::string f_writer2Name,
                        UI_32 f_writer2MaxSize);
  EFrameworkunifiedStatus Start(std::string f_readerName);
  EFrameworkunifiedStatus Stop();
  void ThreadFunction(CReaderWriter* l_pObj);
  void Cleanup(CReaderWriter* l_pObj);
  EFrameworkunifiedStatus FlushCache(std::string f_filename);
  EFrameworkunifiedStatus UpdateLoggingParameters(void);

 private:
  void Pause(void);
  void Resume(void);
  static void* ThreadFunctionWrapper(void* param);

  static void CleanupWrapper(void* param);
  CLoggerCfg* m_pLoggerCfg;
  CReader* m_reader;
  CWriter* m_writer;
  BOOL m_running;
  pthread_t m_thread;
  pthread_barrier_t m_barrier;
  std::string m_readerName;

  pthread_mutex_t m_writeMutex;
  pthread_mutex_t m_pauseMutex;
  pthread_cond_t m_condVariable;
  BOOL m_paused;
};

}  // namespace ReaderWriter

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_READERWRITER_READER_WRITER_H_
