/*******************
 *  Pomodoro Timer
 *  Matthew Grogan
 *******************/
#include <Bounce2.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "timer.h"
#include "pomodoro.h"
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

  pomodoro.disp_begin(MATRIX_I2C_ADDR);
  pomodoro.disp_clear();

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
  
  pomodoro.update();

  if (temp_interval.ready()) {
    pomodoro.set_temp(round(bme.readTemperature() * 9/5 + 32));
  }

 delay(10);

}


