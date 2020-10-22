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

/*****************************************************************************************
*  File name  :GpsInt.h
*  System name  :PastModel002
*  Process name  :Vehicle
*  Overview        :GPS interrupt thread header
******************************************************************************************/
#ifndef  POSITIONING_SERVER_INCLUDE_SENSOR_GPSINT_H_
#define  POSITIONING_SERVER_INCLUDE_SENSOR_GPSINT_H_

/*******************************************************************************
* Include File
*******************************************************************************/

/*******************************************************************************
* Define
*******************************************************************************/
#define  GPS_IRQ_ADR                  0x00000306UL    /* GPS IRQ addresses        */
#define GPS_MASK_6BIT                 0x00000040UL    /* GPS IRQ bit-mask position    */

#define GPS_GPIO0_IRQSTATUS_0_OFFSET          0x0000002CUL    /* IRQSTATUS_0 offset    */
#define GPS_GPIO0_IRQSTATUS_SET_0_OFFSET      0x00000034UL    /* IRQSTATUS_SET_0 offset  */
#define GPS_GPIO0_RISINGDETECT_OFFSET         0x00000148UL    /* RISINGDETECT offsets    */

/* IRQSTATUS_0 address */
#define GPS_GPIO0_IRQSTATUS_0_ADDR            (DM816X_GPIO0_BASE + GPS_GPIO0_IRQSTATUS_0_OFFSET)

/* IRQSTATUS_SET_0 address */
#define GPS_GPIO0_IRQSTATUS_SET_0_ADDR        (DM816X_GPIO0_BASE + GPS_GPIO0_IRQSTATUS_SET_0_OFFSET)

/* RISINGDETECT addresses */
#define GPS_GPIO0_RISINGDETECT_ADDR           (DM816X_GPIO0_BASE + GPS_GPIO0_RISINGDETECT_OFFSET)

/*******************************************************************************
* Struct
*******************************************************************************/
typedef struct {
    DWORD        irq_status_reg;    /* Interrupt Source Clear Register    */
    DWORD        irq_status_set_reg;  /* Interrupt enable register      */
    DWORD        rising_detect_reg;  /* Rising edge detection setting register  */
} GPS_INT_DATA;

/***************************
*  Function prototype declaration
***************************/
RET_API  DEVGpsIntInitial(void);
BOOL  DEVGpsIntMapDevice(void);
void  DEVGpsIntUnMapDevice(void);
void  DEVGpsIntSetInterupt(void);
void  DEVGpsIntIntRegClear(void);
RET_API  DEVGpsIntSndMsg(void);

#endif  // POSITIONING_SERVER_INCLUDE_SENSOR_GPSINT_H_
