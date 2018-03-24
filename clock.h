/******************************************** 
 *  clock.h
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2018
 ********************************************/

#ifndef Clock_h
#define Clock_h

#include "RTClib.h"

class Clock {
  private:
    RTC_DS3231 rtc;
  public:
    bool begin();
    uint8_t hour();
    uint8_t minute();
    void increment_hour();
    void increment_minute();
};

#endif
