/******************************************** 
 *  pomodoro.h
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#ifndef Pomodoro_State_h
#define Pomodoro_State_h

#include "Arduino.h"
#include "timer.h"
#include "vcnl4010.h"
#include "interval.h"
#include "clock.h"
#include "display.h"
#include "animation.h"

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
#define STATE_SETHOUR 4
#define STATE_SETMIN 5

class Pomodoro; 

class State {
  public:
    virtual void button_1(Pomodoro *p) = 0;
    virtual void button_2(Pomodoro *p) = 0;
    virtual void proximity_toggle(Pomodoro *p, bool state) = 0;
    virtual void update(Pomodoro *p) = 0;
};

class State_Off : public State {
  public:
    void button_1(Pomodoro *p);
    void button_2(Pomodoro *p);
    void proximity_toggle(Pomodoro *p, bool state);
    void update(Pomodoro *p);
};

class State_Ready : public State {
  public:
    void button_1(Pomodoro *p);
    void button_2(Pomodoro *p);
    void proximity_toggle(Pomodoro *p, bool state);
    void update(Pomodoro *p);
};

class State_Active : public State {
  public:
    void button_1(Pomodoro *p);
    void button_2(Pomodoro *p);
    void proximity_toggle(Pomodoro *p, bool state);
    void update(Pomodoro *p);
};

class State_Temp : public State {
  public:
    void button_1(Pomodoro *p);
    void button_2(Pomodoro *p);
    void proximity_toggle(Pomodoro *p, bool state);
    void update(Pomodoro *p);
};

class State_Clock : public State {
  public:
    void button_1(Pomodoro *p);
    void button_2(Pomodoro *p);
    void proximity_toggle(Pomodoro *p, bool state);
    void update(Pomodoro *p);
};

class State_SetMin : public State {
  public:
    void button_1(Pomodoro *p);
    void button_2(Pomodoro *p);
    void proximity_toggle(Pomodoro *p, bool state);
    void update(Pomodoro *p);
};

class State_SetHour : public State {
  public:
    void button_1(Pomodoro *p);
    void button_2(Pomodoro *p);
    void proximity_toggle(Pomodoro *p, bool state);
    void update(Pomodoro *p);
};

class Pomodoro {
  private:
    
    Interval _colon_interval = Interval(1000);
    bool _colon_on = true;
    
    int _temp_f;
    int _sequence[POMODORO_STEPS] = {
                        ACTIVE_SECS, BREAK_SECS,
                        ACTIVE_SECS, BREAK_SECS,
                        ACTIVE_SECS, BREAK_SECS,
                        ACTIVE_SECS, LONG_BREAK_SECS};
                        
    int _seq_idx = 0;
    int _led_pins[NBR_LEDS];
    int _last_br_level = 15;

    bool _prox_on;
    
    State *_current_state;
    
    State_Off _state_off;
    State_Ready _state_ready;
    State_Active _state_active;
    State_Temp _state_temp;
    State_Clock _state_clock;
    State_SetHour _state_sethour;
    State_SetMin _state_setmin;
  public:
    Display disp;
    Clock clock = Clock();
    Timer timer;
    Pomodoro();
    VCNL4010 prox;
    void set_leds(int leds[NBR_LEDS]);
    void set_state(int state);
    void set_timer();
    void reset_timer();
    void set_temp(int temp_f);
    void button_1();
    void button_2();
    void proximity_status(bool prox_on);
    void disp_begin(int matrix_addr);
    void disp_countdown();
    void disp_temp();
    void disp_clock();
    void disp_clear();
    void check_brightness();
    void leds_on();
    void leds_off();
    void update();
    void test(uint16_t duration_ms);
};



#endif
