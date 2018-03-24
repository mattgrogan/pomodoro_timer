/******************************************** 
 *  display.h
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2018
 ********************************************/

#ifndef Pomodoro_Display_h
#define Pomodoro_Display_h

#include <Adafruit_LEDBackpack.h>

#include "animation.h"
#include "interval.h"

#define ANIMATION_SPEED 20
#define MAX_BRIGHTNESS 15

class Display {
  private:
    Adafruit_7segment _m;
    MatrixPattern _mp;
    Interval _mp_interval = Interval(ANIMATION_SPEED);

  public:
    void begin(uint8_t _addr);
    void all_on(int br = MAX_BRIGHTNESS);
    void set_brightness(int br);
    void write_display(uint8_t digit_0, uint8_t digit_1, uint8_t digit_2, uint8_t digit_3, bool colon = false);
    void end_animation();
    void reset_animation();
    void set_pattern();

    // These are for refactor only. They delegate to the Adafruit 7Segment object
    void writeDigitRaw(uint8_t n, uint16_t bitmask);
    void drawColon(boolean state);
    void writeDisplay();
    void clear();
};


#endif
