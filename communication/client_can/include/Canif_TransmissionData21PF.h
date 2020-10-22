/*
 * @copyright Copyright (c) 2020 TOYOTA MOTOR CORPORATION.
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

// Following table could not be const because server will use this as R/W.
static CAN_INIT_TABLE Can_TransInitData_21PF[] __attribute__((unused)) = 
{
  /*
   *  Note.
   *  This feature needs to be defined by the vendor.
   */
  { 0x0000, 1, { 0x00 } },
  { 0x0001, 1, { 0x00 } },
  { 0x0002, 8, { 0x00 } },
  { 0x0100, 1, { 0x00 } },
  { 0x0101, 8, { 0x00 } },
  { 0x0102, 8, { 0x00 } }
};
