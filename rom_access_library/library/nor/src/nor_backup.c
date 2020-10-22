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

/*******************************************************************************
* $Header::                                                                   $
* $Revision::                                                                 $
*******************************************************************************/

/*******************************************************************************
@file        mtd_backup.c
@system
@process
@detail
 ******************************************************************************/
#include "system_service/nor_backup.h"
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <asm/unistd.h>
#include <mtd/mtd-user.h>

#define BACKUP_RETRY_CNT    3

/* Invalid Value */
#define FLSBACKUP_SEMAPHORE_INVALID    (0)


#define BACKUP_SECTOR_SIZ        1024                    // Secter Size

#define BACKUP_CHECK_OFFSET    16
#define BACKUP_CHECK_SIZE        12
#define BACKUP_CHECK_TEXT        "MTD_BACKUPDT"
#define BACKUP_CHECK_DAT1        1    // Lower 8bit
#define BACKUP_CHECK_DAT2        2    // Upper 8bit

#define NOR_ERRLOG(fmt, args...) \
  fprintf(stderr, "[ERR]%ld/%s/%d/= "fmt"\n", syscall(__NR_gettid), __func__, __LINE__, ##args)

static pthread_mutex_t    g_semid_flash = PTHREAD_MUTEX_INITIALIZER;       /* Semaphore ID of dram area access    */
static int    g_backup_init = FLSBACKUP_SEMAPHORE_INVALID;

typedef enum {
  RET_WT_CHK_OK = 0,
  RET_WT_ERR_OTHER,
  RET_WT_DEV_ERR,
} NOR_WRITE_BLK_CHK;

typedef enum {
  RET_RD_CHK_OK = 0,
  RET_RD_ERR_OTHER,
  RET_RD_DEV_ERR,
  RET_RD_ERR_1 = -1,
  RET_RD_ERR_2 = -2,
  RET_RD_ERR_3 = -3,
  RET_RD_ERR_4 = -4,
} NOR_READ_BLK_CHK;

typedef enum {
  RET_CHK_OK = 0,
  RET_CHK_TEXT_ERR,
  RET_CHK_BCC_ERR,
} NOR_BLK_CHK;

typedef enum {
  NOR_IDLE = 0,        // 0:Read-out of block data
  NOR_ORG_TEXT,        // 1:OrgText Error
  NOR_ORG_BCC,        // 2:OrgBcc error
  NOR_READ_OK,        // 3:Normal read-out
  NOR_READ_END        // 4:end state
} NOR_R_STATE;

typedef enum {
  NOR_IDEL = 0,            // 0:IDEL
  NOR_READ_ERR,            // 1:Read Error
  NOR_ORG_READ,            // 2:Org Read OK
  NOR_BAK_WRT,            // 3:Bak Write OK
  NOR_BACK_READ,            // 4:Bak Read OK
  NOR_ORG_WRITE,            // 5:Org Write OK
  NOR_WRITE_END            // 6:end state
} NOR_W_STATE;

NOR_READ_BLK_CHK MtdnBackupReadSub(
    const char * path_name, int i_id, int i_offset, int i_size,
    void *p_buff, mtd_info_t * mtd_info, int * get_state);
NOR_WRITE_BLK_CHK MtdnBackupWriteSub(
    const char * path_name, int i_id, int i_offset, int i_size,
    void *p_buff, mtd_info_t * mtd_info);

int MtdnBackupWriteBase(
    const char * path_name, int i_id, int i_offset, int i_size,
    void *p_buff, mtd_info_t * mtd_info);
int MtdnBackupReadBase(
    const char * path_name, int i_id, int i_offset, int i_size,
    void *p_buff, mtd_info_t * mtd_info);

int MtdBackupInfo(const char * path_name, mtd_info_t * mtd_info);        // INFO Get

NOR_BLK_CHK    MtdBackupCheksumRead(char * p_buff, int erase_size, int * ret_sum);            // check SUM
int    MtdBackupCheksumWrite(char * p_buff, int erase_size);            // check SUM

/***************************************************************************
@brief        mtdn_backup_Read
@outline        Read-out of specified block ID
@type        Completion return type
@param[in]    const char * path_name    : (/dev/mtd  Device)
@param[in]    int    i_id        : ID...0~
@param[in]    int    i_offset    :  ( i_offset >= 0)
@param[in]    int    i_size    :  ( i_size > 0)
@param[in]    void *p_buff    : (p_buff != NULL)
@return     int
@retval        0        : No error
@retval        Except    : Error
@usage        It is Backup API of data read.
*****************************************************************************/
int    mtdn_backup_Read(const char * path_name, int i_id, int i_offset, int i_size, void *p_buff ) {
    int    ret_status = RET_DEV_NORMAL;

    if (path_name == NULL) {
        NOR_ERRLOG("invalid path:%p", path_name);
        return  RET_DEV_ERR_PARAM;
    }
    if (p_buff == NULL) {
        NOR_ERRLOG("invalid buf:%p", p_buff);
        return  RET_DEV_ERR_PARAM;
    }
    if (i_id < 0) {
        NOR_ERRLOG("invalid id:%d", i_id);
        return  RET_DEV_ERR_PARAM;
    }
    if ((i_offset < 0) || (i_size <= 0)) {
        NOR_ERRLOG("invalid size:%d offset:%d", i_size, i_offset);
        return  RET_DEV_ERR_PARAM;
    }
    if (g_backup_init == FLSBACKUP_SEMAPHORE_INVALID) {  // LCOV_EXCL_BR_LINE 200: g_backup_init is static variable
        pthread_mutex_init(&g_semid_flash, NULL);
        g_backup_init = ~FLSBACKUP_SEMAPHORE_INVALID;
    }
    mtd_info_t mtd_info;
    memset(&mtd_info, 0, sizeof(mtd_info_t));
    ret_status = MtdBackupInfo((const char *)path_name, &mtd_info);
    if (ret_status == RET_DEV_NORMAL) {
        if (mtd_info.type != MTD_NORFLASH) {
            NOR_ERRLOG("invalid type:%d", mtd_info.type);
            ret_status = RET_DEV_ERR_PARAM;
        } else {
            if ((i_offset + i_size) > (int)(mtd_info.erasesize - BACKUP_CHECK_OFFSET)) {
                NOR_ERRLOG("invalid size:%d offset:%d erasesize:%d", i_size, i_offset, mtd_info.erasesize);
                ret_status = RET_DEV_ERR_PARAM;
            }
        }
        if (ret_status == RET_DEV_NORMAL) {
            /* Start Semaphore(flash)    */
            pthread_mutex_lock(&g_semid_flash);
            int get_state;    // Read status
            ret_status = (int)MtdnBackupReadSub(path_name, i_id, i_offset, i_size,
                                                             p_buff, &mtd_info, &get_state);

            /* End Semaphore(flash)    */
            pthread_mutex_unlock(&g_semid_flash);
        }
    }
    if (ret_status != RET_DEV_NORMAL) {
        NOR_ERRLOG("error return:%d", ret_status);
    }
    return    ret_status;
}

/***************************************************************************
@brief        mtdn_backup_Write
@outline        Writing of specified block ID
@type        Completion return type
@param[in]    const char * path_name    : (/dev/mtd  Device)
@param[in]    int    i_id        : ID...0~
@param[in]    int    i_offset    :  ( i_offset >= 0)
@param[in]    int    i_size    :  ( i_size > 0)
@param[in]    void *p_buff    : (p_buff != NULL)
@return     int
@retval        0    : No error
@retval        Except: Error
@usage        It is Backup API of data read.
*****************************************************************************/
int    mtdn_backup_Write(const char * path_name, int i_id, int i_offset, int i_size, void *p_buff) {
    int    ret_status = RET_DEV_NORMAL;

    if (path_name == NULL) {
        NOR_ERRLOG("invalid path:%p", path_name);
        return  RET_DEV_ERR_PARAM;
    }
    if (p_buff == NULL) {
        NOR_ERRLOG("invalid buf:%p", p_buff);
        return RET_DEV_ERR_PARAM;
    }
    if (i_id < 0) {
        NOR_ERRLOG("invalid id:%d", i_id);
        return RET_DEV_ERR_PARAM;
    }
    if ((i_offset < 0) || (i_size <= 0)) {
        NOR_ERRLOG("invalid size:%d offset:%d", i_size, i_offset);
        return RET_DEV_ERR_PARAM;
    }
    if (g_backup_init == FLSBACKUP_SEMAPHORE_INVALID) {
        pthread_mutex_init(&g_semid_flash, NULL);
        g_backup_init = ~FLSBACKUP_SEMAPHORE_INVALID;
    }
    mtd_info_t mtd_info;
    memset(&mtd_info, 0, sizeof(mtd_info_t));
    ret_status = MtdBackupInfo((const char *)path_name, &mtd_info);
    if (ret_status == RET_DEV_NORMAL) {
        if (mtd_info.type != MTD_NORFLASH) {
            NOR_ERRLOG("invalid type:%d", mtd_info.type);
            ret_status = RET_DEV_ERR_PARAM;
        } else if (mtd_info.erasesize == 0) {
            NOR_ERRLOG("invalid erasesize:%d", mtd_info.erasesize);
            ret_status = RET_DEV_ERR_PARAM;
        } else {
            if ((i_offset + i_size) > (int)(mtd_info.erasesize - BACKUP_CHECK_OFFSET)) {
                NOR_ERRLOG("invalid size:%d offset:%d erasesize:%d", i_size, i_offset, mtd_info.erasesize);
                ret_status = RET_DEV_ERR_PARAM;
            }
        }
        if (ret_status == RET_DEV_NORMAL) {
            /* Start Semaphore(flash)    */
            pthread_mutex_lock(&g_semid_flash);
            ret_status = (int)MtdnBackupWriteSub(path_name, i_id, i_offset, i_size, p_buff, &mtd_info);

            /* End Semaphore(flash)    */
            pthread_mutex_unlock(&g_semid_flash);
        }
    }
    if (ret_status != RET_DEV_NORMAL) {
        NOR_ERRLOG("error return:%d", ret_status);
    }
    return    ret_status;
}

/***************************************************************************
@brief        MtdnBackupReadSub
@outline        Read-out of specified block ID
@type        Completion return type
@param[in]    const char * path_name    : (/dev/mtd  Device)
@param[in]    int    i_id        : ID...0~
@param[in]    int    i_offset    :  ( i_offset >= 0)
@param[in]    int    i_size    :  ( i_size > 0)
@param[in]    void *p_buff    : (p_buff != NULL)
@param[in]    mtd_info_t * mtd_info    : info
@param[in]    int * get_state    : 0:Rrg Normal 1:Bak Normal
@return     NOR_READ_BLK_CHK
@retval        RET_RD_CHK_OK    : No error
@retval        Except        : Error
@usage        It is Backup API of data read.
*****************************************************************************/
NOR_READ_BLK_CHK    MtdnBackupReadSub(const char * path_name, int i_id, int i_offset, int i_size,
                                      void *p_buff, mtd_info_t * mtd_info, int * get_state) {
    NOR_READ_BLK_CHK ret_status = RET_RD_CHK_OK;
    char *    lp_buffer;

    if (mtd_info->erasesize != 0) {  // LCOV_EXCL_BR_LINE 6: double check
        lp_buffer = (char *)malloc(mtd_info->erasesize);
        if (lp_buffer == NULL) {
            NOR_ERRLOG("malloc:%p", lp_buffer);
            ret_status = RET_RD_DEV_ERR;        // error
        }
    } else {
        // LCOV_EXCL_START 8: dead code
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        NOR_ERRLOG("invalid erasesize:%d", mtd_info->erasesize);
        ret_status = RET_RD_DEV_ERR;        // error
        // LCOV_EXCL_STOP
    }
    if (ret_status == RET_RD_CHK_OK) {
        memset(lp_buffer, 0x00, mtd_info->erasesize);
        NOR_R_STATE        cycle_state = NOR_IDLE;
        int        block_check_mode = 0;
        int ret_sum = 0;
        while ((cycle_state < NOR_READ_END)
              &&(ret_status == RET_RD_CHK_OK)) {
            switch (cycle_state) {
            case NOR_IDLE:
                // Read-out of block data
                if (0 != MtdnBackupReadBase((const char *)path_name, i_id, 0,
                                            (int)mtd_info->erasesize, lp_buffer, mtd_info)) {
                    ret_status = RET_RD_ERR_OTHER;        // Other abnormalities
                } else {
                    NOR_BLK_CHK ret = MtdBackupCheksumRead(lp_buffer, (int)mtd_info->erasesize, (int *)&ret_sum);
                    if (ret == RET_CHK_TEXT_ERR) {            // Text character sequence   Abnormalities
                        cycle_state = NOR_ORG_TEXT;
                    } else if (ret == RET_CHK_BCC_ERR) {    // (BCC)error
                        cycle_state = NOR_ORG_BCC;
                    } else {
                        block_check_mode = 0;        // Org Normal
                        cycle_state = NOR_READ_OK;
                    }
                }
                break;
            case NOR_ORG_TEXT:    // OrgText Error
                // Read-out of block data
                if (0 != MtdnBackupReadBase((const char *)path_name, (i_id+1), 0,
                                            (int)mtd_info->erasesize, lp_buffer, mtd_info)) {
                    // other error
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    ret_status = RET_RD_ERR_OTHER;    // LCOV_EXCL_LINE 5: c API error case.
                } else {
                    NOR_BLK_CHK ret = MtdBackupCheksumRead(lp_buffer, (int)mtd_info->erasesize,
                                                           (int *)&ret_sum);      // check SUM
                    if (ret == RET_CHK_TEXT_ERR) {            // Text character sequence   Abnormalities
                        ret_status = RET_RD_ERR_1;        // Uninitialized.
                    } else if (ret == RET_CHK_BCC_ERR) {    // BCC error
                        ret_status = RET_RD_ERR_2;        // write error
                    }
                    block_check_mode = 1;        // Bak Normal
                    cycle_state = NOR_READ_OK;
                }
                break;
            case NOR_ORG_BCC:    // OrgBcc error
                // Read-out of block data
                if (0 != MtdnBackupReadBase((const char *)path_name, (i_id+1), 0,
                                            (int)mtd_info->erasesize, lp_buffer, mtd_info)) {
                    // other error
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    ret_status = RET_RD_ERR_OTHER;    // LCOV_EXCL_LINE 5: c API error case.
                } else {
                    NOR_BLK_CHK ret = MtdBackupCheksumRead(lp_buffer, (int)mtd_info->erasesize,
                                                           (int *)&ret_sum);      // check SUM
                    if (ret == RET_CHK_TEXT_ERR) {            // Text character sequence   Abnormalities
                        ret_status = RET_RD_ERR_3;        // write error
                    } else if (ret == RET_CHK_BCC_ERR) {    // BCC error
                        ret_status = RET_RD_ERR_4;        // write error
                    }
                    block_check_mode = 1;        // Bak Normal
                    cycle_state = NOR_READ_OK;
                }
                break;
            case NOR_READ_OK:    // Normal read-out
                // The pickup of the read data
                memcpy(p_buff, &lp_buffer[i_offset], (size_t)i_size);
                cycle_state = NOR_READ_END;
                break;
            default:
                // LCOV_EXCL_START 8: dead code
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                NOR_ERRLOG("invalid state:%d", cycle_state);
                // other error
                ret_status = RET_RD_ERR_OTHER;
                break;
                // LCOV_EXCL_STOP
            }
        }
        *get_state = block_check_mode;    // 0:OrgNormal 1:BakNormal
        free(lp_buffer);
    }
    return    ret_status;        // Normal
}

static inline NOR_WRITE_BLK_CHK MtdnBackupWriteRetryCount(int *count, NOR_WRITE_BLK_CHK cur_state) {
    NOR_WRITE_BLK_CHK ret_state = cur_state;
    (*count)++;
    if (*count > BACKUP_RETRY_CNT) {
        NOR_ERRLOG("retry over!! state:%d count:%d", ret_state, *count);
        ret_state = RET_WT_DEV_ERR;
    }
    return ret_state;
}

/***************************************************************************
@brief        MtdnBackupWriteSub
@outline        Writing of specified block ID
@type        Completion return type
@param[in]    const char * path_name    : (/dev/mtd  Device)
@param[in]    int    i_id        : ID...0~
@param[in]    int    i_offset    :  ( i_offset >= 0)
@param[in]    int    i_size    :  ( i_size > 0)
@param[in]    void *p_buff    : (p_buff != NULL)
@param[in]    mtd_info_t * mtd_info    : info
@return     NOR_WRITE_BLK_CHK
@retval        RET_WT_CHK_OK    : No error
@retval        Except    : Error
@usage        It is Backup API of data read.
*****************************************************************************/
NOR_WRITE_BLK_CHK    MtdnBackupWriteSub(const char * path_name, int i_id, int i_offset,
                                        int i_size, void *p_buff, mtd_info_t * mtd_info) {
    NOR_WRITE_BLK_CHK ret_status = RET_WT_CHK_OK;
    char*    lp_buffer;

    lp_buffer = (char *)malloc(mtd_info->erasesize);
    if (lp_buffer == NULL) {
        NOR_ERRLOG("malloc:%p", lp_buffer);
        ret_status = RET_WT_DEV_ERR;        // error
    } else {
        memset(lp_buffer, 0x00, mtd_info->erasesize);
        NOR_W_STATE    cycle_state = NOR_IDEL;
        int    retry_cnt = 0;
        int get_state = 0;    // Read status

        while ((cycle_state < NOR_WRITE_END)
              &&(ret_status == RET_WT_CHK_OK)) {
            switch (cycle_state) {
            case NOR_IDEL:    // IDLE
                if (RET_RD_CHK_OK != MtdnBackupReadSub((const char *)path_name, i_id, 0,
                                                       (int)mtd_info->erasesize, lp_buffer, mtd_info, &get_state)) {
                    cycle_state = NOR_READ_ERR;    // read error
                } else {
                    // write data set
                    memcpy(&lp_buffer[i_offset], p_buff, (size_t)i_size);
                    if (get_state == 0) {
                        cycle_state = NOR_ORG_READ;    // Org read Normal
                    } else {
                        // Bakread Normal
                        cycle_state = NOR_BACK_READ;
                    }
                    // Embedding of write-in data
                    memcpy(&lp_buffer[i_offset], p_buff, (size_t)i_size);
                }
                break;
            case NOR_READ_ERR:    // read error
                memset((char *)lp_buffer, 0x00, mtd_info->erasesize);
                // Embedding of write-in data
                memcpy(&lp_buffer[i_offset], p_buff, (size_t)i_size);
                cycle_state = NOR_ORG_READ;    // Org read Normal
                break;
            case NOR_ORG_READ:    // Orgread Normal
                // LCOV_EXCL_BR_START 6: double check
                if (0 != MtdBackupCheksumWrite(lp_buffer, (int)mtd_info->erasesize )) {            // check SUM
                // LCOV_EXCL_BR_STOP
                    // error(It does not generate.)
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    ret_status = RET_WT_DEV_ERR;    // LCOV_EXCL_LINE 8: dead code
                } else {
                    // data write
                    if (0 == MtdnBackupWriteBase((const char *)path_name, (i_id+1), 0,
                                                 (int)mtd_info->erasesize, lp_buffer, mtd_info)) {
                        cycle_state = NOR_BAK_WRT;    // Bakwrite Success
                        retry_cnt = 0;
                    } else {
                        ret_status = MtdnBackupWriteRetryCount(&retry_cnt, ret_status);
                    }
                }
                break;
            case NOR_BAK_WRT:    // BakWriteSuccess
                // Data write
                // LCOV_EXCL_BR_START 11:unexpected branch
                if (0 == MtdnBackupWriteBase((const char *)path_name, i_id, 0,
                                             (int)mtd_info->erasesize, lp_buffer, mtd_info)) {
                // LCOV_EXCL_BR_STOP
                    cycle_state = NOR_WRITE_END;    // Normal end
                } else {
                    // LCOV_EXCL_START 5: c API error case.
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    ret_status = MtdnBackupWriteRetryCount(&retry_cnt, ret_status);
                    // LCOV_EXCL_STOP
                }
                break;
            case NOR_BACK_READ:    // Bak read normal
                // LCOV_EXCL_BR_START 6: double check
                if (0 != MtdBackupCheksumWrite(lp_buffer, (int)mtd_info->erasesize )) {            // check SUM
                // LCOV_EXCL_BR_STOP
                    // Retry over
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    ret_status = RET_WT_DEV_ERR;    // LCOV_EXCL_LINE 8: dead code
                } else {
                    // data write
                    if (0 == MtdnBackupWriteBase((const char *)path_name, i_id, 0,
                                                 (int)mtd_info->erasesize, lp_buffer, mtd_info)) {
                        // Org write Succses
                        cycle_state = NOR_ORG_WRITE;
                        retry_cnt = 0;
                    } else {
                        // LCOV_EXCL_START 5: c API error case.
                        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                        ret_status = MtdnBackupWriteRetryCount(&retry_cnt, ret_status);
                        // LCOV_EXCL_STOP
                    }
                }
                break;
            case NOR_ORG_WRITE:    // Org write Succses
                // data write
                if (0 == MtdnBackupWriteBase((const char *)path_name, (i_id+1), 0,
                                             (int)mtd_info->erasesize, lp_buffer, mtd_info)) {
                    // Normal end
                    cycle_state = NOR_WRITE_END;
                } else {
                    // LCOV_EXCL_START 5: c API error case.
                    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    ret_status = MtdnBackupWriteRetryCount(&retry_cnt, ret_status);
                    // LCOV_EXCL_STOP
                }
                break;
            default:
                // LCOV_EXCL_START 8: dead code
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                NOR_ERRLOG("invalid state:%d", cycle_state);
                // other error
                ret_status = RET_WT_ERR_OTHER;
                break;
                // LCOV_EXCL_STOP
            }
        }
        free(lp_buffer);
    }
    return    ret_status;        // Normal
}

/***************************************************************************
@brief        MtdnBackupReadBase
@outline        Read-out of specified block ID
@type        Completion return type
@param[in]    const char * path_name    : (/dev/mtd  Device)
@param[in]    int    i_id        : ID...0~
@param[in]    int    i_offset    :  ( i_offset >= 0)
@param[in]    int    i_size    :  ( i_size > 0)
@param[in]    void *p_buff    : (p_buff != NULL)
@param[in]    mtd_info_t * mtd_info    :
@return     int
@retval        0    : No error
@retval        Except    : Error
@usage        It is Backup API of data read.
*****************************************************************************/
int    MtdnBackupReadBase(const char * path_name, int i_id, int i_offset,
                          int i_size, void *p_buff, mtd_info_t * mtd_info) {
    int ret_status = RET_DEV_NORMAL;

    int mtd_fd = open(path_name, O_RDONLY|O_CLOEXEC);
    // LCOV_EXCL_BR_START 5:It's impossible to mock open() function,so this line can not be passed.
    if (mtd_fd == -1) {
    // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 5:It's impossible to mock open() function,so this line can not be passed.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        NOR_ERRLOG("open(%s):%s", path_name, strerror(errno));
        ret_status = RET_DEV_ERROR;
    // LCOV_EXCL_STOP
    } else {
        int seek_top = i_id * (int)mtd_info->erasesize + i_offset;
        int sector_size = BACKUP_SECTOR_SIZ;
        int block_size = (int)mtd_info->erasesize / sector_size;    // Division number
        int llpi;
        int    buff_top = 0;
        int    get_size = i_size;
        char * work_buff = (char *)p_buff;
        ssize_t        read_size;

        // It reads several sector minutes.
        for (llpi=0; llpi < block_size; llpi++) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
            // It moves to a head.
            if (-1 == (off_t)lseek(mtd_fd, seek_top, SEEK_SET)) {
                NOR_ERRLOG("lseek():%s", strerror(errno));
                ret_status = RET_DEV_ERROR;
                break;
            }
            if (get_size >= sector_size) {  // LCOV_EXCL_BR_LINE 6:double check
                read_size = read(mtd_fd, &work_buff[buff_top], (size_t)sector_size);
                if (read_size < 0) {
                    NOR_ERRLOG("read():%zd %s", read_size, strerror(errno));
                    ret_status = RET_DEV_ERROR;
                    break;
                }

                get_size -= sector_size;
                if (get_size <= 0) {
                    break;
                }
            } else {
            // LCOV_EXCL_START 8: dead code
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                read_size = read(mtd_fd, &work_buff[buff_top], (size_t)get_size);
                if (read_size < 0) {
                    NOR_ERRLOG("read():%zd %s", read_size, strerror(errno));
                    ret_status = RET_DEV_ERROR;
                    break;
                }
                break;
            // LCOV_EXCL_STOP
            }
            seek_top += (int)read_size;
            buff_top += (int)read_size;
        }
        // LCOV_EXCL_BR_START 5:It's impossible to mock close() function,so this line can not be passed.
        if (0 != close(mtd_fd)) {
        // LCOV_EXCL_BR_STOP
            // LCOV_EXCL_START 5:It's impossible to mock close() function,so this line can not be passed.
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            NOR_ERRLOG("close():%s", strerror(errno));
            ret_status = RET_DEV_ERROR;
            // LCOV_EXCL_STOP
        }
    }
    return    ret_status;
}

/***************************************************************************
@brief        MtdnBackupWriteBase
@outline        Writing of specified block ID
@type        Completion return type
@param[in]    const char * path_name    : (/dev/mtd  Device)
@param[in]    int    i_id        : ID...0~
@param[in]    int    i_offset    :  ( i_offset >= 0)
@param[in]    int    i_size    :  ( i_size > 0)
@param[in]    void *p_buff    : (p_buff != NULL)
@param[in]    mtd_info_t * mtd_info    :
@return     int
@retval        0    : No error
@retval        Except    : Error
@usage        It is Backup API of data read.
*****************************************************************************/
int    MtdnBackupWriteBase(const char * path_name, int i_id, int i_offset,
                           int i_size, void *p_buff, mtd_info_t * mtd_info) {
    int ret_status = RET_DEV_NORMAL;

    int mtd_fd = open(path_name, O_RDWR|O_CLOEXEC);
    if (mtd_fd == -1) {
        NOR_ERRLOG("open(%s):%s", path_name, strerror(errno));
        ret_status = RET_DEV_ERROR;
    } else {
        erase_info_t s_mtd_erase;
        s_mtd_erase.start = (__u32)i_id * mtd_info->erasesize;
        s_mtd_erase.length = mtd_info->erasesize;
        // erase
        if (ioctl(mtd_fd, MEMERASE, &s_mtd_erase) < 0) {
            NOR_ERRLOG("ioctl(MEMERASE):%s", strerror(errno));
            ret_status = RET_DEV_ERROR;
        } else {
            // write
            int seek_top = i_id * (int)mtd_info->erasesize + i_offset;
            int sector_size = BACKUP_SECTOR_SIZ;        //
            int block_size = (int)mtd_info->erasesize/sector_size;
            int llpi;
            int    buff_top = 0;
            int    put_size = i_size;
            char * work_buff = (char *)p_buff;
            ssize_t    write_size;

            for (llpi=0; llpi < block_size; llpi++) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
                if (-1 == (off_t)lseek(mtd_fd, seek_top, SEEK_SET)) {
                    NOR_ERRLOG("lseek():%s", strerror(errno));
                    ret_status = RET_DEV_ERROR;
                    break;
                }
                if (put_size >= sector_size) {  // LCOV_EXCL_BR_LINE 6:double check
                    write_size = write(mtd_fd, &work_buff[buff_top], (size_t)sector_size);
                    if (write_size < 0) {
                        NOR_ERRLOG("write():%zd %s", write_size, strerror(errno));
                        ret_status = RET_DEV_ERROR;
                        break;
                    }
                    put_size -= sector_size;
                    if (put_size <= 0) {
                        break;
                    }
                } else {
                // LCOV_EXCL_START 8: dead code
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                    write_size = write(mtd_fd, &work_buff[buff_top], (size_t)put_size);
                    if (write_size < 0) {
                        NOR_ERRLOG("write():%zd %s", write_size, strerror(errno));
                        ret_status = RET_DEV_ERROR;
                        break;
                    }
                    break;
                // LCOV_EXCL_STOP
                }
                seek_top += (int)write_size;
                buff_top += (int)write_size;
            }
        }
        // LCOV_EXCL_BR_START 5:It's impossible to mock close() function,so this line can not be passed.
        if (0 != close(mtd_fd)) {
        // LCOV_EXCL_BR_STOP
            // LCOV_EXCL_START 5:It's impossible to mock close() function,so this line can not be passed.
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            NOR_ERRLOG("close():%s", strerror(errno));
            ret_status = RET_DEV_ERROR;
            // LCOV_EXCL_STOP
        }
    }
    return    ret_status;
}

/***************************************************************************
int    backup_cheksum_read(char * p_buff, int erase_size ,int * ret_sum)
 * A text character sequence and a sum value are checked.
 IN:     char * p_buff        ...top address
        int erase_size    ...erase size
        int * ret_sum    ...return sum

 OUT: NOR_BLK_CHK    RET_CHK_OK:Normal
                    RET_CHK_TEXT_ERR:test error
                    RET_CHK_BCC_ERR:sum error Except:other error
 Remarks:
*****************************************************************************/
NOR_BLK_CHK    MtdBackupCheksumRead(char * p_buff, int erase_size, int * ret_sum) {
    if (erase_size > BACKUP_CHECK_OFFSET) {  // LCOV_EXCL_BR_LINE 6: double check
        // The check of the compatibility of block data
        if (0 != memcmp(&p_buff[(erase_size-BACKUP_CHECK_OFFSET)], BACKUP_CHECK_TEXT, BACKUP_CHECK_SIZE)) {
            char tmp[BACKUP_CHECK_SIZE + 1];
            memcpy(tmp, &p_buff[(erase_size-BACKUP_CHECK_OFFSET)], BACKUP_CHECK_SIZE);
            tmp[BACKUP_CHECK_SIZE] = '\0';
            NOR_ERRLOG("invalid check_text:%s", tmp);
            return    RET_CHK_TEXT_ERR;
        }
        // The check of the compatibility of block data
        int llpi;
        int get_sum;
        int    work_sum = 0;
        get_sum = ((int)p_buff[(erase_size-BACKUP_CHECK_DAT2)] & 0xff) +
                    (((int)p_buff[(erase_size-BACKUP_CHECK_DAT1)]) << 8 & 0xff00);

        for (llpi=0; llpi < (erase_size-BACKUP_CHECK_OFFSET); llpi++) {
            work_sum += p_buff[llpi];
        }
        work_sum = (work_sum & 0xffff);
        if (work_sum != get_sum) {
            NOR_ERRLOG("invalid checksum: work:%d get:%d", work_sum, get_sum);
            return    RET_CHK_BCC_ERR;        // checksum error
        }
        if (ret_sum != NULL) {  // LCOV_EXCL_BR_LINE 6: double check
            *ret_sum = work_sum;    // chesum Storing
        }
    } else {
    // LCOV_EXCL_START 8: dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        NOR_ERRLOG("invalid erase_size:%d", erase_size);
        // checksum error
        return    RET_CHK_BCC_ERR;
    // LCOV_EXCL_STOP
    }
    return    RET_CHK_OK;
}

/***************************************************************************
int    backup_cheksum_write(char * p_buff, int erase_size )
 * A text character sequence and a sum value are written in a buffer.
 IN:     char * p_buff        ...top address
        int erase_size    ...erase size

 OUT: int    0:normal Except:error
 Remarks:
*****************************************************************************/
int    MtdBackupCheksumWrite(char * p_buff, int erase_size ) {
    int    sum_add = 0;

    if (erase_size > BACKUP_CHECK_OFFSET) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
        int ret_status;
        // The check of the compatibility of block data
        ret_status = memcmp(&p_buff[(erase_size-BACKUP_CHECK_OFFSET)], BACKUP_CHECK_TEXT, BACKUP_CHECK_SIZE);
        if (ret_status != 0) {
            memcpy(&p_buff[(erase_size-BACKUP_CHECK_OFFSET)], BACKUP_CHECK_TEXT, BACKUP_CHECK_SIZE);
        }
        int llpi;
        for (llpi=0; llpi < (erase_size-BACKUP_CHECK_OFFSET); llpi++) {
            sum_add += p_buff[llpi];
        }
    }
    p_buff[(erase_size-BACKUP_CHECK_DAT2)] = (char)(sum_add     & 0xff);
    p_buff[(erase_size-BACKUP_CHECK_DAT1)] = (char)((sum_add>>8) & 0xff);
    return    RET_DEV_NORMAL;
}

/***************************************************************************
@brief        MtdBackupInfo( mtd_info_t * mtd_info)
@outline        The information to obtain is obtained.
@type        Completion return type
@param[in]    mtd_info_t * mtd_info
@return     int
@retval        0        : No error
@retval        Except    : Error
@usage        It is Backup API of data read.
*****************************************************************************/
int    MtdBackupInfo(const char * path_name, mtd_info_t * mtd_info) {
    int         ret_status = RET_DEV_NORMAL;

    int mtd_fd = open(path_name, O_RDONLY|O_CLOEXEC);
    if (mtd_fd == -1) {
        NOR_ERRLOG("open(%s):%s", path_name, strerror(errno));
        ret_status = RET_DEV_ERROR;
    } else {
        // device control  mtdchar_ioctl of mtdchar.c
        if (ioctl(mtd_fd, MEMGETINFO, mtd_info) < 0) {
            NOR_ERRLOG("ioctl(MEMGETINFO):%s", strerror(errno));
            ret_status = RET_DEV_ERROR;
        }
        if (0 != close(mtd_fd)) {
            NOR_ERRLOG("close():%s", strerror(errno));
            ret_status = RET_DEV_ERROR;
        }
    }
    return    ret_status;
}

/**************************************************************
  End Of Files
**************************************************************/
