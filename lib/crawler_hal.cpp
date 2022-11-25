#include "include/crawler_hal.h"
#include "../serialcomm/serial_link.h"

#define TEST_DEVICE 1
#define WHEELDRIVER 2
#define STEERING_DRIVER 3
#define STEERING_DRIVER_LEFT 1
#define STEERING_DRIVER_RIGHT 2
#define STEERING_DRIVER_CENTER 3

#define WHEELDRIVER_STOP 1
#define WHEELDRIVER_SET_FORWARD_PW 2
#define WHEELDRIVER_SET_BACKWARD_PW 3

#define DUMMY_SENSOR_CMD_ACTIVATE 1
#define DUMMY_SENSOR_CMD_DEACTIVATE 2

#define IMU_CALIBRATE 1
#define IMU_SET_SAMPLING_PERIOD 2

CrawlerHAL *CrawlerHAL::_instance = nullptr;

char *CrawlerHAL::allocBuffer(int size)
{
    return (char *)malloc(sizeof(char) * size);
}

CrawlerHAL::CrawlerHAL(const char *device)
{
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

bool CrawlerHAL::setSteeringAngle(int angle)
{
#ifdef DEBUG
    printf("setSteeringAngle(): driver: %d, angle: %d\n", STEERING_DRIVER, angle);
#endif

    if (angle < -40)
        angle = -40;

    if (angle > 40)
        angle = 40;

    if (angle < 0)
    {
        uint8_t p = -1 * angle;
        printf("comm->syncRequest(STEERING_DRIVER: %d, STEERING_DRIVER_LEFT: %d, (unsigned char)p : %d)\n", STEERING_DRIVER, STEERING_DRIVER_LEFT, (unsigned char)p);
        return comm->syncRequest(STEERING_DRIVER, STEERING_DRIVER_LEFT, (unsigned char)p);
    }
    else if (angle > 0)
        return comm->syncRequest(STEERING_DRIVER, STEERING_DRIVER_RIGHT, (unsigned char)angle);
    else
        return comm->syncRequest(STEERING_DRIVER, STEERING_DRIVER_CENTER);
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
