// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLE2902.h>
// #include "esp_bt_main.h"
// #include "esp_bt_device.h"
// #include "esp_gap_ble_api.h"

// // ================= BLE UUIDs =================
// #define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
// #define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// // ================= L298N Pins =================
// // ENA and ENB ignored for now
// static const int IN1_PIN = 41;
// static const int IN2_PIN = 40;
// static const int IN3_PIN = 39;
// static const int IN4_PIN = 38;

// BLECharacteristic *pCharacteristic;
// bool deviceConnected = false;

// // ================= Motor Control Functions =================
// void stopMotors() {
//   digitalWrite(IN1_PIN, LOW);
//   digitalWrite(IN2_PIN, LOW);
//   digitalWrite(IN3_PIN, LOW);
//   digitalWrite(IN4_PIN, LOW);
// }

// void moveForward() {
//   // Motor A forward
//   digitalWrite(IN1_PIN, HIGH);
//   digitalWrite(IN2_PIN, LOW);

//   // Motor B forward
//   digitalWrite(IN3_PIN, HIGH);
//   digitalWrite(IN4_PIN, LOW);
// }

// void moveBackward() {
//   // Motor A backward
//   digitalWrite(IN1_PIN, LOW);
//   digitalWrite(IN2_PIN, HIGH);

//   // Motor B backward
//   digitalWrite(IN3_PIN, LOW);
//   digitalWrite(IN4_PIN, HIGH);
// }

// void turnLeft() {
//   // Left turn: left motor backward, right motor forward
//   digitalWrite(IN1_PIN, LOW);
//   digitalWrite(IN2_PIN, HIGH);

//   digitalWrite(IN3_PIN, HIGH);
//   digitalWrite(IN4_PIN, LOW);
// }

// void turnRight() {
//   // Right turn: left motor forward, right motor backward
//   digitalWrite(IN1_PIN, HIGH);
//   digitalWrite(IN2_PIN, LOW);

//   digitalWrite(IN3_PIN, LOW);
//   digitalWrite(IN4_PIN, HIGH);
// }

// // ================= BLE Server Callbacks =================
// class MyServerCallbacks : public BLEServerCallbacks {
//   void onConnect(BLEServer* pServer) override {
//     deviceConnected = true;
//     Serial.println(">>> [SERVER] Client Connected!");
//   }

//   void onDisconnect(BLEServer* pServer) override {
//     deviceConnected = false;
//     Serial.println(">>> [SERVER] Client Disconnected. Restarting Advertising...");
//     stopMotors();
//     delay(500);
//     BLEDevice::startAdvertising();
//   }
// };

// // ================= BLE Characteristic Callbacks =================
// class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
//   void onWrite(BLECharacteristic *pCharacteristic) override {
//     std::string value = pCharacteristic->getValue();

//     Serial.print(">>> [SERVER] Received: ");
//     Serial.println(value.c_str());

//     if (value == "F") {
//       Serial.println(">>> [SERVER] MOVE FORWARD");
//       moveForward();
//     }
//     else if (value == "B") {
//       Serial.println(">>> [SERVER] MOVE BACKWARD");
//       moveBackward();
//     }
//     else if (value == "L") {
//       Serial.println(">>> [SERVER] TURN LEFT");
//       turnLeft();
//     }
//     else if (value == "R") {
//       Serial.println(">>> [SERVER] TURN RIGHT");
//       turnRight();
//     }
//     else if (value == "S") {
//       Serial.println(">>> [SERVER] STOP");
//       stopMotors();
//     }
//     else {
//       Serial.println(">>> [SERVER] Unknown command");
//     }
//   }
// };

// void setup() {
//   Serial.begin(115200);

//   // ================= Motor Pin Setup =================
//   pinMode(IN1_PIN, OUTPUT);
//   pinMode(IN2_PIN, OUTPUT);
//   pinMode(IN3_PIN, OUTPUT);
//   pinMode(IN4_PIN, OUTPUT);

//   stopMotors();

//   // ================= BLE Setup =================
//   BLEDevice::init("S3_MOTOR_SERVER");

//   // Max BLE TX power
//   esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9);
//   esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
//   esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, ESP_PWR_LVL_P9);

//   BLEServer *pServer = BLEDevice::createServer();
//   pServer->setCallbacks(new MyServerCallbacks());

//   BLEService *pService = pServer->createService(SERVICE_UUID);

//   pCharacteristic = pService->createCharacteristic(
//     CHARACTERISTIC_UUID,
//     BLECharacteristic::PROPERTY_READ |
//     BLECharacteristic::PROPERTY_WRITE |
//     BLECharacteristic::PROPERTY_NOTIFY
//   );

//   pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
//   pCharacteristic->addDescriptor(new BLE2902());

//   pService->start();

//   BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
//   pAdvertising->addServiceUUID(SERVICE_UUID);
//   pAdvertising->setScanResponse(true);
//   pAdvertising->setMinPreferred(0x12);
//   pAdvertising->setMaxPreferred(0x24);

//   BLEDevice::startAdvertising();

//   Serial.println(">>> [SERVER] Setup Complete. Ready for motor commands.");
//   Serial.println(">>> Send: F, B, L, R, or S");
// }

// void loop() {
//   if (deviceConnected) {
//     pCharacteristic->setValue("STAY_ALIVE");
//     pCharacteristic->notify();

//     Serial.println(">>> [SERVER] Heartbeat sent.");
//     delay(2000);
//   }
// }
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"
 
#define BUZZER_PIN 18
#define BUZZER_CHANNEL 0
#define BUZZER_FREQ 2000
#define BUZZER_RESOLUTION 8
 
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
 
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
 
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println(">>> [SERVER] Client Connected!");
    }
 
    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println(">>> [SERVER] Client Disconnected. Restarting Advertising...");
        delay(500);
        BLEDevice::startAdvertising();
    }
};
 
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
 
        std::string value = pCharacteristic->getValue();
 
        Serial.print(">>> [SERVER] Received: ");
        Serial.println(value.c_str());
 
        if(value == "1") {
            Serial.println(">>> [SERVER] BUZZ!");
 
            ledcWriteTone(BUZZER_CHANNEL, 2000);
            delay(300);
            ledcWriteTone(BUZZER_CHANNEL, 0);
        }
    }
};
 
void setup() {
 
    Serial.begin(115200);
 
    // buzzer setup
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
 
    // Initialize BLE
    BLEDevice::init("S3_SERVER_PRO");
 
    // BOOST POWER
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9);
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, ESP_PWR_LVL_P9);
 
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
 
    BLEService *pService = pServer->createService(SERVICE_UUID);
 
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_WRITE |   // <-- important
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
 
    pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
    pCharacteristic->addDescriptor(new BLE2902());
 
    pService->start();
 
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
 
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->setMaxPreferred(0x24);
 
    BLEDevice::startAdvertising();
 
    Serial.println(">>> [SERVER] Setup Complete. Ready for phone commands.");
}
 
void loop() {
 
    if (deviceConnected) {
 
        pCharacteristic->setValue("STAY_ALIVE");
        pCharacteristic->notify();
 
        Serial.println(">>> [SERVER] Heartbeat sent.");
 
        delay(2000);
    }
}