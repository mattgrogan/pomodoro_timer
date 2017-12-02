/******************************************** 
 *  vncl4010.cpp
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#define PROXIMITY_THRESHOLD 3000

#include "Arduino.h"
#include "vcnl4010.h"

bool VCNL4010::begin() {
  return vcnl.begin();
}

bool VCNL4010::near() {
  return (vcnl.readProximity() > PROXIMITY_THRESHOLD);
}

