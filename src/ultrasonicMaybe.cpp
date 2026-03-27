/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-hc-sr04-ultrasonic-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <Arduino.h>

const int trigPin = 13;
const int echoPin = 14;
const int ledPin = 12;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define SAMPLE_PERIOD 1000

long duration;
float distanceCm;
unsigned long currentTime = 0;

void distanceBegin() {
  Serial.println("UltrasonicSensor begin...");
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(ledPin, OUTPUT);
  
  digitalWrite(trigPin, LOW);
}

void distanceUpdate() {
  //after 50ms passed, sample a new measurement
  unsigned long timePassed = millis();
  if (timePassed - currentTime >= SAMPLE_PERIOD) { // check if the interval has passed
    currentTime = timePassed; 
    digitalWrite(ledPin, !digitalRead(ledPin));
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);          // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH, 30000);
    // Calculate the distance
    if (duration > 0) {
      distanceCm = duration * SOUND_SPEED/2;    
      Serial.print("Distance (cm): ");
      Serial.println(distanceCm);
    } else {
      Serial.println("No Echo");
    }
    
    }
}