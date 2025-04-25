#include "config-pandorone.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

String payloadStr = "";
bool paid = false;

unsigned long lightOffTime = 0;
unsigned long lightAlertTime = 0;
unsigned long lastPingTime = 0;
unsigned long lastMessageTime = 0;
const unsigned long pingInterval = 10000;       // ogni 10s invia ping
const unsigned long maxSilentTime = 30000;      // se non riceve nulla per 30s, considera la connessione persa


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

    bool isActuallyConnected = webSocket.isConnected();
    digitalWrite(LED_BUILTIN, isActuallyConnected); // il led rappresenta lo stato del web socket

    unsigned long now = millis();

    // Invio ping periodico
    if (isActuallyConnected && now - lastPingTime > pingInterval)
    {
        Serial.println("Invio ping...");
        webSocket.sendPing();
        lastPingTime = now;
    }

    // Timeout di inattività
    if (isActuallyConnected && (now - lastMessageTime > maxSilentTime))
    {
        Serial.println("WebSocket inattivo troppo a lungo, forzo disconnessione...");
        webSocket.disconnect();
        lastMessageTime = now; // resetto altrimenti disconnette in continuazione
    }

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
        break;
    case WStype_CONNECTED:
        Serial.printf("[WSc] Connected to url: %s\n", payload);
        webSocket.sendTXT("Connected"); // send message to server when Connected
        lastMessageTime = millis();
        break;
    case WStype_PONG:
        Serial.println("Pong ricevuto");
        lastMessageTime = millis();
        break;
    case WStype_TEXT:
        payloadStr = (char *)payload;
        payloadStr.replace(String("'"), String('"'));
        payloadStr.toLowerCase();
        Serial.println("Received data from socket: " + payloadStr);
        paid = true;
        lastMessageTime = millis();
        break;
    case WStype_ERROR:
        Serial.println("[WSc] Errore WebSocket");
        break;
    }
}
