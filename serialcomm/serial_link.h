#ifndef _SERIAL_LINK_H
#define _SERIAL_LINK_H

#include "serial_comm_pi.h"
#include <cstdarg>
#include <thread>
#include <queue>
#include <condition_variable>
#include <map>
#include <functional>

#define ACK_TIMEOUT_ms 100
#define REQUEST_TIMEOUT_ms 1000
//#define DEBUG 1

typedef unsigned char uchar;

class ResponseData
{
public:
    char *data;
    unsigned int size;
    uchar frameId;
    uchar frameType;
    uchar deviceId;
};

class AckWait
{
private:
    uchar frameId;
    bool frameAck;

public:
    AckWait()
    {
        frameId = 0;
        frameAck = false;
    }
    uchar getNextAckFrameId()
    {
        if (frameId == 255)
        {
            frameId = 0;
        }
        return ++frameId;
    }

    void checkAck(ResponseData *frame)
    {
        frameAck = this->frameId == frame->frameId && //
                   frame->size > 2 &&                 //
                   frame->data[2] == PROTOCOL_ACK;
#ifdef DEBUG
        printf("check ack result: %d\n", frameAck);
#endif
    }

    bool isAck(uchar frameId)
    {
#ifdef DEBUG
        //printf("isAck?: %d\n", frameAck && this->frameId == frameId);
#endif
        return frameAck && this->frameId == frameId;
    }
};

class SerialLink
{
private:
    ISerialCommunication *comm;
    std::thread *rcvThread;
    std::mutex commMtx;
    AckWait requestAckWaitCheck;
    std::map<uchar, std::function<void(ResponseData *)>> *handlers;
    std::queue<ResponseData *> rcvFramesQueue;

    bool run;

    void initialize()
    {
        run = true;
        this->rcvThread = new std::thread(&SerialLink::rcvThreadHandler, this);
        this->handlers = new std::map<uchar, std::function<void(ResponseData *)>>();
        comm->clearRcv();
        comm->clearSnd();
    }

    void lock()
    {
        this->commMtx.lock();
    }

    void unlock()
    {
        this->commMtx.unlock();
    }

    int wait()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(SERIAL_WAIT_DELAY_ms));
        return SERIAL_WAIT_DELAY_ms;
    }

    void rcvThreadHandlerValid()
    {
        ResponseData *rcvMsg = new ResponseData();
        rcvMsg->data = comm->copy();
        rcvMsg->size = comm->receivedDataSize();
        rcvMsg->frameId = rcvMsg->data[0];
        rcvMsg->frameType = rcvMsg->data[1];
        rcvMsg->deviceId = rcvMsg->data[2];
        
#ifdef DEBUG
        printf("received valid message: frameId: %d, frameType: %d, deviceId: %d, size: %d\n", rcvMsg->frameId, rcvMsg->frameType, rcvMsg->deviceId, rcvMsg->size);
#endif
        processData(rcvMsg);
    }

    void rcvThreadHandler()
    {
        while (run)
        {
            if (comm->receiveData()) {
                rcvThreadHandlerValid();
                comm->clearRcv();
            } else
                wait();
        }
    }

    void processData(ResponseData *rcvMsg)
    {
        //        printf("data received\n");

        auto it = this->handlers->find(rcvMsg->deviceId);

        if (rcvMsg->frameType == PROTOCOL_FRAME_TYPE_ACK)
        {
            requestAckWaitCheck.checkAck(rcvMsg);
#ifdef DEBUG
            printf("data is ack\n");
#endif
        }

        if (it != this->handlers->end())
        {
#ifdef DEBUG                        
            printf("found handler for deviceId = %d\n", it->first);
#endif            
            it->second(rcvMsg);
        }

        delete rcvMsg;
    }

    uchar *allocBuffer(int size)
    {
        return (uchar *)malloc(sizeof(uchar) * size);
    }

    void request(int num_params, uchar *payload)
    {
        comm->clearSnd();
        payload[0] = requestAckWaitCheck.getNextAckFrameId();
#ifdef DEBUG
        printf("request(): frameId = %d\n", payload[0]);
#endif
        for (int i = 0; i < num_params; i++)
        {
            // printf ("writing payload[%d] = %d\n - buffer: %d\n", i, payload[i], comm->sendDataSize());
            comm->write(payload[i]);
        }

        //lock();
        comm->sendData();
        //unlock();
    }

    bool syncRequest(int num_params, uchar *payload)
    {
        unsigned int time_ms = 0, ack_time_ms = 0;

        while (time_ms < REQUEST_TIMEOUT_ms)
        {
            ack_time_ms = 0;
            request(num_params, payload);

            while (ack_time_ms < ACK_TIMEOUT_ms)
            {
                if (this->requestAckWaitCheck.isAck(payload[0]))
                    return true;

                ack_time_ms += wait();
            }
#ifdef DEBUG
            printf("syncRequest(): ACK timeout\n");
#endif
            time_ms += ack_time_ms;
        }

        return false;
    }

public:
    SerialLink(ISerialCommunication *comm)
    {
        this->comm = comm;
        initialize();
    }
    SerialLink(const char *device)
    {
        this->comm = new SerialCommunication(device);
        initialize();
    }

    ~SerialLink()
    {
        delete this->comm;
        delete this->rcvThread;
        delete this->handlers;
    }

    void addHandler(uchar deviceId, std::function<void(ResponseData *)> &func)
    {
        (*this->handlers)[deviceId] = func;
    }

    bool syncRequest(uchar deviceId, uchar val1)
    {
        uchar *payload = allocBuffer(4);
        payload[0] = 0;
        payload[1] = PROTOCOL_FRAME_TYPE_DATA;
        payload[2] = deviceId;
        payload[3] = val1;
        return syncRequest(4, payload);
    }
    bool syncRequest(int deviceId, uchar val1, uchar val2)
    {
        uchar *payload = allocBuffer(5);
        payload[0] = 0;
        payload[1] = PROTOCOL_FRAME_TYPE_DATA;
        payload[2] = deviceId;
        payload[3] = val1;
        payload[4] = val2;
        return syncRequest(5, payload);
    }
    bool syncRequest(int deviceId, uchar val1, uchar val2, uchar val3)
    {
        uchar *payload = allocBuffer(6);
        payload[0] = 0;
        payload[1] = PROTOCOL_FRAME_TYPE_DATA;
        payload[2] = deviceId;
        payload[3] = val1;
        payload[4] = val2;
        payload[5] = val3;
        return syncRequest(6, payload);
    }

    void asyncRequest(uchar deviceId, uchar val1)
    {
        uchar *payload = allocBuffer(4);
        payload[0] = 0;
        payload[1] = PROTOCOL_FRAME_TYPE_DATA;
        payload[2] = deviceId;
        payload[3] = val1;
        request(4, payload);
    }
    void asyncRequest(int deviceId, uchar val1, uchar val2)
    {
        uchar *payload = allocBuffer(5);
        payload[0] = 0;
        payload[1] = PROTOCOL_FRAME_TYPE_DATA;
        payload[2] = deviceId;
        payload[3] = val1;
        payload[4] = val2;
        request(5, payload);
    }
    void asyncRequest(int deviceId, uchar val1, uchar val2, uchar val3)
    {
        uchar *payload = allocBuffer(6);
        payload[0] = 0;
        payload[1] = PROTOCOL_FRAME_TYPE_DATA;
        payload[2] = deviceId;
        payload[3] = val1;
        payload[4] = val2;
        payload[5] = val3;
        request(6, payload);
    }
};

#endif