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
 *    BackupMgrIf.cpp
 * @brief
 *    BackupMgr service-to-service interface
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
#include "BackupMgrIf.h"

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Structre                                                                        *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Local Function Prototype                                                        *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Grobal Value                                                                    *
 *---------------------------------------------------------------------------------*/

// static BOOL g_backup_mgr_availability = FALSE;
BOOL g_backup_mgr_availability = FALSE;

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/

/**
 * @brief
 *    BackupMgr Services IF Availability Change Notification Registration
 *
 * @param[in]  fp_on_cmd    Callback function
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus BackupMgrIfNotifyOnBackupMgrAvailability(CbFuncPtr fp_on_cmd) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
    HANDLE happ;

    happ = _pb_GetAppHandle();
    if (happ != NULL) {
        /* NS_BackupMgr/Availability Changing notification registration */
        estatus = FrameworkunifiedSubscribeNotificationWithCallback(happ, NTFY_BackupMgr_Availability, (CbFuncPtr)fp_on_cmd);  // LCOV_EXCL_BR_LINE 6:unexpected branch  //NOLINT (whitespace/line_length)
        if (eFrameworkunifiedStatusOK != estatus) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                          "PositioningSubscriveNotificationswithCallback ERROR!! [estatus=%d]", estatus);
        }
    } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
    }

    return estatus;
}

/**
 * @brief
 *    BackupMgr Services IF-Availability Settings
 *
 * @param[in]  b_is_available    Available state
 * @return     none
 */
void BackupMgrIfSetAvailability(BOOL b_is_available) {
    g_backup_mgr_availability = b_is_available;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, \
        "g_backup_mgr_availability=%d", g_backup_mgr_availability);

    return;
}

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief
 *    BackupMgr Services IF-Availability Acquisition
 *
 * @param[in]  none
 * @return     gb_BackupMgrAvailability
 */

BOOL BackupMgrIf_GetAvailability(void)
{
    return g_backup_mgr_availability;
}
#ifdef __cplusplus
}
#endif

/**
 * @brief
 *    Import BackupMgr Services IFs
 *
 * @param[in]  tag_id
 * @param[in]  ui_offset
 * @param[in]  *pv_buf
 * @param[in]  ui_size
 * @param[out] pb_is_available    Available state
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus BackupMgrIfBackupDataRd(PCSTR tag_id, uint32_t ui_offset, void *pv_buf, \
        uint32_t ui_size, BOOL* pb_is_available) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
    int32_t lret;

    if (BackupMgrIf_GetAvailability() == TRUE) {
        lret = Backup_DataRd(tag_id, ui_offset, pv_buf, ui_size);
        if (lret == BKUP_RET_NORMAL) {
            /* 8Byte fixed outputs */
            FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__,  \
             "Backup_DataRd Done [tag_id=%s, pv_buf(Hex):%02x %02x %02x %02x %02x %02x %02x %02x, offset=%d, size=%d]",
             tag_id,
             *(reinterpret_cast<int8_t*>(pv_buf)),
             *((reinterpret_cast<int8_t*>(pv_buf))+1),
             *((reinterpret_cast<int8_t*>(pv_buf))+2),
             *((reinterpret_cast<int8_t*>(pv_buf))+3),
             *((reinterpret_cast<int8_t*>(pv_buf))+4),
             *((reinterpret_cast<int8_t*>(pv_buf))+5),
             *((reinterpret_cast<int8_t*>(pv_buf))+6),
             *((reinterpret_cast<int8_t*>(pv_buf))+7),
             ui_offset, ui_size);

            estatus = eFrameworkunifiedStatusOK;
        } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                "Backup_DataRd ERROR!! [lret=%d, tag_id=%s, ui_offset=%d, pv_buf=%p, ui_size=%d]", \
                lret, tag_id, ui_offset, pv_buf, ui_size);
        }
    } else {
        /* nop */
    }

    *pb_is_available = BackupMgrIf_GetAvailability();

    return estatus;
}

/**
 * @brief
 *    BackupMgr Services IF Write
 *
 * @param[in]  tag_id
 * @param[in]  *pv_buf
 * @param[in]  ui_offset
 * @param[in]  ui_size
 * @param[out] pb_is_available    Available state
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus BackupMgrIfBackupDataWt(PCSTR tag_id, void *pv_buf, uint32_t ui_offset, \
        uint32_t ui_size, BOOL* pb_is_available) {
    EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
    int32_t lret;

    if (BackupMgrIf_GetAvailability() == TRUE) {
        lret = Backup_DataWt(tag_id, pv_buf, ui_offset, ui_size);
        if (lret == BKUP_RET_NORMAL) {
            /* 8Byte fixed outputs */
            FRAMEWORKUNIFIEDLOG(ZONE_28, __FUNCTION__, 
             "Backup_DataWt Done [tag_id=%s, pv_buf(Hex):%02x %02x %02x %02x %02x %02x %02x %02x, offset=%d, size=%d]",
             tag_id,
             *(reinterpret_cast<int8_t*>(pv_buf)),
             *((reinterpret_cast<int8_t*>(pv_buf))+1),
             *((reinterpret_cast<int8_t*>(pv_buf))+2),
             *((reinterpret_cast<int8_t*>(pv_buf))+3),
             *((reinterpret_cast<int8_t*>(pv_buf))+4),
             *((reinterpret_cast<int8_t*>(pv_buf))+5),
             *((reinterpret_cast<int8_t*>(pv_buf))+6),
             *((reinterpret_cast<int8_t*>(pv_buf))+7),
             ui_offset, ui_size);

            estatus = eFrameworkunifiedStatusOK;
        } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, \
                "Backup_DataWt ERROR!! [lret=%d, tag_id=%s, pv_buf=%p, ui_offset=%d, ui_size=%d]", \
                lret, tag_id, pv_buf, ui_offset, ui_size);
        }
    } else {
        /* nop */
    }

    *pb_is_available = BackupMgrIf_GetAvailability();

    return estatus;
}
