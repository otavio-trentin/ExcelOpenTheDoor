# ExcelOpenTheDoor

The present repo have the aim to control the opening of 3 doors from a excel file, it sends a string "id,time" to the Arduino Nano that will open the door (id) by the amount of seconds (time). The door opening is via a relay.

We first tried to use directly VBA to Arduino, but we fail.

The solution was to use the VBA to run a Python code that send the data via serial to the arduino, this arduino will then open the door for the amount of time defined.

To open multiple doors the command needs to be send indivually. 

The status of the doors is saved to the EEPROM due to the fact that when the serial connection is open the arduino nano restart. A capicitora was lately added to avoid the restart.

The eletronic project will be added soon.
