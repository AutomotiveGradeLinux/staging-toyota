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

////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_HMI_CWORD77_Controller
/// \brief
///
////////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file ns__CWORD77__data_pool_table.h
 * @brief \~english This file provides API for convert and C_CWORD77_DataPool class
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
/** @addtogroup _CWORD77_
 *  @ingroup framework_unified
 *  @{
 */
#ifndef _CWORD77__DATA_POOL_TABLE_H_  // NOLINT  (build/header_guard)
#define _CWORD77__DATA_POOL_TABLE_H_

#include <native_service/ns__CWORD77__common.h>
#include <sstream>
#include <map>
#include <string>
#include <vector>

/// Enumeration for Type of Table
typedef enum _ETableType {
  REQUEST_TABLE = 1,
  RESPONSE_TABLE
} ETableType, *PETableType;

/// Enumeration for Data Type
typedef enum _EDataType {
  UNKNOWNTYPE = -1,
  SIGNEDINT,
  UNSIGNEDINT,
  FLOAT,
  DOUBLE,
  CHARACTER,
  BOOLEAN,
  STRING,
} EDataType, *PEDataType;

/// _CWORD77_ Data structure
typedef struct __CWORD77_Data {
  EDataType DataType;
  std::string DataValue;
} _CWORD77_Data, *P_CWORD77_Data;

typedef std::map< _CWORD77_DataPoolKey, std::vector< CHAR > > T_CWORD77_DataPool;


////////////////////////////////////////////////////////////////////////////////////////////
///   NoOfElementsinData
/// Function to get number of elements for given string
///
/// \param  [in] Input
///     string - string
///
/// \return UI_32 - Number of elements
///
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 NoOfElementsinData(std::string Input);

/// Template to Convert Number to String

template <typename T>
std::string NumberToString(T Number) {
  std::ostringstream ss;
  ss << Number;
  return ss.str();
}

/// Template to convert String to Number

template <typename T>
T StringToNumber(const std::string &Text) {
  std::istringstream ss(Text);
  T result;
  ss >> result;
  return result;
}

/// Template to Convert an Array To String

template<class T>
std::string ConvertArrayToString(T *Array, int Arraysize) {
  int length = 0;
  std::string Result = "";
  while (length < Arraysize) {
    Result.append(NumberToString<T>(Array[length]));
    Result.append(",");
    length++;
  }
  Result.append("");
  return Result;
}

/// Template to Convert String To Array
template<class T>
void ConvertStringToArray(std::string Input, std::vector<T> &Array, UI_32 &Arraysize) {  // NOLINT  (readability/nolint)
  int i = 0;
  std::string first = "";
  std::istringstream ss(Input);
  int Size = NoOfElementsinData(Input);
  // T Arr[Size];
  while (!ss.eof()) {
    getline(ss, first, ',');
    if (first != "") {
      Array[i] = StringToNumber<T>(first);
      i++;
    }
  }
  Arraysize = i;
  // memcpy(Array,(void *)Arr,Arraysize*sizeof(T));
}

////////////////////////////////////////////////////////////////////////////////////////////
///   ConvertArrayStringsToString
/// Function to convert array of strings to string
///
/// \param  [in] strArr
///     string * - pointer to array of strings
/// \param  [in] Size
///     SI_32 - Number of strings
///
/// \return string
///
////////////////////////////////////////////////////////////////////////////////////////////
std::string ConvertArrayStringsToString(std::string *strArr, SI_32 Size);


////////////////////////////////////////////////////////////////////////////////////////////
///   ConvertArrayStringsToString
/// Function to convert string to array of strings
///
/// \param  [in] Input
///     string - string contains array of strings
/// \param  [out] strArr
///     string * - Pointer to Array of strings
/// \param  [out] ArraySize
///     UI_32 - No of elements in Array
///
/// \return None
///
////////////////////////////////////////////////////////////////////////////////////////////
void ConvertStringToArrString(std::string Input, std::string *strArr, UI_32 &ArraySize);  // NOLINT  (readability/nolint)

///////////////////////////////////////////////////////////////////////////
/// class: C_CWORD77_DataPool
/// Description: This is interface class to _CWORD77_ Data base.
/// It provides APIs to set and get data from _CWORD77_ Data base
///
////////////////////////////////////////////////////////////////////////////
class C_CWORD77_DataPool {
 private:
  std::map<UI_32, _CWORD77_Data> m__CWORD77_DataRequestTable;   ///< Request Data Table
  std::map<UI_32, _CWORD77_Data> m__CWORD77_DataResponseTable;  ///< Response Data Table

  static C_CWORD77_DataPool *m_p_CWORD77_DataPoolInstance;  ///< Static instance to C_CWORD77_DataPool

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetData
  /// Templatized Function to get data from _CWORD77_ Data base
  ///
  /// \param  [in] TableType
  ///     ETableType - Table Type (Request or Response)
  /// \param  [in] VarName
  ///     string  - Variable Name or key value
  /// \param  [out] Array
  ///     T * - Pointer to an array of given type
  /// \param  [out] ArraySize
  ///     UI_32 - No of elements of Array
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////

  template<class T>
  void GetData(ETableType TableType, UI_32 VarName, T *Array, UI_32 &ArraySize) {  // NOLINT  (readability/nolint)
    EDataType DataType = SIGNEDINT;
    std::string DataValue;
    GetValueFromMap(TableType, VarName, DataType, DataValue);
    UI_32 Num = NoOfElementsinData(DataValue);
    std::vector<T> Temp;
    Temp.resize(Num);
    ConvertStringToArray<T>(DataValue, Temp, ArraySize);
    for (unsigned int i = 0; i < Num; i++) {
      Array[i] = Temp[i];
    }
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetData
  /// Templatized Function to set data into _CWORD77_ Data base
  ///
  /// \param  [in] TableType
  ///     ETableType - Table Type (Request or Response)
  /// \param  [in] VarName
  ///     string  - Variable Name or key value
  /// \param  [in] DataType
  ///     EDataType  - Data Type of key
  /// \param  [in] Array
  ///     T * - Pointer to an array of given type
  /// \param  [in] ArraySize
  ///     UI_32 - No of elements of Array
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  template<class T>
  void SetData(ETableType TableType, UI_32 VarName, EDataType DataType, T *Array, UI_32 ArraySize) {
    std::string Result = ConvertArrayToString(Array, ArraySize);
    SetValueinMap(TableType, VarName, DataType, Result);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetValueFromMap
  /// Function to get data from _CWORD77_ map
  ///
  /// \param  [in] TableType
  ///     ETableType - Table Type (Request or Response)
  /// \param  [in] VarName
  ///     string  - Variable Name or key value
  /// \param  [out] DataType
  ///     EDataType  - Data Type of key
  /// \param  [out] Strvalue
  ///     string - Output in the form of string
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  void GetValueFromMap(ETableType TableType, UI_32 VarName, EDataType &DataType, std::string &Strvalue);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetValueinMap
  /// Function to set data in _CWORD77_ map
  ///
  /// \param  [in] TableType
  ///     ETableType - Table Type (Request or Response)
  /// \param  [in] VarName
  ///     string  - Variable Name or key value
  /// \param  [in] VarType
  ///     EDataType  - Data Type of key
  /// \param  [in] Result
  ///     string - input in the form of string
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  void SetValueinMap(ETableType TableType, UI_32 VarName, EDataType VarType, std::string Result);

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetArrayStringData
  /// Function to set string data in _CWORD77_ map
  ///
  /// \param  [in] TableType
  ///     ETableType - Table Type (Request or Response)
  /// \param  [in] VarName
  ///     string  - Variable Name or key value
  /// \param  [in] VarType
  ///     EDataType  - Data Type of key
  /// \param  [in] DataValue
  ///     string [] - Array of strings
  /// \param  [in] Size
  ///     UI_32  - No of elements in Array
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  void SetArrayStringData(ETableType TableType,
                          UI_32 VarName,
                          EDataType VarType,
                          std::string DataValue[],
                          UI_32 size);
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetArrayStringData
  /// Function to get string data in _CWORD77_ map
  ///
  /// \param  [in] TableType
  ///     ETableType - Table Type (Request or Response)
  /// \param  [in] VarName
  ///     UI_32  - key
  /// \param  [out] DataValue
  ///     string [] - Array of strings
  /// \param  [out] Size
  ///     UI_32  - No of elements in Array
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  void GetArrayStringData(ETableType TableType, UI_32 VarName, std::string DataValue[], UI_32 &size);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetNoofElementsofData
  /// Function to get Number of elements in data item
  ///
  /// \param  [in] TableType
  ///     ETableType - Table Type (Request or Response)
  /// \param  [in] VarName
  ///     UI_32  - key

  ///
  /// \return UI_32 - Number of elements
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  UI_32 GetNoofElementsofData(ETableType TableType, UI_32 Key);
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetDataType
  /// Function to get data type of given key Name
  ///
  /// \param  [in] TableType
  ///     ETableType - Table Type (Request or Response)
  /// \param  [in] VarName
  ///     UI_32  - key

  ///
  /// \return EDataType - Data type of variable
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////

  EDataType GetDataType(ETableType TableType, UI_32 Key);

 public:
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   C_CWORD77_DataPool
  /// Constructor for C_CWORD77_DataPool
  /// \param  None
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  C_CWORD77_DataPool();
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   ~C_CWORD77_DataPool
  /// Destructor for C_CWORD77_DataPool
  /// \param  None
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  ~C_CWORD77_DataPool();
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetInstance
  /// API to get single instance of C_CWORD77_DataPool
  /// \param  None
  /// \return Pointer to single instance of C_CWORD77_Controller
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  static C_CWORD77_DataPool *GetInstance();
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   DeleteInstance
  /// API to delete single instance of C_CWORD77_DataPool
  /// \param  None
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  static void DeleteInstance();

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetRequestArrayData
  /// Templatized Function to set array data into Request _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  /// \param  [in] DataType
  ///     EDataType  - Data Type of key
  /// \param  [in] Array
  ///     T * - Pointer to an array of given type
  /// \param  [in] ArraySize
  ///     UI_32 - No of elements of Array
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  template<class T>
  void SetReqArrayData(UI_32 VarName, EDataType DataType, T *Array, UI_32 ArraySize) {
    SetData<T>(REQUEST_TABLE, VarName, DataType, Array, ArraySize);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetRequestArrayData
  /// Templatized Function to set array data into Response _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  /// \param  [in] DataType
  ///     EDataType  - Data Type of key
  /// \param  [in] Array
  ///     T * - Pointer to an array of given type
  /// \param  [in] ArraySize
  ///     UI_32 - No of elements of Array
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  template<class T>
  void SetRespArrayData(UI_32 VarName, EDataType DataType, T *Array, UI_32 ArraySize) {
    SetData<T>(RESPONSE_TABLE, VarName, DataType, Array, ArraySize);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetRequestArrayData
  /// Templatized Function to get array data from Request _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  /// \param  [out] Array
  ///     T * - Pointer to an array of given type
  /// \param  [out] ArraySize
  ///     UI_32 - No of elements of Array
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  template<class T>
  void GetReqArrayData(UI_32 VarName, T *Array, UI_32 &ArraySize) {  // NOLINT  (readability/nolint)
    GetData<T>(REQUEST_TABLE, VarName, Array, ArraySize);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetRequestArrayData
  /// Templatized Function to get array data from Response _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  /// \param  [out] Array
  ///     T * - Pointer to an array of given type
  /// \param  [out] ArraySize
  ///     UI_32 - No of elements of Array
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  template<class T>
  void GetRespArrayData(UI_32 VarName, T *Array, UI_32 &ArraySize) {  // NOLINT  (readability/nolint)
    GetData<T>(RESPONSE_TABLE, VarName, Array, ArraySize);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetRequestData
  /// Templatized Function to set data into Request _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  /// \param  [in] DataType
  ///     EDataType  - Data Type of key
  /// \param  [in] Array
  ///     T  - Data type of key
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  template<class T>
  void SetReqData(UI_32 VarName, EDataType DataType, T Array) {
    SetData<T>(REQUEST_TABLE, VarName, DataType, &Array, 1);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetRequestData
  /// Templatized Function to set data into Response _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  /// \param  [in] DataType
  ///     EDataType  - Data Type of key
  /// \param  [in] Array
  ///     T  - Data type of key
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  template<class T>
  void SetRespData(UI_32 VarName, EDataType DataType, T Array) {
    SetData<T>(RESPONSE_TABLE, VarName, DataType, &Array, 1);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetRequestData
  /// Templatized Function to get data from Resquest _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  ///
  /// \return T - value belong to type T
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  template<class T>
  T GetReqData(UI_32 VarName) {
    T Data;
    UI_32 ArraySize = 0;
    GetData<T>(REQUEST_TABLE, VarName, &Data, ArraySize);
    return Data;
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetResponseData
  /// Templatized Function to get data from Response _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  ///
  /// \return T - value belong to type T
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  template<class T>
  T GetRespData(UI_32 VarName) {
    T Data;
    UI_32 ArraySize = 0;
    GetData<T>(RESPONSE_TABLE, VarName, &Data, ArraySize);
    return Data;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetRequestArrayStringData
  /// API to set array of string data into Request _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  /// \param  [in] DataType
  ///     EDataType  - Data Type of key
  /// \param  [in] DataValue
  ///     string [] - array of strings
  /// \param  [in] ArraySize
  ///     UI_32 - No of elements of Array
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////

  void SetReqArrayStringData(UI_32 VarName, EDataType VarType,  std::string DataValue[], UI_32 size) {
    SetArrayStringData(REQUEST_TABLE, VarName, VarType,  DataValue, size);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetRequestArrayStringData
  /// API to set array of string data into Response _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  /// \param  [in] DataType
  ///     EDataType  - Data Type of key
  /// \param  [in] DataValue
  ///     string [] - array of strings
  /// \param  [in] ArraySize
  ///     UI_32 - No of elements of Array
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  void SetRespArrayStringData(UI_32 VarName, EDataType VarType,  std::string DataValue[], UI_32 size) {
    SetArrayStringData(RESPONSE_TABLE, VarName, VarType,  DataValue, size);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetRequestArrayStringData
  /// API to get array of string data from Request _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  /// \param  [out] DataValue
  ///     string [] - array of strings
  /// \param  [out] ArraySize
  ///     UI_32 - No of elements of Array
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////

  void GetReqArrayStringData(UI_32 VarName, std::string DataValue[], UI_32 &size) {  // NOLINT  (readability/nolint)
    GetArrayStringData(REQUEST_TABLE, VarName, DataValue, size);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetResponseArrayStringData
  /// API to get array of string data from Response _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  /// \param  [out] DataValue
  ///     string [] - array of strings
  /// \param  [out] ArraySize
  ///     UI_32 - No of elements of Array
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  void GetRespArrayStringData(UI_32 VarName, std::string DataValue[], UI_32 &size) {  // NOLINT  (readability/nolint)
    GetArrayStringData(RESPONSE_TABLE, VarName, DataValue, size);
  }


  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetRequestStringData
  /// API to set string data into Request _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  /// \param  [in] DataType
  ///     EDataType  - Data Type of key
  /// \param  [in] DataValue
  ///     string [] - array of strings
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  void SetReqStringData(UI_32 VarName, EDataType VarType,  std::string DataValue) {
    UI_32 size = 1;
    SetArrayStringData(REQUEST_TABLE, VarName, VarType,  &DataValue, size);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetResponseStringData
  /// API to set string data into Response _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  /// \param  [in] DataType
  ///     EDataType  - Data Type of key
  /// \param  [in] DataValue
  ///     string [] - array of strings
  ///
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  void SetRespStringData(UI_32 VarName, EDataType VarType,  std::string DataValue) {
    UI_32 size = 1;
    SetArrayStringData(RESPONSE_TABLE, VarName, VarType, &DataValue, size);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetRequestStringData
  /// API to get string data from Request _CWORD77_ Data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  ///
  /// \return string - value of data
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  std::string GetReqStringData(UI_32 VarName) {
    UI_32 size = 0;
    std::string DataValue = "";
    GetArrayStringData(REQUEST_TABLE, VarName, &DataValue, size);
    return DataValue;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetNoofElementsFromRequestTable
  /// API to get No of elements in a key from request data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  ///
  /// \return UI_32 - No of elements
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  UI_32 GetReqDataSize(UI_32 Key) {
    return GetNoofElementsofData(REQUEST_TABLE, Key);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetNoofElementsFromResponseTable
  /// API to get No of elements in a key from response data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  ///
  /// \return UI_32 - No of elements
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////

  UI_32 GetRespDataSize(UI_32 Key) {
    return GetNoofElementsofData(RESPONSE_TABLE, Key);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetDataTypeFromRequestTable
  /// API to get data type of key from request data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  ///
  /// \return EDataType - Data type of key
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////

  EDataType GetReqDataType(UI_32 Key) {
    return GetDataType(REQUEST_TABLE, Key);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetDataTypeFromResponseTable
  /// API to get data type of key from response data base
  ///
  /// \param  [in] VarName
  ///     UI_32  - key
  ///
  /// \return EDataType - Data type of key
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EDataType GetRespDataType(UI_32 Key) {
    return GetDataType(RESPONSE_TABLE, Key);
  }
};

extern C_CWORD77_DataPool g__CWORD77_DataPool;


////////////////////////////////////////////////////////////////////////////////////////////
///   SetRequestArrayData
/// Templatized Function to set array data into Response _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
/// \param  [in] DataType
///     EDataType  - Data Type of key
/// \param  [in] Array
///     T * - Pointer to an array of given type
/// \param  [in] ArraySize
///     UI_32 - No of elements of Array
///
/// \return None
///
////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
void SetRequestArrayData(UI_32 VarName, EDataType DataType, T *Array, UI_32 ArraySize) {
  g__CWORD77_DataPool.SetReqArrayData(VarName, DataType, Array, ArraySize);
}


////////////////////////////////////////////////////////////////////////////////////////////
///   SetRequestArrayData
/// Templatized Function to set array data into Response _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
/// \param  [in] DataType
///     EDataType  - Data Type of key
/// \param  [in] Array
///     T * - Pointer to an array of given type
/// \param  [in] ArraySize
///     UI_32 - No of elements of Array
///
/// \return None
///
////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
void SetResponseArrayData(UI_32 VarName, EDataType DataType, T *Array, UI_32 ArraySize) {
  g__CWORD77_DataPool.SetRespArrayData<T>(VarName, DataType, Array, ArraySize);
}
////////////////////////////////////////////////////////////////////////////////////////////
///   GetRequestArrayData
/// Templatized Function to get array data from Request _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
/// \param  [out] Array
///     T * - Pointer to an array of given type
/// \param  [out] ArraySize
///     UI_32 - No of elements of Array
///
/// \return None
///
////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
void GetRequestArrayData(UI_32 VarName, T *Array, UI_32 &ArraySize) {  // NOLINT  (readability/nolint)
  g__CWORD77_DataPool.GetReqArrayData<T>(VarName, Array, ArraySize);
}
////////////////////////////////////////////////////////////////////////////////////////////
///   GetRequestArrayData
/// Templatized Function to get array data from Response _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
/// \param  [out] Array
///     T * - Pointer to an array of given type
/// \param  [out] ArraySize
///     UI_32 - No of elements of Array
///
/// \return None
///
////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
void GetResponseArrayData(UI_32 VarName, T *Array, UI_32 &ArraySize) {  // NOLINT  (readability/nolint)
  g__CWORD77_DataPool.GetRespArrayData<T>(VarName, Array, ArraySize);
}

////////////////////////////////////////////////////////////////////////////////////////////
///   SetRequestData
/// Templatized Function to set data into Request _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
/// \param  [in] DataType
///     EDataType  - Data Type of key
/// \param  [in] Array
///     T  - Data type of key
///
/// \return None
///
////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
void SetRequestData(UI_32 VarName, EDataType DataType, T Array) {
  g__CWORD77_DataPool.SetReqData<T>(VarName, DataType, Array);
}
////////////////////////////////////////////////////////////////////////////////////////////
///   SetRequestData
/// Templatized Function to set data into Response _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
/// \param  [in] DataType
///     EDataType  - Data Type of key
/// \param  [in] Array
///     T  - Data type of key
///
/// \return None
///
////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
void SetResponseData(UI_32 VarName, EDataType DataType, T Array) {
  g__CWORD77_DataPool.SetRespData<T>(VarName, DataType, Array);
}
////////////////////////////////////////////////////////////////////////////////////////////
///   GetRequestData
/// Templatized Function to get data from Resquest _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
///
/// \return T - value belong to type T
///
////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
T GetRequestData(UI_32 VarName) {
  return g__CWORD77_DataPool.GetReqData<T>(VarName);
}
////////////////////////////////////////////////////////////////////////////////////////////
///   GetResponseData
/// Templatized Function to get data from Response _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
///
/// \return T - value belong to type T
///
////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
T GetResponseData(UI_32 VarName) {
  return g__CWORD77_DataPool.GetRespData<T>(VarName);
}

////////////////////////////////////////////////////////////////////////////////////////////
///   SetRequestArrayStringData
/// API to set array of string data into Request _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
/// \param  [in] DataType
///     EDataType  - Data Type of key
/// \param  [in] DataValue
///     string [] - array of strings
/// \param  [in] ArraySize
///     UI_32 - No of elements of Array
///
/// \return None
///
////////////////////////////////////////////////////////////////////////////////////////////

void SetRequestArrayStringData(UI_32 VarName, EDataType VarType,  std::string DataValue[], UI_32 size);


////////////////////////////////////////////////////////////////////////////////////////////
///   SetRequestArrayStringData
/// API to set array of string data into Response _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
/// \param  [in] DataType
///     EDataType  - Data Type of key
/// \param  [in] DataValue
///     string [] - array of strings
/// \param  [in] ArraySize
///     UI_32 - No of elements of Array
///
/// \return None
///
////////////////////////////////////////////////////////////////////////////////////////////
void SetResponseArrayStringData(UI_32 VarName, EDataType VarType,  std::string DataValue[], UI_32 size);

////////////////////////////////////////////////////////////////////////////////////////////
///   GetRequestArrayStringData
/// API to get array of string data from Request _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
/// \param  [out] DataValue
///     string [] - array of strings
/// \param  [out] ArraySize
///     UI_32 - No of elements of Array
///
/// \return None
///
////////////////////////////////////////////////////////////////////////////////////////////

void GetRequestArrayStringData(UI_32 VarName, std::string DataValue[], UI_32 &size);  // NOLINT  (readability/nolint)


////////////////////////////////////////////////////////////////////////////////////////////
///   GetResponseArrayStringData
/// API to get array of string data from Response _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
/// \param  [out] DataValue
///     string [] - array of strings
/// \param  [out] ArraySize
///     UI_32 - No of elements of Array
///
/// \return None
///
////////////////////////////////////////////////////////////////////////////////////////////
void GetResponseArrayStringData(UI_32 VarName, std::string DataValue[], UI_32 &size);  // NOLINT  (readability/nolint)


////////////////////////////////////////////////////////////////////////////////////////////
///   SetRequestStringData
/// API to set string data into Request _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
/// \param  [in] DataType
///     EDataType  - Data Type of key
/// \param  [in] DataValue
///     string [] - array of strings
///
/// \return None
///
////////////////////////////////////////////////////////////////////////////////////////////
void SetRequestStringData(UI_32 VarName, EDataType VarType,  std::string DataValue);

////////////////////////////////////////////////////////////////////////////////////////////
///   SetResponseStringData
/// API to set string data into Response _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
/// \param  [in] DataType
///     EDataType  - Data Type of key
/// \param  [in] DataValue
///     string [] - array of strings
///
/// \return None
///
////////////////////////////////////////////////////////////////////////////////////////////
void SetResponseStringData(UI_32 VarName, EDataType VarType,  std::string DataValue);

////////////////////////////////////////////////////////////////////////////////////////////
///   GetRequestStringData
/// API to get string data from Request _CWORD77_ Data base
///
/// \param  [in] VarName
///     UI_32  - key
///
/// \return string - value of data
///
////////////////////////////////////////////////////////////////////////////////////////////
std::string GetRequestStringData(UI_32 VarName);

////////////////////////////////////////////////////////////////////////////////////////////
///   GetRequestDataLength
/// API to get No of elements in a key from request data base
///
/// \param  [in] VarName
///     UI_32  - key
///
/// \return UI_32 - No of elements
///
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 GetRequestDataLength(UI_32 Key);

////////////////////////////////////////////////////////////////////////////////////////////
///   GetResponseDataLength
/// API to get No of elements in a key from response data base
///
/// \param  [in] VarName
///     UI_32  - key
///
/// \return UI_32 - No of elements
///
////////////////////////////////////////////////////////////////////////////////////////////

UI_32 GetResponseDataLength(UI_32 Key);

////////////////////////////////////////////////////////////////////////////////////////////
///   GetDataTypeFromRequestTable
/// API to get data type of key from request data base
///
/// \param  [in] VarName
///     UI_32  - key
///
/// \return EDataType - Data type of key
///
////////////////////////////////////////////////////////////////////////////////////////////

EDataType GetRequestDataType(UI_32 Key);

////////////////////////////////////////////////////////////////////////////////////////////
///   GetDataTypeFromResponseTable
/// API to get data type of key from response data base
///
/// \param  [in] VarName
///     UI_32  - key
///
/// \return EDataType - Data type of key
///
////////////////////////////////////////////////////////////////////////////////////////////
EDataType GetResponseDataType(UI_32 Key);



class C_CWORD77_Data {
 private:
  T_CWORD77_DataPool m__CWORD77_RequestDataTable;   ///< Request Data Table
  T_CWORD77_DataPool m__CWORD77_ResponseDataTable;    ///< Response Data Table

  // Original data pool
  // Depricated. Remove once all modules migrates to new API's for accessing data pool
  std::map< UI_32, std::vector<CHAR> > m__CWORD77_RequestDataTableOrig;   ///< Request Data Table
  std::map< UI_32, std::vector<CHAR> > m__CWORD77_ResponseDataTableOrig;  ///< Response Data Table

  static C_CWORD77_Data *m_p_CWORD77_DataInstance;          /// instance to C_CWORD77_Data

 public:
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   C_CWORD77_Data
  /// Constructor for C_CWORD77_Data
  /// \param  None
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  C_CWORD77_Data();
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   C_CWORD77_Data
  /// Destructor for C_CWORD77_Data
  /// \param  None
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  ~C_CWORD77_Data();
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetInstance
  /// API to get single instance of C_CWORD77_Data
  /// \param  None
  /// \return Pointer to single instance of C_CWORD77_Data
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  static C_CWORD77_Data *GetInstance();
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   DeleteInstance
  /// API to delete single instance of C_CWORD77_DataPool
  /// \param  None
  /// \return None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  static void DeleteInstance();
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetReqDataIn_CWORD77_DataPool
  /// API to set data associated with request into _CWORD77_ Data Pool
  ///
  /// \param  [in] f_DataPoolKey
  ///     _CWORD77_DataPoolKey  - Data pool key
  /// \param  [in] f_uiSize
  ///     UI_32  - size of the data
  /// \param  [in] f_pData
  ///     PVOID  - void pointer to data
  ///
  /// \return VOID - None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetRequData(const _CWORD77_DataPoolKey &f_DataPoolKey, UI_32 f_uiSize, PVOID f_pData);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetReqDataIn_CWORD77_DataPool
  /// API to set data associated with request into _CWORD77_ Data Pool
  /// Depricated. Do not use.
  ///
  /// \param  [in] f_uiMsgId
  ///     UI_32  - MsgId
  /// \param  [in] f_uiSize
  ///     UI_32  - size of the data
  /// \param  [in] f_pData
  ///     PVOID  - void pointer to data
  ///
  /// \return VOID - None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetRequData(UI_32 f_uiMsgId, UI_32 f_uiSize, PVOID f_pData);
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetRespDataIn_CWORD77_DataPool
  /// API to set data associated with response into _CWORD77_ Data Pool
  ///
  /// \param  [in] f_DataPoolKey
  ///     _CWORD77_DataPoolKey  - Data pool key
  /// \param  [in] f_uiSize
  ///     UI_32  - size of the data
  /// \param  [in] f_pData
  ///     PVOID  - void pointer to data
  ///
  /// \return VOID - None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetRespoData(const _CWORD77_DataPoolKey &f_DataPoolKey,  // NOLINT  (readability/nolint)
                    UI_32 f_uiSize, const PVOID f_pData);

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   SetRespDataIn_CWORD77_DataPool
  /// API to set data associated with response into _CWORD77_ Data Pool
  /// Depricated. Do not use.
  ///
  /// \param  [in] f_uiMsgId
  ///     UI_32  - MsgId
  /// \param  [in] f_uiSize
  ///     UI_32  - size of the data
  /// \param  [in] f_pData
  ///     PVOID  - void pointer to data
  ///
  /// \return VOID - None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  VOID SetRespoData(UI_32 f_uiMsgId, UI_32 f_uiSize, const PVOID f_pData);  // NOLINT  (readability/nolint)
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetReqDataIn_CWORD77_DataPool
  /// API to get data associated with request from _CWORD77_ Data Pool
  ///
  /// \param  [in] f_DataPoolKey
  ///     _CWORD77_DataPoolKey  - Data pool key
  /// \param  [in] f_uiSize
  ///     UI_32  - size of the data
  /// \param  [in] f_pData
  ///     PVOID  - void pointer to data
  ///
  /// \return EFrameworkunifiedStatus - Succes or Error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetRequData(const _CWORD77_DataPoolKey &f_DataPoolKey, UI_32 &f_uiSize,  // NOLINT  (readability/nolint)
                         PVOID &f_pData);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetReqDataIn_CWORD77_DataPool
  /// API to get data associated with request from _CWORD77_ Data Pool
  /// Depricated. Do not use.
  ///
  /// \param  [in] f_uiMsgId
  ///     UI_32  - MsgId
  /// \param  [in] f_uiSize
  ///     UI_32  - size of the data
  /// \param  [in] f_pData
  ///     PVOID  - void pointer to data
  ///
  /// \return EFrameworkunifiedStatus - Succes or Error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetRequData(UI_32 f_uiMsgId, UI_32 &f_uiSize, PVOID &f_pData);  // NOLINT  (readability/nolint)
  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetRespDataFrom_CWORD77_DataPool
  /// API to get data associated with response from _CWORD77_ Data Pool
  ///
  /// \param  [in] f_DataPoolKey
  ///     _CWORD77_DataPoolKey  - Data pool key
  /// \param  [in] f_uiSize
  ///     UI_32  - size of the data
  /// \param  [in] f_pData
  ///     PVOID  - void pointer to data
  ///
  /// \return EFrameworkunifiedStatus - Succes or Error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetRespoData(const _CWORD77_DataPoolKey &f_DataPoolKey,  // NOLINT  (readability/nolint)
                          UI_32 &f_uiSize,  // NOLINT  (readability/nolint)
                          PVOID &f_pData);  // NOLINT  (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////
  ///   GetRespDataFrom_CWORD77_DataPool
  /// API to get data associated with response from _CWORD77_ Data Pool
  /// Depricated. Do not use.
  ///
  /// \param  [in] f_uiMsgId
  ///     UI_32  - MsgId
  /// \param  [in] f_uiSize
  ///     UI_32  - size of the data
  /// \param  [in] f_pData
  ///     PVOID  - void pointer to data
  ///
  /// \return EFrameworkunifiedStatus - Succes or Error
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetRespoData(UI_32 f_uiMsgId, UI_32 &f_uiSize, PVOID &f_pData);  // NOLINT  (readability/nolint)
};

#endif /* _CWORD77__DATA_POOL_TABLE_H_ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
