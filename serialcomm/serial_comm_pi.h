#ifndef _SERIAL_COMMUNICATION_PI
#define _SERIAL_COMMUNICATION_PI

#define SERIAL_BOUND_RATE 115200
#define SERIAL_WAIT_DELAY_ms 2
#define RCV_BUFFER_SIZE 100
#define SND_BUFFER_SIZE 100

#define MSG_START 32
#define MSG_END 31

#define RCV_RESP_NO_DATA 0
#define RCV_RESP_VALID 1
#define RCV_RESP_INVALID 2

#define PROTOCOL_FRAME_TYPE_DATA 1
#define PROTOCOL_FRAME_TYPE_ACK 2
#define PROTOCOL_FRAME_TYPE_DATA_LIST 3

#define PROTOCOL_ACK 1
#define PROTOCOL_NACK 2

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

//#define DEBUG 1

class ISerialCommunication
{
public:
    virtual bool receiveData() = 0;
    virtual void sendData() = 0;
    virtual bool hasData() = 0;
    virtual char read(unsigned int pos) = 0;
    virtual float readF(unsigned int pos) = 0;
    virtual void write(unsigned char val) = 0;
    virtual char *copy() = 0;
    virtual unsigned int receivedDataSize() = 0;
    virtual unsigned int sendDataSize() = 0;
    virtual void clearRcv() = 0;
    virtual void clearSnd() = 0;
};

typedef union {
  float fval;
  char bval[4];
} float_pack;


class SerialCommunication : public ISerialCommunication
{
private:
    int connFd;
    unsigned char rcvBuffer[RCV_BUFFER_SIZE];
    unsigned char sndBuffer[SND_BUFFER_SIZE];
    unsigned int rcvBufferSize;
    unsigned int sndBufferSize;

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
    SerialCommunication(const char *device)
    {
        if (wiringPiSetup() == -1)
        {
            fprintf(stderr, "unable to initialize wiringPi\n");
            exit(1);
        }

        connFd = serialOpen(device, SERIAL_BOUND_RATE);
        if (connFd == -1)
        {
            fprintf(stderr, "unable to open device %s: %s\n", device, strerror(errno));
            exit(1);
        }
    }

    ~SerialCommunication()
    {
    }

    int readByte()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(SERIAL_WAIT_DELAY_ms));
        return serialGetchar(connFd);
    }

    void clearReceiveBuffer()
    {

        while (serialDataAvail(connFd) > 0)
            serialGetchar(connFd);
        rcvBufferSize = 0;
    }

    bool receiveData() override
    {
        if (rcvBufferSize > 0) return false;

        if (serialDataAvail(connFd) <= 0) return false;
            
        rcvBufferSize = 0;
        int ch;
        int resp = RCV_RESP_INVALID;

        //printf ("** buffer size: %d\n", serialDataAvail(connFd));

        bool valid = false;

        while (!valid && serialDataAvail(connFd) > 0)
        {
            ch = readByte();
            if (ch == MSG_START)
                valid = true;
#ifdef DEBUG              
            else
                printf("ignoring: %d\n", ch);
#endif                
        }

        if (!valid) 
            return false;

        valid = false;

        while (!valid && serialDataAvail(connFd) > 0 && rcvBufferSize < RCV_BUFFER_SIZE)
        {
            ch = readByte();
            if (ch == MSG_END)
                return true;

            rcvBuffer[rcvBufferSize++] = (unsigned char)ch;
        }

#ifdef DEBUG
        printf("RCV_RESP_INVALID [");
        for (int i = 0; i < rcvBufferSize; i++)
            printf(" %d", rcvBuffer[i]);
        printf(" ]\n");
#endif

        rcvBufferSize = 0;
        return false;
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
        serialPuts(connFd, msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(SERIAL_WAIT_DELAY_ms));
        sndBufferSize = 0;
    }

    bool hasData() override
    {
        return rcvBufferSize > 0;
    }

    char read(unsigned int pos) override
    {
        return rcvBuffer[pos];
    }

    float readF(unsigned int pos) override { 
        float_pack p;
        for (uint8_t i = 0; i < 4; i++)
            p.bval[i] = rcvBuffer[pos++];
        return p.fval;
    }

    void write(unsigned char val) override
    {
        sndBuffer[sndBufferSize++] = val;
    }

    char *copy() override
    {
        char *p = (char *)malloc(sizeof(char) * (rcvBufferSize + 1));
        memcpy(p, &rcvBuffer, rcvBufferSize);
        p[rcvBufferSize] = 0;
        return p;
    }

    unsigned int receivedDataSize() override
    {
        return rcvBufferSize;
    }

    unsigned int sendDataSize() override
    {
        return sndBufferSize;
    }

    void clearRcv() override
    {
        //clearReceiveBuffer();
        rcvBufferSize = 0;
    }

    void clearSnd() override
    {
        sndBufferSize = 0;
    }
};

#endif