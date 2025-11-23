#include "101_serial_config.h"

#ifndef HARDCODED
String fileContent = "";

void executeSerialConfigForever()
{
  Serial.println("Entering config mode. until we receive /config-done.");
  clearTFT();
  printTFT("SERIAL CONFIG", 21, 21);
  bool done = false;
  while (true)
  {
    done = executeSerialConfig();
    if (done)
    {
      Serial.println("Exiting config mode.");
      return;
    }
  }
}

bool executeSerialConfig()
{
  if (Serial.available() == 0)
    return false;
  String data = Serial.readStringUntil('\n');
  Serial.println("received serial data: " + data);
  if (data == "/config-done")
  {
    if (fileContent.length() > 0)
      saveConfigFromFile();

    delay(1000);
    return true;
  }
  if (data == "/file-remove")
  {
    clearConfig();
    fileContent = "";
  }
  if (data.startsWith("/file-append"))
  {
    int pos = data.indexOf(" ");
    String jsondata = data.substring(pos + 1);
    fileContent += jsondata;
  }
  if (data.startsWith("/file-read"))
  {
    fileContent = readConfigToFile();

    int start = 0;
    while (start < fileContent.length())
    {
      int end = fileContent.indexOf('\n', start);
      if (end == -1)
        end = fileContent.length();
      String line = fileContent.substring(start, end);
      Serial.println("/file-send " + line);
      start = end + 1;
    }

    fileContent = "";
    Serial.println("/file-done");
    return false;
  }
  return false;
}

String readConfigToFile()
{
  readConfig();

  String content = "";

  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();

  JsonObject ssidObj = arr.add<JsonObject>();
  ssidObj["name"] = "config_ssid";
  ssidObj["value"] = config_ssid;

  JsonObject pwObj = arr.add<JsonObject>();
  pwObj["name"] = "config_password";
  pwObj["value"] = config_password;

  JsonObject devStrObj = arr.add<JsonObject>();
  devStrObj["name"] = "config_device_string";
  devStrObj["value"] = config_device_string;

  serializeJsonPretty(doc, content);
  content += "\n";
  return content;
}

void saveConfigFromFile()
{
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, fileContent);
  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  }

  for (JsonObject elem : doc.as<JsonArray>())
  {
    String key = elem["name"].as<String>();

    if (key == "config_ssid")
      config_ssid = elem["value"].as<String>();
    else if (key == "config_password")
      config_password = elem["value"].as<String>();
    else if (key == "config_device_string")
      config_device_string = elem["value"].as<String>();
    else
      Serial.println("Unknown config key: " + key);
  }

  saveConfig();
}
#endif