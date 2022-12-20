#include <gtest/gtest.h>
#include <stdlib.h>
#include <set>
#include <iostream>
#include "../../utils/filesystem.h"
#include "../serialcomm/serial_link.h"

// #define DEBUG 1

class DummySerialCommunication : public ISerialCommunication
{
private:
    unsigned int response;
    char sndbuff[20];
    char rcvbuff[20];
    int rcvSize;
    int sndSize;

public:
    DummySerialCommunication()
    {
        rcvSize = 0;
        sndSize = 0;
    }

    void testSetReceiveDataResponse(unsigned int response)
    {
        this->response = response;
    }

    bool receiveData() override
    {
        rcvbuff[0] = 1; // frameId
        rcvbuff[1] = PROTOCOL_FRAME_TYPE_ACK;

        if (this->response == RCV_RESP_VALID)
        {
            rcvbuff[2] = PROTOCOL_ACK; // ack
        }
        else
        {
            rcvbuff[2] = PROTOCOL_NACK; // nack
        }
        rcvSize = 3;
        return this->response == RCV_RESP_VALID;
    }
    void sendData() override
    {
        sndSize = 0;
    }

    bool hasData() override
    {
        return rcvSize > 0;
    }
    int readByte() override
    {
        return 0;
    }
    char read(unsigned int pos) override
    {
        return rcvbuff[pos];
    }
    float readF(unsigned int pos) override
    {
        floatp p;
        for (uint8_t i = 0; i < 4; i++)
            p.bval[i] = rcvbuff[pos++];
        return p.fval;
    }
    uint16_t readInt16(unsigned int pos)
    {
        uint16p p;
        p.bval[0] = rcvbuff[pos++];
        p.bval[1] = rcvbuff[pos++];
        return p.val;
    }

    void writeInt16(uint16_t val)
    {
        uint16p p;
        p.val = val;
        write(p.bval[0]);
        write(p.bval[1]);
    }
    void write(unsigned char val) override
    {
        sndbuff[sndSize++] = val;
    }
    char *copy() override
    {
        char *p = (char *)malloc(sizeof(char) * (rcvSize + 1));
        memcpy(p, &rcvbuff, rcvSize + 1);
        p[rcvSize] = 0;
        return p;
    }
    unsigned int receivedDataSize() override
    {
        return rcvSize;
    }
    unsigned int sendDataSize() override
    {
        return sndSize;
    }
    void clearRcv() override
    {
        rcvSize = 0;
    }
    void clearSnd() override
    {
        sndSize = 0;
    }
    void clearReceiveBuffer() override
    {
        rcvSize = 0;
    }
};

TEST(LinkCommuncation, DummySerialSyncSendReceiveAck)
{
    ISerialCommunication *comm = new DummySerialCommunication();
    ((DummySerialCommunication *)comm)->testSetReceiveDataResponse(RCV_RESP_VALID);
    SerialLink *link = new SerialLink(comm);

    unsigned char deviceId = 7;
    bool response = link->syncRequest(deviceId, 3);
    EXPECT_TRUE(response);
}

TEST(LinkCommuncation, DummySerialSyncSendReceiveNAck)
{
    ISerialCommunication *comm = new DummySerialCommunication();
    ((DummySerialCommunication *)comm)->testSetReceiveDataResponse(RCV_RESP_INVALID);
    SerialLink *link = new SerialLink(comm);

    unsigned char deviceId = 7;
    bool response = link->syncRequest(deviceId, 3);
    EXPECT_FALSE(response);
}

TEST(LinkCommuncation, ArduinoSendReceiveAck)
{
    return;
    const char *device = "/dev/ttyUSB0";

    if (!fileExists(device))
    {
        std::cout << "Arduino NOT FOUND. Test skipped\n\n";
        return;
    }

    SerialLink *link = new SerialLink(device);

    auto result_test_ack = link->syncRequest(1, 2);
    EXPECT_TRUE(result_test_ack);

    result_test_ack = link->syncRequest(1, 2, (uchar)3);
    EXPECT_TRUE(result_test_ack);

    result_test_ack = link->syncRequest(1, 2, 3, 4);
    EXPECT_TRUE(result_test_ack);
}

static bool test_ack_received;

void asyncTestReceiveAck(ResponseData *data)
{
    std::cout << "async Received called\n";
    test_ack_received = true;
    EXPECT_EQ(1, data->deviceId);
}

TEST(LinkCommuncation, ArduinoSendReceiveAsync)
{
    const char *device = "/dev/ttyUSB0";

    if (!fileExists(device))
    {
        std::cout << "Arduino NOT FOUND. Test skipped\n\n";
        return;
    }

    SerialLink *link = new SerialLink(device);

    test_ack_received = false;

    std::function<void(ResponseData *)> f(asyncTestReceiveAck);
    // link->addHandler(1, f);

    auto result_test_ack = link->syncRequest(1, 2);
    EXPECT_TRUE(result_test_ack);

    link->addHandler(1, 1, f);
    link->asyncRequest(1, 12);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(test_ack_received);
}

class SerialLinkTestHandler : public SerialLink
{
public:
    SerialLinkTestHandler(ISerialCommunication *comm) : SerialLink(comm)
    {
    }

    bool checkHandlerAdded(uchar deviceId, uchar handlerId)
    {

        if ((*this->handlers).find(deviceId) == (*this->handlers).end())
            return false;

        std::vector<SerialLinkResponseCallback *> *vector = (*this->handlers)[deviceId];
        if (vector->size() == 0)
            return false;
        std::cout << "found " << vector->size() << " handlers for device " << (int)deviceId << "\n";

        for (auto it = vector->begin(); it != vector->end(); it++)
        {
            SerialLinkResponseCallback *p = *it;
            if (p == nullptr) {
                std::cout << "callback is null, but shouldnt be\n";
                continue;
            }

            if (p->id == handlerId)
                return true;
        }

        return false;
    }

    void testDataToHandlers(uchar deviceId)
    {
        ResponseData *p = new ResponseData();
        p->deviceId = deviceId;
        p->frameId = 11;
        p->frameType = 1;
        p->data = (char *)malloc(sizeof(char) * 100);
        for (int i = 0; i < 100; i++)
            p->data[i] = i + 1;

        processData(p);
    }
};

TEST(LinkCommuncation, HandlerAddRemoveTest)
{
    ISerialCommunication *comm = new DummySerialCommunication();
    ((DummySerialCommunication *)comm)->testSetReceiveDataResponse(RCV_RESP_VALID);

    auto link = new SerialLinkTestHandler(comm);

    std::function<void(ResponseData * p)> f = [](ResponseData *p) { //
        int i = 0;
    };

    link->addHandler(150, 5, f);

    ASSERT_TRUE(link->checkHandlerAdded(150, 5));
    link->removeHandler(150, 5);
    ASSERT_FALSE(link->checkHandlerAdded(150, 5));
    delete link;
}

TEST(LinkCommuncation, HandlerExec)
{
    ISerialCommunication *comm = new DummySerialCommunication();
    ((DummySerialCommunication *)comm)->testSetReceiveDataResponse(RCV_RESP_VALID);

    auto link = new SerialLinkTestHandler(comm);
    bool dataReceived = false;

    std::function<void(ResponseData * p)> f = [&dataReceived](ResponseData *p) { //
        dataReceived = true;

        for (int i = 0; i < 100; i++)
            ASSERT_EQ(p->data[i], i + 1);
    };

    link->addHandler(150, 5, f);
    link->testDataToHandlers(150);

    ASSERT_TRUE(dataReceived);
    delete link;
}