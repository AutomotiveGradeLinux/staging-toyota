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

#ifndef DIAG_RECORD_LIBRARY_RECORD_THREAD_INCLUDE_VEHICLE_SERVICE_PFDREC_THREAD_IFC_H_
#define DIAG_RECORD_LIBRARY_RECORD_THREAD_INCLUDE_VEHICLE_SERVICE_PFDREC_THREAD_IFC_H_

/**
 * @file pfdrec_thread_ifc.h
 * @brief diag_record thread header file
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup diag_record
 *  @ingroup vehicle_service
 *  @{
 */

#include <native_service/frameworkunified_types.h>

/**
 * @class CPFDRECThread
 * \~english @brief diag_record thread
 * \~english @par   Brief Introduction
 *        Class to provide the trace data record function
 *
 */
class CPFDRECThread {
 public:
    /////////////////////////////////////////////////////////////////////////////////////
    /// \ingroup CPFDRECThread
    /// \~english @par Summary
    ///       Constructor of CPFDRECThread class.
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
    ///       To generate a CPFDRECThread class. \n
    ///       Member variables initialize. \n
    ///       After the constructor, be sure to call Initialize method.
    /// \~english @see  ~CPFDRECThread, Initialize
    ////////////////////////////////////////////////////////////////////////////////////
    CPFDRECThread();

    /////////////////////////////////////////////////////////////////////////////////////
    /// \ingroup ~CPFDRECThread
    /// \~english @par Summary
    ///       Destructor of CPFDRECThread class.
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
    ///       To delete a CPFDRECThread class.
    /// \~english @see  CPFDRECThread
    ////////////////////////////////////////////////////////////////////////////////////
    virtual ~CPFDRECThread();

    /////////////////////////////////////////////////////////////////////////////////////
    /// \ingroup Initialize
    /// \~english @par Summary
    ///      - Initialize diag_record thread.
    /// \~english @param [in]    h_app    The application handle
    /// \~english @retval  eFrameworkunifiedStatusOK         Success
    /// \~english @retval  eFrameworkunifiedStatusFail       Failure
    /// \~english @par Precondition
    ///      - BackupMgr is available.
    ///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
    /// \~english @par Changes in internal status
    ///      - There is no changes in internal status when the API is called.
    /// \~english @par Conditions of processing failure
    ///      - When fail to create TN_PFDRECTHREAD. [eFrameworkunifiedStatusFail]
    /// \~english @par Details
    ///      - The API is assumed to be executed only from the SS_LoggerService.
    ///      - Initialize diag_record thread.
    /// \~english @par Classification
    ///      - Public
    /// \~english @par Type
    ///      - Sync
    /// \~english @see FrameworkunifiedCreateChildThread, FrameworkunifiedStartChildThread
    /////////////////////////////////////////////////////////////////////////////////////
    EFrameworkunifiedStatus Initialize(HANDLE h_app);

    /////////////////////////////////////////////////////////////////////////////////////
    /// \ingroup Finalize
    /// \~english @par Summary
    ///      - Finalize diag_record thread.
    /// \~english @param [in]    h_app    The application handle
    /// \~english @retval  eFrameworkunifiedStatusOK         Success
    /// \~english @par Precondition
    ///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
    ///      - The diag_record thread is running.
    /// \~english @par Changes in internal status
    ///      - There is no changes in internal status when the API is called.
    /// \~english @par Conditions of processing failure
    ///      - None
    /// \~english @par Details
    ///      - The API is assumed to be executed only from the SS_LoggerService.
    ///      - Finalize diag_record thread.
    /// \~english @par Classification
    ///      - Public
    /// \~english @par Type
    ///      - Sync
    /// \~english @see FrameworkunifiedStopChildThread
    /////////////////////////////////////////////////////////////////////////////////////
    EFrameworkunifiedStatus Finalize(HANDLE h_app);

 private:
    EFrameworkunifiedStatus Start(HANDLE h_thread);
    EFrameworkunifiedStatus Stop(HANDLE h_thread);

    EFrameworkunifiedStatus SetFuncOnOff(HANDLE h_thread);
    EFrameworkunifiedStatus GetFuncOnOff(HANDLE h_thread);
    EFrameworkunifiedStatus DeleteData(HANDLE h_thread);

    EFrameworkunifiedStatus DeleteIndividual(HANDLE f_hThread);
    EFrameworkunifiedStatus ComDataBackup(HANDLE h_thread);
    EFrameworkunifiedStatus ReadMemData(HANDLE h_thread);
    EFrameworkunifiedStatus GetDataInfo(HANDLE h_thread);
    EFrameworkunifiedStatus OutputData(HANDLE h_thread);
    EFrameworkunifiedStatus ReadData(HANDLE h_thread);
    EFrameworkunifiedStatus SearchData(HANDLE h_thread);
    EFrameworkunifiedStatus OverwriteData(HANDLE h_thread);
    EFrameworkunifiedStatus SetExtractMode(HANDLE h_thread);

    HANDLE                   m_thread;
};

/** @}*/  // end of diag_record
/** @}*/  // end of vehicle_service
/** @}*/  // end of BaseSystem

#endif  // DIAG_RECORD_LIBRARY_RECORD_THREAD_INCLUDE_VEHICLE_SERVICE_PFDREC_THREAD_IFC_H_
