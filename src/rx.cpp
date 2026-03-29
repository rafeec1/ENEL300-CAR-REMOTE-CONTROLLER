#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <string.h>

typedef struct {
  int left, right, fwd, bck, t1, t2;
} controlPacket;

controlPacket incomingData;

void onDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
  if (len == sizeof(incomingData)) {
    memcpy(&incomingData, data, sizeof(incomingData));

    Serial.println("DATA RECEIVED");
    Serial.print("left: ");  Serial.println(incomingData.left);
    Serial.print("right: "); Serial.println(incomingData.right);
    Serial.print("fwd: ");   Serial.println(incomingData.fwd);
    Serial.print("bck: ");   Serial.println(incomingData.bck);
    Serial.print("t1: ");    Serial.println(incomingData.t1);
    Serial.print("t2: ");    Serial.println(incomingData.t2);
    Serial.println("-------------------");
  } else {
    Serial.print("Wrong packet size: ");
    Serial.println(len);
  }
}

void setup() {
  Serial.begin(115200);
  delay(5000);

  Serial.println("BOOTED");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);
  delay(100);

  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
  Serial.println("RECEIVER READY");
}

void loop() {
  delay(100);
}