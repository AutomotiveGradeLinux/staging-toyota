/*
 * @copyright Copyright (c) 2018-2019 TOYOTA MOTOR CORPORATION.
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
 * @file
 *    VehilceIf.cpp
 * @brief
 *    Vehicle Service Interface
 */

/*---------------------------------------------------------------------------------*
 * Include Files                                                                   *
 *---------------------------------------------------------------------------------*/
#include <vehicle_service/positioning_base_library.h>
//#include "stub/Vehicle_Sensor_Common_API.h"
//#include <stub/Vehicle_API.h>
//#include "stub/vehicle_notifications.h"

#include "VehicleIf.h"

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Structre                                                                        *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Local Function Prototype                                                        *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Grobal Value                                                                    *
 *---------------------------------------------------------------------------------*/
static BOOL gb_vehicleAvailability = FALSE;

/*---------------------------------------------------------------------------------*
 * Function                                                                        *
 *---------------------------------------------------------------------------------*/
/**
 * @brief
 *    Registering Vehicle Services IF Callback Functions
 *
 * @param[in]  *p_msg_handler    Callback function table
 * @param[in]  Ui_handler_count Number of callback functions
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus VehicleIfAttachCallbacksToDispatcher(    // NOLINT(readability/nolint)
                                                _FrameworkunifiedProtocolCallbackHandler const* p_msg_handler,
                                                unsigned int ui_handler_count) {
  EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
  HANDLE happ;

  happ = _pb_GetAppHandle();
  if (NULL != happ) {
    estatus = FrameworkunifiedAttachCallbacksToDispatcher(happ, "NS_ANY_SRC", p_msg_handler, ui_handler_count);
    if (eFrameworkunifiedStatusOK != estatus) {  // LCOV_EXCL_BR_LINE 4: nsfw error
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                    "FrameworkunifiedAttachCallbacksToDispatcher ERROR!! [estatus=%d]", estatus);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
  }

  return estatus;
}

/**
 * @brief
 *    Vehicle IF: Set Availability Status
 *
 * @details  This function sets status of Vehicle/Availablity stored in local. <br>
 *           Only when the status is true, Vehicle I/F is called by wrappers.
 *
 * @param[in]  BOOL bIsAvailable : Availability Status
 */
void VehicleIf_SetAvailability(BOOL bIsAvailable)
{
    gb_vehicleAvailability = bIsAvailable;

    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "gb_vehicleAvailability=%d", gb_vehicleAvailability);

    return;
}

/**
 * @brief
 *    Remove Vehicle Services IF Callback Functions
 *
 * @param[in]  *p_msg_handler    CID table
 * @param[in]  ui_handler_count  Number of CIDs
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus VehicleIfDetachCallbacksFromDispatcher(const PUI_32 pui_cmd_array, UI_32 ui_command_count) {
  EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
  HANDLE happ;

  happ = _pb_GetAppHandle();
  if (NULL != happ) {
    estatus = FrameworkunifiedDetachCallbacksFromDispatcher(happ, "NS_ANY_SRC", pui_cmd_array, ui_command_count, NULL);
    /* In the event of failure */
    if (eFrameworkunifiedStatusOK != estatus) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
                    "PositioningDetachCallbacksToDispatcher Failed in FrameworkunifiedOnStop [estatus=%d]", estatus);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
  }

  return estatus;
}

/**
 * @brief
 *    Vehicle IF: Get Type of Transmission
 *
 * @details  This function is wrapper function of Vehicle I/F. <br>
 *           You can get the type of transmission of the vehicle.
 *
 * @param[out] uint8_t* pType     : Type of Transmission
 * @param[out] BOOL* bIsAvailable : Availability Status
 *
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */

//EFrameworkunifiedStatus VehicleIf_GetTypeOfTransmission(uint8_t* pType, BOOL* pbIsAvailable)
EFrameworkunifiedStatus VehicleIf_GetTypeOfTransmission(uint8_t* pType, uint8_t* pPkb, BOOL* pbIsAvailable)
{
    EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
//    VEHICLE_RET_API ret;
//    HANDLE hApp;
//    uint8_t ucVartrm;
//
//#if 1 /* Plus _CWORD27_ Gear Data Support 180115 */
//    uint8_t ucPkb;
//#endif /* Plus _CWORD27_ Gear Data Support 180115 */
//
//
//    if (gb_vehicleAvailability == TRUE)
//    {
//        hApp = _pb_GetAppHandle();
//        if (hApp != NULL)
//        {
//            ret = Vehicle_GetVehicleData(hApp, VEHICLE_DID_VARTRM1, &ucVartrm, sizeof(ucVartrm));
//            if (ret < VEHICLE_RET_NORMAL)
//            {
//                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ERROR: Vehicle_GetVehicleData:%d", ret);
//            }
//            else
//            {
//                eStatus = eFrameworkunifiedStatusOK;
//
//                switch (ucVartrm)
//                {
//                    case VEHICLE_SNS_VARTRM1_AT:
//                    case VEHICLE_SNS_VARTRM1_CVT:
//                    case VEHICLE_SNS_VARTRM1_HV_AT:
//                    {
//                        *pType = VEHICLEIF_TRANSMISSION_TYPE_AT;
//                        break;
//                    }
//                    case VEHICLE_SNS_VARTRM1_MT:
//                    case VEHICLE_SNS_VARTRM1_MMT:
//                    case VEHICLE_SNS_VARTRM1_SMT:
//                    {
//                        *pType = VEHICLEIF_TRANSMISSION_TYPE_MT;
//                        break;
//                    }
//                    case VEHICLE_SNS_VARTRM1_UNCERTAINTY:
//                    {
//                        *pType = VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN;
//                        break;
//                    }
//                    default:
//                    {
//                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
//                        "ERROR: Type of transmission is unknown. (%d)", ucVartrm);
//
//                        *pType = VEHICLEIF_TRANSMISSION_TYPE_UNKNOWN;
//                        break;
//                    }
//                }
//            }
//
//#if 1 /* Plus _CWORD27_ Gear Data Support 180115 */
//            ret = Vehicle_GetVehicleData(hApp, VEHICLE_DID_PKB, &ucPkb, sizeof(ucPkb));
//            if (ret < VEHICLE_RET_NORMAL)
//            {
//                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ERROR: Vehicle_GetVehicleData:%d", ret);
//            }
//            else
//            {
//                eStatus = eFrameworkunifiedStatusOK;
//
//                switch (ucPkb)
//                {
//                    case VEHICLE_SNS_OFF:       // R-state
//                    //case VEHICLE_SNS_ANTI:      // Antilock(Vehicle undefined)
//                    {
//                        *pPkb = VEHICLEIF_PKB_OFF;
//                        break;
//                    }
//                    case VEHICLE_SNS_ON:        // Lock state
//                    {
//                        *pPkb = VEHICLEIF_PKB_ON;
//                        break;
//                    }
//                    default:
//                    {
//                        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
//                        "ERROR: State of parking brake is unknown. (%d)", ucPkb);
//
//                        *pPkb = VEHICLEIF_PKB_OFF;
//                        break;
//                    }
//                }
//            }
//#endif /* Plus _CWORD27_ Gear Data Support 180115 */
//
//        }
//        else
//        {
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ERROR: _pb_GetAppHandle hApp:%p", hApp);
//        }
//    }
//    else
//    {
//        /* nop */
//    }
//
//    *pbIsAvailable = gb_vehicleAvailability;

    return eStatus;
}

/**
 * @brief
 *    Vehicle Services IF Availability Change Notification Registration
 *
 * @param[in]  fp_on_cmd    Callback function
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus VehicleIfNotifyOnVehicleAvailability(CbFuncPtr fp_on_cmd) {
  EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
  HANDLE happ;

  happ = _pb_GetAppHandle();
  if (NULL != happ) {
    /* Vehicle/Availability Changing notification registration */
//    estatus = FrameworkunifiedSubscribeNotificationWithCallback(happ, NTFY_Vehicle_Availability, fp_on_cmd);  // LCOV_EXCL_BR_LINE 6:unexpected branch  //NOLINT (whitespace/line_length)
//    if (eFrameworkunifiedStatusOK != estatus) {  // LCOV_EXCL_BR_LINE 4: nsfw error
//      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
//                    "PositioningSubscriveNotificationswithCallback ERROR!! [estatus=%d]", estatus);
//    }
  } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
  }

  return estatus;
}

/**
 * @brief
 *    Vehicle IF: Get Shift Position
 *
 * @details  This function is wrapper function of Vehicle I/F. <br>
 *           You can get the shift position of the vehicle.
 *
 * @param[out] uint8_t* pShift    : Shift Position
 * @param[out] BOOL* bIsAvailable : Availability Status
 *
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */
EFrameworkunifiedStatus VehicleIf_GetShiftPosition(uint8_t* pShift, BOOL* pbIsAvailable)
{
    EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
//    VEHICLE_RET_API ret;
//    HANDLE hApp;
//
//    if (gb_vehicleAvailability == TRUE)
//    {
//        hApp = _pb_GetAppHandle();
//        if (hApp != NULL)
//        {
//            ret = Vehicle_GetVehicleData(hApp, VEHICLE_DID_SHIFT, pShift, sizeof(*pShift));
//            if (ret < VEHICLE_RET_NORMAL)
//            {
//                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ERROR: Vehicle_GetVehicleData:%d", ret);
//            }
//            else
//            {
//                eStatus = eFrameworkunifiedStatusOK;
//            }
//        }
//        else
//        {
//            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "ERROR: _pb_GetAppHandle hApp:%p", hApp);
//        }
//    }
//    else
//    {
//        /* nop */
//    }
//
//    *pbIsAvailable = gb_vehicleAvailability;

    return eStatus;
}


/**
 * @brief
 *    Vehicle services IF Info data-delivery registry
 *
 * @param[in]  ul_did
 * @return     eFrameworkunifiedStatusOK
 * @return     eFrameworkunifiedStatusFail
 */

EFrameworkunifiedStatus VehicleIfDeliveryEntry(uint32_t ul_did) {
  EFrameworkunifiedStatus estatus = eFrameworkunifiedStatusFail;
//  HANDLE happ;
//  VEHICLE_RET_API iret;
//
//  if (TRUE == gb_vehicleAvailability) {
//    happ = _pb_GetAppHandle();
//    if (NULL != happ) {
//      /* Sensor data delivery registration */
//      iret = Vehicle_DeliveryEntry(happ, (PCSTR)POS_THREAD_NAME, ul_did,
//                                          VEHICLE_DELIVERY_REGIST, VEHICLE_DELIVERY_TIMING_UPDATE);
//      if (VEHICLE_RET_NORMAL != iret) {
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, 
//                                            "Vehicle_DeliveryEntry ERROR!! [iret=%d]", iret);
//      } else {
//        estatus = eFrameworkunifiedStatusOK;
//      }
//    } else {
//        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "_pb_GetAppHandle ERROR!! [happ=%p]", happ);
//    }
//  } else {
//      /* nop */
//  }
  return estatus;
}

