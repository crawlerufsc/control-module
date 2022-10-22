#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <pthread.h>
#include <thread>
#include <chrono>

using namespace std;

class SerialComm
{
private:
    int deviceId;
    char rcvBuffer[20];
    char sndBuffer[20];
    unsigned int rcvBufferSize;
    unsigned int sndBufferSize;

public:
    SerialComm(const char *device)
    {
        deviceId = serialOpen(device, 9600);
        if (deviceId == -1)
        {
            fprintf(stderr, "unable to open device %s: %s\n", device, strerror(errno));
            exit(1);
        }

        if (wiringPiSetup() == -1)
        {
            fprintf(stderr, "unable to initialize wiringPi\n");
            exit(1);
        }
    }

    void write(char val)
    {
        serialPutchar(deviceId, val);
    }

    void writeStr(char *val)
    {

        serialPuts(deviceId, val);
        serialFlush(deviceId);
    }

    void receiveData()
    {
        if (serialDataAvail(deviceId) > 0)
        {
            printf("new data\n");
            rcvBufferSize = 0;
            while (serialDataAvail(deviceId) > 0)
            {
                char ch = serialGetchar(deviceId);
                rcvBuffer[rcvBufferSize++] = ch;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }

    bool hasData()
    {
        return rcvBufferSize > 0;
    }

    char read(int pos)
    {
        if (rcvBufferSize == 0)
            return 0;
        return rcvBuffer[pos];
    }

    unsigned int receivedDataSize()
    {
        return rcvBufferSize;
    }

    void clearRcv()
    {
        rcvBufferSize = 0;
    }
};

pthread_t rcv;
SerialComm comm("/dev/ttyACM0");

void *rcvfunc(void *arg)
{
    while (true)
    {
        comm.receiveData();
        if (comm.hasData())
        {
            for (int i = 0; i < comm.receivedDataSize(); i++)
                printf("pos %i=%d\n", i, comm.read(i));
            comm.clearRcv();
        }

        sleep(1);
    }
}

int main()
{

    char *msg = (char *)malloc(sizeof(char) * 7);
    msg[0] = 32;
    msg[1] = 7;
    msg[2] = 1;
    msg[3] = 1;
    msg[4] = 3;
    msg[5] = 31;
    msg[6] = 0;

    pthread_create(&rcv, NULL, &rcvfunc, NULL);

    while (true)
    {
        printf("sending test command\n");
        comm.writeStr(msg);
        sleep(2);
    }
}
