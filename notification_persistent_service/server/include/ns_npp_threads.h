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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NS_NPPService
/// .
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_NPPService
/// \brief
///
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_THREADS_H_
#define NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_THREADS_H_

// name of threads
#define NS_NPP_READ_THREAD_NAME               "NS_NPPReadWkr"
#define NS_NPP_WRITE_THREAD_NAME            "NS_NPPWriteWkr"
#define NS_NPP_IMMEDIATE_PERSIST_THREAD_NAME    "NS_NPPIPWkr"
#define NS_NPP_DATA_RESET_THREAD_NAME           "NS_NPPDataRst"

// priority of threads
#define NS_NPP_READ_THREAD_PRIO         15
#define NS_NPP_WRITE_THREAD_PRIO        15
#define NS_NPP_IMMEDIATE_PERSIST_THREAD_PRIO  25
#define NS_NPP_DATA_RESET_THREAD_PRIO     10

#endif  // NOTIFICATION_PERSISTENT_SERVICE_SERVER_INCLUDE_NS_NPP_THREADS_H_
