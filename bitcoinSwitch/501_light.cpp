#include "501_light.h"

static unsigned long lightOffTime = 0;    // absolute millis() when light turns off; 0 = light off
static bool lightAlertGiven = false;      // warning blink already triggered for current session
static unsigned long lightAlertEnd = 0;  // millis() when 500ms blink-off period ends; 0 = not blinking

void setupLight()
{
    pinMode(config_light_pin, OUTPUT);
    digitalWrite(config_light_pin, LOW);
    Serial.println("[Light] pin " + String(config_light_pin) + " initialised");
}

void handleLightPayment(int durationMs)
{
    if (lightOffTime == 0)
    {
        // Light was off: turn on and set deadline
        digitalWrite(config_light_pin, HIGH);
        lightOffTime = millis() + (unsigned long)durationMs;
        Serial.println("[Light] on, off at " + String(lightOffTime) + " ms");
    }
    else
    {
        // Light already on: extend deadline
        lightOffTime += (unsigned long)durationMs;
        Serial.println("[Light] extended, new off at " + String(lightOffTime) + " ms");
    }

    // Re-arm the warning blink for the (new) deadline
    lightAlertGiven = false;
}

void loopLight()
{
    if (lightOffTime == 0)
        return;

    unsigned long now = millis();

    // Phase 1: end of 500ms blink-off → restore light
    if (lightAlertEnd != 0 && now >= lightAlertEnd)
    {
        digitalWrite(config_light_pin, HIGH);
        lightAlertEnd = 0;
    }

    // Phase 2: deadline reached → turn off
    if (now >= lightOffTime)
    {
        digitalWrite(config_light_pin, LOW);
        lightOffTime = 0;
        lightAlertGiven = false;
        lightAlertEnd = 0;
        Serial.println("[Light] off");
        return;
    }

    // Phase 3: warning blink when alert threshold is reached
    unsigned long remaining = lightOffTime - now;
    unsigned long alertThresholdMs = (unsigned long)config_alert_seconds_before_off * 1000UL;

    if (!lightAlertGiven && lightAlertEnd == 0 && remaining <= alertThresholdMs)
    {
        digitalWrite(config_light_pin, LOW);
        lightAlertEnd = now + 500;
        lightAlertGiven = true;
        Serial.println("[Light] warning blink, " + String(remaining / 1000) + "s remaining");
    }
}
