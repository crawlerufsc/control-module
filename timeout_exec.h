#ifndef _TIMEOUT_EXEC_H
#define _TIMEOUT_EXEC_H

#include <stdint.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <thread>

class TimeoutExec
{
private:
    unsigned long time_ms;
    void (*callOnTimeout)();
    std::thread *timeoutThread;
    bool run;

private:
    void timeoutExec()
    {
        run = true;
        unsigned long timeStamp = 0;

        while (run)
        {
            if (timeStamp >= time_ms)
            {
                callOnTimeout();
                run = false;
                break;
            }
            timeStamp++;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

public:
    TimeoutExec(void (*callOnTimeout)(), unsigned long time_ms)
    {
        timeStamp = 0;
        this->callOnTimeout = callOnTimeout;
        this->time_ms = time_ms;
    }

    void stop()
    {
        if (this->timeoutThread == nullptr)
            return;
        cancel();
        this->timeoutThread->join();
        delete this->timeoutThread;
        this->timeoutThread = nullptr;
    }

    void reset()
    {
        stop();
        this->timeoutThread = new std::thread(&TimeoutExec::timeoutExec, this);
    }

    void cancel()
    {
        this->run = false;
    }
};

#endif