#include <Arduino.h>

#include "ultrasonicMaybe.h"
#include "metalDetector.h"
#include "receiver.h"

void setup() {
    Serial.begin(115200);
    Serial.printf("Serial Begin...");
    
    rxBegin();
    distanceBegin();
    metalBegin();
}

void loop() {
    rxUpdate();
    distanceUpdate();
    metalUpdate();
}