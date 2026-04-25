#include "100_config.h"
#include "200_wifi.h"
#include "201_web_socket.h"
#include "300_tft.h"

void setup()
{
  Serial.begin(115200);
// Serial.setDebugOutput(true);
#ifdef TFT
  setupTFT();
  printHome(false, false, false);
#endif

#ifdef BT1_PIN
  pinMode(BT1_PIN, INPUT_PULLUP);
#endif

#ifndef HARDCODED
  checkFactoryReset();
#endif

  setupConfig();
  setupWifi();
  setupWebSocket();
}

void loop()
{
  loopWifi();
  loopWebSocket();
  delay(10); // to allow background processes
}