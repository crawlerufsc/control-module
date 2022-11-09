
#include "timeout_controller.h"
//#include "serial_comm.h"
#include "usb_serial_comm.h"
#include "led_controller.h"
#include "actuators/test_device.h"
#include "actuators/steering_device.h"
#include "actuators/forward_servo.h"
#include "sensors/dummy_sensor.h"


#define WHEELDRIVER 2
#define DIRECTION_DRIVER_FRONT 3
#define DIRECTION_DRIVER_BACK 4
#define SENSOR_DUMMY 5

uint8_t state;

#define STATE_RST 0
#define STATE_WAIT_USB 1
#define STATE_IDLE 2
#define STATE_PROCESS_RCV 3
#define STATE_PROCESS_SND 4

// Use this for USB cable Serial communication
UsbSerialCommunication comm;

// Use this for wiring Serial communication
// SerialCommunication comm(0, 1);

TimeoutController tc;
LedController led(LED_BUILTIN);
ForwardServo wheelMotor(WHEELDRIVER, 6, 4, 7, 5, 30, 36, 34, 32);
TestDevice testDevice;
SteeringDevice frontSteering(DIRECTION_DRIVER_FRONT, 3);
SteeringDevice backSteering(DIRECTION_DRIVER_BACK, 2);
DummySensor dummySensor(SENSOR_DUMMY);


uint8_t runState_RST()
{
  wheelMotor.setStop();
  return STATE_WAIT_USB;
}

uint8_t runState_WAIT_USB()
{
  if (comm.isReady())
  {
    led.off();
    comm.clear();

    return STATE_IDLE;
  }

  if (tc.checkTimeout(1000))
  {
    led.toggle();
  }

  return STATE_WAIT_USB;
}

void readSensors()
{
  dummySensor.sendData(comm);
}

uint8_t runState_IDLE()
{
  if (comm.hasData())
  {
    return STATE_PROCESS_RCV;
  } 

  comm.receiveData();

  readSensors();
  return STATE_PROCESS_SND;
}

uint8_t runState_PROCESS_RCVD()
{
  led.off();
  bool ack = false;

  ack = testDevice.readCommand(comm) || //
        wheelMotor.readCommand(comm) ||
        frontSteering.readCommand(comm) ||
        backSteering.readCommand(comm) ||
        dummySensor.readCommand(comm);

  if (ack)
    comm.ack();
  else
    comm.nack();

  comm.clearReceiveBuffer();
  return STATE_PROCESS_SND;
}

void setup()
{
  comm.initialize();
  wheelMotor.initialize();
  frontSteering.initialize();
  backSteering.initialize();
  state = STATE_RST;
}

void run_state_machine()
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
  case STATE_PROCESS_RCV:
    state = runState_PROCESS_RCVD();
    break;
  case STATE_PROCESS_SND:
    comm.sendData();
    state = STATE_IDLE;
    break;
  case STATE_IDLE:
    state = runState_IDLE();
    break;
  }
}

void loop()
{
  run_state_machine();
}