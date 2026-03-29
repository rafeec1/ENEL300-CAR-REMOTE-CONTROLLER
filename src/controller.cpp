#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"
 
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
 
// ===================== BLE =====================
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
 
// ===================== MOTOR PINS =====================
// CHANGE THESE TO MATCH YOUR WIRING
 
// Left motor
#define LEFT_IN1   9
#define LEFT_IN2   10
#define LEFT_EN    13
 
// Right motor
#define RIGHT_IN1  11
#define RIGHT_IN2  12
#define RIGHT_EN   14
 
// ===================== PWM =====================
const int pwmFreq = 5000;
const int pwmResolution = 8;
 
const int leftChannel  = 0;
const int rightChannel = 1;
 
// 60% speed
const int motorSpeedLeft = 190;   // 255 * 0.60 ≈ 153
const int motorSpeedRight = 210;   // 255 * 0.60 ≈ 153
 
// ===================== MOTOR FUNCTIONS =====================
void stopMotors() {
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);
 
  ledcWrite(leftChannel, 0);
  ledcWrite(rightChannel, 0);
 
  Serial.println(">>> STOP");
}
 
void forward() {
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
 
  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
 
  ledcWrite(leftChannel, motorSpeedLeft);
  ledcWrite(rightChannel, motorSpeedRight);
 
  Serial.println(">>> FORWARD");
}

void reverse() {
    digitalWrite(LEFT_IN1, HIGH);
    digitalWrite(LEFT_IN2, LOW);

    digitalWrite(RIGHT_IN1, LOW);
    digitalWrite(RIGHT_IN2, HIGH);

    ledcWrite(leftChannel, motorSpeedLeft);
    ledcWrite(rightChannel, motorSpeedRight);
}
 
void turnLeft() {
  // Left motor OFF
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);
  ledcWrite(leftChannel, 0);
 
  // Right motor ON
  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
  ledcWrite(rightChannel, motorSpeedRight);
 
  Serial.println(">>> LEFT TURN");
}
 
void turnRight() {
  // Left motor ON
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
  ledcWrite(leftChannel, motorSpeedLeft);
 
  // Right motor OFF
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);
  ledcWrite(rightChannel, 0);
 
  Serial.println(">>> RIGHT TURN");
}
 
// ===================== BLE CALLBACKS =====================
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    deviceConnected = true;
    Serial.println(">>> [SERVER] Client Connected!");
  }

  void onDisconnect(BLEServer* pServer) override {
    deviceConnected = false;
    Serial.println(">>> [SERVER] Client Disconnected. Restarting Advertising...");
    delay(500);
    BLEDevice::startAdvertising();
  }
};
 
class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    String rxValue = pCharacteristic->getValue().c_str();
 
    if (rxValue.length() > 0) {
      Serial.print(">>> Received command: ");
      Serial.println(rxValue);
 
      if (rxValue == "F") {
        forward();
      }
      else if (rxValue == "L") {
        turnLeft();
      }
      else if (rxValue == "R") {
        turnRight();
      }
      else if (rxValue == "S") {
        stopMotors();
      }
      else if (rxValue == "B") {
        reverse();
      }
      else {
        Serial.println(">>> Unknown command");
      }
    }
  }
};
 
// ===================== SETUP =====================
void controllerBegin() {
  Serial.println("Controller Begins...");
 
  // Motor pins
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);
 
  // PWM setup
  ledcSetup(leftChannel, pwmFreq, pwmResolution);
  ledcAttachPin(LEFT_EN, leftChannel);
 
  ledcSetup(rightChannel, pwmFreq, pwmResolution);
  ledcAttachPin(RIGHT_EN, rightChannel);
 
  stopMotors();
 
  // BLE setup
  BLEDevice::init("ESP32_Motor_Control");
 
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
 
  BLEService *pService = pServer->createService(SERVICE_UUID);
 
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
 
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Hello from ESP32");
 
  pService->start();
 
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
 
  Serial.println(">>> BLE device is ready. Connect with nRF Connect.");
  Serial.println(">>> Send: F = forward, L = left motor only, R = right motor only, S = stop");
}
 
// ===================== LOOP =====================
void controllerUpdate() {
  static unsigned long lastHeartbeat = 0;

  if (millis() - lastHeartbeat >= 1000) {
    lastHeartbeat = millis();
    Serial.println("controller alive");
  }
}
 