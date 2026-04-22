/*
 * ESP32-S3 Metal Detector (Instructables Translation)
 * PWM (Pulse Source): GPIO 18
 * ADC (Sense Pin):   GPIO 8 (ADC1_CH7)
 * Active Buzzer:     GPIO 5
 */

#include <Arduino.h>

#define PRINT_PERIOD 1000

const int pulsePin = 18;  // NEW: PWM: 35 
const int sensePin = 8;   // NEW: ADC1 7
const int buzzerPin = 5;

// Tuning parameters
const int threshold = 100;     // Sensitivity: adjust based on Serial Plotter
float runningAverage = 0;     // Long-term baseline (auto-calibration)
const float alpha = 0.001;     // How fast the baseline follows environmental drift
unsigned long counter = 0;

const int nPulses = 256;      // Number of pulses to "charge" the LC filter
int sumsum=0; //running sum of 64 sums 
int skip=0;   //number of skipped sums
int diff=0;        //difference between sum and avgsum
int flash_period=0;//period (in ms) 
unsigned int prev_flash=0; //time stamp of previous flash


// core logic: pulse the coil and read the integrated voltage
int measureInductance() {
  // Start the high-frequency pulses
  ledcWrite(0, 128); // 50% duty cycle
  
  // Wait a very short moment for the capacitor to charge
  // On ESP32-S3, this happens very fast
  delayMicroseconds(3); 
  ledcWrite(0,0);
  delayMicroseconds(3);

  int val = analogRead(sensePin);

  
  return val;
}

void metalBegin() {
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
  unsigned long timeStart = millis();
  int val = measureInductance();

  // Update running average (auto-tunes the baseline)
  runningAverage = (val * alpha) + (runningAverage * (1.0 - alpha));

  float difference = abs(val - runningAverage);

  // Detection logic
  if (difference > threshold) {
    digitalWrite(buzzerPin, HIGH); // Turn on active buzzer
  } else {
    digitalWrite(buzzerPin, LOW);  // Turn off active buzzer
  }
  
  if (timeStart - counter >= PRINT_PERIOD) {
    counter = timeStart;
    if (difference >= threshold) {
      Serial.print("!!! METAL !!! Diff: ");
    } else {
      Serial.print("Scan: ");
    }
    Serial.println(difference);
  } 

  delay(20); 
}