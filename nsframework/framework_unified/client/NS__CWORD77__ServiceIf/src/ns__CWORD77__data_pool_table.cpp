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

////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_HMI_CWORD77_Controller
/// \brief
///
////////////////////////////////////////////////////////////////////////////////

#include <native_service/ns__CWORD77__data_pool_table.h>
#include <native_service/ns__CWORD77__data_pool_if.h>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include "ns__CWORD77__data_pool_internal.h"
#include "ns__CWORD77__common_internal.h"

C_CWORD77_DataPool g__CWORD77_DataPool;
C_CWORD77_DataPool *C_CWORD77_DataPool::m_p_CWORD77_DataPoolInstance = NULL;

// For getting/setting notification data from data pool,
// data key will be pair<g_uiNotificationDataPoolId,"NotificationName">
static const UI_32 g_uiNotificationDataPoolId = 0;

/////////////////////////////////////////////////////////////////////////////////
/// NoOfElementsinData
/////////////////////////////////////////////////////////////////////////////////
UI_32 NoOfElementsinData(std::string Input) {
  std::string first = "";
  std::istringstream ss(Input);
  int i = 0;
  while (!ss.eof()) {
    getline(ss, first, ',');
    if (first != "") {
      i++;
    }
  }
  return i;
}

/////////////////////////////////////////////////////////////////////////////////
/// ConvertArrayStringsToString
/////////////////////////////////////////////////////////////////////////////////
std::string ConvertArrayStringsToString(std::string *strArr, int Size) {
  int length = 0;
  std::string Result;
  if (strArr == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "strArr is NULL");
    Result.append("");
    return Result;
  }
  while (length < Size) {
    Result.append(strArr[length]);
    Result.append(",");
    length++;
  }
  Result.append("");
  return Result;
}
/////////////////////////////////////////////////////////////////////////////////
/// ConvertStringToArrString
/////////////////////////////////////////////////////////////////////////////////
void ConvertStringToArrString(std::string Input, std::string *strArr,
                              UI_32 &ArraySize) {   // NOLINT  (readability/nolint)
  if (strArr == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "strArr is NULL");
    return;
  }
  int i = 0;
  std::string first = "";
  std::istringstream ss(Input);
  while (!ss.eof()) {
    getline(ss, first, ',');
    if (first != "") {
      strArr[i] = first;
      i++;
    }
  }
  ArraySize = i;
}
/////////////////////////////////////////////////////////////////////////////////
/// C_CWORD77_DataPool
/////////////////////////////////////////////////////////////////////////////////
C_CWORD77_DataPool::C_CWORD77_DataPool() {
  m__CWORD77_DataRequestTable.clear();
  m__CWORD77_DataResponseTable.clear();
}
/////////////////////////////////////////////////////////////////////////////////
/// ~C_CWORD77_DataPool
/////////////////////////////////////////////////////////////////////////////////
C_CWORD77_DataPool::~C_CWORD77_DataPool() {
  m__CWORD77_DataRequestTable.clear();
  m__CWORD77_DataResponseTable.clear();
}

//////////////////////////////////////////
/// Function : GetInstance
//////////////////////////////////////////
C_CWORD77_DataPool *C_CWORD77_DataPool::GetInstance() {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "GetInstance");
  if (m_p_CWORD77_DataPoolInstance == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "Before creating object");
    m_p_CWORD77_DataPoolInstance = new C_CWORD77_DataPool();
  }
  return m_p_CWORD77_DataPoolInstance;
}

//////////////////////////////////////////
/// Function : DeleteInstance
//////////////////////////////////////////
void C_CWORD77_DataPool::DeleteInstance() {
  DELETEPTR(m_p_CWORD77_DataPoolInstance);
}
/////////////////////////////////////////////////////////////////////////////////
/// GetValueFromMap
/////////////////////////////////////////////////////////////////////////////////
void C_CWORD77_DataPool::GetValueFromMap(ETableType TableType, UI_32 VarName, EDataType &DataType, std::string &Strvalue) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  std::map<UI_32, _CWORD77_Data>::iterator iter;
  std::string temp;

  if (TableType == REQUEST_TABLE) {
    iter = m__CWORD77_DataRequestTable.begin();
    iter = m__CWORD77_DataRequestTable.find(VarName);
    if (iter != m__CWORD77_DataRequestTable.end()) {
      DataType = iter->second.DataType;
      temp = iter->second.DataValue;
    }
  } else {
    iter = m__CWORD77_DataResponseTable.begin();
    iter = m__CWORD77_DataResponseTable.find(VarName);
    if (iter != m__CWORD77_DataResponseTable.end()) {
      DataType = iter->second.DataType;
      temp = iter->second.DataValue;
    }
  }
  Strvalue.assign(temp);
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
}
/////////////////////////////////////////////////////////////////////////////////
/// SetValueinMap
/////////////////////////////////////////////////////////////////////////////////
void C_CWORD77_DataPool::SetValueinMap(ETableType TableType, UI_32 VarName, EDataType VarType, std::string Result) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  _CWORD77_Data t_CWORD77_Data;
  t_CWORD77_Data.DataType = VarType;
  t_CWORD77_Data.DataValue = Result;
  if (TableType == REQUEST_TABLE) {
    m__CWORD77_DataRequestTable[VarName] = t_CWORD77_Data;
  } else {
    m__CWORD77_DataResponseTable[VarName] = t_CWORD77_Data;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
}

/////////////////////////////////////////////////////////////////////////////////
/// GetNoofElementsofData
/////////////////////////////////////////////////////////////////////////////////
UI_32 C_CWORD77_DataPool::GetNoofElementsofData(ETableType TableType, UI_32 KeyName) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EDataType DataType = UNKNOWNTYPE;
  std::string Result = "";
  GetValueFromMap(TableType, KeyName, DataType, Result);
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return NoOfElementsinData(Result);
}
/////////////////////////////////////////////////////////////////////////////////
/// GetDataType
/////////////////////////////////////////////////////////////////////////////////
EDataType C_CWORD77_DataPool::GetDataType(ETableType TableType, UI_32 KeyName) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EDataType DataType = UNKNOWNTYPE;
  std::string Result = "";
  GetValueFromMap(TableType, KeyName, DataType, Result);
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return DataType;
}
/////////////////////////////////////////////////////////////////////////////////
/// SetArrayStringData
/////////////////////////////////////////////////////////////////////////////////
void C_CWORD77_DataPool::SetArrayStringData(ETableType TableType, UI_32 VarName, EDataType VarType,
                                      std::string DataValue[], UI_32 size) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  std::string Result = ConvertArrayStringsToString(DataValue, size);
  SetValueinMap(TableType, VarName, VarType, Result);
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
}
/////////////////////////////////////////////////////////////////////////////////
/// GetArrayStringData
/////////////////////////////////////////////////////////////////////////////////
void C_CWORD77_DataPool::GetArrayStringData(ETableType TableType, UI_32 VarName, std::string DataValue[], UI_32 &size) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  std::string Result = "";
  EDataType DataType = UNKNOWNTYPE;
  GetValueFromMap(TableType, VarName, DataType, Result);
  ConvertStringToArrString(Result, DataValue, size);
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
}


/////////////////////////////////////////////////////////////////////////////////
/// SetRequestArrayStringData
/////////////////////////////////////////////////////////////////////////////////
void SetRequestArrayStringData(UI_32 VarName, EDataType VarType,  std::string DataValue[], UI_32 size) {
  g__CWORD77_DataPool.SetReqArrayStringData(VarName, VarType, DataValue, size);
}

/////////////////////////////////////////////////////////////////////////////////
/// SetResponseArrayStringData
/////////////////////////////////////////////////////////////////////////////////
void SetResponseArrayStringData(UI_32 VarName, EDataType VarType,  std::string DataValue[], UI_32 size) {
  g__CWORD77_DataPool.SetRespArrayStringData(VarName, VarType, DataValue, size);
}
/////////////////////////////////////////////////////////////////////////////////
/// GetRequestArrayStringData
/////////////////////////////////////////////////////////////////////////////////
void GetRequestArrayStringData(UI_32 VarName, std::string DataValue[],
                               UI_32 &size) {  // NOLINT  (readability/nolint)
  g__CWORD77_DataPool.GetReqArrayStringData(VarName, DataValue, size);
}

/////////////////////////////////////////////////////////////////////////////////
/// GetResponseArrayStringData
/////////////////////////////////////////////////////////////////////////////////
void GetResponseArrayStringData(UI_32 VarName, std::string DataValue[],
                                UI_32 &size) {  // NOLINT  (readability/nolint)
  g__CWORD77_DataPool.GetRespArrayStringData(VarName, DataValue, size);
}

/////////////////////////////////////////////////////////////////////////////////
/// SetRequestStringData
/////////////////////////////////////////////////////////////////////////////////

void SetRequestStringData(UI_32 VarName, EDataType VarType,  std::string DataValue) {
  g__CWORD77_DataPool.SetReqStringData(VarName, VarType, DataValue);
}
/////////////////////////////////////////////////////////////////////////////////
/// SetResponseStringData
/////////////////////////////////////////////////////////////////////////////////
void SetResponseStringData(UI_32 VarName, EDataType VarType,  std::string DataValue) {
  g__CWORD77_DataPool.SetRespStringData(VarName, VarType, DataValue);
}
/////////////////////////////////////////////////////////////////////////////////
/// GetRequestStringData
/////////////////////////////////////////////////////////////////////////////////
std::string GetRequestStringData(UI_32 VarName) {
  return g__CWORD77_DataPool.GetReqStringData(VarName);
}

/////////////////////////////////////////////////////////////////////////////////
/// GetRequestDataSize
/////////////////////////////////////////////////////////////////////////////////
UI_32 GetRequestDataSize(UI_32 Key) {
  return g__CWORD77_DataPool.GetReqDataSize(Key);
}

/////////////////////////////////////////////////////////////////////////////////
/// GetResponseDataSize
/////////////////////////////////////////////////////////////////////////////////
UI_32 GetResponseDataSize(UI_32 Key) {
  return g__CWORD77_DataPool.GetRespDataSize(Key);
}

/////////////////////////////////////////////////////////////////////////////////
/// GetRequestDataType
/////////////////////////////////////////////////////////////////////////////////
EDataType GetRequestDataType(UI_32 Key) {
  return g__CWORD77_DataPool.GetReqDataType(Key);
}
/////////////////////////////////////////////////////////////////////////////////
/// GetResponseDataType
/////////////////////////////////////////////////////////////////////////////////
EDataType GetResponseDataType(UI_32 Key) {
  return g__CWORD77_DataPool.GetRespDataType(Key);
}



C_CWORD77_Data *C_CWORD77_Data::m_p_CWORD77_DataInstance = NULL;

/////////////////////////////////////////////////////////////////////////////////
/// Constructor
/////////////////////////////////////////////////////////////////////////////////
C_CWORD77_Data::C_CWORD77_Data() {
  m__CWORD77_RequestDataTable.clear();
  m__CWORD77_ResponseDataTable.clear();
}
/////////////////////////////////////////////////////////////////////////////////
/// Destructor
/////////////////////////////////////////////////////////////////////////////////
C_CWORD77_Data::~C_CWORD77_Data() {
  m__CWORD77_RequestDataTable.clear();
  m__CWORD77_ResponseDataTable.clear();
}
/////////////////////////////////////////////////////////////////////////////////
/// GetInstacne
/////////////////////////////////////////////////////////////////////////////////
C_CWORD77_Data *C_CWORD77_Data::GetInstance() {
  if (m_p_CWORD77_DataInstance == NULL) {
    m_p_CWORD77_DataInstance = new C_CWORD77_Data();  // LCOV_EXCL_BR_LINE 11:except branch
  }
  return m_p_CWORD77_DataInstance;
}
/////////////////////////////////////////////////////////////////////////////////
/// DeleteInstance
/////////////////////////////////////////////////////////////////////////////////
void C_CWORD77_Data::DeleteInstance() {
  delete m_p_CWORD77_DataInstance;
  m_p_CWORD77_DataInstance = NULL;
}
/////////////////////////////////////////////////////////////////////////////////
/// SetRequData
/////////////////////////////////////////////////////////////////////////////////
VOID C_CWORD77_Data::SetRequData(const _CWORD77_DataPoolKey &f_DataPoolKey, UI_32 f_uiSize, PVOID f_pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  std::vector<CHAR> &DataVector =  m__CWORD77_RequestDataTable[f_DataPoolKey];
  CHAR *l_pTemp = static_cast<CHAR *>(f_pData);
  DataVector.assign(l_pTemp, (l_pTemp + f_uiSize));
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
}
/////////////////////////////////////////////////////////////////////////////////
/// SetRequData
/////////////////////////////////////////////////////////////////////////////////
VOID C_CWORD77_Data::SetRequData(UI_32 f_uiMsgId, UI_32 f_uiSize, PVOID f_pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  std::vector<CHAR> &DataVector =  m__CWORD77_RequestDataTableOrig[f_uiMsgId];
  CHAR *l_pTemp = static_cast<CHAR *>(f_pData);
  DataVector.assign(l_pTemp, (l_pTemp + f_uiSize));
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
}
/////////////////////////////////////////////////////////////////////////////////
/// SetRespoData
/////////////////////////////////////////////////////////////////////////////////
VOID C_CWORD77_Data::SetRespoData(const _CWORD77_DataPoolKey &f_DataPoolKey, UI_32 f_uiSize, const PVOID f_pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  std::vector<CHAR> &DataVector =  m__CWORD77_ResponseDataTable[f_DataPoolKey];
  CHAR *l_pTemp = static_cast<CHAR *>(f_pData);
  DataVector.assign(l_pTemp, (l_pTemp + f_uiSize));
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
}
/////////////////////////////////////////////////////////////////////////////////
/// SetRespoData
/////////////////////////////////////////////////////////////////////////////////
VOID C_CWORD77_Data::SetRespoData(UI_32 f_uiMsgId, UI_32 f_uiSize, const PVOID f_pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  std::vector<CHAR> &DataVector =  m__CWORD77_ResponseDataTableOrig[f_uiMsgId];
  CHAR *l_pTemp = static_cast<CHAR *>(f_pData);
  DataVector.assign(l_pTemp, (l_pTemp + f_uiSize));
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
}
/////////////////////////////////////////////////////////////////////////////////
/// GetRequData
/////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus C_CWORD77_Data::GetRequData(const _CWORD77_DataPoolKey &f_DataPoolKey, UI_32 &f_uiSize, PVOID &f_pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusDbRecNotFound;  // data not found in data pool
  T_CWORD77_DataPool::iterator l_iter;

  l_iter = m__CWORD77_RequestDataTable.find(f_DataPoolKey);
  if (l_iter != m__CWORD77_RequestDataTable.end()) {
    f_uiSize = static_cast<UI_32>(l_iter->second.size());
    if (f_uiSize > 0) {
      f_pData = &l_iter->second[0];
    } else {
      f_pData = NULL;
    }
    l_eStatus = eFrameworkunifiedStatusOK;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}
/////////////////////////////////////////////////////////////////////////////////
/// GetRequData
/////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus C_CWORD77_Data::GetRequData(UI_32 f_uiMsgId, UI_32 &f_uiSize, PVOID &f_pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusDbRecNotFound;  // data not found in data pool
  std::map<UI_32, std::vector<CHAR> >::iterator l_iter;

  l_iter = m__CWORD77_RequestDataTableOrig.find(f_uiMsgId);
  if (l_iter != m__CWORD77_RequestDataTableOrig.end()) {
    f_uiSize = static_cast<UI_32>(l_iter->second.size());
    if (f_uiSize > 0) {
      f_pData = &l_iter->second[0];
    } else {
      f_pData = NULL;
    }
    l_eStatus = eFrameworkunifiedStatusOK;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}
/////////////////////////////////////////////////////////////////////////////////
/// GetRespoData
/////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus C_CWORD77_Data::GetRespoData(const _CWORD77_DataPoolKey &f_DataPoolKey, UI_32 &f_uiSize, PVOID &f_pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  T_CWORD77_DataPool::iterator l_iter;
  l_iter = m__CWORD77_ResponseDataTable.find(f_DataPoolKey);
  if (l_iter != m__CWORD77_ResponseDataTable.end()) {
    f_uiSize = static_cast<UI_32>(l_iter->second.size());
    if (f_uiSize > 0) {
      /**
       * @todo
       * If *f_pData is set to NULL, a segmentation fault occurs.
       */
      f_pData = &l_iter->second[0];
    } else {
      f_pData = NULL;
    }
    l_eStatus = eFrameworkunifiedStatusOK;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  return l_eStatus;
}
/////////////////////////////////////////////////////////////////////////////////
/// GetRespoData
/////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus C_CWORD77_Data::GetRespoData(UI_32 f_uiMsgId,
                                 UI_32 &f_uiSize, PVOID &f_pData) {  // NOLINT  (readability/nolint)
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  std::map<UI_32, std::vector<CHAR> >::iterator l_iter;
  l_iter = m__CWORD77_ResponseDataTableOrig.find(f_uiMsgId);
  if (l_iter != m__CWORD77_ResponseDataTableOrig.end()) {
    f_uiSize = static_cast<UI_32>(l_iter->second.size());
    if (f_uiSize > 0) {
      f_pData = &l_iter->second[0];
    } else {
      f_pData = NULL;
    }
    l_eStatus = eFrameworkunifiedStatusOK;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}

/////////////////////////////////////////////////////////////////////////////////
/// SetReqDataIn_CWORD77_DataPool
/////////////////////////////////////////////////////////////////////////////////
VOID SetReqDataIn_CWORD77_DataPool(const _CWORD77_DataPoolKey &f_DataPoolKey,  // NOLINT  (readability/nolint)
                             UI_32 f_uiSize, PVOID f_pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  if (C_CWORD77_Data::GetInstance()) {
    C_CWORD77_Data::GetInstance()->SetRequData(f_DataPoolKey, f_uiSize, f_pData);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
}

/////////////////////////////////////////////////////////////////////////////////
/// SetRespDataIn_CWORD77_DataPool
/////////////////////////////////////////////////////////////////////////////////
VOID SetRespDataIn_CWORD77_DataPool(const _CWORD77_DataPoolKey &f_DataPoolKey,  // NOLINT  (readability/nolint)
                              UI_32 f_uiSize, const PVOID f_pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  if (C_CWORD77_Data::GetInstance()) {
    C_CWORD77_Data::GetInstance()->SetRespoData(f_DataPoolKey, f_uiSize, f_pData);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
}

/////////////////////////////////////////////////////////////////////////////////
/// SetRespNotfnDataIn_CWORD77_DataPool
/////////////////////////////////////////////////////////////////////////////////
VOID SetRespNotfnDataIn_CWORD77_DataPool(const std::string &f_cNotificationName,  // NOLINT  (readability/nolint)
                                   UI_32 f_uiSize, const PVOID f_pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  if (C_CWORD77_Data::GetInstance()) {
    // LCOV_EXCL_BR_START 11:except branch
    C_CWORD77_Data::GetInstance()->SetRespoData(std::make_pair(g_uiNotificationDataPoolId, f_cNotificationName), f_uiSize,  // LCOV_EXCL_BR_LINE 11:except branch
                                          f_pData);
    // LCOV_EXCL_BR_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
}

/////////////////////////////////////////////////////////////////////////////////
/// GetReqDataFrom_CWORD77_DataPool
/////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus GetReqDataFrom_CWORD77_DataPool(const _CWORD77_DataPoolKey &f_DataPoolKey,  // NOLINT  (readability/nolint)
                                    UI_32 &f_uiSize,  // NOLINT  (readability/nolint)
                                    PVOID &f_pData) {  // NOLINT  (readability/nolint)
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  if (C_CWORD77_Data::GetInstance()) {
    l_eStatus = C_CWORD77_Data::GetInstance()->GetRequData(f_DataPoolKey, f_uiSize, f_pData);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}

/////////////////////////////////////////////////////////////////////////////////
/// GetRespDataFrom_CWORD77_DataPool
/////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus GetRespDataFrom_CWORD77_DataPool(const _CWORD77_DataPoolKey &f_DataPoolKey,  // NOLINT  (readability/nolint)
                                      UI_32 &f_uiSize,  // NOLINT  (readability/nolint)
                                      PVOID &f_pData) {  // NOLINT  (readability/nolint)
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  if (C_CWORD77_Data::GetInstance()) {
    l_eStatus = C_CWORD77_Data::GetInstance()->GetRespoData(f_DataPoolKey, f_uiSize, f_pData);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");
  return l_eStatus;
}

/////////////////////////////////////////////////////////////////////////////////
/// GetRespDataFrom_CWORD77_DataPool
/////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus GetRespNotfnDataFrom_CWORD77_DataPool(const std::string &f_cNotificationName,  // NOLINT  (readability/nolint)
                                           UI_32 &f_uiSize,   // NOLINT  (readability/nolint)
                                           PVOID &f_pData) {  // NOLINT  (readability/nolint)
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "+");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  if (C_CWORD77_Data::GetInstance()) {
    // LCOV_EXCL_BR_START 11:except branch
    l_eStatus = C_CWORD77_Data::GetInstance()->GetRespoData(std::make_pair(g_uiNotificationDataPoolId, f_cNotificationName),
                                                      f_uiSize, f_pData);
    // LCOV_EXCL_BR_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS__CWORD77_, __FUNCTION__, "-");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  return l_eStatus;
}
