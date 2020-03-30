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
 * @file VP_GetEnv.c
 * @brief API to get vehicle parameter environment variable
 */
/* ====================================================================== */


#include <stdlib.h>
#include <string.h>
#include <other_service/VP_GetEnv.h>

/* ====================================================================== */
/**
 * @fn
 * void VP_GetEnv(char *pEnvStr, char *pEnvBuff )
 * @breaf Get Environment Variable
 * @param[in]  (pEnvStr) Pointer to the string of the environment variable name to be gotten
 * @param[out] (pEnvBuff) Start address of the area to store the gotten value of the environment variable
 * @return None
 * @detail Read the value (string) of the environment variable specified by the argument and stores it in the specified address.
 */
/* ====================================================================== */
void VP_GetEnv(const char *pEnvStr, char *pEnvBuff )
{
  char *env_string;
  size_t length;

  if ((pEnvStr != NULL) && (pEnvBuff != NULL)) {
    env_string = getenv(pEnvStr);

    if (env_string == NULL) {
      *pEnvBuff = '\0';
    } else {
      length = strlen( env_string );
      if (length < VP_MAX_LENGTH) {
        (void)strcpy(pEnvBuff, env_string);
      } else {
        (void)strncpy(pEnvBuff, env_string, (VP_MAX_LENGTH - 1));
        pEnvBuff[ VP_MAX_LENGTH - 1 ] = '\0';
      }
    }
  } else {
    if(pEnvBuff != NULL) {
      *pEnvBuff = '\0';
    }
  }
}
