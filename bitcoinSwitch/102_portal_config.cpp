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

  WiFiManagerParameter external_led_field("config_external_led", "External LED pin (-1 = disabled)", String(config_external_led).c_str(), 5, "type='number' min='-1' max='48'");
  wm.addParameter(&external_led_field);

  WiFiManagerParameter servo_pin_field("config_servo_pin", "Servo pin", String(config_servo_pin).c_str(), 5, "type='number' min='0' max='48'");
  wm.addParameter(&servo_pin_field);

  WiFiManagerParameter servo_open_degree_field("config_servo_open_degree", "Servo open degree", String(config_servo_open_degree).c_str(), 5, "type='number' min='0' max='360'");
  wm.addParameter(&servo_open_degree_field);

  WiFiManagerParameter servo_close_degree_field("config_servo_close_degree", "Servo close degree", String(config_servo_close_degree).c_str(), 5, "type='number' min='0' max='360'");
  wm.addParameter(&servo_close_degree_field);

  WiFiManagerParameter light_pin_field("config_light_pin", "Light pin", String(config_light_pin).c_str(), 5, "type='number' min='0' max='48'");
  wm.addParameter(&light_pin_field);

  WiFiManagerParameter alert_seconds_field("config_alert_seconds_before_off", "Alert seconds before off", String(config_alert_seconds_before_off).c_str(), 5, "type='number' min='0' max='120'");
  wm.addParameter(&alert_seconds_field);

  wm.setSaveConfigCallback([&wm]()
                           { saveWiFi(wm); });
  wm.setSaveParamsCallback([&device_string_field, &ap_password_field, &external_led_field, &servo_pin_field, &servo_open_degree_field, &servo_close_degree_field, &light_pin_field, &alert_seconds_field]()
                           { saveParams(device_string_field, ap_password_field, external_led_field, servo_pin_field, servo_open_degree_field, servo_close_degree_field, light_pin_field, alert_seconds_field); });

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

void saveParams(WiFiManagerParameter device_string_field, WiFiManagerParameter ap_password_field, WiFiManagerParameter external_led_field, WiFiManagerParameter servo_pin_field, WiFiManagerParameter servo_open_degree_field, WiFiManagerParameter servo_close_degree_field, WiFiManagerParameter light_pin_field, WiFiManagerParameter alert_seconds_field)
{
  Serial.println("Save device string");
  config_device_string = String(device_string_field.getValue());

  Serial.println("Save AP password");
  config_ap_password = String(ap_password_field.getValue());

  Serial.println("Save external LED pin");
  config_external_led = String(external_led_field.getValue()).toInt();

  Serial.println("Save servo pin");
  config_servo_pin = String(servo_pin_field.getValue()).toInt();

  Serial.println("Save servo open degree");
  config_servo_open_degree = String(servo_open_degree_field.getValue()).toInt();

  Serial.println("Save servo close degree");
  config_servo_close_degree = String(servo_close_degree_field.getValue()).toInt();

  Serial.println("Save light pin");
  config_light_pin = String(light_pin_field.getValue()).toInt();

  Serial.println("Save alert seconds before off");
  config_alert_seconds_before_off = String(alert_seconds_field.getValue()).toInt();

  saveConfig();
}
#endif