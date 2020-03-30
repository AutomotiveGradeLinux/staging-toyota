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

/****************************************************************************
@file            waitforsingleobject.cpp
@detail            Functions that implement waitforsingleobject in PosixBasedOS001<BR>
                Functions to register/delete a table to determine the handle and its type
*****************************************************************************/

#include "_pbWaitforsingleobject.h"
#include <vehicle_service/positioning_base_library.h>
#include "WPF_STD_private.h"
#include "_pbInternalProc.h"

typedef struct HandleTypeTable {
    HANDLE        h_handle;   /* Registration handle */
    HANDLE_KIND    l_kind;   /*  Type of Mutex/Semaphre/Event... */
    struct HandleTypeTable *next;
} HANDLE_TYPE;

static HANDLE_TYPE *g_pst_handle_kind_table = NULL;
static pthread_mutex_t g_func_lock_mutex = PTHREAD_MUTEX_INITIALIZER;   /* Consider replacing it later */

/* Prototype declarations */
static BOOL FindList(HANDLE_TYPE **p_list, HANDLE h_obj);
static BOOL AddList(HANDLE_TYPE *p_add_list);
static BOOL DelList(HANDLE_TYPE *h_del_obj);
static BOOL FunctionLock(void);
static BOOL FunctionUnlock(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Public APIs
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* _sys Internally Used APIs
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/****************************************************************************
@brief            WaitObjectInit<BR>
                Initialization processing for each process
@outline        WaitObjectInit<BR>
                Initialization processing for each process
@type            Completion return type
@return            BOOL
@retval            TRUE    :    Normal
@retval            FALSE    :    Error
*****************************************************************************/
BOOL WaitObjectInit(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    g_pst_handle_kind_table = NULL;
    return TRUE;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            WaitObjectTerm<BR>
                Termination processing for each process
@outline        WaitObjectTerm<BR>
                Termination processing for each process
@type            Completion return type
@return            BOOL
@retval            TRUE    :    Normal
@retval            FALSE    :    Error
*****************************************************************************/
BOOL WaitObjectTerm(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (NULL != g_pst_handle_kind_table) {
        /* Should be Deleted a List? */
    }
    return TRUE;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            WaitObjectAdd<BR>
                Register a handle and a handle-type in a list
@outline        WaitObjectAdd<BR>
                Register a handle and a handle-type in a list
@type            Completion return type
@param[in]    HANDLE        h_obj    :    Handle to register
@param[in]    HANDLE_KIND    l_kind    :    Handle type

@return            BOOL
@retval            TRUE    :    Normal
                FALSE    :    Error
*****************************************************************************/
BOOL WaitObjectAdd(HANDLE h_obj, HANDLE_KIND l_kind) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BOOL bret = FALSE;
    HANDLE_TYPE *p_add_list = NULL;

    if ((NULL != h_obj)
            && (PB_HANDLE_INVAL < l_kind)
            && (PB_HANDLE_KIND_MAX > l_kind)) {
        FunctionLock();
        bret = FindList(&p_add_list, h_obj);
        if (TRUE == bret) {
            /* Handle already registered */
            if (l_kind == p_add_list->l_kind) {
                /* re-register the same item */
                bret = TRUE;
            } else {
                /* An attempt was made to re-register the same handle with different type */
                bret = FALSE;   /** Consider whether to win or not later */
            }
        } else {
            p_add_list = reinterpret_cast<HANDLE_TYPE*>(malloc(sizeof(HANDLE_TYPE)));
            if (NULL == p_add_list) {
                /* Memory acquisition failed */
                bret = FALSE;
            } else {
                memset(p_add_list, 0, sizeof(HANDLE_TYPE));
                p_add_list->h_handle = h_obj;
                p_add_list->l_kind = l_kind;
                p_add_list->next = NULL;
                bret = AddList(p_add_list);
                if (FALSE == bret) {
                    /* Registration failure */
                    free(p_add_list);
                    bret = FALSE;
                } else {
                    bret = TRUE;
                }
            }
        }

        FunctionUnlock();

    } else {
        bret = FALSE;   /** Parameter error */
    }
    return bret;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            WaitObjectDel<BR>
                Remove handle and handle-type from the list
@outline        WaitObjectDel<BR>
                Remove handle and handle-type from the list
@type            Completion return type
@param[in]    HANDLE        h_obj    :    Handle to delete

@return            BOOL
@retval            TRUE    :    Normal
                FALSE    :    Error
*****************************************************************************/
BOOL WaitObjectDel(HANDLE h_obj) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BOOL bret = FALSE;
    HANDLE_TYPE *p_del_list = NULL;

    if (NULL != h_obj) {
        FunctionLock();
        bret = FindList(&p_del_list, h_obj);
        if (TRUE == bret) {
            /* handle already registered */
            if (TRUE == DelList(p_del_list)) {
                free(p_del_list);
                bret = TRUE;
            } else {
                bret = FALSE;
            }
        } else {
            bret = FALSE;   /*  no handles */
        }

        FunctionUnlock();

    } else {
        bret = FALSE;   /* Parameter error */
    }
    return bret;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Private API
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/****************************************************************************
@brief            FindList<BR>
                Searching for a Handle in the List
@outline        FindList<BR>
                Searching for a Handle in the List
@type            Completion return type
@param[out]        HANDLE_TYPE**    p_list    :    Found list pointer
@param[in]        HANDLE            h_obj    :    Handle to look for

@return            BOOL
@retval            TRUE    :    Normal (p_list != NULL)
                FALSE    :    Error (p_list == NULL)
*****************************************************************************/
static BOOL FindList(HANDLE_TYPE **p_list, HANDLE h_obj) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    HANDLE_TYPE *p_now = NULL;
    BOOL bret = FALSE;

    if ((NULL != h_obj) && (NULL != p_list)) {
        /* Search list */
        p_now = g_pst_handle_kind_table;
        while (NULL != p_now) {
            /* h_obj and p_now->h_handle are pointer type.*/
            if ((int64_t)h_obj == (int64_t)p_now->h_handle) {
                *p_list = p_now;
                bret = TRUE;
                break;
            }
            p_now = p_now->next;
        }
    } else {
        bret = FALSE;   /** Parameter error */
    }

    return bret;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            AddList<BR>
                Append data to the end of the list
@outline        AddList<BR>
                Append data to the end of the list
@type            Completion return type
@param[in]        HANDLE_TYPE*    p_list    :    Data to add

@return            BOOL
@retval            TRUE    :    Normal
                FALSE    :    Error
*****************************************************************************/
static BOOL AddList(HANDLE_TYPE *p_add_list) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    HANDLE_TYPE *p_now = NULL;
    BOOL bret = FALSE;

    if (NULL != p_add_list) {
        /* Add unregistered data */
        if (NULL == g_pst_handle_kind_table) {
            g_pst_handle_kind_table = p_add_list;
            bret = TRUE;
        } else {
            /* Add to end of list */
            p_now = g_pst_handle_kind_table;
            while (NULL != p_now) {
                if (NULL == p_now->next) {
                    p_now->next = p_add_list;
                    bret = TRUE;
                    break;
                }
                p_now = p_now->next;
            }
        }
    } else {
        bret = FALSE;   /** Parameter error */
    }

    return bret;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            DelList<BR>
                Remove specified data from a List
@outline        DelList<BR>
                Remove specified data from a List
@type            Completion return type
@param[in,out]    HANDLE_TYPE*    h_del_obj    :

@return            BOOL
@retval            TRUE    :    Normal
                FALSE    :    End
*****************************************************************************/
static BOOL DelList(HANDLE_TYPE *h_del_obj) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    HANDLE_TYPE *p_now = NULL;
    HANDLE_TYPE *pBef = NULL;
    BOOL bret = FALSE;

    if (NULL != h_del_obj) {
        /* Add to end of list */
        p_now = g_pst_handle_kind_table;
        while (NULL != p_now) {
            if (h_del_obj == p_now) {
                if (NULL == pBef) {
                    /* Delete first */
                    g_pst_handle_kind_table = p_now->next;
                } else {
                    /* Others */
                    pBef->next = h_del_obj->next;
                }
                bret = TRUE;
                break;
            }
            pBef = p_now;
            p_now = p_now->next;
        }
    } else {
        bret = FALSE;   /** Parameter error */
    }

    return bret;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            FunctionLock<BR>
                Start locking of g_pst_handle_kind_table
@outline        FunctionLock<BR>
                Start locking of g_pst_handle_kind_table
@type            Completion return type
@return            BOOL
@retval            TRUE    :    Normal
@retval            FALSE    :    Error
*****************************************************************************/
static BOOL FunctionLock(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BOOL bret = FALSE;
    if (EOK == pthread_mutex_lock(&g_func_lock_mutex)) {
        bret = TRUE;
    }
    return bret;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            FunctionUnlock<BR>
                Terminate locking of g_pst_handle_kind_table
@outline        FunctionUnlock<BR>
                Terminate locking of g_pst_handle_kind_table
@type            Completion return type
@return            BOOL
@retval            TRUE    :    Normal
@retval            FALSE    :    Error
*****************************************************************************/
static BOOL FunctionUnlock(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BOOL bret = FALSE;
    if (EOK == pthread_mutex_unlock(&g_func_lock_mutex)) {
        bret = TRUE;
    }
    return bret;
}
// LCOV_EXCL_STOP

