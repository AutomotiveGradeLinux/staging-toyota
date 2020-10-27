/*
 * @copyright Copyright (c) 2019-2020 TOYOTA MOTOR CORPORATION.
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
#include "pfdrec_thread_ifc.h"

CPFDRECThread::CPFDRECThread()
: m_thread(NULL) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
}

CPFDRECThread::~CPFDRECThread() {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
}

EFrameworkunifiedStatus CPFDRECThread::Initialize(HANDLE h_app) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}


EFrameworkunifiedStatus CPFDRECThread::Finalize(HANDLE h_app) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}


EFrameworkunifiedStatus CPFDRECThread::Start(HANDLE h_thread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CPFDRECThread::Stop(HANDLE h_thread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CPFDRECThread::SetFuncOnOff(HANDLE h_thread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CPFDRECThread::GetFuncOnOff(HANDLE h_thread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CPFDRECThread::DeleteData(HANDLE h_thread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CPFDRECThread::DeleteIndividual(HANDLE hThread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}


EFrameworkunifiedStatus CPFDRECThread::ComDataBackup(HANDLE h_thread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CPFDRECThread::ReadMemData(HANDLE h_thread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CPFDRECThread::GetDataInfo(HANDLE h_thread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CPFDRECThread::OutputData(HANDLE h_thread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CPFDRECThread::ReadData(HANDLE h_thread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CPFDRECThread::SearchData(HANDLE h_thread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CPFDRECThread::OverwriteData(HANDLE h_thread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus CPFDRECThread::SetExtractMode(HANDLE h_thread) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return eFrameworkunifiedStatusOK;
}
