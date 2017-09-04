 /**
 * RGBWDimmer - An Arduino library for managing RGBW led strips
 * Copyright (C) 2017 Sergio Rius Rodríguez
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

#include "RGBWDimmer.h"

#ifdef _DEBUG
#include "utility/serial.h"
#endif

// converts hex char to uint8_t
int fromhex(const char* str) {
  char c = str[0] - '0';
  if (c > 9) c -= 7;
  int result = c;
  c = str[1] - '0';
  if (c > 9) c -= 7;
  return (result << 4) | c;
}

RGBWDS::RGBWDS(int r, int g, int b, int w, int d, bool s) {
  Sw = s;
  RGBWDS(r, g, b, w, d);
}

RGBWDS::RGBWDS(int r, int g, int b, int w, int d) {
  RGBWDS(r, g, b, w);
  Dim = d;
}

RGBWDS::RGBWDS(int r, int g, int b, int w) {
  R = r;
  G = g;
  B = b;
  W = w;
}

RGBWDS::RGBWDS() {}

#ifdef _DEBUG
void RGBWDS::print(const char* name) const {
  SERIAL_ECHO(name);
  SERIAL_ECHOPAIR(": R=", R);
  SERIAL_ECHOPAIR(" G=", G);
  SERIAL_ECHOPAIR(" B=", B);
  SERIAL_ECHOPAIR(" W=", W);
  SERIAL_ECHOPAIR(" DIM=", Dim);
  SERIAL_ECHOLNPAIR(" SW=", Sw);
}
#endif

/******************************************************
* RGBWDimmer
*/

RGBWDimmer::RGBWDimmer(uint8_t child_id, uint8_t pinRed, uint8_t pinGreen, uint8_t pinBlue, uint8_t pinWhite) {
  setPin(pinRed, pinGreen, pinBlue, pinWhite);
#ifdef USE_MYSENSORS
  setChildId(child_id);
  _msgLastOn = MyMessage(_child_id, V_VAR2);
  _msgLastDim = MyMessage(_child_id, V_VAR3);
  _msgLastValue = MyMessage(_child_id, V_VAR1);

  // Domoticz doesn't store these values when sent back.
  //_msgStatus = MyMessage(_child_id, V_STATUS);
  //_msgDimmer = MyMessage(_child_id, V_DIMMER);
  //_msgRGBW = MyMessage(_child_id, _pinWhite != 255 ? V_RGBW : V_RGB);
#endif
}

//void RGBWDimmer::init(uint8_t pinRed, uint8_t pinGreen, uint8_t pinBlue , uint8_t pinWhite) {
//}

void RGBWDimmer::setPin(uint8_t pinRed, uint8_t pinGreen, uint8_t pinBlue, uint8_t pinWhite) {
  _pinRed = pinRed;
  _pinGreen = pinGreen;
  _pinBlue = pinBlue;
  _pinWhite = pinWhite;
}

#ifdef USE_MYSENSORS
  void RGBWDimmer::setChildId(uint8_t value) {
  _child_id = value;
}
#endif

void RGBWDimmer::setNewValues(int r, int g, int b, int w) {
  newValues.R = r;
  newValues.G = g;
  newValues.B = b;
  newValues.W = w;
  newValues.stringRGBW = nullptr;
}

void RGBWDimmer::setNewValuesHex(const char* value) {
  if (strlen(value) == 6) {
    newValues.R = fromhex(&value[0]);
    newValues.G = fromhex(&value[2]);
    newValues.B = fromhex(&value[4]);
    newValues.W = 0;
    newValues.stringRGBW = strdup(value);
  }
  else if (strlen(value) == 9) {
    newValues.R = fromhex(&value[1]); // ignore # as first sign
    newValues.G = fromhex(&value[3]);
    newValues.B = fromhex(&value[5]);
    newValues.W = fromhex(&value[7]);
    newValues.stringRGBW = strdup(value);
  }
#ifdef _DEBUG
  else SERIAL_ERRORLNPGM("[RGBWDimmer] ERROR! Wrong length message");
#endif
}

bool RGBWDimmer::statusIs(byte value) const {
  if(_status == value) return true;
  if ((_status & value) != 0) return true;
  return false;
}

void RGBWDimmer::setStatus(byte value) {
  if (value == STATUS_RECEIVED_SWITCH || value == STATUS_RECEIVED_DIM || value == STATUS_RECEIVED_RGBW) {
    if (statusIs(STATUS_AWAITING_RESPONSE)) _status |= value;
  }
  else
    _status = value;
}

#ifdef _DEBUG
void RGBWDimmer::printStatus() const {
  SERIAL_ECHOPGM(" Status: "); SERIAL_ECHO(_status); SERIAL_ECHOPGM(" ");
  if (statusIs(STATUS_NOT_INITIALIZED)) SERIAL_ECHOPGM("[STATUS_NOT_INITIALIZED] ");
  if (statusIs(STATUS_AWAITING_RESPONSE)) SERIAL_ECHOPGM("[STATUS_AWAITING_RESPONSE] ");
  if (statusIs(STATUS_RECEIVED_DIM)) SERIAL_ECHOPGM("[STATUS_RECEIVED_DIM] ");
  if (statusIs(STATUS_RECEIVED_SWITCH)) SERIAL_ECHOPGM("[STATUS_RECEIVED_SWITCH] ");
  if (statusIs(STATUS_RECEIVED_RGBW)) SERIAL_ECHOPGM("[STATUS_RECEIVED_RGBW] ");
  //if (statusIs(STATUS_RECEIVED_ALL)) SERIAL_ECHOPGM("[STATUS_RECEIVED_ALL] ");
  if (statusIs(STATUS_AWAITING)) SERIAL_ECHOPGM("[STATUS_AWAITING] ");
  if (statusIs(STATUS_FADING)) SERIAL_ECHOPGM("[STATUS_FADING] ");
  if (statusIs(STATUS_IDLE)) SERIAL_ECHOPGM("[STATUS_IDLE] ");
  SERIAL_EOL();
}
#endif

void RGBWDimmer::_scheduleFade() {
#ifdef _DEBUG
  SERIAL_ECHOPGM("[RGBWDimmer] SCHEDULEFADE =====>>"); printStatus();
#endif
  if (!(statusIs(STATUS_IDLE) || (statusIs(STATUS_RECEIVED_DIM) && statusIs(STATUS_RECEIVED_SWITCH) && statusIs(STATUS_RECEIVED_RGBW)))) return;
  uint64_t now = millis();
  _toStartOnTime = now + _fadeDelayms;
  setStatus(STATUS_AWAITING);
#ifdef _DEBUG
  SERIAL_ECHOLNPGM("[RGBWDimmer] Schedule Successful");
#endif
}

#ifdef USE_MYSENSORS
// present the sensor to the gateway and controller
void RGBWDimmer::presentation() {
#ifdef _DEBUG
  SERIAL_ECHOPGM("[RGBWDimmer] PRESENTATION =====>>"); printStatus();
#endif
  // Send the sketch version information to the gateway and Controller
  if (_sleep_between_send > 0) wait(_sleep_between_send);
  sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);

  // Register this device as RGBW sensor
  if (_sleep_between_send > 0) wait(_sleep_between_send);
  present(_child_id, _presentation, _description, _ack);

#ifdef _DEBUG
  SERIAL_ECHOLNPGM("[RGBWDimmer] Requesting old values...");
#endif

  //Request last status if this is just a restart
  setStatus(STATUS_AWAITING_RESPONSE);
  if (_sleep_between_send > 0) wait(_sleep_between_send);
  request(_child_id, V_VAR2);  //Status
  if (_sleep_between_send > 0) wait(_sleep_between_send);
  request(_child_id, V_VAR3);  //Dimm level
  if (_sleep_between_send > 0) wait(_sleep_between_send);
  request(_child_id, V_VAR1);  //RGBW values

  // Domoticz doesn't store these values when sent back.
  //if (_sleep_between_send > 0) wait(_sleep_between_send);
  //request(_child_id, V_STATUS);  //Status
  //if (_sleep_between_send > 0) wait(_sleep_between_send);
  //request(_child_id, V_DIMMER);  //Dimm level
  //if (_sleep_between_send > 0) wait(_sleep_between_send);
  //request(_child_id, _pinWhite != 255 ? V_RGBW : V_RGB);  //RGBW values

//#ifdef _DEBUG
//  SERIAL_ECHOLNPAIR("[RGBWDimmer] Presentation-End Mode:", _runningMode);
//#endif

}

void RGBWDimmer::receive(const MyMessage& message) {
  if (message.sensor != _child_id) return;
  bool doSchedule = false;
#ifdef _DEBUG
  SERIAL_ECHOPGM("[RGBWDimmer] RECEIVE =====>>"); printStatus();
  SERIAL_ECHOPGM("[RGBWDimmer] Received ");
#endif
  if (statusIs(STATUS_FADING)) {
#ifdef _DEBUG
    SERIAL_ECHOLNPGM("IGNORED!");
#endif
    return;
  }
  switch (message.type) {

    // SWITCH STATUS *************************************************
  case V_STATUS:
#ifdef _DEBUG
    SERIAL_ECHOPGM("(V_STATUS) ");
#endif
  case V_VAR2: 
#ifdef _DEBUG
    SERIAL_ECHOPGM("(V_VAR2) ");
    SERIAL_ECHOLNPAIR("Turn: ", message.getBool());
#endif

    if (statusIs(STATUS_IDLE)) newValues = currentValues;
    setState(message.getBool());
    setStatus(STATUS_RECEIVED_SWITCH);
    doSchedule = true;
    break;

    // DIMMER VALUE **************************************************
  case V_DIMMER:
#ifdef _DEBUG
    SERIAL_ECHOPGM("(V_DIMMER) ");
#endif
  case V_VAR3: 
#ifdef _DEBUG
    SERIAL_ECHOPGM("(V_VAR3) ");
    SERIAL_ECHOLNPAIR("Dim: ", message.getString());
#endif

    if (statusIs(STATUS_IDLE)) {
      newValues = currentValues;
      setState(true);
    }
    setDimming(message.getInt());
    setStatus(STATUS_RECEIVED_DIM);
    doSchedule = true;
    break;

    // RGBW LEVELS ***************************************************
  case V_RGB:
#ifdef _DEBUG
    SERIAL_ECHOPGM("(V_RGB) ");
#endif
  case V_RGBW:
#ifdef _DEBUG
    SERIAL_ECHOPGM("(V_RGBW) ");
#endif
  case V_VAR1:
#ifdef _DEBUG
    SERIAL_ECHOPGM("(V_VAR1) ");
    if (strlen(message.getString()) == 6) SERIAL_ECHOPGM("RGB: ");
    else SERIAL_ECHOPGM("RGBW: ");
    SERIAL_ECHOLN(message.getString());
#endif

    if (statusIs(STATUS_IDLE)) {
      newValues = currentValues;
      setState(true);
    }
    setNewValuesHex(message.getString());
    setStatus(STATUS_RECEIVED_RGBW);
    doSchedule = true;
    break;
  default:
#ifdef _DEBUG
    SERIAL_ECHOLNPGM("OTHER");
#endif
    return;
    break;
  }
  if (doSchedule) _scheduleFade();
}

#endif

void RGBWDimmer::loop() {
  if (statusIs(STATUS_FADING))
    _doFade();
  else if (statusIs(STATUS_AWAITING) && _toStartOnTime > 0 && millis() >= _toStartOnTime)
    StartFading();
}

void RGBWDimmer::_doEndFade() {
#ifdef _DEBUG
  SERIAL_ECHOPGM("[RGBWDimmer] DOENDFADE =====>>"); printStatus();
  SERIAL_ECHOLNPGM("[RGBWDimmer] Ended fading");
  currentValues.print("[RGBWDimmer] currentValues: ");
  newValues.print("[RGBWDimmer] newValues: ");
  SERIAL_ECHOPGM("[RGBWDimmer]");
#endif
  _current_step = 0;
  setStatus(STATUS_IDLE);
  currentValues = _valuesToApply = newValues;
  _doApply();
  newValues = _valuesToApply = RGBWDS();

#ifdef _DEBUG
  currentValues.print("[RGBWDimmer] Commited: ");
#endif

#ifdef USE_MYSENSORS
  _WAIT();
  send(_msgLastOn.set(currentValues.Sw));
  _WAIT();
  send(_msgLastDim.set(currentValues.Dim));
  _WAIT();
  send(_msgLastValue.set(currentValues.stringRGBW));
#endif

#ifdef _DEBUG
  SERIAL_ECHOPGM("[RGBWDimmer] DOENDFADE <XXXXX"); printStatus();
#endif
}

void RGBWDimmer::_doApply() const {
#ifdef _DEBUG
  SERIAL_ECHOPGM(" Aplied");
#endif
  if (_pinRed != 255) {
    if (!_valuesToApply.Sw) analogWrite(_pinRed, 0);
    else {
      analogWrite(_pinRed, _getGamma(0, round(_valuesToApply.R / (float)100 * _valuesToApply.Dim)));
#ifdef _DEBUG
    SERIAL_ECHOPAIR_F(" R=", _getGamma(0, round(_valuesToApply.R / (float)100 * _valuesToApply.Dim)));
#endif
    }
  }
  if (_pinGreen != 255) {
    if (!_valuesToApply.Sw) analogWrite(_pinGreen, 0);
    else {
      analogWrite(_pinGreen, _getGamma(1, round(_valuesToApply.G / (float)100 * _valuesToApply.Dim)));
#ifdef _DEBUG
      SERIAL_ECHOPAIR_F(" G=", _getGamma(1, round(_valuesToApply.G / (float)100 * _valuesToApply.Dim)));
#endif
    }
  }
  if (_pinBlue != 255) {
    if (!_valuesToApply.Sw) analogWrite(_pinGreen, 0);
    else {
      analogWrite(_pinBlue, _getGamma(2 ,round(_valuesToApply.B / (float)100 * _valuesToApply.Dim)));
#ifdef _DEBUG
      SERIAL_ECHOPAIR_F(" B=", _getGamma(2, round(_valuesToApply.B / (float)100 * _valuesToApply.Dim)));
#endif
    }
  }
  if (_pinWhite != 255) {
    if (!_valuesToApply.Sw) analogWrite(_pinGreen, 0);
    else {
      analogWrite(_pinWhite, _getGamma(3, round(_valuesToApply.W / (float)100 * _valuesToApply.Dim)));
#ifdef _DEBUG
      SERIAL_ECHOPAIR_F(" W=", _getGamma(3, round(_valuesToApply.W / (float)100 * _valuesToApply.Dim)));
#endif
    }
  }
#ifdef _DEBUG
  SERIAL_ECHOLNPAIR(" DIM=", _valuesToApply.Dim);
  //_valuesToApply.print(" Aplied");
#endif
}

int RGBWDimmer::_getEasing(const int start, const int delta) const {
  return ((long)delta * (long)_current_step) / _num_steps + start;
}

void RGBWDimmer::_doFade() {
  if (!statusIs(STATUS_FADING)) return;
  if (_current_step > _num_steps) {
    _doEndFade();
    return;
  }

#ifdef _DEBUG
  SERIAL_ECHOPAIR("[RGBWDimmer] ", _current_step);
#endif

  //SERIAL_ECHOPGM(" B -> ");
  //SERIAL_ECHOPAIR_F("(", _delta_B);SERIAL_ECHOPAIR_F("*", _current_step);SERIAL_ECHOPAIR_F(")/", _num_steps);SERIAL_ECHOPAIR_F("+", currentValues.B);
  //SERIAL_ECHOLNPAIR_F(" = ", ((int)(_delta_B*_current_step) / _num_steps) + currentValues.B);

  // calculate the smooth transition and adjust it in the 0-255 range // / 100. * 255;
  if (_delta_R != 0 && _pinRed != 255) { _valuesToApply.R = _getEasing(currentValues.R, _delta_R); }
  if (_delta_G != 0 && _pinGreen != 255) { _valuesToApply.G = _getEasing(currentValues.G, _delta_G); }
  if (_delta_B != 0 && _pinBlue != 255) { _valuesToApply.B = _getEasing(currentValues.B, _delta_B); }
  if (_delta_W != 0 && _pinWhite != 255) { _valuesToApply.W = _getEasing(currentValues.W, _delta_W); }
  if (_delta_D != 0) { _valuesToApply.Dim = _getEasing(currentValues.Dim, _delta_D); }

  _doApply();

  // wait at the end of this step
  WAIT(_resolutionms);
  _current_step++;
}

void RGBWDimmer::StartFading() {
#ifdef _DEBUG
  SERIAL_ECHOPGM("[RGBWDimmer] STARTFADING =====>>"); printStatus();
#endif

  _toStartOnTime = 0;

  if((!newValues.Sw && !currentValues.Sw) || _easing == EASE_INSTANT) {
    _doEndFade();
    return;
  }

  setStatus(STATUS_FADING);

  // count how many steps we need to do
  _num_steps = _durationms / _resolutionms;

  _delta_R = newValues.R - currentValues.R;
  _delta_G = newValues.G - currentValues.G;
  _delta_B = newValues.B - currentValues.B;
  _delta_W = newValues.W - currentValues.W;

  ///< Logic for fading on and off.
  if (!newValues.Sw && currentValues.Sw) {
    if (!(newValues.R == 0 && newValues.G == 0 && newValues.B == 0 && newValues.W == 0)) {
      _delta_D = -currentValues.Dim;
    }
  }
  else if (newValues.Sw && !currentValues.Sw) {
    if (!(currentValues.R == 0 && currentValues.G == 0 && currentValues.B == 0 && currentValues.W == 0)) {
      _delta_D = newValues.Dim;
      currentValues.Dim = 0;
    }
    currentValues.Sw = true;
  }
  else _delta_D = newValues.Dim - currentValues.Dim;

  _valuesToApply = currentValues;

#ifdef _DEBUG
  SERIAL_ECHOLNPGM("[RGBWDimmer] Starting fading...");
  currentValues.print("[RGBWDimmer] currentValues: ");
  newValues.print("[RGBWDimmer] newValues: ");
  SERIAL_ECHOPAIR("[RGBWDimmer] DELTA R:", _delta_R);
  SERIAL_ECHOPAIR(" G:", _delta_G);
  SERIAL_ECHOPAIR(" B:", _delta_B);
  SERIAL_ECHOPAIR(" W:", _delta_W);
  SERIAL_ECHOLNPAIR(" D:", _delta_D);
  SERIAL_ECHOLNPAIR("[RGBWDimmer] _num_steps:", _num_steps);
#endif
}
