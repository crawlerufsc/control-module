#ifndef _GPS_SENSOR_DEVICE_H
#define _GPS_SENSOR_DEVICE_H

#include <Arduino.h>
#include <TinyGPS.h>

class GPSS : public Sensor
{
private:
    TinyGPS gps;

    // bool readGpsData()
    // {
    //     bool valid = false;
    //     while (Serial1.available())
    //     {
    //         char data = Serial1.read();
    //         Serial.print("gps data: ");
    //         Serial.println(data);
    //         if (gps.encode(data))
    //             valid = true;
    //         delay(20);
    //     }
    //     return valid;
    // }

    bool readGpsData()
    {
        bool valid = false;
        while (Serial1.available())
        {
            valid = true;
            char ch = Serial1.read();
            gps.encode(ch);
        }

        return valid;
    }

protected:
    bool isCalibrated() override
    {
        return true;
    }

    void calibrationLoop() override
    {
    }

    bool timedPublishData(AsyncCommunication &comm) override
    {
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
        comm.write(25);
        comm.writeF(lat);
        comm.writeF(lon);
        comm.writeL(age == TinyGPS::GPS_INVALID_AGE ? 0 : age);
        comm.write(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
        comm.writeL(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
        comm.writeL(gps.altitude() == TinyGPS::GPS_INVALID_ALTITUDE ? 0 : gps.altitude());
        comm.writeF(gps.speed() == TinyGPS::GPS_INVALID_SPEED ? 0 : gps.f_speed_kmph());
        return true;
    }

public:
    GPSS(uint8_t deviceCode) : Sensor(deviceCode)
    {
    }

    ~GPSS() {}

    void initialize() override
    {
        setSamplingPeriod(1000);
        Serial1.begin(4800);
        while (!Serial1)
        {
        }
    }

    bool readCommand(AsyncCommunication &comm) override
    {
        uint8_t deviceId = comm.read(2);
        return checkIsCommandToThisDevice(deviceId);
    }
};

#endif