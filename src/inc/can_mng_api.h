/*
 * @copyright Copyright (c) 2020 TOYOTA MOTOR CORPORATION.
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

#ifndef __CAN_MNG_API_H__
#define __CAN_MNG_API_H__

/** @includes
 *
 */

/** @define
 *
 */
#ifndef UINT8
#define UINT8    unsigned char
#endif
#ifndef UINT16
#define UINT16   unsigned short
#endif
#ifndef INT32
#define INT32    int
#endif
#ifndef UINT32
#define UINT32   unsigned int
#endif

/*------------------------------------------*/
/*  Log                                     */
/*------------------------------------------*/
#if defined(CAN_MNG_API_FORCE_DEBUG)
  #define CAN_MNG_API_LOGT(fmt,...) fprintf(stderr, "[T][CAN_API] %s(%s)(%d):" fmt "\n", __FILE__, __func__, __LINE__, ## __VA_ARGS__)
  #define CAN_MNG_API_LOGD(fmt,...) fprintf(stderr, "[D][CAN_API] %s(%s)(%d):" fmt "\n", __FILE__, __func__, __LINE__, ## __VA_ARGS__)
  #define CAN_MNG_API_LOGE(fmt,...) fprintf(stderr, "[E][CAN_API] %s(%s)(%d):" fmt "\n", __FILE__, __func__, __LINE__, ## __VA_ARGS__)
#else
  #define CAN_MNG_API_LOGT(fmt,...)
  #define CAN_MNG_API_LOGD(fmt,...)
  #define CAN_MNG_API_LOGE(fmt,...)
#endif
/*------------------------------------------*/
/* API return value                         */
/*------------------------------------------*/
#define CAN_CTL_RET_SUCCESS     0  /* Normal end             */
#define CAN_CTL_RET_ERR_PARAM  -1  /* Parameter error        */
#define CAN_CTL_RET_ERR_NOSVC  -2  /* Service not open error */
#define CAN_CTL_RET_ERR_ERR    -3  /* Other error            */

/*------------------------------------------*/
/* Received command information             */
/*------------------------------------------*/
#define CAN_CTL_CMD_LEN_MAX        255       /* Maximum data length                      */
#define CAN_DAT_LEN_MAX CAN_CTL_CMD_LEN_MAX  /* Maximum data length                      */
#define CAN_CTL_CMD_ID_HI_NUM      128       /* Number of higher CAN-ID(8bit) (00 to 7F) */

#define CAN_CTL_MAX_RCV_CAN_SIZE   23

/** @typedefs
 *
 */
/* Object for CAN communication control API */
typedef struct _CanCtlApiObj {
  /* ID assigned to the command reception notification callback */
  INT32 call_id;
} CanCtlApiObj ;

/* CAN command structure */
typedef struct _CanCtlApiCmd { 
  UINT8 len;                    /* Data length */
  UINT8 data[CAN_DAT_LEN_MAX];  /* Data        */
} CanCtlApiCmd;

/* Received ID MAP information */
typedef struct _CanCtlRcvId {
  UINT16  id[CAN_CTL_CMD_ID_HI_NUM];  /* CAN ID MAP(000~7FF) */
} CanCtlRcvId;


/* CAN Frame structure */
typedef union {
  UINT8   Data[2];
} CanId;

#define LCN_SRV_MSB_FRAME_DAT_SIZE  (8)         /* MSB send/receive frame data size */

typedef struct _CanData {
  CanId   id;                                 /* CAN Frame structure */
  UINT8   dlc;                                /* Valid bytes of DATA#1~#8. DLC is 1 to 8. */
  UINT8   Data[LCN_SRV_MSB_FRAME_DAT_SIZE];   /* Receive data */
                                              /* Data of DLC size is valid, otherwise set to 0 as invalid value. */
} CanData;

/** @forward declarations
 *
 */
/* API prototype */
#ifdef __cplusplus
extern "C" {
#endif
UINT32 CanCtlApiOpen(CanCtlApiObj* pClientObj);
UINT32 CanCtlApiClose(CanCtlApiObj* pClientObj);
UINT32 CanCtlApiSndCmd(CanCtlApiObj* pClientObj, CanCtlApiCmd* pSndCmd);
UINT32 CanCtlApiSetRcvId(CanCtlApiObj* pClientObj, CanCtlRcvId* pRcvId);
UINT32 CanCtlApiRcvCmd(CanCtlApiObj* pClientObj, CanCtlApiCmd* pRcvCmd);
UINT32 CanCtlApiRcvSpd(CanCtlApiObj* pClientObj, CanCtlApiCmd* pRcvCmd);
#ifdef __cplusplus
}
#endif
#endif /* __CAN_MNG_API_H__ */
