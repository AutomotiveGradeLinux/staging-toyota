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
/// \ingroup  tag_NS_XmlParser
/// \brief    This file contains implementation of CXmlParser class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_xmlparser_if.h>
#include <fstream>
#include <string>
#include "ns_xmlconfig_parser_frameworkunifiedlog.h"

////////////////////////////////////////////////////////////////////////////////////////////////
/// CXmlParser
/// Constructor
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlParser::CXmlParser(): m_pXmlDoc(NULL), m_cFileName(""), m_pRootNode(NULL) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // for maintaining indentation
  xmlKeepBlanksDefault(0);  // LCOV_EXCL_BR_LINE 11: except branch

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CXmlParser
/// Destructor
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlParser::~CXmlParser() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // clears the document structure
  ClearDocument();  // LCOV_EXCL_BR_LINE 11: except branch

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ParseXml
/// Parses the xml file and creates a document structure
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CXmlParser::ParseXml(std::string f_cFileName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  // clears the document structure
  ClearDocument();  // LCOV_EXCL_BR_LINE 11: except branch

  m_cFileName = f_cFileName;

  if (IsReadable(m_cFileName)) {
    // create document object structure
    m_pXmlDoc = xmlParseFile(m_cFileName.c_str());

    if (NULL != m_pXmlDoc) {
      // sets the root node in class
      m_pRootNode.SetXmlNodePtr(xmlDocGetRootElement(m_pXmlDoc));
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Document not parsed successfully :: %s", f_cFileName.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
      l_eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "File not exists :: %s", f_cFileName.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateNewXmlDoc
/// Creates a new xml document with f_cRootNodeName as root node
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CXmlParser::CreateNewXmlDoc(std::string f_cRootNodeName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  xmlNodePtr l_pRootNodePtr = NULL;

  // clears the document structure
  ClearDocument();

  if (0 < f_cRootNodeName.length()) {
    // creates new document structure
    m_pXmlDoc = xmlNewDoc((const xmlChar *)"1.0");
    if (NULL != m_pXmlDoc) {
      // create new node(root)
      l_pRootNodePtr = xmlNewNode(NULL,
                                  (const xmlChar *)f_cRootNodeName.c_str());
      if (NULL != l_pRootNodePtr) {
        // sets the root node in document structure
        xmlDocSetRootElement(m_pXmlDoc, l_pRootNodePtr);

        // sets the root node in class
        m_pRootNode.SetXmlNodePtr(l_pRootNodePtr);
      } else {
        l_eStatus = eFrameworkunifiedStatusNullPointer;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error creating new root node");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Unable to create new xml document structure");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Invalid root node name :: %s", f_cRootNodeName.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// SaveXml
/// Saves the document structure in an xml file
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CXmlParser::SaveXml(std::string f_cFileName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  std::string l_cFileName = "";

  // if user explicitly provides file name
  if (0 < f_cFileName.length()) {
    l_cFileName.assign(f_cFileName);
  } else {  // takes the file name from the CXmlParser object
    l_cFileName.assign(m_cFileName);
  }

  if (NULL != m_pXmlDoc) {
    if (IsReadable(l_cFileName)) {
      // save the doc structure to the file
      if (-1 == xmlSaveFormatFileEnc(l_cFileName.c_str(), m_pXmlDoc, "UTF-8", 1)) {
        l_eStatus = eFrameworkunifiedStatusFail;
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusInvldParam;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "File not exists :: %s", f_cFileName.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "m_pXmlDoc is NULL");   // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }


  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// GetRootNode
/// Gets the root node object of xml
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode CXmlParser::GetRootNode() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return m_pRootNode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// AddNewNode
/// Adds the new node to the parent node
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode CXmlParser::AddNewNode(CXmlNode m_pParentNode, std::string f_cNewNodeName, std::string f_cContent) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CXmlNode l_pXmlNode;

  if (!m_pParentNode.IsNull()) {
    l_pXmlNode = m_pParentNode.AddChildNode(f_cNewNodeName, f_cContent);  // LCOV_EXCL_BR_LINE 11: except branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pXmlNode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// RemoveNode
/// Removes the node from the xml
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CXmlParser::RemoveNode(CXmlNode m_pNode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (!m_pNode.IsNull()) {
    xmlUnlinkNode(m_pNode.m_pXmlNodePtr);
    xmlFreeNode(m_pNode.m_pXmlNodePtr);
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// FindNode
/// Finds the first matching node, by tag name or path
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode CXmlParser::FindNode(std::string f_cNodePath, CXmlNode f_pCurrentNode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CXmlNode l_pXmlNode;

  xmlXPathObjectPtr l_pXpathObj = GetNodeSet(f_cNodePath, f_pCurrentNode);  // LCOV_EXCL_BR_LINE 11: except branch

  if (NULL != l_pXpathObj) {
    if (NULL != l_pXpathObj->nodesetval && 0 < l_pXpathObj->nodesetval->nodeNr) {
      l_pXmlNode.SetXmlNodePtr(l_pXpathObj->nodesetval->nodeTab[0]);  // LCOV_EXCL_BR_LINE 11: except branch
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "NodeSetVal is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    }

    xmlXPathFreeObject(l_pXpathObj);  // LCOV_EXCL_BR_LINE 11: except branch
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "xmlXPathObjectPtr is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pXmlNode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// FindAllNodes
/// Finds all the matching node, by tag name or path
////////////////////////////////////////////////////////////////////////////////////////////////
TNodeList CXmlParser::FindAllNodes(std::string f_cNodePath, CXmlNode f_pCurrentNode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  TNodeList l_pNodeList;

  xmlXPathObjectPtr l_pXpathObj = GetNodeSet(f_cNodePath, f_pCurrentNode);  // LCOV_EXCL_BR_LINE 11: except branch

  if (NULL != l_pXpathObj) {
    if (NULL != l_pXpathObj->nodesetval && 0 < l_pXpathObj->nodesetval->nodeNr) {
      for (UI_8 l_uiSize = 0;
           l_uiSize < l_pXpathObj->nodesetval->nodeNr;
           l_uiSize++) {
        if (NULL != l_pXpathObj->nodesetval->nodeTab[l_uiSize]) {
          CXmlNode l_pNode(l_pXpathObj->nodesetval->nodeTab[l_uiSize]);  // LCOV_EXCL_BR_LINE 11: except branch
          l_pNodeList.push_back(l_pNode);
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "node pointer is NULL");
        }
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "NodeSetVal is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    }

    xmlXPathFreeObject(l_pXpathObj);  // LCOV_EXCL_BR_LINE 11: except branch
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "xmlXPathObjectPtr is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pNodeList;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// GetNodeSet
/// Gets the node set resulting from search of nodepath using XPath
////////////////////////////////////////////////////////////////////////////////////////////////
xmlXPathObjectPtr CXmlParser::GetNodeSet(std::string f_cNodePath, CXmlNode f_pCurrentNode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  xmlXPathObjectPtr l_pXpathObj = NULL;

  xmlXPathContextPtr l_pXpathCtx = NULL;
  CXmlNode l_pXmlNode;

  if (NULL != m_pXmlDoc) {
    if (!f_pCurrentNode.IsNull()) {
      l_pXmlNode = f_pCurrentNode;
    }

    // Create xpath evaluation context
    l_pXpathCtx = xmlXPathNewContext(m_pXmlDoc);  // LCOV_EXCL_BR_LINE 11: except branch

    if (NULL != l_pXpathCtx) {
      // set the current node in path context
      l_pXpathCtx->node = l_pXmlNode.m_pXmlNodePtr;

      // Evaluate xpath expression
      l_pXpathObj = xmlXPathEvalExpression((const xmlChar *)f_cNodePath.c_str(), l_pXpathCtx);  // LCOV_EXCL_BR_LINE 11: except branch

      if (NULL == l_pXpathObj) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to evaluate nodepath :: \"%s\"", f_cNodePath.c_str());   // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
      }

      // Cleanup of XPath data
      xmlXPathFreeContext(l_pXpathCtx);  // LCOV_EXCL_BR_LINE 11: except branch
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to create new XPath context");   // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "document object is NULL");   // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pXpathObj;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ClearDocument
/// Clears the document structure and resets the root node
////////////////////////////////////////////////////////////////////////////////////////////////
VOID CXmlParser::ClearDocument() {
  if (NULL != m_pXmlDoc) {
    // free the document
    xmlFreeDoc(m_pXmlDoc);
    m_pXmlDoc = NULL;

    // free the global variables that may have been allocated by the parser
    xmlCleanupParser();
  }

  m_pRootNode = NULL;
  m_cFileName.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// IsReadable
/// Checks whether the file exists or not
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CXmlParser::IsReadable(const std::string &filename) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_bReadable = FALSE;

  std::ifstream l_ifstream(filename.c_str());

  if (l_ifstream.good()) {
    l_bReadable = TRUE;
    l_ifstream.close();  // LCOV_EXCL_BR_LINE 11: except branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bReadable;
}
