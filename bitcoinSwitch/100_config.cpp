#include "100_config.h"

String config_ssid;
String config_password;
String config_device_string;

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
    
    preferences.end();
    return false;
  }

  config_ssid = preferences.getString("ssid", CONFIG_SSID);
  config_password = preferences.getString("password", CONFIG_PASSWORD);
  config_device_string = preferences.getString("device_string", CONFIG_DEVICE_STRING);

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
    pinMode(BT1_PIN, INPUT_PULLUP);
    if (digitalRead(BT1_PIN) == LOW)
    {
      Serial.println("Button pressed");
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