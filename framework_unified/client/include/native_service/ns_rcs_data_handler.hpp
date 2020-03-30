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

/**
 * @file
 * @brief \~english This file contains declaration of CPassThruInDataHandler class
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

// todo: move this file to the rcs interface folder

#ifndef _NS_RCS_DATA_HANDLER_H__  // NOLINT  (build/header_guard)
#define _NS_RCS_DATA_HANDLER_H__

#include <native_service/frameworkunified_types.h>
#include <native_service/ns_endianess.h>
#include <vector>

#ifdef AGL_STUB
#include <cstring>
#endif

typedef UI_8 NSRCS_BOOL;

#define SIZE_OF_PASSTHRU_DATATYPE_IDENTIFIER 1
#define SIZE_OF_PASSTHRU_PARAM_BUFFERTYPE_HEADER 3
#define PASSTHRU_DATA_HEADER_LEN 8


// not valid for buffer or array
// \return - 0 - invalid type id
/*UI_16 GetLengthOfBasicType(UI_8 f_ui8TypeId)
{

  UI_16 l_ui16LengthOfBasicType = 0;
  switch(f_ui8TypeId)
  {
  case 0x01:  l_ui16LengthOfBasicType = 2; break;  // eBool
  case 0x02:  l_ui16LengthOfBasicType = 1; break;  // eUint8
  case 0x03:  l_ui16LengthOfBasicType = 2; break;  // eUint16
  case 0x04:  l_ui16LengthOfBasicType = 4; break;  // eUint32
  case 0x05:  l_ui16LengthOfBasicType = 8; break;  // eUint64
  case 0x06:  l_ui16LengthOfBasicType = 1; break;  // eSint8
  case 0x07:  l_ui16LengthOfBasicType = 2; break;  // eSint16
  case 0x08:  l_ui16LengthOfBasicType = 4; break;  // eSint32
  case 0x09:  l_ui16LengthOfBasicType = 8; break;  // eSint64
  case 0x0A:  l_ui16LengthOfBasicType = 8; break;  // eReal
  default: break;
  }
  return l_ui16LengthOfBasicType;
}*/

/////////////////////////////////////////
/// Class: CPassThruInDataHandler
/////////////////////////////////////////
/**
 * @class CPassThruInDataHandler
 * \~english @brief Data Handler
 * \~english @par   Brief Introduction
 *        Class to get/set function to do input data handler
 *
 */
class CPassThruInDataHandler {
 public:
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CPassThruInDataHandler
  /// \~english @par Summary
  ///       Constructor of CPassThruInDataHandler class.
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       None
  /// \~english @par Detail
  ///       To generate a CPassThruInDataHandler class. \n
  ///       Member variables (m_ui8Data, m_ui16TotalLength, m_ui16ParamCount, m_pui8CurOffset, m_ui16RemainingLength) to
  ///       be initialized. \n
  /// \~english @see  ~CPassThruInDataHandler
  ////////////////////////////////////////////////////////////////////////////////////
  CPassThruInDataHandler(UI_8 *f_pui8Data, UI_16 f_ui16Length, UI_16 f_ui16ParamCount)
    : m_ui8Data(f_pui8Data),
      m_ui16TotalLength(f_ui16Length),
      m_ui16ParamCount(f_ui16ParamCount),
      m_pui8CurOffset(f_pui8Data),
      m_ui16RemainingLength(f_ui16Length) {

  }

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~CPassThruInDataHandler
  /// \~english @par Summary
  ///       Destructor of CPassThruInDataHandler class.
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       None
  /// \~english @par Detail
  ///       To delete a CPassThruInDataHandler class. \n
  /// \~english @see  CPassThruInDataHandler
  ////////////////////////////////////////////////////////////////////////////////////
  ~CPassThruInDataHandler() {
    m_ui8Data = NULL;  // Do not delete. Because, not allocated in this class
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetBool
  /// \~english @par Brief
  ///     This function is used to convert value to type NSRCS_BOOL
  /// \~english @param [out] f_rcsboolOutValue
  ///     NSRCS_BOOL & - the given type of covert result
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - invalid data type id passed
  /// \~english @retval eFrameworkunifiedStatusFail - no sufficient data is available
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - Data of value(m_pui8CurOffset) is invalid [eFrameworkunifiedStatusInvldParam]
  ///      - Covert value(m_pui8CurOffset) failed [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Convert member data to the type NSRCS_BOOL(f_rcsboolOutValue).
  ///       Return the value(type EFrameworkunifiedStatus) indicate the operation result.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetBool(NSRCS_BOOL &f_rcsboolOutValue) {
    return GetValue<NSRCS_BOOL>(f_rcsboolOutValue);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetUI8
  /// \~english @par Brief
  ///     This function is used to convert value to type UI_8
  /// \~english @param [out] f_rcsboolOutValue
  ///     UI_8 & - the given type of covert result
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - invalid data type id passed
  /// \~english @retval eFrameworkunifiedStatusFail - no sufficient data is available
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - Data of value(m_pui8CurOffset) is invalid [eFrameworkunifiedStatusInvldParam]
  ///      - Covert value(m_pui8CurOffset) failed [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Convert member data to the type UI_8(f_ui8OutValue).
  ///       Return the value(type EFrameworkunifiedStatus) indicate the operation result.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetUI8(UI_8 &f_ui8OutValue) {
    return GetValue<UI_8>(f_ui8OutValue);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetUI16
  /// \~english @par Brief
  ///     This function is used to convert value to type UI_16
  /// \~english @param [out] f_rcsboolOutValue
  ///     UI_16 & - the given type of covert result
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - invalid data type id passed
  /// \~english @retval eFrameworkunifiedStatusFail - no sufficient data is available
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - Data of value(m_pui8CurOffset) is invalid [eFrameworkunifiedStatusInvldParam]
  ///      - Covert value(m_pui8CurOffset) failed [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Convert member data to the type UI_16(f_ui16OutValue).
  ///       Return the value(type EFrameworkunifiedStatus) indicate the operation result.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetUI16(UI_16 &f_ui16OutValue) {
    EFrameworkunifiedStatus l_eStatus = GetValue<UI_16>(f_ui16OutValue);
    f_ui16OutValue = ConvertEndian_UI16(f_ui16OutValue);
    return l_eStatus;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetUI32
  /// \~english @par Brief
  ///     This function is used to convert value to type UI_32
  /// \~english @param [out] f_rcsboolOutValue
  ///     UI_32 & - the given type of covert result
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - invalid data type id passed
  /// \~english @retval eFrameworkunifiedStatusFail - no sufficient data is available
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - Data of value(m_pui8CurOffset) is invalid [eFrameworkunifiedStatusInvldParam]
  ///      - Covert value(m_pui8CurOffset) failed [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Convert member data to the type UI_32(f_ui32OutValue).
  ///       Return the value(type EFrameworkunifiedStatus) indicate the operation result.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetUI32(UI_32 &f_ui32OutValue) {
    EFrameworkunifiedStatus l_eStatus = GetValue<UI_32>(f_ui32OutValue);
    f_ui32OutValue = ConvertEndian_UI32(f_ui32OutValue);
    return l_eStatus;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetUI64
  /// \~english @par Brief
  ///     This function is used to convert value to type UI_64
  /// \~english @param [out] f_rcsboolOutValue
  ///     UI_64 & - the given type of covert result
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - invalid data type id passed
  /// \~english @retval eFrameworkunifiedStatusFail - no sufficient data is available
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - Data of value(m_pui8CurOffset) is invalid [eFrameworkunifiedStatusInvldParam]
  ///      - Covert value(m_pui8CurOffset) failed [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Convert member data to the type UI_64(f_ui64OutValue).
  ///       Return the value(type EFrameworkunifiedStatus) indicate the operation result.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetUI64(UI_64 &f_ui64OutValue) {
    EFrameworkunifiedStatus l_eStatus = GetValue<UI_64>(f_ui64OutValue);
    f_ui64OutValue = ConvertEndian_UI64(f_ui64OutValue);
    return l_eStatus;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetSI8
  /// \~english @par Brief
  ///     This function is used to convert value to type SI_8
  /// \~english @param [out] f_rcsboolOutValue
  ///     SI_8 & - the given type of covert result
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - invalid data type id passed
  /// \~english @retval eFrameworkunifiedStatusFail - no sufficient data is available
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - Data of value(m_pui8CurOffset) is invalid [eFrameworkunifiedStatusInvldParam]
  ///      - Covert value(m_pui8CurOffset) failed [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Convert member data to the type SI_8(f_si8OutValue).
  ///       Return the value(type EFrameworkunifiedStatus) indicate the operation result.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetSI8(SI_8 &f_si8OutValue) {
    return GetValue<SI_8>(f_si8OutValue);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetSI16
  /// \~english @par Brief
  ///     This function is used to convert value to type SI_16
  /// \~english @param [out] f_rcsboolOutValue
  ///     SI_16 & - the given type of covert result
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - invalid data type id passed
  /// \~english @retval eFrameworkunifiedStatusFail - no sufficient data is available
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - Data of value(m_pui8CurOffset) is invalid [eFrameworkunifiedStatusInvldParam]
  ///      - Covert value(m_pui8CurOffset) failed [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Convert member data to the type SI_16(f_si16OutValue).
  ///       Return the value(type EFrameworkunifiedStatus) indicate the operation result.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetSI16(SI_16 &f_si16OutValue) {
    EFrameworkunifiedStatus l_eStatus = GetValue<SI_16>(f_si16OutValue);
    f_si16OutValue = ConvertEndian_SI16(f_si16OutValue);
    return l_eStatus;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetSI32
  /// \~english @par Brief
  ///     This function is used to convert value to type SI_32
  /// \~english @param [out] f_rcsboolOutValue
  ///     SI_32 & - the given type of covert result
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - invalid data type id passed
  /// \~english @retval eFrameworkunifiedStatusFail - no sufficient data is available
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - Data of value(m_pui8CurOffset) is invalid [eFrameworkunifiedStatusInvldParam]
  ///      - Covert value(m_pui8CurOffset) failed [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Convert member data to the type SI_32(f_si32OutValue).
  ///       Return the value(type EFrameworkunifiedStatus) indicate the operation result.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetSI32(SI_32 &f_si32OutValue) {
    EFrameworkunifiedStatus l_eStatus = GetValue<SI_32>(f_si32OutValue);
    f_si32OutValue = ConvertEndian_SI32(f_si32OutValue);
    return l_eStatus;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetSI64
  /// \~english @par Brief
  ///     This function is used to convert value to type SI_64
  /// \~english @param [out] f_rcsboolOutValue
  ///     SI_64 & - the given type of covert result
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - invalid data type id passed
  /// \~english @retval eFrameworkunifiedStatusFail - no sufficient data is available
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - Data of value(m_pui8CurOffset) is invalid [eFrameworkunifiedStatusInvldParam]
  ///      - Covert value(m_pui8CurOffset) failed [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Convert member data to the type SI_64(f_si64OutValue).
  ///       Return the value(type EFrameworkunifiedStatus) indicate the operation result.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetSI64(SI_64 &f_si64OutValue) {
    EFrameworkunifiedStatus l_eStatus = GetValue<SI_64>(f_si64OutValue);
    f_si64OutValue = ConvertEndian_SI64(f_si64OutValue);
    return l_eStatus;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetBuffer
  /// \~english @par Brief
  ///     This function is used to get the buffer and its size.
  /// \~english @param [out] f_pui8Buffer
  ///     UI_8 *& - buffer address
  /// \~english @param [out] f_ui16BufLength
  ///     UI_16 - buffer size
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - invalid data type id passed
  /// \~english @retval eFrameworkunifiedStatusFail - no sufficient data is available
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - Data of value(m_ui16RemainingLength) is invalid(outrange of the size
  ///      SIZE_OF_PASSTHRU_PARAM_BUFFERTYPE_HEADER) [eFrameworkunifiedStatusInvldParam]
  ///      - Data of value(m_pui8CurOffset) is invalid [eFrameworkunifiedStatusInvldParam]
  ///      - Covert value(m_pui8CurOffset) failed [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get buffer address(f_pui8Buffer) and its size.
  ///       Return the value(type EFrameworkunifiedStatus) indicate the operation result.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetBuffer(UI_8 *&f_pui8Buffer, UI_16 &f_ui16BufLength) {
    EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
    if (m_ui16RemainingLength >= SIZE_OF_PASSTHRU_PARAM_BUFFERTYPE_HEADER) {
      if (0xB == *m_pui8CurOffset) {  // check if the corresponding data is of type buffer
        m_pui8CurOffset++;
        f_ui16BufLength = *(static_cast<UI_16 *>(static_cast<PVOID>(m_pui8CurOffset)));
        f_ui16BufLength = ConvertEndian_UI16(f_ui16BufLength);
        m_pui8CurOffset += sizeof(UI_16);
        m_ui16RemainingLength = static_cast<UI_16>(m_ui16RemainingLength - SIZE_OF_PASSTHRU_PARAM_BUFFERTYPE_HEADER);

        if (m_ui16RemainingLength >= f_ui16BufLength) {
          f_pui8Buffer = m_pui8CurOffset;
          m_pui8CurOffset += f_ui16BufLength;
          m_ui16RemainingLength = static_cast<UI_16>(m_ui16RemainingLength - f_ui16BufLength);
        } else {
          l_eStatus = eFrameworkunifiedStatusFail;
        }
      } else {
        l_eStatus = eFrameworkunifiedStatusInvldID;
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusFail;
    }
    return l_eStatus;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetArrayCount
  /// \~english @par Brief
  ///     This function is used to get the buffer and its size.
  /// \~english @param [out] f_ui16NoOfArrayElements
  ///     UI_16 - buffer size
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - If no array id found at the current location
  /// \~english @retval eFrameworkunifiedStatusFail - no sufficient buffer to read the data
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - Data of value(m_ui16RemainingLength) is invalid(outrange of the size
  ///      SIZE_OF_PASSTHRU_PARAM_BUFFERTYPE_HEADER) [eFrameworkunifiedStatusInvldParam]
  ///      - Data of value(m_pui8CurOffset) is invalid [eFrameworkunifiedStatusInvldParam]
  ///      - Covert value(m_pui8CurOffset) failed [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Get buffer address(f_pui8Buffer) and its size.
  ///       Return the value(type EFrameworkunifiedStatus) indicate the operation result.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus GetArrayCount(UI_16 &f_ui16NoOfArrayElements) {
    EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
    if (m_ui16RemainingLength >= SIZE_OF_PASSTHRU_PARAM_BUFFERTYPE_HEADER) {
      if (0xC == *m_pui8CurOffset) {  // check if the corresponding data is of type array
        m_pui8CurOffset++;
        f_ui16NoOfArrayElements = *(static_cast<UI_16 *>(static_cast<PVOID>(m_pui8CurOffset)));
        f_ui16NoOfArrayElements = ConvertEndian_UI16(f_ui16NoOfArrayElements);
        m_pui8CurOffset += sizeof(UI_16);
        m_ui16RemainingLength = static_cast<UI_16>(m_ui16RemainingLength - SIZE_OF_PASSTHRU_PARAM_BUFFERTYPE_HEADER);
      } else {
        l_eStatus = eFrameworkunifiedStatusInvldID;
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusFail;
    }
    return l_eStatus;
  }

  UI_16 GetDataLength() {
    return m_ui16TotalLength;
  }

 private:
  // template <class DataType>
  // EFrameworkunifiedStatus GetValue(DataType& f_dtOutValue);

  /*
   *  \return -
   *  eFrameworkunifiedStatusFail - no sufficient data is available
   *  eFrameworkunifiedStatusOK - data retrieved
   *  eFrameworkunifiedStatusInvldID - invalid data type id passed
   */
  template <class DataType>
  EFrameworkunifiedStatus GetValue(DataType &l_dtOutValue) {
    EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
    if (m_ui16RemainingLength >= (sizeof(DataType) + SIZE_OF_PASSTHRU_DATATYPE_IDENTIFIER)) {
      if ((0x1 <= *m_pui8CurOffset) && (*m_pui8CurOffset <= 0x9))  // Get value is applicable only for basic types,
        // not for buffer and array
      {
        m_pui8CurOffset++;
        l_dtOutValue = *(static_cast<DataType *>(static_cast<PVOID>(m_pui8CurOffset)));
        m_ui16RemainingLength = static_cast<UI_16>(m_ui16RemainingLength - sizeof(DataType));
        m_pui8CurOffset += sizeof(DataType);
      } else {
        l_eStatus = eFrameworkunifiedStatusInvldID;
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusFail;
    }
    return l_eStatus;
  }

  UI_8 *m_ui8Data;
  UI_16 m_ui16TotalLength;  // total data length
  UI_16 m_ui16ParamCount;

  UI_8 *m_pui8CurOffset;
  UI_16 m_ui16RemainingLength;  // remaining data length. Not yet parsed
};

/////////////////////////////////////////
//// CPassThruOutDataHandler
/////////////////////////////////////////
/**
 * @class CPassThruOutDataHandler
 * \~english @brief Data Handler
 * \~english @par   Brief Introduction
 *        Class to get/set function to do output data handler
 *
 */
class CPassThruOutDataHandler {
 public:
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CPassThruOutDataHandler
  /// \~english @par Summary
  ///       Constructor of CPassThruOutDataHandler class.
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       None
  /// \~english @par Detail
  ///       To generate a CPassThruOutDataHandler class. \n
  ///       Member variables (m_ui16TotalLength, m_ui16ParamCount, m_ui16CurOffset, m_ui16CurrentArrCount,
  ///       m_bArrayTypeSet, m_ui8LastArrDataType) to be initialized. \n
  /// \~english @see  ~CPassThruOutDataHandler
  ////////////////////////////////////////////////////////////////////////////////////
  CPassThruOutDataHandler(UI_8 f_ui8ClientId, UI_8 f_ui8PluginId, UI_32 f_ui32CommandId)
    : m_ui16TotalLength(PASSTHRU_DATA_HEADER_LEN),
      m_ui16ParamCount(0),
      m_ui16CurOffset(0),
      m_ui16CurrentArrCount(0),
      m_bArrayTypeSet(FALSE),
      m_ui8LastArrDataType(0) {  // default data type eNull
    m_vui8Data.resize(PASSTHRU_DATA_HEADER_LEN);
    // TODO: check error condition for resize operation
    // TODO: try if &m_vui8Data[m_ui16CurOffset] directly works without casting
    *static_cast<UI_8 *>(static_cast<PVOID>(&m_vui8Data[m_ui16CurOffset])) = f_ui8ClientId;
    m_ui16CurOffset++;
    *static_cast<UI_8 *>(static_cast<PVOID>(&m_vui8Data[m_ui16CurOffset])) = f_ui8PluginId;
    m_ui16CurOffset++;
    *static_cast<UI_32 *>(static_cast<PVOID>(&m_vui8Data[m_ui16CurOffset])) = ConvertEndian_UI32(f_ui32CommandId);
    m_ui16CurOffset = static_cast<UI_16>(m_ui16CurOffset + sizeof(UI_32));
    *static_cast<UI_16 *>(static_cast<PVOID>(&m_vui8Data[m_ui16CurOffset])) = 0;  // initialize param count
    m_ui16ParamCount = m_ui16CurOffset;
    m_ui16CurOffset = static_cast<UI_16>(m_ui16CurOffset + sizeof(UI_16));
    m_ui8ClientId = f_ui8ClientId;
  }

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~CPassThruOutDataHandler
  /// \~english @par Summary
  ///       Destructor of CPassThruOutDataHandler class.
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None.
  /// \~english @par Change of internal state
  ///       - The internal state is not changed.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       None
  /// \~english @par Detail
  ///       To delete a CPassThruOutDataHandler class. \n
  /// \~english @see  CPassThruOutDataHandler
  ////////////////////////////////////////////////////////////////////////////////////
  ~CPassThruOutDataHandler() {
    m_vui8Data.clear();
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetClientID
  /// \~english @par Brief
  ///     This function is used to get clinet id.
  /// \~english @param none
  /// \~english @retval
  ///     UI_8 - client id
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Return the value(m_ui8ClientId).
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_8 GetClientID() {
    return m_ui8ClientId;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetData
  /// \~english @par Brief
  ///     This function is used to get m_vui8Data data address.
  /// \~english @param none
  /// \~english @retval
  ///     UI_8 * - m_vui8Data data address
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Return m_vui8Data data address.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_8 *GetData() {
    return static_cast<UI_8 *>(static_cast<PVOID>(&m_vui8Data[0]));
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetLength
  /// \~english @par Brief
  ///     This function is used to get m_ui16CurOffset value.
  /// \~english @param none
  /// \~english @retval
  ///     UI_16 - m_ui16CurOffset value
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Return m_ui16CurOffset value.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_16 GetLength() {
    return m_ui16CurOffset;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetBool
  /// \~english @par Brief
  ///     This function is used to set NSRCS_BOOL type value.
  /// \~english @param[in] f_rcsboolValue
  ///        NSRCS_BOOL - new NSRCS_BOOL value
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - set array element of different type
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Set NSRCS_BOOL type value.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetBool(NSRCS_BOOL &f_rcsboolValue) {
    return SetValue<NSRCS_BOOL>(f_rcsboolValue, 0x1);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetUI8
  /// \~english @par Brief
  ///     This function is used to set UI_8 type value.
  /// \~english @param[in] f_ui8Value
  ///        UI_8 - new UI_8 value
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - set array element of different type
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Set UI_8 type value.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetUI8(UI_8 f_ui8Value) {
    return SetValue<UI_8>(f_ui8Value, 0x2);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetUI16
  /// \~english @par Brief
  ///     This function is used to set UI_16 type value.
  /// \~english @param[in] f_ui16Value
  ///        UI_16 - new UI_16 value
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - set array element of different type
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Set UI_16 type value.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetUI16(UI_16 f_ui16Value) {
    return SetValue<UI_16>(ConvertEndian_UI16(f_ui16Value), 0x3);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetUI32
  /// \~english @par Brief
  ///     This function is used to set UI_32 type value.
  /// \~english @param[in] f_ui32Value
  ///        UI_32 - new UI_32 value
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - set array element of different type
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Set UI_32 type value.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetUI32(UI_32 f_ui32Value) {
    return SetValue<UI_32>(ConvertEndian_UI32(f_ui32Value), 0x4);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetUI64
  /// \~english @par Brief
  ///     This function is used to set UI_64 type value.
  /// \~english @param[in] f_ui64Value
  ///        UI_64 - new UI_64 value
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - set array element of different type
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Set UI_64 type value.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetUI64(UI_64 f_ui64Value) {
    return SetValue<UI_64>(ConvertEndian_UI64(f_ui64Value), 0x5);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetSI8
  /// \~english @par Brief
  ///     This function is used to set SI_8 type value.
  /// \~english @param[in] f_si8Value
  ///        SI_8 - new SI_8 value
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - set array element of different type
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Set SI_8 type value.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetSI8(SI_8 f_si8Value) {
    return SetValue<SI_8>(f_si8Value, 0x6);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetSI16
  /// \~english @par Brief
  ///     This function is used to set SI_16 type value.
  /// \~english @param[in] f_si16Value
  ///        SI_16 - new SI_16 value
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - set array element of different type
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Set SI_16 type value.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetSI16(SI_16 f_si16Value) {
    return SetValue<SI_16>(ConvertEndian_SI16(f_si16Value), 0x7);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetSI32
  /// \~english @par Brief
  ///     This function is used to set SI_32 type value.
  /// \~english @param[in] f_si32Value
  ///        SI_32 - new SI_32 value
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - set array element of different type
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Set SI_32 type value.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetSI32(SI_32 f_si32Value) {
    return SetValue<SI_32>(ConvertEndian_SI32(f_si32Value), 0x8);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetSI64
  /// \~english @par Brief
  ///     This function is used to set SI_64 type value.
  /// \~english @param[in] f_si64Value
  ///        SI_64 - new SI_64 value
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - set array element of different type
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Set SI_64 type value.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetSI64(SI_64 f_si64Value) {
    return SetValue<SI_64>(ConvertEndian_SI64(f_si64Value), 0x9);
  }


  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetBuffer
  /// \~english @par Brief
  ///     This function is used to set buffer and size.
  /// \~english @param[in] f_pui8Buffer
  ///        UI_8* - buffer address
  /// \~english @param[in] f_ui16BufLength
  ///        UI_16 - buffer size
  /// \~english @retval EFrameworkunifiedStatus
  /// \~english @retval eFrameworkunifiedStatusOK - data retrieved
  /// \~english @retval eFrameworkunifiedStatusInvldID - data member(m_ui8LastArrDataType) is invalid value
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Set buffer and size.
  /// \~english @see none 
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetBuffer(UI_8 *f_pui8Buffer, UI_16 f_ui16BufLength) {
    EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
    // TODO: add error check for resize operation
    m_ui16TotalLength = static_cast<UI_16>(m_ui16TotalLength + f_ui16BufLength + SIZE_OF_PASSTHRU_PARAM_BUFFERTYPE_HEADER);
    m_vui8Data.resize(m_ui16TotalLength);
    *static_cast<UI_8 *>(static_cast<PVOID>(&m_vui8Data[m_ui16CurOffset])) = 0xB;
    m_ui16CurOffset++;
    *static_cast<UI_16 *>(static_cast<PVOID>(&m_vui8Data[m_ui16CurOffset])) = ConvertEndian_UI16(f_ui16BufLength);
    m_ui16CurOffset = static_cast<UI_16>(m_ui16CurOffset + sizeof(UI_16));
    std::memcpy(&m_vui8Data[m_ui16CurOffset], f_pui8Buffer, f_ui16BufLength);
    m_ui16CurOffset = static_cast<UI_16>(m_ui16CurOffset + f_ui16BufLength);

    if (!m_bArrayTypeSet) {
//      *static_cast<UI_16*>(static_cast<PVOID>(&m_vui8Data[m_ui16ParamCount])) =
//          (*static_cast<UI_16*>(static_cast<PVOID>(&m_vui8Data[m_ui16ParamCount]))) + 1;
      UpdateParamCount();
    } else {
      // setting array element
      m_ui16CurrentArrCount--;

      if ((m_ui8LastArrDataType != 0) && (m_ui8LastArrDataType != 0xB)) {
        l_eStatus = eFrameworkunifiedStatusInvldID;
      } else {
        m_ui8LastArrDataType = 0xB;
        if (0 == m_ui16CurrentArrCount) {
          m_ui8LastArrDataType = 0;  // default data type eNull
          m_bArrayTypeSet = FALSE;
        }
      }
    }
    return l_eStatus;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetArrayCount
  /// \~english @par Brief
  ///     This function is used to set ArrayCount.
  /// \~english @param f_ui16ArrayCount
  ///     UI_16 - ArrayCount
  /// \~english @retval none
  /// \~english @par Prerequisite
  ///      - None
  /// \~english @par Change of internal state
  ///      - None
  /// \~english @par Conditions of processing failure
  ///      - none
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       Set ArrayCount.
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus SetArrayCount(UI_16 f_ui16ArrayCount) {
    EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
    // TODO: add error check for resize operation
    m_ui16TotalLength = static_cast<UI_16>(m_ui16TotalLength + SIZE_OF_PASSTHRU_PARAM_BUFFERTYPE_HEADER);
    m_vui8Data.resize(m_ui16TotalLength);
    *static_cast<UI_8 *>(static_cast<PVOID>(&m_vui8Data[m_ui16CurOffset])) = 0xC;
    m_ui16CurOffset++;
    *static_cast<UI_16 *>(static_cast<PVOID>(&m_vui8Data[m_ui16CurOffset])) = ConvertEndian_UI16(f_ui16ArrayCount);
    m_ui16CurOffset = static_cast<UI_16>(m_ui16CurOffset + sizeof(UI_16));

    m_ui16CurrentArrCount = f_ui16ArrayCount;
    m_bArrayTypeSet = TRUE;
//    *static_cast<UI_16*>(static_cast<PVOID>(&m_vui8Data[m_ui16ParamCount])) =
//        (*static_cast<UI_16*>(static_cast<PVOID>(&m_vui8Data[m_ui16ParamCount]))) + 1;
    UpdateParamCount();

    // TODO: add error handling, if any other element is added before the array count reaches to f_ui16ArrayCount
    return l_eStatus;
  }

 private:
  CPassThruOutDataHandler() {

  }

  // SetValue
  // \return - eFrameworkunifiedStatusInvldID - If setting array element of different type
  template <class DataType>
  EFrameworkunifiedStatus SetValue(DataType l_dtValue, UI_8  f_ui8DataType) {
    EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
    // TODO: add error check for resize operation

    m_ui16TotalLength = static_cast<UI_16>(m_ui16TotalLength + SIZE_OF_PASSTHRU_DATATYPE_IDENTIFIER + sizeof(DataType));
    m_vui8Data.resize(m_ui16TotalLength);
    *static_cast<UI_8 *>(static_cast<PVOID>(&m_vui8Data[m_ui16CurOffset])) = f_ui8DataType;
    m_ui16CurOffset++;
    *static_cast<DataType *>(static_cast<PVOID>(&m_vui8Data[m_ui16CurOffset])) = l_dtValue;
    m_ui16CurOffset = static_cast<UI_16>(m_ui16CurOffset + sizeof(DataType));

    if (!m_bArrayTypeSet) {
//      *static_cast<UI_16*>(static_cast<PVOID>(&m_vui8Data[m_ui16ParamCount])) =
//          (*static_cast<UI_16*>(static_cast<PVOID>(&m_vui8Data[m_ui16ParamCount]))) + 1;
      UpdateParamCount();
    } else {
      // setting array element
      m_ui16CurrentArrCount--;

      if ((m_ui8LastArrDataType != 0) && (m_ui8LastArrDataType != f_ui8DataType)) {
        l_eStatus = eFrameworkunifiedStatusInvldID;
      } else {
        m_ui8LastArrDataType = f_ui8DataType;
        if (0 == m_ui16CurrentArrCount) {
          m_ui8LastArrDataType = 0;  // default data type eNull
          m_bArrayTypeSet = FALSE;
        }
      }
    }
    return l_eStatus;
  }

  VOID UpdateParamCount() {
    UI_16 l_ui16ParamCount = static_cast<UI_16>(ConvertEndian_UI16(*static_cast<UI_16 *>(static_cast<PVOID>
                                                                                         (&m_vui8Data[m_ui16ParamCount]))) + 1);
    l_ui16ParamCount = ConvertEndian_UI16(l_ui16ParamCount);
    *static_cast<UI_16 *>(static_cast<PVOID>(&m_vui8Data[m_ui16ParamCount])) = l_ui16ParamCount;
  }

  // UI_8* m_ui8Data;
  std::vector<UI_8> m_vui8Data;
  UI_16 m_ui16TotalLength;  // total data length
  UI_16 m_ui16ParamCount;
  UI_16 m_ui16CurOffset;
  UI_8  m_ui8ClientId;

  // required for array data type
  UI_16 m_ui16CurrentArrCount;
  BOOL m_bArrayTypeSet;
  UI_8 m_ui8LastArrDataType;
};

#endif  // _NS_RCS_DATA_HANDLER_H__  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
