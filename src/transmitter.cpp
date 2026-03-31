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

#define SAMPLE_PERIOD 200

// ============ DATA STRUCTURE

typedef struct {
  int left, right, fwd, bck, t1, t2;
} controlPacket;
controlPacket txData;

// ========= CONNECTION STATE VARIABLES
String success;
bool connected = false;

unsigned long lastSuccess = 0;
unsigned long lastBlink = 0;
unsigned long timePast = 0;
bool ledState = LOW;

// Replace with RECEIVER MAC
uint8_t receiverMac[] = {0xF0, 0x9E, 0x9E, 0x03, 0x0C, 0xA4};

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    if (status == ESP_NOW_SEND_SUCCESS) {
        connected = true;
        lastSuccess = millis();
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_PIN, OUTPUT);
    pinMode(LEFT, INPUT_PULLUP);
    pinMode(RIGHT, INPUT_PULLUP);
    pinMode(FORWARD, INPUT_PULLUP);
    pinMode(BACKWARD, INPUT_PULLUP);
    
    WiFi.mode(WIFI_STA);
    Serial.println("WIFI STA OK");

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_register_send_cb(onDataSent);

    esp_now_peer_info_t peerInfo;
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
    unsigned long currentTime = millis();
    if (currentTime - timePast >= SAMPLE_PERIOD){
        timePast = currentTime;
        txData.fwd = !digitalRead(FORWARD);
        txData.bck = !digitalRead(BACKWARD);
        txData.left = !digitalRead(LEFT);
        txData.right = !digitalRead(RIGHT);
        txData.t1 = !digitalRead(T1);
        txData.t2 = !digitalRead(T2);
        
        esp_err_t result = esp_now_send(receiverMac, (uint8_t *) &txData, sizeof(txData));
        
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
        }
    
}
