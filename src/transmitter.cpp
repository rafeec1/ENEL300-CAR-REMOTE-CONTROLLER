#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>


// =============== PINOUTS
// Buttons on Controller
#define LEFT        7
#define RIGHT       1
#define FORWARD     4
#define BACKWARD    8
 
// Special Buttons
#define T1          35
#define T2          36

#define LED_PIN     2

// ============ DATA STRUCTURE

typedef struct {
  int left, right, fwd, bck, t1, t2;
} controlPacket;

controlPacket data;

// ========= CONNECTION STATE VARIABLES
String success;
bool connected = false;
unsigned long lastSuccess = 0;
unsigned long lastBlink = 0;
bool ledState = LOW;

// Replace with RECEIVER MAC
uint8_t receiverMac[] = {0xF0, 0x9E, 0x9E, 0x03, 0x0C, 0xA4};

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    if (status == ESP_NOW_SEND_SUCCESS) {
        connected = true;
        lastSuccess = millis();
  }
}

void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    pinMode(LEFT, INPUT_PULLUP);
    pinMode(RIGHT, INPUT_PULLUP);
    pinMode(FORWARD, INPUT_PULLUP);
    pinMode(BACKWARD, INPUT_PULLUP);
    delay(1000);

    Serial.println("BOOTED");
    Serial.println("WIFI STA OK");

    WiFi.mode(WIFI_STA);

    esp_err_t result = esp_now_init();
    Serial.print("ESP-NOW init result: ");
    Serial.println(result);

    if (result != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_register_send_cb(onDataSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiverMac, 6);
    peerInfo.channel = 0;     // use current Wi-Fi channel
    peerInfo.encrypt = false; // start unencrypted first

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    Serial.println("===== SENDER READY =====");
}

void loop() {
    data.fwd = !digitalRead(FORWARD);
    data.bck = !digitalRead(BACKWARD);
    data.left = !digitalRead(LEFT);
    data.right = !digitalRead(RIGHT);
    data.t1 = !digitalRead(T1);
    data.t2 = !digitalRead(T2);
    
    esp_err_t result = esp_now_send(receiverMac, (uint8_t *) &data, sizeof(data));

    if (millis() - lastSuccess > 1000) {
        connected = false;
    }
    
    if (connected) {
        digitalWrite(LED_PIN, HIGH);
    } else {
        if (millis() - lastBlink > 400) {
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState);
            lastBlink = millis();
        }
    }
    Serial.println("---- TX DATA ----");
    delay(1000);
}
/*

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

typedef struct struct_message {
    int x;
    int y;
    bool buttonPressed;
} struct_message;

struct_message outgoingData;

// Receiver MAC: F0:9E:9E:03:0C:A4
uint8_t receiverMAC[] = {0xF0, 0x9E, 0x9E, 0x03, 0x0C, 0xA4};

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
    Serial.begin(115200);
    delay(3000);

    WiFi.mode(WIFI_STA);

    Serial.print("Sender MAC: ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_register_send_cb(onDataSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiverMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    Serial.println("Sender ready");
}

void loop() {
    outgoingData.x = 100;
    outgoingData.y = 200;
    outgoingData.buttonPressed = true;

    esp_err_t result = esp_now_send(receiverMAC,  const (uint8_t)&outgoingData, sizeof(outgoingData));

    if (result == ESP_OK) {
        Serial.println("Sent with success");
    } else {
        Serial.println("Error sending data");
    }

    delay(2000);
}
*/