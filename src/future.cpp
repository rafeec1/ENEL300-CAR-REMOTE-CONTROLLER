#include <Arduino.h>
#include <U8g2lib.h>

// ==========================================================
// HC-SR04 pins
// ==========================================================
static const uint8_t TRIG_PIN = 16;
static const uint8_t ECHO_PIN = 17;

// ==========================================================
// OLED pins, 4-wire SPI
// CLK/SCL = GPIO 9
// MOSI/SDA = GPIO 10
// RST = GPIO 11
// DC  = GPIO 12
// CS  = GPIO 13
// ==========================================================
static const uint8_t OLED_CLK  = 9;
static const uint8_t OLED_MOSI = 10;
static const uint8_t OLED_RST  = 11;
static const uint8_t OLED_DC   = 12;
static const uint8_t OLED_CS   = 13;

// Software SPI U8g2 constructor for SSD1306 128x64 OLED
U8G2_SSD1315_128X64_NONAME_F_4W_SW_SPI display(
    U8G2_R0,
    OLED_CLK,
    OLED_MOSI,
    OLED_CS,
    OLED_DC,
    OLED_RST
);

// ==========================================================
// Ultrasonic shared state
// ==========================================================
volatile uint32_t echoRiseUs = 0;
volatile uint32_t echoFallUs = 0;
volatile bool echoPulseComplete = false;

float latestDistanceCm = 0.0f;
bool distanceValid = false;
unsigned long lastValidDistanceMs = 0;

// sample period
const unsigned long SAMPLE_PERIOD_MS = 50;
unsigned long lastSampleMs = 0;

// timeout for "no echo"
const unsigned long ECHO_TIMEOUT_MS = 35;

// ==========================================================
// ISR for ECHO pin
// ==========================================================
void IRAM_ATTR echoISR() {
  if (digitalRead(ECHO_PIN) == HIGH) {
    echoRiseUs = micros();
  } else {
    echoFallUs = micros();
    echoPulseComplete = true;
  }
}

// ==========================================================
// Trigger one ultrasonic measurement
// ==========================================================
void triggerUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);
}

// ==========================================================
// Convert pulse width to distance
// HC-SR04 distance in cm = pulse_us / 58.0 approximately
// ==========================================================
void processUltrasonicResult() {
  static unsigned long triggerStartMs = 0;
  static bool waitingForEcho = false;

  unsigned long nowMs = millis();

  // Start a new measurement every 50 ms
  if (!waitingForEcho && (nowMs - lastSampleMs >= SAMPLE_PERIOD_MS)) {
    lastSampleMs = nowMs;
    echoPulseComplete = false;
    triggerUltrasonic();
    triggerStartMs = nowMs;
    waitingForEcho = true;
  }

  // If we received a pulse, compute distance
  if (waitingForEcho && echoPulseComplete) {
    noInterrupts();
    uint32_t rise = echoRiseUs;
    uint32_t fall = echoFallUs;
    echoPulseComplete = false;
    interrupts();

    if (fall > rise) {
      uint32_t pulseWidthUs = fall - rise;

      // reject obvious garbage values
      if (pulseWidthUs > 100 && pulseWidthUs < 30000) {
        latestDistanceCm = pulseWidthUs / 58.0f;
        distanceValid = true;
        lastValidDistanceMs = nowMs;
      } else {
        distanceValid = false;
      }
    } else {
      distanceValid = false;
    }

    waitingForEcho = false;
  }

  // Timeout if no echo comes back
  if (waitingForEcho && (nowMs - triggerStartMs > ECHO_TIMEOUT_MS)) {
    distanceValid = false;
    waitingForEcho = false;
  }
}

// ==========================================================
// OLED drawing
// ==========================================================
void drawDisplay() {
  char line1[32];
  char line2[32];
  char line3[32];

  display.clearBuffer();

  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 12, "HC-SR04 Distance");

  if (distanceValid) {
    snprintf(line1, sizeof(line1), "Distance: %.1f cm", latestDistanceCm);
    snprintf(line2, sizeof(line2), "Last OK: %lu ms", lastValidDistanceMs);
    display.drawStr(0, 30, line1);
    display.drawStr(0, 46, line2);
  } else {
    display.drawStr(0, 30, "Distance: ---");
    display.drawStr(0, 46, "Status: No valid echo");
  }

  snprintf(line3, sizeof(line3), "Sample: %lu ms", SAMPLE_PERIOD_MS);
  display.drawStr(0, 62, line3);

  display.sendBuffer();
}

// change back to ultrasonicBegin()
void futureBegin() {
  Serial.begin(115200);
  Serial.println("Connecting...");

  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);

  pinMode(ECHO_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echoISR, CHANGE);

  display.begin();
  display.clearBuffer();
  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 12, "Starting...");
  display.sendBuffer();

  delay(300);
}

// reset back to ultrasonicUpdate()
void futureUpdate() {
  processUltrasonicResult();

  // update OLED at a reasonable rate
  static unsigned long lastDisplayMs = 0;
  if (millis() - lastDisplayMs >= 100) {
    lastDisplayMs = millis();
    drawDisplay();
  }

  // optional serial debug
  static unsigned long lastPrintMs = 0;
  if (millis() - lastPrintMs >= 200) {
    lastPrintMs = millis();

    if (distanceValid) {
      Serial.print("Distance: ");
      Serial.print(latestDistanceCm);
      Serial.println(" cm");
    } else {
      Serial.println("Distance: invalid");
    }
  }
}