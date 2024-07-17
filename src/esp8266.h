#include "./secrets.c"

//type of realtimedatabase return
#include <iostream>
#include <typeinfo>
#include <cxxabi.h>

#include "Arduino.h"
#include "core/AsyncClient/AsyncClient.h"
#include "core/NetConfig.h"
#include "pins_arduino.h"
//#include "firestore/Documents.h"
//#include "firestore/Values.h"
#include <ESP8266WiFi.h>
#include <FirebaseClient.h>
//#include <system_error.h>

// Sensors definition
#define PIR_SENSOR_PIN D1
#define SOUND_SENSOR_PIN D2 // previously A0
#define LED_PIN D3
#define BUTTON_PIN D4

// Wi-Fi credentials
#ifndef WIFI_SSID
#define WIFI_SSID ${WIFI_SSID_REPO_SECRET}
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ${WIFI_PASSWORD_REPO_SECRET}
#endif

// Firebase console > Project Overview > Project settings.
#ifndef API_KEY
#define API_KEY         "web_API_KEY"
#endif

#ifndef USER_MAIL
#define USER_MAIL       "USER_MAIL"
#endif

#ifndef USER_PASSWORD
#define USER_PASSWORD   "USER_PASSWORD"
#endif

// RTDB URL and database secret
#ifndef DATABASE_URL
#define DATABASE_URL "URL"
#endif
//#define DATABASE_URL ${DATABASE_URL_REPO_SECRET}

#define FIREBASE_PROJECT_ID "PROJECT_ID"

void asyncCB(AsyncResult &aResult);
void printResult(AsyncResult &aResult);
void printError(int code, const String &msg);


UserAuth user_auth(API_KEY, USER_MAIL, USER_PASSWORD);

FirebaseApp app;

#if defined(ESP32) || defined(ESP8266)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#endif

DefaultNetwork network;

// async
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client, getNetwork(network));
AsyncResult aResult_no_callback;

// RealtimeDatabase
RealtimeDatabase Database;

// Firestore
//Firestore::Documents Docs;


//int counter = 0;
bool taskComplete = false;
unsigned long dataMillis = 0;
const unsigned long interval = 90000; // 1m30s
bool ledState = false;
bool buttonState = false;
bool lastButtonState = false;

void setup() {
    Serial.begin(115200);
    pinMode(PIR_SENSOR_PIN, INPUT);
    pinMode(SOUND_SENSOR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-FI...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.println("Connected to Wi-Fi");

    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);
    Serial.println("Initializing app...");

#if defined(ESP32) || defined(ESP8266)
    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif
#endif


    // init Firebase connection
    initializeApp(aClient, app, getAuth(user_auth), asyncCB, "authTask");

    // bind FirebaseApp for authentication handler
    app.getApp<RealtimeDatabase>(Database);
    // unbind with Docs.resetApp();
    //app.getApp<Firestore::Documents>(Docs);

    Database.url(DATABASE_URL);

    // In sync functions, we have to set the operating result for the client that works with the function.
    // This code uses async.
    //client.setAsyncResult(result);

    Serial.println("Setup finished.");

}

void loop() {

    app.loop();
    Database.loop();

    unsigned long currentMillis = millis();
    if (currentMillis - dataMillis >= interval)
    {
        dataMillis = currentMillis;

        // Read sensor values
        bool soundValue = digitalRead(SOUND_SENSOR_PIN);
        bool pirValue = digitalRead(PIR_SENSOR_PIN);

        // Send sensor values to Firebase. push to add a line

        Serial.print("Push bool... ");
        //bool name;
        /*Database.push<bool>(aClient, "/sensors/sound", soundValue, asyncCB, "setSoundValueTask");
        if (aClient.lastError().code() == 0) {
            Firebase.printf("ok, sound typeName: %s\n", typename.c_str());
        } else
            printError(aClient.lastError().code(), aClient.lastError().message());

        result = Database.push<bool>(aClient, "/sensors/pir", pirValue, asyncCB, "setPirValueTask");
        if (aClient.lastError().code() == 0) {
            std::string typename = demangle(typeid(result).name());
            Firebase.printf("ok, PIR typeName: %s\n", typename.c_str());
        } else
            printError(aClient.lastError().code(), aClient.lastError().message());
        */
        Database.push<bool>(aClient, "/sensors/sound", soundValue, asyncCB, "setSoundValueTask");
        //if (aClient.lastError().code() == 0)
        //    Firebase.printf("ok, name: %s\n", name.c_str());
        //else
            //    printError(aClient.lastError().code(), aClient.lastError().message());


        Database.push<bool>(aClient, "/sensors/pir", pirValue, asyncCB, "setPirValueTask");
        //if (aClient.lastError().code() == 0)
        //    Firebase.printf("ok, name: %s\n", name.c_str());
        //else
        //    printError(aClient.lastError().code(), aClient.lastError().message());

        Serial.print("Sound value: ");
        Serial.println(soundValue);
        Serial.print("PIR value: ");
        Serial.println(pirValue);

        // Control LED based on sensor values
        if (!soundValue && !pirValue)
        {
            ledState = false; // Turn off LED
        }
        else
        {
            ledState = true; // Turn on LED
        }
        digitalWrite(LED_PIN, ledState);
    }

    // Read button state
    buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == LOW && lastButtonState == HIGH)
    {
        ledState = !ledState; // Toggle LED state
        digitalWrite(LED_PIN, ledState);
    }
    lastButtonState = buttonState;

    //if (app.ready() && !taskComplete) {}

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

void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}

std::string demangle(const char* name) {
    int status = -4; // some arbitrary value to eliminate the compiler warning
    char* res = abi::__cxa_demangle(name, NULL, NULL, &status);
    const char* const demangled_name = (status == 0) ? res : name;
    std::string ret_val(demangled_name);
    free(res);
    return ret_val;
}
