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

#include <native_service/ns_logger_if.h>
#include <native_service/ns_version_if.h>
#include <native_service/ns_version.h>

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

#define ZONE_TEXT_10      "Init"
#define ZONE_TEXT_11      "Function"
#define ZONE_TEXT_12      "Memory"
#define ZONE_TEXT_13      ""
#define ZONE_TEXT_14      ""
#define ZONE_TEXT_15      ""
#define ZONE_TEXT_16      ""
#define ZONE_TEXT_17      ""
#define ZONE_TEXT_18      ""
#define ZONE_TEXT_19      ""
#define ZONE_TEXT_20      ""
#define ZONE_TEXT_21      ""
#define ZONE_TEXT_22      ""
#define ZONE_TEXT_23      ""
#define ZONE_TEXT_24      ""
#define ZONE_TEXT_25      ""
#define ZONE_TEXT_26      ""
#define ZONE_TEXT_27      ""
#define ZONE_TEXT_28      ""
#define ZONE_TEXT_29      "Info"
#define ZONE_TEXT_30      "Warning"
#define ZONE_TEXT_31      "Error"

_CWORD78_LOGPARAM g__CWORD78_LogParams = {
  _CWORD78_LOGOPTIONS,
  {
    ZONE_TEXT_10, ZONE_TEXT_11, ZONE_TEXT_12,
    ZONE_TEXT_13, ZONE_TEXT_14, ZONE_TEXT_15,
    ZONE_TEXT_16, ZONE_TEXT_17, ZONE_TEXT_18,
    ZONE_TEXT_19, ZONE_TEXT_20, ZONE_TEXT_21,
    ZONE_TEXT_22, ZONE_TEXT_23, ZONE_TEXT_24,
    ZONE_TEXT_25, ZONE_TEXT_26, ZONE_TEXT_27,
    ZONE_TEXT_28, ZONE_TEXT_29, ZONE_TEXT_30,
    ZONE_TEXT_31
  },
  _CWORD78_LOGZONES
};

C_CWORD78_Version g__CWORD78_Version(MAJORNO, MINORNO, REVISION);

static void
Usage(char *exec) {
  fprintf(stderr, "Usage: %s options\n", exec != 0 ? exec : NULL);
  fprintf(stderr, "  options\n");
  fprintf(stderr, "    -0 : REALTIMELOG_MODE_OFF\n");
  fprintf(stderr, "    -1 : REALTIMELOG_MODE_UART\n");
  fprintf(stderr, "    -2 : REALTIMELOG_MODE_USB\n");
  fprintf(stderr, "    -3 : REALTIMELOG_MODE_ETHER\n");
  fprintf(stderr, "  (none) print current status\n");
}

int
main(int argc, char **argv) {
  int c;

  if (argc < 2) {
    UI_8 realtimelog;
    NsLogGetRealtimeLog(&realtimelog);
    fprintf(stderr, "current status : -%d\n", realtimelog);
    fprintf(stderr, " refer '%s -h'\n", argv[0] != 0 ? argv[0] : NULL);
    return 0;
  }

  while ((c = getopt(argc, argv, "0123h")) != -1) {
    switch (c) {
      case '0':
        fprintf(stderr, "set REALTIMELOG_MODE_OFF\n");
        NsLogSetRealtimeLog(_CWORD78_LOG_REALTIMELOG_MODE_OFF);
        return 0;
      case '1':
        fprintf(stderr, "set REALTIMELOG_MODE_UART\n");
        NsLogSetRealtimeLog(_CWORD78_LOG_REALTIMELOG_MODE_UART);
        return 0;
      case '2':
        fprintf(stderr, "set REALTIMELOG_MODE_USB\n");
        NsLogSetRealtimeLog(_CWORD78_LOG_REALTIMELOG_MODE_USB);
        return 0;
      case '3':
        fprintf(stderr, "set REALTIMELOG_MODE_ETHER\n");
        NsLogSetRealtimeLog(_CWORD78_LOG_REALTIMELOG_MODE_ETHER);
        return 0;
      default:
        Usage(argv[0]);
        return 0;
    }
  }
  Usage(argv[0]);
  return 0;
}
