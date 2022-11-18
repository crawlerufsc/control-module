#ifndef _DEVICE_CONTROLLER_H
#define _DEVICE_CONTROLLER_H

#include "imu_data.h"
#include "gps_data.h"

#define TEST_DEVICE 1
#define WHEELDRIVER 2
#define FRONT_DIRECTION_DRIVER 3
#define BACK_DIRECTION_DRIVER 4
#define SENSOR_DUMMY 100
#define SENSOR_IMU 101
#define SENSOR_GPS 102

#define WHEELDRIVER_STOP 1
#define WHEELDRIVER_SET_FORWARD_PW 2
#define WHEELDRIVER_SET_BACKWARD_PW 3

#define DUMMY_SENSOR_CMD_ACTIVATE 1
#define DUMMY_SENSOR_CMD_DEACTIVATE 2

#define IMU_CALIBRATE 1
#define IMU_SET_SAMPLING_PERIOD 2

//#define DEBUG 1

typedef unsigned char uchar;

#include <stdint.h>
#include <functional>

typedef union
{
    float fval;
    char bval[4];
} float_pack;

typedef union
{
    uint16_t val;
    char bval[2];
} uint16p;

class ResponseData
{
public:
    char *data;
    unsigned int size;
    uchar frameId;
    uchar frameType;
    uchar deviceId;

    char read(uint8_t pos)
    {
        return data[pos];
    }

    float readF(uint8_t pos)
    {
        float_pack pkt;
        for (uint8_t i = 0; i < 4; i++)
            pkt.bval[i] = data[pos+i];
        return pkt.fval;
    }
    uint16_t read_uint16(uint8_t pos)
    {
        uint16p pkt;
        pkt.bval[0] = data[pos];
        pkt.bval[1] = data[pos+1];
        return pkt.val;
    }
};

class ISerialLink
{
public:
    virtual void addHandler(uchar deviceId, std::function<void(ResponseData *)> &func) = 0;
    virtual bool syncRequest(uchar deviceId, uchar val1) = 0;
    virtual bool syncRequest(int deviceId, uchar val1, uchar val2) = 0;
    virtual bool syncRequest(int deviceId, uchar val1, uint16_t val2) = 0;
    virtual bool syncRequest(int deviceId, uchar val1, uchar val2, uchar val3) = 0;
    virtual void asyncRequest(uchar deviceId, uchar val1) = 0;
    virtual void asyncRequest(int deviceId, uchar val1, uchar val2) = 0;
    virtual void asyncRequest(int deviceId, uchar val1, uchar val2, uchar val3) = 0;
};


class CrawlerHAL
{

private:
    ISerialLink *comm;
    char *allocBuffer(int size);

public:
    CrawlerHAL();
    CrawlerHAL(const char *device);
    ~CrawlerHAL();

    bool setEngineForward(unsigned char powerAccell);
    bool setEngineBackward(unsigned char powerAccell);
    bool setEngineStop();
    bool setWheelFrontAngle(int angle);
    bool setWheelBackAngle(int angle);
    bool setDummySensorActive(bool active);
    void addCallbackHandler(uchar deviceId, std::function<void(ResponseData *)> callback);
    bool IMUCalibrate();
    bool IMUSetSamplingPeriod(uint16_t period);
    
    static IMUData *parseData_IMU(ResponseData *p);
    static GPSData *parseData_GPS(ResponseData *p);
};



#endif
