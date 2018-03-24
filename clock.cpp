/******************************************** 
 *  clock.cpp
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2018
 ********************************************/

#include "clock.h"

bool Clock::begin() {
  return rtc.begin();
}

uint8_t Clock::hour() {
  DateTime now = rtc.now();
  return now.hour();
}

uint8_t Clock::minute() {
  DateTime now = rtc.now();
  return now.minute();
}

void Clock::increment_hour() {
  DateTime now = rtc.now();

  int new_hour = now.hour() + 1;
  new_hour = new_hour <= 24 ? new_hour : 1;

  Serial.println("Setting hour to: ");
  Serial.println(new_hour);
  
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), new_hour, now.minute(), now.second()));
}

void Clock::increment_minute() {
  DateTime now = rtc.now();

  int new_min = now.minute() + 1;
  new_min = new_min < 60 ? new_min : 0;

  Serial.println("Setting minute to: ");
  Serial.println(new_min);

  rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), new_min, now.second()));  
}
 
