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

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGEQUEUE_INCLUDE_NS_MQ_ANOMALY_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGEQUEUE_INCLUDE_NS_MQ_ANOMALY_H_

#include <native_service/frameworkunified_types.h>

typedef struct {
  CHAR name[18];
  UI_32 maxMsg;
} mq_anomaly_t;

mq_anomaly_t mq_anomaly_list[] = {
  { "/NS_NPPService",   256 },
  { "/NS_NPPWriteWkr",  256 },
  { "/NS_NPPReadWkr",   256 },
  { "/NS_NPPIPWkr",     256 },
  { "/audio_app",       256 },
};

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGEQUEUE_INCLUDE_NS_MQ_ANOMALY_H_
