#include "200_wifi.h"

void setupWifi()
{
  WiFi.begin(config_ssid.c_str(), config_password.c_str());
  Serial.print("Connecting to WiFi.");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
#ifdef LED_BUILTIN
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
#endif
  }
  #ifdef LED_BUILTIN
      digitalWrite(LED_BUILTIN, !LED_ON);
  #endif

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
