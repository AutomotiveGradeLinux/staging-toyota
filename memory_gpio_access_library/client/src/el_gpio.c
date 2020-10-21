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

/*
 * Information.
 * This source code is a sample source code .
 * Implementation of the function must be performed by the vendor.
 */

#include "stub/el_gpio.h"

int EL_Gpio_write(int symbol , bool value) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return 0;
}

int EL_Gpio_read(int symbol , bool *value) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return 0;
}

int EL_Gpio_attachInterruptControl(int symbol , unsigned long irq_flags) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return 0;
}

int EL_Gpio_detachInterruptControl(int symbol) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return 0;
}

int EL_Gpio_waitInterrupt(int symbol) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return 0;
}

int EL_Gpio_waitInterrupt_timeout(int symbol, int timeout) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return 0;
}

int EL_Gpio_finalize(void) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return 0;
}
