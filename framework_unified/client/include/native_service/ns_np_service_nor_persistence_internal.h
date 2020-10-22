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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NPServiceIf
/// \brief    This file contains the declaration for internal functions to retrieve and store
///           immediate notification data from and to persistent storage.
///
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_NPSERVICEIF_INCLUDE_NS_NP_SERVICE_NOR_PERSISTENCE_INTERNAL_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_NPSERVICEIF_INCLUDE_NS_NP_SERVICE_NOR_PERSISTENCE_INTERNAL_H_

///////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>
#include <native_service/ns_np_service.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////
/// NPSynchronousGetPersistentData
/// API to retrieve immediate storage notification data structure.
///
///
/// \param [in] pAppName
///         PCSTR - Name of application service
/// \param [in] notif_name
///         PCSTR - Name of notification to be written to immediate storage
/// \param [in] pData
///         PVOID - Notification data
/// \param [in] uiDataSize
///         UI_32 - Size of notification data
/// \param [out] ptNorPersistentData
///         NC_NorPersistentData - Immediate notification structure
/// \param [in] ePersistCategory
///         const EFrameworkunifiedPersistCategory - Persist Category of Notification.
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSynchronousGetPersistentData(PCSTR pAppName,
                                          PCSTR notif_name,
                                          PVOID pData,
                                          UI_32 uiDataSize,
                                          NC_NorPersistentData &ptNorPersistentData,
                                          const EFrameworkunifiedPersistCategory ePersistCategory);

////////////////////////////////////////////////////////////////////////////////////////////
/// NPSynchronousGetPersistentDataSize
/// API to get immediate storage notification data size.
///
/// \param [in] notif_name
///         PCSTR - Name of notification to be read from immediate storage
/// \param [out] uiDataSize
///         PUI_32 - notification data size
/// \param [in] ePersistCategory
///         const EFrameworkunifiedPersistCategory - Persist Category of Notification.
///
/// \return EFrameworkunifiedStatus
///         EFrameworkunifiedStatus - success or failure
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSynchronousGetPersistentDataSize(PCSTR notif_name,
                                              PUI_32 uiDataSize,
                                              const EFrameworkunifiedPersistCategory ePersistCategory);

////////////////////////////////////////////////////////////////////////////////////////////
/// SynchronousGetPersistentDataSize
/// API to write immediate persistence notification data.
///
/// \param [in] notif_name
///         PCSTR - Name of notification to be read from immediate storage
/// \param [in] ePersistCategory
///         const EFrameworkunifiedPersistCategory - Persist Category of Notification.
///
/// \return UI_32
///         UI_32 - size of the data
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SynchronousWritePersistentData(PCSTR pAppName,
                                          PCSTR notif_name,
                                          PVOID pData,
                                          const UI_32 uiDataSize,
                                          const UI_32 uiDelay,
                                          const UI_32 uiMaxSize,
                                          const EFrameworkunifiedPersistCategory ePersistCategory);

////////////////////////////////////////////////////////////////////////////////////////////
/// SynchronousMovePersistentData
/// API to move all the immediate notification data from f_cSrcDir directory to
/// f_cDestDir directory
///
/// \param [in] f_cSrcDir
///         PCSTR - Source directory full path
/// \param [in] f_cDestDir
///         PCSTR - Dest directory full path
///
/// \return EFrameworkunifiedStatus
///         EFrameworkunifiedStatus - eFrameworkunifiedStatusAccessError if source path not exists
///                      eFrameworkunifiedStatusFileLoadError if unable to access src directory
///                      eFrameworkunifiedStatusInvldParam if invalid parameter is received
///                      eFrameworkunifiedStatusFail if error occurs while copying file
///                      eFrameworkunifiedStatus on success
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SynchronousMovePersistentData(PCSTR f_cSrcDir,
                                         PCSTR f_cDestDir);

#ifdef __cplusplus
}
#endif

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_NPSERVICEIF_INCLUDE_NS_NP_SERVICE_NOR_PERSISTENCE_INTERNAL_H_

// EOF
