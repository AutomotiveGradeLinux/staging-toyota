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

/*******************************************************************************
 *  File name        :GpsInt.cpp
 *  System name        :PastModel002
 *  Subsystem name    :GPS interrupt thread
 ******************************************************************************/

/*******************************************************************************
* Include File
*******************************************************************************/
#include <vehicle_service/positioning_base_library.h>
#include "POS_private.h"
#include "GpsInt.h"

#define RD_REG32(reg) \
            (*(volatile ULONG * const)(reg))

#define     COMREGMSK_OR    1
#define     ALLINTMASK      0x0f

#ifdef __cplusplus
extern "C" {
#endif
void    com_regmsk_dword(ULONG *, ULONG, UCHAR, UCHAR);    // NOLINT(readability/nolint)
#ifdef __cplusplus
}
#endif

/*******************************************************************************
* Define
*******************************************************************************/
#define GPS_INT_THREAD_DEBUG_SWITCH                    0    /* 0:OFF 1:ON */

/*******************************************************************************
* Global Parameter
*******************************************************************************/
GPS_INT_DATA    g_gps_int_data;

/*******************************************************************************
* Module
*******************************************************************************/
/*******************************************************************************
 * MODULE    : DEVGpsIntUnMapDevice
 * ABSTRACT  : Register mapping release processing
 * FUNCTION  : Release the register mapping
 * ARGUMENT  : None
 * NOTE      :
 * RETURN    : None
 ******************************************************************************/
void DEVGpsIntUnMapDevice(void) {
    /* Release GPIO Register Mapping */
    if ((g_gps_int_data.irq_status_reg        != 0)
        || (g_gps_int_data.irq_status_set_reg  != 0)
        || (g_gps_int_data.rising_detect_reg  != 0)) {
        /* When GPIO registers are mapped */
        /* #QAC confirmation Rule11.3 Data Size Equivalent (4Byte),No problem due to unsigned relation */
        MunMapDeviceIo((HANDLE)g_gps_int_data.irq_status_reg, sizeof(g_gps_int_data.irq_status_reg));
        /* #QAC confirmation Rule11.3 Data Size Equivalent (4Byte),No problem due to unsigned relation */
        MunMapDeviceIo((HANDLE)g_gps_int_data.irq_status_set_reg, sizeof(g_gps_int_data.irq_status_set_reg));
        /* #QAC confirmation Rule11.3 Data Size Equivalent (4Byte),No problem due to unsigned relation */
        MunMapDeviceIo((HANDLE)g_gps_int_data.rising_detect_reg, sizeof(g_gps_int_data.rising_detect_reg));

        g_gps_int_data.irq_status_reg        = 0;
        g_gps_int_data.irq_status_set_reg    = 0;
        g_gps_int_data.rising_detect_reg    = 0;
    }
}

/*******************************************************************************
 * MODULE    : DEVGpsIntSetInterupt
 * ABSTRACT  : GPS interrupt setting
 * FUNCTION  : Setting GPS Interrupt
 * ARGUMENT  : None
 * NOTE      :
 * RETURN    : TRUE        : Interrupt setting success
 *           : FALSE    : Interrupt setting failure
 ******************************************************************************/
void DEVGpsIntSetInterupt(void) {
#if GPS_INT_THREAD_DEBUG_SWITCH
    /* Dummy read */
    itr_ret = RD_REG32(g_gps_int_data.irq_status_set_reg);

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "GpsInt.cpp, IRQSTATUS_SET reg value(before) = 0x%08X ", itr_ret);
#endif

    /* GPS interrupt Enable setting */
    /* Dummy read */
    /* #QAC confirmation Rule11.3 Data Size Equivalent (4Byte),No problem due to unsigned relation */
    (void)RD_REG32(g_gps_int_data.irq_status_set_reg);

#if GPS_INT_THREAD_DEBUG_SWITCH
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "GpsInt.cpp, IRQSTATUS_SET reg value(after) = 0x%08X ", itr_ret);
#endif



#if GPS_INT_THREAD_DEBUG_SWITCH
    /* Dummy read */
    itr_ret = RD_REG32(g_gps_int_data.rising_detect_reg);

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "GpsInt.cpp, RISINGDETECT reg value(before) = 0x%08X ", itr_ret);
#endif

    /* GPS Interrupt Detection Method Setting */
    /* #QAC confirmation Rule11.3 Data Size Equivalent (4Byte),No problem due to unsigned relation */
    com_regmsk_dword(reinterpret_cast<ULONG *>(g_gps_int_data.rising_detect_reg), \
        GPS_MASK_6BIT, COMREGMSK_OR, ALLINTMASK);

    /* Dummy read */
    /* #QAC confirmation Rule11.3 Data Size Equivalent (4Byte),No problem due to unsigned relation */
    (void)RD_REG32(g_gps_int_data.rising_detect_reg);

#if GPS_INT_THREAD_DEBUG_SWITCH
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "GpsInt.cpp, RISINGDETECT reg value(after)  = 0x%08X ", itr_ret);
#endif
}

/*******************************************************************************
 * MODULE    : DEVGpsIntIntRegClear
 * ABSTRACT  : Interrupt cause register clear processing
 * FUNCTION  : Clear the interrupt cause register
 * ARGUMENT  : None
 * NOTE      :
 * RETURN    : None
 ******************************************************************************/
void DEVGpsIntIntRegClear(void) {
#if GPS_INT_THREAD_DEBUG_SWITCH
    /* Dummy read */
    itr_ret = RD_REG32(g_gps_int_data.irq_status_reg);

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "GpsInt.cpp, IRQSTATUS reg value(before) = 0x%08X ", itr_ret);
#endif

    /* Clear Interrupt Source Register */
    /* Dummy read */
    /* #QAC confirmation Rule11.3 Data Size Equivalent (4Byte),No problem due to unsigned relation */
    (void)RD_REG32(g_gps_int_data.irq_status_reg);

#if GPS_INT_THREAD_DEBUG_SWITCH
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "GpsInt.cpp, IRQSTATUS reg value(after)  = 0x%08X ", itr_ret);
#endif
}

/*******************************************************************************
 * MODULE    : DEVGpsIntSndMsg
 * ABSTRACT  : Send GPS interrupt occurrence notification
 * FUNCTION  : Notify GPS communication management that an interrupt has occurred.
 * ARGUMENT  : None
 * NOTE      :
 * RETURN    : RET_NORMAL    : Normal completion
 *           : Other than the above        : Processing error
 ******************************************************************************/
RET_API DEVGpsIntSndMsg(void) {
    return RET_NORMAL;
}

