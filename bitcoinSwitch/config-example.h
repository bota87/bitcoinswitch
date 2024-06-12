///////////////////////////////////////////////////////////////////////////////////
//         Change these variables directly in the code or use the config         //
//  form in the web-installer https://lnbits.github.io/bitcoinswitch/installer/  //
///////////////////////////////////////////////////////////////////////////////////

String ssid = "null";         // 'String ssid = "ssid";' / 'String ssid = "null";'
String wifiPassword = "null"; // 'String wifiPassword = "password";' / 'String wifiPassword = "null";'
int ledPin = 22;              // in aggiunta al led sulla board, è acceso su LOW
int switchPIN = 13;

// String from the lnurlDevice plugin in LNbits lnbits.com
String switchStr = "null";

// Change for threshold trigger only
String wallet; // ID for the LNbits wallet you want to watch,  'String wallet = "walley ID";' / 'String wallet = "null";'
long threshold; // In sats, 'long threshold = 0;' / 'long threshold = 100;'
int thresholdPin; // GPIO pin, 'int thresholdPin = 16;' / 'int thresholdPin;'
long thresholdTime; // Time to turn pin on, 'long thresholdTime = 2000;' / 'long thresholdTime;'

///////////////////////////////////////////////////////////////////////////////////
//                                 END of variables                              //
///////////////////////////////////////////////////////////////////////////////////