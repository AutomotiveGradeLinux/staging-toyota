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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NS_NPPService
/// .
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_NPPService
/// \brief    This file contain declaration of class CImmediatePersistenceWorker and holds the
///           implementation for worker thread.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOR_PERSISTENCE_WORKER_THREAD_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOR_PERSISTENCE_WORKER_THREAD_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/ns_timer_if.hpp>

#include "ns_npp_types.h"

////////////////////////////////////////////////////////////////////////////////////////////////
/// NSP_NorPersistenceWorkerOnStart
/// Callback method on start of worker thread.
///
/// \param [IN] hthread
///      HANDLE - Thread Handle
///
/// \return EFrameworkunifiedStatus
//      EFrameworkunifiedStatus - success or failure status
///
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSPNorPersistenceWorkerOnStart(HANDLE hthread);

////////////////////////////////////////////////////////////////////////////////////////////////
/// NSPNorPersistenceWorkerOnStop
/// Callback method on stopping of worker thread.
///
/// \param [IN] hthread
///      HANDLE - Thread Handle
///
/// \return EFrameworkunifiedStatus
//      EFrameworkunifiedStatus - success or failure status
///
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSPNorPersistenceWorkerOnStop(HANDLE hthread);

// Forward Declaration for class
class CNotificationsToPersist;


/**
 *  This class handles the immediate persistence notification related requests.
 *
 */
class CNorPersistenceWorker {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CNorPersistenceWorker
  /// Constructor of class CNorPersistenceWorker
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNorPersistenceWorker();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CNorPersistenceWorker
  /// Destructor of class CNorPersistenceWorker
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CNorPersistenceWorker();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// OnNorPersistenceTimerStart
  /// When immediate persistence notification publish request is received by NPPService. It sends a
  /// NOR_PERSISTENCE_TIMER_START command to the worker thread and this function gets called.
  /// This function starts the respective timer of the notification. Timeout of timer writes
  /// corresponding notification data to persistent memory.
  ///
  /// \param [IN] f_hthread
  ///      HANDLE - Thread Handle
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OnNorPersistenceTimerStart(HANDLE hthread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// RegisterImmediatePersistNotification
  /// When immediate persistence notification register request is received by NPPService. It sends a
  /// NOR_PERSISTENCE_REGISTER command to the worker thread and this function gets called.
  /// Registration with worker thread is required because only child thread will be responsible for
  /// data saving related stuff. It creates a timer corresponding to the notification.
  ///
  /// \param [IN] f_hthread
  ///      HANDLE - Thread Handle
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus RegisterImmediatePersistNotification(HANDLE hthread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// UnregisterImmediatePersistNotification
  /// When immediate persistence notification unregister request is received by NPPService. It sends a
  /// NOR_PERSISTENCE_UNREGISTER command to the worker thread and this function gets called.
  /// It deletes a timer corresponding to the notification.
  ///
  /// \param [IN] f_hthread
  ///      HANDLE - Thread Handle
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus UnregisterImmediatePersistNotification(HANDLE hthread);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// OnShutdown
  /// This API stops all the immediate notification timer.
  /// This API immediately writes the data to persistent memory if the timer is runnning,
  /// irrespective of delay unless the data .
  ///
  /// \param [IN] f_hthread
  ///      HANDLE - Thread Handle
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OnShutdown(HANDLE hthread);


  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// OnCategoryChange
  /// This API updates the category of immediate persistence notification.
  ///
  /// \param [IN] f_hthread
  ///        HANDLE - Thread Handle
  ///
  /// \return EFrameworkunifiedStatus
  //          EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OnCategoryChange(HANDLE hthread);
};

/**
 *  CTimerHelper class maps the timer id and corresponding data.
 *
 */
class CTimerHelper: public NSTimer {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CTimerHelper
  /// Constructor of class CTimerHelper
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CTimerHelper();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// ~CTimerHelper
  /// Destructor of class CTimerHelper
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CTimerHelper();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// GetCmdId
  /// Returns the timer id of the timer.
  ///
  /// \param [IN] none
  ///
  /// \return UI_32
  //      UI_32 - Timer id of the timer
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_32 GetCmdId();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// OnTimeOut
  /// This function gets called when timer expires. This function writes the notification data in
  /// persistent memory.
  ///
  /// \param [IN] f_hthread
  ///      HANDLE - Thread Handle
  ///
  /// \return EFrameworkunifiedStatus
  //      EFrameworkunifiedStatus - success or failure status
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OnTimeOut(HANDLE hthread);

  // member variables
  PUI_8 m_pui8HeaderAndData;  // header and corresponding data of the notfn stored as part of timer

 private:
  UI_32 m_uiCmdId;  // command id associated with timer
};

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_NOR_PERSISTENCE_WORKER_THREAD_H_
