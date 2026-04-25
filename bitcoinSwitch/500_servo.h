#pragma once

#include <Arduino.h>
#include <ESP32Servo.h>
#include "100_config.h"
#include "200_wifi.h"

void setupServo();
void handleServoPulse(int pin, int durationMs);