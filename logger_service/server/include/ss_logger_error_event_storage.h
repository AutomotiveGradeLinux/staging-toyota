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
/// \brief    This file supports error event logging.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_STORAGE_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_STORAGE_H_

#include <sys/types.h>
#include <pthread.h>
#include <mqueue.h>
#include <time.h>
#include <native_service/frameworkunified_types.h>
#include <ss_logger_error_event_cfg.h>
//#include <vehicle_service/Clock_API.h>
#include <string>
#include "ss_logger_cfg.h"

enum ELoggerStorageThreadCommands {
  eLoggerStorageThreadCmdOK = 0xAFFE,
  eLoggerStorageThreadCmdWriteFailed,
  eLoggerStorageThreadCmdNotFound,
  eLoggerStorageThreadCmdNoWritten
};

class CLoggerErrorEventStorage {
 public:
  CLoggerErrorEventStorage();
  virtual ~CLoggerErrorEventStorage();

  EFrameworkunifiedStatus Initialize(HANDLE f_hApp, std::string f_ParentName,
                        CLoggerCfg *f_pLoggerCfg);

  EFrameworkunifiedStatus Start(std::string f_target, TArtifactResponseVec f_responseVector,
                   uint32_t f_time);

  void ThreadFunction(void);
  void Cleanup(void);

 private:
  static void* ThreadFunctionWrapper(void* param);
  static void CleanupWrapper(void* param);

  EFrameworkunifiedStatus PackageAndPlaceArtifacts(std::string f_archiveDestination,
                                      TArtifactResponseVec &f_responseVector,  // NOLINT (runtime/references)
                                      uint32_t l_logTime);

  SI_32 CopyFiles(std::string f_archiveDestination,
                  TArtifactResponseVec &f_responseVector, BOOL isDeleteFiles);  // NOLINT (runtime/references)

  EFrameworkunifiedStatus RemoveDeprecatedArtifacts(TArtifactResponseVec &f_responseVector);  // NOLINT (runtime/references)
  HANDLE m_parentMcQueue;
  pthread_t m_thread;
  pthread_mutex_t m_mutex;
  mqd_t m_sendQueue;
  mqd_t m_receiveQueue;
  std::string m_destination;
  TArtifactResponseVec m_responseVec;
  struct mq_attr m_mqattr;
  SI_32 m_mutexInit;
  CLoggerCfg *m_pLoggerCfg;
  SLoggerStorageInfo m_loggerStorageInfo;
  uint32_t m_logTime;
};

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_STORAGE_H_
