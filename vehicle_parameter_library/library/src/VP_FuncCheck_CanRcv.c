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
 * @file  VP_FuncCheck_CanRcv.c
 * @brief Get function existence API
 */


/* ====================================================================== */
/* Header                                                                 */
/* ====================================================================== */
#include <assert.h>
#include <string.h>
#include <regex.h>

#include <other_service/VP_GetEnv.h>
#include "VP_FuncCheck_CanRcv_private.h"

/* ====================================================================== */
/* Enumeration types                                                      */
/* ====================================================================== */
typedef enum VPFuncTableDestIndexTag{
    /*
     *  Note.
     *  This feature needs to be defined by the vendor.
     */
    VP_FUNC_TABLE_DEST_INDEX_A     ,
    VP_FUNC_TABLE_DEST_INDEX_B     ,

    VP_FUNC_TABLE_DEST_INDEX_MAX         /**< Number of choices (index maximum value: 1) */
} VP_FUNC_TABLE_DEST_INDEX;

typedef enum VPFuncTableBrandTag{
    /*
     *  Note.
     *  This feature needs to be defined by the vendor.
     */
    VP_FUNC_TABLE_BRAND_A,
    VP_FUNC_TABLE_BRAND_B,

    VP_FUNC_TABLE_BRAND_INDEX_MAX        /**< Number of choices (index maximum value: 1) */
} VP_FUNC_TABLE_BRAND;

typedef enum VPFuncTableGradeLTag{
    /*
     *  Note.
     *  This feature needs to be defined by the vendor.
     */
    VP_FUNC_TABLE_GRADE_A1,
    VP_FUNC_TABLE_GRADE_A2,

    VP_FUNC_TABLE_GRADE_A_INDEX_MAX      /**< Number of choices (index maximum value: 1) */
} VP_FUNC_TABLE_GRADE_A;

typedef enum VPFuncTableGradeTTag{
    /*
     *  Note.
     *  This feature needs to be defined by the vendor.
     */
    VP_FUNC_TABLE_GRADE_B1,
    VP_FUNC_TABLE_GRADE_B2,

    VP_FUNC_TABLE_GRADE_B_INDEX_MAX      /**< Number of choices (index maximum value: 1) */
} VP_FUNC_TABLE_GRADE_B;

typedef enum VPFuncTableFuncTypeIndexTag{
    VP_FUNC_TABLE_FUNC_TYPE_INDEX_UNKNOWN              , /**< Unknown function                           */
    VP_FUNC_TABLE_FUNC_TYPE_INDEX_XM_AUDIO             , /**< XM Audio                                   */
    VP_FUNC_TABLE_FUNC_TYPE_INDEX_VP_CANRCV__CWORD27_  , /**< _CWORD27_                                  */
    VP_FUNC_TABLE_FUNC_TYPE_INDEX_WIFI_HOTSPOT         , /**< Wifi Hotspot                               */
    VP_FUNC_TABLE_FUNC_TYPE_INDEX_RESTRICT_FUNCTION    , /**< Travel Restriction                         */
    VP_FUNC_TABLE_FUNC_TYPE_INDEX_HD_DATA              , /**< HD Data                                    */

    VP_FUNC_TABLE_FUNC_TYPE_INDEX_MAX                    /**< Number of choices (index maximum value: 1) */
} VP_FUNC_TABLE_FUNC_TYPE;

/** Processing definitions for CAN information availability */
typedef enum CANFuncCheckSwitchTag{
    CAN_FUNC_CHEK_AVAILABLE   ,
    CAN_FUNC_CHEK_UNAVAILABLE ,
    CAN_FUNC_CHEK_COUNTRY_CODE
} CAN_FUNC_CHEK_SWITCH;



/**< Definition of number of functions (required for initial definition of function availability table) */

#define VP_FUNC_TABLE_FUNCTION_INDEX_MAX      ((unsigned int)6)

/* ====================================================================== */
/* Macro definitions                                                      */
/* ====================================================================== */
/* Definition for return value from string functions */
#define RET_STD_LIB_STRING_STR_CMP_OK ((int)0) /**< The return value of the strncmp. this value is returned when matched. */

/* Default index values */
#define DEFAULT_TABLE_INDEX_DEST        (VP_FUNC_TABLE_DEST_INDEX_OTHER) /* Initial value changed by the specification which returns the setting of the belonging property when the undefined country code is gotten. */
#define DEFAULT_TABLE_INDEX_BRAND       (VP_FUNC_TABLE_BRAND_A)
#define DEFAULT_TABLE_INDEX_GRADE_A     (VP_FUNC_TABLE_GRADE_A1)
#define DEFAULT_TABLE_INDEX_GRADE_B     (VP_FUNC_TABLE_GRADE_B1)
#define DEFAULT_CAN_JUDGE_STATUS        (CAN_FUNC_CHEK_COUNTRY_CODE)

/* ====================================================================== */
/* Constant definitions                                                   */
/* ====================================================================== */

/* ====================================================================== */
/* Structures                                                             */
/* ====================================================================== */

/* ====================================================================== */
/* Internal function prototype declarations                               */
/* ====================================================================== */
#ifdef VP_FUNCCHECK_CANRCV_CANRCV_ACTIVATE
MT_STATIC u_int8_t     getResult_CanJudge( unsigned int idx_func     );
MT_STATIC unsigned int getIndexOfFunc(     u_int8_t     func         );
MT_STATIC unsigned int getIndexOfTable(    const char   vp_param_str[]  , const TABLE_INDEX_INFO  table_index_info[]           ,  unsigned int  info_size,  unsigned int  default_index );
MT_STATIC VP_FCCR_BOOL hasMultipleCoutnry( const char   dest_str[]      , const char              multiple_country_code_desig[] );
#endif
/* ====================================================================== */
/* Public function definitions                                            */
/* ====================================================================== */
/**
 * @fn
 * u_int8_t VP_FuncCheck_CanRcv( u_int8_t FunctionName )
 * @brief Get function existence API(Public)
 * @param[in]  (FunctionName)  Function Name (Number)
 * @return FUNCTION_CHECK_RESULT_OK      :This function is available
 *         FUNCTION_CHECK_RESULT_NG      :This function is unavailable
 *         FUNCTION_CHECK_RESULT_UNKNOWN :This function does not exist
 * @details  Read the function name as feature name specified in the argument, then return one of the 
 *           "This function is available"/"This function is unavailable"/"This function does not exist"
 *           of the feature as the following check-result:
 *             - vehicle parameter:CAN information availability(destination package) judge
 *             - vehicle parameter:country code
 *             - Whether or not supported the features corresponding to the country code in this API
 *           
 *           When CAN information availability (destination package) in vehicle parameters is
 *           "Control ON/OFF according to CAN information" or "Country code", return "This function is available"
 *           if this function is called when the country code from CAN is unacquired or undetermined country code.
 */
/* ====================================================================== */
u_int8_t VP_FuncCheck_CanRcv( u_int8_t FunctionName )
{
#ifndef VP_FUNCCHECK_CANRCV_CANRCV_ACTIVATE
        u_int8_t        result;

        switch(FunctionName) {
            case XM_AUDIO:
                result = FUNCTION_CHECK_RESULT_NG;
                break;

            case VP_CANRCV__CWORD27_:
            case WIFI_HOTSPOT:
            case AREA1_STD_TRAF_LIMIT:
            case HD_DATA:
                result = FUNCTION_CHECK_RESULT_OK;
                break;
            default:
                result = FUNCTION_CHECK_RESULT_UNKNOWN;
                break;
        }

        return result;
#else

    /* ====================================================================== */
    /* Internal table definition                                              */
    /* ====================================================================== */
    /** Vehicle Parameter "CAN information Support/Not Support"- processing selector translation table */
    const TABLE_INDEX_INFO TABLE_STATUS_INFO_CAN_JUDGE[] =
    {
        { "available"   ,  CAN_FUNC_CHEK_AVAILABLE    }, // Yes
        { "unavailable" ,  CAN_FUNC_CHEK_UNAVAILABLE  }, // None
        { "CAN_judge"   ,  CAN_FUNC_CHEK_COUNTRY_CODE }, // Control ON/OFF according to CAN information
        { "country_ID"  ,  CAN_FUNC_CHEK_COUNTRY_CODE }  // Country code
    };

    /* ====================================================================== */
    /* Table Size Definition                                                  */
    /* ====================================================================== */
    const unsigned int LENGTHOF_TABLE_INDEX_INFO_CAN_JUDGE = ( sizeof(TABLE_STATUS_INFO_CAN_JUDGE ) / sizeof( TABLE_INDEX_INFO ) );

    /* ====================================================================== */
    /* Internal variable declarations                                         */
    /* ====================================================================== */
    u_int8_t result = FUNCTION_CHECK_RESULT_UNKNOWN;

    unsigned int idx_func  = 0;

    unsigned int can_judge_status  = CAN_FUNC_CHEK_COUNTRY_CODE;
    char VP_canjudge[ VP_MAX_LENGTH ];
    char VP_area[     VP_MAX_LENGTH ];

    /* ====================================================================== */
    /* Process                                                                */
    /* ====================================================================== */
    /* Get vehicle parameter */
    VP_GetEnv( VEHICLEPARAMETERLIBRARY_AREA , &(VP_area[0]) );

    DEBUG_PRINTF( "AREA     =  %d; " , VP_area )

    /* Get feature name index */
    idx_func = getIndexOfFunc( FunctionName );

    DEBUG_PRINTF( "Function = %d; " , FunctionName )
    DEBUG_PRINTF( "idx_func = %d\n" , idx_func     )

    /* Judge as other than area1 because it is not a area1 */
    if ( strncmp( VP_area, "AREA1", VP_MAX_LENGTH ) != RET_STD_LIB_STRING_STR_CMP_OK )
    {
        result = FUNCTION_CHECK_RESULT_OUT_OF_AREA1;
    }
    else
    {
        /* If the feature is unknown, it is judged to be unknown before checking CAN information. */
        if ( VP_FUNC_TABLE_FUNC_TYPE_INDEX_UNKNOWN == idx_func )
        {
            result = FUNCTION_CHECK_RESULT_UNKNOWN;
        }
        else
        {
            /* Get vehicle parameter */ /* It is unnecessary to get the parameter every time, but because there is no process to check the change, it is gotten every time as a precaution. */
            VP_GetEnv( VP__CWORD31__TELEMATICS_FUNCTION , &(VP_canjudge[0]) );

            /* Choose processing according to CAN information Support/Not Support. */
            can_judge_status =  getIndexOfTable(
                            &(VP_canjudge[0]),
                            &(TABLE_STATUS_INFO_CAN_JUDGE[0]),
                            LENGTHOF_TABLE_INDEX_INFO_CAN_JUDGE,
                            DEFAULT_CAN_JUDGE_STATUS
                        );

            DEBUG_PRINTF( "VP_canjudge = %s; ", VP_canjudge )

            /* When judging from CAN information or country code */
            if ( CAN_FUNC_CHEK_COUNTRY_CODE == can_judge_status )
            {
                DEBUG_PRINT( "CAN judge : C_CODE or CAN_JUDGE" )
                result = getResult_CanJudge( idx_func );
            }
            else
            {
                /* For available/unavailable */
                /* Standard travel restriction of AREA1 */
                if ( AREA1_STD_TRAF_LIMIT == idx_func )
                {
                    result = FUNCTION_CHECK_RESULT_OK;
                }
                else
                {
                    switch( can_judge_status )
                    {
                        case CAN_FUNC_CHEK_UNAVAILABLE:
                            DEBUG_PRINT( "CAN judge : UNAVAILABLE" )
                            result = FUNCTION_CHECK_RESULT_NG;
                            break;

                        case CAN_FUNC_CHEK_AVAILABLE:
                        default: /* If the default value of canjudge_status are set correctly, the default cannot be selected */
                            DEBUG_PRINT( "CAN judge : AVAILABLE" )
                            result = FUNCTION_CHECK_RESULT_OK;
                            break;
                    }
                }
            }
        }
    }
    DEBUG_PRINTF( "RESULT = %d\n", result )

    return result;

#endif
}

/* ====================================================================== */
/* Internal function definitions                                          */
/* ====================================================================== */
#ifdef VP_FUNCCHECK_CANRCV_CANRCV_ACTIVATE
/* ====================================================================== */
/**
 * @brief      Get function existence in consideration of country code
 * @detials    Internal-use functions - Processing when CAN information availability is "Country Code"
 * @param[in]  idx_func Table index corresponding to the feature (takes as its argument the value after conversion to a table index, not the feature name)
 * @return     Availability of the feature in current vehicle parameters
 */
/* ====================================================================== */
MT_STATIC u_int8_t getResult_CanJudge( unsigned int idx_func )
{
    /* ====================================================================== */
    /* Internal constant definition                                           */
    /* ====================================================================== */
    static const char MULTIPLE_COUNTRY_CODE_DESIG[] = "^[0-9](;[0-9]);{0,1}"; /**< Regular Expression String for Multiple Countries (Attributes) */

    /* ====================================================================== */
    /* Internal table definitions                                             */
    /* ====================================================================== */
    /** Feature table index conversion table (country code) */
    const TABLE_INDEX_INFO TABLE_INDEX_INFO_DESTINATION[] =
    {
        /*
         *  Note.
         *  This feature needs to be defined by the vendor.
         */
        { "101" ,  VP_FUNC_TABLE_DEST_INDEX_A     }, // Detination A
        { "102" ,  VP_FUNC_TABLE_DEST_INDEX_B     }  // Detination B
    };

    /** @todo The consideration. For the feature table by country code, if it can be summarized the GRADE A1/GRADE A2 and GRADE B1/GRADE B2,
              It can be also summarized the translation table.
              However, it must be able to guarantee that there are no differences in the table format in L/T. */
    /** Index Ttranslation Table for feature availability table (for grade A) */
    const TABLE_INDEX_INFO TABLE_INDEX_INFO_GRADE_A[] =
    {
        /*
         *  Note.
         *  This feature needs to be defined by the vendor.
         */
        { "A1" ,    VP_FUNC_TABLE_GRADE_A1 }, // GRADE A1
        { "A2" ,    VP_FUNC_TABLE_GRADE_A2 }  // GRADE A2
    };

    /** Index Translation Table for feature availability table (for grade B) */
    const TABLE_INDEX_INFO TABLE_INDEX_INFO_GRADE_B[] =
    {
        /*
         *  Note.
         *  This feature needs to be defined by the vendor.
         */
        { "B1" ,    VP_FUNC_TABLE_GRADE_B1 }, // GRADE B1
        { "B2" ,    VP_FUNC_TABLE_GRADE_B2 }, // GRADE B2
    };

    /** Index Translation Table for feature availability table (brand) */
    const TABLE_INDEX_INFO TABLE_INDEX_INFO_BRAND[] =
    {
        /*
         *  Note.
         *  This feature needs to be defined by the vendor.
         */
        { "A" ,  VP_FUNC_TABLE_BRAND_A }, // BRAND A
        { "B" ,  VP_FUNC_TABLE_BRAND_B }  // BRAND B
    };

    /* ====================================================================== */
    /**
     * @brief   Feature availability table for BRAND A
     * @details Array of type u_int8_t accessed by the following index
     */
    /* ====================================================================== */
    const u_int8_t VP_FUNC_CHECK_CANRCV_TABLE_A[ VP_FUNC_TABLE_DEST_INDEX_MAX     ]
                                               [ VP_FUNC_TABLE_GRADE_A_INDEX_MAX  ]
                                               [ VP_FUNC_TABLE_FUNCTION_INDEX_MAX ] =
    {
        /*
         *  Note.
         *  This feature needs to be defined by the vendor.
         */
        /* Destination A */
        {
            // Unknown                     ,  XM Audio                ,  _CWORD27_               ,  Wifi HotSpot            ,  Travel Restriction      ,  HD Data
            { FUNCTION_CHECK_RESULT_UNKNOWN,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK }, // GRADE A1
            { FUNCTION_CHECK_RESULT_UNKNOWN,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_NG }  // GRADE A2
        },

        /* Destination B */
        {
            { FUNCTION_CHECK_RESULT_UNKNOWN,  FUNCTION_CHECK_RESULT_NG,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK }, // GRADE A1
            { FUNCTION_CHECK_RESULT_UNKNOWN,  FUNCTION_CHECK_RESULT_NG,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_NG }  // GRADE A2
        }
    };

    /* ====================================================================== */
    /**
     * @brief  Feature availability table for BRAND B
     */
    /* ====================================================================== */
    const u_int8_t VP_FUNC_CHECK_CANRCV_TABLE_B[ VP_FUNC_TABLE_DEST_INDEX_MAX     ]
                                               [ VP_FUNC_TABLE_GRADE_B_INDEX_MAX  ]
                                               [ VP_FUNC_TABLE_FUNCTION_INDEX_MAX ] =
    {
        /*
         *  Note.
         *  This feature needs to be defined by the vendor.
         */
        /* Destination A */
        {
            // Unknown                     ,  XM Audio                ,  _CWORD27_               ,  Wifi HotSpot            ,  Travel Restriction      ,  HD Data
            { FUNCTION_CHECK_RESULT_UNKNOWN,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK }, // GRADE B1
            { FUNCTION_CHECK_RESULT_UNKNOWN,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_NG }, // GRADE B2
        },

        /* Detination B */
        {
            { FUNCTION_CHECK_RESULT_UNKNOWN,  FUNCTION_CHECK_RESULT_NG,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK }, // GRADE B1
            { FUNCTION_CHECK_RESULT_UNKNOWN,  FUNCTION_CHECK_RESULT_NG,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_OK,  FUNCTION_CHECK_RESULT_NG }, // GRADE B2
        }
    };

    /* ====================================================================== */
    /* Table Size Definitions                                                 */
    /* ====================================================================== */
    const unsigned int SIZE_TABLE_INDEX_INFO_BRAND       = ( sizeof(TABLE_INDEX_INFO_BRAND      ) / sizeof( TABLE_INDEX_INFO ) );
    const unsigned int SIZE_TABLE_INDEX_INFO_GRADE_A     = ( sizeof(TABLE_INDEX_INFO_GRADE_A    ) / sizeof( TABLE_INDEX_INFO ) );
    const unsigned int SIZE_TABLE_INDEX_INFO_GRADE_B     = ( sizeof(TABLE_INDEX_INFO_GRADE_B    ) / sizeof( TABLE_INDEX_INFO ) );
    const unsigned int SIZE_TABLE_INDEX_INFO_DESTINATION = ( sizeof(TABLE_INDEX_INFO_DESTINATION) / sizeof( TABLE_INDEX_INFO ) );

    /* ====================================================================== */
    /* Variable declarations                                                  */
    /* ====================================================================== */
    u_int8_t result = FUNCTION_CHECK_RESULT_UNKNOWN;

    VP_FCCR_BOOL isMultipleCountry = B_FALSE;

    unsigned int idx_dest  = 0;
    unsigned int idx_brand = 0;
    unsigned int idx_grade = 0;

    char VP_destination[ VP_MAX_LENGTH ];
    char VP_brand[       VP_MAX_LENGTH ];
    char VP_grade[       VP_MAX_LENGTH ];

    /* ====================================================================== */
    /* Static assertion                                                       */
    /* ====================================================================== */
    /* The following are expected to be identical */
    VP_DEBUG_STATIC_ASSERT( SIZE_TABLE_INDEX_INFO_BRAND   == VP_FUNC_TABLE_BRAND_INDEX_MAX   )
    VP_DEBUG_STATIC_ASSERT( SIZE_TABLE_INDEX_INFO_GRADE_A == VP_FUNC_TABLE_GRADE_A_INDEX_MAX )

    /* Country code does not match because there are multiple countries. */
    VP_DEBUG_STATIC_ASSERT( VP_FUNC_TABLE_DEST_INDEX_MAX > SIZE_TABLE_INDEX_INFO_DESTINATION )

    /* ====================================================================== */
    /* Process                                                                */
    /* ====================================================================== */

    /* Argument check */
    if ( VP_FUNC_TABLE_FUNC_TYPE_INDEX_MAX <= idx_func )
    {
        DEBUG_PRINTF( "ERROR : tebale index overed idx_func = %d\n", idx_func )

        // Return unknown if the table size is exceeded. (However, this function can be called only from the API at the moment.)
        result = FUNCTION_CHECK_RESULT_UNKNOWN;
    }
    else
    {

        /* Get vehicle parameter */
        VP_GetEnv( VP__CWORD31__DESTINATION, &(VP_destination[0]) );
        VP_GetEnv( VEHICLEPARAMETERLIBRARY_BRAND           , &(VP_brand[0])       );
        VP_GetEnv( VP_VEHICLEPARAMETERLIBRARY_GRADE        , &(VP_grade[0])       );

        DEBUG_PRINTF( "VP_destination = %s\n", VP_destination )

        /* Append NULL at the end just in case. (so as not to cause any problems using the strlen or regcomp later) */
        VP_destination[ VP_MAX_LENGTH - 1 ] = '\0';
        VP_brand[       VP_MAX_LENGTH - 1 ] = '\0';
        VP_grade[       VP_MAX_LENGTH - 1 ] = '\0';

        /* Check if the country code is NULL */
        if ( VP_destination[0] == '\0' )
        {
            /* If NULL, set the index for the country code of the US. */
            idx_dest = VP_FUNC_TABLE_DEST_INDEX_A;
            DEBUG_PRINTF( "[debug]idx_dest = %d\n", idx_dest )
        }
        else
        {
            /* Check whether it is a member (multiple countries) */
            isMultipleCountry = hasMultipleCoutnry( &(VP_destination[0]), &(MULTIPLE_COUNTRY_CODE_DESIG[0]) );

            /* Get the index for a country code */
            if ( B_TRUE == isMultipleCountry )
            {
                /* If it is dependent (multiple countries), set the index for the country code of the dependent */
                DEBUG_PRINT( "isMultipleCountry = TRUE" )

                idx_dest = VP_FUNC_TABLE_DEST_INDEX_B;
            }
            else
            {
                /* If it is not dependent (multiple countries), it searches the translation table and sets the index. */
                DEBUG_PRINT( "isMultipleCountry = FALSE" )

                idx_dest = getIndexOfTable( &(VP_destination[0]), &(TABLE_INDEX_INFO_DESTINATION[0]), SIZE_TABLE_INDEX_INFO_DESTINATION, DEFAULT_TABLE_INDEX_DEST );
            }
        }
        DEBUG_PRINTF( "idx_dest = %d\n", idx_dest )

        /* Get brand index */
        idx_brand = getIndexOfTable(    &(VP_brand[0])      , &(TABLE_INDEX_INFO_BRAND[0])      , SIZE_TABLE_INDEX_INFO_BRAND      , DEFAULT_TABLE_INDEX_BRAND );

        /*
         * Get grade index
         * Setting Defaults for each brand
         * e.g. The default is A, so if it is not B, it is determined to be A.
         */
        if ( VP_FUNC_TABLE_BRAND_B == idx_brand )
        {
            DEBUG_PRINT( "idx_brand = B")

            idx_grade = getIndexOfTable( &(VP_grade[0])     , &(TABLE_INDEX_INFO_GRADE_B[0])    , SIZE_TABLE_INDEX_INFO_GRADE_B    , DEFAULT_TABLE_INDEX_GRADE_B );

            /* Table access */
            result = VP_FUNC_CHECK_CANRCV_TABLE_B[idx_dest][idx_grade][idx_func];

        }
        else
        {
            DEBUG_PRINT( "idx_brand = A")

            idx_grade = getIndexOfTable( &(VP_grade[0])     , &(TABLE_INDEX_INFO_GRADE_A[0])    , SIZE_TABLE_INDEX_INFO_GRADE_A    , DEFAULT_TABLE_INDEX_GRADE_A );

            /* Table access */
            result = VP_FUNC_CHECK_CANRCV_TABLE_A[idx_dest][idx_grade][idx_func];

        }

        //DEBUG_PRINTF( "RESULT = %d\n", result )
    }

    return result;
}


/* ====================================================================== */
/**
 * @brief      Feature-Table Index Translation Function
 * @detials    Internal-use functions. It is translate the feature definition number to an index for the table.
 * @param[in]  Feature name defined by the func define
 * @return     The table index corresponding to the feature. If the given country code is invalid, return the index given by default_idx
 */
 /* ====================================================================== */
MT_STATIC unsigned int getIndexOfFunc(  u_int8_t func )
{
    /* ====================================================================== */
    /* Internal table definitions                                             */
    /* ====================================================================== */
    /** Index translation table for feature table (Features) */
    const VP_FUNC_TABLE_FUNC_TYPE GET_INDEX_OF_FUNC[] = {
        VP_FUNC_TABLE_FUNC_TYPE_INDEX_UNKNOWN              , //#define VP_FUNCCHECK_CANRCV_UNKNOWN ((u_int8_t)0)       /* Unknown Features                             */
        VP_FUNC_TABLE_FUNC_TYPE_INDEX_XM_AUDIO             , //#define XM_AUDIO                    ((u_int8_t)1)       /* XM Audio                                     */
        VP_FUNC_TABLE_FUNC_TYPE_INDEX_VP_CANRCV__CWORD27_  , //#define _CWORD27_                   ((u_int8_t)3)       /* _CWORD27_                                    */
        VP_FUNC_TABLE_FUNC_TYPE_INDEX_WIFI_HOTSPOT         , //#define WIFI_HOTSPOT                ((u_int8_t)4)       /* WiFi HotSpot                                 */
        VP_FUNC_TABLE_FUNC_TYPE_INDEX_RESTRICT_FUNCTION    , //#define AREA1_STD_TRAF_LIMIT        ((u_int8_t)5)       /* Standard Travel Restriction of AREA1         */
        VP_FUNC_TABLE_FUNC_TYPE_INDEX_HD_DATA                //#define HD_DATA                     ((u_int8_t)6)       /* HD Data (Include traffic)                    */

    };


    /* ====================================================================== */
    /* Variable declarations                                                  */
    /* ====================================================================== */
    const unsigned int index_table_size = sizeof( GET_INDEX_OF_FUNC ) / sizeof( VP_FUNC_TABLE_FUNC_TYPE );

    u_int8_t table_index = VP_FUNC_TABLE_FUNC_TYPE_INDEX_UNKNOWN;

    /* ====================================================================== */
    /* Static assertions                                                      */
    /* ====================================================================== */
    /* In the current configuration, the feature's defined value and index match, so the translation table size should also match (remove this assertion if there is no match) */
    VP_DEBUG_STATIC_ASSERT( index_table_size ==  VP_FUNC_TABLE_FUNCTION_INDEX_MAX )

    /* Translation tables or the enumeration pair (VP_FUNC_TABLE_FUNC_TYPE) must also be modified when the definition value changes.
       It may also be changed, added, or deleted following asserts as appropriate according to the defined value. */
    VP_DEBUG_STATIC_ASSERT( XM_AUDIO            == VP_FUNC_TABLE_FUNC_TYPE_INDEX_XM_AUDIO          )

    VP_DEBUG_STATIC_ASSERT( VP_CANRCV__CWORD27_   == VP_FUNC_TABLE_FUNC_TYPE_INDEX_VP_CANRCV__CWORD27_ )
    VP_DEBUG_STATIC_ASSERT( WIFI_HOTSPOT        == VP_FUNC_TABLE_FUNC_TYPE_INDEX_WIFI_HOTSPOT      )
    VP_DEBUG_STATIC_ASSERT( AREA1_STD_TRAF_LIMIT   == VP_FUNC_TABLE_FUNC_TYPE_INDEX_RESTRICT_FUNCTION )
    VP_DEBUG_STATIC_ASSERT( HD_DATA             == VP_FUNC_TABLE_FUNC_TYPE_INDEX_HD_DATA           )


    /* ====================================================================== */
    /* Process                                                                */
    /* ====================================================================== */

    /* Argument check: Treat as unknown feature if it is larger than index translation table */
    if ( index_table_size <= func )
    {
        table_index = VP_FUNC_TABLE_FUNC_TYPE_INDEX_UNKNOWN;
    }
    else
    {
        table_index = (u_int8_t)(GET_INDEX_OF_FUNC[func]);
    }

    return table_index;

}

/* ====================================================================== */
/**
 * @brief      Vehicle parameter string to integer conversion function
 * @detials    Internal-use function. Convert the given string (vehicle parameter) into an index for the table.
 *             The value gotten using VP_GetEnv() is assumed. Thus, the maximum length is VP_MAX_LENGTH.
 *             Return the value set in default_index if the conversion is failed.
 * @param[in]  vp_param_str      String as source of convetion to the table index
 * @param[in]  table_index_info  String corresponding to table index and an array of structs with table indexes set
 * @param[in]  info_size         Size of the table_index_info array
 * @param[in]  default_index     Default index. Return this value if none of table_index_info matches
 * @return     The table index corresponding to the vehicle parameter string given by the argument. Return the value given by the default_index argument if the given country code is invalid.
 */
 /* ====================================================================== */
MT_STATIC unsigned int getIndexOfTable( const char vp_param_str[],  const TABLE_INDEX_INFO table_index_info[], unsigned int info_size, unsigned int  default_index )
{
    unsigned int table_index = 0;

    table_index = default_index; /* Set as initial value so that it can return the set default if it is unknown */

    //DEBUG_PRINTF("info_size = %d\n", info_size)

    /* NULL checking */
    if ( NULL != vp_param_str )
    {
        if ( NULL != table_index_info )
        {
            unsigned int i = 0;

            for ( i = 0; i < info_size; i++ )
            {
                if ( RET_STD_LIB_STRING_STR_CMP_OK == strncmp( &(vp_param_str[0]), &((table_index_info[i]).code_str[0]), VP_MAX_LENGTH ) )
                {
                    table_index = (table_index_info[i]).index;

                    break;
                }
            }
        }
        else
        {
            DEBUG_PRINT( "ERROR!! the Bad Teble overd." )
        }
    }

    return table_index;
}

/* ====================================================================== */
/**
 * @brief      Function to check whether country code is multiple countries or not
 * @detials    Internal-use functions. Check if the country code given in the argument contains more than one country.
 *             The determination whether multiple country code or not shall be true if it exactly matches the specified 
 *             regular expression which is a characteristic of multiple countries.
 *             The exact match determination shall be the exact match if the length of the matched character matches
 *             the length of the character passed in the argument.
 * @param[in]  dest_str                     Country code string. The value gotten using VP_GetEnv() is assumed. Therefore, the maximum length is VP_MAX_LENGTH.
 * @param[in]  multiple_country_code_desig  String for determining multiple country codes. Regular expression strings if a regular expression library is used. Otherwise, a single-character string representing the delimiter.
 *                                          If it gets trapped inside a function, it will have to stub the regcomp during function unit tests, which is very cumbersome and can be removed as arguments.
 * @return     True if country code indicates multiple countries, otherwise false
 */
/* ====================================================================== */
MT_STATIC VP_FCCR_BOOL hasMultipleCoutnry( const char dest_str[], const char multiple_country_code_desig[] )
{
    /* ====================================================================== */
    /* Variable declarations                                                  */
    /* ====================================================================== */
    VP_FCCR_BOOL isMultiple = B_FALSE;

    /* ====================================================================== */
    /* Processing                                                             */
    /* ====================================================================== */

    /* NULL checking for arguments */
    if ( ( NULL != dest_str ) && ( NULL != multiple_country_code_desig ) )
    {
        regex_t preg;

        if ( 0 == regcomp( &preg, multiple_country_code_desig, REG_EXTENDED ) )
        {
            regmatch_t pmatch = { 0, 0 };

            if ( REG_NOMATCH != regexec( &preg, &(dest_str[0]), 1, &pmatch, 0) )
            {
                /** @note Exactly match is considered if the end-of-match offset matches the string length.
                  *       Assumed that it should not be used as the end-of-line character so that "$" is not included in the C base character set.
                  */
                if ( (strnlen( dest_str, VP_MAX_LENGTH ) == (unsigned int)(pmatch.rm_eo) ) )
                {
                    isMultiple = B_TRUE;
                }
                else
                {
                    isMultiple = B_FALSE;
                }

                DEBUG_PRINT( "FOUND" )
            }
            else
            {
                isMultiple = B_FALSE;

                DEBUG_PRINT( "NOT FOUND" )
            }

            regfree( &preg );
        }
        else
        {
            /* In the case of a library error,
             * return false so that the default value that is set by high layer works.
             */

            /** @todo Should output using FRAMEWORKUNIFIEDLOG for library errors? */
            isMultiple = B_FALSE;

            DEBUG_PRINT( "FOUND" )
        }
    }

    return isMultiple;

}
#endif
