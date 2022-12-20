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

bool CrawlerHAL::setSteeringAngle(int angle)
{
    return setSteeringAngle(angle, true, true);
}

bool CrawlerHAL::setSteeringAngleFront(int angle)
{
    return setSteeringAngle(angle, true, false);
}

bool CrawlerHAL::setSteeringAngleBack(int angle)
{
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

void CrawlerHAL::addDummySensorCallbackHandler(uchar handlerId, std::function<void(char *)> &callback)
{
    if (comm->hasHandler(SENSOR_DUMMY, handlerId))
        return;
    std::function<void(ResponseData *)> f = [&callback](ResponseData *val) -> void
    {
        callback(val->data);
    };
    comm->addHandler(SENSOR_DUMMY, handlerId, f);
}

void CrawlerHAL::addGPSCallbackHandler(uchar handlerId, std::function<void(GPSData *)> &callback)
{
    if (comm->hasHandler(SENSOR_GPS, handlerId))
        return;
    std::function<void(ResponseData *)> f = [&callback](ResponseData *val) -> void
    {
        GPSData *data = GPSData::decode(val->data);
        if (data != nullptr)
            callback(data);
    };
    comm->addHandler(SENSOR_GPS, handlerId, f);
}

void CrawlerHAL::addIMUCallbackHandler(uchar handlerId, std::function<void(IMUData *)> &callback)
{
    if (comm->hasHandler(SENSOR_IMU, handlerId))
        return;
    std::function<void(ResponseData *)> f = [&callback](ResponseData *val) -> void
    {
        IMUData *data = IMUData::decode(val->data);
        if (data != nullptr)
            callback(data);
    };
    comm->addHandler(SENSOR_IMU, handlerId, f);
}

void CrawlerHAL::addDummySensorCallbackHandler(std::function<void(char *)> &callback)
{
    addDummySensorCallbackHandler(1, callback);
}

void CrawlerHAL::addGPSCallbackHandler(std::function<void(GPSData *)> &callback)
{
    addGPSCallbackHandler(1, callback);
}

void CrawlerHAL::addIMUCallbackHandler(std::function<void(IMUData *)> &callback)
{
    addIMUCallbackHandler(1, callback);
}

void CrawlerHAL::removeIMUCallbackHandler(uchar handlerId)
{
    if (comm->hasHandler(SENSOR_IMU, handlerId))
        comm->removeHandler(SENSOR_IMU, handlerId);
}
void CrawlerHAL::removeGPSCallbackHandler(uchar handlerId)
{
    if (comm->hasHandler(SENSOR_GPS, handlerId))
        comm->removeHandler(SENSOR_GPS, handlerId);
}
void CrawlerHAL::removeDummyCallbackHandler(uchar handlerId)
{
    if (comm->hasHandler(SENSOR_DUMMY, handlerId))
        comm->removeHandler(SENSOR_DUMMY, handlerId);
}

void CrawlerHAL::removeIMUCallbackHandler()
{
    if (comm->hasHandler(SENSOR_IMU, 1))
        comm->removeHandler(SENSOR_IMU, 1);
}
void CrawlerHAL::removeGPSCallbackHandler()
{
    if (comm->hasHandler(SENSOR_GPS, 1))
        comm->removeHandler(SENSOR_GPS, 1);
}
void CrawlerHAL::removeDummyCallbackHandler()
{
    if (comm->hasHandler(SENSOR_DUMMY, 1))
        comm->removeHandler(SENSOR_DUMMY, 1);
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
