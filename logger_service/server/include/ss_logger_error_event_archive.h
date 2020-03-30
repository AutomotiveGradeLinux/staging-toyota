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
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_ARCHIVE_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_ARCHIVE_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>

#include <libtar_listhash.h>
#include <libtar.h>

#include <system_service/ss_system_types.h>
#include <string>
#include <vector>

class CErrorEventArchive {
 public:
  CErrorEventArchive();
  ~CErrorEventArchive();

  EFrameworkunifiedStatus openArchive(std::string f_pathAndName);

  EFrameworkunifiedStatus addToArchive(std::vector<std::string> f_filePathAndNameList);

  EFrameworkunifiedStatus addToArchive(std::string f_filePathAndName,
                          std::string f_destinationName, UI_8 f_depth =
                              CErrorEventArchive::MAX_DIRECTORY_DEPTH);

  EFrameworkunifiedStatus closeArchive(void);

  static SI_32 gzOpenArchive(PCHAR f_pcPathname, SI_32 f_siOflags,
                             SI_32 f_siMode);

  static SI_32 gzCloseArchive();

  static ssize_t gzWriteArchive(int fd, const void* buf, size_t count);

  static ssize_t gzReadArchive(int fd, void* buf, size_t count);

  static const UI_8 MAX_DIRECTORY_DEPTH = 4;

 private:
  EFrameworkunifiedStatus addFileToArchive(std::string f_filename,
                              std::string f_destinationName);

  EFrameworkunifiedStatus addDirectoryToArchive(std::string f_path,
                                   std::string f_destinationName, UI_8 f_depth =
                                       CErrorEventArchive::MAX_DIRECTORY_DEPTH);

  tartype_t m_gztype;
  TAR *m_pTarInfo;
  std::string m_pathAndName;
  std::string m_tmpName;

  static const UI_32 TARMODE = 0644;
};

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_ERROR_EVENT_ARCHIVE_H_
