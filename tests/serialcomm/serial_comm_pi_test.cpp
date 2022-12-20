#include <gtest/gtest.h>
#include "../../serialcomm/serial_comm_pi.h"
#include <stdlib.h>
#include <set>

#include "../../utils/filesystem.h"

// void openVirtualSerialDevice() {
//     system("socat -d -d pty,raw,echo=0,link=/tmp/vs1 pty,raw,echo=0,link=/tmp/vs2");
// }

TEST(SerialCommuncationTst, VirtualDeviceSerialSendReceive)
{
    if (!fileExists("/tmp/vs1") || !fileExists("/tmp/vs2"))
    {
        FAIL() << "Please run\n\n"
               << "socat -d -d pty,raw,echo=0,link=/tmp/vs1 pty,raw,echo=0,link=/tmp/vs2"
               << "\n\n";
        return;
    }

    // std::thread *virtualSerial = new std::thread(openVirtualSerialDevice);
    SerialCommunication *snd = new SerialCommunication("/tmp/vs2");
    SerialCommunication *rcv = new SerialCommunication("/tmp/vs1");

    snd->write(32);
    snd->write(1);
    snd->write(PROTOCOL_FRAME_TYPE_DATA);

    for (int i = 1; i <= 10; i++)
        snd->write(i);

    snd->write(31);
    snd->sendData();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    rcv->receiveData();

    EXPECT_EQ(13, rcv->receivedDataSize());
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // delete virtualSerial;
    delete snd;
    delete rcv;
}

bool sendTestMessage(SerialCommunication *comm, int frameId)
{
    comm->write((unsigned char)frameId);
    comm->write(PROTOCOL_FRAME_TYPE_DATA);
    comm->write(1);
    comm->write(1);
    comm->sendData();

    int timeout = 2000;

    while (timeout > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (comm->receiveData() != RCV_RESP_VALID)
        {
            comm->clearRcv();
            printf("received something invalid\n");
            timeout -= 100;
            continue;
        }

        printf("received result: [%d, %d, %d]\n", comm->read(0), comm->read(1), comm->read(2));

        if (comm->read(0) == frameId && comm->read(1) == PROTOCOL_FRAME_TYPE_ACK)
        {
            return comm->read(2) == 1;
        }

        timeout -= 100;
        comm->clearRcv();
    }

    return false;
}

bool wakeUpArduino(SerialCommunication *comm)
{
    int p = 1;
    while (p < 21)
    {
        if (sendTestMessage(comm, p))
            return true;
        p++;
    }
    return false;
}

TEST(SerialCommuncationTst, ArduinoSerialSendReceive)
{
    const char *device = "/dev/ttyUSB0";
    // const char *device = "/dev/ttyS0";

    if (!fileExists(device))
    {
        std::cout << "Arduino NOT FOUND. Test skipped\n\n";
        return;
    }

    SerialCommunication *arduino = new SerialCommunication(device);

    if (!wakeUpArduino(arduino))
    {
        FAIL() << "cant communicate with arduino\n";
        return;
    }

    delete arduino;
}