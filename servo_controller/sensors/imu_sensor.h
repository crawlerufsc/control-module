#ifndef _IMU_SENSOR_DEVICE_H
#define _IMU_SENSOR_DEVICE_H

#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>
#include "sensor.h"

#define rad_to_deg  180 / 3.141592654;

class IMU : public Sensor
{
private:
    float accX;
    float accY;
    float accZ;

    float accAngleX;
    float accAngleY;

    float accAngleErrorX;
    float accAngleErrorY;

    float gyroX;
    float gyroY;

    float gyroErrorX;
    float gyroErrorY;

    TimeoutController tc;

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

    void readAccelValues()
    {
        Wire.beginTransmission(0x68);
        Wire.write(0x3B);
        Wire.endTransmission(false);
        Wire.requestFrom(0x68, 6, true);

        accX = readRegister() / 4096.0;
        accY = readRegister() / 4096.0;
        accZ = readRegister() / 4096.0;
    }
    void readGyroValues()
    {
        Wire.beginTransmission(0x68);
        Wire.write(0x43);
        Wire.endTransmission(false);
        Wire.requestFrom(0x68, 4, true);

        gyroX = readRegister() /32.8;
        gyroY = readRegister() /32.8;
    }

    float readRegister()
    {
        Wire.read() << 8 | Wire.read();
    }

    float computeAngleX() {
        return atan((accY) / sqrt(pow((accX), 2) + pow((accZ), 2))) * rad_to_deg;
    }
    float computeAngleY() {
        return atan(-1 * (accX) / sqrt(pow((accY), 2) + pow((accZ), 2))) * rad_to_deg;
    }

    void estimateMeanError()
    {
        accAngleErrorX = 0;
        accAngleErrorY = 0;
        gyroErrorX = 0;
        gyroErrorY = 0;

        

        for (int i = 0; i < 200; i++)
        {
            readAccelValues();
            accAngleErrorX += computeAngleX();
            accAngleErrorY += computeAngleY();
        }

        for (int i = 0; i < 200; i++)
        {
            readGyroValues();
            gyroErrorX += gyroX / 32.8;
            gyroErrorY += gyroY / 32.8;
        }

        accAngleErrorX /= 200;
        accAngleErrorY /= 200;

        gyroErrorX /= 200;
        gyroErrorY /= 200;

        // https://electronoobs.com/eng_arduino_tut76.php
    }

public:
    IMU(uint8_t deviceCode) : Sensor(deviceCode)
    {
    }

    ~IMU()
    {
    }

    void initialize() override
    {
        Wire.begin();
        rst();
        setupGyro();
        setupAcc();
        estimateMeanError();
    }

    bool publishData(AsyncCommunication &comm) override
    {
        if (!tc.checkTimeout(100)) return false;
        
        readAccelValues();
        readGyroValues();

        float accAngleXCorr = accAngleX - accAngleErrorX;
        float accAngleYCorr = accAngleY - accAngleErrorY;
        if (accAngleXCorr < 0) accAngleXCorr = 0;
        if (accAngleYCorr < 0) accAngleYCorr = 0;

        comm.write(getDeviceCode());
        comm.write(28);
        comm.writeF(accX);
        comm.writeF(accY);
        comm.writeF(accZ);
        comm.writeF(accAngleXCorr);
        comm.writeF(accAngleYCorr);
        comm.writeF(gyroX - gyroErrorX);
        comm.writeF(gyroY - gyroErrorY);

        return true;        
    }

    bool readCommand(AsyncCommunication &comm) override
    {
        uint8_t deviceId = comm.read(2);
        return checkIsCommandToThisDevice(deviceId);
    }
};
#endif