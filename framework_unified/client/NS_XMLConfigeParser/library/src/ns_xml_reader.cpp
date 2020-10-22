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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_ConfigParser
/// \brief    This file contains implementation of CXMLReader class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_xml_reader.h>
#include <string>
#include "ns_xmlconfig_parser_frameworkunifiedlog.h"

#ifdef __cplusplus
extern "C" {
#endif

CXMLReader *GetCXMLReaderObject(CHAR *f_cfilepath) {
  CXMLReader *l_p_xml_reader = NULL;
  if (NULL != f_cfilepath) {
    l_p_xml_reader = new(std::nothrow) CXMLReader(f_cfilepath);  // LCOV_EXCL_BR_LINE 11: except branch
  }
  return l_p_xml_reader;
}

#ifdef __cplusplus
}
#endif

CXMLReader::CXMLReader(): m_pXmlDoc(NULL) {
}

CXMLReader::CXMLReader(const std::string &f_cFilePath): m_pXmlDoc(NULL) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "File Path %s", f_cFilePath.c_str());

  // create document object structure
  m_pXmlDoc = xmlParseFile(f_cFilePath.c_str());  // LCOV_EXCL_BR_LINE 11: except branch

  if (!m_pXmlDoc) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Pointer to document structure is NULL");    // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }
}

CXMLReader::~CXMLReader() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Destructor");

  if (NULL != m_pXmlDoc) {
    xmlFreeDoc(m_pXmlDoc);  // LCOV_EXCL_BR_LINE 11: except branch
    m_pXmlDoc = NULL;
  }
}

EFrameworkunifiedStatus CXMLReader::ParseFile(const std::string &f_cFilePath) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // create document object structure
  m_pXmlDoc = xmlParseFile(f_cFilePath.c_str());

  if (NULL == m_pXmlDoc) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Pointer to document structure is NULL");
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

PVOID CXMLReader::GetDataPtr() {
  return m_pXmlDoc;
}

std::string CXMLReader::GetValue(const std::string &f_cKey) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Key %s", f_cKey.c_str());

  std::string l_cValue = "";

  if (NULL != m_pXmlDoc && (!f_cKey.empty())) {
    std::string l_cUserKey = "";  // LCOV_EXCL_BR_LINE 11: except branch
    l_cUserKey.assign(f_cKey);

    // get the root node element
    xmlNodePtr l_pCurrNode = xmlDocGetRootElement(m_pXmlDoc);  // LCOV_EXCL_BR_LINE 11: except branch

    if (NULL != l_pCurrNode) {
      // remove the root node name from key
      size_t l_uiLength = l_cUserKey.find('.');  // LCOV_EXCL_BR_LINE 11: except branch

      if (std::string::npos != l_uiLength) {
        l_cUserKey = l_cUserKey.substr(l_uiLength + 1);  // LCOV_EXCL_BR_LINE 11: except branch

        // if root node name matches with the name in received key
        if (!(f_cKey.substr(0, l_uiLength)).compare((PCSTR)l_pCurrNode->name)) {
          l_cValue = XMLGetValue(l_pCurrNode,
                                 l_cUserKey);
        }
      }
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Parsed Value :: %s", l_cValue.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Document structure pointer m_pXmlDoc is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }


  return l_cValue;
}

std::string CXMLReader::XMLGetValue(xmlNodePtr f_pCurrNode,
                                    const std::string &f_cUserKey) {
  std::string l_cReturnValue = "";

  if (NULL != f_pCurrNode && (!f_cUserKey.empty())) {
    std::string l_cKey = "";   // LCOV_EXCL_BR_LINE 11: except branch

    // result of previous attribute of same node
    BOOL l_bResult = TRUE;

    // this parameter specifies whether to AND or OR the current result with previous result
    // TRUE means AND while FALSE means OR
    BOOL l_bOperation = FALSE;

    // position of "@" in user key if exists
    size_t l_uiAttrLen = 0;

    // get the single attribute from list of attributes of node
    std::string l_cAttr = "";   // LCOV_EXCL_BR_LINE 11: except branch
    std::string l_cNodeName = "";   // LCOV_EXCL_BR_LINE 11: except branch

    // contains the single attribute key, value
    std::string l_cAttrOpt = "";   // LCOV_EXCL_BR_LINE 11: except branch

    // l_cAttrId of the attribute
    std::string l_cAttrId = "";   // LCOV_EXCL_BR_LINE 11: except branch
    // value of the attribute
    std::string l_cAttrValue = "";   // LCOV_EXCL_BR_LINE 11: except branch

    size_t l_uiSplitProp = 0;
    size_t l_uiAttrSplit = 0;
    BOOL l_bCurrentResult = FALSE;

    l_cKey.assign(f_cUserKey);

    // position of first "." in user key
    size_t l_uiLength = l_cKey.find('.');   // LCOV_EXCL_BR_LINE 11: except branch
    if (std::string::npos != l_uiLength) {
      // get the current node name string with node attributes
      l_cKey = l_cKey.substr(0, l_uiLength);  // LCOV_EXCL_BR_LINE 11: except branch
    }

    l_cNodeName.assign(l_cKey);

    // get children of current node
    f_pCurrNode = f_pCurrNode->children;

    while (NULL != f_pCurrNode) {
      // check whether current node is an element node.
      if (XML_ELEMENT_NODE == f_pCurrNode->type) {
        l_bResult = TRUE;
        l_bOperation = FALSE;
        l_uiAttrLen = 0;
        l_cAttr.clear();

        // check whether key contains attribute for current node
        if (std::string::npos != (l_uiAttrLen = l_cKey.find('@'))) {
          // get the attribute string from node key
          l_cAttr.assign(l_cKey, (l_uiAttrLen + 1), std::string::npos);  // LCOV_EXCL_BR_LINE 11: except branch

          // remove the attribute string from key string
          l_cKey = l_cKey.substr(0, l_uiAttrLen);  // LCOV_EXCL_BR_LINE 11: except branch
        }

        // check whether node name string matches with the current node name
        if (!l_cKey.compare((PCSTR)f_pCurrNode->name)) {
          l_cAttrOpt.assign(l_cAttr);

          while (0 < l_cAttr.length()) {
            // initialize variables
            l_cAttrId.clear();
            l_cAttrValue.clear();
            l_uiSplitProp = 0;
            l_uiAttrSplit = 0;
            l_bCurrentResult = FALSE;

            // check whether node have multiple attributes
            if (std::string::npos != (l_uiSplitProp = l_cAttr.find('|'))) {
              l_cAttrOpt = l_cAttr.substr(0, l_uiSplitProp);

              if (std::string::npos != (l_uiAttrSplit = l_cAttrOpt.find('='))) {
                l_cAttrId = l_cAttrOpt.substr(0, l_uiAttrSplit);
                l_cAttrValue = l_cAttrOpt.substr(l_uiAttrSplit + 1);
              }
            } else if (std::string::npos != (l_uiSplitProp = l_cAttr.find('&'))) {
              l_cAttrOpt = l_cAttr.substr(0, l_uiSplitProp);

              if (std::string::npos != (l_uiAttrSplit = l_cAttrOpt.find('='))) {
                l_cAttrId = l_cAttrOpt.substr(0, l_uiAttrSplit);
                l_cAttrValue = l_cAttrOpt.substr(l_uiAttrSplit + 1);
              }
            } else {
              l_uiSplitProp = l_cAttr.length() - 1;
              if (std::string::npos != (l_uiAttrSplit = l_cAttr.find('='))) {
                l_cAttrId = l_cAttr.substr(0, l_uiAttrSplit);  // LCOV_EXCL_BR_LINE 11: except branch
                l_cAttrValue = l_cAttr.substr(l_uiAttrSplit + 1);  // LCOV_EXCL_BR_LINE 11: except branch
              }
            }

            // compare the value of attributes
            xmlChar *l_pAttrValue = xmlGetProp(f_pCurrNode, (const xmlChar *)l_cAttrId.c_str());  // LCOV_EXCL_BR_LINE 11: except branch
            if (NULL != l_pAttrValue) {
              if (!l_cAttrValue.compare((PCSTR)l_pAttrValue)) {
                l_bCurrentResult = TRUE;
              }
            }
            xmlFree(l_pAttrValue);  // LCOV_EXCL_BR_LINE 11: except branch
            l_pAttrValue = NULL;

            // combine the result of all attributes
            if (!l_bOperation) {
              l_bResult = l_bResult && l_bCurrentResult;
            } else {
              l_bResult = l_bResult || l_bCurrentResult;
            }

            if ('|' == l_cAttr[l_uiSplitProp]) {
              l_bOperation = TRUE;
            } else {
              l_bOperation = FALSE;
            }

            if ((!l_bResult) && (!l_bOperation)) {
              // break from current while loop
              break;
            }

            l_cAttr = l_cAttr.substr(l_uiSplitProp + 1);  // LCOV_EXCL_BR_LINE 11: except branch
          }

          // if attributes not matched move to next node
          if (!l_bResult) {
            // search for the same node name in next child nodes
            l_cKey.assign(l_cNodeName);
          } else {
            // read text of current node
            if ((NULL != f_pCurrNode->children) &&
                (XML_TEXT_NODE == f_pCurrNode->children->type) &&
                (NULL == f_pCurrNode->children->next)) {
              // append the result in string
              l_cReturnValue.append((PCSTR) f_pCurrNode->children->content);  // LCOV_EXCL_BR_LINE 11: except branch
            }
          }
        } else {
          l_bResult = FALSE;
        }

        if (l_bResult) {
          // parse children and next nodes of current node
          if (std::string::npos != l_uiLength) {
            l_cKey = f_cUserKey.substr(l_uiLength + 1);  // LCOV_EXCL_BR_LINE 11: except branch

            std::string l_cValue = XMLGetValue(f_pCurrNode, l_cKey);  // LCOV_EXCL_BR_LINE 11: except branch

            if (!l_cReturnValue.empty() && !l_cValue.empty()) {
              l_cReturnValue.append("\n");
            }

            // append the received string
            l_cReturnValue.append(l_cValue);
          }

          l_cKey = l_cNodeName;
        }
      }

      f_pCurrNode = f_pCurrNode->next;
    }
  }

  return l_cReturnValue;
}

EFrameworkunifiedStatus CXMLReader::GetValue(const std::string &f_cKey, std::string &f_cValue) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != m_pXmlDoc && (!f_cKey.empty())) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Key is %s", f_cKey.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"

    std::string l_cUserKey = "";
    l_cUserKey.assign(f_cKey);

    // get the root node element
    xmlNodePtr l_pCurrNode = xmlDocGetRootElement(m_pXmlDoc);

    if (NULL != l_pCurrNode) {
      // remove the root node name from key
      size_t l_uiLength = l_cUserKey.find('.');  // LCOV_EXCL_BR_LINE 11: except branch

      if (std::string::npos != l_uiLength) {
        l_cUserKey = l_cUserKey.substr(l_uiLength + 1);  // LCOV_EXCL_BR_LINE 11: except branch

        // if root node name matches with the name in received key
        if (!(f_cKey.substr(0, l_uiLength)).compare((PCSTR)l_pCurrNode->name)) {
          BOOL l_bKeyFound = FALSE;

          XMLGetValue(l_pCurrNode, l_cUserKey, f_cValue, l_bKeyFound);

          if (!l_bKeyFound) {
            l_eStatus = eFrameworkunifiedStatusFail;
          }
        }
      } else {
        l_eStatus = eFrameworkunifiedStatusInvldParam;
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Root Node ptr is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
      l_eStatus = eFrameworkunifiedStatusNullPointer;
    }

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Parsed Value :: %s", f_cValue.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Document structure pointer m_pXmlDoc is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CXMLReader::XMLGetValue(xmlNodePtr f_pCurrNode, const std::string &f_cUserKey, std::string &f_cValue,
                                   BOOL &f_bKeyFound) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != f_pCurrNode && (!f_cUserKey.empty())) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Key :: %s", f_cUserKey.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"

    std::string l_cKey = "";

    // result of previous attribute of same node
    BOOL l_bResult = TRUE;

    // this parameter specifies whether to AND or OR the current result with previous result
    // TRUE means AND while FALSE means OR
    BOOL l_bOperation = FALSE;

    // position of "@" in user key if exists
    size_t l_uiAttrLen = 0;

    // get the single attribute from list of attributes of node
    std::string l_cAttr = "";  // LCOV_EXCL_BR_LINE 11: except branch
    std::string l_cNodeName = "";  // LCOV_EXCL_BR_LINE 11: except branch

    // contains the single attribute key, value
    std::string l_cAttrOpt = "";  // LCOV_EXCL_BR_LINE 11: except branch

    // l_cAttrId of the attribute
    std::string l_cAttrId = "";  // LCOV_EXCL_BR_LINE 11: except branch
    // value of the attribute
    std::string l_cAttrValue = "";  // LCOV_EXCL_BR_LINE 11: except branch

    size_t l_uiSplitProp = 0;
    size_t l_uiAttrSplit = 0;
    BOOL l_bCurrentResult = FALSE;

    l_cKey.assign(f_cUserKey);

    // position of first "." in user key
    size_t l_uiLength = l_cKey.find('.');  // LCOV_EXCL_BR_LINE 11: except branch
    if (std::string::npos != l_uiLength) {
      // get the current node name string with node attributes
      l_cKey = l_cKey.substr(0, l_uiLength);  // LCOV_EXCL_BR_LINE 11: except branch
    }
    l_cNodeName.assign(l_cKey);

    // get children of current node
    f_pCurrNode = f_pCurrNode->children;

    while (NULL != f_pCurrNode) {
      // check whether current node is an element node.
      if (XML_ELEMENT_NODE == f_pCurrNode->type) {
        l_bResult = TRUE;
        l_bOperation = FALSE;
        l_uiAttrLen = 0;
        l_cAttr.clear();

        // check whether key contains attribute for current node
        if (std::string::npos != (l_uiAttrLen = l_cKey.find('@'))) {
          // get the attribute string from node key
          l_cAttr.assign(l_cKey, (l_uiAttrLen + 1), std::string::npos);  // LCOV_EXCL_BR_LINE 11: except branch

          // remove the attribute string from key string
          l_cKey = l_cKey.substr(0, l_uiAttrLen);  // LCOV_EXCL_BR_LINE 11: except branch
        }

        // check whether node name string matches with the current node name
        if (!l_cKey.compare((PCSTR)f_pCurrNode->name)) {
          l_cAttrOpt.assign(l_cAttr);

          while (0 < l_cAttr.length()) {
            // initialize variables
            l_cAttrId.clear();
            l_cAttrValue.clear();
            l_uiSplitProp = 0;
            l_uiAttrSplit = 0;
            l_bCurrentResult = FALSE;

            // check whether node have multiple attributes
            if (std::string::npos != (l_uiSplitProp = l_cAttr.find('|'))) {
              l_cAttrOpt = l_cAttr.substr(0, l_uiSplitProp);

              if (std::string::npos != (l_uiAttrSplit = l_cAttrOpt.find('='))) {
                l_cAttrId = l_cAttrOpt.substr(0, l_uiAttrSplit);
                l_cAttrValue = l_cAttrOpt.substr(l_uiAttrSplit + 1);
              }
            } else if (std::string::npos != (l_uiSplitProp = l_cAttr.find('&'))) {
              l_cAttrOpt = l_cAttr.substr(0, l_uiSplitProp);

              if (std::string::npos != (l_uiAttrSplit = l_cAttrOpt.find('='))) {
                l_cAttrId = l_cAttrOpt.substr(0, l_uiAttrSplit);
                l_cAttrValue = l_cAttrOpt.substr(l_uiAttrSplit + 1);
              }
            } else {
              l_uiSplitProp = l_cAttr.length() - 1;
              if (std::string::npos != (l_uiAttrSplit = l_cAttr.find('='))) {
                l_cAttrId = l_cAttr.substr(0, l_uiAttrSplit);  // LCOV_EXCL_BR_LINE 11: except branch
                l_cAttrValue = l_cAttr.substr(l_uiAttrSplit + 1);  // LCOV_EXCL_BR_LINE 11: except branch
              }
            }

            // compare the value of attributes
            xmlChar *l_pAttrValue = xmlGetProp(f_pCurrNode, (const xmlChar *)l_cAttrId.c_str());  // LCOV_EXCL_BR_LINE 11: except branch
            if (NULL != l_pAttrValue) {
              if (!l_cAttrValue.compare((PCSTR)l_pAttrValue)) {
                l_bCurrentResult = TRUE;
              }

              xmlFree(l_pAttrValue);   // LCOV_EXCL_BR_LINE 11: except branch
              l_pAttrValue = NULL;
            }

            // combine the result of all attributes
            if (!l_bOperation) {
              l_bResult = l_bResult && l_bCurrentResult;
            } else {
              l_bResult = l_bResult || l_bCurrentResult;
            }

            if ('|' == l_cAttr[l_uiSplitProp]) {
              l_bOperation = TRUE;
            } else {
              l_bOperation = FALSE;
            }

            if ((!l_bResult) && (!l_bOperation)) {
              // break from current while loop
              break;
            }

            l_cAttr = l_cAttr.substr(l_uiSplitProp + 1);  // LCOV_EXCL_BR_LINE 11: except branch
          }

          // if attributes not matched move to next node
          if (!l_bResult) {
            // search for the same node name in next child nodes
            l_cKey.assign(l_cNodeName);
          } else {
            // read text of current node
            if ((NULL != f_pCurrNode->children) &&
                (XML_TEXT_NODE == f_pCurrNode->children->type) &&
                (NULL == f_pCurrNode->children->next)) {
              if (!f_cValue.empty()) {
                f_cValue.append("\n");
              }

              // append the result in string
              f_cValue.append((PCSTR) f_pCurrNode->children->content);  // LCOV_EXCL_BR_LINE 11: except branch

              f_bKeyFound = TRUE;
            }
          }
        } else {
          l_bResult = FALSE;
        }

        if (l_bResult) {
          // parse children and next nodes of current node
          if (std::string::npos != l_uiLength) {
            l_cKey = f_cUserKey.substr(l_uiLength + 1);  // LCOV_EXCL_BR_LINE 11: except branch

            l_eStatus = XMLGetValue(f_pCurrNode, l_cKey, f_cValue, f_bKeyFound);
          }

          l_cKey = l_cNodeName;
        }
      }

      f_pCurrNode = f_pCurrNode->next;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
