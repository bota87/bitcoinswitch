#pragma once
#include <Arduino.h>
#include "200_wifi.h"

struct Coin
{
    double value;
    int gpio;
};

void setupCoin();
void loopCoin();
void inviaImporto(double amount);
void inviaMoneta(Coin &coin);
void testCoin();