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

/**
 * @file PFDREC_API.h
 * @brief diag_record API header file
 */
/*******************************************************************************
 * FILE      : PFDREC_API.h
 * SYSTEM    : LinuxPF
 * SUBSYSTEM :
 * TITLE     : Diag record managementfeature include definition
 ******************************************************************************/

#ifndef DIAG_RECORD_LIBRARY_RECORD_API_INCLUDE_VEHICLE_SERVICE_PFDREC_API_H_
#define DIAG_RECORD_LIBRARY_RECORD_API_INCLUDE_VEHICLE_SERVICE_PFDREC_API_H_

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup diag_record
 *  @ingroup vehicle_service
 *  @{
 */

#include <time.h>
#include <agl_thread.h>
#include <native_service/frameworkunified_types.h>

#ifdef  __cplusplus
extern  "C" {
#endif

/**
 * \~english The definition of the processing result value
 */
typedef int RET_PFDREC_API;

/**
 * \~english The definition of the diag_record thread
 */
#ifndef TN_PFDRECTHREAD
#define TN_PFDRECTHREAD "PFDRECThread"
#endif

/**
 * \~english The definition of the diag_record thread availability.
 */
#define NTFY_PFDRECThread_Availability      "PFDRECThread/Availability"

/**
 * \~english The definition of the processing normal result.
 */
#define RET_PFDREC_NORMAL           (0)
/**
 * \~english The definition of the processing abnormal result.
 */
#define RET_PFDREC_ERROR            (-1)
/**
 * \~english The definition of the invalid arguments result.
 */
#define RET_PFDREC_ERR_ARGUMENTS    (-2)
/**
 * \~english The definition of the stop status result.
 */
#define RET_PFDREC_STOP             (-3)

/**
 * \~english The definition of the invalid time information
 */
#define PFDREC_TIME_INFO_INVALID            (0)

/**
 * \~english The definition of the invalid unique ID
 */
#define PFDREC_UNIQUE_INVALID               (0xFFFFFFFFFFFFFFFF)


/*! @~english The size of the record frame of PWR */
#define PFDREC_FRAME_SIZE_TRC_PWR           (16)
/*! @~english The size of the record frame of MODE */
#define PFDREC_FRAME_SIZE_TRC_MODE          (16)
/*! @~english The size of the record frame of KEY */
#define PFDREC_FRAME_SIZE_TRC_KEY           (16)
/*! @~english The size of the record frame of ERR */
#define PFDREC_FRAME_SIZE_TRC_ERR           (16)
/*! @~english The size of the record frame of CAN */
#define PFDREC_FRAME_SIZE_TRC_CAN           (16)
/*! @~english The size of the record frame of DAPS */
#define PFDREC_FRAME_SIZE_TRC_DAPS          (16)
/*! @~english The size of the record frame of DAPU */
#define PFDREC_FRAME_SIZE_TRC_DAPU          (16)
/*! @~english The size of the record frame of BT */
#define PFDREC_FRAME_SIZE_TRC_BT            (16)
/*! @~english The size of the record frame of BT_INTERNAL */
#define PFDREC_FRAME_SIZE_TRC_BT_INTERNAL   (16)
/*! @~english The size of the record frame of WIFI */
#define PFDREC_FRAME_SIZE_TRC_WIFI          (16)
/*! @~english The size of the record frame of WIFI_INTERNAL */
#define PFDREC_FRAME_SIZE_TRC_WIFI_INTERNAL (16)
/*! @~english The size of the record frame of CENTER */
#define PFDREC_FRAME_SIZE_TRC_CENTER        (16)
/*! @~english The size of the record frame of VERSION */
#define PFDREC_FRAME_SIZE_TRC_VERSION       (16)
/*! @~english The size of the record frame of DCM  */
#define PFDREC_FRAME_SIZE_TRC_DCM           (16)
/*! @~english The size of the record frame of ETC */
#define PFDREC_FRAME_SIZE_TRC_ETC           (16)

/*! @~english The size of the record frame of USB_ETC */
#define PFDREC_FRAME_SIZE_TRC_USB_ETC       (16)
/*! @~english The size of the record frame of DSRC_UPLINK */
#define PFDREC_FRAME_SIZE_TRC_DSRC_UPLINK   (16)

/*! @~english The size of the record frame of _CWORD119_ */
#define PFDREC_FRAME_SIZE_TRC__CWORD119_          (16)
/*! @~english The size of the record frame of RADIO */
#define PFDREC_FRAME_SIZE_TRC_RADIO         (16)
/*! @~english The size of the record frame of USB_UNIT */
#define PFDREC_FRAME_SIZE_TRC_USB_UNIT      (16)
/*! @~english The size of the record frame of USB_API */
#define PFDREC_FRAME_SIZE_TRC_USB_API       (16)
/*! @~english The size of the record frame of ETH */
#define PFDREC_FRAME_SIZE_TRC_ETH           (16)
/*! @~english The size of the record frame of ETC_WATCH */
#define PFDREC_FRAME_SIZE_TRC_ETC_WATCH     (16)
/*! @~english The size of the record frame of ROB */
#define PFDREC_FRAME_SIZE_TRC_ROB           (16)
/*! @~english The size of the record frame of EVAL */
#define PFDREC_FRAME_SIZE_TRC_EVAL          (16)
/*! @~english The size of the record frame of OPT_BEACON_DOWN */
#define PFDREC_FRAME_SIZE_TRC_OPT_BEACON_DOWN  (16)
/*! @~english The size of the record frame of OPT_BEACON_UP */
#define PFDREC_FRAME_SIZE_TRC_OPT_BEACON_UP    (16)

/**
 * \~english The definition of the callback function.
 */
typedef void (*PFDREC_CALLBACK_FUNC)(RET_PFDREC_API result);

/**
 * \~english The definition of the record function status
 */
typedef enum _E_PFDREC_FUNC {
    /*! @~english The record function is ON */
    E_PFDREC_FUNC_ON = 0,
    /*! @~english The record function is OFF */
    E_PFDREC_FUNC_OFF
}E_PFDREC_FUNC;

/**
 * \~english The definition of the category
 */
typedef enum _E_PFDREC_CATEGORY_TYPE {
    /*! @~english The defination of PWR trace*/
    E_PFDREC_CATEGORY_TRC_PWR = 0,
    /*! @~english The defination of MODE trace */
    E_PFDREC_CATEGORY_TRC_MODE,
    /*! @~english The defination of KEY trace */
    E_PFDREC_CATEGORY_TRC_KEY,
    /*! @~english The defination of ERR trace */
    E_PFDREC_CATEGORY_TRC_ERR,
    /*! @~english The defination of CAN trace */
    E_PFDREC_CATEGORY_TRC_CAN,
    /*! @~english The defination of DAPS trace */
    E_PFDREC_CATEGORY_TRC_DAPS,
    /*! @~english The defination of DAPU trace */
    E_PFDREC_CATEGORY_TRC_DAPU,
    /*! @~english The defination of BT trace */
    E_PFDREC_CATEGORY_TRC_BT,
    /*! @~english The defination of BT_INTERNAL trace */
    E_PFDREC_CATEGORY_TRC_BT_INTERNAL,
    /*! @~english The defination of WIFI trace */
    E_PFDREC_CATEGORY_TRC_WIFI,
    /*! @~english The defination of WIFI_INTERNAL trace */
    E_PFDREC_CATEGORY_TRC_WIFI_INTERNAL,
    /*! @~english The defination of CENTER trace */
    E_PFDREC_CATEGORY_TRC_CENTER,
    /*! @~english The defination of VERSION trace */
    E_PFDREC_CATEGORY_TRC_VERSION,
    /*! @~english The defination of DCM trace */
    E_PFDREC_CATEGORY_TRC_DCM,
    /*! @~english The defination of ETC trace */
    E_PFDREC_CATEGORY_TRC_ETC,

    /*! @~english The defination of USB_ETC trace */
    E_PFDREC_CATEGORY_TRC_USB_ETC,
    /*! @~english The defination of DSRC_UPLINK trace */
    E_PFDREC_CATEGORY_TRC_DSRC_UPLINK,

    /*! @~english The defination of _CWORD119_ trace */
    E_PFDREC_CATEGORY_TRC__CWORD119_,
    /*! @~english The defination of RADIO trace */
    E_PFDREC_CATEGORY_TRC_RADIO,
    /*! @~english The defination of USB_UNIT trace */
    E_PFDREC_CATEGORY_TRC_USB_UNIT,
    /*! @~english The defination of USB_API trace */
    E_PFDREC_CATEGORY_TRC_USB_API,
    /*! @~english The defination of ETH trace*/
    E_PFDREC_CATEGORY_TRC_ETH,
    /*! @~english The defination of ETC_WATCH trace */
    E_PFDREC_CATEGORY_TRC_ETC_WATCH,
    /*! @~english The defination of ROB trace */
    E_PFDREC_CATEGORY_TRC_ROB,
    /*! @~english The defination of EVAL trace */
    E_PFDREC_CATEGORY_TRC_EVAL,
    /*! @~english The defination of OPT_BEACON_DOWN trace */
    E_PFDREC_CATEGORY_TRC_OPT_BEACON_DOWN,
    /*! @~english The defination of OPT_BEACON_UP trace */
    E_PFDREC_CATEGORY_TRC_OPT_BEACON_UP,

    /*! @~english The defination of max value */
    E_PFDREC_CATEGORY_TRC_MAX
}E_PFDREC_CATEGORY_TYPE;

/**
 * \~english The definition of the trace data extraction mode
 */
typedef enum _E_PFDREC_EXTRACT_MODE {
    /*! @~english The extraction mode is OFF */
    E_PFDREC_EXTRACT_MODE_OFF = 0,
    /*! @~english The extraction mode is ON */
    E_PFDREC_EXTRACT_MODE_ON
}E_PFDREC_EXTRACT_MODE;


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_FuncStartStop
/// \~english @par Summary
///      - Switch the ON/OFF status of the record function
/// \~english @param [in]    status  The ON/OFF status of the record function\n
///                                  Set the one of the following values.\n
///                                      E_PFDREC_FUNC_ON(The record function is ON)\n
///                                      E_PFDREC_FUNC_OFF(The record function is OFF)\n
///                                  See E_PFDREC_FUNC.
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - BackupMgr is available.
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument ON/OFF status is invalid. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the message queue handle for transmission is NULL. [RET_PFDREC_ERROR]
///      - When failed to transmit synchronous message to MM Diag Recorder. [RET_PFDREC_ERROR]
///      - When the length of the recieved synchronous message is different from the length of the buffer.
///        [RET_PFDREC_ERROR]
///      - When MM Diag Recorder failed to switch the function ON/OFF status. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed only from the diagnosis service.
///      - Start or stop the transmission of record.
///      - When the function is OFF, no data is executed even if the data recording API is called,
///        and RET_PFDREC_NORMAL is returned.
///      - Record the ON/OFF information into the backup memory.
///      - Restore the retained value to the default E_PFDREC_FUNC_ON at +B OFF / factory shipment.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @see PFDREC_OpenSession, FrameworkunifiedInvokeSync
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_FuncStartStop(E_PFDREC_FUNC status);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_DeleteData
/// \~english @par Summary
///      - Delete the recorded data.
/// \~english @param [in]    func   callback function\n
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument callback function is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the session with the MM Diag Recorder has not been established yet. [RET_PFDREC_ERROR]
///      - When the callback function of deletion end was already setted. [RET_PFDREC_ERROR]
///      - When failed to regist the callback information to the dispatcher. [RET_PFDREC_ERROR]
///      - When the message queue handle for transmission is NULL. [RET_PFDREC_ERROR]
///      - When failed to transmit asynchronous message to MM Diag Recorder. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed only from the diagnosis service.
///      - To delete the data recorded in the temporary buffer and nonvolatile memory.
///      - The callback function specified by argument will be called when data deletion end.
///      - typedef void (*DERC_CALLBACK_FUNC)( RET_PFDREC_API result);\n
///        The deletion result is stored in the argument result when the callback function is called.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Method
/// \~english @see PFDREC_OpenSession, FrameworkunifiedAttachCallbackToDispatcher, FrameworkunifiedSendMsg
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_DeleteData(PFDREC_CALLBACK_FUNC func);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_DeleteIndividual
/// \~english @par Summary
///      - Delete the individual recorded data.
/// \~english @param [in]    category    The category information of the trace data to record\n
///                                      See E_PFDREC_CATEGORY_TYPE.
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument category is invalid. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the message queue handle for transmission is NULL. [RET_PFDREC_ERROR]
///      - When failed to the transmit synchronous message [RET_PFDREC_ERROR]
///      - When the length of the recieved synchronous message is different from the length of the buffer.
///        [RET_PFDREC_ERROR]
///      - When the argument category is invalid in MM Diag recorder management information table. [RET_PFDREC_ERROR]
///      - When the MM Diag Recorder failed to allocate memory for writting the recorded data. [RET_PFDREC_ERROR]
///      - When failed to write the initialization date to ROM. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed only from the diagnosis service.
///      - To delete the individual data recorded in the temporary buffer and nonvolatile memory.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @see PFDREC_OpenSession, FrameworkunifiedInvokeSync
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_DeleteIndividual(E_PFDREC_CATEGORY_TYPE category);


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_ComDataBackup
/// \~english @par Summary
///      - Record the trace data.
/// \~english @param [in]    category    The category information of the trace data to record\n
///                                      See E_PFDREC_CATEGORY_TYPE.
/// \~english @param [in]    time_info   The time information.
/// \~english @param [in]    frame_num   The number of frames of the trace data to record.
/// \~english @param [in]    data        The trace data to record\n
///                                      The size of the data buffer is the number of frames * 1 frame size,\n
///                                      and should be secured by the caller.
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument data is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument category is invalid. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument frame_num is 0. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument frame_num exceeds the upper limit value (100). [RET_PFDREC_ERR_ARGUMENTS]
///      - When the frame size of the argument category is 0. [RET_PFDREC_ERR_ARGUMENTS]
///      - When failed to get the message queue handle for transmission to MM Diag Recorder. [RET_PFDREC_ERROR]
///      - When failed to the transmit asynchronous message. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed only from the diagnosis service.
///      - Record the trace data into the buffer temporarily, and when the size of data in buffer reaches the maximum,
///        move the data to ROM firstly.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Fire and Forget
/// \~english @see PFDREC_OpenSession, FrameworkunifiedAttachCallbackToDispatcher, FrameworkunifiedSendMsg
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_ComDataBackup(E_PFDREC_CATEGORY_TYPE category,
                                    time_t time_info,
                                    uint32_t frame_num,
                                    uint8_t* data);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_ReadMemData
/// \~english @par Summary
///      - Read memory data.
/// \~english @param [in]    category    The category information of the trace data to read\n
///                                      See E_PFDREC_CATEGORY_TYPE.
/// \~english @param [in]    offset      The start position of the frame to read\n
///                                      The offset is 0, which means the earliest recorded data.
/// \~english @param [in]    read_num    The number of the frames to read.
/// \~english @param [out]   data        The readed trace data\n
///                                      The size of the data buffer is the number of frames * 1 frame size,\n
///                                      and should be secured by the caller.
/// \~english @param [out]   out_num     The number of readed trace data.
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - The argument data is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - The argument read_num is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument category is invalid. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument read_num is 0. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument read_num exceeds the upper limit value (100). [RET_PFDREC_ERR_ARGUMENTS]
///      - When the frame size of the argument category is 0 [RET_PFDREC_ERR_ARGUMENTS]
///      - When the message queue handle for transmission is NULL. [RET_PFDREC_ERROR]
///      - When failed to the transmit synchronous message. [RET_PFDREC_ERROR]
///      - When the length of the recieved synchronous message is different from the length of the buffer.
///        [RET_PFDREC_ERROR]
///      - When the argument category is invalid in the record management information table of MM Diag recorder.
///        [RET_PFDREC_ERROR]
///      - When the MM Diag Recorder failed to allocate memory for reading out the recorded data. [RET_PFDREC_ERROR]
///      - When the MM Diag Recorder failed in the process of reading the recorded data. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed from the diagnosis service and the AppDataReporting service,\n
///        which is a supplier-specific service.
///      - Read the number of recorded data from the position specified by offset.
///      - When the end of the recorded data is reached, stop reading and return the number of readed data.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @see PFDREC_OpenSession, FrameworkunifiedInvokeSync
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_ReadMemData(E_PFDREC_CATEGORY_TYPE category,
                                    uint32_t offset,
                                    uint32_t read_num,
                                    uint8_t* data,
                                    uint32_t* out_num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_GetDataInfo
/// \~english @par Summary
///      - Get the information of the recorded data.
/// \~english @param [in]    category    The category information\n
///                                      See E_PFDREC_CATEGORY_TYPE.
/// \~english @param [out]   frame_num   The number of the recorded frames.
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument frame_num is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument category is invalid. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the message queue handle for transmission is NULL. [RET_PFDREC_ERROR]
///      - When failed to the transmit synchronous message. [RET_PFDREC_ERROR]
///      - When the length of the recieved synchronous message is different from the length of the buffer.
///        [RET_PFDREC_ERROR]
///      - When the argument category is invalid in the record management information table of MM Diag recorder.
///        [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed from the diagnosis service and the AppDataReporting service,\n
///        which is a supplier-specific service.
///      - Return the number of frames recorded in the record data area of the specified category.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @see PFDREC_OpenSession, FrameworkunifiedInvokeSync
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_GetDataInfo(E_PFDREC_CATEGORY_TYPE category,
                                    uint32_t* frame_num);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_GetFuncStartStop
/// \~english @par Summary
///      - Get the ON/OFF status of the record function.
/// \~english @param [out]    status  The ON/OFF status of the record function\n
///                                   Set the one of the following values.\n
///                                       E_PFDREC_FUNC_ON(The record function is ON)\n
///                                       E_PFDREC_FUNC_OFF(The record function is OFF)\n
///                                   See E_PFDREC_FUNC.
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument ON/OFF status is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the message queue handle for transmission is NULL. [RET_PFDREC_ERROR]
///      - When failed to transmit synchronous message to MM Diag Recorder. [RET_PFDREC_ERROR]
///      - When the length of the recieved synchronous message is different from the length of the buffer.
///        [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed only from the diagnosis service.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @see PFDREC_OpenSession, FrameworkunifiedInvokeSync
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_GetFuncStartStop(E_PFDREC_FUNC* status);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_OutputData
/// \~english @par Summary
///      - Move the recorded data to ROM.
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the message queue handle for transmission is NULL. [RET_PFDREC_ERROR]
///      - When failed to the transmit synchronous message [RET_PFDREC_ERROR]
///      - When the length of the recieved synchronous message is different from the length of the buffer.
///        [RET_PFDREC_ERROR]
///      - When MM Diag Recorder failed to move the recorded data to ROM. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed only from the diagnosis service.
///      - Move the data recorded in the temporary buffer to the ROM.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @see PFDREC_OpenSession, FrameworkunifiedInvokeSync
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_OutputData(void);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_ReadData
/// \~english @par Summary
///      - Read the recorded data from the specified position.
/// \~english @param [in]    category        The category information of the trace data to read\n
///                                          See E_PFDREC_CATEGORY_TYPE.
/// \~english @param [in]    unique_id       The unique ID of the frame to start reading\n
///                                          If the unique_id is invalid, return error and do not read.
/// \~english @param [in]    read_num        The number of frames to read.
/// \~english @param [out]   data            The readed data.\n
///                                          The size of the data buffer is the number of frames * 1 frame size,\n
///                                          and should be secured by the caller.
/// \~english @param [out]   out_num         The number of the readed frames.
/// \~english @param [out]   end_unique_id   The unique ID of the end of the readed data.
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument data is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument out_num is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument end_unique_id is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument unique_id is invalid. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument category is invalid [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument read_num is 0. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument read_num exceeds the upper limit value (100). [RET_PFDREC_ERR_ARGUMENTS]
///      - When the frame size of the argument category is 0 [RET_PFDREC_ERR_ARGUMENTS]
///      - When the message queue handle for transmission is NULL. [RET_PFDREC_ERROR]
///      - When failed to the transmit synchronous message [RET_PFDREC_ERROR]
///      - When the length of the recieved synchronous message is different from the length of the buffer.
///        [RET_PFDREC_ERROR]
///      - When the MM Diag Recorder failed in the process of reading the recorded data. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed from the diagnosis service and the AppDataReporting service,\n
///        which is a supplier-specific service.
///      - When the end of the recorded data is reached, stop reading and return the number of readed data.
///      - Read the number of recorded data from the position specified by unique ID.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @see PFDREC_OpenSession, FrameworkunifiedInvokeSync
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_ReadData(E_PFDREC_CATEGORY_TYPE category,
                                uint64_t unique_id,
                                uint32_t read_num,
                                uint8_t* data,
                                uint32_t* out_num,
                                uint64_t* end_unique_id);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_SearchData
/// \~english @par Summary
///      - Search the recorded data by the specified time information.
/// \~english @param [in]    category    The information of category\n
///                                      See E_PFDREC_CATEGORY_TYPE.
/// \~english @param [in]    time_info   The time information is used to search in recorded data.\n
///                                      If the time information is invalid, to return error and do not search.
/// \~english @param [out]   unique_id   The unique ID
/// \~english @param [out]   data        The result obtained by searching for the time information.\n
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument data is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument unique_id is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When setting search time information is invalid. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument category is invalid [RET_PFDREC_ERR_ARGUMENTS]
///      - When the frame size of the argument category is 0. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the message queue handle for transmission is NULL. [RET_PFDREC_ERROR]
///      - When failed to trasmite the synchronization message. [RET_PFDREC_ERROR]
///      - When the length of synchronous communication received is different from the already set length.
///        [RET_PFDREC_ERROR]
///      - When the argument category is invalid in MM Diag recorder management information table. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed from the diagnosis service and the AppDataReporting service,\n
///        which is a supplier-specific service.
///      - When the end of the recorded data is reached, stop reading and return the number of readed data.
///      - If the recorded time information is invalid, to skip searching.\n
///        Also, to make sure that the input argument time_info(note) is available for searching.\n
///        (note)"1"(from 1970/1/1/0:00.01) or more.
///      - The frame data more than the specified time could not be found, or \n
///        If time_info is invalid, RET_PFDREC_ERROR will be returned.
///      - For the categories which can be searched by time, see the searchable trace.
///      - If the specified category is not searchable, RET_PFDREC_ERROR will be returned.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @see PFDREC_OpenSession, FrameworkunifiedInvokeSync
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_SearchData(E_PFDREC_CATEGORY_TYPE category,
                                    time_t time_info,
                                    uint64_t* unique_id,
                                    uint8_t*  data);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_OpenSession
/// \~english @par Summary
///      - Establish a session with the MM Diag Recorder.
/// \~english @param [in]    h_app    The application handle
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument h_app is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - If the message queue handle for transmission is not NULL\n
///        (when the PFDREC_OpenSession has already been called) [RET_PFDREC_ERROR]
///      - When failed to generate a message queue handle for transmission. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed only from the diagnosis service.
///      - Establish a session with the MM Diag Recorder.
///      - The API should be called before calling the following APIs of the MM Diag Record function.\n
///         PFDREC_FuncStartStop\n
///         PFDREC_DeleteData\n
///         PFDREC_ReadMemData\n
///         PFDREC_GetDataInfo\n
///         PFDREC_GetFuncStartStop\n
///         PFDREC_OutputData\n
///         PFDREC_ReadData\n
///         PFDREC_SearchData\n
///         PFDREC_SetExtractionMode\n
///         PFDREC_SetExtractionMode_Exp\n
///         PFDREC_SearchDataY2K38\n
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @see PFDREC_FuncStartStop, PFDREC_DeleteData, PFDREC_ReadMemData, PFDREC_GetDataInfo, \n
///                 PFDREC_GetFuncStartStop, PFDREC_OutputData, PFDREC_ReadData, PFDREC_SearchData, \n
///                 PFDREC_SetExtractionMode, PFDREC_SetExtractionMode_Exp, PFDREC_SearchDataY2K38, \n
///                 PFDREC_CloseSession, FrameworkunifiedMcOpenSender
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_OpenSession(HANDLE h_app);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_CloseSession
/// \~english @par Summary
///      - Release the session with the MM Diag Recorder.
/// \~english @param [in]    h_app    The application handle
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - When the session with MM Diag Recorder becomes unnecessary.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument h_app is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the message queue handle for transmission is NULL. [RET_PFDREC_ERROR]
///      - When failed to close the message queue transmission handle. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed only from the diagnosis service.
///      - Release the session with the MM Diag Recorder.
///      - To call the API When the session with MM Diag Recorder becomes unnecessary.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @see PFDREC_OpenSession, FrameworkunifiedMcClose
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_CloseSession(HANDLE h_app);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_OverwriteData
/// \~english @par Summary
///      - Overwrite and record the data of one frame to the specified offset.
/// \~english @param [in]    category    The category information\n
///                                      See E_PFDREC_CATEGORY_TYPE.
/// \~english @param [in]    offset      The position to overwrite the frame data.\n
///                                      The offset is 0, which means the earliest recorded data.
/// \~english @param [in]    data        The frame data to be overwrite.
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - Whe argument data is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument category is invalid. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the frame size of the argument category is 0. [RET_PFDREC_ERR_ARGUMENTS]
///      - When failed to get the message queue handle for transmission to MM Diag Recorder. [RET_PFDREC_ERROR]
///      - When failed to the transmit asynchronous message. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed only from the diagnosis service.
///      - Overwrite and record the data of one frame to the specified offset.
///      - When the frame data specified by offset is not exist, RET_PFDREC_ERROR will be returned.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Fire and Forget
/// \~english @see McOpenSender, McSend
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_OverwriteData(E_PFDREC_CATEGORY_TYPE category,
                                    uint32_t offset,
                                    uint8_t* data);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_SetExtractionMode
/// \~english @par Summary
///      - Switch the trace data extraction mode.
/// \~english @param [in]    category    The category information\n
///                                      See E_PFDREC_CATEGORY_TYPE.
/// \~english @param [in]    mode        The information of the trace data extraction mode.\n
///                                      See E_PFDREC_EXTRACT_MODE.
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument mode is invalid. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument category is invalid [RET_PFDREC_ERR_ARGUMENTS]
///      - When the message queue handle for transmission is NULL. [RET_PFDREC_ERROR]
///      - When failed to the transmit synchronous message [RET_PFDREC_ERROR]
///      - When the length of the recieved synchronous message is different from the length of the buffer.
///        [RET_PFDREC_ERROR]
///      - When MM Diag Recorder failed to switch the trace data extraction mode. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed only from the diagnosis service.
///      - Switch the trace data extraction mode.
///      - Save the trace data to the extraction area during the extraction mode is ON.\n
///      - When the size of the recorded extraction data reaches the maximum,\n
///        to discard the next data.
///      - And when the extraction mode switched from ON to OFF, write the data to the temporary buffer are.
///      - For the categories which the trace data extraction mode is effective,\n
///        See 3.4.1.2.8 The switchable trace in trace data extraction mode.
///      - If you specify something other than a valid category, RET_PFDREC_ERROR will be returned.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @see PFDREC_OpenSession, FrameworkunifiedInvokeSync
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_SetExtractionMode(const E_PFDREC_CATEGORY_TYPE category,
                                        const E_PFDREC_EXTRACT_MODE mode);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_SetExtractionMode_Exp
/// \~english @par Summary
///      - Switch the trace data extraction mode.(The AppDataReporting service, which is a supplier-specific service)
/// \~english @param [in]    category    The category information\n
///                                      See E_PFDREC_CATEGORY_TYPE.
/// \~english @param [in]    mode        The information of the trace data extraction mode.\n
///                                      See E_PFDREC_EXTRACT_MODE.
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument mode is invalid. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument category is invalid [RET_PFDREC_ERR_ARGUMENTS]
///      - When the message queue handle for transmission is NULL. [RET_PFDREC_ERROR]
///      - When failed to the transmit synchronous message [RET_PFDREC_ERROR]
///      - When the length of the recieved synchronous message is different from the length of the buffer.
///        [RET_PFDREC_ERROR]
///      - When MM Diag Recorder failed to switch the trace data extraction mode. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed only from the AppDataReporting service,\n
///        which is a supplier-specific service.
///      - Switch the trace data extraction mode.
///      - Save the trace data to the extraction area during the extraction mode is ON.\n
///      - When the size of the recorded extraction data reaches the maximum,\n
///        to discard the next data.
///      - And when the extraction mode switched from ON to OFF, write the data to the temporary buffer are.
///      - For the categories which the trace data extraction mode is effective,\n
///        See 3.4.1.2.8 The switchable trace in trace data extraction mode.
///      - If you specify something other than a valid category, RET_PFDREC_ERROR will be returned.
///      - If the data recorded is deleted from the diagnosis service, the request data recorded may not be obtained\n
///        from the AppDataReporting service, which is a supplier-specific service.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @see PFDREC_OpenSession, FrameworkunifiedInvokeSync
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_SetExtractionMode_Exp(const E_PFDREC_CATEGORY_TYPE category,
                                            const E_PFDREC_EXTRACT_MODE mode);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_ComDataBackupY2K38
/// \~english @par Summary
///      - Record the trace data.
/// \~english @param [in]    category    The category information of the trace data to record\n
///                                      See E_PFDREC_CATEGORY_TYPE.
/// \~english @param [in]    time_info   The time information.
/// \~english @param [in]    frame_num   The number of frames of the trace data to record.
/// \~english @param [in]    data        The trace data to record\n
///                                      The size of the data buffer is the number of frames * 1 frame size,\n
///                                      and should be secured by the caller.
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument data is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument category is invalid. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument frame_num is 0. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument frame_num exceeds the upper limit value (100). [RET_PFDREC_ERR_ARGUMENTS]
///      - When the frame size of the argument category is 0. [RET_PFDREC_ERR_ARGUMENTS]
///      - When failed to get the message queue handle for transmission to MM Diag Recorder. [RET_PFDREC_ERROR]
///      - When failed to the transmit asynchronous message. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed only from the diagnosis service.
///      - The API is used to fix the problem of time beyond 2038, and the function is the same as the
///        PFDREC_ComDataBackup function
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Fire and Forget
/// \~english @see PFDREC_OpenSession, McOpenSender, McSend
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_ComDataBackupY2K38(E_PFDREC_CATEGORY_TYPE category,
                                         uint32_t time_info,
                                         uint32_t frame_num,
                                         uint8_t* data);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PFDREC_SearchDataY2K38
/// \~english @par Summary
///      - Search the recorded data by the specified time information
/// \~english @param [in]    category    The information of category\n
///                                      See E_PFDREC_CATEGORY_TYPE.
/// \~english @param [in]    time_info   The time information is used to search in recorded data.\n
///                                      If the time information is invalid, to return error and do not search.
/// \~english @param [out]   unique_id   The unique ID
/// \~english @param [out]   data        The result obtained by searching for the time information.\n
/// \~english @retval  RET_PFDREC_NORMAL         Success
/// \~english @retval  RET_PFDREC_ERROR          Failure
/// \~english @retval  RET_PFDREC_ERR_ARGUMENTS  Invalid parameter
/// \~english @par Precondition
///      - The generation/initialization of dispatcher for application(PadCreateDispatcherWithoutLoop etc.) is done.
///      - The session with the MM Diag Recorder has been established.
///      - The availablity of the NTFY_PFDRECThread_Availability is already TRUE.
/// \~english @par Changes in internal status
///      - There is no changes in internal status when the API is called.
/// \~english @par Conditions of processing failure
///      - When the argument data is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument unique_id is NULL. [RET_PFDREC_ERR_ARGUMENTS]
///      - When setting search time information is invalid. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the argument category is invalid [RET_PFDREC_ERR_ARGUMENTS]
///      - When the frame size of the argument category is 0. [RET_PFDREC_ERR_ARGUMENTS]
///      - When the message queue handle for transmission is NULL. [RET_PFDREC_ERROR]
///      - When failed to trasmite the synchronization message. [RET_PFDREC_ERROR]
///      - When the length of synchronous communication received is different from the already set length.
///        [RET_PFDREC_ERROR]
///      - When the argument category is invalid in MM Diag recorder management information table. [RET_PFDREC_ERROR]
/// \~english @par Details
///      - The API is assumed to be executed from the diagnosis service and the AppDataReporting service,\n
///        which is a supplier-specific service.
///      - The API is used to fix the problem that the time more than 2038, \n
///        and the function is same as the PFDREC_SearchData function.
/// \~english @par Classification
///      - Public
/// \~english @par Type
///      - Sync
/// \~english @see PFDREC_OpenSession, FrameworkunifiedInvokeSync
///
/////////////////////////////////////////////////////////////////////////////////////
RET_PFDREC_API PFDREC_SearchDataY2K38(E_PFDREC_CATEGORY_TYPE category,
                                      uint32_t  time_info,
                                      uint64_t* unique_id,
                                      uint8_t*  data);

#ifdef  __cplusplus
}
#endif

/** @}*/  // end of diag_record
/** @}*/  // end of vehicle_service
/** @}*/  // end of BaseSystem

#endif  // DIAG_RECORD_LIBRARY_RECORD_API_INCLUDE_VEHICLE_SERVICE_PFDREC_API_H_
