/*******************
 *  Pomodoro Timer
 *  Matthew Grogan
 *******************/
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include <Bounce2.h>

// Hardware interfaces
#define MATRIX_I2C_ADDR 0x70
#define PLAYPAUSE_PIN 2

// Logic states for application
#define STATE_NULL -1
#define STATE_STARTUP 0
#define STATE_READY 1
#define STATE_COUNTDOWN 2
#define STATE_PAUSED 3
int current_state = STATE_NULL;

// Other options
#define DEBOUNCE_MS 5

const int ACTIVE_SECS = 25 * 60;

Adafruit_7segment matrix = Adafruit_7segment();

unsigned long start_time;
unsigned long elapsed;
unsigned long remaining;

Bounce playpause_debouncer = Bounce();

void setup() {

  matrix.begin(MATRIX_I2C_ADDR);

  // Set up the playpause button
  pinMode(PLAYPAUSE_PIN, INPUT_PULLUP);
  playpause_debouncer.attach(PLAYPAUSE_PIN);
  playpause_debouncer.interval(DEBOUNCE_MS);

  Serial.begin(9600);
  

  // Initialize the display
  clear_display();
  current_state = STATE_STARTUP;
  
}

void start_countdown() {
  // Initialize the countdown
  
  start_time = millis();
  elapsed = 0;
  current_state = STATE_COUNTDOWN;
  
}

void clear_display() {
  matrix.clear();
  matrix.writeDisplay();
}

void write_display(int remaining, int draw_colon = true) {

    // Extract the minutes remaining
    int secs = remaining % 60;
    int mins = (remaining - secs) / 60;

    // Write each character to the display
    matrix.writeDigitNum(0, (mins / 10));
    matrix.writeDigitNum(1, (mins % 10));
    matrix.drawColon(draw_colon);
    matrix.writeDigitNum(3, (secs / 10));
    matrix.writeDigitNum(4, (secs % 10));
    matrix.writeDisplay();
}

void pause_countdown() {
  Serial.println("Paused");
  elapsed += (millis() - start_time) / 1000;
  current_state = STATE_PAUSED;
  
}

void resume_countdown() {
  start_time = millis();
  current_state = STATE_COUNTDOWN;
}


void loop() {

  // Update the bouncers
  playpause_debouncer.update();

  int button_state = playpause_debouncer.rose();

  if (button_state == true) {
      Serial.println(button_state);

  }

  switch(current_state) {
    case STATE_STARTUP:
      if (button_state == true) {
        write_display(ACTIVE_SECS);
        current_state = STATE_READY;
        Serial.println("Heard button");
      }
      break;
    case STATE_READY:
      if (button_state == true) {
        start_countdown();
      }
      break;
    case STATE_COUNTDOWN:
      if (button_state == true) {
        pause_countdown();
      }
      break;
    case STATE_PAUSED:
      if (button_state == true) {
        resume_countdown();
      }
      break;
  }

  if (current_state == STATE_COUNTDOWN) {
  
    // Calculate the remaining time
    //elapsed = (millis() - start_time) / 1000;
    remaining = ACTIVE_SECS - elapsed - ((millis() - start_time) / 1000);
  
    write_display(remaining);

    delay(50);
  }

}


