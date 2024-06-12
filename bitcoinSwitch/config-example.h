
#include <Arduino.h>

String ssid = "null"; // 'String ssid = "ssid";' / 'String ssid = "null";'
String wifiPassword = "null"; // 'String wifiPassword = "password";' / 'String wifiPassword = "null";'

// String from the lnurlDevice plugin in LNbits lnbits.com
String switchStr = "null"; // 'String switchStr = "ws url";' / 'String switchStr = "null";'

// Change for threshold trigger only
String WalletReadKey; // Invoice/read key for the LNbits wallet you want to watch,  'String WalletReadKey = "key";' / 'String WalletReadKey = "null";'
double DiscountPerc = 10;