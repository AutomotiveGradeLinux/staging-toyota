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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup MY 14 Platform Software Team
/// \brief   Internal Header for ns_config_parser_cfg.c
///
///  These structures are file handler structure.
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_XMLCONFIGEPARSER_LIBRARY_INCLUDE_NS_CONFIG_PARSER_INTERNAL_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_XMLCONFIGEPARSER_LIBRARY_INCLUDE_NS_CONFIG_PARSER_INTERNAL_H_

#include <libxml/tree.h>
#include <ns_config_parser.h>


// defines enum for MODE
// in which file is opened
typedef enum MODE {
  kReadMode = 0, kWriteMode, kAppend
} MODE;

// defines enum for STATUS
typedef enum STATUS {
  kSuccess = 0, kFailure
} STATUS;

// defines enum for Filetype
// i.e .cfg or .xml or other type of file
typedef enum Filetype {
  kCfg = 0, kXml, kOther
} Filetype;

// FILE STRUCTURE
typedef struct FILESTRUCT {
  char *filename;
  Filetype filetype;
  MODE mode;
  xmlDocPtr doc;
  FILE *fileptr;
} FILESTRUCT;


#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_XMLCONFIGEPARSER_LIBRARY_INCLUDE_NS_CONFIG_PARSER_INTERNAL_H_
