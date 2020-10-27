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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_NPServiceIf
/// \brief
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_NPSERVICEIF_INCLUDE_NS_NP_TYPES_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_NPSERVICEIF_INCLUDE_NS_NP_TYPES_H_

// Path of immediate notification data in NS_NPS version 1
#define IMMEDIATE_PERSISTENCE_STORAGE_V1        "/nv/BS/ns/npp/rwdata/NPSNOR/"

// Path of immediate notification data in NS_NPS version 2
#define IMMEDIATE_PERSISTENCE_STORAGE_V2        "/nv/BS/ns/npp/rwdata/"

// name of persistence folder for persistent type categorization
#define USERDATADIR     "UserData/"   // stores all the user data (all user common data as well as user specific data)
#define FACTORYDATADIR               "FactoryData/"              // stores all the factory data
#define FACTORYCUSTOMERDATADIR       "FactoryCustomerData/"      // store all the factory customer data
#define DEALERDATADIR                "DealerData/"               // stores all the dealer data

#define IMMEDIATEDATADIR   "__ImmNotfn/"  // all immediate notification are stored under this folder name

// folder for storing userdata common for all user
#define ALLUSERAPPDATADIR            "AllUserAppData/"

#define RELEASETEMP_DIR              "/nps_tmp/"
#define RELEASETEMP_FILENAME         "/ReleaseWork.nps_tmp"

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_NPSERVICEIF_INCLUDE_NS_NP_TYPES_H_
