# ESP-NOW
* It's a way for ESP32s to communicate to each other (Up to 20 ESPs). 
* The messages can be encrypted and the max size of each packet is 250 bytes. 
* It's a lightweight protocol (Does not required http/mqtt). It help us save battery for the "on" time is less than in other protocols.
* We need to know the MAC address of the ESP we want to communicate to.