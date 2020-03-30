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
 * @file ns_shared_mem_if.h
 * @brief \~english API Header for Logger.
 *					Declares the external APIs to Logger.
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
#ifndef __CCR_NATIVESERVICES_MESSAGECENTER_NSSHAREDMEMIF_H__  // NOLINT  (build/header_guard)
#define __CCR_NATIVESERVICES_MESSAGECENTER_NSSHAREDMEMIF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_service/frameworkunified_types.h>  ///< Standard Types

typedef unsigned int TMemID;    ///< Type of memory buffer identifiers
#define BAD_MEM_ID (0)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup SetDataToShared
/// \~english @par Summary
///           - Store a large chunk of memory using SharedMem service
/// \~english @param [in] data
///           - const void* - data to be stored in shared memory
/// \~english @param [in] dataBytes
///           - UI_32 - length of the data (in bytes) that is stored in shared memory
/// \~english @param [in] from
///           - const char * - start address
/// \~english @param [in] to
///           - const char * - end address
/// \~english @retval TMemID - unique MemID of the data stored in the shared memory, 0 if error occurs
/// \~english @par Preconditions
///           - None
/// \~english @par Change of the internal state
///           - shared memory changed
/// \~english @par Causes of failures
///           - param is Invalid.[BAD_MEM_ID]
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - The function sets the data to the shared memory and associates it with a\n
///             unique MemID that can be used to refer to data in subsequent calls \n
///             (not necessarily from the same process).
/// \~english @par
///           - TMemID:Result
///               - Except 0:Success
///               -        0:Failure
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
TMemID SetDataToShared(const void *data, UI_32 dataBytes, const char *from, const char *to);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetDataFromShared
/// \~english @par Summary
///           - Retrieve a large chunk of memory using SharedMem service
/// \~english @param [in] id
///           - TMemID - MemID of the shared data
/// \~english @param [in] data
///           - void*  - Buffer to store data in, must be at least as large as buffer associated with id
/// \~english @param [in] dataMaxBytes
///           - UI_32  - Size of data - system will never copy more than dataMaxBytes bytes into data
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldID Invalid id
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Preconditions
///           -
/// \~english @par Change of the internal state
///           - None
/// \~english @par Causes of failures
///           - param is Invalid. [eFrameworkunifiedStatusInvldParam]
///           - id is Invalid. [eFrameworkunifiedStatusInvldID]
///           - Any error has occurred at the time of transmission of the System Manager message for the session.
///           [eFrameworkunifiedStatusFail]
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - Retrieve data, given a MemID. Each MemID is only safe to use until a call to \n
///             DiscardDataFromShared If the call succeeds, the return value is eFrameworkunifiedStatusOK.\n
///             Otherwise, the contents of data are undefined.
/// \~english @par
///           - eFrameworkunifiedStatus:Result
///               - eFrameworkunifiedStatusOK:Success
///               - Except eFrameworkunifiedStatusOK:Failure
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus GetDataFromShared(TMemID id, void *data, UI_32 dataMaxBytes);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetLengthOfDataFromShared
/// \~english @par Summary
///           - Query the minimum buffer size, in bytes, necessary to hold a chunk of memory in SharedMem
/// \~english @param [in] id
///           - TMemID - MemID of the shared data
/// \~english @retval length - Minimum buffer size, in bytes, required to hold the data associated with id.
/// \~english @par Preconditions
///           -
/// \~english @par Change of the internal state
///           - None
/// \~english @par Causes of failures
///           - param is Invalid.[BAD_MEM_ID]
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - Returns the minimum buffer size, in bytes, required to hold the data associated with id.\n
///             Return value of 0 indicates failure.For an id, may be called one or more times \n
///             until a successful call to either GetDataFromShared or DiscardDataFromShared with that id.
/// \~english @par
///           - TMemID:Result
///               - Except 0:Success
///               -        0:Failure
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
UI_32 GetLengthOfDataFromShared(TMemID id);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DiscardDataFromShared
/// \~english @par Summary
///           - Discard a chunk of memory in SharedMem
/// \~english @param [in] id
///           - TMemID - MemID of the shared data
/// \~english @retval eFrameworkunifiedStatusOK - Success
/// \~english @retval eFrameworkunifiedStatusInvldParam -Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldID -Invalid id
/// \~english @retval eFrameworkunifiedStatusFail -Some sort of error occurred
/// \~english @par Preconditions
///           -
/// \~english @par Change of the internal state
///           - None
/// \~english @par Causes of failures
///           - param is Invalid. [eFrameworkunifiedStatusInvldParam]
///           - id is Invalid. [eFrameworkunifiedStatusInvldID]
///           - Any error has occurred at the time of transmission of the System Manager message for the session.
///           [eFrameworkunifiedStatusFail]
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - Invalidates the data stored with the MemID.  Each id is only safe to use in a\n
///             single successful call to GetDataFromShared or DiscardDataFromShared
///             If the call succeeds, the return value is eImageStatusOK.
///             On failure, the state of shared memory shall not be altered
/// \~english @par
///           - eFrameworkunifiedStatus:Result
///               - eFrameworkunifiedStatusOK:Success
///               - Except eFrameworkunifiedStatusOK:Failure
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus DiscardDataFromShared(TMemID id);

#ifdef __cplusplus
}
#endif
#endif  // __CCR_NATIVESERVICES_MESSAGECENTER_NSSHAREDMEMIF_H__  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
