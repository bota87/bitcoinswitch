#include "config.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

String urlPrefix = "ws://";
String apiUrl = "/api/v1/ws/";

struct Coin
{
    double value;
    int gpio;
};

// Dichiarazione delle monete con relative GPIO
Coin coins[] = {
    {1.00, 4},
    {0.50, 17},
    {0.20, 15},
    {0.05, 16}};

// length of lnurldevice id
// 7dhdyJ9bbZNWNVPiFSdmb5
int uidLength = 22;

String payloadStr;
String lnbitsServer;
bool paid;

WebSocketsClient webSocket;

struct KeyValue
{
    String key;
    String value;
};

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
void readFiles();
void inviaImporto(double amount);
void inviaMoneta(Coin &coin);

void setup()
{
    Serial.begin(115200);
    Serial.println("Welcome to BitcoinSwitch");

    // Imposta i pin che simulano la gettoniera come output
    for (Coin &coin : coins)
    {
        pinMode(coin.gpio, OUTPUT);
        digitalWrite(coin.gpio, HIGH);
    }

    pinMode(2, OUTPUT); // To blink on board LED
    readFiles();        // get the saved details and store in global variables

    WiFi.begin(ssid.c_str(), wifiPassword.c_str());
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
        digitalWrite(2, HIGH);
        Serial.print(".");
        delay(500);
        digitalWrite(2, LOW);
    }

    // Use in threshold mode
    Serial.println("");
    Serial.println("Using THRESHOLD mode");
    Serial.println("Connecting to websocket: " + urlPrefix + lnbitsServer + apiUrl + WalletReadKey);
    webSocket.beginSSL(lnbitsServer, 443, apiUrl + WalletReadKey);

    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(1000);
}

void loop()
{
    while (WiFi.status() != WL_CONNECTED)
    { // check wifi again
        Serial.println("Failed to connect");
        delay(500);
    }
    digitalWrite(2, LOW);
    payloadStr = "";
    delay(500);
    while (!paid)
    { // loop and wait for payment
        webSocket.loop();
        if (!paid)
            continue;
        // If in threshold mode we check the "balance" pushed by the websocket and use the pin/time preset
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payloadStr);
        if (error)
        {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
        }
        JsonObject payment = doc["payment"];
        JsonObject extra = payment["extra"];
        double paidAmount = extra["wallet_fiat_amount"];
        Serial.println("paidAmount: " + String(paidAmount));

        // Aumento l'importo pagato per considerare lo sconto in percentuale sui prodotti
        double fullAmount = paidAmount / (1 - (DiscountPerc / 100));
        Serial.println("fullAmount: " + String(fullAmount));

        inviaImporto(fullAmount);
    }
    Serial.println("Paid");
    paid = false;
}

void inviaImporto(double amount)
{
    for (Coin &coin : coins)
    {
        while (amount >= coin.value)
        {
            inviaMoneta(coin);
            amount -= coin.value;
        }
    }
}

void inviaMoneta(Coin &coin)
{
    Serial.println("Invio: " + String(coin.value));
    digitalWrite(coin.gpio, LOW);
    delay(10);
    digitalWrite(coin.gpio, HIGH);
    delay(50);
}

//////////////////HELPERS///////////////////

void readFiles()
{
    Serial.println("");
    Serial.println("SSID hardcoded");
    Serial.println("SSID: " + ssid);

    Serial.println("");
    Serial.println("SSID password hardcoded");

    Serial.println("");
    Serial.println("switchStr hardcoded");
    Serial.println("switchStr: " + switchStr);

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

    Serial.println("LNbits ws prefix: " + urlPrefix);
    Serial.println("LNbits server: " + lnbitsServer);
    Serial.println("LNbits API url: " + apiUrl);
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
