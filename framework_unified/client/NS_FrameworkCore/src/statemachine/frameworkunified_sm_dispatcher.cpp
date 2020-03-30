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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
/// \brief
///
///
///
///////////////////////////////////////////////////////////////////////////////
#include <pthread.h>

#include <native_service/ns_logger_if.h>
#include <native_service/ns_plogger_if.h>
#include <native_service/ns_message_center_if.h>
#include <native_service/frameworkunified_sm_hsmframework.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_sm_dispatcher.h>

#include "frameworkunified_framework_core.h"
#include "frameworkunified_framework_error_internal.hpp"
#include "frameworkunified_sm_framework_core.h"
#include "frameworkunified_framework_utility.h"

static HSMConfigOptions s_tHSMConfigOptions = {eUserchangeIgnore, TRUE, FALSE};

EFrameworkunifiedStatus FrameworkunifiedHSMDispatcherMain(HANDLE hApp);
//////////////////////////////////////////
// Function : FrameworkunifiedCreateDispatcher
//////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCreateHSMDispatcher(PCSTR cAppName,
                                  HANDLE &hFrameworkApp,  // NOLINT  (readability/nolint)
                                  BOOL bIsThread, CFrameworkunifiedHSMFramework *f_pFrameworkunifiedHSM) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (NULL != cAppName) {
    if (eFrameworkunifiedStatusOK == (eStatus = CreateDispatcher(cAppName, hFrameworkApp, bIsThread))) {
      if (NULL != hFrameworkApp) {
        CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hFrameworkApp);

        if (NULL != f_pFrameworkunifiedHSM) {
          pApp->m_pFrameworkunifiedStateMachine = f_pFrameworkunifiedHSM;
          pApp->m_pFrameworkunifiedStateMachine->m_pHApp = pApp;
        } else {
          if (bIsThread) {
            pApp->m_pFrameworkunifiedStateMachine = new CFrameworkunifiedHSMChildFramework(pApp);
          } else {
            pApp->m_pFrameworkunifiedStateMachine = new CFrameworkunifiedHSMParentFramework(pApp);
          }
        }

        if (NULL != pApp->m_pFrameworkunifiedStateMachine) {
          pApp->m_pFrameworkunifiedStateMachine->FrameworkunifiedCreate(&s_tHSMConfigOptions);
        }
      } else {
        FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, "hFrameworkApp is NULL");
        eStatus = eFrameworkunifiedStatusNullPointer;
      }
    } else {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, "CreateDispatcher returned error, status=%d", eStatus);
      // LCOV_EXCL_BR_STOP
    }
  } else {
    FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, "AppName is NULL");
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDispatcherWithArguments
/// Creates, initializes and runs the dispatcher
///
/// \param [in] cAppname
///         PCSTR - Application/ thread name
///
/// \return Never does, main loop for your application.
///
/// \see FrameworkunifiedCreateDispatcher, FrameworkunifiedDispatchBlock,
///      FrameworkunifiedDispatchProcess, FrameworkunifiedCloseDispatcher,
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedHSMDispatcherWithArguments(PCSTR cAppName, int argc, char *argv[],
                                         const FrameworkunifiedDefaultCallbackHandler *CbHandler, CFrameworkunifiedHSMFramework *f_pFrameworkunifiedHSM,
                                         CustomCommandLineOptions *cmdLineOptions) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (cAppName == NULL) {
    return eFrameworkunifiedStatusNullPointer;
  }

  NsLogSetProcessName(cAppName);
  PLOG_TEXT("FrameworkunifiedHSMDispatcher Start");
  // set main thread name as provided in dispatcher
  pthread_setname_np(pthread_self(), cAppName);

  if ((eStatus = RegistDefaultCbHandler(CbHandler)) != eFrameworkunifiedStatusOK) {
    return eStatus;
  }

  FRAMEWORKUNIFIEDLOG0(ZONE_NS_DIS, __FUNCTION__, "In");  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
  try {
    HANDLE hFrameworkApp = NULL;

    if (eFrameworkunifiedStatusOK == (eStatus = FrameworkunifiedCreateHSMDispatcher(cAppName, hFrameworkApp, FALSE, f_pFrameworkunifiedHSM))) {
      if (NULL != hFrameworkApp) {
        THApp hApp(hFrameworkApp);

        /// Parse the Arguments via the FrameworkunifiedArgumentParser
        /// passing an handle to the app and argument list
        eStatus = FrameworkunifiedArgumentParser(hApp, argc, argv, cmdLineOptions);

        if (eFrameworkunifiedStatusOK == eStatus) {
          eStatus = FrameworkunifiedHSMDispatcherMain(hApp);
        }
      } else {
        eStatus = eFrameworkunifiedStatusNullPointer;
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "hFrameworkApp is NULL");
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedCreateHSMDispatcher error, status=%d", eStatus);
    }
  } catch (const THApp::Exception &) {
    /**
     * @todo
     * When an exception error occurs in a FrameworkunifiedCreateHSMDispatcher,
     * the caller cannot detect the exception because the exception becomes the eFrameworkunifiedStatusOK return code.
     */
    FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to FrameworkunifiedHSMCreateDispatcher ");
  }


  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function : FrameworkunifiedHSMDispatcherMain
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedHSMDispatcherMain(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  try {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_DIS, __FUNCTION__, "Success: FrameworkunifiedCreateDispatcher ");

    if (frameworkunifiedCheckValidAppHandle(hApp)) {
      CFrameworkunifiedFrameworkApp *pApp = static_cast<CFrameworkunifiedFrameworkApp *>(hApp);

      CFrameworkunifiedHSMFramework *l_pStateMachine = pApp->m_pFrameworkunifiedStateMachine;

      if (l_pStateMachine) {
        eStatus = l_pStateMachine->FrameworkunifiedStart();

        if (eFrameworkunifiedStatusOK != eStatus) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedOnInitialization failed ");
          l_pStateMachine->FrameworkunifiedClose();
          throw happ_error();
        } else {
          eStatus = RunDispatcher(hApp);
        }

        l_pStateMachine->FrameworkunifiedClose();
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "l_pStateMachine is NULL");
        eStatus = eFrameworkunifiedStatusNullPointer;
      }
    }
  } catch (const frameworkunified::framework::error::CSystemError &err) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "ERROR :: %s", err.what());

    FrameworkunifiedSendSystemErrMessage(hApp, err.m_eSystemError);
  } catch (const THApp::Exception &) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to FrameworkunifiedCreateDispatcher ");
  }

  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function : FrameworkunifiedSetHSMType
////////////////////////////////////////////////////////////////////////////////////////////
void FrameworkunifiedSetHSMType(EUserChangeOptions f_eHSMType) {
  s_tHSMConfigOptions.eUserChange = f_eHSMType;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function : FrameworkunifiedEnableAutoPublishServiceAvailable
////////////////////////////////////////////////////////////////////////////////////////////
void FrameworkunifiedEnableAutoPublishServiceAvailable() {
  s_tHSMConfigOptions.bAutoPublishServiceAvaialble = TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function : FrameworkunifiedDisableAutoPublishServiceAvailable
////////////////////////////////////////////////////////////////////////////////////////////
void FrameworkunifiedDisableAutoPublishServiceAvailable() {
  s_tHSMConfigOptions.bAutoPublishServiceAvaialble = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// Function : FrameworkunifiedIsAutoPublishServiceAvailableEnabled
////////////////////////////////////////////////////////////////////////////////////////////
BOOL FrameworkunifiedIsAutoPublishServiceAvailableEnabled() {
  return s_tHSMConfigOptions.bAutoPublishServiceAvaialble;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedHSMEnableWaitInStoppingState
////////////////////////////////////////////////////////////////////////////////////////////
VOID FrameworkunifiedHSMEnableWaitInStoppingState() {
  s_tHSMConfigOptions.bWaitInStoppingState = TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedHSMDisableWaitInStoppingState
////////////////////////////////////////////////////////////////////////////////////////////
VOID FrameworkunifiedHSMDisableWaitInStoppingState() {
  s_tHSMConfigOptions.bWaitInStoppingState = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedIsWaitInStoppingStateEnabled
////////////////////////////////////////////////////////////////////////////////////////////
BOOL FrameworkunifiedIsWaitInStoppingStateEnabled() {
  return s_tHSMConfigOptions.bWaitInStoppingState;
}
