/**
 * @file ns_xml_reader.h
 * @brief \~english This file contains declaration of class CXMLReader.
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_ConfigParser
/// \brief    This file contains declaration of class CXMLReader.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NSCONFIGPARSER_INC_NS_XML_READER_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NSCONFIGPARSER_INC_NS_XML_READER_H__

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <libxml/tree.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/ns_reader.h>
#include <string>


/**
 * @class CXMLReader
 * \~english @par   Brief Introduction
 *        This class provides functionalities to read from XML config file.
 *
 */
class CXMLReader: public IConfigReader {
 public:
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CXMLReader
  /// \~english @par Brief
  ///       This API constructor of CXMLReader class.
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
  ///       Sync
  /// \~english @par Detail
  ///       This API should be called by ServiceA and create a new object of CXMLReader.
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////
  CXMLReader();
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CXMLReader
  /// \~english @par Brief
  ///       This API constructor of CXMLReader class.
  /// \~english @param  [IN] f_cFilePath
  ///       std::string - Full path of the configuration file
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync
  /// \~english @par Detail
  ///       This API should be called by ServiceA and create a new object of CXMLReader with filepath.
  /// \~english @see GetCXMLReaderObject
  ////////////////////////////////////////////////////////////////////////////////

  CXMLReader(const std::string &f_cFilePath);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~CXMLReader
  /// \~english @par CXMLReader
  ///        Destructor of CXMLReader class.
  /// \~english @param None
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           - Close shared memory object.
  /// \~english @par Conditions of processing failure
  ///           -None
  /// \~english @par Detail
  ///           Destruct the object of CXMLReader
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @see CXMLReader
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CXMLReader();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ParseFile
  /// \~english @par Brief
  ///       This API parser file
  /// \~english @param  [IN] f_cFilePath
  ///       std::string - path of file to parse
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusNullPointer Null pointer
  /// \~english @par Prerequisite
  ///       - Load the shared library first.
  ///       - Create XmlReader object.
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync
  /// \~english @par Detail
  ///       This API is used to parse the file from the path.
  /// \~english @see CXMLReader or GetCXMLReaderObject
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus ParseFile(const std::string &f_cFilePath);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ParseFile
  /// \~english @par Brief
  ///       Get Value with key
  /// \~english @param  [IN] f_cKey
  ///       std::string - key to search
  /// \~english @retval std::string - value for key
  /// \~english @par Prerequisite
  ///       - Load the shared library first.
  ///       - Create XmlReader object.
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync
  /// \~english @par Detail
  ///       This function is used to get the value associated with the key
  /// \~english @see CXMLReader or GetCXMLReaderObject
  ////////////////////////////////////////////////////////////////////////////////
  std::string GetValue(const std::string &f_cKey);
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ParseFile
  /// \~english @par Brief
  ///       Get Value with key
  /// \~english @param  [IN] f_cKey
  ///       std::string - key to search
  /// \~english @param  [REF] f_cValue
  ///       std::string - Value of key
  /// \~english @retval eFrameworkunifiedStatusOK Success / Pass / OK
  /// \~english @retval eFrameworkunifiedStatusFail Failed
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusNullPointer Null pointer
  /// \~english @par Prerequisite
  ///       - Load the shared library first.
  ///       - Create XmlReader object.
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync
  /// \~english @par Detail
  ///       This function is used to get the value associated with the key
  /// \~english @see CXMLReader or GetCXMLReaderObject
  ////////////////////////////////////////////////////////////////////////////////


  EFrameworkunifiedStatus GetValue(const std::string &f_cKey, std::string &f_cValue);  // NOLINT  (readability/nolint)
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ParseFile
  /// \~english @par Brief
  ///       Get Value with key
  /// \~english @param  None
  /// \~english @retval PVOID - pointer of data structure
  /// \~english @par Prerequisite
  ///       - Load the shared library first.
  ///       - Create XmlReader object.
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync
  /// \~english @par Detail
  ///       This function is used to get the data pointer. This pointer is then set in config writer.
  /// \~english @see CXMLReader or GetCXMLReaderObject
  ////////////////////////////////////////////////////////////////////////////////
  PVOID GetDataPtr();

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// XMLGetValue
  /// This function is used to parse and get the value from xml file
  ///
  /// \param  [IN] f_pCurrNode
  ///     xmlNodePtr - pointer to current node in xml
  /// \param  [IN] f_cUserKey
  ///     std::string - Key to be searched for
  ///
  /// \return std::string - content of key node
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  std::string XMLGetValue(xmlNodePtr f_pCurrNode, const std::string &f_cUserKey);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// XMLGetValue
  /// This function is used to parse and get the value from xml file
  ///
  /// \param  [IN] f_pCurrNode
  ///     xmlNodePtr - pointer to current node in xml
  /// \param  [IN] f_cUserKey
  ///     std::string - Key to be searched for
  /// \param  [IN] f_cValue
  ///     std::string - Value of Key
  /// \param  [IN] f_bKeyFound
  ///     BOOL - flag to check whether key exists in xml file or not
  ///
  /// \return EFrameworkunifiedStatus - error if key not found or else eFrameworkunifiedStatusOK
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus XMLGetValue(xmlNodePtr f_pCurrNode,
                         const std::string &f_cUserKey, std::string &f_cValue, BOOL &f_bKeyFound);  // NOLINT  (readability/nolint)
  // pointer of xml document structure(DOM)
  xmlDocPtr m_pXmlDoc;
};

#ifdef __cplusplus
extern "C" {
#endif
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup  GetCXMLReaderObject
  /// \~english @par Brief
  ///       Get CXMLReader object.
  /// \~english @param  [IN] f_cFilePath
  ///       std::string - path of file to parse
  /// \~english @retval CXMLReader *
  /// \~english @par Prerequisite
  ///       - Load the shared library first.
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync
  /// \~english @par Detail
  ///       This API is used to create CXMLReader object with file path and return the CXMLReader pointer.
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////
CXMLReader *GetCXMLReaderObject(CHAR *f_cFilePath);
#ifdef __cplusplus
}
#endif
#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_NSCONFIGPARSER_INC_NS_XML_READER_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
