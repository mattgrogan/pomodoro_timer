/******************************************** 
 *  pomodoro.cpp
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#include "Arduino.h"
#include "pomodoro.h"
#include "pitches.h"
#include "animation.h"

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
 * Variables for animation
 *************************************/

uint16_t decimal[10] = {
   SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
   SEG_B | SEG_C,
   SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,
   SEG_A | SEG_B | SEG_G | SEG_C | SEG_D,
   SEG_B | SEG_C | SEG_F | SEG_G,
   SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
   SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
   SEG_A | SEG_B | SEG_C,
   SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
   SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G
};

const uint8_t n_steps = 3;
uint8_t steps[n_steps] = {RIGHT, MID, LEFT};

/*************************************
 * Pomodoro object
 *************************************/

Pomodoro::Pomodoro() {
  _current_state = &_state_clock;
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

  // Set up the matrix pattern
  _mp.set_pattern(steps, n_steps);

  // Start at max brightness
  set_brightness(15);

  // Run test sequence
  test(750);

  // Set up the animation
  reset_animation();
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

void Pomodoro::proximity_status(bool prox_on) {

  // Toggle the proximity detector
  if (_prox_on != prox_on) {
    _prox_on = prox_on;
    _current_state->proximity_toggle(this, prox_on);
  }
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
  _m.drawColon(_colon_on);
  _m.writeDigitNum(3, (mins / 10));
  _m.writeDigitNum(4, (mins % 10));

 
  _m.writeDisplay();
}

void Pomodoro::disp_clock_anim() {
  DateTime now = rtc.now();
  
  int hour = now.hour();
  int mins = now.minute();

  if (hour > 12) {
    hour -= 12;
  }

  uint8_t digits[SEGMENT_LENGTH] = { 0 };

  digits[0] = decimal[(hour / 10)];
  digits[1] = decimal[(hour % 10)];
  digits[2] = decimal[(mins / 10)];
  digits[3] = decimal[(mins % 10)];

  if ((hour / 10) == 0) {
    digits[0] = 0x00; // Trim leading zero
  }
  
  if (!_mp.is_done()) {
    digits[0] = _mp.mask(0, digits[0]);
    digits[1] = _mp.mask(1, digits[1]);
    digits[2] = _mp.mask(2, digits[2]);
    digits[3] = _mp.mask(3, digits[3]);
  }


  bool draw_colon = (_mp.digit() < 2 || _mp.is_done()) && _colon_on;

  _m.writeDigitRaw(0, digits[0]);
  _m.writeDigitRaw(1, digits[1]);
  _m.drawColon(draw_colon);
  _m.writeDigitRaw(3, digits[2]);
  _m.writeDigitRaw(4, digits[3]);
 
  _m.writeDisplay();

  if (_mp_interval.ready() && !_mp.is_done()) {
    _mp.next();
  }
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
  if (br_level != _last_br_level && !_prox_on) {
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
  proximity_status(prox.near());

  check_brightness();

  // Colon blinks every second
  if (_colon_interval.ready()) {
    _colon_on = !_colon_on;
  }

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

void Pomodoro::reset_animation() {
  _mp.first();
}

void Pomodoro::end_animation() {
  _mp.end();
}

void Pomodoro::test(uint16_t duration_ms) {

  //Light Matrix
  for (int j = 0; j <= 5; j++) {
    _m.writeDigitRaw(j, 0xFF);
  }

  _m.writeDisplay();

  delay(duration_ms);

  // Light all LEDs
  for (int i = 0; i < NBR_LEDS; i++) {
    digitalWrite(_led_pins[i], HIGH);
    delay(200);
  }

  delay(duration_ms);
  
  leds_off();
  _m.clear();
  _m.writeDisplay();
}

/*************************************
 * STATE_OFF
 *************************************/

void State_Off::button_1(Pomodoro *p) {
  // User has chosen to turn on the machine
  p->set_state(STATE_CLOCK);
}

void State_Off::button_2(Pomodoro *p) {
  // This button undefined in the OFF state
}

void State_Off::proximity_toggle(Pomodoro *p, bool state) {
  Serial.println("Resetting animation");
  p->reset_animation();
}

void State_Off::update(Pomodoro *p) {
  // Show the clock if there's proximity detected
  if (p->prox.near()) {
    p->disp_clock_anim();
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

void State_Ready::proximity_toggle(Pomodoro *p, bool state) {
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

void State_Active::proximity_toggle(Pomodoro *p, bool state) {
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
  p->disp_clear();
  p->set_state(STATE_OFF);
}

void State_Temp::button_2(Pomodoro *p) {
  // This button undefined in the TEMP state
}

void State_Temp::proximity_toggle(Pomodoro *p, bool state) {
  p->reset_animation();
}

void State_Temp::update(Pomodoro *p) {
  // Show the clock if there's proximity detected
  if (p->prox.near()) {
    p->disp_clock_anim();
  } else {
    p->disp_temp();    
  }
}

/*************************************
 * STATE_CLOCK
 *************************************/

 void State_Clock::button_1(Pomodoro *p) {
  p->reset_timer();
  p->leds_on();
  p->set_state(STATE_READY);
}

void State_Clock::button_2(Pomodoro *p) {
  // This button undefined in the CLOCK state
}

void State_Clock::proximity_toggle(Pomodoro *p, bool state) {
  // Show animation only when proximity is detected
  if (state) {
    p->reset_animation();    
  } else {
    p->end_animation();
  }
}

void State_Clock::update(Pomodoro *p) {
  // Show the temp if there's a proximity detected, otherwise show clock
  if (p->prox.near()) {
    p->disp_temp();
  } else {
    p->disp_clock_anim();
  }

}

