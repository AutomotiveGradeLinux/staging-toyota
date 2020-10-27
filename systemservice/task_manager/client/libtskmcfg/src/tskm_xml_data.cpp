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

#include "system_service/tskm_xml_data.h"

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <list>

#include "system_service/tskm_svc.h"

#include "system_service/tskm_svcid.h"
#include "tskm_debug.h"
#include "tskm_comm.h"
#include "tskm_auto_build.h"  // Generated data from XML

// This size depends on the size of the TM area of CL_Monitor
#define TSKM_SVC_ID_MAX_SIZE 1024

/***********************************************************************
 *  tskm_initServiceList
 ***********************************************************************/
int tskm_initServiceList(TSKM_SVCS_CTX_t* p_svcs, int iFd) {
  uint32_t ii;

  p_svcs->svcNum = sizeof(serviceList) / sizeof(TSKM_SVC_CTX_t);
  p_svcs->svcList = serviceList;

  // It is initialized with the Inotify floppy disk.
  for (ii = 0; ii < p_svcs->svcNum; ii++) {
    serviceList[ii].attr = &serviceAttr[ii];
    serviceList[ii].iFd = iFd;
    if (serviceList[ii].attr->svcId >= TSKM_SVC_ID_MAX_SIZE) {
      TSKM_ASSERT(0);
      return -1;
    }
  }

  const char* nfsenv = getenv("AGL_NFS");
  bool isNfs = (nfsenv && strcmp(nfsenv, "y") == 0) ? true : false;

  // For NFS environments, replace the PATH with CAP with /tmp
  if (isNfs) {
    std::list<std::string> capFiles;
    const std::string capPath("/usr/debug/share/target/cap.lst");
    std::ifstream fin(capPath.c_str());
    std::string line;
    while (fin && std::getline(fin, line)) {
      std::list<std::string> strList;
      try {  // throw exception by boost::split
        boost::split(strList, line, boost::is_any_of("|"));
        if (!strList.empty()) {
          if (strList.front()[0] == '/') {  // Only character strings beginning with '/' are considered PATH
            capFiles.push_back(strList.front().c_str());
          }
        }
      } catch (...) {
        TSKM_ASSERT(0);
      }
    }

    for (ii = 0; ii < p_svcs->svcNum; ii++) {
      std::string binPath(serviceList[ii].attr->path);

      for (std::list<std::string>::iterator ite = capFiles.begin();
           ite != capFiles.end(); ite++) {
        if (binPath == *ite) {
          std::list<std::string> nodes;
          try {  // throw exception by boost::split
            boost::split(nodes, binPath, boost::is_any_of("/"));
            std::string *p_newPath = new std::string("/tmp/");  // Intent not to free up memory
            *p_newPath += nodes.back();
            TSKM_PRINTF(TSKM_LOG_STATE, "EXCHG %s", p_newPath->c_str());
            serviceList[ii].attr->path = p_newPath->c_str();
            break;
          } catch (...) {
            TSKM_ASSERT(0);
          }
        }
      }
    }
  }

  // If there is no socket resource equal to the number of services + 1 (internal control connection), a compilation error occurs
  TSKM_STATIC_ASSERT(TSKM_COMM_CONNECT_MAX >= (TSKM_SVC_NUM+1));

  return 0;
}

/***********************************************************************
 *  tskm_initWakeupCtx
 ***********************************************************************/
void tskm_initWakeupCtx(TSKM_GSTEP_CTX_t* p_wakeup, BOOL isVupMode) {
  memset(p_wakeup, 0, sizeof(*p_wakeup));

  if (isVupMode) {
    p_wakeup->gstepNum = sizeof(wakeupGstepVup) / sizeof(TSKM_GSTEP_t);
    p_wakeup->gstep = wakeupGstepVup;
  } else {
    p_wakeup->gstepNum = sizeof(wakeupGstep) / sizeof(TSKM_GSTEP_t);
    p_wakeup->gstep = wakeupGstep;
  }

  TSKM_PRINTF(TSKM_LOG_STATE, "gstep(wakeup):%d", p_wakeup->gstepNum);
}

/***********************************************************************
 *  tskm_initDownCtx
 ***********************************************************************/
void tskm_initDownCtx(TSKM_GSTEP_CTX_t* p_down, BOOL isVupMode) {
  memset(p_down, 0, sizeof(*p_down));

  if (isVupMode) {
    p_down->gstepNum = sizeof(downGstepVup) / sizeof(TSKM_GSTEP_t);
    p_down->gstep = downGstepVup;
  } else {
    p_down->gstepNum = sizeof(downGstep) / sizeof(TSKM_GSTEP_t);
    p_down->gstep = downGstep;
  }

  TSKM_PRINTF(TSKM_LOG_STATE, "gstep(down):%d", p_down->gstepNum);
}

