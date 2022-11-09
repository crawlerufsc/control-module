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

class ForwardServo : public Device
{
private:
  uint8_t pwmDirectPinFront;
  uint8_t pwmDirectPinBack;
  uint8_t pwmReversePinFront;
  uint8_t pwmReversePinBack;
  uint8_t enDirectPinFront;
  uint8_t enDirectPinBack;
  uint8_t enReversePinFront;
  uint8_t enReversePinBack;
  uint8_t currentPower;
  bool headingForward;

public:
  ForwardServo(uint8_t deviceCode,         //
               uint8_t pwmDirectPinFront,  //
               uint8_t pwmDirectPinBack,   //
               uint8_t pwmReversePinFront, //
               uint8_t pwmReversePinBack,  //
               uint8_t enDirectPinFront,   //
               uint8_t enDirectPinBack,    //
               uint8_t enReversePinFront,  //
               uint8_t enReversePinBack)

      : Device(deviceCode)
  {

    this->pwmDirectPinFront = pwmDirectPinFront;
    this->pwmDirectPinBack = pwmDirectPinBack;
    this->pwmReversePinFront = pwmReversePinFront;
    this->pwmReversePinBack = pwmReversePinBack;
    this->enDirectPinFront = enDirectPinFront;
    this->enDirectPinBack = enDirectPinBack;
    this->enReversePinFront = enReversePinFront;
    this->enReversePinBack = enReversePinBack;
    this->currentPower = 0;
  }

  ~ForwardServo() {}

  void initialize()
  {
    pinMode(this->pwmDirectPinFront, OUTPUT);
    pinMode(this->pwmDirectPinBack, OUTPUT);
    pinMode(this->pwmReversePinFront, OUTPUT);
    pinMode(this->pwmReversePinBack, OUTPUT);
    pinMode(this->enDirectPinFront, OUTPUT);
    pinMode(this->enDirectPinBack, OUTPUT);
    pinMode(this->enReversePinFront, OUTPUT);
    pinMode(this->enReversePinBack, OUTPUT);

    digitalWrite(this->enDirectPinFront, HIGH);
    digitalWrite(this->enDirectPinBack, HIGH);
    digitalWrite(this->enReversePinFront, HIGH);
    digitalWrite(this->enReversePinBack, HIGH);
  }

  void setStop()
  {
    this->currentPower = 0;
    analogWrite(pwmDirectPinFront, 0);
    analogWrite(pwmDirectPinBack, 0);
    analogWrite(pwmReversePinFront, 0);
    analogWrite(pwmReversePinBack, 0);
  }

  void setForward()
  {
    setStop();
    this->headingForward = true;
  }

  void setBackward()
  {
    setStop();
    this->headingForward = false;
  }

  void setPower(uint8_t power)
  {
    uint8_t p = power;
    if (p > 255)
      p = 255;

    if (this->headingForward)
    {
      analogWrite(pwmDirectPinFront, p);
      analogWrite(pwmDirectPinBack, p);
    }
    else
    {
      analogWrite(pwmReversePinFront, p);
      analogWrite(pwmReversePinBack, p);
    }
  }

  bool readCommand(AsyncCommunication &comm)
  {
    uint8_t deviceId = comm.read(2);

    if (!checkIsCommandToThisDevice(deviceId))
      return false;

    uint8_t cmd = comm.read(3);
    uint8_t powerValue = comm.read(4);

    switch (cmd)
    {
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