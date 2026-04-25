#pragma once

#ifndef HARDCODED
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include "100_config.h"
#include "300_tft.h"

void executePortalConfig();
void saveWiFi(WiFiManager &wm);
void saveParams(WiFiManagerParameter device_string_field, WiFiManagerParameter ap_password_field, WiFiManagerParameter external_led_field);
#endif