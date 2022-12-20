#ifndef _IMU_DATA_H
#define _IMU_DATA_H

#include <sstream>
#include <string>

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

    IMUData *clone(); 
    std::string toJson();
    static IMUData * decode(char *data);
    static IMUData * decode(char *data, uint16_t initPos);
    char * encode();

};

#endif