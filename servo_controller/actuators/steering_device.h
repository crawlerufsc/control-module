#ifndef _DIRECTION_DEVICE_H
#define _DIRECTION_DEVICE_H

#include <stdint.h>
#include <Servo.h>

#include "device.h"
#include "serial_comm.h"
#include <Arduino.h>

class SteeringDevice : public Device
{
private:
    uint8_t deviceCode;
    uint8_t pwmDirection;
    Servo servo;
    
    void setSteeringAngle(uint8_t angle)
    {
        servo.write(angle);
    }

public:
    SteeringDevice(uint8_t deviceCode, uint8_t pwmDirection) :  Device(deviceCode)
    {
        this->deviceCode = deviceCode;
        this->pwmDirection = pwmDirection;
    }

    ~SteeringDevice()
    {
    }

    void initialize()
    {
        servo.attach(this->pwmDirection);
    }

    bool readCommand(AsyncCommunication &comm)
    {
        uint8_t deviceId = comm.read(2);

        if (!checkIsCommandToThisDevice(deviceId))
            return false;

        uint8_t directionValue = comm.read(3);

        if (directionValue < 0)
            directionValue = 0;
        else if (directionValue > 90)
            directionValue = 90;

        setSteeringAngle(directionValue);

        return true;
    }
};
#endif