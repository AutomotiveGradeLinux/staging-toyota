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
/// \ingroup system_manager
/// \file ss_system_manager_protocol_local.h
/// \brief    This file supports the System Manager client interface.
///////////////////////////////////////////////////////////////////////////////

#ifndef __SS_SYSTEM_MANAGER_PROTOCOL_LOCAL_H__  // NOLINT (build/header_guard)
#define __SS_SYSTEM_MANAGER_PROTOCOL_LOCAL_H__

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_service_protocol.h>

/// \addtogroup   system_manager
/// @{

#define     SS_SM_MAX_MODULE_LOG_MASK_LENGTH         (50)
#define   SS_SM_MAX_MODULE_PATH_LENGTH        (256)
#define   SS_SM_MAX_MODULE_ARGS_LENGTH        (512)

//*************************************************************************
// Version Number Structure Specification:                                *
typedef UI_64 SS_VersionNumberType;
// End of Version Number Structure Specification                          *
//*************************************************************************

/// @}

#endif /* __SS_SYSTEM_MANAGER_PROTOCOL_LOCAL_H__ */  // NOLINT (build/header_guard)
// EOF
