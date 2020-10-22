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

/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 File name      : _pbMisc.cpp
 System name    : 05 Integration Platform
 Subsystem name : System common functions
 Title          : System API Time-of-day operations related processing group
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

#include <vehicle_service/positioning_base_library.h>
#include "WPF_STD_private.h"

/*
 Declaration of constant data type
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
typedef struct {
    DWORD time_on_january1_1970;    /* Time on January 1, 1970 when it expressed */
    /* with the total second number from January 1, 1601 */
} BTIME_INSTANCE;

/*
 Internal function prototype declaration
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static DWORD    FileTimeToSeonds(FILETIME* p_ft);

/*
 Global Variable Definitions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BTIME_INSTANCE            g_instance;    // NOLINT(readability/nolint)  global class instance

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : MiscInit()
 * ABSTRACT  : Time-related processing instance initialization processing
 * NOTE      : Assign and hold 1/1/1970 to FILETIME variables
 * ARGUMENT  : None
 * RETURN    : RET_API        RET_NORMAL        Normal completion Note: Always this value
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
MiscInit(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BTIME_INSTANCE    *p_inst = &g_instance;
    SYSTEMTIME        st;
    FILETIME          ft;
    BOOL              bret;

    /* Set the system time to the default value (January 1, 1970, 00:00:00). */
    st.wYear            = 1970;
    st.wMonth           = 1;
    st.wDayOfWeek       = 0;
    st.wDay             = 1;
    st.wHour            = 0;
    st.wMinute          = 0;
    st.wSecond          = 0;
    st.wMilliseconds    = 0;

    /* Converting System Time to File Time */
    bret = PbSystemTimeToFileTime(&st, &ft);
    if (bret != TRUE) {
        /* If the conversion fails, */
        p_inst->time_on_january1_1970 = 0;                        /* Save to instance with zero elapsed seconds */
    } else {
        /* If the conversion is successful, */
        p_inst->time_on_january1_1970 = FileTimeToSeonds(&ft);    /* Save File Time Elapsed Seconds to Instance */
    }

    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : FileTimeToSeonds()
 * ABSTRACT  : File Time -> Time Conversion Processing
 * NOTE      : Convert the file time to seconds
 * ARGUMENT  : FILETIME        *p_ft        Pointer to the file time structure
 * RETURN    : DWORD        Elapsing time(In seconds)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static DWORD
FileTimeToSeonds(FILETIME* p_ft) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return 0;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : MiscTerm()
 * ABSTRACT  : Time-related processing instance release processing
 * NOTE      :
 * ARGUMENT  : None
 * RETURN    : RET_API        RET_NORMAL        Normal completion Note: Always this value
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
MiscTerm(void) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : Sleep()
 * ABSTRACT  : Sleep Processing
 * NOTE      : Cause the caller to sleep at the number of ticks specified by the argument
 * ARGUMENT  : u_int32        ticks        Sleep time(Specify in ticks)
 *           :                0            Sleep permanently
 *           :                1            Discard the time slice
 * RETURN    : RET_API        Normal completion Note: Always this value
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API
Sleep(u_int32 ticks) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    DWORD        milliseconds;

    switch (ticks) {
        case 0:            /* infinite delay. */
        {
            milliseconds = INFINITE;
            break;
        }
        case 1:            /* the thread to relinquish the remainder of its time slice */
            /* to any other thread of equal priority that is ready to run. */
            /* If there are no other threads of equal priority ready to run, */
            /* the function returns immediately, and the thread continues execution. */
        {
            milliseconds = 0;
            break;
        }
        default:        /* Time tranrate from per 10ms tick count to per milli second. */
            milliseconds = (DWORD)ticks * 10;
            break;
    }
    PbMilliSecSleep(static_cast<u_int32>(milliseconds));

    return(RET_NORMAL);
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : PbMilliSecSleep()
 * ABSTRACT  : Sleep Processing(Units of ms)
 * NOTE      : Cause the caller to sleep at the number of ticks specified by the argument
 * ARGUMENT  : u_int32        ul_mill_time    Sleep time(Specified in millimeters)
 *           :                0            Discard the time slice
 *           :                INFINITE    Sleep permanently
 * RETURN    : RET_API        Normal completion Note: Always this value
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API PbMilliSecSleep(u_int32 ul_mill_time) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    switch (ul_mill_time) {
        case 0:
        {
            /* Discard the time slice */
            sched_yield();
            break;
        }
        case INFINITE:
        {
            /* Abort processing indefinitely */
            while (1) {
                sleep(INFINITE);
            }
        }
        default:
            /* Sleep for Specified Time */
            usleep(ul_mill_time * 1000);
            break;
    }

    return RET_NORMAL;
}
// LCOV_EXCL_STOP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MODULE    : SecSleep()
 * ABSTRACT  : Sleep Processing(s unit)
 * NOTE      : Cause the caller to sleep at the number of ticks specified by the argument
 * ARGUMENT  : u_int32        ul_time        Sleep time(Specify in seconds)
 *           :                0            Discard the time slice
 *           :                INFINITE    Sleep permanently
 * RETURN    : RET_API        Normal completion Note: Always this value
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RET_API SecSleep(u_int32 ul_time) {  // LCOV_EXCL_START 8:dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    switch (ul_time) {
        case 0:
        {
            /* Discard the time slice */
            sched_yield();
            break;
        }
        case INFINITE:
        {
            /* Abort processing indefinitely */
            while (1) {
                sleep(INFINITE);
            }
        }
        default:
            /* Sleep for Specified Time */
            sleep(ul_time);
            break;
    }

    return RET_NORMAL;
}
// LCOV_EXCL_STOP

/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 End of File : _sysMisc.cpp
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/
