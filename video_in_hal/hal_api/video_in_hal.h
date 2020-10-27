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

#ifndef HAL_API_VIDEO_IN_HAL_H_
#define HAL_API_VIDEO_IN_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file  video_in_hal.h
 * @brief API definition of the function to control video
 */

/** @addtogroup graphics_service
 *  @{
 */
/** @addtogroup video_in_hal
 *  @ingroup graphics_service
 *  @{
 */


/* Monitor state */
#define VIH_DISP_MONITOR_ON      (0)  //!< \~english Monitor on
#define VIH_DISP_MONITOR_OFF     (1)  //!< \~english Monitor Off

/* Illumination */
#define VIH_DISP_ILLUMI_INIT     (0)  //!< \~english Not configured
#define VIH_DISP_ILLUMI_DAY      (1)  //!< \~english Day screen
#define VIH_DISP_ILLUMI_NIGHT     (2)  //!< \~english Night screen
#define VIH_DISP_ILLUMI_FOURCE_DAY    (3)  //!< \~english Force day screen
#define VIH_DISP_ILLUMI_FOURCE_NIGHT   (4)  //!< \~english Force night screen

/* Auto dimmer */
#define VIH_DISP_AUTO_DIMMER_OFF    (0)  //!< \~english Auto dimmer off
#define VIH_DISP_AUTO_DIMMER_ON     (1)  //!< \~english Auto dimmer on

/* MUTE */
#define VIH_DISP_MUTE_OFF      (0)  //!< \~english Mute off
#define VIH_DISP_MUTE_ON      (1)  //!< \~english Mute on

/* Back light */
#define VIH_DISP_BACKLIGHT_OFF     (0)  //!< \~english Back light off
#define VIH_DISP_BACKLIGHT_ON     (1)  //!< \~english Back light on

/* Type of video source */
#define VIH_VIDEO_SOURCE_R_CAMERA    (0x00)  //!< \~english Rear video camera
#define VIH_VIDEO_SOURCE_F_CAMERA    (0x01)  //!< \~english Front video camera
#define VIH_VIDEO_SOURCE_S_CAMERA    (0x02)  //!< \~english Side video camera
#define VIH_VIDEO_SOURCE_STOP     (0x03)  //!< \~english Stop the video
#define VIH_VIDEO_SOURCE_DVD     (0x04)  //!< \~english DVD video
#define VIH_VIDEO_SOURCE_BD      (0x05)  //!< \~english BD video
#define VIH_VIDEO_SOURCE_DTV     (0x06)  //!< \~english DTV(Built-in)
#define VIH_VIDEO_SOURCE_EXT_DTV    (0x07)  //!< \~english DTV(External)
#define VIH_VIDEO_SOURCE_SD_VIDEO    (0x08)  //!< \~english SD video
#define VIH_VIDEO_SOURCE_VTR     (0x09)  //!< \~english VTR(Built-in)
#define VIH_VIDEO_SOURCE_VTR2     (0x0A)  //!< \~english VTR2(Built-in)
#define VIH_VIDEO_SOURCE_EXT_VTR    (0x0B)  //!< \~english VTR(External)
#define VIH_VIDEO_SOURCE_EXT_NAVI    (0x0C)  //!< \~english NAVI(External)
#define VIH_VIDEO_SOURCE_EXT     (0x0D)  //!< \~english Extension box
#define VIH_VIDEO_SOURCE__CWORD57__VIDEO    (0x0E)  //!< \~english _CWORD57_(Video)
#define VIH_VIDEO_SOURCE__CWORD57__OUT    (0x0F)  //!< \~english _CWORD57_(Out)
#define VIH_VIDEO_SOURCE__CWORD57__NAVI    (0x10)  //!< \~english _CWORD57_(Navi)
#define VIH_VIDEO_SOURCE_MIRRORLINK    (0x11)  //!< \~english Mirror link
#define VIH_VIDEO_SOURCE_USB_VIDEO    (0x12)  //!< \~english USB video
#define VIH_VIDEO_SOURCE_MIRACAST    (0x16)  //!< \~english Miracast
#define VIH_VIDEO_SOURCE__CWORD27_    (0x17)  //!< \~english _CWORD111_
#define VIH_VIDEO_SOURCE_SD_VIDEO_CPRM   (0x18)  //!< \~english SD source(CPRM)
#define VIH_VIDEO_SOURCE_SD_VIDEO_AVCHD   (0x19)  //!< \~english SD source(AVCHD)
#define VIH_VIDEO_SOURCE_DIAG_CAMERA    (0x1A)  //!< \~english Camera diagnosis
#define VIH_VIDEO_SOURCE_BLANK     (0xFE)  //!< \~english Blank

/* Type of TV system */
#define VIH_VIDEO_TVTYPE_NONE     (0)  //!< \~english Not configured
#define VIH_VIDEO_TVTYPE_NTSC     (1)  //!< \~english NTSC
#define VIH_VIDEO_TVTYPE_PAL     (2)  //!< \~english PAL
#define VIH_VIDEO_TVTYPE_SECAM     (3)  //!< \~english SECAM
#define VIH_VIDEO_TVTYPE_AUTO     (255)  //!< \~english Auto

/* de-interlace */
#define VIH_VIDEO_INTERLACE_DEFAULT    (0)  //!< \~english Interlaced(Default)
#define VIH_VIDEO_INTERLACE_REPLICATE   (1)  //!< \~english Interlaced(Reprica)
#define VIH_VIDEO_INTERLACE_WEAVE    (2)  //!< \~english Interlaced(Weave)
#define VIH_VIDEO_INTERLACE_BOB     (3)  //!< \~english Interlaced(Bob)
#define VIH_VIDEO_INTERLACE_AUTO    (4)  //!< \~english Interlaces(Auto)
#define VIH_VIDEO_INTERLACE_LINE    (5)  //!< \~english Interlaces(Line)

/* Aspect */
#define VIH_VIDEO_ASPECT_NORMAL     (0)  //!< \~english Normal
#define VIH_VIDEO_ASPECT_WIDE     (1)  //!< \~english Wide
#define VIH_VIDEO_ASPECT_FULL     (2)  //!< \~english Full
#define VIH_VIDEO_ASPECT_ZOOM     (3)  //!< \~english Zoom

/* Aspect Ratio */
#define VIH_VIDEO_ASPECT_RATIO_DEFAULT   (0)  //!< \~english Default
#define VIH_VIDEO_ASPECT_RATIO_4_3    (1)  //!< \~english 4:3
#define VIH_VIDEO_ASPECT_RATIO_16_9    (2)  //!< \~english 16:9
#define VIH_VIDEO_ASPECT_RATIO_4_3_LB   (3)  //!< \~english 4:3(Letter box)

/* Enable or disable */
#define VIH_ENABLE      (1)  //!< \~english Enable
#define VIH_DISABLE     (0)  //!< \~english Disable

/* Send information: Control module */
#define VIH_DISP_SEQ_CTRL       0x0003  //!< \~english Sequence control
#define VIH_DISP_VCAP1_CTRL     0x0005  //!< \~english Video capture1 control
#define VIH_DISP_VCAP2_CTRL     0x0006  //!< \~english Video capture2 control

/* Message ID: Put or delete diagnosis code */
#define VIH_DISP_MAIN_IND_PUTDIAG   (0x5200)  //!< \~english Put diagnosis code
#define VIH_DISP_MAIN_IND_DELDIAG   (0x5201)  //!< \~english Delete diagnosis code

/**
 * \~english  Event ID
 */
typedef enum TagVideoInHalEventID {
    VIH_EVENT_ID_SOURCE,           //!< \~english Display source change
    VIH_EVENT_ID_QUALITY,          //!< \~english Set quality(display)
    VIH_EVENT_ID_VIDEOQUALITY,     //!< \~english Set quality(video)
    VIH_EVENT_ID_ASPECT,           //!< \~english Set aspect
    VIH_EVENT_ID_MONITOR,          //!< \~english Set monitor(display)
    VIH_EVENT_ID_VIDEOVISIBLE,     //!< \~english Set video visible(video)
    VIH_EVENT_ID_MUTE,             //!< \~english Set mute
    VIH_EVENT_ID_BACKLIGHT,        //!< \~english Set backlight
    VIH_EVENT_ID_NUM_MAX
} VIH_EVENTID;

/**
 * \~english  Control ID
 */
typedef enum TagVideoInHalControlID {
    VIH_CONTROL_ID_SOURCE,         //!< \~english Display source change
    VIH_CONTROL_ID_QUALITY,        //!< \~english Set quality(display)
    VIH_CONTROL_ID_VIDEOQUALITY,   //!< \~english Set quality(video)
    VIH_CONTROL_ID_ASPECT,         //!< \~english Set aspect
    VIH_CONTROL_ID_MONITOR,        //!< \~english Set monitor(display)
    VIH_CONTROL_ID_VIDEOVISIBLE,   //!< \~english Set video visible(video)
    VIH_CONTROL_ID_START_CAPTURE,  //!< \~english Start capture
    VIH_CONTROL_ID_STOP_CAPTURE,   //!< \~english Stop capture
    VIH_CONTROL_ID_TVTYPE,         //!< \~english Set TV type
    VIH_CONTROL_ID_DEINTERLACE,    //!< \~english Set de-interlace
    VIH_CONTROL_ID_VMUTE,          //!< \~english Set mute
    VIH_CONTROL_ID_BACKLIGHT,      //!< \~english Set backlight
    VIH_CONTROL_ID_NUM_MAX
} VIH_CONTROLID;

/**
 * \~english  Type of video device's return value
 */
typedef enum TagVideoInHalRet {
    VIH_RET_SUCCESS,               //!< \~english OK
    VIH_RET_INVALID_PARAMETER,     //!< \~english Invalid parameter
    VIH_RET_INTERNAL_ERROR,        //!< \~english Internal error
    VIH_RET_NUM_MAX
} VIH_RET;

/**
 * \~english  context kind
 */
typedef enum TagVideoInHalContext {
    VIH_CONTEXT_DISP = 0,  //!< \~english Display
    VIH_CONTEXT_VIDEO,  //!< \~english Video
    VIH_CONTEXT_NUM_MAX
} VIH_CONTEXT;

/**
 * \~english  Display ID
 */
typedef enum TagVideoInHalDispIdx {
    VIH_DISP_IDX_MONITOR0,  //!< \~english Monitor0 (Front monitor)
    VIH_DISP_IDX_MONITOR1,  //!< \~english Monitor1 (Rear monitor)
    VIH_DISP_IDX_MONITOR2,  //!< \~english Monitor2
    VIH_DISP_IDX_MONITOR3,  //!< \~english Monitor3
    VIH_DISP_IDX_MONITOR4,  //!< \~english Monitor4
    VIH_DISP_IDX_MONITOR5,  //!< \~english Monitor5
    VIH_DISP_IDX_NUM_MAX
} VIH_DISP_IDX;

/**
 * \~english  Video ID
 */
typedef enum TagVideoInHalVideoOut {
    VIH_VIDEO_OUT_MONITOR0 = 0,  //!< \~english Monitor0 (Front monitor)
    VIH_VIDEO_OUT_MONITOR1,  //!< \~english Monitor1 (Rear monitor)
    VIH_VIDEO_OUT_MONITOR2,  //!< \~english Monitor2
    VIH_VIDEO_OUT_MONITOR3,  //!< \~english Monitor3
    VIH_VIDEO_OUT_MONITOR4,  //!< \~english Monitor4
    VIH_VIDEO_OUT_MONITOR5,  //!< \~english Monitor5
    VIH_VIDEO_OUT_NONE,  //!< \~english Monitor none
    VIH_VIDEO_OUT_NUM_MAX
} VIH_VIDEO_OUT;

/**
 * \~english  Structure for display tone color
 */
typedef struct TagVideoInHalDispTone {
    int16_t day;   //!< \~english Day screen
    int16_t night;   //!< \~english Night screen
    int16_t force_day;   //!< \~english Force day screen
    int16_t force_night;   //!< \~english Force night screen
} VIH_DISPTONE;

/**
 * \~english  Structure for display tone hue
 */
typedef struct TagVideoInHalDispHue {
    int16_t day;  //!< \~english Day screen
    int16_t night;  //!< \~english Night screen
    int16_t force_day;  //!< \~english Force day screen
    int16_t force_night;  //!< \~english Force night screen
} VIH_DISPHUE;

/**
 * \~english  Structure for display tone contrast
 */
typedef struct TagVideoInHalDispContrast {
    int16_t day;  //!< \~english Day screen
    int16_t night;  //!< \~english Night screen
    int16_t force_day;  //!< \~english Force day screen
    int16_t force_night;  //!< \~english Force night screen
} VIH_DISPCONTRAST;

/**
 * \~english  Structure for display tone brightness
 */
typedef struct TagVideoInHalDispBrightness {
    int16_t day;  //!< \~english Day screen
    int16_t night;  //!< \~english Night screen
    int16_t force_day;  //!< \~english Force day screen
    int16_t force_night;  //!< \~english Force night screen
} VIH_DISPBRIGHTNESS;

/**
 * \~english  Structure for display image quality parameters
 */
typedef struct TagVideoInHalDispParamSetUp {
    u_int8_t source;  //!< \~english Source type for quality setting
    u_int8_t reserved[3];  //!< \~english Reserved
    VIH_DISPTONE tone;  //!< \~english Tone
    VIH_DISPHUE hue;  //!< \~english Hue
    VIH_DISPCONTRAST contrast;  //!< \~english Contrast
    VIH_DISPBRIGHTNESS brightness;  //!< \~english Brightness
} VIH_DISP_PARAM_SETUP;

/**
 * \~english  Structure for display parameters
 */
typedef struct TagVideoInHalDispParam {
    u_int8_t monitor;  //!< \~english Monitor state
    u_int8_t reserve[3];  //!< \~english Reserve
    VIH_DISP_PARAM_SETUP setup;  //!< \~english Quality setting
    u_int8_t illumi;  //!< \~english Illumination
    u_int8_t dimmer;  //!< \~english Auto dimmer
    u_int8_t mute;  //!< \~english Mute
    u_int8_t backlight;  //!< \~english Back light
} VIH_DISPPARAM;

/**
 * \~english  Structure for video tone color
 */
typedef struct TagVideoInHalVideoTone {
    int16_t day;  //!< \~english Day screen
    int16_t night;  //!< \~english Night screen
    int16_t force_day;  //!< \~english Force day screen
    int16_t force_night;  //!< \~english Force night screen
} VIH_VIDEOTONE;

/**
 * \~english  Structure for video tone hue
 */
typedef struct TagVideoInHalVideoHue {
    int16_t day;  //!< \~english Day screen
    int16_t night;  //!< \~english Night screen
    int16_t force_day;  //!< \~english Force day screen
    int16_t force_night;  //!< \~english Force night screen
} VIH_VIDEOHUE;

/**
 * \~english  Structure for video tone contrast
 */
typedef struct TagVideoInHalVideoContrast {
    int16_t day;  //!< \~english Day screen
    int16_t night;  //!< \~english Night screen
    int16_t force_day;  //!< \~english Force day screen
    int16_t force_night;  //!< \~english Force night screen
} VIH_VIDEOCONTRAST;

/**
 * \~english  Structure for video tone brightness
 */
typedef struct TagVideoInHalVideoBrightness {
    int16_t day;  //!< \~english Day screen
    int16_t night;  //!< \~english Night screen
    int16_t force_day;  //!< \~english Force day screen
    int16_t force_night;  //!< \~english Force night screen
} VIH_VIDEOBRIGHTNESS;

/**
 * \~english  Structure for video image quality parameters
 */
typedef struct TagVideoInHalVideoParamSetUp {
    u_int8_t source;  //!< \~english Source type for quality setting
    u_int8_t reserve[3];  //!< \~english reserve
    VIH_VIDEOTONE tone;  //!< \~english Tone
    VIH_VIDEOHUE hue;  //!< \~english Hue
    VIH_VIDEOCONTRAST contrast;  //!< \~english Contrast
    VIH_VIDEOBRIGHTNESS brightness;  //!< \~english Brightness
} VIH_VIDEO_PARAM_SETUP;

/**
 * \~english  Structure for video position and angle
 */
typedef struct TagVideoInHalVideoPos {
    u_int32_t aspect_x;  //!< \~english Display position x
    u_int32_t aspect_y;  //!< \~english Display position y
    u_int32_t aspect_w;  //!< \~english Display width
    u_int32_t aspect_h;  //!< \~english Display height
} VIH_VIDEOPOS;

/**
 * \~english  Structure for video parameters
 */
typedef struct TagVideoInHalVideoParam {
    u_int8_t source;  //!< \~english Video Source
    u_int8_t type;  //!< \~english TV type
    u_int8_t interlace;  //!< \~english De-interlace
    u_int8_t aspect;  //!< \~english Aspect Value
    VIH_VIDEOPOS angle;  //!< \~english Angle
    VIH_VIDEO_PARAM_SETUP setup;  //!< \~english Quality setting
    u_int8_t monitor;  //!< \~english Monitor
    u_int8_t mute;  //!< \~english Mute
    u_int8_t aspect_ratio;  //!< \~english Aspect ratio
    u_int8_t reserve;  //!< \~english Reserve
    u_int32_t window_priority;  //!< \~english Window priority
} VIH_VIDEOPARAM;

/**
 * \~english  Structure for display identify
 */
typedef struct TagVideoInHalDispVideoID {
    VIH_CONTEXT kind;  //!< \~english Context type
    struct timespec tp;  //!< \~english Context create time
} VIH_DISPVIDEOID;

/**
 * \~english  Structure for display information
 */
typedef struct TagVideoInHalDispVideoInfo {
    VIH_DISPVIDEOID context_id;  //!< \~english Context identify
    u_int32_t dsp_idx;  //!< \~english Display index
    u_int32_t chgflg;  //!< \~english Setting change flag
} VIH_DISPVIDEOINFO;

/**
 * \~english  Structure for display context
 */
typedef struct TagVideoInHalDispMngCtx {
    VIH_DISPVIDEOINFO info;  //!< \~english Display information
    VIH_DISPPARAM disp_param[VIH_DISP_IDX_NUM_MAX];  //!< \~english Display parameter
    VIH_VIDEOPARAM video_param[VIH_VIDEO_OUT_NUM_MAX];  //!< \~english Video parameter
} VIH_DISPMNGCTX;

/**
 * \~english  Structure for diagnosis code data
 */
typedef struct TagVideoInHalGRMainReqDiagData {
  uint64_t err_id;  //!< \~english Error ID
  uint16_t videoinhal_code;  //!< \~english Code
  uint8_t reserve1[2];  //!< \~english Reserve
} VIH_GR_MAIN_REQ_DIAGDATA;


/**
 * \ingroup SendDiagCodeCallBack
 * \~english @par Brief
 *        Send diagnosis code call back.
 * \~english @param [in] snd_info
 *        u_int16_t - Send information
 * \~english @param [in] msg_id
 *        u_int32_t - Message ID
 * \~english @param [in] diag_data
 *        VIH_GR_MAIN_REQ_DIAGDATA* - Diagnosis code data
 * \~english @par
 *      u_int16_t snd_info
 * \~english @code
 *      VIH_DISP_SEQ_CTRL      0x0003  // Sequence control
 *      VIH_DISP_VCAP1_CTRL    0x0005  // Video capture1 control
 *      VIH_DISP_VCAP2_CTRL    0x0006  // Video capture2 control
 *          @endcode
 * \~english @par
 *      u_int32_t msg_id
 * \~english @code
 *      VIH_DISP_MAIN_IND_PUTDIAG   (0x5200)  // Put diagnosis code
 *      VIH_DISP_MAIN_IND_DELDIAG   (0x5201)  // Delete diagnosis code
 *          @endcode
 * \~english @par
 *      VIH_GR_MAIN_REQ_DIAGDATA struct
 * \~english @code
 *      typedef struct TagVideoInHalGRMainReqDiagData
 *      {
 *          uint64_t err_id;      // Error ID
 *          uint16_t videoinhal_code;   // Videoinhal code
 *          uint8_t reserve1[2];  // Reserve
 *      } VIH_GR_MAIN_REQ_DIAGDATA;
 *          @endcode
 * \~english @retval 0                           :  OK
 * \~english @retval -1                          :  Invalid parameter
 * \~english @par Detail
 *        - Notify diagnosis code request to graphics_service.
 * \~english @par Prerequisite
 *        - None
 * \~english @par Conditions of processing failure
 *        - When input parameter diag_data is NULL, return with error. [-1]
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        None
 */
typedef int (*SendDiagCodeCallBack)(u_int16_t snd_info, u_int32_t msg_id, VIH_GR_MAIN_REQ_DIAGDATA* diag_data);

/**
 * \ingroup InitVideo
 * \~english @par Brief
 *         API to display device initialize.
 * \~english @param [in] call_back
 *        SendDiagCodeCallBack - Send diagnosis trace code call back function
 * \~english @retval VIH_RET_SUCCESS             :  OK
 * \~english @retval VIH_RET_INVALID_PARAMETER   :  Invalid parameter
 * \~english @par Detail
 *        - Display device initialize, init parameter and threads.
 *        - The API can only be used by 1 process.
 * \~english @par Prerequisite
 *        - None
 * \~english @par Conditions of processing failure
 *        - When display device driver can not be found, return with error. [VIH_RET_INVALID_PARAMETER]
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        None
 */
VIH_RET InitVideo(SendDiagCodeCallBack call_back);

/**
 * \ingroup ControlVideo
 * \~english @par Brief
 *         API to Control display device.
 * \~english @param [in] pctx
 *        VIH_DISPMNGCTX* - Point of parameter used by control display device
 * \~english @param [in] event_id
 *        VIH_EVENTID - Event ID
 * \~english @param [in] control_id
 *        VIH_CONTROLID - Control ID
 * \~english @par
 *      VIH_DISPMNGCTX struct 
 * \~english @code
 *      typedef struct TagVideoInHalDispMngCtx
 *      {
 *          VIH_DISPVIDEOINFO info;                                // Information of display
 *          VIH_DISPPARAM disp_param[VIH_DISP_IDX_NUM_MAX];        // Paramter of diaplay
 *          VIH_VIDEOPARAM video_param[VIH_VIDEO_OUT_NUM_MAX];     // Paramter of video
 *      } VIH_DISPMNGCTX;
 *          @endcode
 * \~english @par
 *      VIH_EVENTID enum
 * \~english @code
 *      typedef enum TagVideoInHalEventID
 *      {
 *          VIH_EVENT_ID_SOURCE,                // Display source change
 *          VIH_EVENT_ID_QUALITY,               // Set quality(display)
 *          VIH_EVENT_ID_VIDEOQUALITY,          // Set quality(video)
 *          VIH_EVENT_ID_ASPECT,                // Set aspect
 *          VIH_EVENT_ID_MONITOR,               // Set monitor(display)
 *          VIH_EVENT_ID_VIDEOVISIBLE,          // Set video visible(video)
 *          VIH_EVENT_ID_MUTE,                  // Set mute
 *          VIH_EVENT_ID_BACKLIGHT,             // Set backlight
 *          VIH_EVENT_ID_NUM_MAX
 *      } VIH_EVENTID;
 *          @endcode
 * \~english @par
 *      VIH_CONTROLID enum
 * \~english @code
 *      typedef enum TagVideoInHalControlID
 *      {
 *          VIH_CONTROL_ID_SOURCE,              // Display source change
 *          VIH_CONTROL_ID_QUALITY,             // Set quality(display) 
 *          VIH_CONTROL_ID_VIDEOQUALITY,        // Set quality(video)
 *          VIH_CONTROL_ID_ASPECT,              // Set aspect   
 *          VIH_CONTROL_ID_MONITOR,             // Set monitor(display) 
 *          VIH_CONTROL_ID_VIDEOVISIBLE,        // Set video visible(video) 
 *          VIH_CONTROL_ID_START_CAPTURE,       // Start capture
 *          VIH_CONTROL_ID_STOP_CAPTURE,        // Stop capture 
 *          VIH_CONTROL_ID_TVTYPE,              // Set TV type
 *          VIH_CONTROL_ID_DEINTERLACE,         // Set de-interlace
 *          VIH_CONTROL_ID_VMUTE,               // Set mute
 *          VIH_CONTROL_ID_BACKLIGHT,           // Set backlight
 *          VIH_CONTROL_ID_NUM_MAX
 *      } VIH_CONTROLID;
 *          @endcode
 * \~english @retval VIH_RET_SUCCESS             :  OK
 * \~english @retval VIH_RET_INVALID_PARAMETER   :  Invalid parameter
 * \~english @retval VIH_RET_INTERNAL_ERROR      :  Internal error
 * \~english @par Detail
 *        - Control display device. Include display source, quality(display), quality(video), aspect , monitor, video visible, start capture, stop capture, mute, backlight, TV type, de-interlace.
 *        - The API can only be used by 1 process.
 * \~english @par Prerequisite
 *        - Call InitVideo() 
 * \~english @par Conditions of processing failure
 *        - The point param is null [VIH_RET_INVALID_PARAMETER]
 *        - The display driver return error [VIH_RET_INTERNAL_ERROR]
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Fire and Forget
 * \~english @see
 *        None
 */
VIH_RET ControlVideo(VIH_DISPMNGCTX* pctx, VIH_EVENTID event_id, VIH_CONTROLID control_id);

/**
 * \ingroup TerminateVideo
 * \~english @par Brief
 *         API to display device Terminate.
 * \~english @param None
 * \~english @retval VIH_RET_SUCCESS             :  OK
 * \~english @par Detail
 *        - Display device Terminate, init parameter.
 *        - The API can only be used by 1 process.
 * \~english @par Prerequisite
 *        - Call InitVideo().
 * \~english @par Conditions of processing failure
 *        - None
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        None
 */
VIH_RET TerminateVideo();

#ifdef __cplusplus
}
#endif

/** @}*/  //!< end of video_in_hal
/** @}*/  //!< end of graphics_service

#endif  // HAL_API_VIDEO_IN_HAL_H_
