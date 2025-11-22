#pragma once

#ifndef HARDCODED
#include <Arduino.h>
#include <ArduinoJson.h>
#include "100_config.h"
#include "300_tft.h"

void executeSerialConfigForever();
bool executeSerialConfig();
String readConfigToFile();
void saveConfigFromFile();
void executePortalConfig();
#endif