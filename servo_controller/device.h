#ifndef _DEVICE_H
#define _DEVICE_H

#include <stdint.h>

#include "protocol.h"

class Device
{
private:
  char deviceCode;

protected:
  bool checkIsCommandToThisDevice(uint8_t code)
  {
    return deviceCode == code;
  }
  char getDeviceCode()
  {
    return deviceCode;
  }

public:
  Device(uint8_t deviceCode)
  {
    this->deviceCode = (char)deviceCode;
  }

  virtual void initialize() = 0;

  virtual bool readCommand(AsyncCommunication &comm) = 0;
};

#endif
