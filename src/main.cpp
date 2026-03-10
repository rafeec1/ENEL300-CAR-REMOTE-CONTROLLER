#include <Arduino.h>

#include "ultrasonicSensor.h"
#include "motorDriver.h"
#include "ultrasonicMaybe.h"

void setup() {
    Serial.begin(115200);
    Serial.printf("Serial Begin...");
    // ultrasonicBegin();
    motorBegin();
    usBegin();
}

void loop() {
    // ultrasonicUpdate();
    motorUpdate();
    usUpdate();
}