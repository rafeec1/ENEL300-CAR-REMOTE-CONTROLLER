#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>

void stopMotors();
void forward();
void reverse();
void turnLeft();
void turnRight();

void controllerBegin();
void controllerUpdate();

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override;
};
#endif