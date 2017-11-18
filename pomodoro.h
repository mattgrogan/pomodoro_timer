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

// TODO: This is temporary until refactored
const int ACTIVE_SECS = 25 * 60;
const int BREAK_SECS = 5 * 60;

#define STATE_OFF -1
#define STATE_ACTIVE 0
#define STATE_BREAK 1
#define STATE_TEMP 2

class Pomodoro; 

class State {
  public:
    virtual int name() = 0;
    virtual void button_1(Pomodoro *p) = 0;
};

class State_Off : public State {
  public:
    int name();
    void button_1(Pomodoro *p);
};

class State_Active : public State {
  public:
    int name();
    void button_1(Pomodoro *p);
};

class State_Break : public State {
  public:
    int name();
    void button_1(Pomodoro *p);
};

class State_Temp : public State {
  public:
    int name();
    void button_1(Pomodoro *p);
};

class Pomodoro {
  private:

    State *_current_state;
    
    State_Off _state_off;
    State_Active _state_active;
    State_Break _state_break;
    State_Temp _state_temp;  
  public:
    Timer timer;
    Pomodoro();
    int state();
    void set_state(int state);
    void set_timer(int seconds);
    void button_1();
};

#endif
