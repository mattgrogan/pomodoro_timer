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

void Pomodoro::button_1() {
  _current_state->button_1(this);
}

int State_Off::name() {
  return STATE_OFF;
}

void State_Off::button_1(Pomodoro *p) {
  p->set_state(STATE_ACTIVE);
  
}

int State_Active::name() {
  return STATE_ACTIVE;
}

void State_Active::button_1(Pomodoro *p) {
  p->set_state(STATE_BREAK);
}

int State_Break::name() {
  return STATE_BREAK;
}

void State_Break::button_1(Pomodoro *p) {
  p->set_state(STATE_TEMP);
}

int State_Temp::name() {
  return STATE_TEMP;
}

void State_Temp::button_1(Pomodoro *p) {
  p->set_state(STATE_OFF);
}

