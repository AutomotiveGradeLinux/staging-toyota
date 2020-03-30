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

#ifndef VERSION_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_SS_VER_H_
#define VERSION_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_SS_VER_H_

#include <native_service/frameworkunified_types.h>
#include <string>
#include <algorithm>
#include <map>

/**
 * @file ss_ver.h
 * @brief \~english This file supports CSSVer class, This class is a generic CSSVer abstraction
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup version_library
 *  @ingroup system_service
 *  @{
 */

using namespace std;  //  NOLINT (readability/nolint)

#define SSVER_VERCHAR_MAX   (64)
#define SSVER_DATE_MAX      (16)

/**
* @struct SSVER_PkgInfo
* @brief \~english The structure of version package information
*/
typedef struct {
  char version[SSVER_VERCHAR_MAX]; /*!< \~english Version information (String):*/
                                   /*!< \~english The termination of string must be "\0"*/
  char date[SSVER_DATE_MAX]; /*!< \~english Date (String):The termination of string must be "\0" */
                             /*!< \~english Date (String):Store "\0" when date information does not exist.*/
} SSVER_PkgInfo;

typedef std::map<std::string, SSVER_PkgInfo> SSVerPkgList;
typedef SSVerPkgList::const_iterator SSVerPkgListIter;

///////////////////////////////////////////////////////////////////////////////////////////////////
// \ingroup SS_SystemManager
///////////////////////////////////////////////////////////////////////////////////////////////////
// \brief This class is a generic CSSVer abstraction
///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @class CSSVer
 * \~english @brief Version information set-up/acquisition
 * \~english @par   Brief Introduction
 *        Class to provide the function of the version information set-up/acquisition
 *
 */
class CSSVer {
 private:
  SSVerPkgList m_verList;
  void dump(void) const;

 public:
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SS_SystemManager
  /// \~english @par Summary
  ///        Constructor for CSSVer class
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None
  /// \~english @par Changes of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       None
  /// \~english @par Detail
  ///       Creates the instance of CSSVer class. \n
  ///       This API reads the version information registered in VersionDB and creates the package
  ///       lists inside a instance. \n
  ///       This can create some instances at the same time. \n
  ///       Meanwhile, this does not guarantee the action when some threads
  ///       inside same process access to same instances. \n
  ///       It is possible that version information is overwritten from other process but the information overwritten
  ///       at other process is not reflected into the version information held inside self-instance.
  ///       By this, it is necessary to recreate an instance to get the latest information. \n
  /// \~english @see  ~CSSVer
  ////////////////////////////////////////////////////////////////////////////////////
  CSSVer();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SS_SystemManager
  /// \~english @par Summary
  ///        Destructor for CSSVer class
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None
  /// \~english @par Changes of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classfication
  ///       Public
  /// \~english @par Type
  ///       None
  /// \~english @par Detail
  ///       Releases the instance of CSSVer class.
  /// \~english @see  CSSVer
  ////////////////////////////////////////////////////////////////////////////////////
  ~CSSVer();

  // Wrapper
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SS_SystemManager
  /// \~english @par Summary
  ///        Returns the first iterator of package list.
  /// \~english @param None
  /// \~english @retval SSVerPkgListIter The first iterator of package list
  /// \~english @par
  ///       The structure of version package information
  /// \~english @code
  ///             #define SSVER_VERCHAR_MAX   (64)
  ///             #define SSVER_DATE_MAX      (16)
  ///             typedef struct{
  ///                 char version[SSVER_VERCHAR_MAX];    /* Version information (String): The termination of
  ///                                                         string must be "\0" */
  ///                 char date[SSVER_DATE_MAX];          /* Date (String): The termination of string must be "\0"*/
  ///                                                     /* Store "\0" when date information does not exist. */
  ///             }SSVER_PkgInfo;
  ///
  ///             typedef std::map<std::string, SSVER_PkgInfo> SSVerPkgList;  /* Package list definition            */
  ///             typedef SSVerPkgList::const_iterator SSVerPkgListIter;      /* Iterator definition of package list */
  ///            @endcode
  /// \~english @par Preconditions
  ///       - None
  /// \~english @par Changes of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       Gets and returns the first iterator of package list from the package list inside an instance.
  /// \~english @see  end
  ////////////////////////////////////////////////////////////////////////////////////
  SSVerPkgListIter begin() {
    return m_verList.begin();
  }

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SS_SystemManager
  /// \~english @par Summary
  ///        Returns the end iterator of package list.
  /// \~english @param None
  /// \~english @retval SSVerPkgListIter The end iterator of package list
  /// \~english @par
  ///       The structure of version package information
  /// \~english @code
  ///             #define SSVER_VERCHAR_MAX   (64)
  ///             #define SSVER_DATE_MAX      (16)
  ///             typedef struct{
  ///                 char version[SSVER_VERCHAR_MAX];    /* Version information (String): The termination of
  ///                                                               string must be "\0" */
  ///                 char date[SSVER_DATE_MAX];          /* Date (String): The termination of string must be "\0" */
  ///                                                     /* Store "\0" when date information does not exist. */
  ///             }SSVER_PkgInfo;
  ///
  ///             typedef std::map<std::string, SSVER_PkgInfo> SSVerPkgList;  /* Package list definition            */
  ///             typedef SSVerPkgList::const_iterator SSVerPkgListIter;      /* Iterator definition of package list */
  ///            @endcode
  /// \~english @par Preconditions
  ///       - None
  /// \~english @par Changes of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       Gets and returns the last iterator of package list from the package list inside a instance.
  /// \~english @see  begin
  ////////////////////////////////////////////////////////////////////////////////////
  SSVerPkgListIter end() {
    return m_verList.end();
  }

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SS_SystemManager
  /// \~english @par Summary
  ///        Checks whether data exists in package list or not.
  /// \~english @param None
  /// \~english @retval bool The possibility of data existence of package list
  /// \~english @par
  ///             - true    :    Data does not exist
  ///             - false   :    Data exists
  /// \~english @par Preconditions
  ///       - None
  /// \~english @par Changes of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       Returns true when package list is null and return false when data exists.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  bool empty() {
    return m_verList.empty();
  }

  // Setter,getter
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SS_SystemManager
  /// \~english @par Summary
  ///        Gets the package information of specified package.
  /// \~english @param [in] name
  ///        std::string &   -   Package name
  /// \~english @param [out] p_info
  ///        SSVER_PkgInfo*  -   Pointer to the storage area of package information
  /// \~english @par
  ///       The structure of version package information
  /// \~english @code
  ///             #define SSVER_VERCHAR_MAX   (64)
  ///             #define SSVER_DATE_MAX      (16)
  ///             typedef struct{
  ///                 char version[SSVER_VERCHAR_MAX];    /* Version information (String): The termination of
  ///                                                            string must be "\0" */
  ///                 char date[SSVER_DATE_MAX];          /* Date (String): The termination of string must be "\0" */
  ///                                                     /* Store "\0" when date information does not exist. */
  ///             }SSVER_PkgInfo;
  ///
  ///             typedef std::map<std::string, SSVER_PkgInfo> SSVerPkgList;  /* Package list definition            */
  ///             typedef SSVerPkgList::const_iterator SSVerPkgListIter;      /* Iterator definition of package list */
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Succeeded in getting package information
  /// \~english @retval eFrameworkunifiedStatusFileLoadError Loading error for file
  /// \~english @retval eFrameworkunifiedStatusFail Failed at some process
  /// \~english @par Preconditions
  ///       - None
  /// \~english @par Changes of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - Pointer(p_info) to the storage area for package information specified
  ///                           by parameters is NULL. [eFrameworkunifiedStatusFail]
  ///       - Package information does not exist. [eFrameworkunifiedStatusFileLoadError]
  ///       - Unexpected error occurs. [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       Returns this package information if the package name specified by parameters exists
  ///       in the package information list inside an instance.
  ///       This API reads and returns the package information from package
  ///                               information storing files if it does not exist. \n
  ///       This adds the package information read from files to the package information list
  ///                               inside an instance. \n
  ///       When other CSSVer instance (B) adds package from setPkgInfo during creating instance (A),
  ///                             after that, this API can get the package information added by instance (A).
  /// \~english @see  setPkgInfo
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus getPkgInfo(const std::string &name, SSVER_PkgInfo* p_info);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SS_SystemManager
  /// \~english @par Summary
  ///        Sets the package information of specified package.
  /// \~english @param [in] name
  ///        std::string &   -   Package name
  /// \~english @param [in] info
  ///        SSVER_PkgInfo*  -   Package information
  /// \~english @par
  ///       The structure of version package information
  /// \~english @code
  ///             #define SSVER_VERCHAR_MAX   (64)
  ///             #define SSVER_DATE_MAX      (16)
  ///             typedef struct{
  ///                 char version[SSVER_VERCHAR_MAX];    /* Version information (String): The termination of
  ///                                                           string must be "\0" */
  ///                 char date[SSVER_DATE_MAX];          /* Date (String): The termination of string must be \ */
  ///                                                     /* Store "\0" when date information does not exist. */
  ///             }SSVER_PkgInfo;
  ///
  ///             typedef std::map<std::string, SSVER_PkgInfo> SSVerPkgList;  /* Package list definition            */
  ///             typedef SSVerPkgList::const_iterator SSVerPkgListIter;      /* Iterator definition of package list */
  ///            @endcode
  /// \~english @retval eFrameworkunifiedStatusOK Succeeded in setting package information
  /// \~english @retval eFrameworkunifiedStatusFail Faild at some process
  /// \~english @par Preconditions
  ///       - None
  /// \~english @par Changes of the internal state
  ///       - The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - Unexpected error occurs [eFrameworkunifiedStatusFail]
  /// \~english @par Classification
  ///       Public
  /// \~english @par Type
  ///       Sync only(None communication)
  /// \~english @par Detail
  ///       Sets the package information specified by parameters and update VersionDB. \n
  ///       When some package information exists in the package information list inside an instance,
  ///       this API adds the package information specified by parameters to the package
  ///             information list inside an instance,
  ///       and writes the package information specified by parameters in package information storing files. \n
  ///       When package information does not exist in the package information list inside an instance,
  ///       this sets the package information specified by parameters to the package information list inside an instance
  ///       and writes the package information specified by parameters in package information storing files.
  /// \~english @see  getPkgInfo
  ////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus setPkgInfo(const std::string &name, SSVER_PkgInfo &info);  //  NOLINT (readability/nolint)
};

#endif  // VERSION_LIBRARY_LIBRARY_INCLUDE_SYSTEM_SERVICE_SS_VER_H_
