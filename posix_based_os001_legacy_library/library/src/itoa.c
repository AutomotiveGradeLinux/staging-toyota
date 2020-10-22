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

char* itoa( int value, char buff[], int radix ) {
	static const char table[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char *head = buff;
	char *tail = buff;
	char temp;

	// Converting minus sign to character
	if ( value < 0 ){
		*tail++ = '-';
		value = -value;
	}
	// Converting integer to character
	if ( value == 0 ){
		*tail++ = '0';
	}
	else for ( head = tail ; value != 0 ; value /= radix ){
		*tail++ = table[ value % radix ];
	}
	*tail = '\0';

	// swapping characters
	for ( tail-- ; head < tail ; head++, tail-- ){
		temp = *head;
		*head = *tail;
		*tail = temp;
	}
	
	return buff;
}
