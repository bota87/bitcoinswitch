#include "500_coin.h"

// Dichiarazione delle monete con relative GPIO
Coin coins[] = {
    {1.00, 23}, // LV1 1.00
    {0.50, 21}, // LV3 0.50
    {0.20, 22}, // LV2 0.20
    {0.05, 19}  // LV4 0.05
};

void setupCoin()
{
    // I pin che simulano la gettoniera vanno messi come OUTPUT solo quando si vuole simulare l'inserimento di una moneta
    // altrimenti la gettoniera non funziona più
    for (Coin &coin : coins)
        pinMode(coin.gpio, INPUT);
}

void loopCoin()
{
#ifdef BT1_PIN
    if (digitalRead(BT1_PIN) == LOW)
    {
        Serial.println("Pulsante di test premuto");
        testCoin();
        delay(500); // debounce e per evitare test multipli
    }
#endif
}

void inviaImporto(double amount)
{
    Serial.println("Invia Importo: " + String(amount, 2) + "€");
    for (Coin &coin : coins)
    {
        while (amount >= coin.value)
        {
            inviaMoneta(coin);
            amount -= coin.value;
            amount = round(amount * 100.0) / 100.0; // arrotondo al secondo decimale
        }
    }
    Serial.println("Importo completato");
}

void inviaMoneta(Coin &coin)
{
    Serial.println("Invio moneta: " + String(coin.value, 2) + "€ (GPIO " + String(coin.gpio) + ")");
    pinMode(coin.gpio, OUTPUT);
    digitalWrite(coin.gpio, LOW);
    delay(50);
    pinMode(coin.gpio, INPUT);
    delay(50);
}

void testCoin()
{
    Serial.println("=== TEST MONETE ===");
    setStatusLed(false); // durante il test spengo il led
    for (Coin &coin : coins)
    {
        Serial.println("Testing moneta: " + String(coin.value, 2) + "€");
        inviaMoneta(coin);
    }
    setStatusLed(true);
}
