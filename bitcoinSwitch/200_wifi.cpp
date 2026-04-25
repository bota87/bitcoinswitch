#include "200_wifi.h"

void initStatusLeds()
{
#if defined(LED_BUILTIN) && defined(LED_ON)
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, !LED_ON); // off
#endif
  if (config_external_led >= 0)
  {
    pinMode(config_external_led, OUTPUT);
    digitalWrite(config_external_led, HIGH); // off (active-low)
  }
}

void setStatusLed(bool on)
{
#if defined(LED_BUILTIN) && defined(LED_ON)
  digitalWrite(LED_BUILTIN, on ? LED_ON : !LED_ON);
#endif
  if (config_external_led >= 0)
  {
    digitalWrite(config_external_led, on ? LOW : HIGH);
  }
}

void toggleStatusLed()
{
#ifdef LED_BUILTIN
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
#endif
  if (config_external_led >= 0)
  {
    digitalWrite(config_external_led, !digitalRead(config_external_led));
  }
}

void setupWifi()
{
  initStatusLeds();
  WiFi.begin(config_ssid.c_str(), config_password.c_str());
  Serial.print("Connecting to WiFi.");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
    toggleStatusLed();
  }
  setStatusLed(false);

  Serial.println();
  Serial.println("WiFi connection etablished!");
  printHome(true, false, false);
}

void loopWifi()
{
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi disconnected!");
    printHome(false, false, false);
    delay(500);
    setupWifi();
  }
}
