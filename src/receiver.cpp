#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// PINOUT FOR MOTORS
#define LEFT_IN1   9
#define LEFT_IN2   10
#define LEFT_EN    13
 
// Right motor
#define RIGHT_IN1  11
#define RIGHT_IN2  12
#define RIGHT_EN   14

#define LED_PIN    2
#define LED_TRIG   18 

// ===================== PWM =====================
const int pwmFreq = 5000;
const int pwmResolution = 8;
 
const int leftChannel  = 0;
const int rightChannel = 1;
 
// 60% speed
const int motorSpeedLeft = 190;   // 255 * 0.60 ≈ 153
const int motorSpeedRight = 210;   // 255 * 0.60 ≈ 153


// ====== DATA STRUCT ============
typedef struct {
  int left, right, fwd, bck, t1, t2;
} struct_message;

struct_message rxData;

// CONNECTION STATE VARIABLES
unsigned long lastRecvTime = 0;
bool signalLost = true;

bool ledState = LOW;

// transmitter MAC
uint8_t transmitterMAC[] = {0xF0, 0x9E, 0x9E, 0x03, 0x0C, 0x88};

void onDataRecv(const uint8_t *mac,
                const uint8_t *incomingData, int len) {
  memcpy(&rxData, incomingData, sizeof(rxData));
  lastRecvTime = millis();
  signalLost = false;
}

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

void setup() {
  Serial.begin(115200);
  delay(1000);  // same trick so monitor can catch setup prints
  Serial.println("BOOTED");

  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_TRIG, OUTPUT);

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

  WiFi.mode(WIFI_STA);
  Serial.println("WIFI STA OK");

  esp_err_t result = esp_now_init();
  Serial.print("ESP-NOW init result: ");
  Serial.println(result);

  if (result != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);

  /*esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  */
  Serial.println("RECEIVER READY");
}

void loop() {

    // ===== FAILSAFE =====
  if (millis() - lastRecvTime > 1000) {
    signalLost = true;
    stopMotors();
    digitalWrite(LED_PIN, LOW);
    return;
  }
  digitalWrite(LED_PIN, HIGH);

  // ===== BUTTON PRIORITY =====
  if (rxData.fwd) {
    forward();
  }
  else if (rxData.bck) {
    reverse();
  }
  else if (rxData.left) {
    turnLeft();
  }
  else if (rxData.right) {
    turnRight();
  }
  else if (rxData.t1) {
    digitalWrite(LED_TRIG, HIGH);
  }
  else {
    stopMotors();
  }
    Serial.print("t1:");
    Serial.print(rxData.t1);
    Serial.print("t2:");
    Serial.print(rxData.t2);

  delay(2000);
}
 
 