#ifndef _TIMEOUT_CONTROLLER_H
#define _TIMEOUT_CONTROLLER_H

#include <stdint.h>
#include <Arduino.h>

class TimeoutController {
private:
  unsigned long timeStamp;

public:

  TimeoutController() {
    timeStamp = 0;
  }
 
  bool execEvery(unsigned long time_ms) {

    unsigned long current = millis();

    if (current - timeStamp >= time_ms) {
      timeStamp = current;
      return true;
    }

    return false;
  }
};

#endif
