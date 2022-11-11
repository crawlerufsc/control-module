#ifndef _GPS_SENSOR_DEVICE_H
#define _GPS_SENSOR_DEVICE_H

#include <SoftwareSerial.h>
#include <TinyGPS.h>

class GPSS : public Sensor
{
private:
    SoftwareSerial *ss;
    TinyGPS gps;
    TimeoutController tc;

    bool readGpsData()
    {
        //bool valid = false;
        while (ss->available())
        {
            gps.encode(ss->read());
            delay(20);
        }
        //return valid;
        return true;
    }

public:
    GPSS(uint8_t deviceCode) : Sensor(deviceCode)
    {
    }

    ~GPSS() {}

    void initialize() override
    {
        ss = new SoftwareSerial(2,3);
        ss->begin(4800);
        ss->listen();
    }

    bool publishData(AsyncCommunication &comm) override
    {
        if (!tc.checkTimeout(1000)) return false;

        if (!readGpsData())
            return false;

        float lat = 0.0, lon = 0.0;
        unsigned long age = 0.0;
        gps.f_get_position(&lat, &lon, &age);

        if (TinyGPS::GPS_INVALID_F_ANGLE)
        {
            lat = 0.0;
            lon = 0.0;
        }

        comm.write(getDeviceCode());
        comm.write(8);
        comm.writeF(lat);
        comm.writeF(lon);

        return true;
    }

    bool readCommand(AsyncCommunication &comm) override
    {
        uint8_t deviceId = comm.read(2);
        return checkIsCommandToThisDevice(deviceId);
    }
};

#endif