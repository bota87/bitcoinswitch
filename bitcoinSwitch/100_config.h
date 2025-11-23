#pragma once

#define VERSION "v1.0.1"

#define BOOTUP_TIMEOUT 2 // seconds
#define CONFIG_NAME "config"

// uncomment if you dont want to use the configuration file
// #define HARDCODED

// device specific configuration / defaults
#define CONFIG_SSID "mywifi"
#define CONFIG_PASSWORD "mypw"
#define CONFIG_DEVICE_STRING ""

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