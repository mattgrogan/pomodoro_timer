/******************************************** 
 *  pomodoro.cpp
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#include "Arduino.h"
#include "pomodoro.h"

Pomodoro::Pomodoro() {
  _current_state = &_state_off;
}

int Pomodoro::state() {
  return _current_state->name();
}

void Pomodoro::set_state(int state) {
  switch(state) {
    case STATE_OFF:
      _current_state = &_state_off;
      break;
    case STATE_ACTIVE:
      _current_state = &_state_active;
      break;
    case STATE_BREAK:
      _current_state = &_state_break;
      break;
    case STATE_TEMP:
      _current_state = &_state_temp;
      break;
  }
}

int State_Off::name() {
  return STATE_OFF;
}

int State_Active::name() {
  return STATE_ACTIVE;
}

int State_Break::name() {
  return STATE_BREAK;
}

int State_Temp::name() {
  return STATE_TEMP;
}

