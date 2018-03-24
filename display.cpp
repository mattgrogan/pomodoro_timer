/******************************************** 
 *  display.cpp
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2018
 ********************************************/

#include "display.h"

void Display::begin(uint8_t _addr) {
  _m = Adafruit_7segment();
  _m.begin(_addr);
}

void Display::all_on(int br = MAX_BRIGHTNESS) {
  // Light all LEDs at full brightness

  set_brightness(br);

  for (int j = 0; j <= 5; j++) {
    _m.writeDigitRaw(j, 0xFF);
  }

  _m.writeDisplay();
}

void Display::set_brightness(int br) {
  _m.setBrightness(br);
  _m.writeDisplay();  
}

void Display::write_display(uint8_t digit_0, uint8_t digit_1, uint8_t digit_2, uint8_t digit_3, bool colon = false) {

  if (!_mp.is_done()) {
    digit_0 = _mp.mask(0, digit_0);
    digit_1 = _mp.mask(1, digit_1);
    digit_2 = _mp.mask(2, digit_2);
    digit_3 = _mp.mask(3, digit_3);
  }
  
  colon = (_mp.digit() < 2 || _mp.is_done()) && colon;

  _m.writeDigitRaw(0, digit_0);
  _m.writeDigitRaw(1, digit_1);
  _m.drawColon(colon);
  _m.writeDigitRaw(3, digit_2);
  _m.writeDigitRaw(4, digit_3);

  _m.writeDisplay();
  
  if (_mp_interval.ready() && !_mp.is_done()) {
    _mp.next();
  }
  
}


void Display::end_animation() {
  _mp.end();
}

void Display::reset_animation() {
  _mp.first();
}

void Display::set_pattern(uint8_t steps, uint8_t n_steps) {
  _mp.set_pattern(steps, n_steps);
}


/****************************************
 * 
 */


void Display::writeDigitRaw(uint8_t n, uint16_t bitmask) {
  _m.writeDigitRaw(n, bitmask);
}

void Display::drawColon(boolean state) {
  _m.drawColon(state);
}

void Display::writeDisplay() {
  _m.writeDisplay();
}

void Display::clear() {
  _m.clear();
}

