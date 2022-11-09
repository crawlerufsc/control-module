#ifndef _DEVICE_CONTROLLER_H
#define _DEVICE_CONTROLLER_H

#include "../serialcomm/serial_link.h"

#define TEST_DEVICE 1
#define WHEELDRIVER 2
#define FRONT_DIRECTION_DRIVER 3
#define BACK_DIRECTION_DRIVER 4
#define SENSOR_DUMMY 5

#define WHEELDRIVER_STOP 1
#define WHEELDRIVER_SET_FORWARD_PW 2
#define WHEELDRIVER_SET_BACKWARD_PW 3

#define DUMMY_SENSOR_CMD_ACTIVATE 1
#define DUMMY_SENSOR_CMD_DEACTIVATE 2

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
};

#endif