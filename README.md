# planner-module
Crawler's low level driving control module<br />
 <br />
module | description
--- | ---
hal/ | Hardware Abstraction Layer for requesting Crawler control
serialcomm/ | raspberry pi serial communication with ack control
servo_controller/ | arduino control code + serial communication with ack control
tests/ | test cases for this module
utils/ | util methods (mainly for testing purposes)
<br />

## Compile
<br />

### to compile (Raspberry)
include hal/crawler_hal.h and please include wiringPi lib (see tests/hal/CMakeLists.txt)

### to compile (Arduino)
with Arduino IDE, just compile it normally

### to compile and run tests (inside tests/)
make test


