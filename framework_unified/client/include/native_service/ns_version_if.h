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
/// \brief    Header for CFrameworkunifiedVersion class
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @file ns_version_if.h
* @brief \~english Header for CFrameworkunifiedVersion class
 */

#ifndef NS_NATIVESERVICES_INC_NATIVE_NS_VERSION_IF_H_  // NOLINT  (build/header_guard)
#define NS_NATIVESERVICES_INC_NATIVE_NS_VERSION_IF_H_
#include <native_service/frameworkunified_types.h>
#include <string.h>

#ifdef AGL_STUB
#include <other_service/strlcpy.h>
#include <other_service/strlcat.h>
#endif



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

/**
 * @class CFrameworkunifiedVersion
 * @brief \~english Version Info Class
 * @par   \~english Brief Introduction
 * \~english This class provides functions to get various information of the version.
 *
 */
class CFrameworkunifiedVersion {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Construct a CFrameworkunifiedVersion object.
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function constructs a CFrameworkunifiedVersion object.
  /// \~english @see ~CFrameworkunifiedVersion
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedVersion();

  ///////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///        Constructs a CFrameworkunifiedVersion object with parameters.
  /// \~english @param[in] major
  ///        UI_16 - Major number of the application
  /// \~english @param[in] minor
  ///        UI_16 - Minor number of the application
  /// \~english @param[in] revision
  ///        UI_16 - Revision number of the application
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function constructs a CFrameworkunifiedVersion object with parameters.
  /// \~english @see ~CFrameworkunifiedVersion
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedVersion(UI_16 major, UI_16 minor, UI_16 revision);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Destruct the CFrameworkunifiedVersion object.
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function destructs the CFrameworkunifiedVersion object.
  /// \~english @see CFrameworkunifiedVersion
  ////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunifiedVersion();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Get the data number
  /// \~english @param [in] a
  ///       CFrameworkunifiedVersion - the CFrameworkunifiedVersion object that will be compared against b.
  /// \~english @param [in] b
  ///       CFrameworkunifiedVersion - the CFrameworkunifiedVersion object that will be compared against a.
  /// \~english @retval TRUE: Match, FALSE: Does not Match
  ///
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function is a friend function of the class. \n
  ///       The function checks whether the object matchs the other or not. Match: returns TRUE, \n
  ///       Does not match: returns FALSE.
  /// \~english @see operator ==
  ////////////////////////////////////////////////////////////////////////////////////////////////
  friend BOOL operator == (CFrameworkunifiedVersion &a, CFrameworkunifiedVersion &b);  // NOLINT (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Get the data number
  /// \~english @param [in] f_test_i
  ///       CFrameworkunifiedVersion - the CFrameworkunifiedVersion object that will be compared against this obj.
  /// \~english @retval TRUE: Match, FALSE: Does not Match
  ///
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function checks whether the object matchs the other or not. Match: returns TRUE, \n
  ///       Does not match: returns FALSE.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL operator == (const CFrameworkunifiedVersion &f_test_i); // NOLINT (readability/nolint)

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Get the version as string
  /// \~english @retval the vision string
  ///
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function returns the version string.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////////
  PCSTR VersionStr();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Get the date as string
  /// \~english @retval the date string
  ///
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function returns the date when building.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////////
  PCSTR DateStr();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Get the signature number
  /// \~english @retval the signature
  ///
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function returns the signature number.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_32 Signature() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Get the structure version number
  /// \~english @retval the structure version number
  ///
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function returns the structure version number.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_32 StrucVersion() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Get the product version
  /// \~english @retval the product version
  ///
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function returns the product version.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_32 ProductVersion() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Get the date as number
  /// \~english @retval the date number
  ///
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function returns the date as number.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_32 Date() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Get the major verion number
  /// \~english @retval the major version number
  ///
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function returns the major version number.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_16 Major() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Get the minor version number
  /// \~english @retval the minor version number
  ///
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function returns the minor version number.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_16 Minor() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Get the unique product identifier
  /// \~english @retval the revision number
  ///
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function returns the unique product identifier generated by the build server.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////////
  PCSTR Product() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Get the building version string
  /// \~english @retval the building version string
  ///
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function returns the unique build identifier generated by the build server.
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////////////////
  PCSTR Build() const;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup NS_UtilityCenter
  /// \~english @par Brief
  ///       Get the revision number
  /// \~english @retval the revision number
  ///
  /// \~english @par Prerequisite
  ///       - None
  /// \~english @par Change of internal state
  ///       - None
  /// \~english @par Conditions of processing failure
  ///       - None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync Only
  /// \~english @par Detail
  ///       The function returns the revision as number.
  /// \~english @see CFrameworkunifiedVersion
  ////////////////////////////////////////////////////////////////////////////////////////////////
  UI_16 Revision() const;

 private:
  CFrameworkunifiedVersion(const CFrameworkunifiedVersion &);
  CFrameworkunifiedVersion &operator= (const CFrameworkunifiedVersion &);  // NOLINT (readability/nolint)
  struct VersionInfo {
    PSTR p_str_product;
    UI_32 u32_signature;
    UI_32 u32_struc_version;
    UI_32 u32_product_version;
    UI_32 u32_date;
    UI_16 u16_major;
    UI_16 u16_minor;
    PSTR p_str_build;
    UI_16 u16_revision;
    VersionInfo(PSTR p, UI_32 a, UI_32 b, UI_32 c, UI_32 d, UI_16 e, UI_16 f , PSTR g, UI_16 h) :
      u32_signature(a),
      u32_struc_version(b),
      u32_product_version(c),
      u32_date(d),
      u16_major(e),
      u16_minor(f),
      u16_revision(h) {
      size_t length = strlen(p) + 1;
      p_str_product = new CHAR[length];  // LCOV_EXCL_BR_LINE 11:except branch
      if (NULL != p_str_product) {  // LCOV_EXCL_BR_LINE 5:new's error case
        memset(p_str_product, 0, length);
#ifdef AGL_PosixBasedOS001LEGACY_USED
        strlcpy(p_str_product, p, length);
#endif
      }

      length = strlen(g) + 1;
      p_str_build = new CHAR[length];  // LCOV_EXCL_BR_LINE 11:except branch
      if (NULL != p_str_build) {  // LCOV_EXCL_BR_LINE 5:new's error case
        memset(p_str_build, 0, length);
#ifdef AGL_PosixBasedOS001LEGACY_USED
        strlcpy(p_str_build, g , length);
#endif
      }
    }
    ~VersionInfo() {
      delete[] p_str_product;  // LCOV_EXCL_BR_LINE 11:except branch
      p_str_product = NULL;
      delete[] p_str_build;  // LCOV_EXCL_BR_LINE 11:except branch
      p_str_build = NULL;
    }
  } m_tVersionInfo;
  UI_32 m_u32Month;
  UI_32 m_u32Year;
  UI_32 m_u32Day;
};

/**
 * \~english define macro for version string
 */
#define FRAMEWORKUNIFIED_GET_VERSION() "undefined_undefined_00.00.00"

/**
 * \~english define macro for the major number of the version
 */
#define FRAMEWORKUNIFIED_APP_VERSION_MAJOR() (0)

/**
 * \~english define macro for the minor number of the version
 */
#define FRAMEWORKUNIFIED_APP_VERSION_MINOR() (0)

/**
 * \~english define macro for the revision number
 */
#define FRAMEWORKUNIFIED_APP_VERSION_REVISION() (0)

/**
 * \~english define macro for the building version
 */
#define FRAMEWORKUNIFIED_APP_VERSION_BUILDVER() "undefined"

/**
 * \~english define macro for product version
 */
#define FRAMEWORKUNIFIED_APP_VERSION_PRODUCTID() "undefined"

#endif  // NS_NATIVESERVICES_INC_NATIVE_NS_VERSION_IF_H_  // NOLINT  (build/header_guard)
/** @}*/  // end of native
/** @}*/  // end of framework_unified
/** @}*/  // end of native_service
/** @}*/  // end of BaseSystem
