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

// libraries for replacing PosixBasedOS001 to Linux
// PosixBasedOS001TimeApi.c

#include <other_service/PosixBasedOS001TimeApi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>				// for using usleep()
#include <time.h>				// for using clock()

// delay()
// NOTE: Control a device
// duration
//   The number of milliseconds for which to suspend the calling thread from execution.
//
// _CWORD72_ doesnot use return code of delay, and sets 1,100,and 1000.
unsigned int delay( unsigned int duration ){

	int ret;
	useconds_t duration_usec = duration * 1000;

	ret = EOK;

	if( duration_usec > 1000000 ){
		printf("delay():over flow!\n");
	}else{

		// using usec order
		ret = usleep(duration_usec);

		// error of usleep() = 0(E_OK), -1(ERROR)
		// delay() cannot return error code. [data type unmatch]
		if(ret < 0){  // LCOV_EXCL_BR_LINE 5: fail safe for libc function usleep
			AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
			printf("delay():error!\n");  // LCOV_EXCL_LINE 5: fail safe for libc function usleep
		}
	}

	return EOK;
}
