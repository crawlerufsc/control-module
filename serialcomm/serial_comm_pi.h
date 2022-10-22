#ifndef _SERIAL_COMMUNICATION_PI
#define _SERIAL_COMMUNICATION_PI

#define SERIAL_BOUND_RATE 115200
#define SERIAL_WAIT_DELAY_ms 10
#define RCV_BUFFER_SIZE 20
#define SND_BUFFER_SIZE 20

#define MSG_START 32
#define MSG_END 31

#define RCV_RESP_NO_DATA 0
#define RCV_RESP_VALID 1
#define RCV_RESP_INVALID 2

#define MSG_ACK 1
#define MSG_ERR 2

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
#include <mutex>
#include <sys/ioctl.h>

#define DEBUG 1

class ISerialCommunication
{
public:
    virtual unsigned int receiveData() = 0;
    virtual void sendData() = 0;
    virtual bool hasData() = 0;
    virtual char read(unsigned int pos) = 0;
    virtual void write(unsigned char val) = 0;
    virtual char *copy() = 0;
    virtual unsigned int receivedDataSize() = 0;
    virtual void clearRcv() = 0;
};

class SerialCommunication : public ISerialCommunication
{
private:
    int connFd;
    unsigned char rcvBuffer[20];
    unsigned char sndBuffer[20];
    unsigned int rcvBufferSize;
    unsigned int sndBufferSize;
    std::mutex *serialAccessMtx;

    char *buildSendMessage()
    {
        char *msg = (char *)malloc(sizeof(unsigned char) * (sndBufferSize + 3));

        msg[0] = MSG_START;
        uint8_t i = 0;
        for (i = 0; i < sndBufferSize; i++)
        {
            msg[i + 1] = sndBuffer[i];
        }
        msg[i + 1] = MSG_END;
        msg[i + 2] = 0;
        return msg;
    }

public:
    SerialCommunication(const char *device, bool threadSafe = false)
    {
        connFd = serialOpen(device, SERIAL_BOUND_RATE);
        if (connFd == -1)
        {
            fprintf(stderr, "unable to open device %s: %s\n", device, strerror(errno));
            exit(1);
        }

        if (wiringPiSetup() == -1)
        {
            fprintf(stderr, "unable to initialize wiringPi\n");
            exit(1);
        }

        if (threadSafe)
            serialAccessMtx = new std::mutex();
    }

    ~SerialCommunication()
    {
        if (serialAccessMtx != nullptr)
            delete serialAccessMtx;
    }

    void lock()
    {
        if (serialAccessMtx == nullptr)
            return;
        serialAccessMtx->lock();
    }
    void unlock()
    {
        if (serialAccessMtx == nullptr)
            return;
        serialAccessMtx->unlock();
    }

    int readByte()
    {
        int p = serialGetchar(connFd);
        std::this_thread::sleep_for(std::chrono::milliseconds(SERIAL_WAIT_DELAY_ms));
        return p;
    }

    void clearReceiveBuffer()
    {
        while (serialDataAvail(connFd) > 0)
            serialGetchar(connFd);
        rcvBufferSize = 0;
    }

    unsigned int receiveData() override
    {
        rcvBufferSize = 0;
        int ch;
        int resp = RCV_RESP_INVALID;

        lock();

        if (!serialDataAvail(connFd) > 0)
        {
            unlock();
#ifdef DEBUG
            printf("RCV_RESP_NO_DATA\n");
#endif
            return RCV_RESP_NO_DATA;
        }

        if (readByte() != MSG_START)
        {
            clearReceiveBuffer();
            unlock();
#ifdef DEBUG
            printf("RCV_RESP_INVALID\n");
#endif
            return RCV_RESP_INVALID;
        }

        while (serialDataAvail(connFd) > 0)
        {
            ch = readByte();
            if (ch == MSG_END)
            {
                unlock();
                return RCV_RESP_VALID;
            }

            rcvBuffer[rcvBufferSize++] = (unsigned char)ch;
        }
#ifdef DEBUG
        printf("RCV_RESP_INVALID [");
        for (int i = 0; i < rcvBufferSize; i++)
            printf(" %d", rcvBuffer[i]);
        printf(" ]\n");
#endif
        clearReceiveBuffer();
        unlock();
        return RCV_RESP_INVALID;
    }

    void sendData() override
    {
        if (sndBufferSize == 0)
            return;

        char *msg = buildSendMessage();
#ifdef DEBUG
        printf("sending: [");
        for (int i = 0; i < sndBufferSize + 3; i++)
        {
            printf(" %d", msg[i]);
        }
        printf("]\n");
#endif
        lock();
        serialPuts(connFd, msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(SERIAL_WAIT_DELAY_ms));
        sndBufferSize = 0;
        unlock();
    }

    bool hasData() override
    {
        lock();
        bool p = rcvBufferSize > 0;
        unlock();
        return p;
    }

    char read(unsigned int pos) override
    {
        lock();
        char ch = rcvBuffer[pos];
        unlock();
        return ch;
    }

    void write(unsigned char val) override
    {
        lock();
        sndBuffer[sndBufferSize++] = val;
        unlock();
    }

    char *copy() override
    {
        lock();
        char *p = (char *)malloc(sizeof(char) * (rcvBufferSize + 1));
        memcpy(p, &rcvBuffer, rcvBufferSize);
        p[rcvBufferSize] = 0;
        unlock();
        return p;
    }

    unsigned int receivedDataSize() override
    {
        lock();
        unsigned int p = rcvBufferSize;
        unlock();
        return p;
    }

    void clearRcv() override
    {
        clearReceiveBuffer();
    }
};

#endif