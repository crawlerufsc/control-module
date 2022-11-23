#ifndef _DIRECTION_DEVICE_H
#define _DIRECTION_DEVICE_H

#include <stdint.h>
#include <Servo.h>
#include <Arduino.h>

#include "../device.h"
#include "../async_comm.h"

#define PHY_CENTER_ANGLE_FRONT 90
#define PHY_CENTER_ANGLE_BACK 76

#define MAX_RANGE 40

#define STEERING_DRIVER_LEFT 1
#define STEERING_DRIVER_RIGHT 2
#define STEERING_DRIVER_CENTER 3

class DoubleSteeringDevice : public Device
{
private:
    uint8_t deviceCode;
    uint8_t pwmDirectionFront;
    uint8_t pwmDirectionBack;
    Servo servoFront;
    Servo servoBack;

    void setSteeringAngle(int angle)
    {
        if (angle > MAX_RANGE)
            angle = MAX_RANGE;

        if (angle < -MAX_RANGE)
            angle = -MAX_RANGE;

        servoFront.write(PHY_CENTER_ANGLE_FRONT + angle);
        servoBack.write(PHY_CENTER_ANGLE_BACK - angle);
    }

public:
    DoubleSteeringDevice(uint8_t deviceCode, uint8_t pwmDirectionFront, uint8_t pwmDirectionBack) : Device(deviceCode)
    {
        this->deviceCode = deviceCode;
        this->pwmDirectionFront = pwmDirectionFront;
        this->pwmDirectionBack = pwmDirectionBack;
    }

    ~DoubleSteeringDevice()
    {
    }

    void initialize() override
    {
        servoFront.attach(this->pwmDirectionFront);
        servoBack.attach(this->pwmDirectionBack);
        setSteeringAngle(0);
    }
    bool readCommand(AsyncCommunication &comm) override
    {
        uint8_t deviceId = comm.read(2);

        if (!checkIsCommandToThisDevice(deviceId))
            return false;

        uint8_t direction = comm.read(3);
        uint8_t angle = comm.read(4);

        switch (direction)
        {
        case STEERING_DRIVER_CENTER:
            setSteeringAngle(0);
            break;
        case STEERING_DRIVER_LEFT:
            setSteeringAngle(-angle);
            break;
        case STEERING_DRIVER_RIGHT:
            setSteeringAngle(angle);
            break;
        default:
            break;
        }

        return true;
    }
};
#endif