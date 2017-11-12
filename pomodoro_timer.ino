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
  // Calculate the remaining time
  elapsed = (millis() - start_time) / 1000;
  remaining = ACTIVE_SECS - elapsed;

  // Extract the minutes remaining
  int remaining_secs = remaining % 60;
  int remaining_mins = (remaining - remaining_secs) / 60;
  
  Serial.println(remaining_secs);
  Serial.println(remaining_mins);

  matrix.writeDigitNum(0, (remaining_mins / 10));
  matrix.writeDigitNum(1, (remaining_mins % 10));
  matrix.drawColon(true);
  matrix.writeDigitNum(3, (remaining_secs / 10));
  matrix.writeDigitNum(4, (remaining_secs % 10));

  matrix.writeDisplay();
  delay(1000);

}
