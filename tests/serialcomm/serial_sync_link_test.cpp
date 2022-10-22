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

        if (this->response == RCV_RESP_VALID) {           
            rcvbuff[1] = 1; // ack
        } else {
            rcvbuff[1] = 2; // nack
        }
        rcvSize = 2;
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
    void clearRcv() override
    {
        rcvSize = 0;
    }
};

TEST(SyncLinkCommuncation, DummySerialSyncSendReceiveAck)
{
    ISerialCommunication *comm = new DummySerialCommunication();
    ((DummySerialCommunication *)comm)->testSetReceiveDataResponse(RCV_RESP_VALID);
    SerialSyncLink *link = new SerialSyncLink(comm);

    ResponseData *resp = link->request(21, 5, 10, 11, 12, 14, 15);
    EXPECT_EQ(2, resp->size);
    EXPECT_EQ(1, resp->data[0]);
    EXPECT_EQ(1, resp->data[1]);
}

TEST(SyncLinkCommuncation, DummySerialSyncSendReceiveNAck)
{
    ISerialCommunication *comm = new DummySerialCommunication();
    ((DummySerialCommunication *)comm)->testSetReceiveDataResponse(RCV_RESP_INVALID);
    SerialSyncLink *link = new SerialSyncLink(comm);

    ResponseData *resp = link->request(21, 5, 10, 11, 12, 14, 15);
    EXPECT_TRUE(resp == nullptr);
}