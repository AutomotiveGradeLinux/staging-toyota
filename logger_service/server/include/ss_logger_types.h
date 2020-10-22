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
/// \brief    This file supports shared internal logger module data types.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_TYPES_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_TYPES_H_

#include <native_service/frameworkunified_sm_eventdata.h>

typedef enum _ENPS_Archivetype {
  ARCHTYPE_COMPRESSION = 0,  // Compression
  ARCHTYPE_DECOMPRESSION = 1  // DeCompression
} ENPS_Archivetype;

typedef CHAR CopyInfoStr[MAX_PATH_LENGTH];  ///< Information passed via the framework

typedef enum _LOGGERSERVICE_COMPRESSION_TYPE {
  eLoggerserviceNone = 0,        // No Compression before file persistence
  eLoggerserviceDefaultCompression,  // Compress file before persistence using default compression method
  eLoggerserviceCompressUsingLibz    // Compress file before persistence using libz
} LOGGERSERVICE_COMPRESSION_TYPE;

typedef struct _NPS_CopyInfoMsg {
  CopyInfoStr m_cSourcePath;      // Source path
  CopyInfoStr m_cDestinationPath;  // Destination path
  ENPS_Archivetype m_eArchiveType;    // Compression or DeCompression
  LOGGERSERVICE_COMPRESSION_TYPE m_eCompressionType;  // Check for Compression of the file
} NSP_CopyInfoCmd;

typedef struct _NPS_CopyStatus {
  BOOL m_bPersistenceChk;  // Persistence Success or Failure
  LOGGERSERVICE_COMPRESSION_TYPE m_eCompressionType;  // Check for Compression of the file
} NSP_CopyStatusResponse, NSP_CopyCancel;

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_TYPES_H_
