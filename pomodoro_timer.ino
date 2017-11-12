/*******************
 *  Pomodoro Timer
 *  Matthew Grogan
 *******************/
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include <Bounce2.h>
#include "pitches.h"

// Hardware interfaces
#define MATRIX_I2C_ADDR 0x70
#define ACTIVE_PIN 4
#define BREAK_PIN 3
#define RESET_PIN 2
#define SPEAKER_PIN 9


// Logic states for application
#define STATE_OFF -1
#define STATE_ACTIVE 0
#define STATE_BREAK 1
#define COUNTDOWN_OFF 2
#define COUNTDOWN_READY 3
#define COUNTDOWN_RUNNING 4
#define COUNTDOWN_PAUSED 5

int current_state = STATE_OFF;
int countdown_state = COUNTDOWN_OFF;

// Other options
#define DEBOUNCE_MS 5
#define MUSIC_SPEED 40 

const int ACTIVE_SECS = 25 * 60;
const int BREAK_SECS = 5 * 60;
const int WARNING_SECS = 2 * 60;

int timer_secs = 0;
unsigned long last_draw_time = 0;
int draw_timeout = 200;
int current_dot = 0;
int fade = true;

Adafruit_7segment matrix = Adafruit_7segment();

unsigned long start_time;
unsigned long elapsed;
unsigned long remaining;

Bounce active_pin_db = Bounce();
Bounce break_pin_db = Bounce();
Bounce reset_pin_db = Bounce();

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

void setup() {

  matrix.begin(MATRIX_I2C_ADDR);

  // Setup the active button
  pinMode(ACTIVE_PIN, INPUT_PULLUP);
  active_pin_db.attach(ACTIVE_PIN);
  active_pin_db.interval(DEBOUNCE_MS);

  // Setup the break button
  pinMode(BREAK_PIN, INPUT_PULLUP);
  break_pin_db.attach(BREAK_PIN);
  break_pin_db.interval(DEBOUNCE_MS);

  // Set up the reset button
  pinMode(RESET_PIN, INPUT_PULLUP);
  reset_pin_db.attach(RESET_PIN);
  reset_pin_db.interval(DEBOUNCE_MS);

  Serial.begin(9600);
  

  // Initialize the display
  clear_display();
  current_state = STATE_OFF;
  
}

bool should_draw(int remaining, int dot) {
  // Determine if it should draw for blinking

  if (remaining > WARNING_SECS) {
    return false;
  }

  int time_since = millis() - last_draw_time;

  if (time_since > draw_timeout) {
    current_dot = (current_dot + 1) % 4;
    last_draw_time = millis();
  }

  //return false;

  return current_dot == dot;
}

bool should_fade(int remaining) {
  // Determine if we should fade the display
  // as a warning that the countdown is nearly
  // completed.

  if (remaining > WARNING_SECS) {
    return false;
  }

  int time_since = millis() - last_draw_time;

  if (time_since > draw_timeout) {
    fade = !fade;
  }

  return false;

  return fade;
  
}

void update_display(int remaining) {

  //int draw_dots = should_draw(remaining);

  if (should_fade(remaining)) {
    matrix.setBrightness(11);
  } else {
    matrix.setBrightness(15);
  }

  // Extract the minutes remaining
  int secs = remaining % 60;
  int mins = (remaining - secs) / 60;

  // Write each character to the display
  if ((mins / 10) > 0) {
     matrix.writeDigitNum(0, (mins / 10));
  } else {
    if (should_draw(remaining, 0)) {
      matrix.writeDigitRaw(0, 0b10000000);
    } else {
      matrix.writeDigitRaw(0, 0);
    }
  }
  
  matrix.writeDigitNum(1, (mins % 10), should_draw(remaining, 1));
  matrix.drawColon(true);
  matrix.writeDigitNum(3, (secs / 10), should_draw(remaining, 2));
  matrix.writeDigitNum(4, (secs % 10), should_draw(remaining, 3));
  
  matrix.writeDisplay();

}



void clear_display() {
  matrix.clear();
  matrix.writeDisplay();
}

void loop() {

  // Update the bouncers
  active_pin_db.update();
  break_pin_db.update();
  reset_pin_db.update();

  // Read the button state
  int active_btn = active_pin_db.rose();
  int break_btn = break_pin_db.rose();
  int reset_btn = reset_pin_db.rose();

  int desired_time = 0;

  if (active_btn) {
    switch(current_state) {
      case STATE_OFF:
        timer_secs = ACTIVE_SECS;
        update_display(timer_secs);
        current_state = STATE_ACTIVE;
        countdown_state = COUNTDOWN_READY;
        break;
      case STATE_ACTIVE:
        timer_secs = BREAK_SECS;
        update_display(timer_secs);
        current_state = STATE_BREAK;
        countdown_state = COUNTDOWN_READY;
        break;
      case STATE_BREAK:
        clear_display();
        current_state = STATE_OFF;
        countdown_state = COUNTDOWN_OFF;
        break;
    }
  }

  if (break_btn) {
    Serial.println("Read break");
    
    switch(countdown_state) {
      case COUNTDOWN_OFF:
        Serial.println("Countdown is off");
        break;
      case COUNTDOWN_READY:
        start_time = millis();
        elapsed = 0;
        countdown_state = COUNTDOWN_RUNNING;
        break;
      case COUNTDOWN_RUNNING:
        // Pausing
        elapsed += (millis() - start_time) / 1000;
        countdown_state = COUNTDOWN_PAUSED;
        break;
      case COUNTDOWN_PAUSED:
        start_time = millis();
        countdown_state = COUNTDOWN_RUNNING;
        break;
    }
  }

  if (countdown_state == COUNTDOWN_RUNNING) {

  
    // Calculate the remaining time
    remaining = timer_secs - elapsed - ((millis() - start_time) / 1000);

    update_display(remaining);
    
    if (remaining <= 0) {

      play_charge();

      if (current_state == STATE_ACTIVE) {
        // Go to break

        timer_secs = BREAK_SECS;
        update_display(timer_secs);
        current_state = STATE_BREAK;
        countdown_state = COUNTDOWN_READY;
        
        
      }
      else if (current_state == STATE_BREAK) {
        // Go to active
        timer_secs = ACTIVE_SECS;
        update_display(timer_secs);
        current_state = STATE_ACTIVE;
        countdown_state = COUNTDOWN_READY;       
      }
    }

    delay(10);
  }
}


