/**
 * @file ns_xmlparser_if.h
 * @brief \~english This file contains declaration of class CXmlParser, CXmlNode and CXmlAttr.
 *        This file provides the interface for Parsing XML file, API for operation on node
 *        of an xml structure.
 *
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */
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

#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NS_XMLPARSER_INC_NS_XMLPARSER_IF_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NS_XMLPARSER_INC_NS_XMLPARSER_IF_H__

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <string>
#include <list>

// defines the type of xml nodes
/**
 * \~english  type of xml nodes
 */
typedef enum _EFrameworkunifiedXmlNodeTypes {
  FRAMEWORKUNIFIED_XML_NODE_NONE = 0,  //!< \~english not exist node
  FRAMEWORKUNIFIED_XML_ELEMENT_NODE = 1,  //!< \~english element node
  FRAMEWORKUNIFIED_XML_ATTRIBUTE_NODE = 2,  //!< \~english attribute node
  FRAMEWORKUNIFIED_XML_TEXT_NODE = 3,  //!< \~english text node
  FRAMEWORKUNIFIED_XML_CDATA_SECTION_NODE = 4,  //!< \~english section node
  FRAMEWORKUNIFIED_XML_ENTITY_REF_NODE = 5,  //!< \~english entity ref node
  FRAMEWORKUNIFIED_XML_ENTITY_NODE = 6,  //!< \~english entity node
  FRAMEWORKUNIFIED_XML_PI_NODE = 7,  //!< \~english PI node
  FRAMEWORKUNIFIED_XML_COMMENT_NODE = 8,  //!< \~english comment node
  FRAMEWORKUNIFIED_XML_DOCUMENT_NODE = 9,  //!< \~english document node
  FRAMEWORKUNIFIED_XML_DOCUMENT_TYPE_NODE = 10,  //!< \~english document type node
  FRAMEWORKUNIFIED_XML_DOCUMENT_FRAG_NODE = 11,  //!< \~english document frag node
  FRAMEWORKUNIFIED_XML_NOTATION_NODE = 12,  //!< \~english notation node
  FRAMEWORKUNIFIED_XML_HTML_DOCUMENT_NODE = 13,  //!< \~english document node
  FRAMEWORKUNIFIED_XML_DTD_NODE = 14,  //!< \~english dtd node
  FRAMEWORKUNIFIED_XML_ELEMENT_DECL = 15,  //!< \~english element decl
  FRAMEWORKUNIFIED_XML_ATTRIBUTE_DECL = 16,  //!< \~english attribute decl
  FRAMEWORKUNIFIED_XML_ENTITY_DECL = 17,  //!< \~english entity decl
  FRAMEWORKUNIFIED_XML_NAMESPACE_DECL = 18,  //!< \~english namespace decl
  FRAMEWORKUNIFIED_XML_XINCLUDE_START = 19,  //!< \~english include start
  FRAMEWORKUNIFIED_XML_XINCLUDE_END = 20,  //!< \~english include end
  FRAMEWORKUNIFIED_XML_DOCB_DOCUMENT_NODE = 21  //!< \~english document node
} EFrameworkunifiedXmlNodeTypes;

// forward declaration
class CXmlNode;
class CXmlAttr;

// list of CXmlNode
typedef std::list<CXmlNode> TNodeList;
typedef TNodeList::iterator TNodeListIterator;

// list of CXmlAttr
typedef std::list<CXmlAttr> TAttrList;
typedef std::list<CXmlAttr>::iterator TAttrListIterator;


/**
 * @class CXmlNode
 * \~english @brief This class represents node of an XML.
 * \~english @par   Brief Introduction
 *        Provides API for performing operations on node.
 *
 */
class CXmlNode {
  friend class CXmlParser;

 public:
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CXmlNode
  /// \~english @par Brief
  ///       Destructor of CXmlNode class
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Inside state change
  ///       - None
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Destructor of CXmlNode class
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////
  ~CXmlNode();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IsNull
  /// \~english @par Brief
  ///       Check xmlNodePtr
  /// \~english @param None
  /// \~english @retval BOOL true if valid else false
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Checks whether the corresponding xmlNodePtr is valid or not
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  BOOL IsNull();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Type
  /// \~english @par Brief
  ///       Get type
  /// \~english @param  None
  /// \~english @retval FRAMEWORKUNIFIED_XML_NODE_NONE  not exist node
  /// \~english @retval FRAMEWORKUNIFIED_XML_ELEMENT_NODE   element node
  /// \~english @retval FRAMEWORKUNIFIED_XML_TEXT_NODE  text node
  /// \~english @retval FRAMEWORKUNIFIED_XML_CDATA_SECTION_NODE   section node
  /// \~english @retval FRAMEWORKUNIFIED_XML_ENTITY_REF_NODE  entity ref node
  /// \~english @retval FRAMEWORKUNIFIED_XML_ENTITY_NODE  entity node
  /// \~english @retval FRAMEWORKUNIFIED_XML_PI_NODE  PI node
  /// \~english @retval FRAMEWORKUNIFIED_XML_COMMENT_NODE   comment node
  /// \~english @retval FRAMEWORKUNIFIED_XML_DOCUMENT_NODE  document node
  /// \~english @retval FRAMEWORKUNIFIED_XML_DOCUMENT_TYPE_NODE   document type node
  /// \~english @retval FRAMEWORKUNIFIED_XML_DOCUMENT_FRAG_NODE   document frag node
  /// \~english @retval FRAMEWORKUNIFIED_XML_NOTATION_NODE  notation node
  /// \~english @retval FRAMEWORKUNIFIED_XML_HTML_DOCUMENT_NODE   document node
  /// \~english @retval FRAMEWORKUNIFIED_XML_DTD_NODE   dtd node
  /// \~english @retval FRAMEWORKUNIFIED_XML_ELEMENT_DECL   element decl
  /// \~english @retval FRAMEWORKUNIFIED_XML_ATTRIBUTE_DECL   attribute decl
  /// \~english @retval FRAMEWORKUNIFIED_XML_ENTITY_DECL  entity decl
  /// \~english @retval FRAMEWORKUNIFIED_XML_NAMESPACE_DECL   namespace decl
  /// \~english @retval FRAMEWORKUNIFIED_XML_XINCLUDE_START   include start
  /// \~english @retval FRAMEWORKUNIFIED_XML_XINCLUDE_END   include end
  /// \~english @retval FRAMEWORKUNIFIED_XML_DOCB_DOCUMENT_NODE   document node
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Gets the type of XML node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedXmlNodeTypes Type();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Name
  /// \~english @par Brief
  ///       Get name
  /// \~english @param  None
  /// \~english @retval std::string node name
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get the name/tag of an XML node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  std::string Name();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetContent
  /// \~english @par Brief
  ///       Get content
  /// \~english @param  None
  /// \~english @retval std::string content of current node
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Gets the content of current node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  std::string GetContent();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetContent
  /// \~english @par Brief
  ///       Set content
  /// \~english @param  [IN] f_cUpdatedText
  ///       std::string  - updated value
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusNullPointer Null pointer
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Set or update the content of current node. If the node has children then it removes all the child
  ///       nodes and sets the content for the current node.
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus SetContent(std::string f_cUpdatedText);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetAttributeValue
  /// \~english @par Brief
  ///       Get attribute
  /// \~english @param  [IN] f_cKey
  ///       std::string - attribute name
  /// \~english @param  [OUT] f_cAttributeValue
  ///       std::string& - attribute value
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusNullPointer Null pointer
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get the attribute's value for the key
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus GetAttributeValue(std::string f_cKey, std::string &f_cAttributeValue);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetAttributeValue
  /// \~english @par Brief
  ///       Set attribute
  /// \~english @param  [IN] f_cKey
  ///       std::string - attribute name
  /// \~english @param  [IN] f_cValue
  ///       std::string - attribute value
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Set the attribute's value for the key
  ///       If attribute is not found then new attribute key-value pair is created and added to node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus SetAttributeValue(std::string f_cKey, std::string f_cValue);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup AddNewAttribute
  /// \~english @par Brief
  ///       Add new attribute
  /// \~english @param  [IN] f_cKey
  ///       std::string - attribute name
  /// \~english @param  [IN] f_cValue
  ///       std::string - attribute value
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusNullPointer Null pointer
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Add new attribute to the node. If the attribute key is existing then it updates the existing
  ///       value with the new value or else adds the new attribute
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus AddNewAttribute(std::string f_cKey, std::string f_cAttributeValue);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup RemoveAttribute
  /// \~english @par Brief
  ///       Removes attribute
  /// \~english @param  [IN] f_cKey
  ///       std::string - attribute name
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Remove the attribute from the node with the specified key
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus RemoveAttribute(std::string f_cKey);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetAttributeItems
  /// \~english @par Brief
  ///       Get attributes list
  /// \~english @param  None
  /// \~english @retval TAttrList - list of all attributes of node or empty list in case of no attributes
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get the list of all the attribute's key and value of the node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  TAttrList GetAttributeItems();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Parent
  /// \~english @par Brief
  ///       Get parent
  /// \~english @param  None
  /// \~english @retval CXmlNode - node object
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get the parent node of current node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////


  CXmlNode Parent();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup FirstChild
  /// \~english @par Brief
  ///       Get first child
  /// \~english @param  None
  /// \~english @retval CXmlNode - node object
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get the first child node of current node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  CXmlNode FirstChild();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup LastChild
  /// \~english @par Brief
  ///       Get last child
  /// \~english @param  None
  /// \~english @retval CXmlNode - node object
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get the last child node of current node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  CXmlNode LastChild();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Children
  /// \~english @par Brief
  ///       Get child list
  /// \~english @param  None
  /// \~english @retval TNodeList - list of all child node object
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get the list of all the child nodes of current node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  TNodeList Children();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NextSibling
  /// \~english @par Brief
  ///       Get next node
  /// \~english @param  None
  /// \~english @retval CXmlNode - node object
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get the next sibling node of current node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  CXmlNode NextSibling();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup PrevSibling
  /// \~english @par Brief
  ///       Get previous node
  /// \~english @param  None
  /// \~english @retval CXmlNode - node object
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get the previous sibling node of current node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  CXmlNode PrevSibling();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup FindChildNode
  /// \~english @par Brief
  ///       Find child node
  /// \~english @param  [IN] f_cNodeName std::string - tag name of the node
  /// \~english @retval CXmlNode - node object
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get the first immediate sub node with matching node name
  ///       This function does not provide support to find node based on attribute value
  ///       Use FindNode() API of CXmlParser to find node based on XPath expression
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////


  CXmlNode FindChildNode(std::string f_cNodeName);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup FindAllChildNodes
  /// \~english @par Brief
  ///       Get list of all the immediate sub node
  /// \~english @param  [IN] f_cNodeName std::string - tag name of the node
  /// \~english @retval TNodeList - list of matching nodes
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get the list of all the immediate sub node matching tag as f_cNodeName.
  ///       This function does not provide support to find node based on attribute value.
  ///       Use FindNode() API of CXmlParser to find node based on XPath expression.
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  TNodeList FindAllChildNodes(std::string f_cNodeName);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetContentOfChildNode
  /// \~english @par Brief
  ///       Find first level node
  /// \~english @param  [IN] f_cNodeName
  ///       std::string - name of the node
  /// \~english @param  [OUT] f_cContent
  ///       std::string& - content of the node
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusFail Failed
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Find the first level sub node matching tag as f_cNodeName
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus GetContentOfChildNode(std::string f_cNodeName, std::string &f_cContent);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup AddChildNode
  /// \~english @par Brief
  ///       Add sub node
  /// \~english @param  [IN] f_cNodeName
  ///       std::string - tag of the new node
  /// \~english @param  [IN] f_cText
  ///       std::string - content of the new node
  /// \~english @retval CXmlNode - object of child node
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Add sub node to the current node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  CXmlNode AddChildNode(std::string f_cNodeName, std::string f_cText = "");

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup AddSiblingNode
  /// \~english @par Brief
  ///       Add new node to siblings
  /// \~english @param  [IN] f_cNodeName
  ///       std::string - tag of the new node
  /// \~english @param  [IN] f_cText
  ///       std::string - content of the new node
  /// \~english @retval CXmlNode - object of sibling node
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Add a new node to the list of siblings of current node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  CXmlNode AddSiblingNode(std::string f_cNodeName, std::string f_cText = "");

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup RemoveChildNode
  /// \~english @par Brief
  ///       Remove child node
  /// \~english @param  [IN] f_cNodeName
  ///       std::string - name of child node
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusNullPointer Null pointer
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Remove the child node from current node
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus RemoveChildNode(std::string f_cNodeName);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ClearNode
  /// \~english @par Brief
  ///       Removes elements and attributes
  /// \~english @param  None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusNullPointer Null pointer
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       This function removes all sub elements, clears all attributes,
  ///       and sets the text and tail attributes to None
  /// \~english @see CXmlNode
  ////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ClearNode();

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CXmlNode
  /// Constructor of CXmlNode class
  ///
  /// \param
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CXmlNode();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CXmlNode
  /// Parameterized constructor of CXmlNode class
  ///
  /// \param f_pXmlNodePtr
  ///      xmlNodePtr - pointer of an xml node
  ///
  /// \return
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CXmlNode(xmlNodePtr f_pXmlNodePtr);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// SetXmlNodePtr
  /// Sets the xmlNodePtr
  ///
  /// \param f_pXmlNodePtr
  ///      xmlNodePtr - pointer of an xml node
  ///
  /// \return VOID
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetXmlNodePtr(xmlNodePtr f_pXmlNodePtr);

  // pointer to xml node
  xmlNodePtr m_pXmlNodePtr;
};


/**
 * @class CXmlAttr
 * \~english @brief This class represents the attribute of an xml node.
 * \~english @par   Brief Introduction
 *        This class represents the attribute of an xml node.
 *
 */
class CXmlAttr {
 public:
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CXmlAttr
  /// \~english @par Brief
  ///       CXmlAttr constructor
  /// \~english @param  None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Parameterized constructor of CXmlAttr class
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////
  CXmlAttr();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CXmlAttr
  /// \~english @par Brief
  ///       CXmlAttr constructor
  /// \~english @param  [IN] f_cKey
  ///       std::string - attributes key
  /// \~english @param  [IN] f_cValue
  ///       std::string - attributes value
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Parameterized constructor of CXmlAttr class
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////
  CXmlAttr(std::string f_cKey, std::string f_cValue);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CXmlAttr
  /// \~english @par Brief
  ///       Destructor of CXmlAttr class
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Inside state change
  ///       - None
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Destructor of CXmlAttr class
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////
  ~CXmlAttr();

  // atrribute key
  std::string m_cKey;

  // atrribute value
  std::string m_cValue;
};

/**
 * @class CXmlParser
 * \~english @brief This class represents the XML parser.
 * \~english @par   Brief Introduction
 *        Provides API for parsing xml file. Also provides API to search node/nodes based on Xml Path
 *
 */
class CXmlParser {
 public:
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CXmlParser
  /// \~english @par Brief
  ///       CXmlParser constructor
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Constructor of CXmlParser class
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////
  CXmlParser();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CXmlParser
  /// \~english @par Brief
  ///       Destructor of CXmlParser class
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Destructor of CXmlParser class
  /// \~english @see CXmlParser
  ////////////////////////////////////////////////////////////////////////////////
  ~CXmlParser();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ParseXml
  /// \~english @par Brief
  ///       Parses xml file
  /// \~english @param  [IN] f_cFileName
  ///      std::string - full path of xml file to be parsed
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam  Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusNullPointer Null pointer
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Parses the xml file and creates a document structure
  /// \~english @see CXmlParser
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus ParseXml(std::string f_cFileName);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CreateNewXmlDoc
  /// \~english @par Brief
  ///       Creates new xml
  /// \~english @param  [IN] f_cRootNodeName
  ///      std::string - root node name in new doc
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam  Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusNullPointer Null pointer
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Create a new xml document
  /// \~english @see CXmlParser
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus CreateNewXmlDoc(std::string f_cRootNodeName);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SaveXml
  /// \~english @par Brief
  ///       Save XML file
  /// \~english @param  [IN] f_cFileName
  ///      std::string - full path of output xml file
  ///            In case of parsing an xml file,
  ///               if f_cFileName is empty, output will be saved in source xml file
  ///               else output will be saved in user provided file path
  ///            In case of creating new xml document,
  ///               if f_cFileName is empty, returns error
  ///               else output will be saved in user provided file path
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam  Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusNullPointer Null pointer
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Save the updated structure to the XML file
  /// \~english @see CXmlParser
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus SaveXml(std::string f_cFileName = "");

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetRootNode
  /// \~english @par Brief
  ///       Get root node
  /// \~english @param None
  /// \~english @retval CXmlNode - root node object
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get the root node object
  /// \~english @see CXmlParser
  ////////////////////////////////////////////////////////////////////////////////

  CXmlNode GetRootNode();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup AddNewNode
  /// \~english @par Brief
  ///       Create and add node
  /// \~english @param  [IN] f_pParentNode
  ///       CXmlNode - parent node
  /// \~english @param  [IN] f_cNewNodeName
  ///       std::string - tag name of the new node
  /// \~english @param  [IN] f_cContent
  ///       std::string - text to set for the new node
  /// \~english @retval CXmlNode - new node object
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Create new node and add it to the parent node
  /// \~english @see CXmlParser
  ////////////////////////////////////////////////////////////////////////////////

  CXmlNode AddNewNode(CXmlNode m_pParentNode, std::string f_cNewNodeName, std::string f_cContent = "");

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup RemoveNode
  /// \~english @par Brief
  ///      Remove node
  /// \~english @param  [IN] f_pNode
  ///      CXmlNode - node object
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusNullPointer Null pointer
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Remove the node from the xml
  /// \~english @see CXmlParser
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus RemoveNode(CXmlNode m_pNode);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup FindNode
  /// \~english @par Brief
  ///       Find first matching node
  /// \~english @param  [IN] f_cNodePath
  ///       std::string - name or path of node(XPath) path is relative to current node or absolute path
  /// \~english @param  [IN] f_pCurrentNode
  ///       CXmlNode - current node
  /// \~english @retval CXmlNode - node object
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Find the first matching node, by tag name or path relative to current node path or
  ///       absolute path. Here the path is XPath expression
  /// \~english @see CXmlParser
  ////////////////////////////////////////////////////////////////////////////////


  CXmlNode FindNode(std::string f_cNodePath, CXmlNode f_pCurrentNode);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup FindAllNodes
  /// \~english @par Brief
  ///       Find all matching node
  /// \~english @param  [IN] f_cNodePath
  ///       std::string - name or path of node(XPath) path is relative to current node or absolute path
  /// \~english @param  [IN] f_pCurrentNode
  ///       CXmlNode - current node
  /// \~english @retval TNodeList - list of matching nodes
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Get CXmlNode from CXmlParser
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Find all the matching node, by tag name or path relative to current node path or
  ///       absolute path.
  /// \~english @see CXmlParser
  ////////////////////////////////////////////////////////////////////////////////

  TNodeList FindAllNodes(std::string f_cNodePath, CXmlNode f_pCurrentNode);

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetNodeSet
  /// Gets the node set resulting from search of nodepath using XPath
  ///
  /// \param f_cNodePath
  ///      std::string - name or path of node
  ///            path is relative to current node or absolute path
  /// \param f_pCurrentNode
  ///      CXmlNode - current node
  ///
  /// \return xmlXPathObjectPtr
  ///     xmlXPathObjectPtr - returns the xpath object pointer containing nodes pointer
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  xmlXPathObjectPtr GetNodeSet(std::string f_cNodePath, CXmlNode f_pCurrentNode);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// IsReadable
  /// Checks whether the file exists or not
  ///
  /// \param
  ///
  /// \return VOID
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsReadable(const std::string &filename);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ClearDocument
  /// Clears the document structure and resets the root node
  ///
  /// \param
  ///
  /// \return VOID
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  VOID ClearDocument();

  // pointer to structure created from xml after parsing
  xmlDocPtr m_pXmlDoc;

  // stores the name/path of source xml file
  std::string m_cFileName;

  // pointer to the root node of xml structure
  CXmlNode m_pRootNode;
};


#endif  /* __FRAMEWORKUNIFIED_NATIVESERVICES_NS_XMLPARSER_INC_NS_XMLPARSER_IF_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
