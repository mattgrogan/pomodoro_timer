/******************************************** 
 *  pomodoro.cpp
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#include "Arduino.h"
#include "pomodoro.h"
#include "pitches.h"

#define SPEAKER_PIN 2
#define MUSIC_SPEED 60

/*************************************
 * Play functions that really should
 * be factored into an object...
 *************************************/
 
void play(int pin, int *notes, int *durations, int speed) {
  // Play notes and durations at speed through pin

  for (int i = 0; notes[i] != -1; i++) {
    int note_duration = durations[i] * speed;
    tone(pin, notes[i], note_duration * 0.95);
    delay(note_duration);
  }
}

void play_charge() {
  int charge[] = {
    NOTE_G4, NOTE_C5, NOTE_E5, NOTE_G5, NOTE_E5, NOTE_G5, END
  };
  int charged[] {
    4, 4, 4, 6, 2, 16
  };
  
  int *notes = charge;
  int *durations = charged;
  
  play(SPEAKER_PIN, notes, durations, MUSIC_SPEED);
}

/*************************************
 * Pomodoro object
 *************************************/

Pomodoro::Pomodoro() {
  _current_state = &_state_off;
}

void Pomodoro::disp_begin(int matrix_addr) {

  // Connect to RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }
  
  // Set up the matrix display
  Serial.println("Connecting to matrix");
  _m = Adafruit_7segment();
  _m.begin(matrix_addr);

  // Set up proximity and light sensor
  prox.begin();
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
    case STATE_CLOCK:
      _current_state = &_state_clock;
      break;
  }
}

void Pomodoro::button_1() {
  _current_state->button_1(this);
}

void Pomodoro::button_2() {
  _current_state->button_2(this);
}

void Pomodoro::proximity_detected() {
  _current_state->proximity_detected(this);
}

void Pomodoro::disp_countdown() {
  int remaining = max(timer.remaining(), 0);
  
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

void Pomodoro::disp_clock() {
  DateTime now = rtc.now();
  
  int hour = now.hour();
  int mins = now.minute();

  if (hour > 12) {
    hour -= 12;
  }

  // Write each character to the display
  if ((hour / 10) > 0) {
     _m.writeDigitNum(0, (hour / 10));
  } else {
    _m.writeDigitRaw(0, 0);
  }
 
  _m.writeDigitNum(1, (hour % 10));
  _m.drawColon(true);
  _m.writeDigitNum(3, (mins / 10));
  _m.writeDigitNum(4, (mins % 10));

 
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

void Pomodoro::check_brightness() {
  
  // Determine the proper brightness level
  uint16_t amb = prox.ambient();
  int br_level = _last_br_level;
  
  if (amb                <   20) { br_level =  0; }
  if (amb >=   20 && amb <   29) { br_level =  1; }
  if (amb >=   30 && amb <   30) { br_level =  2; }
  if (amb >=   30 && amb <   49) { br_level =  3; }
  if (amb >=   50 && amb <   69) { br_level =  4; }
  if (amb >=   70 && amb <   99) { br_level =  5; }
  if (amb >=  100 && amb <  199) { br_level =  6; }
  if (amb >=  200 && amb <  299) { br_level =  7; }
  if (amb >=  300 && amb <  399) { br_level =  8; }
  if (amb >=  400 && amb <  499) { br_level =  9; }
  if (amb >=  500 && amb <  599) { br_level = 10; }
  if (amb >=  600 && amb <  699) { br_level = 11; }
  if (amb >=  700 && amb <  999) { br_level = 12; }
  if (amb >= 1000 && amb < 1499) { br_level = 13; }
  if (amb >= 1500 && amb < 2499) { br_level = 14; }
  if (amb >= 2500              ) { br_level = 15; }

  // If the level has changed and there's nothing in front of the sensor...
  if (br_level != _last_br_level && !prox.near()) {
    set_brightness(br_level);
    _last_br_level = br_level;
  }  
}

void Pomodoro::set_brightness(int br) {
  _m.setBrightness(br);
  _m.writeDisplay();
}

void Pomodoro::update() {

  // Check proximity sensor
   if (prox.near()) {
    proximity_detected();
  }

  check_brightness();

  // Run the update
  _current_state->update(this);
  
}

void Pomodoro::set_leds(int leds[]) {
  for (int j = 0; j < NBR_LEDS; j++) {
    _led_pins[j] = leds[j];
    
  }
  
}

void Pomodoro::leds_on() {
  // Determine which leds should be lit
  int lit_leds = ((_seq_idx + 1) - ((_seq_idx + 1) % 2)) / 2;

  for (int i = 0; i < NBR_LEDS; i++) {
    digitalWrite(_led_pins[i], i < lit_leds);
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

void State_Off::proximity_detected(Pomodoro *p) {
  // do nothing
}

void State_Off::update(Pomodoro *p) {
  // Show the clock if there's proximity detected
  if (p->prox.near()) {
    p->disp_clock();
  } else {
    p->disp_clear();    
  }
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

void State_Ready::proximity_detected(Pomodoro *p) {
  // Do nothing
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

void State_Active::proximity_detected(Pomodoro *p) {
  // Do nothing
}

void State_Active::update(Pomodoro *p) {
  p->disp_countdown();

  if (p->timer.expired()) {
    // Time expired, so enter next item in sequence
    Serial.println("Timer expired.");
    p->set_timer();
    p->leds_on();
    play_charge();
    p->set_state(STATE_READY);
  }
}

/*************************************
 * STATE_TEMP
 *************************************/

void State_Temp::button_1(Pomodoro *p) {
  //p->disp_clear();
  p->set_state(STATE_CLOCK);
}

void State_Temp::button_2(Pomodoro *p) {
  // This button undefined in the TEMP state
}

void State_Temp::proximity_detected(Pomodoro *p) {
  // Do nothing
}

void State_Temp::update(Pomodoro *p) {
  // Show the clock if there's proximity detected
  if (p->prox.near()) {
    p->disp_clock();
  } else {
    p->disp_temp();    
  }
}

/*************************************
 * STATE_CLOCK
 *************************************/

 void State_Clock::button_1(Pomodoro *p) {
  p->disp_clear();
  p->set_state(STATE_OFF);
}

void State_Clock::button_2(Pomodoro *p) {
  // This button undefined in the CLOCK state
}

void State_Clock::proximity_detected(Pomodoro *p) {
  // Do nothing
}

void State_Clock::update(Pomodoro *p) {
  // Show the temp if there's a proximity detected, otherwise show clock
  if (p->prox.near()) {
    p->disp_temp();
  } else {
    p->disp_clock();    
  }

}

