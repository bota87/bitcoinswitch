#include "500_servo.h"

Servo servo;

void setupServo()
{
    servo.attach(config_servo_pin);
    Serial.println("[Servo] ready");
}

void handleServoPulse(int pin, int durationMs)
{
    setStatusLed(false);
    servo.write(config_servo_open_degree);
    Serial.println("Servo aperto: " + String(config_servo_open_degree));
    delay(durationMs);
    servo.write(config_servo_close_degree);
    Serial.println("Servo chiuso: " + String(config_servo_close_degree));
    setStatusLed(true);
}
