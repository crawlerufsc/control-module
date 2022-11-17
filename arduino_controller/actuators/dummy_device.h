#ifndef _DUMMY_DEVICE_H
#define _DUMMY_DEVICE_H

#include <stdint.h>
#include <Arduino.h>

#include "../device.h"
#include "../async_comm.h"

#define TEST_DEVICE_ID 1

class DummyDevice : public Device
{
private:
public:
    DummyDevice() : Device(TEST_DEVICE_ID) {}

    void initialize() override
    {
    }
    
    bool readCommand(AsyncCommunication &comm) override
    {
        return checkIsCommandToThisDevice(comm.read(2));
    }
};

#endif
