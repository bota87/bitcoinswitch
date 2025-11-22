#pragma once

#include <Arduino.h>
#ifdef TFT
#include <TFT_eSPI.h>
#endif

void setupTFT();
void printTFT(String message, int x, int y);
void printHome(bool wifi, bool ws, bool ping);
void clearTFT();
void flashTFT();