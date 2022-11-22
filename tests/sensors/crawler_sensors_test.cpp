#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include "../../utils/filesystem.h"
#include "../../lib/include/imu_data.h"
#include "../../lib/include/crawler_hal.h"

//#define DEVICE "/dev/ttyACM0"
#define DEVICE "/dev/ttyUSB0"

void printRawData(const char *sensorName, ResponseData *p)
{
    if (p == nullptr || p->data == nullptr)
    {
        printf("%s received null data!!\n", sensorName);
        return;
    }

    printf("%s received data. Size: %d [", sensorName, p->size);
    for (int i = 0; i < p->size; i++)
        printf(" %d", p->data[i]);
    printf(" ]\n");
}

void dummySensorData(ResponseData *p)
{
    if (p->deviceId != SENSOR_DUMMY)
    {
        printf("error, shouldn't have called for another deviceId");
        exit(1);
    }
    printRawData("DUMMY SENSOR", p);
}

void imuSensorData(ResponseData *p)
{
    if (p->deviceId != SENSOR_IMU)
    {
        printf("error, shouldn't have called for another deviceId");
        exit(1);
    }

    printf("ok 1\n");
    printRawData("IMU SENSOR", p);

    IMUData *data = CrawlerHAL::parseData_IMU(p);

    printf("IMU Data:\n");
    printf("acc [%f, %f, %f]\n", data->accX, data->accY, data->accZ);
    printf("gyro [%f, %f, %f]\n", data->gyroX, data->gyroY, data->gyroZ);
}
void gpsSensorData(ResponseData *p)
{
    if (p->deviceId != SENSOR_GPS)
    {
        printf("error, shouldn't have called for another deviceId");
        exit(1);
    }

    printf("ok 1\n");
    printRawData("GPS SENSOR", p);

    // printf("LAT = %f\n", readF(p, 1));
    // printf("LON = %f\n", readF(p, 5));
}
int main(int argc, char **argv)
{
    if (!fileExists(DEVICE))
    {
        printf("%s not found", DEVICE);
        return 1;
    }

    CrawlerHAL hal(DEVICE);
    bool run = true;
    bool stop = false;
    bool lastAck = false;

    hal.addCallbackHandler(SENSOR_DUMMY, dummySensorData);
    hal.addCallbackHandler(SENSOR_IMU, imuSensorData);
    hal.addCallbackHandler(SENSOR_GPS, gpsSensorData);

    // if (!hal.IMUSetSamplingPeriod(5000))
    //     printf ("error setting sample period to 2000\n");

    // while (!hal.setDummySensorActive(true))
    // {
    //     printf("setDummySensor ACTIVE NACK. Retrying\n");
    //     std::this_thread::sleep_for(std::chrono::milliseconds(1));
    // }

    // sleep(1);
    // while (!hal.setDummySensorActive(false))
    // {
    //     printf("setDummySensor INACTIVE NACK. Retrying\n");
    //     sleep(1);
    // }

        // if (!hal.IMUCalibrate())
        //     printf("error callibrating\n");

    while (1)
    {
        sleep(1);
    }

    return 0;
}