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
/// \brief    This file contains the callback for Notifications and Protocol
///           commands.
///
///////////////////////////////////////////////////////////////////////////////

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup logger_service
 *  @ingroup system_service
 *  @{
 */

#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_COMMON_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_COMMON_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>

const CHAR DEBUG_LOG_PATH_DIR[] = "/tmp/frameworkunifiedlog";
const CHAR DEBUG_LOG_PATH_FN[] = "/tmp/frameworkunified_debug.log";
const CHAR TRANSMIT_LOG_PATH_FN[] = "/tmp/interfaceunified_transmit.log";
const CHAR PERFORMANCE_LOG_PATH_FN[] = "/tmp/frameworkunified_performance.log";
const CHAR DRINITIAL_LOG_PATH_FN[] = "/tmp/loggerservice_drinitial.log";
const CHAR DRLOCATION_LOG_PATH_FN[] = "/tmp/loggerservice_drlocation.log";
const CHAR KERNEL_LOG_PATH_FN[] = "/tmp/kernel.log";
const CHAR KERNEL_BOOT_LOG_PATH_FN[] = "/ramd/BS/ss/logger_service/rwdata/frameworkunifiedlog/kernelboot.log";

const UI_32 PERFORMANCELOG_FILE_SZ = 256 * 1024;

#define CLEAR_LOG_PATH_FN  "/nv/BS/ss/logger_service/rwdata/frameworkunifiedlog/"
#define NULLCHAR  '\0'

#define DEBUG_USB_PATH "/mnt/sda1"
#define KERNEL_LOG_PATH_DIR "kernellog"

template<typename C, EFrameworkunifiedStatus (C::*M)(HANDLE)>
class cpp_callback {
 public:
  static CallbackFunctionPtr set(void* pInst) {
    if (pInst == NULL)
      throw;
    ms_pInst = pInst;
    return &cpp_callback::call;
  }
  static EFrameworkunifiedStatus call(HANDLE y) {
    if (ms_pInst == NULL)
      return eFrameworkunifiedStatusFault;
    C* c = static_cast<C*>(ms_pInst);
    return (c->*M)(y);
  }
 private:
  static void* ms_pInst;
};

template<typename C, EFrameworkunifiedStatus (C::*M)(HANDLE)>
void* cpp_callback<C, M>::ms_pInst = NULL;

typedef enum _LoggerServiceStatus {
  eLSInit,
  eLSStart,
  eLSStop,
} ELOGGERSERVICESTATUS;

typedef enum {
  SS_LOGGER_KLOG_GET, /* type : get kernel.log        */
  SS_LOGGER_KBOOTLOG_CREATE /* type : create kernelboot.log */
} SS_LOGGER_KLOG_OPE_TYPE;

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup StopLoggingFunction
/// \~english @par Summary
///       Stop the logging function.
/// \~english @param [in] hApp
///       HANDLE    - Application handle
/// \~english @retval
///       None
/// \~english @par Detail
///       - The API stops the logging function.\n
///       \n
///       - Include    ss_logger_common.h
///       - Library    libSS_SystemIfUnified.so
/// \~english @par Classification
///       Public
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
void StopLoggingFunction(HANDLE hApp);

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_COMMON_H_

/** @}*/  // end of LoggerService
/** @}*/  // end of SystemService
/** @}*/  // end of BaseSystem
