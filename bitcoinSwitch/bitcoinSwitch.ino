///////////////////////////////////////////////////////////////////////////////////
//         Change these variables directly in the code or use the config         //
//  form in the web-installer https://lnbits.github.io/bitcoinswitch/installer/  //
///////////////////////////////////////////////////////////////////////////////////

String version = "0.1.1";

#include <config.h>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <Servo.h>

fs::SPIFFSFS &FlashFS = SPIFFS;
#define FORMAT_ON_FAIL true
#define PARAM_FILE "/elements.json"

String urlPrefix = "ws://";
String apiUrl = "/api/v1/ws/";

// length of lnurldevice id
// 7dhdyJ9bbZNWNVPiFSdmb5
int uidLength = 22;

String payloadStr;
String lnbitsServer;
String deviceId;
String dataId;
bool paid;
bool down = false;
long thresholdSum = 0;
long payment_amount = 0;
unsigned long lightOffTime = 0;   // 0 = light off;
unsigned long lightAlertTime = 0; // 0 = light off;

// Serial config
int portalPin = 4;

WebSocketsClient webSocket;
Servo servo1;

struct KeyValue
{
    String key;
    String value;
};

void setup()
{
    Serial.begin(115200);
    Serial.println("Welcome to BitcoinSwitch, running on version: " + version);
    bool triggerConfig = false;
    pinMode(2, OUTPUT); // To blink on board LED
    pinMode(ledPin, OUTPUT);
    FlashFS.begin(FORMAT_ON_FAIL);
    // int timer = 0;
    // while (timer < 2000)
    // {
    //     digitalWrite(2, HIGH);
    //     Serial.println(touchRead(portalPin));
    //     if (touchRead(portalPin) < 60)
    //     {
    //         triggerConfig = true;
    //         timer = 5000;
    //     }

    //     timer = timer + 100;
    //     delay(150);
    //     digitalWrite(2, LOW);
    //     delay(150);
    // }

    readFiles(); // get the saved details and store in global variables

    if (triggerConfig == true || ssid == "" || ssid == "null")
    {
        Serial.println("Launch serial config");
        configOverSerialPort();
    }
    else
    {
        WiFi.begin(ssid.c_str(), wifiPassword.c_str());
        Serial.print("Connecting to WiFi");
        while (WiFi.status() != WL_CONNECTED)
        {
            Serial.print(".");
            delay(500);
            digitalWrite(2, HIGH);
            digitalWrite(ledPin, HIGH);
            Serial.print(".");
            delay(500);
            digitalWrite(2, LOW);
            digitalWrite(ledPin, LOW);
        }
        Serial.println("");
    }

    if (servo1.attach(servoPin))
        Serial.println("Servo attached on pin: " + String(servoPin));
    else
        Serial.println("ERROR: failed to attach servo on pin " + String(servoPin));

    if (threshold != 0)
    { // Use in threshold mode
        Serial.println("Using threshold mode");
        Serial.println("Connecting to websocket: " + urlPrefix + lnbitsServer + apiUrl + wallet);
        webSocket.beginSSL(lnbitsServer, 443, apiUrl + wallet);
    }
    else
    { // Use in normal mode
        Serial.println("Using normal mode");
        Serial.println("Connecting to websocket: " + urlPrefix + lnbitsServer + apiUrl + deviceId);
        webSocket.beginSSL(lnbitsServer, 443, apiUrl + deviceId);
    }
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(1000);

    pinMode(openPin, INPUT_PULLUP);
    pinMode(lightPin, OUTPUT);
}

void loop()
{
    while (WiFi.status() != WL_CONNECTED)
    { // check wifi again
        Serial.println("Failed to connect");
        delay(500);
    }
    digitalWrite(2, LOW);
    digitalWrite(ledPin, LOW);
    payloadStr = "";
    delay(2000);

    bool openDoor = false;
    bool turnLightOn = false;

    while (paid == false)
    {
        int duration = 0;
        if (digitalRead(openPin) == LOW)
        {
            Serial.println("Forzata apertura e illuminazione per 1 minuto");
            openDoor = true;
            turnLightOn = true;
            duration = 60 * 1000;
        }
        if (lightOffTime > 0)
        {
            unsigned int time = millis();
            if (lightAlertTime > 0 && time > lightAlertTime)
            {
                Serial.println("Tra " + String(alertSecondsBerforeOff) + " secondi spengo le luci");
                digitalWrite(lightPin, LOW);
                delay(500);
                digitalWrite(lightPin, HIGH);
                lightAlertTime = 0;
            }
            else if (time > lightOffTime)
            {
                Serial.println("Spengo le luci");
                digitalWrite(lightPin, LOW);
                lightOffTime = 0;
            }
        }
        webSocket.loop();
        if (paid)
        {
            if (getValue(payloadStr, '-', 0).toInt() == lightPin)
                turnLightOn = true;
            else
                openDoor = true;
            duration = getValue(payloadStr, '-', 1).toInt();
        }
        if (turnLightOn)
        {
            TurnLightOn(duration);
            turnLightOn = false;
        }
        if (openDoor)
        {
            OpenDoor();
            openDoor = false;
        }
    }
    Serial.println("Paid");
    paid = false;
}

void TurnLightOn(int duration)
{
    if (lightOffTime == 0)
        lightOffTime = millis();
    lightOffTime += duration;
    lightAlertTime = lightOffTime - alertSecondsBerforeOff * 1000;
    Serial.println("Accendo le luci per " + String(duration) + " ms");
    digitalWrite(lightPin, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(2, LOW);
    digitalWrite(ledPin, LOW);
}

void OpenDoor()
{
    digitalWrite(2, HIGH);
    digitalWrite(ledPin, HIGH);
    servo1.write(openDegree);
    Serial.println("Servo aperto: " + String(openDegree));

    delay(2000);

    servo1.write(closeDegree);
    digitalWrite(2, LOW);
    digitalWrite(ledPin, LOW);
    Serial.println("Servo chiuso: " + String(closeDegree));
}

//////////////////HELPERS///////////////////

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (data.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String getJsonValue(JsonDocument &doc, const char *name)
{
    for (JsonObject elem : doc.as<JsonArray>())
    {
        if (strcmp(elem["name"], name) == 0)
        {
            String value = elem["value"].as<String>();
            return value;
        }
    }
    return ""; // return empty string if not found
}

void readFiles()
{
    File paramFile = FlashFS.open(PARAM_FILE, "r");
    if (paramFile)
    {
        StaticJsonDocument<2500> doc;
        DeserializationError error = deserializeJson(doc, paramFile.readString());
        if (error)
        {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
        }
        if (ssid == "null")
        { // check ssid is not set above
            ssid = getJsonValue(doc, "ssid");
            Serial.println("");
            Serial.println("ssid used from memory");
            Serial.println("SSID: " + ssid);
        }
        else
        {
            Serial.println("");
            Serial.println("ssid hardcoded");
            Serial.println("SSID: " + ssid);
        }
        if (wifiPassword == "null")
        { // check wifiPassword is not set above
            wifiPassword = getJsonValue(doc, "wifipassword");
            Serial.println("");
            Serial.println("ssid password used from memory");
            Serial.println("SSID password: " + wifiPassword);
        }
        else
        {
            Serial.println("");
            Serial.println("ssid password hardcoded");
            Serial.println("SSID password: " + wifiPassword);
        }
        if (switchStr == "null")
        { // check switchStr is not set above
            switchStr = getJsonValue(doc, "socket");
            Serial.println("");
            Serial.println("switchStr used from memory");
            Serial.println("switchStr: " + switchStr);
        }
        else
        {
            Serial.println("");
            Serial.println("switchStr hardcoded");
            Serial.println("switchStr: " + switchStr);
        }

        int protocolIndex = switchStr.indexOf("://");
        if (protocolIndex == -1)
        {
            Serial.println("Invalid switchStr: " + switchStr);
            return;
        }
        urlPrefix = switchStr.substring(0, protocolIndex + 3);

        int domainIndex = switchStr.indexOf("/", protocolIndex + 3);
        if (domainIndex == -1)
        {
            Serial.println("Invalid switchStr: " + switchStr);
            return;
        }

        lnbitsServer = switchStr.substring(protocolIndex + 3, domainIndex);
        apiUrl = switchStr.substring(domainIndex, switchStr.length() - uidLength);
        deviceId = switchStr.substring(switchStr.length() - uidLength);

        Serial.println("LNbits ws prefix: " + urlPrefix);
        Serial.println("LNbits server: " + lnbitsServer);
        Serial.println("LNbits api url: " + apiUrl);
        Serial.println("Switch device ID: " + deviceId);
    }
    paramFile.close();
}

//////////////////WEBSOCKET///////////////////

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.printf("[WSc] Disconnected!\n");
        break;
    case WStype_CONNECTED:
        Serial.printf("[WSc] Connected to url: %s\n", payload);
        webSocket.sendTXT("Connected"); // send message to server when Connected
        break;
    case WStype_TEXT:
        payloadStr = (char *)payload;
        payloadStr.replace(String("'"), String('"'));
        payloadStr.toLowerCase();
        Serial.println("Received data from socket: " + payloadStr);
        paid = true;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
        break;
    }
}
