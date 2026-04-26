# ESP32-Based Remote Controlled Car

For a school design course and competition, our group of 5 built a Remote Controlled Car with metal detection, a ultrasonic distance sensor with wireless display on the controller, and working headlights that operates fully remotely with a dedicated remote controller.
Our group used KiCad for schematic and PCB routing, and Fusion360 for mechanical design to construct our car. Through 4 months of work, and several iterations, our final submission included a car with these requirements. For our microcontroller, we chose the ESP-32-S3-devkitc for our transmitter and receiver, with the ESPNOW protocol enabling duplex communication to control the car.

## Functional Sensors
### Metal Detection
To accomplish metal detection, our design used a continuity test, where a 3.3V source from our ESP32-S3 pin was dragged across the ground, with another wire connected to an Active Buzzer which produced sound when over a conductive material. 
### Ultrasonic Distance Sensing
A HC-SR04 ultrasonic distance module was used to measure distances ahead of the bumper. From testing, accurate values up to 2.00m ahead were observed with accuracy up to 1cm for nearby objects (<50cm).
### Wireless LCD Distance Display
Using a 16x2 LCD display with an I2C backpack, live distance measurements were sent from the receiver to the transmitter ESP32 using the ESPNOW protocol to communicate.
### LED Headlights
Headlights from an RC Car kit were implemented on the car. To control the lights, a Relay was used to control.

## Mechanical Design
### Chassis Design
The chassis was 3D printed, with two iterations. Initial models used a two-wheeled E-differential to steer, but the final product switched to a 4 wheel RWD varient with servo-based steering on a Ackermann-based steering axle. 

