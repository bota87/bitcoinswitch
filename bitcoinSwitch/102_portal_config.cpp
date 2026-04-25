#include "102_portal_config.h"

#ifndef HARDCODED

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

  WiFiManagerParameter ap_password_field("config_ap_password", "Config portal password (leave empty for open AP)", config_ap_password.c_str(), 64, "type='password'");
  wm.addParameter(&ap_password_field);

  WiFiManagerParameter device_string_field("config_device_string", "Device string", config_device_string.c_str(), 200, "placeholder=\"wss://\"");
  wm.addParameter(&device_string_field);
  wm.setSaveConfigCallback([&wm]()
                           { saveWiFi(wm); });
  wm.setSaveParamsCallback([&device_string_field, &ap_password_field]()
                           { saveParams(device_string_field, ap_password_field); });

  Serial.println("Starting config portal...");
  wm.startConfigPortal(apName.c_str(), config_ap_password.c_str());
}

void saveWiFi(WiFiManager &wm)
{
  Serial.println("Save WiFi settings");
  config_ssid = wm.getWiFiSSID();
  config_password = wm.getWiFiPass();
  saveConfig();
}

void saveParams(WiFiManagerParameter device_string_field, WiFiManagerParameter ap_password_field)
{
  Serial.println("Save device string");
  config_device_string = String(device_string_field.getValue());

  Serial.println("Save AP password");
  config_ap_password = String(ap_password_field.getValue());
  
  saveConfig();
}
#endif