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
/**
* @file MDev_Gps_Mtrx.h
*/

#ifndef  INC_GPSCOMMON_MDEV_GPS_MTRX_H_
#define  INC_GPSCOMMON_MDEV_GPS_MTRX_H_

/*---------------------------------------------------------------------------*/
// Include files

/*---------------------------------------------------------------------------*/
// Prototype

void DevGpsNop(void);
void DevGpsInitStartSendReq(void);
void DevGpsInitStartGPSResetReq(void);
void DevGpsInitStartRcvCyclCmd(void);
void DevGpsInitStartCyclDataTimeOut(void);
void DevGpsInitStartNaviDataTimeOut(void);    /* #GPF_60_077 */
void DevGpsInitStartDiagClkGuardTimeOut(void);  /* #GPF_60_077 */
void DevGpsInitStartAccOffStart(void);      /* #GPF_60_024 */
void DevGpsInitStartNaviInfoDeliver(void);    /* #GPF_60_077 */
void DevGpsInitStartNaviSpeedDeliver(void);    /* #GPF_60_077 */
void DevGpsInitStartSettingTime(void);      /* #GPF_60_077 */
void DevGpsInitStartNmeaDataGuardTimeOut(void);  /* #GPF_60_109 */

void DevGpsInitStartBackupDataLoadReq(void);
void DevGpsInitStartStopReq(void);
void DevGpsInitStartGpsweekcorcntNtf(void);
void DevGpsInitStartRecoveryTimeOut(void);
void DevGpsInitStartGpsReceiverErrTimeOut(void);

void DevGpsNormalSendReq(void);
void DevGpsNormalGPSResetReq(void);
void DevGpsNormalRcvCyclCmd(void);
void DevGpsNormalCyclDataTimeOut(void);
void DevGpsNormalNaviDataTimeOut(void);      /* #GPF_60_077 */
void DevGpsNormalDiagClkGuardTimeOut(void);    /* #GPF_60_077 */
void DevGpsNormalAccOffStart(void);        /* #GPF_60_024 */
void DevGpsNormalNaviInfoDeliver(void);      /* #GPF_60_077 */
void DevGpsNormalNaviSpeedDeliver(void);      /* #GPF_60_077 */
void DevGpsNormalSettingTime(void);        /* #GPF_60_077 */
void DevGpsNormalNmeaDataGuardTimeOut(void);    /* #GPF_60_109 */

void DevGpsNormalBackupDataLoadReq(void);
void DevGpsNormalStopReq(void);
void DevGpsNormalGpsweekcorcntNtf(void);
void DevGpsNormalRecoveryTimeOut(void);
void DevGpsNormalGpsReceiverErrTimeOut(void);

void DevGpsSendSendReq(void);
void DevGpsSendGPSResetReq(void);
void DevGpsSendRcvCyclCmd(void);
void DevGpsSendRcvRspCmd(void);          /* #GPF_60_040 */
void DevGpsSendRspDataTimeOut(void);        /* #GPF_60_040 */
void DevGpsSendCyclDataTimeOut(void);
void DevGpsSendNaviDataTimeOut(void);        /* #GPF_60_077 */
void DevGpsSendDiagClkGuardTimeOut(void);      /* #GPF_60_077 */
void DevGpsSendAccOffStart(void);          /* #GPF_60_040 */
void DevGpsSendNaviInfoDeliver(void);        /* #GPF_60_077 */
void DevGpsSendNaviSpeedDeliver(void);      /* #GPF_60_077 */
void DevGpsSendSettingTime(void);          /* #GPF_60_077 */
void DevGpsSendNmeaDataGuardTimeOut(void);    /* #GPF_60_109 */

void DevGpsSendBackupDataLoadReq(void);
void DevGpsSendStopReq(void);
void DevGpsSendGpsweekcorcntNtf(void);
void DevGpsSendRecoveryTimeOut(void);
void DevGpsSendGpsReceiverErrTimeOut(void);

/*---------------------------------------------------------------------------*/
#endif  // INC_GPSCOMMON_MDEV_GPS_MTRX_H_

/*---------------------------------------------------------------------------*/
/*EOF*/
