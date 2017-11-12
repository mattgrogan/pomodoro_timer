/*******************
 *  Pomodoro Timer
 *  Matthew Grogan
 *******************/
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include <Bounce2.h>

// Hardware interfaces
#define MATRIX_I2C_ADDR 0x70
#define ACTIVE_PIN 4
#define BREAK_PIN 3
#define RESET_PIN 2

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

const int ACTIVE_SECS = 5;
const int BREAK_SECS = 5 * 60;
int timer_secs = 0;

Adafruit_7segment matrix = Adafruit_7segment();

unsigned long start_time;
unsigned long elapsed;
unsigned long remaining;

Bounce active_pin_db = Bounce();
Bounce break_pin_db = Bounce();
Bounce reset_pin_db = Bounce();

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

 void update_display(int remaining, int draw_colon = true) {

    // Extract the minutes remaining
    int secs = remaining % 60;
    int mins = (remaining - secs) / 60;

    // Write each character to the display
    if ((mins / 10) > 0) {
       matrix.writeDigitNum(0, (mins / 10));
    } else {
      matrix.writeDigitRaw(0, 0);
    }
    
    matrix.writeDigitNum(1, (mins % 10));
    matrix.drawColon(draw_colon);
    matrix.writeDigitNum(3, (secs / 10));
    matrix.writeDigitNum(4, (secs % 10));
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


      if (current_state == STATE_ACTIVE) {
        // Go to break

              Serial.println("timeout");


        timer_secs = BREAK_SECS;
        update_display(timer_secs);
        current_state = STATE_BREAK;
        countdown_state = COUNTDOWN_READY;
        
        
      }
      else if (current_state == STATE_BREAK) {
        // Go to active
        timer_secs = BREAK_SECS;
        update_display(timer_secs);
        current_state = STATE_BREAK;
        countdown_state = COUNTDOWN_READY;       
      }
    }
  


    delay(50);
  }
}


