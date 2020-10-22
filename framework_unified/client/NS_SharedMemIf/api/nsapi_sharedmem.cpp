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

/*************************************************************************
 *
 * FILENAME    : nsapi_sharedmem.cpp
 *
 * DESCRIPTION : TestApp - NS_SharedMem
 *
 ************************************************************************/

#include <native_service/ns_logger_if.h>
#include <native_service/ns_version_if.h>
#include <native_service/ns_sharedmem.h>
#include <native_service/ns_shared_mem_if.h>
#include <native_service/ns_transmit_log.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ss_version.h"
#include "ns_unittest_internal.h"

#define AVAILABLE_API
#if defined(AVAILABLE_API)
static CNSSharedMem *g_pTransmitLogSharedBuf = NULL;

////////////////////////////////////////////////////////////////////////////////////////////
/// NSSharedMemTransmitLogOpen
/// Open the shared memory for transmit logging
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSSharedMemTransmitLogOpen() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL == g_pTransmitLogSharedBuf) {
    // Create the instance
    g_pTransmitLogSharedBuf = new(std::nothrow) CNSSharedMem(TRANSMIT_LOG_SHAREDMEM_NAME, TRANSMIT_LOG_SHAREDMEM_SIZE);
  }

  if (NULL != g_pTransmitLogSharedBuf) {
    if (!g_pTransmitLogSharedBuf->IsOpen()) {
      // maps the shared memory buffer
      l_eStatus = g_pTransmitLogSharedBuf->Open();
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NSSharedMemTransmitLogClose
/// Close the transmit logging shared memory
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSSharedMemTransmitLogClose() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != g_pTransmitLogSharedBuf) {
    // un-map the shared memory object
    l_eStatus = g_pTransmitLogSharedBuf->Close();

    delete g_pTransmitLogSharedBuf;
    g_pTransmitLogSharedBuf = NULL;
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NSSharedMemReadTransmitLog
/// Reads transmit log from the shared memory buffer.
////////////////////////////////////////////////////////////////////////////////////////////
SI_32 NSSharedMemReadTransmitLog(PSTR f_pBuffer, const UI_32 f_uiLength, const BOOL f_bBlock) {
  // no. of bytes read
  SI_32 l_iReadSize = NS_SHM_ERROR;

  if (NULL != g_pTransmitLogSharedBuf && NULL != f_pBuffer) {
    // Writes log data into shared memory buffer
    l_iReadSize = g_pTransmitLogSharedBuf->Read(f_pBuffer, f_uiLength, f_bBlock);
  }

  return l_iReadSize;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// NSSharedMemWriteTransmitLog
/// Write transmit log into the shared memory buffer.
////////////////////////////////////////////////////////////////////////////////////////////
SI_32 NSSharedMemWriteTransmitLog(PCSTR f_pBuffer, const UI_32 f_uiLength) {
  // no. of bytes read
  SI_32 l_iWriteSize = NS_SHM_ERROR;

  if (NULL != g_pTransmitLogSharedBuf && NULL != f_pBuffer) {
    // Writes log data into shared memory buffer
    l_iWriteSize = g_pTransmitLogSharedBuf->Write(f_pBuffer, f_uiLength);
  }

  return l_iWriteSize;
}
#endif


// For NsLog
CFrameworkunifiedVersion g_FrameworkunifiedVersion(MAJORNO, MINORNO, REVISION);
#define ZONE_INIT    ZONEMASK(10)
#define ZONE_FUNC    ZONEMASK(11)
#define ZONE_MEM     ZONEMASK(12)
#define ZONE_INFO    ZONEMASK(29)
#define ZONE_WARN    ZONEMASK(30)
#define ZONE_ERR     ZONEMASK(31)
#define FRAMEWORKUNIFIEDLOGAPPZONES  ZONE_INFO, ZONE_WARN, ZONE_ERR
FRAMEWORKUNIFIEDLOGPARAM   g_FrameworkunifiedLogParams = {
  LPRINT,
  {
    "Init",   "Function", "Memory",
    "",     "",     "",
    "",     "",     "",
    "",     "",     "",
    "",     "",     "",
    "",     "",     "",
    "",
    "Info",   "Warning",  "Error"
  },
  FRAMEWORKUNIFIEDLOGZONES
};

/** sleep */
static int
sleep_msec(int msec) {
  struct timespec ts, rem;
  ts.tv_sec = msec / 1000;
  ts.tv_nsec = (msec % 1000) * 1000000;
  int ret;
  for (;;) {
    ret = nanosleep(&ts, &rem);
    if (ret == 0 || (ret < 0 && errno != EINTR)) {
      break;
    }
    ts = rem;
  }
  return ret;
}

/** usage */
void usage(void) {
  printf("nsapi_sharedmem [command] [option]\n");
  printf("  command: preset\n");
#if !defined(AVAILABLE_API)
  printf("    preset service_write\n");
  printf("    preset service_write_read n\n");
#endif
  printf("    preset sharedmem_write\n");
  printf("    preset sharedmem_write_read n\n");
  printf("    preset write_6_A\n");
  printf("    preset write_6_B\n");
  printf("    preset write_7_A\n");
  printf("    preset write_7_B\n");
  printf("  command: manual\n");
  printf("    manual ***\n");
}

/** main */
int main(int argc, char *argv[]) {
  int status = 0;

  FRAMEWORKUNIFIED_SET_ZONES();

  if (argc < 2) {
    usage();
    return 0;
  } else {
    char *command = argv[1];
    int  option   = 0;

    /* Execute command */
    if (strcmp(command, "preset") == 0) {
      if (argc > 2) {
        char *preset = argv[2];
        if (argc > 3) {
          option = atoi(argv[3]);
        }

#if defined(AVAILABLE_API)
        if (strcmp(preset, "sharedmem_write") == 0) {
          NSUT_ASSERT(NSSharedMemTransmitLogOpen() ==  eFrameworkunifiedStatusOK, "NSSharedMemTransmitLogOpen() error.");
          SI_32 size = NSSharedMemWriteTransmitLog(argv[2], static_cast<UI_32>(strlen(argv[2]) + 1));
          NSUT_ASSERT(size == (SI_32)(strlen(argv[2]) + 1), "NSSharedMemWriteTransmitLog() error.");
          NSUT_ASSERT(NSSharedMemTransmitLogClose() == eFrameworkunifiedStatusOK, "NSSharedMemTransmitLogClose() error.");
          if (size != (SI_32)(strlen(argv[2]) + 1)) {
            return -1;
          }
        } else if (strcmp(preset, "sharedmem_write_read") == 0) {
          for (int i = 0; i < option; i++) {
            NSUT_ASSERT(NSSharedMemTransmitLogOpen() ==  eFrameworkunifiedStatusOK, "NSSharedMemTransmitLogOpen() error.");
            SI_32 size = NSSharedMemWriteTransmitLog(argv[2], static_cast<UI_32>(strlen(argv[2]) + 1));
            NSUT_ASSERT(size == (SI_32)(strlen(argv[2]) + 1), "NSSharedMemWriteTransmitLog() error.");
            char f_pBuffer[100] = {0};
            NSSharedMemReadTransmitLog(f_pBuffer, sizeof(f_pBuffer), FALSE);
            NSUT_ASSERT(NSSharedMemTransmitLogClose() == eFrameworkunifiedStatusOK, "NSSharedMemTransmitLogClose() error.");
          }
        }
#else
        if (strcmp(preset, "service_write") == 0) {
          TMemID id = SetDataToShared(argv[2], strlen(argv[2]) + 1);
          NSUT_ASSERT(id != BAD_MEM_ID, "SetDataToShared() error.");
          if (id == BAD_MEM_ID) {
            return -1;
          }
        } else if (strcmp(preset, "service_write_read") == 0) {
          for (int i = 0; i < option; i++) {
            EFrameworkunifiedStatus status;
            TMemID id = SetDataToShared(argv[2], strlen(argv[2]) + 1);
            NSUT_ASSERT(id != BAD_MEM_ID, "SetDataToShared() error.");
            UI_32 size = GetLengthOfDataFromShared(id);
            NSUT_ASSERT(size == (strlen(argv[2]) + 1), "NSSharedMemWriteTransmitLog() error.");
            void *data[100] = { 0 };
            status = GetDataFromShared(id, data, sizeof(data));
            NSUT_ASSERT(eFrameworkunifiedStatusOK == status, "GetDataFromShared()");
            status = DiscardDataFromShared(id);
            NSUT_ASSERT(eFrameworkunifiedStatusOK == status, "DiscardDataFromShared()");
          }
        } else if (strcmp(preset, "sharedmem_write") == 0) {
          NSUT_ASSERT(NSSharedMemTransmitLogOpen() ==  eFrameworkunifiedStatusOK, "NSSharedMemTransmitLogOpen() error.");
          SI_32 size = NSSharedMemWriteTransmitLog(argv[2], strlen(argv[2]) + 1);
          NSUT_ASSERT(size == (SI_32)(strlen(argv[2]) + 1), "NSSharedMemWriteTransmitLog() error.");
          NSUT_ASSERT(NSSharedMemTransmitLogClose() == eFrameworkunifiedStatusOK, "NSSharedMemTransmitLogClose() error.");
          if (size != (SI_32)(strlen(argv[2]) + 1)) {
            return -1;
          }
        } else if (strcmp(preset, "sharedmem_write_read") == 0) {
          for (int i = 0; i < option; i++) {
            NSUT_ASSERT(NSSharedMemTransmitLogOpen() ==  eFrameworkunifiedStatusOK, "NSSharedMemTransmitLogOpen() error.");
            SI_32 size = NSSharedMemWriteTransmitLog(argv[2], strlen(argv[2]) + 1);
            NSUT_ASSERT(size == (SI_32)(strlen(argv[2]) + 1), "NSSharedMemWriteTransmitLog() error.");
            char f_pBuffer[100] = {0};
            NSSharedMemReadTransmitLog(f_pBuffer, sizeof(f_pBuffer), FALSE);
            NSUT_ASSERT(NSSharedMemTransmitLogClose() == eFrameworkunifiedStatusOK, "NSSharedMemTransmitLogClose() error.");
          }
        }
#endif
        else if (strcmp(preset, "write_6_A") == 0) {  // NOLINT  (readability/nolint)
          CNSSharedMem *testObj = new CNSSharedMem(TRANSMIT_LOG_SHAREDMEM_NAME, TRANSMIT_LOG_SHAREDMEM_SIZE);
          if (testObj->Open() != eFrameworkunifiedStatusOK) {
            status = -1;
          }
          sleep(2);
          char f_pBuffer[100] = {0};
          if (testObj->Read(f_pBuffer, sizeof(f_pBuffer), FALSE) != 7) {
            status = -1;
          }
          if (memcmp(f_pBuffer, "ABCDEFG", 7) != 0) {
            printf("memcmp() error.");
            return -1;
          }
          if (testObj->Close() != eFrameworkunifiedStatusOK) {
            status = -1;
          }
          delete testObj;
        } else if (strcmp(preset, "write_6_B") == 0)  {
          CNSSharedMem *testObj = new CNSSharedMem(TRANSMIT_LOG_SHAREDMEM_NAME, TRANSMIT_LOG_SHAREDMEM_SIZE);
          if (testObj->Open() != eFrameworkunifiedStatusOK) {
            status = -1;
          }
          if (testObj->Write("ABCDEFG", static_cast<UI_32>(strlen("ABCDEFG"))) != 7) {
            status = -1;
          }
          if (testObj->Close() != eFrameworkunifiedStatusOK) {
            status = -1;
          }
          delete testObj;
        } else if (strcmp(preset, "write_7_A") == 0) {
          CNSSharedMem *testObj = new CNSSharedMem(TRANSMIT_LOG_SHAREDMEM_NAME, TRANSMIT_LOG_SHAREDMEM_SIZE);
          if (testObj->Open() != eFrameworkunifiedStatusOK) {
            status = -1;
          }
          sleep(2);
          char f_pBuffer[100] = {0};
          if (testObj->Read(f_pBuffer, sizeof(f_pBuffer), FALSE) == 7) {
            status = -1;
          }
          if (testObj->Close() != eFrameworkunifiedStatusOK) {
            status = -1;
          }
          delete testObj;
        } else if (strcmp(preset, "write_7_B") == 0) {
          CNSSharedMem *testObj = new CNSSharedMem(TRANSMIT_LOG_SHAREDMEM_NAME, TRANSMIT_LOG_SHAREDMEM_SIZE);
          if (testObj->Open() == eFrameworkunifiedStatusOK) {
            status = -1;
          }
          if (testObj->Write("ABCDEFG", static_cast<UI_32>(strlen("ABCDEFG"))) == 7) {
            status = -1;
          }
          if (testObj->Close() == eFrameworkunifiedStatusOK) {
            status = -1;
          }
          delete testObj;
        } else if (strcmp(preset, "stress_4") == 0) {
          int i;
          char *writeBuffer = new char[0x400]();
          for (i = 0; i < 0x400; i++) {
            writeBuffer[i] = static_cast<char>(i);
          }

          CNSSharedMem *testObj = new CNSSharedMem(TRANSMIT_LOG_SHAREDMEM_NAME, TRANSMIT_LOG_SHAREDMEM_SIZE);
          if (testObj->Open() != eFrameworkunifiedStatusOK) {
            status = -1;
          }
          int timeout = 0;
          int test_count = 0;
          while ((timeout < 10) && (test_count < option)) {
            if (testObj->GetSize() > (SI_32)(TRANSMIT_LOG_SHAREDMEM_SIZE - 0x800)) {
              sleep_msec(100);
              timeout++;
              // printf("timeout = %d\n", timeout);
              continue;
            }
            if (testObj->ClearBuf() != eFrameworkunifiedStatusOK) {
              status = -1;
            }
            if (testObj->SetReadPtrToWritePtr() != eFrameworkunifiedStatusOK) {
              status = -1;
            }
            if (testObj->Write(writeBuffer, 0x400) != 0x400) {
              status = -1;
            }
            timeout = 0;
            test_count++;
          }
          if (testObj->Close() != eFrameworkunifiedStatusOK) {
            status = -1;
          }
          delete testObj;

          delete[] writeBuffer;
        } else if (strcmp(preset, "stress_5") == 0) {
          int i;
          char *writeBuffer = new char[0x100]();
          for (i = 0; i < 0x100; i++) {
            writeBuffer[i] = static_cast<char>(0xff - i);
          }

          CNSSharedMem *testObj = new CNSSharedMem(TRANSMIT_LOG_SHAREDMEM_NAME, TRANSMIT_LOG_SHAREDMEM_SIZE);
          if (testObj->Open() != eFrameworkunifiedStatusOK) {
            status = -1;
          }
          int timeout = 0;
          int count = 0;
          while (timeout < 10) {
            int size = (count % 0xff) + 1;
            if (testObj->GetSize() > (SI_32)(TRANSMIT_LOG_SHAREDMEM_SIZE - 0x800)) {
              sleep_msec(100);
              timeout++;
              continue;
            }
            if (testObj->Write(&writeBuffer[0xff - size], size) != size) {
              status = -1;
            }
            timeout = 0;
            count++;
          }
          if (testObj->Close() != eFrameworkunifiedStatusOK) {
            status = -1;
          }
          delete testObj;

          delete[] writeBuffer;
        } else {
          usage();
        }
      } else {
        usage();
      }
    } else if (strcmp(command, "manual") == 0) {
      int   testno;
      char  buf[32];
      INT8  active = 1;

      while (active) {
        printf(" ----- imageapi ----------\n");
        printf("   0: debug_test()\n");
        printf("   1: gfx_ut_image_api()\n");
        printf("   2: gfx_ut_image_csv()\n");
        printf("   q: quit\n");
        printf(" -------------------------\n");

        fgets(buf, sizeof(buf), stdin);
        if (buf[0] == '\n') {
          continue;
        } else if (buf[0] == 'q' || buf[0] == 'Q') {
          active = 0;
          continue;
        }
        testno = atoi(buf);

        switch (testno) {
          case 0:
            // debug_test();
            break;
          case 1:
            // gfx_ut_image_api();
            break;
          case 2:
            // gfx_ut_image_csv((char *)"it_image.csv");
            break;
          default:
            break;
        }
      }
    } else {
      usage();
    }
  }
  return status;
}
