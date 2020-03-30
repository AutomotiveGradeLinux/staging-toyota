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


#ifndef POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBWAITFORSINGLEOBJECT_H_
#define POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBWAITFORSINGLEOBJECT_H_

#include <vehicle_service/positioning_base_library.h>

typedef enum {
    PB_HANDLE_INVAL = 0,
    PB_HANDLE_MUTEX,
    PB_HANDLE_PROC_SEMAPHORE,
    PB_HANDLE_EVENT,
    PB_HANDLE_THREAD,
    PB_HANDLE_PROCESS,

    PB_HANDLE_KIND_MAX    /* Maximum value range check */
} HANDLE_KIND;

/* Prototype */
BOOL WaitObjectInit(void);          /* Initialization process for each process */
BOOL WaitObjectTerm(void);          /* Termination process for each process */
BOOL WaitObjectAdd(HANDLE h_obj, HANDLE_KIND l_kind);   /* Registration process */
BOOL WaitObjectDel(HANDLE h_obj);          /* Deletion process */

#endif   // POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE__PBWAITFORSINGLEOBJECT_H_
