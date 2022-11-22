#ifndef _IMU_DATA_H
#define _IMU_DATA_H

#include <sstream>

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

    IMUData *clone()
    {
        IMUData *p = new IMUData();
        p->temperature = temperature;
        p->accX = accX;
        p->accY = accY;
        p->accZ = accZ;
        p->gyroX = gyroX;
        p->gyroY = gyroY;
        p->gyroZ = gyroZ;
        p->angleX = angleX;
        p->angleY = angleY;
        p->angleZ = angleZ;
        p->accAngleX = accAngleX;
        p->accAngleY = accAngleY;
        return p;
    }

    const char *toJson()
    {
        std::stringstream ss;
        ss << "{\n";
        ss << "'temperature' : " << temperature << "\n";
        ss << "'accX' : " << accX << "\n";
        ss << "'accY' : " << accY << "\n";
        ss << "'accZ' : " << accZ << "\n";
        ss << "'gyroX' : " << gyroX << "\n";
        ss << "'gyroY' : " << gyroY << "\n";
        ss << "'gyroZ' : " << gyroZ << "\n";
        ss << "'angleX' : " << angleX << "\n";
        ss << "'angleY' : " << angleY << "\n";
        ss << "'angleZ' : " << angleZ << "\n";
        ss << "'accAngleX' : " << accAngleX << "\n";
        ss << "'accAngleY' : " << accAngleY << "\n";
        ss << "}\n";
        return ss.str().c_str();
    }
};

#endif