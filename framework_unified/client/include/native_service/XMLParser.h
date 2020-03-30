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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup MY 14 Platform Software Team
/// \brief   Xml parser used to extract and update data from Xml file
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @file XMLParser.h
 * @brief \~english Xml parser used to extract and update data from Xml file
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
/** @addtogroup native
 *  @ingroup framework_unified
 *  @{
 */
#ifndef XMLPARSER_H_  // NOLINT  (build/header_guard)
#define XMLPARSER_H_

#include <libxml/tree.h>

#include <native_service/frameworkunified_types.h>
#include <native_service/ns_logger_if.h>

#include <vector>
#include <list>
#include <string>
#include <map>
#include <sstream>

////////////////////////////////////////////////////////////////////////
///   CTestCaseData : Class used to fill testcase data from XML
////////////////////////////////////////////////////////////////////////


class CTestCaseData {
 public:
  std::string key;

  // Delay for testCase
  SI_32 m_testDelaySec;
  SI_32 m_testDelayMSec;
  SI_32 m_testDelayUSec;
  // TestLoop Count
  SI_32 m_testLoop;

  SI_32 m_testReset;
  SI_32 m_testPrintResult;
  std::string m_testCategory;
  std::string m_testResultType;

  std::vector<std::string> macroVector;
  // Delay for TestMacros
  std::string m_macroDelaySecVal;
  std::string m_macroDelayMsecVal;
  std::string m_macroDelayUsecVal;

  std::list<std::string> expOut;


  CTestCaseData(const xmlChar *name, std::string testDelaySec, std::string testDelayMSec, std::string testDelayUSec,
                std::string testLoopVal, std::string testReset, std::string testPrintResult, std::string testCategory,
                std::string testResultType, std::vector<std::string> paramMacroVector, std::string macroDelaySecVal,
                std::string macroDelayMSecVal,  std::string macroDelayUSecVal,
                std::list<std::string> expOutputVector);

  CTestCaseData(const CTestCaseData &value);

  ~CTestCaseData();
};

class XMLParser {
 private:
  struct FindByName {
    std::string l_sName;
    FindByName(const std::string &name) : l_sName(name) {}  // NOLINT  (readability/nolint)
    bool operator()(const xmlNodePtr &xmlNode) {
      std::stringstream l_sStream;
      l_sStream << xmlNode->name;
      std::string l_sStr = l_sStream.str();
      return l_sStr == l_sName;
    }
  };

 public:
  static std::string xmlCharToString(const xmlChar *value);
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// XMLParser
  /// Class Constructor
  ////////////////////////////////////////////////////////////////////////////////////////////
  XMLParser();
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// XMLParser
  /// Class Destructor
  ////////////////////////////////////////////////////////////////////////////////////////////
  ~XMLParser();
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// readAllXml
  ///   read all XML files from xmlInputFile
  ///
  /// \param [in] xmlInputFile
  ///       string
  ///       xmlList
  ///       string
  /// \return none
  ////////////////////////////////////////////////////////////////////////////////////////////
  void readAllXml(const std::string &xmlInputFile, std::list<std::string> &xmlList);  // NOLINT  (readability/nolint)
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// getChildNodeList
  ///
  /// \param [in] node
  ///       xmlNodePtr
  ///       nodeVector
  ///       vector<xmlNodePtr>
  /// \return none
  ////////////////////////////////////////////////////////////////////////////////////////////
  void getChildNodeList(xmlNodePtr node, std::vector<xmlNodePtr> &nodeVector);  // NOLINT  (readability/nolint)
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// fillMacroVector
  ///
  /// \param [in] node
  ///         xmlFileHandle
  ///         xmlDocPtr
  ///         macroNodeVector
  ///         vector<xmlNodePtr>
  ///         expOutValue
  ///         string
  ///         expOutputVector
  ///         list<std::string>
  ///         macroDelayVal
  ///         string
  ///         paramMacroVector
  ///         vector<std::string>
  /// \return none
  ////////////////////////////////////////////////////////////////////////////////////////////
  void fillMacroVector(xmlDocPtr &xmlFileHandle,  // NOLINT  (readability/nolint)
                       std::vector<xmlNodePtr> &macroNodeVector,  // NOLINT  (readability/nolint)
                       std::string &expOutValue,  // NOLINT  (readability/nolint)
                       std::list<std::string> &expOutputVector,  // NOLINT  (readability/nolint)
                       std::string &macroSecDelayVal,  // NOLINT  (readability/nolint)
                       std::string &macroMSecDelayVal,  // NOLINT  (readability/nolint)
                       std::string &macroUSecDelayVal,  // NOLINT  (readability/nolint)
                       std::vector<std::string> &paramMacroVector);  // NOLINT  (readability/nolint)
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///  StringToNumber
  ///        Convert the string to number
  ///
  /// \param [in]
  ///      Text
  ///      string
  ///
  /// \return number value
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  static int StringToNumber(const std::string &Text) {  // NOLINT  (readability/nolint)
    std::istringstream ss(Text, std::istringstream::in);
    int result;
    return ss >> result ? result : 0;
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// createTestCaseVector
  ///
  /// \param [in]
  ///      testCaseNodeVector
  ///      vector<xmlNodePtr>
  ///      xmlFileHandle
  ///      xmlDocPtr
  ///      testCaseDataVector
  ///      vector<CTestCaseData>
  ///
  /// \return none
  ////////////////////////////////////////////////////////////////////////////////////////////
  void createTestCaseVector(std::vector<xmlNodePtr> &testCaseNodeVector, xmlDocPtr &xmlFileHandle,  // NOLINT  (readability/nolint)
                            std::vector<CTestCaseData> &testCaseDataVector);  // NOLINT  (readability/nolint)
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// GetXmlNodeAttributeValueToString
  ///
  /// \param [in]
  ///      arg
  ///      void *
  ///      str
  ///      const char *
  ///      xmlFileHandle
  ///      xmlDocPtr
  ///
  /// \return none
  ////////////////////////////////////////////////////////////////////////////////////////////
  std::string GetXmlNodeAttributeValueToString(void *arg, const char *str, xmlDocPtr xmlFileHandle);
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// populateTestCaseMap
  ///
  /// \param [in]
  ///      testCaseDataVector
  ///      vector<CTestCaseData>
  ///      xmlFileHandle
  ///      xmlDocPtr
  ///
  /// \return none
  ////////////////////////////////////////////////////////////////////////////////////////////
  void   populateTestCaseMap(std::vector<CTestCaseData> &testCaseDataVector, xmlDocPtr xmlFileHandle) ;  // NOLINT  (readability/nolint)
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// updateXmlActualOutput
  ///
  /// \param [in]
  ///      xmlName
  ///      string
  ///      testCaseName
  ///      string
  ///
  /// \return none
  ////////////////////////////////////////////////////////////////////////////////////////////
  void   updateXmlActualOutput(const std::string &xmlName,  // NOLINT  (readability/nolint)
                               std::string &testCaseName,  // NOLINT  (readability/nolint)
                               std::string &actualOutput);  // NOLINT  (readability/nolint)
  ////////////////////////////////////////////////////////////////////////////////////////////
  /// GetXmlExpectedOutput
  ///
  /// \param [in]
  ///      pKey
  ///      string
  ///      pValue
  ///      string
  ///
  /// \return none
  ////////////////////////////////////////////////////////////////////////////////////////////
  void   GetXmlExpectedOutput(std::string  pKey, std::string pValue);

 private:
  xmlDocPtr m_xmlFileHandle;
  static const std::string EXPECTEDOUTPUT;
  static const std::string VALUE;
  static const std::string DELAYSEC;
  static const std::string DELAYMSEC;
  static const std::string DELAYUSEC;
  static const std::string TESTLOOP;
  static const std::string TESTRESET;
  static const std::string TESTPRINTRESULT;
  static const std::string TESTCATEGORY;
  static const std::string TESTCASEDATA;
  static const std::string MACRODELAYSEC;
  static const std::string MACRODELAYMSEC;
  static const std::string MACRODELAYUSEC;
  static const std::string LOOPPVAL;
  static const std::string ACTUALOUTOUT;
  static const std::string OUTPUT_SEPARATOR;
  static const std::string ACCESS_OPERATOR;
  static const std::string TERMINATE_STR;
  static const SI_32     LOOP_DEFAULT_VALUE;
  static const std::string DEFAULT_VALUE;
  static const std::string PARAMETER_SEPARATOR;
  static const std::string TESTRESULTTYPE;
  static const std::string RESULT_AUTOMATIC;
  static const std::string RESULT_MANUAL;
};

#endif /* XMLPARSER_H_ */  // NOLINT  (build/header_guard)

// EOF
/** @}*/
/** @}*/
/** @}*/
/** @}*/
