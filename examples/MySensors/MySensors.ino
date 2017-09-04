/**
* RGBWDimmer - An Arduino library for managing RGBW led strips
* Copyright (C) 2017 Sergio Rius Rodr?guez
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

///// Mysensors options /////
//#define MY_DEBUG
#define MY_RADIO_NRF24

// Set LOW transmit power level as default, if you have an amplified NRF-module and
// power your radio separately with a good regulator you can turn up PA level.
#define MY_RF24_PA_LEVEL RF24_PA_LOW

// change the pins to free up the pwm pin for led control
#define MY_RF24_CE_PIN 4 //<-- NOTE!!! changed, the default is 9
#define MY_RF24_CS_PIN 10 // default is 10

#include <MySensors.h>

#define _DEBUG
#define USE_MYSENSORS
#define USE_GAMMA_CORRECTION

#include <RGBWDimmer.h>

RGBWDimmer myDimmer(1, 3, 5, 6, 9);

void setup() {
#ifdef _DEBUG
  Serial.begin(MY_BAUD_RATE);
#endif
}

void presentation() {
  myDimmer.presentation();
}

void receive(const MyMessage &message) {
  myDimmer.receive(message);
}

void loop() {
  myDimmer.loop();
}