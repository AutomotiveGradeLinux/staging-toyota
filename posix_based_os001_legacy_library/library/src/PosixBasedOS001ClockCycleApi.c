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

#include <other_service/PosixBasedOS001ClockCycleApi.h>
#include <time.h>

/******************************************************************************/
/*
Overview	:GetTickCount() like Win32 API
Arguments	:void
Return		:OK:usec  NG:0
Note		:
*/
static uint64_t GetTickCount(void )
{
	struct timespec tp;
	/*--------------------------------------------------------------------*/

	if(clock_gettime(CLOCK_MONOTONIC, &tp) != 0)  // LCOV_EXCL_BR_LINE 5: fail safe for libc clock_gettime
	{
		return 0;
	}

	return (uint64_t)(tp.tv_sec * 1000000 + tp.tv_nsec / 1000);
}


/******************************************************************************/
/*
Overview	:Get the number of clock cycles
Arguments	:void
Return		:OK:usec  NG:0
Note		:
*/
uint64_t ClockCycle( void )
{
	return GetTickCount();
}

/******************************************************************************/
/* End of souce code.*/
/******************************************************************************/

