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
 * @file el_gpio.h
 * @brief el_gpio function header file
 */

#ifndef MEMORY_GPIO_ACCESS_LIBRARY_CLIENT_INCLUDE_NATIVE_SERVICE_EL_GPIO_H_
#define MEMORY_GPIO_ACCESS_LIBRARY_CLIENT_INCLUDE_NATIVE_SERVICE_EL_GPIO_H_

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup memory_gpio_access_library
 *  @ingroup native_service
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>

/**
 * \~english Definitions of the GPIO Control Symbol
 */
enum {
  EL_GPO_SUB_RST_N = 0 ,
  EL_GPO_NAVI_DET,
  EL_GPI_ILG_RST_N,
  EL_GPI_GPS_INT,
  EL_GPO_GPS_P_OFF,
  EL_GPO_CP_RST_N,
  EL_GPO_TP_RST_N,
  EL_GPI_SEC_ST1,
  EL_GPI_SEC_ST0,
  EL_GPO__CWORD52__RST,
  EL_GPI__CWORD32__INT,
  EL_GPO__CWORD32__RST,
  EL_GPO_XM_RST,
  EL_GPO_NFC_RST,
  EL_GPO_USB1_PWEN,
  EL_GPI_USB1_OVC,
  EL_GPO__CWORD52__VUP,
  EL_GPO_WL_EN,
  EL_GPO_BT_EN,
  EL_GPO_AVB_RST_N,
  EL_GPO_SEC_RST_N,
  EL_SPI_NAVI_REQ,
  EL_SPI_NAVI_RDY,
  EL_GPI_USB0_QVC,
  EL_GPO_USB0_PWE,
  EL_GPO_GVIFTX1_RST_N,
  EL_GPO_GVIFTX0_RST_N,
  EL_GPO_EMMC_ON_N,
  EL_SPI_GVIF_D_UCS,
  EL_GPO__CWORD65__RST_N,
  EL_GPO_GVIFRX_RST_N,
  EL_GPI_IOEX_INT,
  EL_GPO_FBRQ,
  EL_GPO_IOEX_RST_N,
  EL_GPI_GVIFTX1_GPIO3,
  EL_GPI_DARC_INT,
  EL_GPI_GVIFTX0_GPIO3,
  EL_GPI_GVIF_REQ_INT,
  EL_GPI_ACC_DET_OUT,
  EL_GPI_GVIFRX_GPIO3,
  EL_GPO_DTCP_RST_N,
  EL_NAVI_NMI_N,
  EL_GPI_SDPCHK,
  EL_GPO_XM_SHDN,
  EL_GPI_GVIF_D_REQ_INT,
  EL_SPI_GVIF_D_DCS,
  EL_GPO_EMMC_PU_ON,
  EL_GPIO_GVIFRX_GPIO0,
  EL_GPI_GVIFRX_GPIO2,
  EL_GPI_GVIFRX_LOS,
  EL_GPIO_GVIFTX0_GPIO0,
  EL_GPI_GVIFTX0_GPIO2,
  EL_GPIO_GVIFTX1_GPIO0,
  EL_GPI_GVIFTX1_GPIO2,
  EL_GPO_USBHUB_RST,
  EL_GPI__CWORD65__BOOT_ERR,
  EL_GPO_CKECNT,
  EL_GPO_SDPCLR,
  EL_GPO_SENSOR_RST,
  EL_GPI_SENSOR_INT,
  EL_GPO_DAC_MUTE,
  EL_GPO_DSRC_PON,
  EL_GPI_2PULSE_REV,
  EL_GPI_ILL_DET,
  EL_GPI_DAB_DET,
  EL_GPI_FAN_DET,

  EL_GPO_SOC_DIAG_OUT,
  EL_GPI_SOC_DIAG_IN,

  EL_GPIO_SYMBOL_MAX,
};

/**
 * \~english Rising level trigger
 */
#define EL_GPIO_IRQ_TRIGGER_RISING    (0x01)

/**
 * \~english Falling level trigger
 */
#define EL_GPIO_IRQ_TRIGGER_FALLING   (0x02)

/**
 * \~english High level trigger
 */
#define EL_GPIO_IRQ_TRIGGER_HIGH    (0x04)

/**
 * \~english Low level trigger
 */
#define EL_GPIO_IRQ_TRIGGER_LOW     (0x08)

/**
 * \~english Trigger mask
 */
#define EL_GPIO_IRQ_TRIGGER_MASK    (EL_GPIO_IRQ_TRIGGER_RISING |        \
                                     EL_GPIO_IRQ_TRIGGER_FALLING |       \
                                     EL_GPIO_IRQ_TRIGGER_HIGH |          \
                                     EL_GPIO_IRQ_TRIGGER_LOW)

/**
 * \~english None timeout
 */
#define EL_GPIO_IRQ_NO_TIMEOUT (-1)

/**\ingroup EL_Gpio_write
* \~english @par Brief
*        Write data to specified GPIO.
* \~english @param [in] symbol
*        int   - symbol(Definitions of the GPIO Control Symbol)
* \~english @param [in] value
*        bool  - setting value(0 or 1)
* \~english @par
*        Definitions of the GPIO Control Symbol
* \~english @code
*        enum {
*                   EL_GPO_SUB_RST_N = 0 ,
*                   EL_GPO_NAVI_DET,
*                   EL_GPI_ILG_RST_N,
*                   EL_GPI_GPS_INT,
*                   EL_GPO_GPS_P_OFF,
*                   EL_GPO_CP_RST_N,
*                   EL_GPO_TP_RST_N,
*                   EL_GPI_SEC_ST1,
*                   EL_GPI_SEC_ST0,
*                   EL_GPO__CWORD52__RST,
*                   EL_GPI__CWORD32__INT,
*                   EL_GPO__CWORD32__RST,
*                   EL_GPO_XM_RST,
*                   EL_GPO_NFC_RST,
*                   EL_GPO_USB1_PWEN,
*                   EL_GPI_USB1_OVC,
*                   EL_GPO__CWORD52__VUP,
*                   EL_GPO_WL_EN,
*                   EL_GPO_BT_EN,
*                   EL_GPO_AVB_RST_N,
*                   EL_GPO_SEC_RST_N,
*                   EL_SPI_NAVI_REQ,
*                   EL_SPI_NAVI_RDY,
*                   EL_GPI_USB0_QVC,
*                   EL_GPO_USB0_PWE,
*                   EL_GPO_GVIFTX1_RST_N,
*                   EL_GPO_GVIFTX0_RST_N,
*                   EL_GPO_EMMC_ON_N,
*                   EL_SPI_GVIF_D_UCS,
*                   EL_GPO__CWORD65__RST_N,
*                   EL_GPO_GVIFRX_RST_N,
*                   EL_GPI_IOEX_INT,
*                   EL_GPO_FBRQ,
*                   EL_GPO_IOEX_RST_N,
*                   EL_GPI_GVIFTX1_GPIO3,
*                   EL_GPI_DARC_INT,
*                   EL_GPI_GVIFTX0_GPIO3,
*                   EL_GPI_DARC_INT,
*                   EL_GPI_GVIFTX0_GPIO3,
*                   EL_GPI_GVIF_REQ_INT,
*                   EL_GPI_ACC_DET_OUT,
*                   EL_GPI_GVIFRX_GPIO3,
*                   EL_GPO_DTCP_RST_N,
*                   EL_NAVI_NMI_N,
*                   EL_GPI_SDPCHK,
*                   EL_GPO_XM_SHDN,
*                   EL_GPI_GVIF_D_REQ_INT,
*                   EL_SPI_GVIF_D_DCS,
*                   EL_GPO_EMMC_PU_ON,
*                   EL_GPIO_GVIFRX_GPIO0,
*                   EL_GPI_GVIFRX_GPIO2,
*                   EL_GPI_GVIFRX_LOS,
*                   EL_GPIO_GVIFTX0_GPIO0,
*                   EL_GPI_GVIFTX0_GPIO2,
*                   EL_GPIO_GVIFTX1_GPIO0,
*                   EL_GPI_GVIFTX1_GPIO2,
*                   EL_GPO_USBHUB_RST,
*                   EL_GPI__CWORD65__BOOT_ERR,
*                   EL_GPO_CKECNT,
*                   EL_GPO_SDPCLR,
*                   EL_GPO_SENSOR_RST,
*                   EL_GPI_SENSOR_INT,
*                   EL_GPO_DAC_MUTE,
*                   EL_GPO_DSRC_PON,
*                   EL_GPI_2PULSE_REV,
*                   EL_GPI_ILL_DET,
*                   EL_GPI_DAB_DET,
*                   EL_GPI_FAN_DET,
*                   EL_GPO_SOC_DIAG_OUT,
*                   EL_GPI_SOC_DIAG_IN,
*                   EL_GPIO_SYMBOL_MAX,
*        };
*            @endcode
* \~english @retval 0 Success
* \~english @retval -1 Error:Set errno according to error type when error occurs.
* \~english @par Prerequisite
*        - None.
* \~english @par Change of internal state
*        - Change of internal state according to the API does not occur.
* \~english @par Conditions of processing failure
*        - The value of symbol is less than 0 or greater than EL_GPIO_SYMBOL_MAX-1. [-1]
*        - syscall error(open(), ioctl()). [-1]
* \~english @par Detail
*        - There is authority to access the specified GPIO, writing data is possiable only\n
*        when GPIO port is an output port.
* \~english @par Classification
*        Public
* \~english @par Type
*        Sync
* \~english @see EL_Gpio_read
*/
int EL_Gpio_write(int symbol , bool value);  // NOLINT[readability/naming]

/** \ingroup EL_Gpio_read
* \~english @par Brief
*        Read data from specified GPIO.
* \~english @param [in] symbol
*        int   - symbol(Definitions of the GPIO Control Symbol)
* \~english @param [out] value
*        bool *  - setting value
* \~english @par
*        Definitions of the GPIO Control Symbol
* \~english @code
*        enum {
*                   EL_GPO_SUB_RST_N = 0 ,
*                   EL_GPO_NAVI_DET,
*                   EL_GPI_ILG_RST_N,
*                   EL_GPI_GPS_INT,
*                   EL_GPO_GPS_P_OFF,
*                   EL_GPO_CP_RST_N,
*                   EL_GPO_TP_RST_N,
*                   EL_GPI_SEC_ST1,
*                   EL_GPI_SEC_ST0,
*                   EL_GPO__CWORD52__RST,
*                   EL_GPI__CWORD32__INT,
*                   EL_GPO__CWORD32__RST,
*                   EL_GPO_XM_RST,
*                   EL_GPO_NFC_RST,
*                   EL_GPO_USB1_PWEN,
*                   EL_GPI_USB1_OVC,
*                   EL_GPO__CWORD52__VUP,
*                   EL_GPO_WL_EN,
*                   EL_GPO_BT_EN,
*                   EL_GPO_AVB_RST_N,
*                   EL_GPO_SEC_RST_N,
*                   EL_SPI_NAVI_REQ,
*                   EL_SPI_NAVI_RDY,
*                   EL_GPI_USB0_QVC,
*                   EL_GPO_USB0_PWE,
*                   EL_GPO_GVIFTX1_RST_N,
*                   EL_GPO_GVIFTX0_RST_N,
*                   EL_GPO_EMMC_ON_N,
*                   EL_SPI_GVIF_D_UCS,
*                   EL_GPO__CWORD65__RST_N,
*                   EL_GPO_GVIFRX_RST_N,
*                   EL_GPI_IOEX_INT,
*                   EL_GPO_FBRQ,
*                   EL_GPO_IOEX_RST_N,
*                   EL_GPI_GVIFTX1_GPIO3,
*                   EL_GPI_DARC_INT,
*                   EL_GPI_GVIFTX0_GPIO3,
*                   EL_GPI_DARC_INT,
*                   EL_GPI_GVIFTX0_GPIO3,
*                   EL_GPI_GVIF_REQ_INT,
*                   EL_GPI_ACC_DET_OUT,
*                   EL_GPI_GVIFRX_GPIO3,
*                   EL_GPO_DTCP_RST_N,
*                   EL_NAVI_NMI_N,
*                   EL_GPI_SDPCHK,
*                   EL_GPO_XM_SHDN,
*                   EL_GPI_GVIF_D_REQ_INT,
*                   EL_SPI_GVIF_D_DCS,
*                   EL_GPO_EMMC_PU_ON,
*                   EL_GPIO_GVIFRX_GPIO0,
*                   EL_GPI_GVIFRX_GPIO2,
*                   EL_GPI_GVIFRX_LOS,
*                   EL_GPIO_GVIFTX0_GPIO0,
*                   EL_GPI_GVIFTX0_GPIO2,
*                   EL_GPIO_GVIFTX1_GPIO0,
*                   EL_GPI_GVIFTX1_GPIO2,
*                   EL_GPO_USBHUB_RST,
*                   EL_GPI__CWORD65__BOOT_ERR,
*                   EL_GPO_CKECNT,
*                   EL_GPO_SDPCLR,
*                   EL_GPO_SENSOR_RST,
*                   EL_GPI_SENSOR_INT,
*                   EL_GPO_DAC_MUTE,
*                   EL_GPO_DSRC_PON,
*                   EL_GPI_2PULSE_REV,
*                   EL_GPI_ILL_DET,
*                   EL_GPI_DAB_DET,
*                   EL_GPI_FAN_DET,
*                   EL_GPO_SOC_DIAG_OUT,
*                   EL_GPI_SOC_DIAG_IN,
*                   EL_GPIO_SYMBOL_MAX,
*        };
*            @endcode
* \~english @retval 0 Success
* \~english @retval -1 Error:Set errno according to error type when error occurs.
* \~english @par Prerequisite
*        - None.
* \~english @par Change of internal state
*        - Change of internal state according to the API does not occur.
* \~english @par Conditions of processing failure
*        - The value of symbol is less than 0 or greater than EL_GPIO_SYMBOL_MAX-1. [-1]
*        - syscall error(open(), ioctl()). [-1]
* \~english @par Detail
*        - Reading data is possiable when there is authority to access the specified GPIO.
* \~english @par Classification
*        Public
* \~english @par Type
*        Sync
* \~english @see EL_Gpio_write
*/
int EL_Gpio_read(int symbol , bool *value);  // NOLINT[readability/naming]

/** \ingroup EL_Gpio_attachInterruptControl
* \~english @par Brief
*        Register interruption control of specified GPIO.
* \~english @param [in] symbol
*        int   - symbol(Definitions of the GPIO Control Symbol)
* \~english @param [in] irq_flags
*        unsigned long - Interruption control flag of GPIO.
* \~english @par
*        Definitions of the GPIO Control Symbol
* \~english @code
*        enum {
*                   EL_GPO_SUB_RST_N = 0 ,
*                   EL_GPO_NAVI_DET,
*                   EL_GPI_ILG_RST_N,
*                   EL_GPI_GPS_INT,
*                   EL_GPO_GPS_P_OFF,
*                   EL_GPO_CP_RST_N,
*                   EL_GPO_TP_RST_N,
*                   EL_GPI_SEC_ST1,
*                   EL_GPI_SEC_ST0,
*                   EL_GPO__CWORD52__RST,
*                   EL_GPI__CWORD32__INT,
*                   EL_GPO__CWORD32__RST,
*                   EL_GPO_XM_RST,
*                   EL_GPO_NFC_RST,
*                   EL_GPO_USB1_PWEN,
*                   EL_GPI_USB1_OVC,
*                   EL_GPO__CWORD52__VUP,
*                   EL_GPO_WL_EN,
*                   EL_GPO_BT_EN,
*                   EL_GPO_AVB_RST_N,
*                   EL_GPO_SEC_RST_N,
*                   EL_SPI_NAVI_REQ,
*                   EL_SPI_NAVI_RDY,
*                   EL_GPI_USB0_QVC,
*                   EL_GPO_USB0_PWE,
*                   EL_GPO_GVIFTX1_RST_N,
*                   EL_GPO_GVIFTX0_RST_N,
*                   EL_GPO_EMMC_ON_N,
*                   EL_SPI_GVIF_D_UCS,
*                   EL_GPO__CWORD65__RST_N,
*                   EL_GPO_GVIFRX_RST_N,
*                   EL_GPI_IOEX_INT,
*                   EL_GPO_FBRQ,
*                   EL_GPO_IOEX_RST_N,
*                   EL_GPI_GVIFTX1_GPIO3,
*                   EL_GPI_DARC_INT,
*                   EL_GPI_GVIFTX0_GPIO3,
*                   EL_GPI_DARC_INT,
*                   EL_GPI_GVIFTX0_GPIO3,
*                   EL_GPI_GVIF_REQ_INT,
*                   EL_GPI_ACC_DET_OUT,
*                   EL_GPI_GVIFRX_GPIO3,
*                   EL_GPO_DTCP_RST_N,
*                   EL_NAVI_NMI_N,
*                   EL_GPI_SDPCHK,
*                   EL_GPO_XM_SHDN,
*                   EL_GPI_GVIF_D_REQ_INT,
*                   EL_SPI_GVIF_D_DCS,
*                   EL_GPO_EMMC_PU_ON,
*                   EL_GPIO_GVIFRX_GPIO0,
*                   EL_GPI_GVIFRX_GPIO2,
*                   EL_GPI_GVIFRX_LOS,
*                   EL_GPIO_GVIFTX0_GPIO0,
*                   EL_GPI_GVIFTX0_GPIO2,
*                   EL_GPIO_GVIFTX1_GPIO0,
*                   EL_GPI_GVIFTX1_GPIO2,
*                   EL_GPO_USBHUB_RST,
*                   EL_GPI__CWORD65__BOOT_ERR,
*                   EL_GPO_CKECNT,
*                   EL_GPO_SDPCLR,
*                   EL_GPO_SENSOR_RST,
*                   EL_GPI_SENSOR_INT,
*                   EL_GPO_DAC_MUTE,
*                   EL_GPO_DSRC_PON,
*                   EL_GPI_2PULSE_REV,
*                   EL_GPI_ILL_DET,
*                   EL_GPI_DAB_DET,
*                   EL_GPI_FAN_DET,
*                   EL_GPO_SOC_DIAG_OUT,
*                   EL_GPI_SOC_DIAG_IN,
*                   EL_GPIO_SYMBOL_MAX,
*        };
*            @endcode
* \~english @par
*        Interruption control flag of GPIO
*        - EL_GPIO_IRQ_TRIGGER_RISING  :Rising level trigger
*        - EL_GPIO_IRQ_TRIGGER_FALLING :Falling level trigger
*        - EL_GPIO_IRQ_TRIGGER_HIGH  :High level trigger
*        - EL_GPIO_IRQ_TRIGGER_LOW :Low level trigger
* \~english @retval 0 Success
* \~english @retval -1 Error
* \~english @par Prerequisite
*        - None.
* \~english @par Change of internal state
*        - Change of internal state according to the API does not occur.
* \~english @par Conditions of processing failure
*        - Flag specified by irq_flags is not defined flag. [-1]
*        - The value of symbol is less than 0 or greater than EL_GPIO_SYMBOL_MAX-1. [-1]
*        - syscall error(open(), ioctl()). [-1]
* \~english @par Detail
*        - Register interruption control of specified GPIO.
* \~english @par Classification
*        Public
* \~english @par Type
*        Sync
* \~english @see EL_Gpio_detachInterruptControl
*/
int EL_Gpio_attachInterruptControl(int symbol , unsigned long irq_flags);  // NOLINT [readability/nolint]

/** \ingroup EL_Gpio_detachInterruptControl
* \~english @par Brief
*        Detach interruption control of specified GPIO.
* \~english @param [in] symbol
*        int   - symbol(Definitions of the GPIO Control Symbol)
* \~english @par
*        Definitions of the GPIO Control Symbol
* \~english @code
*        enum {
*                   EL_GPO_SUB_RST_N = 0 ,
*                   EL_GPO_NAVI_DET,
*                   EL_GPI_ILG_RST_N,
*                   EL_GPI_GPS_INT,
*                   EL_GPO_GPS_P_OFF,
*                   EL_GPO_CP_RST_N,
*                   EL_GPO_TP_RST_N,
*                   EL_GPI_SEC_ST1,
*                   EL_GPI_SEC_ST0,
*                   EL_GPO__CWORD52__RST,
*                   EL_GPI__CWORD32__INT,
*                   EL_GPO__CWORD32__RST,
*                   EL_GPO_XM_RST,
*                   EL_GPO_NFC_RST,
*                   EL_GPO_USB1_PWEN,
*                   EL_GPI_USB1_OVC,
*                   EL_GPO__CWORD52__VUP,
*                   EL_GPO_WL_EN,
*                   EL_GPO_BT_EN,
*                   EL_GPO_AVB_RST_N,
*                   EL_GPO_SEC_RST_N,
*                   EL_SPI_NAVI_REQ,
*                   EL_SPI_NAVI_RDY,
*                   EL_GPI_USB0_QVC,
*                   EL_GPO_USB0_PWE,
*                   EL_GPO_GVIFTX1_RST_N,
*                   EL_GPO_GVIFTX0_RST_N,
*                   EL_GPO_EMMC_ON_N,
*                   EL_SPI_GVIF_D_UCS,
*                   EL_GPO__CWORD65__RST_N,
*                   EL_GPO_GVIFRX_RST_N,
*                   EL_GPI_IOEX_INT,
*                   EL_GPO_FBRQ,
*                   EL_GPO_IOEX_RST_N,
*                   EL_GPI_GVIFTX1_GPIO3,
*                   EL_GPI_DARC_INT,
*                   EL_GPI_GVIFTX0_GPIO3,
*                   EL_GPI_DARC_INT,
*                   EL_GPI_GVIFTX0_GPIO3,
*                   EL_GPI_GVIF_REQ_INT,
*                   EL_GPI_ACC_DET_OUT,
*                   EL_GPI_GVIFRX_GPIO3,
*                   EL_GPO_DTCP_RST_N,
*                   EL_NAVI_NMI_N,
*                   EL_GPI_SDPCHK,
*                   EL_GPO_XM_SHDN,
*                   EL_GPI_GVIF_D_REQ_INT,
*                   EL_SPI_GVIF_D_DCS,
*                   EL_GPO_EMMC_PU_ON,
*                   EL_GPIO_GVIFRX_GPIO0,
*                   EL_GPI_GVIFRX_GPIO2,
*                   EL_GPI_GVIFRX_LOS,
*                   EL_GPIO_GVIFTX0_GPIO0,
*                   EL_GPI_GVIFTX0_GPIO2,
*                   EL_GPIO_GVIFTX1_GPIO0,
*                   EL_GPI_GVIFTX1_GPIO2,
*                   EL_GPO_USBHUB_RST,
*                   EL_GPI__CWORD65__BOOT_ERR,
*                   EL_GPO_CKECNT,
*                   EL_GPO_SDPCLR,
*                   EL_GPO_SENSOR_RST,
*                   EL_GPI_SENSOR_INT,
*                   EL_GPO_DAC_MUTE,
*                   EL_GPO_DSRC_PON,
*                   EL_GPI_2PULSE_REV,
*                   EL_GPI_ILL_DET,
*                   EL_GPI_DAB_DET,
*                   EL_GPI_FAN_DET,
*                   EL_GPO_SOC_DIAG_OUT,
*                   EL_GPI_SOC_DIAG_IN,
*                   EL_GPIO_SYMBOL_MAX,
*        };
*            @endcode
* \~english @retval 0 Success
* \~english @retval -1 Error
* \~english @par Prerequisite
*        - None.
* \~english @par Change of internal state
*        - Change of internal state according to the API does not occur.
* \~english @par Conditions of processing failure
*        - The value of symbol is less than 0 or greater than EL_GPIO_SYMBOL_MAX-1. [-1]
*        - syscall error(open(), ioctl()). [-1]
* \~english @par Detail
*        - Detach interruption control of specified GPIO.
* \~english @par Classification
*        Public
* \~english @par Type
*        Sync
* \~english @see EL_Gpio_attachInterruptControl
*/
int EL_Gpio_detachInterruptControl(int symbol);  // NOLINT[readability/naming]

/** \ingroup EL_Gpio_waitInterrupt
* \~english @par Brief
*        Waiting interruption of specified GPIO occured without timeout.
* \~english @param [in] symbol
*        int   - symbol(Definitions of the GPIO Control Symbol)
* \~english @par
*        Definitions of the GPIO Control Symbol
* \~english @code
*        enum {
*                   EL_GPO_SUB_RST_N = 0 ,
*                   EL_GPO_NAVI_DET,
*                   EL_GPI_ILG_RST_N,
*                   EL_GPI_GPS_INT,
*                   EL_GPO_GPS_P_OFF,
*                   EL_GPO_CP_RST_N,
*                   EL_GPO_TP_RST_N,
*                   EL_GPI_SEC_ST1,
*                   EL_GPI_SEC_ST0,
*                   EL_GPO__CWORD52__RST,
*                   EL_GPI__CWORD32__INT,
*                   EL_GPO__CWORD32__RST,
*                   EL_GPO_XM_RST,
*                   EL_GPO_NFC_RST,
*                   EL_GPO_USB1_PWEN,
*                   EL_GPI_USB1_OVC,
*                   EL_GPO__CWORD52__VUP,
*                   EL_GPO_WL_EN,
*                   EL_GPO_BT_EN,
*                   EL_GPO_AVB_RST_N,
*                   EL_GPO_SEC_RST_N,
*                   EL_SPI_NAVI_REQ,
*                   EL_SPI_NAVI_RDY,
*                   EL_GPI_USB0_QVC,
*                   EL_GPO_USB0_PWE,
*                   EL_GPO_GVIFTX1_RST_N,
*                   EL_GPO_GVIFTX0_RST_N,
*                   EL_GPO_EMMC_ON_N,
*                   EL_SPI_GVIF_D_UCS,
*                   EL_GPO__CWORD65__RST_N,
*                   EL_GPO_GVIFRX_RST_N,
*                   EL_GPI_IOEX_INT,
*                   EL_GPO_FBRQ,
*                   EL_GPO_IOEX_RST_N,
*                   EL_GPI_GVIFTX1_GPIO3,
*                   EL_GPI_DARC_INT,
*                   EL_GPI_GVIFTX0_GPIO3,
*                   EL_GPI_DARC_INT,
*                   EL_GPI_GVIFTX0_GPIO3,
*                   EL_GPI_GVIF_REQ_INT,
*                   EL_GPI_ACC_DET_OUT,
*                   EL_GPI_GVIFRX_GPIO3,
*                   EL_GPO_DTCP_RST_N,
*                   EL_NAVI_NMI_N,
*                   EL_GPI_SDPCHK,
*                   EL_GPO_XM_SHDN,
*                   EL_GPI_GVIF_D_REQ_INT,
*                   EL_SPI_GVIF_D_DCS,
*                   EL_GPO_EMMC_PU_ON,
*                   EL_GPIO_GVIFRX_GPIO0,
*                   EL_GPI_GVIFRX_GPIO2,
*                   EL_GPI_GVIFRX_LOS,
*                   EL_GPIO_GVIFTX0_GPIO0,
*                   EL_GPI_GVIFTX0_GPIO2,
*                   EL_GPIO_GVIFTX1_GPIO0,
*                   EL_GPI_GVIFTX1_GPIO2,
*                   EL_GPO_USBHUB_RST,
*                   EL_GPI__CWORD65__BOOT_ERR,
*                   EL_GPO_CKECNT,
*                   EL_GPO_SDPCLR,
*                   EL_GPO_SENSOR_RST,
*                   EL_GPI_SENSOR_INT,
*                   EL_GPO_DAC_MUTE,
*                   EL_GPO_DSRC_PON,
*                   EL_GPI_2PULSE_REV,
*                   EL_GPI_ILL_DET,
*                   EL_GPI_DAB_DET,
*                   EL_GPI_FAN_DET,
*                   EL_GPO_SOC_DIAG_OUT,
*                   EL_GPI_SOC_DIAG_IN,
*                   EL_GPIO_SYMBOL_MAX,
*        };
*            @endcode
* \~english @retval 0 Success
* \~english @retval -1 Error
* \~english @par Prerequisite
*        - Interruption of specified GPIO was registered by \ref EL_Gpio_attachInterruptControl.
* \~english @par Change of internal state
*        - Change of internal state according to the API does not occur.
* \~english @par Conditions of processing failure
*        - The value of symbol is less than 0 or greater than EL_GPIO_SYMBOL_MAX-1. [-1]
*        - syscall error(open(), ioctl()). [-1]
* \~english @par Detail
*        - Waiting interruption of specified GPIO occured without timeout.
* \~english @par Classification
*        Public
* \~english @par Type
*        Sync
* \~english @see EL_Gpio_waitInterrupt_timeout
*/
int EL_Gpio_waitInterrupt(int symbol);  // NOLINT[readability/naming]

/** \ingroup EL_Gpio_waitInterrupt_timeout
* \~english @par Brief
*        Waiting interruption of specified GPIO with timeout.
* \~english @param [in] symbol
*        int   - Symbol(Definitions of the GPIO Control Symbol)
* \~english @param [in] timeout
*        int   - Timeout of time(unit: jiffies)
* \~english @par
*        Definitions of the GPIO Control Symbol
* \~english @code
*        enum {
*                   EL_GPO_SUB_RST_N = 0 ,
*                   EL_GPO_NAVI_DET,
*                   EL_GPI_ILG_RST_N,
*                   EL_GPI_GPS_INT,
*                   EL_GPO_GPS_P_OFF,
*                   EL_GPO_CP_RST_N,
*                   EL_GPO_TP_RST_N,
*                   EL_GPI_SEC_ST1,
*                   EL_GPI_SEC_ST0,
*                   EL_GPO__CWORD52__RST,
*                   EL_GPI__CWORD32__INT,
*                   EL_GPO__CWORD32__RST,
*                   EL_GPO_XM_RST,
*                   EL_GPO_NFC_RST,
*                   EL_GPO_USB1_PWEN,
*                   EL_GPI_USB1_OVC,
*                   EL_GPO__CWORD52__VUP,
*                   EL_GPO_WL_EN,
*                   EL_GPO_BT_EN,
*                   EL_GPO_AVB_RST_N,
*                   EL_GPO_SEC_RST_N,
*                   EL_SPI_NAVI_REQ,
*                   EL_SPI_NAVI_RDY,
*                   EL_GPI_USB0_QVC,
*                   EL_GPO_USB0_PWE,
*                   EL_GPO_GVIFTX1_RST_N,
*                   EL_GPO_GVIFTX0_RST_N,
*                   EL_GPO_EMMC_ON_N,
*                   EL_SPI_GVIF_D_UCS,
*                   EL_GPO__CWORD65__RST_N,
*                   EL_GPO_GVIFRX_RST_N,
*                   EL_GPI_IOEX_INT,
*                   EL_GPO_FBRQ,
*                   EL_GPO_IOEX_RST_N,
*                   EL_GPI_GVIFTX1_GPIO3,
*                   EL_GPI_DARC_INT,
*                   EL_GPI_GVIFTX0_GPIO3,
*                   EL_GPI_DARC_INT,
*                   EL_GPI_GVIFTX0_GPIO3,
*                   EL_GPI_GVIF_REQ_INT,
*                   EL_GPI_ACC_DET_OUT,
*                   EL_GPI_GVIFRX_GPIO3,
*                   EL_GPO_DTCP_RST_N,
*                   EL_NAVI_NMI_N,
*                   EL_GPI_SDPCHK,
*                   EL_GPO_XM_SHDN,
*                   EL_GPI_GVIF_D_REQ_INT,
*                   EL_SPI_GVIF_D_DCS,
*                   EL_GPO_EMMC_PU_ON,
*                   EL_GPIO_GVIFRX_GPIO0,
*                   EL_GPI_GVIFRX_GPIO2,
*                   EL_GPI_GVIFRX_LOS,
*                   EL_GPIO_GVIFTX0_GPIO0,
*                   EL_GPI_GVIFTX0_GPIO2,
*                   EL_GPIO_GVIFTX1_GPIO0,
*                   EL_GPI_GVIFTX1_GPIO2,
*                   EL_GPO_USBHUB_RST,
*                   EL_GPI__CWORD65__BOOT_ERR,
*                   EL_GPO_CKECNT,
*                   EL_GPO_SDPCLR,
*                   EL_GPO_SENSOR_RST,
*                   EL_GPI_SENSOR_INT,
*                   EL_GPO_DAC_MUTE,
*                   EL_GPO_DSRC_PON,
*                   EL_GPI_2PULSE_REV,
*                   EL_GPI_ILL_DET,
*                   EL_GPI_DAB_DET,
*                   EL_GPI_FAN_DET,
*                   EL_GPO_SOC_DIAG_OUT,
*                   EL_GPI_SOC_DIAG_IN,
*                   EL_GPIO_SYMBOL_MAX,
*        };
*            @endcode
* \~english @retval 0 Success
* \~english @retval -1 Error
* \~english @par Prerequisite
*        - Interruption of specified GPIO was registered by \ref EL_Gpio_attachInterruptControl .
* \~english @par Change of internal state
*        - Change of internal state according to the API does not occur.
* \~english @par Conditions of processing failure
*        - The value of symbol is less than 0 or greater than EL_GPIO_SYMBOL_MAX-1. [-1]
*        - syscall error(open(), ioctl()). [-1]
* \~english @par Detail
*        - Waiting interruption of specified GPIO with timeout.
*        - If the value of timeout is not greater than 0, timeout will not happened.
* \~english @par Classification
*        Public
* \~english @par Type
*        Sync
* \~english @see none
*/
int EL_Gpio_waitInterrupt_timeout(int symbol, int timeout);  // NOLINT[readability/naming]

/** \ingroup EL_Gpio_finalize
* \~english @par Brief
*        End processing.
* \~english @retval 0 Return 0
* \~english @par Prerequisite
*        - None.
* \~english @par Change of internal state
*        - Change of internal state according to the API does not occur.
* \~english @par Conditions of processing failure
*        - none
* \~english @par Detail
*        - Change IO-Expander device to reset status.
* \~english @par Classification
*        Public
* \~english @par Type
*        Sync
* \~english @see EL_Gpio_write, EL_Gpio_read
*/
int EL_Gpio_finalize(void);  // NOLINT[readability/naming]

#ifdef __cplusplus
}
#endif  // __cplusplus

/** @}*/  // end of memory_gpio_access_library
/** @}*/  // end of native_service
/** @}*/  // end of BaseSystem
#endif  // MEMORY_GPIO_ACCESS_LIBRARY_CLIENT_INCLUDE_NATIVE_SERVICE_EL_GPIO_H_
