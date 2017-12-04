/******************************************** 
 *  vncl4010.h
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#ifndef vcnl4010_h
#define vcnl4010_h

#define DEFAULT_UPDATE_FREQ_MS 500

#include "Adafruit_VCNL4010.h"
#include "interval.h"

class VCNL4010 {
  public:
    bool begin();
    bool near();
    void update();
    uint16_t proximity();
    uint16_t ambient();
    uint16_t read_proximity();
    uint16_t read_ambient();
  private:
    Adafruit_VCNL4010 vcnl;
    Interval _interval = Interval(DEFAULT_UPDATE_FREQ_MS);
    uint16_t _last_ambient;
    uint16_t _last_proximity;
};

#endif
