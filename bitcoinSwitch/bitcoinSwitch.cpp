#include "config-pandorone.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

String payloadStr;
bool paid, webSocketConnected;

WebSocketsClient webSocket;

void ElaboraPagamento();
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);

void setup()
{
    Serial.begin(115200);
    Serial.println("Welcome to BitcoinSwitch");

    // Aspetto 1 secondo e accendo la TV
    delay(1000);
    pinMode(tvPIN, OUTPUT);
    digitalWrite(tvPIN, HIGH);
    delay(500);
    digitalWrite(tvPIN, LOW);

    pinMode(LED_BUILTIN, OUTPUT); // To blink on board LED

    WiFi.begin(ssid.c_str(), wifiPassword.c_str());
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.print(".");
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
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

    if (!paid)
        return;

    paid = false;
    Serial.println("Paid");

    digitalWrite(LED_BUILTIN, LOW); // durante l'elaborazione del pagamento spengo il led
    ElaboraPagamento();
    digitalWrite(LED_BUILTIN, HIGH);
}

void ElaboraPagamento()
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

    pinMode(switchPIN, OUTPUT);
    digitalWrite(switchPIN, HIGH);
    delay(durationms);
    digitalWrite(switchPIN, LOW);
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
