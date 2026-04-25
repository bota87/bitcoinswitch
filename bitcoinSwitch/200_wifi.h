#pragma once

#include <Arduino.h>
#include <WiFi.h>

#include "100_config.h"
#include "300_tft.h"

void initStatusLeds();
void setStatusLed(bool on);
void toggleStatusLed();
void setupWifi();
void loopWifi();