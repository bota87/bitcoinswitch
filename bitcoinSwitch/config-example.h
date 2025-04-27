#include <Arduino.h>

String ssid = "wifi";
String wifiPassword = "password";
String lnbitsServer = "lnbits.com";
String WalletReadKey = "xxxxxxxx"; // Invoice/read key for the LNbits wallet you want to watch

const int servoPin = 12;
const int openPin = 21;
const int lightPin = 25;
const int alertSecondsBeforeOff = 30; // avvisa x secondi prima di spengere
const int closeDegree = 0;
const int openDegree = 60;
String lnAddressGettone = "biliardino"; // senza dominio
const float lightPricePerMinute = 0.02; // costo illuminazione al minuto