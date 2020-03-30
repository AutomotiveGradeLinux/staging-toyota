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

/************************************************************************
* File name      : INI_API.h
* System name      : _CWORD107_
* Subsystem name  : Staged Start and Termination Framework
* Title      : Staged Start and End Framework API Header Files
************************************************************************/

#ifndef POSITIONING_CLIENT_INCLUDE_INI_API_H_
#define POSITIONING_CLIENT_INCLUDE_INI_API_H_

#include <vehicle_service/positioning_base_library.h>

/*
*****     DEFINE     *****
*/

#define INI_EXTMEM_DUM            0             /* Message buffer header extension member */

/* Return value */
#define INI_SUCCESS               0             /* Normal */
#define INI_FALSE                 (-1)          /* Fail */
#define INI_ERRNOTRDY             (-2)          /* No destination process(Not wakeup) */

/* Stage start completion notification ID related Note: ACC-OFF process completion notification is also used. */
#define GINI_COMPID_INI           0x00000000    /* Initial value of completion notification ID */
#define GINI_COMPID_MASK          0x80000000    /* Completion notification ID mask value(Startup completion or for event completion determination) */

/* ID for Phase Start Event Completion Notification */

#define INI_INITCOMP_BTCOM_PRT    0x80000001    /* BT Communication Protocol Thread Startup Complete */
#define INI_INITCOMP_BTCOM_MNG    0x80000002    /* BT Communication Protocol Thread Startup Complete */
#define INI_INITCOMP_BTCOM_RCV    0x80000004    /* BT Communication Protocol Thread Startup Complete */
#define INI_INITCOMP_MC_UPCHK     0x80000008    /* To receive the confirmation notice of connection between microcomputers  */

/************************************************************************
  ID for Phase Start Event Completion Notification (For VUP)
 ************************************************************************/
#define INI_FWRDCOMP__CWORD24_      0x80000008    /* _CWORD24_ transmission completed          */
#define INI_FWRDCOMP_VUP          0x80000010    /* VUPLoader transmission completed        */
#define INI_INITCOMP_MFT          0x80000020    /* Module transfer thread start completion  */
#define INI_INITCOMP_UPCHK        0x80000040    /* Receive Start Type Confirmation Notification        */

/* ID for Phase ACC-OFF Event Completion Notification ID */

/* Boot phase */
/*************************************************************************
  Note: Maintenance Precautions*
  Initial only affects the addition and deletion of startup parameters.
  For addition or deletion of shared memory creation,The primary of each process is also affected
*************************************************************************/
/* --->CHG  */
enum IniStep {
    GINI_STEPFORK_INI =  0,             /* Initial state */
    GINI_STEPFORK_COMMEM,               /* Shared Memory Generation */
    GINI_STEPFORK_FIRST,                /* First start */
    GINI_STEPFORK_SECOND,               /* 2nd start */
    GINI_STEPFORK_THIRD,                /* 3rd start */
    GINI_STEPFORK_FOURTH,               /* 4th start */
    GINI_STEPFORK_BUPCHK,               /* Backup Data Check/Initialization */
    GINI_STEPFORK_FIFTH,                /* 5th start */
    GINI_STEPFORK_SIXTH,                /* 6th start */
    GINI_STEPFORK_SEVENTH,              /* 7th start */
    GINI_STEPFORK_EIGHTH,               /* 8th start */
    GINI_STEPFORK_NINTH,                /* 9th start */
    GINI_STEPFORK_TENTH,                /* 10th start */
    GINI_STEPFORK_ELEVENTH,             /* 11th start */
    GINI_STEPFORK_TWELFTH,              /* 12th start */
    GINI_STEPFORK_THIRTEENTH,           /* 13th start */
    GINI_STEPFORK_FOURTEENTH,           /* 14th start */
    GINI_STEPFORK_FIFTEENTH,            /* 15th Start */
    GINI_STEPFORK_SIXTEENTH,            /* 16th start */
    GINI_STEPFORK_SEVENTEENTH,          /* 17th Start */
    GINI_STEPFORK_EIGHTEENTH,           /* 18th Start */
    GINI_STEPFORK_MAX                   /* Total number of startup phases */
};
/* <--- CHG */

/* ACC-OFF phase */
enum AccOffStep {
    GINI_ACCOFF_INI = 0,                /* Early stage(Normal state) */
    GINI_ACCOFF_FIRST,                  /* Stage 1 */
    GINI_ACCOFF_SECOND,                 /* Step 2 */
    GINI_ACCOFF_THIRD,                  /* Step 3 */
    GINI_ACCOFF_FOURTH,                 /* Step 4 */
    GINI_ACCOFF_FIFTH,                  /* Step 5 */
    GINI_ACCOFF_MAX                     /* Total number of ACC-OFF steps */
};

/* Backup data check process type */
enum IniDramBkup {
    INI_BUPCHK_SRAM_INIT = 0,           /* Initialization of SRAM */
    INI_BUPCHK_SRAM_CHK,                /* SRAM range-checking process */
    INI_BUPCHK_USER_INIT,               /* Initialization of personal information */
    INI_BUPCHK_DRAM_INIT                /* Initializing DRAM backups */
};

/* ---> */
/*************************************************
 Startup identification
*************************************************/
enum {
    INI_CND_UNSET = 0,                  /* Startup identification value not set    */
    INI_CND_UNKNOWN,                    /* Startup identification unknown      */
    INI_CND_CLSTART,                    /* COLD START      */
    INI_CND_HTSTART,                    /* HOT START      */
    INI_CND_ERRRESET,                   /* Error Reset      */
    INI_CND_USERCL                      /* User-specified COLD    */
};

/*************************************************
 Startup Security State
*************************************************/
enum {
    INI_ANTITHEFT_UNSET = 0,            /* Not set condition      */
    INI_ANTITHEFT_UNLOCK,               /* Unlocked status    */
    INI_ANTITHEFT_LOCK,                 /* Lock state      */
    INI_ANTITHEFT_INITIAL               /* Initial state(Not used)    */
};

/*************************************************
 Version upgrade completion information
*************************************************/
enum {
    INI_VUP_INFO_UNSET = 0,             /* VUP information not set      */
    INI_VUP_INFO_NORMAL,                /* VUP Information Normal(VUP not executed)  */
    INI_VUP_INFO_ERROR,                 /* VUP information error(VUP incomplete)  */
    INI_VUP_INFO_VUPFIN                 /* Completion of VUP information        */
};

/*************************************************
 +B Disconnect Information
*************************************************/
enum {
    INI_BATTERY_INFO_UNSET = 0,         /* +B Information not set      */
    INI_BATTERY_INFO_NORMAL,            /* +B Information Normal(no disconnected)  */
    INI_BATTERY_INFO_DISCON             /* +B Information with disconnected      */
};

/*************************************************
 User information initialization information
*************************************************/
enum {
    INI_INITUSERDAT_UNSET = 0,          /* User information not set      */
    INI_INITUSERDAT_ON,                 /* Personal Information Initialization Status Setting ON    */
    INI_INITUSERDAT_OFF                 /* Personal information initialization status setting OFF  */
};

/*************************************************
 DRAM back-up status at startup
*************************************************/
enum {
    INI_DRAMBKUP_UNSET = 0,             /* DRAM back-up status not set  */
    INI_DRAMBKUP_OK,                    /* DRAM back-up status OK    */
    INI_DRAMBKUP_NG                     /* DRAM backup status NG    */
};

/******  STRUCT  ******/
/*************************************************
 Startup system information
*************************************************/
typedef struct {
    u_int8                    boot_type;           /* Startup identification          */
    u_int8                    security_sts;        /* Startup Security State    */
    u_int8                    vup_info;            /* Version upgrade completion information  */
    u_int8                    battery_off_info;    /* +B Disconnect  Information        */
    u_int8                    usrdat_init_info;    /* User data initialization information  */
    u_int8                    dram_bup_sts;        /* DRAM back-up status    */
    u_int8                    reserve[2];          /* Reserved          */
    u_int32                   boot_time;           /* Passed Time    */
} T_INI_BOOTINFO;

/* <--- */

/*
*****    STRUCTURE    *****
*/
/******************************************************************************
* TAG        : T_PRIM_FORKCOMP
* ABSTRACT   : Stage-up completion notification structure
* NOTE       : Body portion of messages generated from API (Staged Start Completion Notification)
******************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER    head;     /* Message buffer header    */
    u_int32                   id;       /* ID for start phase completion notification        */
} T_PRIM_FORKCOMP;

/******************************************************************************
* TAG        : T_PRIM_ACCOFFSTART
* ABSTRACT   : Structures for ACC-OFF Process Instructions
* NOTE       : Body of messages generated from API(ACC-OFF process instructions)
******************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER    head;     /* Message buffer header    */
    u_int32                   id;       /* ID for start phase completion notification        */
} T_PRIM_ACCOFFSTART;

/******************************************************************************
* TAG        : T_PRIM_ACCOFFCOMP
* ABSTRACT   : Structures for notifying completion of ACC-OFF process
* NOTE       : Main body of messages generated from API(ACC-OFF process completion notification)
******************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER    head;     /* Message buffer header    */
    u_int32                   id;       /* Identifier for notifying completion of ACC-OFF process      */
} T_PRIM_ACCOFFCOMP;

/******************************************************************************
* TAG        : T_PRIM_INTERNAL_ACCOFFSTART
* ABSTRACT   : Structures for in-function ACC-OFF indication
* NOTE       : Body portion of messages generated from API (notification of completion of ACC-OFF process in function)
******************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER    head;     /* Message buffer header    */
    u_int32                   id;       /* Identifier for notifying completion of ACC-OFF process      */
} T_PRIM_INTERNAL_ACCOFFSTART;

/******************************************************************************
* TAG        : T_PRIM_INTERNAL_ACCOFFCOMP
* ABSTRACT   : Structures for notifying completion of ACC-OFF in functions
* NOTE       : Body portion of messages generated from NOTEAPI (notification of completion of ACC-OFF process in function)
******************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER    head;     /* Message buffer header    */
    u_int32                   id;       /* Identifier for notifying completion of ACC-OFF process      */
} T_PRIM_INTERNAL_ACCOFFCOMP;

/*
*****  PROTOTYPE  *****
*/
#ifdef __cplusplus
extern "C" {
#endif

int32  IniInternalAccOffComp(PNO pno , u_int32 id);
void  IniNotifyInit_CWORD69_Comp(void);
void  IniWaitInit_CWORD69_Comp(void);
BOOL  IniIsInit_CWORD69_Comp(void);

#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_INI_API_H_
