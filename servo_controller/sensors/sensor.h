#ifndef _SENSOR_DEVICE_H
#define _SENSOR_DEVICE_H

#include "../device.h"
#include "../async_comm.h"

class Sensor : public Device
{
public:
    Sensor(uint8_t deviceCode) : Device(deviceCode) {}

    virtual bool publishData(AsyncCommunication &comm) = 0;
};

#endif