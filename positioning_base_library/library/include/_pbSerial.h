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


#ifndef POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBSERIAL_H_
#define POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBSERIAL_H_

#include <vehicle_service/positioning_base_library.h>

/* Prototype */
BOOL SerialTableInit(void);              /* Initialization process for each process */
BOOL SerialTableTerm(void);              /* Termination process for each process */
BOOL SerialObjectTimeoutAdd(HANDLE h_obj, DWORD dw_read_timeout, DWORD dw_write_timeout);  /* Timeout registration process */
BOOL SerialObjectTimeoutGet(HANDLE h_obj, DWORD*, DWORD*);  /* Timeout acquisition processing */
BOOL SerialObjectWaitmaskAdd(HANDLE h_obj, DWORD dw_mask);      /* Mask registration process */
BOOL SerialObjectWaitmaskGet(HANDLE h_obj, DWORD*);    /* Mask acquisition processing */
BOOL SerialObjectDel(HANDLE h_obj);            /* Deletion process */

#endif   // POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBSERIAL_H_
