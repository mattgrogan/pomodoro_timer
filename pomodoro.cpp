/******************************************** 
 *  pomodoro.cpp
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#include "Arduino.h"
#include "pomodoro.h"

/*************************************
 * Pomodoro object
 *************************************/

Pomodoro::Pomodoro() {
  _current_state = &_state_off;
}

void Pomodoro::disp_begin(int matrix_addr) {
  // Set up the matrix display
  Serial.println("Connecting to matrix");
  _m = Adafruit_7segment();
  _m.begin(matrix_addr);
}

void Pomodoro::reset_timer() {
  _seq_idx = 0;
  timer.reset();
  timer.set(_sequence[_seq_idx]);
}

void Pomodoro::set_timer() {
  // Iterate through the Pomodoro Technique

  _seq_idx++;

  if (_seq_idx >= POMODORO_STEPS) {
    _seq_idx = 0;
  }

  timer.set(_sequence[_seq_idx]);
}

void Pomodoro::set_temp(int temp_f) {
  _temp_f = temp_f;
}

void Pomodoro::set_state(int state) {
  Serial.print("State change: ");
  Serial.println(state);
  switch(state) {
    case STATE_OFF:
      _current_state = &_state_off;
      break;
    case STATE_READY:
      _current_state = &_state_ready;
      break;
    case STATE_ACTIVE:
      _current_state = &_state_active;
      break;
    case STATE_TEMP:
      _current_state = &_state_temp;
      break;
  }
}

void Pomodoro::button_1() {
  _current_state->button_1(this);
}

void Pomodoro::button_2() {
  _current_state->button_2(this);
}

void Pomodoro::disp_countdown() {
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
  _m.writeDigitNum(0, (_temp_f / 10));
  _m.writeDigitNum(1, (_temp_f % 10));
  _m.drawColon(false);
  _m.writeDigitRaw(3, 0x63);
  _m.writeDigitRaw(4, 0x71);

  _m.writeDisplay();
}

void Pomodoro::disp_clear() {
  _m.clear();
  _m.writeDisplay();
}

void Pomodoro::update() {
  _current_state->update(this);
  
}

void Pomodoro::set_leds(int leds[]) {
  for (int j = 0; j < NBR_LEDS; j++) {
    _led_pins[j] = leds[j];
    
  }
  
}

void Pomodoro::leds_on() {
  // Determine which leds should be lit
  int lit_leds = (_seq_idx - (_seq_idx % 2)) / 2;

  Serial.println(2 % 2);

  Serial.print("Number of lit leds: ");
  Serial.println(lit_leds);

  for (int i = 0; i < NBR_LEDS; i++) {
    Serial.print("Writing to led: ");
    Serial.println(i);
    digitalWrite(_led_pins[i], i <= lit_leds);
  }
  
}

void Pomodoro::leds_off() {
  for (int i = 0; i < NBR_LEDS; i++) {
    digitalWrite(_led_pins[i], LOW);
  }
  
}

/*************************************
 * STATE_OFF
 *************************************/

void State_Off::button_1(Pomodoro *p) {
  // User has chosen to turn on the machine
  p->reset_timer();
  p->leds_on();
  p->set_state(STATE_READY);
}

void State_Off::button_2(Pomodoro *p) {
  // This button undefined in the OFF state
}

void State_Off::update(Pomodoro *p) {
  // Do nothing
}

/*************************************
 * STATE_READY
 *************************************/

void State_Ready::button_1(Pomodoro *p) {
  // User has chosen to move to the next item
  p->leds_off();
  p->reset_timer();
  p->set_state(STATE_TEMP);
}

void State_Ready::button_2(Pomodoro *p) {
  // This button will start the countdown
  p->timer.start();
  p->set_state(STATE_ACTIVE);
}

void State_Ready::update(Pomodoro *p) {
  p->disp_countdown();
}

/*************************************
 * STATE_ACTIVE
 *************************************/

void State_Active::button_1(Pomodoro *p) {
    p->leds_off();
    p->reset_timer();
    p->set_state(STATE_TEMP);
}

void State_Active::button_2(Pomodoro *p) {
  // This button will iterate to next item
  // in the pomodoro sequence
  p->set_timer();
  p->leds_on();
  p->set_state(STATE_READY);
}

void State_Active::update(Pomodoro *p) {
  p->disp_countdown();

  if (p->timer.expired()) {
    // Time expired, so enter next item in sequence
    Serial.println("Timer expired.");
    p->set_timer();
    p->leds_on();
    p->set_state(STATE_READY);
  }
}

/*************************************
 * STATE_TEMP
 *************************************/

void State_Temp::button_1(Pomodoro *p) {
  p->disp_clear();
  p->set_state(STATE_OFF);
}

void State_Temp::button_2(Pomodoro *p) {
  // This button undefined in the TEMP state
}

void State_Temp::update(Pomodoro *p) {
  p->disp_temp();
}
