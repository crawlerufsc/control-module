#ifndef _DEVICE_CONTROLLER_H
#define _DEVICE_CONTROLLER_H

#include "../serialcomm/serial_sync_link.h"

#define TEST_DEVICE 1
#define FRONT_WHEELDRIVER 2
#define BACK_WHEELDRIVER 3

#define WHEELDRIVER_STOP 1
#define WHEELDRIVER_SET_FORWARD_PW 2
#define WHEELDRIVER_SET_BACKWARD_PW 3

class CrawlerHAL
{

private:
    SerialSyncLink *comm;

    char *allocBuffer(int size)
    {
        return (char *)malloc(sizeof(char) * size);
    }

public:
    CrawlerHAL()
    {
        comm = new SerialSyncLink("/dev/ttyACM0");
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
        printf("setEngineForward(): driver: %d, cmd: %d, accell: %d\n", BACK_WHEELDRIVER, WHEELDRIVER_SET_FORWARD_PW, powerAccell);
#endif
        char *msg = allocBuffer(3);
        msg[0] = BACK_WHEELDRIVER;
        msg[1] = WHEELDRIVER_SET_FORWARD_PW;
        msg[2] = powerAccell;
        return comm->request(3, msg) != nullptr;
    }
    bool setEngineBackward(unsigned char powerAccell)
    {
        if (powerAccell == 0)
            return setEngineStop();
#ifdef DEBUG
        printf("setEngineBackward(): driver: %d, cmd: %d, accell: %d\n", BACK_WHEELDRIVER, WHEELDRIVER_SET_BACKWARD_PW, powerAccell);
#endif
        char *msg = allocBuffer(3);
        msg[0] = BACK_WHEELDRIVER;
        msg[1] = WHEELDRIVER_SET_BACKWARD_PW;
        msg[2] = powerAccell;
        return comm->request(3, msg) != nullptr;
    }

    bool setEngineStop()
    {
#ifdef DEBUG
        printf("setEngineStop(): driver: %d, cmd: %d\n", BACK_WHEELDRIVER, WHEELDRIVER_STOP);
#endif
        char *msg = allocBuffer(2);
        msg[0] = BACK_WHEELDRIVER;
        msg[1] = WHEELDRIVER_STOP;
        return comm->request(2, msg) != nullptr;
    }

    bool setWheelRight(int powerAccell)
    {
        if (powerAccell == 0)
            return setWheelStop();

#ifdef DEBUG
        printf("setWheelRight(): driver: %d, cmd: %d, accell: %d\n", FRONT_WHEELDRIVER, WHEELDRIVER_SET_FORWARD_PW, powerAccell);
#endif
        char *msg = allocBuffer(3);
        msg[0] = FRONT_WHEELDRIVER;
        msg[1] = WHEELDRIVER_SET_FORWARD_PW;
        msg[2] = powerAccell;
        return comm->request(3, msg) != nullptr;
    }

    bool setWheelLeft(int powerAccell)
    {
        if (powerAccell == 0)
            return setWheelStop();

#ifdef DEBUG
        printf("setWheelRight(): driver: %d, cmd: %d, accell: %d\n", FRONT_WHEELDRIVER, WHEELDRIVER_SET_BACKWARD_PW, powerAccell);
#endif
        char *msg = allocBuffer(3);
        msg[0] = FRONT_WHEELDRIVER;
        msg[1] = WHEELDRIVER_SET_BACKWARD_PW;
        msg[2] = powerAccell;
        return comm->request(3, msg) != nullptr;
    }

    bool setWheelStop()
    {
#ifdef DEBUG
        printf("setWheelStop(): driver: %d, cmd: %d\n", FRONT_WHEELDRIVER, WHEELDRIVER_STOP);
#endif

        char *msg = allocBuffer(2);
        msg[0] = FRONT_WHEELDRIVER;
        msg[1] = WHEELDRIVER_STOP;
        return comm->request(2, msg) != nullptr;
    }
};

#endif