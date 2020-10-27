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
 * @file ss_devicedetection_service_ifc.h
 * @brief This file supports the Device Detection service interface.
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup device_detection_service
 *  @ingroup system_service
 *  @{
 */
/** @addtogroup device_manager
 *  @ingroup device_detection_service
 *  @{
 */

#ifndef SS_DEVICE_DETECTION_SERVICE_IFC_H_  // NOLINT (build/header_guard)
#define SS_DEVICE_DETECTION_SERVICE_IFC_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>
#include "system_service/ss_devicedetection_service_protocol.h"

// CID(Card Identification Register Data)of SD/MMC card
typedef struct T_cid {
  UI_8 mid;    // Manufacture ID
  UI_8 oid[3];   // OEM/Application ID
  UI_8 pnm[6];   // Product name
  UI_8 prv;    // Product revision
  UI_32 psn;    // Product serial number
  UI_16 mdt;    // Manufacture date code
  UI_8 cr7cs;    // CRC7 Check sum
} Tcid;

/**
 * @class DeviceDetectionServiceIf
 * \~english @brief Device detection
 * \~english @par   Brief Introduction
 *        Class to provide the device detection function
 *
 */
class DeviceDetectionServiceIf {
 public:
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup DeviceDetectionServiceIf
  /// \~english @par Summary
  ///       Constructor of DeviceDetectionServiceIf class.
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
  ///       To generate a DeviceDetectionServiceIf class. \n
  ///       Member variables (m_hApp, m_hService, m_hSession) to be NULL initialization. \n
  ///       After the constructor, be sure to call Initialize method.
  /// \~english @see  ~DeviceDetectionServiceIf, Initialize
  ////////////////////////////////////////////////////////////////////////////////////
  DeviceDetectionServiceIf();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~DeviceDetectionServiceIf
  /// \~english @par Summary
  ///       Destructor of DeviceDetectionServiceIf class.
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
  ///       To delete a DeviceDetectionServiceIf class. \n
  ///       Session is the case still open, to close the session.\n
  ///       Service is the case still open, to close the service.
  ///       Member variables (m_hApp, m_hService, m_hSession) to be NULL initialization.
  /// \~english @see  DeviceDetectionServiceIf
  ////////////////////////////////////////////////////////////////////////////////////
  virtual ~DeviceDetectionServiceIf();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Initialize
  /// \~english @par Summary
  ///       API to initialize the DeviceDetectionServiceIf class.
  /// \~english @param [in] hApp
  ///        HANDLE   - HANDLE for Application
  /// \~english @retval TRUE   Success
  /// \~english @retval FALSE  Failure
  /// \~english @par Preconditions
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       Conditions for return FALSE is as follows.
  ///       - NULL handle specified in the argument
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       Application to use the services of device_detection_service is, after generating a \n
  ///       DeviceDetectionServiceIf class, which is a method to be executed first. \n
  ///       Member variable(m_hApp) of DeviceDetectionServiceIf class initialized with hApp \n
  ///       that has been specified by the argument.
  /// \~english @see  DeviceDetectionServiceIf
  ////////////////////////////////////////////////////////////////////////////////////
  BOOL Initialize(HANDLE hApp);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NotifyOnDeviceDetectionAvailability
  /// \~english @par Summary
  ///       API to set the Callback function for Availability notification of Device Detection Service.
  /// \~english @param [in] f_pCallBackFn
  ///        CbFuncPtr   - Pointer to function callback for Device Detection Service Availability Notification
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Callback function pointer specified by the argument is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Can not get the App/Thread name from HANDLE in the class member variables (m_hApp).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE (m_hApp) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldParam]
  ///       - The acquisition of the buffer that stores the Callback information specified in the argument failed.
  ///       [eFrameworkunifiedStatusInvldParam]
  ///       - Can not be acquired memory in the acquisition of the buffer of the message for transmission (malloc).
  ///       [eFrameworkunifiedStatusFail]
  ///       - Message queue HANDLE is NULL in the HANDLE of the class member variables (m_hApp). [eFrameworkunifiedStatusInvldParam]
  ///       - Message queue HANDLE in HANDLE (m_hApp) of the class member variables are not appropriate(which is
  ///       invalid). [eFrameworkunifiedStatusInvldParam]
  ///       - It is not appropriate message queue name of HANDLE (m_hApp) in the class member variables(The name is
  ///       NULL, more than 20byte). [eFrameworkunifiedStatusInvldHandle]
  ///       - Is not a transmission type message queue type of HANDLE (m_hApp) in the class member variables.
  ///       [eFrameworkunifiedStatusInvldHndlType]
  ///       - Session message queue is full to SS_DeviceDetectionService. [eFrameworkunifiedStatusMsgQFull]
  ///       - It is invalid transmission file descriptor of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message for
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - Incorrect size of the transmit buffer of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Any errors occur during the transmission of a session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message for SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Method x Pub-Sub
  /// \~english @par Detail
  ///       Which is a method to register a Callback function for the initialization of the \n
  ///       device_detection_service receives the timing at which the start (open) is possible \n
  ///       of completed service.\n
  ///       (Native Service of API: Use the FrameworkunifiedSubscribeNotificationsWithCallback, to register \n
  ///        a Callback function for Availability notification of Device Detection Service.)
  /// \~english @see  DetachDeviceDetectionAvailabilityCb
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotifyOnDeviceDetectionAvailability(CbFuncPtr f_pCallBackFn);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NotifyOnOpenSessionAck
  /// \~english @par Summary
  ///       API to set the Callback function for OpenSessionRequest response from the Device Detection Service.
  /// \~english @param [in] f_pCallBackFn
  ///        CbFuncPtr   - Callback function pointer for receiving the response of OpenSessionRequest
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @par Preconditions
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Callback function pointer specified by the argument is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE (m_hApp) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldParam]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       To register the Callback function for receiving a response of completion of OpenSessionRequest to
  ///       Dispatcher. \n
  ///       (Native Service of API: Use the FrameworkunifiedAttachCallbackToDispatcher, to register a Callback function for receiving
  ///       \n
  ///        a response of completion of OpenSessionRequest to Dispatcher.) \n
  ///       On successful session establishment, the client can register for device information.
  /// \~english @see  DetachOpenSessionAckCb
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotifyOnOpenSessionAck(CbFuncPtr f_pCallBackFn);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NotifyOnCloseSessionAck
  /// \~english @par Summary
  ///       API to set the Callback function for CloseSessionRequest response from the Device Detection Service.
  /// \~english @param [in] f_pCallBackFn
  ///        CbFuncPtr   - Callback function pointer for receiving the response of CloseSessionRequest
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @par Preconditions
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Callback function pointer specified by the argument is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE (m_hApp) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldParam]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       To register the Callback function for receiving a response of completion of CloseSessionRequest to
  ///       Dispatcher. \n
  ///       (Native Service of API: Use the FrameworkunifiedAttachCallbackToDispatcher, to register a Callback function for receiving
  ///       \n
  ///        a response of completion of CloseSessionRequest to Dispatcher.) \n
  /// \~english @see  DetachCloseSessionAckCb
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus NotifyOnCloseSessionAck(CbFuncPtr f_pCallBackFn);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup OpenSessionRequest
  /// \~english @par Summary
  ///       API to perform the generation of the service/session of Device Detection Service
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - SS_DeviceDetectionService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done, and Availability of Device Detection Service is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Service of Device Detection Service is not Availability state. [eFrameworkunifiedStatusFail]
  ///       - Service of Device Detection Service can not be opened (can not start). [eFrameworkunifiedStatusFail]
  ///       - SS_DeviceDetectionService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Is invalid transmission file descriptor of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Method x Pub-Sub
  /// \~english @par Detail
  ///       And generate the service of device_detection_service, which is a method for generating a session.\n
  ///       (To send the command of PROTOCOL_OPEN_SESSION_REQ to device_detection_service.)\n
  ///       If you already service has been created, once to close the service.
  /// \~english @see  CloseSessionRequest
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus OpenSessionRequest();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup DecodeOpenSessionResponse
  /// \~english @par Summary
  ///       API to perform the generation of the service/session of Device Detection Service
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @par Preconditions
  ///       - SS_DeviceDetectionService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done, and Availability of Device Detection Service is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Incorrect PROTOCOL_OPEN_SESSION_ACK message length of Device Detection Service. [eFrameworkunifiedStatusFail]
  ///       - PROTOCOL_OPEN_SESSION_ACK message of Device Detection Service can not be acquired. [eFrameworkunifiedStatusFail]
  ///       - Can not session generation of message transmission to Device Detection Service. [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       Get the session initiation response message(PROTOCOL_OPEN_SESSION_ACK) of Device Detection \n
  ///       Service, and get the session ID from the message. \n
  ///       Acquired session ID, (after opening the message transmission queue) created after a session \n
  ///       of message transmission, hold as the session information in the session HANDLE (m_hSession).\n
  ///       of the class member variable.
  /// \~english @see  OpenSessionRequest, CloseSessionRequest
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus DecodeOpenSessionResponse();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup RegisterForDeviceDetectionEvent
  /// \~english @par Summary
  ///       API to register a Callback function for receiving the timing of when an event occurs in the device.
  /// \~english @param  [in] f_eDevDetectEvent
  ///        SS_DeviceDetectionServerEvents   - Event type (USB/SD/...etc  event)
  /// \~english @param  [in] f_pCallBackFn
  ///        CbFuncPtr   -  Pointer to a callback function that corresponds to the event
  /// \~english @param  [in] pfilepath
  ///        PCSTR   - Pointer to the file path name(Non-support:Be NULL)
  /// \~english @par
  ///       Event type (enum values of SS_DeviceDetectionServerEvents)
  ///       - SS_DEV_DETECT_ANY_USB_EV              : Notify the insertion and mount path of USB MassStorage class(USB
  ///       mass storage is subject)
  ///       - SS_DEV_DETECT_ANY_SD_EV               : Notify the insertion and mount path of SD card
  ///       - SS_DEV_INFO_ANY_USB_EV                : Notify the USB device insertion and removal detection(All USB
  ///       devices are subject)
  ///       - SS_DEV_DETECT_ANY_DISC_EV             : Notify the insertion of DISC(BD, DVD, CD) and mount path of block
  ///       devices
  ///       - SS_DEV_DETECT_ANY_USB_NCM_EV          : Notify the insertion and network IF name of the USB NCM class
  ///       device(for MirrorLink)
  ///       - SS_DEV_DETECT_ANY_USB__CWORD57__EV         : Notify the insertion and removal and the device file name of the
  ///       _CWORD57_ device(Rather than the _CWORD57_ only class, the device is subject)
  ///       - SS_DEV_DETECT_ANY_USB_ACM_EV          : Notify the insertion and removal and device file name of the USB
  ///       ACM class device(for DCM device)
  ///       - SS_DEV_DETECT_ANY_USB_DEV_MNG_EV      : Notify the insertion and removal and device file name of the USB
  ///       DeviceManagementModel class(for DCM device)
  ///       - SS_DEV_DETECT_ANY_USB_VEHICLE_CTRL_EV : Notify the insertion and removal and device file name of the USB
  ///       Vehicle Control class(for DCM device)
  ///       - SS_DEV_DETECT_ANY_USB_DSRC_APP_EV     : Notify the insertion and removal and device file name of the USB
  ///       DSRC Application class(for DSRC device)
  ///       - SS_DEV_DETECT_ANY_USB_DSRC_CTRL_EV    : Notify the insertion and removal and device file name of the USB
  ///       DSRC Control class(for DSRC device)
  ///       - SS_DEV_DETECT_ANY_USB_IR_VICS_DATA_EV : Notify the insertion and removal and device file name of the USB
  ///       IR-VICS Data class(for DSRC device)
  ///       - SS_DEV_ERR_USB_OVER_CURRENT_EV        : Notify the overcurrent detection error of USB
  ///       - SS_DEV_DETECT_ANY_USB_MTP_EV          : Notify the insertion and mount path of USB MTP class device
  ///       - SS_DEV_INFO_ANY_USB_NOTIFY_EV         : Notify the connection of non-compliant USB device
  ///       - SS_DEV_DETECT_ANY_USB__CWORD57__ROLE_EV    : Notify the insertion and removal of the _CWORD57_ device after
  ///       RoleSW(Rather than the _CWORD57_ only class, the device is subject)
  ///       - SS_DEV_INFO_SD_FORMAT_COMP_EV         : Notify the SD format results
  ///       - SS_DEV_DETECT_ANY_USB_DCM_NCM_EV      : Notify the insertion and removal and network IF name of the USB
  ///       NCM class device(for DCM)
  ///       - SS_DEV_INFO_ANY_USB_DVDP_EV           : Notify the insertion and removal of the DVD-P device
  /// \~english @par
  ///       For SS_DEV_INFO_ANY_USB_NOTIFY_EV, it is also notified in the event the following conditions.
  ///       - When the enumeration failed.
  ///       - When it detects the current excess of the USB device connected to the bus power HUB.
  ///       - When it is detected that the number of the USB device has exceeded the number of support.
  ///       - When it is detected that the HUB is connected beyond the HUB number that supports.
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_DeviceDetectionService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done, and Availability of Device Detection Service is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Callback function pointer specified by the argument is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
  ///       - HANDLE in the class member variables (m_hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldParam]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_DeviceDetectionService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_DeviceDetectionService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_DeviceDetectionService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_DeviceDetectionService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Pub-Sub x Pub-Sub
  /// \~english @par Detail
  ///       Which is a method to register a callback function for receiving the timing of the \n
  ///       occurrence of an event in the device. \n
  /// \~english @see  UnRegisterForDeviceDetectionEvent
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus RegisterForDeviceDetectionEvent(
      SS_DeviceDetectionServerEvents f_eDevDetectEvent, CbFuncPtr f_pCallBackFn,
      PCSTR pfilepath = NULL);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup UnRegisterForDeviceDetectionEvent
  /// \~english @par Summary
  ///       API to release a Callback function that corresponds to the event notification from the device
  /// \~english @param  [in] f_eDevDetectEvent
  ///        SS_DeviceDetectionServerEvents   - Event notification type (USB/SD/...etc  event)
  /// \~english @par
  ///       Releasable event notification type (enum values of SS_DeviceDetectionServerEvents)
  ///       - SS_DEV_DETECT_ANY_USB_EV              : Notify the insertion and mount path of USB MassStorage class(USB
  ///       mass storage is subject)
  ///       - SS_DEV_DETECT_ANY_SD_EV               : Notify the insertion and mount path of SD card
  ///       - SS_DEV_INFO_ANY_USB_EV                : Notify the USB device insertion and removal detection(All USB
  ///       devices are subject)
  ///       - SS_DEV_DETECT_ANY_DISC_EV             : Notify the insertion of DISC(BD, DVD, CD) and mount path of block
  ///       devices
  ///       - SS_DEV_DETECT_ANY_USB_NCM_EV          : Notify the insertion and network IF name of the USB NCM class
  ///       device(for MirrorLink)
  ///       - SS_DEV_DETECT_ANY_USB__CWORD57__EV         : Notify the insertion and removal and the device file name of the
  ///       _CWORD57_ device(Rather than the _CWORD57_ only class, the device is subject)
  ///       - SS_DEV_DETECT_ANY_USB_ACM_EV          : Notify the insertion and removal and device file name of the USB
  ///       ACM class device(for DCM device)
  ///       - SS_DEV_DETECT_ANY_USB_DEV_MNG_EV      : Notify the insertion and removal and device file name of the USB
  ///       DeviceManagementModel class(for DCM device)
  ///       - SS_DEV_DETECT_ANY_USB_VEHICLE_CTRL_EV : Notify the insertion and removal and device file name of the USB
  ///       Vehicle Control class(for DCM device)
  ///       - SS_DEV_DETECT_ANY_USB_DSRC_APP_EV     : Notify the insertion and removal and device file name of the USB
  ///       DSRC Application class(for DSRC device)
  ///       - SS_DEV_DETECT_ANY_USB_DSRC_CTRL_EV    : Notify the insertion and removal and device file name of the USB
  ///       DSRC Control class(for DSRC device)
  ///       - SS_DEV_DETECT_ANY_USB_IR_VICS_DATA_EV : Notify the insertion and removal and device file name of the USB
  ///       IR-VICS Data class(for DSRC device)
  ///       - SS_DEV_ERR_USB_OVER_CURRENT_EV        : Notify the overcurrent detection error of USB
  ///       - SS_DEV_DETECT_ANY_USB_MTP_EV          : Notify the insertion and mount path of USB MTP class device
  ///       - SS_DEV_INFO_ANY_USB_NOTIFY_EV         : Notify the connection of non-compliant USB device
  ///       - SS_DEV_DETECT_ANY_USB__CWORD57__ROLE_EV    : Notify the insertion and removal of the _CWORD57_ device after
  ///       RoleSW(Rather than the _CWORD57_ only class, the device is subject)
  ///       - SS_DEV_INFO_SD_FORMAT_COMP_EV         : Notify the SD format results
  ///       - SS_DEV_DETECT_FILEPATHNAME_EV         : Notify the specified file access in pfilepath
  ///       - SS_DEV_DETECT_ANY_USB_DCM_NCM_EV      : Notify the insertion and removal and network IF name of the USB
  ///       NCM class device(for DCM)
  ///       - SS_DEV_INFO_ANY_USB_DVDP_EV           : Notify the insertion and removal of the DVD-P device
  /// \~english @par
  ///       For SS_DEV_INFO_ANY_USB_NOTIFY_EV, it is also notified in the event the following conditions.
  ///       - When the enumeration failed.
  ///       - When it detects the current excess of the USB device connected to the bus power HUB.
  ///       - When it is detected that the number of the USB device has exceeded the number of support.
  ///       - When it is detected that the HUB is connected beyond the HUB number that supports.
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error(It failed to open/allocation of shared memory)
  /// \~english @par Preconditions
  ///       - SS_DeviceDetectionService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done, and Availability of Device Detection Service is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
  ///       - HANDLE in the class member variables (m_hApp) is not appropriate (which is invalid) [eFrameworkunifiedStatusInvldParam]
  ///       - The service can not be determined from the HANDLE in the class member variables (m_hApp). [eFrameworkunifiedStatusFail]
  ///       - HANDLE in the class member variables (m_hSession) is not appropriate. (which is invalid)
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - SS_DeviceDetectionService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Message Queue HANDLE for the session for SS_DeviceDetectionService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Message Queue HANDLE for the session for SS_DeviceDetectionService are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Session message queue name to SS_DeviceDetectionService is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Is invalid transmission file descriptor of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Pub-Sub x Pub-Sub
  /// \~english @par Detail
  ///       Which is a method to release a callback function for receiving the timing of the \n
  ///       occurrence of an event in the device. \n
  /// \~english @see  RegisterForDeviceDetectionEvent
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus UnRegisterForDeviceDetectionEvent(
      SS_DeviceDetectionServerEvents f_eDevDetectEvent);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CloseSessionRequest
  /// \~english @par Summary
  ///       API to perform the destruction of services/session of Device Detection Service
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - SS_DeviceDetectionService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done, and Availability of Device Detection Service is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hService/m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - SS_DeviceDetectionService message queue for the session is full. [eFrameworkunifiedStatusMsgQFull]
  ///       - Is invalid transmission file descriptor of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message to
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - It is an incorrect size of the transmit buffer of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Some sort of error has occurred at the time of transmission of the session message to
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Method x Method
  /// \~english @par Detail
  ///       And destruct the service of device_detection_service, which is a method for destructing a session.\n
  ///       (To send the command of PROTOCOL_CLOSE_SESSION_REQ to device_detection_service.)\n
  /// \~english @see  OpenSessionRequest
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus CloseSessionRequest();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup DetachDeviceDetectionAvailabilityCb
  /// \~english @par Summary
  ///       API to release the Callback function for Availability notification of Device Detection Service.
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done, and Availability of Device Detection Service is TRUE.
  ///       - The call-back function for Availability notification has been registered.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Can not get the App/Thread name from HANDLE in the class member variables (m_hApp).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE (m_hApp) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldParam]
  ///       - Can not be acquired memory in the acquisition of the buffer of the message for transmission (malloc).
  ///       [eFrameworkunifiedStatusFail]
  ///       - Message queue HANDLE is NULL in the HANDLE of the class member variables (m_hApp). [eFrameworkunifiedStatusInvldParam]
  ///       - Message queue HANDLE in HANDLE (m_hApp) of the class member variables are not appropriate(which is
  ///       invalid). [eFrameworkunifiedStatusInvldParam]
  ///       - It is not appropriate message queue name of HANDLE (m_hApp) in the class member variables(The name is
  ///       NULL, more than 20byte). [eFrameworkunifiedStatusInvldHandle]
  ///       - Is not a transmission type message queue type of HANDLE (m_hApp) in the class member variables.
  ///       [eFrameworkunifiedStatusInvldHndlType]
  ///       - Callback function for Availability notification is not registered. [eFrameworkunifiedStatusFail]
  ///       - Session message queue is full to SS_DeviceDetectionService. [eFrameworkunifiedStatusMsgQFull]
  ///       - It is invalid transmission file descriptor of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message for
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - Incorrect size of the transmit buffer of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Any errors occur during the transmission of a session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message for SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       Is a method for releasing a Callback function registered to receive the timing \n
  ///       initialization is completed device_detection_service
  ///       (Native Service of API: Use the FrameworkunifiedUnsubscribeNotificationWithCallback, to release \n
  ///        a Callback function for Availability notification of Device Detection Service.)
  /// \~english @see  NotifyOnDeviceDetectionAvailability
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus DetachDeviceDetectionAvailabilityCb();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup DetachOpenSessionAckCb
  /// \~english @par Summary
  ///       API to release the Callback function for OpenSessionRequest response from the Device Detection Service.
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @par Preconditions
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE (m_hApp) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldParam]
  ///       - Not Callback function information of the release target is present. [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       To cancel the Callback function for receiving a response of completion of OpenSessionRequest from
  ///       Dispatcher. \n
  ///       (Native Service of API: Use the FrameworkunifiedDetachCallbackFromDispatcher, to cancel a Callback function for receiving
  ///       \n
  ///        a response of completion of OpenSessionRequest from Dispatcher.) \n
  /// \~english @see  NotifyOnOpenSessionAck
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus DetachOpenSessionAckCb();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup DetachCloseSessionAckCb
  /// \~english @par Summary
  ///       API to release the Callback function for CloseSessionRequest response from the Device Detection Service.
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @par Preconditions
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE (m_hApp) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusInvldParam]
  ///       - Not Callback function information of the release target is present. [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       To cancel the Callback function for receiving a response of completion of CloseSessionRequest from
  ///       Dispatcher. \n
  ///       (Native Service of API: Use the FrameworkunifiedDetachCallbackFromDispatcher, to cancel a Callback function for receiving
  ///       \n
  ///        a response of completion of CloseSessionRequest from Dispatcher.) \n
  /// \~english @see  NotifyOnCloseSessionAck
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus DetachCloseSessionAckCb();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup MountSD
  /// \~english @par Summary
  ///       API to mount the specified SD partition
  /// \~english @param  [in] part_num
  ///        uint8_t   - Partition number of SD card(From 0 to 3)
  /// \~english @param  [out] already_mounted
  ///        BOOL*   -  Mount existence result
  /// \~english @par
  ///       - TRUE  : Already mount
  ///       - FALSE : Mount request is accepted. Wait for mount notification.
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusAccessError Access error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - SS_DeviceDetectionService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done, and Availability of Device Detection Service is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - SD card partition number (part_num) exceeds the 3 specified by the argument. [eFrameworkunifiedStatusInvldParam]
  ///       - Can not have access to the device file for the SD. [eFrameworkunifiedStatusAccessError]
  ///       - HANDLE (m_hSession) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - Message queue HANDLE is NULL in the HANDLE of the class member variables (m_hSession).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) in the message queue name is NULL.
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Message queue HANDLE in HANDLE (m_hSession) of the class member variables are not appropriate(which is
  ///       invalid). [eFrameworkunifiedStatusInvldHandle]
  ///       - It is not appropriate message queue name of HANDLE (m_hSession) in the class member variables(The name is
  ///       NULL, more than 20byte). [eFrameworkunifiedStatusInvldHandle]
  ///       - Is not a transmission type message queue type of HANDLE (m_hSession) in the class member variables.
  ///       [eFrameworkunifiedStatusInvldHndlType]
  ///       - Session message queue is full to SS_DeviceDetectionService. [eFrameworkunifiedStatusMsgQFull]
  ///       - It is invalid transmission file descriptor of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message for
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - Incorrect size of the transmit buffer of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Any errors occur during the transmission of a session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message for SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Method only
  /// \~english @par Detail
  ///       The API is a method used to mount the specified partition (area).\n
  ///       The target device is an SD card.
  ///       Service(Thread) to call this method is not limited. Therefore, \n
  ///       to be utilized as a mismatch does not occur between the service \n
  ///       to use the SD card. \n
  ///       SD card in the specified partition, already returned eFrameworkunifiedStatusOK \n
  ///       if mount existence results (already_mounted) in the TRUE that is mounted.\n
  ///       If it is not already mounted mount existence results (already_mounted) \n
  ///       and to FALSE, then carry out the SD of the mount request to Device \n
  ///       Detection Service.
  /// \~english @see  UmountSD, RegisterForDeviceDetectionEvent
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus MountSD(uint8_t part_num, BOOL *already_mounted);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup UmountSD
  /// \~english @par Summary
  ///       API to mount the specified SD partition
  /// \~english @param  [in] part_num
  ///        uint8_t   - Partition number of SD card(From 0 to 3)
  /// \~english @param  [out] already_umounted
  ///        BOOL*   -  Mount existence result
  /// \~english @par
  ///       - TRUE  : Already umount
  ///       - FALSE : Umount request is accepted. Wait for umount notification.
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusAccessError Access error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - SS_DeviceDetectionService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done, and Availability of Device Detection Service is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - SD card partition number (part_num) exceeds the 3 specified by the argument. [eFrameworkunifiedStatusInvldParam]
  ///       - Can not have access to the device file for the SD. [eFrameworkunifiedStatusAccessError]
  ///       - HANDLE (m_hSession) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - Message queue HANDLE is NULL in the HANDLE of the class member variables (m_hSession).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) in the message queue name is NULL.
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Message queue HANDLE in HANDLE (m_hSession) of the class member variables are not appropriate(which is
  ///       invalid). [eFrameworkunifiedStatusInvldHandle]
  ///       - It is not appropriate message queue name of HANDLE (m_hSession) in the class member variables(The name is
  ///       NULL, more than 20byte). [eFrameworkunifiedStatusInvldHandle]
  ///       - Is not a transmission type message queue type of HANDLE (m_hSession) in the class member variables.
  ///       [eFrameworkunifiedStatusInvldHndlType]
  ///       - Session message queue is full to SS_DeviceDetectionService. [eFrameworkunifiedStatusMsgQFull]
  ///       - It is invalid transmission file descriptor of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message for
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - Incorrect size of the transmit buffer of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Any errors occur during the transmission of a session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message for SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Method only
  /// \~english @par Detail
  ///       The API is a method used to mount the specified partition (area).\n
  ///       The target device is an SD card.
  ///       Service(Thread) to call this method is not limited. Therefore, \n
  ///       to be utilized as a mismatch does not occur between the service \n
  ///       to use the SD card. \n
  ///       SD card in the specified partition, already returned eFrameworkunifiedStatusOK \n
  ///       if unmount existence results (already_unmounted) in the TRUE that is unmounted.
  ///       If it is not already unmounted unmount existence results (already_unmounted) \n
  ///       and to FALSE, then carry out the SD of the unmount request to Device \n
  ///       Detection Service. \n
  ///       That all of the services specifically, to call this API after the close \n
  ///       completion of the directory and file of the SD card. \n
  ///       Also, do not call this API in a state in which a directory on the SD card \n
  ///       in the current directory. (At SD drive processing threads in SS_DeviceDetectionService, \n
  ///       umount fails, error of SS_DEV_ERR_BUSY is returned as an asynchronous notification \n
  ///       events.)
  /// \~english @see  MountSD, RegisterForDeviceDetectionEvent
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus UmountSD(uint8_t part_num, BOOL *already_umounted);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup UsbVBusReset
  /// \~english @par Summary
  ///       API to reset the VBUS of the specified USB port
  /// \~english @param  [in] port
  ///        uint8_t   - USB port number
  /// \~english @param  [in] inteval
  ///        uint8_t   - interval (interval * 100 [msec])
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - SS_DeviceDetectionService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done, and Availability of Device Detection Service is TRUE.
  ///       - Checking USB port number should be done before calling this API.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Environment variable (TARGET_BOARD) is not defined. [eFrameworkunifiedStatusFail]
  ///       [eFrameworkunifiedStatusInvldParam]
  ///       - Name of the target board in the environment variable (TARGET_BOARD) is not correct. [eFrameworkunifiedStatusInvldParam]
  ///       - HANDLE (m_hSession) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - Message queue HANDLE is NULL in the HANDLE of the class member variables (m_hSession).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) in the message queue name is NULL.
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Message queue HANDLE in HANDLE (m_hSession) of the class member variables are not appropriate(which is
  ///       invalid). [eFrameworkunifiedStatusInvldHandle]
  ///       - It is not appropriate message queue name of HANDLE (m_hSession) in the class member variables(The name is
  ///       NULL, more than 20byte). [eFrameworkunifiedStatusInvldHandle]
  ///       - Is not a transmission type message queue type of HANDLE (m_hSession) in the class member variables.
  ///       [eFrameworkunifiedStatusInvldHndlType]
  ///       - Session message queue is full to SS_DeviceDetectionService. [eFrameworkunifiedStatusMsgQFull]
  ///       - It is invalid transmission file descriptor of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message for
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - Incorrect size of the transmit buffer of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Any errors occur during the transmission of a session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message for SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Fire and Forget only
  /// \~english @par Detail
  ///       The API is a method for resetting the VBUS of the specified USB port.\n
  ///       Environment variable (TARGET_BOARD) is defined in the configure options \n
  ///       specified at the time of build. \n
  ///        (Example.  TARGET_BOARD:_CWORD71__CWORD91__e2_na_t__CWORD84_2__CWORD71_) \n
  ///       Before reset USB port for DCM, the device files(dcm_acm, dcm_vc, dcm_wdm) \n
  ///       related to DCM-related must be closed. \n
  ///       *) Device file may not be created correctly when the process of Disconnect/Connect \n
  ///          without closing to work.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus UsbVBusReset(int8_t port);
  EFrameworkunifiedStatus UsbVBusReset(int8_t port, uint8_t interval);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup RoleSwStateNotify
  /// \~english @par Summary
  ///       API to notify RoleSW state
  /// \~english @param  [in] state
  ///        SS_DEV_ROLE_SW_STATE   - RoleSW state
  /// \~english @par
  ///       enum variable SS_DEV_ROLE_SW_STATE
  ///       - SS_DEV_ROLE_SW_START      : Start
  ///       - SS_DEV_ROLE_SW_BEGIN_END  : Beginning of End
  ///       - SS_DEV_ROLE_SW_END        : End
  ///       - SS_DEV_ROLE_SW_RESTART    : Restart
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
  /// \~english @retval eFrameworkunifiedStatusInvldQName Invalid Message Queue Name
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusBadConnection It can not be a socket connection
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - SS_DeviceDetectionService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done, and Availability of Device Detection Service is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE (m_hSession) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - It failed to open the message queue for messages received from SS_DeviceDetectionService. [eFrameworkunifiedStatusFail]
  ///       - It failed to buffer acquisition for messages received from SS_DeviceDetectionService(malloc failure).
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to create a sequence ID of the message to be sent to the SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - Message queue HANDLE is NULL in the HANDLE of the class member variables (m_hSession).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) in the message queue name is NULL.
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Message queue HANDLE name in the HANDLE (m_hSession) in the class member variables is NULL.
  ///       [eFrameworkunifiedStatusInvldQName]
  ///       - Message queue HANDLE name in the HANDLE (hService) in the class member variables exceeds 20byte.
  ///       [eFrameworkunifiedStatusInvldQName]
  ///       - Message queue HANDLE in HANDLE (m_hSession) of the class member variables are not appropriate(which is
  ///       invalid). [eFrameworkunifiedStatusInvldHandle]
  ///       - It is not appropriate message queue name of HANDLE (m_hSession) in the class member variables(The name is
  ///       NULL, more than 20byte). [eFrameworkunifiedStatusInvldHandle]
  ///       - Is not a transmission type message queue type of HANDLE (m_hSession) in the class member variables.
  ///       [eFrameworkunifiedStatusInvldHndlType]
  ///       - Session message queue is full to SS_DeviceDetectionService. [eFrameworkunifiedStatusMsgQFull]
  ///       - It is invalid send and receive file descriptor of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the send and receive of the session message
  ///       for SS_DeviceDetectionService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - Incorrect size of the send and receive buffer of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Any errors occur during the send and receive of a session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for send and receive of the session message for
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusErrOther]
  ///       - It failed to connect to the socket for the response message to be received from SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to wait control of the response message received from SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - Interruption by the system call (signal) has occurred in wait in the response message received from
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusFail]
  ///       - After the failure at the time of the response message received from SS_DeviceDetectionService, it failed
  ///       to re-connection of the Socket. [eFrameworkunifiedStatusBadConnection]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync x Method
  /// \~english @par Detail
  ///       By executing this API, Device Detection Service recognizes the state \n
  ///       has been switched from the USB Host due to RoleSW to USB Function. \n
  ///       The initial state of Detection is a state(SS_DEV_ROLE_SW_END:End state) \n
  ///       that has not been RoleSW. \n
  ///       (It is a state that accepts a SS_DEV_ROLE_SW_START:Start.)
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus RoleSwStateNotify(SS_DEV_ROLE_SW_STATE state);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup FormatSD
  /// \~english @par Summary
  ///       API to Format SD
  /// \~english @param  None
  /// \~english @retval eFrameworkunifiedStatusOK Success
  /// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
  /// \~english @retval eFrameworkunifiedStatusNullPointer Invalid pointer
  /// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
  /// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
  /// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
  /// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
  /// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
  /// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
  /// \~english @retval eFrameworkunifiedStatusAccessError Access error occurred
  /// \~english @retval eFrameworkunifiedStatusBadConnection It can not be a socket connection
  /// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
  /// \~english @par Preconditions
  ///       - SS_DeviceDetectionService process has been started.
  ///       - Generation/initialization of Dispatcher for the Application (FrameworkunifiedCreateDispatcherWithoutLoop etc) \n
  ///         has been done, and Availability of Device Detection Service is TRUE.
  /// \~english @par Change of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - HANDLES in the class member variables (m_hSession) is NULL. [eFrameworkunifiedStatusInvldHandle]
  ///       - Can not have access to the device file for the SD. [eFrameworkunifiedStatusAccessError]
  ///       - HANDLE (m_hSession) in the class member variables are not appropriate(which is invalid).
  ///       [eFrameworkunifiedStatusNullPointer]
  ///       - Message queue HANDLE is NULL in the HANDLE of the class member variables (m_hSession).
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - HANDLE in the class member variables (m_hSession) in the message queue name is NULL.
  ///       [eFrameworkunifiedStatusInvldHandle]
  ///       - Message queue HANDLE in HANDLE (m_hSession) of the class member variables are not appropriate(which is
  ///       invalid). [eFrameworkunifiedStatusInvldHandle]
  ///       - It is not appropriate message queue name of HANDLE (m_hSession) in the class member variables(The name is
  ///       NULL, more than 20byte). [eFrameworkunifiedStatusInvldHandle]
  ///       - Is not a transmission type message queue type of HANDLE (m_hSession) in the class member variables.
  ///       [eFrameworkunifiedStatusInvldHndlType]
  ///       - Session message queue is full to SS_DeviceDetectionService. [eFrameworkunifiedStatusMsgQFull]
  ///       - It is invalid transmission file descriptor of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrNoEBADF]
  ///       - Interruption by the system call (signal) has occurred during the transmission of the session message for
  ///       SS_DeviceDetectionService. [eFrameworkunifiedStatusErrNoEINTR]
  ///       - Incorrect size of the transmit buffer of the session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusInvldBufSize]
  ///       - Any errors occur during the transmission of a session message to SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusFail]
  ///       - It failed to shared memory access for transmission of the session message for SS_DeviceDetectionService.
  ///       [eFrameworkunifiedStatusErrOther]
  /// \~english @par Detail
  ///       The SD card inserted into the _CWORD84_ side in this API is formatted in compliance with the SD standard.\n
  ///       A result of format implementation is notified by SS_DEV_INFO_SD_FORMAT_COMP_EV events.\n
  ///       String pickled notification content and the result is as follows.\n
  ///       - SS_DEV_OK       : Format success \n
  ///       - SS_DEV_ERR_BUSY : For unmount unexecuted, format failure \n
  ///       - SS_DEV_ERR      : Format fails for any other factors
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Method x Method
  /// \~english @par
  ///       This API is able to call after the SD card umount completion and the device file close of \n
  ///       SD card is complete.\n
  ///       If the SD card umount of has not been completed, SS_DEV_ERR_BUSY is notified.\n
  ///       If the device file of the SD card has not been close, although it is the format becomes \n
  ///       the operation completed, there can be no assurance of the operation.
  /// \~english @see  MountSD, UmountSD, RegisterForDeviceDetectionEvent
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus FormatSD(void);

 private:
  HANDLE OpenService();
  UI_8 Crc7Checksum(UI_8 *data, UI_8 sz);
  HANDLE m_hApp;
  HANDLE m_hService;
  HANDLE m_hSession;
  BOOL CheckDetectEvent(SS_DeviceDetectionServerEvents f_eDevDetectEvent);
};

#endif /* SS_DEVICE_DETECTION_SERVICE_IFC_H_ */  // NOLINT (build/header_guard)

/** @}*/  // end of device_manager
/** @}*/  // end of device_detection_service
/** @}*/  // end of system_service
/** @}*/  // end of BaseSystem
