#ifndef _ASYNC_COMMUNICATION_H
#define _ASYNC_COMMUNICATION_H

#include <SoftwareSerial.h>
#include <Arduino.h>
#include <stdint.h>

#define MSG_ACK 1
#define MSG_ERR 2
#define MSG_START 32
#define MSG_END 31

class AsyncCommunication
{
public:
    virtual void initialize() = 0;
    virtual char read(uint8_t pos) = 0;
    virtual void write(char val) = 0;
    virtual void receiveData() = 0;
    virtual void sendData() = 0;
    virtual bool hasData() = 0;

    virtual void ack() = 0;
    virtual void nack() = 0;
    virtual bool isReady() = 0;
    virtual void clearReceiveBuffer() = 0;
    virtual void clear() = 0;
};

#endif
