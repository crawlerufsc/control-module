#include "include/crawler_hal.h"
#include "../serialcomm/serial_link.h"

char *CrawlerHAL::allocBuffer(int size)
{
    return (char *)malloc(sizeof(char) * size);
}

CrawlerHAL::CrawlerHAL()
{
    comm = new SerialLink("/dev/ttyUSB0");
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

bool CrawlerHAL::setWheelFrontAngle(int angle)
{
#ifdef DEBUG
    printf("setWheelFront(): driver: %d, angle: %d\n", FRONT_DIRECTION_DRIVER, angle);
#endif
    return comm->syncRequest(FRONT_DIRECTION_DRIVER, (unsigned char)angle);
}

bool CrawlerHAL::setWheelBackAngle(int angle)
{
#ifdef DEBUG
    printf("setWheelBack(): driver: %d, angle: %d\n", BACK_DIRECTION_DRIVER, angle);
#endif
    return comm->syncRequest(BACK_DIRECTION_DRIVER, (unsigned char)angle);
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

IMUData *CrawlerHAL::parseData_IMU(ResponseData *p)
{
    uint8_t size = p->read(0);

    IMUData *imu = new IMUData();
    int pos = 1;
    if (pos < size)
        imu->accX = p->readF(pos);
    pos += 4;
    if (pos < size)
        imu->accY = p->readF(pos);
    pos += 4;
    if (pos < size)
        imu->accZ = p->readF(pos);
    pos += 4;
    if (pos < size)
        imu->gyroX = p->readF(pos);
    pos += 4;
    if (pos < size)
        imu->gyroY = p->readF(pos);
    pos += 4;
    if (pos < size)
        imu->gyroZ = p->readF(pos);
    pos += 4;
    if (pos < size)
        imu->angleX = p->readF(pos);
    pos += 4;
    if (pos < size)
        imu->angleY = p->readF(pos);
    pos += 4;
    if (pos < size)
        imu->angleZ = p->readF(pos);
    pos += 4;
    if (pos < size)
        imu->accAngleX = p->readF(pos);
    pos += 4;
    if (pos < size)
        imu->accAngleY = p->readF(pos);
    return imu;
}
GPSData *parseData_GPS(ResponseData *p)
{
    uint8_t size = p->read(0);
    GPSData *gps = new GPSData();
    int pos = 1;
    if (pos < size)
        gps->lat = p->readF(pos);
    pos += 4;
    if (pos < size)
        gps->lon = p->readF(pos);
    return gps;
}
