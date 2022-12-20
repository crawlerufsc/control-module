#ifndef _DEVICE_HAL_H
#define _DEVICE_HAL_H

#include "imu_data.h"
#include "gps_data.h"
#include "comm_types.h"

#include <functional>

#define SENSOR_DUMMY 100
#define SENSOR_IMU 101
#define SENSOR_GPS 102

class CrawlerHAL
{

private:
    const char *device;
    ISerialLink *comm;
    char *allocBuffer(int size);

    CrawlerHAL(const CrawlerHAL &) = delete;
    CrawlerHAL(const char *device);

    bool setSteeringAngle(int angle, bool front, bool back);

public:
    static CrawlerHAL *_instance;

    static bool initialize(const char *device);

    static CrawlerHAL *getInstance()
    {
        return CrawlerHAL::_instance;
    };

    ~CrawlerHAL();

    bool reset();
    bool setEngineForward(unsigned char powerAccell);
    bool setEngineBackward(unsigned char powerAccell);
    bool setEngineStop();
    bool setSteeringAngle(int angle);
    bool setSteeringAngleFront(int angle);
    bool setSteeringAngleBack(int angle);
    bool setDummySensorActive(bool active);

    void addDummySensorCallbackHandler(uchar handlerId, std::function<void(char *)> &callback);
    void addGPSCallbackHandler(uchar handlerId, std::function<void(GPSData *)> &callback);
    void addIMUCallbackHandler(uchar handlerId, std::function<void(IMUData *)> &callback);

    void addDummySensorCallbackHandler(std::function<void(char *)> &callback);
    void addGPSCallbackHandler(std::function<void(GPSData *)> &callback);
    void addIMUCallbackHandler(std::function<void(IMUData *)> &callback);

    void removeGPSCallbackHandler(uchar handlerId);
    void removeDummyCallbackHandler(uchar handlerId);
    void removeIMUCallbackHandler(uchar handlerId);

    void removeIMUCallbackHandler();
    void removeGPSCallbackHandler();
    void removeDummyCallbackHandler();

    bool IMUCalibrate();
    bool IMUSetSamplingPeriod(uint16_t period);
    bool deviceExists();
};

#endif
