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
/// \brief    This file contains implementation of CXmlNode class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_xmlparser_if.h>
#include <string>
#include "ns_xmlconfig_parser_frameworkunifiedlog.h"


////////////////////////////////////////////////////////////////////////////////////////////////
/// CXmlNode
/// Constructor
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode::CXmlNode(): m_pXmlNodePtr(NULL) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// CXmlNode
/// Parameterized Constructor
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode::CXmlNode(xmlNodePtr f_pXmlNodePtr): m_pXmlNodePtr(f_pXmlNodePtr) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CXmlNode
/// Destructor
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode::~CXmlNode() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// SetXmlNodePtr
/// Sets the xmlNodePtr in class
////////////////////////////////////////////////////////////////////////////////////////////////
VOID CXmlNode::SetXmlNodePtr(xmlNodePtr f_pXmlNodePtr) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  m_pXmlNodePtr = f_pXmlNodePtr;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// IsNull
/// Checks whether the node is valid or not
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CXmlNode::IsNull() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_bNull = FALSE;

  if (NULL == m_pXmlNodePtr) {
    l_bNull = TRUE;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_bNull;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Type
/// Gets the type of node
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedXmlNodeTypes CXmlNode::Type() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedXmlNodeTypes l_eNodeType = FRAMEWORKUNIFIED_XML_NODE_NONE;

  if (NULL != m_pXmlNodePtr) {
    switch (m_pXmlNodePtr->type) {
      case XML_ELEMENT_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_ELEMENT_NODE;
        break;
      case XML_ATTRIBUTE_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_ATTRIBUTE_NODE;
        break;
      case XML_TEXT_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_TEXT_NODE;
        break;
      case XML_CDATA_SECTION_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_CDATA_SECTION_NODE;
        break;
      case XML_ENTITY_REF_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_ENTITY_REF_NODE;
        break;
      case XML_ENTITY_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_ENTITY_NODE;
        break;
      case XML_PI_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_PI_NODE;
        break;
      case XML_COMMENT_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_COMMENT_NODE;
        break;
      case XML_DOCUMENT_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_DOCUMENT_NODE;
        break;
      case XML_DOCUMENT_TYPE_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_DOCUMENT_TYPE_NODE;
        break;
      case XML_DOCUMENT_FRAG_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_DOCUMENT_FRAG_NODE;
        break;
      case XML_NOTATION_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_NOTATION_NODE;
        break;
      case XML_HTML_DOCUMENT_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_HTML_DOCUMENT_NODE;
        break;
      case XML_DTD_NODE:
        l_eNodeType = FRAMEWORKUNIFIED_XML_DTD_NODE;
        break;
      case XML_ELEMENT_DECL:
        l_eNodeType = FRAMEWORKUNIFIED_XML_ELEMENT_DECL;
        break;
      case XML_ATTRIBUTE_DECL:
        l_eNodeType = FRAMEWORKUNIFIED_XML_ATTRIBUTE_DECL;
        break;
      case XML_ENTITY_DECL:
        l_eNodeType = FRAMEWORKUNIFIED_XML_ENTITY_DECL;
        break;
      case XML_NAMESPACE_DECL:
        l_eNodeType = FRAMEWORKUNIFIED_XML_NAMESPACE_DECL;
        break;
      case XML_XINCLUDE_START:
        l_eNodeType = FRAMEWORKUNIFIED_XML_XINCLUDE_START;
        break;
      case XML_XINCLUDE_END:
        l_eNodeType = FRAMEWORKUNIFIED_XML_XINCLUDE_END;
        break;
      default:
        break;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eNodeType;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Name
/// Gets the name of node
////////////////////////////////////////////////////////////////////////////////////////////////
std::string CXmlNode::Name() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::string l_cNodeName = "";

  if (NULL != m_pXmlNodePtr && NULL != m_pXmlNodePtr->name) {
    l_cNodeName = std::string((PCSTR)m_pXmlNodePtr->name);  // LCOV_EXCL_BR_LINE 11: except branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_cNodeName;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// GetContent
/// Gets the content of node
////////////////////////////////////////////////////////////////////////////////////////////////
std::string CXmlNode::GetContent() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::string l_cText = "";

  if ((NULL != m_pXmlNodePtr) &&
      (NULL != m_pXmlNodePtr->children) &&
      (XML_TEXT_NODE == m_pXmlNodePtr->children->type) &&
      (NULL == m_pXmlNodePtr->children->next)) {
    xmlChar * p_xmlText = xmlNodeGetContent(m_pXmlNodePtr);
    l_cText = std::string((PCSTR)p_xmlText);  // LCOV_EXCL_BR_LINE 11: except branch
    xmlFree(p_xmlText);  // LCOV_EXCL_BR_LINE 11: except branch
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_cText;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// SetContent
/// Sets or updates the content of current node.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CXmlNode::SetContent(std::string f_cUpdatedText) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pXmlNodePtr) {
    xmlNodeSetContent(m_pXmlNodePtr, (const xmlChar *)f_cUpdatedText.c_str());
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// GetAttributeValue
/// Gets the attribute's value for the key.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CXmlNode::GetAttributeValue(std::string f_cKey, std::string &f_cAttributeValue) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pXmlNodePtr) {
    xmlChar *l_pAttrValue = xmlGetProp(m_pXmlNodePtr,
                                       (const xmlChar *)f_cKey.c_str());
    if (NULL != l_pAttrValue) {
      f_cAttributeValue.assign((PCSTR)l_pAttrValue);

      xmlFree(l_pAttrValue);
      l_pAttrValue = NULL;
    } else {
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Attribute key :: %s not found", f_cKey.c_str());
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Node ptr is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// SetAttributeValue
/// Sets the attribute's value for the key
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CXmlNode::SetAttributeValue(std::string f_cKey, std::string f_cValue) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "SetAttributeValue feature is not available,"
      "Reason: This API is not supported by libxml2 present in PosixBasedOS001");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// AddNewAttribute
/// Add new attribute to the node
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CXmlNode::AddNewAttribute(std::string f_cKey, std::string f_cAttributeValue) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  xmlChar* p_key = reinterpret_cast<xmlChar*>(const_cast<char*>(f_cKey.c_str()));
  xmlChar* p_value = reinterpret_cast<xmlChar*>(const_cast<char*>(f_cAttributeValue.c_str()));
  if (NULL != m_pXmlNodePtr) {
    if (NULL == xmlNewNsProp(m_pXmlNodePtr,
                             NULL,
                             p_key,
                             p_value)) {
      l_eStatus = eFrameworkunifiedStatusNullPointer;
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Unable to add new property :: %s", f_cKey.c_str());
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Node ptr is NULL");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// RemoveAttribute
/// Removes the attribute from the node with the specified key
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CXmlNode::RemoveAttribute(std::string f_cKey) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  FRAMEWORKUNIFIEDLOG(ZONE_NS_WAR, __FUNCTION__, "RemoveAttribute feature is not available,"
      "Reason: This API is not supported by libxml2 present in PosixBasedOS001");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// GetAttributeItems
/// Gets the list of all the attribute's key and value of the node
////////////////////////////////////////////////////////////////////////////////////////////////
TAttrList CXmlNode::GetAttributeItems() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  TAttrList l_cAttrList;
  std::string l_cKey;
  std::string l_cValue;

  if (NULL != m_pXmlNodePtr) {
    for (xmlAttrPtr l_pAttr = m_pXmlNodePtr->properties;
         l_pAttr != NULL;
         l_pAttr = l_pAttr->next) {
      l_cKey = (PCSTR)l_pAttr->name;
      l_cValue = "";

      if (NULL != l_pAttr->children) {
        l_cValue = (PCSTR)l_pAttr->children->content;
      }

      CXmlAttr l_pXmlAttr(l_cKey, l_cValue);
      l_cAttrList.push_back(l_pXmlAttr);  // LCOV_EXCL_BR_LINE 11: except branch
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_cAttrList;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Parent
/// Gets the parent node
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode CXmlNode::Parent() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CXmlNode l_pXmlNode;

  if (NULL != m_pXmlNodePtr && NULL != m_pXmlNodePtr->parent) {
    if (XML_ELEMENT_NODE == m_pXmlNodePtr->parent->type) {
      l_pXmlNode.SetXmlNodePtr(m_pXmlNodePtr->parent);  // LCOV_EXCL_BR_LINE 11: except branch
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pXmlNode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// FirstChild
/// Gets the first child of node
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode CXmlNode::FirstChild() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CXmlNode l_pXmlNode;

  if (NULL != m_pXmlNodePtr) {
    for (xmlNodePtr l_pNodePtr = m_pXmlNodePtr->children;
         l_pNodePtr != NULL;
         l_pNodePtr = l_pNodePtr->next) {
      if (XML_ELEMENT_NODE == l_pNodePtr->type) {
        l_pXmlNode.SetXmlNodePtr(l_pNodePtr);  // LCOV_EXCL_BR_LINE 11: except branch
        break;
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pXmlNode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// LastChild
/// Gets the last child of node
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode CXmlNode::LastChild() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CXmlNode l_pXmlNode;

  if (NULL != m_pXmlNodePtr) {
    for (xmlNodePtr l_pNodePtr = m_pXmlNodePtr->last;
         l_pNodePtr != NULL;
         l_pNodePtr = l_pNodePtr->prev) {
      if (XML_ELEMENT_NODE == l_pNodePtr->type) {
        l_pXmlNode.SetXmlNodePtr(l_pNodePtr);  // LCOV_EXCL_BR_LINE 11: except branch
        break;
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pXmlNode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Children
/// Gets the list of all the child node of current node
////////////////////////////////////////////////////////////////////////////////////////////////
TNodeList CXmlNode::Children() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  TNodeList l_pNodeList;

  if (NULL != m_pXmlNodePtr) {
    for (xmlNodePtr l_pNodePtr = m_pXmlNodePtr->children;
         l_pNodePtr != NULL;
         l_pNodePtr = l_pNodePtr->next) {
      if (XML_ELEMENT_NODE == l_pNodePtr->type) {
        CXmlNode l_pXmlNode(l_pNodePtr);
        l_pNodeList.push_back(l_pXmlNode);
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pNodeList;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// NextSibling
/// Gets the next sibling node of current node
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode CXmlNode::NextSibling() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CXmlNode l_pXmlNode;

  if (NULL != m_pXmlNodePtr) {
    for (xmlNodePtr l_pNodePtr = m_pXmlNodePtr->next;
         l_pNodePtr != NULL;
         l_pNodePtr = l_pNodePtr->next) {
      if (XML_ELEMENT_NODE == l_pNodePtr->type) {
        l_pXmlNode.SetXmlNodePtr(l_pNodePtr);  // LCOV_EXCL_BR_LINE 11: except branch
        break;
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pXmlNode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// PrevSibling
/// Gets the previous sibling node of current node
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode CXmlNode::PrevSibling() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CXmlNode l_pXmlNode;

  if (NULL != m_pXmlNodePtr) {
    for (xmlNodePtr l_pNodePtr = m_pXmlNodePtr->prev;
         l_pNodePtr != NULL;
         l_pNodePtr = l_pNodePtr->prev) {
      if (XML_ELEMENT_NODE == l_pNodePtr->type) {
        l_pXmlNode.SetXmlNodePtr(l_pNodePtr);  // LCOV_EXCL_BR_LINE 11: except branch
        break;
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pXmlNode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// FindChildNode
/// Finds the first immediate sub node matching tag as f_cNodeName.
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode CXmlNode::FindChildNode(std::string f_cNodeName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  CXmlNode l_pXmlNode;

  if (NULL != m_pXmlNodePtr) {
    for (xmlNodePtr l_pNodePtr = m_pXmlNodePtr->children;
         l_pNodePtr != NULL;
         l_pNodePtr = l_pNodePtr->next) {
      if ((XML_ELEMENT_NODE == m_pXmlNodePtr->type) &&
          (0 == f_cNodeName.compare((PCSTR)l_pNodePtr->name))) {
        l_pXmlNode.SetXmlNodePtr(l_pNodePtr);  // LCOV_EXCL_BR_LINE 11: except branch
        break;
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pXmlNode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// FindAllChildNodes
/// Gets the list of all the immediate sub node matching tag as f_cNodeName.
////////////////////////////////////////////////////////////////////////////////////////////////
TNodeList CXmlNode::FindAllChildNodes(std::string f_cNodeName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  TNodeList l_pNodeList;

  if (NULL != m_pXmlNodePtr) {
    for (xmlNodePtr l_pNodePtr = m_pXmlNodePtr->children;
         l_pNodePtr != NULL;
         l_pNodePtr = l_pNodePtr->next) {
      if ((XML_ELEMENT_NODE == m_pXmlNodePtr->type) &&
          (0 == f_cNodeName.compare((PCSTR)l_pNodePtr->name))) {
        CXmlNode l_pXmlNode(l_pNodePtr);
        l_pNodeList.push_back(l_pXmlNode);
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_pNodeList;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// GetContentOfChildNode
/// Finds the first level sub node matching tag as f_cNodeName.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CXmlNode::GetContentOfChildNode(std::string f_cNodeName, std::string &f_cContent) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  CXmlNode l_pXmlNode = FindChildNode(f_cNodeName);

  if (!l_pXmlNode.IsNull()) {
    f_cContent.assign(l_pXmlNode.GetContent());
  } else {
    l_eStatus = eFrameworkunifiedStatusFail;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// AddChildNode
/// Adds sub node to the current node
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode CXmlNode::AddChildNode(std::string f_cNodeName, std::string f_cText) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // pointer of child node
  xmlNodePtr l_pChildNode = NULL;

  // content of child node
  xmlChar *l_cNodeContent = NULL;

  // validate the node name
  if (NULL != m_pXmlNodePtr && 0 < f_cNodeName.length()) {
    if (0 < f_cText.length()) {
      xmlChar* p_text = reinterpret_cast<xmlChar*>(const_cast<char*>(f_cText.c_str()));
      l_cNodeContent = p_text;
    }

    if (NULL == (l_pChildNode = xmlNewDocNode(m_pXmlNodePtr->doc,
                                              m_pXmlNodePtr->ns,
                                              (const xmlChar *)f_cNodeName.c_str(),
                                              l_cNodeContent))) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error creating child node");
    } else {
      l_pChildNode->type = XML_ELEMENT_NODE;
      l_pChildNode->parent = m_pXmlNodePtr;
      l_pChildNode->doc = m_pXmlNodePtr->doc;

      // if there is no other child to the parent
      if (m_pXmlNodePtr->children == NULL) {
        m_pXmlNodePtr->children = l_pChildNode;
        m_pXmlNodePtr->last = l_pChildNode;
      } else {  // else add the new child to the end of children list
        // temp node
        xmlNodePtr l_pTempNode = m_pXmlNodePtr->last;
        l_pTempNode->next = l_pChildNode;
        l_pChildNode->prev = l_pTempNode;

        m_pXmlNodePtr->last = l_pChildNode;
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return CXmlNode(l_pChildNode);
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// AddSiblingNode
/// Adds new node as sibling to the current node
////////////////////////////////////////////////////////////////////////////////////////////////
CXmlNode CXmlNode::AddSiblingNode(std::string f_cNodeName, std::string f_cText) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  xmlNodePtr l_pNewNode = NULL;
  xmlNodePtr l_pSiblingNode = NULL;

  if (NULL != m_pXmlNodePtr &&
      !this->Parent().IsNull()) {  // check whether the node is root node
    if (NULL != (l_pNewNode = xmlNewNode(NULL,
                                         (const xmlChar *)f_cNodeName.c_str()))) {
      if (0 < f_cText.length()) {
        xmlChar* p_text = reinterpret_cast<xmlChar*>(const_cast<char*>(f_cText.c_str()));
        xmlNodeSetContent(l_pNewNode, p_text);
      }

      l_pSiblingNode = xmlAddSibling(m_pXmlNodePtr,
                                     l_pNewNode);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return CXmlNode(l_pSiblingNode);
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// RemoveChildNode
/// Removes the child node from current node
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CXmlNode::RemoveChildNode(std::string f_cNodeName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  CXmlNode l_pXmlNode = FindChildNode(f_cNodeName);
  if (!l_pXmlNode.IsNull()) {
    xmlUnlinkNode(l_pXmlNode.m_pXmlNodePtr);
    xmlFreeNode(l_pXmlNode.m_pXmlNodePtr);
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// ClearNode
/// Clears the contents, attributes of node
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CXmlNode::ClearNode() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_pXmlNodePtr) {
    for (xmlNodePtr l_pXmlNode = m_pXmlNodePtr->children;
         l_pXmlNode != NULL;
         l_pXmlNode = m_pXmlNodePtr->children) {
      xmlUnlinkNode(l_pXmlNode);
      xmlFreeNode(l_pXmlNode);
    }

    for (xmlAttrPtr l_pXmlAttr = m_pXmlNodePtr->properties;
         l_pXmlAttr != NULL;
         l_pXmlAttr = m_pXmlNodePtr->properties) {
      xmlRemoveProp(l_pXmlAttr);
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
