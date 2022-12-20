#ifndef _SENSOR_DEVICE_H
#define _SENSOR_DEVICE_H

#include "../device.h"
#include "../async_comm.h"
#include "../timeout_controller.h"

class Sensor : public Device
{
private:
    unsigned long samplingPeriod;
    TimeoutController *tc;

    void setTimeoutController()
    {
        if (tc != nullptr)
            return;
        tc = new TimeoutController();
    }

    void destroyTimeoutController()
    {
        if (tc == nullptr)
            return;
        delete tc;
    }

protected:
    void setSamplingPeriod(uint16_t samplingPeriod)
    {
        this->samplingPeriod = (unsigned long)samplingPeriod;
        if (samplingPeriod > 0)
            setTimeoutController();
    }

    virtual bool isCalibrated() = 0;

    virtual void calibrationLoop() = 0;

    virtual bool timedPublishData(AsyncCommunication &comm) = 0;

public:
    Sensor(uint8_t deviceCode) : Device(deviceCode) {}

    ~Sensor()
    {
        destroyTimeoutController();
    }

    bool checkShouldSample()
    {
        if (tc == nullptr)
            return true;
        return tc->checkTimeout(samplingPeriod);
    }
    unsigned long lastExecutionTimestamp()
    {
        if (tc == nullptr)
            return 0;
            
        return tc->lastTimestamp();
    }
    bool publishData(AsyncCommunication &comm)
    {
        if (!checkShouldSample())
            return false;

        if (isCalibrated())
            return timedPublishData(comm);

        calibrationLoop();
    }
};

#endif