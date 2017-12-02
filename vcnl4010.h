/******************************************** 
 *  vncl4010.h
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#ifndef vcnl4010_h
#define vcnl4010_h

#include "Adafruit_VCNL4010.h"

class VCNL4010 {
  public:
    bool begin();
    bool near();
    uint16_t read_ambient();
  private:
    Adafruit_VCNL4010 vcnl;  
};

#endif
