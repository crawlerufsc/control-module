#ifndef _SERIAL_COMMUNICATION_H
#define _SERIAL_COMMUNICATION_H

#include <SoftwareSerial.h>
#include <Arduino.h>
#include <stdint.h>

#define SERIAL_BOUND_RATE 115200
#define SERIAL_RCV_WAIT_DELAY_ms 1
#define RCV_BUFFER_SIZE 20
#define SND_BUFFER_SIZE 20

#include "async_comm.h"

class SerialCommunication : public AsyncCommunication
{
private:
    SoftwareSerial *serialPort;
    char rcvBuffer[RCV_BUFFER_SIZE];
    char sndBuffer[SND_BUFFER_SIZE];
    uint8_t rcvLastId;
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

public:
    SerialCommunication(uint8_t rxPin, uint8_t txPin)
    {
        serialPort = new SoftwareSerial(rxPin, txPin);
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
        char ch;
        bool valid = false;
        serialPort->listen();
        delay(SERIAL_RCV_WAIT_DELAY_ms);
        if (serialPort->available() > 0)
        {
            rcvBufferSize = 0;

            if (serialPort->read() != MSG_START)
            {
                clearReceiveBuffer();
                return;
            }

            delay(SERIAL_RCV_WAIT_DELAY_ms);
            while (serialPort->available() > 0 && rcvBufferSize < RCV_BUFFER_SIZE)
            {
                ch = serialPort->read();
                if (ch == MSG_END)
                {
                    valid = true;
                    break;
                }

                rcvBuffer[rcvBufferSize++] = ch;
                delay(SERIAL_RCV_WAIT_DELAY_ms);
            }

            clearReceiveBuffer();

            if (!valid)
            {
                rcvBufferSize = 0;
                return;
            }

            rcvLastId = rcvBuffer[0];
        }
    }

    void sendData() override
    {
        if (sndBufferSize == 0)
            return;

        char *msg = buildSendMessage();
        serialPort->write(msg);
        delay(SERIAL_RCV_WAIT_DELAY_ms);
        free(msg);
        sndBufferSize = 0;
    }
    bool hasData() override
    {
        return rcvBufferSize > 0;
    }

    void ack() override
    {
        write(rcvLastId);
        write(MSG_ACK);
    }

    void nack() override
    {
        write(rcvLastId);
        write(MSG_ERR);
    }

    bool isReady() override
    {
        // if (serialPort == nullptr)
        //     return (Serial);

        return true;
    }

    void clearReceiveBuffer() override
    {
        while (serialPort->available() > 0)
            serialPort->read();
    }

    void clear() override
    {
        sndBufferSize = 0;
        rcvBufferSize = 0;
    }
};

#endif
