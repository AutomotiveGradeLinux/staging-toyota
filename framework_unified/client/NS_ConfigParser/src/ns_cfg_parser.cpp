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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_ConfigParser
/// \brief    This file contains implementation of CCFGParser class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <ns_cfg_internal.h>
#include <ns_cfg_parser.h>
#include <ns_config_parser_frameworkunifiedlog.h>

#include <fstream>
#include <string>

CCFGParser::CCFGParser()
  : m_pmCFGData(NULL) {
}

// LCOV_EXCL_START 200:CCFGParser is not external API class, this constructed function isn't used by other function
CCFGParser::CCFGParser(const std::string &f_cfilepath)
  : m_pmCFGData(NULL) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  CFGParseFile(f_cfilepath);
}
// LCOV_EXCL_STOP

CCFGParser::~CCFGParser() {
  CFGData_Type::iterator l_itCfgStructure;

  if (NULL != m_pmCFGData) {  // LCOV_EXCL_BR_LINE 5:m_pmCFGData cannot be NULL, because internal logic guarantee
    // release the structure
    for (l_itCfgStructure = m_pmCFGData->begin();
         l_itCfgStructure != m_pmCFGData->end();
         l_itCfgStructure++) {
      CBlock *l_pBlock = (*l_itCfgStructure).second;
      if (NULL != l_pBlock) {  // LCOV_EXCL_BR_LINE 5:l_pBlock cannot be NULL, because map insert data success.
        // clear the vector
        l_pBlock->m_vElementPair.clear();

        delete l_pBlock;  // LCOV_EXCL_BR_LINE 11:except branch
        l_pBlock = NULL;
      }
    }

    delete m_pmCFGData;  // LCOV_EXCL_BR_LINE 11:except branch
    m_pmCFGData = NULL;
  }
}

EFrameworkunifiedStatus CCFGParser::CFGParseFile(const std::string &f_cfilepath) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  size_t l_uiPos = 0;
  std::string l_cLine("");
  std::string l_cTag("");  // LCOV_EXCL_BR_LINE 11:except branch
  std::string l_cComment("");  // LCOV_EXCL_BR_LINE 11:except branch

  m_pmCFGData = new(std::nothrow) CFGData_Type();  // LCOV_EXCL_BR_LINE 11:except branch

  if (NULL != m_pmCFGData) {  // LCOV_EXCL_BR_LINE 5:new's error case
    // open the file
    std::ifstream l_objFile(f_cfilepath.c_str());  // LCOV_EXCL_BR_LINE 11:except branch

    // check if file is opened
    if (l_objFile.is_open()) {
      CBlock *l_pBlock = NULL;

      // read until end of file is not reached or any other stream error occurred
      while (!l_objFile.eof()) {
        // read the line from the file
        getline(l_objFile, l_cLine);  // LCOV_EXCL_BR_LINE 11:except branch

        // if line is a comment
        if ('#' == l_cLine[0] || ';' == l_cLine[0]) {
          if (!l_cComment.empty()) {
            l_cComment.append("\n");  // LCOV_EXCL_BR_LINE 11:except branch
          }
          l_cComment.append(l_cLine);
        } else if ('[' == l_cLine[0]) {  // if line is main tag
          // creates new CBlock object,
          // each block contains information of single like all its key-value pair and
          // all the comments associated with it.
          l_pBlock = new CBlock();  // LCOV_EXCL_BR_LINE 11:except branch
          if (NULL != l_pBlock) {  // LCOV_EXCL_BR_LINE 5:new's error case
            // set the tag comment
            l_pBlock->m_cComment = l_cComment;
            l_cComment.clear();

            if (std::string::npos != (l_uiPos = l_cLine.find(']'))) {
              // set the tag
              l_cTag = l_cLine.substr(1, l_uiPos - 1);  // LCOV_EXCL_BR_LINE 11:except branch

              // insert the block with its tag name in structure(map)
              m_pmCFGData->insert(make_pair(l_cTag, l_pBlock));  // NOLINT (readability/nolint)
            } else {
              delete l_pBlock;  // LCOV_EXCL_BR_LINE 11:except branch
              l_pBlock = NULL;

              // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
              FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "End Tag missing");
              // LCOV_EXCL_BR_STOP
              l_eStatus = eFrameworkunifiedStatusErrOther;
              break;
            }
          } else {
            // LCOV_EXCL_START 5:new's error case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error allocating memory for l_pBlock");
            l_eStatus = eFrameworkunifiedStatusNullPointer;
            break;
            // LCOV_EXCL_STOP
          }
        } else if (std::string::npos != (l_uiPos = l_cLine.find('='))) {  // LCOV_EXCL_BR_LINE 11:except branch
          if (NULL != l_pBlock) {  // LCOV_EXCL_BR_LINE 5:new's error case
            // create new node object,
            // each node object contains the information of singlr key-value pair and
            /// it's comment(if any)
            // LCOV_EXCL_BR_START 11:except branch
            CNode l_objNode;
            l_objNode.m_cComment.append(l_cComment);
            l_objNode.m_cTag = l_cLine.substr(0, l_uiPos);
            l_objNode.m_cValue = l_cLine.substr(l_uiPos + 1);

            // insert the node object in block
            l_pBlock->m_vElementPair.push_back(l_objNode);
            // LCOV_EXCL_BR_STOP

            l_cComment.clear();
          } else {
            // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "l_pBlock is NULL");
            // LCOV_EXCL_BR_STOP
            l_eStatus = eFrameworkunifiedStatusNullPointer;
            break;
          }
        }
      }

      // last comment
      if (!l_cComment.empty()) {
        m_cLastComment = l_cComment;
      } else {
        m_cLastComment.clear();
      }

      // close the file
      l_objFile.close();  // LCOV_EXCL_BR_LINE 11:except branch
    } else {
      l_eStatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error Opening File");
      // LCOV_EXCL_BR_STOP
    }
  } else {
    // LCOV_EXCL_START 5:new's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "m_pmCFGData is NULL");
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    // LCOV_EXCL_STOP
  }

  return l_eStatus;
}

std::string CCFGParser::CFGGetData(const std::string &f_ckey) {
  CFGData_Type::iterator l_itCfgStructure;
  std::string l_cHeadTag = "";
  std::string l_cTag = "";  // LCOV_EXCL_BR_LINE 11:except branch
  std::string l_cValue = "";  // LCOV_EXCL_BR_LINE 11:except branch

  size_t l_uiPos = 0;

  if (NULL != m_pmCFGData && !f_ckey.empty()) {  // LCOV_EXCL_BR_LINE 5:new's error case(in function CFGParseFile)
    if (std::string::npos != (l_uiPos = f_ckey.find('.'))) {  // LCOV_EXCL_BR_LINE 11:except branch
      l_cHeadTag = f_ckey.substr(0, l_uiPos);  // LCOV_EXCL_BR_LINE 11:except branch
      l_cTag = f_ckey.substr(l_uiPos + 1);  // LCOV_EXCL_BR_LINE 11:except branch
    }

    // search for the key and its associated value in internal structure
    for (l_itCfgStructure = m_pmCFGData->begin();
         l_itCfgStructure != m_pmCFGData->end();
         l_itCfgStructure++) {
      CBlock *l_pBlock = (*l_itCfgStructure).second;

      if (!l_cHeadTag.compare((*l_itCfgStructure).first) && NULL != l_pBlock) {
        for (UI_32 l_uiCount = 0; l_uiCount < l_pBlock->m_vElementPair.size(); l_uiCount++) {
          if (!l_cTag.compare(l_pBlock->m_vElementPair[l_uiCount].m_cTag)) {
            // LCOV_EXCL_BR_START 200:After l_cValue assignment is empty,there is no other place to assignment the value
            if (!l_cValue.empty()) {
            // LCOV_EXCL_BR_STOP
              // LCOV_EXCL_START 200:After l_cValue assignment is empty,there is no other place to assignment the value
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              l_cValue.append("\n");
              // LCOV_EXCL_STOP 200
            }
            l_cValue.append(l_pBlock->m_vElementPair[l_uiCount].m_cValue);
          }
        }
      }
    }
  }

  return l_cValue;
}

EFrameworkunifiedStatus CCFGParser::CFGGetData(const std::string &f_ckey, std::string &f_cvalue) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  CFGData_Type::iterator l_itCfgStructure;
  std::string l_cHeadTag = "";
  std::string l_cTag = "";  // LCOV_EXCL_BR_LINE 11:except branch
  std::string l_cValue = "";  // LCOV_EXCL_BR_LINE 11:except branch

  size_t l_uiPos = 0;

  if (NULL != m_pmCFGData && !f_ckey.empty()) {  // LCOV_EXCL_BR_LINE 5:new's error case(in function CFGParseFile)
    if (std::string::npos != (l_uiPos = f_ckey.find('.'))) {  // LCOV_EXCL_BR_LINE 11:except branch
      l_cHeadTag = f_ckey.substr(0, l_uiPos);  // LCOV_EXCL_BR_LINE 11:except branch
      l_cTag = f_ckey.substr(l_uiPos + 1);  // LCOV_EXCL_BR_LINE 11:except branch
    }

    // search for the key and its associated value in internal structure
    for (l_itCfgStructure = m_pmCFGData->begin();
         l_itCfgStructure != m_pmCFGData->end();
         l_itCfgStructure++) {
      CBlock *l_pBlock = (*l_itCfgStructure).second;

      if (!l_cHeadTag.compare((*l_itCfgStructure).first) && NULL != l_pBlock) {
        for (UI_32 l_uiCount = 0; l_uiCount < l_pBlock->m_vElementPair.size(); l_uiCount++) {
          if (!l_cTag.compare(l_pBlock->m_vElementPair[l_uiCount].m_cTag)) {
            if (!f_cvalue.empty()) {
              f_cvalue.append("\n");
            }
            f_cvalue.append(l_pBlock->m_vElementPair[l_uiCount].m_cValue);

            l_eStatus = eFrameworkunifiedStatusOK;
          }
        }
      }
    }
  }

  return l_eStatus;
}

EFrameworkunifiedStatus CCFGParser::CFGSetData(const std::string &f_cKey, std::string f_cvalue) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;

  CFGData_Type::iterator l_itCfgStructure;
  std::string l_cHeadTag = "";
  std::string l_cTag = "";  // LCOV_EXCL_BR_LINE 11:except branch
  std::string l_cValue = "";  // LCOV_EXCL_BR_LINE 11:except branch
  size_t l_uiPos = 0;

  // LCOV_EXCL_BR_START 6:f_cKey is not NULL, because checked in CCFGWriter::SetValue
  if (NULL != m_pmCFGData && !f_cKey.empty()) {
  // LCOV_EXCL_BR_STOP
    if (std::string::npos != (l_uiPos = f_cKey.find('.'))) {  // LCOV_EXCL_BR_LINE 11:except branch
      l_cHeadTag = f_cKey.substr(0, l_uiPos);  // LCOV_EXCL_BR_LINE 11:except branch
      l_cTag = f_cKey.substr(l_uiPos + 1);  // LCOV_EXCL_BR_LINE 11:except branch
    }

    // search for the key in internal structure and set the data for it
    for (l_itCfgStructure = m_pmCFGData->begin();
         l_itCfgStructure != m_pmCFGData->end();
         l_itCfgStructure++) {
      CBlock *l_pBlock = (*l_itCfgStructure).second;

      if (!l_cHeadTag.compare((*l_itCfgStructure).first) && NULL != l_pBlock) {
        for (UI_32 l_uiCount = 0; l_uiCount < l_pBlock->m_vElementPair.size(); l_uiCount++) {
          if (!l_cTag.compare(l_pBlock->m_vElementPair[l_uiCount].m_cTag)) {
            l_pBlock->m_vElementPair[l_uiCount].m_cValue.assign(f_cvalue);
            l_eStatus = eFrameworkunifiedStatusOK;
          }
        }
      }
    }
  } else {
    // LCOV_EXCL_START 6:f_cKey is not NULL, because checked in CCFGWriter::SetValue
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "m_pmCFGData is NULL");
    //  LCOV_EXCL_STOP
  }

  return l_eStatus;
}

EFrameworkunifiedStatus CCFGParser::CFGSaveData(const std::string &f_cfilepath) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  CFGData_Type::iterator l_itCfgStructure;

  if (NULL != m_pmCFGData) {  // LCOV_EXCL_BR_LINE 5:m_pmCFGData cannot be NULL, because internal logic guarantee
    std::ofstream l_objFile(f_cfilepath.c_str());

    // check if file is open
    if (l_objFile.is_open()) {
      // write the updated CFG structure back to the file
      for (l_itCfgStructure = m_pmCFGData->begin();
           l_itCfgStructure != m_pmCFGData->end();
           l_itCfgStructure++) {
        CBlock *l_pBlock = (*l_itCfgStructure).second;
        if (NULL != l_pBlock) {  // LCOV_EXCL_BR_LINE 11:except branch
          // write the main tag comment in file
          if (!l_pBlock->m_cComment.empty()) {
            l_objFile << l_pBlock->m_cComment << std::endl;
          }

          // write main tag to file
          l_objFile << "[" << (*l_itCfgStructure).first << "]" << std::endl;

          for (UI_32 l_uiCount = 0; l_uiCount < l_pBlock->m_vElementPair.size(); l_uiCount++) {
            // comment over the key-value pair
            if (!l_pBlock->m_vElementPair[l_uiCount].m_cComment.empty()) {
              l_objFile << l_pBlock->m_vElementPair[l_uiCount].m_cComment << std::endl;;
            }

            // key-value
            l_objFile << l_pBlock->m_vElementPair[l_uiCount].m_cTag << "="
                      << l_pBlock->m_vElementPair[l_uiCount].m_cValue << std::endl;
          }
        }
      }

      // last comment
      if (!m_cLastComment.empty()) {
        l_objFile << m_cLastComment << std::endl;
      }

      // close the file
      l_objFile.close();  // LCOV_EXCL_BR_LINE 11:except branch
    } else {
      l_eStatus = eFrameworkunifiedStatusFail;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Cannot Open File");
    }
  } else {
    // LCOV_EXCL_START 5:m_pmCFGData cannot be NULL, because internal logic guarantee
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "m_pmCFGData is NULL");
    // LCOV_EXCL_STOP
  }

  return l_eStatus;
}
