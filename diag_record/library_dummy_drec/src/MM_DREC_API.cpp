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

#include "MM_DREC_API.h"

RET_DREC_API DREC_ChangeRecording(E_DREC_REC state) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_AccOffOn(E_DREC_ACC state) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcPwr(T_DREC_TRCPWR* data, u_int32_t num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcPwr_Tick(T_DREC_TRCPWR_WITHSYSTIME* data, u_int32_t num, u_int32_t tick) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcMode(T_DREC_TRCMODE* data, u_int32_t num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcVehicleSignal(T_DREC_TRCVEHICLESIGNAL* data, u_int32_t num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcKey(T_DREC_TRCKEY* data, u_int32_t num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcKey_Exp(T_DREC_TRCKEY_EXP* data, u_int32_t num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcKey_Voice(T_DREC_TRCKEY_VOICE* data, u_int32_t num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcErr(T_DREC_TRCERR* data, u_int32_t num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcCan(T_DREC_TRCCAN* data, u_int32_t num, u_int32_t tick) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcOwnCarPosition(T_DREC_TRCAWDATA* data) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcDeliveryPFWatchLog(T_DREC_TRCAWDATA* data) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcDeliveryPFUseLog(T_DREC_TRCAWDATA* data) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcBT(T_DREC_TRCBT* data, u_int32_t num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcBTInternal(u_int8_t* data, u_int32_t size) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcWiFiInternal(u_int8_t* data, u_int32_t size) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcWiFi(T_DREC_TRCWIFI* data, u_int32_t num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcCenter(u_int8_t* data, u_int32_t size) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcVersion(T_DREC_TRCVERSION* data, u_int32_t num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcDCM(u_int8_t* data, u_int32_t size) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcDSRC(T_DREC_TRCAWDATA* data) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrc_CWORD17_(T_DREC_TRCCAN* data, u_int32_t num, u_int32_t tick) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcWave(T_DREC_TRCWAVE* data, u_int32_t num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcUSB(u_int8_t* data, u_int32_t size) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_GetRecordSize(E_PFDREC_CATEGORY_TYPE type, u_int32_t *num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_GetRecordData(E_PFDREC_CATEGORY_TYPE type, u_int32_t num,
    u_int32_t record_ptr, u_int8_t *data, u_int32_t *out_num) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_GetRecordState(E_DREC_REC* state) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_SetSysemTimeDifference(u_int32_t data) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcGPS(T_DREC_TRCGPS* data) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}


RET_DREC_API DREC_WriteTrcUSBETC(T_DREC_TRCAWDATA* data)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcUplink(T_DREC_TRCAWDATA* data)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
	return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcETCWatch(T_DREC_TRCETCWATCH* data, u_int32_t num)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcRoB(T_DREC_TRCROB* data, u_int32_t num)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcEvaluation(u_int8_t* data, u_int32_t size)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_DeleteTrcDeliveryPFUseLog(void)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
	return RET_DREC_NORMAL;
}

RET_DREC_API DREC_DeleteTrcDeliveryPFUseLog_inSession(HANDLE hThread)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
	return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcOptBeaconDownLink(T_DREC_TRCOPTBEACON_DOWN_LINK* data, u_int32_t num)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_WriteTrcOptBeaconUpLink(T_DREC_TRCOPTBEACON_UP_LINK* data, u_int32_t num)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_MakerTrace_DeliveryEntry(HANDLE hApp,
                                                  PCSTR notifyName)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_MakerTrace_SizeRequest(HANDLE hApp,
                                                PCSTR notifyName)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_MakerTrace_SizeResponce(HANDLE hApp,
                                                 PCSTR notifyName,
                                                 u_int32_t size)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_MakerTrace_OutputRequest(HANDLE hApp,
                                                  PCSTR notifyName,
                                                  PCSTR output,
                                                  DREC_OUTPUT_STATE state)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_MakerTrace_OutputResponce(HANDLE hApp,
                                                   PCSTR notifyName,
                                                   u_int8_t progress,
                                                   DREC_OUTPUT_RESULT state)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_MakerTrace_DeleteRequest(HANDLE hApp,
                                                  PCSTR notifyName)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_MakerTrace_DeleteResponce(HANDLE hApp,
                                                   PCSTR notifyName,
                                                   DREC_OUTPUT_RESULT state)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_MakerTrace_FuncStatusRequest(HANDLE hApp,
                                                      PCSTR notifyName,
                                                      DREC_OUTPUT_FUNC func)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_MakerTrace_FuncStatusResponce(HANDLE hApp,
                                                       PCSTR notifyName,
                                                       DREC_OUTPUT_RESULT state)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

RET_DREC_API DREC_MakerTrace_NotifyRestrict(HANDLE hApp,
                                                   PCSTR notifyName,
                                                   DREC_NOTIFY_RESTRICT notifyRestrict)
{
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return RET_DREC_NORMAL;
}

