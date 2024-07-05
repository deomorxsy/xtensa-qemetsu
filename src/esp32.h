//#include "./secrets.h"
#include "./camera-pins.h"

#include "Arduino.h"
#include <WiFi.h>
#include <FirebaseClient.h>
#include <system_error>
#include <esp_http_server.h>

#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "soc/soc.h" 
#include "soc/rtc_cntl_reg.h"  
#include "esp_http_server.h"

//https://blog.eletrogate.com/introducao-ao-esp32-cam/
//https://github.com/easytarget/esp32-cam-webserver/tree/4c9ab6e174f224ca67ed06c693f466acfc913475
//https://github.com/maxgerhardt/pio-esp32cam/tree/main

// ESP-32CAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM

//ESP-32CAM pinout
/*#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27
#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34
#define Y7_GPIO_NUM    39
#define Y6_GPIO_NUM    36
#define Y5_GPIO_NUM    21
#define Y4_GPIO_NUM    19
#define Y3_GPIO_NUM    18
#define Y2_GPIO_NUM    5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22*/


//#include <addons/RTDBHelper.h>

// Wi-Fi credentials
#ifndef WIFI_SSID
#define WIFI_SSID "fdsafdsas"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "rfdfds"
#endif

// Firebase console > Project Overview > Project settings.
#define API_KEY "web_API_KEY"

#define USER_MAIL       "USER_MAIL"
#define USER_PASSWORD   "USER_PASSWORD"

// RTDB URL and database secret
#ifndef DATABASE_URL
#define DATABASE_URL ${{DATABASE_URL_REPO_SECRET}}
#endif

#define FIREBASE_PROJECT_ID "PROJECT_ID"

void asyncCB(AsyncResult &aResult);
void printResult(AsyncResult &aResult);


DefaultNetwork network;
UserAuth user_auth(API_KEY, USER_MAIL, USER_PASSWORD);

FirebaseApp app;

void setup() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // ESP32CAM
    pinMode(33, OUTPUT);


    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }

}

void loop() {
    //ESP32CAM
    digitalWrite(33, LOW);

}

void asyncCB(AsyncResult &aResult)
{
    // WARNING!
    // Do not put your codes inside the callback and printResult.

    printResult(aResult);
}

void printResult(AsyncResult &aResult)
{
    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}
