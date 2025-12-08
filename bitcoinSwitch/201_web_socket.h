#pragma once

#include <Arduino.h>
#include <WebSocketsClient.h>

#include "100_config.h"
#include "300_tft.h"
#include "400_split_string.h"

void setupWebSocket();
void loopWebSocket();
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
void executePayment(uint8_t *payload);