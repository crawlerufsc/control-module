//#define DEBUG 1

#include <gtest/gtest.h>
#include "../../serialcomm/serial_comm_pi.h"
#include <stdlib.h>
#include <set>

#include "../../utils/filesystem.h"
#include "../serialcomm/serial_sync_link.h"

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

    unsigned int receiveData() override
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
        return this->response;
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

TEST(SyncLinkCommuncation, DummySerialSyncSendReceiveAck)
{
    ISerialCommunication *comm = new DummySerialCommunication();
    ((DummySerialCommunication *)comm)->testSetReceiveDataResponse(RCV_RESP_VALID);
    SerialSyncLink *link = new SerialSyncLink(comm);

    char *p = (char *)malloc(sizeof(char) * 8);
    p[0] = 1;
    p[1] = PROTOCOL_FRAME_TYPE_DATA;
    p[2] = 5;
    p[3] = 10;
    p[4] = 11;
    p[5] = 12;
    p[6] = 14;
    p[7] = 15;

    ResponseData *resp = link->request(8, p);
    EXPECT_EQ(3, resp->size);
    EXPECT_EQ(1, resp->data[0]);
    EXPECT_EQ(PROTOCOL_FRAME_TYPE_ACK, resp->data[1]);
    EXPECT_EQ(1, resp->data[2]);
}

TEST(SyncLinkCommuncation, DummySerialSyncSendReceiveNAck)
{
    ISerialCommunication *comm = new DummySerialCommunication();
    ((DummySerialCommunication *)comm)->testSetReceiveDataResponse(RCV_RESP_INVALID);
    SerialSyncLink *link = new SerialSyncLink(comm);

    char *p = (char *)malloc(sizeof(char) * 8);
    p[0] = 1;
    p[1] = PROTOCOL_FRAME_TYPE_DATA;
    p[2] = 5;
    p[3] = 10;
    p[4] = 11;
    p[5] = 12;
    p[6] = 14;
    p[7] = 15;

    ResponseData *resp = link->request(8, p);
    EXPECT_TRUE(resp == nullptr);
}