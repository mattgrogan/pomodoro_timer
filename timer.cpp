/******************************************** 
 *  timer.cpp
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#include "Arduino.h"
#include "Timer.h"

int Timer::set(int duration_secs) {
  reset();
  _duration_secs = duration_secs;
  _current_state = COUNTDOWN_READY;
  return 0;  
}

int Timer::start() {
  if (_current_state == COUNTDOWN_READY || _current_state == COUNTDOWN_PAUSED) {
    _start_time = millis();
    _current_state = COUNTDOWN_RUNNING;
    return 0;
  } else {
    return -1;
  }  
}

int Timer::pause() {
  if (_current_state == COUNTDOWN_RUNNING) {
    _elapsed += (millis() - _start_time) / 1000;
    _current_state = COUNTDOWN_PAUSED;
    return 0;
  } else {
    return -1;
  }
}
  
void Timer::reset() {
  _elapsed = 0;
  _duration_secs = 0;
  _current_state = COUNTDOWN_OFF;
}

int Timer::remaining() {
  if (_current_state == COUNTDOWN_RUNNING) {
    return _duration_secs - _elapsed - ((millis() - _start_time) / 1000);
  } else {
    return _duration_secs - _elapsed;
  }
}

bool Timer::expired() {
  return remaining() < 0;
}

int Timer::state() {
  return _current_state;
}
