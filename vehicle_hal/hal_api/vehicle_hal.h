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

#ifndef HAL_API_VEHICLE_HAL_H_
#define HAL_API_VEHICLE_HAL_H_
/**
 * @file vehicle_hal.h
 */

/** @addtogroup vehicle
 *  @{
 */
/** @addtogroup vehicle_hal
 *  @ingroup vehicle
 *  @{
 */

#include <native_service/frameworkunified_types.h>

/**
 * \~english  Name of the send thread
 */
#define TN_LINE_SENS_READ "Vehicle Hal"

/**
 * @enum LineSensReadInternalServiceProtocol
 * \~english Command ID used by Vehicle_hal to send the vehicle signal input from the jicage line to the user \n
              To receive events with this command ID, use the interface defined in framework_unified as follows 
 * @code 
 * ret_status = FrameworkunifiedAttachCallbackToDispatcher(h_app, TN_LINE_SENS_READ, CID_VEHICLEHAL_LINESENS, CbCallbackFunc);
 * @endcode
 */
enum LineSensReadInternalServiceProtocol {
  CID_VEHICLEHAL_LINESENS = 0x00000101,  //!< \~english LineSens Message Command ID
  CID_VEHICLEHAL_SPDPULSE = 0x00000102,  //!< \~english SpeedPulse Message Command ID
  CID_VEHICLEHAL_SPD = 0x00000103,       //!< \~english Speed Message Command ID
  CID_VEHICLEHAL_LINESENS_POL = 0x00000104,
  //!< \~english voltage and RHEOSTAT Message Command ID
};

/**
 * @enum VehiclehalLinesenskindMicBit
 * \~english  MIC BIT
 */
enum VehiclehalLinesenskindMicBit {
  VEHICLEHAL_LINESENSKIND_MIC_BIT_MIC = 0x01,          //!< \~english MIC
  VEHICLEHAL_LINESENSKIND_MIC_BIT_ANALOGMIC1 = 0x02,   //!< \~english ANALOGMIC1
  VEHICLEHAL_LINESENSKIND_MIC_BIT_ANALOGMIC2 = 0x04,   //!< \~english ANALOGMIC2
};
 
/**
 * @enum VehiclehalLinesenskindMic2Bit
 * \~english  MIC2 BIT
 */
enum VehiclehalLinesenskindMic2Bit {
  VEHICLEHAL_LINESENSKIND_MIC2_BIT_A2BHUB = 0x01,      //!< \~english A2BHUB
  VEHICLEHAL_LINESENSKIND_MIC2_BIT_MULTIMIC1 = 0x02,   //!< \~english MULTIMIC1
  VEHICLEHAL_LINESENSKIND_MIC2_BIT_MULTIMIC2 = 0x04,   //!< \~english MULTIMIC2
  VEHICLEHAL_LINESENSKIND_MIC2_BIT_MULTIMIC3 = 0x08,   //!< \~english MULTIMIC3
  VEHICLEHAL_LINESENSKIND_MIC2_BIT_MULTIMIC4 = 0x10,   //!< \~english MULTIMIC4
  VEHICLEHAL_LINESENSKIND_MIC2_BIT_MULTIMIC5 = 0x20,   //!< \~english MULTIMIC5
  VEHICLEHAL_LINESENSKIND_MIC2_BIT_MULTIMIC6 = 0x40,   //!< \~english MULTIMIC6
  VEHICLEHAL_LINESENSKIND_MIC2_BIT_ANC = 0x80,         //!< \~english ANC
};

/**
 * @enum VehicleHalLineSensKind
 * \~english  Line Sensor kind
 */
enum VehicleHalLineSensKind {
  VEHICLEHAL_LINESENSKIND_IG = 0,       //!< \~english IG
  VEHICLEHAL_LINESENSKIND_PKB,          //!< \~english PKB
  VEHICLEHAL_LINESENSKIND_REV,          //!< \~english REV
  VEHICLEHAL_LINESENSKIND_ILL,          //!< \~english ILL
  VEHICLEHAL_LINESENSKIND_LOWVOLTAGE,   //!< \~english LOWVOLTAGE
  VEHICLEHAL_LINESENSKIND_ADIM,         //!< \~english ADIM
  VEHICLEHAL_LINESENSKIND_MIC,          //!< \~english MIC,ANALOGMIC1,ANALOGMIC2
  VEHICLEHAL_LINESENSKIND_USBPOWERSUPPLY,  //!< \~english USB Power Supply[10mA]
  VEHICLEHAL_LINESENSKIND_MIC2,         //!< \~english A2BHUB,MULTIMIC1 to 6,ANC
  VEHICLEHAL_LINESENSKIND_NUM,          //!< \~english MAX Number
};

/**
 * @enum VEHICLEHAL_RET_TYPE
 * \~english  The return value of vehicle_hal API
 */
typedef enum {
  /**
  * \~english Success
  */
  VEHICLEHAL_RET_SUCCESS = 0,
  /**
  * \~english Car Signal Ctrl Api Initialize failed
  */
  VEHICLEHAL_RET_ERR_HARDINIT,
  /**
  * \~english McSend Open failed
  */
  VEHICLEHAL_RET_ERR_MCOPEN,
  /**
  * \~english Create Thread failed
  */
  VEHICLEHAL_RET_ERR_THREADSTART,
  /**
  * \~english Invalid paramter
  */
  VEHICLEHAL_RET_ERR_PARAM,
}VEHICLEHAL_RET_TYPE;

/**
 * @struct VehicleHalSpeedPulse
 * \~english  Struct of SpeedPulse
 */
struct VehicleHalSpeedPulse {
  float pulse;  //!< \~english speed pulse[Hz]
  float period;  //!< \~english pulse period[ms]
};

/**
 * @struct VehicleHalLineSensMsg
 * \~english  Struct of Meassage
 */
struct VehicleHalLineSensMsg {
  uint8_t stdata[VEHICLEHAL_LINESENSKIND_NUM];  //!< \~english line data
};

/**
 * @struct VehicleHalPol
 * \~english  Struct of VehicleHalPolling
 */
struct VehicleHalPol {
  uint8_t vb;        //!< \~english voltage [10 * v]
  uint8_t rheostat;  //!< \~english RHEOSTAT [10 * v]
};

/**
 * \ingroup VehicleHalStart
 * \~english @par Brief
 *        API to start the vehicle_hal
 * \~english @param [in] happ
 *        HANDLE - Handle for Application
 * \~english @retval VEHICLEHAL_RET_SUCCESS              :  Success
 * \~english @retval VEHICLEHAL_RET_ERR_HARDINIT         :  Car Signal Ctrl Api Initialize failed
 * \~english @retval VEHICLEHAL_RET_ERR_MCOPEN           :  McSend Open failed
 * \~english @retval VEHICLEHAL_RET_ERR_THREADSTART      :  Create Thread failed
 * \~english @retval VEHICLEHAL_RET_ERR_PARAM            :  Invalid paramter
 * \~english @par Prerequisite
 *        None
 * \~english @par Change of internal state
 *        - If start successfully, vehicle_hal changes from stop to start.
 *        - If start failed, the state of the vehicle_hal remains no change.
 * \~english @par Conditions of processing failure
 *        - Car Signal Ctrl Api Initialize failed.
 *        - McSend Open failed.
 *        - Create Thread failed.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @par Detail
 *        - Start the vehicle_hal service.
 *        - The API can only be used by 1 process.
 * \~english @see
 *        None
 */
VEHICLEHAL_RET_TYPE VehicleHalStart(HANDLE happ);

/**
 * \ingroup VehicleHalStop
 * \~english @par Brief
 *        API to stop the vehicle_hal
 * \~english @param [in] happ
 *        HANDLE - Handle for Application
 * \~english @retval VEHICLEHAL_RET_SUCCESS          :  Success
 * \~english @retval VEHICLEHAL_RET_ERR_PARAM        :  Invalid paramter
 * \~english @par Prerequisite
 *        vehicle_hal start successfully.
 * \~english @par Change of internal state
 *        - If start successfully, vehicle_hal changes from start to stop.
 *        - If start failed, the state of the vehicle_hal remains no change.
 * \~english @par Conditions of processing failure
 *        None.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @par Detail
 *        - Stop the vehicle_hal service.
 *        - The API can only be used by 1 process.
 * \~english @see
 *        None
 */
VEHICLEHAL_RET_TYPE VehicleHalStop(HANDLE happ);

/** @}*/  //!<  end of vehicle_hal
/** @}*/  //!<  end of vehicle
#endif  // HAL_API_VEHICLE_HAL_H_
