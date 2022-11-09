#include "../../hal/crawler_hal.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include "../../utils/filesystem.h"

#define DEVICE "/dev/ttyACM0"

void dummySensorData(ResponseData *p)
{
    if (p->deviceId != SENSOR_DUMMY) {
        printf ("error, shouldn't have called for another deviceId");
        exit(1);
    }
    printf("Received Data. Size: %d [", p->size);
    for (int i = 0; i < p->size; i++)
        printf(" %d", p->data[i]);
    printf (" ]\n");
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

    while (!hal.setDummySensorActive(true))
    {
        printf("setDummySensor ACTIVE NACK. Retrying\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    sleep(1);
    while (!hal.setDummySensorActive(false))
    {
        printf("setDummySensor INACTIVE NACK. Retrying\n");
        sleep(1);
    }

    sleep(2);

    return 0;
}