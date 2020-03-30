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

#ifndef COMMUNICATION_SERVER_INCLUDE_COM_ERROR_TYPE_H_
#define COMMUNICATION_SERVER_INCLUDE_COM_ERROR_TYPE_H_

/*************************************************************************
 *	RET_API define
 *************************************************************************/

#define RET_NORMAL 0 /* Normal completion */

/* Function code (normal termination) system */
#define RET_RCVMSG 1 /* Receive only message */

/* Error code system */
#define RET_OSERROR -127  /* OS System call error*/
#define RET_ERROR -1      /* Error termination (content is unspecified) */
#define RET_ERRPARAM -2   /* Parameter error */
#define RET_ERRTIMEOUT -5 /* Timeout with Processing Not Complete */

#endif  // COMMUNICATION_SERVER_INCLUDE_COM_ERROR_TYPE_H_
