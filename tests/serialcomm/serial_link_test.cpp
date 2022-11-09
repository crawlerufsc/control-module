#include <gtest/gtest.h>
#include <stdlib.h>
#include <set>
#include "../../utils/filesystem.h"
#include "../serialcomm/serial_link.h"

//#define DEBUG 1

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
    char read(unsigned int pos) override
    {
        return rcvbuff[pos];
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
};

TEST(LinkCommuncation, DummySerialSyncSendReceiveAck)
{
    return;
    ISerialCommunication *comm = new DummySerialCommunication();
    ((DummySerialCommunication *)comm)->testSetReceiveDataResponse(RCV_RESP_VALID);
    SerialLink *link = new SerialLink(comm);

    unsigned char deviceId = 7;
    bool response = link->syncRequest(deviceId, 3);
    EXPECT_TRUE(response);
}

TEST(LinkCommuncation, DummySerialSyncSendReceiveNAck)
{
    return;
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
    const char *device = "/dev/ttyACM0";

    if (!fileExists(device))
    {
        std::cout << "Arduino NOT FOUND. Test skipped\n\n";
        return;
    }

    SerialLink *link = new SerialLink(device);

    auto result_test_ack = link->syncRequest(1, 2);
    EXPECT_TRUE(result_test_ack);

    result_test_ack = link->syncRequest(1, 2, 3);
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
    const char *device = "/dev/ttyACM0";

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

    link->addHandler(1, f);
    link->asyncRequest(1, 12);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(test_ack_received);
}
