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

/**
 * @file ns_xml_writer.h
 * @brief \~english This file contains declaration of class CXMLWriter.
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_ConfigParser
/// \brief    This file contains declaration of class CXMLWriter.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FRAMEWORKUNIFIED_NATIVESERVICES_NSCONFIGPARSER_INC_NS_XML_WRITER_H__  // NOLINT  (build/header_guard)
#define __FRAMEWORKUNIFIED_NATIVESERVICES_NSCONFIGPARSER_INC_NS_XML_WRITER_H__

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <libxml/tree.h>

#include <native_service/frameworkunified_types.h>
#include <native_service/ns_writer.h>

#include <string>

/**
 * @class CXMLWriter
 * \~english @brief this file has the CXMLWriter class definitions
 * \~english @par   Brief Introduction
 *        This class provides functionalities to write to XML config file
 *
 */
class CXMLWriter: public IConfigWriter {
 public:
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CXMLWriter
  /// \~english @par Brief
  ///       Parameterless Constructor of CXMLWriter class
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
  ///       This API should be called by ServiceA and create a new object without file path of CXMLWriter
  /// \~english @see GetCXMLWriterObjectNoParam
  ////////////////////////////////////////////////////////////////////////////////

  CXMLWriter();
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CXMLWriter
  /// \~english @par Brief
  ///       Constructor of CXMLWriter class
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
  ///       This API should be called by ServiceA and create a new object with file path of CXMLWriter
  /// \~english @see GetCXMLWriterObject
  ////////////////////////////////////////////////////////////////////////////////
  CXMLWriter(const std::string &f_cFilePath);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~CXMLWriter
  /// \~english @par CXMLWriter
  ///        Destructor of CXMLWriter class.
  /// \~english @param None
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           -None
  /// \~english @par Conditions of processing failure
  ///           -None
  /// \~english @par Detail
  ///           Destruct the object of CXMLWriter
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @see CXMLWriter
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CXMLWriter();

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ParseFile
  /// \~english @par Brief
  ///       Parse file
  /// \~english @param  [IN] f_cFilePath
  ///       std::string - path of file to parse
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusNullPointer Null pointer
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Create CXMLWriter object
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync
  /// \~english @par Detail
  ///       This function is used to parse the file
  /// \~english @see CXMLWriter
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus ParseFile(const std::string &f_cFilePath);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetValue
  /// \~english @par Brief
  ///       Set vaule for key
  /// \~english @param  [IN] f_cKey
  ///       std::string - key to search
  /// \~english @param  [IN] f_cValue
  ///       std::string - value to set
  /// \~english @retval eFrameworkunifiedStatusOK Success / Pass / OK
  /// \~english @retval eFrameworkunifiedStatusFail Failed
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusNullPointer Null pointer
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Create CXMLWriter object
  ///       - ParseFile set the path
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync
  /// \~english @par Detail
  ///       This function is used to set the value for the key
  /// \~english @see CXMLWriter,ParseFile
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus SetValue(const std::string &f_cKey, std::string f_cValue);
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SaveData
  /// \~english @par Brief
  ///       Save changed data
  /// \~english @param  None
  /// \~english @retval eFrameworkunifiedStatusOK Success / Pass / OK
  /// \~english @retval eFrameworkunifiedStatusFail Failed
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Create CXMLWriter object
  ///       - ParseFile set the path
  ///       - Change the data
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync
  /// \~english @par Detail
  ///       This function is used to save the changed value to the xml config file
  /// \~english @see GetCXMLWriterObjectNoParam
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus SaveData();
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetPath
  /// \~english @par Brief
  ///       Updata file path
  /// \~english @param  [IN] f_cPath
  ///       std::string - Path of file
  /// \~english @retval eFrameworkunifiedStatusOK Success / Pass / OK
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Create CXMLWriter object
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync
  /// \~english @par Detail
  ///       This function is used to set the config file path
  /// \~english @see GetCXMLWriterObjectNoParam
  ////////////////////////////////////////////////////////////////////////////////

  EFrameworkunifiedStatus SetPath(const std::string &f_cPath);
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetDataPtr
  /// \~english @par Brief
  ///       Parse file
  /// \~english @param  [IN] f_pData
  ///       PVOID - Pointer to data structure
  /// \~english @retval VOID
  /// \~english @par Prerequisite
  ///       - Load the shared library first
  ///       - Create CXMLWriter object
  /// \~english @par Inside state change
  ///       - The API no inside state change
  /// \~english @par None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync
  /// \~english @par Detail
  ///       This function is used to set the data pointer in config writer class
  /// \~english @see CXMLWriter
  ////////////////////////////////////////////////////////////////////////////////

  VOID SetDataPtr(PVOID f_pData);

 private:
////////////////////////////////////////////////////////////////////////////////////////////////
/// XMLSetValue
/// This function is used to parse the value from xml file
///
/// \param  [IN] f_pCurrNode
///     xmlNodePtr - pointer to current node in xml
/// \param  [IN] f_cUserKey
///     std::string - Key to be searched for
/// \param  [IN] f_cValue
///     std::string - Required Value for key
/// \param  [IN] f_bKeyFound
///     std::string - Flag to check whether key is present in xml file
///
/// \return EFrameworkunifiedStatus- success or failure
///
////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus XMLSetValue(xmlNodePtr f_pCurrNode,
                         const std::string &f_cUserKey,  // NOLINT  (readability/nolint)
                         std::string &f_cValue,  // NOLINT  (readability/nolint)
                         BOOL &f_bKeyFound);  // NOLINT  (readability/nolint)

// pointer of xml document structure(DOM)
  xmlDocPtr m_pXmlDoc;

// path of config file
  std::string m_cFilePath;
};

#ifdef __cplusplus
extern "C" {
#endif
  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetCXMLWriterObject
  /// \~english @par Brief
  ///      Get CXMLWriter object.
  /// \~english @param  [IN] f_cFilePath
  ///      std::string - path of file to parse
  /// \~english @retval CXMLWriter *
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
  ///       This API is used to create CXMLWriter object with file path and return the CXMLWriter pointer.
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////
CXMLWriter *GetCXMLWriterObject(CHAR *f_cFilePath);

  ////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetCXMLWriterObjectNoParam
  /// \~english @par Brief
  ///      Get CXMLWriter object.
  /// \~english @param  None
  /// \~english @retval CXMLWriter *
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
  ///       This API is used to create CXMLWriter object without file path and return the CXMLWriter pointer.
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////

CXMLWriter *GetCXMLWriterObjectNoParam();
#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORKUNIFIED_NATIVESERVICES_NSCONFIGPARSER_INC_NS_XML_WRITER_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
