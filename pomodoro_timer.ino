/*******************
 *  Pomodoro Timer
 *  Matthew Grogan
 *******************/
#include <Adafruit_LEDBackpack.h>
#include <Bounce2.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "pitches.h"
#include "timer.h"
#include "pomodoro_state.h"

// Hardware interfaces
#define MATRIX_I2C_ADDR 0x70
#define ACTIVE_PIN 4
#define BREAK_PIN 3
#define SPEAKER_PIN 9
#define BMP280_I2C_ADDR 0x76

// Brightness controls 0 - 15 (15=brightest)
#define MAX_BRIGHTNESS 15
#define MIN_BRIGHTNESS 0

#define FADE_STEP_MS 100

State *current_state;
State_Off state_off;
State_Active state_active;
State_Break state_break;
State_Temp state_temp;

// Other options
#define DEBOUNCE_MS 5
#define MUSIC_SPEED 60

const int ACTIVE_SECS = 25 * 60;
const int BREAK_SECS = 5 * 60;
const int WARNING_SECS = 2 * 60;

Adafruit_7segment matrix = Adafruit_7segment();
Adafruit_BME280 bme;

const int TEMP_TIMEOUT_SECS = 1;
int temp_f;

Bounce active_pin_db = Bounce();
Bounce break_pin_db = Bounce();

class BrightnessCtrl {
  int current_brightness = 15;
  int dir = 1;

  public:
    int next() {
      // Return next brightness
      if (current_brightness == MAX_BRIGHTNESS || current_brightness == MIN_BRIGHTNESS) {
        dir *= -1;
      }

      current_brightness += dir;

      return current_brightness;
    }
};

BrightnessCtrl br;

class IntervalCtrl {
  unsigned long _last;
  int _interval_ms;

  public:
    IntervalCtrl(int interval_ms) {
      _interval_ms = interval_ms;
    }
    bool ready() {
      // Has the interval passed since the last measurement
      bool result = false;
      int time_since = millis() - _last;
    
      if (time_since > _interval_ms) {
        result = true;
        _last = millis();
      }

      return result;
    }
};

IntervalCtrl temp_interval(TEMP_TIMEOUT_SECS * 1000);
IntervalCtrl fader(FADE_STEP_MS);

Timer timer;

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
  
  Serial.begin(9600);

  matrix.begin(MATRIX_I2C_ADDR);

  // Setup the active button
  pinMode(ACTIVE_PIN, INPUT_PULLUP);
  active_pin_db.attach(ACTIVE_PIN);
  active_pin_db.interval(DEBOUNCE_MS);

  // Setup the break button
  pinMode(BREAK_PIN, INPUT_PULLUP);
  break_pin_db.attach(BREAK_PIN);
  break_pin_db.interval(DEBOUNCE_MS);

  // Set up speaker
  pinMode(SPEAKER_PIN, OUTPUT);
  analogWrite(SPEAKER_PIN, 0);

  // Connect to the BMP280
  bool bmp280_status;
  bmp280_status = bme.begin(BMP280_I2C_ADDR);
  if (!bmp280_status) {
    Serial.println("Could not find a valid BMP280 sensor");
  }

  // Initialize the display
  clear_display();
  current_state = &state_off;
  
}

void update_display(int remaining) {

  // Extract the minutes remaining
  int secs = remaining % 60;
  int mins = (remaining - secs) / 60;

  // Write each character to the display
  if ((mins / 10) > 0) {
     matrix.writeDigitNum(0, (mins / 10));
  } else {
    matrix.writeDigitRaw(0, 0);
  }

  if (remaining < WARNING_SECS && fader.ready() ) {
    matrix.setBrightness(br.next());
  } 
  
  matrix.writeDigitNum(1, (mins % 10));
  matrix.drawColon(true);
  matrix.writeDigitNum(3, (secs / 10));
  matrix.writeDigitNum(4, (secs % 10));
  
  matrix.writeDisplay();

}



void clear_display() {
  matrix.clear();
  matrix.writeDisplay();
}

void show_temp() {
  matrix.writeDigitNum(0, (temp_f / 10));
  matrix.writeDigitNum(1, (temp_f % 10));
  matrix.writeDigitRaw(3, 0x63);
  matrix.writeDigitRaw(4, 0x71);

  matrix.writeDisplay();
}

void loop() {

  // Update the bouncers
  active_pin_db.update();
  break_pin_db.update();

  // Read the button state
  int active_btn = active_pin_db.rose();
  int break_btn = break_pin_db.rose();

  int desired_time = 0;

  if (active_btn) {
    switch(current_state->name()) {
      case STATE_OFF:
        timer.set(ACTIVE_SECS);
        update_display(ACTIVE_SECS);
        current_state = &state_active;
        break;
      case STATE_ACTIVE:
        timer.set(BREAK_SECS);
        update_display(BREAK_SECS);
        current_state = &state_break;
        break;
      case STATE_BREAK:
        clear_display();
        current_state = &state_temp;
        timer.reset();
        break;  
      case STATE_TEMP:
        clear_display();
        current_state = &state_off;
        timer.reset();
        break;
    }
  }

  if (break_btn) {
   
    switch(timer.state()) {
      case COUNTDOWN_OFF:
        break;
      case COUNTDOWN_READY:
        timer.start();
        break;
      case COUNTDOWN_RUNNING:
        timer.pause();
        break;
      case COUNTDOWN_PAUSED:
        timer.start();
        break;
    }
  }

  if (timer.state() == COUNTDOWN_RUNNING) {
  
    update_display(timer.remaining());
    
    if (timer.expired()) {

      play_charge();

      if (current_state->name() == STATE_ACTIVE) {
        // Go to break
        timer.set(BREAK_SECS);
        update_display(BREAK_SECS);
        current_state = &state_break;
      }
      else if (current_state->name() == STATE_BREAK) {
        // Go to active
        timer.set(ACTIVE_SECS);
        update_display(ACTIVE_SECS);
        current_state = &state_active;
      }
    }

  }

  if (current_state->name() == STATE_TEMP) {
    show_temp();
  }

  if (temp_interval.ready()) {
    temp_f = round(bme.readTemperature() * 9/5 + 32);
  }

 delay(10);

}


