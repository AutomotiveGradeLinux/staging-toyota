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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup <<Group Tag>> <<Group Name>>
/// \ingroup  tag_NSFramework
/// .
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
/// \brief
///
/// This file has the CFrameworkunifiedHSM class definitions. CFrameworkunifiedHSM is base class for HSM Framework.
/// This class implements interfaces for connecting child states to parent states, connecting events
/// to state.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_sm_hsm.h>
#include <native_service/frameworkunified_sm_reaction.h>
#include <native_service/frameworkunified_sm_state.h>
#include <native_service/frameworkunified_sm_compositestate.h>
#include <native_service/frameworkunified_sm_framework_types.h>
#include <native_service/frameworkunified_sm_orthogonalstate.h>
#include <native_service/frameworkunified_framework_if.h>

#include <cstdio>
#include <sstream>
#include <string>

#include "frameworkunified_framework_internal.h"
#include "frameworkunified_sm_framework_core.h"

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedHSM
/// Class constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSM::CFrameworkunifiedHSM(PVOID f_pHApp): m_uiCurrentEvent(0), m_pHApp(f_pHApp), m_pActiveState(NULL), m_pRootState(NULL),
  m_bIsTransitioning(FALSE) {
  // PostEventList stores the list of events posted in the state
  m_pPostEventList =  new EventInfoList();
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedHSM
/// Class constructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSM::CFrameworkunifiedHSM(): m_uiCurrentEvent(0), m_pHApp(NULL), m_pActiveState(NULL), m_pRootState(NULL),
  m_bIsTransitioning(FALSE) {
  // PostEventList stores the list of events posted in the state
  m_pPostEventList =  new EventInfoList();
  m_pHApp = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ~CFrameworkunifiedHSM
/// Class destructor
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedHSM::~CFrameworkunifiedHSM() {
  if (NULL != m_pPostEventList) {
    delete m_pPostEventList;
    m_pPostEventList = NULL;
  }

  if (m_pRootState) {
    FrameworkunifiedClose();
  }

  // we are not deleting this pointer because memory pointed by this pointer
  // had already been deleted in above step.
  m_pActiveState = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetActiveState
/// Returns the active state of the statemachine
///////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedState *CFrameworkunifiedHSM::FrameworkunifiedGetActiveState() {
  CFrameworkunifiedState *l_pCurrentState = m_pRootState;
  CFrameworkunifiedState *l_pActiveState = NULL;

  try {
    // Iterate till the current state is leafstate or orthogonal state
    while (l_pCurrentState != l_pActiveState) {
      l_pActiveState = l_pCurrentState;
      l_pCurrentState = l_pCurrentState->FrameworkunifiedGetActiveState();
      CHKNULL(l_pCurrentState);
    }

    // Set active state
    m_pActiveState = l_pActiveState;

    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Active state is  %s "
           , m_pActiveState->m_strStateName.c_str());
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_pActiveState = NULL;
  }


  return l_pActiveState;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedStart
/// This starts the state machine
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSM::FrameworkunifiedStart(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  try {
    CHKNULL(m_pActiveState);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h

    m_bIsTransitioning = TRUE;

    // Start the state machine execution on current state
    m_pActiveState = m_pActiveState->FrameworkunifiedOnHSMStart(f_pEventData);  // LCOV_EXCL_BR_LINE 11:except branch

    // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Active state is  %s "
           , m_pActiveState->m_strStateName.c_str());
    // LCOV_EXCL_BR_STOP
    // post the event from events list
    eStatus = ProcessEventQueue();  // LCOV_EXCL_BR_LINE 11:except branch

    m_bIsTransitioning = FALSE;
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedConnect
/// This connects the reaction to event and add event to child states then add child state
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSM::CFrameworkunifiedHSM::FrameworkunifiedConnect(CFrameworkunifiedState *f_pParentState, CFrameworkunifiedState *f_pChildState,
                                        UI_32 f_uiEventId, CFrameworkunifiedReaction *f_pReaction,
                                        BOOL f_bIsDefaultState, BOOL f_bIsDeferredEventType,
                                        std::string f_strEventName) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  try {
    // attach reaction to event and add it to the  child state
    eStatus = FrameworkunifiedConnect(f_pChildState, f_uiEventId, f_pReaction, f_strEventName, f_bIsDeferredEventType);
    if (eFrameworkunifiedStatusOK == eStatus) {
      // add child state to parent state
      eStatus = FrameworkunifiedConnect(f_pParentState, f_pChildState, f_bIsDefaultState);
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}
///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedConnect
/// This add child state to parent state.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSM::FrameworkunifiedConnect(CFrameworkunifiedState *f_pParentState, CFrameworkunifiedState *f_pChildState,
                               BOOL f_bIsDefaultState) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(f_pParentState);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h
    CHKNULL(f_pChildState);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h
    // Check if the state is composite state
    if (f_pParentState->FrameworkunifiedHasSubStates()) {
      // Check if the child state is default state
      if (f_bIsDefaultState) {
        // Add child state as default state
        (reinterpret_cast<CFrameworkunifiedCompositeState *>(f_pParentState))->FrameworkunifiedAddState(f_pChildState,
                                                            CFrameworkunifiedCompositeState::eFrameworkunifiedDefaultState);  // LCOV_EXCL_BR_LINE 11: except branch
      } else {
        // Add state as regular state
        (reinterpret_cast<CFrameworkunifiedCompositeState *>(f_pParentState))->FrameworkunifiedAddState(f_pChildState);  // LCOV_EXCL_BR_LINE 11: except branch
      }

      f_pChildState->FrameworkunifiedSetHSM(this);  // LCOV_EXCL_BR_LINE 11: except branch

    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error %s is not a composite state",
             f_pParentState->m_strStateName.c_str());
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;  //LCOV_EXCL_BR_LINE 11:except branch
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedConnect
/// This connects the reaction to event and add event to child states
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSM::FrameworkunifiedConnect(CFrameworkunifiedState *f_pState, UI_32 f_uiEventId, CFrameworkunifiedReaction *f_pReaction,
                               std::string f_strEventName , BOOL f_bIsDeferredEventType) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(f_pState);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h
    // check if the event is deferred event
    if (f_bIsDeferredEventType) {
      // Add event as deferred event
      f_pState->FrameworkunifiedAddDeferredEvent(f_uiEventId, f_strEventName);  // LCOV_EXCL_BR_LINE 11: except branch
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Deferred Event %d %s is "
        "associated with Reaction and added to state %s "
        , f_uiEventId, f_strEventName.c_str(), (f_pState->m_strStateName).c_str());
      // LCOV_EXCL_BR_STOP
    } else {
      CHKNULL(f_pReaction);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h
      // Add event as regular event
      f_pState->FrameworkunifiedAddEvent(f_uiEventId, f_pReaction, f_strEventName);  // LCOV_EXCL_BR_LINE 11: except branch

      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Event %d %s is associated with Reaction and added to state %s "
             , f_uiEventId, f_strEventName.c_str(), (f_pState->m_strStateName).c_str());
      // LCOV_EXCL_BR_STOP
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: Failed to add event %d in %s", f_uiEventId,
           (f_pState->m_strStateName).c_str());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }

  return eStatus;;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedPostEvent
/// This creates the default event data and sends the event to the active HSM state.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSM::FrameworkunifiedPostEvent(UI_32 f_uiEventId) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;  // LCOV_EXCL_BR_LINE 11: except branch
  try {
    CEventDataPtr l_pEventData(new CEventData(f_uiEventId));  // LCOV_EXCL_BR_LINE 11:except branch
    CHKNULL(l_pEventData);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h

    l_eStatus = FrameworkunifiedPostEvent(l_pEventData);  // LCOV_EXCL_BR_LINE 11: except branch
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedPostEvent
/// This sends the event to the active HSM state
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSM::FrameworkunifiedPostEvent(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(f_pEventData);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h

    if (m_bIsTransitioning) {
      eStatus = FrameworkunifiedQueueEvent(f_pEventData);  // LCOV_EXCL_BR_LINE 11: except branch
    } else {
      m_bIsTransitioning = TRUE;

      eStatus = ProcessEvent(f_pEventData);  // LCOV_EXCL_BR_LINE 11: except branch

      m_bIsTransitioning = FALSE;

      CHKNULL(m_pActiveState);
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Active state is  %s ", m_pActiveState->m_strStateName.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}

EFrameworkunifiedStatus CFrameworkunifiedHSM::ProcessEvent(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(m_pActiveState);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h
    CHKNULL(f_pEventData);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h
    CHKNULL(m_pActiveState->m_pEventName);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h

    EventNameList::iterator l_itEventName;
    l_itEventName = m_pActiveState->m_pEventName->find(f_pEventData->m_uiEventId);

    if (m_pActiveState->m_pEventName->end() != l_itEventName) {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Posting event %d %s to state  %s",
             f_pEventData->m_uiEventId,
             l_itEventName->second.c_str(),
             m_pActiveState->m_strStateName.c_str());
      // LCOV_EXCL_BR_STOP
    }

    m_uiCurrentEvent = f_pEventData->m_uiEventId;

    // Send event to active state for processing
    m_pActiveState = m_pActiveState->FrameworkunifiedOnEvent(f_pEventData);  // LCOV_EXCL_BR_LINE 11:except branch

    CHKNULL(m_pActiveState);
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, " Active state is  %s ", m_pActiveState->m_strStateName.c_str());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"

    // post the event from events list
    eStatus = ProcessEventQueue();  // LCOV_EXCL_BR_LINE 11:except branch
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"

    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedClose
/// This stops the state machine and destroys all states and events
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSM::FrameworkunifiedClose(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(m_pRootState);
    delete m_pRootState;
    m_pRootState = NULL;
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedConnect
/// This sets the givens state as root state in the state machine
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSM::FrameworkunifiedConnect(CFrameworkunifiedState *f_pRootState) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(f_pRootState);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h

    m_pActiveState = f_pRootState;
    m_pRootState = f_pRootState;

    m_pRootState->FrameworkunifiedSetHSM(this);  // LCOV_EXCL_BR_LINE 11:except branch
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;  // LCOV_EXCL_BR_LINE 11:except branch
}

///////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedPrintAllStates
/// This prints all states and events associated with every state on console.
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSM::FrameworkunifiedPrintAllStates() {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(m_pRootState);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h
    // Print the states
    m_pRootState->FrameworkunifiedPrintStates();
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());

    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;  // LCOV_EXCL_BR_LINE 11:except branch
}

EFrameworkunifiedStatus  CFrameworkunifiedHSM::FrameworkunifiedConnectOrthogonal(CFrameworkunifiedOrthogonalState *f_pOrthogonalState,
                                          CFrameworkunifiedCompositeState *f_pOrthogonalRegion) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(f_pOrthogonalState)  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h
    CHKNULL(f_pOrthogonalRegion)  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h

    f_pOrthogonalState->FrameworkunifiedAddOrthogonalRegion(f_pOrthogonalRegion);
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}

HANDLE CFrameworkunifiedHSM::FrameworkunifiedGetAppHandle() {
  return m_pHApp;
}

EFrameworkunifiedStatus CFrameworkunifiedHSM::FrameworkunifiedPrintXML() {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  try {
    std::ostringstream l_strXMLString;
    l_strXMLString << "<Statemachine>";

    CHKNULL(m_pRootState);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h

    // Write statemachine information in XML stream
    m_pRootState->FrameworkunifiedPrintXML(l_strXMLString);
    l_strXMLString << "</Statemachine>";

    // Write a stream to XML file
    if (m_pHApp) {
      size_t l_uiLength = static_cast<size_t>(l_strXMLString.str().length());

      PCHAR l_pStream = new CHAR[l_uiLength + 1];
      if (NULL != l_pStream) {
        std::FILE *l_pFile = NULL;

        std::memset(l_pStream, 0, l_uiLength + 1);
        std::strncpy(l_pStream, l_strXMLString.str().c_str(), l_uiLength);

        std::ostringstream l_strFileName;

        l_strFileName << "StatemachineXML_";

        l_strFileName << FrameworkunifiedGetAppName(m_pHApp) << ".xml";

        l_pFile = std::fopen(l_strFileName.str().c_str(), "wbe");

        if (l_pFile) {
          std::fwrite(l_pStream, l_uiLength, 1, l_pFile);
          std::fclose(l_pFile);
          FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, "%s File Created"
                 , l_strFileName.str().c_str());
        }
        delete[] l_pStream;
        l_pStream = NULL;
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error: StatemachineXML_%s.xml file not created", FrameworkunifiedGetAppName(m_pHApp));
      }
    }
    FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_USR_INFO, __FUNCTION__, l_strXMLString.str().c_str());
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
  }
  return l_eStatus;
}

EFrameworkunifiedStatus CFrameworkunifiedHSM::FrameworkunifiedQueueEvent(CEventDataPtr f_pEventData) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  try {
    CHKNULL(m_pPostEventList);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h
    CHKNULL(m_pActiveState);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h
    CHKNULL(f_pEventData);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h

    // Push the event in the post event list(FIFO)
    m_pPostEventList->push_back(f_pEventData);  // LCOV_EXCL_BR_LINE 11:except branch
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;  // LCOV_EXCL_BR_LINE 11:except branch
}

///////////////////////////////////////////////////////////////////////////////////////////
/// ProcessEventQueue
/// This post the event from events list
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSM::ProcessEventQueue() {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  try {
    CHKNULL(m_pPostEventList);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h

    if (!m_pPostEventList->empty()) {
      CEventDataPtr l_pEventData = m_pPostEventList->front();  // LCOV_EXCL_BR_LINE 11:except branch
      CHKNULL(l_pEventData);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h

      CHKNULL(m_pActiveState);  // LCOV_EXCL_BR_LINE 15: marco defined in frameworkunified_sm_framework_types.h
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG(ZONE_NS_SM_DEV_INFO, __FUNCTION__, "Processing posted event %d in state %s",
             l_pEventData->m_uiEventId, m_pActiveState->m_strStateName.c_str());
      // LCOV_EXCL_BR_STOP
      m_pPostEventList->erase(m_pPostEventList->begin());

      eStatus = ProcessEvent(l_pEventData);  // LCOV_EXCL_BR_LINE 11:except branch
    }
  } catch (std::exception &e) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Exception %s", e.what());
    eStatus = eFrameworkunifiedStatusNullPointer;
  }
  return eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// RemoveEventFromPostedEventQueue
/// This API is used to remove the all events of eventId f_uiEventId from event queue of statemachine
///////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedHSM::RemoveEventFromPostedEventQueue(const UI_32 f_uiEventId) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldID;
  FRAMEWORKUNIFIEDLOG_CUT(ZONE_NS_FUNC, __FUNCTION__, "+");

  if (NULL != m_pPostEventList) {
    int32_t l_siCnt = static_cast<int32_t>(m_pPostEventList->size() - 1);

    for (; l_siCnt >= 0; l_siCnt--) {
      if (NULL != m_pPostEventList->at(l_siCnt).get()) {
        if (f_uiEventId == m_pPostEventList->at(l_siCnt).get()->m_uiEventId) {
          m_pPostEventList->erase(m_pPostEventList->begin() + l_siCnt);
          l_eStatus = eFrameworkunifiedStatusOK;
        }
      }
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  FRAMEWORKUNIFIEDLOG_CUT(ZONE_NS_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
