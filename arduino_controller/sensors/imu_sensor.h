#ifndef _IMU_SENSOR_DEVICE_H
#define _IMU_SENSOR_DEVICE_H

#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>
#include "sensor.h"

#define MPU6050_ADDR 0x68
#define MPU6050_SMPLRT_DIV 0x19
#define MPU6050_CONFIG 0x1a
#define MPU6050_GYRO_CONFIG 0x1b
#define MPU6050_ACCEL_CONFIG 0x1c
#define MPU6050_WHO_AM_I 0x75
#define MPU6050_PWR_MGMT_1 0x6b
#define MPU6050_TEMP_H 0x41
#define MPU6050_TEMP_L 0x42

#define CALIBRATE 1

#define CALIBRATION_SAMPLING_PERIOD 1
#define MEASURE_SAMPLING_PERIOD 1000

class IMU : public Sensor
{
    TwoWire *wire;

    float accCoef;
    float gyroCoef;
    float gyroOffsetX;
    float gyroOffsetY;
    float gyroOffsetZ;

    bool calibrated;
    uint16_t callibrationCount;

    float temperature;
    float accX;
    float accY;
    float accZ;
    float gyroX;
    float gyroY;
    float gyroZ;
    float angleX;
    float angleY;
    float angleZ;
    float accAngleX;
    float accAngleY;
    float angleGyroX;
    float angleGyroY;
    float angleGyroZ;

private:
    void writeReg(byte reg, byte data)
    {
        wire->beginTransmission(MPU6050_ADDR);
        wire->write(reg);
        wire->write(data);
        wire->endTransmission();
    }

    byte readReg(byte reg)
    {
        wire->beginTransmission(MPU6050_ADDR);
        wire->write(reg);
        wire->endTransmission(true);
        wire->requestFrom((uint8_t)MPU6050_ADDR, (uint8_t)1);
        return wire->read();
    }

    void requestSensorData(uint8_t size)
    {
        wire->beginTransmission(MPU6050_ADDR);
        wire->write(0x43);
        wire->endTransmission(false);
        wire->requestFrom((uint8_t)MPU6050_ADDR, size);
    }

    int16_t readNextSensorData()
    {
        return wire->read() << 8 | wire->read();
    }

    void startCalibration()
    {
        //Serial.println("IMU startCalibration()");
        setSamplingPeriod(CALIBRATION_SAMPLING_PERIOD);
        callibrationCount = 3001;
        this->gyroX = 0;
        this->gyroY = 0;
        this->gyroZ = 0;
        calibrated = false;
    }

    void readFullSensorData()
    {
        requestSensorData(14);

        float rawAccX = readNextSensorData();
        float rawAccY = readNextSensorData();
        float rawAccZ = readNextSensorData();
        float rawTemp = readNextSensorData();
        float rawGyroX = readNextSensorData();
        float rawGyroY = readNextSensorData();
        float rawGyroZ = readNextSensorData();

        this->temperature = (rawTemp + 12412.0) / 340.0;
        this->accX = ((float)rawAccX) / 16384.0;
        this->accY = ((float)rawAccY) / 16384.0;
        this->accZ = ((float)rawAccZ) / 16384.0;

        this->gyroX = (((float)rawGyroX) / 65.5) - gyroOffsetX;
        this->gyroY = (((float)rawGyroY) / 65.5) - gyroOffsetY;
        this->gyroZ = (((float)rawGyroZ) / 65.5) - gyroOffsetZ;

        float interval = (millis() - lastExecutionTimestamp()) * 0.001;

        this->angleGyroX += this->gyroX * interval;
        this->angleGyroY += this->gyroY * interval;
        this->angleGyroZ += this->gyroZ * interval;

        this->accAngleX = atan2(this->accY, sqrt(this->accZ * this->accZ + this->accX * this->accX)) * 360 / 2.0 / PI;
        this->accAngleY = atan2(this->accX, sqrt(this->accZ * this->accZ + this->accY * this->accY)) * 360 / -2.0 / PI;

        this->angleX = (gyroCoef * (this->angleX + this->gyroX * interval)) + (accCoef * this->accAngleX);
        this->angleY = (gyroCoef * (this->angleY + this->gyroY * interval)) + (accCoef * this->accAngleY);
        this->angleZ = this->angleGyroZ;
    }

protected:
    bool isCalibrated() override
    {
        return calibrated;
    }

    void calibrationLoop() override
    {
        callibrationCount--;
        calibrated = false;

        if (callibrationCount == 0)
        {
            this->angleGyroX = 0;
            this->angleGyroY = 0;
            this->angleGyroZ = 0;

            gyroOffsetX = this->gyroX / 3000;
            gyroOffsetY = this->gyroY / 3000;
            gyroOffsetZ = this->gyroZ / 3000;

            setSamplingPeriod(MEASURE_SAMPLING_PERIOD);
            calibrated = true;
        }
        else
        {
            requestSensorData(6);
            uint16_t rx = readNextSensorData();
            uint16_t ry = readNextSensorData();
            uint16_t rz = readNextSensorData();

            this->gyroX += ((float)rx) / 65.5;
            this->gyroY += ((float)ry) / 65.5;
            this->gyroZ += ((float)rz) / 65.5;
        }
    }

    bool timedPublishData(AsyncCommunication &comm) override
    {
        readFullSensorData();

        comm.write(getDeviceCode());
        comm.write(48);
        comm.writeF(this->temperature);
        comm.writeF(this->accX);
        comm.writeF(this->accY);
        comm.writeF(this->accZ);
        comm.writeF(this->gyroX);
        comm.writeF(this->gyroY);
        comm.writeF(this->gyroZ);
        comm.writeF(this->angleX);
        comm.writeF(this->angleY);
        comm.writeF(this->angleZ);
        comm.writeF(this->accAngleX);
        comm.writeF(this->accAngleY);
        //Serial.println("IMU timedPublishData()");
        return true;
    }

public:
    IMU(uint8_t deviceCode) : Sensor(deviceCode)
    {
        wire = &Wire;
        accCoef = 0.02f;
        gyroCoef = 0.98f;
    }

    ~IMU()
    {
    }

    void initialize() override
    {
        wire->begin();
        setSamplingPeriod(MEASURE_SAMPLING_PERIOD);

        writeReg(MPU6050_SMPLRT_DIV, 0x00);
        writeReg(MPU6050_CONFIG, 0x00);
        writeReg(MPU6050_GYRO_CONFIG, 0x08);
        writeReg(MPU6050_ACCEL_CONFIG, 0x00);
        writeReg(MPU6050_PWR_MGMT_1, 0x01);

        this->angleGyroX = 0;
        this->angleGyroY = 0;
        this->angleGyroZ = 0;

        gyroOffsetX = 0;
        gyroOffsetY = 0;
        gyroOffsetZ = 0;

        readFullSensorData();

        this->angleGyroX = 0;
        this->angleGyroY = 0;
        this->angleX = this->accAngleX;
        this->angleY = this->accAngleY;

        callibrationCount = 0;

        //Serial.println("received command for calibration on initialize");
        startCalibration();
    }

    bool readCommand(AsyncCommunication &comm) override
    {
        uint8_t deviceId = comm.read(2);
        if (!checkIsCommandToThisDevice(deviceId))
            return false;

        uint8_t cmd = comm.read(3);

        if (cmd == CALIBRATE)
        {
            //Serial.println("received command for calibration");
            startCalibration();
            return true;
        }

        return false;
    }
};
#endif