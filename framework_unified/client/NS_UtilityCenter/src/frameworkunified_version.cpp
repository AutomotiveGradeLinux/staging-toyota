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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NativeServices
/// \brief    Implementation of CFrameworkunifiedVersion class
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/ns_version_if.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string>


// defaults - top level makefile for entire system will provide an override
#ifndef FRAMEWORKUNIFIED_SIGNATURE
#define FRAMEWORKUNIFIED_SIGNATURE   0xE0344333
#endif

#ifndef FRAMEWORKUNIFIED_STRUC_VER
#define FRAMEWORKUNIFIED_STRUC_VER   0x02
#endif

#ifndef FRAMEWORKUNIFIED_PRODUCT_VER
#define FRAMEWORKUNIFIED_PRODUCT_VER   0x01
#endif

#ifndef PRODUCT_LABEL
#define PRODUCT_LABEL  "undefined"
#endif

#ifndef FRAMEWORKUNIFIED_BUILD_VER
#define FRAMEWORKUNIFIED_BUILD_VER  "undefined"
#endif

/* For future use */
#define YEAR ((((__DATE__[7] - '0') * 10 + (__DATE__[8] - '0')) * 10 \
+ (__DATE__[9] - '0')) * 10 + (__DATE__[10] - '0'))

#define MONTH (__DATE__[2] == 'n' ? 0 \
: __DATE__[2] == 'b' ? 1 \
: __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? 2 : 3) \
: __DATE__[2] == 'y' ? 4 \
: __DATE__[2] == 'n' ? 5 \
: __DATE__[2] == 'l' ? 6 \
: __DATE__[2] == 'g' ? 7 \
: __DATE__[2] == 'p' ? 8 \
: __DATE__[2] == 't' ? 9 \
: __DATE__[2] == 'v' ? 10 : 11)

#define DAY ((__DATE__[4] == ' ' ? 0 : __DATE__[4] - '0') * 10 \
+ (__DATE__[5] - '0'))

#define DATE_AS_INT (((YEAR - 2000) * 12 + MONTH) * 31 + DAY)

#define HOUR (((__TIME__[0] - '0') * 10 + (__TIME__[1] - '0')))

#define MIN (((__TIME__[3] - '0') * 10 + (__TIME__[4] - '0')))


////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Default Constructor
////////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedVersion::CFrameworkunifiedVersion():
  m_tVersionInfo(const_cast<PSTR>(PRODUCT_LABEL),
                 FRAMEWORKUNIFIED_SIGNATURE,
                 FRAMEWORKUNIFIED_STRUC_VER,
                 FRAMEWORKUNIFIED_PRODUCT_VER,
                 DATE_AS_INT,
                 0,
                 0,
                 const_cast<PSTR>(FRAMEWORKUNIFIED_BUILD_VER),
                 0),
  m_u32Month(MONTH + 1),
  m_u32Year(YEAR),
  m_u32Day(DAY) {
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Parametrized constructor
////////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedVersion::CFrameworkunifiedVersion(UI_16 major, UI_16 minor, UI_16 revision) :
  m_tVersionInfo(const_cast<PSTR>(PRODUCT_LABEL),
                 FRAMEWORKUNIFIED_SIGNATURE,
                 FRAMEWORKUNIFIED_STRUC_VER,
                 FRAMEWORKUNIFIED_PRODUCT_VER,
                 DATE_AS_INT,
                 major,
                 minor,
                 const_cast<PSTR>(FRAMEWORKUNIFIED_BUILD_VER),
                 revision),
  m_u32Month(MONTH + 1),
  m_u32Year(YEAR),
  m_u32Day(DAY) {
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Destructor
////////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedVersion::~CFrameworkunifiedVersion() {
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Compares two version instances
////////////////////////////////////////////////////////////////////////////////////////////
BOOL operator == (CFrameworkunifiedVersion &a, CFrameworkunifiedVersion &b) {  // NOLINT (readability/nolint)
  return a.operator == (b);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Compares two version instances
////////////////////////////////////////////////////////////////////////////////////////////
BOOL CFrameworkunifiedVersion::operator == (const CFrameworkunifiedVersion &f_test_i) {
  if (!strcmp(m_tVersionInfo.p_str_product, f_test_i.m_tVersionInfo.p_str_product)     &&
      !strcmp(m_tVersionInfo.p_str_build, f_test_i.m_tVersionInfo.p_str_build)       &&
      (m_tVersionInfo.u16_major == f_test_i.m_tVersionInfo.u16_major)         &&
      (m_tVersionInfo.u16_minor == f_test_i.m_tVersionInfo.u16_minor)         &&
      (m_tVersionInfo.u16_revision == f_test_i.m_tVersionInfo.u16_revision)       &&
      (m_tVersionInfo.u32_date == f_test_i.m_tVersionInfo.u32_date)           &&
      (m_tVersionInfo.u32_product_version == f_test_i.m_tVersionInfo.u32_product_version) &&
      (m_tVersionInfo.u32_signature == f_test_i.m_tVersionInfo.u32_signature)     &&
      (m_tVersionInfo.u32_struc_version == f_test_i.m_tVersionInfo.u32_struc_version)) {
    return TRUE;
  }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Returns the version string
////////////////////////////////////////////////////////////////////////////////////////////
PCSTR CFrameworkunifiedVersion::VersionStr() {
  static char version[30] = {};

  snprintf(version, sizeof(version) - 1, "%s_%s_%02d.%02d.%02d",
           m_tVersionInfo.p_str_product,
           m_tVersionInfo.p_str_build,
           m_tVersionInfo.u16_major,
           m_tVersionInfo.u16_minor,
           m_tVersionInfo.u16_revision);

  return version;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Returns the date when the new version of the app was created
////////////////////////////////////////////////////////////////////////////////////////////
PCSTR CFrameworkunifiedVersion::DateStr() {
  static char date[30] = {};

  snprintf(date, sizeof(date) - 1, "%d-%02d-%02d",
           m_u32Year,
           m_u32Month,
           m_u32Day);

  return date;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Returns the Signature
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 CFrameworkunifiedVersion::Signature() const {
  return m_tVersionInfo.u32_signature;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Returns a new structure version
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 CFrameworkunifiedVersion::StrucVersion() const {
  return m_tVersionInfo.u32_struc_version;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Returns the product version
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 CFrameworkunifiedVersion::ProductVersion() const {
  return m_tVersionInfo.u32_product_version;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Returns the date
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 CFrameworkunifiedVersion::Date() const {
  return m_tVersionInfo.u32_date;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Returns the major number
////////////////////////////////////////////////////////////////////////////////////////////
UI_16 CFrameworkunifiedVersion::Major() const {
  return m_tVersionInfo.u16_major;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Returns the minor number
////////////////////////////////////////////////////////////////////////////////////////////
UI_16 CFrameworkunifiedVersion::Minor() const {
  return m_tVersionInfo.u16_minor;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Returns the unique product identifier
////////////////////////////////////////////////////////////////////////////////////////////
PCSTR CFrameworkunifiedVersion::Product() const {
  return m_tVersionInfo.p_str_product;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Returns the unique build identifier number
////////////////////////////////////////////////////////////////////////////////////////////
PCSTR CFrameworkunifiedVersion::Build() const {
  return m_tVersionInfo.p_str_build;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function: Returns the revision number
////////////////////////////////////////////////////////////////////////////////////////////
UI_16 CFrameworkunifiedVersion::Revision() const {
  return m_tVersionInfo.u16_revision;
}
