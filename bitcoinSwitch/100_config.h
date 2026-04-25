#pragma once

#define VERSION "v1.1.0"

#define BOOTUP_TIMEOUT 2 // seconds
#define CONFIG_NAME "config"

// uncomment if you dont want to use the configuration file
// #define HARDCODED

// device specific configuration / defaults
#define CONFIG_SSID "mywifi"
#define CONFIG_PASSWORD "mypw"
#define CONFIG_DEVICE_STRING ""
#define CONFIG_AP_PASSWORD ""
#define CONFIG_EXTERNAL_LED -1

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
extern String config_ap_password;
extern int config_external_led;
void showWelcomeScreen();
void setupConfig();

#ifndef HARDCODED
bool readConfig();
void executeConfigBoot();
void clearConfig();
void saveConfig();
void checkFactoryReset();
#endif