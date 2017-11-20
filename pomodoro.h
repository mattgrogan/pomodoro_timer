/******************************************** 
 *  pomodoro.h
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#ifndef Pomodoro_State_h
#define Pomodoro_State_h

#include <Adafruit_LEDBackpack.h>
#include "RTClib.h"

#include "Arduino.h"
#include "timer.h"

const int ACTIVE_SECS = 25 * 60;
const int BREAK_SECS = 5 * 60;
const int LONG_BREAK_SECS = 15 * 60;
const int POMODORO_STEPS = 8;
const int NBR_LEDS = 5;

#define STATE_OFF -1
#define STATE_READY 0
#define STATE_ACTIVE 1
#define STATE_TEMP 2
#define STATE_CLOCK 3

class Pomodoro; 

class State {
  public:
    virtual void button_1(Pomodoro *p) = 0;
    virtual void button_2(Pomodoro *p) = 0;
    virtual void update(Pomodoro *p) = 0;
};

class State_Off : public State {
  public:
    void button_1(Pomodoro *p);
    void button_2(Pomodoro *p);
    void update(Pomodoro *p);
};

class State_Ready : public State {
  public:
    void button_1(Pomodoro *p);
    void button_2(Pomodoro *p);
    void update(Pomodoro *p);
};

class State_Active : public State {
  public:
    void button_1(Pomodoro *p);
    void button_2(Pomodoro *p);
    void update(Pomodoro *p);
};

class State_Temp : public State {
  public:
    void button_1(Pomodoro *p);
    void button_2(Pomodoro *p);
    void update(Pomodoro *p);
};

class State_Clock : public State {
  public:
    void button_1(Pomodoro *p);
    void button_2(Pomodoro *p);
    void update(Pomodoro *p);
};

class Pomodoro {
  private:
    Adafruit_7segment _m;
    RTC_DS3231 rtc;
    
    int _temp_f;
    int _sequence[POMODORO_STEPS] = {
                        ACTIVE_SECS, BREAK_SECS,
                        ACTIVE_SECS, BREAK_SECS,
                        ACTIVE_SECS, BREAK_SECS,
                        ACTIVE_SECS, LONG_BREAK_SECS};
                        
    int _seq_idx = 0;
    int _led_pins[NBR_LEDS];
    
    State *_current_state;
    
    State_Off _state_off;
    State_Ready _state_ready;
    State_Active _state_active;
    State_Temp _state_temp;
    State_Clock _state_clock;
  public:
    Timer timer;
    Pomodoro();
    void set_leds(int leds[NBR_LEDS]);
    void set_state(int state);
    void set_timer();
    void reset_timer();
    void set_temp(int temp_f);
    void button_1();
    void button_2();
    void disp_begin(int matrix_addr);
    void disp_countdown();
    void disp_temp();
    void disp_clock();
    void disp_clear();
    void leds_on();
    void leds_off();
    void update();
};

#endif
