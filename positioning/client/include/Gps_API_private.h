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
 *   Gps_API_private.h
 * @brief
 *   Definition for GPS Internal I/F Liblary
 */
#ifndef POSITIONING_CLIENT_INCLUDE_GPS_API_PRIVATE_H_
#define POSITIONING_CLIENT_INCLUDE_GPS_API_PRIVATE_H_

/*---------------------------------------------------------------------------------*
 * Incluce                                                                         *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/
/*--- for message ---*/

/*---------------------------------------------------------------------------------*
 * Typedef declaration                                                             *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Struct declaration                                                              *
 *---------------------------------------------------------------------------------*/
/*!
  @brief   message delivery GPS reset information
 */
typedef struct {
    uint8_t    mode;          /**< Reset mode  */
    uint8_t    reserve[3];    /**< reserve    */
    PNO        snd_pno;       /**< Caller PNO    */
    PNO        res_pno;       /**< Destination PNO    */
} POS_RESETINFO;

/*---------------------------------------------------------------------------------*
 * Prototype Declaration                                                           *
 *---------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_GPS_API_PRIVATE_H_
