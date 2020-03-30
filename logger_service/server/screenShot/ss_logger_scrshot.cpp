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
/// \ingroup  tag_SS_LoggerService
/// \brief
///
///
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>
#include <du.h>

#define MAX_IMAGE_WIDTH    1280
#define MAX_IMAGE_HEIGHT  480
#define COLOR_COMPONENT    4

int SS_Logger_SaveScreenShot(const char* filename, int type) {
  FILE *outfile;
  struct SCRSHOT_capture_arg stSCR;
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  unsigned char *work_buf;

  stSCR.du = type;  // 0:E2 1:M2
  stSCR.fmt = SCRSHOT_FMT_ARGB8888;
  stSCR.buff = malloc(MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * COLOR_COMPONENT);
  stSCR.buff_len = MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * COLOR_COMPONENT;
  stSCR.ev_handler = NULL;  // sync mode

  if (stSCR.buff == NULL) {
    exit(1);
  }

  if (SCRSHOT_RET_SUCCESS != SCRSHOT_capture(&stSCR)) {
    exit(1);
  }

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  if ((outfile = fopen(filename, "wb")) == NULL) {
    exit(1);
  }
  jpeg_stdio_dest(&cinfo, outfile);
  cinfo.image_width = stSCR.width;
  cinfo.image_height = stSCR.height;
  cinfo.input_components = COLOR_COMPONENT;
  cinfo.in_color_space = JCS_EXT_BGRA;
  jpeg_set_defaults(&cinfo);
  jpeg_start_compress(&cinfo, TRUE);

  work_buf = (unsigned char*) stSCR.buff;
  for (unsigned int i = 0; i < stSCR.height; i++) {
    jpeg_write_scanlines(&cinfo, (JSAMPARRAY) & (work_buf), 1);
    work_buf += stSCR.width * COLOR_COMPONENT;
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  free(stSCR.buff);
  fclose(outfile);
  exit(0);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    exit(1);
  }
  int chType = atoi(argv[2]);
  SS_Logger_SaveScreenShot(argv[1], chType);

  return 0;
}
