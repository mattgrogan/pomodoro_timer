/******************************************** 
 *  animation.h
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#ifndef animation_h
#define animation_h

#import "interval.h"

#define SEGMENT_LENGTH 4

#define SEG_A 0x01
#define SEG_B 0x02
#define SEG_C 0x04
#define SEG_D 0x08
#define SEG_E 0x10
#define SEG_F 0x20
#define SEG_G 0x40
#define SEG_P 0x80

#define RIGHT SEG_B | SEG_C
#define MID   SEG_A | SEG_G | SEG_D
#define LEFT  SEG_F | SEG_E

#define NULL_ANIMATION 0
#define COLON_BLINK_ANIM 1
#define HOUR_BLINK_ANIM 2
#define MIN_BLINK_ANIM 3
#define SWIPE_IN_ANIM 4

class DisplayData {
  public:
    DisplayData();
    DisplayData(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, bool c = false);
    uint8_t digit_0 = 0x00;
    uint8_t digit_1 = 0x00;
    uint8_t digit_2 = 0x00;
    uint8_t digit_3 = 0x00;
    bool colon = true; 
};

class Animation {
  public:
    void reset();
    void next();
    void end();
    bool is_done();
    virtual DisplayData mask(DisplayData);
};

class NullAnimation : public Animation {
  public:
    DisplayData mask(DisplayData);
};

class ColonAnimation : public Animation {
  // Blink the colon every one seconds
  private:
    Interval _colon_interval = Interval(1000);
    bool _colon_on = true;
  public:
    DisplayData mask(DisplayData);
};


class Pattern {
  private:
    uint8_t * _steps;
    uint8_t _n_steps;
    uint8_t _mask = 0x00;
    uint8_t _index = 0;
  public:
    void set_pattern(uint8_t *steps, uint8_t n_steps);
    void first();
    void next();
    bool is_done();
    uint8_t mask();
    uint8_t mask(uint8_t bitmap);
    uint8_t current_item();
};

class MatrixPattern {
  private:
    Pattern _digits[SEGMENT_LENGTH];
    bool _end = false;
    int _index = 3;
    
  public:
    void set_pattern(uint8_t *steps, uint8_t n_steps);
    void first();
    void next();
    void end();
    bool is_done();
    uint8_t mask(uint8_t digit, uint8_t bitmap);
    uint8_t digit();
};

#endif
