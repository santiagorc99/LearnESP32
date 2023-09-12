# MQTT
In MQTT, we have 3 players, the ESP32 (Client), a server or computer and a broker (Mosquito is a source free MQTT broker SW, it's used for this example).

In theory we treat the both, the ESP and the PC "server" as clients. We have a client interested in 1 topic, the topic is kind of a url then, the ESP32 can publish to the topic and then the mosquito receives the message and send the info to the other client.

## TOPICS
### Topics are hierarchical
General/less-general/more-specific. I.g:
* house1/room1/temperature (Gets the teperature of room1 at house 1)
* house2/room3/humidity    (Gets the teperature of room3 at house 2)
In case we are interested on reading temp and hum in different houses, in thepry we have to subscribe to each topic. But also in practice, we do not need to subscribe to each different thing, we can just use wildcards to suscribe to all of the things: 

### Topics can have wildcards
1. '+' replaces one segment in a topic:
    * house1/+/temperature (Gets the temperature reading from all roroms in house 1)
2. '#' replaces the rest of the topic
    * house1/# (Gets temperature and humidity reading from all rooms in house 1).

## Retain
Let's say we disconnect the PC "server" from the network and our ESP32 delivers a message to the mosquito broker, the ESP sends it and the message is sent once the PC "server" subscribe to the particular topic again

## QoS (Quality of service)
Note: Unless we have a good reason, QoS should be used.
### QoS 0
ESP32 sends a message to the broker and that's it, there's no reply from mosquito.

### QoS 1
ESP32 sends a message to the broker and then the broker replies with a publish acknowledge back. So the ESP32 knows the broker received the message. The broker still does not know that the ESP32 received the publish ACK (PUBACK).

### QoS2
ESP32 sends a message to the broker and then the broker replies with a publish received (PUBREC), the ESP32 replies with a publish release (PUBREL), and finally, the broker replies with a publish completion (PUBCOMP).

## Last Will and Testament (LWT)
When the ESP32 goes online and makes a connection it saysthat it wants to publish the last will and testament to some topic. The PC client can subscribe the topic but if something happens to the ESP32 (It reboot or disconnected), the broker will send through the last will and testament to the PC client.

## MQTT Client
We are going to use MQTT explorer. It has some features such as enabling security over TLS and various different other options.

We select our broker (In this case we are going to use eclipse)