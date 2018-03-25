/******************************************** 
 *  display.cpp
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2018
 ********************************************/

#include "display.h"

/***************************************************
 * SEVEN SEGMENT DISPLAY
 ***************************************************/
 
void SevenSegDisplay::begin(uint8_t _addr) {
  _m = Adafruit_7segment();
  _m.begin(_addr);
}

void SevenSegDisplay::set_brightness(int br) {
  _m.setBrightness(br);
  _m.writeDisplay();  
}

void SevenSegDisplay::write(DisplayData d) {
  
  // Write raw data to display
  _m.writeDigitRaw(0, d.digit_0);
  _m.writeDigitRaw(1, d.digit_1);
  _m.drawColon(d.colon);
  _m.writeDigitRaw(3, d.digit_2);
  _m.writeDigitRaw(4, d.digit_3);

  _m.writeDisplay();
}

void SevenSegDisplay::clear() {
  _m.clear();
  _m.writeDisplay();
}

/***************************************************
 * ANIMATED DISPLAY
 ***************************************************/

void AnimatedDisplay::reset() {
  _current_animation->reset();
}

void AnimatedDisplay::end() {
  _current_animation->end();
}

void AnimatedDisplay::set_animation(uint8_t a) {
  switch(a) {
    case NULL_ANIMATION:
      _current_animation = &_null_anim;
      break;
    case COLON_BLINK_ANIM:
      _current_animation = &_colon_blink;
      break;
    case HOUR_BLINK_ANIM:
      _current_animation = &_hour_blink;
      break;
    case MIN_BLINK_ANIM:
      _current_animation = &_min_blink;
      break;
    case SWIPE_IN_ANIM:
      // Hack due to c++ overload calls parent type, so reset is not being called
      // on the swipe in animation.
      _swipe_in.reset();
      _current_animation = &_swipe_in;
      break;
  }

  // Reset the animation
  //_current_animation->reset();
}

void AnimatedDisplay::write(DisplayData d) {
  DisplayData masked = _current_animation->mask(d);
  SevenSegDisplay::write(masked);
}

