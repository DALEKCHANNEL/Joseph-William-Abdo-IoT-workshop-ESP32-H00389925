# Author 
Joseph William Abdo
HW-ID: H00389925
Uni-Email: jwa2001@hw.ac.uk


# Overview
This repository implements an Internet of Things project using an ESP32 microcontroller alongside a DHT11 sensor for measuring temperature and humidity, and a NeoPixel LED that changes color depending on the temperature level. The system gathers the ESP32's data and sends it to a MQTT service as a publisher, which then sends the data to the Node-RED service, a web-based visualization tool that uses nodes to control the flow of data. Using Node-RED, the data is then visualized as a graph and stored in MongoDB. Additionally, in part 3, the code was modified to be bidirectional so Node-RED could publish JSON commands to the ESP32 to change the interval state or temporarily change the LED colour.

## Components
- Hardware: ESP32, DHT11, NeoPixel LED, Jumper Cables, Breadboard, 100k Ohms resistor
- Software: MQTT, Node-RED, MongoDB

### MQTT Topics
The checklist below are the topic names that publish data and subscribe to data from Node-RED
- esp32-test-01joe/evt/status/fmt/json
- esp32-test-01joe/fmt/display
- esp32-test-01joe/fmt/interval
#### MQTT Host Name
- #define MQTT_HOST "broker.mqtt.cool"
- #define MQTT_PORT 1883
#### Device name
- #define MQTT_DEVICEID "esp32-test-01joe"

## Part 1
Part 1 was completed using the libraries and sketches of the ESP32 and have been added in the part 1 file although they have not been modified they were helpful in understanding the system

## Part 2 
Part 2 has the code that combines the ESP32 with the sensors and only publishes data to MQTT for Node-RED to visualize and store

## Part 3
Part 3 has the fully finished code that allows for bidirectional connection allowing for Node-RED to recieve data but also provide data for the ESP32 to either change its delay time or temporarily control the LED light colours
