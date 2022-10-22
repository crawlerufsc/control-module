#ifndef _SERIAL_SYNC_LINK_H
#define _SERIAL_SYNC_LINK_H

#include "serial_comm_pi.h"
#include <cstdarg>

#define ACK_TIMEOUT_ms 50
#define REQUEST_TIMEOUT_ms 1000

class ResponseData
{
public:
    char *data;
    unsigned int size;
};

class SerialSyncLink
{
private:
    ISerialCommunication *comm;
    unsigned char lastFrame;
    unsigned int time_ms;
    bool run;

    unsigned char computeNextFrameId()
    {
        if (lastFrame == 254)
        {
            return 1;
        }
        return ++lastFrame;
    }

    bool ackReceived()
    {
        int ack_time_ms = 0;
        while (ack_time_ms < ACK_TIMEOUT_ms && time_ms < REQUEST_TIMEOUT_ms)
        {
            switch (comm->receiveData())
            {
            case RCV_RESP_NO_DATA:
                std::this_thread::sleep_for(std::chrono::milliseconds(SERIAL_WAIT_DELAY_ms));
                time_ms += SERIAL_WAIT_DELAY_ms;
                ack_time_ms += SERIAL_WAIT_DELAY_ms;
                break;
            case RCV_RESP_INVALID:
#ifdef DEBUG
                printf("received invalid data\n");
#endif
                return false;
            default:
#ifdef DEBUG
                printf("received valid data [%d, %d] x [%d, %d]\n", comm->read(0), comm->read(1), lastFrame, MSG_ACK);
#endif
                if (comm->read(0) == lastFrame //
                    && comm->read(1) == MSG_ACK)
                    return true;
            }
        }
#ifdef DEBUG
        printf("Ack timeout\n");
#endif
        return false;
    }

public:
    SerialSyncLink(const char *device)
    {
        comm = new SerialCommunication(device);
        lastFrame = 0;
    }

    SerialSyncLink(ISerialCommunication *comm)
    {
        this->comm = comm;
        lastFrame = 0;
    }

    ~SerialSyncLink()
    {
        delete comm;
    }

    ResponseData *request(int num_params, char *message)
    {
        time_ms = 0;

        while (time_ms < REQUEST_TIMEOUT_ms)
        {
            lastFrame = computeNextFrameId();
            comm->write(lastFrame);

            for (int i = 0; i < num_params; i++)
                comm->write(message[i]);

            comm->sendData();

            if (ackReceived())
            {
                ResponseData *resp = new ResponseData();
                resp->data = comm->copy();
                resp->size = comm->receivedDataSize();
                comm->clearRcv();
                return resp;
            }
#ifdef DEBUG
            printf("NO ACK!! time_ms = %d\n", time_ms);
#endif
            comm->clearRcv();
        }

#ifdef DEBUG
        printf("REQUEST TIMEOUT!!\n");
#endif
        return nullptr;
    }
};

#endif