#ifndef _DEVICE_CONTROLLER_H
#define _DEVICE_CONTROLLER_H

#include "../serialcomm/serial_link.h"
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

#define DEBUG 1

class CrawlerHAL
{

private:
    SerialLink *comm;

    char *allocBuffer(int size)
    {
        return (char *)malloc(sizeof(char) * size);
    }

public:
    CrawlerHAL()
    {
        // comm = new SerialLink("/dev/ttyS0");
        comm = new SerialLink("/dev/ttyUSB0");
        // comm = new SerialLink("/dev/ttyACM0");
    }
    CrawlerHAL(const char *device)
    {
        comm = new SerialLink(device);
    }
    ~CrawlerHAL()
    {
        delete comm;
    }

    bool setEngineForward(unsigned char powerAccell)
    {
        if (powerAccell == 0)
            return setEngineStop();

#ifdef DEBUG
        printf("setEngineForward(): driver: %d, cmd: %d, accell: %d\n", WHEELDRIVER, WHEELDRIVER_SET_FORWARD_PW, powerAccell);
#endif
        return comm->syncRequest(WHEELDRIVER, WHEELDRIVER_SET_FORWARD_PW, powerAccell);
    }
    bool setEngineBackward(unsigned char powerAccell)
    {
        if (powerAccell == 0)
            return setEngineStop();
#ifdef DEBUG
        printf("setEngineBackward(): driver: %d, cmd: %d, accell: %d\n", WHEELDRIVER, WHEELDRIVER_SET_BACKWARD_PW, powerAccell);
#endif
        return comm->syncRequest(WHEELDRIVER, WHEELDRIVER_SET_BACKWARD_PW, powerAccell);
    }

    bool setEngineStop()
    {
#ifdef DEBUG
        printf("setEngineStop(): driver: %d, cmd: %d\n", WHEELDRIVER, WHEELDRIVER_STOP);
#endif
        return comm->syncRequest(WHEELDRIVER, WHEELDRIVER_STOP);
    }

    bool setWheelFrontAngle(int angle)
    {
#ifdef DEBUG
        printf("setWheelFront(): driver: %d, angle: %d\n", FRONT_DIRECTION_DRIVER, angle);
#endif
        return comm->syncRequest(FRONT_DIRECTION_DRIVER, (unsigned char)angle);
    }

    bool setWheelBackAngle(int angle)
    {
#ifdef DEBUG
        printf("setWheelBack(): driver: %d, angle: %d\n", BACK_DIRECTION_DRIVER, angle);
#endif
        return comm->syncRequest(BACK_DIRECTION_DRIVER, (unsigned char)angle);
    }

    bool setDummySensorActive(bool active)
    {

#ifdef DEBUG
        printf("setDummySensorActive(): driver: %d", SENSOR_DUMMY);
        if (active)
            printf(", active: true\n");
        else
            printf(", active: false\n");
#endif
        uchar p = DUMMY_SENSOR_CMD_ACTIVATE;
        if (!active)
            p = DUMMY_SENSOR_CMD_DEACTIVATE;

        return comm->syncRequest(SENSOR_DUMMY, p);
    }

    void addCallbackHandler(uchar deviceId, std::function<void(ResponseData *)> callback)
    {
        comm->addHandler(deviceId, callback);
    }

    bool IMUCalibrate()
    {
#ifdef DEBUG
        printf("IMUCalibrate(): driver: %d, cmd: %d\n", SENSOR_IMU, IMU_CALIBRATE);
#endif
        return comm->syncRequest(SENSOR_IMU, IMU_CALIBRATE);
    }

    bool IMUSetSamplingPeriod(uint16_t period)
    {
#ifdef DEBUG
        printf("IMUSetSamplingPeriod(): driver: %d, cmd: %d, val: %d\n", SENSOR_IMU, IMU_SET_SAMPLING_PERIOD, period);
#endif
        return comm->syncRequest(SENSOR_IMU, IMU_SET_SAMPLING_PERIOD, period);
    }

    static IMUData * parseData_IMU(ResponseData *p)
    {
        uint8_t size = p->read(0);

        IMUData *imu = new IMUData ();
        int pos = 1;
        if (pos < size) imu->accX = p->readF(pos); pos += 4;
        if (pos < size) imu->accY = p->readF(pos); pos += 4;
        if (pos < size) imu->accZ = p->readF(pos); pos += 4;
        if (pos < size) imu->gyroX = p->readF(pos); pos += 4;
        if (pos < size) imu->gyroY = p->readF(pos); pos += 4;
        if (pos < size) imu->gyroZ = p->readF(pos); pos += 4;        
        if (pos < size) imu->angleX = p->readF(pos); pos += 4;
        if (pos < size) imu->angleY = p->readF(pos); pos += 4;
        if (pos < size) imu->angleZ = p->readF(pos); pos += 4;        
        if (pos < size) imu->accAngleX = p->readF(pos); pos += 4;
        if (pos < size) imu->accAngleY = p->readF(pos); 
        return imu;
    }
        static GPSData * parseData_GPS(ResponseData *p)
    {
        uint8_t size = p->read(0);
        GPSData *gps = new GPSData ();
        int pos = 1;
        if (pos < size) gps->lat = p->readF(pos); pos += 4;
        if (pos < size) gps->lon = p->readF(pos);
        return gps;
    }
};

#endif

