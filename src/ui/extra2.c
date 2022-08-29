/*****************************************************************************
 *   Nano App Extra Functions
 *   (c) 2022 The Pen
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/


#include "extra2.h"


//////////////////////////////
//  PEN: EXTRA FUNCTIONS 2  //
//////////////////////////////


void chars_to_lowerCase( unsigned char * myChars, int charCount) {
    int i;
    char thisChar;

    for (i = 0; i < charCount; i++) {
        thisChar = myChars[i];
        if ((thisChar > 0x40) && (thisChar < 0x5b)) {
            myChars[i] = thisChar + 0x20;
        }
    }
}

void dotDecimal( unsigned char * rawDecimal, char * outChars) {

    int firstDigitPos = 0;
    int ai = 0;
    int wroteDot = 0;
    char thisDigit;
    for (ai = 0; ai < 16; ai++) {
        thisDigit = rawDecimal[ai];
        if (thisDigit != 0x30) {
            firstDigitPos = ai;
            break;
        }
    }
    if (firstDigitPos > 9) {  // THIS ONE WORKS
        outChars[0] = 0x30;
        outChars[1] = 0x2e;
        for (ai = 0; ai < 6; ai++) {
            outChars[ai + 2] = rawDecimal[ai + 10];
        }
        outChars[ai + 2] = 0x00;  // NULL TERMINATE
    }
    else {
        for (ai = 0; ai < 16; ai++) {
            if (ai < firstDigitPos) {
            }
            else if (ai == 9) {  // POSITION OF DECIMAL POINT AFTER DIGIT 10
                outChars[ai - firstDigitPos] = rawDecimal[ai];
                outChars[ai - firstDigitPos + 1] = 0x2e;
                wroteDot = 1;
            }
            else {
                outChars[ai - firstDigitPos + wroteDot] = rawDecimal[ai];
            }
        }   
        outChars[ai -firstDigitPos + wroteDot] = 0x00;  // NULL TERMINATE
    }

}
