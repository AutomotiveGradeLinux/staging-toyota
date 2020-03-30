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


#include "_pbSerial.h"
#include "WPF_STD_private.h"
#include "_pbInternalProc.h"

typedef struct SerialTable {
    HANDLE   h_handle;            /* Registration handle */
    DWORD    dw_wait_mask;            /* SetMask */
    DWORD    dw_read_timeout;        /* milisec */
    DWORD    dw_write_timeout;        /* milisec */
    struct SerialTable *next;
} SERIAL_TABLE;

static SERIAL_TABLE *g_pst_serial_table = NULL;
static pthread_mutex_t g_func_lock_mutex = PTHREAD_MUTEX_INITIALIZER; /* Consider replacing it later */

/* Prototype declarations */
static BOOL FindList(SERIAL_TABLE **p_list, HANDLE h_obj);
static BOOL AddList(SERIAL_TABLE *p_add_list);
static BOOL DelList(SERIAL_TABLE *h_del_obj);
static BOOL FunctionLock(void);
static BOOL FunctionUnlock(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* _sys internal public APIs
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/****************************************************************************
@brief            SerialTableInit<BR>
                Initialize each process
@outline        SerialTableInit<BR>
                Initialize each process
@type            Completion return type

@return            BOOL
@retval            TRUE    :    Normal
@retval            FALSE    :    Error
*****************************************************************************/
BOOL SerialTableInit(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    /* do nothing at this time */
    return TRUE;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            SerialTableTerm<BR>
                termination processing for each process
@outline        SerialTableTerm<BR>
                termination processing for each process
@type            Completion return type

@return            BOOL
@retval            TRUE    :    Normal
@retval            FALSE    :    Error
*****************************************************************************/
BOOL SerialTableTerm(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (NULL != g_pst_serial_table) {
        /* delete the list? */
    }
    return TRUE;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            SerialObjectTimeoutAdd<BR>
                Set read/write timeout
@outline        SerialObjectTimeoutAdd<BR>
                Set read/write timeout
@type            Completion return type

@param[in]    HANDLE    h_obj            :    Handle to set the timeout
@param[in]    DWORD    dw_read_timeout    :    Timeout to read (Millisecond)
@param[in]    DWORD    dw_write_timeout    :   Timeout to write (Millisecond)

@return            BOOL
@retval            TRUE    :    Normal
@retval            FALSE    :    Error
*****************************************************************************/
BOOL SerialObjectTimeoutAdd(HANDLE h_obj, DWORD dw_read_timeout, DWORD dw_write_timeout) {  // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SERIAL_TABLE *p_list = NULL;
    BOOL bret = FALSE;

    if (NULL != h_obj) {
        FunctionLock();
        bret = FindList(&p_list, h_obj);
        if (TRUE == bret) {
            /* Already exists in the list */
            if (NULL != p_list) {
                p_list->dw_read_timeout = dw_read_timeout;
                p_list->dw_write_timeout = dw_write_timeout;
                bret = TRUE;
            } else {
                /* The list pointer is expected to be in the list but cannot be retrieved. */
                bret = FALSE;
            }
        } else {
            /* Not exist in the list */
            p_list = reinterpret_cast<SERIAL_TABLE*>(malloc(sizeof(SERIAL_TABLE)));
            if (NULL != p_list) {
                p_list->next = NULL;
                p_list->dw_wait_mask = 0;
                p_list->h_handle = h_obj;
                p_list->dw_read_timeout = dw_read_timeout;
                p_list->dw_write_timeout = dw_write_timeout;
                bret = AddList(p_list);
                if (FALSE == bret) {
                    /* Registration failure */
                    free(p_list);
                    bret = FALSE;
                } else {
                    /* Registration success */
                    bret = TRUE;
                }
            } else {
                /* Falied to get memory */
                bret = FALSE;
            }
        }
        FunctionUnlock();
    } else {
        /* Parameter error */
        bret = FALSE;
    }

    return bret;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            SerialObjectTimeoutGet<BR>
                Get read/write timeout
@outline        SerialObjectTimeoutGet<BR>
                Get read/write timeout
@type            Completion return type

@param[in]        HANDLE    h_obj            :    Handle for getting the timeout
@param[out]        DWORD*    dw_read_timeout    :    Timeout to read (Millisecond)
@param[out]        DWORD*    dw_write_timeout    :    Timeout to write (Millisecond)

@return            BOOL
@retval            TRUE    :    Normal
@retval            FALSE    :    Error
*****************************************************************************/
BOOL SerialObjectTimeoutGet(HANDLE h_obj, DWORD *dw_read_timeout, DWORD *dw_write_timeout) {  // LCOV_EXCL_START 8:dead code  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SERIAL_TABLE *p_list = NULL;
    BOOL bret = FALSE;

    if ((NULL != h_obj) && (NULL != dw_read_timeout) && (NULL != dw_write_timeout)) {
        FunctionLock();
        bret = FindList(&p_list, h_obj);
        if (TRUE == bret) {
            /* Exists in the list */
            if (NULL != p_list) {
                *dw_read_timeout = p_list->dw_read_timeout;
                *dw_write_timeout = p_list->dw_write_timeout;
                bret = TRUE;
            } else {
                /* The list pointer is expected to be in the list but cannot be retrieved. */
                bret = FALSE;
            }
        } else {
            /* Not exist in the list */
            bret = FALSE;
        }
        FunctionUnlock();
    } else {
        /* Parameter error */
        bret = FALSE;
    }

    return bret;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            SerialObjectWaitmaskAdd<BR>
                Set the mask of event wait factor
@outline        SerialObjectWaitmaskAdd<BR>
                Set the mask of event wait factor
@type            Completion return type

@param[in]    HANDLE    h_obj    :    Handle to set the mask
@param[in]    DWORD    dw_mask    :    Value of mask

@return            BOOL
@retval            TRUE    :    Normal
@retval            FALSE    :    Error
*****************************************************************************/
BOOL SerialObjectWaitmaskAdd(HANDLE h_obj, DWORD dw_mask) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SERIAL_TABLE *p_list = NULL;
    BOOL bret = FALSE;

    if (NULL != h_obj) {
        FunctionLock();
        bret = FindList(&p_list, h_obj);
        if (TRUE == bret) {
            /* already exists in the list */
            if (NULL != p_list) {
                /* Clear unused flags */
                p_list->dw_wait_mask = (DWORD)((dw_mask) & (EV_RXCHAR | EV_ERROR | EV_DSR));
                bret = TRUE;
            } else {
                /* The list pointer is expected to be in the list but cannot be retrieved. */
                bret = FALSE;
            }
        } else {
            /* Not exist in the list */
            p_list = reinterpret_cast<SERIAL_TABLE*>(malloc(sizeof(SERIAL_TABLE)));
            if (NULL != p_list) {
                p_list->next = NULL;
                p_list->h_handle = h_obj;
                p_list->dw_read_timeout = INFINITE;    /* Infinity wait as initial value */
                p_list->dw_write_timeout = INFINITE;    /* Infinity wait as initial value */
                /* Clear unused flags */
                p_list->dw_wait_mask = (DWORD)((dw_mask) & (EV_RXCHAR | EV_ERROR | EV_DSR));
                bret = AddList(p_list);
                if (FALSE == bret) {
                    /* Registration failure */
                    free(p_list);
                    bret = FALSE;
                } else {
                    /* registration success */
                    bret = TRUE;
                }
            } else {
                /* Failed to get memory */
                bret = FALSE;
            }
        }
        FunctionUnlock();
    } else {
        /* Parameter error */
        bret = FALSE;
    }

    return bret;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            SerialObjectWaitmaskGet<BR>
                Get the set mask value from a handle
@outline        SerialObjectWaitmaskGet<BR>
                Get the set mask value from a handle
@type            Completion return type

@param[in]        HANDLE    h_obj    :    Handle from which the mask is to be acquired
@param[out]        DWORD*    dw_mask    :    mask value

@return            BOOL
@retval            TRUE    :    Normal
@retval            FALSE    :    Error
*****************************************************************************/
BOOL SerialObjectWaitmaskGet(HANDLE h_obj, DWORD *dw_mask) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SERIAL_TABLE *p_list = NULL;
    BOOL bret = FALSE;

    if ((NULL != h_obj) && (NULL != dw_mask)) {
        *dw_mask = 0;
        FunctionLock();
        bret = FindList(&p_list, h_obj);
        if (TRUE == bret) {
            /* Exists in the list */
            if (NULL != p_list) {
                *dw_mask = p_list->dw_wait_mask;
                bret = TRUE;
            } else {
                /* The list pointer is expected to be in the list but cannot be retrieved. */
                bret = FALSE;
            }
        } else {
            /* Not exist in the list */
            bret = FALSE;
        }
        FunctionUnlock();
    } else {
        /* Parameter error */
        bret = FALSE;
    }

    return bret;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            SerialObjectDel<BR>
                Delete Timeout and Mask Setting
@outline        SerialObjectDel<BR>
                Delete Timeout and Mask Setting
@type            Completion return type

@param[in]    HANDLE    h_obj    :    Handle from which the setting is to be deleted

@return            BOOL
@retval            TRUE    :    Normal
@retval            FALSE    :    Error
*****************************************************************************/
BOOL SerialObjectDel(HANDLE h_obj) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SERIAL_TABLE *p_list = NULL;
    BOOL bret = FALSE;

    if (NULL != h_obj) {
        FunctionLock();
        bret = FindList(&p_list, h_obj);
        if (TRUE == bret) {
            /* Already exists in the list */
            if (NULL != p_list) {
                bret = DelList(p_list);
                if (TRUE == bret) {
                    free(p_list);
                } else {
                    /* Failed to delete */
                }
            } else {
                /* The list pointer is expected to be in the list but cannot be retrieved. */
                bret = FALSE;
            }
        } else {
            /* Not exist in the list */
        }
        FunctionUnlock();
    } else {
        /* Parameter error */
    }

    return bret;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Private APIs
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/****************************************************************************
@brief            FindList<BR>
                Searching for a Handle in the List
@outline        FindList<BR>
                Searching for a Handle in the List
@type            Completion return type

@param[out]        SERIAL_TABLE**    p_list    :    Found list pointer
@param[in]        HANDLE            h_obj    :    Handle to look for

@return            BOOL
@retval            TRUE    :    Normal (p_list != NULL)
@retval            FALSE    :    Error (p_list == NULL)
*****************************************************************************/
static BOOL FindList(SERIAL_TABLE **p_list, HANDLE h_obj) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SERIAL_TABLE *pNow = NULL;
    BOOL bret = FALSE;

    if ((NULL != h_obj) && (NULL != p_list)) {
        /* Search list */
        pNow = g_pst_serial_table;
        while (NULL != pNow) {
            /* h_obj and pNow->h_handle are pointer type.*/
            if ((int64_t)h_obj == (int64_t)pNow->h_handle) {
                *p_list = pNow;
                bret = TRUE;
                break;
            }
            pNow = pNow->next;
        }
    } else {
        bret = FALSE; /* Parameter error */
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

@param[in]        SERIAL_TABLE*    p_list    :    Data to add

@return            BOOL
@retval            TRUE    :    Normal
@retval            FALSE    :    Error
*****************************************************************************/
static BOOL AddList(SERIAL_TABLE *p_add_list) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SERIAL_TABLE *pNow = NULL;
    BOOL bret = FALSE;

    if (NULL != p_add_list) {
        /* Add unregistered data */
        if (NULL == g_pst_serial_table) {
            g_pst_serial_table = p_add_list;
            bret = TRUE;
        } else {
            /* Add to end of list */
            pNow = g_pst_serial_table;
            while (NULL != pNow) {
                if (NULL == pNow->next) {
                    pNow->next = p_add_list;
                    bret = TRUE;
                    break;
                }
                pNow = pNow->next;
            }
        }
    } else {
        bret = FALSE; /* Parameter error */
    }

    return bret;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            DelList<BR>
                Remove Specified Data from a List
@outline        DelList<BR>
                Remove Specified Data from a List
@type            Completion return type

@param[in,out]    SERIAL_TABLE*    h_del_obj    :

@return            BOOL
@retval            TRUE    :    Normal
@retval            FALSE    :    Error
*****************************************************************************/
static BOOL DelList(SERIAL_TABLE *h_del_obj) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SERIAL_TABLE *pNow = NULL;
    SERIAL_TABLE *pBef = NULL;
    BOOL bret = FALSE;

    if (NULL != h_del_obj) {
        /* Add to end of list */
        pNow = g_pst_serial_table;
        while (NULL != pNow) {
            if (h_del_obj == pNow) {
                if (NULL == pBef) {
                    /* Delete first data */
                    g_pst_serial_table = pNow->next;
                } else {
                    /* Others */
                    pBef->next = h_del_obj->next;
                }
                bret = TRUE;
                break;
            }
            pBef = pNow;
            pNow = pNow->next;
        }
    } else {
        bret = FALSE; /* Parameter error */
    }

    return bret;
}
// LCOV_EXCL_STOP

/****************************************************************************
@brief            FunctionLock<BR>
                Start locking g_pst_serial_table
@outline        FunctionLock<BR>
                Start locking g_pst_serial_table
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

/***************************************************************************
@brief            FunctionUnlock<BR>
                Terminate locking of g_pst_serial_table
@outline        FunctionUnlock<BR>
                Terminate locking of g_pst_serial_table
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

