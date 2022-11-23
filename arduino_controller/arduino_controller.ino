#include "timeout_controller.h"
//#include "serial_comm.h"
#include "usb_serial_comm.h"
#include "led_controller.h"
#include "actuators/dummy_device.h"
#include "actuators/double_steering_device.h"
#include "actuators/forward_servo.h"
#include "sensors/dummy_sensor.h"
#include "sensors/imu_sensor.h"
#include "sensors/gps_sensor.h"

#define WHEELDRIVER 2
#define STEERING_DRIVER 3
// #define DIRECTION_DRIVER_FRONT 3
// #define DIRECTION_DRIVER_BACK 4
#define SENSOR_DUMMY 100
#define SENSOR_IMU 101
#define SENSOR_GPS 102
#define RESET 254

uint8_t state;

#define STATE_RST 0
#define STATE_WAIT_USB 1
#define STATE_IDLE 2
#define STATE_PROCESS_RCV 3
#define STATE_PROCESS_SND_SENSOR_DATA 6

// Use this for USB cable Serial communication
UsbSerialCommunication comm;

// Use this for wiring Serial communication
// SerialCommunication comm(0, 1);

TimeoutController tc;
LedController led(LED_BUILTIN);
ForwardServo wheelMotor(WHEELDRIVER, 6, 4, 7, 5, 30, 36, 34, 32);
DoubleSteeringDevice steering (STEERING_DRIVER, 3, 2);
DummyDevice dummyDevice;
// SteeringDevice frontSteering(DIRECTION_DRIVER_FRONT, 3);
// SteeringDevice backSteering(DIRECTION_DRIVER_BACK, 2);
DummySensor dummySensor(SENSOR_DUMMY);
IMU imu(SENSOR_IMU);
GPSS gps(SENSOR_GPS);

uint8_t runState_RST()
{
  wheelMotor.setStop();
  dummySensor.initialize();
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

bool readSensors()
{
  //IMU has a lot of data 
  if (imu.publishData(comm)) {
    return true;
  }

  return dummySensor.publishData(comm) || //
         gps.publishData(comm);
}

uint8_t runState_IDLE()
{
  if (comm.hasData())
  {
    return STATE_PROCESS_RCV;
  }

  comm.receiveData();

  if (comm.hasData())
  {
    return STATE_PROCESS_RCV;
  }

   if (readSensors())
     return STATE_PROCESS_SND_SENSOR_DATA;

  return STATE_IDLE;
}

bool checkResetCmd()
{
  return comm.read(2) == RESET;
}

uint8_t runState_PROCESS_RCVD()
{

  bool ack = false;

  if (checkResetCmd())
  {
    comm.ack();
    comm.clearReceiveBuffer();
    return STATE_RST;
  }

  // actuators
  ack = dummyDevice.readCommand(comm) ||   //
        wheelMotor.readCommand(comm) ||    //
        steering.readCommand(comm);

  // sensors
  ack = ack || dummySensor.readCommand(comm) || //
        imu.readCommand(comm) ||                //
        gps.readCommand(comm);

  if (ack)
    comm.ack();
  else
    comm.nack();

  comm.clearReceiveBuffer();

  return STATE_IDLE;
}

void setup()
{
  comm.initialize();
  wheelMotor.initialize();
  steering.initialize();
  imu.initialize();
  gps.initialize();

  state = STATE_RST;
  //Serial.println("setup() called");
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
  case STATE_PROCESS_SND_SENSOR_DATA:
    comm.sendData(1, PROTOCOL_FRAME_TYPE_DATA_LIST);
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