# Servo Controller <br/>
This code runs the Arduino portion of the Crawler code, responsible for controlling the hardware servos and acquiring sensor data <br/>


## Communication Protocol<br/>
 The communication between this module and the Planner is done by UART (Serial) interface. <br/><br/>
 The controlling protocol for sending commands is the following:<br/>
 \[32|frame_id|device_id|cmd_data|cmd_data|cmd_data|cmd_data...|31\]<br/>
 <br/>
 where<br/>
 frame_id - is a 8-bit unsigned int<br/>
 device_id - is a 8-bit unsigned int that identifies the device being controlled<br/>
 cmd_data[] - is the unsigned int 8-bit payload for the device_id being controlled<br/>
<br/>
### Command Ack/NAck
<br/>
In case of an accepted command, an ack is issued by this code using the following format:<br/>
[32|frame_id|1|31]<br/>
<br/>
In case of a rejected command, a nack is issued by this code using the following format:<br/>
[32|frame_id|2|31]<br/>

