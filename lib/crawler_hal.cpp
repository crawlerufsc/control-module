#include "include/crawler_hal.h"
#include "../serialcomm/serial_link.h"
#include "../utils/filesystem.h"

#define TEST_DEVICE 1
#define WHEELDRIVER 2
#define STEERING_DRIVER 3
#define STEERING_DRIVER_LEFT 1
#define STEERING_DRIVER_RIGHT 2
#define STEERING_DRIVER_CENTER 3
#define STEERING_FRONT_DRIVER_LEFT 11
#define STEERING_FRONT_DRIVER_RIGHT 12
#define STEERING_FRONT_DRIVER_CENTER 13
#define STEERING_BACK_DRIVER_LEFT 21
#define STEERING_BACK_DRIVER_RIGHT 22
#define STEERING_BACK_DRIVER_CENTER 23

#define WHEELDRIVER_STOP 1
#define WHEELDRIVER_SET_FORWARD_PW 2
#define WHEELDRIVER_SET_BACKWARD_PW 3

#define DUMMY_SENSOR_CMD_ACTIVATE 1
#define DUMMY_SENSOR_CMD_DEACTIVATE 2

#define IMU_CALIBRATE 1
#define IMU_SET_SAMPLING_PERIOD 2

#define RESET_CMD 254

CrawlerHAL *CrawlerHAL::_instance = nullptr;

char *CrawlerHAL::allocBuffer(int size)
{
    return (char *)malloc(sizeof(char) * size);
}

CrawlerHAL::CrawlerHAL(const char *device)
{
    this->device = device;
    comm = new SerialLink(device);
}

CrawlerHAL::~CrawlerHAL()
{
    delete comm;
}

bool CrawlerHAL::setEngineForward(unsigned char powerAccell)
{
    if (powerAccell == 0)
        return setEngineStop();

#ifdef DEBUG
    printf("setEngineForward(): driver: %d, cmd: %d, accell: %d\n", WHEELDRIVER, WHEELDRIVER_SET_FORWARD_PW, powerAccell);
#endif
    return comm->syncRequest(WHEELDRIVER, WHEELDRIVER_SET_FORWARD_PW, powerAccell);
}
bool CrawlerHAL::setEngineBackward(unsigned char powerAccell)
{
    if (powerAccell == 0)
        return setEngineStop();
#ifdef DEBUG
    printf("setEngineBackward(): driver: %d, cmd: %d, accell: %d\n", WHEELDRIVER, WHEELDRIVER_SET_BACKWARD_PW, powerAccell);
#endif
    return comm->syncRequest(WHEELDRIVER, WHEELDRIVER_SET_BACKWARD_PW, powerAccell);
}

bool CrawlerHAL::setEngineStop()
{
#ifdef DEBUG
    printf("setEngineStop(): driver: %d, cmd: %d\n", WHEELDRIVER, WHEELDRIVER_STOP);
#endif
    return comm->syncRequest(WHEELDRIVER, WHEELDRIVER_STOP);
}

bool CrawlerHAL::setSteeringAngle(int angle, bool front, bool back)
{
#ifdef DEBUG
    printf("setSteeringAngle(): driver: %d, angle: %d\n", STEERING_DRIVER, angle);
#endif

    uchar cmd = 0;

    if (angle < -40)
        angle = -40;

    if (angle > 40)
        angle = 40;

    if (angle < 0)
    {
        angle = -1 * angle;
        if (front && back)
            cmd = STEERING_DRIVER_LEFT;
        else if (front)
            cmd = STEERING_FRONT_DRIVER_LEFT;
        else
            cmd = STEERING_BACK_DRIVER_LEFT;
    }
    else if (angle > 0)
    {
        if (front && back)
            cmd = STEERING_DRIVER_RIGHT;
        else if (front)
            cmd = STEERING_FRONT_DRIVER_RIGHT;
        else
            cmd = STEERING_BACK_DRIVER_RIGHT;
    }
    else
    {
        if (front && back)
            cmd = STEERING_DRIVER_CENTER;
        else if (front)
            cmd = STEERING_FRONT_DRIVER_CENTER;
        else
            cmd = STEERING_BACK_DRIVER_CENTER;
    }

    return comm->syncRequest(STEERING_DRIVER, cmd, (unsigned char)angle);
}

bool CrawlerHAL::setSteeringAngle(int angle) {
    return setSteeringAngle(angle, true, true);
}

bool CrawlerHAL::setSteeringAngleFront(int angle) {
    return setSteeringAngle(angle, true, false);
}

bool CrawlerHAL::setSteeringAngleBack(int angle) {
    return setSteeringAngle(angle, false, true);
}

bool CrawlerHAL::setDummySensorActive(bool active)
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

void CrawlerHAL::addCallbackHandler(uchar deviceId, std::function<void(ResponseData *)> callback)
{
    comm->addHandler(deviceId, callback);
}

bool CrawlerHAL::IMUCalibrate()
{
#ifdef DEBUG
    printf("IMUCalibrate(): driver: %d, cmd: %d\n", SENSOR_IMU, IMU_CALIBRATE);
#endif
    return comm->syncRequest(SENSOR_IMU, IMU_CALIBRATE);
}

bool CrawlerHAL::IMUSetSamplingPeriod(uint16_t period)
{
#ifdef DEBUG
    printf("IMUSetSamplingPeriod(): driver: %d, cmd: %d, val: %d\n", SENSOR_IMU, IMU_SET_SAMPLING_PERIOD, period);
#endif
    return comm->syncRequest(SENSOR_IMU, IMU_SET_SAMPLING_PERIOD, period);
}

void CrawlerHAL::parseData_IMU(ResponseData *p, IMUData *outp)
{
    uint8_t size = p->read(0);

    int pos = 1;
    if (pos < size)
        outp->accX = p->readF(pos);
    pos += 4;
    if (pos < size)
        outp->accY = p->readF(pos);
    pos += 4;
    if (pos < size)
        outp->accZ = p->readF(pos);
    pos += 4;
    if (pos < size)
        outp->gyroX = p->readF(pos);
    pos += 4;
    if (pos < size)
        outp->gyroY = p->readF(pos);
    pos += 4;
    if (pos < size)
        outp->gyroZ = p->readF(pos);
    pos += 4;
    if (pos < size)
        outp->angleX = p->readF(pos);
    pos += 4;
    if (pos < size)
        outp->angleY = p->readF(pos);
    pos += 4;
    if (pos < size)
        outp->angleZ = p->readF(pos);
    pos += 4;
    if (pos < size)
        outp->accAngleX = p->readF(pos);
    pos += 4;
    if (pos < size)
        outp->accAngleY = p->readF(pos);
}
void CrawlerHAL::parseData_GPS(ResponseData *p, GPSData *outp)
{
    uint8_t size = p->read(0);

    int pos = 1;

    if (pos < size)
        outp->lat = p->readF(pos);

    pos += 4;

    if (pos < size)
        outp->lon = p->readF(pos);
}
bool CrawlerHAL::reset()
{
#ifdef DEBUG
    printf("reset()\n");
#endif
    return comm->syncRequest(RESET_CMD);
}

bool CrawlerHAL::initialize(const char *device)
{
    if (!fileExists(device))
        return false;

    if (CrawlerHAL::_instance != nullptr)
        delete CrawlerHAL::_instance;

    CrawlerHAL::_instance = new CrawlerHAL(device);

    return true;
}

bool CrawlerHAL::deviceExists()
{
    return fileExists(this->device);
}
