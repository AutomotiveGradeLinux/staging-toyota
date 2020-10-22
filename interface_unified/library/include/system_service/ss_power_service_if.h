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
 * @file ss_power_service_if.h
 * @brief \~english This file supports the Power Service client interface.
 */
/**
 * @file
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup power_service
 *  @ingroup system_service
 *  @{
 */

#ifndef POWER_SERVICE_CLIENT_INCLUDE_SYSTEM_SERVICE_SS_POWER_SERVICE_IF_H_  // NOLINT (build/header_guard)
#define POWER_SERVICE_CLIENT_INCLUDE_SYSTEM_SERVICE_SS_POWER_SERVICE_IF_H_

#include "system_service/ss_power_service.h"
#include "system_service/ss_power_service_protocol.h"


////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup OpenPowerService
/// \~english @par Summary
///       Obtain a handle for using the PowerService.
/// \~english @param [in] f_hApp
///       HANDLE    - Application handle
/// \~english @retval Handle     Success
/// \~english @retval NULL       Failed
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only(None communication)
/// \~english @par Detail
///       - The API creates a communication resource between
///         the user service and the PowerService
///         and returns a handle for using the PowerService.\n
///       \n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE OpenPowerService(HANDLE f_hApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup ClosePowerService
/// \~english @par Summary
///       Free a handle for using PowerService.
/// \~english @param [in] f_hApp
///       HANDLE    - Application handle
/// \~english @param [in] f_hService
///       HANDLE    - PowerService handle
/// \~english @retval eFrameworkunifiedStatusOK             Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle    Invalid handle
/// \~english @retval eFrameworkunifiedStatusFail           Failed
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail
///       - The API terminates the communication resource between
///         the user service and the PowerServiceand frees the given handle. \n
///       \n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus ClosePowerService(HANDLE f_hApp, HANDLE f_hService);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PwrServiceOpenSessionRequest
/// \~english @par Summary
///       Make the PowerService available.
/// \~english @param [in] f_hService
///       HANDLE    - PowerService handle
/// \~english @param [in] f_eSessionType
///       EPWR_SESSION_TYPE    - Session type
/// \~english @retval eFrameworkunifiedStatusOK             Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle    Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldParam     Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail           Failed
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Fire and Forget x Method
/// \~english @par Detail
///       - The API opens a session between the user service
///         and the PowerService and makes the PowerService available. \n
///       \n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PwrServiceOpenSessionRequest(HANDLE f_hService,
                                        EPWR_SESSION_TYPE f_eSessionType);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PwrServiceCloseSessionRequest
/// \~english @par Summary
///       End using the PowerService.
/// \~english @param [in] f_hService
///       HANDLE    - PowerService handle
/// \~english @param [in] f_hSession
///       HANDLE    - Session type
/// \~english @retval eFrameworkunifiedStatusOK             Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle    Invalid handle
/// \~english @retval eFrameworkunifiedStatusFail           Failed
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Fire and Forget x Method
/// \~english @par Detail
///       - The API closes the session between the user service
///         and the PowerService and terminatesthe PowerSerivce use. \n
///       \n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PwrServiceCloseSessionRequest(HANDLE f_hService, HANDLE f_hSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PwrServiceDecodeOpenSessionResponse
/// \~english @par Summary
///       Obtain a handle for OpenSessionRequest response message.
/// \~english @param [in] f_hApp
///       HANDLE     - Application handle
/// \~english @param [in] f_hSession
///       HANDLE&    - Address of storing session handle
/// \~english @retval eFrameworkunifiedStatusOK             Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle    Invalid handle
/// \~english @retval eFrameworkunifiedStatusNullPointer    NULL Pointer
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync only(None communication)
/// \~english @par Detail
///       - The API obtains a handle for OpenSessionRequest
///         response message using the given application handle.\n
///       \n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PwrServiceDecodeOpenSessionResponse(HANDLE f_hApp,
                                               HANDLE& f_hSession);  // NOLINT (runtime/references)

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PwrServiceSetVoltageState
/// \~english @par Summary
///       Set power supply state.
/// \~english @param [in] f_hSession
///       HANDLE    - Session handle
/// \~english @param [in] f_eVoltage_state
///       EPWR_VOLTAGE_STATE_TYPE    - Power supply status value
/// \~english @par
///       - Power supply status value
///         <table>
///         <tr bgcolor="black"><th>Name</th><th>Value</th></tr>
///         <tr><td>epsvsINVALID</td><td>0xFF</td></tr>
///         <tr><td>epsvsNORMAL</td><td>0x20</td></tr>
///         <tr><td>epsvsLVI1</td><td>0x22</td></tr>
///         <tr><td>epsvsLVI2</td><td>0x24</td></tr>
///         </table>
/// \~english @retval eFrameworkunifiedStatusOK             Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle    Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldParam     Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail           Failed
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Method only
/// \~english @par Detail
///       - The API sets power supply state to the PowerService. \n
///       \n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PwrServiceSetVoltageState(HANDLE f_hSession,
                    EPWR_VOLTAGE_STATE_TYPE  f_eVoltage_state);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PwrServiceSetCrankState
/// \~english @par Summary
///       Set crank state.
/// \~english @param [in] f_hSession
///       HANDLE    - Session handle
/// \~english @param [in] f_eCrank_state
///       EPWR_CRANK_STATE_TYPE    - Crank state value
/// \~english @par
///       - Crank state value
///         <table>
///         <tr bgcolor="black"><th>Name</th><th>Value</th></tr>
///         <tr><td>epscsINVALID</td><td>0xFF</td></tr>
///         <tr><td>epscsENTRY</td><td>0xA0</td></tr>
///         <tr><td>epscsEXIT</td><td>0xA2</td></tr>
///         </table>
/// \~english @retval eFrameworkunifiedStatusOK             Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle    Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldParam     Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail           Failed
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Fire and Forget only
/// \~english @par Detail
///       - The API sets crank state to the PowerService.\n
///       \n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PwrServiceSetCrankState(HANDLE f_hSession,
                  EPWR_CRANK_STATE_TYPE f_eCrank_state);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PwrServiceSystemModeInfoRequest
/// \~english @par Summary
///       Request to obtain the system mode information.
/// \~english @param [in] f_hSession
///       HANDLE    - Session handle
/// \~english @retval eFrameworkunifiedStatusOK             Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle    Invalid handle
/// \~english @retval eFrameworkunifiedStatusFail           Failed
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Method x Method
/// \~english @par Detail
///       - The API requests the PowerService to obtain
///         the system mode information.\n
///       \n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PwrServiceSystemModeInfoRequest(HANDLE f_hSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PwrServiceSendInitCompReport
/// \~english @par Summary
///       Send an initialize complete response.
/// \~english @param [in] f_hSession
///       HANDLE    - Session handle
/// \~english @retval eFrameworkunifiedStatusOK             Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle    Invalid handle
/// \~english @retval eFrameworkunifiedStatusFail           Failed
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Fire and Forgeat x Fire and Forget
/// \~english @par Detail
///       - The API sends an initialize complete response to the PowerService.\n
///       \n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PwrServiceSendInitCompReport(HANDLE f_hSession);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PwrServiceSendSetShutdownPopupRequest
/// \~english @par Summary
///       Obtain shutdown state.
/// \~english @param [in] f_hSession
///       HANDLE    - Session handle
/// \~english @param [in] f_eShutdownPopup
///       EPWR_SHUTDOWN_POPUP_TYPE    - Shutdown state
/// \~english @retval eFrameworkunifiedStatusOK             Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle    Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldParam     Invalid parameter
/// \~english @retval eFrameworkunifiedStatusFail           Failed
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Method only
/// \~english @par Detail
///       - The API obtains the shutdown state from the PowerService. \n
///       \n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PwrServiceSendSetShutdownPopupRequest(HANDLE f_hSession,
                             EPWR_SHUTDOWN_POPUP_TYPE f_eShutdownPopup);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PwrServiceSendStartupConfirmationMsgRequest
/// \~english @par Summary
///       Send startup confirmation request.
/// \~english @param [in] f_hSession
///       HANDLE    - Session handle
/// \~english @param [in] f_eState
///       EPWR_SC_MSG_STRUCT
///       - Address of the structure for startup confirmation request message
/// \~english @par
///       - Structure for startup confirmation request message
/// \~english @code
///       typedef struct {
///           EPWR_SC__CWORD56__BOOT_MODE_TYPE _CWORD56_BootMode;
///           EPWR_SC_WAKEUP_TYPE         wakeupType;
///           EPWR_SC_COLD_START_REQ_TYPE coldStartRequest;
///           EPWR_SC_SECURITY_STATUS     securityStatus;
///           UI_32                       HWVersion;
///           UI_32                       matchedHardwareType;
///           UI_32                       softwareType;
///           UI_32                       imageType;
///           UI_32                       majorVersion;
///           UI_32                       minorVersion;
///           UI_32                       SWVersionYear;
///           UI_32                       SWVersionWeek;
///           UI_32                       SWVersionPatchLevel;
///       } StartupConfirmationMsgStrut, EPWR_SC_MSG_STRUCT;
///            @endcode\n
///       \n
///       - EPWR_SC__CWORD56__BOOT_MODE_TYPE
///         <table>
///         <tr bgcolor="black"><th>Name</th><th>Value</th></tr>
///         <tr><td>eps_CWORD56_bmINVALID</td><td>0xFF</td></tr>
///         <tr><td>eps_CWORD56_bmAPPLICATION_MODE</td><td>0x00</td></tr>
///         <tr><td>eps_CWORD56_bmPROGRAMMING_MODE</td><td>0x08</td></tr>
///         </table>\n
///       \n
///       - EPWR_SC_WAKEUP_TYPE
///         <table>
///         <tr bgcolor="black"><th>Name</th><th>Value</th></tr>
///         <tr><td>epsstINVALID</td><td>0xFF</td></tr>
///         <tr><td>epsstWARMSTART</td><td>0x00</td></tr>
///         <tr><td>epsstCOLDSTART</td><td>0x01</td></tr>
///         </table>\n
///       \n
///       - EPWR_SC_COLD_START_REQ_TYPE
///         <table>
///         <tr bgcolor="black"><th>Name</th><th>Value</th></tr>
///         <tr><td>epsscrtINVALID</td><td>0xFF</td></tr>
///         <tr><td>epsscrtNOT_REQUIRED</td><td>0x00</td></tr>
///         <tr><td>epsscrtREQUIRED</td><td>0x01</td></tr>
///         </table>\n
///       \n
///       - EPWR_SC_SECURITY_STATUS
///         <table>
///         <tr bgcolor="black"><th>Name</th><th>Value</th></tr>
///         <tr><td>epsssINVALID</td><td>0xFF</td></tr>
///         <tr><td>epsssUNLOCK</td><td>0x00</td></tr>
///         <tr><td>epsssLOCK</td><td>0x01</td></tr>
///         </table>\n
///       \n
/// \~english @retval eFrameworkunifiedStatusOK             Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle    Invalid handle
/// \~english @retval eFrameworkunifiedStatusFail           Failed
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Fire and Forget x Fire and Forget
/// \~english @par Detail
///       - The API sends a startup confirmation request to the PowerService.\n
///       \n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PwrServiceSendStartupConfirmationMsgRequest(HANDLE f_hSession
                                          , EPWR_SC_MSG_STRUCT f_eState);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PwrServiceSendPowerRequest
/// \~english @par Brief
///       Send power request.
/// \~english @param [in] f_hSession
///       HANDLE    - Session handle
/// \~english @param [in] f_eState
///       \ref PowerRequestMsgStrutWithUMCR "EPWR_POWER_REQUEST_MSG_STRUCT_WITH_UMCR&" - Address of the structure for power request message
/// \~english @par
///       \n
///       Of the eight elements of the \ref PowerRequestMsgStrutWithUMCR "EPWR_POWER_REQUEST_MSG_STRUCT_WITH_UMCR" structure, five elements set fixed values.\n
///       startupType = \ref epsprm_CWORD102_si_CWORD102__STARTUP_NORMAL\n
///       openingType = \ref epsprmotTIMING_TYPE_1\n
///       lastUserMode = \ref epsumON\n
///       manResetInfo = \ref epsprmriNORMAL\n
///       lastSystemRetentionReq = \ref epsprlsrrLAST_SYSTEM_OFF\n 
///       \n
///       For the following, set different values at system startup and shutdown.\n
///       (Element name) = (Startup settings or Shutdown settings)\n
///       startupReason = (\ref epswfIGN_ACC or \ref epswfINVALID)\n
///       userMode = (\ref epsumON or \ref epsumOFF)\n
///       \n
///       For the following, set different values at power state transition.\n
///       <table>
///       <tr bgcolor="black"><th>Element name</th><th>Power state</th><th>Value</th></tr>
///       <tr><td rowspan=7>userModeChangeReason</td><tr><td>Not Available</td><td>\ref epsumcrNOT_AVAILABLE</td></tr>
///       <tr><td>  </td><td>\ref epsumcrON_KEY</td></tr>
///       <tr><td>Parking(+B)</td><td>\ref epsumcrPARKING_B</td></tr>
///       <tr><td>Pre-boot(+BA)</td> <td>\ref epsumcrPRE_BA</td></tr>
///       <tr><td>Normal-boot</td> <td>\ref epsumcrNORMAL</td></tr>
///       <tr><td>Background-boot(+BA)</td> <td>\ref epsumcrBACKGROUND_BA</td></tr>
///       </table>
///       \n
/// \~english @retval eFrameworkunifiedStatusOK             Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle    Invalid handle
/// \~english @retval eFrameworkunifiedStatusFail           Failed
/// \~english @par Prerequisite
///       None
/// \~english @par Change of internal state
///       None
/// \~english @par Conditions of processing failure
///     - When the session handle (f_hSession) specified by the argument is NULL. [eFrameworkunifiedStatusInvldHandle]
///     - The process fails for some reason. [eFrameworkunifiedStatusFail]
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Method only
/// \~english @par Detail
///       - The API send a power request to the PowerService.\n
///       \n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PwrServiceSendPowerRequest(HANDLE f_hSession,
                                      EPWR_POWER_REQUEST_MSG_STRUCT_WITH_UMCR& f_eState);


////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PwrServiceSendShutdownRequest
/// \~english @par Summary
///       Send shutdown request.
/// \~english @param [in] f_hSession
///       HANDLE    - Session handle
/// \~english @param [in] f_eState
///       EPWR_SHUTDOWN_REQUEST_MSG_STRUCT&
///      - Address of the structure for shutdown request message
/// \~english @par
///       - Structure for shutdown request message
/// \~english @code
///       typedef struct {
///           EPWR_ONS_TYPE                  ONS_Type;
///           EPWR_SHUTDOWN_TRIGGER_TYPE     shutdownTrigger;
///           EPWR_USER_MODE_TYPE            lastUserMode;
///           EPWR_TRANSPORT_MODE_TYPE       transportMode;
///           EPWR_PROD_MODE_TYPE            productionMode;
///           EPWR_LHC_TYPE                  limpHomeCutoffRequest;
///       } ShutdownRequestMsgStrut, EPWR_SHUTDOWN_REQUEST_MSG_STRUCT
///            @endcode\n
///       \n
///       - EPWR_ONS_TYPE
///         <table>
///         <tr bgcolor="black"><th>Name</th><th>Value</th></tr>
///         <tr><td>epssdmonsINVALID</td><td>0xFF</td></tr>
///         <tr><td>epssdmonsNO_ONS</td><td>0x00</td></tr>
///         <tr><td>epssdmonsONS</td><td>0x01</td></tr>
///         </table>\n
///       \n
///       - EPWR_SHUTDOWN_TRIGGER_TYPE
///         <table>
///         <tr bgcolor="black"><th>Name</th><th>Value</th></tr>
///         <tr><td>epssdmsdtINVALID</td><td>0xFF</td></tr>
///         <tr><td>epssdmsdtTESTACC_OFF</td><td>0x00</td></tr>
///         <tr><td>epssdmsdtON_KEY</td><td>0x01</td></tr>
///         <tr><td>epssdmsdtIGN_LOCK</td><td>0x02</td></tr>
///         <tr><td>epssdmsdtPWR_SAVE</td><td>0x03</td></tr>
///         <tr><td>epssdmsdtTMP_STARTUP</td><td>0x04</td></tr>
///         <tr><td>epssdmsdtDIAG_DEACTIVATION</td><td>0x05</td></tr>
///         <tr><td>epssdmsdtABNORMAL_VOLTAGE</td><td>0x06</td></tr>
///         <tr><td>epssdmsdtABNORMAL_TEMP</td><td>0x07</td></tr>
///         <tr><td>epssdmsdtBATTERYCUTOFF</td><td>0x08</td></tr>
///         <tr><td>epssdmsdtLIMPHOME</td><td>0x09</td></tr>
///         <tr><td>epssdmsdtHU_CAN_ERROR</td><td>0x0A</td></tr>
///         <tr><td>epssdmsdtBODY_CAN_ERROR</td><td>0x0B</td></tr>
///         <tr><td>epssdmsdtTRANSPORT_MODE</td><td>0x0C</td></tr>
///         <tr><td>epssdmsdtPRODUCTION_MODE</td><td>0x0D</td></tr>
///         <tr><td>epssdmsdtIGN_OFF</td><td>0x0E</td></tr>
///         <tr><td>epssdmsdtGENERIC_ERROR_RESET</td><td>0x0F</td></tr>
///         <tr><td>epssdmsdtFATAL_ERROR_RESET</td><td>0x10</td></tr>
///         <tr><td>epssdmsdtUSER_DATA_RESET</td><td>0x11</td></tr>
///         <tr><td>epssdmsdtFACTORY_DATA_RESET</td><td>0x12</td></tr>
///         <tr><td>epssdmsdtFAST_SLEEP_MODE</td><td>0x13</td></tr>
///         <tr><td>epssdmsdtNORMAL_RESET</td><td>0x14</td></tr>
///         <tr><td>epssdmsdtPROGUPDATE_RESET</td><td>0x15</td></tr>
///         </table>\n
///       \n
///       - EPWR_USER_MODE_TYPE
///         <table>
///         <tr bgcolor="black"><th>Name</th><th>Value</th></tr>
///         <tr><td>epsumINVALID</td><td>0xFF</td></tr>
///         <tr><td>epsumOFF</td><td>0x00</td></tr>
///         <tr><td>epsumON</td><td>0x01</td></tr>
///         </table>\n
///       \n
///       - EPWR_TRANSPORT_MODE_TYPE
///         <table>
///         <tr bgcolor="black"><th>Name</th><th>Value</th></tr>
///         <tr><td>epstmINVALID</td><td>0xFF</td></tr>
///         <tr><td>epstmDISABLED</td><td>0x00</td></tr>
///         <tr><td>epstmENABLED</td><td>0x01</td></tr>
///         </table>\n
///       \n
///       - EPWR_PROD_MODE_TYPE
///         <table>
///         <tr bgcolor="black"><th>Name</th><th>Value</th></tr>
///         <tr><td>epspmINVALID</td><td>0xFF</td></tr>
///         <tr><td>epspmDISABLED</td><td>0x00</td></tr>
///         <tr><td>epspmENABLED</td><td>0x01</td></tr>
///         </table>\n
///       \n
///       - EPWR_LHC_TYPE
///         <table>
///         <tr bgcolor="black"><th>Name</th><th>Value</th></tr>
///         <tr><td>epslhcINVALID</td><td>0xFF</td></tr>
///         <tr><td>epslhcDISABLED</td><td>0x00</td></tr>
///         <tr><td>epslhcENABLED</td><td>0x01</td></tr>
///         </table>\n
///       \n
/// \~english @retval eFrameworkunifiedStatusOK             Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle    Invalid handle
/// \~english @retval eFrameworkunifiedStatusFail           Failed
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Method x Method
/// \~english @par Detail
///       - The API sends shutdown request to the PowerService.\n
///       \n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PwrServiceSendShutdownRequest(
          HANDLE f_hSession
        , EPWR_SHUTDOWN_REQUEST_MSG_STRUCT &f_eState);  // NOLINT (runtime/references)

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup PwrServiceSendHeartBeatRequest
/// \~english @par Summary
///       Send a request for HeartBeat monitoring start.
/// \~english @param [in] f_hSession
///       HANDLE    - Session handle
/// \~english @param [in] f_eHbReqMsg
///       EPWR_HB_REQ_MSG_STRUCT*
///    - Address of the structure for HeartBeat monitoring start request message
/// \~english @par
///       - Structure for HeartBeat monitoring start request message
/// \~english @code
///       typedef struct {
///           UI_8  IntervalSec;
///       } EPWR_HB_REQ_MSG_STRUCT;
///            @endcode\n
///       \n
/// \~english @retval eFrameworkunifiedStatusOK             Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle    Invalid handle
/// \~english @retval eFrameworkunifiedStatusFail           Failed
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Method x Method
/// \~english @par Detail
///       - The API sends HeartBeat monitoring start\n
///       request to the PowerService\n
///       - Include    ss_power_service_ifc.h
///       - Library    libSS_PowerServiceIf.so
/// \~english @see
///       None
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus PwrServiceSendHeartBeatRequest(
                                HANDLE f_hSession,
                                EPWR_HB_REQ_MSG_STRUCT  *f_eHbReqMsg);

#endif  // POWER_SERVICE_CLIENT_INCLUDE_SYSTEM_SERVICE_SS_POWER_SERVICE_IF_H_  // NOLINT (build/header_guard)

/** @}*/  // end of PowerService
/** @}*/  // end of SystemService
/** @}*/  // end of BaseSystem
