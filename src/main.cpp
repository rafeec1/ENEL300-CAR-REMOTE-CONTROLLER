#include <Arduino.h>

#include "ultrasonicMaybe.h"
// #include "controller.h"
#include "metalDetector.h"
#include "receiver.h"

void setup() {
    Serial.begin(115200);
    Serial.printf("Serial Begin...");
    
    //controllerBegin();
    rxBegin();
    distanceBegin();
    metalBegin();
}

void loop() {
    //controllerUpdate()
    rxUpdate();
    distanceUpdate();
    metalUpdate();
}