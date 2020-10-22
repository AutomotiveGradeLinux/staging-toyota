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
/// \ingroup  tag_NPService
/// \brief    This file contains the API to store and retrieve immediate
///           notification data from and to persistent storage.
///
///
///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <semaphore.h>
#include <aglpath.h>

#include <native_service/ns_np_service.h>
#include <native_service/ns_np_service_protocol.h>
#include <native_service/ns_np_service_if.h>
#include <native_service/ns_message_center_if.h>
#include <native_service/ns_util_directory.h>
#include <native_service/ns_np_service_nor_persistence.h>
#include <native_service/frameworkunified_framework_types.h>

#include <string>

#include <native_service/ns_np_service_nor_persistence_internal.h>
#include <native_service/ns_np_types.h>
#include <native_service/ns_mc_system_info.h>

sem_t g_FileAccessSemaphore;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NPSynchronousReadPersistentData
/// Read immediate persistent data from the persistent memory
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSynchronousReadPersistentData(PCSTR pAppName,
                                           PCSTR notif_name,
                                           PVOID pData,
                                           UI_32 uiDataSize,
                                           const EFrameworkunifiedPersistCategory ePersistCategory) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;
  NC_NorPersistentData l_tNorPersistentData = {};

  if (NULL != pAppName && NULL != notif_name && NULL != pData) {
    if (eFrameworkunifiedStatusOK != NPSynchronousGetPersistentData(pAppName, notif_name, pData, uiDataSize, l_tNorPersistentData,
                                                       ePersistCategory)) {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NPSynchronousGetPersistentData
/// Get the notification data from the persistent storage
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSynchronousGetPersistentData(PCSTR pAppName,
                                  PCSTR notif_name,
                                  PVOID pData,
                                  UI_32 uiDataSize,
                                  NC_NorPersistentData &ptImmediatePersistentData,  // NOLINT  (readability/nolint)
                                  const EFrameworkunifiedPersistCategory ePersistCategory) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;

  if (NULL != notif_name && NULL != pData && 0 < uiDataSize) {
    FILE *l_fFilePtr = NULL;

    if (0 == sem_init(&g_FileAccessSemaphore, 1, 1)) {  // LCOV_EXCL_BR_LINE 5: sem_init's error case
      if (sem_wait(&g_FileAccessSemaphore) == 0) {  // LCOV_EXCL_BR_LINE 5: sem_wait's error case
        // Create unique filename
        std::string l_cTempPath = notif_name;
        size_t l_uiTempPathSize = 0;

        std::string l_cPath = "";
        std::string l_cNewPath = IMMEDIATE_PERSISTENCE_STORAGE_V2;
        std::string l_cOldPath = IMMEDIATE_PERSISTENCE_STORAGE_V1;

        // replace all '/' by '_'
        l_uiTempPathSize = l_cTempPath.find_first_of("/");
        while (std::string::npos != l_uiTempPathSize) {
          l_cTempPath[l_uiTempPathSize] = '_';
          l_uiTempPathSize = l_cTempPath.find_first_of("/", l_uiTempPathSize + 1);
        }

        switch (ePersistCategory) {
          case eFrameworkunifiedFactoryData: {
              l_cNewPath.append(FACTORYDATADIR);
            }
            break;

          case eFrameworkunifiedFactoryCustomerData: {
              l_cNewPath.append(FACTORYCUSTOMERDATADIR);
            }
            break;

          case eFrameworkunifiedDealerData: {
              l_cNewPath.append(DEALERDATADIR);
            }
            break;

          case eFrameworkunifiedUserData:
          default: {
              l_cNewPath.append(USERDATADIR);
              l_cNewPath.append(ALLUSERAPPDATADIR);
            }
            break;
        }

        l_cNewPath.append(IMMEDIATEDATADIR);
        l_cNewPath.append(l_cTempPath);

        if (NULL != pAppName) {  // LCOV_EXCL_BR_LINE 6: pAppName was checed in NPSynchronousReadPersistentData
          l_cNewPath.append(pAppName);
        }

        if (0 == access(l_cNewPath.c_str(), F_OK)) {
          l_cPath.assign(l_cNewPath);
        } else {
          l_cOldPath.append(l_cTempPath);
          if (NULL != pAppName) {  // LCOV_EXCL_BR_LINE 6: pAppName was checed in NPSynchronousReadPersistentData
            l_cOldPath.append(pAppName);
          }

          if (0 == access(l_cOldPath.c_str(), F_OK)) {
            l_cPath.assign(l_cOldPath);
          } else {
            eFrameworkunifiedStatus = eFrameworkunifiedStatusFileLoadError;
          }
        }

        if (eFrameworkunifiedStatusOK ==  eFrameworkunifiedStatus) {
          size_t PersistData;
          // Open and read from a file
          l_fFilePtr = fopen(l_cPath.c_str(), "re");
          if (NULL != l_fFilePtr) {
            // read notification info structure
            if (fread(&ptImmediatePersistentData, 1, sizeof(ptImmediatePersistentData),
                      l_fFilePtr) == sizeof(ptImmediatePersistentData)) {
              if (0 == feof(l_fFilePtr)) {
                // read data
                PersistData = fread(pData, 1, uiDataSize, l_fFilePtr);
                if (PersistData != uiDataSize) {
                  eFrameworkunifiedStatus = eFrameworkunifiedStatusFail;
                }
              } else {
                eFrameworkunifiedStatus = eFrameworkunifiedStatusFail;
              }
            } else {
              eFrameworkunifiedStatus = eFrameworkunifiedStatusFail;
            }
            fclose(l_fFilePtr);
          } else {
            eFrameworkunifiedStatus = eFrameworkunifiedStatusFail;
          }
        }

        sem_post(&g_FileAccessSemaphore);
      } else {
        eFrameworkunifiedStatus = eFrameworkunifiedStatusErrOther;
      }
      sem_destroy(&g_FileAccessSemaphore);
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusErrOther;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }
  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NPSynchronousWritePersistentData
/// Store the notification data on the persistent memory
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSynchronousWritePersistentData(PCSTR pAppName,
                                            PCSTR notif_name,
                                            PVOID pData,
                                            const UI_32 uiDataSize,
                                            const EFrameworkunifiedPersistCategory ePersistCategory) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  HANDLE l_hReceiverMq = NULL;

  // if this API is used to write the data directly then max data size will be same as data write size
  l_eStatus = SynchronousWritePersistentData(pAppName, notif_name, pData, uiDataSize, 0, uiDataSize, ePersistCategory);

  l_hReceiverMq = McOpenSender(FRAMEWORKUNIFIED_NS_NPSERVICE);

  if (NULL != l_hReceiverMq) {
    (VOID)McSendWithSysInfo(l_hReceiverMq,
                            pAppName,
                            NPS_SYNCHRONOUS_WRITE_NOTIFY_REQ,
                            (PCHAR)notif_name,
                            uiDataSize,
                            pData,
                            0);

    McClose(l_hReceiverMq);
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SynchronousWritePersistentData
/// Store the notification data on the persistent memory
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SynchronousWritePersistentData(PCSTR pAppName,
                                          PCSTR notif_name,
                                          PVOID pData,
                                          const UI_32 uiDataSize,
                                          const UI_32 uiDelay,
                                          const UI_32 uiMaxSize,
                                          const EFrameworkunifiedPersistCategory ePersistCategory) {
  EFrameworkunifiedStatus eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;
  FILE *l_fFilePtr = NULL;
  NC_NorPersistentData l_tImmediatePersistentData = {};

  if (NULL != notif_name && NULL != pAppName && NULL != pData) {
    if (strlen(notif_name) < MAX_STRING_SIZE_NOTIFICATION && strlen(pAppName) < MAX_NAME_SIZE_APP) {
      // Create unique filename
      std::string l_cTempPath = notif_name;
      size_t l_uiTempPathSize = 0;

      // replace all '/' by '_'
      l_uiTempPathSize = l_cTempPath.find_first_of("/");
      while (std::string::npos != l_uiTempPathSize) {
        l_cTempPath[l_uiTempPathSize] = '_';
        l_uiTempPathSize = l_cTempPath.find_first_of("/", l_uiTempPathSize + 1);
      }

      strncpy(l_tImmediatePersistentData.notificationName, notif_name,
              sizeof(l_tImmediatePersistentData.notificationName) - 1);
      strncpy(l_tImmediatePersistentData.pPublisherName, pAppName,
        sizeof(l_tImmediatePersistentData.pPublisherName) - 1);
      l_tImmediatePersistentData.dataSize = uiDataSize;
      l_tImmediatePersistentData.uiDelay = uiDelay;
      l_tImmediatePersistentData.uiMaxSize = uiMaxSize;

      if (0 == sem_init(&g_FileAccessSemaphore, 1, 1)) {  // LCOV_EXCL_BR_LINE 5: sem_init's error case
        if (sem_wait(&g_FileAccessSemaphore) == 0) {  // LCOV_EXCL_BR_LINE 5: sem_wait's error case
          std::string l_cPath = IMMEDIATE_PERSISTENCE_STORAGE_V2;

          switch (ePersistCategory) {
            case eFrameworkunifiedFactoryData: {
                l_cPath.append(FACTORYDATADIR);
              }
              break;

            case eFrameworkunifiedFactoryCustomerData: {
                l_cPath.append(FACTORYCUSTOMERDATADIR);
              }
              break;

            case eFrameworkunifiedDealerData: {
                l_cPath.append(DEALERDATADIR);
              }
              break;

            case eFrameworkunifiedUserData:
            default: {
                l_cPath.append(USERDATADIR);
                l_cPath.append(ALLUSERAPPDATADIR);
              }
              break;
          }

          l_cPath.append(IMMEDIATEDATADIR);

          if (!DoesDirecotryExist(l_cPath)) {
            eFrameworkunifiedStatus = CreateDirectory(l_cPath);
          }

          if (eFrameworkunifiedStatusOK == eFrameworkunifiedStatus) {
            l_cPath.append(l_cTempPath);

            l_cPath.append(pAppName);

            l_fFilePtr = fopen(l_cPath.c_str(), "we");
            if (NULL != l_fFilePtr) {
              // write header
              fwrite(&l_tImmediatePersistentData , 1 , sizeof(l_tImmediatePersistentData) , l_fFilePtr);
              if (0 == ferror(l_fFilePtr)) {
                // write data
                fwrite(pData , 1 , uiDataSize , l_fFilePtr);
                if (0 != ferror(l_fFilePtr)) {
                  eFrameworkunifiedStatus = eFrameworkunifiedStatusOK;
                }
              }
              fsync(fileno(l_fFilePtr));
              fclose(l_fFilePtr);
            }
          }

          sem_post(&g_FileAccessSemaphore);
        }
        sem_destroy(&g_FileAccessSemaphore);
      } else {
        eFrameworkunifiedStatus = eFrameworkunifiedStatusErrOther;
      }
    } else {
      eFrameworkunifiedStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    eFrameworkunifiedStatus = eFrameworkunifiedStatusInvldParam;
  }
  return eFrameworkunifiedStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NPSynchronousGetPersistentDataSize
/// Get the size of the notification data stored in the persistent memory
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NPSynchronousGetPersistentDataSize(PCSTR notif_name, PUI_32 uiDataSize,
                                              const EFrameworkunifiedPersistCategory ePersistCategory) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != notif_name && NULL != uiDataSize) {
    FILE *l_fFilePtr = NULL;
    NC_NorPersistentData l_ptImmediatePersistentData = {};

    *uiDataSize = 0;

    if (0 == sem_init(&g_FileAccessSemaphore, 1, 1)) {  // LCOV_EXCL_BR_LINE 5: sem_init's error case
      if (sem_wait(&g_FileAccessSemaphore) == 0) {  // LCOV_EXCL_BR_LINE 5: sem_wait's error case
        // Create unique filename
        std::string l_cTempPath = notif_name;
        size_t l_uiTempPathSize = 0;

        std::string l_cPath = "";
        std::string l_cNewPath = IMMEDIATE_PERSISTENCE_STORAGE_V2;
        std::string l_cOldPath = IMMEDIATE_PERSISTENCE_STORAGE_V1;

        // replace all '/' by '_'
        l_uiTempPathSize = l_cTempPath.find_first_of("/");
        while (std::string::npos != l_uiTempPathSize) {
          l_cTempPath[l_uiTempPathSize] = '_';
          l_uiTempPathSize = l_cTempPath.find_first_of("/", l_uiTempPathSize + 1);
        }

        switch (ePersistCategory) {
          case eFrameworkunifiedFactoryData: {
              l_cNewPath.append(FACTORYDATADIR);
            }
            break;

          case eFrameworkunifiedFactoryCustomerData: {
              l_cNewPath.append(FACTORYCUSTOMERDATADIR);
            }
            break;

          case eFrameworkunifiedDealerData: {
              l_cNewPath.append(DEALERDATADIR);
            }
            break;

          case eFrameworkunifiedUserData:
          default: {
              l_cNewPath.append(USERDATADIR);
              l_cNewPath.append(ALLUSERAPPDATADIR);
            }
            break;
        }

        l_cNewPath.append(IMMEDIATEDATADIR);
        l_cNewPath.append(l_cTempPath);

        if (0 == access(l_cNewPath.c_str(), F_OK)) {
          l_cPath.assign(l_cNewPath);
        } else {
          l_cOldPath.append(l_cTempPath);

          if (0 == access(l_cOldPath.c_str(), F_OK)) {
            l_cPath.assign(l_cOldPath);
          } else {
            l_eStatus = eFrameworkunifiedStatusFileLoadError;
          }
        }

        if (eFrameworkunifiedStatusOK ==  l_eStatus) {
          size_t PersistData;
          // Open and read from a file
          l_fFilePtr = fopen(l_cPath.c_str(), "re");
          if (NULL != l_fFilePtr) {
            // read notification info structure
            PersistData = fread(&l_ptImmediatePersistentData, 1, sizeof(l_ptImmediatePersistentData), l_fFilePtr);
            if (PersistData == sizeof(l_ptImmediatePersistentData)) {
              *uiDataSize = l_ptImmediatePersistentData.dataSize;
            } else {
              *uiDataSize = static_cast<UI_32>(PersistData);
            }
            fclose(l_fFilePtr);
          }
        }

        sem_post(&g_FileAccessSemaphore);
      } else {
        l_eStatus = eFrameworkunifiedStatusErrOther;
      }

      sem_destroy(&g_FileAccessSemaphore);
    } else {
      l_eStatus = eFrameworkunifiedStatusErrOther;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// SynchronousMovePersistentData
/// API to move all the immediate notification data from f_cSrcDir directory to
/// f_cDestDir directory
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SynchronousMovePersistentData(PCSTR f_cSrcDir,
                                         PCSTR f_cDestDir) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != f_cSrcDir && NULL != f_cDestDir) {
    if (0 == sem_init(&g_FileAccessSemaphore, 1, 1)) {
      if (sem_wait(&g_FileAccessSemaphore) == 0) {  // LCOV_EXCL_BR_LINE 5: sem_init's error case
        if (DoesDirecotryExist(f_cSrcDir)) {  // LCOV_EXCL_BR_LINE 5: sem_wait's error case
          if (!DoesDirecotryExist(f_cDestDir)) {
            l_eStatus = CreateDirectory(f_cDestDir);
          }

          if (eFrameworkunifiedStatusOK == l_eStatus) {
            DIR *l_pDir = NULL;
            struct dirent *l_pdirent = NULL;

            std::string l_cSrcPath = "";
            std::string l_cDestPath = "";

            if (NULL != (l_pDir = opendir(f_cSrcDir))) {
              while (NULL != (l_pdirent = readdir(l_pDir))) {
                if (0 != std::strcmp(l_pdirent->d_name, ".") &&
                    0 != std::strcmp(l_pdirent->d_name, "..")) {
                  l_cSrcPath.assign(f_cSrcDir);
                  l_cSrcPath.append(l_pdirent->d_name);

                  l_cDestPath.assign(f_cDestDir);
                  l_cDestPath.append(l_pdirent->d_name);

                  // move the old app user data to the UserData fodler
                  if (0 != rename(l_cSrcPath.c_str(), l_cDestPath.c_str())) {
                    l_eStatus = eFrameworkunifiedStatusFail;
                  }
                }
              }
              closedir(l_pDir);

              sync();
            } else {
              l_eStatus = eFrameworkunifiedStatusFileLoadError;
            }
          }
        }

        sem_post(&g_FileAccessSemaphore);
      } else {
        l_eStatus = eFrameworkunifiedStatusFail;
      }

      sem_destroy(&g_FileAccessSemaphore);
    } else {
      l_eStatus = eFrameworkunifiedStatusFail;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return l_eStatus;
}
