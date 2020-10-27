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

///////////////////////////////////////////////////////////////////////////////
/// This file supports templates for high use common tasks.
///////////////////////////////////////////////////////////////////////////////
/**
 * @file ss_templates.h
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup system_manager
 *  @ingroup system_service
 *  @{
 */
#ifndef __SS_TEMPLATES_H__  // NOLINT (build/header_guard)
#define __SS_TEMPLATES_H__

#include <stdio.h>
#include <errno.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/ns_logger_if.h>
#include <native_service/frameworkunified_framework_if.h>
#include <map>
#include "system_service/ss_system_types.h"
#include "system_service/ss_string_maps.h"

#ifndef ZONE_ERR
#define ZONE_ERR     ZONEMASK(31)
#endif

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup ReadMsg
/// \~english @par Summary
///       check the data information of msg.
/// \~english @param [in] hApp
///       HANDLE - HANDLE Application
/// \~english @param [in] Data
///       T - The reference to the Data memory location
/// \~english @param [in] f_eRetrieveMethod
///       ESMRetrieveTypes - The msg retrieval method ( release or retain )
/// \~english @par
///            T template type
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @par Preconditions
///       -
/// \~english @par Change of the internal state
///       - The internal state is not changed.
/// \~english @par Causes of failures
///       - System Manager handle for the session is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - data'size is not corruct. [eFrameworkunifiedStatusInvldParam]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Method only
/// \~english @par Detail
///       Check hApp ptr, msg size, msg reception, read msg if all ok
/// \~english @par
///             eFrameworkunifiedStatus:Result
///                - eFrameworkunifiedStatusOK:Success
///                - Except eFrameworkunifiedStatusOK:Fail
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
template <typename T> EFrameworkunifiedStatus ReadMsg(
                     HANDLE hApp,
                     T &Data,  // NOLINT (runtime/references)
                     ESMRetrieveTypes f_eRetrieveMethod = eSMRRelease) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if ( NULL == hApp ) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error: NULL = hApp");
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
  } else if (sizeof(Data) != FrameworkunifiedGetMsgLength(hApp)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__
       , " Error: message buffer sizes mismatch: expected %ld, received %d"
       , sizeof(Data), FrameworkunifiedGetMsgLength(hApp));
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  } else if ( eFrameworkunifiedStatusOK != (l_eStatus =
                 FrameworkunifiedGetMsgDataOfSize(hApp, (PVOID)&Data, sizeof(Data), f_eRetrieveMethod))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__
       , " Error: FrameworkunifiedGetMsgDataOfSize() errored: 0x%x", l_eStatus);
  }
  return l_eStatus;
}  // End of template <typename T> EFrameworkunifiedStatus ReadMsg ()
  /* Copy & paste easy-to-use little 't' template
  // ReadMsg():                                                        *
  //     Check hApp ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if ( eFrameworkunifiedStatusOK != ( l_eStatus = ReadMsg<T>
                                            ( hApp
                                            , Data )))
  {
    LOG_ERROR("ReadMsg()");
  }
  else
  */

//****************************************************************************
/*!
  \def LOG_ERROR(pStr)
   Log pStr and l_eStatus
 */
#define  LOG_ERROR(pStr)                     \
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__            \
           , " Error: %s errored: %d/'%s'"   \
           , pStr                            \
           , l_eStatus                       \
           , GetStr(static_cast<EFrameworkunifiedStatus>(l_eStatus)).c_str());

//****************************************************************************
/*!
  \def LOG_ERROR(pStr)
   Log pStr and l_eStatus
 */
#define  LOG_SUCCESS(pStr) \
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " %s successful", pStr);

//****************************************************************************
/*!
  \def LOG_STATUS(l_eStatus,pStr)
   Log pStr and success or failure. Include l_eStatus when not successful.
 */
#define LOG_STATUS(l_eStatus, pStr)  \
    if ( eFrameworkunifiedStatusOK != l_eStatus ) \
    {                                \
        LOG_ERROR(pStr);             \
    }                                \
    else                             \
    {                                \
        LOG_SUCCESS(pStr);           \
    }  // End of #define LOG_STATUS(l_eStatus,pStr)

//****************************************************************************
/*!
  \def CALL_AND_LOG_STATUS(fnc)
     Call the function and log the returned EFrameworkunifiedStatus.
 */
#define CALL_AND_LOG_STATUS(fnc)           \
    l_eStatus = (fnc);                     \
    LOG_STATUS(l_eStatus, #fnc)
    // End of #define CALL_AND_LOG_STATUS(fnc)

//****************************************************************************
/*!
  \def LOG_STATUS_IF_ERRORED(l_eStatus, pStr)
   Log pStr on failure, including EFrameworkunifiedStatus.
 */
#define LOG_STATUS_IF_ERRORED(l_eStatus, pStr)         \
    if ( eFrameworkunifiedStatusOK != l_eStatus )                   \
    {                                                  \
        LOG_ERROR(pStr);                               \
    }  // End of #define LOG_STATUS_IF_ERRORED(l_eStatus,pStr)

//****************************************************************************
/*!
  \def CALL_AND_LOG_STATUS_IF_ERRORED(fnc)
   Call the function and log the returned EFrameworkunifiedStatus on failure.
 */
#define CALL_AND_LOG_STATUS_IF_ERRORED(fnc) \
    l_eStatus = (fnc);                      \
    LOG_STATUS_IF_ERRORED(l_eStatus, #fnc)
    // End of #define CALL_AND_LOG_STATUS_IF_ERRORED(fnc)

//****************************************************************************
/*!
  \def MAP_ENTRY( f_map, f_enum )
  Simplify initializing string map entry.

  Use to set a map entry's key and value to the specified enum and the enum's
  literal text ( i.e., stringified ) equivalent.
 */
#define MAP_ENTRY(f_map, f_enum) \
  f_map[ f_enum ] = #f_enum

//****************************************************************************
/*!
  \def INTERFACEUNIFIEDLOG_RECEIVED_FROM
   Log whom we received message or notification from.

   Use this macro to ensure that the string "Received from" is uniformly
   logged; this string can be grepped on to find when/where callback functions
   were invoked.
 */
#define INTERFACEUNIFIEDLOG_RECEIVED_FROM(hApp) \
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Received from %s", FrameworkunifiedGetMsgSrc(hApp))

//****************************************************************************
/*!
  \def INTERFACEUNIFIEDLOG_WHEN_COMPILED
  Log when this file was compiled.

  Useful when overlaying a delivered object file ( from an official build ) with
  a debug-built obj file - verifies that the debug file did indeed get loaded.
  */
#define INTERFACEUNIFIEDLOG_WHEN_COMPILED \
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " %s was compiled at %s @ %s", \
    __FILE__, __DATE__, __TIME__);


#define SS_ASERT(x) \
  if (!(x)) { \
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SS_ASSERT"); \
  }

#define SS_ASERT_ERRNO(x) \
  if (!(x)) { \
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SS_ASSERT %d:%s", errno, strerror(errno)); \
  }

#define SS_ASERT_LOG(x, fmt, ...) \
  if (!(x)) { \
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SS_ASSERT " fmt, ## __VA_ARGS__); \
  }

#define SS_STATIC_ASERT(expr) \
  { \
    char STATIC_ASSERTION_FAILED[(expr) ? 1 : -1]; \
    (void)STATIC_ASSERTION_FAILED; \
  }

/**
 * @class EnumStringMap
 * \~english @brief EnumStringMap
 * \~english @par   Brief Introduction
 *        Class to provide EnumStringMap template function
 *
 */
template <typename enumT, void(*fp)(std::map<enumT, SS_String> & m_strMap)> class EnumStringMap {  // NOLINT (runtime/references)
public:
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup EnumStringMap
  /// \~english @par Summary
  ///       Default constructor of EnumStringMap class.
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
  ///       To generate a EnumStringMap class, and initialize member variables. \n
  ///       After the constructor, be sure to call the Initialize. \n
  /// \~english @see  ~EnumStringMap, Initialize
  ////////////////////////////////////////////////////////////////////////////////////
  EnumStringMap() {
    (*fp)(m_strMap);
  }
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~EnumStringMap
  /// \~english @par Summary
  ///
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
  ///       To delete a EnumStringMap object. \n
  /// \~english @see  EnumStringMap
  ////////////////////////////////////////////////////////////////////////////////////
  ~EnumStringMap() {}
  SS_String GetStr( enumT f_enum) { return m_strMap[ f_enum ];}
private:
  std::map<enumT, SS_String> m_strMap;
};
// End of template <typename enumT, void(*fp)(std::map<enumT, SS_String> & m_strMap)> class EnumStringMap

#endif /* __SS_TEMPLATES_H__ */  // NOLINT (build/header_guard)

