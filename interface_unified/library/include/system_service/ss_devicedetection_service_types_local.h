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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SystemServicesIf
/// \brief    This file supports the Device Detection service.
///
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 */
#ifndef SS_DEVDETECT_TYPES_LOCAL_H_  // NOLINT (build/header_guard)
#define SS_DEVDETECT_TYPES_LOCAL_H_

/**
 * \~english Service state.
 */
typedef enum _ServiceStatus {
  eInit,        //!< \~english Initialize.
  ePre,         //!< \~english In-vehicle.
  eBackground,  //!< \~english Background boot.
  eStart,       //!< \~english Normal boot.
  eStop,        //!< \~english Stop.
} ESERVICESTATUS;

typedef enum _USBMount {
  SS_DEV_RO,  // Read Only
  SS_DEV_RW  // Read/Write
} SS_DEV_USB_REMOUNT;

#endif /* SS_DEVDETECT_TYPES_LOCAL_H_ */  // NOLINT (build/header_guard)
