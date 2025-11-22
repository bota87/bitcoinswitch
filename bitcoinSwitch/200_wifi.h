#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

#include "100_config.h"
#include "300_tft.h"

void setupWifi();
void loopWifi();