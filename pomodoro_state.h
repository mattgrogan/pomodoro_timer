/******************************************** 
 *  pomodoro_state.h
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

#endif
