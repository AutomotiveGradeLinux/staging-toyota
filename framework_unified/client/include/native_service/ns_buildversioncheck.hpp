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
 * @file ns_buildversioncheck.hpp
 * @brief \~english Provide the APIs to check/get version
 */

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_UTILITY_BUILDVERSIONCHECK_LIBRARY_INCLUDE_NATIVE_SERVICE_NS_BUILDVERSIONCHECK_HPP_  // NOLINT  (build/header_guard)
#define FRAMEWORK_UNIFIED_CLIENT_NS_UTILITY_BUILDVERSIONCHECK_LIBRARY_INCLUDE_NATIVE_SERVICE_NS_BUILDVERSIONCHECK_HPP_

#include <native_service/frameworkunified_types.h>

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

/**
 * \~english the namespace frameworkunified
 */
namespace frameworkunified {
/**
 * \~english the namespace ns
 */
namespace ns {
/**
 * \~english the namespace utility
 */
namespace utility {

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_BuildVersionCheck
/// \~english @par Brief
///       Check whether the input param build_version is matched or not.
/// \~english @param [in] build_version
///       PCSTR - The version to be checked
/// \~english @retval TRUE: match; FALSE: does not match
///
/// \~english @par Prerequisite
///       - NULL
/// \~english @par Change of internal state
///       - NULL
/// \~english @par Conditions of processing failure
///       - NULL
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       If the input param version string build_version is empty, the function returns FALSE.      \n
///       If the input param version string build_version is equal to the environment build version \n
///       and library build version, the function returns TRUE.
/// \~english @see getEnvironmentBuildVersion getLibraryBuildVersion
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL buildVersionsMatch(PCSTR build_version);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_BuildVersionCheck
/// \~english @par Brief
///       Get the build version from environment variable.
/// \~english @retval the build version string
///
/// \~english @par Prerequisite
///       - NULL
/// \~english @par Change of internal state
///       - NULL
/// \~english @par Conditions of processing failure
///       - NULL
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The funcion returns the value of the system environment variable "BUILD_VERSION". If \n
///       the environment variable does not exist, the function returns "".
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////////
PCSTR getEnvironmentBuildVersion();  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_BuildVersionCheck
/// \~english @par Brief
///       Get the library build version string
/// \~english @retval the library buidling version string
///
/// \~english @par Prerequisite
///       - NULL
/// \~english @par Change of internal state
///       - NULL
/// \~english @par Conditions of processing failure
///       - NULL
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       The function returns the string as macro _BUILD_VERSION.
/// \~english @see _BUILD_VERSION
////////////////////////////////////////////////////////////////////////////////////////////////
PCSTR getLibraryBuildVersion();  // NOLINT (readability/nolint)

/**
 * \~english the namespace utility_private
 */
namespace utility_private {

////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup NS_BuildVersionCheck
/// \~english @par Brief
///       Check whether the input param is empty or not.
/// \~english @param [in] build_version
///       PCSTR - the build version
/// \~english @retval the input param build_version or "<Not defined>"
///
/// \~english @par Prerequisite
///       - NULL
/// \~english @par Change of internal state
///       - NULL
/// \~english @par Conditions of processing failure
///       - NULL
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       If the input param build_version is not empty, the function returns build_version, \n
///       or the function returns "<Not defined>".
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////////////////
PCSTR niceBuildVersion(PCSTR build_version);  // NOLINT (readability/nolint)

/**
 * \~english the end of namespace utility_private
 */
}
/**
 * \~english the end of namespace utility
 */
}
/**
 * \~english the end of namespace ns
 */
}
/**
 * \~english the end of namespace frameworkunified
 */
}

/**
 * \~english define the macro _BUILD_VERSION
 */
#ifndef BUILD_VERSION
#define _BUILD_VERSION ""
#else
#define QUOTE( ___str___ ) #___str___
#define EXPAND_AND_QUOTE( ___str___ ) QUOTE( ___str___ )
#define _BUILD_VERSION EXPAND_AND_QUOTE( BUILD_VERSION )
#endif

/**
 * \~english define the macro WARNING_STRING to print the warning
 */
#define WARNING_STRING "\n\
!!! WARNING - Build Versions do not match !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
project build:     %s\n\
library build:     %s\n\
environment build: %s\n\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"

/**
 * \~english define the macro _BUILD_VERSION
 */
#if defined STDOUT_LOGGING
#include <cstdio>
#define BUILDVERSIONLOG( ___log_string___, ...) std::printf( ___log_string___ "\n", __VA_ARGS__)
#define BUILDVERSION_NULL_STATEMENT (0)
#elif defined NO_LOGGING
#define BUILDVERSIONLOG(...) ((void)0)
#define BUILDVERSION_NULL_STATEMENT ((void)0)
#else
#include <native_service/ns_logger_if.h>
#define BUILDVERSIONLOG( ___log_string___, ...) TEXT( __PRETTY_FUNCTION__, ___log_string___, __VA_ARGS__)
/**
 * \~english define the macro BUILDVERSION_NULL_STATEMENT
 */
#define BUILDVERSION_NULL_STATEMENT ((void)0)
#endif

/**
 * \~english define the macro CHECK_BUILD_VERSION to check the version
 */
#define CHECK_BUILD_VERSION() \
    (!framework_unified::ns::utility::buildVersionsMatch( _BUILD_VERSION )) ? \
            BUILDVERSIONLOG( WARNING_STRING, \
                             framework_unified::ns::utility::utility_private::niceBuildVersion( _BUILD_VERSION ), \
                             framework_unified::ns::utility::utility_private::niceBuildVersion( framework_unified::ns::utility::getLibraryBuildVersion() ), \
                             framework_unified::ns::utility::utility_private::niceBuildVersion( framework_unified::ns::utility::getEnvironmentBuildVersion() ) ) :\
            BUILDVERSION_NULL_STATEMENT

/** @}*/  // end of framework_unified

/** @}*/  // end of native_service

/** @}*/  // end of BaseSystem

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_UTILITY_BUILDVERSIONCHECK_LIBRARY_INCLUDE_NATIVE_SERVICE_NS_BUILDVERSIONCHECK_HPP_  // NOLINT  (build/header_guard)
