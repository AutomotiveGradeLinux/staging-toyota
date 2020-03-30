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

/**
 * @file
 *    _pbMsg.cpp
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <native_service/frameworkunified_types.h>
#include <vehicle_service/positioning_base_library.h>
#include <native_service/ns_message_center_if.h>
#include "_pbEvent_Internal.h"
#include "_pbInternalProc.h"

#include <native_service/frameworkunified_framework_if.h>
#include "WPF_STD_private.h"
#include "tchar.h"

/*---------------------------------------------------------------------------------*
 * Define                                                                          *
 *---------------------------------------------------------------------------------*/
#define MSG_MAX_NUM_CTRL_MSGQUE       (32)    /* Maximum number of message queues              */
#define MSG_MAX_NUM_CTRL_THREAD       (16)    /* Maximum number of threads/process           */

#define FULL_MSG_NUM_CTRL_MSGQUE      (MSG_MAX_NUM_CTRL_MSGQUE - 4)  /* Message control table threshold (no free) */
#define WARN_MSG_NUM_CTRL_MSGQUE      (MSG_MAX_NUM_CTRL_MSGQUE - 10) /* Message control table threshold (warning) */

/*---------------------------------------------------------------------------------*
 * Structure                                                                       *
 *---------------------------------------------------------------------------------*/
/*!
   @brief    Control table for message queues
*/
typedef struct {
    PNO     pno;                                 /**< Process No.             */
    char    name[MAX_QUEUE_NAME_SIZE];           /**< Message Que Name         */
    HANDLE  h_positioningbaselibrary_sender[MSG_MAX_NUM_CTRL_THREAD]; /**< handle to the Sender   */
    HANDLE  h_sender;                             /**< handle to the Sender   */
    HANDLE  h_receiver;                           /**< handle to the Receiver */
    HANDLE  h_zc_sender;                           /**< handle to the ZcSender */
    uint8_t msg_rcv_buf[MAX_QUEUE_MSG_SIZE];       /**< Message buffer         */
} MSG_CTRL_DETAIL_INFO;

/*!
   @brief    Control table for message queues
*/
typedef struct {
    MSG_CTRL_DETAIL_INFO info[MSG_MAX_NUM_CTRL_MSGQUE];   /**< message infomation */
    uint32_t             use_cnt;                          /**< Use Counter        */
    uint32_t             rsv_cnt;                          /**< Reserve Counter    */
} MSG_CTRL_INFO;

/*---------------------------------------------------------------------------------*
 * Grobal Value                                                                    *
 *---------------------------------------------------------------------------------*/
/**
   Message control table
   Note: Access to this instance shall be made through the operation module.
*/
static MSG_CTRL_INFO    g_msg_ctrl_tbl;   // NOLINT(readability/nolint)  global Class instance

/** Message-control-table-locking Mutex handles */
static HANDLE            g_h_mtx;

/*---------------------------------------------------------------------------------*
 * Local Function Prototype                                                        *
 *---------------------------------------------------------------------------------*/
/* Message Control Table Operation Functions */
static void     MsgSetPnoOfCtrlTbl(u_int32 idx, PNO pno);                  /* PNO setting                     */
static PNO      MsgGetPnoOfCtrlTbl(u_int32 idx);                           /* PNO acquisition                     */
static void     MsgSetNameOfCtrlTbl(u_int32 idx, LPCTSTR name);            /* Message queue name setting        */
static char*    MsgGetNameOfCtrlTbl(u_int32 idx);                          /* Get Message Queue Name        */
static void     MsgSetReceiverHandleOfCtrlTbl(u_int32 idx, HANDLE handle); /* Message queue handle setting    */
static HANDLE   MsgGetReceiverHandleOfCtrlTbl(u_int32 idx);                /* Get message queue handle    */
static void     MsgSetSenderHandleOfCtrlTbl(u_int32 idx, HANDLE handle);    /* Message send handle setting    */
static HANDLE   MsgGetSenderHandleOfCtrlTbl(u_int32 idx);                    /* Get message send handle    */
/* Message send handle setting*/
static void     MsgSetPositioningbaselibrarySenderHandleOfCtrlTbl(u_int32 idx, HANDLE handle, uint32_t offset);
static HANDLE   MsgGetPositioningbaselibrarySenderHandleOfCtrlTbl(u_int32 idx, uint32_t offset);   /* Get message send handle */
static void     MsgSetZcSenderHandleOfCtrlTbl(u_int32 idx, HANDLE handle);     /* Message send handle setting */
static HANDLE   MsgGetZcSenderHandleOfCtrlTbl(u_int32 idx);     /* Get message send handle   */
static uint8_t* MsgGetMsgRcvBufOfCtrlTbl(uint32_t idx);       /* Get message receive buffer   */
static u_int32  MsgSearchEmptyOfCtrlTbl(void);                   /* Retrieval of free space in control table        */
static u_int32  MsgSearchPnoOfCtrlTbl(PNO pno);               /* Retrieval of control table PNO            */
static u_int32  MsgSearchNameOfCtrlTbl(LPCTSTR name);           /* Retrieve control table queue name        */
static void     MsgIncUseCntOfCtrlTbl(void);                  /* Control table usage counter increment */
static void     MsgDecUseCntOfCtrlTbl(void);                  /* Control table usage counter increment */
static void     MsgIncRsvCntOfCtrlTbl(void);                  /* Control table reservation counter increment */
static void     MsgDecRsvCntOfCtrlTbl(void);                  /* Control table reservation counter increment */

/* Mutex handling Functions for Accessing Message Control Tables */
static void MsgCreateMutex(void);    /* Mutex generating */
/* Message send handle setting */
/* static void MsgDeleteMutex(void); */   // Todo:Uncomment out after completion of implementation of termination processing
static void MsgLockMutex(void);    /* Mutex retrieval */
static void MsgUnlockMutex(void);    /* Mutex release */

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   Initialize the message function
 *
 *   Message control table initialization
 *
 * @return    RET_NORMAL        Normal completion<br>
 *            RET_ERRPARAM    Parameter error
 */
RET_API MsgInit(void) {
    RET_API ret_api = RET_NORMAL;
    u_int32 i;

    MsgCreateMutex();

    /* Control table initialization */
    memset(g_msg_ctrl_tbl.info, 0x00, sizeof(g_msg_ctrl_tbl.info));

    for (i = 0; i < MSG_MAX_NUM_CTRL_MSGQUE; i++) {
        /* Empty character is set for the message queue name. */
        _tcscpy(g_msg_ctrl_tbl.info[i].name, "");
    }
    g_msg_ctrl_tbl.use_cnt = 0;
    g_msg_ctrl_tbl.rsv_cnt = 0;

    return ret_api;
}

/**
 * @brief
 *   Term
 *
 * @return    RET_NORMAL    Normal completion<br>
 *               RET_ERROR    ABENDs
 */
RET_API MsgTerm(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API    ret_api = RET_ERROR;
    RET_API    ret;
    EFrameworkunifiedStatus estatus;
    HANDLE     handle;
    u_int32    idx;
    PNO        pno;

    MsgLockMutex();

    /* Release Message Transmission/Reception Handle */
    for (idx = 0; idx < MSG_MAX_NUM_CTRL_MSGQUE; idx++) {
        /* Receive handle acquisition */
        pno = MsgGetPnoOfCtrlTbl(idx);
        if (pno != 0) {
            /* For queue information for internal threads */
            /* Delete Message Queue */
            ret = PbDeleteMsg(pno);
            if (ret != RET_NORMAL) {
                FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "McClose ERROR " \
                    "[ret:%d, pno:%d]", ret, pno);
                /* The module returns OK even if it fails. */
            }
        } else {
            /* Other than the above(Send handle to external process) */
            /* Get send handle */
            handle = MsgGetSenderHandleOfCtrlTbl(idx);
            if (handle != NULL) {
                estatus = FrameworkunifiedMcClose(handle);
                if (estatus != eFrameworkunifiedStatusOK) {
                    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "FrameworkunifiedMcClose ERROR " \
                        "[estatus:%d, handle:%p]", estatus, handle);
                    /* The module returns OK even if it fails. */
                } else {
                    /* Message control table update */
                    MsgSetSenderHandleOfCtrlTbl(idx, NULL); /* Send handle */

                    MsgSetNameOfCtrlTbl(idx, ""); /* Name */
                }

                FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "### MESSAGE TABLE INFORMATION # " \
                    "(--) idx=%d, pno=%d, h_positioningbaselibrary_sender[%d]=%p, h_sender=%p, h_receiver=%p, name=%s",
                    idx, MsgGetPnoOfCtrlTbl(idx), PbGetLocalTid(), MsgGetPositioningbaselibrarySenderHandleOfCtrlTbl(idx, \
                    PbGetLocalTid()), MsgGetSenderHandleOfCtrlTbl(idx), MsgGetReceiverHandleOfCtrlTbl(idx), \
                    MsgGetNameOfCtrlTbl(idx));
            }
        }
    }

    MsgUnlockMutex();

    /* MsgDeleteMutex(); */  // Todo:Uncomment out after completion of implementation of termination processing

    /* TODO:Delete the shared memory for the message management table */

    return ret_api;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Create the message queue
 *
 *    Creates a message queue.
 *   This function is implemented on the assumption that a thread name is assigned by prctl() and then called from that thread.
 *   The generated message queue name is the same as the thread name.
 *
 * @param[in]  pno    Process number
 *
 * @return    RET_NORMAL        Normal completion<br>
 *            RET_ERRPARAM    Parameter error<br>
 *            RET_ERROR        Other errors
 */
RET_API _pb_CreateMsg(PNO pno) {   // NOLINT(readability/nolint)  interface
    RET_API    ret_api = RET_NORMAL; /* Results of this Module process */
    u_int32    idx;
    HANDLE    handle = NULL;
    char    name[MAX_QUEUE_NAME_SIZE];
    size_t  len;

    /* If PNO is invalid (0), the parameter is abnormal and processing is not performed. */
    if (pno == 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [pno:%d]", pno);
        ret_api = RET_ERRPARAM;
    } else {
        /* Get Thread Name */
        (void)prctl(PR_GET_NAME, name);

        len = _tcslen(name);
        if (len >= MAX_QUEUE_NAME_SIZE) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Length of thread name is too long(>=%d). "  \
                "[len:%zu]", MAX_QUEUE_NAME_SIZE, len);
            ret_api = RET_ERRPARAM;
        } else {
            MsgLockMutex();  // LCOV_EXCL_BR_LINE 200: no branch

            /* Check if the specified PNO is already registered */
            idx = MsgSearchPnoOfCtrlTbl(pno);  // LCOV_EXCL_BR_LINE 200: no branch

            /* When the entry is already stored */
            if (idx != MSG_MAX_NUM_CTRL_MSGQUE) {
                /* No processing */
                FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "### MESSAGE TABLE INFORMATION # " \
                    "(+) idx=%d, pno=%d, h_positioningbaselibrary_sender[%d]=%p, h_sender=%p, h_receiver=%p, name=%s", \
                    idx, MsgGetPnoOfCtrlTbl(idx), PbGetLocalTid(), MsgGetPositioningbaselibrarySenderHandleOfCtrlTbl(idx, \
                    PbGetLocalTid()), MsgGetSenderHandleOfCtrlTbl(idx), MsgGetReceiverHandleOfCtrlTbl(idx), \
                    MsgGetNameOfCtrlTbl(idx));
            } else {
                /* Not registered */
                /* Search for free space */
                idx = MsgSearchEmptyOfCtrlTbl();
                if (idx == MSG_MAX_NUM_CTRL_MSGQUE) {  // LCOV_EXCL_BR_LINE 200: idx can not be MSG_MAX_NUM_CTRL_MSGQUE
                    /* Be impossible by design */
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "MsgSearchEmptyOfCtrlTbl ERROR " \
                        "[idx:%d]", idx);
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    _pb_Exit();  // LCOV_EXCL_LINE 200: idx can not be MSG_MAX_NUM_CTRL_MSGQUE

                    /* don't arrive here. */
                } else {
                    /* Create Message Queue */
                    handle = McOpenReceiver(name);  // LCOV_EXCL_BR_LINE 4: nsfw error
                    if (handle == NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                        /* In the event of failure */
                        // LCOV_EXCL_START 5: standard lib error
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McOpenReceiver ERROR " \
                            "[handle:%p, name:%s]", handle, name);
                        _pb_Exit();
                        // LCOV_EXCL_STOP
                        /* don't arrive here. */
                    } else {
                        /* If successful */
                        /* Message control table update */
                        MsgSetPnoOfCtrlTbl(idx, pno); /* PNO */  // LCOV_EXCL_BR_LINE 200: no branch

                        MsgSetReceiverHandleOfCtrlTbl(idx, handle); /* Receive handle */  // LCOV_EXCL_BR_LINE 200: no branch  // NOLINT(whitespace/line_length)

                        MsgSetNameOfCtrlTbl(idx, name); /* Name */  // LCOV_EXCL_BR_LINE 200: no branch

                        /* Increment Message Control Table Usage Counter */
                        MsgIncUseCntOfCtrlTbl();

                        FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, \
                            "### MESSAGE TABLE INFORMATION # (++) idx=%d, pno=%d, h_positioningbaselibrary_sender[%d]=%p, " \
                            "h_sender=%p, h_receiver=%p, name=%s", idx, MsgGetPnoOfCtrlTbl(idx), \
                            PbGetLocalTid(), MsgGetPositioningbaselibrarySenderHandleOfCtrlTbl(idx, PbGetLocalTid()), \
                            MsgGetSenderHandleOfCtrlTbl(idx), MsgGetReceiverHandleOfCtrlTbl(idx), \
                            MsgGetNameOfCtrlTbl(idx));
                    }
                }
            }

            MsgUnlockMutex();  // LCOV_EXCL_BR_LINE 200: no branch
        }
    }

    return ret_api;
}

/**
 * @brief
 *   Delete the message queue
 *
 *    Delete a message queue.
 *   Deleted from the message control table even if closing of the send/receive handle fails
 *
 * @param[in]  pno
 *
 * @return    RET_NORMAL    Normal completion
 */
RET_API PbDeleteMsg(PNO pno) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RET_API    ret_api = RET_NORMAL; /* Results of this Module process        */
    uint32_t idx;
    uint32_t i;
    HANDLE handle;
    EFrameworkunifiedStatus estatus;

    idx = MsgSearchPnoOfCtrlTbl(pno);
    if (idx != MSG_MAX_NUM_CTRL_MSGQUE) {
        /* Receive handle acquisition */
        handle = MsgGetReceiverHandleOfCtrlTbl(idx);
        if (handle != NULL) {
            estatus = McClose(handle);
            if (estatus != eFrameworkunifiedStatusOK) {
                FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "McClose ERROR " \
                    "[estatus:%d, handle:%p]", estatus, handle);
                /* The module returns OK even if the Close fails. */
            }
        }

        /* Get send handle */
        handle = MsgGetSenderHandleOfCtrlTbl(idx);
        if (handle != NULL) {
            estatus = McClose(handle);
            if (estatus != eFrameworkunifiedStatusOK) {
                FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "McClose ERROR " \
                    "[estatus:%d, handle:%p]", estatus, handle);
                /* The module returns OK even if the Close fails. */
            }
        }

        /* Get send handle */
        handle = MsgGetZcSenderHandleOfCtrlTbl(idx);
        if (handle != NULL) {
            estatus = McZcClose(handle);
            if (estatus != eFrameworkunifiedStatusOK) {
                FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "McZcClose ERROR " \
                    "[estatus:%d, handle:%p]", estatus, handle);
                /* The module returns OK even if the Close fails. */
            }
        }

        /* Message control table update */
        MsgSetPnoOfCtrlTbl(idx, 0); /* PNO */

        for (i = 0; i < MSG_MAX_NUM_CTRL_THREAD; i++) {
            MsgSetPositioningbaselibrarySenderHandleOfCtrlTbl(idx, NULL, i); /* Send handle */
        }

        MsgSetSenderHandleOfCtrlTbl(idx, NULL); /* Send handle */

        MsgSetReceiverHandleOfCtrlTbl(idx, NULL); /* Receive handle */

        MsgSetNameOfCtrlTbl(idx, ""); /* Name */

        /* Decrement Message Control Table Usage Counter */
        MsgDecUseCntOfCtrlTbl();

        FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "### MESSAGE TABLE INFORMATION # " \
            "(--) idx=%d, pno=%d, h_positioningbaselibrary_sender[%d]=%p, h_sender=%p, h_receiver=%p, name=%s", \
            idx, MsgGetPnoOfCtrlTbl(idx), PbGetLocalTid(), MsgGetPositioningbaselibrarySenderHandleOfCtrlTbl(idx, \
            PbGetLocalTid()), MsgGetSenderHandleOfCtrlTbl(idx), MsgGetReceiverHandleOfCtrlTbl(idx), \
            MsgGetNameOfCtrlTbl(idx));
    }

    return ret_api;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Receive the message
 *
 *    Receive a message for the specified PNO.If the specified PNO is invalid, an error is returned.
 *    If the size of the received data exceeds the size specified in the parameter, an error is returned.
 *
 * @param[in]  pno            Process number
 * @param[in]  size        Message size
 * @param[out]  msgbuf    Pointer to message area
 * @param[in]  mode        Not used
 *
 * @return    RET_NORMAL        Normal completion<br>
 *            RET_ERRPARAM    Parameter error<br>
 *            RET_ERROR        Other errors
 */
RET_API _pb_RcvMsg(PNO pno, u_int16 size, void** msgbuf, u_int16 mode) {   // NOLINT(readability/nolint)  interface
    RET_API    ret_api = RET_RCVMSG;   /* Results of this Module process        */
    u_int32    idx;
    HANDLE     h_msg_que = NULL;
    char       source[MAX_QUEUE_NAME_SIZE];
    u_int32    cmd_id;
    EFrameworkunifiedStatus rcv_sts;
    u_int8*    p_msg_rcv_buf;
    u_int32    msg_len = 0;
    void*      p_rcv_data;

    /* Null Check */
    if (msgbuf == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [msgbuf:%p]", msgbuf);
        ret_api = RET_ERRPARAM;
    } else if (pno == 0) {
        /* PNO-invalid Check */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [pno:%d]", pno);
        ret_api = RET_ERRPARAM;
    } else {
        /* Search if the specified PNO is registered in the control table */
        idx = MsgSearchPnoOfCtrlTbl(pno);  // LCOV_EXCL_BR_LINE 200: no branch
        /* Not stored */
        if (idx == MSG_MAX_NUM_CTRL_MSGQUE) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "MsgSearchPnoOfCtrlTbl ERROR " \
                "[idx:%d, pno:0x%x]", idx, pno);
            ret_api = RET_ERRPARAM;
        } else {
            /* If it is registered */
            /* Handle acquisition */
            h_msg_que = MsgGetReceiverHandleOfCtrlTbl(idx);  // LCOV_EXCL_BR_LINE 200: recv handle created in _pb_CreateMsg  //NOLINT(whitespace/line_length)
            if (h_msg_que == NULL) {  // LCOV_EXCL_BR_LINE 200: recv handle created in _pb_CreateMsg
                /* Be impossible by design */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Msg_getMsgQueHandleOfCtrlTbl ERROR " \
                    "[h_msg_que:%p, idx:%d]", h_msg_que, idx);
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                _pb_Exit();  // LCOV_EXCL_LINE 200: recv handle created in _pb_CreateMsg

                /* don't arrive here. */
            }

            /* Get Message Buffer */
            p_msg_rcv_buf = MsgGetMsgRcvBufOfCtrlTbl(idx);  // LCOV_EXCL_BR_LINE 200: no branch

            /* Receive message */
            // LCOV_EXCL_BR_START 4: nsfw error
            rcv_sts = McReceive(h_msg_que,
                               source, /* app that sent this message */
                               &cmd_id, /* Command ID */
                               MAX_QUEUE_MSG_SIZE,
                               (PVOID)p_msg_rcv_buf);
            // LCOV_EXCL_BR_STOP

            if (rcv_sts != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                /* In the event of failure */
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McReceive ERROR " \
                    "[rcv_sts:%d, h_msg_que:%p, source:%s, cmd_id:0x%x]", rcv_sts, h_msg_que, source, cmd_id);
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                ret_api = RET_ERROR;  // LCOV_EXCL_LINE 4: nsfw error
            } else {
                /* If successful */
                /* Size check */
                msg_len = McGetLength(p_msg_rcv_buf);  // LCOV_EXCL_BR_LINE 4: nsfw error
                if ((msg_len <= size) && (msg_len > 0)) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                    /* OK */
                    /* Get Message */
                    p_rcv_data = McGetDataPointer(p_msg_rcv_buf);  // LCOV_EXCL_BR_LINE 4: nsfw error
                    if (p_rcv_data == NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                        // LCOV_EXCL_START 4: nsfw error
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        rcv_sts = McGetDataOfSize(p_msg_rcv_buf, *msgbuf, msg_len);
                        if (rcv_sts != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                            /* Message acquisition failure */
                            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                                "McGetDataOfSize ERROR [rcv_sts:%d]", rcv_sts);
                            ret_api = RET_ERROR;
                            // LCOV_EXCL_STOP
                        } else {
                            FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, \
                                "McReceive/McGetDataOfSize SUCCESS [rcv_sts:%d, h_msg_que:%p, source:%s, " \
                                "cmd_id:0x%x, msg_len:%d]", rcv_sts, h_msg_que, source, cmd_id, msg_len);
                        }
                    } else {
                        *msgbuf = p_rcv_data;
                    }
                } else {
                    /* NG */
                    // LCOV_EXCL_START 4: nsfw error
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McGetLength ERROR " \
                        "[size:%d < msg_len:%d]", size, msg_len);
                    ret_api = RET_ERROR;
                    // LCOV_EXCL_STOP
                }
            }
        }
    }

    return ret_api;
}

/**
 * @brief
 *   Send the message
 *
 *   For in-process communication
 *    Sends a message to the specified PNO.If the specified PNO is invalid, an error is returned.
 *
 * @param[in]  pno        Process number
 * @param[in]  size    Message size
 * @param[in]  msgbuf    Pointer to message storage area
 * @param[in]  mode    Not used
 *
 * @return    RET_NORMAL        Normal completion<br>
 *            RET_ERRPARAM    Parameter error<br>
 *            RET_ERROR        Other errors
 */
RET_API _pb_SndMsg(PNO pno, u_int16 size, void* msgbuf, u_int16 mode) {   // NOLINT(readability/nolint)  interface
    RET_API    ret_api = RET_NORMAL; /* Results of this Module process */
    u_int32    idx;
    PCSTR      msg_que_name;
    HANDLE     handle; /* handle to the send message queue */
    EFrameworkunifiedStatus estatus;
    T_APIMSG_MSGBUF_HEADER *p_msg_header;
    CID        cid;

    /* Null Check */
    if (msgbuf == NULL) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [msgbuf:%p]", msgbuf);
        ret_api = RET_ERRPARAM;
    } else if (pno == 0) {
        /* PNO-invalid Check */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [pno:%d]", pno);
        ret_api = RET_ERRPARAM;
    } else {
        MsgLockMutex();

        /* Check if the specified PNO is registered in the message control table */
        idx = MsgSearchPnoOfCtrlTbl(pno);
        if (idx == MSG_MAX_NUM_CTRL_MSGQUE) {
            /* Not stored */
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "MsgSearchPnoOfCtrlTbl ERROR!! " \
                "[pno=%d]", pno);

            ret_api = RET_ERRPARAM;
        } else {
            /* If it is registered */
            /* Acquire transmission handle from management table */
            handle = MsgGetSenderHandleOfCtrlTbl(idx);
            if (handle == NULL) {
                /* Get message queue name */
                msg_que_name = MsgGetNameOfCtrlTbl(idx);

                /* Get send handle  */
                handle = McOpenSender(msg_que_name);  // LCOV_EXCL_BR_LINE 4: nsfw error
                /* When handle acquisition fails */
                if (handle == NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                    // LCOV_EXCL_START 4: nsfw error
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McOpenSender ERROR " \
                        "[handle:%p, msg_que_name:%s]", handle, msg_que_name);

                    ret_api = RET_ERROR;
                    // LCOV_EXCL_STOP
                }

                /* Message control table update */
                MsgSetSenderHandleOfCtrlTbl(idx, handle); /* Send handle */

                FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "### MESSAGE TABLE INFORMATION # " \
                    "(+) idx=%d, pno=%d, h_positioningbaselibrary_sender[%d]=%p, h_sender=%p, h_receiver=%p, name=%s", \
                    idx, MsgGetPnoOfCtrlTbl(idx), PbGetLocalTid(), MsgGetPositioningbaselibrarySenderHandleOfCtrlTbl(idx, \
                    PbGetLocalTid()), MsgGetSenderHandleOfCtrlTbl(idx), MsgGetReceiverHandleOfCtrlTbl(idx), \
                    MsgGetNameOfCtrlTbl(idx));
            }
            /* When handle acquisition is successful */
            if (handle != NULL) {
                p_msg_header = reinterpret_cast<T_APIMSG_MSGBUF_HEADER*>(msgbuf);
                cid = p_msg_header->hdr.cid;

                /* Messaging */
                estatus = McSend(handle,
                                 "", /* Sender name */
                                 cid, /* Command ID */
                                 size,
                                 msgbuf);  // LCOV_EXCL_BR_LINE 4: nsfw error
                /* When transmission fails */
                if (estatus != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                    // LCOV_EXCL_START 4: nsfw error
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McSend ERROR " \
                        "[estatus:%d, idx:%d, handle:%p, cid:0x%x, size:%d]", estatus, idx, handle, cid, size);
                    ret_api = RET_ERROR;
                    // LCOV_EXCL_STOP
                } else {
                    FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "McSend SUCCESS " \
                        "[estatus:%d, idx:%d, handle:%p, cid:0x%x, size:%d]", estatus, idx, handle, cid, size);
                }

                /* Close all handles at the time of termination */
            }
        }

        MsgUnlockMutex();
    }

    return ret_api;
}

/**
 * @brief
 *   Acquire transmission buffer for in-process communication (for non-copy API)
 *   Gets the buffer for sending messages to the specified PNO.If the specified PNO is invalid, an error is returned.
 *   Set the send data in the acquired buffer and send it with _pb_ZcSndMsg().
 *
 * @param[in]  pno        Process number
 * @param[out] p_snd_buf    Transmitting buffer
 *
 * @return    RET_NORMAL        Normal completion<br>
 *            RET_ERRPARAM    Parameter error<br>
 *            RET_ERROR        Other errors
 */
RET_API _pb_GetZcSndBuf(PNO pno, void** p_snd_buf) {   // NOLINT(readability/nolint)  interface
    RET_API     ret_api = RET_NORMAL; /* Results of this Module process */
    u_int32     idx;
    PCSTR       msg_que_name;
    HANDLE      handle; /* handle to the send message queue */

    /* PNO-invalid Check */
    if (pno == 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [pno:%d]", pno);
        ret_api = RET_ERRPARAM;
    } else {
        MsgLockMutex();

        /* Check if the specified PNO is registered in the message control table */
        idx = MsgSearchPnoOfCtrlTbl(pno);
        if (idx == MSG_MAX_NUM_CTRL_MSGQUE) {
            /* Not stored */
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "MsgSearchPnoOfCtrlTbl ERROR!! [pno=%d]", pno);

            ret_api = RET_ERRPARAM;
        } else {
            /* If it is registered */
            /* Acquire transmission handle from management table */
            handle = MsgGetZcSenderHandleOfCtrlTbl(idx);
            if (handle == NULL) {
                /* Get message queue name */
                msg_que_name = MsgGetNameOfCtrlTbl(idx);

                /* Get send handle  */
                handle = McZcOpenSender(msg_que_name);  // LCOV_EXCL_BR_LINE 4: nsfw error
                if (handle == NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                    // LCOV_EXCL_START 4: nsfw error
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    /* When handle acquisition fails */
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McOpenSender ERROR " \
                        "[handle:%p, msg_que_name:%s]", handle, msg_que_name);

                    ret_api = RET_ERROR;
                    // LCOV_EXCL_STOP
                }

                /* Message control table update */
                MsgSetZcSenderHandleOfCtrlTbl(idx, handle); /* Send handle */

                FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "### MESSAGE TABLE INFORMATION # " \
                    "(+) idx=%d, pno=%d, h_positioningbaselibrary_sender[%d]=%p, h_sender=%p, h_receiver=%p, name=%s",
                    idx, MsgGetPnoOfCtrlTbl(idx), PbGetLocalTid(), MsgGetPositioningbaselibrarySenderHandleOfCtrlTbl(idx, \
                    PbGetLocalTid()), MsgGetSenderHandleOfCtrlTbl(idx), MsgGetReceiverHandleOfCtrlTbl(idx), \
                    MsgGetNameOfCtrlTbl(idx));
            }

            if (handle != NULL) {
                /* When handle acquisition is successful */
                /* Get message send buffer */
                *p_snd_buf = McZcGetBuf(handle);  // LCOV_EXCL_BR_LINE 4: nsfw error
                if (*p_snd_buf == NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                    // LCOV_EXCL_START 4: nsfw error
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    /* When buffer acquisition fails */
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McZcSetParam ERROR " \
                        "[idx:%d, handle:%p]", idx, handle);
                    ret_api = RET_ERROR;
                    // LCOV_EXCL_STOP
                }

                /* Close all handles at the time of termination */
            }
        }

        MsgUnlockMutex();
    }

    return ret_api;
}

/**
 * @brief
 *   Send the message
 *
 *   For in-process communication(for non-copy API)
 *   Sends a message to the specified PNO.If the specified PNO is invalid, an error is returned.
 *   Before calling this function, send data must be set in the area acquired by _pb_GetZcSndBuf().
 *
 * @param[in]  pno        Process number
 * @param[in]  size    Message size
 * @param[in]  mode    Not used
 *
 * @return    RET_NORMAL        Normal completion<br>
 *            RET_ERRPARAM    Parameter error<br>
 *            RET_ERROR        Other errors
 */
RET_API _pb_ZcSndMsg(PNO pno, u_int16 size, u_int16 mode) {   // NOLINT(readability/nolint)  interface
    RET_API    ret_api = RET_NORMAL; /* Results of this Module process */
    u_int32    idx;
    HANDLE    handle; /* handle to the send message queue */
    EFrameworkunifiedStatus    estatus;
    T_APIMSG_MSGBUF_HEADER *p_msg_header;
    CID        cid;
    void* p_send_data;

    /* PNO-invalid Check */
    if (pno == 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [pno:%d]", pno);
        ret_api = RET_ERRPARAM;
    } else {
        MsgLockMutex();

        /* Check if the specified PNO is registered in the message control table */
        idx = MsgSearchPnoOfCtrlTbl(pno);
        if (idx == MSG_MAX_NUM_CTRL_MSGQUE) {
            /* Not stored */
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "MsgSearchPnoOfCtrlTbl ERROR!! [pno=%d]", pno);

            ret_api = RET_ERRPARAM;
        } else {
            /* If it is registered */
            /* Acquire transmission handle from management table */
            handle = MsgGetZcSenderHandleOfCtrlTbl(idx);  // LCOV_EXCL_BR_LINE 200: sender handle set in _pb_GetZcSndBuf
            if (handle == NULL) {  // LCOV_EXCL_BR_LINE 200: sender handle set in _pb_GetZcSndBuf
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "handle ERROR " \
                    "[pno=%d, idx=%d]", pno, idx);

                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                ret_api = RET_ERROR;  // LCOV_EXCL_LINE 200: sender handle set in _pb_GetZcSndBuf
            }
            /* When handle acquisition is successful */
            if (handle != NULL) {
                /* Messaging */
                /* Set the transmission data in advance. */
                p_send_data = McZcGetBuf(handle);
                if (p_send_data != NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                    p_msg_header = reinterpret_cast<T_APIMSG_MSGBUF_HEADER*>(p_send_data);
                    cid = p_msg_header->hdr.cid;
                    estatus = McZcSetParam(handle, cid, size);
                    if (estatus == eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                        estatus = McZcSend(handle);  // LCOV_EXCL_BR_LINE 4: nsfw error
                        /* When transmission fails */
                        if (estatus != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McZcSend ERROR "  \
                                "[estatus:%d, idx:%d, handle:%p, cid:0x%x, size:%d]", estatus, \
                                idx, handle, cid, size);
                            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                            ret_api = RET_ERROR;  // LCOV_EXCL_LINE 4: nsfw error
                        } else {
                            FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "McZcSend SUCCESS " \
                                "[estatus:%d, idx:%d, handle:%p, cid:0x%x, size:%d]", estatus, \
                                idx, handle, cid, size);
                        }
                    } else {
                        /* When parameter setting fails */
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McZcSetParam ERROR " \
                            "[estatus:%d, idx:%d, handle:%p, cid:0x%x, size:%d]", estatus, idx, handle, cid, size);
                        ret_api = RET_ERROR;
                    }
                } else {
                    /* When parameter buffer acquisition fails */
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McZcGetBuf ERROR " \
                        "[idx:%d, handle:%p, size:%d]", idx, handle, size);
                    ret_api = RET_ERROR;
                }

                /* Close all handles at the time of termination */
            }
        }

        MsgUnlockMutex();
    }

    return ret_api;
}

/**
 * @brief
 *   Send the message
 *
 *   For interprocess communication
 *    Sends a message to the specified process.If the specified process is invalid, an error is returned.
 *
 *   Note: Since the data sent by this function is to be received by the dispatcher, the message header
 *      must not be included in the send data specified by the argument.
 *
 * @param[in]  name    Destination Process Name
 * @param[in]  cid        Command ID
 * @param[in]  size    Message size
 * @param[in]  msgbuf    Pointer to message storage area
 * @param[in]  mode    Not used
 *
 * @return    RET_NORMAL        Normal completion<br>
 *            RET_ERRPARAM    Parameter error<br>
 *            RET_ERROR        Other errors
 */
RET_API _pb_SndMsg_Ext(PCSTR name, CID cid, u_int16 size, const void* msgbuf,   // NOLINT(readability/nolint) interface
        u_int16 mode) {
    RET_API     ret_api = RET_NORMAL; /* Results of this Module process */
    uint32_t    thread_offset;
    u_int32     idx;
    HANDLE      h_positioningbaselibrary_service = NULL;
    EFrameworkunifiedStatus  estatus;
    size_t      len;
    HANDLE      h_app;

    /* Null Check */
    if ((name == NULL) || (msgbuf == NULL)) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR " \
            "[name:%p, msgbuf:%p]", name, msgbuf);
        ret_api = RET_ERRPARAM;
    } else {
        h_app = _pb_GetAppHandle();

        len = _tcslen(name);
        if (len >= MAX_QUEUE_NAME_SIZE) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR!! " \
                "Length of thread name is too long(>=%d). [len:%zu]", MAX_QUEUE_NAME_SIZE, len);
            ret_api = RET_ERRPARAM;
        } else {
            thread_offset = PbGetLocalTid();

            MsgLockMutex();

            idx = MsgSearchNameOfCtrlTbl(name);

            if (idx != MSG_MAX_NUM_CTRL_MSGQUE) {
                h_positioningbaselibrary_service = MsgGetPositioningbaselibrarySenderHandleOfCtrlTbl(idx, thread_offset);
                if (h_positioningbaselibrary_service == NULL) {
                    h_positioningbaselibrary_service = FrameworkunifiedMcOpenSender(h_app, name);  // LCOV_EXCL_BR_LINE 4: nsfw error
                    /* When handle acquisition fails */
                    if (h_positioningbaselibrary_service == NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedMcOpenSender ERROR!! " \
                            "[h_positioningbaselibrary_service=%p, h_app=%p, name=%s]", h_positioningbaselibrary_service, h_app, name);
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        ret_api = RET_ERROR;  // LCOV_EXCL_LINE 4: nsfw error
                    } else {
                        /* Message control table update */
                        MsgSetPositioningbaselibrarySenderHandleOfCtrlTbl(idx, h_positioningbaselibrary_service, thread_offset); /* Send handle */

                        FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, "### MESSAGE TABLE INFORMATION # " \
                            "(+) idx=%d, pno=%d, h_positioningbaselibrary_sender[%d]=%p, h_sender=%p, h_receiver=%p, name=%s", \
                            idx, MsgGetPnoOfCtrlTbl(idx), PbGetLocalTid(), \
                            MsgGetPositioningbaselibrarySenderHandleOfCtrlTbl(idx, PbGetLocalTid()), MsgGetSenderHandleOfCtrlTbl(idx), \
                            MsgGetReceiverHandleOfCtrlTbl(idx), MsgGetNameOfCtrlTbl(idx));
                    }
                }
            } else {
                /* Search for free space */
                idx = MsgSearchEmptyOfCtrlTbl();
                if (idx == MSG_MAX_NUM_CTRL_MSGQUE) {  // LCOV_EXCL_BR_LINE 200: idx can not be MSG_MAX_NUM_CTRL_MSGQUE
                    /* Be impossible by design */
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "MsgSearchEmptyOfCtrlTbl ERROR!! " \
                        "[idx:%d]", idx);
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    _pb_Exit();  // LCOV_EXCL_LINE 200: idx can not be MSG_MAX_NUM_CTRL_MSGQUE

                    /* don't arrive here. */
                } else {
                    /* Get send handle */
                    h_positioningbaselibrary_service = FrameworkunifiedMcOpenSender(h_app, name);  // LCOV_EXCL_BR_LINE 4: nsfw error
                    /* When handle acquisition fails */
                    if (h_positioningbaselibrary_service == NULL) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedMcOpenSender ERROR!! " \
                            "[h_positioningbaselibrary_service=%p, h_app=%p, name=%s]", h_positioningbaselibrary_service, h_app, name);
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        ret_api = RET_ERROR;  // LCOV_EXCL_LINE 4: nsfw error
                    } else {
                        /* Message control table update */
                        MsgSetPositioningbaselibrarySenderHandleOfCtrlTbl(idx, h_positioningbaselibrary_service, thread_offset); /* Send handle */

                        MsgSetNameOfCtrlTbl(idx, name); /* Name */

                        /* Increment Message Control Table Usage Counter */
                        MsgIncUseCntOfCtrlTbl();

                        FRAMEWORKUNIFIEDLOG(ZONE_20, __FUNCTION__, \
                            "### MESSAGE TABLE INFORMATION # (++) idx=%d, pno=%d, h_positioningbaselibrary_sender[%d]=%p, " \
                            "h_sender=%p, h_receiver=%p, name=%s", idx, MsgGetPnoOfCtrlTbl(idx), \
                            PbGetLocalTid(), MsgGetPositioningbaselibrarySenderHandleOfCtrlTbl(idx, PbGetLocalTid()), \
                            MsgGetSenderHandleOfCtrlTbl(idx), MsgGetReceiverHandleOfCtrlTbl(idx), \
                            MsgGetNameOfCtrlTbl(idx));
                    }
                }
            }

            MsgUnlockMutex();
            /* When handle acquisition is successful */
            if (h_positioningbaselibrary_service != NULL) {
                /* Messaging */
                estatus = FrameworkunifiedSendMsg(h_positioningbaselibrary_service, cid, size, msgbuf);  // LCOV_EXCL_BR_LINE 4: nsfw error
                /* When transmission fails */
                if (estatus != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: nsfw error
                    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg ERROR!! " \
                        "[name=%s, estatus=%d, h_positioningbaselibrary_service=%p, cid=%d, size=%d]", \
                        name, estatus, h_positioningbaselibrary_service, cid, size);
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    ret_api = RET_ERROR;  // LCOV_EXCL_LINE 4: nsfw error
                } else {
                    FRAMEWORKUNIFIEDLOG(ZONE_17, __FUNCTION__, "Send message = " \
                        "[Destination:%s][CID:0x%X]", name, cid);
                }
                /* Close all handles at the time of termination */
            }
        }
    }

    return ret_api;
}

/*---------------------------------------------------------------------------------*
 * Local Function                                                                  *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *   PNO setting(Message control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx    Control Table Element Number
 * @param[in]    pno Process number
 */
static void MsgSetPnoOfCtrlTbl(u_int32 idx, PNO pno) {
    /* check index */
    if (idx >= MSG_MAX_NUM_CTRL_MSGQUE) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [idx:%d, pno:%d]", idx, pno);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater

        /* don't arrive here. */
    } else {
        g_msg_ctrl_tbl.info[idx].pno = pno;
    }

    return;
}

/**
 * @brief
 *   PNO acquisition(Message control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx    Control Table Element Number
 * @param[in]    pno Process number
 */
static PNO MsgGetPnoOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MSG_MAX_NUM_CTRL_MSGQUE) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater

        /* don't arrive here. */
    }

    return g_msg_ctrl_tbl.info[idx].pno;
}

/**
 * @brief
 *   Message queue name setting(Message control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx    Control Table Element Number
 * @param[in]    name Message queue name
 */
static void MsgSetNameOfCtrlTbl(u_int32 idx, LPCTSTR name) {
    /* check index */
    if (idx >= MSG_MAX_NUM_CTRL_MSGQUE) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [idx:%d, name:%s]", idx, name);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater

        /* don't arrive here. */
    }

    _tcscpy(g_msg_ctrl_tbl.info[idx].name, name);

    return;
}

/**
 * @brief
 *   Get Message Queue Name(Message control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx  Control Table Element Number
 *
 * @return Message queue name
 */
static char* MsgGetNameOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MSG_MAX_NUM_CTRL_MSGQUE) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater

        /* don't arrive here. */
    }

    return g_msg_ctrl_tbl.info[idx].name;
}

/**
 * @brief
 *   Message reception handle setting(Message control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx        Control Table Element Number
 * @param[in]    handle    Message queue handle
 */
static void MsgSetReceiverHandleOfCtrlTbl(u_int32 idx, HANDLE handle) {
    /* check index */
    if (idx >= MSG_MAX_NUM_CTRL_MSGQUE) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR " \
            "[idx:%d, handle:%p]", idx, handle);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater

        /* don't arrive here. */
    } else {
        g_msg_ctrl_tbl.info[idx].h_receiver = handle;
    }

    return;
}

/**
 * @brief
 *   Get message reception handle(Message control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx    Control Table Element Number
 *
 * @return    Message queue handle
 */
static HANDLE MsgGetReceiverHandleOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MSG_MAX_NUM_CTRL_MSGQUE) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater

        /* don't arrive here. */
    }

    return g_msg_ctrl_tbl.info[idx].h_receiver;
}

/**
 * @brief
 *   Message send handle setting(Message control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx        Control Table Element Number
 * @param[in]    handle    Message queue handle
 */
static void MsgSetSenderHandleOfCtrlTbl(u_int32 idx, HANDLE handle) {
    /* check index */
    if (idx >= MSG_MAX_NUM_CTRL_MSGQUE) {    // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR " \
            "[idx:%d, handle:%p]", idx, handle);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();    // LCOV_EXCL_LINE 6: idx cannot greater

        /* don't arrive here. */
    } else {
        g_msg_ctrl_tbl.info[idx].h_sender = handle;
    }

    return;
}

/**
 * @brief
 *   Get message send handle(Message control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx    Control Table Element Number
 *
 * @return    Message queue handle
 */
static HANDLE MsgGetSenderHandleOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MSG_MAX_NUM_CTRL_MSGQUE) {    // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater

        /* don't arrive here. */
    }

    return g_msg_ctrl_tbl.info[idx].h_sender;
}


/**
 * @brief
 *   Positioningbaselibrary message send handle setting(Message control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx        Control Table Element Number
 * @param[in]    handle    Message queue handle
 * @param[in]    offset  Local thread ID
 */
static void MsgSetPositioningbaselibrarySenderHandleOfCtrlTbl(u_int32 idx, HANDLE handle, uint32_t offset) {
    /* check index */
    if (idx >= MSG_MAX_NUM_CTRL_MSGQUE) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR " \
            "[idx:%d, handle:%p]", idx, handle);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater

        /* don't arrive here. */
    } else {
        g_msg_ctrl_tbl.info[idx].h_positioningbaselibrary_sender[offset] = handle;
    }

    return;
}

/**
 * @brief
 *   Positioningbaselibrary message send handle acquisition(Message control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx    Control Table Element Number
 * @param[in]    offset  Local thread ID
 *
 * @return    Message queue handle
 */
static HANDLE MsgGetPositioningbaselibrarySenderHandleOfCtrlTbl(u_int32 idx, uint32_t offset) {
    /* check index */
    if (idx >= MSG_MAX_NUM_CTRL_MSGQUE) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater

        /* don't arrive here. */
    }

    return g_msg_ctrl_tbl.info[idx].h_positioningbaselibrary_sender[offset];
}

/**
 * @brief
 *   Message send handle setting(Message control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx        Control Table Element Number
 * @param[in]    handle    Message queue handle
 */
static void MsgSetZcSenderHandleOfCtrlTbl(u_int32 idx, HANDLE handle) {
    /* check index */
    if (idx >= MSG_MAX_NUM_CTRL_MSGQUE) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR " \
            "[idx:%d, handle:%p]", idx, handle);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater

        /* don't arrive here. */
    } else {
        g_msg_ctrl_tbl.info[idx].h_zc_sender = handle;
    }

    return;
}

/**
 * @brief
 *   Get message send handle(Message control table)
 *
 *   If an invalid value is specified for an argument, the system assumes that it is a design problem and calls _pb_Exit().
 *
 * @param[in]    idx    Control Table Element Number
 *
 * @return    Message queue handle
 */
static HANDLE MsgGetZcSenderHandleOfCtrlTbl(u_int32 idx) {
    /* check index */
    if (idx >= MSG_MAX_NUM_CTRL_MSGQUE) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater

        /* don't arrive here. */
    }

    return g_msg_ctrl_tbl.info[idx].h_zc_sender;
}

/**
 * @brief
 *   Get message receive buffer(Message control table)
 *
 * @param[in]    idx    Control Table Element Number
 *
 * @return    Pointer to message receive buffer
 */
static uint8_t* MsgGetMsgRcvBufOfCtrlTbl(uint32_t idx) {
    /* check index */
    if (idx >= MSG_MAX_NUM_CTRL_MSGQUE) {  // LCOV_EXCL_BR_LINE 6: idx cannot greater
        /* forbidden */
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Argument ERROR [idx:%d]", idx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 6: idx cannot greater

        /* don't arrive here. */
    }

    return g_msg_ctrl_tbl.info[idx].msg_rcv_buf;
}

/**
 * @brief
 *   Search unused area(Message control table)
 *
 *   Returns the index (minimum value) for accessing the unused area in the message control table.<br>
 *   Returns the maximum message queue management value (MSG_MAX_NUM_CTRL_MSGQUE) <br>
 *   when no unused area exists
 *
 * @return    Index for access(Unused area exists.)<br>
 *                 Maximum mutex management value(No unused area)
 */
static u_int32 MsgSearchEmptyOfCtrlTbl(void) {
    int32   ret;
    u_int32    idx;

    for (idx = 0; idx < MSG_MAX_NUM_CTRL_MSGQUE; idx++) {
        ret = _tcscmp(g_msg_ctrl_tbl.info[idx].name, "");

        /* For unused space */
        if (ret == 0) {
            break;
        }
    }

    return idx;
}

/**
 * @brief
 *   PNO search(Message control table)
 *
 *   Retrieves whether the specified PNO is already registered in the message control table.<br>
 *   If it is registered, the access index is returned.If it is not stored,<br>
 *   Returns the maximum message queue management value (MSG_MAX_NUM_CTRL_MSGQUE).
 *
 * @param[in]    pno Process number
 *
 * @return    Index for access(If it is registered)<br>
 *            Maximum value of message queue management(Not stored)
 */
static u_int32 MsgSearchPnoOfCtrlTbl(PNO pno) {
    u_int32    idx;
    PNO        lPno;

    for (idx = 0; idx < MSG_MAX_NUM_CTRL_MSGQUE; idx++) {
        lPno = MsgGetPnoOfCtrlTbl(idx);

        /* If there is a match */
        if (lPno == pno) {
            break;
        }
    }

    return idx;
}


/**
 * @brief
 *   Queue name search(Message control table)
 *
 *   Retrieves whether the specified queue name is already registered in the message control table.<br>
 *   If it is registered, the access index is returned.If it is not stored,<br>
 *   Returns the maximum message queue management value (MSG_MAX_NUM_CTRL_MSGQUE).
 *
 * @param[in]    Name queue-name
 *
 * @return    Index for access(If it is registered)<br>
 *            Maximum value of message queue management(Not stored)
 */
static u_int32 MsgSearchNameOfCtrlTbl(LPCTSTR name) {
    int32   ret;
    u_int32    idx;

    for (idx = 0; idx < MSG_MAX_NUM_CTRL_MSGQUE; idx++) {
        ret = _tcscmp(g_msg_ctrl_tbl.info[idx].name, name);

        /* If there is a match */
        if (ret == 0) {
            break;
        }
    }

    return idx;
}


/**
 * @brief
 *   Creation of Mutex for accessing the message control table
 */
static void MsgCreateMutex(void) {
    g_h_mtx = _pb_CreateMutex(NULL, 0, "Msg_Mutex");
    if (g_h_mtx == NULL) {  // LCOV_EXCL_BR_LINE 200: can not be not NULL
        // LCOV_EXCL_START 200: can not be not NULL
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_CreateMutex ERROR [g_h_mtx:%p]", g_h_mtx);
        _pb_Exit();
        // LCOV_EXCL_STOP

        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Acquire Mutex for accessing the message control table
 */
static void MsgLockMutex(void) {
    DWORD    ret;
    ret = PbMutexLock(g_h_mtx, INFINITE);  // LCOV_EXCL_BR_LINE 200: mutex lock can not failed
    if (ret != WAIT_OBJECT_0) {  // LCOV_EXCL_BR_LINE 200: mutex lock can not failed
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PbMutexLock ERROR " \
            "[ret:%lu, g_h_mtx:%p]", ret, g_h_mtx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 200: mutex lock can not failed

        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Releasing the Mutex for accessing the message control table
 */
static void MsgUnlockMutex(void) {
    BOOL    ret;
    ret = PbMutexUnlock(g_h_mtx);  // LCOV_EXCL_BR_LINE 200: mutex lock can not failed
    if (ret != TRUE) {  // LCOV_EXCL_BR_LINE 200: mutex lock can not failed
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "PbMutexUnlock ERROR " \
            "[ret:%d, g_h_mtx:%p]", ret, g_h_mtx);
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        _pb_Exit();  // LCOV_EXCL_LINE 200: mutex lock can not failed

        /* don't arrive here. */
    }

    return;
}

/**
 * @brief
 *   Obtain dump information
 *
 * @param[out]     pBuf    Dump info
 * @param[in/out]  pLen    Buffer size
 */
void _pb_GetDebugMsgMngTbl(void* pBuf, uint8_t* pLen) {
    static uint8_t  buf[DEBUG_DUMP_MAX_SIZE];
    static uint8_t  bufPositioningbaselibrarySender[DEBUG_DUMP_MAX_SIZE];
    static uint8_t  bufTmp[DEBUG_DUMP_MAX_SIZE];
    static uint8_t  bufMsg[DEBUG_DUMP_MAX_SIZE];
    uint32_t         i;
    uint32_t         e;
    uint8_t          cnt = 0;

    if ((pBuf != NULL) && (pLen != NULL)) {
        memset(&buf[0], 0x00, sizeof(buf));
        memset(&bufMsg[0], 0x00, sizeof(bufMsg));
        for (i = 0; i < MSG_MAX_NUM_CTRL_MSGQUE; i++) {
            memset(&bufPositioningbaselibrarySender[0], 0x00, sizeof(bufPositioningbaselibrarySender));
            for (e = 0; e < MSG_MAX_NUM_CTRL_THREAD; e++) {
                memset(&bufTmp[0], 0x00, sizeof(bufTmp));
                snprintf(reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp),
                        "[%02d]%10p ", e, g_msg_ctrl_tbl.info[i].h_positioningbaselibrary_sender[e]);
                strncat(reinterpret_cast<char *>(&bufPositioningbaselibrarySender[0]), reinterpret_cast<char *>(&bufTmp[0]), \
                    strlen(reinterpret_cast<char *>(&bufTmp[0])));
            }
            memset(&bufTmp[0], 0x00, sizeof(bufTmp));
            snprintf(reinterpret_cast<char *>(&bufTmp[0]), sizeof(bufTmp),
                    "\n [%02d] pno:0x%04x, name:%16s, hSnd:%10p, hRcv:%10p, hPSnd:%s",
                    i,
                    g_msg_ctrl_tbl.info[i].pno,
                    g_msg_ctrl_tbl.info[i].name,
                    g_msg_ctrl_tbl.info[i].h_sender,
                    g_msg_ctrl_tbl.info[i].h_receiver,
                    &bufPositioningbaselibrarySender[0]);
            strncat(reinterpret_cast<char *>(&bufMsg[0]), reinterpret_cast<char *>(&bufTmp[0]), \
                strlen(reinterpret_cast<char *>(&bufTmp[0])));
            if (((i+1) % 8) == 0) {
                cnt++;
                memset(&buf[0], 0x00, sizeof(buf));
                snprintf(reinterpret_cast<char *>(&buf[0]), sizeof(buf),
                        "Message-%d%s",
                        cnt,
                        &bufMsg[0]);
                memcpy(pBuf, &buf[0], sizeof(buf));
                pBuf = reinterpret_cast<void *>((reinterpret_cast<uint8_t *>(pBuf)) + sizeof(buf));
                memset(&bufMsg[0], 0x00, sizeof(bufMsg));
                if (cnt >= *pLen) {
                    break;
                }
            }
        }
        if (cnt < *pLen) {
            if (bufMsg[0] != 0x00) {
                cnt++;
                memset(&buf[0], 0x00, sizeof(buf));
                snprintf(reinterpret_cast<char *>(&buf[0]), sizeof(buf),
                        "Message-%d%s",
                        cnt,
                        &bufMsg[0]);
                memcpy(pBuf, &buf[0], sizeof(buf));
            }
            *pLen = cnt;
        }
    }
}

/**
 * @brief
 *   Message Control Table Usage Counter Increment
 *
 * @param[in] none
 */
static void MsgIncUseCntOfCtrlTbl(void) {
    g_msg_ctrl_tbl.use_cnt++;
    return;
}

/**
 * @brief
 *   Message control table usage counter decrement
 *
 * @param[in] none
 */
static void MsgDecUseCntOfCtrlTbl(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    g_msg_ctrl_tbl.use_cnt--;
    return;
}
// LCOV_EXCL_STOP

/**
 * @brief
 *   Message Control Table Reserved Counter Increment
 *
 * @param[in] none
 */
static void MsgIncRsvCntOfCtrlTbl(void) {
    g_msg_ctrl_tbl.rsv_cnt++;
    return;
}

/**
 * @brief
 *   Message Control Table Reserved Counter Decrement
 *
 * @param[in] none
 */
static void MsgDecRsvCntOfCtrlTbl(void) {
    g_msg_ctrl_tbl.rsv_cnt--;
    return;
}

/**
 * @brief
 *   Resource Acquisition Decision(Message control table)
 *
 * @param[in]  none
 *
 * @return BOOL
 * @retval TRUE  : Normal
 * @retval FALSE : Anomaly (Resource shortage)
 */
BOOL _pb_GetMsgResource(void) {
    BOOL ret = TRUE;
    uint32_t cnt;

    MsgLockMutex();

    /* Increment Message Control Table Reservation Counter */
    MsgIncRsvCntOfCtrlTbl();

    cnt = g_msg_ctrl_tbl.use_cnt + g_msg_ctrl_tbl.rsv_cnt;
    if (cnt >= FULL_MSG_NUM_CTRL_MSGQUE) {
        ret = FALSE;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Lack of resources " \
            "[FATAL][use_cnt:%d rsv_cnt:%d]", g_msg_ctrl_tbl.use_cnt, g_msg_ctrl_tbl.rsv_cnt);
    } else if (cnt >= WARN_MSG_NUM_CTRL_MSGQUE) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Lack of resources " \
            "[WARN][use_cnt:%d rsv_cnt:%d]", g_msg_ctrl_tbl.use_cnt, g_msg_ctrl_tbl.rsv_cnt);
    }

    MsgUnlockMutex();

    return ret;
}

/**
 * @brief
 *   Resource release(Message control table)
 *
 * @param[in]  none
 *
 * @return none
 */
void _pb_ReleaseMsgResource(void) {
    MsgLockMutex();

    /* Decrement Message Control Table Reservation Counter */
    MsgDecRsvCntOfCtrlTbl();

    MsgUnlockMutex();

    return;
}
