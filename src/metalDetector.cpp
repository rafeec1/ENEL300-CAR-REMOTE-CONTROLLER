/*
 * ESP32-S3 Metal Detector (Instructables Translation)
 * PWM (Pulse Source): GPIO 18
 * ADC (Sense Pin):   GPIO 8 (ADC1_CH7)
 * Active Buzzer:     GPIO 5
 */

#include <Arduino.h>
const int pulsePin = 18;
const int sensePin = 8;
const int buzzerPin = 5;

// Tuning parameters
const int nPulses = 250;      // Number of pulses to "charge" the LC filter
const int threshold = 100;     // Sensitivity: adjust based on Serial Plotter
float runningAverage = 0;     // Long-term baseline (auto-calibration)
const float alpha = 0.02;     // How fast the baseline follows environmental drift


// core logic: pulse the coil and read the integrated voltage
int measureInductance() {
  // Start the high-frequency pulses
  ledcWrite(0, 128); // 50% duty cycle
  
  // Wait a very short moment for the capacitor to charge
  // On ESP32-S3, this happens very fast
  delayMicroseconds(200); 

  int val = analogRead(sensePin);

  // Stop pulses to "reset" for the next cycle
  ledcWrite(0, 0); 
  
  return val;
}

void metalBegin() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // Setup PWM (LEDC) to act as our high-frequency driver
  ledcSetup(0, 100000, 8); // 100 kHz, 8-bit
  ledcAttachPin(pulsePin, 0);

  // Initial calibration
  Serial.println("Calibrating... keep coil away from metal.");
  delay(1000);
  long sum = 0;
  for(int i = 0; i < 100; i++) {
    sum += measureInductance();
    delay(5);
  }
  runningAverage = sum / 100.0;
  Serial.print("Baseline set to: ");
  Serial.println(runningAverage);
}


void metalUpdate() {
  int val = measureInductance();

  // Update running average (auto-tunes the baseline)
  runningAverage = (val * alpha) + (runningAverage * (1.0 - alpha));

  float difference = abs(val - runningAverage);

  // Detection logic
  if (difference > threshold) {
    digitalWrite(buzzerPin, HIGH); // Turn on active buzzer
    Serial.print("!!! METAL !!! Diff: ");
  } else {
    digitalWrite(buzzerPin, LOW);  // Turn off active buzzer
    Serial.print("Scan: ");
  }

  Serial.println(difference);
  delay(20); 
}