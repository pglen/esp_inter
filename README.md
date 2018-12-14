# ESP32 Monitor interpreter

## Command line for the ESP32 monitor

A simple monitor shell for the esp32. Type help to see a list of commands. 

This is meant to be a template for your command additions. See 'reboot' command for an example on how to extend this interpreter.

The command line responds to a handful od simple commands. For instamnce the command 'reboot' will restart the esp32 board. There are no editing facilities, like backspace or arrows will not work as expected. Simply press enter, and type in the new command.

One can connect to the ESP32 boarf from a terminal like Putty. Set the com parameters to 115200 8N1.
On the DevKitC V4 the XON/XOFF has to be unchecked for it to communicate properly.

Project is placed in the public domain.

This is a sample application to be used with 
`Espressif IoT Development Framework`_ (ESP-IDF). 

