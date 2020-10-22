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
 * @file
 * @brief \~english This file contains the declaration of CNSRcsPlugin class
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
/** @addtogroup nsrcs
 *  @ingroup framework_unified
 *  @{
 */

#ifndef _NS_RCS_PLUGIN_H__  // NOLINT  (build/header_guard)
#define _NS_RCS_PLUGIN_H__

#include <native_service/frameworkunified_types.h>
#include <string>

typedef EFrameworkunifiedStatus(*TFPNSRcsSendPassthruData)(UI_8 f_ui8clientId, PVOID f_data, UI_16 f_ui16PayloadLength);

// forward declaration
class CPassThruInDataHandler;

/**
 * @class CNSRcsPlugin
 * \~english @brief CNSRcsPlugin
 * \~english @par   Brief Introduction
 *        Class to provide base class of CNSRcsPlugin
 */

class CNSRcsPlugin {
 public:
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRcsPlugin
  /// \~english @par Summary
  ///       Constructor of CNSRcsPlugin class.
  /// \~english @param [in] f_cPluginName
  ///        std::string  - the name of plugin
  /// \~english @param [in] f_fpNSRcsSendPassthruData
  ///        TFPNSRcsSendPassthruData - the pointer of send data function
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       Constructor of CNSRcsPlugin class. \n
  ///       variables (m_cPluginName,m_fpNSRcsSendPassthruData,m_uiPluginId) to be initialization.
  /// \~english @see  ~CNSRcsPlugin
  ////////////////////////////////////////////////////////////////////////////////////
  CNSRcsPlugin(std::string f_cPluginName, TFPNSRcsSendPassthruData f_fpNSRcsSendPassthruData)
    : m_fpNSRcsSendPassthruData(f_fpNSRcsSendPassthruData),
      m_cPluginName(f_cPluginName) {
    static UI_8 l_uiPluginId = 0;
    m_uiPluginId = ++l_uiPluginId;
  }
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRcsPlugin
  /// \~english @par Summary
  ///       Destructor of CNSRcsPlugin class.
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       To delete a CNSRcsPlugin class. \n
  /// \~english @see  CNSRcsPlugin
  ////////////////////////////////////////////////////////////////////////////////////
  virtual ~CNSRcsPlugin() {

  }
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRcsPlugin
  /// \~english @par Summary
  ///       get plugin id.
  /// \~english @param None
  /// \~english @retval Plugin Id
  ///        UI_8  - the ID of plugin
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       provide API whar get plugin id. \n
  /// \~english @see  CNSRcsPlugin
  ////////////////////////////////////////////////////////////////////////////////////
  UI_8 GetPluginId() {
    return m_uiPluginId;
  }
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRcsPlugin
  /// \~english @par Summary
  ///       get plugin id.
  /// \~english @param None
  /// \~english @retval m_cPluginName
  ///        std::string - the name of plugin
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       provide API whar get plugin name. \n
  /// \~english @see  CNSRcsPlugin
  ////////////////////////////////////////////////////////////////////////////////////
  std::string GetPluginName() {
    return m_cPluginName;
  }

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRcsPlugin
  /// \~english @par Summary
  ///       set parent name.
  /// \~english @param f_cParentName
  ///        std::string - the name of parent
  /// \~english @retval none
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       provide API whar set parent name. \n
  /// \~english @see  CNSRcsPlugin
  ////////////////////////////////////////////////////////////////////////////////////
  static VOID SetParentName(const std::string &f_cParentName) {
    CNSRcsPlugin::m_cParentName = f_cParentName;
  }

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRcsPlugin
  /// \~english @par Summary
  ///       request from host side.
  /// \~english @param f_uiProtocol
  ///        UI_32 - protocol ID
  /// \~english @param f_ui8ClientId
  ///        UI_8 - client ID
  /// \~english @param f_pData
  ///        CPassThruInDataHandler - the handler of transfer data
  /// \~english @retval none
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       provide API whar request from host side.\n
  /// \~english @see  CNSRcsPlugin
  ////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus ProcessRequestMessage(UI_32 f_uiProtocol, UI_8 f_ui8ClientId, CPassThruInDataHandler *f_pData) = 0;
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRcsPlugin
  /// \~english @par Summary
  ///       response from service side on target
  /// \~english @param f_cMsgSource
  ///        PCSTR  -
  /// \~english @param f_uiProtocol
  ///        UI_32 - protocol ID
  /// \~english @param f_pui8Data
  ///        UI_8 - pointer of data
  /// \~english @param f_uiDataLength
  ///        UI_32 - length of data
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       provide API whar response from service side on target.\n
  /// \~english @see  CNSRcsPlugin
  ////////////////////////////////////////////////////////////////////////////////////
  virtual EFrameworkunifiedStatus ProcessResponseMessage(PCSTR f_cMsgSource, UI_32 f_uiProtocol, UI_8 *f_pui8Data,
                                            UI_32 f_uiDataLength) = 0;
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRcsPlugin
  /// \~english @par Summary
  ///       get response protocol array
  /// \~english @param none
  /// \~english @retval UI_32 const *
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       provide API whar get response protocol array.\n
  /// \~english @see  CNSRcsPlugin
  ////////////////////////////////////////////////////////////////////////////////////
  virtual UI_32 const *GetResponseProtocolArray() = 0;
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRcsPlugin
  /// \~english @par Summary
  ///       get length of response protocol array
  /// \~english @param none
  /// \~english @retval UI_32 const
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       provide API whar get length of response protocol array.\n
  /// \~english @see  CNSRcsPlugin
  ////////////////////////////////////////////////////////////////////////////////////
  virtual UI_32 const GetResponseProtocolArrayLength() = 0;

  static std::string m_cParentName;  // TODO: move to private

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSRcsPlugin
  /// \~english @par Summary
  ///       the function of transport data
  /// \~english @param f_ui8ClientId
  ///        UI_8 - client id
  /// \~english @param f_data
  ///        PVOID - the pointer of transport data
  /// \~english @param f_ui16PayloadLength
  ///        UI_16 - the size of transport data
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - Change of internal state according to the API does not occur.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///          sync only
  /// \~english @par Detail
  ///       provide API whar the function of transport data.\n
  /// \~english @see  CNSRcsPlugin
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SendPassthruOutData(UI_8 f_ui8clientId, PVOID f_data, UI_16 f_ui16PayloadLength) {
    return m_fpNSRcsSendPassthruData(f_ui8clientId, f_data, f_ui16PayloadLength);
  }
 private:
  CNSRcsPlugin() {
  }

  UI_8    m_uiPluginId;
  TFPNSRcsSendPassthruData m_fpNSRcsSendPassthruData;
  std::string m_cPluginName;
};

// std::string CNSRcsPlugin::m_cParentName = "";

#endif  // _NS_RCS_PLUGIN_H__  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
