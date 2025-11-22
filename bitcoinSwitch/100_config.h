#pragma once

#include <Arduino.h>

#include "300_tft.h"
#ifndef HARDCODED
#include <Preferences.h>
#include "101_serial_config.h"
#include "102_portal_config.h"
#endif

extern String config_ssid;
extern String config_password;
extern String config_device_string;

void showWelcomeScreen();
void setupConfig();

#ifndef HARDCODED
bool readConfig();
void executeConfigBoot();
void clearConfig();
void saveConfig();
#endif