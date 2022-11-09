#ifndef _DIRECTION_DEVICE_H
#define _DIRECTION_DEVICE_H

#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>
#include "device.h"
#include "async_comm.h"

class IMU : public Device
{
private:
    float accX;
    float accY;
    float accZ;

    float accAngleErrorX;
    float accAngleErrorY;

    float gyroX;
    float gyroY;

    float gyroErrorX;
    float gyroErrorY;

    void rst()
    {
        Wire.beginTransmission(0x68);
        Wire.write(0x6B);
        Wire.write(0x00);
        Wire.endTransmission(true);
    }

    void setupGyro()
    {
        Wire.beginTransmission(0x68);
        Wire.write(0x1B);
        Wire.write(0x10);
        Wire.endTransmission(true);
    }
    void setupAcc()
    {
        Wire.beginTransmission(0x68);
        Wire.write(0x1C);
        Wire.write(0x10);
        Wire.endTransmission(true);
    }

    void reqAccValues()
    {
        Wire.beginTransmission(0x68);
        Wire.write(0x3B);
        Wire.endTransmission(false);
        Wire.requestFrom(0x68, 6, true);
    }
    void reqGyroValues()
    {
        Wire.beginTransmission(0x68);
        Wire.write(0x43);
        Wire.endTransmission(false);
        Wire.requestFrom(0x68, 4, true);
    }

    float readRegister()
    {
        Wire.read() << 8 | Wire.read();
    }

    void estimateMeanError()
    {
        accAngleErrorX = 0;
        accAngleErrorY = 0;
        gyroErrorX = 0;
        gyroErrorY = 0;

        float rad_to_deg = 180 / 3.141592654;

        for (int i = 0; i < 200; i++)
        {
            reqAccValues();
            accX = readRegister() / 4096.0;
            accY = readRegister() / 4096.0;
            accZ = readRegister() / 4096.0;

            accAngleErrorX += atan((accY) / sqrt(pow((accX), 2) + pow((accZ), 2))) * rad_to_deg;
            accAngleErrorY += atan(-1 * (accX) / sqrt(pow((accY), 2) + pow((accZ), 2))) * rad_to_deg;
        }

        for (int i = 0; i < 200; i++)
        {
            reqGyroValues();
            gyroX = readRegister();
            gyroY = readRegister();

            gyroErrorX += gyroX / 32.8;
            gyroErrorY += gyroY / 32.8;
        }

        accAngleErrorX /= 200;
        accAngleErrorY /= 200;

        gyroErrorX /= 200;
        gyroErrorY /= 200;

        //https://electronoobs.com/eng_arduino_tut76.php
    }

public:
    IMU(uint8_t deviceCode) : Device(deviceCode)
    {
    }

    ~IMU()
    {
    }

    void initialize()
    {
        Wire.begin();
        rst();
        setupGyro();
        setupAcc();
    }

    bool readCommand(AsyncCommunication &comm)
    {
        uint8_t deviceId = comm.read(2);
        return checkIsCommandToThisDevice(deviceId);
    }
};
#endif