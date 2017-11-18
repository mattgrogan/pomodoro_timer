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

void Pomodoro::disp_begin(int matrix_addr) {
  // Set up the matrix display
  Serial.println("Connecting to matrix");
  _m = Adafruit_7segment();
  _m.begin(matrix_addr);
}

int Pomodoro::state() {
  return _current_state->name();
}

void Pomodoro::set_temp(int temp_f) {
  _temp_f = temp_f;
}

void Pomodoro::set_state(int state) {
  Serial.println("State change");
  Serial.println(state);
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

void Pomodoro::disp_countdown() {
  Serial.println("Disp countdown");
  int remaining = timer.remaining();
  
  int secs = remaining % 60;
  int mins = (remaining - secs) / 60;

  // Write each character to the display
  if ((mins / 10) > 0) {
     _m.writeDigitNum(0, (mins / 10));
  } else {
    _m.writeDigitRaw(0, 0);
  }
 
  _m.writeDigitNum(1, (mins % 10));
  _m.drawColon(true);
  _m.writeDigitNum(3, (secs / 10));
  _m.writeDigitNum(4, (secs % 10));
  
  _m.writeDisplay();
}

void Pomodoro::disp_temp() {
  Serial.println("Showing temp");
  _m.writeDigitNum(0, (_temp_f / 10));
  _m.writeDigitNum(1, (_temp_f % 10));
  _m.drawColon(false);
  _m.writeDigitRaw(3, 0x63);
  _m.writeDigitRaw(4, 0x71);

  _m.writeDisplay();
}

void Pomodoro::disp_clear() {
  Serial.println("Clearing");
  _m.clear();
  _m.writeDisplay();
}

void Pomodoro::update() {
  _current_state->update(this);
  
}

int State_Off::name() {
  return STATE_OFF;
}

void State_Off::button_1(Pomodoro *p) {
  p->timer.set(ACTIVE_SECS);
  p->set_state(STATE_ACTIVE);
}

void State_Off::update(Pomodoro *p) {
  p->disp_clear();
}

int State_Active::name() {
  return STATE_ACTIVE;
}

void State_Active::button_1(Pomodoro *p) {
  p->timer.set(BREAK_SECS);
  p->set_state(STATE_BREAK);
}

void State_Active::update(Pomodoro *p) {
  p->disp_countdown();

//  if (p->timer.expired()) {
//    p->disp_clear();
//    p->timer.reset();
//  }
}

int State_Break::name() {
  return STATE_BREAK;
}

void State_Break::button_1(Pomodoro *p) {
  p->timer.reset();
  p->set_state(STATE_TEMP);
}

void State_Break::update(Pomodoro *p) {
  p->disp_countdown();

//  if (p->timer.expired()) {
//    p->disp_clear();
//    p->timer.reset();
//  }
}

int State_Temp::name() {
  return STATE_TEMP;
}

void State_Temp::button_1(Pomodoro *p) {
  p->timer.reset();
  p->set_state(STATE_OFF);
}

void State_Temp::update(Pomodoro *p) {
  p->disp_temp();
}
