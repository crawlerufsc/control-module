#ifndef _LED_CONTROLLER_H
#define _LED_CONTROLLER_H

#include <Arduino.h>
#include <stdint.h>

class LedController
{
private:
    uint8_t pin;
    int ledState;

public:
    LedController(uint8_t pin)
    {
        this->pin = pin;
        pinMode(this->pin, OUTPUT);
        off();
    }

    void on()
    {
        ledState = HIGH;
        digitalWrite(pin, ledState);
    }
    void off()
    {
        ledState = LOW;
        digitalWrite(pin, ledState);
    }

    void toggle()
    {
        if (ledState == HIGH)
            off();
        else
            on();
    }
};

#endif
