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

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <libxml/xmlerror.h>
#include <libxml/parser.h>
#include <libxml/SAX2.h>
#include <inttypes.h>
#include <cerrno>
#include <cstdio>
#include <string>
#include <map>
#include <utility>
#include "bkup_param.h"
#include "bkup_backupmanagerlog.h"
#include "bkup_util.h"

#define BKUP_XML_PATH "/usr/agl/conf/backup_manager/backup.xml"

typedef std::string Category;
typedef struct {
  bool nand;
  bool cache_dram;
  bool backup_dram;
  bool sync;
  bool encrypt;
  int backup_cycle;
} CategoryProperty;
typedef std::map<Category, CategoryProperty> CategoryTable;

typedef std::string Item;
typedef struct {
  int id;
  int size;
  void *opt;
  Category category;
} ItemProperty;  // LCOV_EXCL_BR_LINE 11:except,C++ STL
typedef std::map<Item, ItemProperty> ItemTable;
typedef std::map<int, Item> IdTable;

typedef struct {
  Category current;
  CategoryTable category_table;
  ItemTable item_table;
  IdTable id_table;
} BackupParams;  // LCOV_EXCL_BR_LINE 11:except,C++ STL

static BackupParams *g_backup_params = NULL;

static void StartElementNs(void *ctx, const xmlChar *localname,
                           const xmlChar *prefix, const xmlChar *uri,
                           int nb_namespaces, const xmlChar **namespaces,
                           int nb_attributes, int nb_defaulted,
                           const xmlChar **attributes) {
  BackupParams *p_backup_params = static_cast<BackupParams *>(ctx);
  std::string tag = (const char *)localname;  // LCOV_EXCL_BR_LINE 11:except,C++ STL

  if (tag.compare("backup") == 0) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    // do nothing
  } else if (tag.compare("category") == 0) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    CategoryProperty category_property = {false, false, false, false, false, 0};
    std::string category_name;  // LCOV_EXCL_BR_LINE 11:except,C++ STL

    for (int i = 0; i < nb_attributes; i++) {
      // LCOV_EXCL_BR_START 11:except,C++ STL
      std::string localname = (const char *)attributes[i * 5];
      std::string value((const char *)attributes[i * 5 + 3], (const char *)attributes[i * 5 + 4]);
      if (localname.compare("name") == 0) {
        category_name = value;
      } else if (localname.compare("nand") == 0) {
        if (value.compare("true") == 0) {
          category_property.nand = true;
        }
      } else if (localname.compare("backupDram") == 0) {
        if (value.compare("true") == 0) {
          category_property.backup_dram = true;
        }
      } else if (localname.compare("cacheDram") == 0) {
        if (value.compare("true") == 0) {
          category_property.cache_dram = true;
        }
      } else if (localname.compare("sync") == 0) {
        if (value.compare("true") == 0) {
          category_property.sync = true;
        }
      } else if (localname.compare("encrypt") == 0) {
        if (value.compare("true") == 0) {
          category_property.encrypt = true;
        }
      } else if (localname.compare("backupCycle") == 0) {
        category_property.backup_cycle = atoi(value.c_str());
      // LCOV_EXCL_BR_STOP 11:except,C++ STL
      } else {
        // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __func__, "Unknown attribute:%s", localname.c_str());
        // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
      }
    }

    // LCOV_EXCL_BR_START 11:except,C++ STL
    CategoryTable::iterator c_it = p_backup_params->category_table.find(category_name);
    // LCOV_EXCL_BR_STOP 11:except,C++ STL
    if (c_it != p_backup_params->category_table.end()) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
      // LCOV_EXCL_START 200:Execute in server start period
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "duplicate category:%s", category_name.c_str());
      exit(EXIT_FAILURE);
      // LCOV_EXCL_STOP 200:Execute in server start period
    }
    // LCOV_EXCL_BR_START 11:except,C++ STL
    p_backup_params->category_table.insert(std::make_pair(category_name, category_property));
    // LCOV_EXCL_BR_STOP 11:except,C++ STL
    p_backup_params->current = category_name;  // LCOV_EXCL_BR_LINE 11:except,C++ STL
  } else if (tag.compare("item") == 0) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    if (p_backup_params->current.compare("") == 0) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
      // LCOV_EXCL_START 200:Execute in server start period
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Detect item before category");
      exit(EXIT_FAILURE);
      // LCOV_EXCL_STOP 200:Execute in server start period
    }

    ItemProperty item_property = {0, 0, NULL, p_backup_params->current};  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    std::string item_name;  // LCOV_EXCL_BR_LINE 11:except,C++ STL

    for (int i = 0; i < nb_attributes; i++) {
      // LCOV_EXCL_BR_START 11:except,C++ STL
      std::string localname = (const char *)attributes[i * 5];
      std::string value((const char *)attributes[i * 5 + 3], (const char *)attributes[i * 5 + 4]);
      if (localname.compare("name") == 0) {
        item_name = value;
      } else if (localname.compare("id") == 0) {
        item_property.id = atoi(value.c_str());
      } else if (localname.compare("size") == 0) {
        item_property.size = atoi(value.c_str());
      // LCOV_EXCL_BR_STOP 11:except,C++ STL
      } else {
        // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
        FRAMEWORKUNIFIEDLOG(ZONE_WARN, __func__, "Unknown attribute:%s", localname.c_str());
        // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
      }
    }

    ItemTable::iterator i_it = p_backup_params->item_table.find(item_name);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    if (i_it != p_backup_params->item_table.end()) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
      // LCOV_EXCL_START 200:Execute in server start period
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Duplicate item:%s", item_name.c_str());
      exit(EXIT_FAILURE);
      // LCOV_EXCL_STOP 200:Execute in server start period
    }
    // LCOV_EXCL_BR_START 11:except,C++ STL
    p_backup_params->item_table.insert(std::make_pair(item_name, item_property));
    // LCOV_EXCL_BR_STOP 11:except,C++ STL

    IdTable::iterator id_it = p_backup_params->id_table.find(item_property.id);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    if (id_it != p_backup_params->id_table.end()) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
      // LCOV_EXCL_START 200:Execute in server start period
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Duplicate id:%d", item_property.id);
      exit(EXIT_FAILURE);
      // LCOV_EXCL_STOP 200:Execute in server start period
    }
    // LCOV_EXCL_BR_START 11:except,C++ STL
    p_backup_params->id_table.insert(std::make_pair(item_property.id, item_name));
    // LCOV_EXCL_BR_STOP 11:except,C++ STL
  } else {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __func__, "Unknown tag:%s", localname);
    // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"
  }
}

int BckupParamInit(void) {
  int ret = -1;
  xmlParserCtxtPtr xmlctx;
  xmlSAXHandler saxh;
  xmlDocPtr doc;
  int fd = -1;
  struct stat sb;
  char *map = NULL;
  uint64_t start, end;

  g_backup_params = new BackupParams;  // LCOV_EXCL_BR_LINE 11:except,C++ operator

  LIBXML_TEST_VERSION  // LCOV_EXCL_BR_LINE 11:unexpected branch
  // LCOV_EXCL_BR_START 11:unexpected branch
  xmlInitParser();  // NOLINT(readability/nolint) Defined in library of libxml
  // LCOV_EXCL_BR_STOP 11:unexpected branch

  memset(&saxh, 0, sizeof(saxh));
  saxh.initialized = XML_SAX2_MAGIC;
  saxh.startElementNs = StartElementNs;

  xmlctx = xmlCreatePushParserCtxt(&saxh, g_backup_params, NULL, 0, NULL);  // LCOV_EXCL_BR_LINE 11:unexpected branch

  if ((fd = open(BKUP_XML_PATH, O_RDONLY)) < 0) {  // LCOV_EXCL_BR_LINE 5:open's error case.
    // LCOV_EXCL_START 5:open's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "open:%s", strerror(errno));
    goto exit;
    // LCOV_EXCL_STOP 5:open's error case.
  }
  if (fstat(fd, &sb) < 0) {  // LCOV_EXCL_BR_LINE 5:fstat's error case.
    // LCOV_EXCL_START 5:fstat's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "fstat:%s", strerror(errno));
    goto exit;
    // LCOV_EXCL_STOP 5:fstat's error case.
  }
  map = reinterpret_cast<char *>(mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
  if (map == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5:mmap's error case.
    // LCOV_EXCL_START 5:mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "mmap:%s", strerror(errno));
    goto exit;
    // LCOV_EXCL_STOP 5:mmap's error case.
  }

  start = BkupTimerReal();
  if (xmlParseChunk(xmlctx, map, static_cast<int>(sb.st_size), 1)) {  // LCOV_EXCL_BR_LINE 5:xmlParseChunk's error case.
    // LCOV_EXCL_START 5:xmlParseChunk's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    xmlParserError(xmlctx, "xmlParseChunk");
    goto exit;
    // LCOV_EXCL_STOP 5:xmlParseChunk's error case.
  }
  end = BkupTimerReal();
  // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __func__, "spent:%" PRIu64 ".%06" PRIu64 "ms",
         static_cast<uint64_t>((end - start) / 1000000),
         static_cast<uint64_t>((end - start) % 1000000));
  // LCOV_EXCL_BR_STOP 15:marco defined in "native_service/ns_logger_if.h"

  doc = xmlctx->myDoc;
  xmlFreeParserCtxt(xmlctx);  // LCOV_EXCL_BR_LINE 11:unexpected branch
  xmlFreeDoc(doc);  // LCOV_EXCL_BR_LINE 11:unexpected branch
  xmlCleanupParser();  // LCOV_EXCL_BR_LINE 11:unexpected branch
  ret = 0;

exit:
  if (fd >= 0) {  // LCOV_EXCL_BR_LINE 5:open's error case.
    if (close(fd) < 0) {  // LCOV_EXCL_BR_LINE 5:close's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, "close:%s", strerror(errno));  // LCOV_EXCL_LINE 5:close's error case.
    }
  }
  if (map) {  // LCOV_EXCL_BR_LINE 5:mmap's error case.
    if (munmap(map, sb.st_size) < 0) {  // LCOV_EXCL_BR_LINE 5:munmap's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, "munmap:%s", strerror(errno));  // LCOV_EXCL_LINE 5:munmap's error case.
    }
  }
  return ret;
}

int BkupParamGet(const char *item_name, bkup_query_result_t *result) {
  if (g_backup_params == NULL) {  // LCOV_EXCL_BR_LINE 6:g_backup_params will not be NULL
    // LCOV_EXCL_START 6:g_backup_params will not be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Uninitialize call");
    return -1;
    // LCOV_EXCL_STOP 6:g_backup_params will not be NULL
  }

  ItemTable::iterator item_it = g_backup_params->item_table.find(item_name);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
  if (item_it == g_backup_params->item_table.end()) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    return -1;
  }

  // LCOV_EXCL_BR_START 11:except,C++ STL
  CategoryTable::iterator category_it = g_backup_params->category_table.find(item_it->second.category);
  // LCOV_EXCL_BR_STOP 11:except,C++ STL
  if (category_it == g_backup_params->category_table.end()) {  // LCOV_EXCL_BR_LINE 6:double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return -1;  // LCOV_EXCL_LINE 6:double check
  }

  // LCOV_EXCL_BR_START 11:except,C++ STL
  result->id = item_it->second.id;
  result->size = item_it->second.size;
  result->opt = item_it->second.opt;
  result->nand = category_it->second.nand;
  result->cache_dram = category_it->second.cache_dram;
  result->backup_dram = category_it->second.backup_dram;
  result->sync = category_it->second.sync;
  result->encrypt = category_it->second.encrypt;
  result->backup_cycle = category_it->second.backup_cycle;
  BkupStrlcpy(result->category_name, item_it->second.category.c_str(), sizeof(result->category_name));
  // LCOV_EXCL_BR_STOP 11:except,C++ STL

  return 0;
}

int BkupParamGetNumid(uint32_t num_id, bkup_query_result_t *result,
                      char *item_name, size_t item_name_size) {
  if (g_backup_params == NULL) {  // LCOV_EXCL_BR_LINE 6:g_backup_params will not be NULL
    // LCOV_EXCL_START 6:g_backup_params will not be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Uninitialize call");
    return -1;
    // LCOV_EXCL_STOP 6:g_backup_params will not be NULL
  }

  // LCOV_EXCL_BR_START 11:except,C++ STL
  IdTable::iterator id_it = g_backup_params->id_table.find(static_cast<int>(num_id));
  // LCOV_EXCL_BR_STOP 11:except,C++ STL
  if (id_it == g_backup_params->id_table.end()) {  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    return -1;
  }
  BkupStrlcpy(item_name, id_it->second.c_str(), item_name_size);  // LCOV_EXCL_BR_LINE 11:except,C++ STL

  return BkupParamGet(id_it->second.c_str(), result);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
}

void *BkupParamSetOpt(const char *item_name, void *opt) {
  if (g_backup_params == NULL) {  // LCOV_EXCL_BR_LINE 6:g_backup_params will not be NULL
    // LCOV_EXCL_START 6:g_backup_params will not be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "Uninitialize call");
    return reinterpret_cast<void *>((-1));
    // LCOV_EXCL_STOP 6:g_backup_params will not be NULL
  }

  ItemTable::iterator item_it = g_backup_params->item_table.find(item_name);  // LCOV_EXCL_BR_LINE 11:except,C++ STL
  if (item_it == g_backup_params->item_table.end()) {  // LCOV_EXCL_BR_LINE 6:double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return reinterpret_cast<void *>((-1));   // LCOV_EXCL_LINE 6:double check
  }

  // LCOV_EXCL_BR_START 11:unexpected branch
  return __sync_val_compare_and_swap(&item_it->second.opt, NULL, opt);  // NOLINT(readability/nolint) Supplied by gcc
  // LCOV_EXCL_BR_STOP 11:unexpected branch
}
