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
#include <pthread.h>

#include <native_service/ns_logger_if.h>
#include <native_service/ns_resource_controler.h>

#include <map>
#include <string>
#include <utility>

struct frameworkunifiedResource {
  int ref_counter;
  long resource;  // NOLINT  (readability/nolint)
};

typedef std::map<std::string, frameworkunifiedResource> frameworkunifiedResourceTable_t;
typedef std::map<long, std::string> frameworkunifiedResourceTableR_t;  // NOLINT  (readability/nolint)

struct frameworkunifiedResourceTable {
  frameworkunifiedResourceTable_t resTable;
  frameworkunifiedResourceTableR_t resTableR;
};

typedef std::map<std::string, frameworkunifiedResourceTable> frameworkunifiedResourceModuleTable_t;

static __thread frameworkunifiedResourceModuleTable_t *frameworkunifiedResourceModuleTable;

static frameworkunifiedResource *frameworkunifiedSearchResourse(const char *mod, const char *key) {
  if (frameworkunifiedResourceModuleTable == NULL) {
    frameworkunifiedResourceModuleTable = new frameworkunifiedResourceModuleTable_t;
    /*
     * @todo
     * There is no release processing for "new frameworkunifiedResourceModuleTable_t" created under McOpenMonitor() (Valgrind pointed).
     */
  }
  frameworkunifiedResourceModuleTable_t::iterator mod_it = frameworkunifiedResourceModuleTable->find(mod);
  if (mod_it == frameworkunifiedResourceModuleTable->end()) {
    return NULL;
  }

  frameworkunifiedResourceTable_t::iterator res_it = mod_it->second.resTable.find(key);
  if (res_it == mod_it->second.resTable.end()) {
    return NULL;
  }

  return &res_it->second;
}

int frameworkunifiedGetResource(const char *mod, const char *key, long *resource) {  // NOLINT  (readability/nolint)
  frameworkunifiedResource *r = frameworkunifiedSearchResourse(mod, key);
  if (r == NULL) {
    return -1;
  }
  *resource = r->resource;
  return 0;
}

int frameworkunifiedAcquireResouce(const char *mod, const char *key, long *resource) {  // NOLINT  (readability/nolint)
  frameworkunifiedResource *r = frameworkunifiedSearchResourse(mod, key);
  if (r == NULL) {
    return -1;
  }
  *resource = r->resource;
  return ++(r->ref_counter);
}

int frameworkunifiedReleaseResouce(const char *mod, const char *key) {
  frameworkunifiedResource *r = frameworkunifiedSearchResourse(mod, key);
  if (r == NULL) {
    return -1;
  }
  return --(r->ref_counter);
}

int frameworkunifiedSearchResourseKey(const char *mod, long resource, const char **key) {  // NOLINT  (readability/nolint)
  if (frameworkunifiedResourceModuleTable == NULL) {
    frameworkunifiedResourceModuleTable = new frameworkunifiedResourceModuleTable_t;
  }
  frameworkunifiedResourceModuleTable_t::iterator mod_it = frameworkunifiedResourceModuleTable->find(mod);
  if (mod_it == frameworkunifiedResourceModuleTable->end()) {
    return -1;
  }

  frameworkunifiedResourceTableR_t::iterator resR_it = mod_it->second.resTableR.find(resource);
  if (resR_it == mod_it->second.resTableR.end()) {
    return -1;
  }

  *key = (resR_it->second).c_str();
  return 0;
}

int frameworkunifiedRegistResouce(const char *mod, const char *key, long resource,  // NOLINT  (readability/nolint)
                    int init_counter) {
  int ret = 0;

  if (frameworkunifiedResourceModuleTable == NULL) {
    frameworkunifiedResourceModuleTable = new frameworkunifiedResourceModuleTable_t;
  }
  frameworkunifiedResourceModuleTable_t::iterator mod_it = frameworkunifiedResourceModuleTable->find(mod);

  if (mod_it == frameworkunifiedResourceModuleTable->end()) {
    frameworkunifiedResourceTable mod_res;
    frameworkunifiedResourceModuleTable->insert(std::make_pair(mod, mod_res));  // LCOV_EXCL_BR_LINE 11:unexpected branch
    mod_it = frameworkunifiedResourceModuleTable->find(mod);  // LCOV_EXCL_BR_LINE 11:unexpected branch
  }

  if (mod_it->second.resTable.find(key) == mod_it->second.resTable.end()) {
    frameworkunifiedResource regist_res;
    regist_res.ref_counter = init_counter;
    regist_res.resource = resource;
    mod_it->second.resTable.insert(std::make_pair(key, regist_res));
    mod_it->second.resTableR.insert(std::make_pair(resource, key));
  } else {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __func__, "Key(%s) already regist", key != 0 ? key : NULL);
    // LCOV_EXCL_BR_STOP
    ret = -1;
  }

  return ret;
}

int frameworkunifiedUnregistResouce(const char *mod, const char *key) {
  if (frameworkunifiedResourceModuleTable == NULL) {
    frameworkunifiedResourceModuleTable = new frameworkunifiedResourceModuleTable_t;
  }
  frameworkunifiedResourceModuleTable_t::iterator mod_it = frameworkunifiedResourceModuleTable->find(mod);

  if (mod_it == frameworkunifiedResourceModuleTable->end()) {
    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __func__, "Mod(%s) undefined", mod != 0 ? mod : NULL);
    // LCOV_EXCL_BR_STOP
  } else {
    frameworkunifiedResourceTable_t::iterator res_it = mod_it->second.resTable.find(key);
    if (res_it != mod_it->second.resTable.end()) {
      long resource = res_it->second.resource;  // NOLINT  (readability/nolint)
      mod_it->second.resTable.erase(key);

      if (mod_it->second.resTableR.find(resource) != mod_it->second.resTableR.end()) {
        mod_it->second.resTableR.erase(resource);
      }
    } else {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __func__, "Key(%s) no regist", key != 0 ? key : NULL);
      // LCOV_EXCL_BR_STOP
    }
  }
  return 0;
}
