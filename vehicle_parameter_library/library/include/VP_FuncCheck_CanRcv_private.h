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

/* ====================================================================== */
/**
 * @file VP_FuncCheck_CanRcv_private.h
 * @brief Get function existence API internal header file
 * @attention Cannot be included from external functions
 *
 * Exported because it will be inconvenient at MT
 */
/* ====================================================================== */

#ifndef VEHICLE_PARAMETER_LIBRARY_LIBRARY_INCLUDE_VP_FUNCCHECK_CANRCV_PRIVATE_H_
#define VEHICLE_PARAMETER_LIBRARY_LIBRARY_INCLUDE_VP_FUNCCHECK_CANRCV_PRIVATE_H_

/* ====================================================================== */
/* Include public headers                                                 */
/* ====================================================================== */
#include <other_service/VP_FuncCheck_CanRcv.h>

/* ====================================================================== */
/* Internal definitions for debugging                                                     */
/* ====================================================================== */
#ifdef DEBUG_MT
    #include <stdio.h>

    #define MT_STATIC  /**< Static definitions - Remove the static in order to remove the file scope at MT. */

    /**< Debugging output function - Compile the entity at MT */
    #define DEBUG_PRINT(msg)                {printf("%s\n", (msg) ) ;}
    /**< Debugging output function - Compile the entity at MT */
    #define DEBUG_PRINTF(format, msg)        {printf((format), (msg) ) ;}

    /**< Static assertion */
    #define VP_DEBUG_STATIC_ASSERT(expr)    do { int static_assert[ (expr)? 1:-1 ]; }while(0);
#else
    /**< Static definitions. The scope shall be limited by static at the time of releasing.
     * Remove the static in order to remove the file scope at MT.
     */
    #define MT_STATIC static

    /**< Static definitions. The scope shall be limited by static at the time of releasing.
     * Remove the static in order to remove the file scope at MT.
     */
    #define DEBUG_PRINT(msg)

    /**< Static definitions. The scope shall be limited by static at the time of releasing.
     * Remove the static in order to remove the file scope at MT.
     */
    #define DEBUG_PRINTF(format, msg)

    /**< Static definitions. The scope shall be limited by static at the time of releasing.
     * Remove the static in order to remove the file scope at MT.
     */
    #define VP_DEBUG_STATIC_ASSERT(expr)
#endif

/* ====================================================================== */
/* General-purpose type definition - Redifined for internal use           */
/* ====================================================================== */
/**< Internal boolean type - sys/type.h has no type definitions, so defined it here. FCCR: "F"unc"C"heck_"C"an"R"cv */
typedef enum VP_FCCR_BOOL_Tag {
    B_FALSE, /**< FALSE */
    B_TRUE   /**< TRUE */
} VP_FCCR_BOOL;


/* ====================================================================== */
/* Structure                                                                 */
/* ====================================================================== */
typedef struct TABLE_INDEX_INFO_tag {
    char*           code_str; /**< String that can be gotten from vehicle parameters */
    unsigned int    index;    /**< Index for function existence          */
} TABLE_INDEX_INFO;

#endif  // VEHICLE_PARAMETER_LIBRARY_LIBRARY_INCLUDE_VP_FUNCCHECK_CANRCV_PRIVATE_H_
