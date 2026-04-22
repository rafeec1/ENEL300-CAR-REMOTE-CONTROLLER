#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include "ultrasonicMaybe.h"

// ===================== PINOUT FOR MOTORS
#define LEFT_IN1   9
#define LEFT_IN2   10
#define LEFT_EN    13
 
#define RIGHT_IN1  11
#define RIGHT_IN2  12
#define RIGHT_EN   14

#define LED_TRIG   17

// ===================== PWM SETTINGS
const int pwmFreq = 5000;
const int pwmResolution = 8;
const int leftChannel  = 0;
const int rightChannel = 1;

// distance measurement to read
// extern float distance;

// Motor Speeds (Adjust if car veers)
const int motorSpeedLeft = 230;   
const int motorSpeedRight = 170;  

uint8_t TxMACaddress[] = {0xF0, 0x9E, 0x9E, 0x03, 0x0C, 0x88};
bool connected = false;
// ===================== DATA STRUCT
// MUST MATCH SENDER EXACTLY
typedef struct {
  int t1, t2;                // t1 is our Light State
  int fwd, bck, left, right; // Motor controls
} struct_message;

struct_message rxData;

typedef struct {
  float dist;
} dist_message;

dist_message tData;

// CONNECTION STATE
unsigned long lastRecvTime = 0;
const unsigned long TIMEOUT_MS = 500; // Auto-stop if signal lost

void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&rxData, incomingData, sizeof(rxData));
  lastRecvTime = millis();
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    if (status == ESP_NOW_SEND_SUCCESS) {
        connected = true;
    }
}

// ===================== MOTOR FUNCTIONS
void stopMotors() {
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);
  ledcWrite(leftChannel, 0);
  ledcWrite(rightChannel, 0);
}
 
void forward() {
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
  ledcWrite(leftChannel, motorSpeedLeft);
  ledcWrite(rightChannel, motorSpeedRight);
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
  // Tank turn: Left motor Back, Right motor Forward
  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);
  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
  ledcWrite(leftChannel, motorSpeedLeft);
  ledcWrite(rightChannel, motorSpeedRight);
}
 
void turnRight() {
  // Tank turn: Left motor Forward, Right motor Back
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, HIGH);
  ledcWrite(leftChannel, motorSpeedLeft);
  ledcWrite(rightChannel, motorSpeedRight);
}

void rxBegin() {
  Serial.begin(115200);
  
  pinMode(LED_TRIG, OUTPUT);
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);
 
  ledcSetup(leftChannel, pwmFreq, pwmResolution);
  ledcAttachPin(LEFT_EN, leftChannel);
  ledcSetup(rightChannel, pwmFreq, pwmResolution);
  ledcAttachPin(RIGHT_EN, rightChannel);
 
  stopMotors();

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }
  //----------------------------------------------------------
  esp_now_register_send_cb(onDataSent);
  //----------------------------------------------------------
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, TxMACaddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  //----------------------------------------------------------
  if(esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
  Serial.println("RECEIVER READY");
}

void rxUpdate() {
  // 1. Safety Timeout (Stop motors if we lose connection)
  if (millis() - lastRecvTime > TIMEOUT_MS) {
    stopMotors();
    return; 
  }

  // 2. Light Control (Toggled on Sender, result sent in t1)
  digitalWrite(LED_TRIG, rxData.t1 ? HIGH : LOW);

  // 3. Motor Logic (Priority handling)
  if (rxData.fwd) {
    forward();
    Serial.println("F");
  } else if (rxData.bck) {
    reverse();
    Serial.println("B");
  } else if (rxData.left) {
    turnLeft();
    Serial.println("L");
  } else if (rxData.right) {
    turnRight();
    Serial.println("R");
  } else {
    stopMotors();
    Serial.println("S");
  }

  tData.dist = distanceCm;
  Serial.println(tData.dist);

  esp_err_t result = esp_now_send(TxMACaddress, (uint8_t *) &tData, sizeof(tData));
  //----------------------------------------------------------
  if (result == ESP_OK) Serial.println("Sent with success");
  else Serial.println("Error sending the data");
  delay(10); // Fast loop for motor response
}