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

/**
 * @file ss_devicedetection_service.h
 * @brief This file supports the Device Detection service.
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup device_detection_service
 *  @ingroup system_service
 *  @{
 */

#ifndef _SS_DEVICEDETECTION_SERVER_H_  // NOLINT (build/header_guard)
#define _SS_DEVICEDETECTION_SERVER_H_


#include <native_service/frameworkunified_types.h>
#include <string>
#include "system_service/ss_devicedetection_service_types.h"

/**
 * \~english The max size of the file path.
 */
#define FILEPATH_SIZE  (256)

/**
 * \~english The max size of URL.
 */
#define URL_SIZE  (256)

/**
 * \~english The max size of the device name.
 */
#define SS_DEV_DEVICE_NAME_SIZE (64)

/**
 * \~english The max size of the name.
 */
#define SS_DEV_NAME_SIZE (256)


/**
 * \~english Structure for FuseDAV info.
 */
typedef struct _FuseDAVInfo {
  /**
   * \~english FuseDAV server address
   */
  int8_t  url[URL_SIZE];  // FuseDAV server address
}SS_FuseDAVInfo;

/**
 * \~english Structure for USB device info.
 */
typedef struct _UsbInfo {
  /**
   * \~english Bus Number
   */
  uint8_t  bus_no;    // Bus Number
  /**
   * \~english Device Number
   */
  uint8_t  dev_no;    // Device Number
  /**
   * \~english Vendor ID
   */
  uint16_t  idVendor;  // Vendor ID
  /**
   * \~english Product ID
   */
  uint16_t  idProduct;  // Product ID
  /**
   * \~english reserved area
   */
  uint8_t  reserved[2];
  /**
   * \~english Device file path
   */
  int8_t  deviceFile[FILEPATH_SIZE];  // Device file path
}SS_UsbInfo;

/**
 * \~english CID(Card Identification Register Data)of SD/MMC card
 */
typedef struct _CID {
  /**
   * \~english Manufacturer ID
   */
  uint8_t  manufacturerID;      /* Manufacturer ID        */
  /**
   * \~english Application ID
   */
  uint16_t  applicationID;      /* Application ID        */
  /**
   * \~english Product Revision (PRV)
   */
  uint8_t  revision;      /* Product Revision (PRV)    */
  /**
   * \~english Product Serial Number (PSN)
   */
  uint32_t  serialNumber;      /* Product Serial Number (PSN)  */
  /**
   * \~english Product Name (PNM)
   */
  uint8_t  productName[6];      /* Product Name (PNM)      */
  /**
   * \~english Reserved area
   */
  uint8_t  reserve[2];      /* Reserve            */
  /**
   * \~english Manufacture date (MDT)
   */
  uint16_t  manufactureDate;    /* Manufacture date (MDT)    */
}SS_CID;

// CSD(Card Specification Register Data)of SD/MMC card
/**
 * \~english CSD(Card Specification Register Data)of SD/MMC card
 */
typedef struct _CSD {
  /**
   * \~english CSD structure
   */
  uint8_t  csd_structure;      /* CSD structure        */
  /**
   * \~english data read access-time-1
   */
  uint8_t  taac;        /* data read access-time-1    */
  /**
   * \~english data read access-time-2 in CLK cycles (NSAC*100)
   */
  uint8_t  nsac;        /* data read access-time-2 in CLK cycles (NSAC*100)  */
  /**
   * \~english max. data transfer rate
   */
  uint8_t  tran_speed;      /* max. data transfer rate  */
  /**
   * \~english card command classes
   */
  uint16_t  ccc;        /* card command classes      */
  /**
   * \~english max. read data block length
   */
  uint8_t  read_bl_len;      /* max. read data block length  */
  /**
   * \~english partial blocks for read allowed
   */
  uint8_t  read_bl_partial;    /* partial blocks for read allowed  */
  /**
   * \~english write block misalignment
   */
  uint8_t  write_blk_misalign;    /* write block misalignment    */
  /**
   * \~english read block misalignment
   */
  uint8_t  read_blk_misalign;    /* read block misalignment    */
  /**
   * \~english DSR implemented
   */
  uint8_t  dsr_imp;      /* DSR implemented        */
  /**
   * \~english Reserve area 1
   */
  uint8_t  reserve1;      /* Reserve 1          */
  /**
   * \~english device size
   */
  uint32_t  c_size;        /* device size          */
  /**
   * \~english max. read current -VDD min
   */
  uint8_t  vdd_r_curr_min;      /* max. read current @VDD min  */
  /**
   * \~english max. read current -VDD max
   */
  uint8_t  vdd_r_curr_max;      /* max. read current @VDD max  */
  /**
   * \~english max. write current -VDD min
   */
  uint8_t  vdd_w_curr_min;      /* max. write current @VDD min  */
  /**
   * \~english max. write current -VDD max
   */
  uint8_t  vdd_w_curr_max;      /* max. write current @VDD max  */
  /**
   * \~english device size multiplier
   */
  uint8_t  c_size_mult;      /* device size multiplier    */
  /**
   * \~english erase single block enable
   */
  uint8_t  erase_blk_en;      /* erase single block enable  */
  /**
   * \~english erase sector size
   */
  uint8_t  sector_size;      /* erase sector size      */
  /**
   * \~english write protect group size
   */
  uint8_t  wp_grp_size;      /* write protect group size    */
  /**
   * \~english write protect group enable
   */
  uint8_t  wp_grp_enable;      /* write protect group enable  */
  /**
   * \~english write speed factor
   */
  uint8_t  r2w_factor;      /* write speed factor      */
  /**
   * \~english max. write data block length
   */
  uint8_t  write_bl_len;      /* max. write data block length  */
  /**
   * \~english partial blocks for write allowed
   */
  uint8_t  write_bl_partial;    /* partial blocks for write allowed  */
  /**
   * \~english File format group
   */
  uint8_t  file_format_grp;    /* File format group      */
  /**
   * \~english copy flag
   */
  uint8_t  copy;        /* copy flag          */
  /**
   * \~english permanent write protection
   */
  uint8_t  perm_write_protect;    /* permanent write protection  */
  /**
   * \~english temporary write protection
   */
  uint8_t  tmp_write_protect;    /* temporary write protection  */
  /**
   * \~english File format
   */
  uint8_t  file_format;      /* File format          */
  /**
   * \~english Reserve area 2
   */
  uint8_t  reserve2[3];      /* Reserve 2          */
} SS_CSD;

// Structure for SD device info.
/**
 * \~english Structure for SD device info.
 */
typedef struct _SdInfo {
  /**
   * \~english CID
   */
  SS_CID  cid;    // CID
  /**
   * \~english CSD
   */
  SS_CSD  csd;    // CSD
  /**
   * \~english Device file path
   */
  int8_t  deviceFile[FILEPATH_SIZE];  // Device file path
  /**
   * \~english Current Partion No
   */
  int32_t  part_num;  // Current Partion No
  /**
   * \~english Number of partions
   */
  int32_t  nparts;    // Number of partions
}SS_SdInfo;

// Structure for DISC info.
/**
 * \~english Structure for DISC info.
 */
typedef struct _DiscInfo {
  /**
   * \~english Device file path
   */
  int8_t  deviceFile[FILEPATH_SIZE];  // Device file path
  /**
   * \~english Disc Type(eCD, eDVD, eBD)
   */
  EDISC_TYPE  disc_type;      // Disc Type(eCD, eDVD, eBD)
}SS_DiscInfo;

// Structure that is available when SS_DEV_ANY_USB_EV is published.
/**
 * \~english Structure that is available when SS_DEV_ANY_USB_EV is published.
 */
typedef struct _UsbDeviceInfo {
  /**
   * \~english Insert:TURE, Remove:FALSE
   */
  BOOL        bIsDeviceAvailable;  // Insert:TURE, Remove:FALSE
  /**
   * \~english USB Infomation
   */
  SS_UsbInfo    usb_info;    // USB Infomation
}SS_UsbDeviceInfo;

// Structure for device info.
/**
 * \~english Structure for device info.
 */
typedef union _DeviceInfo {
  /**
   * \~english usb device info
   */
  SS_UsbInfo      usb_info;  // usb device info
  /**
   * \~english sd info
   */
  SS_SdInfo    sd_info;  // sd info
  /**
   * \~english disc info
   */
  SS_DiscInfo    disc_info;  // disc info
  /**
   * \~english fusedav info
   */
  SS_FuseDAVInfo   fusedav_info;  // fusedav info
}SS_DeviceInfo;

// Structure for volume label infomation.
/**
 * \~english Structure for volume label infomation.
 */
typedef struct _VolumeInfo {
  /**
   * \~english volume label's availability
   */
  BOOL     volume_label_availability;  // volume label's availability
  /**
   * \~english volume label
   */
  CHAR    volume_label[SS_DEV_NAME_SIZE];  // volume label
}SS_VolumeInfo;

// Structure that is available when SS_DEV_DETECT*_EV is published.
/**
 * \~english Structure that is available when SS_DEV_DETECT*_EV is published.
 */
typedef struct _MediaDetectInfo {
  /**
   * \~english Deive type(eUSB,eSD...)
   */
  EDEVICE_TYPE   dev_type;      // Deive type(eUSB,eSD...)
  /**
   * \~english Insert:TURE, Remove:FALSE
   */
  BOOL     bIsDeviceAvailable;    // Insert:TURE, Remove:FALSE
  /**
   * \~english Device Infomation
   */
  SS_DeviceInfo  dev_info;      // Device Infomation
  /**
   * \~english Mount path
   */
  CHAR    deviceMountpath[FILEPATH_SIZE];  // Mount path
  /**
   * \~english  Do Not Used.(This member is for compatibility)
   */
  CHAR    filepathName[FILEPATH_SIZE];  // Do Not Used.(This menber is for compatibility)
  /**
   * \~english FuseDav:TRUE, Not FuseDav:FALSE
   */
  BOOL    bIsFuseDav;
  /**
   * \~english result of mount  NORMAL:SS_DEV_OK, ERR:SS_DEV_ERR_MOUNT
   */
  SS_DEV_ERROR  err_code;      // result of mount  NORMAL:SS_DEV_OK, ERR:SS_DEV_ERR_MOUNT
  /**
   * \~english volume infomation (USB mass storage only support)
   */
  SS_VolumeInfo  volume_info;      // volume infomation (USB mass storage only support)
}SS_MediaDetectInfo, *PSS_MediaDetectInfo;

// Structure that is available when SS_DEV_DETECT_USB_NCM_EV is published.
/**
 * \~english Structure that is available when SS_DEV_DETECT_USB_NCM_EV is published.
 */
typedef struct _NcmDetectInfo {
  /**
   * \~english Insert:TURE, Remove:FALSE
   */
  BOOL     bIsDeviceAvailable;    // Insert:TURE, Remove:FALSE
  /**
   * \~english USB Infomation
   */
  SS_UsbInfo  usb_info;      // USB Infomation
  /**
   * \~english Network Name
   */
  int8_t  networkName[SS_DEV_DEVICE_NAME_SIZE];  // Network Name
}SS_NcmDetectInfo;

// Structure that is available when SS_DEV_DETECT_USB__CWORD57__EV is published.
/**
 * \~english Structure that is available when SS_DEV_DETECT_USB__CWORD57__EV is published.
 */
typedef struct __CWORD57_DetectInfo {
  /**
   * \~english Insert:TURE, Remove:FALSE
   */
  BOOL     bIsDeviceAvailable;    // Insert:TURE, Remove:FALSE
  /**
   * \~english USB Infomation
   */
  SS_UsbInfo  usb_info;      // USB Infomation
}SS__CWORD57_DetectInfo;

// Structure that is available when SS_DEV_DETECT_USB_ACM_EV is published.
/**
 * \~english Structure that is available when SS_DEV_DETECT_USB_ACM_EV is published.
 */
typedef struct _AcmDetectInfo {
  /**
   * \~english Insert:TURE, Remove:FALSE
   */
  BOOL          bIsDeviceAvailable;             // Insert:TURE, Remove:FALSE
  /**
   * \~english USB Infomation
   */
  SS_UsbInfo    usb_info;                       // USB Infomation
  /**
   * \~english ACM device file
   */
  int8_t        deviceFile[FILEPATH_SIZE];      // ACM device file
}SS_AcmDetectInfo;

// Structure that is available when SS_DEV_DETECT_USB_DEV_MNG_EV is published.
/**
 * \~english Structure that is available when SS_DEV_DETECT_USB_DEV_MNG_EV is published.
 */
typedef struct _DevMngDetectInfo {
  /**
   * \~english Insert:TURE, Remove:FALSE
   */
  BOOL          bIsDeviceAvailable;             // Insert:TURE, Remove:FALSE
  /**
   * \~english USB Infomation
   */
  SS_UsbInfo    usb_info;                       // USB Infomation
  /**
   * \~english Device Manager device file
   */
  int8_t        deviceFile[FILEPATH_SIZE];      // Device Manager device file
}SS_DevMngDetectInfo;

// Structure that is available when SS_DEV_DETECT_USB_DEV_VEHICLE_CTRL_EV is published.
/**
 * \~english Structure that is available when SS_DEV_DETECT_USB_DEV_VEHICLE_CTRL_EV is published.
 */
typedef struct _VehicleCtrlDetectInfo {
  /**
   * \~english Insert:TURE, Remove:FALSE
   */
  BOOL          bIsDeviceAvailable;             // Insert:TURE, Remove:FALSE
  /**
   * \~english USB Infomation
   */
  SS_UsbInfo    usb_info;                       // USB Infomation
  /**
   * \~english Vehicle control device file
   */
  int8_t        deviceFile[FILEPATH_SIZE];      // Vehicle control device file
}SS_VehicleCtrlDetectInfo;

// Structure that is available when SS_DEV_DETECT_USB_DSRC_APP_EV is published.
/**
 * \~english Structure that is available when SS_DEV_DETECT_USB_DSRC_APP_EV is published.
 */
typedef struct _DsrcAppDetectInfo {
  /**
   * \~english Insert:TURE, Remove:FALSE
   */
  BOOL          bIsDeviceAvailable;             // Insert:TURE, Remove:FALSE
  /**
   * \~english USB Infomation
   */
  SS_UsbInfo    usb_info;                       // USB Infomation
  /**
   * \~english Dsrc Application device file
   */
  int8_t        deviceFile[FILEPATH_SIZE];      // Dsrc Application device file
}SS_DsrcAppDetectInfo;

// Structure that is available when SS_DEV_DETECT_USB_DSRC_CTRL_EV is published.
/**
 * \~english Structure that is available when SS_DEV_DETECT_USB_DSRC_CTRL_EV is published.
 */
typedef struct _DsrcCtrlDetectInfo {
  /**
   * \~english Insert:TURE, Remove:FALSE
   */
  BOOL          bIsDeviceAvailable;             // Insert:TURE, Remove:FALSE
  /**
   * \~english USB Infomation
   */
  SS_UsbInfo    usb_info;                       // USB Infomation
  /**
   * \~english Dsrc Control device file
   */
  int8_t        deviceFile[FILEPATH_SIZE];      // Dsrc Control device file
}SS_DsrcCtrlDetectInfo;

// Structure that is available when SS_DEV_DETECT_USB_IR_VICS_DATA_EV is published.
/**
 * \~english Structure that is available when SS_DEV_DETECT_USB_IR_VICS_DATA_EV is published.
 */
typedef struct _IrVicsDataDetectInfo {
  /**
   * \~english Insert:TURE, Remove:FALSE
   */
  BOOL          bIsDeviceAvailable;             // Insert:TURE, Remove:FALSE
  /**
   * \~english USB Infomation
   */
  SS_UsbInfo    usb_info;                       // USB Infomation
  /**
   * \~english Ir Vics Data device file
   */
  int8_t        deviceFile[FILEPATH_SIZE];      // Ir Vics Data device file
}SS_IrVicsDataDetectInfo;

/**
 * \~english USB overcurrent error info.
 */
typedef  struct  _UsbOverCurrentErrInfo {
  /**
   * \~english error port no
   */
  uint8_t   portNo;    // error port no
} SS_UsbOverCurrentErrInfo;

// Structure that is available when SS_DEV_INFO_USB_NOTIFY_EV is published.
/**
 * \~english Structure that is available when SS_DEV_INFO_USB_NOTIFY_EV is published.
 */
typedef struct _UsbNotifyInfo {
  /**
   * \~english notify code
   */
  SS_NOTIFY_CODE  nofity_code;    // notify code
  /**
   * \~english USB device information's availability
   */
  BOOL      usb_info_availability;  // USB device information's availability
  /**
   * \~english USB device information
   */
  SS_UsbInfo    usb_info;    // USB device information
} SS_UsbNotifyInfo;

// Structure that is available when SS_DEV_DETECT_ANY_USB__CWORD57__ROLE_EV is published.
/**
 * \~english Structure that is available when SS_DEV_DETECT_ANY_USB__CWORD57__ROLE_EV is published.
 */
typedef struct __CWORD57_RoleDetectInfo {
  /**
   * \~english Insert:TURE, Remove:FALSE
   */
  BOOL          bIsDeviceAvailable;             // Insert:TURE, Remove:FALSE
} SS__CWORD57_RoleDetectInfo;

// Structure that is available when SS_DEV_INFO_SD_FORMAT_COMP_EV is published.
/**
 * \~english Structure that is available when SS_DEV_INFO_SD_FORMAT_COMP_EV is published.
 */
typedef struct _SDFormatInfo {
  /**
   * \~english Result of SD Format.
   */
  SS_DEV_ERROR          result;             // Result of SD Format.
} SS_SDFormatInfo;

#endif /* _SS_DEVICEDETECTION_SERVER_H_ */  // NOLINT (build/header_guard)

/** @}*/  // end of device_detection_service
/** @}*/  // end of system_service
/** @}*/  // end of BaseSystem
