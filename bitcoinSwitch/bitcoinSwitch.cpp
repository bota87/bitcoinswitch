#include "config-distributore-lattine.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

struct Coin
{
    double value;
    int gpio;
};

// Dichiarazione delle monete con relative GPIO
Coin coins[] = {
    {1.00, 23},  // LV1 1.00
    {0.50, 21},  // LV3 0.50
    {0.20, 22},  // LV2 0.20
    {0.05, 19}}; // LV4 0.05

String payloadStr;
bool paid, webSocketConnected;

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
void ElaboraPagamento();
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

    pinMode(LED_BUILTIN, OUTPUT); // To blink on board LED

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

    // Aumento l'importo pagato per considerare lo sconto in percentuale sui prodotti
    double fullAmount = paidAmount / (1 - (DiscountPerc / 100));
    Serial.println("fullAmount: " + String(fullAmount));

    if (true)
    {
        // Per semplicità imposto importo fisso
        fullAmount = 3;
        Serial.println("FORZATO fullAmount: " + String(fullAmount));
    }

    inviaImporto(fullAmount);
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
