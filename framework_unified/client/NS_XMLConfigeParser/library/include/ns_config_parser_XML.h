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
/// \ingroup MY 14 Platfom Software Team
/// \brief   Internal Header for ns_config_parser_XML.c
///
///  These functions are used to read  from and write to XML file.
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_XMLCONFIGEPARSER_LIBRARY_INCLUDE_NS_CONFIG_PARSER_XML_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_XMLCONFIGEPARSER_LIBRARY_INCLUDE_NS_CONFIG_PARSER_XML_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <ns_config_parser_internal.h>
#define BUFFERSIZE 200
#define ZERO 0

////////////////////////////////////////////////////////////////////////////////////////////
/// XMLGetValue:
/// Function : get value for given key.
///
/// \param [in] pCurrNode
///       xmlNodePtr - pCurrNode is a Pointer to node of XML tree.
/// \param [in] pNodePath
///       const char * - pNodePath is a Pointer to path of Current XML node.
/// \param [in] pUserKey
///       const char * - pUserKey is a Pointer to Key provided by user.
/// \param [in] pReturnValue
///       char * - pReturnValue is a Pointer to value to be returned corresponding to Key.
/// \param [in] pReturnState
///       int * -  pReturnState is updated depending upon user provided Key\n
///                    returns 0 if user passed valid key.\n
///                returns 1 if user passed invalid key.\n
/// \return  status
///             void
///////////////////////////////////////////////////////////////////////////////////////////
void XMLGetValue(xmlNodePtr pcurrnode , const char *pnodepath, const char *puserkey, char *preturnvalue,
                 int *preturnstate);

////////////////////////////////////////////////////////////////////////////////////////////
/// XMLUpdateOldValue:
/// Function : update value corresponding to given user key.
///
/// \param [in] pCurrNode
///       xmlNodePtr - pCurrNode is a Pointer to node of XML tree.
/// \param [in] pNodePath
///       const char * - pNodePath is a Pointer to path of Current XML node.
/// \param [in] pUserKey
///       const char * - pUserKey is a Pointer to Key provided by user.
/// \param [in] pValue
///       const char * - pValue is a Pointer to the new value for attribute or text.
/// \param [in] pReturnState
///       int * - returns 0 if user passed valid key.\n
///                     returns -6 if old value is not updated with new value.\n
/// \return status
///             void
///////////////////////////////////////////////////////////////////////////////////////////
void XMLUpdateOldValue(xmlNodePtr pcurrnode , const char *pnodepath, const char *puserkey, const char *preturnvalue,
                       int *preturnstate);

////////////////////////////////////////////////////////////////////////////////////////////
/// XMLUpdateOldValueWithNewValue:
/// Function : Traverse all nodes of XML tree read value to given node,compare with old value,\n
///            if old value matched then write new value in place of old value.
///
/// \param [in] pCurrNode
///       xmlNodePtr - pCurrNode is a Pointer to node of XML tree.
/// \param [in] pNodePath
///       const char * -pNodePath is a Pointer to path of Current XML node.
/// \param [in] pUserKey
///       const char * -pUserKey is a Pointer to Key provided by user.
/// \param [in] pOldValue
///       const char * - pOldValue is a Pointer to the old value for attribute or text of key passed.
/// \param [in] pNewValue
///       const char * -pNewValue is a Pointer to the new value for attribute or text to be assigned to the key.
/// \param [in] pReturnState
///       int * - returns 0 if user passed valid key.\n
///                     returns -6 if old value is not updated with new value.\n
/// \return status
///             void
///////////////////////////////////////////////////////////////////////////////////////////
void XMLUpdateOldValueWithNewValue(xmlNodePtr pcurrnode , const char *pnodepath, const char *puserkey,
                                   const char *poldvalue,  const char *pnewvalue, int *preturnstate);



#ifdef __cplusplus
}
#endif

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_XMLCONFIGEPARSER_LIBRARY_INCLUDE_NS_CONFIG_PARSER_XML_H_

// EOF
