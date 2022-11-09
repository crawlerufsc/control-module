#ifndef _SERVO_DEVICE_H
#define _SERVO_DEVICE_H

#include <stdint.h>
#include <SoftwareSerial.h>

#include "device.h"
#include "serial_comm.h"
#include <Arduino.h>

// SUPPORTED COMMANDS
#define WHEELDRIVER_STOP 1
#define WHEELDRIVER_SET_FORWARD_PW 2
#define WHEELDRIVER_SET_BACKWARD_PW 3

class ServoDevice : public Device {
private:
  uint8_t pwmDirectPin;
  uint8_t pwmReversePin;
  uint8_t enDirectPin;
  uint8_t enReversePin;
  uint8_t overloadDirectPin;
  uint8_t overloadReversePin;

  uint8_t currentPower;
  bool headingForward;

public:
  ServoDevice(uint8_t deviceCode, uint8_t pwmDirectPin, uint8_t pwmReversePin, uint8_t enDirectPin, uint8_t enReversePin,
              uint8_t overloadDirectPin, uint8_t overloadReversePin)
    : Device(deviceCode) {

    this->pwmDirectPin = pwmDirectPin;
    this->pwmReversePin = pwmReversePin;
    this->enDirectPin = enDirectPin;
    this->enReversePin = enReversePin;
    this->overloadDirectPin = overloadDirectPin;
    this->overloadReversePin = overloadReversePin;
    this->currentPower = 0;
  }

  ~ServoDevice() {}

  void initialize() {
    pinMode(this->pwmDirectPin, OUTPUT);
    pinMode(this->pwmReversePin, OUTPUT);
    pinMode(this->enDirectPin, OUTPUT);
    pinMode(this->enReversePin, OUTPUT);
    pinMode(this->overloadDirectPin, INPUT);
    pinMode(this->overloadReversePin, INPUT);
    
    digitalWrite(this->enDirectPin, HIGH);
    digitalWrite(this->enReversePin, HIGH);
  }

  void setStop() {
    this->currentPower = 0;
    analogWrite(pwmDirectPin, 0);
    analogWrite(pwmReversePin, 0);
  }

  void setForward() {
    setStop();
    this->headingForward = true;
  }

  void setBackward() {
    setStop();
    this->headingForward = false;
  }

  void setPower(uint8_t power) {
    uint8_t p = power;
    if (p > 255) p = 255;

    if (this->headingForward)
      analogWrite(pwmDirectPin, p);
    else
      analogWrite(pwmReversePin, p);
  }

  bool readCommand(AsyncCommunication &comm) {
    uint8_t deviceId = comm.read(2);

    if (!checkIsCommandToThisDevice(deviceId)) return false;

    uint8_t cmd = comm.read(3);
    uint8_t powerValue = comm.read(4);

    switch (cmd) {
      case WHEELDRIVER_STOP:
        setStop();
        break;
      case WHEELDRIVER_SET_FORWARD_PW:
        setForward();
        if (powerValue > 0)
          setPower(powerValue);
        break;
      case WHEELDRIVER_SET_BACKWARD_PW:
        setBackward();
        if (powerValue > 0)
          setPower(powerValue);
        break;
      default:
        return false;
    }

    return true;
  }
};

#endif