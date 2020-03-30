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
/// \ingroup
/// \brief
///
///
///
///
///
///////////////////////////////////////////////////////////////////////////////

/*
 * This file has been generated automatically.
 * User hand written code entry is not allowed. Do not modify the file content.
 * 
 */
#ifndef COMMUNICATION_SERVER_INCLUDE_MAIN_COMMUNICATION_CID_H_
#define COMMUNICATION_SERVER_INCLUDE_MAIN_COMMUNICATION_CID_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////

#define LAN_SERVICE_MAIN "Communication"

typedef enum PSCommunicationServiceProtocol {
  CID_COMMSYS_MSG_BASE = PROTOCOL_FRAMEWORKUNIFIED_BASE_CMD,  // Base command id.
  CID_COMMSYS_CAN_READY,                         // CAN_COM_PROT->Communication
  CID_COMMSYS_TIMEOUT,                           // Communication->CAN
} PS_CommunicationServiceProtocol;

#endif  // COMMUNICATION_SERVER_INCLUDE_MAIN_COMMUNICATION_CID_H_
