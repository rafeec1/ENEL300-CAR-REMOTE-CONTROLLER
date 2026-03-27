#include <Arduino.h>

#include "ultrasonicMaybe.h"
#include "controller.h"

void setup() {
    Serial.begin(115200);
    Serial.printf("Serial Begin...");
    
    // futureBegin();
    distanceBegin();
    controllerBegin();
}

void loop() {

    // futureUpdate();
    distanceUpdate();
    controllerUpdate();
}