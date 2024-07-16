/*
 * detect presence,
 * control lighting,
 * publish logs to firebase
 *
 * */
#ifdef ESP32
#pragma message "Compiling esp32 project"
#include "./esp32.h"
#endif

#ifdef ESP8266
#pragma message "Compiling the nodemcu/esp8266 project"
#include "./esp8266.h"
#endif
