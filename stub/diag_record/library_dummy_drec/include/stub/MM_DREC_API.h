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

/////////////////////////////////////////////////////////////////////////////
/// \file MM_DREC_API.h
/////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
 * @file    MM_DREC_API.h
 * @brief   MM diag recorder function API definition
*****************************************************************************/


/*****************************************************************************
 * ---------------
 * -  Die-Record Function API Operation  -
 * ---------------
 *
 * $$ Notes on Creation and Release $$
 *   - Submit and request the created headers and dummy libs (empty functions defined only for I/F)
 *     from DiagService to BaseSystem and build them from BaseSystem.
 *
 *   - The created regular Lib (not dummy Lib) is built from DiagService.
 *
 *     -> Since this API is also used in BaseSystem, the API must be inserted
 *        from the BaseSystem build to avoid errors in JointBuild.
 *
 * $$ Notes on change $$
 *   - When modifying a header without changing the I/F, submit and request only the header to the BaseSystem.
 *
 *   - Submit header and source code for dummy Lib when the header is modified due to an I/F change.
 *
 *   - When the legitimate lib is changed without changing headers and I/F, no need to submit or request to BaseSystem.
 *
 *****************************************************************************/

#if !defined( __MM_DREC_API_H__ )
#define __MM_DREC_API_H__

#include <native_service/frameworkunified_framework_if.h>
#include <stub/PFDREC_API.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/***********************************************************
    Macro Functions
***********************************************************/
#define ENDIAN16(val)   \
  (((val << 8) & 0xFF00) | ((val >> 8) & 0x00FF))  //!< \~english ENDIAN conversion(16bit)

#define ENDIAN32(val)   \
  ((val << 24) & 0xFF000000) | ((val << 8) & 0x00FF0000) | ((val >> 8) & 0x0000FF00) | ((val >> 24) & 0x000000FF)  //!< \~english ENDIAN conversion(32bit)

/***********************************************************
    Data Frame/Size Definition
***********************************************************/

/***********************************************************
    Data Frame Related/Structure (Common)
***********************************************************/
/**
 * \~english ACC OFF information
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_ACC;

/**
 * \~english GPS time information
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_GPS;

/**
 * \~english Frame for searching recording stop and start
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_REC;

/**
 * \~english Common data frame
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_COMMON_DATAFRAME;

/***********************************************************
    Data Frame/Structure Definition
***********************************************************/

/**
 * \~english Power trace data frame
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCPWR;

/**
 * \~english Power trace data frame_system time attached
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCPWR_WITHSYSTIME;

/**
 * \~english Mode control/vehicle signal trace (mode control)
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCMODE;

/**
 * \~english Mode control/vehicle signal trace (vehicle signal)
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCVEHICLESIGNAL;

/**
 * \~english Key trace
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCKEY;

/**
 * \~english Key trace (key information for 23byte)
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCKEY_EXP;

/**
 * \~english Key trace(voice operation)
 */
typedef struct _T_DREC_TRCKEY_VOICE {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCKEY_VOICE;

/**
 * \~english Error trace
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCERR;

/**
 * \~english CAN communication trace
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCCAN;

/**
 * \~english AW Trace data (trace output of the own car position difference/ Monitoring trace log output of the distributed application PF/ Usage trace log output of the distributed application PF/ Trace output of DSRC/ETC communication)
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCAWDATA;

/**
 * \~english Bluetooth trace
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCBTCONNECT;

/**
 * \~english Bluetooth trace
 */
typedef struct _T_DREC_TRCBTINTERNAL {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCBTINTERNAL;

/**
 * \~english Bluetooth trace
 */
typedef struct _T_DREC_TRCBT {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCBT;

/**
 * \~english SSID information
 */
typedef struct _T_DREC_TRCWIFI_SSID_INFO {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCWIFI_SSID_INFO;

/**
 * \~english WiFi trace
 */
typedef struct _T_DREC_TRCWIFI {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCWIFI;

/**
 * \~english Trace data of the version information
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCVERSION;

/**
 * \~english Trace of the radio wave receipt
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCWAVE;

/**
 * \~english GPS information
 */
typedef struct {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCGPS;

/**
 * \~english ETC watch log trace
 */
typedef struct _T_DREC_TRCETCWATCH {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCETCWATCH;

/**
 * \~english ROB log trace
 */
typedef struct _T_DREC_TRCROB {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCROB;

/**
 * \~english Optical beacon down-link information trace
 */
typedef struct _T_DREC_TRCOPTBEACON_DOWN_LINK {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCOPTBEACON_DOWN_LINK;

/**
 * \~english Optical beacon up-link information trace
 */
typedef struct _T_DREC_TRCOPTBEACON_UP_LINK {
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
} T_DREC_TRCOPTBEACON_UP_LINK;

/***********************************************************
    Enumeration definitions
***********************************************************/

/**
 * \~english Diag record API return value
 */
typedef enum {
  RET_DREC_NORMAL = 0,  //!< \~english Normal end
  RET_DREC_ERROR,      //!< \~english Abnormal end
  RET_DREC_ERR_ARGUMENTS,  //!< \~english Incorrect argument
} RET_DREC_API;

/**
 * \~english Diag record ACC state
 */
typedef enum {
  E_DREC_ACC_OFF = 0,    //!< \~english ACC OFF
  E_DREC_ACC_ON,      //!< \~english ACC ON
} E_DREC_ACC;

/**
 * \~english Diag recording function state
 */
typedef enum {
  E_DREC_REC_OFF = 0,      //!< \~english Recording stop
  E_DREC_REC_ON        //!< \~english Recording start
} E_DREC_REC;

/**
 * \~english Maker peculiarity trace output state
 */
typedef enum {
  DREC_OUTPUT_STATE_START = 1,  //!< \~english Output start
  DREC_OUTPUT_STATE_END,  //!< \~english Output end
} DREC_OUTPUT_STATE;

/**
 * \~english Maker peculiarity trace output result
 */
typedef enum {
  DREC_OUTPUT_NOTIFY_OK = 0,  //!< \~english OK
  DREC_OUTPUT_NOTIFY_NG,  //!< \~english NG
} DREC_OUTPUT_RESULT;

/**
 * \~english Maker peculiarity trace function state
 */
typedef enum {
  DREC_OUTPUT_FUNC_ON = 0,  //!< \~english Function ON
  DREC_OUTPUT_FUNC_OFF,  //!< \~english Function OFF
} DREC_OUTPUT_FUNC;

/**
 * \~english Maker peculiarity trace notification restrict
 */
typedef enum {
  DREC_NOTIFY_RESTRICT_API,  //!< \~english restrict API
  DREC_NOTIFY_RESTRICT_CAN,  //!< \~english restrict CAN
  DREC_NOTIFY_RESTRICT_NONE,  //!< \~english non-restrict
} DREC_NOTIFY_RESTRICT;

/***********************************************************
    Constant
***********************************************************/
#define  DREC_COM_GPS_ID_CORD                0xF0  //!< \~english GPS time information identification code
#define DREC_COM_RECORD_STOP                0xF1  //!< \~english Recording stop
#define DREC_COM_RECORD_START                0xF2  //!< \~english Recording start

/***********************************************************
    API
***********************************************************/
/** @addtogroup ApplicationManagement
 *  @{
 */
/** @addtogroup Vehicle
 *  @ingroup ApplicationManagement
 *  @{
 */
/** @addtogroup Diag
 *  @ingroup Vehicle
 *  @{
 */
/** @addtogroup Diag_DrecOperate
 *  @ingroup Diag
 *  @{
 */
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_ChangeRecording
/// \~english @par Brief
/// Instruct the record function start/stop and write the start/stop information.
/// \~english @param [in] state
///   \ref E_DREC_REC - Function start/stop state
/// \~english @par
///   - Set either in the following "Diag recording function states" for the argument (state) (Function start/stop state)
/// \~english @code
///       Diag recording function states
///       E_DREC_REC_OFF     : Recording stop
///       E_DREC_REC_ON      : Recording start
///            @endcode
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @par Prerequisite
///   - \ref PFDREC_OpenSession must be called prior to calling this API.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - None
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   The log output of Diag Record is output in the following format by calling this API.\n
/// \n
/// < Function ON >\n
/// Record "Recording start information" in each trace data. Notify the function start of the recording function to BaseSystem.\n
/// Register GPS time information from Diag service by \ref DREC_WriteTrcGPS.\n
/// \n
/// < Function OFF >\n
/// Record "Recording stop information" in each trace data. Notify the function stop of the recording function to BaseSystem.\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_ChangeRecording(E_DREC_REC state);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_AccOffOn
/// \~english @par Brief
///   Write ACCOFF time information to the memory at ACC OFF.
/// \~english @param [in] state
///   \ref E_DREC_ACC - ACCOFFON (reset) state
/// \~english @par
///   - Set either in the following "Diag record ACC state" for the argument (state) (ACCOFFON (reset) state)
/// \~english @code
///       Diag record ACC state
///       E_DREC_ACC_OFF    : ACC OFF
///       E_DREC_ACC_ON     : ACC ON
///            @endcode
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - None
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
/// Wrapper executes the following processes by calling this API. \n
/// This API must be called when this Diag service detects FrameworkunifiedOnStop. (Called only by the service having Diag recorder function.) \n
/// Wrapper writes ACCOFF information in each trace file. \n
/// Wrapper writes #power OFF state by the power trace output based on the notification from SYS. \n
/// Wrapper writes GPS time at Function OFF and at ACCOFF (reset) in preparation for a case that GPS time information cannot be acquired at the start up. \n
/// Wrapper does not reaquire GPS time if valid GPS time cannot be acquired at the startup. \n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_AccOffOn(E_DREC_ACC state);
/** @}*/  // end of Diag_DrecOperate
/** @addtogroup Diag_DrecWriteTrc
 *  @ingroup Diag
 *  @{
 */
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcPwr
/// \~english @par Brief
///     Write the power trace output information to the memory.
/// \~english @param [in] data
///     \ref T_DREC_TRCPWR* - Pointer to the power trace data
/// \~english @param [in] num
///     u_int32_t - Number of written frames (1 to 100)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the power trace output information to the memory.
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcPwr(T_DREC_TRCPWR* data, u_int32_t num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcPwr_Tick
/// \~english @par Brief
///     Write the power trace output information to the memory (tick specification exists).
/// \~english @param [in] data
///     \ref T_DREC_TRCPWR_WITHSYSTIME* - Pointer to the power trace data (with the system time)
/// \~english @param [in] num
///     u_int32_t - Number of written frames (1 to 100)
/// \~english @param [in] tick
///     u_int32_t - TICK value of the current SYS internal
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///     - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync only(None communication)
/// \~english @par Detail
///     Write the power trace output information to the memory (tick specification exists).
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcPwr_Tick(T_DREC_TRCPWR_WITHSYSTIME* data,
                                          u_int32_t num, u_int32_t tick);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcMode
/// \~english @par Brief
///   Write the mode control parts of the mode control/vehicle signal trace output information to the memory.
/// \~english @param [in] data
///   \ref T_DREC_TRCMODE* - The pointer to mode management trace data
/// \~english @param [in] num
///   u_int32_t - Number of written frames (1 to 100)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the mode control parts of the mode control/vehicle signal trace output information to the memory.\n
///   The system time is gotten by wrapper library and added.\n
///   This API adds the vehicle signal state fixed at 0xFF.\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcMode(T_DREC_TRCMODE* data, u_int32_t num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcVehicleSignal
/// \~english @par Brief
///   Write the vehicle signal parts of the mode control/vehicle signal trace output information to the memory.
/// \~english @param [in] data
/// \ref T_DREC_TRCVEHICLESIGNAL* - Pointer to the vehicle signal trace data
/// \~english @param [in] num
/// u_int32_t - Number of written frames (1 to 100)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the vehicle signal parts of the mode control/vehicle signal trace output information to the memory.\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcVehicleSignal(T_DREC_TRCVEHICLESIGNAL* data,
                                               u_int32_t num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcKey
/// \~english @par Brief
///   Write the key trace output information to the memory (for 11byte key information).
/// \~english @param [in] data
///   \ref T_DREC_TRCKEY* - The pointer to key trace data (Key information for 11 bytes)
/// \~english @param [in] num
///   u_int32_t - Number of written frames (1 to 100)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the key trace output information to the memory (for 11byte key information).\n
///   (The caller uses only when size of the key information is 11 bytes. For data which it is 27 bytes, the caller uses \ref DREC_WriteTrcKey_Exp .)\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcKey_Exp
/// \~english @par Brief
///   Write the key trace output information to the memory (for 23byte key information).
/// \~english @param [in] data
///   \ref T_DREC_TRCKEY_EXP* - The pointer to key trace data (Key information for 23 bytes)
/// \~english @param [in] num
///   u_int32_t - Number of written frames (1 to 50)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the key trace output information to the memory (for 23byte key information).\n
///   (For data which key information is 11 bytes, the caller uses \ref DREC_WriteTrcKey .)\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcKey_Exp(T_DREC_TRCKEY_EXP* data,
                                         u_int32_t num);


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcKey_Voice
/// \~english @par Brief
///     Write the voice trace output information to the memory(for voice operation).
/// \~english @param[in] data
///     T_DREC_TRCKEY_VOICE* - The pointer to key trace data()
/// \~english @param[in] num
///     u_int32_t - Number of written frames
/// \~english @par
///     Number of written frames (1 to 50)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
///      - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///      - The internal state is not changed.
/// \~english @par Conditions of processing failure
///      - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///      - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync only(None communication)
/// \~english @par Detail
///     Write the voice trace output information to the memory(for voice operation).
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcKey_Voice(T_DREC_TRCKEY_VOICE* data,
                                           u_int32_t num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcErr
/// \~english @par Brief
///   Write the error trace output information to the memory.
/// \~english @param [in] data
///   \ref T_DREC_TRCERR* - The pointer to error trace data
/// \~english @param [in] num
///   u_int32_t - Number of written frames (1 to 100)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DIAG_API_DCCC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the error trace output information to the memory.
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcErr(T_DREC_TRCERR* data, u_int32_t num);


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcCan
/// \~english @par Brief
///   Write CAN communication trace output information to the memory.
/// \~english @param [in] data
/// \ref T_DREC_TRCCAN* - Pointer to CAN communication trace data
/// \~english @param [in] num
/// u_int32_t - Number of written frames (1 to 100)
/// \~english @param [in] tick
/// u_int32_t - TICK value of the current SYS internal
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DIAG_API_DCCC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write CAN communication trace output information to the memory.\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcCan(T_DREC_TRCCAN* data, u_int32_t num,
                                     u_int32_t tick);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcDeliveryPFWatchLog
/// \~english @par Brief
///   Write the monitoring trace log output information of the distributed application PF to the memory.
/// \~english @param [in] data
///   \ref T_DREC_TRCAWDATA* - The pointer to monitoring log data of the distribution application PF
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the monitoring trace log output information of the distributed application PF to the memory.\n
///   The library side adds the system time. The caller must call for each segment of data.\n
///   The library side adds the system time to the beginning of the data and ALL 0 to from the head of the remaining data to 3 bytes.\n
///   This API adds the system time to the beginning with a single call and regarding data exceeding one frame, divides and registers it.\n
///   The caller must not call some headers and data record parts together. (Because system time can not be added for each section.)\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcDeliveryPFWatchLog(T_DREC_TRCAWDATA* data);


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcDeliveryPFUseLog
/// \~english @par Brief
///   Write the usage trace log output information of the distributed application PF to the memory.
/// \~english @param [in] data
///   \ref T_DREC_TRCAWDATA* - The pointer to usage log data of the distribution application PF
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the usage trace log output information of the distributed application PF to the memory.\n
///   The library side adds the system time. The caller must call for each segment of data.\n
///   The library side adds the system time to the beginning of the data and ALL 0 to from the head of the remaining data to 3 bytes.\n
///   This API adds the system time to the beginning with a single call and regarding data exceeding one frame, divides and registers it.\n
///   The caller must not call some headers and data record parts together. (Because system time can not be added for each section.)\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcDeliveryPFUseLog(T_DREC_TRCAWDATA* data);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcBT
/// \~english @par Brief
///     Write the trace output information of Bluetooth to the memory.
/// \~english @param[in] data
///     T_DREC_TRCBT* - The pointer to write data
/// \~english @param[in] num
///     u_int32_t - Number of written frames
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
///      - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///      - The internal state is not changed.
/// \~english @par Conditions of processing failure
///      - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///      - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync only(None communication)
/// \~english @par Detail
///     Write the trace output information of Bluetooth to the memory.
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcBT(T_DREC_TRCBT* data, u_int32_t num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcBTInternal
/// \~english @par Brief
///   (Only in the specific Supplier) Write the trace output information of Bluetooth internal to the memory.
/// \~english @param [in] data
///   u_int8_t* - The pointer to write data
/// \~english @param [in] size
///   u_int32_t - Data size (1 to 1200bytes)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   This API is API for specific supplier.(TECHWG-443)\n
///   Write the trace output information of Bluetooth internal to the memory.\n
///   The log output of Diag Record is output in the following format.\n
///   This API registers system time from Byte1 to 3.\n
///   As there is data reserved for each format, this API registers the value defined in the wrapper layer at Byte4\n
///   so that it does not overlap with that data.\n
///   This API registers specified data after Byte5.\n
///   Regarding data exceeding 12 bytes at a time, this API divides records and registers it.\n
///   Regarding data after Byte13, this API registers 0x00 from Byte1 to 4 and registers the specified data after Byte5.\n
///   It is filled with 0x00 if the data is less than 16 bytes.\n
///   This API adds the system time to the beginning with a single call.\n
///   Regarding data exceeding one frame, this API divides and registers it.\n
///   The caller must not call some data by connecting them. (Because system time can not be added for each section.)\n
///   (The format is optional for the manufacturer.)
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcBTInternal(u_int8_t* data,
                                            u_int32_t size);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcWiFi
/// \~english @par Brief
///     Write the trace output information of WiFi to the memory.
/// \~english @param[in] data
///     T_DREC_TRCWIFI* - The pointer to write data
/// \~english @param[in] num
///     u_int32_t - Number of written frames
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
///      - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///      - The internal state is not changed.
/// \~english @par Conditions of processing failure
///      - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///      - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync only(None communication)
/// \~english @par Detail
///     Write the trace output information of WiFi to the memory.\n
///     \ref T_DREC_TRCWIFI Structure\n
///     Data frame format are different by setting value of WiFi setting state.\n
///     WiFi Setting state( \ref T_DREC_TRCWIFI.processing )\n
///      - Connection(0x00), Auto connection(0x01) and Disconnection(0x02) data format definition \ref notinitialdata used.\n
///      - Creation Mode(0x03) and Deletion Mode((0x04)) data format definition \ref modedata used.\n
///      - Initial is activation(0x10) format definition \ref initialdata used.
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcWiFi(T_DREC_TRCWIFI* data, u_int32_t num);


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcWiFiInternal
/// \~english @par Brief
///   (Only in the specific Supplier) Write the trace output information of WiFi internal to the memory.
/// \~english @param [in] data
///   u_int8_t* - The pointer to write data
/// \~english @param [in] size
///   u_int32_t - Data size (1 to 1200bytes)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   This API is API for specific supplier.(TECHWG-443)\n
///   Write the trace output information of WiFi internal to the memory.\n
///   The log output of Diag Record is output in the following format.\n
///   This API registers system time from Byte1 to 3.\n
///   As there is data reserved for each format, this API registers the value defined in the wrapper layer at Byte4\n
///   so that it does not overlap with that data.\n
///   This API registers specified data after Byte5.\n
///   Regarding data exceeding 12 bytes at a time, this API divides records and registers it.\n
///   Regarding data after Byte13, this API registers 0x00 from Byte1 to 4 and registers the specified data after Byte5.\n
///   It is filled with 0x00 if the data is less than 16 bytes.\n
///   This API adds the system time to the beginning with a single call.\n
///   Regarding data exceeding one frame, this API divides and registers it.\n
///   The caller must not call some data by connecting them. (Because system time can not be added for each section.)\n
///   (The format is optional for the manufacturer.)
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcWiFiInternal(u_int8_t* data, u_int32_t size);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcCenter
/// \~english @par Brief
///   Write the trace output information of the communication with the center to the memory.
/// \~english @param [in] data
///   u_int8_t* - The pointer to write data
/// \~english @param [in] size
///   u_int32_t - Data size (1 to 1200bytes)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the trace output information of the communication with the center to the memory.\n
///   The log output of Diag Record is output in the following format.\n
///   This API registers system time from Byte1 to 3.\n
///   As there is data reserved for each format, this API registers the value defined in the wrapper layer at Byte4\n
///   so that it does not overlap with that data.\n
///   This API registers specified data after Byte5.\n
///   Regarding data exceeding 12 bytes at a time, this API divides records and registers it.\n
///   Regarding data after Byte13, this API registers 0x00 from Byte1 to 4 and registers the specified data after Byte5.\n
///   It is filled with 0x00 if the data is less than 16 bytes.\n
///   This API adds the system time to the beginning with a single call.\n
///   Regarding data exceeding one frame, this API divides and registers it.\n
///   The caller must not call some data by connecting them. (Because system time can not be added for each section.)\n
///   (The format is optional for the manufacturer.)
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcCenter(u_int8_t* data, u_int32_t size);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcVersion
/// \~english @par Brief
///   Write the trace output information of the version information to the memory.
/// \~english @param [in] data
/// \ref T_DREC_TRCVERSION* - Pointer to the Trace data of the version information
/// \~english @param [in] num
/// u_int32_t - Number of written frames (1 to 25)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the trace output information of the version information to the memory.\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcVersion(T_DREC_TRCVERSION* data,
                                         u_int32_t num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcDCM
/// \~english @par Brief
///   Write the trace output information of DCM communication to the memory.
/// \~english @param [in] data
///   u_int8_t* - The pointer to write data
/// \~english @param [in] size
///   u_int32_t - Data size (1 to 1200bytes)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the trace output information of DCM communication to the memory.\n
///   The log output of Diag Record is output in the following format.\n
///   This API registers system time from Byte1 to 3.\n
///   As there is data reserved for each format, this API registers the value defined in the wrapper layer at Byte4\n
///   so that it does not overlap with that data.\n
///   This API registers specified data after Byte5.\n
///   Regarding data exceeding 12 bytes at a time, this API divides records and registers it.\n
///   Regarding data after Byte13, this API registers 0x00 from Byte1 to 4 and registers the specified data after Byte5.\n
///   It is filled with 0x00 if the data is less than 16 bytes.\n
///   This API adds the system time to the beginning with a single call.\n
///   Regarding data exceeding one frame, this API divides and registers it.\n
///   The caller must not call some data by connecting them. (Because system time can not be added for each section.)\n
///   (The format is optional for the manufacturer.)
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcDCM(u_int8_t* data, u_int32_t size);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcDSRC
/// \~english @par Brief
///   Write the trace output information of DSRC/ETC communication to the memory.
/// \~english @param [in] data
///   \ref T_DREC_TRCAWDATA* - The pointer to DSRC/ETC communication trace data
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the trace output information of DSRC/ETC communication to the memory.\n
///   The log output of Diag Record is output in the following format.\n
///   This API registers system time from Byte1 to 3.\n
///   As there is data reserved for each format, this API registers the value defined in the wrapper layer at Byte4\n
///   so that it does not overlap with that data.\n
///   This API registers specified data after Byte5.\n
///   Regarding data exceeding 12 bytes at a time, this API divides records and registers it.\n
///   Regarding data after Byte13, this API registers 0x00 from Byte1 to 4 and registers the specified data after Byte5.\n
///   It is filled with 0x00 if the data is less than 16 bytes.\n
///   This API adds the system time to the beginning with a single call.\n
///   Regarding data exceeding one frame, this API divides and registers it.\n
///   The caller must not call some data by connecting them. (Because system time can not be added for each section.)\n
///   (The format is optional for the manufacturer.)
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcDSRC(T_DREC_TRCAWDATA* data);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrc_CWORD17_
/// \~english @par Brief
///   Write the trace output information of _CWORD5_ communication to the memory.
/// \~english @param [in] data
/// \ref T_DREC_TRCCAN* - Pointer to the _CWORD5_ communication trace data
/// \~english @param [in] num
///   u_int32_t - Number of written frames (1 to 100)
/// \~english @param [in] tick
/// u_int32_t - TICK value of the current SYS internal
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the trace output information of _CWORD5_ communication to the memory.\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrc_CWORD17_(T_DREC_TRCCAN* data, u_int32_t num,
                                          u_int32_t tick);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcWave
/// \~english @par Brief
///   Write the trace output information of the radio wave receipt to the memory.
/// \~english @param [in] data
///   \ref T_DREC_TRCWAVE* - The pointer to trace data of the electric wave reception
/// \~english @param [in] num
///   u_int32_t - Number of written frames (1 to 100)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the trace output information of the radio wave receipt to the memory.\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcWave(T_DREC_TRCWAVE* data, u_int32_t num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcUSB
/// \~english @par Brief
///   Write the trace output information of USB I/F connected application internal to the memory.
/// \~english @param [in] data
///   u_int8_t* - The pointer to wrire data
/// \~english @param [in] size
///   u_int32_t - Data size (1 to 1200bytes)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the trace output information of USB I/F connected application internal to the memory.\n
///   The log output of Diag Record is output in the following format.\n
///   This API registers system time from Byte1 to 3.\n
///   As there is data reserved for each format, this API registers the value defined in the wrapper layer at Byte4\n
///   so that it does not overlap with that data.\n
///   This API registers specified data after Byte5.\n
///   Regarding data exceeding 12 bytes at a time, this API divides records and registers it.\n
///   Regarding data after Byte13, this API registers 0x00 from Byte1 to 4 and registers the specified data after Byte5.\n
///   It is filled with 0x00 if the data is less than 16 bytes.\n
///   This API adds the system time to the beginning with a single call.\n
///   Regarding data exceeding one frame, this API divides and registers it.\n
///   The caller must not call some data by connecting them. (Because system time can not be added for each section.)\n
///   (The format is optional for the manufacturer.)
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcUSB(u_int8_t* data, u_int32_t size);


/** @}*/  // end of Diag_DrecWriteTrc
/** @addtogroup Diag_DrecOperate
 *  @ingroup Diag
 *  @{
 */

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_GetRecordSize
/// \~english @par Brief
///   Acquire the number of Diag record registration whose type is specified.
/// \~english @param [in] type
/// \ref E_PFDREC_CATEGORY_TYPE - Type to acquire the record size
/// \~english @param [out] num
/// u_int32_t * - Pointer to the number of registered records.
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
///   - \ref PFDREC_OpenSession must be called prior to calling this API.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
/// - The argument (num) is NULL. [RET_DREC_ERROR]
/// - The argument (type) is not the defined value. [RET_DREC_ERROR]
/// - The caller is not calling \ref PFDREC_OpenSession in advance. [RET_DREC_ERROR]
/// - Message synchronous communication (FrameworkunifiedInvokeSync) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
/// Acquire the number of record registration whose type is specified.\n
/// In BaseSystem, returns the number of records of the specified type. \n
/// The caller determines the total size with the number of records and each record length.\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_GetRecordSize(E_PFDREC_CATEGORY_TYPE type,
                                       u_int32_t *num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_GetRecordData
/// \~english @par Brief
///   Acquire the registered Diag record whose type is specified.
/// \~english @param [in] type
/// \ref E_PFDREC_CATEGORY_TYPE - Type to acquire the record size (\ref E_PFDREC_CATEGORY_TRC_PWR - ref E_PFDREC_CATEGORY_TRC_TRAF )
/// \~english @param [in] num
/// u_int32_t - The number of records to acquire (1 to 100)
/// \~english @param [in] record_ptr
/// u_int32_t - The start point of the record to acquire
/// \~english @param [out] data
/// u_int8_t *- Pointer to the storage area for the record to acquire
/// \~english @param [out] out_num
/// u_int32_t *- Pointer to the size of the stored record
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
///   - \ref PFDREC_OpenSession must be called prior to calling this API.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
/// - The argument (data) is NULL [RET_DREC_ERROR]
/// - The argument (out_num) is NULL [RET_DREC_ERROR]
/// - The argument (type) is not the defined value [RET_DREC_ERROR]
/// - The argument (num) is 0 or greater than 100. [RET_DREC_ERROR]
/// - The caller is not calling \ref PFDREC_OpenSession in advance. [RET_DREC_ERROR]
/// - Message synchronous communication (FrameworkunifiedInvokeSync) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
/// Acquire the record whose type is specified. \n
/// In BaseSystem, acquire the records of the specified type and the specified number. In this case, the caller specifies from which number of record is acquired.\n
/// The caller is supposed to acquire data in order from the beginning in several batches.\n
/// When the number of record requested to acquire is more than that of registered records, acquire the registered number only and returns it. \n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_GetRecordData(E_PFDREC_CATEGORY_TYPE type,
                                       u_int32_t num, u_int32_t record_ptr,
                                       u_int8_t *data, u_int32_t *out_num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_GetRecordState
/// \~english @par Brief
///   Acquire the start/stop state of the record function.
/// \~english @param [out] state
/// \ref E_DREC_REC* - Pointer to the start/stop state of the record function.
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - \ref PFDREC_OpenSession must be called prior to calling this API.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
/// - The argument (state) is NULL [RET_DREC_ERROR]
/// - The caller is not calling \ref PFDREC_OpenSession in advance. [RET_DREC_ERROR]
/// - Message synchronous communication (FrameworkunifiedInvokeSync) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Acquire the start/stop state of the record function.
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_GetRecordState(E_DREC_REC* state);
/** @}*/  // end of Diag_DrecOperate
/** @addtogroup Diag_DrecOperate
 *  @ingroup Diag
 *  @{
 */
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_SetSysemTimeDifference
/// \~english @par Brief
///   Set the system time difference.
/// \~english @param [in] data
///   u_int32_t - System time difference
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @par Prerequisite
///   - None
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - None
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
/// Store the system time difference in the shared memory.\n
/// Reflect the stored value in the addition process of the system time.\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_SetSysemTimeDifference(u_int32_t data);
/** @}*/  // end of Diag_DrecOperate
/** @addtogroup Diag_DrecWriteTrc
 *  @ingroup Diag
 *  @{
 */
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcGPS
/// \~english @par Brief
///   Set GPS time information to each trace data.
/// \~english @param [in] data
///   \ref T_DREC_TRCGPS* - Pointer to the GPS information
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
/// Set the GPS time information \n
/// The target is all trace outputs belonging to the unit in which this API is called. \n
/// When registering the trace data in BaseSystem, regarding the key trace, GPS information is separately passed as the argument (The second argument of PFDREC_ComDataBuckup).\n
/// After Function ON, when a valid GPS time can be acquired after ACCON, the Diag service must register it.\n
/// During Function ON, when a valid GPS time cannot be acquired, retry must be performed until the acquired time is registered.\n
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcGPS(T_DREC_TRCGPS* data);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcUSBETC
/// \~english @par Brief
///   Write USB log information of DSRC/ETC communication trace output into the memory.
/// \~english @param [in] data
///   \ref T_DREC_TRCAWDATA* - The pointer to DSRC/ETC communication trace data
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write USB log information of DSRC/ETC communication trace output into the memory.\n
///   The log output of Diag Record is output in the following format.\n
///   This API registers system time from Byte1 to 3.\n
///   As there is data reserved for each format, this API registers the value defined in the wrapper layer at Byte4\n
///   so that it does not overlap with that data.\n
///   This API registers specified data after Byte5.\n
///   Regarding data exceeding 12 bytes at a time, this API divides records and registers it.\n
///   Regarding data after Byte13, this API registers 0x00 from Byte1 to 4 and registers the specified data after Byte5.\n
///   It is filled with 0x00 if the data is less than 16 bytes.\n
///   This API adds the system time to the beginning with a single call.\n
///   Regarding data exceeding one frame, this API divides and registers it.\n
///   The caller must not call some data by connecting them. (Because system time can not be added for each section.)\n
///   (The format is optional for the manufacturer.)
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcUSBETC(T_DREC_TRCAWDATA* data);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcUplink
/// \~english @par Brief
///   Write the uplink log information of DSRC/ETC communication trace output into the memory.
/// \~english @param [in] data
///   \ref T_DREC_TRCAWDATA* - The pointer to DSRC/ETC communication trace data
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
///   - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///   - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Write the uplink log information of DSRC/ETC communication trace output into the memory.\n
///   The log output of Diag Record is output in the following format.\n
///   This API registers system time from Byte1 to 3.\n
///   As there is data reserved for each format, this API registers the value defined in the wrapper layer at Byte4\n
///   so that it does not overlap with that data.\n
///   This API registers specified data after Byte5.\n
///   Regarding data exceeding 12 bytes at a time, this API divides records and registers it.\n
///   Regarding data after Byte13, this API registers 0x00 from Byte1 to 4 and registers the specified data after Byte5.\n
///   It is filled with 0x00 if the data is less than 16 bytes.\n
///   This API adds the system time to the beginning with a single call.\n
///   Regarding data exceeding one frame, this API divides and registers it.\n
///   The caller must not call some data by connecting them. (Because system time can not be added for each section.)\n
///   (The format is optional for the manufacturer.)
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
///   None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcUplink(T_DREC_TRCAWDATA* data);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcETCWatch
/// \~english @par Brief
///     Write trace output of the ETC watch to the memory.
/// \~english @param[in] data
///     T_DREC_TRCETCWATCH* - Pointer to  the trace data of the ETC watch.
/// \~english @param[in] num
///     u_int32_t - Number of written frames
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
///      - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///      - The internal state is not changed.
/// \~english @par Conditions of processing failure
///      - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///      - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync only(None communication)
/// \~english @par Detail
///     Write the monitoring trace output information of the ETC to the memory.\n
///     \ref T_DREC_TRCETCWATCH structure\n
///     Data format is different by device type / frame type( \ref T_DREC_TRCETCWATCH.frameType )\n
///     0:ETC status notification log \n
///     1:ETC latest charge notification log \n
///     2:ETC operation response log \n
///     Use the \ref T_DREC_TRCETCWATCH.command format destination.\n
///     3:ETC communication log  \n
///     Use the \ref T_DREC_TRCETCWATCH.com_command format destination.\n
///     4:Diag Clear event log \n
///     5:Diagnosis detection event log \n
///     6:Diagnosis recovery event log \n
///     Use the \ref T_DREC_TRCETCWATCH.diag format destination.
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcETCWatch(T_DREC_TRCETCWATCH* data,
                                          u_int32_t num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcRoB
/// \~english @par Brief
///     Write the trace output information of RoB log to the memory.
/// \~english @param[in] data
///     T_DREC_TRCROB* - The pointer to write data
/// \~english @param[in] num
///     u_int32_t - Number of written frames
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
///      - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///      - The internal state is not changed.
/// \~english @par Conditions of processing failure
///      - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///      - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync only(None communication)
/// \~english @par Detail
///     Write the trace output information of RoB log to the memory.
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcRoB(T_DREC_TRCROB* data,
                                     u_int32_t num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcEvaluation
/// \~english @par Brief
///     Write the evaluation trace output information to the memory.
/// \~english @param[in] data
///     u_int8_t* - The pointer to write data
/// \~english @param[in] size
///     u_int32_t - Data size (1 to 1200bytes)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
///      - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///      - The internal state is not changed.
/// \~english @par Conditions of processing failure
///      - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///      - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync only(None communication)
/// \~english @par Detail
///     Write the evaluation trace output information to the memory.\n
///     The log output of Diag Record is output in the following format.\n
///     This API registers system time from Byte1 to 3.\n
///     As there is data reserved for each format, this API registers the value defined in the wrapper layer at Byte4\n
///     so that it does not overlap with that data.\n
///     This API registers specified data after Byte5.\n
///     Regarding data exceeding 12 bytes at a time, this API divides records and registers it.\n
///     Regarding data after Byte13, this API registers 0x00 from Byte1 to 4 and registers the specified data after Byte5.\n
///     It is filled with 0x00 if the data is less than 16 bytes.\n
///     This API adds the system time to the beginning with a single call.\n
///     Regarding data exceeding one frame, this API divides and registers it.\n
///     The caller must not call some data by connecting them. (Because system time can not be added for each section.)\n
///     (The format is optional for the manufacturer.)
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcEvaluation(u_int8_t* data,
                                            u_int32_t size);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_DeleteTrcDeliveryPFUseLog
/// \~english @par Brief
///   Delete the trace of distribution application PF usage log.
/// \~english @param None
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
///   - \ref PFDREC_OpenSession must be called prior to calling this API.
/// \~english @par Change of internal state
///   - The internal state is not changed.
/// \~english @par Conditions of processing failure
/// - The argument (func) is NULL [RET_DREC_ERROR]
/// - The caller is not calling \ref PFDREC_OpenSession in advance. [RET_DREC_ERROR]
/// - Message synchronous communication (FrameworkunifiedInvokeSync) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///   Public
/// \~english @par Type
///   Sync only(None communication)
/// \~english @par Detail
///   Delete the trace of distribution application PF usage log.
/// \~english @par
/// - include MM_DREC_API.h
/// - Library libMM_DREC_API.so
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_DeleteTrcDeliveryPFUseLog(void);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_DeleteTrcDeliveryPFUseLog_inSession
/// \~english @par Brief
///      Delete the trace of distribution application PF usage log.
///      - Execute "Session start(\ref PFDREC_OpenSession)" before the deletion.
///      - Execute "Session termination(\ ref PFDREC_CloseSession)" after the deletion.
/// \~english @param None
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
/// - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///      - The internal state is not changed.
/// \~english @par Conditions of processing failure
///      - \ref PFDREC_OpenSession failed. [RET_DREC_ERROR]
///      - \ref PFDREC_CloseSession failed. [RET_DREC_ERROR]
///      - Message synchronous communication (FrameworkunifiedInvokeSync) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///      Public
/// \~english @par Type
///      Sync only(None communication)
/// \~english @par Detail
///      Execute "Session starti(\ref PFDREC_OpenSession)" before the deletion.\n
///      Delete the trace of distribution application PF usage log \n
///      Execute "Session termination(\ref PFDREC_CloseSession)" after the deletion. \n
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///      None
////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_DeleteTrcDeliveryPFUseLog_inSession(HANDLE hThread);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcOptBeaconDownLink
/// \~english @par Brief
///     Write Optical beacon down-link information trace to the memory.
/// \~english @param[in] data
///     T_DREC_TRCOPTBEACON_DOWN_LINK* - Pointer to  the trace data of the Optical beacon down-link information.
/// \~english @param[in] num
///     u_int32_t - Number of written frames
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
///      - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///      - The internal state is not changed.
/// \~english @par Conditions of processing failure
///      - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///      - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync only(None communication)
/// \~english @par Detail
///     Write the Optical beacon down-link information trace to the memory.\n
///     \ref T_DREC_TRCOPTBEACON_DOWN_LINK structure\n
///     Data format is different by rcv type( \ref T_DREC_TRCOPTBEACON_DOWN_LINK.rcvkind )\n
///     01h : Route signal information \n
///     Use the  \ref T_DREC_TRCOPTBEACON_DOWN_LINK.downlinkinfo format destination.\n
///     02h : Lack route signal information\n
///     03h : No route signal information  \n
///     Use the  \ref T_DREC_TRCOPTBEACON_DOWN_LINK.rcvgpspositioningdata format destination.\n
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcOptBeaconDownLink(
    T_DREC_TRCOPTBEACON_DOWN_LINK* data, u_int32_t num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_WriteTrcOptBeaconUpLink
/// \~english @par Brief
///     Write Optical beacon up-link information trace to the memory.
/// \~english @param[in] data
///     T_DREC_TRCOPTBEACON_UP_LINK* - Pointer to  the trace data of the  Optical beacon up-link information.
/// \~english @param[in] num
///     u_int32_t - Number of written frames
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @par Prerequisite
///      - This API must be called only when Availability of PFDRECThread is TRUE.
/// \~english @par Change of internal state
///      - The internal state is not changed.
/// \~english @par Conditions of processing failure
///      - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///      - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync only(None communication)
/// \~english @par Detail
///     Write the Optical beacon up-link information trace to the memory.
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_WriteTrcOptBeaconUpLink(T_DREC_TRCOPTBEACON_UP_LINK* data,
                                                 u_int32_t num);

/** @}*/  // end of Diag_DrecWriteTrc
/** @addtogroup Diag_Drec_MakerTrace
 *  @ingroup Diag
 *  @{
 */
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_MakerTrace_DeliveryEntry
/// \~english @par Brief
///     Register the delivery destination application for Maker peculiarity trace
/// \~english @param[in] hApp
///     HANDLE - The handle for the delivery source application
/// \~english @param[in] notifyName
///     PCSTR - The thread name of the delivery source
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @retval RET_DREC_ERR_ARGUMENTS Incorrect argument
/// \~english @par Prerequisite
///     - This API must be called only when Availability of Diag Service is TRUE.
/// \~english @par Change of internal state
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - The argument (notifyName) is NULL. [RET_DREC_ERR_ARGUMENTS]
///     - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///     - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Fire and Forget only
/// \~english @par Detail
///     Registers the handle and thread name for the delivery destination application in the delivery registration table.\n
///     The client must call this API on a thread that needs to receive that are related to the Maker peculiarity trace in advance.
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_MakerTrace_DeliveryEntry(HANDLE hApp,
                                                  PCSTR notifyName);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_MakerTrace_SizeRequest
/// \~english @par Brief
///     Request to get record size of Maker peculiarity trace.
/// \~english @param[in] hApp
///     HANDLE - The handle for the delivery source application
/// \~english @param[in] notifyName
///     PCSTR - The thread name of the delivery source
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @retval RET_DREC_ERR_ARGUMENTS Incorrect argument
/// \~english @par Prerequisite
///     - None
/// \~english @par Change of internal state
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - The argument (notifyName) is NULL. [RET_DREC_ERR_ARGUMENTS]
///     - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///     - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Private
/// \~english @par Type
///     Fire and Forget only
/// \~english @par Detail
///     Request to get record size of Maker peculiarity trace to the registered application for delivery.\n
///     Message ID sended by DiagService is as follows\n
///     MSGID_DREC_MAKERTRACE_SIZEREQUEST
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_MakerTrace_SizeRequest(HANDLE hApp,
                                                PCSTR notifyName);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_MakerTrace_SizeResponce
/// \~english @par Brief
///     Send record size of Maker peculiarity trace to DiagService.
/// \~english @param[in] hApp
///     HANDLE - The handle for the delivery source application
/// \~english @param[in] notifyName
///     PCSTR - The thread name of the delivery source
/// \~english @param[in] size
///     u_int32_t - Data size (Unit: Byte)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @retval RET_DREC_ERR_ARGUMENTS Incorrect argument
/// \~english @par Prerequisite
///     - This API must be called only when Availability of Diag Service is TRUE.
/// \~english @par Change of internal state
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - The argument (notifyName) is NULL. [RET_DREC_ERR_ARGUMENTS]
///     - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///     - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Fire and Forget only
/// \~english @par Detail
///     Send record size of Maker peculiarity trace to DiagService.
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_MakerTrace_SizeResponce(HANDLE hApp,
                                                 PCSTR notifyName,
                                                 u_int32_t size);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_MakerTrace_OutputRequest
/// \~english @par Brief
///     Request to output of Maker peculiarity trace.
/// \~english @param[in] hApp
///     HANDLE - The handle for the delivery source application
/// \~english @param[in] notifyName
///     PCSTR - The thread name of the delivery source
/// \~english @param[in] output
///     PCSTR - Output destination
/// \~english @param[in] state
///     DREC_OUTPUT_STATE - Status (output start or output stop)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @retval RET_DREC_ERR_ARGUMENTS Incorrect argument
/// \~english @par Prerequisite
///     - None
/// \~english @par Change of internal state
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - The argument (notifyName) is NULL. [RET_DREC_ERR_ARGUMENTS]
///     - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///     - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Private
/// \~english @par Type
///     Fire and Forget only
/// \~english @par Detail
///     Request to output of Maker peculiarity trace to the registered application for delivery.\n
///     Message ID sended by DiagService is as follows\n
///     MSGID_DREC_MAKERTRACE_OUTPUTREQUEST
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_MakerTrace_OutputRequest(HANDLE hApp,
                                                  PCSTR notifyName,
                                                  PCSTR output,
                                                  DREC_OUTPUT_STATE state);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_MakerTrace_OutputResponce
/// \~english @par Brief
///     Send output status of Maker peculiarity trace to DiagService.
/// \~english @param[in] hApp
///     HANDLE - The handle for the delivery source application
/// \~english @param[in] notifyName
///     PCSTR - The thread name of the delivery source
/// \~english @param[in] progress
///     u_int8_t - Progress rate (0 to 100)
/// \~english @param[in] state
///     DREC_OUTPUT_RESULT - Status (OK or NG)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @retval RET_DREC_ERR_ARGUMENTS Incorrect argument
/// \~english @par Prerequisite
///     - This API must be called only when Availability of Diag Service is TRUE.
/// \~english @par Change of internal state
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - The argument (notifyName) is NULL. [RET_DREC_ERR_ARGUMENTS]
///     - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///     - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Fire and Forget only
/// \~english @par Detail
///     Send output status of Maker peculiarity trace to DiagService.
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_MakerTrace_OutputResponce(HANDLE hApp,
                                                   PCSTR notifyName,
                                                   u_int8_t progress,
                                                   DREC_OUTPUT_RESULT state);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_MakerTrace_DeleteRequest
/// \~english @par Brief
///     Request to delete of Maker peculiarity trace.
/// \~english @param[in] hApp
///     HANDLE - The handle for the delivery source application
/// \~english @param[in] notifyName
///     PCSTR - The thread name of the delivery source
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @retval RET_DREC_ERR_ARGUMENTS Incorrect argument
/// \~english @par Prerequisite
///     - None
/// \~english @par Change of internal state
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - The argument (notifyName) is NULL. [RET_DREC_ERR_ARGUMENTS]
///     - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///     - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Private
/// \~english @par Type
///     Fire and Forget only
/// \~english @par Detail
///     Request to delete of Maker peculiarity trace to the registered application for delivery.\n
///     Message ID sended by DiagService is as follows\n
///     MSGID_DREC_MAKERTRACE_DELETEREQUEST
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_MakerTrace_DeleteRequest(HANDLE hApp,
                                                  PCSTR notifyName);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_MakerTrace_DeleteResponce
/// \~english @par Brief
///     Send result of delete Maker peculiarity trace to DiagService.
/// \~english @param[in] hApp
///     HANDLE - The handle for the delivery source application
/// \~english @param[in] notifyName
///     PCSTR - The thread name of the delivery source
/// \~english @param[in] state
///     DREC_OUTPUT_RESULT - Status (OK or NG)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @retval RET_DREC_ERR_ARGUMENTS Incorrect argument
/// \~english @par Prerequisite
///     - This API must be called only when Availability of Diag Service is TRUE.
/// \~english @par Change of internal state
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - The argument (notifyName) is NULL. [RET_DREC_ERR_ARGUMENTS]
///     - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///     - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Fire and Forget only
/// \~english @par Detail
///     Send result of delete Maker peculiarity trace to DiagService.
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_MakerTrace_DeleteResponce(HANDLE hApp,
                                                   PCSTR notifyName,
                                                   DREC_OUTPUT_RESULT state);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_MakerTrace_FuncStatusRequest
/// \~english @par Brief
///     Request to change function status of Maker peculiarity trace.
/// \~english @param[in] hApp
///     HANDLE - The handle for the delivery source application
/// \~english @param[in] notifyName
///     PCSTR - The thread name of the delivery source
/// \~english @param[in] func
///     DREC_OUTPUT_FUNC - Function status (function ON or ffunction OFF)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @retval RET_DREC_ERR_ARGUMENTS Incorrect argument
/// \~english @par Prerequisite
///     - None
/// \~english @par Change of internal state
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - The argument (notifyName) is NULL. [RET_DREC_ERR_ARGUMENTS]
///     - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///     - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Private
/// \~english @par Type
///     Fire and Forget only
/// \~english @par Detail
///     Request to change function status of Maker peculiarity trace to the registered application for delivery.\n
///     Message ID sended by DiagService is as follows\n
///     MSGID_DREC_MAKERTRACE_FUNCSTATUSREQUEST
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_MakerTrace_FuncStatusRequest(HANDLE hApp,
                                                      PCSTR notifyName,
                                                      DREC_OUTPUT_FUNC func);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_MakerTrace_FuncStatusResponce
/// \~english @par Brief
///     Send result of change function status of Maker peculiarity trace to DiagService.
/// \~english @param[in] hApp
///     HANDLE - The handle for the delivery source application
/// \~english @param[in] notifyName
///     PCSTR - The thread name of the delivery source
/// \~english @param[in] state
///     DREC_OUTPUT_RESULT - Status (OK or NG)
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @retval RET_DREC_ERR_ARGUMENTS Incorrect argument
/// \~english @par Prerequisite
///     - This API must be called only when Availability of Diag Service is TRUE.
/// \~english @par Change of internal state
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - The argument (notifyName) is NULL. [RET_DREC_ERR_ARGUMENTS]
///     - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///     - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Fire and Forget only
/// \~english @par Detail
///     Send result of change function status of Maker peculiarity trace to DiagService.
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     None
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_MakerTrace_FuncStatusResponce(HANDLE hApp,
                                                       PCSTR notifyName,
                                                       DREC_OUTPUT_RESULT state);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DREC_MakerTrace_NotifyRestrict
/// \~english @par Brief
///     Restrict notify of Maker peculiarity trace
/// \~english @param[in] hApp
///     HANDLE - The handle for the delivery source application
/// \~english @param[in] notifyName
///     PCSTR - The thread name of the delivery source
/// \~english @param[in] notifyRestrict
///     DREC_NOTIFY_RESTRICT - Restrict kind of Maker peculiarity trace
/// \~english @retval RET_DREC_NORMAL Normal end
/// \~english @retval RET_DREC_ERROR Abnormal end
/// \~english @retval RET_DREC_ERR_ARGUMENTS Incorrect argument
/// \~english @par Prerequisite
///     - This API must be called only when Availability of Diag Service is TRUE.
/// \~english @par Change of internal state
///     - The internal state is not changed.
/// \~english @par Conditions of processing failure
///     - The argument (notifyName) is NULL. [RET_DREC_ERR_ARGUMENTS]
///     - Opening the sending message queue (McOpenSender) failed. [RET_DREC_ERROR]
///     - Sending asynchronous data (McSend) failed. [RET_DREC_ERROR]
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Fire and Forget only
/// \~english @par Detail
///     Restrict trace notifications triggered by the specified notifyRestrict.
///     Restrict notification(MessageID) are as follows.
///     - MSGID_DREC_MAKERTRACE_SIZEREQUEST
///     - MSGID_DREC_MAKERTRACE_OUTPUTREQUEST
///     - MSGID_DREC_MAKERTRACE_DELETEREQUEST
///     - MSGID_DREC_MAKERTRACE_FUNCSTATUSREQUEST
///     \n
///     if this API is not called, not restrict(Status is DREC_NOTIFY_RESTRICT_NONE).
/// \~english @par
///      - include MM_DREC_API.h
///      - Library libMM_DREC_API.so
/// \~english @see
///     DREC_MakerTrace_FuncStatusRequest, DREC_MakerTrace_DeleteRequest,
///     DREC_MakerTrace_OutputRequest, DREC_MakerTrace_SizeRequest
/////////////////////////////////////////////////////////////////////////////////////
extern RET_DREC_API DREC_MakerTrace_NotifyRestrict(HANDLE hApp,
                                                   PCSTR notifyName,
                                                   DREC_NOTIFY_RESTRICT notifyRestrict);
#ifdef __cplusplus
}
#endif  // __cplusplus
/** @}*/  // end of Diag_Drec_MakerTrace
/** @}*/  // end of Diag
/** @}*/  // end of Vehicle
/** @}*/  // end of ApplicationManagement
#endif  // __MM_DREC_API_H__

