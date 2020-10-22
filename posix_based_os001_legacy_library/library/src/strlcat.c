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

#include <sys/types.h>
#include <string.h>

size_t
strlcat(char *dst, const char *src, size_t size)
{
	size_t dstsize, srcsize, copysize;

	dstsize = strnlen(dst, size);
	srcsize = strlen(src);
	if (dstsize == size) {
		return dstsize + srcsize;
	}

	copysize = size - dstsize - 1;
	if (srcsize < copysize) {
		copysize = srcsize;
	}

	memcpy(dst + dstsize, src, copysize);
	dst[dstsize + copysize] = '\0';

	return dstsize + srcsize;
}
