#ifndef _DUMMY_SENSOR_H
#define _DUMMY_SENSOR_H

#include <stdint.h>
#include <Arduino.h>

#include "sensor.h"

#define DUMMY_SENSOR_CMD_ACTIVATE 1
#define DUMMY_SENSOR_CMD_DEACTIVATE 2

class DummySensor : public Sensor
{
private:
    bool active;

public:
    DummySensor(uint8_t deviceCode) : Sensor(deviceCode)
    {
    }

    ~DummySensor()
    {
    }

    void initialize() override
    {
        setSamplingPeriod(1000);
        active = false;
    }

    bool isCalibrated() override
    {
        return true;
    }
    
    void calibrationLoop() override
    {
        
    }

    bool timedPublishData(AsyncCommunication &comm) override
    {
        if (!active)
            return false;

        // comm.write(1); // frameID
        // comm.write(PROTOCOL_FRAME_TYPE_DATA);
        comm.write(getDeviceCode());
        comm.write(3);
        comm.write(9);
        comm.write(19);
        comm.write(29);
        return true;
    }

    bool readCommand(AsyncCommunication &comm) override
    {
        uint8_t deviceId = comm.read(2);

        if (!checkIsCommandToThisDevice(deviceId))
            return false;

        switch (comm.read(3))
        {
        case DUMMY_SENSOR_CMD_ACTIVATE:
            active = true;
            return true;
        case DUMMY_SENSOR_CMD_DEACTIVATE:
            active = false;
            return true;
        }

        return false;
    }
};
#endif