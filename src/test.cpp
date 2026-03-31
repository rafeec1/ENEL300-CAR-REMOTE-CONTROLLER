#include <Arduino.h>

 
// Left motor
#define LEFT_IN1   9
#define LEFT_IN2   10
#define LEFT_EN    13
 
// Right motor
#define RIGHT_IN1  11
#define RIGHT_IN2  12
#define RIGHT_EN   14

void setup() {
  Serial.begin(115200);

  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(LEFT_EN, OUTPUT);

  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);
  digitalWrite(LEFT_EN, HIGH);   // no PWM, just force enable
}

void loop() {}