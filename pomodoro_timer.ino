/*******************
 *  Pomodoro Timer
 *  Matthew Grogan
 *******************/
#include <Bounce2.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "timer.h"
#include "pomodoro.h"
#include "vcnl4010.h"
#include "interval.h"

// Hardware interfaces
#define MATRIX_I2C_ADDR 0x70
#define BTN_1_PIN 3
#define BTN_2_PIN 4

#define LED_1_PIN 5
#define LED_2_PIN 6
#define LED_3_PIN 7
#define LED_4_PIN 8

#define SPEAKER_PIN 2
#define BMP280_I2C_ADDR 0x76

// Brightness controls 0 - 15 (15=brightest)
#define MAX_BRIGHTNESS 15
#define MIN_BRIGHTNESS 0

Pomodoro pomodoro;
VCNL4010 prox;

// This is for debugging the brightness level
int last_br_level = 15;

// Other options
#define DEBOUNCE_MS 5

const int WARNING_SECS = 2 * 60;

Adafruit_BME280 bme;

const int TEMP_TIMEOUT_SECS = 1;

Bounce btn_1_db = Bounce();
Bounce btn_2_db = Bounce();

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

Interval temp_interval(TEMP_TIMEOUT_SECS * 1000);

void setup() {
  
  Serial.begin(9600);

  //matrix.begin(MATRIX_I2C_ADDR);
  pomodoro.disp_begin(MATRIX_I2C_ADDR);

  // Setup button 1
  pinMode(BTN_1_PIN, INPUT_PULLUP);
  btn_1_db.attach(BTN_1_PIN);
  btn_1_db.interval(DEBOUNCE_MS);

  // Setup button 2
  pinMode(BTN_2_PIN, INPUT_PULLUP);
  btn_2_db.attach(BTN_2_PIN);
  btn_2_db.interval(DEBOUNCE_MS);

  // Set up speaker
  pinMode(SPEAKER_PIN, OUTPUT);
  analogWrite(SPEAKER_PIN, 0);

  // Set up LEDs
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  pinMode(LED_4_PIN, OUTPUT);

  int leds[] = {LED_1_PIN, LED_2_PIN, LED_3_PIN, LED_4_PIN};

  pomodoro.set_leds(leds);
  pomodoro.leds_off();

  // Connect to the BMP280
  bool bmp280_status;
  bmp280_status = bme.begin(BMP280_I2C_ADDR);
  if (!bmp280_status) {
    Serial.println("Could not find a valid BMP280 sensor");
  }

  // Initialize the display
  pomodoro.disp_clear();

  // Initialize the proximity sensor
  prox.begin();
}

void loop() {

  // Update the bouncers
  btn_1_db.update();
  btn_2_db.update();

  if (btn_1_db.rose()) {
    pomodoro.button_1();
  }

  if (btn_2_db.rose()) {
    pomodoro.button_2();
  }

  if (prox.near()) {
    pomodoro.proximity_detected();
  }

  // Determine the proper brightness level
  uint16_t amb = prox.ambient();
  int br_level = last_br_level;
  
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
  if (br_level != last_br_level && !prox.near()) {
    pomodoro.set_brightness(br_level);
    last_br_level = br_level;
  }
  
  pomodoro.update();

  if (temp_interval.ready()) {
    pomodoro.set_temp(round(bme.readTemperature() * 9/5 + 32));
  }

 delay(10);

}


