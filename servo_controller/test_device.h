#ifndef _TEST_DEVICE_H
#define _TEST_DEVICE_H

#include <stdint.h>
#include <SoftwareSerial.h>

#include "device.h"
#include <Arduino.h>

#include "async_comm.h"

#define TEST_DEVICE_ID 1

class TestDevice : public Device {
private:

public:
    TestDevice() : Device(TEST_DEVICE_ID) {}

    bool readCommand(AsyncCommunication &comm) {
        return checkIsCommandToThisDevice(comm.read(1));
    }
};

#endif
