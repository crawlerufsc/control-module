# Prof-of-Concept for UART communication on a Raspberry Pi <br/>
This code implements a PoC for communicating with an Arduino via UART<br/>


## Communication Protocol<br/>
 The communication between this module and the Planner is done by UART (Serial) interface. <br/><br/>
 The controlling protocol for sending commands is the following:<br/>
 \[32|frame_id|device_id|cmd_data|cmd_data|cmd_data|cmd_data...|31\]<br/>
 <br/>
 where<br/>
 frame_id - is a 8-bit unsigned int<br/>
 device_id - is a 8-bit unsigned int that identifies the device being controlled<br/>
 cmd_data[] - is the unsigned int 8-bit payload for the device_id being controlled<br/>



