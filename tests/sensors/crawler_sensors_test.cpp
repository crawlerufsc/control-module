#include "../../hal/crawler_hal.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include "../../utils/filesystem.h"

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

// float readF (ResponseData *p, int pos) {
//     // uchar *f = (uchar *)malloc(sizeof(float) * sizeof(uchar));
//     // for (int i = 0; i < sizeof(float); i++)
//     //         f[i] = p->data[pos + i];

//     float value = 0.0;
//     memcpy(&value, &p[pos], sizeof(float));

//     return value;
// }
float readF(ResponseData *p, unsigned int pos)
{
    float_pack pkt;
    for (uint8_t i = 0; i < 4; i++)
        pkt.bval[i] = p->data[pos++];
    return pkt.fval;
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

    for (int i = 1, j = 1; i < 28; i += 4, j++)
    {
        printf("sensor data %d [%d]: %f\n", j, i, readF(p, i));
    }
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

    printf("LAT = %f\n", readF(p, 1));
    printf("LON = %f\n", readF(p, 5));
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
    //hal.addCallbackHandler(SENSOR_IMU, imuSensorData);
    hal.addCallbackHandler(SENSOR_GPS, gpsSensorData);

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

    while (1)
        sleep(5);

    return 0;
}