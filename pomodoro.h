/******************************************** 
 *  pomodoro.h
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#ifndef Pomodoro_State_h
#define Pomodoro_State_h

#include "Arduino.h"

#define STATE_OFF -1
#define STATE_ACTIVE 0
#define STATE_BREAK 1
#define STATE_TEMP 2

class State {
  public:
    virtual int name() = 0;
};

class State_Off : public State {
  public:
    int name();
};

class State_Active : public State {
  public:
    int name();
};

class State_Break : public State {
  public:
    int name();
};

class State_Temp : public State {
  public:
    int name();
};

class Pomodoro {
  private:
    State *_current_state;
    
    State_Off _state_off;
    State_Active _state_active;
    State_Break _state_break;
    State_Temp _state_temp;  
  public:
    Pomodoro();
    int state();
    void set_state(int state);
};

#endif
