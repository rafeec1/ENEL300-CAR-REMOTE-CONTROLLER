#include <Arduino.h>

#include "ultrasonicSensor.h"
#include "motorDriver.h"

void setup() {
    Serial.begin(115200);
    ultrasonicBegin();
    motorBegin();
}

void loop() {
    ultrasonicUpdate();
    motorUpdate();
}