#ifndef _USB_SERIAL_COMMUNICATION_H
#define _USB_SERIAL_COMMUNICATION_H

#include <Arduino.h>
#include <stdint.h>

#include "async_comm.h"

#define SERIAL_BOUND_RATE 115200
#define SERIAL_RCV_WAIT_DELAY_ms 2
#define RCV_BUFFER_SIZE 64
#define SND_BUFFER_SIZE 64

#define MSG_ACK 1
#define MSG_ERR 2
#define MSG_START 32
#define MSG_END 31

class UsbSerialCommunication : public AsyncCommunication
{
private:
    char rcvBuffer[RCV_BUFFER_SIZE];
    char sndBuffer[SND_BUFFER_SIZE];
    uint8_t lastFrameId;
    uint8_t rcvBufferSize;
    uint8_t sndBufferSize;

    char serialRead()
    {
        waitBus();
        return Serial.read();
    }

    void waitBus()
    {
        delay(SERIAL_RCV_WAIT_DELAY_ms);
    }

public:
    UsbSerialCommunication()
    {
    }

    void initialize() override
    {
        Serial.begin(SERIAL_BOUND_RATE);
        rcvBufferSize = 0;
        sndBufferSize = 0;
    }

    char read(uint8_t pos) override
    {
        return rcvBuffer[pos];
    }

    void write(char val) override
    {
        sndBuffer[sndBufferSize++] = val;
    }

    void receiveData() override
    {
        if (rcvBufferSize > 0)
            return;

        if (Serial.available() == 0)
            return;

        rcvBufferSize = 0;
        char ch;

        bool valid = false;
        while (!valid && Serial.available() > 0)
        {
            if (serialRead() == MSG_START)
                valid = true;
        }

        if (!valid)
        {
            rcvBufferSize = 0;
            return;
        }

        valid = false;
        while (!valid && Serial.available() > 0 && rcvBufferSize < RCV_BUFFER_SIZE)
        {
            ch = serialRead();
            if (ch == MSG_END)
                valid = true;
            else
                rcvBuffer[rcvBufferSize++] = ch;
        }

        if (!valid)
            rcvBufferSize = 0;

        lastFrameId = rcvBuffer[0];
    }

    void sendData() override
    {
        if (sndBufferSize == 0)
            return;

        if (Serial.availableForWrite() < sndBufferSize)
            return;

        Serial.write(MSG_START);
        for (int i = 0; i < sndBufferSize; i++)
            Serial.write(sndBuffer[i]);
        Serial.write(MSG_END);

        sndBufferSize = 0;

        Serial.flush();
    }

    bool hasData() override
    {
        return rcvBufferSize > 0;
    }

    void ack() override
    {
        write(lastFrameId);
        write(PROTOCOL_FRAME_TYPE_ACK);
        write(MSG_ACK);
    }

    void nack() override
    {
        write(lastFrameId);
        write(PROTOCOL_FRAME_TYPE_ACK);
        write(MSG_ERR);
    }

    bool isReady() override
    {
        return (Serial);
    }

    void clear() override
    {
        while (Serial.available() > 0)
            Serial.read();

        sndBufferSize = 0;
        rcvBufferSize = 0;
    }

    void clearReceiveBuffer() override
    {
        rcvBufferSize = 0;
    }
};

#endif
