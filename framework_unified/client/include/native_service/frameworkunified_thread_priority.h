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
/// \ingroup  tag_NSFramework
/// \brief    The file provide CFrameworkunifiedThreadPriorities class.
///
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_thread_priority.h
 * @brief \~english The file contains declaration of CFrameworkunifiedThreadPriorities class
 *
 */
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
/** @addtogroup framework
 *  @ingroup native_service
 *  @{
 */

#ifndef FRAMEWORKUNIFIED_THREAD_PRIORITY_H_  // NOLINT  (build/header_guard)
#define FRAMEWORKUNIFIED_THREAD_PRIORITY_H_

#include <native_service/frameworkunified_types.h>
#include <string>
#include <map>

#define FRAMEWORKUNIFIED_PRIORITY_NOT_FOUND -1  ///< \~english if specified thread name not found

namespace frameworkunified {
namespace framework {
class args {
 public:
  enum {
    name,
    priority
  };
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class parses and stores thread priorities
///////////////////////////////////////////////////////////////////////////////////////////////////
class CFrameworkunifiedThreadPriorities {
 public:
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedThreadPriorities
  /// \~english @par Brief
  ///        CFrameworkunifiedThreadPriorities constructor
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        None
  /// \~english @par Detail
  ///        CFrameworkunifiedThreadPriorities constructor, for create CFrameworkunifiedThreadPriorities
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  CFrameworkunifiedThreadPriorities();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedThreadPriorities
  /// \~english @par Brief
  ///        CFrameworkunifiedThreadPriorities destructor
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Prerequisite
  ///        None
  /// \~english @par Change of internal state
  ///        None
  /// \~english @par Conditions of processing failure
  ///        None
  /// \~english @par Detail
  ///        CFrameworkunifiedThreadPriorities destructor, for destory CFrameworkunifiedThreadPriorities
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///
  /////////////////////////////////////////////////////////////////////////////////////
  virtual ~CFrameworkunifiedThreadPriorities();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedThreadPriorities
  /// \~english @par Brief
  ///        Get the priority of the thread. Priority of the thread was added by \ref AddPriority(),
  ///        when parsing command line arguments.
  /// \~english @param [in] f_cThreadName
  ///        const std::string&   - Name of thread
  /// \~english @retval SI_32 Returns the priority of a thread.
  /// \~english @retval FRAMEWORKUNIFIED_PRIORITY_NOT_FOUND if specified thread name not found
  /// \~english @par
  ///        FRAMEWORKUNIFIED_PRIORITY_NOT_FOUND
  /// \~english @code
  ///                 #define FRAMEWORKUNIFIED_PRIORITY_NOT_FOUND -1  // if specified thread name not found
  ///            @endcode
  /// \~english @par Prerequisite
  ///           - Call AddPriority() and set priority.
  /// \~english @par Change of internal state
  ///        - Change of internal state according to the API does not occur.
  /// \~english @par Conditions of processing failure
  ///        - Thread name is not registered in AddPriority()
  /// \~english @par Detail
  ///        Get the priority of the thread. Priority of the thread was added by \ref AddPriority(),
  ///        when parsing command line arguments.
  /// \~english @par Classification
  ///        Public
  /// \~english @par Type
  ///        sync only
  /// \~english @see
  ///        AddPriority
  /////////////////////////////////////////////////////////////////////////////////////
  static SI_32 GetPriority(const std::string &f_cThreadName);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedThreadPriorities
  /// \~english @par Summary
  ///        set thread Priority
  /// \~english @param [in] f_cThreadName
  ///        const std::string &f_cThreadName  - thread name
  /// \~english @param [in] f_si32Priority
  ///        SI_32 f_si32Priority  - Priority
  /// \~english @retval eFrameworkunifiedStatusOK    set Priority success
  /// \~english @par Preconditions
  ///       - None
  /// \~english @par Change of the internal state
  ///       - ms_mapThreadPritories[f_cThreadName] = f_si32Priority
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       set thread Priority through thread name
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
  static EFrameworkunifiedStatus AddPriority(const std::string &f_cThreadName, SI_32 f_si32Priority);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedThreadPriorities
  /// \~english @par Summary
  ///        print thread Priority
  /// \~english @param None
  /// \~english @retval None
  /// \~english @par Preconditions
  ///       - None
  /// \~english @par Change of the internal state
  ///       - None
  /// \~english @par Causes of failures
  ///       None
  /// \~english @par Classification
  ///       public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       read each thread and print it's Priority
  /// \~english @see
  ////////////////////////////////////////////////////////////////////////////////////
  static VOID PrintPriorites();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CFrameworkunifiedThreadPriorities
  /// \~english @par Summary
  ///        analyse thread param
  /// \~english @param [in] f_cArgumentValue
  ///        PCHAR f_cArgumentValue  -  thread param string
  /// \~english @retval eFrameworkunifiedStatusOK   parse thread param success
  /// \~english @retval eFrameworkunifiedStatusFail   parse thread param failed
  /// \~english @par Preconditions
  ///       - None
  /// \~english @par Change of the internal state
  ///       - None
  /// \~english @par Causes of failures
  ///       - f_cArgumentValue == NULL
  ///       - f_cArgumentValue is invalid
  /// \~english @par Classification
  ///       public
  /// \~english @par Type
  ///       sync only
  /// \~english @par Detail
  ///       analyse thread param data, and set param to thread.
  ///       if given data is NULL or invalid , return false
  /// \~english @see getsubopt, AddPriority
  ////////////////////////////////////////////////////////////////////////////////////
  static EFrameworkunifiedStatus ParseThreadArguments(PCHAR f_cArgumentValue);

 private:
  typedef std::map<std::string, SI_32> TThreadPriorityList;
  typedef TThreadPriorityList::iterator ThreadPrioMapIter;
  static TThreadPriorityList ms_mapThreadPritories;  // map of thread name and priority
};

}  // namespace framework
};  // namespace frameworkunified

#endif /* FRAMEWORKUNIFIED_THREAD_PRIORITY_H_ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
