/******************************************** 
 *  interval.h
 *  
 *  Library for pomodoro timer arduino project
 *  Created by Matthew Grogan 2017
 ********************************************/

#ifndef interval_h
#define interval_h

class Interval {
  unsigned long _last;
  int _interval_ms;

  public:
    Interval(int interval_ms) {
      _interval_ms = interval_ms;
    }
    void reset() {
      _last = millis();
    }
    bool ready() {
      // Has the interval passed since the last measurement
      bool result = false;
      int time_since = millis() - _last;
    
      if (time_since > _interval_ms) {
        result = true;
        _last = millis();
      }

      return result;
    }
};

#endif
