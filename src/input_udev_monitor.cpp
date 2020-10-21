/*
 * @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
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

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <libudev.h>
#include <native_service/ns_message_center_if.h>

#include "input_hal.h"
#include "input_hal_debug.h"
#include "input_hal_internal.h"
#include "input_drm.h"
#include "input_udev_monitor.h"

#define HAL_INPUT_UDEVMONITOR_THREADNAME "input_hal_udevm"
#define max_value(x, y) ((x) > (y) ? (x) : (y))

#define INPUT_VIRTUAL_INPUTDEVICE_NAME "/devices/virtual" /* Virtual device name created by SwitchManager */

// Enough name length to contain node name
#define INPUT_NODE_NAME_LENGTH 256

#define TRACKING_ID_NONE    -1

static struct InputUdevMonitorInfo g_input_udevmonitor_info;
static HANDLE g_sender_handle;
static bool g_input_touch_init_notify = false;
extern char* g_app_name;  /* app name */
bool g_break_from_watch = false;  /* watch thread loop break flag */
pthread_t g_udev_monitor_thread = -1;  /* udev monitor thread */

static void InputUdevMonitorTouchInitFinEventSend(int result) {
  struct TouchInitFinishInput msg;

  if (g_input_touch_init_notify) {
    return;
  }

  if (NULL == g_sender_handle) {
    g_sender_handle = McOpenSender(g_app_name);
  }
  msg.result = result;

  InputUtilMCSend(
      g_sender_handle, HAL_INPUT_SOURCE_NAME,
      HAL_INPUT_NOTIFY_TOUCH_INIT_FINISH, sizeof(msg), &msg);
  g_input_touch_init_notify = true;
}

static void InputUdevMonitorInputEventHandle(int fd, int device_type, int notify_id) {
  ssize_t   read_size;
  struct EventsPackageInput events = {0};

  read_size = read(fd, events.event, sizeof(events.event));
  if (read_size > 0) {
    events.count = read_size / sizeof(struct input_event);
    events.device_type = device_type;

    if (NULL == g_sender_handle) {
      g_sender_handle = McOpenSender(g_app_name);
    }

    InputUtilMCSend(
        g_sender_handle, HAL_INPUT_SOURCE_NAME,
        notify_id, sizeof(events), &events);
  }
}

static void InputUdevMonitorTouchEventHandle(int fd, int notify_id) {
  ssize_t   read_size;
  struct input_event event[HAL_INPUT_EVENT_COUNT] = {0};
  int touch_status;

  read_size = read(fd, event, sizeof(event));

  if (read_size > 0) {
    if (NULL == g_sender_handle) {
      g_sender_handle = McOpenSender(g_app_name);
    }

    int event_num = read_size / sizeof(struct input_event);

    for (int index = 0; index < event_num; ++index) {
      if (EV_ABS == event[index].type) {
        if (event[index].code == ABS_MT_TRACKING_ID) {
          if (TRACKING_ID_NONE == event[index].value) {
            touch_status = HAL_INPUT_TOUCH_RELEASE;
          } else {
            touch_status = HAL_INPUT_TOUCH_PRESS;
          }
          InputUtilMCSend(
              g_sender_handle, HAL_INPUT_SOURCE_NAME,
              notify_id, sizeof(touch_status), &touch_status);
        }
      }
    }
  }
}

static int InputUdevMonitorDeviceAssort(struct udev_device *dev) {
  const char *property;

  char touch_key_device_name[INPUT_NODE_NAME_LENGTH] = { 0 };
  GetKeyNameTouch(touch_key_device_name, sizeof(touch_key_device_name));
  const char *sysattr_name = udev_device_get_sysattr_value(dev, "name");
  if ((sysattr_name) &&
      (0 == strcmp(touch_key_device_name, sysattr_name))) {
    INPUT_LOG_TRACE("DeviceAssort : ESC_SW\n");
    return HAL_INPUT_DEVICE_TOUCH_ESCKEY;
  }

  char touch_device_name[INPUT_NODE_NAME_LENGTH] = { 0 };
  GetPanelNameTouch(touch_device_name, sizeof(touch_device_name));
  sysattr_name = udev_device_get_sysattr_value(dev, "name");
  if ((sysattr_name) &&
      (0 == strcmp(touch_device_name, sysattr_name))) {
    INPUT_LOG_TRACE("DeviceAssort : Touch\n");
    return HAL_INPUT_DEVICE_TOUCH;
  }

  property = udev_device_get_property_value(dev, "ID_INPUT_KEYBOARD");
  if ((property) && (strcmp("1", property) == 0)) {
    INPUT_LOG_TRACE("DeviceAssort : KeyBoard\n");
    return HAL_INPUT_DEVICE_KEYBOARD;
  }

  property = udev_device_get_property_value(dev, "ID_INPUT_TABLET");
  if ((property) && (strcmp("1", property) == 0)) {
    INPUT_LOG_TRACE("DeviceAssort : Tablet\n");
    return HAL_INPUT_DEVICE_INVALID;
  }

  property = udev_device_get_property_value(dev, "ID_INPUT_TOUCHPAD");
  if ((property) && (strcmp("1", property) == 0)) {
    INPUT_LOG_TRACE("DeviceAssort : Touch pad\n");
    return HAL_INPUT_DEVICE_TABLET_FINGER;
  }

  return HAL_INPUT_DEVICE_INVALID;
}

static void InputUdevMonitorDeviceListOutput(void) {
  struct InputUtilList *p;

  INPUT_LOG_TRACE("OutputList >> start ======================\n");
  input_list_for_each(p, &g_input_udevmonitor_info.dev_list.list) {
    struct InputInputDeviceList *entry = input_list_entry(p, struct InputInputDeviceList, list);
    if (NULL != entry) {
      INPUT_LOG_TRACE("FD: %d Device=%s Assort=%d\n",
                     entry->fd, entry->device_node, entry->device_assort);
    }
  }
  INPUT_LOG_TRACE("OutputList << end ======================\n");
}

static void InputUdevMonitorDeviceListDelete(const char *node) {
  struct InputUtilList *p;

  input_list_for_each(p, &g_input_udevmonitor_info.dev_list.list) {
    struct InputInputDeviceList *entry = input_list_entry(p, struct InputInputDeviceList, list);
    if (strcmp(node, entry->device_node) == 0) {
      close(entry->fd);
      InputUtilListDelete(p);
      free(entry);
      break;
    }
  }
  InputUdevMonitorDeviceListOutput();
}

static int InputUdevMonitorDeviceListAdd(int fd, int device_assort, const char *node) {
  int   ret = HAL_INPUT_RET_NORMAL;
  struct InputInputDeviceList *p;

  p = (struct InputInputDeviceList *) malloc(sizeof(struct InputInputDeviceList));
  if (p) {
    p->fd = fd;
    p->device_assort = device_assort;

    if (strlen(node) < INPUT_DEVICE_NODE_LENGTH_MAX) {
      strncpy(p->device_node, node, sizeof(p->device_node) - 1);
      InputUdevMonitorDeviceListDelete(node);
      InputUtilListAdd(&p->list, &g_input_udevmonitor_info.dev_list.list);
    } else {
      ret = HAL_INPUT_RET_ERROR;
    }
  } else {
    ret = HAL_INPUT_RET_ERROR;
  }

  InputUdevMonitorDeviceListOutput();

  return ret;
}

static int InputUdevMonitorInputEventGrab(const char *node, struct udev_device *dev) {
  int fd, rtn;

  InputUdevMonitorDeviceListDelete(node);

  fd = open(node, O_RDONLY);

  if (fd < 0) {
    INPUT_ERROR_LOG("ERR: open %s errno=%d \n", node, errno);
    goto err_rtn;
  }

  rtn = ioctl(fd, EVIOCGRAB, 1);
  if (rtn) {
    INPUT_ERROR_LOG("ERR: ioctl grab %s \n", node);
    goto err_rtn_close;
  }
  INPUT_LOG_TRACE("%s Grab \n", node);

  /* Backup FD */
  rtn = InputUdevMonitorDeviceListAdd(fd, InputUdevMonitorDeviceAssort(dev), node);
  if (rtn) {
    goto err_rtn_close;
  }

  return HAL_INPUT_RET_NORMAL;

err_rtn_close:
  close(fd);
err_rtn:
  INPUT_LOG_TRACE("%s Grab impossible \n", node);
  return HAL_INPUT_RET_ERROR;
}

static void InputUdevMonitorFDSet(int *nfds, fd_set *fds) {
  struct InputUtilList *p;

  input_list_for_each(p, &g_input_udevmonitor_info.dev_list.list) {
    struct InputInputDeviceList *entry = input_list_entry(p, struct InputInputDeviceList, list);

    switch (entry->device_assort) {
      case HAL_INPUT_DEVICE_TOUCH_ESCKEY:
      case HAL_INPUT_DEVICE_TOUCH:
      case HAL_INPUT_DEVICE_KEYBOARD:
      case HAL_INPUT_DEVICE_TABLET_FINGER:
      FD_SET(entry->fd, fds);
      *nfds = max_value(*nfds, entry->fd);
      break;
      default:
      break;
    }
  }
}

static void InputUdevMonitorFDHandle(fd_set *fds) {
  struct InputUtilList *p;

  input_list_for_each(p, &g_input_udevmonitor_info.dev_list.list) {
    struct InputInputDeviceList *entry = input_list_entry(p, struct InputInputDeviceList, list);
    if (FD_ISSET(entry->fd, fds)) {
      int notify_id = 0;
      switch (entry->device_assort) {
      case HAL_INPUT_DEVICE_TOUCH_ESCKEY:
        notify_id = HAL_INPUT_NOTIFY_ESC_KEY;
        break;
      case HAL_INPUT_DEVICE_TOUCH:
        notify_id = HAL_INPUT_NOTIFY_TOUCH;
        break;
      case HAL_INPUT_DEVICE_KEYBOARD:
        notify_id = HAL_INPUT_NOTIFY_KEY_BOARD;
        break;
      case HAL_INPUT_DEVICE_TABLET_FINGER:
        notify_id = HAL_INPUT_NOTIFY_TABLET_FINGER;
        break;
      default:
        return;
      }
      if (notify_id == HAL_INPUT_NOTIFY_TOUCH) {
        InputUdevMonitorTouchEventHandle(entry->fd, notify_id);
      } else {
        InputUdevMonitorInputEventHandle(entry->fd, entry->device_assort, notify_id);
      }
    }
  }
}

static int InputUdevMonitorInit(void) {
  g_input_udevmonitor_info.udev = udev_new();
  if (NULL == g_input_udevmonitor_info.udev) {
    INPUT_ERROR_LOG("ERR: udev_new ret=%d \n", errno);
    return HAL_INPUT_RET_ERROR;
  }

  // create the udev monitor
  g_input_udevmonitor_info.monitor = udev_monitor_new_from_netlink(g_input_udevmonitor_info.udev, "udev");
  udev_monitor_filter_add_match_subsystem_devtype(g_input_udevmonitor_info.monitor, "input", NULL);

  // start receiving hotplug events
  udev_monitor_enable_receiving(g_input_udevmonitor_info.monitor);

  INPUT_INIT_LIST_HEAD(&g_input_udevmonitor_info.dev_list.list)

  return HAL_INPUT_RET_NORMAL;
}

static void InputUdevMonitorDeinit(void) {
    // destroy the udev monitor
    udev_monitor_unref(g_input_udevmonitor_info.monitor);
    // destroy the udev object
    udev_unref(g_input_udevmonitor_info.udev);

    if (g_sender_handle != NULL) {
      McClose(g_sender_handle);
      g_sender_handle = NULL;
    }

    g_input_touch_init_notify = false;
}

static int InputUdevMonitorDevicesGet(void) {
  int   ret;
  struct udev_enumerate *enumerate;
  struct udev_list_entry *devices, *dev_list_entry;
  char touch_device_name[INPUT_NODE_NAME_LENGTH] = { 0};
  const char* sysattr_name;
  bool input_touch_device_found = false;

  // Create a list of the devices in the 'usb_device' subsystem.
  enumerate = udev_enumerate_new(g_input_udevmonitor_info.udev);
  udev_enumerate_add_match_subsystem(enumerate, "input");
  udev_enumerate_scan_devices(enumerate);
  devices = udev_enumerate_get_list_entry(enumerate);
  GetPanelNameTouch(touch_device_name, sizeof(touch_device_name));

  // Enumerate device list
  udev_list_entry_foreach(dev_list_entry, devices) {
    const char *path, *node;

    // Get the filename of the /sys entry for the device and create a udev_device object (dev) representing it
    path = udev_list_entry_get_name(dev_list_entry);
    struct udev_device* dev = udev_device_new_from_syspath(g_input_udevmonitor_info.udev, path);
    if (NULL == dev) continue;

    // usb_device_get_devnode() returns the path to the device node itself in /dev.
    node = udev_device_get_devnode(dev);
    if (!node) {
      udev_device_unref(dev);
      continue;
    }

    // Filter device name is eventX
    if (strncmp("event", udev_device_get_sysname(dev), sizeof("event") -1) != 0) {
      udev_device_unref(dev);
      continue;
    }

    // virtual device
    if (strncmp(INPUT_VIRTUAL_INPUTDEVICE_NAME, udev_device_get_devpath(dev),
                sizeof(INPUT_VIRTUAL_INPUTDEVICE_NAME) - 1) == 0) {
      INPUT_LOG_TRACE("Found Virtual Device : %s \n", node);
      udev_device_unref(dev);
      continue;
    }

    // check parent is input
    struct udev_device* input_dev = udev_device_get_parent_with_subsystem_devtype(dev, "input", NULL);
    if (NULL == input_dev) {
      udev_device_unref(dev);
      continue;
    }

    sysattr_name = udev_device_get_sysattr_value(input_dev, "name");
    if (NULL == sysattr_name) {
      INPUT_ERROR_LOG("ERR: Unable to find sysattr \n");
      udev_device_unref(dev);
      continue;
    }

    // touchpanel device
    if (0 == strcmp(touch_device_name, sysattr_name)) {
      INPUT_LOG_TRACE("Found %s : %s \n", touch_device_name, path);
      int spec_reso_h;
      int spec_reso_v;
      GetPanelSpecResolutionInput(&spec_reso_h, &spec_reso_v);
      ret = ConfigTouch(udev_device_get_syspath(input_dev),
                        spec_reso_h, spec_reso_v);

      if (HAL_INPUT_RET_NORMAL == ret) {
        int status;
        ret = GetConfigStatusTouch(&status);
        if (HAL_INPUT_RET_NORMAL == ret) {
          if (HAL_INPUT_TOUCH_CONFIG_OFF == status) {
            InputUdevMonitorTouchInitFinEventSend(HAL_INPUT_RET_NORMAL);
          }
        } else {
          INPUT_ERROR_LOG("GetConfigStatusTouch fail\n");
          InputUdevMonitorTouchInitFinEventSend(HAL_INPUT_RET_ERROR);
        }
      } else {
        INPUT_ERROR_LOG("ConfigTouch fail\n");
        InputUdevMonitorTouchInitFinEventSend(HAL_INPUT_RET_ERROR);
      }

      input_touch_device_found = true;
    }

    INPUT_LOG_TRACE("Found Device : %s \n", node);

    /* Modified not to notify input events to other processes */
    InputUdevMonitorInputEventGrab(node, input_dev);

    udev_device_unref(dev);
  }  // end foreach

  if (!input_touch_device_found) {
    INPUT_ERROR_LOG("ERR: Dummy Device Create");
    InputUdevMonitorTouchInitFinEventSend(HAL_INPUT_RET_NORMAL);
  }

  // Free the enumerator object
  udev_enumerate_unref(enumerate);

  return HAL_INPUT_RET_NORMAL;
}

static void InputUdevMonitorDeviceStatusChange(int fd, fd_set *fds) {
  if (FD_ISSET(fd, fds)) {
    // receive the relevant device
    struct udev_device* dev = udev_monitor_receive_device(g_input_udevmonitor_info.monitor);
    if (NULL == dev) return;

    //   input_udev_DevicesGet(udm);
    const char* action = udev_device_get_action(dev);
    const char* node = udev_device_get_devnode(dev);
    if ((!action) || (!node)) {
      udev_device_unref(dev);
      return;
    }

    if (strncmp("event", udev_device_get_sysname(dev), sizeof("event") -1) != 0) {
      INPUT_LOG_TRACE("not event device %s \n", udev_device_get_sysname(dev));
      udev_device_unref(dev);
      return;
    }

    if (strncmp(INPUT_VIRTUAL_INPUTDEVICE_NAME, udev_device_get_devpath(dev),
                sizeof(INPUT_VIRTUAL_INPUTDEVICE_NAME) - 1) == 0) {
      udev_device_unref(dev);
      return;
    }

    if (strcmp(action, "remove") == 0) {
      InputUdevMonitorDeviceListDelete(node);
    }

    struct udev_device* input_dev = udev_device_get_parent_with_subsystem_devtype(dev, "input", NULL);
    if (NULL == input_dev) {
      udev_device_unref(dev);
      return;
    }
    char touch_device_name[INPUT_NODE_NAME_LENGTH] = { 0};
    GetPanelNameTouch(touch_device_name, sizeof(touch_device_name));
    const char *sysattr_name = udev_device_get_sysattr_value(input_dev, "name");
    if (NULL == sysattr_name) {
      INPUT_ERROR_LOG("ERR: Unable to find sysattr \n");
      udev_device_unref(dev);
      return;
    }

    if (0 == strcmp(touch_device_name, sysattr_name)) {
      if (strcmp(action, "remove") == 0) {
      } else {
        INPUT_LOG_TRACE("Found %s \n", touch_device_name);
        InputUdevMonitorTouchInitFinEventSend(HAL_INPUT_RET_NORMAL);
      }
    }

    INPUT_LOG_TRACE("%s : %s \n", node, action);
    if (strcmp(action, "remove") == 0) {
    } else {
      /* Modified not to notify input events to other processes */
      InputUdevMonitorInputEventGrab(node, input_dev);
    }
    udev_device_unref(dev);
  }
}

static void InputUdevMonitorWatch(void) {
  fd_set fds;
  int fd = udev_monitor_get_fd(g_input_udevmonitor_info.monitor);

  if (fd != -1) {
    while (!g_break_from_watch) {
      FD_ZERO(&fds);
      FD_SET(fd, &fds);
      int nfds = max_value(0, fd);

      InputUdevMonitorFDSet(&nfds, &fds);

      struct timeval timeout;
      timeout.tv_sec = 0;
      timeout.tv_usec = 100 * 1000;
      int ret = select(nfds + 1, &fds, NULL, NULL, &timeout);
      if (ret > 0) {
        InputUdevMonitorDeviceStatusChange(fd, &fds);

        InputUdevMonitorFDHandle(&fds);
      }
    }
  } else {
    INPUT_ERROR_LOG("ERR: udev_monitor_get_fd");
  }
}

static void *InputUdevMonitorMain(void * arg) {
  int   rtn;
  prctl(PR_SET_NAME, HAL_INPUT_UDEVMONITOR_THREADNAME);

  rtn = InputUdevMonitorInit();
    if (rtn != HAL_INPUT_RET_NORMAL) {
    goto err_rtn;
  }

  /* Get input device */
  InputUdevMonitorDevicesGet();

  /* Input device monitoring (basically never get out from here) */
  InputUdevMonitorWatch();

  InputUdevMonitorDeinit();

  return NULL;

err_rtn:
  INPUT_LOG_TRACE("pthread_detach\n");
  return NULL;
}

int32_t InputUdevMonitorThreadCreate(void) {
  int ret;

  ret = pthread_create(&g_udev_monitor_thread, NULL, InputUdevMonitorMain, NULL);
  if (ret != 0) {
    INPUT_ERROR_LOG("ERR: pthread_create =%d\n", errno);
    return HAL_INPUT_RET_ERROR;
  }

  return HAL_INPUT_RET_NORMAL;
}
