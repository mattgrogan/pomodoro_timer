/******************************************** 
 *  vncl4010.cpp
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#define PROXIMITY_THRESHOLD 2400

#include "Arduino.h"
#include "vcnl4010.h"

bool VCNL4010::begin() {
  return vcnl.begin();
}

bool VCNL4010::near() {
  return (proximity() > PROXIMITY_THRESHOLD);
}

void VCNL4010::update() {
  if (_interval.ready()) {
    _last_ambient = read_ambient();
    _last_proximity = read_proximity();
  }
}

uint16_t VCNL4010::ambient() {
  update();
  return _last_ambient;
}

uint16_t VCNL4010::proximity() {
  update();
  return _last_proximity;
}

uint16_t VCNL4010::read_ambient() {
  return vcnl.readAmbient();
}

uint16_t VCNL4010::read_proximity() {
  return vcnl.readProximity();
}
