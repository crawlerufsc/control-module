#ifndef _SERIAL_COMMUNICATION_H
#define _SERIAL_COMMUNICATION_H

#include <SoftwareSerial.h>
#include <Arduino.h>
#include <stdint.h>

#define SERIAL_BOUND_RATE 115200
#define SERIAL_RCV_WAIT_DELAY_ms 1
#define RCV_BUFFER_SIZE 20
#define SND_BUFFER_SIZE 20

#define PROTOCOL_FRAME_TYPE_DATA 1
#define PROTOCOL_FRAME_TYPE_ACK 2
#define PROTOCOL_ACK 1
#define PROTOCOL_NACK 2

#include "async_comm.h"
#include "protocol.h"

class SerialCommunication : public AsyncCommunication
{
private:
    SoftwareSerial *serialPort;
    char rcvBuffer[RCV_BUFFER_SIZE];
    char sndBuffer[SND_BUFFER_SIZE];
    uint8_t lastFrameId;
    uint8_t rcvBufferSize;
    uint8_t sndBufferSize;

    char *buildSendMessage()
    {
        char *msg = (char *)malloc(sizeof(char) * (sndBufferSize + 3));

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

    char serialRead() {
      return serialPort->read();
      delay(SERIAL_RCV_WAIT_DELAY_ms);
    }

public:
    SerialCommunication(int rx, int tx)
    {
        serialPort = new SoftwareSerial(rx, tx);
        rcvBufferSize = 0;
        sndBufferSize = 0;
    }

    void initialize() override
    {
        serialPort->begin(SERIAL_BOUND_RATE);
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
        rcvBufferSize = 0;
        char ch;
        bool valid = false;

        serialPort->listen();
        delay(SERIAL_RCV_WAIT_DELAY_ms);
        while (serialPort->available() > 0)
        {
            if (serialRead() == MSG_START)
            {
                valid = true;
                break;
            }
        }

        if (!valid)
        {
            rcvBufferSize = 0;
            return;
        }

        valid = false;

        while (!valid && serialPort->available() > 0 && rcvBufferSize < RCV_BUFFER_SIZE)
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

        if (serialPort->availableForWrite() < sndBufferSize)
            return;

        char *msg = buildSendMessage();
        serialPort->write(msg);
        free(msg);

        sndBufferSize = 0;
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
        return true;
    }

    void clear() override
    {
        while (serialPort->available() > 0)
            serialPort->read();

        sndBufferSize = 0;
        rcvBufferSize = 0;
    }

    void clearReceiveBuffer() override {
        rcvBufferSize = 0;
    }
};

#endif
