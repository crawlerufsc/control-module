#ifndef _DUMMY_SENSOR_H
#define _DUMMY_SENSOR_H

#include <stdint.h>
#include <Arduino.h>

#include "device.h"
#include "async_comm.h"

#define DUMMY_SENSOR_CMD_ACTIVATE 1
#define DUMMY_SENSOR_CMD_DEACTIVATE 2

class DummySensor : public Device
{
private:
    bool active;

public:
    DummySensor(uint8_t deviceCode) : Device(deviceCode)
    {
    }

    ~DummySensor()
    {
    }

    void initialize()
    {
        active = false;
    }

    void sendData(AsyncCommunication &comm)
    {
        if (!active)
            return;

        comm.write(1); // frameID
        comm.write(PROTOCOL_FRAME_TYPE_DATA);
        comm.write(getDeviceCode());
        comm.write(9);
        comm.write(19);
        comm.write(29);
        comm.sendData();
    }

    bool readCommand(AsyncCommunication &comm)
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