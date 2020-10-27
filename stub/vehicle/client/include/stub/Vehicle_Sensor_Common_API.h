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

#ifndef VEHICLE_CLIENT_INCLUDE_VEHICLE_SERVICE_VEHICLE_SENSOR_COMMON_API_H_
#define VEHICLE_CLIENT_INCLUDE_VEHICLE_SERVICE_VEHICLE_SENSOR_COMMON_API_H_

/**
 * @file Vehicle_Sensor_Common_API.h
 * @brief vehicle common id defined header file
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle
 *  @ingroup vehicle_service
 *  @{
 */

/******************************************************************************
 * File name    :Vehicle_Sensor_Common_API.h
 * System name    :_CWORD107_
 * Subsystem name  :Vehicle I/F library, Sensor I/F library
 ******************************************************************************/

/************************************************************************
 *      Macro definition                          *
 ************************************************************************/

#if 1                    // Ignore -> MISRA-C++ Rules 16-1-2, 16-2-2
/* #GPF_60_023 For testing */
#ifndef CONFIG_EXTTERM_INPUT_DAB2BIT    // Ignore -> MISRA-C++ Rules 16-1-2, 16-2-2
/**
 * \~english  DAB data 2 bits macro
 */
#define CONFIG_EXTTERM_INPUT_DAB2BIT 1    // Ignore -> MISRA-C++ Rules 16-2-2
#endif
#ifndef CONFIG_EXTTERM_INPUT_GPS2BIT    // Ignore -> MISRA-C++ Rules 16-1-2, 16-2-2
/**
 * \~english  GPS data 2 bits macro
 */
#define CONFIG_EXTTERM_INPUT_GPS2BIT 1    // Ignore -> MISRA-C++ Rules 16-2-2
#endif
#ifndef CONFIG_EXTTERM_INPUT_IMAGETERMINAL  // Ignore -> MISRA-C++ Rules 16-1-2, 16-2-2
/**
 * \~english  Image terminal macro
 */
#define CONFIG_EXTTERM_INPUT_IMAGETERMINAL  1  // Ignore -> MISRA-C++ Rules 16-2-2
#endif
#endif
/* The above definition is a remnant of the external terminal processing part inside *
 * the sennsor that was enabled or disabled, and is not originally required at all.  *
 * Enable all sensor internal processing and then delete.  */

/*----------------------------------------------------------------------*
 *  Data ID corresponding to vehicle sensor information                 *
 *----------------------------------------------------------------------*/
/*
 *  Note.
 *  This feature needs to be defined by the vendor.
 */

/**
 * \~english  Destination information
 */
#define  VEHICLE_DID_DESTINATION      (0x00000001UL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Hybrid electric vehicle information
 */
#define  VEHICLE_DID_HV          (0x00000002UL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Steering wheel information
 */
#define  VEHICLE_DID_STEERING_WHEEL    (0x00000003UL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Voltage
 */
#define  VEHICLE_DID_VB          (0x00000004UL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  IG information
 */
#define  VEHICLE_DID_IG          (0x00000005UL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Microphone information
 */
#define  VEHICLE_DID_MIC          (0x00000006UL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Back door information
 */
#define  VEHICLE_DID_BACKDOOR      (0x00000007UL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Parking brake information
 */
#define  VEHICLE_DID_PKB          (0x00000008UL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  ADIM information
 */
#define  VEHICLE_DID_ADIM        (0x00000009UL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Illumination information
 */
#define  VEHICLE_DID_ILL          (0x0000000AUL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  RHEOSTAT information
 */
#define  VEHICLE_DID_RHEOSTAT      (0x0000000BUL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  System temperature information
 */
#define  VEHICLE_DID_SYSTEMP        (0x0000000CUL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Reverse information
 */
#define  VEHICLE_DID_REV          (0x00000017UL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Reverse information(line sensor)
 */
#define  VEHICLE_DID_REV_LINE      (0x0000000EUL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Reverse CAN information
 */
#define  VEHICLE_DID_REV_CAN        (0x0000000FUL)  // Ignore -> MISRA-C++ Rule 16-2-2
/**
 * \~english  SPEED information
 */
#define  VEHICLE_DID_SPEED        (0x00000010UL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  SPEED PULSE information
 */
#define  VEHICLE_DID_SPEED_PULSE        (0x00000011UL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  VB information
 */
#define  VEHICLE_DID_VBLOW        (0x00000012UL)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Drive information(2WD/4WD)
 */
#define  VEHICLE_DID_DVINF                (0x00000013UL)

/**
 * \~english  Transmission information
 */
#define  VEHICLE_DID_VARTRM1                (0x00000014UL)

/**
 * \~english  Shift position signal(R, P, N, D)
 */
#define  VEHICLE_DID_SHIFT                (0x00000015UL)

/**
 * \~english  Direct line sensor(PKB)
 */
#define  VEHICLE_DID_PKB_LINE              (0x00000016UL)

/**
 * \~english  D seat door signal switch
 */
#define VEHICLE_DID_DSEAT_DOOR              (0x0000000DUL)

/**
 * \~english  P seat door signal switch
 */
#define VEHICLE_DID_PSEAT_DOOR              (0x00000018UL)

/**
 * \~english  RR seat door signal switch
 */
#define VEHICLE_DID_RRSEAT_DOOR              (0x00000019UL)

/**
 * \~english  RL seat door signal switch
 */
#define VEHICLE_DID_RLSEAT_DOOR              (0x0000001AUL)

/**
 * \~english  IGP information
 */
#define VEHICLE_DID_IGP                      (0x0000001BUL)

/**
 * \~english  HV information status
 */
#define VEHICLE_DID_HV_STS              (0x0000001CUL)

/**
 * \~english  Shift position signal(Unspecified)
 */
#define VEHICLE_DID_SHIFT_U             (0x0000001DUL)

/**
 * \~english USB power supply
 */
#define VEHICLE_DID_USBPOWERSUPPLY      (0x800000A0UL)

/**
 * \~english  Analog Microphone Connect information
 */
#define  VEHICLE_DID_ANALOG_MIC        (0x800000B0UL)

/**
 * \~english  MultiMic Connect information
 */
#define  VEHICLE_DID_MULTI_MIC         (0x800000B1UL)

/*----------------------------------------------------------------------*
 *  Vehicle sensor information data                        *
 *----------------------------------------------------------------------*/
/**
 * \~english  OFF
 */
#define  VEHICLE_SNS_OFF          (0x00U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  ON
 */
#define  VEHICLE_SNS_ON          (0x01U)  // Ignore -> MISRA-C++ Rule 16-2-2

#define  VEHICLE_SNS_BLACKOUT    (0x02U)  // Disruptions
/**
 * \~english  Undefined
 */
#define  VEHICLE_SNS_UNDEF        (0x03U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Disconnection
 */
#define  VEHICLE_SNS_UNCONNECTION    (0x00U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Connection
 */
#define  VEHICLE_SNS_CONNECTION      (0x01U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Uncertainty
 */
#define  VEHICLE_SNS_BW_UNCERTAINTY   (0x02U)

/**
 * \~english  Unspecified
 */
#define  VEHICLE_SNS_UNSPECIFIED      (0x03U)  // Ignore -> MISRA-C++ Rule 16-2-2
/* #GPF_60_023 ADD START */
#if (CONFIG_EXTTERM_INPUT_DAB2BIT | CONFIG_EXTTERM_INPUT_GPS2BIT)    // Ignore -> MISRA-C++ Rules 16-1-2, 16-2-2
/**
 * \~english  Short
 */
#define  VEHICLE_SNS_SHORT        (0x02U)  // Ignore -> MISRA-C++ Rule 16-2-2
#endif
/* #GPF_60_023 ADD END */

/**
 * \~english  Hybrid electric vehicle
 */
#define  VEHICLE_SNS_HV          (0x01U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Plug-in hybrid electric vehicle
 */
#define  VEHICLE_SNS_PHV          (0x02U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Electric vehicle
 */
#define  VEHICLE_SNS_EV           (0x03U)

/**
 * \~english  Fuel Cell vehicle
 */
#define  VEHICLE_SNS_FCV          (0x04U)

/**
 * \~english  1 motor vehicle
 */
#define  VEHICLE_SNS_1MOT         (0x05U)

/**
 * \~english  Conventional(48V) vehicle
 */
#define  VEHICLE_SNS_48V          (0x06U)

/**
 *  \~english S&S vehicle
 */
#define  VEHICLE_SNS_SANDS        (0x07U)

/**
 * \~english  Conventional vehicle
 */
#define  VEHICLE_SNS_NONHV        (0x00U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Two wheel drive
 */
#define  VEHICLE_SNS_2WD          (0x00U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**gb
 * \~english  Four wheel drive
 */
#define  VEHICLE_SNS_4WD          (0x01U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Left handle
 */
#define  VEHICLE_SNS_LH          (0x01U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Right handle
 */
#define  VEHICLE_SNS_RH          (0x02U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Close
 */
#define  VEHICLE_SNS_CLOSE        (0x00U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Open
 */
#define  VEHICLE_SNS_OPEN        (0x01U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Back door uncertainty
 */
#define  VEHICLE_SNS_BD_UNCERTAINTY    (0x02U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  ADIM uncertainty
 */
#define  VEHICLE_SNS_ADIM_UNCERTAINTY  (0x00U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  ADIM undefined
 */
#define  VEHICLE_SNS_ADIM_UNDEF      (0x01U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  ADIM off
 */
#define  VEHICLE_SNS_ADIM_OFF      (0x02U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  ADIM on
 */
#define  VEHICLE_SNS_ADIM_ON        (0x03U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Drive information(2WD)
 */
#define  VEHICLE_SNS_DVINF_2WD      (0x00U)

/**
 * \~english  Drive information(4WD)
 */
#define  VEHICLE_SNS_DVINF_4WD      (0x01U)

/**
 * \~english  Drive information uncertainty
 */
#define  VEHICLE_SNS_DVINF_UNCERTAINTY  (0x03U)

/**
 * \~english  Transmission information(MT)
 */
#define  VEHICLE_SNS_VARTRM1_MT      (0x00U)

/**
 * \~english  Transmission information(AT)
 */
#define  VEHICLE_SNS_VARTRM1_AT      (0x01U)

/**
 * \~english  Transmission information(CVT)
 */
#define  VEHICLE_SNS_VARTRM1_CVT      (0x02U)

/**
 * \~english  Transmission information(MMT)
 */
#define  VEHICLE_SNS_VARTRM1_MMT      (0x03U)

/**
 * \~english  Transmission information(SMT)
 */
#define  VEHICLE_SNS_VARTRM1_SMT      (0x04U)

/**
 * \~english  Transmission information(HV_AT)
 */
#define  VEHICLE_SNS_VARTRM1_HV_AT    (0x05U)

/**
 * \~english  Transmission information uncertainty
 */
#define  VEHICLE_SNS_VARTRM1_UNCERTAINTY  (0x0FU)

/**
 * \~english  D seat door uncertainty
 */
#define VEHICLE_SNS_DS_UNCERTAINTY    (0x02U)

/**
 * \~english  P seat door uncertainty
 */
#define VEHICLE_SNS_PS_UNCERTAINTY    (0x02U)

/**
 * \~english  RR seat door uncertainty
 */
#define VEHICLE_SNS_RRS_UNCERTAINTY    (0x02U)

/**
 * \~english  RL seat door uncertainty
 */
#define VEHICLE_SNS_RLS_UNCERTAINTY    (0x02U)

/**
 * \~english  HV information not decision
 */
#define  VEHICLE_SNS_HV_NOT_DECISION    (0x00U)

/**
 * \~english  HV information decision
 */
#define VEHICLE_SNS_HV_DECISION      (0x01U)

/**
 * @enum VehicleSnsAnlogMicBit
 * \~english  ANALOGMIC connect bit information
 */
enum VehicleSnsAnlogMicBit {
  VEHICLE_SNS_ANALOG_MIC1_BIT = 0x02,  //!< \~english ANALOGMIC1 connect
  VEHICLE_SNS_ANALOG_MIC2_BIT = 0x04,  //!< \~english ANALOGMIC2 connect
};

/**
 * @enum VehicleSnsMultiMicBit
 * \~english  MULTI MIC connect bit information
 */
enum VehicleSnsMultiMicBit {
  VEHICLE_SNS_A2B_HUB_BIT = 0x01,           //!< \~english A2BHUB connect
  VEHICLE_SNS_MULTI_MIC1_BIT = 0x02,        //!< \~english MULTIMIC1 connect
  VEHICLE_SNS_MULTI_MIC2_BIT = 0x04,        //!< \~english MULTIMIC2 connect
  VEHICLE_SNS_MULTI_MIC3_BIT = 0x08,        //!< \~english MULTIMIC3 connect
  VEHICLE_SNS_MULTI_MIC4_BIT = 0x10,        //!< \~english MULTIMIC4 connect
  VEHICLE_SNS_MULTI_MIC5_BIT = 0x20,        //!< \~english MULTIMIC5 connect
  VEHICLE_SNS_MULTI_MIC6_BIT = 0x40,        //!< \~english MULTIMIC6 connect
  VEHICLE_SNS_ANC_BIT = 0x80,               //!< \~english ANC connect
};

/*-------------------------------------------------------------------------------
 * Time status return value
 -------------------------------------------------------------------------------*/
/**
 * \~english  Real time clock
 */
#define  GPS_TIMESTS_RTC      (0x02U)  // Ignore -> MISRA-C++ Rule 16-2-2

/**
 * \~english  Exception time
 */
#define  GPS_TIMESTS_NG      (0x03U)  // Ignore -> MISRA-C++ Rule 16-2-2

/** @}*/  // end of vehicle
/** @}*/  // end of VehicleService
/** @}*/  // end of BaseSystem

#endif  // VEHICLE_CLIENT_INCLUDE_VEHICLE_SERVICE_VEHICLE_SENSOR_COMMON_API_H_
