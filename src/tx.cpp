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

#define ESPNOW_CHANNEL 1

// ============ DATA STRUCTURE
typedef struct {
  int left, right, fwd, bck, t1, t2;
} controlPacket;

controlPacket data;

// ========= CONNECTION STATE VARIABLES
bool connected = false;
unsigned long lastSuccess = 0;
unsigned long lastBlink = 0;
bool ledState = LOW;

// Replace with RECEIVER MAC
uint8_t receiverMac[] = {0xF0, 0x9E, 0x9E, 0x03, 0x0C, 0xA4};

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");

  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Success");
    connected = true;
    lastSuccess = millis();
  } else {
    Serial.println("Fail");
  }
}
void setup() {
  Serial.begin(115200);
  delay(3000);

  pinMode(LED_PIN, OUTPUT);
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(FORWARD, INPUT_PULLUP);
  pinMode(BACKWARD, INPUT_PULLUP);
  pinMode(T1, INPUT_PULLUP);
  pinMode(T2, INPUT_PULLUP);

  Serial.println("BOOTED");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println("WIFI STA OK");
  Serial.print("Sender MAC: ");
  Serial.println(WiFi.macAddress());

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
  peerInfo.channel = ESPNOW_CHANNEL;
  
  peerInfo.encrypt = false;

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

  esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&data, sizeof(data));
  Serial.println(result == ESP_OK ? "send queued OK" : "send failed");
  Serial.print("esp_now_send result: ");
  Serial.println(result);

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
  Serial.print("fwd: ");
  Serial.println(data.fwd);
  Serial.print("bck: ");
  Serial.println(data.bck);
  Serial.print("left: ");
  Serial.println(data.left);
  Serial.print("right: ");
  Serial.println(data.right);
  Serial.print("t1: ");
  Serial.println(data.t1);
  Serial.print("t2: ");
  Serial.println(data.t2);
  Serial.println("-----------------");

  delay(1000);
}
 
 