# control-module
Crawler's low level driving control module<br />
 <br />
module | description
--- | ---
hal/ | Hardware Abstraction Layer for requesting Crawler control
lib/ | Packaging config for building the control module  as a shared lib for importing in other modules such as the planner or slam.
serialcomm/ | raspberry pi serial communication with ack control
arduino_controller/ | arduino control code + serial communication with ack control
tests/ | test cases for this module
utils/ | util methods (mainly for testing purposes)
<br />

## Compile
<br />

### to compile (Raspberry)
include hal/crawler_hal.h and please include wiringPi lib (see tests/hal/CMakeLists.txt)

### to compile (Arduino)
within the Arduino IDE, just compile the code in arduino_controller normally

### to compile and run tests (inside tests/)
make test


