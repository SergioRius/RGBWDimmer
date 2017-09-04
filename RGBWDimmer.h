/**
* RGBWDimmer - An Arduino library for managing RGBW led strips
* Copyright (C) 2017 Sergio Rius Rodr�guez
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

#ifndef _RGBWDIMMER_h
#define _RGBWDIMMER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

// include MySensors libraries
#define USE_MYSENSORS
#ifdef USE_MYSENSORS
  #include <core/MySensorsCore.h>
//#include <core/MyCapabilities.h>
//#include <core/MyTransport.h>
//#include <core/Version.h>

#ifndef SKETCH_NAME
#define SKETCH_NAME "RGBWDimmer"
#endif

#ifndef SKETCH_VERSION
#define SKETCH_VERSION "1.0"
#endif

#define WAIT(x) wait(x)
#else
#define WAIT(x) delay(x)
#endif

class RGBWDS
{
public:
  RGBWDS(int r, int g, int b, int w, int d, bool s);
  RGBWDS(int r, int g, int b, int w, int d);
  RGBWDS(int r, int g, int b, int w);
  RGBWDS();

  int  R    = 0;      ///< Red.
  int  G    = 0;      ///< Green.
  int  B    = 0;      ///< Blue.
  int  W    = 0;      ///< White.
  int  Dim  = 100;    ///< Dim level.
  bool Sw   = false;  ///< Switch

  char* stringRGBW;   ///< RGBW Levels in string.

#ifdef _DEBUG
  void print(const char* name) const;
#endif
};

#ifdef USE_GAMMA_CORRECTION
PROGMEM const byte _gamma[4][256] = {
  //{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 17, 17, 17, 18, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 26, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 54, 55, 56, 57, 58, 60, 61, 62, 64, 65, 67, 68, 70, 71, 73, 75, 76, 78, 80, 81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 104, 106, 108, 111, 113, 116, 118, 121, 123, 126, 129, 132, 135, 138, 141, 144, 147, 150, 154, 157, 161, 164, 168, 171, 175, 179, 183, 187, 191, 195, 200, 204, 209, 213, 218, 223, 228, 233, 238, 243, 249, 255 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 54, 55, 56, 57, 59, 60, 61, 63, 64, 65, 67, 68, 70, 72, 73, 75, 76, 78, 80, 82, 83, 85, 87, 89, 91, 93, 95, 97, 99, 102, 104, 106, 109, 111, 114, 116, 119, 121, 124, 127, 129, 132, 135, 138, 141, 144, 148, 151, 154, 158, 161, 165, 168, 172, 176, 180, 184, 188, 192, 196, 201, 205, 209, 214, 219, 224, 229, 234, 239, 244, 249, 255 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 54, 55, 56, 57, 59, 60, 61, 63, 64, 65, 67, 68, 70, 72, 73, 75, 76, 78, 80, 82, 83, 85, 87, 89, 91, 93, 95, 97, 99, 102, 104, 106, 109, 111, 114, 116, 119, 121, 124, 127, 129, 132, 135, 138, 141, 144, 148, 151, 154, 158, 161, 165, 168, 172, 176, 180, 184, 188, 192, 196, 201, 205, 209, 214, 219, 224, 229, 234, 239, 244, 249, 255 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 54, 55, 56, 57, 59, 60, 61, 63, 64, 65, 67, 68, 70, 72, 73, 75, 76, 78, 80, 82, 83, 85, 87, 89, 91, 93, 95, 97, 99, 102, 104, 106, 109, 111, 114, 116, 119, 121, 124, 127, 129, 132, 135, 138, 141, 144, 148, 151, 154, 158, 161, 165, 168, 172, 176, 180, 184, 188, 192, 196, 201, 205, 209, 214, 219, 224, 229, 234, 239, 244, 249, 255 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 54, 55, 56, 57, 59, 60, 61, 63, 64, 65, 67, 68, 70, 72, 73, 75, 76, 78, 80, 82, 83, 85, 87, 89, 91, 93, 95, 97, 99, 102, 104, 106, 109, 111, 114, 116, 119, 121, 124, 127, 129, 132, 135, 138, 141, 144, 148, 151, 154, 158, 161, 165, 168, 172, 176, 180, 184, 188, 192, 196, 201, 205, 209, 214, 219, 224, 229, 234, 239, 244, 249, 255 }
};
#endif

class RGBWDimmer
{
public:
  explicit RGBWDimmer(uint8_t child_id, uint8_t pinRed = 255, uint8_t pinGreen = 255, uint8_t pinBlue = 255, uint8_t pinWhite = 255);

  void setPin(uint8_t pinRed = 255, uint8_t pinGreen = 255, uint8_t pinBlue = 255, uint8_t pinWhite = 255);
#ifdef USE_MYSENSORS
    void setChildId(uint8_t value);
#endif

  RGBWDS currentValues; ///< Values currently used.
  RGBWDS newValues;     ///< Input values to initiate a fade.

  enum easing {
    EASE_LINEAR,
    EASE_GEOMETRIC,
    EASE_INSINE,
    EASE_OUTSINE,
    EASE_INOUTSINE,
    EASE_INSTANT
  };

  //void init(uint8_t pinRed, uint8_t pinGreen, uint8_t pinBlue, uint8_t pinWhite);
#ifdef USE_MYSENSORS
  virtual void presentation();
  virtual void receive(const MyMessage &message);
#endif
  void loop();

  // [101] set the effect to use for a smooth transition, can be one of SensorDimmer::EASE_LINEAR, SensorDimmer::EASE_INSINE, SensorDimmer::EASE_OUTSINE, SensorDimmer::EASE_INOUTSINE (default: EASE_LINEAR)
  void setEasing(uint8_t value) { _easing = value; }

  void setNewValues(int r, int g, int b, int w);
  // converts incoming color string to actual (int) values
  // ATTENTION this currently does nearly no checks, so the format needs to be exactly like domoticz sends the strings
  void setNewValuesHex(const char* value);

  void setDimming(int d) { newValues.Dim = d; }
  void setState(bool sw) { newValues.Sw = sw; }

  void StartFading();

protected:
  int _sleep_between_send = 50;
  uint8_t _pinRed         = 255;
  uint8_t _pinGreen       = 255;
  uint8_t _pinBlue        = 255;
  uint8_t _pinWhite       = 255;
#ifdef USE_MYSENSORS
  int _child_id = 1;
  int _presentation = S_RGBW_LIGHT;
  //int _type = V_CUSTOM;
  char* _description = "RGBWDimmer";
  bool _ack = true;

  MyMessage _msgLastValue;
  MyMessage _msgLastOn;
  MyMessage _msgLastDim;

  // Domoticz doesn't store these values when sent back.
  //MyMessage _msgStatus;
  //MyMessage _msgDimmer;
  //MyMessage _msgRGBW;
#endif

#define _WAIT() if (_sleep_between_send > 0) WAIT(_sleep_between_send)

  enum _enumStatus {
    STATUS_NOT_INITIALIZED = 0,
    STATUS_AWAITING_RESPONSE = 1,
    STATUS_RECEIVED_DIM = 2,
    STATUS_RECEIVED_SWITCH = 4,
    STATUS_RECEIVED_RGBW = 8,
    //STATUS_RECEIVED_ALL = 14,
    STATUS_AWAITING = 16,
    STATUS_FADING = 32,
    STATUS_IDLE = 64
  };

  bool statusIs(byte value) const;
  void setStatus(byte value);
#ifdef _DEBUG
  void printStatus() const;
#endif

private:

  byte _status;
  uint16_t _fadeDelayms = 1000; ///< Delay from first order since the fading starts.
                                ///< Note that if zero is set, it can miss composite messages.
  uint64_t _toStartOnTime;

  int _durationms = 1785;       ///< Duration in ms of the entire fading process.
  int _resolutionms = 7;        ///< Aprox. time that eache step takes or resolution of the change.

  /**
  * Difference between two set of values
  */
  int _delta_R = 0;
  int _delta_G = 0;
  int _delta_B = 0;
  int _delta_W = 0;
  int _delta_D = 0;

  int _num_steps = 0;
  int _current_step = 0;

  uint8_t _easing = EASE_LINEAR;

  RGBWDS _valuesToApply;

  int _getEasing(const int start, const int delta) const;
  void _doApply() const;
  void _scheduleFade();
  void _doFade();
  void _doEndFade();

#ifdef USE_GAMMA_CORRECTION
  const uint8_t _getGamma(const uint8_t pinNumber, const uint8_t value) const { return (uint8_t)pgm_read_byte(&_gamma[pinNumber][value]); }
  //const uint8_t _getGamma(const uint8_t pinNumber, const uint8_t value) const { (uint8_t)&_gamma[pinNumber][value]; }
#else
  const uint8_t _getGamma(const uint8_t pinNumber, const uint8_t value) const { return value; }
#endif

};

#endif
