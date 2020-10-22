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
#ifndef POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_VEHICLE_SERVICE_STD_TYPES_H_
#define POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_VEHICLE_SERVICE_STD_TYPES_H_

/**
 * @file std_types.h
 * @brief Header file for base type definitions
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup positioning_base_library
 *  @ingroup vehicle_service
 *  @{
 */

typedef signed char        int8;
typedef unsigned char      u_int8;
typedef signed short int   int16;
typedef unsigned short int u_int16;
typedef signed int         int32;
typedef unsigned int       u_int32;
typedef float              float_t;
typedef double             double_t;

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif
/** @}*/  // end of positioning_base_library
/** @}*/  // end of vehicle_service
/** @}*/  // end of BaseSystem
#endif  // POSITIONING_BASE_LIBRARY_LIBRARY_INCLUDE_VEHICLE_SERVICE_STD_TYPES_H_
