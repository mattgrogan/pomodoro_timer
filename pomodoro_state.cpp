/******************************************** 
 *  pomodoro_state.cpp
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#include "Arduino.h"
#include "pomodoro_state.h"

int State_Off::name() {
  return STATE_OFF;
}

int State_Active::name() {
  return STATE_ACTIVE;
}

int State_Break::name() {
  return STATE_BREAK;
}

int State_Temp::name() {
  return STATE_TEMP;
}

