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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <agl_wakeup_order.h>
#include <list>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include "system_service/ss_last_to_order.h"
#include "ss_last_to_order_local.h"

#include "system_service/ss_templates.h"
#include "ss_system_if_interfaceunifiedlog.h"
#define SCO_STATE_LOG(...) FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, __VA_ARGS__);
#define SCO_ERROR_LOG(...) FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, __VA_ARGS__);
#define SCO_ASSERT(x)      SS_ASERT(x);

//************* CONFIG FILE Format ****************
#define ORDERNAME_MAX 64
#define CONTENT_NAME_MAX 128
#pragma pack(push, 1)
typedef struct {
  char    lastinfo[SS_CCAT_MAX][CONTENT_NAME_MAX];
  char    orderName[ORDERNAME_MAX];
}SS_LASTINFO_TO_OEDER_PACKED_t;
#pragma pack(pop)
//*************************************************


typedef struct {
  SS_LAST_INFO_t lastinfo;
  std::string         orderName;
}SS_LASTINFO_TO_OEDER_t;



//****************************
//  load configuration file
//****************************
static int
loadConfig(std::string &cfgPath, std::list<SS_LASTINFO_TO_OEDER_t> *p_cfgList) throw() {  // NOLINT (runtime/references)
  int ret = 0;

  try {
    std::ifstream fin(cfgPath.c_str(), std::ios::in | std::ios::binary);
    if (!fin) {
      SCO_ERROR_LOG("cfg file error %s", cfgPath.c_str());
      throw std::domain_error("open error");
    }
    char magic[4];

    fin.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (fin.fail()) {
      throw std::domain_error("read magic");
    }

    if (memcmp(magic, "CTOO", sizeof(magic)) != 0) {
      throw std::domain_error("magic error");
    }

    uint32_t numOfElement = 0;
    fin.read(reinterpret_cast<char*>(&numOfElement), sizeof(numOfElement));
    if ( fin.fail() ) {
      throw std::domain_error("read numOfElement");
    }

    for (uint32_t ii = 0; ii < numOfElement; ii++) {
      SS_LASTINFO_TO_OEDER_PACKED_t element;
      fin.read(reinterpret_cast<char*>(&element), sizeof(element));
      if ( fin.fail() ) {
        throw std::domain_error("read element");
      }
      SS_LASTINFO_TO_OEDER_t tmp;
      tmp.lastinfo[SS_CCAT_F_VIDEO] = element.lastinfo[SS_CCAT_F_VIDEO];
      tmp.lastinfo[SS_CCAT_F_SUB_VIDEO] = element.lastinfo[SS_CCAT_F_SUB_VIDEO];
      tmp.lastinfo[SS_CCAT_F_AUDIO] = element.lastinfo[SS_CCAT_F_AUDIO];
      tmp.lastinfo[SS_CCAT_R_VIDEO] = element.lastinfo[SS_CCAT_R_VIDEO];
      tmp.lastinfo[SS_CCAT_R_AUDIO] = element.lastinfo[SS_CCAT_R_AUDIO];
      tmp.orderName = element.orderName;
      p_cfgList->push_back(tmp);
    }
    SCO_STATE_LOG("%s is loaded", cfgPath.c_str());
  } catch (std::exception &e) {
    if (ret == 0) ret = -1;
    SCO_ERROR_LOG("%s", e.what());
    SCO_ASSERT(0);
  } catch (...) {
    SCO_ASSERT(0);
    ret = -1;
  }
  return ret;
}



static
bool
isPassCondition(SS_CONT_CATEGORY_t cat, SS_LAST_INFO_t &lastInfo, SS_LASTINFO_TO_OEDER_t &tblElement) {  // NOLINT (runtime/references)
  std::string &tblStr = tblElement.lastinfo[cat];

  if (tblStr != "EMPTY") {
    //Category with content specification
    SS_LAST_INFO_t::iterator iteCont = lastInfo.find(cat);
    if (iteCont == lastInfo.end() || iteCont->second != tblStr) {
      return false;
    }
  }
  return true;
}



//****************************
//  SS_ConvLastInfoToOrder
//****************************
EFrameworkunifiedStatus
SS_ConvLastInfoToOrder(SS_LAST_INFO_t &curInfo, std::string &order, const char* p_cfgPath/* = NULL*/) {  // NOLINT (runtime/references)
  std::string cfgPath = (p_cfgPath) ? p_cfgPath : SS_LAST_TO_ORDER_CONF_PATH;

  static std::list<SS_LASTINFO_TO_OEDER_t> s_convList;
  static std::string s_readCfgPath = "__default__";
  bool isHit = false;

  // If the same CONFIG, do not READ after the second.
  if (s_readCfgPath != cfgPath) {
    int ret;
    s_convList.clear();

    ret = loadConfig(cfgPath, &s_convList);
    if (ret != 0) { goto ERROR; }
    s_readCfgPath = cfgPath;

#if 0  // DEBUG
    for (std::list<SS_LASTINFO_TO_OEDER_PACKED_t>::iterator tblElt = s_convList.begin();
      tblElt != s_convList.end();
      tblElt++) {
      SCO_STATE_LOG("FV:%s FSV:%s FA:%s RV:%s RA:%s O:%s",
        tblElt->lastinfo[SS_CCAT_F_VIDEO], tblElt->lastinfo[SS_CCAT_F_SUB_VIDEO], tblElt->lastinfo[SS_CCAT_F_AUDIO],
        tblElt->lastinfo[SS_CCAT_R_VIDEO], tblElt->lastinfo[SS_CCAT_R_AUDIO], tblElt->orderName);
    }
#endif
  }

  for (std::list<SS_LASTINFO_TO_OEDER_t>::iterator tblElt = s_convList.begin();
      tblElt != s_convList.end();
      tblElt++) {
    if      (isPassCondition(SS_CCAT_F_VIDEO    , curInfo, *tblElt) == false) {
      continue;
    } else if (isPassCondition(SS_CCAT_F_SUB_VIDEO, curInfo, *tblElt) == false) {
      continue;
    } else if (isPassCondition(SS_CCAT_F_AUDIO    , curInfo, *tblElt) == false) {
      continue;
    } else if (isPassCondition(SS_CCAT_R_VIDEO    , curInfo, *tblElt) == false) {
      continue;
    } else if (isPassCondition(SS_CCAT_R_AUDIO    , curInfo, *tblElt) == false) {
      continue;
    }

    isHit = true;
    order = tblElt->orderName;
    break;
  }
  if (!isHit) {
    order = WON_DEFAULT;
  }
  return eFrameworkunifiedStatusOK;
ERROR:
  return eFrameworkunifiedStatusFail;
}
