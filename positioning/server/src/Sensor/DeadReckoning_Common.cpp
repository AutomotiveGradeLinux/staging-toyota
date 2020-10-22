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
 *  File name    :DeadReckoning_Common.cpp
 *  System name    :PastModel002
 *  Subsystem name  :DeadReckoning processes
 *  Program name  :DeadReckoning shared process(DEADRECKONING_COMMON)
 *  Module configuration  DeadReckoningMemcmp()        Functions for Common Processing Memory Block Comparisons
 *            DeadReckoningCheckDid()        Common Processing Data ID Check Function
 *            DeadReckoningGetDataMasterOffset()  Get function for common processing data master offset value
 ******************************************************************************/
#include "DeadReckoning_Common.h"
#include <vehicle_service/positioning_base_library.h>

/*************************************************/
/*       Global variable            */
/*************************************************/
static const DEADRECKONING_DID_OFFSET_TBL kGstDidListS[] = {
    /*      Data ID            Offset size    Reserved */
    {  VEHICLE_DID_DR_LONGITUDE,                      DEADRECKONING_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_DR_LATITUDE,                       DEADRECKONING_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_DR_ALTITUDE,                       DEADRECKONING_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_DR_SPEED,                          DEADRECKONING_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_DR_HEADING,                        DEADRECKONING_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_DR_GYRO_OFFSET,                    DEADRECKONING_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_DR_GYRO_SCALE_FACTOR,              DEADRECKONING_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_DR_GYRO_SCALE_FACTOR_LEVEL,        DEADRECKONING_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR,       DEADRECKONING_OFFSET_NORMAL,    {0, 0} },
    {  VEHICLE_DID_DR_SPEED_PULSE_SCALE_FACTOR_LEVEL, DEADRECKONING_OFFSET_NORMAL,    {0, 0} },
    {  0,                        0,                  {0, 0} }    /* Termination code */
};

/*******************************************************************************
* MODULE   : DeadReckoningMemcmp
* ABSTRACT  : Functions for Common Processing Memory Block Comparisons
* FUNCTION  : Memory block comparison processing
* ARGUMENT  : *vp_data1  : Comparison target address 1
*       : *vp_data2  : Comparison target address 2
*       : uc_size  : Comparison Size
* NOTE    :
* RETURN   : DEADRECKONING_EQ    : No data change
*       : DEADRECKONING_NEQ  : Data change
******************************************************************************/
u_int8 DeadReckoningMemcmp(const void *vp_data1, const void *vp_data2, size_t uc_size) {  // LCOV_EXCL_START 8: dead code.  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int8 l_ret = DEADRECKONING_EQ;
    const u_int8 *p_data1 = (const u_int8 *)vp_data1;
    const u_int8 *p_data2 = (const u_int8 *)vp_data1;

    /* Loop by data size */
    while (uc_size > 0) {
        if (*p_data1 != *p_data2) {
            /* Data mismatch */
            l_ret = DEADRECKONING_NEQ;
            break;
        }
        p_data1++;
        p_data2++;
        uc_size--;
    }
    return(l_ret);
}

/*******************************************************************************
* MODULE   : DeadReckoningCheckDid
* ABSTRACT  : Common Processing Data ID Check Function
* FUNCTION  : Check if the specified DID corresponds to the vehicle sensor information
* ARGUMENT  : ul_did  : Data ID
* NOTE    :
* RETURN   : DEADRECKONING_INVALID  :Disabled
*       : DEADRECKONING_EFFECTIVE  :Enabled
******************************************************************************/
int32 DeadReckoningCheckDid(DID ul_did) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32 i = 0;
    int32 l_ret = DEADRECKONING_INVALID;

    while (0 != kGstDidListS[i].ul_did) {
        if (kGstDidListS[i].ul_did == ul_did) {
            /* DID enabled */
            l_ret = DEADRECKONING_EFFECTIVE;
            break;
        }
        i++;
    }
    return(l_ret);
}

/*******************************************************************************
* MODULE   : DeadReckoningGetDataMasterOffset
* ABSTRACT  : Get function for common processing data master offset value
* FUNCTION  : Get the fixed offset value for a given DID
* ARGUMENT  : ul_did  : Data ID
* NOTE    :
* RETURN   : Offset value(Returns 0 if DID is invalid)
*       :
******************************************************************************/
u_int16  DeadReckoningGetDataMasterOffset(DID ul_did) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    int32    i = 0;              /* Generic counters      */
    u_int16    us_ret = 0;            /* Return value of this function    */

    while (0 != kGstDidListS[i].ul_did) {
        if (kGstDidListS[i].ul_did == ul_did) {
            /* DID enabled */
            us_ret = kGstDidListS[i].us_offset;
            break;
        }
        i++;
    }
    return(us_ret);
}
// LCOV_EXCL_STOP
