#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// =============== PINOUTS (GPIOs)
#define BTN_FWD    4
#define BTN_BCK    8
#define BTN_LEFT   7
#define BTN_RIGHT  1
#define BTN_LIGHT  35
#define LED_CONN   2  // Connection status LED

#define TIME_DISP 1000

// ============ DATA STRUCTURE
typedef struct {
  int t1, t2;                // Reserved / Light toggle state
  int fwd, bck, left, right; // Motor controls
} controlPacket;

controlPacket data;
typedef struct {
    float dist;
} receivePacket;

receivePacket rData;

// ========= STATE VARIABLES
bool connected = false;
bool lightState = false;
bool lastLightBtnState = HIGH; // For debounce/toggle logic
unsigned long currentTime =0;
unsigned long counter = 0;

uint8_t receiverMac[] = {0xF0, 0x9E, 0x9E, 0x03, 0x0C, 0xA4};

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  connected = (status == ESP_NOW_SEND_SUCCESS);
}

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
  memcpy(&rData, incomingData, sizeof(rData));
}

void setup() {
  Serial.begin(115200);
  delay(2000); 

  pinMode(LED_CONN, OUTPUT);

  // Set all buttons as Input Pullup
  pinMode(BTN_FWD, INPUT_PULLUP);
  pinMode(BTN_BCK, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_LIGHT, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) return;

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  esp_now_register_recv_cb(onDataRecv);

  Serial.println("--- CAR CONTROLLER START ---");
}
void loop() {
  unsigned long currentTime = millis();
  // --- MOTOR CONTROL (Hold logic) ---
  // !digitalRead returns 1 when pressed (connected to GND)
  data.fwd   = !digitalRead(BTN_FWD);
  data.bck   = !digitalRead(BTN_BCK);
  data.left  = !digitalRead(BTN_LEFT);
  data.right = !digitalRead(BTN_RIGHT);

  // --- LIGHT CONTROL (Toggle logic) ---
  bool currentLightBtnState = digitalRead(BTN_LIGHT);

  // Check if button was just pressed (transition from HIGH to LOW)
  if (currentLightBtnState == LOW && lastLightBtnState == HIGH) {
    lightState = !lightState; // Switch the state
    delay(50); // Simple debounce
  }
  lastLightBtnState = currentLightBtnState;

  data.t1 = lightState; // Sending the light state in the t1 variable
  data.t2 = 0;

  // --- SEND DATA ---
  esp_err_t result = esp_now_send(receiverMac, (uint8_t *) &data, sizeof(data));
  //if (result == ESP_OK) Serial.println("Sent with success");
  //else Serial.println("Error Sending the Data");

  // --- VISUALS & MONITORING ---
  digitalWrite(LED_CONN, connected ? HIGH : (millis() % 500 < 250)); // Blink if disconnected
  
  //Serial.printf("F:%d B:%d L:%d R:%d | Light:%d | Conn:%s\n", 
  //            data.fwd, data.bck, data.left, data.right, 
  //            data.t1, connected ? "OK" : "FAIL");

  // print distance measurement
  if (currentTime - counter >= TIME_DISP) {
    counter = currentTime;
    Serial.print("Distance");
    Serial.println(rData.dist);

  } 

  delay(10); // Faster polling for better car responsiveness
}


