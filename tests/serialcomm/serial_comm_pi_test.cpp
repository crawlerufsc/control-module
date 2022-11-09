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
    comm->write(frameId);
    comm->write(PROTOCOL_FRAME_TYPE_DATA);
    comm->write(1);
    comm->write(1);
    comm->sendData();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (comm->receiveData() == RCV_RESP_VALID)
    {
        printf("received result: [%d, %d]\n", comm->read(0), comm->read(1));
    }

    bool p = (comm->read(0) == frameId                    //
              && comm->read(1) == PROTOCOL_FRAME_TYPE_ACK //
              && comm->read(2) == 1                       //
    );
    comm->clearRcv();
    return p;
}

bool wakeUpArduino(SerialCommunication *comm)
{
    int p = 0;
    while (p < 20)
    {
        if (sendTestMessage(comm, 1))
            return true;
        p++;
    }
    return false;
}

TEST(SerialCommuncationTst, ArduinoSerialSendReceive)
{
    const char *device = "/dev/ttyACM0";
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

    int deviceId = 1;
    int frameId = 23;
    int command = 1;

    arduino->write(frameId);
    arduino->write(deviceId);
    arduino->write(PROTOCOL_FRAME_TYPE_DATA);
    arduino->write(command);
    // payload
    arduino->write(10);
    arduino->write(11);
    arduino->write(12);
    arduino->sendData();
    // sleep(2);

    // int p = 0;
    // while (p < 20)
    // {
    //     if (arduino->receiveData() == RCV_RESP_VALID)
    //         break;
    //     p++;
    // }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    arduino->receiveData();
    EXPECT_EQ(3, arduino->receivedDataSize());
    EXPECT_EQ(frameId, arduino->read(0));
    EXPECT_EQ(PROTOCOL_FRAME_TYPE_ACK, arduino->read(1));
    EXPECT_EQ(1, arduino->read(2)); // ack

    delete arduino;
}