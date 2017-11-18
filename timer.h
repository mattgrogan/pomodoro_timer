/******************************************** 
 *  timer.h
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#ifndef Timer_h
#define Timer_h

#include "Arduino.h"

#define COUNTDOWN_OFF 100
#define COUNTDOWN_READY 101
#define COUNTDOWN_RUNNING 102
#define COUNTDOWN_PAUSED 103

class Timer {
  public:
    int set(int duration_secs);
    int start();
    int pause();
    void reset();
    int remaining();
    bool expired();
    int state();
  private:
    int _duration_secs = 0;
    int _elapsed = 0;
    int _current_state = COUNTDOWN_OFF;
    unsigned long _start_time;
};

#endif
