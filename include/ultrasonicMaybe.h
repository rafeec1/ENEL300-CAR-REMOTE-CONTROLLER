#ifndef ULTRASONIC_MAYBE_H
#define ULTRASONIC_MAYBE_H

void distanceBegin();
void distanceUpdate();

#pragma once
extern volatile float distanceCm;

#endif