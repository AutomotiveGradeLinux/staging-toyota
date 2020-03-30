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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NS_LogTraceSender
/// \brief    This file supports logging module version identification.
///
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <native_service/frameworkunified_types.h>

#define MOD_VERSION_LENGTH  12

static CHAR s_ModVersion[MOD_VERSION_LENGTH + 1] = "0.0.0";

SI_8 GetModuleVersion (CHAR* pRetBuf, UI_8 nBufLen)
{  // LCOV_EXCL_START 8: dead code
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    UI_8 CopyLen = 0;
    SI_8 RetVal = -1;

    if (NULL != pRetBuf)
    {
        CopyLen = MIN_VAL(nBufLen, MOD_VERSION_LENGTH);
        strncpy(pRetBuf, s_ModVersion, CopyLen);
        pRetBuf[CopyLen] = '\0';
        RetVal = 0;
    }
    return RetVal;
}
// LCOV_EXCL_STOP 8: dead code

