#include "servo_device.h"
#include "timeout_controller.h"
//#include "serial_comm.h"
#include "usb_serial_comm.h"
#include "led_controller.h"
#include "test_device.h"

#define FRONT_WHEELDRIVER 2
#define BACK_WHEELDRIVER 3

uint8_t state;

#define STATE_RST 0
#define STATE_WAIT_USB 1
#define STATE_IDLE 2

// Use this for USB cable Serial communication
UsbSerialCommunication comm;

// Use this for wiring Serial communication
//SerialCommunication comm(0, 1);

TimeoutController tc;
LedController led(LED_BUILTIN);
ServoDevice backWheels(BACK_WHEELDRIVER, 11, 10, 12, 13, 9, 8);
ServoDevice frontWheels(FRONT_WHEELDRIVER, 6, 5, 7, 4, 3, 2);
TestDevice testDevice;

uint8_t runState_RST()
{
  led.on();
  frontWheels.setStop();
  backWheels.setStop();
  return STATE_WAIT_USB;
}

uint8_t runState_WAIT_USB()
{
  if (comm.isReady())
  {
    led.off();
    comm.clearReceiveBuffer();
    return STATE_IDLE;
  }

  if (tc.execEvery(1000))
  {
    led.toggle();
  }

  return STATE_WAIT_USB;
}

uint8_t runState_IDLE()
{
  if (!comm.hasData())
    return STATE_IDLE;

  if (testDevice.readCommand(comm))
    comm.ack();
  else if (frontWheels.readCommand(comm))
    comm.ack();
  else if (backWheels.readCommand(comm))
    comm.ack();
  else
  {
    comm.nack();
  }

  return STATE_IDLE;
}

void runStateMachine()
{
  switch (state)
  {
  default:
  case STATE_RST:
    state = runState_RST();
    break;
  case STATE_WAIT_USB:
    state = runState_WAIT_USB();
    break;
  case STATE_IDLE:
    state = runState_IDLE();
    break;
  }
}

void setup()
{
  comm.initialize();
  state = STATE_RST;
}

void loop()
{
  comm.receiveData();
  runStateMachine();
  comm.sendData();
  comm.clear();
}
