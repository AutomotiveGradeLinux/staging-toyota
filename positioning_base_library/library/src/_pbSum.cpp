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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 File name      : _pbSum.cpp
 System name    :
 Subsystem name : System common functions
 Title          : System APIs Asynchronous Data HDD Backup Processing
 --------------------------------------------------------------------------------------
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <vehicle_service/positioning_base_library.h>

/*
 Function prototype declarations
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : Sum()
 * ABSTRACT  : SUM calculation process
 * NOTE      : Calculate the SUM value for the specified size from the SUM calculation start address.
 * ARGUMENT  : void     *p        SUM calculation start address
 *             int32    size      Size (bytes)
 *             u_int8   type      Calculation width    Byte        :sizeof(u_char)
 *                                        Word        :sizeof(u_int16)
 *                                        Long-word:sizeof(u_int32)
 * NOTE         : Calculate the SUM value from the SUM calculation start address by the size specified by the calculation width.
 * RETURN    : u_int32            SUM value
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
u_int32 Sum(void *p, int32 size, u_int8 type) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    u_int32        sum = 0;                    /* SUM value            */
    u_int32        loop_cnt;                    /* Loop counter        */
    u_int32        loop_num;                    /* Number of loops            */

    /* Pointer size check */
    if ((p == NULL) || (size == 0)) {
        /* nop */
    } else {
        /* When the calculation width is "Long word" */
        if (type == sizeof(int32)) {
            u_int32 * ptr32 = reinterpret_cast<u_int32 *>(p);        /* Long word pointer    */

            /* Calculate the number of processes and loop for the number of Times to compute the Sum value. */
            loop_num = static_cast<u_int32>(size / sizeof(u_int32));
            for (loop_cnt = 0; loop_cnt < loop_num; loop_cnt++) {
                sum += *ptr32;
                ptr32++;
            }
        } else if (type == sizeof(int16)) {
            /* For "Word" */
            u_int16 * ptr16 = reinterpret_cast<u_int16 *>(p);        /* Word pointer    */

            loop_num = static_cast<u_int32>(size / sizeof(u_int16));
            for (loop_cnt = 0; loop_cnt < loop_num; loop_cnt++) {
                sum += *ptr16;
                ptr16++;
            }
        } else if (type == sizeof(char)) {
            /* For "Byte" */
            u_char * ptr8 = reinterpret_cast<u_char *>(p);        /* Byte pointer    */

            loop_num = static_cast<u_int32>(size / sizeof(u_char));
            for (loop_cnt = 0; loop_cnt < loop_num; loop_cnt++) {
                sum += *ptr8;
                ptr8++;
            }
        } else {
            /* For other than above */
            /* No processing */
        }
    }
    return sum;
}
// LCOV_EXCL_STOP

/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 End of File : Sum.cpp
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

