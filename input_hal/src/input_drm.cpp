/*
 * @copyright Copyright (c) 2018-2020 TOYOTA MOTOR CORPORATION.
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
#include "input_drm.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#if defined(_USE_DRM)
#include <xf86drm.h>
#include <xf86drmMode.h>
#endif

#include "input_hal.h"
#include "input_hal_debug.h"

#define DIR_PATH         "/dev/dri/card0"
#define DEFAULT_RESOLUTION_HORIZONTAL   1280       /* Horizontal resolution default value */
#define DEFAULT_RESOLUTION_VERTICAL     800        /* Vertical resolution default value */
/**
 * Panel resolution acquisition / GetPanelSpecResolutionInput
 */
int GetPanelSpecResolutionInput(int *reso_h, int *reso_v) {
  if ((NULL == reso_h) || (NULL == reso_v)) {
    return HAL_INPUT_RET_ERROR;
  }

#if defined(_USE_DRM)
  int  fd;
  drmModeRes *resources = NULL;
  drmModeConnector *connector = NULL;
  drmModeEncoder *encoder = NULL;
  drmModeCrtc *crtc = NULL;

  fd = open(DIR_PATH, O_RDWR);
  if (fd < 0) {
    INPUT_ERROR_LOG("DRI Open Error=%s\n", DIR_PATH);
    goto err_rtn;
  }

  resources = drmModeGetResources(fd);
  if (!resources) {
    INPUT_ERROR_LOG("No resources\n");
    goto err_rtn;
  }
  if (2 > resources->count_connectors) {
    INPUT_ERROR_LOG("DRI Connect Num Error connectors=%d\n",
                      resources->count_connectors);
    goto err_rtn;
  }

  connector = drmModeGetConnector(fd, resources->connectors[1]);
  if (!connector) {
    INPUT_ERROR_LOG("No Connector\n");
    goto err_rtn;
  }

  if ((DRM_MODE_CONNECTED == connector->connection) &&(connector->count_modes > 0)) {
  } else {
    INPUT_ERROR_LOG("Not found connected connector\n");
    goto err_rtn;
  }

  encoder = drmModeGetEncoder(fd, connector->encoder_id);
  if (!encoder) {
    INPUT_ERROR_LOG("drmModeGetEncoder null\n");
    goto err_rtn;
  }

  crtc = drmModeGetCrtc(fd, encoder->crtc_id);
  if (!crtc) {
    INPUT_ERROR_LOG("drmModeGetCrtc null\n");
    goto err_rtn;
  }

  *reso_h  = crtc->mode.hdisplay;
  *reso_v = crtc->mode.vdisplay;

  drmModeFreeCrtc(crtc);
  drmModeFreeEncoder(encoder);
  drmModeFreeConnector(connector);
  drmModeFreeResources(resources);
  close(fd);

  INPUT_LOG_TRACE("width=%d height=%d\n", *reso_h, *reso_v);
  return HAL_INPUT_RET_NORMAL;

err_rtn:

  if (encoder) {
    drmModeFreeEncoder(encoder);
  }
  if (connector) {
    drmModeFreeConnector(connector);
  }
  if (resources) {
    drmModeFreeResources(resources);
  }
  if (fd >= 0) {
    close(fd);
  }

  INPUT_ERROR_LOG("Use Default Resolution\n");
  *reso_h =  DEFAULT_RESOLUTION_HORIZONTAL;
  *reso_v  =  DEFAULT_RESOLUTION_VERTICAL;

  return HAL_INPUT_RET_ERROR;
#else
  *reso_h  =  DEFAULT_RESOLUTION_HORIZONTAL;
  *reso_v  =  DEFAULT_RESOLUTION_VERTICAL;

  return HAL_INPUT_RET_NORMAL;
#endif
}
