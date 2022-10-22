#ifndef _DEVICE_H
#define _DEVICE_H

#include <stdint.h>
#include <Arduino.h>

class Device
{
private:
  char deviceCode;

protected:
  bool checkIsCommandToThisDevice(uint8_t code)
  {
    return deviceCode == code;
  }

public:
  Device(uint8_t deviceCode)
  {
    this->deviceCode = (char)deviceCode;
  }
};

#endif
