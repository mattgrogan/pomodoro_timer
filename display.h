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

class SevenSegDisplay {
  // Base class for seven segment display
  protected:
    Adafruit_7segment _m;

  public:
    void begin(uint8_t _arr);
    void set_brightness(int br);
    void write(DisplayData);
    void clear(); 
};

class AnimatedDisplay : public SevenSegDisplay {
  private:
    Animation       *_current_animation;
    NullAnimation   _null_anim;
    ColonAnimation  _colon_blink;
    NullAnimation   _hour_blink;
    NullAnimation _min_blink;
    NullAnimation     _swipe_in;
  public:
    void set_animation(uint8_t a);
    void write(DisplayData);
    void end();
    void reset();
};

#endif
