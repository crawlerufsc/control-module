#ifndef _SERIAL_LINK_H
#define _SERIAL_LINK_H

#include "serial_comm_pi.h"
#include "../lib/include/crawler_hal.h"
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
        // printf("isAck?: %d\n", frameAck && this->frameId == frameId);
#endif
        return frameAck && this->frameId == frameId;
    }
};


class SerialLink: public ISerialLink
{
private:
    ISerialCommunication *comm;
    std::thread *rcvThread;
    std::mutex commMtx;
    AckWait requestAckWaitCheck;
    std::map<uchar, std::function<void(ResponseData *)>> *handlers;
    std::queue<ResponseData *> rcvFramesQueue;

    bool run;

    void initialize();
    void lock();
    void unlock();
    int wait();
    void rcvThreadHandlerValid();
    void rcvThreadHandler();
    void executeCallbackForMessageData(ResponseData *rcvMsg);
    void printRawData(const char *sensorName, ResponseData *p);
    void processListData(ResponseData *rcvMsg);
    void processData(ResponseData *rcvMsg);
    uchar *allocBuffer(int size);
    void request(int num_params, uchar *payload);
    bool syncRequest(int num_params, uchar *payload);

public:
    SerialLink(ISerialCommunication *comm);
    SerialLink(const char *device);

    ~SerialLink();
    void addHandler(uchar deviceId, std::function<void(ResponseData *)> &func) override;
    bool syncRequest(uchar deviceId) override;
    bool syncRequest(uchar deviceId, uchar val1) override;
    bool syncRequest(int deviceId, uchar val1, uchar val2) override;
    bool syncRequest(int deviceId, uchar val1, uint16_t val2) override;
    bool syncRequest(int deviceId, uchar val1, uchar val2, uchar val3) override;
    void asyncRequest(uchar deviceId) override;
    void asyncRequest(uchar deviceId, uchar val1) override;
    void asyncRequest(int deviceId, uchar val1, uchar val2) override;
    void asyncRequest(int deviceId, uchar val1, uchar val2, uchar val3) override;
};

#endif