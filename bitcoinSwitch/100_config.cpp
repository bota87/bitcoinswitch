#include "100_config.h"

String config_ssid;
String config_password;
String config_device_string;
String config_ap_password;
int config_external_led;
int config_servo_pin;
int config_servo_open_degree;
int config_servo_close_degree;
int config_light_pin;
int config_alert_seconds_before_off;

#ifdef HARDCODED
void setupConfig()
{
  Serial.println("Setting hardcoded values...");
  config_ssid = CONFIG_SSID;
  Serial.println("SSID: " + config_ssid);
  config_password = CONFIG_PASSWORD;
  Serial.println("SSID password: " + config_password);
  config_device_string = CONFIG_DEVICE_STRING;
  Serial.println("Device string: " + config_device_string);

  showWelcomeScreen();
}
#else
void setupConfig()
{
  // first give the installer a chance to delete configuration file
  executeConfigBoot();

  showWelcomeScreen();

  if (!readConfig())
  {
    // file does not exist, so we will enter endless config mode
    Serial.println("Config file does not exist.");
    executeSerialConfigForever();
  }
}

bool readConfig()
{
  Preferences preferences;
  preferences.begin(CONFIG_NAME, true);

  if (!preferences.isKey("ssid"))
  {
    config_ssid = CONFIG_SSID;
    config_password = CONFIG_PASSWORD;
    config_device_string = CONFIG_DEVICE_STRING;
    config_ap_password = CONFIG_AP_PASSWORD;
    config_external_led = CONFIG_EXTERNAL_LED;
    config_servo_pin = CONFIG_SERVO_PIN;
    config_servo_open_degree = CONFIG_SERVO_OPEN_DEGREE;
    config_servo_close_degree = CONFIG_SERVO_CLOSE_DEGREE;
    config_light_pin = CONFIG_LIGHT_PIN;
    config_alert_seconds_before_off = CONFIG_ALERT_SECONDS_BEFORE_OFF;

    preferences.end();
    return false;
  }

  config_ssid = preferences.getString("ssid", CONFIG_SSID);
  config_password = preferences.getString("password", CONFIG_PASSWORD);
  config_device_string = preferences.getString("device_string", CONFIG_DEVICE_STRING);
  config_ap_password = preferences.getString("ap_password", CONFIG_AP_PASSWORD);
  config_external_led = preferences.getInt("external_led", CONFIG_EXTERNAL_LED);
  config_servo_pin = preferences.getInt("servo_pin", CONFIG_SERVO_PIN);
  config_servo_open_degree = preferences.getInt("servo_open", CONFIG_SERVO_OPEN_DEGREE);
  config_servo_close_degree = preferences.getInt("servo_close", CONFIG_SERVO_CLOSE_DEGREE);
  config_light_pin = preferences.getInt("light_pin", CONFIG_LIGHT_PIN);
  config_alert_seconds_before_off = preferences.getInt("alert_sec", CONFIG_ALERT_SECONDS_BEFORE_OFF);

  preferences.end();
  return true;
}

void executeConfigBoot()
{
  Serial.println("Entering boot mode. Waiting for " + String(BOOTUP_TIMEOUT) + " seconds.");
  clearTFT();
  printTFT("BOOT MODE", 21, 21);

  int counter = (BOOTUP_TIMEOUT + 1) * 10;
  while (counter-- > 0)
  {
#ifdef TOUCH_PIN
    int val = touchRead(TOUCH_PIN);
    Serial.println("Touch read value: " + String(val));
    if (val < 60)
    {
      Serial.println("Touch detected");
      executePortalConfig();
      return;
    }
#endif

#ifdef BT1_PIN
    if (digitalRead(BT1_PIN) == LOW)
    {
      Serial.println("Button pressed. Hold 5 seconds for factory reset.");
      int holdCount = 0;
      while (digitalRead(BT1_PIN) == LOW)
      {
        delay(100);
        holdCount++;
        if (holdCount >= 50)
        {
          Serial.println("Factory reset...");
          clearConfig();
          Serial.println("Factory reset done. Restarting...");
          ESP.restart();
          return;
        }
      }
      Serial.println("Button released, entering portal config.");
      executePortalConfig();
      return;
    }
#endif

    if (Serial.available() == 0)
    {
      delay(100);
      continue;
    }
    Serial.println();
    // if we get serial data in the first seconds, we will enter config mode
    counter = 0;
    executeSerialConfigForever();
    return;
  }

  Serial.println("Exiting boot mode.");
}

void clearConfig()
{
  Preferences preferences;
  preferences.begin(CONFIG_NAME, false);
  preferences.clear();
  preferences.end();

  readConfig();
}

void saveConfig()
{
  Preferences preferences;
  preferences.begin(CONFIG_NAME, false);

  preferences.putString("ssid", config_ssid);
  preferences.putString("password", config_password);
  preferences.putString("device_string", config_device_string);
  preferences.putString("ap_password", config_ap_password);
  preferences.putInt("external_led", config_external_led);
  preferences.putInt("servo_pin", config_servo_pin);
  preferences.putInt("servo_open", config_servo_open_degree);
  preferences.putInt("servo_close", config_servo_close_degree);
  preferences.putInt("light_pin", config_light_pin);
  preferences.putInt("alert_sec", config_alert_seconds_before_off);

  preferences.end();
}

#endif

void showWelcomeScreen()
{
  Serial.print("Welcome to BitcoinSwitch!");
  Serial.println(" (" + String(VERSION) + ")");
  clearTFT();
  printTFT("BitcoinSwitch", 21, 21);
  printTFT(String(VERSION), 21, 42);
}