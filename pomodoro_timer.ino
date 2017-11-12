/*******************
 *  Pomodoro Timer
 *  Matthew Grogan
 *******************/

#define MATRIX_I2C_ADDR 0x70
int ACTIVE_SECS = 25 * 60;

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix = Adafruit_7segment();

unsigned long start_time;
unsigned long elapsed;
unsigned long remaining;

void setup() {
  matrix.begin(MATRIX_I2C_ADDR);

  start_time = millis();

  Serial.begin(9600);

}

void loop() {
  elapsed = (millis() - start_time) / 1000;
  remaining = ACTIVE_SECS - elapsed;

  Serial.println(remaining);
  

  matrix.println(remaining);
  matrix.drawColon(false);
  matrix.writeDisplay();
  delay(1000);

}
