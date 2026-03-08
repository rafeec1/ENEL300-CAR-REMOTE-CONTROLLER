#include <Arduino.h>

// ===== Pin mapping (edit these to match your wiring) =====
static const int ENA_PIN = 19;  // PWM for Motor A
static const int IN1_PIN = 41;
static const int IN2_PIN = 40;

static const int ENB_PIN = 20;  // PWM for Motor B
static const int IN3_PIN = 39;
static const int IN4_PIN = 38;

// ===== PWM config =====
static const int PWM_FREQ_HZ = 15000;
static const int PWM_RES_BITS = 8;      // 0..255
static const int PWM_MAX = (1 << PWM_RES_BITS) - 1;

class L298Motor {
public:
  L298Motor(int in1, int in2, int enPin, int pwmChannel)
    : _in1(in1), _in2(in2), _en(enPin), _ch(pwmChannel) {}

  void begin() {
    pinMode(_in1, OUTPUT);
    pinMode(_in2, OUTPUT);

    // Stop initially
    digitalWrite(_in1, LOW);
    digitalWrite(_in2, LOW);

    // Setup PWM
    ledcSetup(_ch, PWM_FREQ_HZ, PWM_RES_BITS);
    ledcAttachPin(_en, _ch);
    ledcWrite(_ch, 0);
  }

  // speed: -255..255 (sign = direction, magnitude = duty)
  void setSpeed(int speed) {
    speed = constrain(speed, -PWM_MAX, PWM_MAX);

    if (speed == 0) {
      // "Coast" stop (both inputs low)
      digitalWrite(_in1, LOW);
      digitalWrite(_in2, LOW);
      ledcWrite(_ch, 0);
      return;
    }

    int duty = abs(speed);

    if (speed > 0) {
      digitalWrite(_in1, HIGH);
      digitalWrite(_in2, LOW);
    } else {
      digitalWrite(_in1, LOW);
      digitalWrite(_in2, HIGH);
    }

    ledcWrite(_ch, duty);
  }

  // Optional: active braking (short the motor through the driver)
  void brake(int strength = PWM_MAX) {
    strength = constrain(strength, 0, PWM_MAX);
    digitalWrite(_in1, HIGH);
    digitalWrite(_in2, HIGH);
    ledcWrite(_ch, strength);
  }

private:
  int _in1, _in2, _en, _ch;
};

L298Motor motorA(IN1_PIN, IN2_PIN, ENA_PIN, 0);
L298Motor motorB(IN3_PIN, IN4_PIN, ENB_PIN, 1);

void setup() {
  Serial.begin(115200);
  delay(200);

  motorA.begin();
  motorB.begin();

  Serial.println("L298N + ESP32 demo starting...");
}

void loop() {
  // Simple functional test:
  // 1) ramp forward
  for (int s = 0; s <= 255; s += 5) {
    motorA.setSpeed(s);
    motorB.setSpeed(s);
    delay(30);
  }

  delay(400);

  // 2) stop
  motorA.setSpeed(0);
  motorB.setSpeed(0);
  delay(400);

  // 3) ramp reverse
  for (int s = 0; s >= -255; s -= 5) {
    motorA.setSpeed(s);
    motorB.setSpeed(s);
    delay(30);
  }

  delay(400);

  // 4) stop
  motorA.setSpeed(0);
  motorB.setSpeed(0);
  delay(800);
}