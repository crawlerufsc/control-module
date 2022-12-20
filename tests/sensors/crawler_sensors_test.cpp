#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include "../../utils/filesystem.h"
#include "../../lib/include/imu_data.h"
#include "../../lib/include/crawler_hal.h"

// #define DEVICE "/dev/ttyACM0"
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

void dummySensorData(char *p)
{
    printf("DUMMY SENSOR\n");
}

void imuSensorData(IMUData *data)
{
    printf("IMU Data:\n");
    printf("acc [%f, %f, %f]\n", data->accX, data->accY, data->accZ);
    printf("gyro [%f, %f, %f]\n", data->gyroX, data->gyroY, data->gyroZ);
}
void gpsSensorData(GPSData *data)
{
}
int main(int argc, char **argv)
{
    if (!fileExists(DEVICE))
    {
        printf("%s not found", DEVICE);
        return 1;
    }

    CrawlerHAL::initialize(DEVICE);

    CrawlerHAL *hal = CrawlerHAL::getInstance();
    bool run = true;
    bool stop = false;
    bool lastAck = false;

    // hal->addDummySensorCallbackHandler(dummySensorData);
    std::function<void(GPSData *)> f = [=](GPSData *data)
    {
        printf("GPS Data:\n");
        printf ("%s\n", data->toJson().c_str());
    };

    std::function<void(IMUData *)> fIMU = [=](IMUData *data)
    {
        printf("IMU Data:\n");
        printf("acc [%f, %f, %f]\n", data->accX, data->accY, data->accZ);
        printf("gyro [%f, %f, %f]\n", data->gyroX, data->gyroY, data->gyroZ);
    };

    hal->addIMUCallbackHandler(fIMU);
    hal->addGPSCallbackHandler(f);

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