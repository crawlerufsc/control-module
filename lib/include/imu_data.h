#ifndef _IMU_DATA_H
#define _IMU_DATA_H

class IMUData
{
private:
    /* data */
public:
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

};

#endif