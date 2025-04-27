#include "config-biliardino.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <Servo.h>

String payloadStr = "";
bool paid = false, webSocketConnected = false;

unsigned long lightOffTime = 0;
unsigned long lightAlertTime = 0;

WebSocketsClient webSocket;
Servo servo1;

void ElaboraPagamento(bool &openDoor, bool &turnLightOn, int &durationms);
void ControllaSpengimentoLuci();
void OpenDoor();
void TurnLightOn(int durationms);
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);

void setup()
{
    Serial.begin(115200);
    Serial.println("Welcome to BitcoinSwitch");

    pinMode(LED_BUILTIN, OUTPUT);   // To blink on board LED
    pinMode(openPin, INPUT_PULLUP); // bottone per simulare il pagamento del gettone e dell'illuminazione
    pinMode(lightPin, OUTPUT);      // per accendere le luci

    if (servo1.attach(servoPin))
        Serial.println("Servo attached on pin: " + String(servoPin));
    else
    {
        Serial.println("ERROR: failed to attach servo on pin " + String(servoPin));
        while (true) // se non ho trovato il servo blocco l'esecuzione del programma e faccio lampeggiare velocemente il led
        {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(100);
            digitalWrite(LED_BUILTIN, LOW);
            delay(100);
        }
    }

    WiFi.begin(ssid.c_str(), wifiPassword.c_str());
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        Serial.print(".");
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }

    // Use in threshold mode
    String apiUrl = "/api/v1/ws/";
    Serial.println("");
    Serial.println("Connecting to websocket: " + lnbitsServer + apiUrl + WalletReadKey);
    webSocket.beginSSL(lnbitsServer, 443, apiUrl + WalletReadKey);

    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(1000);
}

void loop()
{
    webSocket.loop();
    digitalWrite(LED_BUILTIN, webSocketConnected); // il led rappresenta lo stato del web socket

    bool openDoor = false, turnLightOn = false;
    int durationms = 0;

    if (paid)
    {
        paid = false;
        Serial.println("Paid");

        digitalWrite(LED_BUILTIN, LOW); // durante l'elaborazione del pagamento spengo il led (si riaccende all'inizio del loop con lo stato del websocket)
        ElaboraPagamento(/*byref*/ openDoor, /*byref*/ turnLightOn, /*byref*/ durationms);
    }
    else if (digitalRead(openPin) == LOW)
    {
        Serial.println("Forzata apertura e illuminazione");
        openDoor = true;
        turnLightOn = true;
        durationms = (10 + alertSecondsBeforeOff) * 1000;
    }

    ControllaSpengimentoLuci();

    if (turnLightOn)
        TurnLightOn(durationms);

    if (openDoor)
        OpenDoor();
}

void ControllaSpengimentoLuci()
{
    if (lightOffTime == 0)
        return;
    unsigned int time = millis();
    if (lightAlertTime > 0 && time > lightAlertTime)
    {
        Serial.println("Tra " + String(alertSecondsBeforeOff) + " secondi spengo le luci");
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

void ElaboraPagamento(bool &openDoor, bool &turnLightOn, int &durationms)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payloadStr);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }
    payloadStr = "";
    JsonObject payment = doc["payment"];
    JsonObject extra = payment["extra"];
    double paidAmount = extra["wallet_fiat_amount"];
    Serial.println("paidAmount: " + String(paidAmount));
    String lnaddress = extra["lnaddress"];
    Serial.println("lnaddress: " + lnaddress);

    if (lnaddress.startsWith(lnAddressGettone + "@"))
        openDoor = true;
    else
    {
        turnLightOn = true;
        durationms = (paidAmount / lightPricePerMinute) * 60 * 1000;
        Serial.println("durationms: " + String(durationms));
    }
}

void TurnLightOn(int durationms)
{
    if (lightOffTime == 0)
        lightOffTime = millis();
    lightOffTime += durationms;
    lightAlertTime = lightOffTime - alertSecondsBeforeOff * 1000;
    Serial.println("Accendo le luci per " + String(durationms) + " ms");
    digitalWrite(lightPin, HIGH);
    delay(500);
}

void OpenDoor()
{
    servo1.write(openDegree);
    Serial.println("Servo aperto: " + String(openDegree));

    delay(2000);

    servo1.write(closeDegree);
    Serial.println("Servo chiuso: " + String(closeDegree));
}

//////////////////WEBSOCKET///////////////////

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.printf("[WSc] Disconnected!\n");
        webSocketConnected = false;
        break;
    case WStype_CONNECTED:
        Serial.printf("[WSc] Connected to url: %s\n", payload);
        webSocket.sendTXT("Connected"); // send message to server when Connected
        webSocketConnected = true;
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
