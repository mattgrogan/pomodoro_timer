/******************************************** 
 *  animation.cpp
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#include "Arduino.h"
#include "animation.h"

// TODO: Need to fix animation so that it counts up the digits
// instead of counting down. Logic is hard to understand now...

/*************************************
 * PATTERN
 *************************************/
 
void Pattern::set_pattern(uint8_t *steps, uint8_t n_steps) {
  _steps = steps;
  _n_steps = n_steps;
}

void Pattern::first() {
  _index = 0;
  _mask = current_item();
}

void Pattern::next() {
  _index++;
  _mask |= current_item();
}

bool Pattern::is_done() {
  // Is not >= because we want to hit that very last step
  return _index > _n_steps;
}

uint8_t Pattern::mask() {
  return _mask;
}

uint8_t Pattern::mask(uint8_t bitmap) {
  return bitmap & mask() | current_item();
}

uint8_t Pattern::current_item() {
  if (_index >= _n_steps) {
    return 0x00;
  }
  return _steps[_index];
}

/*************************************
 * MATRIX PATTERN
 *************************************/

void MatrixPattern::set_pattern(uint8_t *steps, uint8_t n_steps) {
  for (int i=0; i<=SEGMENT_LENGTH; i++) {
    _digits[i].set_pattern(steps, n_steps);
    _digits[i].first();     
  }
  first();
}

void MatrixPattern::first() {
  _index = 3;
  _digits[_index].first();
  _end = false;
}

void MatrixPattern::next() {

  _digits[_index].next();
  
  if (_digits[_index].is_done()) {
    _index--;
    if (!is_done()) {
      _digits[_index].first();
    }
    
  } 
}

void MatrixPattern::end() {
  _end = true;
}

bool MatrixPattern::is_done() {
  return (_index < 0 || _end);
}

uint8_t MatrixPattern::mask(uint8_t digit, uint8_t bitmap) {
  if (digit >= _index) {
    return _digits[digit].mask(bitmap);
  } else {
    return 0x00;
  }
  
}

uint8_t MatrixPattern::digit() {
  return _index;
}


