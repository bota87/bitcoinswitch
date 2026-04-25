#include "201_web_socket.h"

WebSocketsClient webSocket;
bool ping_toggle = false;

void setupWebSocket()
{
    if (config_device_string == "")
    {
        Serial.println("No device string configured!");
        printTFT("No device string!", 21, 95);
        return;
    }

    if (!config_device_string.startsWith("wss://"))
    {
        Serial.println("Device string does not start with wss://");
        printTFT("no wss://!", 21, 95);
        return;
    }

    String cleaned_device_string = config_device_string.substring(6); // Remove wss://
    String host = cleaned_device_string.substring(0, cleaned_device_string.indexOf('/'));
    String apiPath = cleaned_device_string.substring(cleaned_device_string.indexOf('/'));
    Serial.println("Websocket host: " + host);
    Serial.println("Websocket API Path: " + apiPath);

    // Use in normal mode
    Serial.println("Using NORMAL mode");
    Serial.println("Connecting to websocket: " + host + apiPath);
    webSocket.beginSSL(host, 443, apiPath);

    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(1000);
    // Send a ping every 5s; disconnect if pong missing 2 times in a row (~6-11s detection)
    webSocket.enableHeartbeat(5000, 3000, 2);
}

void loopWebSocket()
{
    webSocket.loop();
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_ERROR:
        Serial.printf("[WebSocket] Error: %s\n", payload);
        setStatusLed(false);
        printHome(true, false, false);
        break;
    case WStype_DISCONNECTED:
        Serial.println("[WebSocket] Disconnected!\n");
        setStatusLed(false);
        printHome(true, false, false);
        break;
    case WStype_CONNECTED:
        Serial.printf("[WebSocket] Connected to url: %s\n", payload);
        // send message to server when Connected
        webSocket.sendTXT("Connected");
        setStatusLed(true);
        printHome(true, true, false);
        break;
    case WStype_TEXT:
        executePayment(payload);
        break;
    case WStype_BIN:
        Serial.printf("[WebSocket] Received binary data: %s\n", payload);
        break;
    case WStype_FRAGMENT_TEXT_START:
        break;
    case WStype_FRAGMENT_BIN_START:
        break;
    case WStype_FRAGMENT:
        break;
    case WStype_FRAGMENT_FIN:
        break;
    case WStype_PING:
        Serial.printf("[WebSocket] Ping!\n");
        setStatusLed(true);
        ping_toggle = !ping_toggle;
        printHome(true, true, ping_toggle);
        // pong will be sent automatically
        break;
    case WStype_PONG:
        // is not used
        Serial.printf("[WebSocket] Pong!\n");
        printHome(true, true, true);
        break;
    }
}

void executePayment(uint8_t *payload)
{
    printTFT("Payment received!", 21, 15);
    flashTFT();

    String parts[3]; // pin, time, comment
    // format: {pin-time-comment} where comment is optional
    String payloadStr = String((char *)payload);
    int numParts = splitString(payloadStr, '-', parts, 3);

    int pin = parts[0].toInt();
    printTFT("Pin: " + String(pin), 21, 35);

    int time = parts[1].toInt();
    printTFT("Time: " + String(time), 21, 55);

    String comment = "";
    if (numParts == 3)
    {
        comment = parts[2];
        Serial.println("[WebSocket] received comment: " + comment);
        printTFT("Comment: " + comment, 21, 75);
    }
    Serial.println("[WebSocket] received pin: " + String(pin) + ", duration: " + String(time));

    // the magic happens here
    if (pin == config_servo_pin)
        handleServoPulse(pin, time);
    else if (pin == config_light_pin)
        handleLightPayment(time);

    printHome(true, true, false);
}