#include "102_portal_config.h"

#define AP_PASSWORD "111222333"

void executePortalConfig()
{
  String apName = "BitcoinSwitch-" + String(WIFI_getChipId());

  Serial.println("Entering portal config mode. Connect to AP: " + apName);
  clearTFT();
  printTFT("PORTAL CONFIG", 21, 21);
  printTFT("Connect to AP", 21, 51);
  printTFT(apName, 21, 81);

  WiFiManager wm;
  wm.setTitle("Bitcoin Switch");
  wm.setBreakAfterConfig(true);
  wm.setParamsPage(true);
  wm.setShowInfoUpdate(true);
  wm.setDarkMode(true);
  wm.setConfigPortalTimeout(0);
  wm.setConnectTimeout(15);

  readConfig();
  WiFiManagerParameter device_string_field("config_device_string", "Device string", config_device_string.c_str(), 200, "placeholder=\"wss://\"");
  wm.addParameter(&device_string_field);
  wm.setSaveConfigCallback([&wm]()
                           { saveWiFi(wm); });
  wm.setSaveParamsCallback([&device_string_field]()
                           { saveParams(device_string_field); });

  Serial.println("Starting config portal...");
  wm.startConfigPortal(apName.c_str(), AP_PASSWORD);
}

void saveWiFi(WiFiManager &wm)
{
  Serial.println("Save WiFi settings");
  config_ssid = wm.getWiFiSSID();
  config_password = wm.getWiFiPass();
  saveConfig();
}

void saveParams(WiFiManagerParameter device_string_field)
{
  Serial.println("Save device string");
  config_device_string = String(device_string_field.getValue());
  saveConfig();
}